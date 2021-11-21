/*
 * SqueezeClient.cpp
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#include "SqueezeClient.h"

#include "SqueezeClientImpl.h"

namespace slimprotolib {

SqueezeClient::SqueezeClient()
{
}

SqueezeClient::~SqueezeClient()
{
}

SqueezeClient* SqueezeClient::NewWithGstPlayer()
{
	return SqueezeClientImpl::NewWithGstPlayer();
}

SqueezeClient* SqueezeClient::NewWithCustomPlayer(
		IPlayer::IPlayerBuilder *playerBuilder)
{
	return SqueezeClientImpl::NewWithCustomPlayer(playerBuilder);
}

void SqueezeClient::Destroy(SqueezeClient *squeezeClient)
{
	SqueezeClientImpl::Destroy((SqueezeClientImpl *)squeezeClient);
}

} /* slimprotolib */
