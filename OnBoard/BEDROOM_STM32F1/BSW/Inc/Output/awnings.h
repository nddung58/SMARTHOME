#ifndef INC_AWNINGS_H_
#define INC_AWNINGS_H_
#include "stdint.h"

void awnings_init(void);

void awnings_open(void);

void awnings_close(void);

uint8_t awnings_getstate(void);

#endif // INC_AWNINGS_H_