//---------------------------------------------------------------------------

#ifndef _logH_INCLUDED
#define _logH_INCLUDED

#include "stdio.h"

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#ifndef __UTILSFUNCTIONS_H__
#include <utilsFunctions.h>
#endif

//---------------------------------------------------------------------------
//Class DebugLog

#define MAX_STRING_LOG 255
class CDebugLog {
private:
        FILE* file;
//        TCriticalSection * CriticalSection;
public:
        char name[100];
        ~CDebugLog();
        CDebugLog(char* fileName);
        void AddLog(char* fmt, ...);
};//class DebugLog

#endif


