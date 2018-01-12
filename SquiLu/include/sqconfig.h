
#ifdef _SQ64

typedef short SQInt16;
typedef unsigned short SQUnsignedInt16;

#ifdef _MSC_VER
typedef __int64 SQInteger;
typedef unsigned __int64 SQUnsignedInteger;
typedef unsigned __int64 SQHash; /*should be the same size of a pointer*/
#else
typedef long long SQInteger;
typedef unsigned long long SQUnsignedInteger;
typedef unsigned long long SQHash; /*should be the same size of a pointer*/
#endif
typedef int SQInt32;
typedef unsigned int SQUnsignedInteger32;
#else
typedef int SQInteger;
typedef int SQInt32; /*must be 32 bits(also on 64bits processors)*/
typedef unsigned int SQUnsignedInteger32; /*must be 32 bits(also on 64bits processors)*/
typedef unsigned int SQUnsignedInteger;
typedef short SQInt16;
typedef unsigned short SQUnsignedInt16;
typedef unsigned int /*ptrdiff_t*/ SQHash; /*should be the same size of a pointer*/
#endif


#ifdef SQUSEDOUBLE
typedef double SQFloat;
#else
typedef float SQFloat;
#endif

#if defined(SQUSEDOUBLE) && !defined(_SQ64) || !defined(SQUSEDOUBLE) && defined(_SQ64)
#ifdef _MSC_VER
typedef __int64 SQRawObjectVal; //must be 64bits
#else
typedef long long SQRawObjectVal; //must be 64bits
#endif
#define SQ_OBJECT_RAWINIT() { _unVal.raw = 0; }
#else
typedef SQUnsignedInteger SQRawObjectVal; //is 32 bits on 32 bits builds and 64 bits otherwise
#define SQ_OBJECT_RAWINIT()
#endif

#ifndef SQ_ALIGNMENT // SQ_ALIGNMENT shall be less than or equal to SQ_MALLOC alignments, and its value shall be power of 2.
#if defined(SQUSEDOUBLE) || defined(_SQ64)
#define SQ_ALIGNMENT 8
#else
#define SQ_ALIGNMENT 4
#endif
#endif

typedef void* SQUserPointer;
typedef SQUnsignedInteger SQBool;
typedef SQInteger SQRESULT;

#ifdef SQUNICODE
#include <wchar.h>
#include <wctype.h>


typedef wchar_t SQChar;
typedef unsigned wchar_t SQUChar;


#define scstrcmp    wcscmp
#ifdef _WIN32
#define scsprintf   _snwprintf
#define scvfprintf	_vfwprintf
#define scfprintf fwprintf
#define scvprintf vfwprintf
#define scfopen	_wfopen
#define scgetenv _wgetenv
#define scsystem _wsystem
#define scasctime _wasctime
#define scstrftime _wstrftime
#define scremove _wremove
#define screname _wrename
#else
#define scsprintf   swprintf
#define scvfprintf	vfwprintf
#define scfprintf fwprintf
#define scvprintf vfwprintf
#define scfopen	wfopen
#define scgetenv wgetenv
#define scsystem wsystem
#define scasctime wasctime
#define scstrftime wstrftime
#define scremove wremove
#define screname wrename
#endif
#define scstrlen    wcslen
#define scstrcpy    wcstrcpy
#define scstrtod    wcstod
#ifdef _SQ64
#define scstrtol    wcstoll
#else
#define scstrtol    wcstol
#endif
#define scstrtoul   wcstoul
#define scvsprintf  vswprintf
#define scstrstr    wcsstr
#define scstrchr wcschr
#define scstrncmp wcstrncmp
#define scstrpbrk	wcspbrk
#define scstrtok	wcstrtok
#define scprintf    wprintf
#define scgetenv wgetenv
#define scsystem wsystem
#define scasctime wasctime
#define scstrftime wstrftime
#define scremove wremove
#define screname wrename

#ifdef _WIN32
#define WCHAR_SIZE 2
#define WCHAR_SHIFT_MUL 1
#define MAX_CHAR 0xFFFF
#else
#define WCHAR_SIZE 4
#define WCHAR_SHIFT_MUL 2
#define MAX_CHAR 0xFFFFFFFF
#endif

#define _SC(a) L##a


#define scisspace   iswspace
#define scisdigit   iswdigit
#define scisprint   iswprint
#define scisxdigit  iswxdigit
#define scisalpha   iswalpha
#define sciscntrl   iswcntrl
#define scisalnum   iswalnum


#define sq_rsl(l) ((l)<<WCHAR_SHIFT_MUL)
#define sq_str_sizeof(p) (sizeof(p)/WCHAR_SIZE)

#else
typedef char SQChar;
typedef unsigned char SQUChar;
#define _SC(a) a
#define scstrcmp    strcmp
#ifdef _MSC_VER
#define scsprintf   _snprintf
#define scvfprintf	_vfprintf
#define scfprintf fprintf
#define scvprintf vfprintf
#define scfopen	fopen
#else
#define scsprintf   snprintf
#define scvfprintf	vfprintf
#define scfprintf fprintf
#define scvprintf vfprintf
#define scfopen	fopen
#endif
#define scstrlen    strlen
#define scstrcpy    strcpy
#define scstrtod    strtod
#ifdef _SQ64
#ifdef _MSC_VER
#define scstrtol    _strtoi64
#else
#define scstrtol    strtoll
#endif
#else
#define scstrtol    strtol
#endif
#define scstrtoul   strtoul
#define scvsprintf  vsnprintf
#define scstrstr    strstr
#define scstrpbrk	strpbrk
#define scstrchr	strchr
#define scstrncmp	strncmp
#define scstrtok	strtok
#define scisspace   isspace
#define scisdigit   isdigit
#define scisprint   isprint
#define scisxdigit  isxdigit
#define sciscntrl   iscntrl
#define scisalpha   isalpha
#define scisalnum   isalnum
#define scprintf    printf
#define MAX_CHAR 0xFF

#define scgetenv getenv
#define scsystem system
#define scasctime asctime
#define scstrftime strftime
#define scremove remove
#define screname rename
#define sq_rsl(l) (l)
#define sq_str_sizeof(p) (sizeof(p))

#endif

#ifdef _SQ64
#define _PRINT_INT_PREC _SC("ll")
#define _PRINT_INT_FMT _SC("%lld")
#else
#define _PRINT_INT_FMT _SC("%d")
#endif

#if !defined(SQUILU_MAX_PATH)
#define SQUILU_MAX_PATH	1024
#endif

#if defined(_WIN32)
#define SQUILU_DIRSEP	_SC("\\")
#else
#define SQUILU_DIRSEP	_SC("/")
#endif

#if !defined(SQUILU_PATH_VAR)
#define SQUILU_PATH_VAR	_SC("SQUILU_PATH")
#endif

#if !defined (SQUILU_PATH_SEP)
#define SQUILU_PATH_SEP		_SC(";")
#endif
