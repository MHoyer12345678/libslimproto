/*
 * GstPlayerTest.cpp
 *
 *  Created on: 23.01.2021
 *      Author: joe
 */

#include "GstPlayerTest.h"

#include <cpp-app-utils/Logger.h>
#include <glib-unix.h>

using namespace CppAppUtils;

using namespace squeezeclient;

GstPlayerTest* GstPlayerTest::instance=NULL;

GstPlayerTest::GstPlayerTest() :
		returnCode(0)
{
	this->mainloop=g_main_loop_new(NULL,FALSE);
	this->player=new GstPlayer(this);
}

GstPlayerTest::~GstPlayerTest()
{
	delete player;
	g_main_loop_unref (this->mainloop);
}

GstPlayerTest* GstPlayerTest::Instance()
{
	if (GstPlayerTest::instance==NULL)
		GstPlayerTest::instance=new GstPlayerTest();

	return GstPlayerTest::instance;
}

bool GstPlayerTest::Init(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	Logger::LogDebug("GstPlayerTest::Init - Initalizing GstPlayerTest");

	if (!player->Init())
		return false;

    g_unix_signal_add(1, &UnixSignalHandler, this);
    g_unix_signal_add(2, &UnixSignalHandler, this);
    g_unix_signal_add(15, &UnixSignalHandler, this);

   	return true;
}

void GstPlayerTest::DeInit()
{
	this->player->DeInit();
	delete this;
	GstPlayerTest::instance=NULL;

	Logger::LogDebug("GstPlayerTest::DeInit - DeInitalized GstPlayerTest");
}

gboolean GstPlayerTest::UnixSignalHandler(gpointer user_data)
{
	GstPlayerTest *instance=(GstPlayerTest *)user_data;
	g_main_loop_quit(instance->mainloop);
	return TRUE;
}

void GstPlayerTest::Run()
{
    //this->player->PlayStream("https://icecast.ndr.de/ndr/njoy/live/mp3/128/stream.mp3","",true);
    this->player->PlayStream("http://192.168.178.29:9000/stream.mp3?player=b8:27:eb:b9:a7:63","",true);
    //this->player->PlayStream("http://mediacenter/test.mp3","",true);
	Logger::LogDebug("GstPlayerTest::Run -> Going to enter main loop.");
	g_timeout_add(4000, OnTimeoutElapsed, this);
	g_main_loop_run(this->mainloop);
	Logger::LogDebug("GstPlayerTest::Run -> Main loop left. Shutting down.");
}

gboolean GstPlayerTest::OnTimeoutElapsed(gpointer user_data)
{
	GstPlayerTest *instance=(GstPlayerTest *)user_data;
	instance->player->SkipFrames(1000);
	return FALSE;
}

int GstPlayerTest::GetReturnCode()
{
	return this->returnCode;
}

const char* squeezeclient::GstPlayerTest::GetGstAudioSinkElementType()
{
	return "alsasink";
}

void squeezeclient::GstPlayerTest::DoConfigureSinkElement(
		GstElement *sinkElement)
{
	g_object_set (G_OBJECT (sinkElement), "device", "default", NULL);
}
