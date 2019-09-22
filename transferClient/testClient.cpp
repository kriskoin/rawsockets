

#include <criticalSectionsFunctions.h>
#include <threadFunctions.h>
#include <string.h>
#include <signal.h>

#ifndef __UTILSFUNCTIONS_H__
#include <utilsFunctions.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef __SOCKETCLIENT_H__
#include <socketClient.h>
#endif

#ifndef __PACKET_H__
#include <packet.h>
#endif

socketClient *cs;
CRITICAL_SECTION  criticalSection; //mutex to control access to the OutputMessages Queue
bool Terminate = false;
bool Terminate1 = false;
bool Terminate2 = false;
bool Terminate3 = false;
int flag1=0;
int flag2=0;
int flag3=0;
int sockfd;
void OurSignalHandlerRoutine(int signal, struct sigcontext sc)
{
	switch (signal) {
	case SIGTERM:
		Terminate=true;
		printf("SIGTERM proccesed ! .\n");
		break;
	case SIGHUP:

		break;
	case SIGINT:	// usually ctrl+C

		break;
	case SIGPIPE:

		break;
	case SIGQUIT:

		break;
	case SIGBUS:
		exit(10);
		break;
	case SIGFPE:
		 exit(10);
		break;
	case SIGSEGV:
		exit(10);
		break;

	case SIGALRM:

		break;
	default:
		//Error(ERR_ERROR, "%s(%d) Unknown control signal received from OS (%d).",_FL,signal);
		break;
	};//switch
};//OurSignalHandlerRoutine


/*
static void _cdecl MainOutputThreadLauncher(void *args)
{
	printf("MainOutputThreadLauncher Called.\n");
	((CMainOutput *)args)->Execute();
}
*/


void printPacket(CPACKET *p){
	if(p->dataLength){
		//printf("---------------------------------\n");
		//printf("p:%p dataLength:%d.\n",p,p->dataLength);
		printf("%s\n",p->userData);
		//printf("---------------------------------\n");
	};//if
};//printPacket


int sendall(int s, char *buf, int *len){
	int total = 0;        // cuántos bytes hemos enviado
	int bytesleft = *len; // cuántos se han quedado pendientes
	int n;
	n=0;
	while(total < *len) {
		n = send(s, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // devuelve aquí la cantidad enviada en realidad
	return n==-1?-1:0; // devuelve -1 si hay fallo, 0 en otro caso
};//sendall 


/*
//PACKET_TYPE_MSG_TEST
struct testStruct{
	int x;
	int y;
	char texto[50];
};//testStruct

//PACKET_TYPE_MSG_TEST1
struct testStruct1{
	int x;
	int y;
	char texto[50];
	char texto2[150];
};//testStruct

//PACKET_TYPE_MSG_TEST2
struct testStruct2{
	int x[10];
	char texto[50];
};//testStruct

//PACKET_TYPE_MSG_TEST3
struct testStruct3{
	int valor;
	int x[10];
	char texto[50];
};//testStruct

*/

inline int aleatorio(int limit){
   return 1+(rand()%limit);
}; //aleatorio

inline void initStruct(struct testStruct * s){
	s->x =aleatorio(100);
	s->y =aleatorio(50);
	sprintf(s->texto,"Testing %d %d",s->x,s->y); 
};//initStruct

inline void initStruct1(struct testStruct1 * s){
	s->x =aleatorio(100);
	s->y =aleatorio(50);
	sprintf(s->texto,"Testing %d %d",s->x,s->y); 
	sprintf(s->texto2,"Hola a todas Testing %d %d",s->x,s->y);
};//initStruct1


inline void initStruct2(struct testStruct2 * s){
	for(int i=0;i<10;i++){
	   s->x[i] =aleatorio(100);
	}
	strcpy(s->texto,"Testing"); 	
};//initStruct2

inline void initStruct3(struct testStruct3 * s){
	for(int i=0;i<10;i++){
	   s->x[i] =aleatorio(500);
	}
	s->valor =aleatorio(100);
	strcpy(s->texto,"Testing"); 	
};//initStruct3


void hilo1(){
	//PACKET_TYPE_MSG_TEST
    struct testStruct testStruct;
    struct testStruct1 testStruct1;
	struct testStruct2 testStruct2;
	struct testStruct3 testStruct3;
	char texto[127];
	int counter;
	int loop;
	int op;
	counter=1;
	loop=0;
	while((!Terminate1)){	 

		
        #if USE_STRUCT
		    op=aleatorio(5);
			printf("op:%d\n",op);
			if(op==1){
			  zstruct(testStruct);
			  initStruct(&testStruct);
			 
			  cs->csocket.sendDataStructure (&testStruct,sizeof(struct testStruct),PACKET_TYPE_MSG_TEST);
			};//
			if(op==2){
			  zstruct(testStruct1);
			  initStruct1(&testStruct1);
			  cs->csocket.sendDataStructure (&testStruct1,sizeof(struct testStruct1),PACKET_TYPE_MSG_TEST1);
			};//
			if(op==3){
			  zstruct(testStruct2);
			  initStruct2(&testStruct2);
			  cs->csocket.sendDataStructure (&testStruct2,sizeof(struct testStruct2),PACKET_TYPE_MSG_TEST2);
			};//
			if(op==4){			  
			  zstruct(testStruct3);
			  initStruct3(&testStruct3);
			  cs->csocket.sendDataStructure (&testStruct3,sizeof(struct testStruct3),PACKET_TYPE_MSG_TEST3);
			};//
			
        #else
			CPACKET *p;
            sprintf(texto, "Client message %d", counter);
			printf("thread1 Loop (%d) send %s.\n",loop,texto);
			p=NULL;
			p=new CPACKET;			
			if(!p){			
			  printf("Error socketServer::readSocket cannot create a CPACKET object.\n");
			  exit(1);
			};//if(p)
			p->alloc(strlen(texto));
			p->writeData(texto,strlen(texto));
			cs->csocket.addPacketOutputList(p); 
        #endif
		counter++;
		if(counter==32000){
			counter=1;
			loop++;
		};//if  
		
		Sleep(3000);

	};//while
	EnterCriticalSection(&criticalSection);
    printf("thread1 Finished.\n");
    LeaveCriticalSection(&criticalSection);
	flag1=1;
};//hilo1

void processTestPacket(struct testStruct *ts, int size){
	if(sizeof(*ts)!=size){
	};//if
	printf("######TEST PACKET########\n");
	printf("x: %d y: %d\n Texto: %s\n",ts->x,ts->y,ts->texto);
};//processTestPacket

void processPacket(CPACKET *p){
	struct PacketHeader *ph;
	void *data_ptr ;
    ph = (struct PacketHeader*)p->userData;
	data_ptr = (void *)((char *)p->userData+sizeof(*ph));
	switch(ph->packetType){
	    case PACKET_TYPE_MSG_TEST:
			processTestPacket((struct testStruct *)data_ptr,ph->packetSize);
		break;
		default:
			printf ("Error packet type undefined.\n");
		break;
	};//switch
};//process Packet

void hilo2(){
	CPACKET *p;	
	while(!Terminate2){		  
	    printf("Reading thread InputList Size: %d.\n",(int)cs->csocket.inputList.size() );
		//if(cs->csocket.conectedFlag==TRUE){
			p=NULL;
			p=cs->csocket.readPacketInputList(); 
			if(p){
			 printf("Packet Arrived.\n");
			 processPacket(p);
			};
			delete p;
		//};//if
		Sleep(750);
	};//while
	
	flag2=1;
};//hilo2

static void _cdecl MainOutputThread1(void *args)
{
	hilo1();
	
};

static void _cdecl MainOutputThread2(void *args)
{
	hilo2();
	
};


 int main(void){
	 // Install our signal handler for the signals we trap.
	signal(SIGHUP,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGTERM, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGINT,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGSEGV, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGBUS,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGFPE,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGQUIT, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGALRM, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGPIPE, SIG_IGN);	// ignore broken pipe signals (socket disconnects cause SIGPIPE)
	 
	 
	InitializeCriticalSection(&criticalSection);
	
	cs=new socketClient;
	cs->openConn();
	 
	 
	//create a sending thread
	int result ;
	result= _beginthread(MainOutputThread1, 0, (void *)NULL);
	 
	result= _beginthread(MainOutputThread2, 0, (void *)NULL);
	while(!Terminate){		
		//EnterCriticalSection(&criticalSection);
        printf("Main thread.\n");
        //LeaveCriticalSection(&criticalSection);
		Sleep(10000);
	};//while
	cs->stopThreads();
	while(flag1!=1){
		printf("Waiting for the send thread!!!!.\n");
		Sleep(3000);
	};//while
	
	while(flag2!=1){
		printf("Waiting for the recv thread!!!!.\n");
		Sleep(3000);
	};//while 
	delete cs;
	printf("Before close socket.\n");
	return 0;
};//main
