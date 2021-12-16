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

SqueezeClientImpl::SqueezeClientImpl(IEventInterface *evIFace,
			IPlayer *aPlayer, bool freePlayer) :
		player(aPlayer),
		freePlayerInstance(freePlayer)
{
	this->controller=new PlayerController(evIFace, this->player);
}

SqueezeClientImpl::~SqueezeClientImpl()
{
	delete this->controller;

	if (this->freePlayerInstance)
		delete this->player;
}

SqueezeClient *SqueezeClientImpl::NewWithGstPlayerCustomConfig(IEventInterface *evIFace,
		IGstPlayerConfig *configuration)
{
	IPlayer *player=new GstPlayer(configuration);
	return new SqueezeClientImpl(evIFace, player, true);
}

SqueezeClient *SqueezeClientImpl::NewWithGstPlayerDefaultConfig(IEventInterface *evIFace)
{
	return SqueezeClientImpl::NewWithGstPlayerCustomConfig(evIFace,
			GstPlayerDefaultConfig::Instance());
}

SqueezeClient *SqueezeClientImpl::NewWithCustomPlayer(IEventInterface *evIFace, IPlayer *player)
{
	return new SqueezeClientImpl(evIFace, player, false);
}

bool SqueezeClientImpl::Init()
{
	if (!this->player->Init())
		return false;

	if (!this->controller->Init())
		return false;

	return true;
}

void SqueezeClientImpl::DeInit()
{
	this->player->DeInit();
	this->controller->DeInit();
}

void SqueezeClientImpl::KickOff()
{
	this->controller->KickOff();
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

void SqueezeClientImpl::Destroy(SqueezeClientImpl *client)
{
	delete client;
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

