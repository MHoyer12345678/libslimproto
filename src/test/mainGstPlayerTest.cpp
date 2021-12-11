/*
 * main.cpp
 *
 *  Created on: 22.10.2017
 *      Author: joe
 */

#include "cpp-app-utils/Logger.h"
#include "test/GstPlayerTest.h"

using namespace squeezeclient;
using namespace CppAppUtils;

int main (int argc, char **argv)
{
	int result;

	//TODO: Change sort of
	Logger::SetLogLevel(Logger::DEBUG);

	GstPlayerTest* playerTestInstance=GstPlayerTest::Instance();
	if (playerTestInstance->Init(argc,argv))
		playerTestInstance->Run();

	result=playerTestInstance->GetReturnCode();

	playerTestInstance->DeInit();

	return result;
}
