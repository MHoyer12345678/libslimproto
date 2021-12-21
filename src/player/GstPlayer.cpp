/*
 * GstPlayer.cpp
 *
 *  Created on: 31.01.2021
 *      Author: joe
 */

#include "GstPlayer.h"

#include <string.h>
#include <libsoup/soup.h>
#include <stdio.h>
#include <assert.h>

#include <sstream>

#include <cpp-app-utils/Logger.h>

#define MAX_HTTP_REQUEST_SIZE 8096

using std::stringstream;
using namespace CppAppUtils;

using namespace squeezeclient;


GstPlayer::GstPlayer(IGstPlayerConfig *config) :
				busWatchId(0),
				needPlaybackReadySignal(false),
				clockCalibrationMS(0),
				configuration(config)
{
	memset(&this->pipelineElements, 0, sizeof(PipeLineElementsT));
}

GstPlayer::~GstPlayer()
{
}

bool GstPlayer::Init()
{
	GstBus *bus;

	if (this->pipelineElements.pipeline!=NULL)
		this->DeInit();

	this->needPlaybackReadySignal=false;

	Logger::LogDebug("GstPlayer::Init - Initializing player.");

	Logger::LogDebug("GstPlayer::Init - Creating pipeline and elements.");
	gst_init(NULL,NULL);

	//create pipeline
	this->pipelineElements.pipeline=gst_pipeline_new ("GstSqueezePlayer_Pipeline");
	if (this->pipelineElements.pipeline==NULL)
	{
		Logger::LogError("Unable to create GST pipeline.");
		return false;
	}

	this->pipelineElements.soupHttpSource=this->CreateElement("souphttpsrc", "http_source");
	this->pipelineElements.decodeBin=this->CreateElement("decodebin", "decode_bin");
	this->pipelineElements.audioConvert=this->CreateElement("audioconvert", "audio_convert");
	this->pipelineElements.audioAutoSink=this->CreateElement(
			this->configuration->GetGstAudioSinkElementType(), "audio_sink");

	this->configuration->DoConfigureSinkElement(this->pipelineElements.audioAutoSink);

	if (this->pipelineElements.soupHttpSource==NULL || this->pipelineElements.decodeBin==NULL ||
			this->pipelineElements.audioConvert==NULL || this->pipelineElements.audioAutoSink==NULL)
		return false;

	bus = gst_pipeline_get_bus (GST_PIPELINE (this->pipelineElements.pipeline));
	this->busWatchId = gst_bus_add_watch (bus, GstPlayer::OnBusMessage, this);
	gst_object_unref (bus);

	Logger::LogDebug("GstPlayer::Init - Linking elements in pipeline.");
	gst_bin_add_many (GST_BIN (this->pipelineElements.pipeline), this->pipelineElements.soupHttpSource,
			this->pipelineElements.decodeBin, this->pipelineElements.audioConvert, this->pipelineElements.audioAutoSink, NULL);

	if (!this->LinkElements(this->pipelineElements.soupHttpSource, this->pipelineElements.decodeBin, "souphttpsrc with decodebin."))
		return false;

	//need to link decodebin and audioconvert dynamically
	g_signal_connect (this->pipelineElements.decodeBin, "pad-added", G_CALLBACK (GstPlayer::OnPadAdded), this);

	if (!this->LinkElements(this->pipelineElements.audioConvert, this->pipelineElements.audioAutoSink, "audioconvert with autoaudiosink."))
		return false;

	this->SetStateAndNotify(PlayerStateT::STOPPED);

	return true;
}

GstElement *GstPlayer::CreateElement(const char *factoryElement, const char *name)
{
	GstElement *element;

	element=gst_element_factory_make (factoryElement, name);
	if (element==NULL)
		Logger::LogError("Unable to create GST element %s as %s.", factoryElement, name);

	return element;
}

bool GstPlayer::LinkElements(GstElement *srcE,GstElement *sinkE, const char *whomToLinkStr)
{
	if (gst_element_link (srcE, sinkE) != TRUE)
	{
		Logger::LogError("Unable to link %s.", whomToLinkStr);
		return false;
	}

	return true;
}

void GstPlayer::DeInit()
{
	Logger::LogDebug("GstPlayer::DeInit - Deinizialing player.");

	g_source_remove(this->busWatchId);
	this->busWatchId=0;

	gst_element_set_state (this->pipelineElements.pipeline, GST_STATE_NULL);
	gst_object_unref (this->pipelineElements.pipeline);
	this->pipelineElements.pipeline=NULL;
	gst_deinit();
	Logger::LogDebug("GstPlayer::DeInit - Deinizialized player.");
}

void GstPlayer::UpdatePlayerStatus(PlayerStatusT *status)
{
	GstClockTime runningTime;

	assert(status != NULL);
	Logger::LogDebug("GstPlayer::UpdatePlayerStatus - Filling player status data structure.");

	assert(this->pipelineElements.pipeline!=NULL);
	//returns play time in nano secs
	runningTime=gst_element_get_current_running_time(this->pipelineElements.audioAutoSink);
	status->elapsedMilliseconds=runningTime/1000000;
	Logger::LogDebug("GstPlayer::UpdatePlayerStatus - ElapsedMillisoncs: %d", status->elapsedMilliseconds);

	//uint32_t streamBufferSize;
	//uint32_t streamBufferFullness;
	//uint64_t bytesReceived;
	//uint32_t outputBufferSize;
	//uint32_t outputBufferFullness;


	//uint32_t elapsedMilliseconds;

}

void GstPlayer::ResetClockCalibration()
{
	this->clockCalibrationMS=0;
	this->SetClockCalibration();
}

void GstPlayer::UpdateClockCalibration(uint32_t value)
{
	this->clockCalibrationMS+=value;
	this->SetClockCalibration();
}

void GstPlayer::SetClockCalibration()
{
	GstClock *clock;

	if (this->pipelineElements.pipeline!=NULL)
	{
		clock=gst_pipeline_get_clock((GstPipeline *)this->pipelineElements.pipeline);
		gst_clock_set_calibration(clock,0,this->clockCalibrationMS*1000000,1,1);
		gst_object_unref(clock);
	}

}

bool GstPlayer::PlayStream(const char *url, const char *headersStr, bool autostart)
{
	GstStructure *headers=NULL;
	GstStateChangeReturn ret;

	if (this->pipelineElements.soupHttpSource==NULL)
	{
		Logger::LogError("PlayStream called on not initialized player. Call init() first.");
		return false;
	}

	//reset clock calibration since new synchronization is done on each new play
	this->ResetClockCalibration();

	this->needPlaybackReadySignal=false;
	headers=this->CreateHeadersStructureFromRequestStr(headersStr);

	if (headers!=NULL)
	{
		Logger::LogDebug("GstPlayer::PlayStream - headers structure created. Adding headers to souphttpsrc.");
		Logger::LogDebug("Struct: %s", gst_structure_to_string(headers));
		g_object_set (G_OBJECT (this->pipelineElements.soupHttpSource), "extra-headers", headers, NULL);
	}

	Logger::LogDebug("GstPlayer::PlayStream - Start playing url: %s.",url);

	g_object_set (G_OBJECT (this->pipelineElements.soupHttpSource), "location", url, NULL);
	ret=gst_element_set_state (this->pipelineElements.pipeline, autostart ? GST_STATE_PLAYING : GST_STATE_PAUSED);

	if (ret==GST_STATE_CHANGE_SUCCESS || GST_STATE_CHANGE_NO_PREROLL==GST_STATE_CHANGE_SUCCESS)
	{
		if (!autostart && this->playerEventListener!=NULL)
			this->playerEventListener->OnReadyToPlay();
	}
	else if (ret==GST_STATE_CHANGE_FAILURE)
	{
		Logger::LogError("Error playing: %s",url);
		return false;
	}
	else if (ret==GST_STATE_CHANGE_ASYNC)
	{
		this->needPlaybackReadySignal=!autostart;
		Logger::LogDebug("GstPlayer::PlayStream - Playback prepared in the background.");
	}

	return true;
}

void GstPlayer::Stop()
{
	Logger::LogDebug("GstPlayer::Stop - Stopping currently playing stream.");
	if (this->pipelineElements.pipeline!=NULL)
		gst_element_set_state (this->pipelineElements.pipeline, GST_STATE_READY);
	else
		Logger::LogError("Called stop on not initialized player.");
}

void GstPlayer::OnPipelineStateChanged()
{
	GstState gstState;
	GstStateChangeReturn ret;

	//timeout 100ms in ns
	ret=gst_element_get_state(this->pipelineElements.pipeline, &gstState, NULL, 100000000L);
	if (ret==GST_STATE_CHANGE_SUCCESS)
	{
		Logger::LogDebug("GstPlayer::OnPipelineStateChanged - Pipeline state changed to %d", gstState);
		switch(gstState)
		{
		case GST_STATE_PAUSED:
			if (this->needPlaybackReadySignal && this->playerEventListener!=NULL)
			{
				this->playerEventListener->OnReadyToPlay();
				this->needPlaybackReadySignal=false;
			}
			this->SetStateAndNotify(PlayerStateT::PAUSED);
			break;
		case GST_STATE_PLAYING:
			this->SetStateAndNotify(PlayerStateT::PLAYING);
			break;
		case GST_STATE_READY:
			this->SetStateAndNotify(PlayerStateT::STOPPED);
			break;
		default:
			break;
		}
	}
}

void GstPlayer::Pause()
{
	Logger::LogDebug("GstPlayer::Pause - Pausing currently playing stream.");
	if (this->pipelineElements.pipeline!=NULL)
	{
		gst_element_set_state (this->pipelineElements.pipeline, GST_STATE_PAUSED);
	}
	else
		Logger::LogError("Called pause on not initialized player.");
}

void GstPlayer::Resume()
{
	Logger::LogDebug("GstPlayer::Resume - Resuming currently playing stream.");
	if (this->pipelineElements.pipeline!=NULL)
	{
		gst_element_set_state (this->pipelineElements.pipeline, GST_STATE_PLAYING);
	}
	else
		Logger::LogError("Called resume on not initialized player.");
}

void GstPlayer::SkipFrames(uint32_t skippingDuration)
{
	Logger::LogDebug("GstPlayer::SkipFrames - Skipping %d ms of frames.", skippingDuration);
	this->UpdateClockCalibration(skippingDuration);
}

gboolean GstPlayer::OnBusMessage(GstBus *bus, GstMessage *message,
		gpointer userData)
{
	GstPlayer* instance=(GstPlayer *)userData;

	switch (GST_MESSAGE_TYPE (message))
	{
	case GST_MESSAGE_STATE_CHANGED:
		if (message->src==(GstObject *)instance->pipelineElements.pipeline)
			instance->OnPipelineStateChanged();
		break;

	case GST_MESSAGE_ELEMENT:
		if (message->src==(GstObject *)instance->pipelineElements.soupHttpSource)
		{
			const GstStructure *structure=gst_message_get_structure(message);
			char *responseStr=NULL;
			if (instance->CreateResponseString(structure, &responseStr))
			{
				Logger::LogDebug("GstPlayer::OnBusMessage - response string derived from header structure.");
				if (instance->playerEventListener!=NULL)
					instance->playerEventListener->OnConnectionResponseReceived(responseStr);
				free(responseStr);
			}
		}
		break;
	case GST_MESSAGE_EOS:
		instance->OnEoSMessage();
		break;

	case GST_MESSAGE_ERROR:
	{
		gchar  *debug;
		GError *error;

		gst_message_parse_error (message, &error, &debug);
		Logger::LogError("Gstreamer Pipeline Error: %s", error->message);
		if (debug!=NULL)
			Logger::LogError("Debug info: %s", debug);
		g_error_free (error);
		g_free (debug);
		break;
	}

	default:
		break;
	}

	return TRUE;
}

void GstPlayer::OnEoSMessage()
{
	Logger::LogDebug("GstPlayer::OnEoSMessage - gstreamer pipeline informed us about end of stream.");
	this->Stop();
	if (this->playerEventListener!=NULL)
		this->playerEventListener->OnTrackEnded();
}

void GstPlayer::OnPadAdded(GstElement *element, GstPad *pad, gpointer data)
{
	GstPad *sinkpad;

	GstPlayer* instance=(GstPlayer *)data;
	Logger::LogDebug("GstPlayer::OnPadAdded - decodebin added pad: ");

	sinkpad = gst_element_get_static_pad (instance->pipelineElements.audioConvert, "sink");
	gst_pad_link (pad, sinkpad);
	gst_object_unref (sinkpad);
}

GstStructure* GstPlayer::CreateHeadersStructureFromRequestStr(const char *headersStr)
{
	GstStructure *headers=gst_structure_new_empty("http-request-headers");
	char *line;
	line=strdupa(headersStr);

	while (line != NULL)
	{
		char *key,*value;

		//key is beginning of current line
		key=line;

		//find next line before cutting current line into pieces
		line=strchr(line, '\n');

		//terminate current line
		if (line!=NULL)
		{
			*line='\0';
			line++;
		}

		//value starts after ':'
		value=strchr(key, ':');

		//skip line without ':'
		if (value==NULL)
			continue;

		//terminate key
		*value='\0';
		//move to value (after ':')
		value++;

		//add to structure
		gst_structure_set(headers, key, G_TYPE_STRING, value, NULL);
	}

	return headers;
}

bool GstPlayer::CreateResponseString(const GstStructure *requestResponseStructure, char **responseStrPtr)
{
	uint32_t status;

	stringstream responseStringStream;
	std::stringbuf *stringbuf;
	int size;

	GstStructure *responseHeaders=NULL;

	Logger::LogDebug("GstPlayer::CreateRequestStrFromHeaderStructure - Found response header structure.");
	Logger::LogDebug("Structure: %s", gst_structure_to_string(requestResponseStructure));
	Logger::LogDebug("--------------------------------------------------------------------------------");

	//Get response status code
	if (!gst_structure_get_uint(requestResponseStructure, "http-status-code",&status))
	{
		Logger::LogError("Status code (http-status-code) not contained in response structure. Something went wrong.");
		return false;
	}

	responseStringStream << "HTTP/1.0 " << status << " " << soup_status_get_phrase(status)<<'\n';

	//Get Response headers and make a http compatible string out of it
	if (!gst_structure_get(requestResponseStructure, "response-headers", GST_TYPE_STRUCTURE, &responseHeaders,NULL))
	{
		Logger::LogError("No response-header field in response structure. Something went wrong.");
		return false;
	}

	gst_structure_foreach(responseHeaders, GstPlayer::AddHeaderToResponseString, &responseStringStream);

	responseStringStream << '\n';

	stringbuf=responseStringStream.rdbuf();
	stringbuf->pubseekoff(0,stringstream::beg);
	size=stringbuf->in_avail();

	//alloc size plus 1 for trailing \0
	*responseStrPtr=(char *)malloc(size+1);
	//copy string
	stringbuf->sgetn(*responseStrPtr,size);
	//terminate
	(*responseStrPtr)[size]='\0';

	return true;
}

gboolean GstPlayer::AddHeaderToResponseString(GQuark fieldID, const GValue *value, gpointer userData)
{
	stringstream *stringStream=(stringstream *)userData;
	*stringStream << g_quark_to_string (fieldID) << ": " << g_value_get_string(value) << '\n';
	return TRUE;
}

bool GstPlayer::LoadStream(const StreamingServerInfoT *srvInfo,
		const AudioFormatT *audioFmt, bool autostart)
{
	//first line of request: <METHOD> <REQUEST> <HTTP-VERSION>
	const char *methodStr;
	const char *requestStr;
	const char *httpVersionStr;
	//following lines -> header
	const char *httpHeadersStr;

	//buffer to take first line. Copy from srvInfo to split directly into elements
	char httpRequest[MAX_HTTP_REQUEST_SIZE+1];

	//buffer to take url to connect
	char url[4096];

	this->SplitHttpRequestAndHeaders(srvInfo->httpRequest, httpRequest, &httpHeadersStr);

	if (!this->ExtractHttpRequestItems(httpRequest, &methodStr, &requestStr, &httpVersionStr))
		return false;

	if (strcmp(methodStr,"GET")!=0)
	{
		Logger::LogError("Requested http method \'%s\' to connect to server not supported.", methodStr);
		assert(0);
	}

	snprintf(url, 4096,"http://%d.%d.%d.%d:%d%s",
			srvInfo->ip & 0xFF & 0xFF, (srvInfo->ip >> 8) & 0xFF, (srvInfo->ip >> 16) & 0xFF, (srvInfo->ip >> 24),
			srvInfo->port,requestStr);

	return this->PlayStream(url, httpHeadersStr, autostart);
}

void GstPlayer::SplitHttpRequestAndHeaders(const char *httpRequest, char *requestLine,
		const char **headers)
{
	const char *httpHeaders;

	memset(requestLine,0, MAX_HTTP_REQUEST_SIZE);

	httpHeaders=strchr(httpRequest,'\n');
	if (httpHeaders!=NULL)
	{
		int strlength=httpHeaders-httpRequest;
		//newline found -> request has headers
		memcpy(requestLine, httpRequest, MIN(MAX_HTTP_REQUEST_SIZE, strlength));
		requestLine[strlength]='\0';
		httpHeaders++;
	}
	else
	{
		//no newline -> only one line which is the request itsself. No headers
		strncpy(requestLine,httpRequest,MAX_HTTP_REQUEST_SIZE);
		httpHeaders="";
	}

	*headers=httpHeaders;
}

bool GstPlayer::ExtractHttpRequestItems(char *requestLine, const char **methodStrPtr,
		const char **requestStrPtr, const char **httpVersionStrPtr)
{
	bool result;
	char *searchPtr;

	searchPtr=strchr(requestLine,' ');
	result=(searchPtr!=NULL);

	if (result)
	{
		*methodStrPtr=requestLine;
		*searchPtr='\0';
		*searchPtr++;
		*requestStrPtr=searchPtr;
		searchPtr=strchr(searchPtr,' ');
		result=(searchPtr!=NULL);
	}

	if (result)
	{
		*searchPtr='\0';
		*searchPtr++;
		*httpVersionStrPtr=searchPtr;
	}

	if (!result)
		Logger::LogError("Invalid request string: %s", requestLine);

	return result;
}
