/*
 * IPlayer.cpp
 *
 *  Created on: 31.01.2021
 *      Author: joe
 */

#include "IPlayer.h"

#include <stdlib.h>

namespace slimprotolib {

IPlayer::IPlayer() :
		playerEventListener(NULL)
{
}

IPlayer::~IPlayer()
{
}

void IPlayer::SetPlayerEventListener(IPlayerEventListener *listener)
{
	this->playerEventListener=listener;
}

} /* namespace slimprotolib */
