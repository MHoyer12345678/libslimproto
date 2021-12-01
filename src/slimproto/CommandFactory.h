/*
 * CommandFactory.h
 *
 *  Created on: 24.01.2021
 *      Author: joe
 */

#ifndef SRC_COMMANDFACTORY_H_
#define SRC_COMMANDFACTORY_H_

#include "stdint.h"

#include "LMSConnection.h"
#include "IPlayer.h"

typedef struct CliCmdBaseT CliCmdBaseT;
typedef struct StrmSrvCmdT StrmSrvCmdT;
typedef struct SetdSrvCmdT SetdSrvCmdT;


namespace slimprotolib
{

class CommandFactory
{
public:
	class IServerCmdListener
	{
	public:
		virtual ~IServerCmdListener() {};

		virtual void OnSrvRequestedLoadStream(IPlayer::StreamingServerInfoT *srvInfo,
				IPlayer::AudioFormatT *audioFMT, bool autostart)=0;

		virtual void OnSrvRequestedPlayerStatus(uint32_t replayGain)=0;

		virtual void OnSrvRequestedPause(uint32_t pauseDuration)=0;

		virtual void OnSrvRequestedStop()=0;

		virtual void OnSrvRequestedUnPause(uint32_t unpauseTime)=0;

		virtual void OnSrvRequestedSkippingFrames(uint32_t skippingDuration)=0;

		virtual void OnSrvRequestedFlush()=0;

		virtual void OnSrvRequestedPlayerName()=0;

		virtual void OnSrvRequestedDisableDACSetting()=0;

		virtual void OnSrvSetNewPlayerName(const char *playerName)=0;

		virtual void OnSrvSetDisableDACSetting(bool value)=0;

		virtual void OnSrvRequestedAudioEnabledChange(bool spdiffEnabled, bool dacEnabled)=0;

		virtual void OnSrvRequestedVolumeChange(unsigned int volL, unsigned int volR, bool adjustLocally)=0;
	};

private:
	LMSConnection *lmsConnection;

	IServerCmdListener *cmdListener;

	void SetCmdBase(CliCmdBaseT *cmdBasePtr, const void *cmd, uint32_t messageSize);

	bool SendMessage(void *data, ssize_t dataSize);

	bool SendStatCmd(const char *event,
			const IPlayer::PlayerStatusT *playerStatus, uint32_t serverTimestamp);

	static bool CheckSizeMin(const char *cmd, uint16_t sizeReceived, size_t sizeExpected);

	static bool CheckSizeExact(const char *cmd, uint16_t sizeReceived, size_t sizeExpected);

	void DoProcessSetdCmd(SetdSrvCmdT *cmd, uint16_t cmdSize);

	//Process Strm commands
	void DoProcessStrmCommand(StrmSrvCmdT *cmd, uint16_t cmdSize);

	void DoProcessStrmSCmd(StrmSrvCmdT *cmd);

public:
	CommandFactory(LMSConnection *connection, IServerCmdListener *cmdListener);

	~CommandFactory();

public:
	bool SendHeloCmd(uint8_t macAdress[6], char uid[16]);

	bool SendPlayerName(const char *playerName);

	bool SendDisableDACSetting(bool value);

	bool SendSTMaCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMcCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMdCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMeCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMfCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMhCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMlCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMnCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMoCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMpCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMrCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMsCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendSTMtCmd(const IPlayer::PlayerStatusT* playerStatus, uint32_t serverTimestamp);

	bool SendSTMuCmd(const IPlayer::PlayerStatusT* playerStatus);

	bool SendByeCmd();

	bool SendConnectResponse(const char *responseStr);

	bool SendIRCmd(uint32_t irCode);

	void DoProcessReceivedCommand(void *data, uint16_t cmdSize);

};

} /* namespace slimprotolib */

#endif /* SRC_COMMANDFACTORY_H_ */
