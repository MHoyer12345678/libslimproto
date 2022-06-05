/*
 * SCBController.cpp
 *
 *  Created on: 11.12.2021
 *      Author: joe
 */

#include "SCBController.h"

#include "squeezeclient/SqueezeClientBuilder.h"
#include "cpp-app-utils/Logger.h"

#define SCB_BUSNAME			"Squeeze.Client"
#define SCB_OBJECT_PATH 	"/SqueezeClient"

using namespace CppAppUtils;
using namespace squeezeclient;

SCBController::SCBController() :
		dbusConId(0),
		dbusConnection(NULL),
		squeezeClient(NULL)
{
	this->dbusInterface=squeeze_client_control_skeleton_new();
}

SCBController::~SCBController()
{
	g_object_unref(this->dbusInterface);
}


bool SCBController::Init(SCBConfig *config)
{
	SqueezeClientBuilder sueezeClientBuilder(this, config);

	sueezeClientBuilder.PlayerUseGstCustomConf(config);
	if (config->IsInternalVolumeCtrlEnabled())
		sueezeClientBuilder.VolumeControlUseAlsa(config);
	else
		Logger::LogDebug("SCBController::SCBController - No internal volume control used.");
	this->squeezeClient=sueezeClientBuilder.CreateInstance();

	if (!this->squeezeClient->Init(false))
		return false;

	this->dbusConId =  g_bus_own_name(G_BUS_TYPE_SYSTEM,SCB_BUSNAME,
			G_BUS_NAME_OWNER_FLAGS_NONE,SCBController::OnBusAcquired,
			SCBController::OnNameAcquired,SCBController::OnNameLost,
			this, NULL);

	return true;
}

void SCBController::DeInit()
{
	if (this->squeezeClient!=NULL)
	{
		this->squeezeClient->DeInit();
		SqueezeClientBuilder::DestroyInstance(this->squeezeClient);
		this->squeezeClient=NULL;
	}
}

void SCBController::KickOff()
{
    this->squeezeClient->StartConnectingServer();
}

void SCBController::OnPlayerNameRequested(char name[1024])
{
	strncpy(name, "A test player2",1023);
	Logger::LogInfo("Client requested player name from us.");
}

void SCBController::OnServerSetsNewPlayerName(const char *newName)
{
	Logger::LogInfo("Server changed our name to: %s", newName);
}

void SCBController::OnClientStateChanged(SqueezeClient::SqueezeClientStateT newState)
{
	Logger::LogDebug("SCBController::OnClientStateChanged - Client state changed to: %s", SqueezeClient::CLIENT_STATE_NAMES[newState]);
	Logger::LogInfo("Client state changed to: %s", SqueezeClient::CLIENT_STATE_NAMES[newState]);
	squeeze_client_control_set_squeeze_client_state(this->dbusInterface, (uint8_t)newState);
}

gboolean SCBController::OnDBusPlayRequested(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation, gpointer pUserData)
{
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusPlayRequested - DBUS client requested us to play.");
	squeeze_client_control_complete_request_play(object,invocation);
	controller->squeezeClient->SignalPlayButtonPressed();

	return TRUE;
}

gboolean SCBController::OnDBusPauseResumeRequested(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation,
		guchar arg_which, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	SqueezeClient::SqueezeClientStateT clientState=controller->squeezeClient->GetState();

	Logger::LogDebug("SCBController::OnDBusPauseResumeRequested - DBUS client requested us to pause/resume: %d", arg_which);
	squeeze_client_control_complete_request_pause_resume (object,invocation);

	if (arg_which==SqueezeClient::PauseResumeModeT::TOGGLE)
		controller->squeezeClient->SignalPauseButtonPressed();
	else if (arg_which==SqueezeClient::PauseResumeModeT::PAUSE
			&& clientState==SqueezeClient::SqueezeClientStateT::PLAYING)
		controller->squeezeClient->SignalPauseButtonPressed();
	else if (arg_which==SqueezeClient::PauseResumeModeT::RESUME
			&& clientState==SqueezeClient::SqueezeClientStateT::PAUSED)
		controller->squeezeClient->SignalPauseButtonPressed();

	return TRUE;
}

gboolean SCBController::OnDBusPreviousRequested(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusPreviousRequested - DBUS client requested us to change to previous title.");
	squeeze_client_control_complete_request_prev(object,invocation);

	controller->squeezeClient->SignalPreviousButtonPressed();

	return TRUE;
}

gboolean SCBController::OnDBusNextRequested(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusNextRequested - DBUS client requested us to change to next title.");
	squeeze_client_control_complete_request_next (object,invocation);

	controller->squeezeClient->SignalNextButtonPressed();

	return TRUE;
}

gboolean SCBController::OnDBusStopRequested(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusStopRequested - DBUS client requested us to stop playing.");
	squeeze_client_control_complete_request_stop(object,invocation);

	Logger::LogError("Stop request not yet supported by SqueezeClient.");

	return TRUE;
}

gboolean SCBController::OnDBusVolUp(SqueezeClientControl *object,
		GDBusMethodInvocation *invocation, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusVolUp - DBUS client requested us to decrease volume.");
	squeeze_client_control_complete_request_vol_up(object,invocation);
	controller->squeezeClient->SignalVolUpButtonPressed();

	return TRUE;
}

gboolean SCBController::OnDBusVolDown(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusVolDown - DBUS client requested us to increase volume.");
	squeeze_client_control_complete_request_vol_down(object,invocation);
	controller->squeezeClient->SignalVolDownButtonPressed();

	return TRUE;
}

gboolean SCBController::OnDBusMute(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusMute - DBUS client requested us to mute/unmute.");
	squeeze_client_control_complete_request_mute(object,invocation);
	controller->squeezeClient->SignalMuteButtonPressed();

	return TRUE;
}

gboolean SCBController::OnDBusPowerStateChange(
		SqueezeClientControl *object, GDBusMethodInvocation *invocation,
		guchar arg_state, gpointer pUserData) {
	SCBController *controller=(SCBController *)pUserData;
	Logger::LogDebug("SCBController::OnDBusPowerStateChange - DBUS client requested us to change power state to %d", arg_state);
	squeeze_client_control_complete_request_power_state_change(object,invocation);

	controller->squeezeClient->SignalPowerButtonPressed((SqueezeClient::PowerSignalT)arg_state);

	return TRUE;
}

void SCBController::OnVolumeChanged(unsigned int volL, unsigned int volR)
{
	Logger::LogDebug("SCBController::OnVolumeChanged - Server changed our volume: L=%u, R=%u", volL, volR);

	squeeze_client_control_set_volume_l(this->dbusInterface, volL);
	squeeze_client_control_set_volume_r(this->dbusInterface, volR);

	squeeze_client_control_emit_volume_changed(this->dbusInterface);
}

void SCBController::OnConnectingServerFailed(int &retryTimeoutMS)
{
#warning connect to configuration file
	retryTimeoutMS=1000;
}

void SCBController::OnServerConnectionLost(int &retryTimeoutMS, SqueezeClient::ConnectLostReasonT reason)
{
#warning connect to configuration file
	retryTimeoutMS=100;
}

void SCBController::OnBusAcquired(GDBusConnection *pConnection,
		const gchar *sName, gpointer pUserData)
{
	Logger::LogDebug("SCBController::OnBusAcquired - Connected to dbus.");
}

void SCBController::OnNameAcquired(GDBusConnection *pConnection,
		const gchar *sName, gpointer pUserData)
{
	GError *err=NULL;
	SCBController *controller=(SCBController *)pUserData;
	controller->dbusConnection = pConnection;

	Logger::LogDebug("SCBController::OnNameAcquired - Name aquired on dbus: %s",sName);

	g_dbus_connection_set_exit_on_close(controller->dbusConnection, FALSE);

	g_signal_connect(controller->dbusInterface, "handle-request-play", G_CALLBACK(OnDBusPlayRequested), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-pause-resume", G_CALLBACK(OnDBusPauseResumeRequested), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-stop", G_CALLBACK(OnDBusStopRequested), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-next", G_CALLBACK(OnDBusNextRequested), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-prev", G_CALLBACK(OnDBusPreviousRequested), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-power-state-change", G_CALLBACK(OnDBusPowerStateChange), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-vol-up", G_CALLBACK(OnDBusVolUp), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-vol-down", G_CALLBACK(OnDBusVolDown), controller);
	g_signal_connect(controller->dbusInterface, "handle-request-mute", G_CALLBACK(OnDBusMute), controller);

	/* Export the interfaces */
	if(g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(controller->dbusInterface),
			controller->dbusConnection,SCB_OBJECT_PATH,&err) != TRUE)
	{
		Logger::LogError("Unable to export dbus interface: %s", err->message);
		g_object_unref(err);
	}

	// set initial values of properties
	squeeze_client_control_set_volume_l (controller->dbusInterface, 0);
	squeeze_client_control_set_volume_r (controller->dbusInterface, 0);
	squeeze_client_control_set_squeeze_client_state (controller->dbusInterface, SqueezeClient::SqueezeClientStateT::POWERED_OFF);
}

void squeezeclient::SCBController::OnNameLost(GDBusConnection *pConnection,
		const gchar *sName, gpointer pUserData)
{
	if (pConnection==NULL)
		Logger::LogError("Unable to connect to dbus.");
	else
		Logger::LogDebug("SCBController::OnNameLost - Connection to dbus lost.");
}
