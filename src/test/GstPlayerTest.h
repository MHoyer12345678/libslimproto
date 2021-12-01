/*
 * GstPlayerTest.h
 *
 *  Created on: 23.01.2021
 *      Author: joe
 */

#ifndef SRC_TEST_GSTPLAYERTEST_H_
#define SRC_TEST_GSTPLAYERTEST_H_

#include <glib.h>

#include "GstPlayer.h"
#include "GstPlayerConfig.h"

namespace slimprotolib {

class GstPlayerTest {
private:
	static GstPlayerTest* instance;

	GstPlayer *player;

	GMainLoop *mainloop;

	int returnCode;

	GstPlayerTest();

	~GstPlayerTest();

	static gboolean UnixSignalHandler(gpointer user_data);

	static gboolean OnTimeoutElapsed(gpointer user_data);

public:
	static GstPlayerTest* Instance();

	bool Init(int argc, char *argv[]);

	void DeInit();

	void Run();

	int GetReturnCode();

};

} /* namespace slimprotolib */

#endif /* SRC_TEST_GSTPLAYERTEST_H_ */
