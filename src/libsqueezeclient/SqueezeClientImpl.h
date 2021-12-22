/*
 * SlimClientImpl.h
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_
#define SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_

#include "ClientController.h"
#include "SqueezeClient.h"

#include "IVolumeControl.h"

namespace squeezeclient {

#define __BUILDER_FLAG_RESET		0x00
#define BUILDER_FLAG_DELETE_PLAYER	0x01
#define BUILDER_FLAG_DELETE_VOLCTRL	0x02

class SqueezeClientImpl : public SqueezeClient {
private:
	IPlayer *player;

	IVolumeControl *volCtrl;

	ClientController *controller;

	uint8_t builderFlags;

public:
	SqueezeClientImpl(IEventInterface *evIFace, IClientConfiguration *clientConfig,
			IPlayer *aPlayer, IVolumeControl *volCtrl, uint8_t builderFlags);

	virtual ~SqueezeClientImpl();

	virtual bool Init();

	virtual void DeInit();

	virtual void KickOff();

	virtual void SignalPowerButtonPressed(PowerSignalT powerSignal);

	virtual void SignalPlayButtonPressed();

	virtual void SignalPauseButtonPressed();

	virtual void SignalNextButtonPressed();

	virtual void SignalPreviousButtonPressed();

	virtual void SignalVolUpButtonPressed();

	virtual void SignalVolDownButtonPressed();

	virtual void SignalMuteButtonPressed();

	virtual void SignalFakeFaster();

	virtual void SignalFakeSlower();

	virtual SqueezeClientStateT GetState();
};

} /* namespace squeezeclient */

#endif /* SRC_SQUEEZECLIENT_SQUEEZECLIENTIMPL_H_ */
