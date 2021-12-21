/*
 * PlayerController.cpp
 *
 *  Created on: 25.01.2021
 *      Author: joe
 */

#include "PlayerController.h"

#include <netinet/in.h>
#include <assert.h>

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

PlayerController::PlayerController(SqueezeClient::IEventInterface *evIFace,
		SqueezeClient::IClientConfiguration *sclConfig, IPlayer *aPlayer, IVolumeControl *volCtrl) :
		player(aPlayer),
		volCtrl(volCtrl),
		squeezeClientEventInterface(evIFace),
		clientState(SqueezeClient::SqueezeClientStateT::POWERED_OFF),
		squeezeClientConfig(sclConfig)
{
	assert(aPlayer!=NULL);
	memset(&this->lmsPlayerStatusData, 0, sizeof(IPlayer::PlayerStatusT));
	this->lmsConnection=new LMSConnection(this);
	this->commandFactory=new CommandFactory(this->lmsConnection, this);
	aPlayer->SetPlayerEventListener(this);
}

PlayerController::~PlayerController()
{
	delete this->commandFactory;
	delete this->lmsConnection;
}

bool PlayerController::Init()
{
	clientState=SqueezeClient::SqueezeClientStateT::POWERED_OFF;

	if (!this->lmsConnection->Init())
		return false;

    return true;
}

void PlayerController::DeInit()
{
	if (this->lmsConnection->IsConnected())
	{
		this->commandFactory->SendByeCmd();
		this->lmsConnection->Disconnect();
	}
    this->lmsConnection->DeInit();

    clientState=SqueezeClient::SqueezeClientStateT::POWERED_OFF;
}

void PlayerController::OnConnectionEstablished()
{
    Logger::LogDebug("PlayerController::OnConnectionEstablished - Got informed about connection to server established.");
}

void PlayerController::OnConnectionLost(bool isClosedByClient)
{
    Logger::LogDebug("PlayerController::OnConnectionLost - Got informed about"
    		" connection to server closed by %s.", isClosedByClient ? "client" : "server");
}

void PlayerController::OnCommandReceived(void *data, uint16_t cmdSize)
{
	this->commandFactory->DoProcessReceivedCommand(data, cmdSize);
}

void PlayerController::OnConnectionResponseReceived(const char *responseStr)
{
    Logger::LogDebug("PlayerController::OnConnectionResponseReceived - Received connection response string from player.");
	if (!this->lmsConnection->IsConnected())
	{
	    Logger::LogDebug("PlayerController::OnConnectionResponseReceived - No LMS connection anymore. Skipping the response.");
		return;
	}
	if (!this->commandFactory->SendConnectResponse(responseStr))
	    Logger::LogError("Received connection response string from player. Unable to send it further to LMS.");

}

void PlayerController::OnReadyToPlay()
{
    Logger::LogDebug("PlayerController::OnReadyToPlay - Player informed us about being ready to play.");
	this->commandFactory->SendSTMlCmd(&this->lmsPlayerStatusData);
}

void PlayerController::OnTrackEnded()
{
    Logger::LogDebug("PlayerController::OnReadyToPlay - Player informed us about an ended track.");
	this->commandFactory->SendSTMdCmd(&this->lmsPlayerStatusData);
}

void PlayerController::KickOff()
{
	uint8_t macAdress[6];
	char uid[16];

    Logger::LogDebug("PlayerController::KickOff - Connecting to server.");

    //TODO: Implement auto connect / reconnect
    this->lmsConnection->Connect();

    Logger::LogDebug("PlayerController::KickOff - Sending helo command.");
    this->squeezeClientConfig->GetMACAddress(macAdress);
    this->squeezeClientConfig->GetUID(uid);

   	if (!this->commandFactory->SendHeloCmd(macAdress, uid))
   	   	Logger::LogError("Failed to send helo command to server.");
}

void PlayerController::SignalPowerButtonPressed(SqueezeClient::PowerSignalT powerSignal)
{
    Logger::LogDebug("PlayerController::SignalPowerButtonPressed - Power button pressed.");
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

void PlayerController::SignalPlayButtonPressed()
{
    Logger::LogDebug("PlayerController::SignalPlayButtonPressed - Play button pressed.");
    if (this->clientState!=SqueezeClient::SqueezeClientStateT::PLAYING)
    	this->commandFactory->SendIRCmd(IR_CMD_PLAY);
}

void PlayerController::SignalPauseButtonPressed()
{
    Logger::LogDebug("PlayerController::SignalPauseButtonPressed - Pause button pressed.");
    this->commandFactory->SendIRCmd(IR_CMD_PAUSE);
}

void PlayerController::SignalNextButtonPressed()
{
    Logger::LogDebug("PlayerController::SignalNextButtonPressed - Next button pressed.");
    this->commandFactory->SendIRCmd(IR_CMD_NEXT);
}

void PlayerController::SignalPreviousButtonPressed()
{
    Logger::LogDebug("PlayerController::SignalPreviousButtonPressed - Previous button pressed.");
    this->commandFactory->SendIRCmd(IR_CMD_PREV);
}

void PlayerController::SignalVolUpButtonPressed()
{
    Logger::LogDebug("PlayerController::SignalVolUpButtonPressed - Volup button pressed.");
    this->commandFactory->SendIRCmd(IR_CMD_VOLUP);
}

void PlayerController::SignalVolDownButtonPressed()
{
    Logger::LogDebug("PlayerController::SignalVolDownButtonPressed - VolDown button pressed.");
    this->commandFactory->SendIRCmd(IR_CMD_VOLDOWN);
}

void PlayerController::SignalMuteButtonPressed()
{
    Logger::LogDebug("PlayerController::SignalMuteButtonPressed - Mute button pressed.");
    this->commandFactory->SendIRCmd(IR_CMD_MUTE);
}

void PlayerController::UpdatePlayerStatusData()
{
    Logger::LogDebug("PlayerController::UpdatePlayerStatusData - Updating player status data structure.");
    this->player->UpdatePlayerStatus(&this->lmsPlayerStatusData);
}

void PlayerController::OnSrvRequestedPlayerStatus(uint32_t replayGain)
{
	this->UpdatePlayerStatusData();
	this->commandFactory->SendSTMtCmd(&this->lmsPlayerStatusData, replayGain);
}

void PlayerController::OnSrvRequestedPause(uint32_t pauseDuration)
{
	if (pauseDuration==0)
	{
		this->player->Pause();
		this->UpdatePlayerStatusData();
		this->commandFactory->SendSTMpCmd(&this->lmsPlayerStatusData);
	}
	else
	{
		Logger::LogDebug("PlayerController::OnSrvRequestedPause - Server asks us to pause for %d ms.", pauseDuration);
		this->player->Pause();
		//TODO: replace by a delay thread ...
		g_usleep(pauseDuration*1000);
		this->player->Resume();
	}
}

void PlayerController::OnSrvRequestedStop()
{
	this->player->Stop();
	this->UpdatePlayerStatusData();
	this->commandFactory->SendSTMfCmd(&this->lmsPlayerStatusData);
}

void PlayerController::OnSrvRequestedUnPause(uint32_t unpauseTime)
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
		Logger::LogDebug("PlayerController::OnSrvRequestedUnPause - Server asks us to resume at: %u ms, now: %u ms, diff: %u ms", unpauseTime, now, diff);

		//TODO: replace by a delay thread ...
		if (diff > 0)
			g_usleep(diff*1000);
		this->player->Resume();
	}
}

void PlayerController::OnSrvRequestedSkippingFrames(uint32_t skippingDuration)
{
	Logger::LogDebug("PlayerController::OnSrvRequestedSkippingFrames - Server asks us to skip %u ms.", skippingDuration);
	this->player->SkipFrames(skippingDuration);
}

void PlayerController::OnSrvRequestedFlush()
{
	Logger::LogError("Server asks us to flush. Not yet implemented.");
}

void PlayerController::OnSrvRequestedPlayerName()
{
	char name[1024];
	this->squeezeClientEventInterface->OnPlayerNameRequested(name);
    Logger::LogDebug("OnSrvRequestedPlayerName - Server asked us to send our player name. Sending back '%s'.", name);
	this->commandFactory->SendPlayerName(name);
}

void PlayerController::OnSrvSetNewPlayerName(const char *playerName)
{
    Logger::LogDebug("PlayerController::OnSrvProvidedNewPlayerName - Server send us a new player name: %s", playerName);
	// sending playername back to confirm reception
    this->commandFactory->SendPlayerName(playerName);
    this->squeezeClientEventInterface->OnServerSetsNewPlayerName(playerName);
}

void PlayerController::OnSrvRequestedAudioEnabledChange(bool spdiffEnabled,
		bool dacEnabled)
{
    Logger::LogDebug("PlayerController::OnSrvRequestedAudioEnabledChange - SPDIFF: %s, DAC: %s.",
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

void PlayerController::OnSrvRequestedDisableDACSetting()
{
	bool value=false;
    Logger::LogDebug("PlayerController::OnSrvRequestedDisableDACSetting - Server asked us to send the status of the \'DisableDAC\' setting. Sending back '%d'.", value);
	// No useful need to handle this setting. In fact, the server shouldn't even ask since the setting is not even displayed in settings page
	// anyway. Just send '0' back to server
	this->commandFactory->SendDisableDACSetting(value);
}

void PlayerController::OnSrvSetDisableDACSetting(bool value)
{
    Logger::LogDebug("PlayerController::OnSrvSetDisableDACSetting - Server updated \'DisableDAC\' setting. New value: %s.", value ? "true" : "false");
	this->commandFactory->SendDisableDACSetting(value);
}


void PlayerController::OnSrvRequestedVolumeChange(unsigned int volL,
		unsigned int volR, bool adjustLocally)
{
    Logger::LogDebug("PlayerController::OnSrvRequestedVolumeChange - VolL: %d, VolR: %d, Apply locally: %s",
    		volL, volR, adjustLocally ? "true" : "false");

    //if internal vol ctrl is enabled signal volume change to player
    if (this->volCtrl!=NULL)
    	this->volCtrl->SetVolume(volL, volR);

    //inform outside anyway
    this->squeezeClientEventInterface->OnVolumeChanged(volL, volR);
}

void PlayerController::OnSrvRequestedLoadStream(
		IPlayer::StreamingServerInfoT *srvInfo, IPlayer::AudioFormatT *audioFMT,
		bool autostart)
{
	this->UpdatePlayerStatusData();
	this->commandFactory->SendSTMcCmd(&this->lmsPlayerStatusData);

	if (!player->LoadStream(srvInfo, audioFMT, autostart))
	{
		Logger::LogError("Unable to play stream.");
		//TODO: There is sort of error reporting to server -> Find out what and how
	}
	this->commandFactory->SendSTMsCmd(&this->lmsPlayerStatusData);
}

void PlayerController::OnPlayerStateChanged(
		IPlayer::PlayerStateT state)
{
    Logger::LogInfo("Player changed to state: %s",IPlayer::PLAYER_STATE_NAMES[state]);
    //state changes of player are just of interest when switched on
    if (this->clientState!=SqueezeClient::SqueezeClientStateT::POWERED_OFF)
    {
    	//just cast, state maping of enums is done in SqueezeClient.h
    	this->clientState=(SqueezeClient::SqueezeClientStateT)state;
    	this->squeezeClientEventInterface->OnClientStateChanged(this->clientState);
    }
}

SqueezeClient::SqueezeClientStateT PlayerController::GetClientState()
{
	return this->clientState;
}

#warning following functions away later

void PlayerController::SignalFakeFaster()
{
	//add 10ms on playtime
    Logger::LogDebug("PlayerController::SignalFakeFaster - Not yet implemented.");
}

void PlayerController::SignalFakeSlower()
{
	//sub 10ms on playtime
    Logger::LogDebug("PlayerController::SignalFakeSlower - Pausing for 50ms to simulate to be slower.");
    this->player->Pause();
	g_usleep(50*1000);
    this->player->Resume();
}
