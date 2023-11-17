#include <>

int GetComList(int* ports,int size,int* numofports);
int SerialAttach(int portNum);

int SWrite(char* buf,int size,long unsigned int* bytesWritten);
int SRead(char* buf,int size,long unsigned int* bytesRead);
int SAwaitEOF();
