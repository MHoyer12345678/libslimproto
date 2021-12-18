/*
 * SCBConfig.cpp
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#include "SCBConfig.h"

using namespace squeezeclient;

SCBConfig::SCBConfig()
{
}

SCBConfig::~SCBConfig()
{
}

const char *SCBConfig::GetDevice()
{
	return "default";
}
