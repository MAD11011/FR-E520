#include <cstdio>
#include <sec_api/stdio_s.h>
#include <windows.h>
#include <winbase.h>
#include <minwindef.h>
#include <winerror.h>


void PrintPortState(DCB dcb);
int GetComList(int* ports,int size,int* numofports);
int OpenPort(int portNum);
int ClosePort();

int SWrite(char* buf,int size,long unsigned int* bytesWritten);
int SRead(char* buf,int size,long unsigned int* bytesRead);
int SAwaitEOF();
