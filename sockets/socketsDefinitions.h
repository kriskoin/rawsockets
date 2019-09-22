#ifndef __SOCKETSDEFINITIONS_H__
#define __SOCKETSDEFINITIONS_H__

#include <string.h>
#include <stdio.h>

#ifndef __ERRORSDEFINITIONS_H__
#include <errorsDefinitions.h>
#endif


#define NON_BLOCKING_SOCKETS 1
#define USE_SSL 0
#define USE_STRUCT 1

#if USE_SSL
	#include <openssl/rsa.h>       /* SSLeay stuff */
	#include <openssl/crypto.h>
	#include <openssl/x509.h>
	#include <openssl/pem.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
#endif

#ifndef WIN32


#include <errno.h>
// typedef's and defines to make socket.h stuff look like winsock.h stuff.
typedef int 				SOCKET_DESCRIPTOR;
typedef struct sockaddr		SOCKADDR;
typedef struct sockaddr_in	SOCKADDR_IN;
typedef SOCKADDR *			LPSOCKADDR;
#define INVALID_SOCKET		(-1)
#define SOCKET_ERROR		(-1)
#define WSAEWOULDBLOCK		EWOULDBLOCK
#define WSAENOTCONN			ENOTCONN
// From /usr/src/linux/include/asm/errno.h:
// #define ENOTCONN        107     /* Transport endpoint is not connected */

/* BSD Sockets
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE
*/

#define WSAECONNRESET		ECONNRESET
#define WSAECONNABORTED		ECONNABORTED
#define WSAESHUTDOWN		ESHUTDOWN
#define WSAEAGAIN			EAGAIN
#define WSAEHOSTUNREACH		EHOSTUNREACH
#define WSAEHOSTDOWN		EHOSTDOWN

#define closesocket(a)		close(a)

inline int WSAGetLastError(void)  {return errno;}
#else
   #include <winsock.h>
   typedef int 				SOCKET_DESCRIPTOR;
#endif // !WIN32




enum THREAD_STATUS{
	 THREAD_STARTED,
	 THREAD_STOPED,
	 THREAD_EXIT
	};


inline void checkErrorNum(int errorNum){
	char errName[25];
	switch (errorNum){

        case WSAEWOULDBLOCK:
			strcpy(errName,"WSAEWOULDBLOCK/EWOULDBLOCK");
		break;

		case WSAENOTCONN:
			strcpy(errName,"WSAENOTCONN/ENOTCONN");
		break;


	    case WSAECONNRESET:
			strcpy(errName,"WSAECONNRESET/ECONNRESET");
		break;

		case WSAECONNABORTED:
			strcpy(errName,"WSAECONNABORTED/ECONNABORTED");
		break;

		case WSAESHUTDOWN:
			strcpy(errName,"WSAESHUTDOWN/ESHUTDOWN");
		break;

#ifndef WIN32
		case WSAEAGAIN:
			strcpy(errName,"WSAEAGAIN/EAGAIN");
		break;
#endif
		case WSAEHOSTUNREACH:
			strcpy(errName,"WSAEHOSTUNREACH/EHOSTUNREACH");
		break;

		case WSAEHOSTDOWN:
			strcpy(errName,"WSAEHOSTDOWN/EHOSTDOWN");
		break;

		default:
			strcpy(errName,"UNKNOW");
		break;
	};//switch
	printf("Error (%d) %s , WSAGetLastError()=%d.\n", errorNum,errName, WSAGetLastError());					  														
};//checkErrorNum


inline void checkErrorNum(int errorNum,char * str){
	char errName[25];
	switch (errorNum){

/*
*WSANOTINITIALISED
*WSAENETDOWN
*WSAEFAULT
*WSAENOTCONN
*WSAEINTR
*WSAEINPROGRESS
*WSAENETRESET
*WSAENOTSOCK
*WSAEOPNOTSUPP
*WSAESHUTDOWN
*WSAEWOULDBLOCK
*WSAEMSGSIZE
*WSAEINVAL
*WSAECONNABORTED
*WSAETIMEDOUT
*WSAECONNRESET
*/

/*
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

*/
		

		case WSANOTINITIALISED:
			strcpy(errName,"WSANOTINITIALISED");
		break;

		case WSAENETDOWN:
			strcpy(errName,"WSAENETDOWN");
		break;

        case WSAEWOULDBLOCK:
			strcpy(errName,"WSAEWOULDBLOCK/EWOULDBLOCK");
		break;
		
		case WSAEFAULT:
			strcpy(errName,"WSAEFAULT");
		break;
		case WSAEINTR:
			strcpy(errName,"WSAEINTR");
		break;
		case WSAEINPROGRESS:
			strcpy(errName,"WSAEINPROGRESS");
		break;
		case WSAENETRESET:
			strcpy(errName,"WSAENETRESET");
		break;
		case WSAENOTSOCK:
			strcpy(errName,"WSAENOTSOCK");
		break;
		case WSAEOPNOTSUPP:
			strcpy(errName,"WSAEOPNOTSUPP");
		break;
		case WSAEMSGSIZE:
			strcpy(errName,"WSAEMSGSIZE");
		break;
		case WSAEINVAL:
			strcpy(errName,"WSAEINVAL");
		break;
		
		case WSAETIMEDOUT:
			strcpy(errName,"WSAETIMEDOUT");
		break;



		case WSAENOTCONN:
			strcpy(errName,"WSAENOTCONN/ENOTCONN");
		break;


	    case WSAECONNRESET:
			strcpy(errName,"WSAECONNRESET/ECONNRESET");
		break;

		case WSAECONNABORTED:
			strcpy(errName,"WSAECONNABORTED/ECONNABORTED");
		break;

		case WSAESHUTDOWN:
			strcpy(errName,"WSAESHUTDOWN/ESHUTDOWN");
		break;

#ifndef WIN32
		case WSAEAGAIN:
			strcpy(errName,"WSAEAGAIN/EAGAIN");
		break;
#endif
		case WSAEHOSTUNREACH:
			strcpy(errName,"WSAEHOSTUNREACH/EHOSTUNREACH");
		break;

		case WSAEHOSTDOWN:
			strcpy(errName,"WSAEHOSTDOWN/EHOSTDOWN");
		break;

		default:
			strcpy(errName,"UNKNOW");
		break;
	};//switch
	sprintf(str,"Error (%d) %s , WSAGetLastError()=%d.\n", errorNum,errName, WSAGetLastError());
};//checkErrorNum


	
#if USE_SSL	

	
inline void checkSSLError(int errorNum){
	char errName[25];
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
	printf("Error (%d) %s , WSAGetLastError()=%d.\n", errorNum,errName, WSAGetLastError());					  														
};//checkSSLError

#endif //USE_SSL	



#endif
