/*
 * GstPlayerConfig.h
 *
 *  Created on: 01.12.2021
 *      Author: joe
 */

#ifndef SRC_INCLUDE_IALSAVOLUMECONTROLCONFIG_H_
#define SRC_INCLUDE_IALSAVOLUMECONTROLCONFIG_H_

namespace squeezeclient {

class IAlsaVolumeControlConfig
{
public:
	virtual ~IAlsaVolumeControlConfig() {};

	virtual const char *GetMixerAlsaDeviceName()=0;

	virtual const char *GetMixerAlsaMixerName()=0;
};

}; /* namespace squeezeclient */

#endif /* SRC_INCLUDE_IALSAVOLUMECONTROLCONFIG_H_ */
