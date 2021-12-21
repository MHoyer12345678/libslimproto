/*
 * GstPlayerConfig.cpp
 *
 *  Created on: 01.12.2021
 *      Author: joe
 */
#include "IGstPlayerConfig.h"

using namespace squeezeclient;

#define ALSA_SINK_ELEMENT_TYPE		"alsasink"

IGstPlayerConfig *GstPlayerDefaultConfig::instance=NULL;


const char *GstPlayerDefaultConfig::GetGstAudioSinkElementType()
{
	return "autoaudiosink";
}

IGstPlayerConfig* GstPlayerDefaultConfig::Instance()
{
	if (GstPlayerDefaultConfig::instance==NULL)
		GstPlayerDefaultConfig::instance=new GstPlayerDefaultConfig();

	return GstPlayerDefaultConfig::instance;
}

const char *IGstPlayerAlsaSinkConfig::GetGstAudioSinkElementType()
{
	return ALSA_SINK_ELEMENT_TYPE;
}

void IGstPlayerAlsaSinkConfig::DoConfigureSinkElement(GstElement *sinkElement)
{
	g_object_set (G_OBJECT (sinkElement), "device", this->GetPlayerAlsaDeviceName(), NULL);
}
