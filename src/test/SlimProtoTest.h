/*
 * SlimProtoTest.h
 *
 *  Created on: 23.01.2021
 *      Author: joe
 */

#ifndef SRC_TEST_SLIMPROTOTEST_H_
#define SRC_TEST_SLIMPROTOTEST_H_

#include <glib.h>

#include "SqueezeClient.h"

namespace slimprotolib {

class SlimProtoTest {
private:
	GIOChannel *keyEvents;

	static SlimProtoTest* instance;

	SqueezeClient *squeezeClient;

	GMainLoop *mainloop;

	int returnCode;

	SlimProtoTest();

	~SlimProtoTest();

	static gboolean UnixSignalHandler(gpointer user_data);

	static gboolean	OnKeyEvent(GIOChannel *source,
	            GIOCondition condition,
	            gpointer data);

	void OnKeyPressed(uint16_t type, uint16_t code, uint32_t value);
public:
	static SlimProtoTest* Instance();

	bool Init(int argc, char *argv[]);

	void DeInit();

	void Run();

	int GetReturnCode();

};

} /* namespace slimprotolib */

#endif /* SRC_TEST_SLIMPROTOTEST_H_ */
