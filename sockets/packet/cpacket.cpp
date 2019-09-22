#ifndef __CPACKET_H__
#include <cpacket.h>
#endif

CPACKET::CPACKET(){
  userData=NULL;
  dataLength=0;
};//Constructor

CPACKET::CPACKET(int size){
  userData=NULL;
  dataLength=0;
  alloc(size);
};//Constructor
CPACKET::~CPACKET(){
  if(dataLength){
	  free (userData);
  };
};//Destructor
void CPACKET::writeData(char *data, int length){
  memmove(userData+dataLength,data,length);
  dataLength=dataLength+length;
};//writeData

//alloc the necesary memory
void CPACKET::alloc(int size){
  userData=(char*)malloc(size);
  if(!userData){
    printf("Error cannot asign memory CPACKET::alloc.\n");
    exit(1);
  };
};//alloc

#if 0

void CPACKET::writeData(char *data, int length){

	if (dataLength==0){
		//create a new buffer
		userData=(char*)malloc(length*sizeof(char));
		if(!userData){
			perror("Error cannot allocate memory with malloc for userData buffer CPACKET::writeData.");
			exit(1);
		};//if
		memcpy(userData,data,length);
		dataLength=length;
	}else{
		//concat new buffer to the old buffer
		char *oldPtr;
		oldPtr=userData;
		//new espace to allocate the old bufer +new buffer
		userData=(char*)realloc(userData,(strlen(userData)*sizeof(char))+(length*sizeof(char)));
		if(!userData){
			perror("Error cannot allocate memory with realloc for userData buffer CPACKET::writeData.");
			exit(1);
		};//if
		strcat(userData,data);
		//check if the new buffer was physical reallocated
		if(userData!=oldPtr){
			free (oldPtr);
			printf("Warning: realloc change the data pointer.\n");
		};
		dataLength=dataLength+length;
	};//if

};//writeData
#endif

void CPACKET::cleanData(){
	if(userData){
		free (userData);
		userData=NULL;
		dataLength=0;
	};
};//cleanData

