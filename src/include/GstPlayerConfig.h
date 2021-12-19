/*
 * GstPlayerConfig.h
 *
 *  Created on: 01.12.2021
 *      Author: joe
 */

#ifndef SRC_INCLUDE_GSTPLAYERCONFIG_H_
#define SRC_INCLUDE_GSTPLAYERCONFIG_H_

#include <gst/gst.h>

namespace squeezeclient {

class IGstPlayerConfig
{
public:
	virtual ~IGstPlayerConfig() {};

	virtual const char *GetGstAudioSinkElementType()=0;

	virtual void DoConfigureSinkElement(GstElement *sinkElement)=0;
};


class GstPlayerDefaultConfig : public IGstPlayerConfig
{
private:
	static IGstPlayerConfig *instance;

	GstPlayerDefaultConfig() {};

	virtual ~GstPlayerDefaultConfig() {};

	virtual void DoConfigureSinkElement(GstElement *sinkElement) {};

public:

	static IGstPlayerConfig* Instance();

	virtual const char *GetGstAudioSinkElementType();
};

class GstPlayerAlsaSinkConfig : public IGstPlayerConfig
{
public:
	virtual ~GstPlayerAlsaSinkConfig() {};

	virtual const char *GetGstAudioSinkElementType();

	virtual void DoConfigureSinkElement(GstElement *sinkElement);

	virtual const char *GetAlsaDeviceName()=0;
};

}; /* namespace squeezeclient */

#endif /* SRC_INCLUDE_GSTPLAYERCONFIG_H_ */
