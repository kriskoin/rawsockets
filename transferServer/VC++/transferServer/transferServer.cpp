// transferServer.cpp : Defines the entry point for the console application.
//
/*
#include "stdafx.h"

int main(int argc, char* argv[])
{
	printf("Hello World!\n");
	return 0;
}

*/

#include "stdafx.h"

#include <stdio.h>

#ifndef __SOCKETSERVER_H__
#include <socketServer.h>
#endif


bool Terminate = false;

#ifndef WIN32
void OurSignalHandlerRoutine(int signal, struct sigcontext sc)
{
	switch (signal) {
	case SIGTERM:
		Terminate=true;
		printf("\n\n\t################\tShuting down the server!\n\t################\n\n");
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
#endif

#ifdef WIN32
BOOL WINAPI OurCtrlHandlerRoutine(DWORD dwCtrlType)
{
	//kp(("%s(%d) OurCtrlHandleRoutine() was called with type = %d ($%08lx)\n",_FL,dwCtrlType, dwCtrlType));
	BOOL handled = FALSE;
	switch (dwCtrlType) {
	case CTRL_C_EVENT:
		Terminate=true;
		printf("\n\n\t################################\n\t\tShuting down the server!\n\t################################\n\n");
		handled = TRUE;
		break;
	case CTRL_BREAK_EVENT:
		// Ctrl+Break signal was received
	    Terminate=true;
		printf("CTRL_BREAK proccesed ! .\n");
		handled = TRUE;
		break;
	case CTRL_CLOSE_EVENT:
		// A signal that the system sends to all processes attached to a
		// console when the user closes the console (either by choosing
		// the Close command from the console window's System menu, or by
		// choosing the End Task command from the Task List).
		Terminate=true;
		printf("CTRL_CLOSE proccesed ! .\n");
		handled = TRUE;
		break;
	case CTRL_LOGOFF_EVENT:
		// A signal that the system sends to all console processes when a
		// user is logging off. This signal does not indicate which user is
		// logging off, so no assumptions can be made.
		Terminate=true;
		printf("CTRL_LOGOFF proccesed ! .\n");
		handled = TRUE;
		break;
	case CTRL_SHUTDOWN_EVENT:
		// A signal that the system sends to all console processes when
		// the system is shutting down.
		Terminate=true;
		printf("CTRL_SHUTDOWN proccesed ! .\n");
		handled = TRUE;
		break;
	default:
		printf("Unknown control signal received from OS.\n");
		break;
	};//switch
	return handled;	// return a BOOL indicating whether or not we handled this signal
};//OurCtrlHandlerRoutine
#endif


int main(){
    #ifndef WIN32
		signal(SIGHUP,  (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGTERM, (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGINT,  (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGSEGV, (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGBUS,  (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGFPE,  (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGQUIT, (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGALRM, (void (*)(int))OurSignalHandlerRoutine);
		signal(SIGPIPE, SIG_IGN);	// ignore broken pipe signals (socket disconnects cause SIGPIPE)
    #else
		SetConsoleCtrlHandler(OurCtrlHandlerRoutine, TRUE);
    #endif
	socketServer *ss;
    ss=new socketServer;
	if(!ss){
		printf("Cannot create a server socket.\n");
		exit (1);
	};//if
	ErrorType err=ss->startService(7775);
	if(err!=ERR_NONE){
		printf("Printf error startService.\n");
		delete (ss);
		exit(1);
	};
	while(!Terminate){				
       // printf("Main thread.\n");
		Sleep(5000);
	};//while
    ss->stopService(); 
	if(ss){
		delete (ss);
	};
	printf("\n\n\t################################\n\t\tServer is Down!\n\t################################\n\n");
	return 0;
};//main

