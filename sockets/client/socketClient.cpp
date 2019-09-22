#ifndef __SOCKETCLIENT_H__
#include <socketClient.h>
#endif

static void _cdecl readThreadLauncher(void *args)
{
	printf("readThreadLauncher Called.\n");
	((socketClient *)args)->readThread();
}

static void _cdecl sendThreadLauncher(void *args)
{
	printf("sendThreadLauncher Called.\n");
	((socketClient *)args)->sendThread();
}

socketClient::socketClient(){
  log=NULL;
  log= new CDebugLog ("socketClient.log");
  if(!log){
		printf("Fatal cannot create a log file");
		exit(1);
  };//if
  InitializeCriticalSection(&outputListLock);
  InitializeCriticalSection(&inputListLock);
  #if USE_SSL
      /*	  
	  SSLeay_add_ssl_algorithms();
	  meth = SSLv2_client_method();
	  SSL_load_error_strings();
	  ctx = SSL_CTX_new (meth); 
	  */
	  SSLeay_add_ssl_algorithms();
	  SSL_load_error_strings();
      meth=SSLv23_client_method();
      ctx=NULL;
      ctx = SSL_CTX_new (meth);
      if(ctx==NULL){
		  printf("Error SSL_CTX_new.\n");
		  ERR_print_errors_fp(stdout);
		  exit(1);
	  };//if
	// SSL_CTX_set_cipher_list(ctx,"HIGH:MEDIUM:LOW:ADH+3DES:ADH+RC4:ADH+DES:+EXP");
  #endif
	  csocket.conectedFlag =FALSE;
};//constructor

socketClient::~socketClient(){
  //stopThreads();
  cleanOutputList();
  cleanInputList();
  close(sockfd);
  #if USE_SSL
    X509_free(server_cert);
    SSL_shutdown(sslSockfd);
    SSL_free(sslSockfd);
    SSL_CTX_free(ctx);
  #endif

  if(log){
	delete log;
  };//if
};//Destructor

void socketClient::cleanOutputList(){
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

void socketClient::cleanInputList(){
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


void socketClient::startSendThread(){
	int result;
	sendThreadStatus=THREAD_STARTED;
	result = _beginthread(sendThreadLauncher, 0, (void *)this);
};//tartSendThread

void socketClient::sendThread(){
	while((sendThreadStatus==THREAD_STARTED)){
		//printf("This is the send Thread.\n");
		EnterCriticalSection(&outputListLock);
        if(csocket.outputList.size()>0){
			processSocketOutputQueue(&csocket);
		};
        LeaveCriticalSection(&outputListLock);
		Sleep(500);
	};//while
	sendThreadStatus=THREAD_EXIT;
};//sendThread

inline void socketClient::processSocketOutputQueue(CSOCKET *s){
	CPACKET *p;	
	ErrorType err;
	int bytesSent;
	if(s->outputList.size()< MAX_PACKET_TO_PROCESS_OUTPUT_QUEUE){
	  while (s->outputList.size() > 0) {		 
		  p=s->outputList.back();			 
		  #if USE_SSL		     
			 err=SSL_sendSocket(s,p->userData,p->dataLength);		     
		  #else			  
			 err=sendSocket(s,p->userData,p->dataLength);			  
		  #endif
		  if(err==ERR_NONE){
			  s->outputList.pop_back(); //delete the message
		   };//if		  
       };//while
	}else{
	  for(int i=0;i<MAX_PACKET_TO_PROCESS_OUTPUT_QUEUE;i++){
		 p=s->outputList.back();
		 #if USE_SSL		     
			 err=SSL_sendSocket(s,p->userData,p->dataLength);		     
		  #else			  
			 err=sendSocket(s,p->userData,p->dataLength);			  
		  #endif
		  if(err==ERR_NONE){
			  s->outputList.pop_back(); //delete the message
		  };//if		  		  	
       };//for
	};//if(s->outputList.size()< MAX
};//processSocketOutputQueue

#if USE_SSL
inline ErrorType  socketClient::SSL_sendSocket(CSOCKET* s,char *buff,int buffSize){
    int bytes_sent; 
	bytes_sent = SSL_write(s->sslSocket, buff, buffSize);
	if (!s->conectedFlag){
		return ERR_NONE;
	}; 
	
	if (bytes_sent <= 0) {
		int err = SSL_get_error(s->sslSocket, bytes_sent);
		bytes_sent = 0;
		int wsa_err = WSAGetLastError();
		int ssl_err = 0;
		if (err==SSL_ERROR_SSL) {
			ssl_err = ERR_get_error();
		};//if (err==SSL_ERROR_SSL)

		if (err == SSL_ERROR_SYSCALL && wsa_err==WSAEWOULDBLOCK) {
			// The output queue seems full.  This isn't good.
			printf("send() received WSAEWOULDBLOCK... assuming output buffer is full.\n");		  
			return ERR_ERROR;
		};//if (err == SSL_ERROR_SYSCALL &&

		if (   err == SSL_ERROR_NONE
			|| err == SSL_ERROR_WANT_WRITE
			|| err == SSL_ERROR_WANT_READ
			|| err == SSL_ERROR_WANT_X509_LOOKUP
			|| (err == SSL_ERROR_SYSCALL && wsa_err==WSAENOTCONN)
		) {
			// No data sent.  Tell caller we could not send it.  Caller should
			// retry again in 100ms or so.		  
			return ERR_ERROR;
		} else  {
			// Assume connection lost
			printf ("Got SSL_ERROR_* # %d (see openssl/ssl.h). Assuming connection lost.\n", err);		  
			//HandleNotConnErr(WSAENOTCONN);
			s->conectedFlag =FALSE;
			return ERR_ERROR;
		};//if (   err == SSL_ERROR_NONE

	};//if (bytes_sent<= 0)
	

	if (bytes_sent < buffSize) {
		printf("Not all bytes sended (%d vs. %d) during socket send().\n",bytes_sent,buffSize);
		return ERR_ERROR;
	};//if
	printf("I send %d bytes.\n",bytes_sent);
	return ERR_NONE;

};//SSL_sendSocket
#endif

#if !USE_SSL
inline ErrorType  socketClient::sendSocket(CSOCKET* s,char *buff,int buffSize){
    int bytes_sent;
	bytes_sent = send(s->socket , buff, buffSize, 0);
	if (bytes_sent == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err==WSAEWOULDBLOCK) {
			// The output queue seems full.  This isn't good.
			printf("send() received WSAEWOULDBLOCK... assuming output buffer is full.\n");
			return ERR_ERROR;
		};//if
		if (err==WSAENOTCONN ||
			err==WSAECONNRESET ||
			err==WSAECONNABORTED ||
			err==WSAEHOSTUNREACH ||
			err==WSAEHOSTDOWN ||
		  #if !WIN32
			err==EPIPE ||
			err==ETIMEDOUT ||
		  #else
			err==WSAENOTSOCK ||
		  #endif
			err==WSAESHUTDOWN) {
			//HandleNotConnErr(err);	// check if we've lost a connection.
			return ERR_ERROR;
		};//if (err==WSAENOTCONN 
	    printf("SOCKET_ERROR occurred during socket send().  socket=%d, Error=%d. Treating as disconnect..\n",s->socket ,err);
 
		//HandleNotConnErr(err);	// check if we've lost a connection.
		return ERR_ERROR;
	};//if (bytes_sent == SOCKET_ERROR)


	if (bytes_sent < buffSize) {
		printf("Not all bytes sended (%d vs. %d) during socket send().\n",bytes_sent,buffSize);
		return ERR_ERROR;
	};//if
    printf("I send %d bytes.\n",bytes_sent);
	return ERR_NONE;

};//sendSocket
#endif

void socketClient::stopSendThread(){
	sendThreadStatus=THREAD_STOPED;
	while(sendThreadStatus!=THREAD_EXIT){
		printf("waiting for send thread.\n");
		Sleep(1000);
	};
	printf("sendThread Stoped.\n");
};//stopSendThread


void socketClient::startReadThread(){
	int result;
	readThreadStatus=THREAD_STARTED;
	result = _beginthread(readThreadLauncher, 0, (void *)this);
};//startReadThread

void socketClient::readThread(){
	int s;
	//int tmpMaxDescrip;
	ErrorType err;
	struct timeval tv;
	while((readThreadStatus==THREAD_STARTED)){
	    zstruct(tv);
		tv.tv_usec = 125000;	// 125ms timeout
		FD_ZERO(&read_fds); 
        //add socket to the master descriptors set
		FD_SET(csocket.socket, &read_fds); 			
        //whichs sockets have something to read	??       	  
		s=select(csocket.socket+1, &read_fds, NULL, NULL, &tv);
		//printf("Read thread.\n");
		if(s==-1){
		   printf("Error errno=%d",errno);
		   perror("Select");	
		   exit(1);
		};//if
        if(FD_ISSET(csocket.socket,&read_fds)){
			//we have something to read
			//printf("Data arrived.\n");
            #if USE_STRUCT 
			   err=readPacket(&csocket);
            #else
			   processSocket(&csocket);
            #endif
		};//if
	};//while
	readThreadStatus=THREAD_EXIT;
};//readThread


inline void socketClient::processSocket(CSOCKET* s){
	CPACKET *p;
	int bytes;
	char buffer[1024];
	zstruct(buffer);
	ErrorType err;
	#if USE_SSL
	  err=SSL_readSocket(s,buffer,1024,&bytes);	  
	#else
	  err=readSocket(s, buffer,1024, &bytes); 
	#endif
	if(err==ERR_NONE){
		//create a packet
		p=NULL;
		p=new CPACKET;
		if(p){
			//printf("SOCKET %d Told me: %s.\n",s->socket,buffer);
			buffer[bytes+1]=(char)NULL;
			p->writeData(buffer,bytes);
			//add the packet into the incomming packets queue
			s->addPacketInputList(p);
		}else{
			printf("Error socketServer::readSocket cannot create a CPACKET object.\n");
			exit(1);
		};//if(p)
	};//if(err==ERROR_NONE)	
};//readSocket()

#if USE_SSL
inline ErrorType  socketClient::SSL_readSocket(CSOCKET* s,char * buff,int bufSize,int* realBytesRead){
	int received;
	*realBytesRead =0;
    received = SSL_read(s->sslSocket , buff, bufSize);
	if (received <= 0) {
		int err = SSL_get_error(s->sslSocket, received);
		int ssl_err = 0;
		if (err==SSL_ERROR_SSL) {
			ssl_err = ERR_get_error();
		};//if (err==SSL_ERROR_SSL) 

		//kp(("%s(%d) Got SSL_ERROR_* # %d (see openssl/ssl.h)\n", _FL, err));
		received = 0;
		if (   err == SSL_ERROR_NONE
			|| err == SSL_ERROR_WANT_WRITE
			|| err == SSL_ERROR_WANT_READ
			|| err == SSL_ERROR_WANT_X509_LOOKUP
		) {
			// No data received.  This is not an error we care about because
			// our timeout code will handle it eventually.		  
			return ERR_NONE;
		} else  {
			// Assume connection lost.
			printf("Got SSL_ERROR_* # %d (see openssl/ssl.h). Assuming connection lost.\n", err);		  
			//HandleNotConnErr(WSAENOTCONN);	// check if we've lost a connection.		 
			return ERR_NONE;
		};//if (   err == SSL_ERROR_NONE

	};//if (received <= 0)
	*realBytesRead =received ;
	return ERR_NONE;

};//SSL_readSocket
#endif

#if !USE_SSL
inline ErrorType socketClient::readSocket(CSOCKET* s,char * buff,int bufSize, int* realBytesRead ){
    int received;
	*realBytesRead =0;
	
	received = recv(s->socket, buff, bufSize, 0);
	if (received == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK
			  #if !WIN32
				|| err==WSAEAGAIN
			  #endif
				|| err==0) {
			// No data received.  This is not an error we care about.
			return ERR_NONE;
		};//if (err == WSAEWOULDBLOCK

		if (err==WSAENOTCONN ||
			err==WSAECONNRESET ||
			err==WSAECONNABORTED ||
			err==WSAEHOSTUNREACH ||
			err==WSAEHOSTDOWN ||
		  #if !WIN32
			err==EPIPE ||
			err==ETIMEDOUT ||
		  #else
			err==WSAENOTSOCK ||
		  #endif
			err==WSAESHUTDOWN) {
			// Not connected.
			//HandleNotConnErr(err);	// check if we've lost a connection.
			return ERR_NONE;
		};//if (err==WSAENOTCONN

		printf("RawReadBytes() recv()=%d WSAGetLastError() = %d. Treating as disconnect.", received, err);
		//HandleNotConnErr(err);	// assume we've lost a connection.
		return ERR_NONE;
	};//if (received == SOCKET_ERROR)
	
	// We seem to have received some data...
	//
	//if (!connected_flag) {
	//	connected_flag = TRUE;	// we've successfully received something, therefore we must be connected.
	//};//if
    *realBytesRead =received ;
	return ERR_NONE;
};//readSocket
#endif

void socketClient::stopReadThread(){
	readThreadStatus=THREAD_STOPED;
	while(readThreadStatus!=THREAD_EXIT){
		printf("waiting for read thread.\n");
		Sleep(1000);
	};
	printf("read Thread Stoped.\n");
};//stopReadThread

void socketClient::stopThreads(){
	stopReadThread();
	stopSendThread();
};//stopThreads

void socketClient::setRemoteIp(char *ip){
	strcpy(this->remoteIp ,ip);
};//setRemoteIp

void socketClient::setRemotePort(int p){
	this->port =p;
};//setRemotePort

void socketClient::openConn(){
	#define DEST_IP   "192.168.0.2"
    #define DEST_PORT 7775
    char* strCert;
    
	int sockerr = 0;
    int sockSSLerr = 0;

	#ifdef WIN32
      #ifdef MSVCC
	  WORD sockVersion;
	  WSADATA wsaData;
	  int nret;
	  sockVersion = MAKEWORD(1, 1);// We'd like Winsock version 1.1
	   // We begin by initializing Winsock
	  WSAStartup(sockVersion, &wsaData);
      #endif
    #endif 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd==INVALID_SOCKET){
		perror("socket .\n");
		exit(1);
	}else{
		printf("socket %d was created.\n",sockfd);
	};

	#if NON_BLOCKING_SOCKETS 
		#if WIN32
		 dword one = 1;
		 ioctlsocket(sockfd, FIONBIO, &one);// enable non-blocking mode.
	   #else
		 fcntl(sockfd, F_SETFL, O_NONBLOCK);//non blocking socket
	   #endif
    #endif	//non blocking sockets

   
   bool true_bool = true;
   int err = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&true_bool, sizeof(true_bool));
   if (err) {
	   printf("WARNIING setsockopt() to disable nagle failed  WSA error = %d.\n", WSAGetLastError());		
   };//if

	external_addr.sin_family = AF_INET;          // Ordenación de máquina
	external_addr.sin_port = htons(port);   // short, Ordenación de la red
	external_addr.sin_addr.s_addr = inet_addr(remoteIp);
	memset(&(external_addr.sin_zero), '\0', 8);  // Poner a cero el resto de la estructura
    
	
	// no olvides comprobar los errores de connect()!
	sockerr=connect(sockfd, (LPSOCKADDR)&external_addr, sizeof(external_addr));
	
	if (sockerr) {
		sockerr = WSAGetLastError();
		// WSAEWOULDBLOCK is a Normal error condition...
		// the connect() is actually pending.
	/*	
		if (sockerr==WSAENOBUFS) {
			perror("connect() error: no more sockets available.");
			exit(1);
		} */
		
		if (sockerr!=WSAEWOULDBLOCK
		  #if !WIN32
			&& sockerr != EINPROGRESS
			&& sockerr != EAGAIN
		  #endif
			) {
			perror("Error connecting to port");
			shutdown(sockfd, 2);// shut down sending and receiving on the socket
			close(sockfd);
			sockfd = INVALID_SOCKET;
			exit(1);
		}
	};//if (sockerr) 
	
	sockerr=0;
	//create a ssl socket
	#if USE_SSL
	  sslSockfd=NULL;
	  sslSockfd= SSL_new(ctx);
	  
	  if(!sslSockfd){
		  printf("Error cannot create a SSL object.\n");
		  exit(1);
	  };//if
	  BIO* bio;
	  bio=BIO_new_socket(sockfd,BIO_NOCLOSE);
	  BIO_set_nbio(bio,1);
	  SSL_set_bio(sslSockfd,bio,bio);
	     printf("List of ciphers allowed by this client: (in order of preference)\n");
		 log->AddLog("List of ciphers allowed by this client: (in order of preference)\n"); 
		int i = 0;
		const char *c = NULL;
		do {		  
			c = SSL_get_cipher_list((SSL *)sslSockfd, i);
			if (c) {
				printf("Cipher #%2d: %s\n", i+1, c);
				log->AddLog ("Cipher #%2d: %s\n", i+1, c);
			};//if (c)
			i++;
		} while(c);
	  SSL_set_fd(sslSockfd,sockfd);	  

	  sockerr=SSL_createNewConection(sslSockfd);
	  
      if(sockerr!=0){		  
		  checkSSLError(sockerr);
		  //SSL_errordump("SSL_connect.\n")'
		  close (sockfd);
		  SSL_free(sslSockfd);
		  exit(1);
	  };
	  
	  //Display some ssl conections details.
	  printf("SSL conections using %s.\n",SSL_get_cipher(sslSockfd));
      log->AddLog("SSL conections using %s.\n",SSL_get_cipher(sslSockfd));
	  //Get Server certificate
      server_cert=SSL_get_peer_certificate(sslSockfd);
	  if(server_cert==NULL){
		  printf("Error SSL_get_peer_certificate.\n");
          log->AddLog("Error SSL_get_peer_certificate.\n");
		  sockSSLerr = SSL_get_error(sslSockfd, 0);
		  checkSSLError(sockSSLerr);
		  exit(1);
	  };//if(server_cert)

	  printf("Server certificate:\n");
	  log->AddLog("Server certificate:\n");
	  strCert=X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
	  if(strCert==NULL){
		  printf("Error X509_NAME_oneline(X509_get__subject_name(server_cert).\n");
		  log->AddLog("Error X509_NAME_oneline(X509_get__subject_name(server_cert).\n");
		  exit(1);
	  };//
	  printf("\t subject: %s\n",strCert);
	  log->AddLog ("\t subject: %s\n",strCert);
	  OPENSSL_free(strCert);
      strCert = X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
	  if(strCert==NULL){
		  printf("Error X509_NAME_oneline(X509_get__issuer_name(server_cert).\n");
		  log->AddLog ("Error X509_NAME_oneline(X509_get__issuer_name(server_cert).\n");
		  exit(1);
	  };//
	  printf("\t issuer: %s\n",strCert);
	  log->AddLog ("\t issuer: %s\n",strCert);
	   char szConnectionTypeString[255];
	  int bits = 0;
	  int result = SSL_get_cipher_bits(sslSockfd, &bits);
	  printf("######################################\n\n");
	  log->AddLog ("######################################\n\n");
	  printf("Connected using %s (%s, %d/%d-bits)\n",				
						SSL_get_cipher_version(sslSockfd),
						SSL_get_cipher_name(sslSockfd),
						bits, result);
      log->AddLog ("Connected using %s (%s, %d/%d-bits)\n",				
						SSL_get_cipher_version(sslSockfd),
						SSL_get_cipher_name(sslSockfd),
						bits, result);

	  sprintf(szConnectionTypeString, "Connected to the server using encryption.\nAlgorithm: %s (%s, %d-bits)",SSL_get_cipher_version(sslSockfd),SSL_get_cipher_name(sslSockfd),result);
	  printf(szConnectionTypeString);
	  log->AddLog (szConnectionTypeString);
	  printf("\n\n######################################\n\n");
	  log->AddLog ("\n\n######################################\n\n");
	  OPENSSL_free(strCert);

	#endif
	

	
	csocket.socket=sockfd;
	csocket.conectedFlag =TRUE;
	#if USE_SSL  
	  csocket.sslSocket=sslSockfd;
	#endif
	//start I/O thread
	startReadThread();
	Sleep(2000);
	startSendThread();
};//connect

#if USE_SSL 
inline int socketClient::SSL_createNewConection(SSL *ssl){
	int sockSSLerr,sockerr;
	while((sockerr=SSL_connect (sslSockfd))<=0){
	  sockSSLerr = SSL_get_error(sslSockfd, sockerr);
	  if( //sockSSLerr ==SSL_ERROR_WANT_CONNECT ||
		  sockSSLerr == SSL_ERROR_WANT_WRITE  ||
		  sockSSLerr == SSL_ERROR_WANT_READ){
		  Sleep(200);
		  printf("Conecting...\n");
	  }else{
		  checkSSLError(sockSSLerr);
          return sockSSLerr;
	  };//if	
	};//while
	return 0;
};//SSL_createNewConection
#endif

void socketClient::sendPacket(CPACKET *p){
  EnterCriticalSection(&outputListLock);
  csocket.addPacketOutputList(p);
  LeaveCriticalSection(&outputListLock);
};//sendPacket

int socketClient::sendRawData(char *buf, int *len){
	int total = 0;        // cuántos bytes hemos enviado
	int bytesleft = *len; // cuántos se han quedado pendientes
	int n;
	n=0;
	while(total < *len) {
		n = send(csocket.socket, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}

	*len = total; // devuelve aquí la cantidad enviada en realidad
	return n==-1?-1:0; // devuelve -1 si hay fallo, 0 en otro caso
};//sendRawData

int socketClient::sendall(char *buf, int *len){
	int total = 0;        // cuántos bytes hemos enviado
	int bytesleft = *len; // cuántos se han quedado pendientes
	int n;
	n=0;
	while(total < *len) {
		n = send(csocket.socket, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // devuelve aquí la cantidad enviada en realidad
	return n==-1?-1:0; // devuelve -1 si hay fallo, 0 en otro caso
};//sendall



//return a packet form the incomming
//packet list
CPACKET * socketClient::getPacket(){
	CPACKET * tmpPacket;
	tmpPacket=NULL;
	EnterCriticalSection(&inputListLock);
    tmpPacket=csocket.readPacketInputList();
    LeaveCriticalSection(&inputListLock);
	return tmpPacket;
};//getPacket




ErrorType socketClient::readPacket(CSOCKET* s){
	CPACKET *p;
    int bytesRead;
	int bytesRecieved;//real byted extracted from the socket buffer
	
	int bufferSize;//how many bytes we whant to extract
    struct PacketHeader ph; 
	char buffer[MAX_BUFFER_SIZE];
	ErrorType err;

    bytesRecieved=0;
	bufferSize=sizeof( struct PacketHeader);//target
	printf("readPacket OUT\n");
    if(!s->conectedFlag){
       return ERR_NONE;
	};//if
    printf("readPacket IN\n");
    //read the packet header
    do{
       #if USE_SSL
		  err=SSL_readSocket(s,(char*)&ph+bytesRecieved,bufferSize-bytesRecieved,&bytesRead);		
       #else
		  err=readSocket(s,(char*)&ph+bytesRecieved,bufferSize-bytesRecieved,&bytesRead);		  
       #endif

		  if((err==ERR_NONE) && (s->conectedFlag==FALSE) ){
		     return ERR_NONE;
		  };//if

		  if(err!=ERR_NONE){
			  return ERR_ERROR;
		  };
       bytesRecieved=bytesRecieved+bytesRead;
	}while(bufferSize>bytesRecieved);
	printf("Header size:%d\n",bytesRecieved);

    //read the packet data
	bytesRecieved=0;
	bufferSize=ph.packetSize;//target
    //read the packet header
    do{
       #if USE_SSL
		  err=SSL_readSocket(s,buffer+bytesRecieved,bufferSize-bytesRecieved,&bytesRead);		
       #else
		  err=readSocket(s,buffer+bytesRecieved,bufferSize-bytesRecieved,&bytesRead);		  
       #endif
	   if(err!=ERR_NONE){
		  return ERR_ERROR;
	   }; 
       bytesRecieved=bytesRecieved+bytesRead;
	}while(bufferSize>bytesRecieved);
	printf("Data size:%d\n",bytesRecieved);

	//Build a packet
	p=NULL;
	p=new CPACKET;
	if(!p){			
	  printf("Error socketServer::readSocket cannot create a CPACKET object.\n");
	  exit(1);
	};//if(p)
    //put the packet header and data into the packet object
	p->alloc(sizeof(struct PacketHeader)+bufferSize);
	p->writeData((char*)&ph,sizeof( struct PacketHeader));
	p->writeData (buffer,bufferSize);
	//add the packet into the incomming packets queue
	log->AddLog("I build a packet %p with %d bytes\n",p,sizeof(struct PacketHeader)+bufferSize);
	
    log->AddLog("Queue size before: %d ",(int)s->inputList.size() );
	s->addPacketInputList(p);
	log->AddLog ("after: %d \n",(int)s->inputList.size() );
    return ERR_NONE;
};//readPacket


