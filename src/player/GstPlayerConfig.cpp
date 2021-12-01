/*
 * GstPlayerConfig.cpp
 *
 *  Created on: 01.12.2021
 *      Author: joe
 */
#include "GstPlayerConfig.h"

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

const char *GstPlayerAlsaSinkConfig::GetGstAudioSinkElementType()
{
	return "alsasink";
}

void GstPlayerAlsaSinkConfig::DoConfigureSinkElement(GstElement *sinkElement)
{
	g_object_set (G_OBJECT (sinkElement), "device", this->GetDevice(), NULL);
}
