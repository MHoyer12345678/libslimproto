/*
 * SCBController.h
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#ifndef SRC_SQUEEZECLIENT_BIN_SCBCONTROLLER_H_
#define SRC_SQUEEZECLIENT_BIN_SCBCONTROLLER_H_

#include "SqueezeClient.h"
#include "SCBConfig.h"
#include "SCBController.h"
#include "generated/SqueezeClientInteface.h"

namespace squeezeclient {

class SCBController : public SqueezeClient::IEventInterface
{
private:
	SqueezeClient *squeezeClient;

	SqueezeClientControl  *dbusInterface;

	guint dbusConId;

	GDBusConnection *dbusConnection;

	static void OnBusAcquired(GDBusConnection *pConnection, const gchar* sName, gpointer pUserData);

	static void OnNameAcquired(GDBusConnection *pConnection, const gchar* sName, gpointer pUserData);

	static void OnNameLost(GDBusConnection *pConnection, const gchar* sName, gpointer pUserData);

	static gboolean OnDBusPlayRequested(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			gpointer pUserData);

	static gboolean OnDBusPauseResumeRequested(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			  guchar arg_which, gpointer pUserData);

	static gboolean OnDBusPreviousRequested(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			gpointer pUserData);

	static gboolean OnDBusNextRequested(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			gpointer pUserData);

	static gboolean OnDBusStopRequested(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			gpointer pUserData);

	static gboolean OnDBusVolUp(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			gpointer pUserData);

	static gboolean OnDBusVolDown(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			gpointer pUserData);

	static gboolean OnDBusMute(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			gpointer pUserData);

	static gboolean OnDBusPowerStateChange(SqueezeClientControl *object, GDBusMethodInvocation *invocation,
			guchar arg_state, gpointer pUserData);

public:
	SCBController(SCBConfig *config);

	virtual ~SCBController();

	bool Init();

	void DeInit();

	void KickOff();

	//------------------------ SqueezeClient::IEventInterface ------------------------
	void OnPlayerNameRequested(char name[1024]);

	void OnServerSetsNewPlayerName(const char *newName);

	void OnClientStateChanged(SqueezeClient::SqueezeClientStateT newState);

	void OnVolumeChanged(unsigned int volL, unsigned int volR);

};

} /* namespace squeezeclient */

#endif /* SRC_SQUEEZECLIENT_BIN_SCBCONTROLLER_H_ */
