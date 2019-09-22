#ifndef __SOCKETSERVER_H__
#define __SOCKETSERVER_H__

#include <csocket.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/tcp.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <fcntl.h>
#endif


#ifdef WIN32
  #ifdef MSVCC
    #include <process.h>
    #include <windows.h>
  #endif
#endif

#ifndef WIN32
 #include <list.h>
 #include <deque.h>
#else
 #ifdef MSVCC
  #include <io.h> 
  #include <deque>
  #include <list>
  using namespace std;
 #endif
#endif

#ifndef _logH_INCLUDED
#include "log.h"
#endif

#ifndef __UTILSFUNCTIONS_H__
#include <utilsFunctions.h>
#endif

#ifndef __SOCKETSDEFINITIONS_H__
#include <socketsDefinitions.h>
#endif

#if USE_SSL
	#include <openssl/rsa.h>       /* SSLeay stuff */
	#include <openssl/crypto.h>
	#include <openssl/x509.h>
	#include <openssl/pem.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
    //Original cert and key
    //#define CERTF "/socketTest/testings/sslSockets/cert/pokersrv.cert"
    //#define KEYF  "/socketTest/testings/sslSockets/cert/pokersrv.key"
    #define CERTF "/socketTest/testings/sslSockets/cert/socketServer.cert"
    #define KEYF  "/socketTest/testings/sslSockets/cert/socketServer.key"
    //#define CERTF "/socketTest/testings/sslSockets/cert/socketServer4096.cert"
    //#define KEYF  "/socketTest/testings/sslSockets/cert/socketServer4096.key"
	//#define CERTF "/socketTest/testings/sslSockets/cert/server-req.pem"
    //#define KEYF  "/socketTest/testings/sslSockets/cert/server-key.pem"
#endif

#ifndef __THREADFUNCTIONS_H__
#include <threadFunctions.h>
#endif

#ifndef __CLIENT_H__
#include <client.h>
#endif

#ifndef __CRITICALSECTIONSFUNCTIONS_H__
#include <criticalSectionsFunctions.h>
#endif

#define MAX_PACKET_TO_PROCESS_OUTPUT_QUEUE 5


typedef deque <CSOCKET *, allocator<CSOCKET *> > SOCKETLIST;
typedef SOCKETLIST::iterator SOCKETLISTITERATOR;//for iteract in to the socketlist



typedef deque <TRANSFERCLIENT *, allocator<TRANSFERCLIENT *> > CLIENTLIST;
typedef CLIENTLIST::iterator CLIENTLISTITERATOR;//for iteract in to the clientlist

#define SSL_ACCEPT_DELAY 30000
#define SOCKET_TTL 15000 //15seg before close an inactive socket

struct broadcastPacket{
	CPACKET *packet;
	CSOCKET *source;
};//broadcastPacket

typedef deque <broadcastPacket *, allocator<broadcastPacket *> > BROADCASTLIST;

typedef WORD32 IPADDRESS;	
class socketServer{
	public:
		//public variables
	    CDebugLog *log;
		CDebugLog *sockLog;
	    SOCKET_DESCRIPTOR sockfd;
        SOCKADDR_IN my_addr;
		int mainThreadStatus;
		int readThreadStatus;
	    
	    int conections;//Num active conections
	
	    
	
		//public functions
		socketServer();		
	    ~socketServer();
		ErrorType createListenSocket(int portNumber);
		void closeSocket();
	    
	  
	    void mainThread();
		void readThread();
		
		ErrorType startService(int port);
		void stopService();
		void stopReadThread();
		void pauseService();
		
	
		
	    void addClient(TRANSFERCLIENT *client);
		void addSocket(CSOCKET* sock);
		void removeSocket(CSOCKET* sock);

		
		
		
		void sendBuffer(char* b,int buffSize,SOCKET_DESCRIPTOR source);//test
		
	private:
		//private variables
	
		#if USE_SSL
	      SSL* sslSockfd;
	      SSL_CTX* ctx;
	      X509* server_cert;
	      SSL_METHOD* meth;
	    #endif
	    
	    int maxDescriptor;//maximo descriptor para la funcion select
	    fd_set master;   // conjunto maestro de descriptores de fichero
        fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
		BROADCASTLIST broadcastList;
		SOCKETLIST socketList;
		CLIENTLIST clientList;
	    SOCKETLISTITERATOR socketListIterator;
		CLIENTLISTITERATOR clientListIterator;
		CRITICAL_SECTION  broadcastLock;
		CRITICAL_SECTION  clientsLock;
		CRITICAL_SECTION  socketsLock;
	    CRITICAL_SECTION  descriptorsLock;
	
	    //private methods

		//main thread
		ErrorType startMainThread();
		void startReadThread();
		
		void stopMainThread();

		void processClientList();
	    
	    inline void processSocket(CSOCKET* s);
		inline void processSocket2(CSOCKET* s);
		//I/O
	    void processSocketsOutputList();
		void processSocketInputList();
		void processBroadcastMessages();
		void addBroadcastMessage(CPACKET *packet,CSOCKET *source);
		//I
		inline void processSocketsInputRequest(fd_set *read_fds);
		//O
		inline void processSocketOutputQueue(CSOCKET *s);

		//new conections
		ErrorType acceptConnection(SOCKET_DESCRIPTOR sockServer);

	    #if USE_SSL
		 inline ErrorType  SSL_readSocket(CSOCKET* s,char * buff,int bufSize,int* realBytesRead);
		 inline ErrorType SSL_sendSocket(SSL *ssl,char *buff,int buffSize);
        #else
		 inline ErrorType readSocket(CSOCKET* s,char * buff,int bufSize,int* realBytesRead);
		 inline ErrorType sendSocket(SOCKET_DESCRIPTOR sock,char *buff,int buffSize);
        #endif

		void shutDownClient(TRANSFERCLIENT *client);
		void removeClient(TRANSFERCLIENT *client);

		void handleLostConecction(CSOCKET* s);
			    		    
	    void cleanSocketList();
		void cleanClientList();
		ErrorType readPacket(CSOCKET* s);
	    
	    
};//socketServer

#endif
