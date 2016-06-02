#ifndef _LIB_BASE_H_
#define _LIB_BASE_H_

#ifdef WIN32
//#include <afx.h>
#include <WinSock2.h>
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>

typedef int TBOOL;

typedef unsigned long BASE_DWORD;

#define VOID    void
#undef  TRUE
#undef  FALSE
#define FALSE   0
#define TRUE    1
#ifndef _WINDEF_
#ifndef _LONG_DEFINED_
#define _LONG_DEFINED_
typedef unsigned char BYTE;
typedef unsigned short WORD;
#ifndef UINT
#define UINT unsigned int
//typedef unsigned int UINT;
#endif
typedef int INT;
typedef long LONG;
typedef char CHAR;
typedef CHAR *LPSTR;
typedef const CHAR *LPCSTR;
typedef unsigned char UCHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef BASE_DWORD ULONG;
typedef double DOUBLE;
#endif
#endif

#ifndef WIN32
#define MAX_PATH  256
//#define inline //__inline
typedef struct  XC_TSYSTEMTIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
}
TSYSTEMTIME;

#else

#ifndef strcasecmp
#define strcasecmp stricmp
#endif

#ifndef atoll
#define atoll _atoi64
#endif

#ifndef vsnprintf
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif




typedef SYSTEMTIME TSYSTEMTIME ;	
#endif

#ifndef _WINDEF_
typedef double DATE;
#endif

/*
#define BASE_ASSERT ASSERT
#define BASE_ASSERT_VALID ASSERT_VALID
#define BASE_VERIFY VERIFY
#define BASE_TRACE TRACE
*/

#define THIS_FILE          __FILE__

#define BASE_ASSERT(bf) \
{ \
    if(!(bf)) \
    { \
        printf("Assert failed at line %d of file %s\n", \
            __LINE__,THIS_FILE); \
	    exit(0); \
    } \
} 

#define BASE_ASSERT_VALID(p)    BASE_ASSERT(p!=NULL)
#define BASE_VERIFY(f)          BASE_ASSERT(f)
#define BASE_DEBUG_ONLY(f)      (f)

// The following trace macros are provided for backward compatiblity
//  (they also take a fixed number of parameters which provides
//   some amount of extra error checking)
#define BASE_TRACE                  printf

// These AFX_DUMP macros also provided for backward compatibility
#define BASE_AFX_DUMP0(dc, sz)   dc << sz
#define BASE_AFX_DUMP1(dc, sz, p1) dc << sz << p1


#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN 
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//#ifndef WIN32
#undef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
//#else
// Disable deprecation warnings, because setting _CRT_SECURE_NO_DEPRECATE
// did not prevent the warnings.
#pragma warning(disable:4996)
//#endif

#ifdef WIN32
#define BASE_TEXT(X) StringUtils::replaceAll(X,0,"lld","I64d").c_str()
#else
#define BASE_TEXT(X) X
#endif



#endif
