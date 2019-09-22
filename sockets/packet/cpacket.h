#ifndef __CPACKET_H__
#define __CPACKET_H__


#ifndef __MESSAGES_H__
#include <messages.h>
#endif

#ifndef __UTILSFUNCTIONS_H__
#include <utilsFunctions.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>




#define MAX_BUFFER_SIZE 204800 //200K

class CPACKET{
	   public:
	   //public variables
	   //char  userData[1024];
	   char * userData;
	   //char * userData;
	   int dataLength;

	   //public methods
       void cleanData();
       void alloc(int size);
	   void writeData(char * data, int length);
       CPACKET();
       CPACKET(int size);
	   ~CPACKET();

	   private:
	   //private variables

	   //private methods
};//CPACKET

struct PacketHeader{
	   WORD16 packetSig;
	   WORD16 packetVersion;
	   WORD16 packetFlags;
	   WORD16 packetType;
	   WORD16 packetSize;
};///PacketHeader


#endif
