/*
* Do not forget to run the Wizzard in the IDE to enable the 2nd UART
*	-> UART Settings -> Ports = 2
*/

/* **************************************************************************																					
 *
 *$Date&
 *$Author&
 *$Rev$
 *
 **************************************************************************/
 
#ifndef __EO_ESP3_C
#define __EO_ESP3_C

#include "enocean_esp3.h"
#include "console.h"

#define UART_DEBUG

#define UART_PORT 2

//#define UART_DEBUG

//ID de l'interrupteur Enocean
UINT8 ID_PTM[4]={0x00, 0x8B, 0x75, 0x3D};

//table for CRC8 computing
const UINT8 u8CRC8Table[256] = {  
  0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,  
  0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,  
  0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,  0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,  
  0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,  
  0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,  
  0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,  
  0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,  
  0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,  
  0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6A, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,  
  0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8D, 0x84, 0x83,  
  0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3  
  }; 
 
  


/**
* ESP3 packet structure through the serial port. 
* Protocol bytes are generated and sent by the application 
* 
* Sync = 0x55  
* CRC8H 
* CRC8D 
* 
*    1              2                1           1           1      u16DataLen + u8OptionLen       1 
* +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+ 
* | 0x55 |     u16DataLen   |   u8OptionLen |  u8Type   |    CRC8H  |            DATAS         |   CRC8D   | 
* +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+ 
*  
* DATAS structure: 
*                   u16DataLen                    u8OptionLen 
* +--------------------------------------------+----------------------+ 
* |                   Data                     |     Optional         |    
* +--------------------------------------------+----------------------+ 
*
* \param port - number of the COM port to send data
* \param pBuffer - see table bellow. The CRC8H & CRC8D are computed in the fonction
*	|	1			|		1		| 		1	  |	 1	   | u16DataLen + u8OptionLen	|
*	+---------------+---------------+-------------+--------+----------------------------+
*	|u16DataLenHigh | u16DataLenLow | u8OptionLen | u8Type | 			*DATAS			|
*	+---------------+---------------+-------------+--------+----------------------------+
*/ 
RETURN_TYPE UARTSendPacket(UINT8 *pBuffer) 
{ 
	int port = UART_PORT;
	
	UINT32 i=0; 
	UINT32 dataSize =0;
	UINT8  u8CRC=0; 
		
	#ifdef UART_DEBUG
	vTaskDelay(10);
	UARTWrite(1,"\r\n#####UARTSendPacket#####");
	UARTWrite(1,"\r\n");
	#endif
	
	// When both length fields are 0, then this telegram is not allowed.  
	if(pBuffer[0] ==0 && pBuffer[1] == 0 && pBuffer[2]==0) 
	{ 
		return OUT_OF_RANGE;  
	} 
	
	dataSize = (((UINT32)pBuffer[0])<<8) + pBuffer[1] + pBuffer[2];
	
	#ifdef UART_DEBUG
	vTaskDelay(10);
	UARTDebug32("- dataSize = ", dataSize);
	UARTWrite(1,"\r\n");
	#endif
		
		
	// Sync 
	UARTWriteCh(port, ESP3_SYNC_BYTE); 
	#ifdef UART_DEBUG
	UARTDebugHexa(ESP3_SYNC_BYTE);
	#endif

	
	#ifdef UART_DEBUG
	UARTWrite(1,"\r\nHEADER:");
	#endif
	// Header 
	while(UARTWriteBuffer((UINT8*)pBuffer, ESP3_HEADER_SIZE) != OK); 

	// Header CRC 
	u8CRC = 0; 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)pBuffer)[0]); 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)pBuffer)[1]); 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)pBuffer)[2]); 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)pBuffer)[3]); 
	UARTWriteCh(port, u8CRC); 
	#ifdef UART_DEBUG
	UARTWrite(1,"\r\nu8CRCH:");
	UARTDebugHexa(u8CRC);
	#endif
	
	// Data & Optional Data
	#ifdef UART_DEBUG
	UARTWrite(1,"\r\nDATA:");
	#endif
	u8CRC = 0; 
	for (i = 0 ; i < dataSize ; i++) 
	{ 
		u8CRC = proc_crc8(u8CRC, pBuffer[i+4]); 
		UARTWriteCh(port, pBuffer[i+4]);
		#ifdef UART_DEBUG
		UARTDebugHexa(pBuffer[i+4]);
		#endif
	} 

	// Data CRC 
	UARTWriteCh(port, u8CRC); 
	#ifdef UART_DEBUG
	UARTWrite(1,"\r\nu8CRCD:");
	UARTDebugHexa(u8CRC);
	#endif
		
		
	#ifdef UART_DEBUG
	UARTWrite(1,"\r\n");
	vTaskDelay(10);
	#endif
	return OK; 
} //UARTSendPacket



 
/**********************************
Function used to read telegram comming from the enocean module
@param pu8RxRadioTelegram struct that will contain the telegram data
@param pu8TelParam struct that will contain optional data 
***********************************/
RETURN_TYPE radio_getTelegram(TEL_RADIO_TYPE *pu8RxRadioTelegram, TEL_PARAM_TYPE *pu8TelParam)
{ 
	UINT32 i=0; 
	UINT8  u8CRC=0; 
	UINT32 dataSize =0;
	UINT32 packetSize=0;
	
	// ConsoleWrite("ENTER radio_getTelegram()\r\n");
	
	//First we have to check if we received more than one ESP3 packet
	dataSize = (((UINT32)enocean_radioRXBuffer[1])<<8) + enocean_radioRXBuffer[2] + enocean_radioRXBuffer[3];
	
	packetSize = dataSize + ESP3_HEADER_SIZE + 3; // SYNC+CRC8H+CRC8D = 3
	if(packetSize < enocean_radioRXtoRead) 
	{
		#ifdef UART_DEBUG
			UARTWrite(1,"\r\n DOUBLE PACKET");
			UARTDebug32("\r\n length= ", enocean_radioRXtoRead);
			UARTDebug32("- dataSize = ", packetSize);
		#endif
		//recursivity to handle the 2nd packet
		//TODO A MODIFIER
		// getPacket(&enocean_radioRXBuffer[packetSize],enocean_radioRXtoRead-packetSize);
	}
		
	//Sync
	if(enocean_radioRXBuffer[0]!=ESP3_SYNC_BYTE)
		return NOT_VALID_TEL;
		
	// When both length fields are 0, then this telegram is not allowed.  
	if((enocean_radioRXBuffer[1] == 0) && (enocean_radioRXBuffer[2] ==0) && (enocean_radioRXBuffer[3] == 0)) 
		return NOT_VALID_TEL;  

	// Header CRC 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)enocean_radioRXBuffer)[1]); 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)enocean_radioRXBuffer)[2]); 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)enocean_radioRXBuffer)[3]); 
	u8CRC = proc_crc8(u8CRC, ((UINT8*)enocean_radioRXBuffer)[4]); 
	
	if(u8CRC!= ((UINT8*)enocean_radioRXBuffer)[5])
		return NOT_VALID_CHKSUM; //bad CRC

	// Data & Optional Data
	u8CRC = 0; 
	
	for (i = 0 ; i < dataSize ;i++) 
	{ 
		u8CRC = proc_crc8(u8CRC, enocean_radioRXBuffer[i+6]);
	} 
	if(u8CRC != ((UINT8*)enocean_radioRXBuffer)[packetSize-1])
		return NOT_VALID_CHKSUM;


	switch(((UINT8*)enocean_radioRXBuffer)[4])
	{		
		case RADIO:
			//! number of subtelegrams received	(= number of originals + number of repeated)
			pu8TelParam->p_rx.u8SubTelNum = enocean_radioRXBuffer[packetSize-8];
			//! received Destination ID
			memcpy(&(pu8TelParam->p_rx.u32DestinationId), &enocean_radioRXBuffer[packetSize-7], 4);
			//! u8Dbm of the last subtelegram calculated from RSSI. Note this value is an unsigned value. The real dBm signal is a negative value.
			pu8TelParam->p_rx.u8Dbm = enocean_radioRXBuffer[packetSize-3];
			
			// DEBUGDisplayTELPARAM(pu8TelParam);
			
			//switch RORG
			switch( ((UINT8*)enocean_radioRXBuffer)[6])
			{
				case RADIO_CHOICE_RPS:
					pu8RxRadioTelegram->trps.u8Choice	= RADIO_CHOICE_RPS;
					pu8RxRadioTelegram->trps.u8Data 	= enocean_radioRXBuffer[7];
					memcpy(&(pu8RxRadioTelegram->trps.u32Id), &enocean_radioRXBuffer[8], 4);
					pu8RxRadioTelegram->trps.u8Status 	= enocean_radioRXBuffer[12];											
					pu8RxRadioTelegram->trps.u8Length 	= RADIO_DEC_LENGTH_RPS;
	
					// DEBUGDisplayTRPS(pu8RxRadioTelegram);
				break;
				
				case RADIO_CHOICE_1BS:
					pu8RxRadioTelegram->t1bs.u8Choice	= RADIO_CHOICE_1BS;
					pu8RxRadioTelegram->t1bs.u8Data 	= enocean_radioRXBuffer[7];
					memcpy(&(pu8RxRadioTelegram->t1bs.u32Id), &enocean_radioRXBuffer[8], 4);
					pu8RxRadioTelegram->t1bs.u8Status 	= enocean_radioRXBuffer[12];											
					pu8RxRadioTelegram->t1bs.u8Length 	= RADIO_DEC_LENGTH_1BS;
					
					// DEBUGDisplayT1BS(pu8RxRadioTelegram);
				break;
				
				case RADIO_CHOICE_4BS:
					pu8RxRadioTelegram->t4bs.u8Choice	= RADIO_CHOICE_4BS;
					pu8RxRadioTelegram->t4bs.u8Data3 	= enocean_radioRXBuffer[7];
					pu8RxRadioTelegram->t4bs.u8Data2 	= enocean_radioRXBuffer[8];
					pu8RxRadioTelegram->t4bs.u8Data1 	= enocean_radioRXBuffer[9];
					pu8RxRadioTelegram->t4bs.u8Data0 	= enocean_radioRXBuffer[10];
					memcpy(&(pu8RxRadioTelegram->t4bs.u32Id), &enocean_radioRXBuffer[11], 4);
					pu8RxRadioTelegram->t4bs.u8Status 	= enocean_radioRXBuffer[15];											
					pu8RxRadioTelegram->t4bs.u8Length 	= RADIO_DEC_LENGTH_4BS;
					
					// DEBUGDisplayT4BS(pu8RxRadioTelegram);
				break;
					
				default:
					ConsoleWrite("EEP not supported\r\n");
				break;
			}
		break;
		
		case RESPONSE:
			switch(enocean_radioRXBuffer[6])
			{
				case RET_OK:
					//TODO !!!this will only work for CO_RD_IDBASE !!!
					//il faut passer un paramètre supllémentaire à la fonction pour qu'elle sache quelle COMMON COMMAND a été envoyé
					//la longueur de la réponse et différente en fonction de la commande (CO_WR_REPEATER réponse longueur 1)
					memcpy(&(pu8RxRadioTelegram->raw.bytes), &(enocean_radioRXBuffer[6]), 6); //Ret code + BaseId + remaining wr cycle = 6		
					ConsoleWrite("\r\nRAW: ");
					for(i=0;i<packetSize;i++)
						uart_debugHexa(enocean_radioRXBuffer[i]);
					break;
					
				default:
					return NOT_VALID_TEL;
				break;
			}
			
		break;
		
		default:
			ConsoleWrite("\r\n unknown TYPE");
			break;
	}//switch telegram type

	
	// DEBUGDisplayRAW(pu8RxRadioTelegram);
	
	// ConsoleWrite("EXIT radio_getTelegram()\r\n");
	return NEW_RX_TEL; 
} //radio_getTelegram


 
 
 /* exemple de télegramme enocean
 55 00 07 07 01 7A 		F6 30 00 1F 5B 1C 30	 	01 FF FF FF FF 31 00 BF 
 */
 
 /**********************************************************
 *		DEBUG FUNCTIONS
 **********************************************************/
 void DEBUGDisplayRAW(TEL_RADIO_TYPE* const telegram)
{
	#ifdef UART_DEBUG
	int i=0;
	ConsoleWrite("RAW: ");
	for(i=0;i<RADIO_DEC_LENGTH_RPS;i++)
	{
		uart_debugHexa(telegram->raw.bytes[i]);
		ConsoleWrite(" ");
	}
	ConsoleWrite("\r\n");
	#endif
}
void DEBUGDisplayTRPS(TEL_RADIO_TYPE* const telegram)
{
	#ifdef UART_DEBUG
	ConsoleWrite("\r\n######\r\n");
	ConsoleWrite("\r\nRPS:\r\n-trps.u8Choice:");
	uart_debugHexa(telegram->trps.u8Choice	);
	ConsoleWrite("\r\n-trps.u8Data:");
	uart_debugHexa(telegram->trps.u8Data	);
	ConsoleWrite("\r\n-trps.u32Id:");
	uart_debugUINT32(telegram->trps.u32Id	);
	ConsoleWrite("\r\n-trps.u8Status:");
	uart_debugHexa(telegram->trps.u8Status	);
	ConsoleWrite("\r\n");
	#endif
}
void DEBUGDisplayT1BS(TEL_RADIO_TYPE* const telegram)
{
	#ifdef UART_DEBUG
	ConsoleWrite("\r\n######\r\n");
	ConsoleWrite("\r\nRPS:\r\n-t1bs.u8Choice:");
	uart_debugHexa(telegram->t1bs.u8Choice	);
	ConsoleWrite("\r\n-t1bs.u8Data:");
	uart_debugHexa(telegram->t1bs.u8Data	);
	ConsoleWrite("\r\n-t1bs.u32Id:");
	uart_debugUINT32(telegram->t1bs.u32Id	);
	ConsoleWrite("\r\n-t1bs.u8Status:");
	uart_debugHexa(telegram->t1bs.u8Status	);
	ConsoleWrite("\r\n");
	#endif
}
void DEBUGDisplayT4BS(TEL_RADIO_TYPE* const telegram)
{
	#ifdef UART_DEBUG
	ConsoleWrite("\r\n######\r\n");
	ConsoleWrite("\r\nRPS:\r\n-t4bs.u8Choice:");
	uart_debugHexa(telegram->t4bs.u8Choice	);
	ConsoleWrite("\r\n-t4bs.u8Data3-2-1-0:");
	uart_debugHexa(telegram->t4bs.u8Data3	);
	uart_debugHexa(telegram->t4bs.u8Data2	);
	uart_debugHexa(telegram->t4bs.u8Data1	);
	uart_debugHexa(telegram->t4bs.u8Data0	);
	ConsoleWrite("\r\n-t4bs.u32Id:");
	uart_debugUINT32(telegram->t4bs.u32Id	);
	ConsoleWrite("\r\n-t4bs.u8Status:");
	uart_debugHexa(telegram->t4bs.u8Status	);
	ConsoleWrite("\r\n");
	#endif
}
 
void DEBUGDisplayTELPARAM(TEL_PARAM_TYPE* param)
{
	#ifdef UART_DEBUG
	ConsoleWrite("\r\nTEL_PARAM_TYPE:\r\n");
	ConsoleWrite("-p_rx.u8SubTelNum:");
	uart_debugHexa(param->p_rx.u8SubTelNum);
	ConsoleWrite("\r\n-p_rx.u32DestinationId:");
	uart_debugUINT32(param->p_rx.u32DestinationId	);
	ConsoleWrite("\r\n-p_rx.u8Dbm:");
	uart_debugHexa(param->p_rx.u8Dbm);
	ConsoleWrite("\r\n");
	#endif
}

/*************************************************************************************
*	 TOOLS
*************************************************************************************/

/**
*UARTWriteEOSequence - writes the specified EnOcean Sequence on the UART port.
* \param port - the UART port to write to.
* \param buf - the string to write (a char array).
* \param lentgh - lentgh of the char array.
* \return None
*/
RETURN_TYPE UARTWriteBuffer(UINT8 *buf, UINT32 length)
{
	UINT32 i=0;
	for(i=0; i<=length-1 ;i++) //-1 pas bon
	{
		UARTWriteCh(UART_PORT, buf[i]);
		#ifdef UART_DEBUG
		UARTDebugHexa(buf[i]);
		#endif
	}
		
	return OK;
}

/**
* Compare 2 EnOcean ID
* \param pBuffer - ID received
* \param ID - reference ID
* \return TRUE if both ID are the same, FALSE else
*/
BOOL compareID(UINT8 *pBuffer, UINT8 *ID)
{
	int i=0;
	for(i=0;i<=3;i++)
	{
		if(ID[i] !=  pBuffer[i])
		{
			return FALSE;
			#ifdef UART_DEBUG
				UARTWrite(1,"compareID FALSE\r\n");
			#endif
		}
	}
	#ifdef UART_DEBUG
		UARTWrite(1,"compareID TRUE\r\n");
	#endif
	return TRUE;
}

#endif 
