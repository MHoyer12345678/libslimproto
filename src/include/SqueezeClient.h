/*
 * SqueezeClient.h
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#ifndef SRC_LIB_SQUEEZECLIENT_H_
#define SRC_LIB_SQUEEZECLIENT_H_

#include "IPlayer.h"

namespace squeezeclient {

#define RETRY_TIMEOUT_NO_RETRY				-1
#define RETRY_TIMEOUT_IMMEDIATE_RETRY		0

class SqueezeClient {

public:
	static const char* CLIENT_STATE_NAMES[];

	enum SqueezeClientStateT
	{
		__NOT_SET			= 0,
		STOPPED				= IPlayer::PlayerStateT::STOPPED,
		PLAYING				= IPlayer::PlayerStateT::PLAYING,
		PAUSED				= IPlayer::PlayerStateT::PAUSED,
		POWERED_OFF			= IPlayer::PlayerStateT::__NEXT_FREE,
		SRV_DISCONNECTED	= POWERED_OFF+1,
		SRV_CONNECTING		= SRV_DISCONNECTED+1,
	};

	class IClientConfiguration
	{
	public:
		virtual ~IClientConfiguration() {};

		virtual void GetUID(char uid[16])=0;

		virtual void GetMACAddress(uint8_t  mac[6])=0;

		virtual const char* GetServerAddress()=0;

		virtual const char* GetServerPort()=0;
	};

	enum ConnectLostReasonT
	{
		MANUAL_DISCONNECT	= 0,
		SERVER_CLOSED		= 1,
		CONNECTION_RW_ERROR = 2
	};

	class IEventInterface
	{
	public:

		virtual ~IEventInterface() {};

		virtual void OnPlayerNameRequested(char name[1024])=0;

		virtual void OnServerSetsNewPlayerName(const char *newName)=0;

		virtual void OnClientStateChanged(SqueezeClientStateT newState)=0;

		virtual void OnVolumeChanged(unsigned int volL, unsigned int volR)=0;

		virtual void OnConnectingServerFailed(int &retryTimeoutMS)=0;

		virtual void OnServerConnectionLost(int &retryTimeoutMS, ConnectLostReasonT reason)=0;
	};

	enum PowerSignalT
	{
		POWER_TOGGLE	= 0,
		POWER_OFF		= 1,
		POWER_ON		= 2
	};

	enum PauseResumeModeT
	{
		TOGGLE	= 0,
		PAUSE	= 1,
		RESUME	= 2
	};

public:

	virtual ~SqueezeClient() {};

	virtual bool Init(bool autoConnectToServer)=0;

	virtual void DeInit()=0;

	virtual void StartConnectingServer()=0;

	virtual void DisconnectServer()=0;

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
