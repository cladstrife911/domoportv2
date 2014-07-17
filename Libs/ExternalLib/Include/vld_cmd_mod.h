#ifndef _VLD_CMD_INCLUDED
#define _VLD_CMD_INCLUDED



//default time for VLD autoreport timeout
#define MEASURE_MAX_TIME_DEFAULT 6	//1min
#define MEASURE_MIN_TIME_DEFAULT 60	//1min
#define MEASURE_UNIT_WS	0
#define MEASURE_UNIT_WH	1
#define MEASURE_UNIT_KWH	2
#define MEASURE_UNIT_W		3
#define MEASURE_UNIT_KW	4

#define MEASURE_ENERGY 0
#define MEASURE_POWER 1

#define MEASUREMENT_REQUEST 0
#define MEASUREMENT_AUTO 1

/*
en mode METERING:
commandes envoyées par la plug: 04 07
commandes reçues par la plug: 01 02 03 05 06
*/

#define VLD_PAYLOAD_CMD1      (3)
#define VLD_PAYLOAD_CMD2      (4)
#define VLD_PAYLOAD_CMD3      (2)
#define VLD_PAYLOAD_CMD4      (3)
#define VLD_PAYLOAD_CMD5      (6)
#define VLD_PAYLOAD_CMD6      (2)
#define VLD_PAYLOAD_CMD7      (6)

#define VLD_LENGTH_CMD1  (VLD_PAYLOAD_CMD1 + 7)                // 7 byte (1(u8Choice)+4(u32Id)+1(u8Status)+1(u8Chk)) 
#define VLD_LENGTH_CMD2  (VLD_PAYLOAD_CMD2 + 7)                // 7 byte (1(u8Choice)+4(u32Id)+1(u8Status)+1(u8Chk)) 
#define VLD_LENGTH_CMD3  (VLD_PAYLOAD_CMD3 + 7)                // 7 byte (1(u8Choice)+4(u32Id)+1(u8Status)+1(u8Chk)) 
#define VLD_LENGTH_CMD4  (VLD_PAYLOAD_CMD4 + 7)                // 7 byte (1(u8Choice)+4(u32Id)+1(u8Status)+1(u8Chk)) 
#define VLD_LENGTH_CMD5  (VLD_PAYLOAD_CMD5 + 7)                // 7 byte (1(u8Choice)+4(u32Id)+1(u8Status)+1(u8Chk)) 
#define VLD_LENGTH_CMD6  (VLD_PAYLOAD_CMD6 + 7)                // 7 byte (1(u8Choice)+4(u32Id)+1(u8Status)+1(u8Chk)) 
#define VLD_LENGTH_CMD7  (VLD_PAYLOAD_CMD7 + 7)                // 7 byte (1(u8Choice)+4(u32Id)+1(u8Status)+1(u8Chk)) 

#define VLD_OUTPUT_CHANNEL_0		0x00
#define VLD_OUTPUT_CHANNEL_1		0x01
#define VLD_OUTPUT_CHANNEL_ALL	0x1E
#define VLD_OUTPUT_CHANNEL_NOT_SUPPORTED 0x7F
#define VLD_OUTPUT_CHANNEL_MASK	0x1F

#define VLD_DEFAULT_STATE_MASK 0x30
#define VLD_CMD_MASK 0x0F

//Command 05 definitions
#define VLD_CMD05_DB4_MASK_AUTOREPORT 0x80
#define VLD_CMD05_DB4_MASK_RESETMEAS 0x40	//not used
#define VLD_CMD05_DB4_MASK_MEASMODE 0x20
#define VLD_CMD05_DB4_MASK_IOCHANNEL 0x1F
#define VLD_CMD05_DB3_MASK_LSB 0xF0
#define VLD_CMD05_DB3_MASK_UNIT 0x07

#define VLD_CMD05_DB4_MEASMODE_POWER 0x01
#define VLD_CMD05_DB4_MEASMODE_ENERGY 0x00

#define VLD_CMD06_DB0_MASK_QUERY 0x20
#define VLD_CMD06_DB0_MASK_CHANNEL 0x1F


typedef enum 
{
	VLD_CMD_01 = 0x01,	//Actuator set Output
	VLD_CMD_02 = 0x02,	//Actuator set Local
	VLD_CMD_03 = 0x03,	//Actuator Status Query
	VLD_CMD_04 = 0x04,	//Actuator Status Response
	VLD_CMD_05 = 0x05,	//Actuator set Measurement
	VLD_CMD_06 = 0x06,	//Actuator Measurement Query
	VLD_CMD_07 = 0x07	//Actuator Measurement Response
}VLD_CMD_ID;


typedef enum
{
	VLD_SUCCESS=0,
	VLD_NO_SUCCESS
}VLD_RETURN_TYPE;


/********************** FUNCTIONS DEFINITIONS***********************/


#endif
