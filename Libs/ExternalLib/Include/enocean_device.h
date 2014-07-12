#ifndef __ENOCEAN_DEVICE_H_
#define __ENOCEAN_DEVICE_H_

#include "enocean_esp3.h"
#include "tools.h"
#include "console.h"

//! Maximum number of entries (IDs) that can be stored in the ID table
#define ID_MAX_NR_ENTRIES 30

#define ENOCEAN_SIZE_IDTABLE 0xF6

//! ID functions return codes.
typedef enum
{
	//! Action correctly carried out 
	ID_SUCCESS = 0      ,
	//! Error, ID table full with #ID_MAX_NR_ENTRIES IDs
	ID_MEMORY_FULL      , 
	//! Error, ID table is empty
	ID_MEMORY_EMPTY     , 
	//! No success in the search.
	ID_NO_SUCCESS
    	
} ID_RETURN_TYPE;



typedef enum
{
	//! Identification code for rocker 0 in the ID table entry
 	ID_TABLE_ROCKER0 = 0    ,
	//! Identification code for rocker 1 in the ID table entry
 	ID_TABLE_ROCKER1        ,
	//! Identification code for rocker 2 in the ID table entry
 	ID_TABLE_ROCKER2        ,
	//! Identification code for rocker 3 in the ID table entry
 	ID_TABLE_ROCKER3

} ID_CTRL_ROCKER_TYPE;


//! ID table entry control byte information. This information accompanies the ID in each ID entry of the ID table
typedef struct
{
	//! Code of the switch rocker stored 
	ID_CTRL_ROCKER_TYPE u2rocker 	:2 	;
	//! Code of the switch channel associated
	// IO_CHANNEL_TYPE     u2channel   :2 	;
	//! Not used, will be used for keycard detection
	UINT8               u6reserved  :6 	;	
	
	//! Telegram choice of the ID - RORG FUNC TYPE
	CHOICE_TYPE         u8RORG 	    :8;
	UINT8				u8FUNC		:8;
	UINT8				u8TYPE		:8;
		
} ID_ENTRY_CTRL_TYPE; //4 bytes

//! Output information for a search in the ID table performed by #id_search(). 
typedef struct
{
	//! ID table index where the entry was found.
	UINT8               u8index;
	//! Control entry information
	ID_ENTRY_CTRL_TYPE  u32control;	

} ID_SEARCH_OUT_TYPE;
//@}
 
//!brief ID table entry information
typedef struct
{
	//! Telegram type, switch telegram rocker, switch telegram channel 
	ID_ENTRY_CTRL_TYPE  u32control;
	//! Telegram ID bytes 
	uint32              u32Id;

} ID_ENTRY_TYPE; // 8 bytes

//! ID table structure. 
typedef struct
{	
	//! Nr. of stored IDs in the ID table. 
	uint8               u8nrIDs;
	//! BaseID of the Enocean module;
	UINT32				u32BaseID;
	//! ID table is an array of ID entries. 
	ID_ENTRY_TYPE       entry[ID_MAX_NR_ENTRIES]; //30

} ID_TABLE_TYPE;



/***************** FUNCTION PROTOTYPES*****/


//brief Get the number of stored items in the ID table. 
#define  ID_TABLE_NR_ENTRIES(IDtable)    (IDtable.u8nrIDs)
#define  ID_PTABLE_NR_ENTRIES(pIDtable)    (pIDtable->u8nrIDs)

ID_RETURN_TYPE enocean_idAdd(ID_TABLE_TYPE *pIDtable, TEL_RADIO_TYPE* const pRadioTel);
ID_RETURN_TYPE enocean_idSearch(ID_TABLE_TYPE const *pIdTable, TEL_RADIO_TYPE const *pRadioTel, ID_SEARCH_OUT_TYPE *pSearchOut);
ID_RETURN_TYPE enocean_idDeleteAll();

#endif

