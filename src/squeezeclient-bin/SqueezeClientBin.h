/*
 * SqueezeClientBin.h
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENTBIN_SQUEEZECLIENTBIN_H_
#define SRC_SQUEEZECLIENTBIN_SQUEEZECLIENTBIN_H_

#include <glib.h>

#include "SqueezeClient.h"
#include "GstPlayerConfig.h"

namespace squeezeclient {

class SqueezeClientBin : public SqueezeClient::IEventInterface, GstPlayerAlsaSinkConfig {
private:
	static SqueezeClientBin* instance;

	SqueezeClient *squeezeClient;

	GMainLoop *mainloop;

	int returnCode;

	SqueezeClientBin();

	~SqueezeClientBin();

	static gboolean UnixSignalHandler(gpointer user_data);

public:
	static SqueezeClientBin* Instance();

	bool Init(int argc, char *argv[]);

	void DeInit();

	void Run();

	int GetReturnCode();

	void OnPlayerNameRequested(char name[1024]);

	void OnUIDRequested(char uid[16]);

	void OnMACAddressRequested(uint8_t mac[6]);

	void OnServerSetsNewPlayerName(const char *newName);

	void OnPowerStateChanged(bool value);

	void OnVolumeChanged(unsigned int volL, unsigned int volR);

	const char *GetDevice();
};

} /* namespace slimprotolib */

#endif /* SRC_TEST_SLIMPROTOTEST_H_ */
