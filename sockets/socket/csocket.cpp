#ifndef __SOCKET_H__
#include <csocket.h>
#endif


//EnterCriticalSection(&outputListLock);
//LeaveCriticalSection(&outputListLock);

//EnterCriticalSection(&inputListLock);
//LeaveCriticalSection(&inputListLock);


CSOCKET::CSOCKET(){
	socket=INVALID_SOCKET;
	InitializeCriticalSection(&outputListLock);
	InitializeCriticalSection(&inputListLock);
	ttl=0;
	conectedFlag = FALSE;
};//constructor

 CSOCKET::~CSOCKET(){
  //close the socket
  cleanOutputList();
  cleanInputList();
  if(socket!=INVALID_SOCKET){
	 close(socket);
  };//if
  #if USE_SSL	   
    if(sslSocket){
	  delete sslSocket;
	};//if
  #endif

 };//destructor


//add a packet into the inputList
//incomming packets
void CSOCKET::addPacketInputList(CPACKET *p){
  EnterCriticalSection(&inputListLock);
  inputList.push_front(p);
  LeaveCriticalSection(&inputListLock);
};//addPacketInputList

//read a packet from the inputList
CPACKET * CSOCKET:: readPacketInputList(){
  CPACKET* p;	
  EnterCriticalSection(&inputListLock);
  if(inputList.size()>0){
	  p=inputList.back();
	  inputList.pop_back();		  	 
  }else{
	  p=NULL;
  };//if
  LeaveCriticalSection(&inputListLock);
  return p;
};//readPacketInputLis

//add a packet into the outputList
//outcomming packets
void CSOCKET::addPacketOutputList(CPACKET *p){
  EnterCriticalSection(&outputListLock);	
  outputList.push_front(p);
  LeaveCriticalSection(&outputListLock);
};//addPacketOutputList

//read a packet from the outputList
CPACKET * CSOCKET::readPacketOutputList(){
  CPACKET* p;
  EnterCriticalSection(&outputListLock);
  if(outputList.size()>0){
	  p=outputList.back();
	  outputList.pop_back();		  	 
  }else{
	  p=NULL;
  };//if
  LeaveCriticalSection(&outputListLock);
  return p;	
};//readPacketOutputLis

void CSOCKET::cleanOutputList(){
	CPACKET* p;
	int i;
	i=0;
	while(outputList.size()>0){
	  p=outputList.back();
	  outputList.pop_back();
	  delete p;
	  i++;
	};//while
	if(i){
	  printf("Warning cleanOutputList had %d packets.\n",i);
	};
};//cleanOutputList

void CSOCKET::cleanInputList(){
	CPACKET* p;
	int i;
	i=0;
	while(inputList.size()>0){
	  p=inputList.back();
	  inputList.pop_back();
	  delete p;
	  i++;
	};//while
	if(i){
	  printf("Warning cleanInputList had %d packets.\n",i);
	};
};//cleanInputList


void CSOCKET::sendDataStructure(void* structPtr,int structSize,int packetType){
	CPACKET *p;
	struct PacketHeader ph;
	//Build a packet
	p=NULL;
	p=new CPACKET;
	if(!p){			
	  printf("Error socketServer::readSocket cannot create a CPACKET object.\n");
	  exit(1);
	};//if(p)
	//buil packet header
	ph.packetType=packetType;
	ph.packetSize=structSize;
    //put the packet header and data into the packet object
	p->alloc(structSize+sizeof( struct PacketHeader));
	p->writeData((char*)&ph,sizeof( struct PacketHeader));
	p->writeData ((char*)structPtr,structSize);
	//add the packet into the outgoingpackets queue
	addPacketOutputList(p);
	//s-
};//sendDataStructure

