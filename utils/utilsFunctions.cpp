#ifndef __UTILSFUNCTIONS_H__
#include <utilsFunctions.h>
#endif



#define PPEnterCriticalSection0(p, __FILE__, __LINE__)  EnterCriticalSection(p) 
#define PPLeaveCriticalSection0(p, __FILE__, __LINE__)  LeaveCriticalSection(p) 
#define PPInitializeCriticalSection(p,__FILE__,__LINE__) InitializeCriticalSection(p)


#ifndef WIN32

WORD32 GetTickCount(void){
	struct timeval now;
	gettimeofday(&now, NULL);
	static WORD32 initial_ms_value;
	if (!initial_ms_value) {
		// We've never initialized the initial ms value. Do so now.
		initial_ms_value = (now.tv_sec * 1000) + (now.tv_usec / 1000);
	}
	return (now.tv_sec * 1000) + (now.tv_usec / 1000) - initial_ms_value;
};//GetTickCount

#endif




//static PPCRITICAL_SECTION LocalTimeCritSec;
static CRITICAL_SECTION LocalTimeCritSec;

static int LocalTimeCritSec_initialized = FALSE;
struct tm *localtime(const time_t *timer, struct tm *out){

	if (!LocalTimeCritSec_initialized) {
		LocalTimeCritSec_initialized = TRUE;
		PPInitializeCriticalSection(&LocalTimeCritSec, CRITSECPRI_TIMEFUNCS, "localtime");
		//InitializeCriticalSection(&LocalTimeCritSec);
	}
	EnterCriticalSection(&LocalTimeCritSec);
	struct tm *t = localtime(timer);
	if (out) {		// optionally fill a return structure
		if (t) {
			*out = *t;	// make a copy before releasing the critical section
		} else {
			zstruct(*out);
		};//if
		t = out;	// return ptr to the structure they gave us rather than localtime()'s
	};//if(out)
	LeaveCriticalSection(&LocalTimeCritSec);
	return t;
};//localtime



struct tm *gmtime(const time_t *timer, struct tm *out){
	if (!LocalTimeCritSec_initialized) {
		LocalTimeCritSec_initialized = TRUE;
		PPInitializeCriticalSection(&LocalTimeCritSec, CRITSECPRI_TIMEFUNCS, "localtime");
	}
	EnterCriticalSection(&LocalTimeCritSec);
	struct tm *t = gmtime(timer);
	if (out) {		// optionally fill a return structure
		*out = *t;	// make a copy before releasing the critical section
		t = out;	// return ptr to the structure they gave us rather than localtime()'s
	}
	LeaveCriticalSection(&LocalTimeCritSec);
	return t;
};//*gmtime



//*********************************************************
// 1999/07/24 - MB
//
// Return a formatted string for use in error logs and whatnot
// Format looks like this: 07/23@21:45:21
//

char *TimeStr(void){
	return TimeStr(time(NULL), FALSE, FALSE, 0);
};

char *TimeStrWithYear(void){
	return TimeStr(time(NULL), TRUE, FALSE, 0);
};//char *TimeStrWithYear

char *TimeStr(time_t tt){
	return TimeStr(tt, FALSE, FALSE, 0);
};//char *TimeStr

char *TimeStr(time_t tt, char year_flag){
	return TimeStr(tt, year_flag, FALSE, 0);
};//char *TimeStr



/*********************************************************
// HK11/11/1999 -- modified to use rotating buffers      */


char *TimeStr(time_t tt, char year_flag, int specific_time_zone_flag, int time_zone_offset){

	#define TIME_STR_BUFFERS	8
	static char time_str[TIME_STR_BUFFERS][30];
	static int buf_index;
	buf_index = (buf_index+1) % TIME_STR_BUFFERS;	// increment to next usable one
	zstruct(time_str[buf_index]);
	struct tm tm;
	struct tm *t;

	if (specific_time_zone_flag) {
		tt -= time_zone_offset;
		t = gmtime(&tt, &tm);
	} else {
		t = localtime(&tt, &tm);
	};//if (specific_time_zone_flag)

	if (year_flag) {
		sprintf(time_str[buf_index], "%04d/%02d/%02d @ %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	} else {
		sprintf(time_str[buf_index], "%02d/%02d@%02d:%02d:%02d", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	};//if (year_flag)
	return time_str[buf_index];
};//char *TimeStr	



//*********************************************************
// 2000/02/17 - MB
//
// Return just the date as a string
// DateStr() returns "mm/dd", DateStrWithYear() returns "yy/mm/dd"
//



char *DateStrWithYear(void){
	return DateStr(time(NULL), TRUE, FALSE, 0);
};//char *DateStrWithYear(void)



char *DateStr(time_t tt, char year_flag, int specific_time_zone_flag, int time_zone_offset){
	static char time_str[TIME_STR_BUFFERS][30];
	static int buf_index;
	buf_index = (buf_index+1) % TIME_STR_BUFFERS;	// increment to next usable one
	zstruct(time_str[buf_index]);
	struct tm tm;
	struct tm *t;

	if (specific_time_zone_flag) {
		tt -= time_zone_offset;
		t = gmtime(&tt, &tm);
	} else {
		t = localtime(&tt, &tm);
	};//if (specific_time_zone_flag)

	if (year_flag) {
		sprintf(time_str[buf_index], "%02d/%02d/%02d", t->tm_year%100, t->tm_mon+1, t->tm_mday);
	} else {
		sprintf(time_str[buf_index], "%02d/%02d", t->tm_mon+1, t->tm_mday);
	};//if (year_flag)

	return time_str[buf_index];
};//DateStr


/****************************************************************/
/*  Mon July 11/94 - MB											*/
/*																*/
/*	strnncpy(): strncpy that makes sure the dest is terminated	*/
/*																*/
/****************************************************************/

char *strnncpy(char *dest, const char *src, int max_dest_len){
	char *p = dest;
		if (!src) {
			src = "(null)";	// avoid null pointer crashes
		};//if

	    while (--max_dest_len > 0 && *src)
	        *p++ = *src++;
	    *p++ = 0;

		// 19990805MB: always fill remaining space in dest string with zeros.
		if (max_dest_len > 0) {
			memset(p, 0, max_dest_len);
		};//if
	    return dest;
};//strnncpy



//*********************************************************
// 2000/02/17 - MB
//
// strcat() that takes a max dest length.
//

char *strnncat(char *dest, const char *src, int max_dest_len){
	int current_len = strlen(dest);
	int chars_left = max_dest_len - current_len;
	if (chars_left > 0) {
		strnncpy(dest + current_len, src, chars_left);
	};//if
	return dest;
};//strnncat

