/* **************************************************************************																					
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/
 
#ifndef __TOOLS_C_
#define __TOOLS_C_

#include "tools.h"
#include "HWlib.h"

//Display in the UART, the size of the type
void UART_SizeOf()
{
	char buf[8];
	
	UARTWrite(1,"\r\nchar = ");
    sprintf(buf, "%d  ", sizeof(char));
    UARTWrite(1,buf);
	
	UARTWrite(1,"\r\nunsigned char = ");
    sprintf(buf, "%d  ", sizeof(UINT8));
    UARTWrite(1,buf);
	
	UARTWrite(1,"\r\nint = ");
    sprintf(buf, "%d  ", sizeof(int));
    UARTWrite(1,buf);
	
	UARTWrite(1,"\r\nlong = ");
    sprintf(buf, "%d  ", sizeof(long));
    UARTWrite(1,buf);
	
	UARTWrite(1,"\r\nunsigned int = ");
    sprintf(buf, "%d  ", sizeof(UINT));
    UARTWrite(1,buf);
	
	UARTWrite(1,"\r\nunsigned short int = ");
    sprintf(buf, "%d  ", sizeof(UINT16));
    UARTWrite(1,buf);
	
	UARTWrite(1,"\r\n");
}

/**
* This function display an unsigned integer in the UART in hexadecimal
* \param str - String to display in the UART
* \param val - UINT value to display
*/
void UARTDebug(char* str, unsigned int val)
{
	UINT8 buf[4];
	
	UARTWrite(1,str);
	sprintf((char*)buf, "%#x", (int)val);
	//sprintf(buf, "%d  ",  val);
	UARTWrite(1,(char*)buf);
	
}



/**
* This function display an unsigned integer in the UART in hexadecimal
* \param val - UINT value to display in Hexa
*/
void UARTDebugHexa(unsigned int val)
{
	if(val == 0)
	{
		UARTWrite(1,"0x00 ");
	}
	else
	{
		UINT8 buf[4];
		sprintf((char*)buf, "%#x ", val);
		UARTWrite(1,(char*)buf);
	}
}

/**
* This function will send a string and a value in the UART1
*/
void UARTDebug32(char* str, unsigned long int val)
{
	UINT32 buf[4];
	
	UARTWrite(1,str);
	vTaskDelay(2);
	sprintf((char*)buf, " %ld  ",  val);
	UARTWrite(1,(char*)buf);
	
}

void uart_debugHexa(UINT8 val)
{
	char MSbits; //4 most significant bits
	char LSbits; //4 less significant bits
	
	MSbits = (val >> 4) & 0x0F;
	LSbits = val & 0x0F;
	
	if(MSbits >= 0 && MSbits <=9)
	{
		UARTWriteCh(1,MSbits+'0');
	}else if(MSbits>9 && MSbits<0x10)
	{
		UARTWriteCh(1,MSbits+'A'-10);
	}
	
	if(LSbits>= 0 && LSbits <=9)
	{
		UARTWriteCh(1,LSbits+'0');
	}else if(LSbits>9 && LSbits<0x10)
	{
		UARTWriteCh(1,LSbits+'A'-10);
	}
}

void uart_debugUINT32(UINT32 val)
{
	int i=0;
	char uint32Char[4];
	memcpy(uint32Char, &val, 4);
	
	for(i=0;i<4;i++)
		uart_debugHexa(uint32Char[i]);
}

void uart_debugHexa4(UINT8 *val)
{
	int i=0;
	for(i=0;i<4;i++)
		uart_debugHexa(val[i]);
		
}


/*
* Generate a random integer between a min and max value
* @param _iMin: minimum integer value
* @param _iMax: maximum integer value
* @return random integer
*/
int Random (int _iMin, int _iMax)
{
	return (_iMin + (rand () % (_iMax-_iMin+1)));
} 








/***************************************************************************
UINT to CHAR conversion  (ENSTO)
***************************************************************************/
/**********************************************
//change only 1 char into ASCII
**********************************************/
void uint8ToChar1(UINT8 val, char*buffer)
{
	if(val<=9)
	{
		buffer[0]=val+'0';
		buffer[1]='\0';
	}
	return;
}

/**********************************************
Convert an uint8 to a char* in ASCII in Hexa
param:
	- val must be <100 and >=0
	- buffer will be used to store the integer ASCII formated
**********************************************/
void uint8ToASCII(UINT8 val, char *buffer)
{
	char MSbits; //4 most significant bits - block address 0 to 7
	char LSbits; //4 less significant bits - channel address 1 to 4
	
	MSbits = (val >> 4) & 0x0F;
	LSbits = val & 0x0F;
	
	buffer[0] = MSbits +'0';
	buffer[1] = LSbits +'0';
	
	buffer[2] = '\0'; //end of string char
	return;
}		

/******************************
******************************/
void uint8ToCharHexa(UINT8 val, char *buffer)
{
	char MSbits; //4 most significant bits - block address 0 to 7
	char LSbits; //4 less significant bits - channel address 1 to 4
	
	MSbits = (val >> 4) & 0x0F;
	LSbits = val & 0x0F;

	if(MSbits >= 0 && MSbits <=9)
	{
		buffer[0] = MSbits+'0';
	}else if(MSbits>9 && MSbits<0x10)
	{
		buffer[0] = MSbits+ 'A' -10;
	}
	
	if(LSbits>= 0 && LSbits <=9)
	{
		buffer[1] = LSbits+'0';
	}else if(LSbits>9 && LSbits<0x10)
	{
		buffer[1] = LSbits+ 'A'-10;
	}
	
	buffer[2] = '\0'; //end of string char
	return;
}	
	







#endif 
