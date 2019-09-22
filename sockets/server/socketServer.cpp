#ifndef __SOCKETSERVER_H__
#include <socketServer.h>
#endif




//EnterCriticalSection(&socketsLock);
//LeaveCriticalSection(&socketsLock);


void IP_ConvertIPtoString(IPADDRESS ip_address, char *dest_str, int dest_str_len){
  char str[20];
  sprintf(str, "%d.%d.%d.%d", (ip_address) & 255,(ip_address>>8) & 255,(ip_address>>16) & 255,(ip_address>>24) & 255);
  strnncpy(dest_str, str, dest_str_len);
};//IP_ConvertIPtoString


static void _cdecl mainThreadLauncher(void *args){
	printf("listenThreadLauncher Called.\n");
	((socketServer *)args)->mainThread();
};//mainThreadLauncher



static void _cdecl readThreadLauncher(void *args){
	printf("Read ThreadLauncher Called.\n");
	((socketServer *)args)->readThread();
};//readThreadLauncher


#if USE_SSL


char* checkSSLErrorXXX(int errorNum){
	char errName[25];
	char strTmp[255];
	switch (errorNum){
		case SSL_ERROR_NONE:
			strcpy(errName,"SSL_ERROR_NONE");
			break;
		case SSL_ERROR_ZERO_RETURN:
			strcpy(errName,"SSL_ERROR_ZERO_RETURN");
			break;
		
		case SSL_ERROR_WANT_READ:
			strcpy(errName,"SSL_ERROR_WANT_READ");
			break;
		
		case SSL_ERROR_WANT_WRITE:
			strcpy(errName,"SSL_ERROR_WANT_WRITE");
			break;
		
		case SSL_ERROR_WANT_CONNECT:
			strcpy(errName,"SSL_ERROR_WANT_CONNECT");
			break;
		
		case SSL_ERROR_WANT_ACCEPT:
			strcpy(errName,"SSL_ERROR_WANT_ACCEPT");
			break;
		
		
		case SSL_ERROR_WANT_X509_LOOKUP:
			strcpy(errName,"SSL_ERROR_WANT_X509_LOOKUP");
			break;
		
		case SSL_ERROR_SYSCALL:
			strcpy(errName,"SSL_ERROR_SYSCALL");
			break;
		
		case SSL_ERROR_SSL:
			strcpy(errName,"SSL_ERROR_SSL");
			break;
		
		default:
			strcpy(errName,"UNKNOW");
			break;
				
	};//switch (error)
	sprintf(strTmp,"Error (%d) %s , WSAGetLastError()=%d.\n", errorNum,errName, WSAGetLastError());					  														
	return strTmp;
};//checkSSLError2

#endif


void printPacketHeader(struct PacketHeader *ph){
	char tmpStr[60];
	zstruct(tmpStr);
	switch(ph->packetType){
	    case PACKET_TYPE_MSG_TEST:
			strcpy(tmpStr,"PACKET_TYPE_MSG_TEST");
		break;
		default:
			printf ("????");
		break;
	};//switch
	printf("\t >>>>>Packet Header<<<<<<\n");
	printf("Type: %s(%d) size:%d ",tmpStr,ph->packetType,ph->packetSize);
	printf("Flags: %d Sig: %d Ver: %d\n",ph->packetFlags,ph->packetSig,ph->packetVersion);
};//printPacketHeader

void processTestPacket(struct testStruct *ts){
	
	printf("######TEST PACKET########\n");
        printf("x: %d y: %d\n Texto: %s\n",ts->x,ts->y,ts->texto);
  //printf("Texto: %s\n",ts->texto);
    //strcpy(ts->texto,"Thanks you. \n" );
};//processTestPacket



socketServer::socketServer(){
    log=NULL;
	log= new CDebugLog ("broadcastServer.log");
	if(!log){
		printf("Fatal cannot create a log file");
		exit(1);
	};//if
	sockLog=NULL;
	sockLog= new CDebugLog ("broadcastServerSock.log");
	if(!sockLog){
		printf("Fatal cannot create a log file");
		exit(1);
	};//if
	InitializeCriticalSection(&broadcastLock);
	InitializeCriticalSection(&socketsLock);
	InitializeCriticalSection(&clientsLock);
	InitializeCriticalSection(&descriptorsLock);
	mainThreadStatus=THREAD_STOPED;
	
	// borra los conjuntos de descriptores
	// maestro y temporal
	FD_ZERO(&master);    
    FD_ZERO(&read_fds);
	maxDescriptor=-1;
	conections=0;
	//load all ssl functions and algorimts
	#if USE_SSL
	  int err;
	  err=0;
	  SSLeay_add_ssl_algorithms();
	  SSL_load_error_strings();
      //meth=SSLv2_server_method();
	  meth=SSLv23_server_method();
	  //ctx==NULL;
      ctx = SSL_CTX_new (meth);
      if(ctx==NULL){
		  printf("Error SSL_CTX_new.\n");
		  ERR_print_errors_fp(stdout);
		  exit(2);
	  };//if
	  
	  
	  //cargar certificados
	  err = SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM);
	  if(err==-1){
		  printf("ERROR SSL_CTX_use_certificate_file.\n");
		  ERR_print_errors_fp(stderr);
		  exit(3);
	  };//if
	  
	  //err = SSL_CTX_use_RSAPrivateKey_file (ctx, KEYF,  SSL_FILETYPE_PEM);
	  err = SSL_CTX_use_PrivateKey_file (ctx, KEYF,  SSL_FILETYPE_PEM);
	  if(err==-1){
		  printf("ERROR SSL_CTX_use_PrivateKey_file.\n");
		  ERR_print_errors_fp(stderr);
		  exit(4);
	  };//if
      
  
      //check the key
	  err = SSL_CTX_check_private_key(ctx);
	  if(err!=1){
		  printf("ERROR SSL_CTX_check_private_key.\n");
		  ERR_print_errors_fp(stderr);
		  exit(1);
	  };//if
      //Default strings from ssl.h:
	  // 56 bits or less...
      //: use RC4-MD5 because it's faster then DES.
	  //: note, this change was done on the server instead.
	  //"EXP:!RSA:!EDH:!DES"	// use 40-bit ciphers and disallow RSA and EDH-RSA.			 
	  //	"EXP:!RSA:!EDH"	// use 40-bit ciphers and disallow RSA and EDH-RSA.
	  // full strength (128 bit or higher)
	  // Allow large ciphers, but avoid patented ones (RSA and RSA-EDH)
	  //"ALL:!RSA:!EDH"
	  //"ALL:!RSA:!EDH:!DES:!3DES"
	  //with RSA: "ALL:!ADH:RC4+RSA:+HIGH:+MEDIUM:+LOW:+SSLv2:+EXP"
	  //w/o  RSA: "HIGH:MEDIUM:LOW:ADH+3DES:ADH+RC4:ADH+DES:+EXP"
	  //SSL_CTX_set_cipher_list(ctx, "ALL:ADH:+EDH:+RSA");
	  //SSL_CTX_set_cipher_list(ctx,"ALL:!RSA:!EDH");
	  //SSL_CTX_set_cipher_list(ctx,"ALL");
	  //SSL_CTX_set_cipher_list(ctx, "ALL:!DES");
      //SSL_CTX_set_cipher_list(ctx, "ALL");
      //SSL_CTX_set_cipher_list(ctx, "ALL:ADH:+EDH:+RSA");
      //SSL_CTX_set_cipher_list(ctx,"RC4-MD5");
	 // SSL_CTX_set_cipher_list(ctx,"MD5");
	#endif
};//constructor

socketServer::~socketServer(){
	cleanSocketList();
	if(sockfd!=-1){
		close(sockfd);
	};

	#if USE_SSL
	  if(ctx){
	   	SSL_CTX_free(ctx);
	  };//if
	  if(sslSockfd){
	    SSL_free (sslSockfd);
	  };//if
    #endif
	if(log){
		delete log;
	 };//if
	if(sockLog){
		delete sockLog;
	 };//if
};//destructor

ErrorType socketServer::createListenSocket(int portNumber){
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

	int yes=1;

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // ¡Comprobar errores!
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
	
	//bool true_bool = true;
	//char true_bool = true;
	//int err = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&true_bool, sizeof(true_bool));
	//int err = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (int *)&true_bool, sizeof(true_bool));
	//if (err) {
	//	printf("WARNING setsockopt() to disable nagle failed  WSA error = %d.\n", WSAGetLastError());		
	//};//if
    
	
	my_addr.sin_family = AF_INET;         // Ordenación de máquina
	my_addr.sin_port = htons(portNumber);     // short, Ordenación de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))==INVALID_SOCKET){
		perror("Bind.\n");
		return ERR_ERROR;
	};
	
//	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == INVALID_SOCKET) {
//        perror("setsockopt");
//        return ERR_ERROR; 
//   };
	
	if(listen(sockfd, 10)==INVALID_SOCKET){
		perror("Listen.\n");
		return ERR_ERROR;
	};//if
	//create a ssl socket
	#if USE_SSL 
	  BIO* bio;
	  sslSockfd=NULL;
	  sslSockfd= SSL_new(ctx);
	  if(!sslSockfd){
		  printf("Error cannot create a SSL object.\n");
		  return ERR_ERROR;
	  };//if
	  bio=BIO_new_socket(sockfd,BIO_NOCLOSE);
	  BIO_set_nbio(bio,1);
	  SSL_set_bio(sslSockfd,bio,bio);
	  SSL_set_fd(sslSockfd,sockfd);
	#endif
    return ERR_NONE;
};//createListenSocket

void socketServer::closeSocket(){
	//close sockfd;
};//createListenSocket

ErrorType socketServer::startMainThread(){
	int result;
	mainThreadStatus=THREAD_STARTED;
	result =_beginthread(mainThreadLauncher, 0, (void *)this);
	if(result==-1){
		return ERR_ERROR;
	};
	return ERR_NONE;
};//startListenThread

void socketServer::mainThread(){
    int size;
	ErrorType err;
	size=0;
	unsigned int t;
	t=0;
	while(mainThreadStatus==THREAD_STARTED){
	   	
	   //new conections
       //printf("%d>>>>>>>>>>>>>>>>>>>>>>>\n",size);
	   t=GetTickCount();
	   err=acceptConnection(sockfd);
	   t=GetTickCount()-t;
	   //log->AddLog("AcceptConnection took: %d ms.\n",t);
	   //I/O functions
	   //Sleep(200);
	   
       processClientList();
	   
	   processBroadcastMessages();

	   //printf("Before processSocketsOutputList().\n");
	   processSocketsOutputList();
	   //printf("After processSocketsOutputList().\n");
	   
	   //printf("processSocketsOutputList took: %d ms , sec: %.3f.\n",t,t/1000);
	  // printf("<<<<<<<<<<<<<<<<<<<<<<<\n\n");
	   Sleep(1050);
	   
	};//while
	
	//************************
	    
	//************************
	mainThreadStatus=THREAD_EXIT;
};//listenThread

void socketServer::startReadThread(){
	int result;
	readThreadStatus=THREAD_STARTED;
	result = _beginthread(readThreadLauncher, 0, (void *)this);
};//startReadThread


void socketServer::readThread(){
	int s;
	unsigned int t;
	t=0;
	while(readThreadStatus==THREAD_STARTED){
	  if(t==7){
	     printf("\n\n\tActive connections: %d \n\nReading Packets ....\n",this->conections);
		 t=0;
	  }else{
		  t++;
	  };//if
	  processSocketInputList();	  
	  Sleep(750); 
	};//while
	readThreadStatus=THREAD_EXIT;
};//readThread

void socketServer::stopReadThread(){
	readThreadStatus=THREAD_STOPED;
	while(readThreadStatus!=THREAD_EXIT){
		printf("waiting for read thread.\n");
		Sleep(5000);
	};
	printf("read Thread Stoped.\n");
};//stopReadThread




void socketServer::stopMainThread(){	
	mainThreadStatus=THREAD_STOPED;
	while(mainThreadStatus!=THREAD_EXIT){
		printf("waiting for Listen thread.\n");
		Sleep(5000);
	};
	printf("main Thread Stoped.\n");
};//stopListenThread




void socketServer::processSocketsOutputList(){
  //printf("processSocketsOutputList()..\n");
  CSOCKET* s;	
  socketListIterator=socketList.begin();
  EnterCriticalSection(&socketsLock);
  for(int i=0;i<(int)socketList.size();i++){
	  s=socketList.at(i);
	 if(s->outputList.size()>0){
		//printf("Before processSocketOutputQueue.\n");
		processSocketOutputQueue(s);
        //printf("After processSocketOutputQueue. \n"); 
	 }; 
	 socketListIterator++;
  };//for
  LeaveCriticalSection(&socketsLock);
};//processSocketsOutputList

void socketServer::processClientList(){
//	printf("processClientList...\n");
	TRANSFERCLIENT *client;
	EnterCriticalSection(&clientsLock);
	for(int i=0;i<(int)clientList.size();i++){
	  client=clientList.at(i);
	  if(client&&client->socket->conectedFlag){
	 	//client->process();
	  }else{
        shutDownClient(client);
	  };// 
	  socketListIterator++;
	};//for
	LeaveCriticalSection(&clientsLock);
};//processClientList


//OUTOPUT
inline void socketServer::processSocketOutputQueue(CSOCKET *s){
	CPACKET *p;	
	ErrorType err;
	if(s->outputList.size()< MAX_PACKET_TO_PROCESS_OUTPUT_QUEUE){
	  while (s->outputList.size() > 0) {		 
		  p=s->outputList.back();			 
		  #if USE_SSL		     
			 err=SSL_sendSocket(s->sslSocket,p->userData,p->dataLength);		     
		  #else			  
			 err=sendSocket(s->socket,p->userData,p->dataLength);			  
		  #endif
		  if(err==ERR_NONE){
			  s->outputList.pop_back(); //delete the message
		   };//if		  
       };//while
	}else{
	  for(int i=0;i<MAX_PACKET_TO_PROCESS_OUTPUT_QUEUE;i++){
		 p=s->outputList.back();
		 #if USE_SSL		     
			 err=SSL_sendSocket(s->sslSocket,p->userData,p->dataLength);		     
		  #else			  
			 err=sendSocket(s->socket,p->userData,p->dataLength);			  
		  #endif
		  if(err==ERR_NONE){
			  s->outputList.pop_back(); //delete the message
		  };//if		  		  	
       };//for
	};//if(s->outputList.size()< MAX
};//processSocketOutputQueue

#if USE_SSL
inline ErrorType  socketServer::SSL_sendSocket(SSL *ssl,char *buff,int buffSize){
    int bytes_sent; 
	bytes_sent = SSL_write(ssl, buff, buffSize);  
	
	if (bytes_sent <= 0) {
		int err = SSL_get_error(ssl, bytes_sent);
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
inline ErrorType  socketServer::sendSocket(SOCKET_DESCRIPTOR sock,char *buff,int buffSize){
    int bytes_sent;
	bytes_sent = send(sock, buff, buffSize, 0);
	if (bytes_sent == SOCKET_ERROR) {
		char sockErrStr[80];
		int err = WSAGetLastError();
		if (err==WSAEWOULDBLOCK ||
			err==WSAEFAULT 
			) {
			// The output queue seems full.  This isn't good.
			printf("send() received WSAEWOULDBLOCK |WSAEFAULT...assuming output buffer is full BufferSize: %d.\n",buffSize);
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
	    printf("SOCKET_ERROR occurred during socket send().  socket=%d, Error=%d. Treating as disconnect..\n",sock,err);
        checkErrorNum(err,sockErrStr);
		sockLog->AddLog(sockErrStr);
		//HandleNotConnErr(err);	// check if we've lost a connection.
		return ERR_ERROR;
	};//if (bytes_sent == SOCKET_ERROR)


	if (bytes_sent < buffSize) {
		printf("Not all bytes sended (%d vs. %d) during socket send().\n",bytes_sent,buffSize);
		return ERR_ERROR;
	};//if

	return ERR_NONE;

};//sendSocket
#endif



//NEW CONECTIONS

ErrorType  socketServer::acceptConnection(SOCKET_DESCRIPTOR sockServer){
//	printf("Listening...\n");
	SOCKET_DESCRIPTOR newsock;
	SOCKADDR_IN dest_addr;
	int size;
	size=sizeof(SOCKADDR_IN);

    
	#if WIN32
	   newsock=accept(sockfd, (sockaddr *)&dest_addr, &size);
	#else
	   newsock=accept(sockfd, (sockaddr *)&dest_addr, (unsigned *)&size);
	#endif
	if(newsock==INVALID_SOCKET){		
		int err = WSAGetLastError();
		//  after switching to quattro with the 2.2.18 kernel (from 2.2.14)
		// we seem to get this error (104) about every 30 minutes under normal usage.
		// #define ECONNRESET      104     /* Connection reset by peer */
		if (err==WSAEWOULDBLOCK || err==WSAECONNRESET) {
			// Nobody is ready to connect yet.
			return ERR_NONE;	// no error, but no connection either.
		}else{
		   printf("error %d.\n",err);
		   perror("accept");			  
		   exit(1);	
	    };//if(err==WSAEWOULDBLOCK || err==WSAECONNRESET)
		
	}else{
		   	   
       #if NON_BLOCKING_SOCKETS 
		 #if WIN32
			dword one = 1;
			ioctlsocket(newsock, FIONBIO, &one);// enable non-blocking mode.
		 #else
		  fcntl(newsock, F_SETFL, O_NONBLOCK);//non blocking socket
		 #endif
       #endif	//non blocking sockets
		  
	   bool true_bool = true;
	   int err = setsockopt(newsock, IPPROTO_TCP, TCP_NODELAY, (char *)&true_bool, sizeof(true_bool));
	   if (err) {
		   printf("WARNING setsockopt() to disable nagle failed  WSA error = %d.\n", WSAGetLastError());		
	   };//if
	   //TRANFERCLIENT
		
		
		//SSL
		#if USE_SSL
		  //create a ssl socket
		  SSL *tmpSocket;
		  tmpSocket=NULL;
		  tmpSocket= SSL_new(ctx);
		  if(!tmpSocket){
			  printf("Error cannot create a SSL object listen thread.\n");
			  shutdown(newsock, 2);	//20000325MB: shut down sending and receiving on the socket
			  closesocket(newsock);
			  return ERR_ERROR;
		  };//if

		  BIO* bio;
		  bio=BIO_new_socket(sockfd,BIO_NOCLOSE);
	      BIO_set_nbio(bio,1);
	      SSL_set_bio(tmpSocket,bio,bio);

		  
		  SSL_set_fd(tmpSocket,newsock);  
		  printf("Accepting");
		  while((err = SSL_accept(tmpSocket))<=0){		
			  if (err <= 0) {		  
				err = SSL_get_error(tmpSocket, err);			   
				// The accept might just be pending... don't flag an error in those
				// pending situations.
				if (   err != SSL_ERROR_NONE
					&& err != SSL_ERROR_WANT_WRITE
					&& err != SSL_ERROR_WANT_READ
					&& err != SSL_ERROR_WANT_X509_LOOKUP
				) {	
					checkSSLError(err);
					printf("SSL_accept() failed. No common ciphers? SSL err=%d, WSAGetLastError()=%d\n", err, WSAGetLastError());			  
					log->AddLog ("SSL_accept() failed. No common ciphers? SSL err=%d, WSAGetLastError()=%d\n", err, WSAGetLastError());			  
					log->AddLog ("SSL_accept: %s.\n",checkSSLErrorXXX(err));
					shutdown(newsock, 2);	//20000325MB: shut down sending and receiving on the socket
					close(newsock);
					delete (tmpSocket);
					return ERR_ERROR;
				};//if (   err != SSL_ERROR_NONE
			  };//if(err <= 0) 
		  };//while 

		#endif
	   	
	   TRANSFERCLIENT * client;
	   client=NULL;
	   client =new TRANSFERCLIENT(this);
	   if(!client){
		   printf("Cannot create a TRANSFERCLIENT object in AcceptConnection function. \n");
		   log->AddLog("Cannot create a TRANSFERCLIENT object in AcceptConnection function. \n");
		   shutdown(newsock, 2);	//20000325MB: shut down sending and receiving on the socket
		   closesocket(newsock);
		   #if USE_SSL
		    delete (tmpSocket);
           #endif
		   exit(1);
	   };//if(!client)
       client->socket->socket =newsock;
	   #if USE_SSL
	   client->socket->sslSocket=tmpSocket;
	   #endif
	  
	   EnterCriticalSection(&descriptorsLock);		   	
	   //add socket to the master descriptors set
	   FD_SET(client->socket->socket, &master); 			
	   //actualiza el maximo descriptor
	   if(newsock>maxDescriptor){
		   maxDescriptor=newsock;
	   };
	   conections++;//increment the number of conections
	   LeaveCriticalSection(&descriptorsLock);
	   client->socket->conectedFlag =TRUE;
	   addSocket(client->socket);
	   addClient(client);
	   //get the remote ip Address
	   if(getpeername(client->socket->socket,(sockaddr *)&dest_addr,&size)){
		   perror("getpeername");
	   }else{
		   char name[25];
		    //inet_ntoa (dest_addr.sin_family, &dest_addr.sin_addr, client->socket->ipAddress , 20);
			//name=inet_ntoa(dest_addr);
			IP_ConvertIPtoString(dest_addr.sin_addr.s_addr, client->ipAddress, 20);
			printf("\n\n\tNew connection from:%s\n",client->ipAddress);
			log->AddLog("New connection from:%s\n",client->ipAddress);
	   };
		
	};//if(newsock==INVALID_SOCKET)
	//printf("Pause.\n");
	//getc(stdin);

	return ERR_NONE;
	
};//AcceptConnection


//INPUT
void socketServer::processSocketInputList(){
	//printf("processSocketInputList.\n");

#if 0
  CSOCKET* s;	
  socketListIterator=socketList.begin();
  EnterCriticalSection(&socketsLock);
  for(int i=0;i<(int)socketList.size();i++){
	  s=socketList.at(i);
	 if(s->conectedFlag==TRUE){
		processSocket2(s);
		//readPacket(s);
	 }; 
	 socketListIterator++;
  };//for
  LeaveCriticalSection(&socketsLock);  
#else

	int s;
	int tmpMaxDescrip;
	struct timeval tv;

	  if(conections){		
		zstruct(tv);
		tv.tv_sec =0;
		//tv.tv_usec =0.2;
		tv.tv_usec = 125000;	// 125ms timeout
		//tv.tv_usec = 200;	// 125ms timeout
		FD_ZERO(&read_fds);  
		EnterCriticalSection(&descriptorsLock);
		read_fds = master;
        tmpMaxDescrip=maxDescriptor+1;		  
        LeaveCriticalSection(&descriptorsLock);	
        //whichs sockets have something to read	??
       	if(conections>0){   //conections++
			s=select(tmpMaxDescrip, &read_fds, NULL, NULL, &tv);
			//s=select(tmpMaxDescrip, &read_fds, NULL, NULL, NULL);
			if(s==-1){
			   perror("Select");	
			   exit(1);
			};//if  
			processSocketsInputRequest(&read_fds);
		};//connections
	  };//if(conections)

#endif
};//rprocessSocketInputList


inline void socketServer::processSocketsInputRequest(fd_set *read_fds){
 // int i;
  CSOCKET* s;	
  int p;
  p=0;
  //printf("processSocketsInputRequest\n");
  socketListIterator=socketList.begin();
  //printf("EnterCriticalSection processSocketsInputRequest .\n");
  EnterCriticalSection(&socketsLock);
  for(int i=0;i<(int)socketList.size();i++){
	 s=socketList.at(i);
	 //if((FD_ISSET(s->socket,read_fds))&&(!s->ttl)){		 		 
	 if(FD_ISSET(s->socket,read_fds)){		 		 
		 #if USE_STRUCT
		  //log->AddLog ("Before readPacket \n");
		  //processSocket2(s);
		  readPacket(s);
		  //log->AddLog ("After readPacket \n");
         #else
		  //log->AddLog ("Before processSocket \n");
          processSocket(s);
		  //log->AddLog ("After processSocket \n");
         #endif
		 p++;		
	 }; //if(FD_ISSET(s->socket,read_fds))
	 socketListIterator++;
  };//for
  LeaveCriticalSection(&socketsLock);  
  //printf("LeaveCriticalSection processSocketsInputRequest.\n");
  //printf("%d Sockets procesed.\n",p);
  //log->AddLog("%d Sockets procesed.\n",p);
};// processSocketsInputRequest



inline void socketServer::processSocket(CSOCKET* s){
	CPACKET *p;
	int bytes;
	char buffer[1024];
	zstruct(buffer);
	ErrorType err;
	#if USE_SSL
	  //log->AddLog ("Before SSL_readSocket\n");
	  err=SSL_readSocket(s,buffer,1024,&bytes);	
	  //log->AddLog ("After SSL_readSocket bytes :%d\n",bytes);
	#else
	  //log->AddLog ("Before readSocket\n");
	  err=readSocket(s, buffer,1024, &bytes); 
	  //->AddLog ("After readSocket bytes:%d\n",bytes);
	#endif
	if((err==ERR_NONE)&&(bytes>0)){
		//create a packet
		p=NULL;
		p=new CPACKET;
		if(p){		
			buffer[bytes]=(char)NULL;
			p->alloc (strlen(buffer));
			p->writeData(buffer,bytes);
			//add the packet into the incomming packets queue
			s->addPacketInputList(p);
		}else{
			printf("Error socketServer::readSocket cannot create a CPACKET object.\n");
			exit(1);
		};//if(p)
	};//if(err==ERROR_NONE)	
};//processSocket

inline void socketServer::processSocket2(CSOCKET* s){
	struct PacketHeader *ph;
    struct PacketHeader header;
	struct testStruct * ts;
	struct testStruct msg;
	CPACKET *p;
	int bytes;
	char buffer[104];
	char buffer2[1024];
	int headerSize;
	zstruct(buffer);
	ErrorType err;

	headerSize=sizeof(struct PacketHeader)+sizeof( struct testStruct);
/*
	#if USE_SSL
	  err=SSL_readSocket(s ,buffer,headerSize,&bytes);	  
	#else
	  err=readSocket(s , buffer,headerSize, &bytes); 
	#endif
	
	if(err==ERR_NONE&&bytes==headerSize){
		ph = (struct PacketHeader *)buffer;
		printPacketHeader(ph);
		Sleep(5000);	
	};
*/

    printf("READING....\n");
	//read the data
	//do{
	  printf("Building  packet bytes %d headerSize %d.\n",bytes,headerSize);
    #if USE_SSL
	  err=SSL_readSocket(s,buffer2,headerSize ,&bytes);	  
	#else
	  err=readSocket(s, buffer2,headerSize, &bytes); 
	  //err=readSocket(s, buffer2,ph->packetSize, &bytes); 
	#endif
	//}while (bytes<headerSize);
    
    if((err==ERR_NONE)&&(bytes==headerSize)){
		//ph = (struct PacketHeader *)buffer;
		//create a packet
		p=NULL;
		p=new CPACKET;
		if(p){
			p->alloc(headerSize);		    
			//p->writeData((char*)ph,headerSize);
			p->writeData (buffer2,headerSize);
			//add the packet into the incomming packets queue
			s->addPacketInputList(p);
		}else{
			printf("Error socketServer::readSocket cannot create a CPACKET object.\n");
			exit(1);
		};//if(p)
	}else{
		printf("Cannot build a packet bytes %d headerSize %d.\n",bytes,headerSize);
		log->AddLog ("Cannot build a packet.\n");
	};//if(err==ERR_NONE&
};//processSocket2


#if USE_SSL
inline ErrorType  socketServer::SSL_readSocket(CSOCKET* s,char * buff,int bufSize,int* realBytesRead){
	int received;
	*realBytesRead =0;
	SSL * ssl;
	ssl=s->sslSocket; 
    received = SSL_read(ssl, buff, bufSize);
	sockLog->AddLog("function SSL_read called.\n");
	if (received <= 0) {
		int err = SSL_get_error((SSL *)ssl, received);
		
		if(err==SSL_ERROR_WANT_CONNECT ||err== SSL_ERROR_WANT_ACCEPT){
			return ERR_ERROR;
		};//if
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
			//printf("Got SSL_ERROR_* # %d (see openssl/ssl.h). Assuming connection lost.\n", err);		  
			//HandleNotConnErr(WSAENOTCONN);	// check if we've lost a connection.
			handleLostConecction(s);
			return ERR_NONE;
		};//if (   err == SSL_ERROR_NONE

	};//if (received <= 0)
	*realBytesRead =received ;
	return ERR_NONE;

};//SSL_readSocket
#endif

#if !USE_SSL
inline ErrorType socketServer::readSocket(CSOCKET* s,char * buff,int bufSize, int* realBytesRead ){
	if(s->conectedFlag ==FALSE){
		return ERR_NONE;
	};
    int received;
	*realBytesRead =0;
	SOCKET_DESCRIPTOR sock;
	sock=s->socket; 
	received = recv(sock, buff, bufSize, 0);	
	if (received == SOCKET_ERROR) {

		sockLog->AddLog("function recv called and error detected.\n");
		char sockErrStr[200];
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK
			  #if !WIN32
				|| err==WSAEAGAIN
			  #endif
				|| err==0) {
			// No data received.  This is not an error we care about.
			checkErrorNum(err,sockErrStr);
			sockLog->AddLog(sockErrStr);
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
			checkErrorNum(err,sockErrStr);
			sockLog->AddLog(sockErrStr);
			handleLostConecction(s);
			return ERR_NONE;
		}else{
			checkErrorNum(err,sockErrStr);
			sockLog->AddLog(sockErrStr);
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

void socketServer::handleLostConecction(CSOCKET* s){
	log->AddLog ("handleLostConecction called. \n");
	if (!s->ttl){
       s->ttl=GetTickCount()+SOCKET_TTL;
	}else{
	   if(GetTickCount()>s->ttl){
		   s->conectedFlag =FALSE;
		   printf("YAAAAAAAAAAAAAAAA.\n");
	   };//if
	};//if
};//handleLostConecction






void socketServer::addClient(TRANSFERCLIENT *client){
	EnterCriticalSection(&clientsLock);
	clientList.push_front(client);
	LeaveCriticalSection(&clientsLock);
};//addClient

void socketServer::addSocket(CSOCKET* sock){
	EnterCriticalSection(&socketsLock);
	socketList.push_front(sock);
	LeaveCriticalSection(&socketsLock);
};//addSocket

void socketServer::removeSocket(CSOCKET* sock){
	CSOCKET* s;
	//EnterCriticalSection(&socketsLock);
	socketListIterator=socketList.begin();
	for(int i=0;i<(int)socketList.size();i++){		
		s=socketList.at(i);
		if(s==sock){
		  socketList.erase(socketListIterator);
		  //printf("Socket %p was deleted.\n",sock);
		  log->AddLog("Socket %p was deleted.\n",sock);
		  EnterCriticalSection(&descriptorsLock);		            
		    FD_CLR(sock->socket, &master); 
		    conections--;
          LeaveCriticalSection(&descriptorsLock);			  
		};//if
		socketListIterator++;
	};//for
//	LeaveCriticalSection(&socketsLock);
};//removeSocket

void socketServer::removeClient(TRANSFERCLIENT *client){
	TRANSFERCLIENT *c;
	EnterCriticalSection(&clientsLock);
	clientListIterator=clientList.begin();
	for(int i=0;i<(int)clientList.size();i++){		
		c=clientList.at(i);
		if(c==client){
		  clientList.erase(clientListIterator);
		  //printf("Client %p was deleted.\n",c);
		  log->AddLog("Client %p was deleted.\n",c);
		  //delete (sock);
		};//if
		clientListIterator++;
	};//for
	LeaveCriticalSection(&clientsLock);
};//removeClient


void socketServer::shutDownClient(TRANSFERCLIENT *client){
    removeSocket(client->socket);
	removeClient(client);	
	EnterCriticalSection(&descriptorsLock);		  
	FD_CLR(client->socket->socket,&master);
	LeaveCriticalSection(&descriptorsLock);
	printf("\n\n\tClose connection from:%s\n",client->ipAddress);
	log->AddLog("Close connection from:%s\n",client->ipAddress);
	delete (client);
};//shutDownClient


void socketServer::cleanSocketList(){
    TRANSFERCLIENT *c;
	int count;
	count=0;
	EnterCriticalSection(&socketsLock);
	if(socketList.size()>0){		
		while(socketList.size()>0){
			count++;
			c=clientList.back();
			shutDownClient(c); 
			clientList.pop_back();					
		};//while
    };//if
	LeaveCriticalSection(&socketsLock);
	printf("WARNING Clean socketlist delete %d sockets.\n",count);
	log->AddLog("WARNING Clean socketlist delete %d sockets.\n",count);
	FD_ZERO(&master);  
};// cleanSocketList()



void socketServer::cleanClientList(){
	
	int count;
	count=0;
	EnterCriticalSection(&clientsLock);
	if(clientList.size()>0){		
		while(clientList.size()>0){
			count++;
			
			clientList.pop_back();
					
		};//while
    };//if
	LeaveCriticalSection(&clientsLock);
	printf("WARNING Clean clientslist delete %d clients.\n",count);
	log->AddLog("WARNING Clean clientslist delete %d clients.\n",count); 
};//cleanClientList

ErrorType socketServer::startService(int port){
	createListenSocket(port);
	if(startMainThread()!=ERR_NONE){
		printf("Cannot start the main thread.\n");
		return ERR_ERROR;
	};//if
	startReadThread();
	return ERR_NONE;
};//startService

void socketServer::stopService(){
};//stopService

void socketServer::pauseService(){
	stopMainThread();
	stopReadThread();
};//pauseService

ErrorType socketServer::readPacket(CSOCKET* s){
	CPACKET *p;
    int bytesRead;
	int bytesRecieved;//real byted extracted from the socket buffer
	
	int bufferSize;//how many bytes we whant to extract
    struct PacketHeader ph; 
	char buffer[MAX_BUFFER_SIZE];
	ErrorType err;

    bytesRecieved=0;
	bufferSize=sizeof( struct PacketHeader);//target
    //read the packet header
	if(!s->conectedFlag){
       return ERR_NONE;
	};//if
    do{
		
       #if USE_SSL
		  err=SSL_readSocket(s,(char*)&ph+bytesRecieved,bufferSize-bytesRecieved,&bytesRead);		
       #else
		 // log->AddLog ("Before bytesRecieved :%d building HEADER readPacket::readSocket.\n",bytesRead);
		  err=readSocket(s,(char*)&ph+bytesRecieved,bufferSize-bytesRecieved,&bytesRead);		  
		 // log->AddLog ("After bytesRecieved :%d building HEADER readPacket::readSocket.\n",bytesRead);
       #endif

	  if(bytesRead==0){
		  //how many time we
		  //dont received data 
		  //from the remote host
		  if (!s->ttl){
            s->ttl=GetTickCount()+5000;
			printf("Lost Connection detected.\n");
		  }else{
	      if(GetTickCount()>=s->ttl){
		     s->conectedFlag =FALSE;		     
		  };//if
		  };//if
		  return ERR_ERROR;
	  };//if

      if((err==ERR_NONE) && (s->conectedFlag==FALSE) ){
		  return ERR_NONE;
	  };//if

	  if(err!=ERR_NONE){
		  return ERR_ERROR;
	  };//if

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
		 // log->AddLog ("Before building BODY readPacket::readSocket.\n");
		  err=readSocket(s,buffer+bytesRecieved,bufferSize-bytesRecieved,&bytesRead);		  
		  //log->AddLog ("After building BODY readPacket::readSocket.\n");
       #endif
		  if(bytesRead==0){
             //how many time we
		    //dont received data 
		    //from the remote host
		    if (!s->ttl){
              s->ttl=GetTickCount()+5000;
			  printf("Lost Connection detected.\n");
			}else{
	          if(GetTickCount()>=s->ttl){
		        s->conectedFlag =FALSE;		     
			  };//if
			};//if
		    return ERR_ERROR;		    
		  };//if


		  if((err==ERR_NONE) && (s->conectedFlag==FALSE) ){
		     return ERR_NONE;
		  };//if
		  if(err!=ERR_NONE){
			  return ERR_ERROR;
		  };
       bytesRecieved=bytesRecieved+bytesRead;
	}while(bufferSize>bytesRecieved);
//	printf("Data size:%d\n",bytesRecieved);

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
	//s->addPacketInputList(p);
	printf("Packet arrived<=====\n");
	this->addBroadcastMessage(p,s);
    return ERR_NONE;
};//readPacket


void socketServer::processBroadcastMessages(){
	struct broadcastPacket *bp;
	CSOCKET* s;
	
	EnterCriticalSection(&socketsLock);
	EnterCriticalSection(&broadcastLock);
	
	if(broadcastList.size()>0){
	  for(int j=0;j<(int)broadcastList.size();j++){
		  bp=broadcastList.back ();
		  broadcastList.pop_back ();
		  for(int i=0;i<(int)socketList.size();i++){
	        s=socketList.at(i);
		    if((s!=bp->source)&&(!s->ttl)){
			   s->addPacketOutputList(bp->packet);
			};//if
		  };//for sockets
		  free(bp);
		  printf("Packet delivered =====>\n");
	  };//for broadcast messages
	};//if(broadcastList.size()>0)
	
	LeaveCriticalSection(&broadcastLock);
	LeaveCriticalSection(&socketsLock);
	
};//processBroadcastMessages

void socketServer::addBroadcastMessage(CPACKET *packet,CSOCKET *source){
	struct broadcastPacket *bp=NULL;
    bp=(struct broadcastPacket*)malloc(sizeof(struct broadcastPacket));
    if(!bp){
		printf("Error cannot initialize memmory for struct broadcastPacket*.\n");
		exit(1);
	};//if
	bp->packet =packet;
	bp->source =source;
	EnterCriticalSection(&broadcastLock);
	broadcastList.push_front(bp);
	LeaveCriticalSection(&broadcastLock);
};//addBroadcastMessage





