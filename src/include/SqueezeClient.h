/*
 * SqueezeClient.h
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#ifndef SRC_LIB_SQUEEZECLIENT_H_
#define SRC_LIB_SQUEEZECLIENT_H_

#include "IPlayer.h"

namespace slimprotolib {

class SqueezeClient {

public:
	enum PowerSignalT
	{
		POWER_TOGGLE,
		POWER_OFF,
		POWER_ON
	};

protected:
	SqueezeClient();

	virtual ~SqueezeClient();

public:

	static SqueezeClient *NewWithGstPlayer();

	static SqueezeClient *NewWithCustomPlayer(IPlayer::IPlayerBuilder *playerBuilder);

	static void Destroy(SqueezeClient *squeezeClient);

	virtual bool Init()=0;

	virtual void DeInit()=0;

	virtual void KickOff()=0;

	virtual void SignalPowerButtonPressed(PowerSignalT powerSignal)=0;

	virtual void SignalNextButtonPressed()=0;

	virtual void SignalPreviousButtonPressed()=0;

	virtual void SignalVolUpButtonPressed()=0;

	virtual void SignalVolDownButtonPressed()=0;

	virtual void SignalFakeFaster()=0;

	virtual void SignalFakeSlower()=0;

};

} /* slimprotolib */

#endif /* SRC_LIB_SQUEEZECLIENT_H_ */
