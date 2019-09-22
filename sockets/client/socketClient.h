#ifndef __SOCKETCLIENT_H__
#define __SOCKETCLIENT_H__

/*
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
*/
#include <csocket.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <list.h>
	#include <deque.h>
    #include <netinet/tcp.h>
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
#endif



#ifndef __THREADFUNCTIONS_H__
#include <threadFunctions.h>
#endif


#ifndef __CRITICALSECTIONSFUNCTIONS_H__
#include <criticalSectionsFunctions.h>
#endif

#ifndef __PACKET_H__
#include <packet.h>
#endif

#define MAX_PACKET_TO_PROCESS_OUTPUT_QUEUE 3


typedef deque <CSOCKET *, allocator<CSOCKET *> > SOCKETLIST;
typedef SOCKETLIST::iterator SOCKETLISTITERATOR;//for iteract in to the socketlist
typedef deque<CPACKET *, allocator<CPACKET *> > PACKETLIST;

#define SSL_CONNECT_DELAY 300

class socketClient{
	public:
		//public variables
	    CSOCKET csocket;
	    SOCKET_DESCRIPTOR sockfd;
        SOCKADDR_IN my_addr;
	    SOCKADDR_IN external_addr;
	    #if USE_SSL
	      SSL* sslSockfd;
	      SSL_CTX* ctx;
	      X509* server_cert;
	      SSL_METHOD* meth;
	    #endif
	
	    int readThreadStatus;
	    int sendThreadStatus;
	    char remoteIp[25];
		int port;
	    //public methods
	    socketClient();
		~socketClient();
	    
	    void openConn();
		void setRemoteIp(char* ip);
		void setRemotePort(int p);
		
	    //send thread
	    void startSendThread();
		void sendThread();
		void stopSendThread();
	
	    //read thread
		void startReadThread();
		void readThread();
		void stopReadThread();
		
		void sendPacket(CPACKET *p);
		
		CPACKET * getPacket();

		void stopThreads();
	    
	protected:
		//proctected variables
	    CDebugLog *log;
	
	    //protected methods
	    inline void processSocketOutputQueue(CSOCKET *s);
	    int sendRawData(char *buf, int *len);
	    int sendall(char *buf, int *len);
	    ErrorType readPacket(CSOCKET* s);
	private:
		
		//private variables	    
	    fd_set master;   // conjunto maestro de descriptores de fichero
        fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
		SOCKETLIST socketList;
	    SOCKETLISTITERATOR socketListIterator;
		PACKETLIST outputList;
        PACKETLIST inputList;	
        CRITICAL_SECTION  outputListLock;	  
	    CRITICAL_SECTION  inputListLock;
	
	    //private methods
		#if USE_SSL 
		  inline int SSL_createNewConection(SSL *ssl);		
		  inline ErrorType SSL_sendSocket(CSOCKET* s,char *buff,int buffSize);
		  inline ErrorType  SSL_readSocket(CSOCKET* s,char * buff,int bufSize,int* realBytesRead);
        #else
           inline ErrorType sendSocket(CSOCKET* s,char *buff,int buffSize);
		   inline ErrorType readSocket(CSOCKET* s,char *buff,int bufSize,int* realBytesRead);
		#endif		 
		 
	    void cleanInputList();
	    void cleanOutputList();
	    
	    inline void processSocket(CSOCKET* s);
	
};//socketClient

#endif


