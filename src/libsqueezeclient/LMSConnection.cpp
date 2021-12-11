/*
 * LMSConnection.cpp
 *
 *  Created on: 24.01.2021
 *      Author: joe
 */

#include "LMSConnection.h"

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <glib-unix.h>
#include <glib.h>
#include <stdint.h>
#include <assert.h>

#include <cpp-app-utils/Logger.h>

#include "CommandFactory.h"

#define MAX_CMD_SIZE 8096

using CppAppUtils::Logger;

namespace squeezeclient {

#pragma pack(push, 1)

typedef struct SrvCmdBaseT
{
	uint16_t cmdSize;
	char cmd[4];
} SrvCmdBaseT;

#pragma pack(pop)


LMSConnection::LMSConnection(IConnectionListener *listener) :
		connectionSocket(-1),
		connectionListener(listener),
		serverIp(0)
{
}

LMSConnection::~LMSConnection()
{
}

bool LMSConnection::Init()
{
	Logger::LogDebug("LMSConnection::Init - Initalizing LMSConnection.");
	return true;
}

void LMSConnection::DeInit()
{
	Logger::LogDebug("SlimProtoTest::DeInit - DeInitalizing LMS connection.");
	if (this->IsConnected())
		this->Disconnect();
}

bool LMSConnection::Connect()
{
	//TODO:
	//- configure ip address and port
	//- resolve ip address if name

	struct sockaddr_in server;
	int aSocket;

	if (this->IsConnected())
		this->Disconnect();

	Logger::LogDebug("LMSConnection::Connect - Connecting to LMS");

	aSocket=socket(AF_INET , SOCK_STREAM , 0);

	if (aSocket == -1)
	{
		Logger::LogError("Unable to create INET socket: %s", strerror(errno));
		return false;
	}

	this->serverIp=inet_addr("192.168.178.29");
	server.sin_addr.s_addr = this->serverIp;
	server.sin_family = AF_INET;
	server.sin_port = htons(3483);

		//Connect to remote server
	if (connect(aSocket, (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		Logger::LogError("Unable connect to server: %s", strerror(errno));
		close(aSocket);
		return false;
	}

	g_unix_fd_add(aSocket,G_IO_IN,LMSConnection::OnDataReceived,this);

	if (fcntl(aSocket, F_SETFL, fcntl(aSocket, F_GETFL, 0) | O_NONBLOCK))
	{
		Logger::LogError("Unable to set connection socket to non blocking: %s", strerror(errno));
		close(aSocket);
		return false;
	}

	this->connectionSocket=aSocket;

	Logger::LogDebug("SlimProtoTest::Connect - Connected with LMS");

	if (this->connectionListener!=NULL)
		this->connectionListener->OnConnectionEstablished();

	return true;
}

void LMSConnection::DoDisconnect(bool isInitiatedByClient)
{
	Logger::LogDebug("SlimProtoTest::DoDisconnect - Disconnecting from LMS");
	if (this->connectionSocket!=-1)
	{
		close(this->connectionSocket);
		this->connectionSocket=-1;
		if (this->connectionListener!=NULL)
			this->connectionListener->OnConnectionLost(isInitiatedByClient);
	}
}

void LMSConnection::Disconnect()
{
	this->DoDisconnect(true);
}

bool LMSConnection::SendCmd(void *data, int size)
{
	if (!this->IsConnected())
		return false;

	if (write(this->connectionSocket,data,size)!=size)
	{
		Logger::LogError("Unable to write %d bytes of data. Closing connection.", size);
		this->Disconnect();
		return false;
	}

	return true;
}

gboolean LMSConnection::OnDataReceived(gint fd, GIOCondition condition, gpointer userData)
{
	LMSConnection *instance=(LMSConnection *)userData;
	instance->OnDataReceived();
	return TRUE;
}

ssize_t LMSConnection::SafeReadNBytes(char *buffer, size_t bytes2Read)
{
	ssize_t bytesRead=0;
	char *dataPtr=buffer;
	int retries=10;

	assert(this->connectionSocket!=-1);

	while(retries>0)
	{
		int br;
		br=read(this->connectionSocket, dataPtr, bytes2Read);
		if (br<=0)
			return br;
		bytesRead+=br;
		bytes2Read-=br;
		if (bytes2Read>0)
		{
			usleep(100000);
			dataPtr+=br;
			retries--;
		}
		else
			break;
	}

	return bytesRead;
}

bool LMSConnection::EvaluateReadResult(ssize_t result, size_t expectedSize)
{
	if (result==0)
	{
		Logger::LogDebug("LMSConnection::EvaluateReadResult - Server closed connection.");
		this->DoDisconnect(false);
		return false;
	}
	else if (result==-1)
	{
		Logger::LogError("Unable to read received data: %s. Closing connection.", strerror(errno));
		this->DoDisconnect(false);
		return false;	}
	else if (result!=expectedSize)
	{
		Logger::LogError("Corrupt data from server received. # Bytes received: %ld, expected: %ld. Ignoring the data.",
				result, expectedSize);
		return false;
	}

	return true;
}

void LMSConnection::OnDataReceived()
{
	char buffer[MAX_CMD_SIZE+1];
	ssize_t bytesRead;
	SrvCmdBaseT *srvCmd;
	uint16_t cmdSize;

	if (this->connectionSocket==-1)
		return;

	bytesRead=this->SafeReadNBytes(buffer, sizeof(SrvCmdBaseT));
	if (!this->EvaluateReadResult(bytesRead, sizeof(SrvCmdBaseT))) return;

	srvCmd=(SrvCmdBaseT *)buffer;
	cmdSize=ntohs(srvCmd->cmdSize);

	//check for potential buffer overrun
	if (cmdSize+2 > MAX_CMD_SIZE)
	{
		Logger::LogError("Received message from server that exceeds the currently "
				"set message buffer size of %d bytes. Message: %d bytes", MAX_CMD_SIZE, cmdSize+2);
		assert(NULL!=NULL);
	}

	//cmdsize-4: 4 bytes command string are already read with SrvCmdBaseT, 2 bytes cmdSize does
	//not belong to cmdSize
	bytesRead=this->SafeReadNBytes(buffer+bytesRead, cmdSize-4);
	if (!this->EvaluateReadResult(bytesRead, cmdSize-4)) return;

	Logger::LogDebug("LMSConnection::OnDataReceived - Received command \'%.4s\' from server."
					" Command size: %u", srvCmd->cmd, cmdSize);

	//not all commands end with a string. Anyway, terminating all commands with '\0', buffer has size of MAX_CMD_SIZE+1 to ensure
	//that one byte is left in any case
	buffer[cmdSize+2]='\0';
	if (this->connectionListener!=NULL)
		this->connectionListener->OnCommandReceived(&srvCmd->cmd, cmdSize);
}

bool LMSConnection::IsConnected()
{
	return this->connectionSocket!=-1;
}

in_addr_t LMSConnection::GetServerIp()
{
	return this->serverIp;
}

} /* namespace squeezeclient */
