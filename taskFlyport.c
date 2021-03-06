#include "taskFlyport.h"
#include "enocean.h"
#include "console.h"	//for debug command
#include "tools.h"
#include "mem_mod.h"

#include "ute_mod.h"
#include "enocean_esp3.h"

#define UART_DEBUG

extern ID_TABLE_TYPE IDTable;


SM_STATE_NR_TYPE u8mode = SM_INIT;
	
//UINT8 myID[4] ={0x01, 0x84, 0x2E, 0x38};

#ifdef UART_DEBUG
extern BOOL cmdNew;	//console.c
#endif
		
//appelee dans sm_init()
void DEBUG()
{
	// TEL_RADIO_TYPE telegram;
	// int i = 0;
	
	ConsoleWrite("#### DEBUG\r\n");
	
	// telegram.raw.u8Length = RADIO_BUFF_LENGTH;
	// for(i=0;i<RADIO_BUFF_LENGTH;i++)
	// {
		// telegram.raw.bytes[i]=i;
	// }
	ConsoleWrite("sizeof(int)=");
	uart_debugHexa(sizeof(int));
	ConsoleWrite("\r\n");
	ConsoleWrite("sizeof(CHOICE_TYPE)=");
	uart_debugHexa(sizeof(CHOICE_TYPE));
	ConsoleWrite("\r\n");
	ConsoleWrite("sizeof(UINT8)=");
	uart_debugHexa(sizeof(UINT8));
	ConsoleWrite("\r\n");
	
	// DEBUGDisplayRAW(&telegram);
	// DEBUGDisplayTRPS(&telegram);
	// DEBUGDisplayT1BS(&telegram);
	// DEBUGDisplayT4BS(&telegram);
	
	 
	ConsoleWrite("#### DEBUG END\r\n");
	ConsoleWrite("\r\n");
}


void FlyportTask()
{
	
	vTaskDelay(50);
	
	//	Flyport waiting for the cable connection
	//while (!MACLinked);
	//vTaskDelay(100);
	//UARTWrite(1,"Flyport ethernet connected to the cable... hello world!\r\n");
	
	// u8mode = SM_OPERATION;
	// u8mode = SM_LEARN;
	
	while(1)
	{
		switch(u8mode)
		{
			case SM_INIT:
				sm_init();
				break;
			case SM_OPERATION:	//operation state. See #sm_operation()
				//if new telegram received 
				//	handle the new telegram
				//if LEARN pressed
				//	go to learn mode
				sm_operation();
			break;
			case  SM_LEARN:	//! Learn  state code. See #sm_learn()
				sm_learn();
			break;
			// case SM_REMOTE_LEARN:	//! Remote learn state state code. See #sm_learn
			// break;
			// case SM_CLEAR:	//! Clear  state code. See #sm_clear
			// break;
			default:
				ConsoleWrite("STATE error\r");
				u8mode = SM_OPERATION;
			break;
		}
		
		//	Main user's firmware loop
		vTaskDelay(5);
		
		/*********************
		// DEBUG UART1 command
		*********************/
		#ifdef UART_DEBUG
		CheckUART1();
		if(cmdNew == TRUE)
		{
			CheckCmds();
			cmdNew = FALSE;
		}
		#endif

	}
}



/***************************************************
*		INIT MODE
***************************************************/
RETURN_TYPE sm_init()
{
	
	
	ConsoleWrite("ENTER SM_INIT()\r\n");
	
	//*******************************************
	//	INIT UART FOR ENOCEAN COMMUNICATION
	enocean_init();
	
	
	
	// DEBUG();
		
	//*******************************************
	// READ FLASH MEMORY 
	// mem_readFlash(MEMORY_ADDRESS_IDTABLE, resultBuff);
	// memcpy(&(IDTable),resultBuff,sizeof(IDTable));
	// mem_readIDTableFlash(&IDTable);
	
	//*******************************************
	// GET BASEID
	// enocean_getBaseId();
	
	//init complet, go to operation mode
	u8mode = SM_OPERATION;
	
	ConsoleWrite("EXIT SM_INIT()\r\n");
	return OK;
}

/***************************************************
*		OPERATION MODE
***************************************************/
RETURN_TYPE sm_operation()
{
	TEL_RADIO_TYPE telegram;
	TEL_PARAM_TYPE telParam;
	ID_SEARCH_OUT_TYPE idSearchOut;
	
	ConsoleWrite("ENTER SM_OPERATION()\r\n");

	while(1)
	{
		if(enocean_checkCmd(&telegram, &telParam) == NEW_RX_TEL)
		{
			ConsoleWrite("RX Tel\r\n");
			// if(enocean_idSearch(&IDTable, &telegram, &idSearchOut) == ID_SUCCESS)
			// {
				// // ConsoleWrite("ID registered ");
				// uart_debugU32ID(IDTable.entry[idSearchOut.u8index].u32Id);
				// ConsoleWrite("\r\n");
				
				
			// }else
			// {
				// // ConsoleWrite("ID not registered\r\n");
			// }
			
		}
	
	
		#ifdef UART_DEBUG
		CheckUART1();
		if(cmdNew == TRUE)
		{
			CheckCmds();
			cmdNew = FALSE;
		}
		#endif
		
		if(u8mode != SM_OPERATION)
		{
			break;
		}
	}
	
	ConsoleWrite("EXIT SM_OPERATION()\r\n");
	
	return OK;
}

/***************************************************
*		LEARN MODE
***************************************************/
RETURN_TYPE sm_learn()
{
	TEL_RADIO_TYPE telegram;
	TEL_PARAM_TYPE telParam;
	ID_SEARCH_OUT_TYPE idSearchOut;
	
	
	ConsoleWrite("ENTER SM_LEARN()\r\n");
	enocean_clearRX();
	
	while(1)
	{
		if(enocean_checkCmd(&telegram, &telParam) == NEW_RX_TEL)
		{
			enocean_enableRadioRX(FALSE);
			
			if(enocean_validToLearn(&telegram, &telParam) == ID_SUCCESS)
			{
				if(enocean_idSearch(&IDTable, &telegram, &idSearchOut) == ID_NO_SUCCESS)
				{
					//special treatment for UTE which requires and answer
					if(telegram.raw.bytes[0] == RADIO_CHOICE_UTE)
					{
						vTaskDelay(10);
						ute_sendTeachInResponse(&telegram);
					}
					
					enocean_idAdd(&IDTable, &telegram);
				}else
				{
					ConsoleWrite("ID already existing\r\n");
				}
			}else
				ConsoleWrite("Not valid to learn\r\n");
			
			enocean_enableRadioRX(TRUE);
		}
		
		
		#ifdef UART_DEBUG
		CheckUART1();
		if(cmdNew == TRUE)
		{
			CheckCmds();
			cmdNew = FALSE;
		}
		#endif
		
		if(u8mode != SM_LEARN)
		{
			//before leaving learn mode, save the IDTable in external flash 		- write only the necessary bytes, this will reset to FF the unused bytes in the memory block (4096 bytes)
			mem_writeFlash(MEMORY_ADDRESS_IDTABLE,(BYTE*)&IDTable, (sizeof(IDTable) - (30*sizeof(ID_ENTRY_TYPE)-(sizeof(ID_ENTRY_TYPE)*IDTable.u8nrIDs))));
			break;
		}
	}
	
	ConsoleWrite("EXIT SM_LEARN()\r\n");
	return OK;
}
