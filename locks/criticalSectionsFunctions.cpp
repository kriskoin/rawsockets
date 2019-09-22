#ifndef __CRITICALSECTIONSFUNCTIONS_H__
#include "criticalSectionsFunctions.h"
#endif

#ifndef WIN32
//*********************************************************
// Wrapper for converting InitializeCriticalSection() to
// pthread_mutex_init().
//
void InitializeCriticalSection(CRITICAL_SECTION *crit_sec_ptr){

	//*crit_sec_ptr = (CRITICAL_SECTION)PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	*crit_sec_ptr = (CRITICAL_SECTION)PTHREAD_MUTEX_INITIALIZER;
	//*crit_sec_ptr = (CRITICAL_SECTION)PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
};//InitializeCriticalSection

#endif
