#ifndef __THREADFUNCTIONS_H__
#include <threadFunctions.h>
#endif



#ifndef WIN32
//*********************************************************
// 1999/05/19 - MB
//
// Wrapper for converting _beginthread() to pthread_create().
//
unsigned long _beginthread(void (* funcptr)(void *), unsigned stack_size, void *lparam)

{

	pthread_t pt = 0;
	int error = pthread_create(&pt, NULL, (void *(*)(void *))funcptr, lparam);
	if (error) {
		return (unsigned long)-1;	// error.
	}
	//pr(("%s(%d) _beginthread() just started thread %d\n", _FL, pt));
	return pt;	// return thread id.

}

#endif // WIN32
