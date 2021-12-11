/*
 * IPlayer.h
 *
 *  Created on: 07.02.2021
 *      Author: joe
 */

#ifndef SRC_IPLAYER_H_
#define SRC_IPLAYER_H_

#include <netinet/in.h>


namespace squeezeclient {

class IPlayer
{
public:
	typedef struct StreamingServerInfoT
	{
		in_addr_t ip;
		uint16_t port;
		const char *httpRequest;

	} StreamingServerInfoT;

	typedef enum {FORMAT_PCM='p', FORMAT_MP3='m', FORMAT_FLAC='f', FORMAT_WMA='w',
		FORMAT_OGG='o', FORMAT_ACC='a', FORMAT_ALAC='l'} StreamFormatT;

	typedef enum {SZ8=0, SZ16='1', SZ20='2', SZ32='3', SZ_SELF_DESCR='?'} PCMSampleSizeT;

	typedef enum {SR11KHZ='0', SR22KHZ='1', SR32KHZ='2', SR44_1KHZ='3', SR48KHZ='4', SR8KHZ='5',
		SR12KHZ='6', SR16KHZ='7', SR24KHZ='8', SR96KHZ='9', SR_SELF_DESCR='?'} PCMSampleRateT;

	typedef enum {CH_MONO='0', CH_STEREO='1', CH_SELF_DESCR='?'} PCMChannelCountT;

	typedef enum {EN_BIG='0', EN_LITTLE='1', EN_SELF_DESCR='?'} PCMEndianT;

	typedef struct AudioFormatT
	{
		StreamFormatT streamFormat;

		PCMSampleSizeT pcmSampleSize;

		PCMSampleRateT pcmSampleRate;

		PCMChannelCountT pcmChannelCount;

		PCMEndianT pcmEndian;
	} AudioFormatT;

	typedef struct PlayerStatusT
	{
		uint32_t streamBufferSize;
		uint32_t streamBufferFullness;
		uint64_t bytesReceived;
		uint32_t outputBufferSize;
		uint32_t outputBufferFullness;
		uint32_t elapsedMilliseconds;
	} PlayerStatusT;

	class IPlayerEventListener
	{
	public:
		virtual ~IPlayerEventListener() {};

		/**
		 * Called after LoadStream once the player is connected to the stream server and got a connection response from the server
		 * @params responseStr the string containing the response from the server
		 */
		virtual void OnConnectionResponseReceived(const char *responseStr)=0;

		/**
		 * Called after LoadStream once the player is ready to play. The event needs to be fired only if load stream is called with autostart=false
		 */
		virtual void OnReadyToPlay()=0;

		/**
		 * Called on an ended track.
		 */
		virtual void OnTrackEnded()=0;
	};

protected:
	IPlayerEventListener *playerEventListener;

public:
	IPlayer();

	virtual ~IPlayer();

	virtual void SetPlayerEventListener(IPlayerEventListener *listener);

	virtual bool LoadStream(const StreamingServerInfoT *srvInfo, const AudioFormatT *audioFmt, bool autoStart)=0;

	virtual void UpdatePlayerStatus(PlayerStatusT *status)=0;

	virtual bool Init()=0;

	virtual void DeInit()=0;

	virtual void Stop()=0;

	virtual void Resume()=0;

	virtual void Pause()=0;

	virtual void SkipFrames(uint32_t skippingDuration)=0;

};

} /* namespace squeezeclient */

#endif /* SRC_IPLAYER_H_ */
