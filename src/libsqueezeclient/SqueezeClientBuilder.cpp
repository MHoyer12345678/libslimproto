/*
 * SqueezeClientBuilder.cpp
 *
 *  Created on: 20.12.2021
 *      Author: joe
 */
#include <assert.h>

#include "squeezeclient/SqueezeClientBuilder.h"

#include "SqueezeClientImpl.h"

#include "GstPlayer.h"
#include "AlsaVolumeControl.h"

using namespace squeezeclient;

SqueezeClientBuilder::SqueezeClientBuilder(SqueezeClient::IEventInterface *eventInterface,
		SqueezeClient::IClientConfiguration *sclConfig) :
		sclEventInterface(eventInterface),
		sclConfig(sclConfig),
		player(NULL),
		volCtrl(NULL),
		builderFlags(__BUILDER_FLAG_RESET)
{
}

SqueezeClientBuilder::~SqueezeClientBuilder()
{
}

SqueezeClient* SqueezeClientBuilder::CreateInstance()
{
	//check if every items is set which must not be NULL
	assert(this->player!=NULL);
	assert(this->sclConfig!=NULL);
	//volCtrl==NULL -> internal volume control disabled
	//sclEventInterface==NULL -> no events needed outside

	return new SqueezeClientImpl(this->sclEventInterface,this->sclConfig,
			this->player, this->volCtrl, builderFlags);
}

void SqueezeClientBuilder::PlayerUseCustom(IPlayer *player)
{
	assert(this->player==NULL);
	this->player=player;
	//do not mark player as being freed when squeezeclient is deleted
}

void SqueezeClientBuilder::PlayerUseGstDefaultConf()
{
	assert(this->player==NULL);
	this->player=new GstPlayer(GstPlayerDefaultConfig::Instance());
	//player created by us -> mark for deletion when squeeze client is destroyed
	this->builderFlags |= BUILDER_FLAG_DELETE_PLAYER;
}

void SqueezeClientBuilder::PlayerUseGstCustomConf(IGstPlayerConfig *playerConfig)
{
	assert(this->player==NULL);
	this->player=new GstPlayer(playerConfig);
	//player created by us -> mark for deletion when squeeze client is destroyed
	this->builderFlags |= BUILDER_FLAG_DELETE_PLAYER;
}

void SqueezeClientBuilder::VolumeControlUseCustom(IVolumeControl *volControl)
{
	assert(this->volCtrl==NULL);
	this->volCtrl=volControl;
	//do not mark volctrl as being freed when squeezeclient is deleted
}

void SqueezeClientBuilder::VolumeControlUseAlsa(IAlsaVolumeControlConfig *volControlConfig)
{
	assert(this->volCtrl==NULL);
	this->volCtrl=new AlsaVolumeControl(volControlConfig);
	//volume control created by us -> mark for deletion when squeeze client is destroyed
	this->builderFlags |= BUILDER_FLAG_DELETE_VOLCTRL;
}

void SqueezeClientBuilder::DestroyInstance(SqueezeClient *instance)
{
	delete instance;
}
