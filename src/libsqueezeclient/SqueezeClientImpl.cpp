/*
 * SlimClientImpl.cpp
 *
 *  Created on: 11.02.2021
 *      Author: joe
 */

#include "SqueezeClientImpl.h"

#include "cpp-app-utils/Logger.h"
#include "player/GstPlayer.h"

#include "assert.h"

using namespace squeezeclient;
using namespace CppAppUtils;

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
	this->controller->DeInit();
	if (this->volCtrl!=NULL)
		this->volCtrl->DeInit();
	this->player->DeInit();
}

void SqueezeClientImpl::StartConnectingServer()
{
	assert(this->GetState()!=__NOT_SET);
	this->controller->StartConnectingServer();
}

void SqueezeClientImpl::DisconnectServer()
{
	assert(this->GetState()!=__NOT_SET);
	this->controller->DisconnectServer();
}

void SqueezeClientImpl::SignalPowerButtonPressed(PowerSignalT powerSignal)
{
    Logger::LogDebug("SqueezeClientImpl::SignalPowerButtonPressed - Power button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalPowerButtonPressed(powerSignal);
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"PowerButtonPressed\".");
}

void SqueezeClientImpl::SignalPlayButtonPressed()
{
    Logger::LogDebug("SqueezeClientImpl::SignalPlayButtonPressed - Play button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalPlayButtonPressed();
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"PlayButtonPressed\".");
}

void SqueezeClientImpl::SignalPauseButtonPressed()
{
    Logger::LogDebug("SqueezeClientImpl::SignalPauseButtonPressed - Pause button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalPauseButtonPressed();
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"PauseButtonPressed\".");
}

void SqueezeClientImpl::SignalNextButtonPressed()
{
    Logger::LogDebug("SqueezeClientImpl::SignalNextButtonPressed - Next button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalNextButtonPressed();
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"NextButtonPressed\".");
}

void SqueezeClientImpl::SignalPreviousButtonPressed()
{
    Logger::LogDebug("SqueezeClientImpl::SignalPreviousButtonPressed - Previous button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalPreviousButtonPressed();
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"PreviousButtonPressed\".");
}

void SqueezeClientImpl::SignalVolUpButtonPressed()
{
    Logger::LogDebug("SqueezeClientImpl::SignalVolUpButtonPressed - VolUp button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalVolUpButtonPressed();
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"VolUpButtonPressed\".");
}

void SqueezeClientImpl::SignalVolDownButtonPressed()
{
    Logger::LogDebug("SqueezeClientImpl::SignalVolDownButtonPressed - VolDown button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalVolDownButtonPressed();
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"VolDownButtonPressed\".");
}

void SqueezeClientImpl::SignalMuteButtonPressed()
{
    Logger::LogDebug("SqueezeClientImpl::SignalMuteButtonPressed - Mute button pressed.");
    if (this->IsConnectedToServer())
    	this->controller->SignalMuteButtonPressed();
    else
        Logger::LogError("Squeezeclient not connected to server. Ignoring signal \"MuteButtonPressed\".");
}

void SqueezeClientImpl::SignalFakeFaster()
{
	this->controller->SignalFakeFaster();
}

void SqueezeClientImpl::SignalFakeSlower()
{
	this->controller->SignalFakeSlower();
}

bool SqueezeClientImpl::IsConnectedToServer()
{
	SqueezeClientStateT clientState=this->controller->GetClientState();
	return clientState!=__NOT_SET && clientState!=SRV_DISCONNECTED && clientState!=SRV_CONNECTING;
}

SqueezeClient::SqueezeClientStateT SqueezeClientImpl::GetState()
{
	return this->controller->GetClientState();
}
