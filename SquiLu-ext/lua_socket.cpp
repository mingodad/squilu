/*=========================================================================*\
* LuaSocket toolkit
* Networking support for the Lua language
* Diego Nehab
* 26/11/1999
*
* This library is part of an  effort to progressively increase the network
* connectivity  of  the Lua  language.  The  Lua interface  to  networking
* functions follows the Sockets API  closely, trying to simplify all tasks
* involved in setting up both  client and server connections. The provided
* IO routines, however, follow the Lua  style, being very similar  to the
* standard Lua read and write functions.
*
* Adapted to Squirrel by Domingo Alvarez Duarte on 04/01/2013
*
* RCS ID: $Id: luasocket.c,v 1.53 2005/10/07 04:40:59 diego Exp $
\*=========================================================================*/
#include "lua_socket.h"
/*-------------------------------------------------------------------------*\
* Initializes module
\*-------------------------------------------------------------------------*/
#ifdef WIN32
/* WinSock doesn't have a strerror... */
static const char *wstrerror(int err);
#endif

int lua_socket_open(void){
#ifdef WIN32
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    int err = WSAStartup(wVersionRequested, &wsaData );
    if (err != 0) return 0;
    if ((LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0) &&
        (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)) {
        WSACleanup();
        return 0;
    }
#else
    /* instals a handler to ignore sigpipe or it will crash us */
    signal(SIGPIPE, SIG_IGN);
#endif
    return 1;
}

/*-------------------------------------------------------------------------*\
* Close module
\*-------------------------------------------------------------------------*/
int lua_socket_close(void){
#ifdef WIN32
    WSACleanup();
#endif
    return 1;
}

/*-------------------------------------------------------------------------*\
* Close and inutilize socket
\*-------------------------------------------------------------------------*/
void lua_socket_destroy(p_socket ps){
    if (*ps != SOCKET_INVALID) {
        lua_socket_setblocking(ps); /* close can take a long time on WIN32 */
#ifdef WIN32
        closesocket(*ps);
#else
        close(*ps);
#endif
        *ps = SOCKET_INVALID;
    }
}

void lua_socket_shutdown(p_socket ps, int how){
    lua_socket_setblocking(ps);
    shutdown(*ps, how);
    lua_socket_setnonblocking(ps);
}

/*-------------------------------------------------------------------------*\
* Wait for readable/writable/connected socket with timeout
\*-------------------------------------------------------------------------*/
#ifdef SOCKET_POLL
#include <sys/poll.h>

#define WAITFD_R        POLLIN
#define WAITFD_W        POLLOUT
#define WAITFD_C        (POLLIN|POLLOUT)
#else
#define WAITFD_R        1
#define WAITFD_W        2
#define WAITFD_C        (WAITFD_R|WAITFD_W)
#endif

int lua_socket_waitfd(p_socket ps, int sw, p_timeout tm){
    int ret;
#ifdef WIN32
#define WAITFD_E        4

    fd_set rfds, wfds, efds, *rp = NULL, *wp = NULL, *ep = NULL;
    struct timeval tv, *tp = NULL;
    double t;
    if (timeout_iszero(tm)) return IO_TIMEOUT;  /* optimize timeout == 0 case */
    if (sw & WAITFD_R) {
        FD_ZERO(&rfds);
		FD_SET(*ps, &rfds);
        rp = &rfds;
    }
    if (sw & WAITFD_W) { FD_ZERO(&wfds); FD_SET(*ps, &wfds); wp = &wfds; }
    if (sw & WAITFD_C) { FD_ZERO(&efds); FD_SET(*ps, &efds); ep = &efds; }
    if ((t = lua_timeout_get(tm)) >= 0.0) {
        tv.tv_sec = (int) t;
        tv.tv_usec = (int) ((t-tv.tv_sec)*1.0e6);
        tp = &tv;
    }
    ret = select(0, rp, wp, ep, tp);
    if (ret == -1) return WSAGetLastError();
    if (ret == 0) return IO_TIMEOUT;
    if (sw == WAITFD_C && FD_ISSET(*ps, &efds)) return IO_CLOSED;
#else
#ifdef SOCKET_POLL
    struct pollfd pfd;
    pfd.fd = *ps;
    pfd.events = sw;
    pfd.revents = 0;
    if (timeout_iszero(tm)) return IO_TIMEOUT;  /* optimize timeout == 0 case */
    do {
		int t = (int)(timeout_getretry(tm)*1e3);
		ret = poll(&pfd, 1, t >= 0? t: -1);
	} while (ret == -1 && errno == EINTR);
    if (ret == -1) return errno;
    if (ret == 0) return IO_TIMEOUT;
    if (sw == WAITFD_C && (pfd.revents & (POLLIN|POLLERR))) return IO_CLOSED;
#else
    fd_set rfds, wfds, *rp, *wp;
    struct timeval tv, *tp;
    double t;
    if (timeout_iszero(tm)) return IO_TIMEOUT;  /* optimize timeout == 0 case */
    do {
        /* must set bits within loop, because select may have modifed them */
        rp = wp = NULL;
        if (sw & WAITFD_R) { FD_ZERO(&rfds); FD_SET(*ps, &rfds); rp = &rfds; }
        if (sw & WAITFD_W) { FD_ZERO(&wfds); FD_SET(*ps, &wfds); wp = &wfds; }
        t = lua_timeout_getretry(tm);
        tp = NULL;
        if (t >= 0.0) {
            tv.tv_sec = (int)t;
            tv.tv_usec = (int)((t-tv.tv_sec)*1.0e6);
            tp = &tv;
        }
        ret = select(*ps+1, rp, wp, NULL, tp);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1) return errno;
    if (ret == 0) return IO_TIMEOUT;
    if (sw == WAITFD_C && FD_ISSET(*ps, &rfds)) return IO_CLOSED;
#endif
#endif
    return IO_DONE;
}

/*-------------------------------------------------------------------------*\
* Sendto with timeout
\*-------------------------------------------------------------------------*/
int lua_socket_sendto(p_socket ps, const char *data, size_t count,
        size_t *sent, SA *addr, socklen_t addr_len, p_timeout tm){
    int err;
    *sent = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
#ifdef WIN32
        int put = sendto(*ps, data, (int) count, 0, addr, addr_len);
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;
#else
        long put = (long) sendto(*ps, data, count, 0, addr, addr_len);
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        err = errno;
        if (put == 0 || err == EPIPE) return IO_CLOSED;
        if (err == EINTR) continue;
        if (err != EAGAIN) return err;
#endif
        if ((err = lua_socket_waitfd(ps, WAITFD_W, tm)) != IO_DONE) return err;
    }
    return IO_UNKNOWN;
}

int lua_socket_recvfrom(p_socket ps, char *data, size_t count,
        size_t *got, SA *addr, socklen_t *addr_len, p_timeout tm){
    int err;
    *got = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
#ifdef WIN32
        int taken = recvfrom(*ps, data, (int) count, 0, addr, addr_len);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        if (taken == 0) return IO_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;
#else
        long taken = (long) recvfrom(*ps, data, count, 0, addr, addr_len);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        err = errno;
        if (taken == 0) return IO_CLOSED;
        if (err == EINTR) continue;
        if (err != EAGAIN) return err;
#endif
        if ((err = lua_socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err;
    }
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Put socket into non-blocking mode
\*-------------------------------------------------------------------------*/
void lua_socket_setnonblocking(p_socket ps){
#ifdef WIN32
    u_long argp = 1;
    ioctlsocket(*ps, FIONBIO, &argp);
#else
    int flags = fcntl(*ps, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(*ps, F_SETFL, flags);
#endif
}

/*-------------------------------------------------------------------------*\
* Put socket into blocking mode
\*-------------------------------------------------------------------------*/
void lua_socket_setblocking(p_socket ps){
#ifdef WIN32
    u_long argp = 0;
    ioctlsocket(*ps, FIONBIO, &argp);
#else
    int flags = fcntl(*ps, F_GETFL, 0);
    flags &= (~(O_NONBLOCK));
    fcntl(*ps, F_SETFL, flags);
#endif
}

/*-------------------------------------------------------------------------*\
* Select with int timeout in ms
\*-------------------------------------------------------------------------*/
int lua_socket_select(t_socket n, fd_set *rfds, fd_set *wfds, fd_set *efds,
        p_timeout tm){
#ifdef WIN32
    struct timeval tv;
    double t = lua_timeout_get(tm);
    tv.tv_sec = (int) t;
    tv.tv_usec = (int) ((t - tv.tv_sec) * 1.0e6);
    if (n <= 0) {
        Sleep((DWORD) (1000*t));
        return 0;
    } else return select(0, rfds, wfds, efds, t >= 0.0? &tv: NULL);
#else
    int ret;
    do {
        struct timeval tv;
        double t = lua_timeout_getretry(tm);
        tv.tv_sec = (int) t;
        tv.tv_usec = (int) ((t - tv.tv_sec) * 1.0e6);
        /* timeout = 0 means no wait */
        ret = select(n, rfds, wfds, efds, t >= 0.0 ? &tv: NULL);
    } while (ret < 0 && errno == EINTR);
    return ret;
#endif
}

/*-------------------------------------------------------------------------*\
* Connects or returns error message
\*-------------------------------------------------------------------------*/
int lua_socket_connect(p_socket ps, SA *addr, socklen_t addr_len, p_timeout tm){
    int err;
    /* don't call on closed socket */
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
#ifdef WIN32
    /* ask system to connect */
    if (connect(*ps, addr, addr_len) == 0) return IO_DONE;
    /* make sure the system is trying to connect */
    err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) return err;
    /* zero timeout case optimization */
    if (timeout_iszero(tm)) return IO_TIMEOUT;
    /* we wait until something happens */
    err = lua_socket_waitfd(ps, WAITFD_C, tm);
    if (err == IO_CLOSED) {
        int addr_len = sizeof(err);
        /* give windows time to set the error (yes, disgusting) */
        Sleep(10);
        /* find out why we failed */
        getsockopt(*ps, SOL_SOCKET, SO_ERROR, (char *)&err, &addr_len);
        /* we KNOW there was an error. if 'why' is 0, we will return
        * "unknown error", but it's not really our fault */
        return err > 0? err: IO_UNKNOWN;
    } else return err;
#else
    /* call connect until done or failed without being interrupted */
    do if (connect(*ps, addr, addr_len) == 0) return IO_DONE;
    while ((err = errno) == EINTR);
    /* if connection failed immediately, return error code */
    if (err != EINPROGRESS && err != EAGAIN) return err;
    /* zero timeout case optimization */
    if (timeout_iszero(tm)) return IO_TIMEOUT;
    /* wait until we have the result of the connection attempt or timeout */
    err = lua_socket_waitfd(ps, WAITFD_C, tm);
    if (err == IO_CLOSED) {
        if (recv(*ps, (char *) &err, 0, 0) == 0) return IO_DONE;
        else return errno;
    } else return err;
#endif
}

/*-------------------------------------------------------------------------*\
* Creates and sets up a socket
\*-------------------------------------------------------------------------*/
int lua_socket_create(p_socket ps, int domain, int type, int protocol){
    *ps = socket(domain, type, protocol);
    if (*ps != SOCKET_INVALID) return IO_DONE;
#ifdef WIN32
    else return WSAGetLastError();
#else
    else return errno;
#endif
}

/*-------------------------------------------------------------------------*\
* Binds or returns error message
\*-------------------------------------------------------------------------*/
int lua_socket_bind(p_socket ps, SA *addr, socklen_t addr_len){
    int err = IO_DONE;
    lua_socket_setblocking(ps);
    if (bind(*ps, addr, addr_len) < 0)
#ifdef WIN32
	err = WSAGetLastError();
#else
	err = errno;
#endif
    lua_socket_setnonblocking(ps);
    return err;
}

int lua_socket_listen(p_socket ps, int backlog){
    int err = IO_DONE;
    lua_socket_setblocking(ps);
    if (listen(*ps, backlog) < 0)
#ifdef WIN32
	err = WSAGetLastError();
#else
	err = errno;
#endif
    lua_socket_setnonblocking(ps);
    return err;
}

/*-------------------------------------------------------------------------*\
* Accept with timeout
\*-------------------------------------------------------------------------*/
int lua_socket_accept(p_socket ps, p_socket pa, SA *addr,
        socklen_t *addr_len, p_timeout tm){
    SA daddr;
    socklen_t dlen = sizeof(daddr);
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    if (!addr) addr = &daddr;
    if (!addr_len) addr_len = &dlen;
    for ( ;; ) {
        int err;
        /* try to get client socket */
        if ((*pa = accept(*ps, addr, addr_len)) != SOCKET_INVALID) return IO_DONE;
#ifdef WIN32
        /* find out why we failed */
        err = WSAGetLastError();
        /* if we failed because there was no connectoin, keep trying */
        if (err != WSAEWOULDBLOCK && err != WSAECONNABORTED) return err;
#else
        err = errno;
        if (err == EINTR) continue;
        if (err != EAGAIN && err != ECONNABORTED) return err;
#endif
        /* call select to avoid busy wait */
        if ((err = lua_socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err;
    }
    /* can't reach here */
    return IO_UNKNOWN;
}

/* these are perfect to use with the io abstraction module
   and the buffered input module */

/*-------------------------------------------------------------------------*\
* Send with timeout
* On windows, if you try to send 10MB, the OS will buffer EVERYTHING
* this can take an awful lot of time and we will end up blocked.
* Therefore, whoever calls this function should not pass a huge buffer.
\*-------------------------------------------------------------------------*/
int lua_socket_send(p_socket ps, const char *data, size_t count,
        size_t *sent, p_timeout tm){
    int err;
    *sent = 0;
    /* avoid making system calls on closed sockets */
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    /* loop until we send something or we give up on error */
    for ( ;; ) {
#ifdef WIN32
        /* try to send something */
		int put = send(*ps, data, (int) count, 0);
        /* if we sent something, we are done */
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        /* deal with failure */
        err = WSAGetLastError();
        /* we can only proceed if there was no serious error */
        if (err != WSAEWOULDBLOCK) return err;
#else
        long put = (long) send(*ps, data, count, 0);
        /* if we sent anything, we are done */
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        err = errno;
        /* send can't really return 0, but EPIPE means the connection was
           closed */
        if (put == 0 || err == EPIPE) return IO_CLOSED;
        /* we call was interrupted, just try again */
        if (err == EINTR) continue;
        /* if failed fatal reason, report error */
        if (err != EAGAIN) return err;
#endif
        /* avoid busy wait */
        /* wait until we can send something or we timeout */
        if ((err = lua_socket_waitfd(ps, WAITFD_W, tm)) != IO_DONE) return err;
    }
    /* can't reach here */
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Receive with timeout
\*-------------------------------------------------------------------------*/
int lua_socket_recv(p_socket ps, char *data, size_t count, size_t *got, p_timeout tm){
    int err;
    *got = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
#ifdef WIN32
        int taken = recv(*ps, data, (int) count, 0);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        if (taken == 0) return IO_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;
#else
        long taken = (long) recv(*ps, data, count, 0);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        err = errno;
        if (taken == 0) return IO_CLOSED;
        if (err == EINTR) continue;
        if (err != EAGAIN) return err;
#endif
        if ((err = lua_socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err;
    }
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Error translation functions
* Make sure important error messages are standard
\*-------------------------------------------------------------------------*/

#ifdef WIN32
static const char *wstrerror(int err) {
    switch (err) {
        case WSAEINTR: return "Interrupted function call";
        case WSAEACCES: return "Permission denied";
        case WSAEFAULT: return "Bad address";
        case WSAEINVAL: return "Invalid argument";
        case WSAEMFILE: return "Too many open files";
        case WSAEWOULDBLOCK: return "Resource temporarily unavailable";
        case WSAEINPROGRESS: return "Operation now in progress";
        case WSAEALREADY: return "Operation already in progress";
        case WSAENOTSOCK: return "Socket operation on nonsocket";
        case WSAEDESTADDRREQ: return "Destination address required";
        case WSAEMSGSIZE: return "Message too long";
        case WSAEPROTOTYPE: return "Protocol wrong type for socket";
        case WSAENOPROTOOPT: return "Bad protocol option";
        case WSAEPROTONOSUPPORT: return "Protocol not supported";
        case WSAESOCKTNOSUPPORT: return "Socket type not supported";
        case WSAEOPNOTSUPP: return "Operation not supported";
        case WSAEPFNOSUPPORT: return "Protocol family not supported";
        case WSAEAFNOSUPPORT:
            return "Address family not supported by protocol family";
        case WSAEADDRINUSE: return "Address already in use";
        case WSAEADDRNOTAVAIL: return "Cannot assign requested address";
        case WSAENETDOWN: return "Network is down";
        case WSAENETUNREACH: return "Network is unreachable";
        case WSAENETRESET: return "Network dropped connection on reset";
        case WSAECONNABORTED: return "Software caused connection abort";
        case WSAECONNRESET: return "Connection reset by peer";
        case WSAENOBUFS: return "No buffer space available";
        case WSAEISCONN: return "Socket is already connected";
        case WSAENOTCONN: return "Socket is not connected";
        case WSAESHUTDOWN: return "Cannot send after socket shutdown";
        case WSAETIMEDOUT: return "Connection timed out";
        case WSAECONNREFUSED: return "Connection refused";
        case WSAEHOSTDOWN: return "Host is down";
        case WSAEHOSTUNREACH: return "No route to host";
        case WSAEPROCLIM: return "Too many processes";
        case WSASYSNOTREADY: return "Network subsystem is unavailable";
        case WSAVERNOTSUPPORTED: return "Winsock.dll version out of range";
        case WSANOTINITIALISED:
            return "Successful WSAStartup not yet performed";
        case WSAEDISCON: return "Graceful shutdown in progress";
        case WSAHOST_NOT_FOUND: return "Host not found";
        case WSATRY_AGAIN: return "Nonauthoritative host not found";
        case WSANO_RECOVERY: return "Nonrecoverable name lookup error";
        case WSANO_DATA: return "Valid name, no data record of requested type";
        default: return "Unknown error";
    }
}
#endif

const char *lua_socket_strerror(int err){
    if (err <= 0) return lua_io_strerror(err);
#ifdef WIN32
    switch (err) {
        case WSAEADDRINUSE: return "address already in use";
        case WSAECONNREFUSED: return "connection refused";
        case WSAEISCONN: return "already connected";
        case WSAEACCES: return "permission denied";
        case WSAECONNABORTED: return "closed";
        case WSAECONNRESET: return "closed";
        case WSAETIMEDOUT: return "timeout";
        default: return wstrerror(err);
    }
#else
    switch (err) {
        case EADDRINUSE: return "address already in use";
        case EISCONN: return "already connected";
        case EACCES: return "permission denied";
        case ECONNREFUSED: return "connection refused";
        case ECONNABORTED: return "closed";
        case ECONNRESET: return "closed";
		case EPIPE: return "closed";
        case ETIMEDOUT: return "timeout";
        default: return strerror(errno);
    }
#endif
}

const char *lua_socket_ioerror(p_socket ps, int err){
    (void) ps;
    return lua_socket_strerror(err);
}

const char *lua_socket_hoststrerror(int err){
    if (err <= 0) return lua_io_strerror(err);
#ifdef WIN32
    switch (err) {
        case WSAHOST_NOT_FOUND: return "host not found";
        default: return wstrerror(err);
    }
#else
    switch (err) {
        case HOST_NOT_FOUND: return "host not found";
        default: return hstrerror(err);
    }
#endif
}

/*-------------------------------------------------------------------------*\
* DNS helpers
\*-------------------------------------------------------------------------*/
int lua_socket_gethostbyaddr(const char *addr, socklen_t len, struct hostent **hp){
    *hp = gethostbyaddr(addr, len, AF_INET);
    if (*hp) return IO_DONE;
#ifdef WIN32
    else return WSAGetLastError();
#else
    else if (h_errno) return h_errno;
    else if (errno) return errno;
    else return IO_UNKNOWN;
#endif
}

int lua_socket_gethostbyname(const char *addr, struct hostent **hp){
    *hp = gethostbyname(addr);
    if (*hp) return IO_DONE;
#ifdef WIN32
    else return  WSAGetLastError();
#else
    else if (h_errno) return h_errno;
    else if (errno) return errno;
    else return IO_UNKNOWN;
#endif
}

/*=========================================================================*\
* Timeout Exported functions.
\*=========================================================================*/
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

/* min and max macros */
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? x : y)
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? x : y)
#endif
/*-------------------------------------------------------------------------*\
* Initialize structure
\*-------------------------------------------------------------------------*/
void lua_timeout_init(p_timeout tm, double block, double total) {
    tm->block = block;
    tm->total = total;
}

/*-------------------------------------------------------------------------*\
* Determines how much time we have left for the next system call,
* if the previous call was successful
* Input
*   tm: timeout control structure
* Returns
*   the number of ms left or -1 if there is no time limit
\*-------------------------------------------------------------------------*/
double lua_timeout_get(p_timeout tm) {
    if (tm->block < 0.0 && tm->total < 0.0) {
        return -1;
    } else if (tm->block < 0.0) {
        double t = tm->total - lua_timeout_gettime() + tm->start;
        return MAX(t, 0.0);
    } else if (tm->total < 0.0) {
        return tm->block;
    } else {
        double t = tm->total - lua_timeout_gettime() + tm->start;
        return MIN(tm->block, MAX(t, 0.0));
    }
}

/*-------------------------------------------------------------------------*\
* Returns time since start of operation
* Input
*   tm: timeout control structure
* Returns
*   start field of structure
\*-------------------------------------------------------------------------*/
double lua_timeout_getstart(p_timeout tm) {
    return tm->start;
}

/*-------------------------------------------------------------------------*\
* Determines how much time we have left for the next system call,
* if the previous call was a failure
* Input
*   tm: timeout control structure
* Returns
*   the number of ms left or -1 if there is no time limit
\*-------------------------------------------------------------------------*/
double lua_timeout_getretry(p_timeout tm) {
    if (tm->block < 0.0 && tm->total < 0.0) {
        return -1;
    } else if (tm->block < 0.0) {
        double t = tm->total - lua_timeout_gettime() + tm->start;
        return MAX(t, 0.0);
    } else if (tm->total < 0.0) {
        double t = tm->block - lua_timeout_gettime() + tm->start;
        return MAX(t, 0.0);
    } else {
        double t = tm->total - lua_timeout_gettime() + tm->start;
        return MIN(tm->block, MAX(t, 0.0));
    }
}

/*-------------------------------------------------------------------------*\
* Marks the operation start time in structure
* Input
*   tm: timeout control structure
\*-------------------------------------------------------------------------*/
p_timeout lua_timeout_markstart(p_timeout tm) {
    tm->start = lua_timeout_gettime();
    return tm;
}

/*-------------------------------------------------------------------------*\
* Gets time in s, relative to January 1, 1970 (UTC)
* Returns
*   time in s.
\*-------------------------------------------------------------------------*/
#ifdef _WIN32
double lua_timeout_gettime(void) {
    FILETIME ft;
    double t;
#if defined(UNDER_CE)
    // Windows CE does not define GetSystemTimeAsFileTime so we do it in two steps.
    SYSTEMTIME st;
    GetSystemTime( &st );
    SystemTimeToFileTime( &st, &ft );
  #else
    GetSystemTimeAsFileTime( &ft );  // never fails
  #endif
    /* Windows file time (time since January 1, 1601 (UTC)) */
    t  = ft.dwLowDateTime/1.0e7 + ft.dwHighDateTime*(4294967296.0/1.0e7);
    /* convert to Unix Epoch time (time since January 1, 1970 (UTC)) */
    return (t - 11644473600.0);
}
#else
double lua_timeout_gettime(void) {
    struct timeval v;
    gettimeofday(&v, (struct timezone *) NULL);
    /* Unix Epoch time (time since January 1, 1970 (UTC)) */
    return v.tv_sec + v.tv_usec/1.0e6;
}
#endif

/*=========================================================================*\
* IO Exported functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Initializes C structure
\*-------------------------------------------------------------------------*/
void lua_io_init(p_io io, p_send send, p_recv recv, p_error error, void *ctx) {
    io->send = send;
    io->recv = recv;
    io->error = error;
    io->ctx = ctx;
}

/*-------------------------------------------------------------------------*\
* I/O error strings
\*-------------------------------------------------------------------------*/
const char *lua_io_strerror(int err) {
    switch (err) {
        case IO_DONE: return NULL;
        case IO_CLOSED: return "closed";
        case IO_TIMEOUT: return "timeout";
        default: return "unknown error";
    }
}
