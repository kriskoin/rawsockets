#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifndef WIN32
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
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

#ifndef __CPACKET_H__
#include <cpacket.h>
#endif


#ifndef __CRITICALSECTIONSFUNCTIONS_H__
#include <criticalSectionsFunctions.h>
#endif

#ifndef WIN32
 #include <deque.h>
#else
 #ifdef MSVCC
  #include <io.h> 
  #include <deque>
  using namespace std;
 #endif
#endif

typedef deque<CPACKET *, allocator<CPACKET *> > PACKETLIST;

class CSOCKET {
	  public:
	   //public variables
	 
	  int conectedFlag;	
	  unsigned int ttl;//time to live
	  SOCKET_DESCRIPTOR socket;//file descriptor of the socket
	  #if USE_SSL
	   SSL* sslSocket; //secure socket
	  #endif
	  struct sockaddr_in address;

	  PACKETLIST outputList;
      PACKETLIST inputList;
	  //public methods
	  CSOCKET();
	  ~CSOCKET();
	  
	  //add a packet into the inputList
	  //incomming packets
	  void addPacketInputList(CPACKET *p);
	  
	  //read a packet from the inputList
	  CPACKET * readPacketInputList();
	  
	   //add a packet into the outputList
	   //outcomming packets
	  void addPacketOutputList(CPACKET *p);
	  
	  //read a packet from the outputList
	  CPACKET * readPacketOutputList();

	  void sendDataStructure(void* structPtr,int structSize,int packetType);
	  
	  private:
	  
	  //private methods
	  void cleanOutputList();
	  void cleanInputList();
	  	
      CRITICAL_SECTION  outputListLock;	  
	  CRITICAL_SECTION  inputListLock;
	};

#endif
