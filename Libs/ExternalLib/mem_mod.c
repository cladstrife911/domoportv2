#include "mem_mod.h"


BOOL mem_readFlash(UINT32 addressToRead, BYTE resultBuff[SIZE_TO_READ])
{
	// int i=0;
	
	BYTE bResult[SIZE_TO_READ];
	
	ConsoleWrite("ENTER mem_readFlash()r\n");

	SPIFlashReadArray(addressToRead, bResult, SIZE_TO_READ);
	
	// UARTWrite(1,"\r\n");
	// for(i=0;i<SIZE_TO_READ;i++)
		// UARTDebugHexa(bResult[i]);
	// UARTWrite(1,"\r\n");
	memcpy(resultBuff, bResult, SIZE_TO_READ);
	
	ConsoleWrite("EXIT mem_readFlash()\r\n");	

	return TRUE;
}

BOOL mem_writeFlash(DWORD dwAddr, BYTE* vData, WORD wLen)
{
	ConsoleWrite("ENTER mem_writeFlash()r\n");

	SPIFlashBeginWrite(dwAddr);
	SPIFlashWriteArray(vData, wLen);
	
	ConsoleWrite("EXIT mem_writeFlash()r\n");
	
	return TRUE;
}


void mem_DEBUG()
{
	UINT32 addressToRead=0;
	BYTE resultBuff[SIZE_TO_READ];
	BYTE writeBuff[SIZE_TO_READ] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A};
	
	
	ConsoleWrite("ENTER DEBUG()\r\n");
	vTaskDelay(10);
	mem_readFlash(addressToRead, resultBuff);
	vTaskDelay(100);
	mem_writeFlash((DWORD)addressToRead, writeBuff, SIZE_TO_READ);
	vTaskDelay(100);
	mem_readFlash(addressToRead, resultBuff);
	vTaskDelay(10);
	ConsoleWrite("EXIT DEBUG()\r\n");
}
