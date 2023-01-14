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
#include "squeezeclient/IGstPlayerConfig.h"

namespace squeezeclient {

class GstPlayerTest : public IGstPlayerConfig {
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

	virtual const char *GetGstAudioSinkElementType();

	virtual void DoConfigureSinkElement(GstElement *sinkElement);
};

} /* namespace squeezeclient */

#endif /* SRC_TEST_GSTPLAYERTEST_H_ */
