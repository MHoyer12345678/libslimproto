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

namespace squeezeclient
{

class LMSConnection
{

public:
	class IConnectionListener
	{
	public:
		virtual void OnConnectionEstablished()=0;
		virtual void OnConnectionLost(bool isClosedByClient)=0;
		virtual void OnCommandReceived(void *data, uint16_t cmdSize)=0;
	};

private:
	int connectionSocket;

	in_addr_t serverIp;

	IConnectionListener *connectionListener;

	static gboolean	OnDataReceived(gint fd,GIOCondition condition, gpointer userData);

	void OnDataReceived();

	void DoDisconnect(bool isInitiatedByClient);

	ssize_t SafeReadNBytes(char *buffer, size_t bytes2Read);

	bool EvaluateReadResult(ssize_t result, size_t expectedSize);

public:
	LMSConnection(IConnectionListener *conlistener);
	~LMSConnection();

	bool Init();

	void DeInit();

	bool Connect();

	void Disconnect();

	bool SendCmd(void *data, int size);

	bool IsConnected();

	in_addr_t GetServerIp();
};

} /* namespace squeezeclient */

#endif /* SRC_SLIMPROTO_LMSCONNECTION_H_ */
