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

#define CONFIG_GRP_NAME_SQUEEZE_CLIENT	"SqueezeClient"

#define CONFIG_TAG_SCL_UID				"UID"
#define DEFAULT_SCL_UID					"01-02-03-04-05-1"

#define CONFIG_TAG_INTERNAL_VOL			"EnableInternalVolumeCtrl"
#define DEFAULT_INTERNAL_VOL			true

const char *SCBConfig::Version=VERSION;

SCBConfig::SCBConfig() :
		Configuration (DEFAULT_CONF_FILE,Logger::INFO)
{
	this->alsaDeviceName=strdup(DEFAULT_ALSA_DEVICE_NAME);
	this->alsaMixerName=strdup(DEFAULT_ALSA_MIXER_NAME);
	this->uid=strdup(DEFAULT_SCL_UID);
	this->internalVolCtrlEnabled=DEFAULT_INTERNAL_VOL;
}

SCBConfig::~SCBConfig()
{
	free(this->alsaMixerName);
	free(this->alsaDeviceName);
	free(this->uid);
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
	else if (strcmp(group, CONFIG_GRP_NAME_SQUEEZE_CLIENT)==0)
	{

		if (strcasecmp(key, CONFIG_TAG_SCL_UID)==0)
		{
			char *uid;
			if (Configuration::GetStringValueFromKey(confFile,key,group, &uid))
			{
				if (this->uid!=NULL)
					free(this->uid);
				this->uid=uid;
			}
			else
				result=false;
		}
		else if (strcasecmp(key, CONFIG_TAG_INTERNAL_VOL)==0)
		{
			if (!Configuration::GetBooleanValueFromKey(confFile,key,group, &this->internalVolCtrlEnabled))
				result=false;
		}
	}

	return result;
}

bool SCBConfig::IsConfigFileGroupKnown(const char *group)
{
	return strcmp(group, CONFIG_GRP_NAME_SOUND)==0 ||
			strcmp(group, CONFIG_GRP_NAME_SQUEEZE_CLIENT)==0;
}

const char *SCBConfig::GetPlayerAlsaDeviceName()
{
	return this->alsaDeviceName;
}

const char *SCBConfig::GetMixerAlsaDeviceName()
{
	return this->alsaDeviceName;
}
const char* SCBConfig::GetMixerAlsaMixerName()
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

void SCBConfig::GetUID(char uid[16])
{
	memcpy(uid, this->uid, 16);
}

void SCBConfig::GetMACAddress(uint8_t mac[6])
{
#warning connect to config file
	uint8_t MAC_ADDRESS[6]={0x5C,0xE0, 0xC5, 0x49, 0x54, 0xAD};
	memcpy(mac, MAC_ADDRESS,6);
}

bool SCBConfig::IsInternalVolumeCtrlEnabled()
{
	return this->internalVolCtrlEnabled;
}
