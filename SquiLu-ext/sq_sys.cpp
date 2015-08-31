#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"
SQ_OPT_STRING_STRLEN();

static const SQChar sq_sys_TAG[] = "sqsys";


#ifdef _WIN32

#define _WIN32_WINNT	0x0600

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winsock2.h>
#include <mmsystem.h>	/* timeGetTime */

#else

#ifndef _GNU_SOURCE
#define _GNU_SOURCE	/* pthread_*affinity_np */
#endif

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS	64
#define _LARGEFILE_SOURCE	1
#endif

#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <pthread.h>
#include <sched.h>

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/param.h>
#endif

#endif


#include <sys/types.h>
#include <errno.h>


#ifdef _WIN32

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef SSIZE_T		ssize_t;
#endif

#ifndef ULONG_PTR
typedef SIZE_T		ULONG_PTR, DWORD_PTR;
#endif

#if (_WIN32_WINNT >= 0x0500)
#define InitCriticalSection(cs) \
    InitializeCriticalSectionAndSpinCount(cs, 3000)
#else
#define InitCriticalSection(cs) \
    (InitializeCriticalSection(cs), TRUE)
#endif

#define SYS_ERRNO		GetLastError()
#define SYS_IS_EAGAIN(e)	((e) == WSAEWOULDBLOCK)

#else

#define SYS_ERRNO		errno
#define SYS_IS_EAGAIN(e)	((e) == EAGAIN || (e) == EWOULDBLOCK)

#define SYS_SIGINTR		SIGUSR2

#endif

/*
#define LUA_SYSLIBNAME "sys"
LUALIB_API int luaopen_sys (lua_State *L);

#define LUA_SOCKLIBNAME "sys.sock"
LUALIB_API int luaopen_sys_sock (lua_State *L);

#define LUA_ISAPILIBNAME "sys.isapi"
LUALIB_API int luaopen_sys_isapi (lua_State *L);
*/

/*
 * 64-bit integers
 */

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64			int64_t;
typedef unsigned __int64	uint64_t;
#else
#include <stdint.h>
#endif

#define INT64_MAKE(lo,hi)	(((int64_t) (hi) << 32) | (unsigned int) (lo))
#define INT64_LOW(x)		((unsigned int) (x))
#define INT64_HIGH(x)		((unsigned int) ((x) >> 32))


/*
 * File and Socket Handles
 */

#define FD_TYPENAME	"sys.handle"

#ifdef _WIN32
typedef HANDLE	fd_t;
typedef SOCKET	sd_t;
#else
typedef int	fd_t;
typedef int	sd_t;
#endif


/*
 * Error Reporting
 */

#define SYS_ERROR_MESSAGE	"SYS_ERR"

//int sys_seterror (lua_State *L, int err);


/*
 * Time
 */

typedef int	msec_t;

#ifdef _WIN32
#define sys_milliseconds	timeGetTime
#else
msec_t sys_milliseconds (void);
#endif

#define TIMEOUT_INFINITE	((msec_t) -1)


/*
 * Buffer Management
 */

#define MAX_PATHNAME	512

#define SYS_BUFIO_TAG	"bufio__"  /* key to indicate buffer I/O */
#define SYS_BUFSIZE	4096

struct membuf;

struct sys_buffer
{
    union
    {
        const char *r;  /* read from buffer */
        char *w;  /* write to buffer */
    } ptr;
    size_t size;
    struct membuf *mb;
};


static int sys_buffer_read_init (HSQUIRRELVM v, int idx, struct sys_buffer *sb);
static void sys_buffer_read_next (struct sys_buffer *sb, const size_t n);

static void sys_buffer_write_init (HSQUIRRELVM v, int idx, struct sys_buffer *sb,
                                   char *buf, const size_t buflen);
static int sys_buffer_write_next (HSQUIRRELVM v, struct sys_buffer *sb,
                                  char *buf, const size_t buflen);
static int sys_buffer_write_done (HSQUIRRELVM v, struct sys_buffer *sb,
                                  char *buf, const size_t tail);


/*
 * Threading
 */

struct sys_thread;

static void sys_thread_set (struct sys_thread *td);
static struct sys_thread *sys_thread_get (void);

static HSQUIRRELVM sys_thread_tosquilu (struct sys_thread *td);

static void sys_thread_switch (struct sys_thread *td);

static void sys_thread_check (struct sys_thread *td, HSQUIRRELVM v);

static void sys_vm2_enter (struct sys_thread *td);
static void sys_vm2_leave (struct sys_thread *td);

static void sys_vm_enter (HSQUIRRELVM v);
static void sys_vm_leave (HSQUIRRELVM vL);

static struct sys_thread *sys_thread_new (HSQUIRRELVM v,
        struct sys_thread *vmtd,
        struct sys_thread *vmtd2,
        const int push_udata);
static void sys_thread_del (struct sys_thread *td);

static int sys_thread_suspend (struct sys_thread *td, const msec_t timeout);
static void sys_thread_resume (struct sys_thread *td);

static int sys_eintr (void);


/*
 * Event Queue & Scheduler
 */

#define EVQ_TYPENAME	"sys.event_queue"

enum
{
    SYS_ERR_SYSTEM = -1,
    SYS_ERR_TIMEOUT = 1,
    SYS_ERR_THROW
};

enum
{
    EVQ_SCHED_OBJ = 0,
    EVQ_SCHED_TIMER,
    EVQ_SCHED_PID,
    EVQ_SCHED_DIRWATCH,
#ifdef _WIN32
    EVQ_SCHED_REGWATCH,
#endif
    EVQ_SCHED_SIGNAL,
    EVQ_SCHED_SOCKET
};

struct event_queue;

static int sys_evq_loop (HSQUIRRELVM v, struct event_queue *evq,
                         const msec_t timeout, const int linger,
                         const int once, const int evq_idx);

static int sys_evq_sched_add (HSQUIRRELVM vL, const int evq_idx, const int type);
static void sys_evq_sched_del (HSQUIRRELVM v, void *ev_op, const int ev_added);

static void sys_sched_event_added (HSQUIRRELVM v, void *ev);
static void sys_sched_event_ready (HSQUIRRELVM v, void *ev);


#ifdef _WIN32

/*
 * Windows NT specifics
 */

#ifndef STATUS_CANCELLED
#define STATUS_CANCELLED	((DWORD) 0xC0000120L)
#endif

typedef BOOL (WINAPI *PCancelSynchronousIo) (HANDLE hThread);

typedef BOOL (WINAPI *PCancelIoEx) (HANDLE hThread, LPOVERLAPPED overlapped);

extern PCancelSynchronousIo pCancelSynchronousIo;
extern PCancelIoEx pCancelIoEx;

extern CRITICAL_SECTION g_CritSect;

#if defined(_WIN32_WCE) || defined(WIN32_VISTA)
#define is_WinNT	1
#else
extern int is_WinNT;
#endif


/* Windows Registry */

#define WREG_TYPENAME	"sys.win32.registry"


/*
 * Convert Windows OS filenames to UTF-8
 */

static void *utf8_to_filename (const char *s);
static char *filename_to_utf8 (const void *s);

#endif


/*
 * Arguments: ..., [number]
 * Returns: string
 */
static int sys_strerror (HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, err, SYS_ERRNO);

#ifndef _WIN32
#if defined(BSD) || (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
    char buf[512];

    if (!err) goto success;

    if (!strerror_r(err, buf, sizeof(buf)))
    {
        sq_pushstring(v, buf, -1);
        return 1;
    }
#endif
    sq_pushstring(v, strerror(err), -1);
#else
    const int flags = FORMAT_MESSAGE_IGNORE_INSERTS
                      | FORMAT_MESSAGE_FROM_SYSTEM;
    WCHAR buf[512];

    if (!err) goto success;

    if (is_WinNT
            ? FormatMessageW(flags, NULL, err, 0, buf, sizeof(buf) / sizeof(buf[0]), NULL)
            : FormatMessageA(flags, NULL, err, 0, (char *) buf, sizeof(buf), NULL))
    {
        char *s = filename_to_utf8(buf);
        if (s)
        {
            char *cp;
            for (cp = s; *cp != '\0'; ++cp)
            {
                if (*cp == '\r' || *cp == '\n')
                    *cp = ' ';
            }

            sq_pushstring(v, s, -1);
            free(s);
            return 1;
        }
    }
    sq_pushfstring(v, _SC("System error %d"), err);
#endif
    return 1;
success:
    sq_pushliteral(v, _SC("OK"));
    return 1;
}

/*
 * Returns: nil, string
 */
static int sys_seterror (HSQUIRRELVM v, int err)
{
  if (err) {
#ifndef _WIN32
    errno = err;
#else
    SetLastError(err);
#endif
  }
  sq_pushnull(v);
  sys_strerror(v);
  sq_pushvalue(v, -1);
  sq_setglobal(v, SYS_ERROR_MESSAGE);
  return 2;
}


#include <sys/time.h> /* for struct timeval */

#ifndef __WIN32__
#include <sys/times.h> /* for struct tms */
#endif

#ifdef __WIN32__
#include "win/win.h"
#endif

#include <errno.h>
#include <limits.h> /* for CLK_TCK */

#ifndef CLK_TCK
#define CLK_TCK CLOCKS_PER_SEC
#endif

#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN (-1)

struct rusage
{
    struct timeval ru_utime; /* user time used */
    struct timeval ru_stime; /* system time used */
};

static int getrusage(int who, struct rusage *rusage)
{
#ifdef WIN32
    FILETIME starttime;
    FILETIME exittime;
    FILETIME kerneltime;
    FILETIME usertime;
    ULARGE_INTEGER li;
    if (who != RUSAGE_SELF)
    {
        /* Only RUSAGE_SELF is supported in this implementation for now */
        errno = EINVAL;
        return -1;
    }
    if (rusage == (struct rusage *) NULL)
    {
        errno = EFAULT;
        return -1;
    }
    memset(rusage, 0, sizeof(struct rusage));
    if (GetProcessTimes(GetCurrentProcess(),
                        &starttime, &exittime, &kerneltime, &usertime) == 0)
    {
        _dosmaperr(GetLastError());
        return -1;
    }
    /* Convert FILETIMEs (0.1 us) to struct timeval */
    memcpy(&li, &kerneltime, sizeof(FILETIME));
    li.QuadPart /= 10L; /* Convert to microseconds */
    rusage->ru_stime.tv_sec = li.QuadPart / 1000000L;
    rusage->ru_stime.tv_usec = li.QuadPart % 1000000L;
    memcpy(&li, &usertime, sizeof(FILETIME));
    li.QuadPart /= 10L; /* Convert to microseconds */
    rusage->ru_utime.tv_sec = li.QuadPart / 1000000L;
    rusage->ru_utime.tv_usec = li.QuadPart % 1000000L;
#else /* all but WIN32 */
    struct tms tms;
    int tick_rate = CLK_TCK; /* ticks per second */
    clock_t u,
    s;
    if (rusage == (struct rusage *) NULL)
    {
        errno = EFAULT;
        return -1;
    }
    if (times(&tms) < 0)
    {
        /* errno set by times */
        return -1;
    }
    switch (who)
    {
    case RUSAGE_SELF:
        u = tms.tms_utime;
        s = tms.tms_stime;
        break;
    case RUSAGE_CHILDREN:
        u = tms.tms_cutime;
        s = tms.tms_cstime;
        break;
    default:
        errno = EINVAL;
        return -1;
    }
#define TICK_TO_SEC(T, RATE) ((T)/(RATE))
#define TICK_TO_USEC(T,RATE) (((T)%(RATE)*1000000)/RATE)
    rusage->ru_utime.tv_sec = TICK_TO_SEC(u, tick_rate);
    rusage->ru_utime.tv_usec = TICK_TO_USEC(u, tick_rate);
    rusage->ru_stime.tv_sec = TICK_TO_SEC(s, tick_rate);
    rusage->ru_stime.tv_usec = TICK_TO_USEC(u, tick_rate);
#endif /* WIN32 */
    return 0;
}

static SQRESULT sq_sys_getrusage(HSQUIRRELVM vm)
{
    return SQ_OK;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_sys_##name,nparams,tycheck}
static SQRegFunction sq_sys_methods[] =
{
    _DECL_FUNC(getrusage,  -1, _SC("x")),
    {0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_sys(HSQUIRRELVM v)
    {
        //add a namespace sqsys
        sq_pushstring(v, sq_sys_TAG,-1);
        sq_newclass(v, SQFalse);
        sq_insert_reg_funcs(v, sq_sys_methods);
        sq_newslot(v,-3,SQTrue); //add sqmix table to the root table

        return SQ_OK;
    }

#ifdef __cplusplus
}
#endif
