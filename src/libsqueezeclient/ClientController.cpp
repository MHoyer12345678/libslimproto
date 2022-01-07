/*
 * ClientController.cpp
 *
 *  Created on: 25.01.2021
 *      Author: joe
 */

#include <netinet/in.h>
#include <assert.h>
#include <libsqueezeclient/ClientController.h>

#include "cpp-app-utils/Logger.h"
#include "CommandFactory.h"

#include "Utils.h"

#define IR_CMD_POWER	0x768940bf
#define IR_CMD_POWERON	0x76898f70
#define IR_CMD_POWEROFF	0x76898778
#define IR_CMD_NEXT		0x7689a05f
#define IR_CMD_PREV		0x7689c03f
#define IR_CMD_VOLUP	0x7689807f
#define IR_CMD_VOLDOWN	0x768900ff
#define IR_CMD_PAUSE	0x768920df
#define IR_CMD_PLAY		0x768910ef
#define IR_CMD_MUTE		0x7689c43b

using CppAppUtils::Logger;
using namespace squeezeclient;

ClientController::ClientController(SqueezeClient::IEventInterface *evIFace,SqueezeClient::IClientConfiguration *sclConfig,
		IPlayer *aPlayer, IVolumeControl *volCtrl) :
		player(aPlayer),
		volCtrl(volCtrl),
		squeezeClientEventInterface(evIFace),
		clientState(SqueezeClient::SqueezeClient::SRV_DISCONNECTED),
		squeezeClientConfig(sclConfig)
{
	assert(aPlayer!=NULL);
	memset(&this->lmsPlayerStatusData, 0, sizeof(IPlayer::PlayerStatusT));
	this->lmsConnection=new LMSConnection(this);
	this->commandFactory=new CommandFactory(this->lmsConnection, this);
	aPlayer->SetPlayerEventListener(this);
}

ClientController::~ClientController()
{
	delete this->commandFactory;
	delete this->lmsConnection;
}

bool ClientController::Init()
{
	if (!this->lmsConnection->Init(this->squeezeClientConfig))
		return false;

    return true;
}

void ClientController::DeInit()
{
	this->DisconnectServer();
	this->lmsConnection->DeInit();
}

void ClientController::StartConnectingServer()
{
	if (this->clientState!=SqueezeClient::SqueezeClientStateT::SRV_DISCONNECTED) return;
    Logger::LogDebug("ClientController::StartConnectingServer - Connecting to server.");
    this->SetClientState(SqueezeClient::SRV_CONNECTING);
    this->lmsConnection->StartConnecting();
}

void ClientController::DisconnectServer()
{
	if (this->clientState==SqueezeClient::SqueezeClientStateT::SRV_DISCONNECTED ||
		this->clientState==SqueezeClient::SqueezeClientStateT::__NOT_SET) return;

    Logger::LogDebug("ClientController::DisconnectServer - Disconnecting from server.");
	this->commandFactory->SendByeCmd();
    this->lmsConnection->Disconnect();
    this->SetClientState(SqueezeClient::SRV_DISCONNECTED);
}

void ClientController::OnConnectionEstablished()
{
	uint8_t macAdress[6];
	char uid[16];
	bool autodetectMac;

	Logger::LogDebug("ClientController::OnConnectionEstablished - Got informed about connection to server established.");
	this->squeezeClientConfig->GetMACAddress(macAdress, autodetectMac);
	if (autodetectMac)
	{
		this->lmsConnection->GetMACAddress(macAdress);
		Logger::LogDebug("ClientController::OnConnectionEstablished - Using MAC from eth interface: %X:%X:%X:%X:%X:%X",
				macAdress[0],macAdress[1],macAdress[2],macAdress[3],macAdress[4],macAdress[5]);
	}
	else
		Logger::LogDebug("ClientController::OnConnectionEstablished - Using confgured MAC: %X:%X:%X:%X:%X:%X",
				macAdress[0],macAdress[1],macAdress[2],macAdress[3],macAdress[4],macAdress[5]);


	this->squeezeClientConfig->GetUID(uid);

	if (this->commandFactory->SendHeloCmd(macAdress, uid))
	{
		//start with state "POWEROFF". Server will tell us if we are powered on
		this->SetClientState(SqueezeClient::POWERED_OFF);
		//no addional error handling is needed here. If sending a command fails,
		//LMSConnection enters the failure state on its own leading to respective
		//callbacks from ClientController called.
	}
}

void ClientController::OnConnectingServerFailed(int &retryTimeoutMS)
{
	this->SetClientState(SqueezeClient::SRV_DISCONNECTED);
	this->squeezeClientEventInterface->OnConnectingServerFailed(retryTimeoutMS);
}

void ClientController::OnServerConnectionLost(int &retryTimeoutMS,
		SqueezeClient::ConnectLostReasonT reason)
{
	this->player->Stop();
	this->SetClientState(SqueezeClient::SRV_DISCONNECTED);
	this->squeezeClientEventInterface->OnServerConnectionLost(
			retryTimeoutMS,reason);
}

void ClientController::OnCommandReceived(void *data, uint16_t cmdSize)
{
	this->commandFactory->DoProcessReceivedCommand(data, cmdSize);
}

void ClientController::OnConnectionResponseReceived(const char *responseStr)
{
    Logger::LogDebug("ClientController::OnConnectionResponseReceived - Received connection response string from player.");
	if (!this->lmsConnection->IsConnected())
	{
	    Logger::LogDebug("ClientController::OnConnectionResponseReceived - No LMS connection anymore. Skipping the response.");
		return;
	}
	if (!this->commandFactory->SendConnectResponse(responseStr))
	    Logger::LogError("Received connection response string from player. Unable to send it further to LMS.");

}

void ClientController::OnReadyToPlay()
{
	Logger::LogDebug("ClientController::OnReadyToPlay - Player informed us about being ready to play.");
	if (this->lmsConnection->IsConnected())
		this->commandFactory->SendSTMlCmd(&this->lmsPlayerStatusData);
	//else
		//ignore event if not connected anymore to server
}

void ClientController::OnTrackEnded()
{
    Logger::LogDebug("ClientController::OnReadyToPlay - Player informed us about an ended track.");
	if (this->lmsConnection->IsConnected())
		this->commandFactory->SendSTMdCmd(&this->lmsPlayerStatusData);
	//else
		//ignore event if not connected anymore to server
}

void ClientController::SignalPowerButtonPressed(SqueezeClient::PowerSignalT powerSignal)
{
	//correct state to send a command ensured in squeezeclientimpl
    switch(powerSignal)
    {
    case SqueezeClient::POWER_OFF:
        this->commandFactory->SendIRCmd(IR_CMD_POWEROFF);
        break;
    case SqueezeClient::POWER_ON:
        this->commandFactory->SendIRCmd(IR_CMD_POWERON);
        break;
    default:
        this->commandFactory->SendIRCmd(IR_CMD_POWER);
    }
}

void ClientController::SignalPlayButtonPressed()
{
	//correct state to send a command ensured in squeezeclientimpl
    if (this->clientState!=SqueezeClient::SqueezeClientStateT::PLAYING)
    	this->commandFactory->SendIRCmd(IR_CMD_PLAY);
}

void ClientController::SignalPauseButtonPressed()
{
	//correct state to send a command ensured in squeezeclientimpl
    this->commandFactory->SendIRCmd(IR_CMD_PAUSE);
}

void ClientController::SignalNextButtonPressed()
{
	//correct state to send a command ensured in squeezeclientimpl
    this->commandFactory->SendIRCmd(IR_CMD_NEXT);
}

void ClientController::SignalPreviousButtonPressed()
{
	//correct state to send a command ensured in squeezeclientimpl
    this->commandFactory->SendIRCmd(IR_CMD_PREV);
}

void ClientController::SignalVolUpButtonPressed()
{
	//correct state to send a command ensured in squeezeclientimpl
    this->commandFactory->SendIRCmd(IR_CMD_VOLUP);
}

void ClientController::SignalVolDownButtonPressed()
{
	//correct state to send a command ensured in squeezeclientimpl
    this->commandFactory->SendIRCmd(IR_CMD_VOLDOWN);
}

void ClientController::SignalMuteButtonPressed()
{
	//correct state to send a command ensured in squeezeclientimpl
    this->commandFactory->SendIRCmd(IR_CMD_MUTE);
}

void ClientController::UpdatePlayerStatusData()
{
    Logger::LogDebug("ClientController::UpdatePlayerStatusData - Updating player status data structure.");
    this->player->UpdatePlayerStatus(&this->lmsPlayerStatusData);
}

void ClientController::OnSrvRequestedPlayerStatus(uint32_t replayGain)
{
	this->UpdatePlayerStatusData();
	this->commandFactory->SendSTMtCmd(&this->lmsPlayerStatusData, replayGain);
}

void ClientController::OnSrvRequestedPause(uint32_t pauseDuration)
{
	if (pauseDuration==0)
	{
		this->player->Pause();
		this->UpdatePlayerStatusData();
		this->commandFactory->SendSTMpCmd(&this->lmsPlayerStatusData);
	}
	else
	{
		Logger::LogDebug("ClientController::OnSrvRequestedPause - Server asks us to pause for %d ms.", pauseDuration);
		this->player->Pause();
		//TODO: replace by a delay thread ...
		g_usleep(pauseDuration*1000);
		this->player->Resume();
	}
}

void ClientController::OnSrvRequestedStop()
{
	this->player->Stop();
	this->UpdatePlayerStatusData();
	this->commandFactory->SendSTMfCmd(&this->lmsPlayerStatusData);
}

void ClientController::OnSrvRequestedUnPause(uint32_t unpauseTime)
{
	if (unpauseTime==0)
	{
		this->player->Resume();
		this->UpdatePlayerStatusData();
		this->commandFactory->SendSTMrCmd(&this->lmsPlayerStatusData);
	}
	else
	{
		uint32_t now=Utils::GetTimeMS();
		uint32_t diff=unpauseTime-now;
		Logger::LogDebug("ClientController::OnSrvRequestedUnPause - Server asks us to resume at: %u ms, now: %u ms, diff: %u ms", unpauseTime, now, diff);

		//TODO: replace by a delay thread ...
		if (diff > 0)
			g_usleep(diff*1000);
		this->player->Resume();
	}
}

void ClientController::OnSrvRequestedSkippingFrames(uint32_t skippingDuration)
{
	Logger::LogDebug("ClientController::OnSrvRequestedSkippingFrames - Server asks us to skip %u ms.", skippingDuration);
	this->player->SkipFrames(skippingDuration);
}

void ClientController::OnSrvRequestedFlush()
{
	Logger::LogError("Server asks us to flush. Not yet implemented.");
}

void ClientController::OnSrvRequestedPlayerName()
{
	char name[1024];
	this->squeezeClientEventInterface->OnPlayerNameRequested(name);
    Logger::LogDebug("OnSrvRequestedPlayerName - Server asked us to send our player name. Sending back '%s'.", name);
	this->commandFactory->SendPlayerName(name);
}

void ClientController::OnSrvSetNewPlayerName(const char *playerName)
{
    Logger::LogDebug("ClientController::OnSrvProvidedNewPlayerName - Server send us a new player name: %s", playerName);
	// sending playername back to confirm reception
    this->commandFactory->SendPlayerName(playerName);
    this->squeezeClientEventInterface->OnServerSetsNewPlayerName(playerName);
}

void ClientController::OnSrvRequestedAudioEnabledChange(bool spdiffEnabled,
		bool dacEnabled)
{
    Logger::LogDebug("ClientController::OnSrvRequestedAudioEnabledChange - SPDIFF: %s, DAC: %s.",
    		spdiffEnabled ? "true":"false", dacEnabled ? "true":"false");

    //take this one as indication for POWER_ON / POWER_OFF
    if (dacEnabled || spdiffEnabled)
    {
    	//already on -> return
    	if (this->clientState!=SqueezeClient::SqueezeClientStateT::POWERED_OFF) return;
    	this->clientState=(SqueezeClient::SqueezeClientStateT)this->player->GetPlayerState();
    }
    else
    {
    	//already off -> return
    	if (this->clientState==SqueezeClient::SqueezeClientStateT::POWERED_OFF) return;
    	this->clientState=SqueezeClient::SqueezeClientStateT::POWERED_OFF;
    }

    this->squeezeClientEventInterface->OnClientStateChanged(this->clientState);
}

void ClientController::OnSrvRequestedDisableDACSetting()
{
	bool value=false;
    Logger::LogDebug("ClientController::OnSrvRequestedDisableDACSetting - Server asked us to send the status of the \'DisableDAC\' setting. Sending back '%d'.", value);
	// No useful need to handle this setting. In fact, the server shouldn't even ask since the setting is not even displayed in settings page
	// anyway. Just send '0' back to server
	this->commandFactory->SendDisableDACSetting(value);
}

void ClientController::OnSrvSetDisableDACSetting(bool value)
{
    Logger::LogDebug("ClientController::OnSrvSetDisableDACSetting - Server updated \'DisableDAC\' setting. New value: %s.", value ? "true" : "false");
	this->commandFactory->SendDisableDACSetting(value);
}


void ClientController::OnSrvRequestedVolumeChange(unsigned int volL,
		unsigned int volR, bool adjustLocally)
{
    Logger::LogDebug("ClientController::OnSrvRequestedVolumeChange - VolL: %d, VolR: %d, Apply locally: %s",
    		volL, volR, adjustLocally ? "true" : "false");

    //if internal vol ctrl is enabled signal volume change to player
    if (this->volCtrl!=NULL)
    	this->volCtrl->SetVolume(volL, volR);

    //inform outside anyway
    this->squeezeClientEventInterface->OnVolumeChanged(volL, volR);
}

void ClientController::OnSrvRequestedLoadStream(
		IPlayer::StreamingServerInfoT *srvInfo, IPlayer::AudioFormatT *audioFMT,
		bool autostart)
{
	this->UpdatePlayerStatusData();
	this->commandFactory->SendSTMcCmd(&this->lmsPlayerStatusData);

	if (!player->LoadStream(srvInfo, audioFMT, autostart))
	{
		Logger::LogError("Unable to play stream.");
		this->commandFactory->SendSTMnCmd(&this->lmsPlayerStatusData);
		#warning: There is sort of error reporting to server -> Find out what and how
	}
	this->commandFactory->SendSTMsCmd(&this->lmsPlayerStatusData);
}

void ClientController::OnPlayerStateChanged(
		IPlayer::PlayerStateT state)
{
    Logger::LogDebug("ClientController::OnPlayerStateChanged - Player changed to state: %s",IPlayer::PLAYER_STATE_NAMES[state]);
    //state changes of player are just of interest when switched on
    if (this->clientState!=SqueezeClient::SqueezeClientStateT::POWERED_OFF &&
    	this->clientState!=SqueezeClient::SqueezeClientStateT::SRV_DISCONNECTED &&
		this->clientState!=SqueezeClient::SqueezeClientStateT::SRV_CONNECTING)
    {
    	//just cast, state maping of enums is done in SqueezeClient.h
    	this->SetClientState((SqueezeClient::SqueezeClientStateT)state);
    }
}

SqueezeClient::SqueezeClientStateT ClientController::GetClientState()
{
	return this->clientState;
}

void squeezeclient::ClientController::SetClientState(
		SqueezeClient::SqueezeClientStateT newState)
{
	if (this->clientState==newState) return;
	this->clientState=newState;
	this->squeezeClientEventInterface->OnClientStateChanged(newState);
}

#warning following functions away later

void ClientController::SignalFakeFaster()
{
	//add 10ms on playtime
    Logger::LogDebug("ClientController::SignalFakeFaster - Not yet implemented.");
}

void ClientController::SignalFakeSlower()
{
	//sub 10ms on playtime
    Logger::LogDebug("ClientController::SignalFakeSlower - Pausing for 50ms to simulate to be slower.");
    this->player->Pause();
	g_usleep(50*1000);
    this->player->Resume();
}
