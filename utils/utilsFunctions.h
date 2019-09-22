#ifndef __UTILSFUNCTIONS_H__
#define __UTILSFUNCTIONS_H__


#include <string.h>
#include <stdio.h>

#ifdef WIN32 
  #ifdef MSVCC //use windows and microsoft visual c++
   #include <windows.h>
  #endif
#endif

#ifndef __CRITICALSECTIONSFUNCTIONS_H__
#include "criticalSectionsFunctions.h"
#endif

#ifndef __TYPESDEF_H
#include <typesDef.h>
#endif



#include <time.h>



#ifndef FALSE

  #define FALSE 0

#endif

#ifndef TRUE

 #define TRUE (!FALSE)

#endif







char *strnncpy(char *dest, const char *src, int max_dest_len);
char *strnncat(char *dest, const char *src, int max_dest_len);



//#if (defined (_TIME_H) || defined (_TM_DEFINED))

  // Our version of localtime that takes an output struct so we are thread-safe.
  struct tm *localtime(const time_t *timer, struct tm *out);
  struct tm *gmtime(const time_t *timer, struct tm *out);
  char *TimeStr(time_t tt);					// TimeStr(tt, FALSE);
  char *TimeStr(time_t tt, char year_flag);	// pass time as an argument
  char *TimeStr(time_t tt, char year_flag, int specific_time_zone_flag, int time_zone_offset);
  char *DateStr(time_t tt, char year_flag, int specific_time_zone_flag, int time_zone_offset);

// Return just the date as a string
// DateStr() returns "mm/dd", DateStrWithYear() returns "yy/mm/dd"
char *DateStr(void);
char *DateStrWithYear(void);

// Return a formatted string for use in error logs and whatnot
// Format looks like this: 07/23@21:45:21
// Long Format looks like this: 07/23/1999@21:45:21
char *TimeStr(void);			// use current time
char *TimeStrWithYear(void);	// use current time, also display year.



#ifndef WIN32
  #include <sys/time.h>
  #include <unistd.h>
  #define Sleep(a) usleep((a)*1000)
  WORD32 GetTickCount(void);
#endif //WIN32

#define zstruct(a)	memset(&(a),0,sizeof(a))


#endif
