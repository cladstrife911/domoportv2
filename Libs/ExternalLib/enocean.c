#include "enocean.h"

// #define UART_DEBUG
#undef UART_DEBUG



static BYTE smInternal = 0; // State machine for internal use of callback functions
static char ENOCEANBuffer[ENOCEAN_BUFFER_SIZE];
static char ENOCEANbuffover;
static int bufind_w;
static int bufind_r;
static int last_op;

/****************
extern variables defined Regs.c
define the registers of the PIC24 for UART
****************/
extern int *UMODEs[];
extern int *USTAs[];
extern int *UBRGs[];
extern int *UIFSs[];
extern int *UIECs[];
extern int *UTXREGs[];
extern int *URXREGs[];
extern int UTXIPos[];
extern int URXIPos[];


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
	// UARTInit(2,57600);
	// UARTOn(2);
	
	ENOCEANUARTInit(ENOCEAN_BAUD_RATE);
}


/********************************************
Get Enocean buffer size fill in by interrupt
********************************************/
int EnoceanBufferSize()
{
	BYTE loc_last_op = last_op;
	int conf_buff;
	int bsize=0;

	conf_buff = bufind_r - bufind_w;
	if (conf_buff > 0)
		bsize = ENOCEAN_BUFFER_SIZE - bufind_r + bufind_w;
	else if (conf_buff < 0)
		bsize = bufind_w - bufind_r;
	else if (conf_buff == 0)
		if (loc_last_op == 1)
			bsize = ENOCEAN_BUFFER_SIZE;

	return bsize;
}

/******************************************
clear Enocean RX buffer
******************************************/
void ENOCEANFlush()
{
	bufind_w = 0;
	bufind_r = 0;
	last_op = 0;
	ENOCEANBuffer[0] = '\0';
}



// Initializes Flyport UART4 to be used with Hilo Modem with sperified "long int baud" baudrate. It enables also HW flow signals CTS/RTS

void ENOCEANUARTInit(long int baud)
{
    // Initialize HILO UART...
    int port = ENOCEAN_UART-1;
	long int brg , baudcalc , clk , err;
	clk = GetInstructionClock();
	brg = (clk/(baud*16ul))-1;
	baudcalc = (clk/16ul/(brg+1));
	err = (abs(baudcalc-baud)*100ul)/baud;

	int UMODEval = 0;
	UMODEval = (*UMODEs[port] & 0x3CFF);
	
	if (err<2)
	{
		*UMODEs[port] = (0xFFF7 & UMODEval);
		*UBRGs[port] = brg;
	}
	else
	{
		brg = (clk/(baud*4ul))-1;
		*UMODEs[port] = (0x8 | UMODEval);
		*UBRGs[port] = brg;
	}
	
	// UART ON:
	*UMODEs[port] = *UMODEs[port] | 0x8000;
	*USTAs[port] = *USTAs[port] | 0x400;

	*UIFSs[port] = *UIFSs[port] & (~URXIPos[port]);
	*UIFSs[port] = *UIFSs[port] & (~UTXIPos[port]);
	*UIECs[port] = *UIECs[port] | URXIPos[port];
}

// UART2 Rx Interrupt to store received chars from modem
void ENOCEANRxInt()
{
	int port = ENOCEAN_UART - 1;
	
	while ((*USTAs[port] & 1)!=0)
	{
		if (bufind_w == bufind_r)
		{
			if (last_op == 1)
			{
				ENOCEANbuffover = 1;
				bufind_w = 0;
				bufind_r = 0;
				last_op = 0;
			}
		}

		ENOCEANBuffer[bufind_w] = *URXREGs[port];
		
        #if defined UART_DEBUG
		// if(ENOCEANBuffer[bufind_w] == 0x55)
		// {
			// ConsoleWrite("\r\n[RX] ");
		// }
        // uart_debugHexa(ENOCEANBuffer[bufind_w]);
        // ConsoleWrite(" ");
        #endif
		
		
		if (bufind_w == ENOCEAN_BUFFER_SIZE - 1)
		{
			bufind_w = 0;
		}
		else
			bufind_w++;
	}
	last_op = 1;
	*UIFSs[port] = *UIFSs[port] & (~URXIPos[port]);
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
		UARTFlush(ENOCEAN_UART);
		UARTOn(ENOCEAN_UART);
	}else
	{
		UARTOff(ENOCEAN_UART);
		UARTFlush(ENOCEAN_UART);
	}
}


/**********************************************
read UART RX buffer to check if there is a new telegram arrived
new verion based on interrupt
**********************************************/
RETURN_TYPE enocean_checkCmd(TEL_RADIO_TYPE *pu8RxRadioTelegram, TEL_PARAM_TYPE *pu8TelParam)
{	
	int i;
	
	enocean_newRXTel = NO_RX_TEL;
	
	switch(smInternal)
	{
		case 0:
			if(ENOCEANBuffer[0] == 0x55)
			{
				// ConsoleWrite("\r\n[RX] ");
				smInternal++;
			}
			else
				ENOCEANFlush();
			break;
		case 1:
			if(EnoceanBufferSize() >= 7)
			{
				// ConsoleWrite("$");
				if(EnoceanBufferSize() >= (ENOCEANBuffer[2]+ENOCEANBuffer[3]+7))
					smInternal++;
			}
			break;
		case 2:
			// ConsoleWrite("#");
			enocean_radioRXtoRead = (ENOCEANBuffer[2]+ENOCEANBuffer[3]+7);
			memcpy(&enocean_radioRXBuffer[0],&(ENOCEANBuffer[0]),  enocean_radioRXtoRead);
			ENOCEANFlush();
			
			// ConsoleWrite("%");
			// uart_debugHexa(enocean_radioRXtoRead);
			
			if(enocean_radioRXtoRead != 0)
			{		
				#ifdef UART_DEBUG
				for(i=0;i<enocean_radioRXtoRead;i++)
				{
					uart_debugHexa(enocean_radioRXBuffer[i]);
					ConsoleWrite(" ");
				}
				#endif
				
				enocean_newRXTel = radio_getTelegram(pu8RxRadioTelegram,pu8TelParam);
					
			}
			smInternal = 0;
			break;
		default:
			smInternal = 0;
		break;
	}
	
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
	
	memcpy(&IDTable.u32BaseID, &(telegram.raw.bytes[1]),4);
	
	ConsoleWrite("\r\n BaseId: ");
	uart_debugU32ID(IDTable.u32BaseID);
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
