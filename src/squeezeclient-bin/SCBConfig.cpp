/*
 * SCBConfig.cpp
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#include "SCBConfig.h"

#include <string.h>

using namespace squeezeclient;

#define DEFAULT_CONF_FILE	"/etc/squeezeclient/squeezeclient.conf"

#define COMMAND			"sqeezeclient"
#define DESCRIPTION 	"Starts squeeze client (logitech media server client)."

#define CONFIG_GRP_NAME_SOUND 			"Sound"

#define CONFIG_TAG_ALSA_DEVICE_NAME		"AlsaDeviceName"
#define DEFAULT_ALSA_DEVICE_NAME		"default"

#define CONFIG_TAG_ALSA_MIXER_NAME		"AlsaMixerName"
#define DEFAULT_ALSA_MIXER_NAME			"Master"

const char *SCBConfig::Version=VERSION;

SCBConfig::SCBConfig() :
		Configuration (DEFAULT_CONF_FILE,Logger::INFO)
{
	this->alsaDeviceName=strdup(DEFAULT_ALSA_DEVICE_NAME);
	this->alsaMixerName=strdup(DEFAULT_ALSA_MIXER_NAME);
}

SCBConfig::~SCBConfig()
{
	free(this->alsaMixerName);
	free(this->alsaDeviceName);
}

bool SCBConfig::ParseConfigFileItem(GKeyFile *confFile,
		const char *group, const char *key)
{
	bool result=true;

	if (strcmp(group, CONFIG_GRP_NAME_SOUND)==0)
	{
		if (strcasecmp(key, CONFIG_TAG_ALSA_DEVICE_NAME)==0)
		{
			char *devName;
			if (Configuration::GetStringValueFromKey(confFile,key,group, &devName))
			{
				if (this->alsaDeviceName!=NULL)
					free(this->alsaDeviceName);
				this->alsaDeviceName=devName;
			}
			else
				result=false;
		}
		else if (strcasecmp(key, CONFIG_TAG_ALSA_MIXER_NAME)==0)
		{
			char *mixerName;
			if (Configuration::GetStringValueFromKey(confFile,key,group, &mixerName))
			{
				if (this->alsaMixerName!=NULL)
					free(this->alsaMixerName);
				this->alsaMixerName=mixerName;
			}
			else
				result=false;
		}
	}
//	else
//		next group

	return result;
}

bool SCBConfig::IsConfigFileGroupKnown(const char *group)
{
	return strcmp(group, CONFIG_GRP_NAME_SOUND)==0;
}

const char *SCBConfig::GetAlsaDeviceName()
{
	Logger::LogError("Alsa Name requested: ...");
	Logger::LogError("Alsa Name requested: %s",this->alsaDeviceName);
	return this->alsaDeviceName;
}

const char* SCBConfig::GetAlsaMixerName()
{
	return this->alsaMixerName;
}

const char* SCBConfig::GetVersion()
{
	return VERSION;
}

const char* SCBConfig::GetDescriptionString()
{
	return DESCRIPTION;
}

const char* SCBConfig::GetCommand()
{
	return COMMAND;
}
