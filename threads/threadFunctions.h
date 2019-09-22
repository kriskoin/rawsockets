#ifndef __THREADFUNCTIONS_H__
#define __THREADFUNTIONS_H__



#ifndef WIN32

    #include <pthread.h>
	#include <sys/sem.h>
	#include <sys/ipc.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <signal.h>
    #define _cdecl
    unsigned long _beginthread(void (*)(void *), unsigned stack_size, void *lparam);
#endif



#endif
