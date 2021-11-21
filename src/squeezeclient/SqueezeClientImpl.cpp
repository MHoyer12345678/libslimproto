/*
 * SlimClientImpl.cpp
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#include "squeezeclient/SqueezeClientImpl.h"

#include "player/GstPlayer.h"

namespace slimprotolib {


SqueezeClientImpl::SqueezeClientImpl(PlayerT internalPlayer) :
		playerBuilder(NULL)
{
	switch(internalPlayer)
	{
	case GST_PLAYER:
	default:
		this->player=new GstPlayer();
	}

	this->controller=new PlayerController(this->player);
}

SqueezeClientImpl::SqueezeClientImpl(IPlayer::IPlayerBuilder *aPlayerBuilder) :
		playerBuilder(aPlayerBuilder)
{
	this->player=this->playerBuilder->CreatePlayer();
	this->controller=new PlayerController(this->player);
}

SqueezeClientImpl::~SqueezeClientImpl()
{
	delete this->controller;

	//external player -> destroy using builder
	//internal player -> destroy using delete
	if (this->playerBuilder!=NULL)
		this->playerBuilder->DestroyPlayer(this->player);
	else
		delete this->player;
}

SqueezeClient* SqueezeClientImpl::NewWithGstPlayer()
{
	return new SqueezeClientImpl(GST_PLAYER);
}

SqueezeClient* SqueezeClientImpl::NewWithCustomPlayer(
		IPlayer::IPlayerBuilder *playerBuilder)
{
	return new SqueezeClientImpl(playerBuilder);
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

void SqueezeClientImpl::SignalFakeFaster()
{
	this->controller->SignalFakeFaster();
}

void SqueezeClientImpl::SignalFakeSlower()
{
	this->controller->SignalFakeSlower();
}

} /* namespace retroradio_controller */
