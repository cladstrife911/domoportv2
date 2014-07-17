#ifndef __ENOCEAN_H_
#define __ENOCEAN_H_

#include "enocean_esp3.h"
#include "enocean_device.h"
#include "tools.h"
#include "console.h"
	

/******** function prototypes ******/

void enocean_enableRadioRX(BOOL enable);
RETURN_TYPE enocean_checkCmd(TEL_RADIO_TYPE *pu8RxRadioTelegram, TEL_PARAM_TYPE *pu8TelParam);
void enocean_init();
void enocean_clearRX();


//COMMON COMMAND ESP3
RETURN_TYPE enocean_getBaseId();
RETURN_TYPE enocean_setRepeater(uint8 repLevel);
	
#endif
