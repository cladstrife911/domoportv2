#ifndef _UTE_MOD_H_INCLUDED
#define _UTE_MOD_H_INCLUDED

#include <string.h>
#include "enocean_esp3.h"
#include "enocean_device.h"



#define UTE_MANUFACTURER_ID 0x3E //Man ID Giga-Concept

#define UTE_EEP_RORG	0xD2
#define UTE_EEP_FUNC	0x01
// #ifdef METERING
	// #define UTE_EEP_TYPE	0x08
// #else
	#define UTE_EEP_TYPE	0x01
// #endif	

#define UTE_TELEGRAM_LENGTH 0x0E

typedef enum
{
	UTE_SUCCESS,
	UTE_NO_SUCCESS,
}UTE_RETURN_TYPE;


/*
This function is called in proc_telValidToLearn
It checks telegram length, RORG, FUNC and TYPE only
*/
UTE_RETURN_TYPE ute_validToLearn(TEL_RADIO_TYPE const *pRadioTel, TEL_PARAM_TYPE const *pRadioParam);

UTE_RETURN_TYPE ute_getEntryType(TEL_RADIO_TYPE const *pRadioTel, ID_ENTRY_TYPE *pSearchOut);

UTE_RETURN_TYPE ute_sendTeachInResponse(TEL_RADIO_TYPE const *pRadioTel);


#endif
