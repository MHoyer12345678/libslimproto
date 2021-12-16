/*
 * IPlayer.cpp
 *
 *  Created on: 31.01.2021
 *      Author: joe
 */

#include "IPlayer.h"

#include <stdlib.h>

using namespace squeezeclient;

const char *IPlayer::PLAYER_STATE_NAMES[]={"__UNSET", "STOPPED","PLAYING","PAUSED"};


IPlayer::IPlayer() :
		playerEventListener(NULL),
		playerState(PlayerStateT::__NOT_SET)
{
}

IPlayer::~IPlayer()
{
}

void IPlayer::SetPlayerEventListener(IPlayerEventListener *listener)
{
	this->playerEventListener=listener;
}

void squeezeclient::IPlayer::SetStateAndNotify(PlayerStateT newState)
{
	bool changed=this->playerState!=newState;
	this->playerState=newState;
	if (changed && this->playerEventListener!=NULL)
		this->playerEventListener->OnPlayerStateChanged(newState);
}

IPlayer::PlayerStateT IPlayer::GetPlayerState()
{
	return this->playerState;
}
