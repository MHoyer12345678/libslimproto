/*
 * SqueezeClient.h
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#ifndef SRC_LIB_SQUEEZECLIENT_H_
#define SRC_LIB_SQUEEZECLIENT_H_

#include "IPlayer.h"

#include "GstPlayerConfig.h"

namespace squeezeclient {

class SqueezeClient {

public:
	static const char* CLIENT_STATE_NAMES[];

	enum SqueezeClientStateT
	{
		__NOT_SET	= 0,
		STOPPED		= IPlayer::PlayerStateT::STOPPED,
		PLAYING		= IPlayer::PlayerStateT::PLAYING,
		PAUSED		= IPlayer::PlayerStateT::PAUSED,
		POWERED_OFF	= IPlayer::PlayerStateT::__NEXT_FREE,
	};

	class IEventInterface
	{
	public:

		virtual ~IEventInterface() {};

		virtual void OnPlayerNameRequested(char name[1024])=0;

		virtual void OnUIDRequested(char uid[16])=0;

		virtual void OnMACAddressRequested(uint8_t  mac[6])=0;

		virtual void OnServerSetsNewPlayerName(const char *newName)=0;

		virtual void OnClientStateChanged(SqueezeClientStateT newState)=0;

		virtual void OnVolumeChanged(unsigned int volL, unsigned int volR)=0;

		/** TODO: missing
		  * configure ip & port
		  * */
	};

	enum PowerSignalT
	{
		POWER_TOGGLE=0,
		POWER_OFF	=1,
		POWER_ON	=2
	};

	enum PauseResumeModeT
	{
		TOGGLE	= 0,
		PAUSE	= 1,
		RESUME	= 2
	};

public:

	virtual ~SqueezeClient() {};

	static SqueezeClient *NewWithGstPlayerCustomConfig(IEventInterface *evIFace,
			IGstPlayerConfig *configuration);

	static SqueezeClient *NewWithGstPlayerDefaultConfig(IEventInterface *evIFace);

	static SqueezeClient *NewWithCustomPlayer(IEventInterface *evIFace, IPlayer *player);

	static void Destroy(SqueezeClient *squeezeClient);

	virtual bool Init()=0;

	virtual void DeInit()=0;

	virtual void KickOff()=0;

	virtual void SignalPowerButtonPressed(PowerSignalT powerSignal)=0;

	virtual void SignalPlayButtonPressed()=0;

	virtual void SignalPauseButtonPressed()=0;

	virtual void SignalNextButtonPressed()=0;

	virtual void SignalPreviousButtonPressed()=0;

	virtual void SignalVolUpButtonPressed()=0;

	virtual void SignalVolDownButtonPressed()=0;

	virtual void SignalMuteButtonPressed()=0;

	virtual void SignalFakeFaster()=0;

	virtual void SignalFakeSlower()=0;

	virtual SqueezeClientStateT GetState()=0;
};

} /* squeezeclient */

#endif /* SRC_LIB_SQUEEZECLIENT_H_ */
