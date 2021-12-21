/*
 * AlsaMixerControl.h
 *
 *  Created on: 08.12.2019
 *      Author: joe
 */

#ifndef SRC_SRC_SQUEEZECLIENT_BIN_VOLUMECONTROL_H_
#define SRC_SRC_SQUEEZECLIENT_BIN_VOLUMECONTROL_H_

#include <alsa/asoundlib.h>
#include <glib.h>

#include "IVolumeControl.h"
#include "IAlsaVolumeControlConfig.h"

namespace squeezeclient {

class AlsaVolumeControl : public IVolumeControl {

private:
	IAlsaVolumeControlConfig *config;

	gint alsaPollEventIdsCnt;

	guint *alsaPollEventIds;

	static gboolean OnAlsaFDEvent(gint fd,
            GIOCondition condition, gpointer user_data);

	static int OnAlsaMixerEvent(snd_mixer_elem_t *elem, unsigned int mask);

	void ProcessAlsaEvent(GIOCondition condition);

	bool SetupAlsaPollFDs();

	void RegisterEventFDs(struct pollfd *fds, int count);

	void DestroyEventFDs();

protected:
	long rangeMin;

	long rangeMax;

    snd_mixer_elem_t* mixerElement;

    snd_mixer_t *mixerHandle;

    int MixerVolToSqueezeClientVol(long mixerVol);

    long SqueezeClientVolToMixerVol(int vol);

	virtual void SetVolumeReal(long volRealL, long volRealR);

	virtual long GetVolumeRealL();

	virtual long GetVolumeRealR();

	virtual long GetVolumeL();

	virtual long GetVolumeR();

	virtual void OnMixerEvent(unsigned int mask);

public:
	AlsaVolumeControl(IAlsaVolumeControlConfig *config);

	virtual ~AlsaVolumeControl();

	bool Init();

	void DeInit();

	void SetVolume(unsigned int volL, unsigned int volR);
};

} /* namespace squeezeclient */

#endif /* SRC_SRC_SQUEEZECLIENT_BIN_VOLUMECONTROL_H_ */
