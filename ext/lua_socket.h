#ifndef LUA_SOCKET_H
#define LUA_SOCKET_H
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

#ifdef WIN32
/*=========================================================================*\
* Socket compatibilization module for Win32
* LuaSocket toolkit
*
* RCS ID: $Id: wsocket.h,v 1.4 2005/10/07 04:40:59 diego Exp $
\*=========================================================================*/

/*=========================================================================*\
* WinSock include files
\*=========================================================================*/
#include <winsock.h>

typedef int socklen_t;
typedef SOCKET t_socket;
typedef t_socket *p_socket;

#define SOCKET_INVALID (INVALID_SOCKET)
#else
/*=========================================================================*\
* Socket compatibilization module for Unix
* LuaSocket toolkit
*
* RCS ID: $Id: usocket.h,v 1.7 2005/10/07 04:40:59 diego Exp $
\*=========================================================================*/

/*=========================================================================*\
* BSD include files
\*=========================================================================*/
/* error codes */
#include <errno.h>
#include <string.h>
/* close function */
#include <unistd.h>
/* fnctnl function and associated constants */
#include <fcntl.h>
/* struct sockaddr */
#include <sys/types.h>
/* socket function */
#include <sys/socket.h>
/* struct timeval */
#include <sys/time.h>
/* gethostbyname and gethostbyaddr functions */
#include <netdb.h>
/* sigpipe handling */
#include <signal.h>
/* IP stuff*/
#include <netinet/in.h>
#include <arpa/inet.h>
/* TCP options (nagle algorithm disable) */
#include <netinet/tcp.h>

typedef int t_socket;
typedef t_socket *p_socket;

#define SOCKET_INVALID (-1)

#endif


/* we are lazy... */
typedef struct sockaddr SA;

/* timeout control structure */
typedef struct t_timeout_ {
    double block;          /* maximum time for blocking calls */
    double total;          /* total number of miliseconds for operation */
    double start;          /* time of start of operation */
} t_timeout;
typedef t_timeout *p_timeout;

void lua_timeout_init(p_timeout tm, double block, double total);
double lua_timeout_get(p_timeout tm);
double lua_timeout_getretry(p_timeout tm);
p_timeout lua_timeout_markstart(p_timeout tm);
double lua_timeout_getstart(p_timeout tm);
double lua_timeout_gettime(void);

#define timeout_iszero(tm)   ((tm)->block == 0.0)

/* IO error codes */
enum {
    IO_DONE = 0,        /* operation completed successfully */
    IO_TIMEOUT = -1,    /* operation timed out */
    IO_CLOSED = -2,     /* the connection has been closed */
    IO_UNKNOWN = -3,  /* Unknown error */
    IO_SSL = -4       /* SSL error */
};

/* interface to error message function */
typedef const char *(*p_error) (
    void *ctx,          /* context needed by send */
    int err             /* error code */
);

/* interface to send function */
typedef int (*p_send) (
    void *ctx,          /* context needed by send */
    const char *data,   /* pointer to buffer with data to send */
    size_t count,       /* number of bytes to send from buffer */
    size_t *sent,       /* number of bytes sent uppon return */
    p_timeout tm        /* timeout control */
);

/* interface to recv function */
typedef int (*p_recv) (
    void *ctx,          /* context needed by recv */
    char *data,         /* pointer to buffer where data will be writen */
    size_t count,       /* number of bytes to receive into buffer */
    size_t *got,        /* number of bytes received uppon return */
    p_timeout tm        /* timeout control */
);

/* IO driver definition */
typedef struct t_io_ {
    void *ctx;          /* context needed by send/recv */
    p_send send;        /* send function pointer */
    p_recv recv;        /* receive function pointer */
    p_error error;      /* strerror function */
} t_io;
typedef t_io *p_io;

void lua_io_init(p_io io, p_send send, p_recv recv, p_error error, void *ctx);
const char *lua_io_strerror(int err);

/*=========================================================================*\
* Functions bellow implement a comfortable platform independent
* interface to sockets
\*=========================================================================*/
int lua_socket_open(void);
int lua_socket_close(void);
void lua_socket_destroy(p_socket ps);
void lua_socket_shutdown(p_socket ps, int how);
int lua_socket_sendto(p_socket ps, const char *data, size_t count,
        size_t *sent, SA *addr, socklen_t addr_len, p_timeout tm);
int lua_socket_recvfrom(p_socket ps, char *data, size_t count,
        size_t *got, SA *addr, socklen_t *addr_len, p_timeout tm);

void lua_socket_setnonblocking(p_socket ps);
void lua_socket_setblocking(p_socket ps);

int lua_socket_waitfd(p_socket ps, int sw, p_timeout tm);
int lua_socket_select(t_socket n, fd_set *rfds, fd_set *wfds, fd_set *efds,
        p_timeout tm);

int lua_socket_connect(p_socket ps, SA *addr, socklen_t addr_len, p_timeout tm);
int lua_socket_create(p_socket ps, int domain, int type, int protocol);
int lua_socket_bind(p_socket ps, SA *addr, socklen_t addr_len);
int lua_socket_listen(p_socket ps, int backlog);
int lua_socket_accept(p_socket ps, p_socket pa, SA *addr,
        socklen_t *addr_len, p_timeout tm);

const char *lua_socket_hoststrerror(int err);
const char *lua_socket_strerror(int err);

/* these are perfect to use with the io abstraction module
   and the buffered input module */
int lua_socket_send(p_socket ps, const char *data, size_t count,
        size_t *sent, p_timeout tm);
int lua_socket_recv(p_socket ps, char *data, size_t count, size_t *got, p_timeout tm);
const char *lua_socket_ioerror(p_socket ps, int err);

int lua_socket_gethostbyaddr(const char *addr, socklen_t len, struct hostent **hp);
int lua_socket_gethostbyname(const char *addr, struct hostent **hp);

#endif //LUA_SOCKET_H
