/*
 * SCBConfig.h
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_
#define SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_

#include "GstPlayerConfig.h"

#include "cpp-app-utils/Configuration.h"

using namespace CppAppUtils;



namespace squeezeclient {

class SCBConfig : public GstPlayerAlsaSinkConfig,
	public Configuration::IConfigurationParserModule,
	public Configuration
{
private:
	char *alsaDeviceName;

	char *alsaMixerName;

public:
	SCBConfig();

	virtual ~SCBConfig();

	bool ParseConfigFileItem(GKeyFile *confFile, const char *group, const char *key);

	bool IsConfigFileGroupKnown(const char *group);

	const char *GetAlsaDeviceName();

	const char *GetAlsaMixerName();

	const char *GetVersion();

	const char *GetDescriptionString();

	const char *GetCommand();

	static const char *Version;
};

} /* namespace squeezeclient */

#endif /* SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_ */
