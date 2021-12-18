/*
 * SCBConfig.h
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_
#define SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_

#include "GstPlayerConfig.h"

namespace squeezeclient {

class SCBConfig : public GstPlayerAlsaSinkConfig
{
public:
	SCBConfig();

	virtual ~SCBConfig();

	const char *GetDevice();

};

} /* namespace squeezeclient */

#endif /* SRC_SQUEEZECLIENT_BIN_SCBCONFIG_H_ */
