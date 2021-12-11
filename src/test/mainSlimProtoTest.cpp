/*
 * main.cpp
 *
 *  Created on: 22.10.2017
 *      Author: joe
 */

#include "cpp-app-utils/Logger.h"
#include "test/SlimProtoTest.h"

using namespace squeezeclient;
using namespace CppAppUtils;

int main (int argc, char **argv)
{
	int result;

	//TODO: Change sort of
	Logger::SetLogLevel(Logger::DEBUG);
	//Logger::SetLogLevel(Logger::INFO);

	SlimProtoTest* slimProtoTestInstance=SlimProtoTest::Instance();
	if (slimProtoTestInstance->Init(argc,argv))
		slimProtoTestInstance->Run();

	result=slimProtoTestInstance->GetReturnCode();

	slimProtoTestInstance->DeInit();

	return result;
}
