#ifndef __TASKFLY_H
#define __TASKFLY_H
#include "INTlib.h"

#include "string.h"
#include "FTPlib.h"
#include "UDPlib.h"
#include "NETlib.h"
#include "SMTPlib.h"
#include "RTCClib.h"

#include "enocean_device.h"

#if defined (FLYPORT_ETH) || defined (FLYPORT_G) 
#include "SPIFlash.h"
#endif
#ifndef __MAIN_C
#define WFStatus WFGetStat()
//	RTOS components - Semaphore and queues
extern xQueueHandle xQueue;
extern xSemaphoreHandle xSemFrontEnd;
extern xSemaphoreHandle xSemHW;
extern APP_CONFIG AppConfig;

//	FrontEnd variables
extern BYTE xIPAddress[];
extern WORD xTCPPort;
extern TCP_SOCKET xSocket;
extern int xFrontEndStat;
extern int xErr;
extern BOOL xBool;
extern WORD xWord;
extern char *xChar;
extern BYTE *xByte;
extern BYTE xByte2;
extern BYTE xByte3;
extern NODE_INFO xNode;

extern SMTP_POINTERS SMTPClient;
extern BOOL DHCPAssigned;
#if defined (FLYPORT_ETH)
extern BOOL MACLinked;
#endif	

#endif	// #ifndef __MAIN_C

typedef enum
{
	//! Initialisation state. State executed immediately after reset.
	SM_INIT = 0             ,
	//! Operation  state code. See #sm_operation
	SM_OPERATION            ,
	//! Learn  state code. See #sm_learn
	SM_LEARN                ,
	//! Remote learn state state code. See #sm_learn
	SM_REMOTE_LEARN         ,
	//! Clear  state code. See #sm_clear
	SM_CLEAR            
    
} SM_STATE_NR_TYPE;

RETURN_TYPE sm_init();
RETURN_TYPE sm_learn();
RETURN_TYPE sm_operation();

void FlyportTask();


#endif	// #ifndef __TASKFLY_
