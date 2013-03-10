
#ifndef LUA_WINCE_H
#define LUA_WINCE_H

#ifndef _WIN32_WCE
#error "_WIN32_WCE should be defined first"
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windef.h>

// these pragmas help Studio to build for some CE platforms - use as necessary

#ifndef __GNUC__
// Number of clock ticks per second. Used by the clock() function.
//
#define CLOCKS_PER_SEC		1000

// Macro for CLK_PER_SEC
//
#define CLK_TICK			CLOCKS_PER_SEC

// structure definitions
//
#ifndef _TM_DEFINED
struct tm {
    int tm_sec;			// seconds
    int tm_min;			// minutes
    int tm_hour;		// hours
    int tm_mday;		// day of the month
    int tm_mon;			// month
    int tm_year;		// years since 1900 (from 0)
    int tm_wday;		// days since Sunday (from 0)
    int tm_yday;		// days since Jan 1
    int tm_isdst;		// daylight savings (summertime) flag
    };
#define _TM_DEFINED
#endif

// typedefs
//

// since time_t is already defined in CE as a ULONG, we need a new definition
//
typedef LONGLONG time_t;

static struct tm tmbuf;

// Convert tm to a string in the format "Www Mmm dd hh:mm:ss yyyy",
// where Www is the weekday, Mmm the month in letters, dd the day
// of the month, hh:mm:ss the time, and yyyy the year. The string
// is followed by a newline and a terminating null character,
// conforming a total of 26 characters.
//
char *asctime(const struct tm* tmptr);

#endif //__GNUC__

#define clock_t DWORD
// Return number of clock ticks since process start.
//
clock_t clock(void);

#ifndef __GNUC__
// Convert time_t value to string in the same format as asctime.
//
char* ctime(const time_t* timer);

// Convert a time_t value to a tm structure as UTC time.
//
struct tm* gmtime(const time_t* timer);

// Convert a time_t value to a tm structure as local time.
//
struct tm* localtime(const time_t* timer);

// Returns the Unix timestamp corresponding to the arguments given.
// This timestamp is a long integer containing the number of seconds between the Unix Epoch
// (January 1 1970 00:00:00 GMT) and the time specified.
//
time_t mktime(struct tm *tptr);

// Get the current time from the system clock. Stores that value in timer.
// If timer is null, the value is not stored, but it is still returned by
// the function.
//
time_t time(time_t* timer);

// Format tm into a date/time string
//
size_t strftime(char *s, size_t maxs, const char *f, const struct tm *t);
#endif


/* these are terrible, but just for CE quick-dirty (pedro) */
#define strcoll strcmp
#ifndef isalpha
#define isalpha(c)			( ('a' <= (c) && (c) <= 'z') || ('A' <= (c) && (c) <= 'Z') )
#define isdigit(c)			( '0' <= (c) && (c) <= '9' )
#define isalnum(c)			( isalpha(c) || isdigit(c) )
#define isspace(c)			( (c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' )
#define iscntrl(c)			( (0x00 <= (c) && (c) <= 0x1F) || (c) == 0x7F)
#define islower(c)			( 'a' <= (c) && (c) <= 'z' )
#define isprint(c)			( (0x20 <= (c) && (c) <= 0x7E) )
#define ispunct(c)			( isprint(c) && ( !isalnum(c) && !isspace(c) ))
#define isupper(c)			( 'A' <= (c) && (c) <= 'Z' )
#define isxdigit(c)			( isdigit(c) || ('a' <= (c) && (c) <= 'f') || ('a' <= (c) && (c) <= 'f') )
#endif

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

#ifndef MAX_PATH
#define MAX_PATH 2048
#endif

#ifndef L_tmpnam
#define L_tmpnam MAX_PATH
#endif


char *strdup( const char *str );
const char *getenv( const char *name );
FILE *tmpfile();

int system( const char * );
int rename( const char *, const char * );
int remove( const char * );
char *tmpnam( char * );

//int chdir(const char *path);
int mkdir(const char *path);

char *setlocale( int category, const char *locale );
struct lconv *localeconv( void );

HINSTANCE LoadLibraryA(LPCSTR);
HMODULE GetModuleHandleA(LPCSTR);

#define strerror(errnum) ("(Error ocurred)")
//extern int errno;
#define errno (GetLastError ())

FILE* _fopen(const char *filename, const char *flags);


#ifdef	__cplusplus
}
#endif

#endif
