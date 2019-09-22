#ifndef __CRITICALSECTIONSFUNCTIONS_H__
#define __CRITICALSECTIONSFUNCTIONS_H__

#ifndef WIN32

    #include <pthread.h>
	#include <sys/sem.h>
	#include <sys/ipc.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <signal.h>


    typedef pthread_mutex_t	CRITICAL_SECTION;
    typedef pthread_mutex_t	TCriticalSection;    
    typedef CRITICAL_SECTION TCriticalSection;
	void InitializeCriticalSection(CRITICAL_SECTION *crit_sec_ptr);
   	#define EnterCriticalSection(crit_sec_ptr)	pthread_mutex_lock(crit_sec_ptr)
	#define TryEnterCriticalSection(crit_sec_ptr)	(!pthread_mutex_trylock(crit_sec_ptr))
	#define LeaveCriticalSection(crit_sec_ptr)	pthread_mutex_unlock(crit_sec_ptr)
	#define DeleteCriticalSection(crit_sec_ptr)	pthread_mutex_destroy(crit_sec_ptr)
#endif

#endif