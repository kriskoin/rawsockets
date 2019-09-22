#include <stdio.h>

#ifndef __SOCKETSERVER_H__
#include <socketServer.h>
#endif


bool Terminate = false;

void OurSignalHandlerRoutine(int signal, struct sigcontext sc)
{
	switch (signal) {
	case SIGTERM:
		Terminate=true;
		printf("SIGTERM proccesed ! .\n");
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



int main(){
	signal(SIGHUP,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGTERM, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGINT,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGSEGV, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGBUS,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGFPE,  (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGQUIT, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGALRM, (void (*)(int))OurSignalHandlerRoutine);
	signal(SIGPIPE, SIG_IGN);	// ignore broken pipe signals (socket disconnects cause SIGPIPE)

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
	printf("B Y E .\n");
	return 0;
};//main

