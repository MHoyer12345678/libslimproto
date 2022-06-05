/*
 * SCBConfig.cpp
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#include "SCBConfig.h"

#include <string.h>

using namespace squeezeclient;

#define DEFAULT_SERVER_AUTODETECTION_TAG	"::auto::"

#define DEFAULT_MAC_AUTODETECTION_TAG		"::auto::"


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

#define CONFIG_TAG_SERVER_ADDRESS		"ServerAddress"
#define DEFAULT_SERVER_ADDRESS			DEFAULT_SERVER_AUTODETECTION_TAG

#define CONFIG_TAG_SERVER_PORT			"ServerPort"
#define DEFAULT_SERVER_PORT				"3483"

#define CONFIG_TAG_MAC_ADDRESS			"MacAddress"
#define DEFAULT_MAC_ADDRESS				{0x00,0x00,0x00,0x00,0x00,0x00}
#define DEFAULT_MAC_ADDRESS_AUTODET		true

const char *SCBConfig::Version=VERSION;

SCBConfig::SCBConfig() :
		Configuration (DEFAULT_CONF_FILE,Logger::INFO)
{
	uint8_t mac[]=DEFAULT_MAC_ADDRESS;

	this->alsaDeviceName=strdup(DEFAULT_ALSA_DEVICE_NAME);
	this->alsaMixerName=strdup(DEFAULT_ALSA_MIXER_NAME);
	this->uid=strdup(DEFAULT_SCL_UID);
	this->internalVolCtrlEnabled=DEFAULT_INTERNAL_VOL;
	this->serverAddress=strdup(DEFAULT_SERVER_ADDRESS);
	this->serverPort=strdup(DEFAULT_SERVER_PORT);
	memcpy(this->macAddress, mac, 6);
	this->autodetectMac=DEFAULT_MAC_ADDRESS_AUTODET;
}

SCBConfig::~SCBConfig()
{
	free(this->alsaMixerName);
	free(this->alsaDeviceName);
	free(this->uid);
	free(this->serverPort);
	free(this->serverAddress);
}

bool SCBConfig::ParseConfigFileItem(GKeyFile *confFile,
		const char *group, const char *key)
{
	bool result=true;

	if (strcmp(group, CONFIG_GRP_NAME_SOUND)==0)
	{
		if (strcasecmp(key, CONFIG_TAG_ALSA_DEVICE_NAME)==0)
		{
			if (!this->ParseStringElement(confFile,group,key,&this->alsaDeviceName))
				result=false;
		}
		else if (strcasecmp(key, CONFIG_TAG_ALSA_MIXER_NAME)==0)
		{
			if (!this->ParseStringElement(confFile,group,key,&this->alsaMixerName))
				result=false;
		}
	}
	else if (strcmp(group, CONFIG_GRP_NAME_SQUEEZE_CLIENT)==0)
	{

		if (strcasecmp(key, CONFIG_TAG_SCL_UID)==0)
		{
			if (!this->ParseStringElement(confFile,group,key,&this->uid))
				result=false;
		}
		else if (strcasecmp(key, CONFIG_TAG_INTERNAL_VOL)==0)
		{
			if (!Configuration::GetBooleanValueFromKey(confFile,key,group, &this->internalVolCtrlEnabled))
				result=false;
			Logger::LogDebug("SCBConfig::ParseConfigFileItem - interal vol crtl: %d", this->internalVolCtrlEnabled);
		}
		else if (strcasecmp(key, CONFIG_TAG_SERVER_ADDRESS)==0)
		{
			if (!this->ParseStringElement(confFile,group,key,&this->serverAddress))
				result=false;
		}
		else if (strcasecmp(key, CONFIG_TAG_SERVER_PORT)==0)
		{
			if (!this->ParseStringElement(confFile,group,key,&this->serverPort))
				result=false;
		}
		else if (strcasecmp(key, CONFIG_TAG_MAC_ADDRESS)==0)
		{
			if (!this->ParseMacAddress(confFile,group, key))
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

void SCBConfig::GetMACAddress(uint8_t mac[6], bool &autodetectMac)
{
	autodetectMac=this->autodetectMac;
	if (!this->autodetectMac)
		memcpy(mac, this->macAddress, 6);
}

const char* SCBConfig::GetServerAddress()
{
	//NULL is returned to indicate that the server is to be determined automatically
	if (strcasecmp(this->serverAddress, DEFAULT_SERVER_AUTODETECTION_TAG)==0)
		return NULL;
	else
		return this->serverAddress;
}

const char* SCBConfig::GetServerPort()
{
	return this->serverPort;
}

bool SCBConfig::IsInternalVolumeCtrlEnabled()
{
	return this->internalVolCtrlEnabled;
}

bool SCBConfig::ParseMacAddress(GKeyFile *confFile,
		const char *group, const char *key)
{
	char *elementAttribute;
	bool result=true;

	if (!Configuration::GetStringValueFromKey(confFile,key,group, &elementAttribute))
	{
		Logger::LogError("Error get value from key \'%s\' in group \'%s\'.", key, group);
		return false;
	}

	if (strcasecmp(elementAttribute, DEFAULT_MAC_AUTODETECTION_TAG)==0)
	{
		this->autodetectMac=true;
		memset(this->macAddress, 0, 6);
	}
	else
	{
		unsigned int m1,m2,m3,m4,m5,m6;
		int processedVariables;

		processedVariables=sscanf(elementAttribute, "%2x:%2x:%2x:%2x:%2x:%2x", &m1,
				&m2,&m3,&m4,&m5,&m6);

		if (processedVariables!=6)
		{
			Logger::LogError("Error parsing %s address. Ensure format is: XX:XX:XX:XX:XX:XX with XX being hexadecimal numbers.", key);
			result=false;
		}
		else
		{
			this->macAddress[0]=(uint8_t)m1;
			this->macAddress[1]=(uint8_t)m2;
			this->macAddress[2]=(uint8_t)m3;
			this->macAddress[3]=(uint8_t)m4;
			this->macAddress[4]=(uint8_t)m5;
			this->macAddress[5]=(uint8_t)m6;
			this->autodetectMac=false;
		}
	}

	free(elementAttribute);
	return result;
}

bool SCBConfig::ParseStringElement(GKeyFile *confFile,
		const char *group, const char *key, char **elementAttributePtr)
{
	char *elementAttribute;
	if (!Configuration::GetStringValueFromKey(confFile,key,group, &elementAttribute))
	{
		Logger::LogError("Error get value from key \'%s\' in group \'%s\'.", key, group);
		return false;
	}

	if (*elementAttributePtr!=NULL)
		free(*elementAttributePtr);
	*elementAttributePtr=elementAttribute;
	return true;
}
