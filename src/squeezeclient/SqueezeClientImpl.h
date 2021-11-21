/*
 * SlimClientImpl.h
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_
#define SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_

#include "SqueezeClient.h"

#include "slimproto/PlayerController.h"

namespace slimprotolib {

class SqueezeClientImpl : public SqueezeClient {
private:
	enum PlayerT
	{
		GST_PLAYER
	};

	IPlayer *player;

	PlayerController *controller;

	IPlayer::IPlayerBuilder *playerBuilder;

	SqueezeClientImpl(PlayerT internalPlayer);

	SqueezeClientImpl(IPlayer::IPlayerBuilder *aPlayerBuilder);

	virtual ~SqueezeClientImpl();

public:
	static SqueezeClient *NewWithGstPlayer();

	static SqueezeClient *NewWithCustomPlayer(IPlayer::IPlayerBuilder *playerBuilder);

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

} /* namespace slimprotolib */

#endif /* SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_ */
