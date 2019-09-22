#ifndef __MESSAGES_H__
#define __MESSAGES_H__



#ifndef __TYPESDEF_H
#include <typesDef.h>
#endif



enum{
	PACKET_TYPE_MSG_DEMO,
	PACKET_TYPE_MSG_TEST,
    PACKET_TYPE_MSG_TEST1,
	PACKET_TYPE_MSG_TEST2,
	PACKET_TYPE_MSG_TEST3,
	PACKET_TYPE_MSG_NONE,
};

//PACKET_TYPE_MSG_DEMO
struct messageDemo{
	char sourceIp[60];
	char message[150];
};

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


#endif