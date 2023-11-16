#include <cstring>
#include <stdio.h>
#ifdef __MINGW32__
    #include "commWin32.h"
#else
    #include "commWin32.h"
#endif
#include "MitProtocol.h"

void Arduinotest();

int main (int argc, char *argv[]) {
    int numofports = 0;
    int ports[32] = {0};
    GetComList(ports,32,&numofports);
    for(int p = 0;p<numofports;p++){
        printf("%d> COM%d\n",p+1,ports[p]);

    }
    printf("Input Port\n");
    bool valid = false;
    int portNum = -1;
    while(!valid){
        char strinput[3];
        int input;
        fgets(strinput,3,stdin);
        sscanf(strinput,"%d",&input);
        for(int p=0;p<numofports;p++){
            if(input == ports[p]){
                portNum = input;
                valid = true;
                break;
            }
        }
    }
    printf("Openning Port COM%d\n",portNum);
    if(OpenPort(portNum)){
        printf("Error Opening Port\n");
    }
    else{
        printf("Success\n");
        // InverterReset("00");
        ParamaterWrite("00","83","1D4C");
        ParameterRead("00","03");
    }
    ClosePort();

    return 0;
}

void Arduinotest(){
    char buffer[64];
    long unsigned int bytesRead;
    long unsigned int bytesWritten;

    strcpy(buffer,"HELLO\r");

    printf("Starting Ardunio test\n");
    SWrite(buffer,sizeof(buffer),&bytesWritten);
    printf("bytes written %d\n",bytesWritten);
    SAwaitEOF();
    SRead(buffer,sizeof(buffer),&bytesRead);
    *strchr(buffer,'\r') = '\0';
    printf("Arduino responded with \"%s\"\n",buffer);

}

