/*
 * SqueezeClient.cpp
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#include "SqueezeClient.h"

#include "SqueezeClientImpl.h"

using namespace squeezeclient;

const char* SqueezeClient::CLIENT_STATE_NAMES[]=
		{
				"__NOT_SET",
				IPlayer::PLAYER_STATE_NAMES[IPlayer::PlayerStateT::STOPPED],
				IPlayer::PLAYER_STATE_NAMES[IPlayer::PlayerStateT::PLAYING],
				IPlayer::PLAYER_STATE_NAMES[IPlayer::PlayerStateT::PAUSED],
				"POWERED_OFF"
		};

SqueezeClient *SqueezeClient::NewWithGstPlayerCustomConfig(IEventInterface *evIFace,
		IGstPlayerConfig *configuration)
{
	return SqueezeClientImpl::NewWithGstPlayerCustomConfig(evIFace, configuration);
}

SqueezeClient *SqueezeClient::NewWithGstPlayerDefaultConfig(IEventInterface *evIFace)
{
	return SqueezeClientImpl::NewWithGstPlayerDefaultConfig(evIFace);
}

SqueezeClient* SqueezeClient::NewWithCustomPlayer(IEventInterface *evIFace,
		IPlayer *player)
{
	return SqueezeClientImpl::NewWithCustomPlayer(evIFace, player);
}

void SqueezeClient::Destroy(SqueezeClient *squeezeClient)
{
	SqueezeClientImpl::Destroy((SqueezeClientImpl *)squeezeClient);
}

