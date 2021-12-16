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
	this->config=new SCBConfig();
	this->controller=new SCBController(this->config);

	this->mainloop=g_main_loop_new(NULL,FALSE);
}

SqueezeClientBin::~SqueezeClientBin()
{
	g_main_loop_unref (this->mainloop);

	delete this->controller;
	delete this->config;
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

    g_unix_signal_add(1, &UnixSignalHandler, this);
    g_unix_signal_add(2, &UnixSignalHandler, this);
    g_unix_signal_add(15, &UnixSignalHandler, this);

    if (!this->controller->Init())
    	return false;

    this->controller->KickOff();
   	return true;
}

void SqueezeClientBin::DeInit()
{
	this->controller->DeInit();

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
	Logger::LogDebug("SqueezeClientBin::Run -> Going to enter main loop.");
	g_main_loop_run(this->mainloop);
	Logger::LogDebug("SqueezeClientBin::Run -> Main loop left. Shutting down.");
}

int SqueezeClientBin::GetReturnCode()
{
	return this->returnCode;
}
