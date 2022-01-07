/*
 * SCBConfig.h
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_
#define SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_

#include "IGstPlayerConfig.h"
#include "IAlsaVolumeControlConfig.h"
#include "SqueezeClient.h"

#include "cpp-app-utils/Configuration.h"

using namespace CppAppUtils;



namespace squeezeclient {

class SCBConfig : public IGstPlayerAlsaSinkConfig,
	public IAlsaVolumeControlConfig,
	public SqueezeClient::IClientConfiguration,
	public Configuration::IConfigurationParserModule,
	public Configuration
{
private:
	char *alsaDeviceName;

	char *alsaMixerName;

	char *uid;

	bool internalVolCtrlEnabled;

	char *serverAddress;

	char *serverPort;

	bool autodetectMac;

	uint8_t macAddress[6];

	bool ParseMacAddress(GKeyFile *confFile, const char *group, const char *key);

	bool ParseStringElement(GKeyFile *confFile, const char *group, const char *key, char **elementAttributePtr);

public:
	static const char *Version;

public:
	SCBConfig();

	virtual ~SCBConfig();

	//--------------------------------------- IConfigurationParserModule ------------------------
	bool ParseConfigFileItem(GKeyFile *confFile, const char *group, const char *key);

	bool IsConfigFileGroupKnown(const char *group);

	//--------------------------------------- IGstPlayerAlsaSinkConfig ---------------------------
	const char *GetPlayerAlsaDeviceName();

	//--------------------------------------- IAlsaVolumeControlConfig ---------------------------
	const char *GetMixerAlsaDeviceName();

	const char *GetMixerAlsaMixerName();

	//--------------------------------------- Configuration -------------------------------------
	const char *GetVersion();

	const char *GetDescriptionString();

	const char *GetCommand();

	const char* GetServerAddress();

	const char* GetServerPort();
	//--------------------------------------- SqueezeClient::IClientConfiguration ---------------
	void GetUID(char uid[16]);

	void GetMACAddress(uint8_t  mac[6], bool &autodetectMac);

	bool IsInternalVolumeCtrlEnabled();

};

} /* namespace squeezeclient */

#endif /* SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_ */
