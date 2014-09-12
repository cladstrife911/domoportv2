#include "enocean_device.h"

#include "ute_mod.h"
#include "vld_cmd_mod.h"

#include "tools.h"
#include "console.h"

/********* IDTABLE ******/
ID_TABLE_TYPE IDTable;


ID_RETURN_TYPE enocean_idAdd(ID_TABLE_TYPE *pIDtable, TEL_RADIO_TYPE* const pRadioTel)
{
	// Information to enter in the ID table
	ID_ENTRY_TYPE   entryToAdd;
	// ID table index
	uint8 		    u8i;

	ConsoleWrite("ENTER enocean_idAdd()\r\n");
	DEBUGDisplayRAW(pRadioTel);
	
	// Get the current nr. of IDs stored in ID table
	u8i	= ID_PTABLE_NR_ENTRIES(pIDtable);				

	// Space in the ID table for a new entry?
	if(u8i >= ID_MAX_NR_ENTRIES)
	{  	// No more space
		ConsoleWrite("EXIT FULL enocean_idAdd()\r\n");
	 	return ID_MEMORY_FULL;
	}
	
	memset(&(entryToAdd.u32control), 0, sizeof(ID_ENTRY_CTRL_TYPE));
	
	// Depending on the telegram choice the preparation for storage is different
	// Switch telegrams (RPS and HRC) store, apart from ID and telegram type, store channel and rocker in the entry.
	switch (pRadioTel->raw.bytes[0])
	{	
		case RADIO_CHOICE_RPS:
			entryToAdd.u32control.u8RORG 	= RADIO_CHOICE_RPS;
			entryToAdd.u32control.u2rocker	= 0;//PROC_GET_PRPS_ROCKER1(pRadioTel); 
			entryToAdd.u32Id 			    = pRadioTel->trps.u32Id;
			break;//RADIO_CHOICE_RPS

		case RADIO_CHOICE_1BS:
			entryToAdd.u32control.u8RORG 	= RADIO_CHOICE_1BS;
			entryToAdd.u32Id 			    = pRadioTel->t1bs.u32Id;
			break;//RADIO_CHOICE_1BS

		case RADIO_CHOICE_4BS:
			entryToAdd.u32control.u8RORG 	= RADIO_CHOICE_4BS;
			entryToAdd.u32control.u8FUNC 	= pRadioTel->t4bs.u8Data3 >>2;
			entryToAdd.u32control.u8TYPE 	= (pRadioTel->t4bs.u8Data3 & 0x03) + (pRadioTel->t4bs.u8Data2 >>3);
			entryToAdd.u32Id 			    = pRadioTel->t4bs.u32Id;
			break;//RADIO_CHOICE_4BS
		
		case RADIO_CHOICE_UTE: //UTE request for VLD messages
			if(ute_getEntryType(pRadioTel, &entryToAdd) == UTE_SUCCESS)
			break;	//RADIO_CHOICE_UTE

		// Telegram choice error. This type of telegram can not be stored in the ID table.
		default:
			ConsoleWrite("EXIT NO_SUCCESS enocean_idAdd()\r\n");
			return ID_NO_SUCCESS;
	}
	
	
	// Addition of the information to the ID table
	pIDtable->entry[u8i].u32control = entryToAdd.u32control;
	memcpy(&(pIDtable->entry[u8i].u32Id),&(entryToAdd.u32Id),4);

	ConsoleWrite("\r\nEntry ID: ");
	uart_debugUINT32(entryToAdd.u32Id);
	// ConsoleWrite("\r\nEntry Control: ");
	// uart_debugUINT32((UINT32)entryToAdd.u32control);

	ConsoleWrite("\r\npIDtable ID: ");
	uart_debugUINT32(pIDtable->entry[u8i].u32Id);
	// ConsoleWrite("\r\npIDtable Control: ");
	// uart_debugUINT32((UINT32)pIDtable->entry[u8i].u32control);
	ConsoleWrite("\r\n");
	
	// Increment the ID table entry counter
	pIDtable->u8nrIDs++; 
	
	ConsoleWrite("EXIT SUCCESS enocean_idAdd()\r\n");
	return 	ID_SUCCESS;
}


ID_RETURN_TYPE enocean_idSearch(ID_TABLE_TYPE const *pIdTable, TEL_RADIO_TYPE const *pRadioTel, ID_SEARCH_OUT_TYPE *pSearchOut)
{
	// Information to search in the ID table
	ID_ENTRY_TYPE   	entryToSearch;
	// ID table entry index
	uint8           	u8index=0;

	// ConsoleWrite("ENTER enocean_idSearch()\r\n");
	
	//check if IDTable is empty
	if(ID_PTABLE_NR_ENTRIES(pIdTable) <=0 )
	{
		// ConsoleWrite("EXIT NO_SUCCESS enocean_idSearch()\r\n");
		return ID_NO_SUCCESS;
	}
	
	// Transform the input information to ID table format, for faster comparison.
	// Get the radio telegram type, to decide what is to be transformed.
	switch (pRadioTel->raw.bytes[0])
	{
		case RADIO_CHOICE_RPS:
			entryToSearch.u32control.u8RORG 	= RADIO_CHOICE_RPS;	// Load the RPS code as stored in the ID table control.type bits
			entryToSearch.u32Id 		        = pRadioTel->trps.u32Id;			// Load the ID bytes to search
			// entryToSearch.u16control.u2rocker = PROC_GET_PRPS_ROCKER(pRadioTel);	//get the rocker (A or B), 0 or 1. Important for 2 rockers devices
			break;

		case RADIO_CHOICE_1BS:
			entryToSearch.u32control.u8RORG 	= RADIO_CHOICE_1BS;	// Load the 1BS code as stored in the ID table control.type bits
			entryToSearch.u32Id      	        = pRadioTel->t1bs.u32Id;		// Load the ID bytes to search
			break;

		case RADIO_CHOICE_4BS:
			entryToSearch.u32control.u8RORG 	= RADIO_CHOICE_4BS;	// Load the 4BS code as stored in the ID table control.type bits
			entryToSearch.u32Id       	        = pRadioTel->t4bs.u32Id;		// Load the ID bytes to search
			break;
			
		case RADIO_CHOICE_UTE:
			ute_getEntryType(pRadioTel, &entryToSearch); //extract RORG and ID	
			break;

		default:
			// ConsoleWrite("EXIT enocean_idSearch()\r\n");
            return ID_NO_SUCCESS;

	}
	
	// Search through the ID table (IDtable.entry[]) the information entryToSearch  
	for(u8index = 0; u8index < (ID_PTABLE_NR_ENTRIES(pIdTable)); u8index++)
	{
		//Device ID && RORG match
		if((pIdTable->entry[u8index].u32control.u8RORG != entryToSearch.u32control.u8RORG ) ||
			(pIdTable->entry[u8index].u32Id	!=  entryToSearch.u32Id))
		{
		  	continue;
		}

		// The switch rocker must be searched AND was correctly found?
		// if(	(entryToSearch.u16control.u8type == RADIO_CHOICE_RPS)&&
			// (pIdTable->entry[u8index].u16control.u2rocker != entryToSearch.u16control.u2rocker) &&
			// (pIdTable->entry[u8index].u16control.u4reserved == 0))	//only rocker
		// {	// No search needed OR not found.
			// continue;   
		// }
		
		// Load the values to return: the entry index and the control where info was found
		pSearchOut->u8index     = u8index; 
		pSearchOut->u32control  = pIdTable->entry[u8index].u32control; 
			
		// Information found! The entry index, where the info was found, is stored in *u8index
		// ConsoleWrite("EXIT SUCCESS enocean_idSearch()\r\n");
		return ID_SUCCESS;					
	}

	// ConsoleWrite("EXIT NO_SUCCESS enocean_idSearch()\r\n");
	
	// No ID entry found with the given search criteria
 	return ID_NO_SUCCESS;
}


ID_RETURN_TYPE enocean_validToLearn(TEL_RADIO_TYPE const *pRadioTel, TEL_PARAM_TYPE const *pRadioParam)
{

    // When the repeater counter is to be checked AND
    // the repeater counter (low nible from penultimate byte) is not 0 (repeated telegram) telegram not valid to learn 
    // if((repeaterCheck == ORIGINAL_ONLY)  && (pRadioTel->raw.bytes[pRadioTel->raw.u8Length - 2] & 0x0F))
    // {
        // return ID_NO_SUCCESS;
    // }
	
	/****************************/
	/*		UTE - Universal Teah-in     */
	/****************************/
	if( (pRadioTel->raw.bytes[0]                        == (uint8)RADIO_CHOICE_UTE) ) 
    {	
		if(ute_validToLearn(pRadioTel, pRadioParam) == UTE_SUCCESS)		
        return ID_SUCCESS;
		
    }
	
	/****************************/
	/*		RPS							  */
	/****************************/
    // RPS valid in all modes AND only 1 rocker pressed
    if( (pRadioTel->raw.bytes[0]                        == (uint8)RADIO_CHOICE_RPS) &&	
        (PROC_GET_PRPS_NORMAL_UNASSIGNED(pRadioTel)     == 1                )&&
	    (PROC_GET_PRPS_2ND_ACTIVATION(pRadioTel)	    == 0                ) ) 
    {
        return ID_SUCCESS;
    }
  
      // RPS valid key card
    if( (pRadioTel->raw.bytes[0]                        == (uint8)RADIO_CHOICE_RPS) &&
		(pRadioTel->raw.bytes[1]==0x10)  && //key card inserted
	    (PROC_GET_PRPS_2ND_ACTIVATION(pRadioTel)	    == 0                ) ) 
    {
        return ID_SUCCESS;
    }
   
     // RPS valid Windows Handle
    if( (pRadioTel->raw.bytes[0]                        == (uint8)RADIO_CHOICE_RPS) &&
		(pRadioTel->raw.bytes[1] & 0x80) )// && //window handle		
    {
        return ID_SUCCESS;
    }
	
	/****************************/
	/*		1BS					*/
	/****************************/
    // 1BS only valid in bidirectional mode AND if the learn bit active
	// magnetic contact
    if( (pRadioTel->raw.bytes[0]          == (uint8)RADIO_CHOICE_1BS ) &&
        ((pRadioTel->t1bs.u8Data & 0x08)  == 0 ) )
    {
        return ID_SUCCESS;
    }
	  
	/********************************/
	/*		4BS						*/
	/* temperature sensor A5-02		*/
	/* light sensor A5-06			*/
	/* occupancy A5-07				*/
	/* occupancy light A5-08		*/
	/********************************/
    if( (pRadioTel->raw.bytes[0] == (uint8)RADIO_CHOICE_4BS) && 	//RORG
        ((pRadioTel->t4bs.u8Data0 & EEP_4BS_TEACHIN_LEARN_BIT) == EEP_4BS_TEACHIN_LEARN_BIT ) &&	//learn bit
			(	//##### TYPE #####
				//temperature sensor
				(pRadioTel->t4bs.u8Data3 & ID_SEARCH_FUNC_4BS_MASK) == (ID_FUNC_4BS_TEMPERATURE<<2) ||
				//Light sensor
				(pRadioTel->t4bs.u8Data3 & ID_SEARCH_FUNC_4BS_MASK) == (ID_FUNC_4BS_LIGHT<<2) ||
				//Occupancy sensor
				(pRadioTel->t4bs.u8Data3 & ID_SEARCH_FUNC_4BS_MASK) == (ID_FUNC_4BS_OCCUPANCY<<2) ||
				//light and occupancy sensor 
				(pRadioTel->t4bs.u8Data3 & ID_SEARCH_FUNC_4BS_MASK) == (ID_FUNC_4BS_OCCUPANCY_LIGHT<<2)
			)
	)
	{
        return ID_SUCCESS;
	}
	
    return ID_NO_SUCCESS;
}

// ***************************** ID_DELETE *******************
//TODO a tester
ID_RETURN_TYPE enocean_idDelete(ID_TABLE_TYPE *pIDtable, uint8 u8index)
{
	// Stores the index of the last entry in the ID table 
	uint8 u8lastEntryIndex;

	ConsoleWrite("ENTER enocean_idDelete()\r\n");

	// Useless to delete if no IDs in ID table
	if(pIDtable->u8nrIDs == 0)
	{
		return ID_MEMORY_EMPTY;
	}

	u8lastEntryIndex = pIDtable->u8nrIDs - 1;
   	
	// The entry index to delete can not be over the index of the last entry, since the ID table is compacted 
	if(u8index > u8lastEntryIndex)
	{
	 	return ID_NO_SUCCESS;
	}

	// Compact the ID table, by moving the last entry to the entry to erase.
	// In case that only 1 entry or the last entry is to erase, no move is needed 
	pIDtable->entry[u8index] = pIDtable->entry[u8lastEntryIndex];	

    memset(&pIDtable->entry[u8lastEntryIndex], 0, sizeof(ID_ENTRY_TYPE));

	// Decrement the nr. of entries in the ID table
	pIDtable->u8nrIDs--;
	
	ConsoleWrite("EXIT enocean_idDelete()\r\n");

	return ID_SUCCESS;	 
}
// ***************************** END ID_DELETE *******************



ID_RETURN_TYPE enocean_idDeleteAll()
{
	IDTable.u8nrIDs = 0;
	return ID_SUCCESS; 
}




/********************************************************************
* send RPS telegram to simulate a switch
********************************************************************/

/**
*
envoi d'un telegram RPS
@pare	senderID	- ID à utiliser pour envoyer la commande
@param	value	- 0 pour éteindre, 1 pour allumer
@param	canal	- canal à utiliser (0 ou 1)
@param 	release 1 - envoyer le code touche 0 pour simuler le relachement de la touche
		release 0 - ne pas envoyer le relachement de la touche
*/
ID_RETURN_TYPE enocean_sendRPS(UINT32 senderID, uint8 value, uint8 release)//, uint8 canal, uint8 release)
{
	TEL_RADIO_TYPE telegram;
	TEL_PARAM_TYPE telParam;

	ConsoleWrite("ENTER enocean_sendRPS()\r\n");
	
	memset(&(telegram.raw.bytes[0]),0,RADIO_BUFF_LENGTH);	//21

	telParam.p_tx.u8SubTelNum = 3;	//send 3 subtelegram
	
	telParam.p_tx.u32DestinationId = 0xFFFFFFFF; 
	ConsoleWrite("DestId=");
	uart_debugUINT32(telParam.p_tx.u32DestinationId);
	ConsoleWrite("\r\n");

	telegram.raw.bytes[0] = RADIO_CHOICE_RPS;
	
	ConsoleWrite("DB0=");
	uart_debugHexa(value);//canal<<2 + value<<1 + 1);
	ConsoleWrite("\r\n");
	
	//code de la touche 0x10, 0x30, 0x50, 0x70
	telegram.raw.bytes[1] = value;//canal<<2 + value<<1 + 1;
	memcpy(telegram.raw.bytes[2], &senderID, 4);
	telegram.raw.bytes[6] = 0x30; //status
	
	
	telegram.raw.u8Length =  14;	//(rorg + data + opt data)
	
	radio_sendTelegram(&telegram, &telParam);
	
	
	if(release == 1)
	{
		vTaskDelay(20);
		telegram.raw.bytes[1] = 0;
		telegram.raw.bytes[6] = 0x20; //status
		radio_sendTelegram(&telegram, &telParam);
	}

	return ID_SUCCESS;
}


/********************************************************************
*		fonctions VLD spécifique à la gigogne
*********************************************************************/
/**
*
EEP D2-01
VLD CMD 0x01
@param 	status = 0 -> output OFF
		status = 1 -> output ON
*/
ID_RETURN_TYPE vld_sendCMD01(ID_ENTRY_TYPE const * pEntryId, uint8 status)
{
	TEL_RADIO_TYPE telegram;
	TEL_PARAM_TYPE telParam;

	ConsoleWrite("ENTER vlc_sendCMD01()\r\n");
	
	memset(&(telegram.raw.bytes[0]),0,RADIO_BUFF_LENGTH);	//21

	telParam.p_tx.u8SubTelNum = 3;	//send 3 subtelegram
	
	memcpy(&(telParam.p_tx.u32DestinationId) , &(pEntryId->u32Id), 4); 
	ConsoleWrite("DestId=");
	uart_debugUINT32(telParam.p_tx.u32DestinationId);
	ConsoleWrite("\r\n");

	telegram.raw.bytes[0] = RADIO_CHOICE_VLD;
	
	//#### VLD CMD 0x01 payload ###
	telegram.raw.bytes[1] = (VLD_CMD_ID)VLD_CMD_01;
	telegram.raw.bytes[2] = 0;
	telegram.raw.bytes[3] = status;
	//######
	
	telegram.raw.u8Length =  1 + VLD_PAYLOAD_CMD1 + 5 + 7;	//18 (rorg + data + opt data)
	
	radio_sendTelegram(&telegram, &telParam);
	

	return ID_SUCCESS;
}

/**
*
EEP D2-01
VLD CMD 0x03 actuator status query
@param 	channel = 0 - channel number (valid: 0 or 0x1E)
*/
ID_RETURN_TYPE vld_sendCMD03(ID_ENTRY_TYPE const * pEntryId, uint8 channel)
{
	TEL_RADIO_TYPE telegram;
	TEL_PARAM_TYPE telParam;

	ConsoleWrite("ENTER vlc_sendCMD03()\r\n");
	
	memset(&(telegram.raw.bytes[0]),0,RADIO_BUFF_LENGTH);	//21

	telParam.p_tx.u8SubTelNum = 3;	//send 3 subtelegram
	
	memcpy(&(telParam.p_tx.u32DestinationId) , &(pEntryId->u32Id), 4); 
	ConsoleWrite("DestId=");
	uart_debugUINT32(telParam.p_tx.u32DestinationId);
	ConsoleWrite("\r\n");

	telegram.raw.bytes[0] = RADIO_CHOICE_VLD;
	
	//#### VLD CMD 0x01 payload ###
	telegram.raw.bytes[1] = (VLD_CMD_ID)VLD_CMD_03;
	telegram.raw.bytes[2] = channel;
	//######
	
	telegram.raw.u8Length =  1 + VLD_PAYLOAD_CMD3 + 5 + 7;	//18 (rorg + data + opt data)
	
	radio_sendTelegram(&telegram, &telParam);
	

	return ID_SUCCESS;
}


