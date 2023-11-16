#include "commWin32.h"
#include <cstdio>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <minwindef.h>
#include <winnt.h>

HANDLE CurrentCom;
COMMTIMEOUTS gTimeout;

void PrintPortState(DCB dcb){
    char str[64];
    snprintf(str,64,"BaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n",
            dcb.BaudRate,
            dcb.ByteSize,
            dcb.Parity,
            dcb.StopBits);
    printf("%s",str);
}

int GetComList(int* ports,int size,int* numofports) //added function to find the present serial
{

    TCHAR lpTargetPath[5000]; // buffer to store the path of the COMPORTS
    DWORD test;
    *numofports = 0;
    for(int i=0; i<255; i++) // checking ports from COM0 to COM255
    {
        char comname[8] = {'\0'};
        snprintf(comname,8,"COM%d",i);

        test = QueryDosDevice(comname, (LPSTR)lpTargetPath, 5000);

        if(test!=0) //QueryDosDevice returns zero if it didn't find an object
        {
            if(*numofports > size ){
                fprintf(stderr, "Ports Array size not enough\n");
                return -1;
            }
            ports[*numofports] = (unsigned short int) i;
            (*numofports)++;

        }


    }

    return 1;
}

int OpenPort(int portNum){

    DCB dcb;
    HANDLE hCom;
    BOOL FSuccess;
    char pcCommPort[8];
    snprintf(pcCommPort,8,"COM%d",portNum);
    hCom = CreateFile(pcCommPort,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
    if(hCom == INVALID_HANDLE_VALUE){
        printf ("CreateFile failed with error %d.\n", GetLastError());
        return 1;
    }

    SecureZeroMemory(&dcb,sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    FSuccess = GetCommState(hCom,&dcb);

    if(!FSuccess){
        printf ("GetCommState failed with error %d.\n", GetLastError());
        return (2);
    }

    dcb.BaudRate = CBR_19200;
    dcb.ByteSize = 8;
    dcb.Parity   = 2;
    dcb.StopBits = TWOSTOPBITS;
    dcb.EofChar  = 0x0D;
    dcb.EvtChar  = 0x0D;

    FSuccess = SetCommState(hCom,&dcb);

    if (!FSuccess)
    {
        //  Handle the error.
        printf ("SetCommState failed with error %d.\n", GetLastError());
        return (3);
    }

    //  Get the comm config again.
    FSuccess = GetCommState(hCom, &dcb);

    if (!FSuccess)
    {
        //  Handle the error.
        printf ("GetCommState failed with error %d.\n", GetLastError());
        return (2);
    }

    FSuccess = SetCommMask(hCom,EV_RXFLAG);

    if(!FSuccess){
        printf("SetCommMask failed with error %d.\n", GetLastError());

    }

    gTimeout.ReadIntervalTimeout = 50;
    gTimeout.ReadTotalTimeoutConstant = 50;
    gTimeout.ReadTotalTimeoutMultiplier = 10;
    gTimeout.WriteTotalTimeoutConstant = 50;
    gTimeout.WriteTotalTimeoutMultiplier = 10;

    if (SetCommTimeouts(hCom, &gTimeout) == FALSE)
    {
        printf_s("\nError to Setting Time outs");
    }

    PrintPortState(dcb);
    CurrentCom = hCom;
    return 0;

}

int ClosePort(){
    if(CloseHandle(CurrentCom) == 0){
        fprintf(stderr,"ERROR = %d\n",GetLastError());
        return 0;
    }
    return 1;

}

int SWrite(char* buf,int size,long unsigned int* bytesWritten){

    BOOL FSuccess;
    FSuccess = WriteFile(CurrentCom,buf,size,bytesWritten,NULL);

    if(FSuccess == FALSE){

        printf("Write Failed\n");
        return 1;
    }

    return 0;
}

int SRead(char* buf,int size,long unsigned int* bytesRead){

    BOOL FSuccess;
    FSuccess = ReadFile(CurrentCom,buf,size,bytesRead,NULL);

    if(FSuccess == FALSE){

        printf("Read Failed\n");
        return 1;
    }

    return 0;

}

int SAwaitEOF(){

   DWORD lpEvtMask;

   WaitCommEvent(CurrentCom,&lpEvtMask,NULL);
   if(lpEvtMask != EV_RXFLAG){

    return 0;
   }
    return 1;
}

