#ifndef __EO_ESP3_H
#define __EO_ESP3_H

#include "tools.h"
#include "HWlib.h"
 
//compute the CRC8
#define proc_crc8(u8CRC, u8Data) (u8CRC8Table[u8CRC ^ u8Data]) 

//Enocean synchronization byte
#define ESP3_SYNC_BYTE 0x55
//size of the packet header defined in the ESP3 protocole
#define ESP3_HEADER_SIZE 4

//! Length of RX/TX radio buffers in bytes - the maximum decoded telegram length 
#define   RADIO_BUFF_LENGTH		21

//! Length of UART fifo buffer. If not using EnOcean Serial Protocol2, you can redefine this in your program in order to save memory
#define UART_MAX_LENGTH			34

//defined in enocean.c
extern UINT8 	enocean_radioRXBuffer[UART_MAX_LENGTH];
extern int 		enocean_radioRXtoRead;

BOOL compareID(UINT8 *pBuffer, UINT8 *ID);


typedef unsigned long int  uint32  ; //!< Unsigned Integer 32 bits

/**
* Packet types as describe in ESP3 protocol (§1.6.2)
*/
typedef enum
{
	RADIO = 0x01,
	RESPONSE = 0x02,
	RADIO_SUB_TEL =0x03,
	EVENT =0x04,
	COMMON_COMMAND = 0x05,
	SMART_ACK_COMMAND = 0x06,
	REMOTE_MAN_COMMAND = 0x07
}ESP3_PACKET_TYPE;

/*
typedef enum
{
	//TODO
}ESP3_DATA_LENGHT;
*/

typedef enum
{
	OPTDATA_RPS = 0x07
}ESP3_OPTDATA_LENGHT;


//Packet Type 2: RESPONSE
//list of return codes
//ESP3 page 15
/*
typedef enum
{
	RET_OK = 0x00,
	RET_ERROR = 0x01, 
	RET_NOT_SUPPORTED = 0x02,
	RET_WRONG_PARAM = 0x03,
	RET_OPERATION_DENIED = 0x04
}ESP3_RESPONSE;
*/

typedef enum
{
	RORG_RPS = 0xF6,
	RORG_1BS = 0xD5,
	RORG_4BS = 0xA5,
	RORG_VLD = 0xD2,
	RORG_MSC = 0xD1,
	RORG_ADT = 0xA6,
	RORG_SM_LRN_REQ = 0xC6,
	RORG_SM_LRN_ANS = 0xC7, 
	RORG_SM_REC = 0xA7,
	RORG_SYS_EX = 0xC5
}ESP3_RORG;


typedef UINT8 uint8  ;
typedef UINT16 uint16;

/*****************FROM EO3000I_API.h*****************/
#define SER_SYNCH_CODE 0x55
#define SER_HEADER_NR_BYTES 0x04

//! Packet structure (ESP3)
typedef struct
{
	uint16	u16DataLength;	         //! Amount of raw data bytes to be received. The most significant byte is sent/received first
	uint8	u8OptionLength;			 //! Amount of optional data bytes to be received
	uint8	u8Type;					 //! Packe type code
	uint8	*u8DataBuffer;			 //! Packe type code

} PACKET_SERIAL_TYPE;

//! Packet type (ESP3)
typedef enum
{
	PACKET_RESERVED 			= 0x00,	//! Reserved
	PACKET_RADIO 				= 0x01,	//! Radio telegram
	PACKET_RESPONSE				= 0x02,	//! Response to any packet
	PACKET_RADIO_SUB_TEL		= 0x03,	//! Radio subtelegram (EnOcean internal function )
	PACKET_EVENT 				= 0x04,	//! Event message
	PACKET_COMMON_COMMAND 		= 0x05,	//! Common command
	PACKET_SMART_ACK_COMMAND	= 0x06,	//! Smart Ack command
	PACKET_REMOTE_MAN_COMMAND	= 0x07,	//! Remote management command
	PACKET_PRODUCTION_COMMAND	= 0x08,	//! Production command
	PACKET_RADIO_MESSAGE		= 0x09	//! Radio message (chained radio telegrams)
} PACKET_TYPE;

//! Response type
typedef enum
{
	RET_OK 					= 0x00, //! OK ... command is understood and triggered
	RET_ERROR 				= 0x01, //! There is an error occured
	RET_NOT_SUPPORTED 		= 0x02, //! The functionality is not supported by that implementation
	RET_WRONG_PARAM 		= 0x03, //! There was a wrong parameter in the command
	RET_OPERATION_DENIED 	= 0x04, //! Example: memory access denied (code-protected)
	RET_USER				= 0x80	//! Return codes greater than 0x80 are used for commands with special return information, not commonly useable.
} RESPONSE_TYPE;

//! Common command enum
typedef enum
{
	CO_WR_SLEEP			= 1,	//! Order to enter in energy saving mode
	CO_WR_RESET			= 2,	//! Order to reset the device
	CO_RD_VERSION		= 3,	//! Read the device (SW) version / (HW) version, chip ID etc.
	CO_RD_SYS_LOG		= 4,	//! Read system log from device databank
	CO_WR_SYS_LOG		= 5,	//! Reset System log from device databank
	CO_WR_BIST			= 6,	//! Perform Flash BIST operation
	CO_WR_IDBASE		= 7,	//! Write ID range base number
	CO_RD_IDBASE		= 8,	//! Read ID range base number
	CO_WR_REPEATER		= 9,	//! Write Repeater Level off,1,2
	CO_RD_REPEATER		= 10,	//! Read Repeater Level off,1,2
	CO_WR_FILTER_ADD	= 11,	//! Add filter to filter list
	CO_WR_FILTER_DEL	= 12,	//! Delete filter from filter list
	CO_WR_FILTER_DEL_ALL= 13,	//! Delete filters
	CO_WR_FILTER_ENABLE	= 14,	//! Enable/Disable supplied filters
	CO_RD_FILTER		= 15,	//! Read supplied filters
	CO_WR_WAIT_MATURITY	= 16,	//! Waiting till end of maturity time before received radio telegrams will transmitted
	CO_WR_SUBTEL		= 17,	//! Enable/Disable transmitting additional subtelegram info
	CO_WR_MEM			= 18,	//! Write x bytes of the Flash, XRAM, RAM0 ….
	CO_RD_MEM			= 19,	//! Read x bytes of the Flash, XRAM, RAM0 ….
	CO_RD_MEM_ADDRESS	= 20,	//! Feedback about the used address and length of the config area and the Smart Ack Table
	CO_RD_SECURITY		= 21,	//! Read security informations (level, keys)
	CO_WR_SECURITY		= 22,	//! Write security informations (level, keys)
} COMMON_COMMAND_TYPE; 

//! Function return codes
typedef enum
{
	//! <b>0</b> - Action performed. No problem detected
	OK=0,							
	//! <b>1</b> - Action couldn't be carried out within a certain time.  
	TIME_OUT,		
	//! <b>2</b> - The write/erase/verify process failed, the flash page seems to be corrupted
	FLASH_HW_ERROR,				
	//! <b>3</b> - A new UART/SPI byte received
	NEW_RX_BYTE,				
	//! <b>4</b> - No new UART/SPI byte received	
	NO_RX_BYTE,					
	//! <b>5</b> - New telegram received
	NEW_RX_TEL,	  
	//! <b>6</b> - No new telegram received
	NO_RX_TEL,	  
	//! <b>7</b> - Checksum not valid
	NOT_VALID_CHKSUM,
	//! <b>8</b> - Telegram not valid  
	NOT_VALID_TEL,
	//! <b>9</b> - Buffer full, no space in Tx or Rx buffer
	BUFF_FULL,
	//! <b>10</b> - Address is out of memory
	ADDR_OUT_OF_MEM,
	//! <b>11</b> - Invalid function parameter
	NOT_VALID_PARAM,
	//! <b>12</b> - Built in self test failed
	BIST_FAILED,
	//! <b>13</b> - Before entering power down, the short term timer had timed out.	
	ST_TIMEOUT_BEFORE_SLEEP,
	//! <b>14</b> - Maximum number of filters reached, no more filter possible
	MAX_FILTER_REACHED,
	//! <b>15</b> - Filter to delete not found
	FILTER_NOT_FOUND,
	//! <b>16</b> - BaseID out of range
	BASEID_OUT_OF_RANGE,
	//! <b>17</b> - BaseID was changed 10 times, no more changes are allowed
	BASEID_MAX_REACHED,
	//! <b>18</b> - XTAL is not stable
	XTAL_NOT_STABLE,
	//! <b>19</b> - No telegram for transmission in queue  
	NO_TX_TEL,
	//!	<b>20</b> - Waiting before sending broadcast message
	TELEGRAM_WAIT,
	//!	<b>21</b> - Generic out of range return code
	OUT_OF_RANGE,
	//!	<b>22</b> - Function was not executed due to sending lock
	LOCK_SET,
	//! <b>23</b> - New telegram transmitted
	NEW_TX_TEL
} RETURN_TYPE;

//! Length of the radio telegrams
typedef enum
{
	//! RPS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_RPS =			 8,			 
	//! 1BS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_1BS =			 8,			 
	//! 4BS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_4BS =			 11,			 
	//! HRC decoded telegram length	in bytes
	RADIO_DEC_LENGTH_HRC =			 8,			 
	//! SYS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_SYS =			 11,			 
	//! SYS_EX decoded telegram length	in bytes
	RADIO_DEC_LENGTH_SYS_EX =		 16,			 
	//! Smart ack learn request decoded telegram length in bytes
	RADIO_DEC_LENGTH_SM_LRN_REQ =        17,
	//! Smart ack learn answer telegram length in bytes
	RADIO_DEC_LENGTH_SM_LRN_ANS =        15,	 
	//! Smart ack reclaim telegram length in bytes
	RADIO_DEC_LENGTH_SM_REC =        8,
	//! Signal telegram length in bytes
	RADIO_DEC_LENGTH_SIGNAL =        8,	 
	//! U2S decoded telegram length in bytes
	RADIO_DEC_LENGTH_U2S =           6,
	//! T2S decoded telegram length iin bytes
	RADIO_DEC_LENGTH_T2S =           6,			 

} RADIO_TEL_LENGTH;


	//! Telegram choice codes applies to radio telegram only
	typedef enum
	{
	 	//! RPS telegram	
		RADIO_CHOICE_RPS = 0xF6,
		//! 1BS telegram
		RADIO_CHOICE_1BS = 0xD5,			
		//! 4BS telegram
		RADIO_CHOICE_4BS = 0xA5,			
		//! HRC telegram 			
		RADIO_CHOICE_HRC = 0xA3,			
		//! SYS telegram 			
		RADIO_CHOICE_SYS = 0xA4,			
		//! SYS_EX telegram 			
		RADIO_CHOICE_SYS_EX = 0xC5,
		//! Smart Ack Learn Reuqest telegram
		RADIO_CHOICE_SM_LRN_REQ = 0xC6,
		//! Smart Ack Learn Answer telegram
		RADIO_CHOICE_SM_LRN_ANS = 0xC7,
		//! Smart Ack Reclaim telegram
		RADIO_CHOICE_RECLAIM = 0xA7,
		//! Smart Request telegram
		RADIO_CHOICE_SIGNAL = 0xD0,
		//! Encapsulated addressable telegram
		RADIO_CHOICE_ADT = 0xA6,
		//! Variable Length Data
    	RADIO_CHOICE_VLD = 0xD2,
		//! Universal Teach In EEP based
		RADIO_CHOICE_UTE = 0xD4,
		//! Manufacturer Specific Communication
		RADIO_CHOICE_MSC = 0xD1,
		//! Chained data message
		RADIO_CHOICE_CDM = 0x40,
		//! Secure telegram	without choice encapsulation
		RADIO_CHOICE_SEC = 0x30,
		//! Secure telegram	with choice encapsulation
		RADIO_CHOICE_SEC_ENCAPS = 0x31,
		//! Non-secure telegram
		RADIO_CHOICE_NON_SEC = 0x32,
		//! Secure teach-in telegram
		RADIO_CHOICE_SEC_TI = 0x35,

	} CHOICE_TYPE;

	/****************************************************/
	//			TEL_RADIO_TYPE
	//! Telegram structure used for Radio transmission.
	typedef union {
		struct raw_TEL_RADIO_TYPE
		{
			//! Telegram seen as a array of bytes without logical structure.
			uint8 bytes[RADIO_BUFF_LENGTH];
			//! Radio telegram length, it's not part of the transmitted/received data, it is used only for processing the telegram
			RADIO_TEL_LENGTH u8Length;
		} raw;
				
		//! RPS/1BS/HRC/smart_req radio telegram structure
		struct trps_t1bs_thrc_smart_req{
			CHOICE_TYPE u8Choice;
			uint8  		u8Data;
			uint32 		u32Id;
			uint8  		u8Status;											
			uint8  		u8Chk;
			uint8		u8Fill[RADIO_BUFF_LENGTH-8];
			RADIO_TEL_LENGTH u8Length;
		}trps, t1bs, thrc, smart_req;	

		//! 4BS/SYS radio telegram structure
		struct t4bs_tsys{
			CHOICE_TYPE u8Choice;
			uint8  		u8Data3;
			uint8  		u8Data2;
			uint8  		u8Data1;
			uint8  		u8Data0;
			uint32 		u32Id;
			uint8  		u8Status;
			uint8  		u8Chk;
			uint8		u8Fill[RADIO_BUFF_LENGTH-11];
			RADIO_TEL_LENGTH u8Length;
		}t4bs,tsys;							   

		//! SYS_EX radio telegram structure
		// struct sys_ex{
			// CHOICE_TYPE u8Choice;
            // // ! Note the sequence is correct because Keil takes the bitfield from right to left. See telegram definition for more information
			// uint8 	 	u6Index	: 6; 	
			// uint8 		u2Seq	: 2; 
			// sys_ex_data	u64Data;
			// uint32 		u32SenderId;
			// uint8  		u8Status;
			// uint8  		u8Chk;
			// uint8		u8Fill[RADIO_BUFF_LENGTH-16];
			// RADIO_TEL_LENGTH u8Length;
		// }sys_ex;

		//! smart ack learn request radio telegram structure
		// struct sm_lrn_req{
			// CHOICE_TYPE u8Choice;
			// uint16 	 	u11ManufacturerId	: 11; 	
			// uint16 		u5ReqCode			: 5;
			// uint8		u8EEP[3];
			// uint8		u8RssiDbm;		
			// uint32 		u32RepeaterId;
			// uint32 		u32SenderId;
			// uint8  		u8Status;
			// uint8  		u8Chk;
			// uint8		u8Fill[RADIO_BUFF_LENGTH-17];
			// RADIO_TEL_LENGTH u8Length;
		// }sm_lrn_req;

		//! smart ack learn asnwer radio telegram structure
		// struct sm_lrn_ans{
			// CHOICE_TYPE 	u8Choice;
			// uint8  			u8MsgIdx;
			// smart_answer	u56Data;
			// uint32 			u32SenderId;
			// uint8  			u8Status;
			// uint8	  		u8Chk;
			// uint8			u8Fill[RADIO_BUFF_LENGTH-15];
			// RADIO_TEL_LENGTH u8Length;
		// }sm_lrn_ans;

		//! smart ack reclaim radio telegram structure
		// struct sm_rec{
			// CHOICE_TYPE u8Choice;
			// uint8		u7MailBoxIdx : 7;
			// uint8		u1ReclaimIdx : 1;
			// uint32 		u32SenderId;
			// uint8  		u8Status;
			// uint8  		u8Chk;
			// uint8		u8Fill[RADIO_BUFF_LENGTH-8];
			// RADIO_TEL_LENGTH u8Length;
		// }sm_rec;

		//! signal radio telegram structure
		// struct sig{
			// CHOICE_TYPE u8Choice;
			// uint8		u8SignalIdx;
			// uint32 		u32SenderId;
			// uint8  		u8Status;
			// uint8  		u8Chk;
			// uint8		u8Fill[RADIO_BUFF_LENGTH-8];
			// RADIO_TEL_LENGTH u8Length;
		// }sig;

		//! generic profiles single telegram 
		// struct gp_single_raw
		// {
			// CHOICE_TYPE u8Choice;
			// uint8  		u8Data[20];
			// RADIO_TEL_LENGTH u8Length;
		// } msg_raw;
	
		//! generic profiles chain telegram structures
		// struct msg_chain
		// {
			// CHOICE_TYPE u8Choice;
			// uint8 	 	u6Index	: 6; 	   		//!<Note the sequence is correct because Keil takes the bitfield from right to left. See telegram definition for more information
			// uint8 		u2Seq	: 2; 
			// uint8  		u8Data[19];		   		//!<Data + ID + Status + Checksum
			// RADIO_TEL_LENGTH u8Length;
		// } msg_chain;
	
		//! Secure teach-in telegram
		// struct sec_ti
		// {
			// CHOICE_TYPE u8Choice;
			// uint8 	 	u2Info	: 2; 			//!<Note the sequence is correct because Keil takes the bitfield from right to left. See telegram definition for more information
			// uint8 		u2Type	: 2;
			// uint8 		u2Count	: 2;
			// uint8 		u2Index	: 2; 
			// uint8  		u8Data[19];				//!<Data + ID + Status + Checksum
			// RADIO_TEL_LENGTH u8Length;
		// } sec_ti;

		//! generic profiles chain telegram stucture for the zero telegram
		// struct msg_chain_0
		// {
			// CHOICE_TYPE u8Choice;
			// uint8 	 	u6Index	: 6; 			//!<Note the sequence is correct because Keil takes the bitfield from right to left. See telegram definition for more information
			// uint8 		u2Seq	: 2; 
			// uint16		u16Length;
			// uint8  		u8Data[17];		  		//!<Data + ID + Status + Checksum
			// RADIO_TEL_LENGTH u8Length;
		// } msg_chain_0;

	} TEL_RADIO_TYPE;	

	/****************************************************/
  	//! Telegram parameter structure
	typedef union 
	{
		//! param structure for transmitted telegrams
		struct p_tx {
			//! number of subtelegrams to send 
			uint8  u8SubTelNum;
			//! to send Destination ID
			uint32 u32DestinationId;

		}p_tx;

		//! param structure for received telegrams
		struct p_rx{
			//! number of subtelegrams received	(= number of originals + number of repeated)
			uint8  u8SubTelNum;
			//! received Destination ID
			uint32 u32DestinationId;
			//! u8Dbm of the last subtelegram calculated from RSSI. Note this value is an unsigned value. The real dBm signal is a negative value.
			uint8 u8Dbm;
		}p_rx;
	} TEL_PARAM_TYPE;


	
	
/*********************************************/

RETURN_TYPE UARTWriteBuffer(UINT8 *buf, UINT32 length);
RETURN_TYPE UARTSendPacket(UINT8 *pBuffer);
RETURN_TYPE getPacket(UINT8 *pBuffer, UINT32 length) ;

BOOL compareID(UINT8 *pBuffer, UINT8 *ID);

RETURN_TYPE parseRadio(UINT8 *pBuffer, UINT32 length);
RETURN_TYPE parseResponse(UINT8 *pBuffer, UINT32 length);
RETURN_TYPE parseRadioSubTel(UINT8 *pBuffer, UINT32 length);
RETURN_TYPE parseEvent(UINT8 *pBuffer, UINT32 length);
RETURN_TYPE parseCommonCommand(UINT8 *pBuffer, UINT32 length);
RETURN_TYPE parseSmartAckCommand(UINT8 *pBuffer, UINT32 length);
RETURN_TYPE parseRemoteManCommand(UINT8 *pBuffer, UINT32 length);





RETURN_TYPE radio_getTelegram(TEL_RADIO_TYPE *pu8RxRadioTelegram, TEL_PARAM_TYPE *pu8TelParam);


void DEBUGDisplayRAW(TEL_RADIO_TYPE* const telegram);
void DEBUGDisplayTRPS(TEL_RADIO_TYPE* const  telegram);
void DEBUGDisplayT1BS(TEL_RADIO_TYPE* const telegram);
void DEBUGDisplayT4BS(TEL_RADIO_TYPE* const telegram);
void DEBUGDisplayTELPARAM(TEL_PARAM_TYPE* const param);

#endif 
