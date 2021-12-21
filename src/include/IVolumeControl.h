/*
 * IVolumeControl.h
 *
 *  Created on: 20.12.2021
 *      Author: joe
 */

#ifndef SRC_INCLUDE_IVOLUMECONTROL_H_
#define SRC_INCLUDE_IVOLUMECONTROL_H_


namespace squeezeclient {

class IVolumeControl
{
public:
	virtual ~IVolumeControl() {};

	virtual bool Init()=0;

	virtual void DeInit()=0;

	virtual void SetVolume(unsigned int volL, unsigned int volR)=0;
};

} /* namespace squeezeclient */


#endif /* SRC_INCLUDE_IVOLUMECONTROL_H_ */
