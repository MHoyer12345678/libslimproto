/*
 * SlimClientImpl.cpp
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#include "SqueezeClientImpl.h"

#include "player/GstPlayer.h"

#include "assert.h"

using namespace squeezeclient;

const char* SqueezeClient::CLIENT_STATE_NAMES[]=
		{
				"__NOT_SET",
				IPlayer::PLAYER_STATE_NAMES[IPlayer::PlayerStateT::STOPPED],
				IPlayer::PLAYER_STATE_NAMES[IPlayer::PlayerStateT::PLAYING],
				IPlayer::PLAYER_STATE_NAMES[IPlayer::PlayerStateT::PAUSED],
				"POWERED_OFF",
				"DISCONNECTED",
				"CONNECTING"
		};

SqueezeClientImpl::SqueezeClientImpl(IEventInterface *evIFace, IClientConfiguration *clientConfig,
		IPlayer *aPlayer, IVolumeControl *volCtrl, uint8_t builderFlags) :
		player(aPlayer),
		volCtrl(volCtrl),
		builderFlags(builderFlags)
{
	this->controller=new ClientController(evIFace, clientConfig, this->player, this->volCtrl);
}

SqueezeClientImpl::~SqueezeClientImpl()
{
	delete this->controller;

	if ((this->builderFlags & BUILDER_FLAG_DELETE_PLAYER)!=0)
		delete this->player;
	if (this->volCtrl!=NULL &&(this->builderFlags & BUILDER_FLAG_DELETE_VOLCTRL)!=0)
		delete this->volCtrl;
}

bool SqueezeClientImpl::Init(bool autoConnectToServer)
{
	if (!this->player->Init())
		return false;

	if (this->volCtrl!=NULL)
		if (!this->volCtrl->Init())
			return false;

	if (!this->controller->Init())
		return false;

	if (autoConnectToServer)
		this->controller->StartConnectingServer();

	return true;
}

void SqueezeClientImpl::DeInit()
{
	if (this->volCtrl!=NULL)
		this->volCtrl->DeInit();
	this->player->DeInit();
	this->controller->DeInit();
}

void SqueezeClientImpl::StartConnectingServer()
{
	this->controller->StartConnectingServer();
}

void SqueezeClientImpl::DisconnectServer()
{
	this->controller->DisconnectServer();
}

void SqueezeClientImpl::SignalPowerButtonPressed(PowerSignalT powerSignal)
{
	this->controller->SignalPowerButtonPressed(powerSignal);
}

void SqueezeClientImpl::SignalPlayButtonPressed()
{
	this->controller->SignalPlayButtonPressed();
}

void SqueezeClientImpl::SignalPauseButtonPressed()
{
	this->controller->SignalPauseButtonPressed();
}

void SqueezeClientImpl::SignalNextButtonPressed()
{
	this->controller->SignalNextButtonPressed();
}

void SqueezeClientImpl::SignalPreviousButtonPressed()
{
	this->controller->SignalPreviousButtonPressed();
}

void SqueezeClientImpl::SignalVolUpButtonPressed()
{
	this->controller->SignalVolUpButtonPressed();
}

void SqueezeClientImpl::SignalVolDownButtonPressed()
{
	this->controller->SignalVolDownButtonPressed();
}

void SqueezeClientImpl::SignalMuteButtonPressed()
{
	this->controller->SignalMuteButtonPressed();
}

void SqueezeClientImpl::SignalFakeFaster()
{
	this->controller->SignalFakeFaster();
}

void SqueezeClientImpl::SignalFakeSlower()
{
	this->controller->SignalFakeSlower();
}

SqueezeClient::SqueezeClientStateT SqueezeClientImpl::GetState()
{
	return this->controller->GetClientState();
}

