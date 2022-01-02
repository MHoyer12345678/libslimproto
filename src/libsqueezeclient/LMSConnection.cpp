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
#include <netdb.h>
#include <errno.h>

#include <cpp-app-utils/Logger.h>

#include "CommandFactory.h"

#define MAX_CMD_SIZE 		8096

#define LMS_DISCOVERY_PORT	3483

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
		connectionListener(listener),
		serverIp(0),
		state(StateT::__NOT_SET),
		sclConfig(NULL)
{
	this->discoveryConnection.socket=-1;
	this->discoveryConnection.gEventId=0;
	this->lmsConnection.socket=-1;
	this->lmsConnection.gEventId=0;

}

LMSConnection::~LMSConnection()
{
}

bool LMSConnection::Init(SqueezeClient::IClientConfiguration *sclConfig)
{
	Logger::LogDebug("LMSConnection::Init - Initalizing LMSConnection.");
	this->state=StateT::DISCONNECTED;
	this->sclConfig=sclConfig;
	return true;
}

void LMSConnection::DeInit()
{
	Logger::LogDebug("LMSConnection::DeInit - DeInitalizing LMS connection.");
	if (this->IsConnected())
		this->Disconnect();
	this->state=StateT::__NOT_SET;
	this->sclConfig=NULL;
}

void LMSConnection::StartConnecting()
{
	//start connecting out of mainloop to be a bit more robust regarding initialization
	//of components and usage of squeezeclient API in combination with callbacks
	g_idle_add(OnStartConnecting,this);
}

gboolean LMSConnection::OnStartConnecting(gpointer userData)
{
	LMSConnection* instance=(LMSConnection *)userData;
	instance->DoStartConnecting();
	return FALSE;
}

void LMSConnection::DoStartConnecting()
{
	if (this->state!=StateT::DISCONNECTED && this->state!=StateT::FAILED_CONNECTING)
		return;

	if (this->sclConfig->GetServerAddress()!=NULL)
	{
		Logger::LogDebug("LMSConnection::StartConnecting - LMS server found in configuration file. Trying to connect to it.");
		this->EnterConnectingConfiguredLMS();
	}
	else
	{
		Logger::LogDebug("LMSConnection::StartConnecting - Auto discovery of LMS server requested.");
		this->EnterConnectingDiscoveredLMS();
	}
}

void LMSConnection::Disconnect()
{
	int retryTimeoutMS;

	if (this->state==StateT::DISCONNECTED || this->state==StateT::__NOT_SET)
		return;

	if (this->connectionListener!=NULL)
			this->connectionListener->OnServerConnectionLost(retryTimeoutMS, SqueezeClient::MANUAL_DISCONNECT);

	this->EnterDisconnected();

	//ignoring retryTimeoutMS -> no automatic reconnect on explicit disconnect call
}

bool LMSConnection::IsConnected()
{
	return this->state==StateT::CONNECTED;
}

in_addr_t LMSConnection::GetServerIp()
{
	return this->serverIp;
}

bool LMSConnection::SendCmd(void *data, int size)
{
	if (!this->IsConnected())
		return false;

	if (write(this->lmsConnection.socket,data,size)!=size)
	{
		Logger::LogError("Unable to write %d bytes of data. Closing connection.", size);
		this->EnterConnectionError(SqueezeClient::CONNECTION_RW_ERROR);
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

	assert(this->lmsConnection.socket!=-1);

	while(retries>0)
	{
		int br;
		br=read(this->lmsConnection.socket, dataPtr, bytes2Read);
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
		this->EnterConnectionError(SqueezeClient::SERVER_CLOSED);
		return false;
	}
	else if (result==-1)
	{
		Logger::LogError("Unable to read received data: %s. Closing connection.", strerror(errno));
		this->EnterConnectionError(SqueezeClient::CONNECTION_RW_ERROR);
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

	if (this->lmsConnection.socket==-1)
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

void LMSConnection::CloseConnection(SocketConnection *conPtr)
{
	if (conPtr->socket!=-1)
	{
		g_source_remove(conPtr->gEventId);
		close(conPtr->socket);
		conPtr->socket=-1;
		conPtr->gEventId=0;
	}
}

bool LMSConnection::SendDiscoveryMessage()
{
	struct sockaddr_in d;
	const char *buf;
	int disc_sock = socket(AF_INET, SOCK_DGRAM, 0);
	socklen_t enable = 1;
	setsockopt(disc_sock, SOL_SOCKET, SO_BROADCAST, (const void *)&enable, sizeof(enable));
	buf = "e";
	memset(&d, 0, sizeof(d));
	d.sin_family = AF_INET;
	d.sin_port = htons(LMS_DISCOVERY_PORT);
	d.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	if (sendto(disc_sock, buf, 1, 0, (struct sockaddr *)&d, sizeof(d)) < 0)
	{
		Logger::LogError("Error sending discovery broadcast message.");
		return false;
	}

	this->discoveryConnection.socket=disc_sock;
	this->discoveryConnection.gEventId=g_unix_fd_add(disc_sock,G_IO_IN,LMSConnection::OnDiscoveryConnectionDataReceived,this);

	return true;
}

gboolean LMSConnection::OnDiscoveryConnectionDataReceived(gint fd,
		GIOCondition condition, gpointer userData)
{
	LMSConnection *instance=(LMSConnection *)userData;
	instance->OnDiscoveryDataReceived();
	return FALSE;
}

void LMSConnection::OnDiscoveryDataReceived()
{
	char readbuf[10];
	struct sockaddr_in s;

	socklen_t slen = sizeof(s);
	if (this->discoveryConnection.socket==-1) return;

	recvfrom(this->discoveryConnection.socket, readbuf, 10, 0, (struct sockaddr *)&s, &slen);
	Logger::LogDebug("LMSConnection::OnDiscoveryDataReceived - Got response from: %s:%d", inet_ntoa(s.sin_addr), ntohs(s.sin_port));

	close(this->discoveryConnection.socket);
	g_source_remove(this->discoveryConnection.gEventId);

	this->discoveryConnection.socket=-1;
	this->discoveryConnection.gEventId=0;

	if (this->ConnectToDiscoveredServer(s))
		this->EnterConnected();
	else
		this->EnterFailedConnecting();
}

bool LMSConnection::FinalizeConnection(int aSocket, in_addr_t serverAddress)
{
    guint gEventId;

	gEventId=g_unix_fd_add(aSocket,G_IO_IN,LMSConnection::OnDataReceived,this);

	if (fcntl(aSocket, F_SETFL, fcntl(aSocket, F_GETFL, 0) | O_NONBLOCK))
	{
		Logger::LogError("Unable to set connection socket to non blocking: %s", strerror(errno));
		close(aSocket);
		return false;
	}

	this->lmsConnection.socket=aSocket;
	this->lmsConnection.gEventId=gEventId;
	this->serverIp=serverAddress;

	Logger::LogDebug("LMSConnect::Connect - Connected to LMS. IP: %d.%d.%d.%d", this->serverIp & 0xFF,
			(this->serverIp >> 8) & 0xFF, (this->serverIp >> 16) & 0xFF, (this->serverIp >> 24));

	return true;
}

bool LMSConnection::ConnectToDiscoveredServer(struct sockaddr_in s)
{
    int aSocket;
    in_addr	serverInAddr;

	aSocket = socket(s.sin_family, SOCK_STREAM, 0);

	if (aSocket == -1)
	{
		Logger::LogError("Unable to create socket for discovered server.");
		return false;
	}

	if (connect(aSocket, (struct sockaddr *)&s, sizeof(s)) == -1)
	{
		Logger::LogError("Unable connect to server.");
		close(aSocket);
		return false;
	}

	return this->FinalizeConnection(aSocket, s.sin_addr.s_addr);
}

bool LMSConnection::ConnectToConfiguredServer()
{
	const char *server;
	const char *port;

	struct addrinfo hints;
    struct addrinfo *result;
    in_addr	serverInAddr;
    int aSocket;
    int ret;

	server=this->sclConfig->GetServerAddress();
	port=this->sclConfig->GetServerPort();

	Logger::LogDebug("LMSConnection::Connect - Connecting to LMS. Server: %s:%s",server,port);

    /* Obtain address(es) matching host/port. */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;    /* LMS only supports IPv4 currently*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;          /* Any protocol */

    ret = getaddrinfo(server, port, &hints, &result);
    if (ret != 0)
    {
    	Logger::LogError("Unable to resolve server address %s:%s: %s",
    			server, port, gai_strerror(ret));
    	return false;
	}

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */
	for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
	{
		struct sockaddr_in *inet_addr=(struct sockaddr_in *)rp->ai_addr;
		Logger::LogDebug("LMSConnection::ConnectToConfiguredServer - Trying to connect to: %s:%d (family=%d, socktype=%d, protocol=%d)",
				inet_ntoa(inet_addr->sin_addr), ntohs(inet_addr->sin_port), rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		aSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (aSocket == -1)
		   continue;

		if (connect(aSocket, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			serverInAddr=inet_addr->sin_addr;
			break;                  /* Success */
		}

		Logger::LogInfo("Unable to connect to %s:%d: %s",
				inet_ntoa(inet_addr->sin_addr), ntohs(inet_addr->sin_port), strerror(errno));

		close(aSocket);
		aSocket=-1;
	}

	freeaddrinfo(result);           /* No longer needed */

	if (aSocket==-1)
	{
		Logger::LogError("Unable connect to server.");
		return false;
	}

	return this->FinalizeConnection(aSocket, serverInAddr.s_addr);
}

void LMSConnection::EnterConnectingDiscoveredLMS()
{
	Logger::LogDebug("LMSConnection::EnterConnectingDiscoveredLMS - Try to discover and connect to LMS.");
	assert(this->state==DISCONNECTED || this->state==FAILED_CONNECTING);
	this->state=CONNECTING_DISCOVERED_LMS;
	if (!this->SendDiscoveryMessage())
		this->EnterFailedConnecting();
}

void LMSConnection::EnterConnectingConfiguredLMS()
{
	Logger::LogDebug("LMSConnection::EnterConnectingConfiguredLMS - Try to connect to configured LMS.");
	assert(this->state==DISCONNECTED || this->state==FAILED_CONNECTING);
	this->state=CONNECTING_CONFIGURED_LMS;
	if (this->ConnectToConfiguredServer())
		this->EnterConnected();
	else
		this->EnterFailedConnecting();
}

void LMSConnection::EnterConnected()
{
	Logger::LogDebug("LMSConnection::EnterConnected - Connection to LMS established.");
	assert(this->state==CONNECTING_DISCOVERED_LMS || this->state==CONNECTING_CONFIGURED_LMS);
	this->state=StateT::CONNECTED;

	if (this->connectionListener!=NULL)
		this->connectionListener->OnConnectionEstablished();
}

void LMSConnection::EnterDisconnected()
{
	Logger::LogDebug("LMSConnection::EnterDisconnected - Disconnected from LMS");

	this->CloseConnection(&this->lmsConnection);
	this->CloseConnection(&this->discoveryConnection);

	this->state=StateT::DISCONNECTED;
}

void LMSConnection::EnterFailedConnecting()
{
	int retryTimeoutMS=RETRY_TIMEOUT_NO_RETRY;

	Logger::LogDebug("LMSConnection::EnterFailedConnecting - Failed connecting to LMS.");
	this->state=StateT::FAILED_CONNECTING;

	this->EnterDisconnected();

	if (this->connectionListener!=NULL)
		this->connectionListener->OnConnectingServerFailed(retryTimeoutMS);

	this->KickOffReconnect(retryTimeoutMS);
}

void LMSConnection::EnterConnectionError(SqueezeClient::ConnectLostReasonT reason)
{
	int retryTimeoutMS=RETRY_TIMEOUT_NO_RETRY;

	Logger::LogDebug("LMSConnection::EnterConnectionError - Error on existing LMS connecting.");
	this->state=StateT::CONNECTION_ERROR;

	this->EnterDisconnected();

	if (this->connectionListener!=NULL)
		this->connectionListener->OnServerConnectionLost(retryTimeoutMS, reason);

	this->KickOffReconnect(retryTimeoutMS);
}

gboolean LMSConnection::OnReconnectTimeoutElapsed(gpointer userData)
{
	LMSConnection *instance=(LMSConnection *)userData;
	if (instance->state==DISCONNECTED)
		instance->StartConnecting();
	//just one timeout event needed
	return FALSE;
}

void LMSConnection::KickOffReconnect(int retryTimeoutMS)
{
	if (retryTimeoutMS==RETRY_TIMEOUT_NO_RETRY)
		return;
	else if (retryTimeoutMS==RETRY_TIMEOUT_IMMEDIATE_RETRY)
		this->StartConnecting();
	else
		g_timeout_add(retryTimeoutMS, LMSConnection::OnReconnectTimeoutElapsed, this);
}

} /* namespace squeezeclient */
