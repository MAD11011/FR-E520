#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>
#include <sec_api/stdio_s.h>
#ifdef __MINGW32__
    #include "commWin32.h"
#else

#endif

#include "MitProtocol.h"

struct ControlCode{
    char STX = 0x02;
    char ETX = 0x03;
    char ENQ = 0x05;
    char ACK = 0x06;
    char LF  = 0x0A;
    char CR  = 0x0D;
    char NAK = 0x15;
}CCode;

int WaitingTime = 1;
char Newline = CCode.CR;

void toASCII(char* dest,int source){

    sprintf(dest,"%X",source);

}

void MChecksum(char* dest,char* source){

    int  checkSum = 0;
    char sCheckSum[4] = {'\0'};
    for(int c=0;c<strlen(source);c++){
        // printf("%d- %c | %02X\n",c,source[c],source[c]);
        checkSum +=  source[c];
    }
    toASCII(sCheckSum,checkSum);
    int bytesToSkip = 0;
    bytesToSkip = (strlen(sCheckSum) > 2) ?  strlen(sCheckSum) - 2 : 0;
    strcpy(dest,sCheckSum+bytesToSkip);
    printf("Sum = %X , \"%s\"\n",checkSum,dest);

}

void DataWrite(const char sNum[3],const char ins[3],const char data[7],DataLength dPacketL){

    char* packetFrame = (char*) calloc(9+dPacketL+1,1);
    char wTime[3]= {'\0'};
    char cSum[3] = {'\0'};
    unsigned long bytesW;
    toASCII(wTime,WaitingTime);
    snprintf(packetFrame,9+dPacketL+1,"%c%s%s%s%s\0",CCode.ENQ
                                                ,sNum
                                                ,ins
                                                ,wTime
                                                ,data);
    //skip first byte
    MChecksum(cSum,packetFrame+1);
    strcat(packetFrame,cSum);
    strncat(packetFrame,&Newline,1);
    SWrite(packetFrame,9+dPacketL+1,&bytesW);
    free(packetFrame);
}


void DataRead(const char sNum[3],const char ins[3]){

    char* packetFrame = (char*) calloc(10,1);
    char wTime[3]= {'\0'};
    char cSum[3] = {'\0'};
    unsigned long bytesW;
    toASCII(wTime,WaitingTime);
    snprintf(packetFrame,9,"%c%s%s%s\0",CCode.ENQ
                                       ,sNum
                                       ,ins
                                       ,wTime);
    //skip first byte
    MChecksum(cSum,packetFrame+1);
    strcat(packetFrame,cSum);
    strncat(packetFrame,&Newline,1);
    SWrite(packetFrame,strlen(packetFrame),&bytesW);
    free(packetFrame);
}

int WriteIReplyData(char* buf,char* error){

    if(buf[0] == CCode.ACK){
        return 0;
    }
    else{
        strncpy(error,&buf[3],1);
        return 1;
    }

}

int ReadIReplyData(char* buf,char* dataBuf,int dataBufSize,char* error){

    if(buf[0] == CCode.NAK){
        strncpy(error,&buf[3],1);
        return 1;
    }
    char* endOfData = strchr(buf,CCode.ETX);
    char* newBuf = (char*) calloc(endOfData-buf+1,1);
    memcpy(newBuf,buf,endOfData-buf);
    char cSum[3] = {'\0'};
    MChecksum(cSum,newBuf+1);
    if(strncmp(cSum,buf+8,2) != 0){
        *error = 2;
        return 1;
    }
    strncpy(dataBuf,buf+3,endOfData-(buf+3));
    return 0;
}

void ReadPReplyData(char Ccode,const char sNum[3]){

    char packetFrame[5] = {'\0'};
    unsigned long bytesW;
    sprintf(packetFrame,"%c%s%c",Ccode,sNum,Newline);
    SWrite(packetFrame,sizeof(packetFrame),&bytesW);
}

void InverterReset(const char sNum[3]){
    DataWrite(sNum,"FD","9696",DATA4BYTES);
}

void ParamaterWrite(const char sNum[3],const char par[3],const char val[5]){
    DataWrite(sNum,par,"9696",DATA4BYTES);
}

void ParameterRead(const char sNum[3],const char par[3]){
    using namespace std::chrono_literals;
    char buffer[32] = {0};
    char dataBuf[16]= {0};
    char error = 0;
    unsigned long bytesRead = 0;
    DataRead(sNum,par);
    // std::this_thread::sleep_for(20ms);
    SAwaitEOF();
    SRead(buffer,sizeof(buffer),&bytesRead);
    for(int i=0;i<bytesRead;i++){
        printf("%d- %02X | %c\n",i,buffer[i],buffer[i]);
    }
    if(ReadIReplyData(buffer,dataBuf,sizeof(dataBuf),&error)){
        if(error != 2){
            fprintf(stderr,"Data read error, Recevied NAK\n");
            return;
        }
        else{
            fprintf(stderr,"Data read error, Recevied mismatched checksum\n");
            return;
        }
    }
    ReadPReplyData(CCode.ACK,sNum);
    printf("DATA RECEVIED :::\n");
    for(int i=0;i<4;i++){
        printf("%d- %02X | %c\n",i,dataBuf[i],dataBuf[i]);
    }


}
