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

namespace slimprotolib {

class SqueezeClient {

public:
	class IEventInterface
	{
	public:

		virtual ~IEventInterface() {};

		virtual void OnPlayerNameRequested(char name[1024])=0;

		virtual void OnUIDRequested(char uid[16])=0;

		virtual void OnMACAddressRequested(uint8_t  mac[6])=0;

		virtual void OnServerSetsNewPlayerName(const char *newName)=0;

		virtual void OnPowerStateChanged(bool value)=0;

		virtual void OnVolumeChanged(unsigned int volL, unsigned int volR)=0;

		/** missing
		  * - Player attributes (gst / alsa / ???)
		  * - configure ip & port
		  * */
	};


	enum PowerSignalT
	{
		POWER_TOGGLE,
		POWER_OFF,
		POWER_ON
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

	virtual void SignalNextButtonPressed()=0;

	virtual void SignalPreviousButtonPressed()=0;

	virtual void SignalVolUpButtonPressed()=0;

	virtual void SignalVolDownButtonPressed()=0;

	virtual void SignalFakeFaster()=0;

	virtual void SignalFakeSlower()=0;

};

} /* slimprotolib */

#endif /* SRC_LIB_SQUEEZECLIENT_H_ */
