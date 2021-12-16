/*
 * SqueezeClientBin.h
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENTBIN_SQUEEZECLIENTBIN_H_
#define SRC_SQUEEZECLIENTBIN_SQUEEZECLIENTBIN_H_

#include <glib.h>

#include "SCBConfig.h"
#include "SCBController.h"

namespace squeezeclient {

class SqueezeClientBin {
private:
	static SqueezeClientBin* instance;

	SCBConfig *config;

	SCBController *controller;

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
};

} /* namespace squeezeclient */

#endif /* SRC_TEST_SLIMPROTOTEST_H_ */
