/*
 * SlimProtoTest.cpp
 *
 *  Created on: 23.01.2021
 *      Author: joe
 */

#include "SqueezeClientBin.h"

#include <cpp-app-utils/Logger.h>
#include <glib-unix.h>

using namespace CppAppUtils;
using namespace squeezeclient;

SqueezeClientBin* SqueezeClientBin::instance=NULL;

SqueezeClientBin::SqueezeClientBin() :
		returnCode(0)
{
	this->mainloop=g_main_loop_new(NULL,FALSE);
	this->squeezeClient=SqueezeClient::NewWithGstPlayerCustomConfig(this, this);
}

SqueezeClientBin::~SqueezeClientBin()
{
	SqueezeClient::Destroy(this->squeezeClient);
	g_main_loop_unref (this->mainloop);
}

SqueezeClientBin* SqueezeClientBin::Instance()
{
	if (SqueezeClientBin::instance==NULL)
		SqueezeClientBin::instance=new SqueezeClientBin();

	return SqueezeClientBin::instance;
}

bool SqueezeClientBin::Init(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	Logger::LogDebug("SqueezeClientBin::Init - Initalizing SqueezeClientBin");

	if (!this->squeezeClient->Init())
		return false;

    g_unix_signal_add(1, &UnixSignalHandler, this);
    g_unix_signal_add(2, &UnixSignalHandler, this);
    g_unix_signal_add(15, &UnixSignalHandler, this);

   	return true;
}

void SqueezeClientBin::DeInit()
{
	this->squeezeClient->DeInit();
	delete this;
	SqueezeClientBin::instance=NULL;

	Logger::LogDebug("SqueezeClientBin::DeInit - DeInitalized SqueezeClientBin");
}

gboolean SqueezeClientBin::UnixSignalHandler(gpointer user_data)
{
	SqueezeClientBin *instance=(SqueezeClientBin *)user_data;
	g_main_loop_quit(instance->mainloop);
	return TRUE;
}

void SqueezeClientBin::Run()
{
    this->squeezeClient->KickOff();
	Logger::LogDebug("SqueezeClientBin::Run -> Going to enter main loop.");
	g_main_loop_run(this->mainloop);
	Logger::LogDebug("SqueezeClientBin::Run -> Main loop left. Shutting down.");
}

int SqueezeClientBin::GetReturnCode()
{
	return this->returnCode;
}

void SqueezeClientBin::OnPlayerNameRequested(char name[1024])
{
	strncpy(name, "A test player2",1023);
	Logger::LogInfo("Client requested player name from us.");
}

void SqueezeClientBin::OnUIDRequested(char uid[16])
{
	memcpy(uid, "Dies ist eine id", 16);
	Logger::LogInfo("Client requested uid from us.");
}

void SqueezeClientBin::OnMACAddressRequested(uint8_t mac[6])
{
	uint8_t MAC_ADDRESS[6]={0x5C,0xE0, 0xC5, 0x49, 0x54, 0xAD};
	memcpy(mac, MAC_ADDRESS,6);
	Logger::LogInfo("Client requested mac address from us.");
}

void SqueezeClientBin::OnServerSetsNewPlayerName(const char *newName)
{
	Logger::LogInfo("Server changed our name to: %s", newName);
}

void SqueezeClientBin::OnPowerStateChanged(bool value)
{
	Logger::LogInfo("Server changed our power state to: %s", value ? "On" : "Off");
}

void SqueezeClientBin::OnVolumeChanged(unsigned int volL, unsigned int volR)
{
	Logger::LogInfo("Server changed our volume: L=%u, R=%u", volL, volR);
}

const char* SqueezeClientBin::GetDevice()
{
	return "pulse";
}

