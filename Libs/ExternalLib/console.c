#include "console.h"


char u_cmd[125];
BOOL cmdNew = FALSE;
int len = 0;

extern SM_STATE_NR_TYPE u8mode;
extern ID_TABLE_TYPE IDTable;

#ifdef TCP_CONSOLE
TCP_SOCKET tcpSocket = INVALID_SOCKET;
BOOL cmdTCP = FALSE;
char tcpPort[] = "50800";
BOOL tcpConn = FALSE;
#endif


void ConsoleWrite(char* txt)
{
	#ifdef UART_CONSOLE
	UARTWrite(1,txt);
	#endif
	
	#ifdef TCP_CONSOLE
	if(tcpConn == TRUE)
	{
		TCPWrite(tcpSocket, txt, strlen(txt));
	}
	#endif
}

#ifdef TCP_CONSOLE
void CheckTCP()
{
	// Check TCP Server connection state
	if(tcpSocket == INVALID_SOCKET)
	{
		tcpSocket = TCPServerOpen(tcpPort);
		IOPut(p19, off);
		cmdTCP = FALSE;
	}
	else
	{
		tcpConn = TCPisConn(tcpSocket);
		IOPut(p19, on);
		cmdTCP = TRUE;
	}
	
	// Check TCPRxLen...
	if(tcpConn == TRUE)
	{
		if(TCPRxLen(tcpSocket) > 2)
		{
			AppDebug("TCP received:");
			len = TCPRxLen(tcpSocket);
			TCPRead(tcpSocket, u_cmd, len);
			u_cmd[len] = '\0';
			AppDebug(u_cmd);
			AppDebug("\r\n");
			cmdNew = TRUE;
		}
	}
}
#endif

void CheckUART1()
{
	// Check UART1 Commands
	int tmpLen = UARTBufferSize(1);
	if(tmpLen > 1)
	{
		vTaskDelay(5);
		len = UARTBufferSize(1);
		UARTRead(1, u_cmd, len);
		u_cmd[len] = '\0';
		cmdNew = TRUE;
		UARTFlush(1);
	}
}

#define COMMAND_NR 15
char* commandList[COMMAND_NR] = {"help","startlearn","stoplearn","ping","readflash","showidtable", "writeflash", "clearidtable", "clearflash", "outon", "outoff", "outstatus", "deleteid", "rpson", "rpsoff"};

void CheckCmds()
{	
	BYTE resultBuff[SIZE_TO_READ];
	int i=0;
	
	ID_ENTRY_TYPE entryType;
	entryType.u32Id = 0xFFFFFFFF;
	uint8 tmp;
	
	if(cmdNew)
	{
		// Clear the Flag
		cmdNew = FALSE;
		
		//**********************************************
		//	Command help
		if(strstr(u_cmd, commandList[0])!=NULL)
		{
			ConsoleWrite("#help:\r\n");
			for(i=0;i<COMMAND_NR;i++)
			{
				ConsoleWrite("- ");
				ConsoleWrite(commandList[i]);
				ConsoleWrite("\r\n");
			}
		}
		
		
		//**********************************************
		//	Command startlearn
		else if(strstr(u_cmd, commandList[1])!=NULL)
		{
			ConsoleWrite("#startlearn\r\n");
			u8mode = SM_LEARN;
		}
		
		//**********************************************
		// Command stoplearn
		else if(strstr(u_cmd, commandList[2])!=NULL)
		{
			ConsoleWrite("#stoplearn\r\n");
			u8mode = SM_OPERATION;
		}
		
		//**********************************************
		// Command devicelist
		else if(strstr(u_cmd, commandList[3])!=NULL)
		{

			ConsoleWrite("#pong\r\n");
		}
		
		//**********************************************
		// Command readflash
		else if(strstr(u_cmd, commandList[4])!=NULL)
		{

			ConsoleWrite("#readflash\r\n");
			mem_readFlash(MEMORY_ADDRESS_IDTABLE, resultBuff);

			for(i=0;i<SIZE_TO_READ;i++)
				UARTDebugHexa(resultBuff[i]);			
			
			if(resultBuff[0]>ID_MAX_NR_ENTRIES) resultBuff[0] = 0;
			
			memcpy(&(IDTable),resultBuff,sizeof(IDTable));
		}
		
		//**********************************************
		// Command showidtable
		else if(strstr(u_cmd, commandList[5])!=NULL)
		{

			ConsoleWrite("#showidtable\r\n");
			debug_showIDTable();
		}
		
		//**********************************************
		// Command writeflash
		else if(strstr(u_cmd, commandList[6])!=NULL)
		{

			ConsoleWrite("#writeflash\r\n");
			ConsoleWrite("size: ");
			uart_debugHexa((sizeof(IDTable) - (30*sizeof(ID_ENTRY_TYPE)-(sizeof(ID_ENTRY_TYPE)*IDTable.u8nrIDs))));
			ConsoleWrite("\r\n");
			// if(IDTable.u8nrIDs == 0)
			// {
				// mem_writeFlash(MEMORY_ADDRESS_IDTABLE,(BYTE*)&IDTable, 5); //u8nrIDs+BaseId = 5
			// }else
				mem_writeFlash(MEMORY_ADDRESS_IDTABLE,(BYTE*)&IDTable, (sizeof(IDTable) - (30*sizeof(ID_ENTRY_TYPE)-(sizeof(ID_ENTRY_TYPE)*IDTable.u8nrIDs))));
		}
		
		//**********************************************
		// Command clearidtable
		else if(strstr(u_cmd, commandList[7])!=NULL)
		{

			ConsoleWrite("#clearidtable\r\n");
			enocean_idDeleteAll();
		}
		
		//**********************************************
		// Command clearflash
		// erase all the sector of the flash (all bytes set to 0xFF
		else if(strstr(u_cmd, commandList[8])!=NULL)
		{

			ConsoleWrite("#clearflash\r\n");
			mem_clearFlash(MEMORY_ADDRESS_IDTABLE);
		}
		
		//**********************************************
		// Command outon pour la gigogne
		// @param index de l'ID à utiliser (de 0 à 9), 9 pour broadcast
		else if(strstr(u_cmd, commandList[9])!=NULL)
		{
			ConsoleWrite("#");
			ConsoleWrite(u_cmd);
			ConsoleWrite("\r\n");
			
			tmp = u_cmd[5]-'0';
			if(tmp>9) tmp=9;
			if(tmp!=9)
			{
				ConsoleWrite("-ID:");
				memcpy(&(entryType.u32Id), &(IDTable.entry[tmp].u32Id), 4);
				uart_debugUINT32(entryType.u32Id);
				ConsoleWrite("\r\n");
			}
			vld_sendCMD01(&entryType, 1);
		}
		//**********************************************
		// Command outoff pour la gigogne
		// @param index de l'ID à utiliser (de 0 à 9), 9 pour broadcast
		else if(strstr(u_cmd, commandList[10])!=NULL)
		{
			ConsoleWrite("#");
			ConsoleWrite(u_cmd);
			ConsoleWrite("\r\n");

			tmp = u_cmd[6]-'0';
			if(tmp>9) tmp=9;
			if(tmp!=9)
			{
				ConsoleWrite("-ID:");
				memcpy(&(entryType.u32Id), &(IDTable.entry[tmp].u32Id), 4);
				uart_debugUINT32(entryType.u32Id);
				ConsoleWrite("\r\n");
			}
			vld_sendCMD01(&entryType, 0);
		}
		
		//**********************************************
		// Command outstatus pour la gigogne
		// @param 1 - index de l'ID à utiliser (de 0 à 9), 9 pour broadcast 
		else if(strstr(u_cmd, commandList[11])!=NULL)
		{
			ConsoleWrite("#");
			ConsoleWrite(u_cmd);
			ConsoleWrite("\r\n");
			
			tmp = u_cmd[9]-'0';
			if(tmp>9) tmp=9;
			if(tmp!=9)
			{
				ConsoleWrite("-ID:");
				memcpy(&(entryType.u32Id), &(IDTable.entry[tmp].u32Id), 4);
				uart_debugUINT32(entryType.u32Id);
				ConsoleWrite("\r\n");
			}
			vld_sendCMD03(&entryType, 0); //get output status for output 0 (default output for the smart plug)
		}
		
		//**********************************************
		// deleteid - delete 1 ID in the IDTable
		// @param 1 - index de l'ID à supprimer (de 0 à 9), 9 pour broadcast 
		else if(strstr(u_cmd, commandList[12])!=NULL)
		{
			ConsoleWrite("#");
			ConsoleWrite(u_cmd);
			ConsoleWrite("\r\n");
			
			tmp = u_cmd[8]-'0';
			if(tmp>9) tmp=9;
			if(tmp!=9)
			{
				ConsoleWrite("-ID:");
				memcpy(&(entryType.u32Id), &(IDTable.entry[tmp].u32Id), 4);
				uart_debugUINT32(entryType.u32Id);
				ConsoleWrite("\r\n");
			}
			enocean_idDelete(&IDTable, tmp);
		}
		
		
		//**********************************************
		// rpson 
		else if(strstr(u_cmd, commandList[13])!=NULL)
		{
			ConsoleWrite("#");
			ConsoleWrite(u_cmd);
			ConsoleWrite("\r\n");
			
			enocean_sendRPS(0, 0x10, 1);
			// vTaskDelay(200);
			// enocean_sendRPS(0, 0x30, 1);
			
			// vTaskDelay(200);
			// enocean_sendRPS(0, 0x50, 0);
			// vTaskDelay(200);
			// enocean_sendRPS(0, 0x70, 0);
		}
		//**********************************************
		// rpsoff
		else if(strstr(u_cmd, commandList[14])!=NULL)
		{
			ConsoleWrite("#");
			ConsoleWrite(u_cmd);
			ConsoleWrite("\r\n");
			
	
			enocean_sendRPS(0, 0x30, 1);
			
		}
		
		
		//**********************************************
		// unknown command
		else
		{
			ConsoleWrite("#Unkown command: ");
			ConsoleWrite(u_cmd);
			ConsoleWrite("\r\n");
		}
		
	}
}



/*
Send IDTable content to UART:

"Enter LRN mode<CR><LF>
IDTable:<CR><LF>
nrEntries: 1<CR><LF>
--ID 0: 001F5053 ctrl: 0 0 1, F6<CR><LF>"
				ID		 ctlr: u2rocker, u2channel, u4reserved, u8type
*/
void debug_showIDTable()
{
	// Nr. of ID table entries stored currently
	UINT8 u8nrEntries;
	// ID table entry index
	UINT8 u8index;

	u8nrEntries = 0;
	u8index = 0;

	// Get the nr. of IDs stored in the table
	u8nrEntries = ID_TABLE_NR_ENTRIES(IDTable);
	if(u8nrEntries >ID_MAX_NR_ENTRIES )
		u8nrEntries = 0;
		
	ConsoleWrite("Size of IDTable: ");
	uart_debugHexa(sizeof(IDTable));
	ConsoleWrite("\r\n");
	
	if(u8nrEntries!=0)
	{
		ConsoleWrite("IDTable:\r\n");
		ConsoleWrite("nrEntries: ");
		uart_debugHexa(u8nrEntries);
		ConsoleWrite("\r\n");
	
		for(u8index=0;u8index<u8nrEntries;u8index++)
		{
			ConsoleWrite("--ID ");
			uart_debugHexa(u8index);
			ConsoleWrite(": ");
			uart_debugUINT32(IDTable.entry[u8index].u32Id);
			ConsoleWrite(", u2rocker: ");
			uart_debugHexa(IDTable.entry[u8index].u32control.u2rocker);
			// ConsoleWrite(", u2channel:");
			// uart_debugHexa(IDtable.entry[u8index].u32control.u2channel);
			ConsoleWrite(", u6reserved:");
			uart_debugHexa(IDTable.entry[u8index].u32control.u6reserved);
			ConsoleWrite(", EEP:");
			uart_debugHexa(IDTable.entry[u8index].u32control.u8RORG);
			ConsoleWrite("-");
			uart_debugHexa(IDTable.entry[u8index].u32control.u8FUNC);
			ConsoleWrite("-");
			uart_debugHexa(IDTable.entry[u8index].u32control.u8TYPE);
			ConsoleWrite("\r\n");
		}
	}else
	{
		ConsoleWrite("ID Table empty\r\n");
	}
	
}

