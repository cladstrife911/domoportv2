#include "ute_mod.h"

// #include "tools.h"
#include "console.h"

/*
Check if the UTE telegram is valid to learn
called by proc_telValidToLearn
*/
UTE_RETURN_TYPE ute_validToLearn(TEL_RADIO_TYPE const *pRadioTel, TEL_PARAM_TYPE const *pRadioParam)
{
	// uint32 idTmp;
	
	ConsoleWrite("ENTER ute_validToLearn\r\n");

	if(	pRadioTel->raw.bytes[1] == UTE_EEP_RORG &&	//DB_0
		pRadioTel->raw.bytes[2] == UTE_EEP_FUNC &&	//DB_1
		pRadioTel->raw.bytes[3] == UTE_EEP_TYPE	&&	//DB_2
		pRadioTel->raw.bytes[4] == 0x00	&&					//DB_3
		pRadioTel->raw.bytes[5] == UTE_MANUFACTURER_ID	&&	//DB_4
		pRadioTel->raw.bytes[6] == 0xFF	//all supported channel		//DB_5
		) 
	{
		if((pRadioTel->raw.bytes[7] & 0x0F ) != 0x00) 	//DB_6.BIT_3..0 CMD00
			return UTE_NO_SUCCESS;
		
		if((pRadioTel->raw.bytes[7] & 0xF0) == 0xA0)	//DB_6.BIT_7..4 bidirectionnal + teach-in request
			return UTE_SUCCESS;	
		
	}
		
	ConsoleWrite("EXIT ERROR ute_validToLearn\r\n");	
	return UTE_NO_SUCCESS;
}


//called by id_search
UTE_RETURN_TYPE ute_getEntryType(TEL_RADIO_TYPE const *pRadioTel, ID_ENTRY_TYPE *pSearchOut)
{
	ConsoleWrite("ENTER ute_getEntryType()\r\n");

	
	//UTE is used for teach-in procedure with VLD messages
	pSearchOut->u32control.u8RORG = RADIO_CHOICE_VLD;
	pSearchOut->u32control.u6reserved = 0;
	
	//get sender ID
	memcpy(&(pSearchOut->u32Id), &(pRadioTel->raw.bytes[8]),4);
	
	return UTE_SUCCESS;
}


/*
send teach-in query when LRN mode starts
*/
UTE_RETURN_TYPE ute_sendTeachInResponse(TEL_RADIO_TYPE const *pRadioTel)
{

	TEL_RADIO_TYPE	rTel;
	TEL_PARAM_TYPE	pTel;
	
	
	memset(&(rTel.raw.bytes[0]),0,RADIO_BUFF_LENGTH);	//21

	pTel.p_tx.u8SubTelNum = 3;	//send 3 subtelegram
	
	//response must be unicast
	memcpy(&(pTel.p_tx.u32DestinationId), &(pRadioTel->raw.bytes[8]),4);

	rTel.raw.bytes[0] = RADIO_CHOICE_UTE;
	
	//#### UTE CMD 0x0 payload ###
	//TODO à modifier pour gérer plusieurs gigogne
	rTel.raw.bytes[1] = pRadioTel->raw.bytes[1]; 	//UTE_EEP_RORG;
	rTel.raw.bytes[2] = pRadioTel->raw.bytes[2];	//UTE_EEP_FUNC;
	rTel.raw.bytes[3] = pRadioTel->raw.bytes[3];	//UTE_EEP_TYPE;
	rTel.raw.bytes[4] = pRadioTel->raw.bytes[4];	//0;	//MSB ManID
	rTel.raw.bytes[5] = pRadioTel->raw.bytes[5];	//UTE_MANUFACTURER_ID; //manID Giga-Concept 0x03F
	rTel.raw.bytes[6] = 0xFF; //teach-in all supported channel
	rTel.raw.bytes[7] = 0x91;//0b10010001 - Bidirectionnal + Response accepted + CMD 0x1
	//######
	
	rTel.raw.u8Length =  (RADIO_TEL_LENGTH) RADIO_DEC_LENGTH_UTE;	//20
	
	enocean_enableRadioRX(TRUE);
	while (radio_sendTelegram(&rTel, &pTel)!=OK);
	enocean_enableRadioRX(FALSE);
	
	return UTE_SUCCESS;
}

//EOF
