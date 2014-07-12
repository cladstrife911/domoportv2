#include "enocean_device.h"

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
			entryToAdd.u32Id 			    = pRadioTel->t4bs.u32Id;
			break;//RADIO_CHOICE_4BS
		
		case RADIO_CHOICE_UTE: //UTE request for VLD messages
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

	ConsoleWrite("ENTER enocean_idSearch()\r\n");
	
	//check if IDTable is empty
	if(ID_PTABLE_NR_ENTRIES(pIdTable) <=0 )
	{
		ConsoleWrite("EXIT NO_SUCCESS enocean_idSearch()\r\n");
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
			
		// case RADIO_CHOICE_UTE:
			// if(mode == 1) //LEARN
			// {
				// ute_getEntryType(pRadioTel, &entryToSearch); //extract RORG and ID
				// break;
			// }else
				// return ID_NO_SUCCESS;
		// case RADIO_CHOICE_VLD: //UTE radio message is use for VLD telegram
			// if(mode == 0) //OPERATION
			// {
				// entryToSearch.u32control.u8RORG 	= RADIO_CHOICE_VLD;
				// vld_getId(pRadioTel, &entryToSearch);	
				// break;
			// }else
				// return ID_NO_SUCCESS;
		default:
			ConsoleWrite("EXIT enocean_idSearch()\r\n");
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
		ConsoleWrite("EXIT SUCCESS enocean_idSearch()\r\n");
		return ID_SUCCESS;					
	}

	ConsoleWrite("EXIT NO_SUCCESS enocean_idSearch()\r\n");
	
	// No ID entry found with the given search criteria
 	return ID_NO_SUCCESS;
}



ID_RETURN_TYPE enocean_idDeleteAll()
{
	IDTable.u8nrIDs = 0;
	return ID_SUCCESS; 
}
