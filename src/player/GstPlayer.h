/*
 * GstPlayer.h
 *
 *  Created on: 31.01.2021
 *      Author: joe
 */

#ifndef SRC_TEST_GSTPLAYER_H_
#define SRC_TEST_GSTPLAYER_H_

#include <gst/gst.h>
#include <stdint.h>

#include "squeezeclient/IPlayer.h"
#include "squeezeclient/IGstPlayerConfig.h"

namespace squeezeclient {

class GstPlayer : public IPlayer {

private:
	typedef struct PipeLineElementsT
	{
		GstElement *pipeline;

		GstElement *soupHttpSource;

		GstElement *decodeBin;

		GstElement *audioConvert;

		GstElement *audioAutoSink;

	} PipeLineElementsT;

	IGstPlayerConfig *configuration;

	PipeLineElementsT pipelineElements;

	guint busWatchId;

	bool needPlaybackReadySignal;

	uint32_t clockCalibrationMS;

	GstElement *CreateElement(const char *factoryElement, const char *name);

	bool LinkElements(GstElement *srcE,GstElement *sinkE, const char *whomToLinkStr);

	static gboolean	OnBusMessage(GstBus *bus,GstMessage *message,gpointer userData);

	void OnEoSMessage();

	void OnPipelineStateChanged();

	static void	OnPadAdded(GstElement *element, GstPad *pad, gpointer data);

	GstStructure *CreateHeadersStructureFromRequestStr(const char *headersStr);

	bool CreateResponseString(const GstStructure *requestResponseStructure, char **responseStrPtr);

	static gboolean AddHeaderToResponseString(GQuark fieldID, const GValue * value, gpointer userData);

	void SplitHttpRequestAndHeaders(const char *httpRequest, char *requestLine, const char **headers);

	bool ExtractHttpRequestItems(char *requestLine, const char **methodStrPtr,
			const char **requestStrPtr, const char **httpVersionStrPtr);

	void ResetClockCalibration();

	void UpdateClockCalibration(uint32_t value);

	void SetClockCalibration();

public:
	GstPlayer(IGstPlayerConfig *config);

	~GstPlayer();

	bool Init();

	void DeInit();

	void UpdatePlayerStatus(PlayerStatusT *status);

	bool PlayStream(const char *url, const char *headers, bool autostart);

	bool LoadStream(const StreamingServerInfoT *srvInfo, const AudioFormatT *audioFmt, bool autoStart);

	void Stop();

	void Resume();

	void Pause();

	void SkipFrames(uint32_t skippingDuration);
};

} /* namespace squeezeclient */

#endif /* SRC_TEST_GSTPLAYER_H_ */
