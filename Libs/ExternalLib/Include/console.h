#include "HWlib.h"
#include "TCPlib.h"
#include "taskFlyport.h"
#include "enocean.h"
#include "tools.h"
#include "mem_mod.h"

#ifndef _CONSOLE_H
#define _CONSOLE_H

#define UART_CONSOLE
// #define TCP_CONSOLE

extern char u_cmd[];
extern BOOL cmdNew;
extern BOOL cmdTCP;
extern int len;

// extern ID_TABLE_TYPE IDTable;


#ifdef TCP_CONSOLE
extern TCP_SOCKET tcpSocket;
extern char tcpPort[];
extern BOOL tcpConn;
void CheckTCP();
#endif

void CheckCmds();
void CheckUART1();

void ConsoleWrite(char* txt);

void debug_showIDTable();

#endif //_CONSOLE_H
