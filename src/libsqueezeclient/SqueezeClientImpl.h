/*
 * SlimClientImpl.h
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_
#define SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_

#include "SqueezeClient.h"

#include "PlayerController.h"

namespace squeezeclient {

class SqueezeClientImpl : public SqueezeClient {
private:
	IPlayer *player;

	PlayerController *controller;

	bool freePlayerInstance;

	SqueezeClientImpl(IEventInterface *evIFace, IPlayer *aPlayer, bool freePlayer);

	virtual ~SqueezeClientImpl();

public:
	static SqueezeClient *NewWithGstPlayerCustomConfig(IEventInterface *evIFace,
			IGstPlayerConfig *configuration);

	static SqueezeClient *NewWithGstPlayerDefaultConfig(IEventInterface *evIFace);

	static SqueezeClient *NewWithCustomPlayer(IEventInterface *evIFace, IPlayer *player);

	static void Destroy(SqueezeClientImpl *client);

	virtual bool Init();

	virtual void DeInit();

	virtual void KickOff();

	virtual void SignalPowerButtonPressed(PowerSignalT powerSignal);

	virtual void SignalNextButtonPressed();

	virtual void SignalPreviousButtonPressed();

	virtual void SignalVolUpButtonPressed();

	virtual void SignalVolDownButtonPressed();

	virtual void SignalFakeFaster();

	virtual void SignalFakeSlower();
};

} /* namespace squeezeclient */

#endif /* SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_ */
