#ifndef __CLIENT_H__
#include <client.h>
#endif



void processTestPacket1(struct testStruct1 *ts, int size){
	printf("###### TEST PACKET 1 ########\n\t PACKET_TYPE_MSG_TEST1 \n");
    printf("\tx:%d\ty:%d\n",ts->x,ts->y );
	printf("Texto: %s\n Texto2: %s\n",ts->texto,ts->texto2);
};//processTestPacket1

void processTestPacket2(struct testStruct2 *ts, int size){
	printf("###### TEST PACKET 2 ########\n\t PACKET_TYPE_MSG_TEST2 \n");
	for(int i=0;i<10;i++){
	  printf("x[%d]:%d\t",i,ts->x[i]);
	  if(i==4){
		  printf("\n");
	  }
	};//for
	printf("\n");
	printf("Texto: %s\n",ts->texto);
};//processTestPacket2	

void processTestPacket3(struct testStruct3 *ts, int size){
	printf("###### TEST PACKET 3 ########\n\t PACKET_TYPE_MSG_TEST3 \n");
	printf("Valor:%d\n",ts->valor);
	for(int i=0;i<10;i++){
	  printf("x[%d]:%d\t",i,ts->x[i]);
	  if(i==4){
		  printf("\n");
	  }
	};//for
	printf("\n");
	printf("Texto: %s\n",ts->texto);
};//processTestPacket3
	
TRANSFERCLIENT::TRANSFERCLIENT(){
  socket=NULL;
  socket=new CSOCKET;
  if(!socket){
	  printf("Printf cannot create a CSOCKET object in TRANFERCLIENT constructor.\n");
	  exit(1);
  };//if
};//constructor

	
TRANSFERCLIENT::TRANSFERCLIENT(socketServer* s){
  server=s;
  socket=NULL;
  socket=new CSOCKET;
  if(!socket){
	  printf("Printf cannot create a CSOCKET object in TRANFERCLIENT constructor.\n");
	  exit(1);
  };//if
};//constructor

TRANSFERCLIENT::~TRANSFERCLIENT(){
	if(socket){
		delete (socket);
	};//if
	
};//destructor

/*
inline void TRANSFERCLIENT::processPacket(CPACKET* p){
   printf(p->userData);
   printf("\n");
   p->cleanData();
   char texto [100];
   sprintf(texto,"Thanks socket # %d i recieved your message.",socket->socket);
   p->writeData(texto,strlen(texto));
   socket->addPacketOutputList(p);
};//processPacket
*/

void TRANSFERCLIENT::processPacket(CPACKET *p){
	struct PacketHeader *ph;
	struct testStruct testStruct;
	void *data_ptr ;
    ph = (struct PacketHeader*)p->userData;
	data_ptr = (void *)((char *)p->userData+sizeof(*ph));
	zstruct(testStruct);
	switch(ph->packetType){
	    case PACKET_TYPE_MSG_TEST:
			processTestPacket((struct testStruct *)data_ptr,ph->packetSize);
			strcpy(testStruct.texto ,"i recieved PACKET_TYPE_MSG_TEST from you.");
		break;
		case PACKET_TYPE_MSG_TEST1:
			processTestPacket1((struct testStruct1 *)data_ptr,ph->packetSize);
			strcpy(testStruct.texto ,"i recieved PACKET_TYPE_MSG_TEST1 from you.");
		break;
		case PACKET_TYPE_MSG_TEST2:
			processTestPacket2((struct testStruct2 *)data_ptr,ph->packetSize);
			strcpy(testStruct.texto ,"i recieved PACKET_TYPE_MSG_TEST2 from you.");
		break;
		case PACKET_TYPE_MSG_TEST3:
			processTestPacket3((struct testStruct3 *)data_ptr,ph->packetSize);
			strcpy(testStruct.texto ,"i recieved PACKET_TYPE_MSG_TEST3 from you.");
		break;
		default:
			printf ("Error packet type undefined.\n");
			strcpy(testStruct.texto ,"i recieved Error packet from you.");
		break;
	};//switch
	if(!socket->ttl){ 
    	socket->sendDataStructure (&testStruct,sizeof(struct testStruct),PACKET_TYPE_MSG_TEST);
	};//if
};//process Packet

void TRANSFERCLIENT::processPacket2(CPACKET *p){
	printf("Client %d Told me: %s\n",socket->socket  ,p->userData);
     
};//process Packet2



void TRANSFERCLIENT::processTestPacket(struct testStruct *ts, int size){
	if(sizeof(*ts)!=size){
	};//if
	printf("######TEST PACKET########\n\tPACKET_TYPE_MSG_TEST\n");
	printf("x: %d y: %d\n Texto: %s\n",ts->x,ts->y,ts->texto);
    //strcpy(ts->texto,"Thanks you. \n" );
	///socket->sendDataStructure (ts,sizeof(struct testStruct),PACKET_TYPE_MSG_TEST);
};//processTestPacket

void TRANSFERCLIENT::process(){
	CSOCKET* s;	
	CPACKET* p;
	p=NULL;
	
	p=socket->readPacketInputList();
	if(p){
		
		#if USE_STRUCT
		  processPacket(p);
        #else
		  processPacket2(p);
        #endif
		
		/*
		for(int i=0;i<(int)socketList.size();i++){
		  s=socketList.at(i);
		 if(s->outputList.size()>0){
			printf("Before processSocketOutputQueue.\n");
			processSocketOutputQueue(s);
			printf("After processSocketOutputQueue. \n"); 
		 }; 
		 socketListIterator++;
		};//for
		*/
	};//if
	delete p;
};//process


inline void TRANSFERCLIENT::sendPacket (CPACKET* p){
    socket->addPacketOutputList (p);
};//sendPacket


