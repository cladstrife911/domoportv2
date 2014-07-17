#include "enocean.h"

#define UART_PORT 2

#define UART_DEBUG

RETURN_TYPE enocean_newRXTel = NO_RX_TEL;
UINT8 	enocean_radioRXBuffer[UART_MAX_LENGTH];
int 	enocean_radioRXtoRead = 0;

extern ID_TABLE_TYPE IDTable;

/**********************************************
clear UART RX buffer and reset new telegram flag
**********************************************/
void enocean_clearRX()
{
	memset(enocean_radioRXBuffer, 0, UART_MAX_LENGTH);
	enocean_radioRXtoRead = 0;
	enocean_newRXTel = NO_RX_TEL;
}

/**********************************************
init UART for communication with enocean module
**********************************************/
void enocean_init()
{	
	//don't forget to activate 2nd UART on the wizzard
	//Init of the UART for EnOcean module when using the flyport Pro dev board (connected on J5)

	IOInit(p5,UART2RX); //ADIO7 of TCM310
	IOInit(p4, UART2TX); //ADIO6 of TCM310
	UARTInit(2,57600);
	UARTOn(2);
}


/**********************************************
disable enocean UART while doing some treatments
used in learn mode
**********************************************/
void enocean_enableRadioRX(BOOL enable)
{
	if(enable)
	{
		enocean_clearRX();
		UARTFlush(UART_PORT);
		UARTOn(UART_PORT);
	}else
	{
		UARTOff(UART_PORT);
		UARTFlush(UART_PORT);
	}
}


/**********************************************
read UART RX buffer to check if there is a new telegram arrived
**********************************************/
RETURN_TYPE enocean_checkCmd(TEL_RADIO_TYPE *pu8RxRadioTelegram, TEL_PARAM_TYPE *pu8TelParam)
{	
	if(UARTBufferSize(UART_PORT)>10)
	{
		vTaskDelay(2);
		enocean_clearRX();
		
		enocean_radioRXtoRead = UARTBufferSize(UART_PORT);
		UARTRead(UART_PORT,(char*)enocean_radioRXBuffer,enocean_radioRXtoRead);
		
		enocean_newRXTel = radio_getTelegram(pu8RxRadioTelegram,pu8TelParam);
		
		#ifdef UART_DEBUG
		ConsoleWrite("result=");
		uart_debugHexa(enocean_newRXTel);
		ConsoleWrite("\r\n");
		#endif
		
		UARTFlush(UART_PORT);
	}else
		return NO_RX_TEL;
	
	return enocean_newRXTel;
}


/**********************************************
Function to get BaseId 
the BaseId is stored in the IDTable.u32BaseID param
the BaseId can be used to send telegram
*********************************************/
RETURN_TYPE enocean_getBaseId()
{
	TEL_RADIO_TYPE telegram;
	
	UINT8 result;
	
	ConsoleWrite("ENTER enocean_getBaseId()\r\n");
						/*|dataLenght| opt | 		 type  		    		         | data |*/
	UINT8 testPacket[5] ={0x00, 0x01, 0x00,(UINT8)((ESP3_PACKET_TYPE)COMMON_COMMAND), CO_RD_IDBASE };	//command 0x08 to read BaseId
	result = UARTSendPacket(testPacket);
	vTaskDelay(10);
	
	enocean_checkCmd(&telegram,NULL);
	
	
	memcpy(&(IDTable.u32BaseID), &(telegram.raw.bytes[1]), 4);
	ConsoleWrite("\r\n BaseId: ");
	uart_debugUINT32(IDTable.u32BaseID);
	ConsoleWrite("\r\n");
		
		

	ConsoleWrite("EXIT enocean_getBaseId()\r\n");

	return OK;
}

/**********************************************
function to change repeater level
@param repLevel:	0	-> rep disable
					1	-> rep lvl 1
					2	-> rep lvl2
					
!!! NON TESTE !!!
***********************************************/
RETURN_TYPE enocean_setRepeater(uint8 repLevel)
{
	TEL_RADIO_TYPE telegram;
	UINT8 result;
	UINT8 param_rep_enable;
	
	if(repLevel == 0)
		param_rep_enable = 0;
	else
		param_rep_enable = 1;
		
	ConsoleWrite("ENTER enocean_setRepeater()\r\n");
						/*|dataLenght| opt | 		 type  		    		         | data |*/
	UINT8 testPacket[7] ={0x00, 0x01, 0x00,(UINT8)((ESP3_PACKET_TYPE)COMMON_COMMAND), CO_WR_REPEATER, param_rep_enable, repLevel };	//command 0x09 to set repeater level
	result = UARTSendPacket(testPacket);
	vTaskDelay(10);
	
	//TODO modifier la fonction radio_getTelegram pour gérer correctement la réponse
	enocean_checkCmd(&telegram,NULL);

	ConsoleWrite("EXIT enocean_setRepeater()\r\n");

	return OK;
}
