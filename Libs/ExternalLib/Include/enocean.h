#ifndef __ENOCEAN_H_
#define __ENOCEAN_H_

#include "enocean_esp3.h"
#include "enocean_device.h"
#include "tools.h"
#include "console.h"
	
	

#define ENOCEAN_UART 2
#define ENOCEAN_BUFFER_SIZE 256
#define ENOCEAN_BAUD_RATE 57600	

/******** function prototypes ******/

void enocean_enableRadioRX(BOOL enable);
RETURN_TYPE enocean_checkCmd(TEL_RADIO_TYPE *pu8RxRadioTelegram, TEL_PARAM_TYPE *pu8TelParam);
void enocean_init();
void enocean_clearRX();


void ENOCEANRxInt();
int EnoceanBufferSize();
void ENOCEANUARTInit(long int baud);
void ENOCEANFlush();

//COMMON COMMAND ESP3
RETURN_TYPE enocean_getBaseId();
RETURN_TYPE enocean_setRepeater(uint8 repLevel);
	
#endif
