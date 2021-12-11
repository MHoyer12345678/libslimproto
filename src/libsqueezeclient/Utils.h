/*
 * Utils.h
 *
 *  Created on: 25.01.2021
 *      Author: joe
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stdint.h>

namespace squeezeclient {

class Utils {
private:
	Utils();
	~Utils();

public:
	static uint32_t GetTimeMS();
};

} /* namespace slimprotolib */

#endif /* SRC_UTILS_H_ */
