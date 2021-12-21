/*
 * AlsaMixerControl.cpp
 *
 *  Created on: 08.12.2019
 *      Author: joe
 */

#include "AlsaVolumeControl.h"

#include "cpp-app-utils/Logger.h"
#include <glib-unix.h>

using namespace CppAppUtils;

namespace squeezeclient {

AlsaVolumeControl::AlsaVolumeControl(IAlsaVolumeControlConfig *config) :
		mixerElement(NULL),
		mixerHandle(NULL),
		rangeMin(0),
		rangeMax(100),
		alsaPollEventIds(NULL),
		alsaPollEventIdsCnt(-1),
		config(config)
{
}

AlsaVolumeControl::~AlsaVolumeControl()
{
}

bool AlsaVolumeControl::Init()
{
	const char *cardName=this->config->GetMixerAlsaDeviceName();
	const char *mixerName=this->config->GetMixerAlsaMixerName();
    snd_mixer_selem_id_t *mixerId;

    if (this->mixerHandle!=NULL)
    	this->DeInit();

    if (snd_mixer_open(&this->mixerHandle, 0)!=0)
	{
		Logger::LogError("Unable to get handle for alsa mixer API.");
		return false;
	}

    if (snd_mixer_attach(this->mixerHandle, cardName)!=0)
	{
		Logger::LogError("Unable to attach handle to card %s.", cardName);
		return false;
	}

    if (snd_mixer_selem_register(this->mixerHandle, NULL, NULL)!=0)
	{
		Logger::LogError("Unable register mixer simple element class.");
		return false;
	}

    if (snd_mixer_load(this->mixerHandle)!=0)
	{
		Logger::LogError("Unable load mixer element.");
		return false;
	}

    snd_mixer_selem_id_alloca(&mixerId);

	snd_mixer_selem_id_set_index(mixerId, 0);
    snd_mixer_selem_id_set_name(mixerId, mixerName);
    this->mixerElement=snd_mixer_find_selem(this->mixerHandle, mixerId);

    if (this->mixerElement == NULL)
	{
		Logger::LogError("Unable to find mixer element %s.", mixerName);
		return false;
	}

	snd_mixer_elem_set_callback_private(this->mixerElement, this);
	snd_mixer_elem_set_callback(this->mixerElement, OnAlsaMixerEvent);

    if (snd_mixer_selem_get_playback_volume_range(this->mixerElement, &this->rangeMin, &this->rangeMax)!=0)
    {
		Logger::LogInfo("Unable to determine range of mixer. Taking 0 and 100 as range.");
		this->rangeMin=0;
		this->rangeMin=100;
    }

    if (!this->SetupAlsaPollFDs())
    {
		Logger::LogError("Unable to register alsa event file descriptors for mixer.");
		return false;
    }

	Logger::LogDebug("VolumeControl::Init() - Initialized mixer %s of card %s. Volume range: %ld-%ld",
			mixerName, cardName, this->rangeMin, this->rangeMax);

	return true;
}

void AlsaVolumeControl::DeInit()
{
	this->DestroyEventFDs();

	if (this->mixerElement!=NULL)
		snd_mixer_elem_set_callback(this->mixerElement, NULL);

	if (this->mixerHandle!=NULL)
	{
	    snd_mixer_close(this->mixerHandle);
	    this->mixerHandle=NULL;
	    this->mixerElement=NULL;
	    this->rangeMin=0;
	    this->rangeMax=100;
	}
}

gboolean AlsaVolumeControl::OnAlsaFDEvent(gint fd, GIOCondition condition,
		gpointer user_data)
{
	AlsaVolumeControl *instance=(AlsaVolumeControl *)user_data;
	instance->ProcessAlsaEvent(condition);
	return TRUE;
}

void AlsaVolumeControl::ProcessAlsaEvent(GIOCondition condition)
{
	if ((condition & G_IO_ERR)!=0)
	{
		Logger::LogError("Poll FD of mixer released with condition==G_IO_ERR. Sound card removed. Deinitializing mixer.");
		this->DeInit();
	}
	else if (this->mixerHandle)
		snd_mixer_handle_events(this->mixerHandle);
}

bool AlsaVolumeControl::SetupAlsaPollFDs()
{
	struct pollfd *fds = NULL;
	int count,rc;
	long mixerVol;

	Logger::LogDebug("VolumeControl::SetupAlsaPollFDs() - Setting up alsa event poll fds in main loop.");

	count = snd_mixer_poll_descriptors_count(this->mixerHandle);
	if (count < 0)
	{
		Logger::LogError("snd_mixer_poll_descriptors_count() failed\n");
		return false;
	}

	fds = (struct pollfd *)alloca(count*sizeof(struct pollfd));
	rc = snd_mixer_poll_descriptors (this->mixerHandle, fds, count);
	if (rc < 0)
	{
		Logger::LogError("snd_mixer_poll_descriptors() failed\n");
		return false;
	}

	this->RegisterEventFDs(fds, count);

	return true;
}

void AlsaVolumeControl::RegisterEventFDs(struct pollfd *fds, int count)
{
	this->DestroyEventFDs();

	this->alsaPollEventIdsCnt=count;
	this->alsaPollEventIds=new guint[count];

	for (int a=0;a<count;a++)
	{
	    GIOCondition con=(GIOCondition)fds[a].events; //take care: assumes that glib2.0 uses same bitmask as poll which is currently actually the case
		Logger::LogDebug("VolumeControl::RegisterEventFDs() - Adding poll fd with event mask: 0x%X.", con);
	    this->alsaPollEventIds[a]=g_unix_fd_add(fds[a].fd,con,AlsaVolumeControl::OnAlsaFDEvent,this);
	}
}

void AlsaVolumeControl::DestroyEventFDs()
{
	if (this->alsaPollEventIdsCnt!=-1)
	{
		Logger::LogDebug("VolumeControl::DestroyEventFDs() - Removing alsa event fds from main loop.");
		for (int a=0;a<alsaPollEventIdsCnt;a++)
			g_source_remove(this->alsaPollEventIds[a]);

		delete this->alsaPollEventIds;
		this->alsaPollEventIdsCnt=-1;
	}
}

int AlsaVolumeControl::OnAlsaMixerEvent(snd_mixer_elem_t *elem,
		unsigned int mask)
{
	AlsaVolumeControl *instance=(AlsaVolumeControl *)snd_mixer_elem_get_callback_private(elem);
	instance->OnMixerEvent(mask);
	return 0;
}

void AlsaVolumeControl::OnMixerEvent(unsigned int mask)
{

}

void AlsaVolumeControl::SetVolumeReal(long volRealL, long volRealR)
{
	if (this->mixerElement==NULL) return;

	if (volRealL<this->rangeMin) volRealL=rangeMin;
	if (volRealL>this->rangeMax) volRealL=rangeMax;

	if (volRealR<this->rangeMin) volRealR=rangeMin;
	if (volRealR>this->rangeMax) volRealR=rangeMax;

	Logger::LogDebug("VolumeControl::SetVolumeReal - now setting volume to L:%ld, R:%ld", volRealL,volRealR);

    if (snd_mixer_selem_set_playback_volume(this->mixerElement,SND_MIXER_SCHN_FRONT_LEFT, volRealL)!=0)
		Logger::LogError("Unable to set volume of mixer (LeftFront) to mixerVolume %ld",volRealL);
    if (snd_mixer_selem_set_playback_volume(this->mixerElement,SND_MIXER_SCHN_FRONT_RIGHT, volRealR)!=0)
		Logger::LogError("Unable to set volume of mixer (RightFront) to mixerVolume %ld",volRealR);
}

void AlsaVolumeControl::SetVolume(unsigned int volL, unsigned int volR)
{
	this->SetVolumeReal(SqueezeClientVolToMixerVol(volL),
			SqueezeClientVolToMixerVol(volR));
}

long AlsaVolumeControl::GetVolumeRealL()
{
	long mixerVolReal;

	if (this->mixerElement==NULL) return -1;

	if (snd_mixer_selem_get_playback_volume(this->mixerElement,
			SND_MIXER_SCHN_FRONT_LEFT,&mixerVolReal)!=0)
	{
		Logger::LogError("Unable to read volume from mixer.");
		return this->rangeMin;
	}

	return mixerVolReal;
}

long AlsaVolumeControl::GetVolumeRealR()
{
	long mixerVolReal;

	if (this->mixerElement==NULL) return -1;

	if (snd_mixer_selem_get_playback_volume(this->mixerElement,
			SND_MIXER_SCHN_FRONT_RIGHT,&mixerVolReal)!=0)
	{
		Logger::LogError("Unable to read volume from mixer.");
		return this->rangeMin;
	}

	return mixerVolReal;
}

long AlsaVolumeControl::GetVolumeL()
{
	return this->MixerVolToSqueezeClientVol(this->GetVolumeRealL());
}

long AlsaVolumeControl::GetVolumeR()
{
	return this->MixerVolToSqueezeClientVol(this->GetVolumeRealR());
}

int AlsaVolumeControl::MixerVolToSqueezeClientVol(long mixerVol)
{
	//squeezeclient volume range: 0...0xFFFF
	return ((mixerVol-this->rangeMin)*0xFFFF)/(this->rangeMax-this->rangeMin);
}

long AlsaVolumeControl::SqueezeClientVolToMixerVol(int vol)
{
	//squeezeclient volume range: 0...0xFFFF
	return this->rangeMin+(vol*(this->rangeMax-this->rangeMin)/0xFFFF);
}

} /* namespace retroradio_controller */
