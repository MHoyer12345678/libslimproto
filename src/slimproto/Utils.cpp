/*
 * Utils.cpp
 *
 *  Created on: 25.01.2021
 *      Author: joe
 */

#include "../slimproto/Utils.h"

#include <time.h>

using namespace slimprotolib;

Utils::Utils()
{
}

Utils::~Utils()
{
}

uint32_t Utils::GetTimeMS()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
