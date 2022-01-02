/*
 * LMSConnection.h
 *
 *  Created on: 24.01.2021
 *      Author: joe
 */

#ifndef SRC_SLIMPROTO_LMSCONNECTION_H_
#define SRC_SLIMPROTO_LMSCONNECTION_H_

#include <glib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "SqueezeClient.h"

namespace squeezeclient
{

class LMSConnection
{

public:
	class IConnectionListener
	{
	public:
		virtual ~IConnectionListener(){};
		virtual void OnConnectionEstablished()=0;
		virtual void OnConnectingServerFailed(int &retryTimeoutMS)=0;
		virtual void OnServerConnectionLost(int &retryTimeoutMS, SqueezeClient::ConnectLostReasonT reason)=0;
		virtual void OnCommandReceived(void *data, uint16_t cmdSize)=0;
	};

private:
	typedef enum StateT
	{
		__NOT_SET						= 0,
		DISCONNECTED					= 1,
		CONNECTING_DISCOVERED_LMS		= 2,
		CONNECTING_CONFIGURED_LMS		= 3,
		CONNECTED						= 4,
		FAILED_CONNECTING				= 5,
		CONNECTION_ERROR				= 6

	} StateT;

	class SocketConnection
	{
	public:
		int socket;

		guint gEventId;
	};

	SocketConnection discoveryConnection;

	SocketConnection lmsConnection;

	StateT state;

	SqueezeClient::IClientConfiguration *sclConfig;

	in_addr_t serverIp;

	IConnectionListener *connectionListener;

	static gboolean	OnDataReceived(gint fd,GIOCondition condition, gpointer userData);

	static gboolean	OnDiscoveryConnectionDataReceived(gint fd,GIOCondition condition, gpointer userData);

	static gboolean	OnReconnectTimeoutElapsed(gpointer userData);

	static gboolean	OnStartConnecting(gpointer userData);

	bool SendDiscoveryMessage();

	void OnDataReceived();

	void OnDiscoveryDataReceived();

	void DoStartConnecting();

	bool ConnectToDiscoveredServer(struct sockaddr_in s);

	bool ConnectToConfiguredServer();

	bool FinalizeConnection(int aSocket, in_addr_t serverAddress);

	void CloseConnection(SocketConnection *conPtr);

	void KickOffReconnect(int retryTimeoutMS);

	ssize_t SafeReadNBytes(char *buffer, size_t bytes2Read);

	bool EvaluateReadResult(ssize_t result, size_t expectedSize);

	void EnterDisconnected();

	void EnterConnectingDiscoveredLMS();

	void EnterConnectingConfiguredLMS();

	void EnterConnected();

	void EnterFailedConnecting();

	void EnterConnectionError(SqueezeClient::ConnectLostReasonT reason);

public:
	LMSConnection(IConnectionListener *conlistener);

	~LMSConnection();

	bool Init(SqueezeClient::IClientConfiguration *sclConfig);

	void DeInit();

	void StartConnecting();

	void Disconnect();

	bool SendCmd(void *data, int size);

	bool IsConnected();

	in_addr_t GetServerIp();
};

} /* namespace squeezeclient */

#endif /* SRC_SLIMPROTO_LMSCONNECTION_H_ */
