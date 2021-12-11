/*
 * SqueezeClient.cpp
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#include "SqueezeClient.h"

#include "SqueezeClientImpl.h"

using namespace squeezeclient;

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
