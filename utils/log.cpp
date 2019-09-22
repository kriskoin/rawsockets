//---------------------------------------------------------------------------


#ifndef _logH_INCLUDED
#include "log.h"
#endif

#include <string.h>
//---------------------------------------------------------------------------




CDebugLog::CDebugLog(char* fileName)
{
        //TODO: Add your source code here
 //     CriticalSection = new TCriticalSection;
       file= fopen(fileName, "w");
       if(!file){
          printf("%s%s","Can't open the file: ",fileName);
       };//if(!file)
       strcpy(name,fileName);
//       CriticalSection->Enter();
       fprintf(file,"%s","\n-------------------------------------------------\n");
       fprintf(file,"LOG Opened at: %s",TimeStr()) ;
       fprintf(file,"%s","\n-------------------------------------------------\n\n\n");
       fflush(file);
//       CriticalSection->Leave();
};//Constructor


CDebugLog::~CDebugLog()
{
        //TODO: Add your source code here
        if(file){
           //CriticalSection->Enter();
           fprintf(file,"%s","\n-------------------------------------------------\n");
           fprintf(file,"LOG Closed  at :%s",TimeStr());
           fprintf(file,"%s","\n-------------------------------------------------\n");
           fflush(file);
           fclose(file);
           //CriticalSection->Leave();
        };
		/*
        if(CriticalSection){
           delete (CriticalSection);
        };*/
};//Destructor



void CDebugLog::AddLog(char* fmt, ...)
{
    //TODO: Add your source code here
    va_list arg_ptr;
    char str[MAX_STRING_LOG];
   //	zstruct(str);
    va_start(arg_ptr, fmt);
    vsprintf(str, fmt, arg_ptr);
    va_end(arg_ptr);
    //CriticalSection->Enter();
    fprintf(file,"%s",str);
    fflush(file);
   // CriticalSection->Leave();
/*
void kprintf(char *fmt, ...){
    va_list arg_ptr;
    char str[KPRINTF_MAX_STRING];
	zstruct(str);
    va_start(arg_ptr, fmt);
    vsprintf(str, fmt, arg_ptr);
    va_end(arg_ptr);
    kwrites(str);
    kflush();
}
*/
};//AddLog
