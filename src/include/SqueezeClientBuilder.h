/*
 * SqueezeClientBuilder.h
 *
 *  Created on: 20.12.2021
 *      Author: joe
 */

#ifndef SRC_LIBSQUEEZECLIENT_SQUEEZECLIENTBUILDER_H_
#define SRC_LIBSQUEEZECLIENT_SQUEEZECLIENTBUILDER_H_

#include "SqueezeClient.h"

#include "IPlayer.h"
#include "IVolumeControl.h"

#include "IGstPlayerConfig.h"
#include "IAlsaVolumeControlConfig.h"

namespace squeezeclient {

class SqueezeClientBuilder {
private:
	uint8_t builderFlags;

	SqueezeClient::IEventInterface *sclEventInterface;

	IPlayer *player;

	IVolumeControl *volCtrl;

	SqueezeClient::IClientConfiguration *sclConfig;

public:
	SqueezeClientBuilder(SqueezeClient::IEventInterface *eventInterface,
			SqueezeClient::IClientConfiguration *sclConfig);

	virtual ~SqueezeClientBuilder();

	SqueezeClient *CreateInstance();

	static void DestroyInstance(SqueezeClient *instance);

	void PlayerUseCustom(IPlayer *player);

	void PlayerUseGstDefaultConf();

	void PlayerUseGstCustomConf(IGstPlayerConfig *playerConfig);

	void VolumeControlUseCustom(IVolumeControl *volControl);

	void VolumeControlUseAlsa(IAlsaVolumeControlConfig *volControlConfig);
};

} /* namespace squeezeclient */

#endif /* SRC_LIBSQUEEZECLIENT_SQUEEZECLIENTBUILDER_H_ */
