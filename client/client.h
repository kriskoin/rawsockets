#ifndef __CLIENT_H__
#define __CLIENT_H__


#ifndef __SOCKET_H__
#include <csocket.h>
#endif

class socketServer;
class TRANSFERCLIENT{

public:
	//public variables
	char ipAddress[25];
	CSOCKET* socket;
	//public methods
	TRANSFERCLIENT();
	TRANSFERCLIENT(socketServer* s);
	~TRANSFERCLIENT();
	void process();
private:
	//private variables
	socketServer* server;
	//private methods
    void processPacket(CPACKET* p);
	void processPacket2(CPACKET* p);
    inline void sendPacket (CPACKET* p);
	void processTestPacket(struct testStruct *ts, int size);
};//TRANFERCLIENT

#endif

