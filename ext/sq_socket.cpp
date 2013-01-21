#include "squirrel.h"
#include "sqstdblobimpl.h"

#include <string.h>
#include <stdio.h>
SQ_OPT_STRING_STRLEN();

#include "lua_socket.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

/* option registry */
typedef struct t_opt {
  const char *name;
  int (*func)(HSQUIRRELVM v, p_socket ps);
} t_opt;
typedef t_opt *p_opt;

/*=========================================================================*\
* Internal functions prototypes
\*=========================================================================*/
static int opt_setmembership(HSQUIRRELVM v, p_socket ps, int level, int name);
static int opt_setboolean(HSQUIRRELVM v, p_socket ps, int level, int name);
static int opt_set(HSQUIRRELVM v, p_socket ps, int level, int name,
        void *val, int len);

/*=========================================================================*\
* Exported functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Calls appropriate option handler
\*-------------------------------------------------------------------------*/
static int opt_meth_setoption(HSQUIRRELVM v, p_opt opt, p_socket ps)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, name); /* obj, name, ... */
    while (opt->name && strcmp(name, opt->name))
        opt++;
    if (!opt->func) {
        return sq_throwerror(v, _SC("unsupported option `%.35s'"), name);
    }
    return opt->func(v, ps);
}

/* enables reuse of local address */
static int opt_reuseaddr(HSQUIRRELVM v, p_socket ps)
{
    return opt_setboolean(v, ps, SOL_SOCKET, SO_REUSEADDR);
}

/* disables the Naggle algorithm */
static int opt_tcp_nodelay(HSQUIRRELVM v, p_socket ps)
{
    return opt_setboolean(v, ps, IPPROTO_TCP, TCP_NODELAY);
}

static int opt_keepalive(HSQUIRRELVM v, p_socket ps)
{
    return opt_setboolean(v, ps, SOL_SOCKET, SO_KEEPALIVE);
}

static int opt_dontroute(HSQUIRRELVM v, p_socket ps)
{
    return opt_setboolean(v, ps, SOL_SOCKET, SO_DONTROUTE);
}

static int opt_broadcast(HSQUIRRELVM v, p_socket ps)
{
    return opt_setboolean(v, ps, SOL_SOCKET, SO_BROADCAST);
}

static int opt_ip_multicast_loop(HSQUIRRELVM v, p_socket ps)
{
    return opt_setboolean(v, ps, IPPROTO_IP, IP_MULTICAST_LOOP);
}

static int opt_linger(HSQUIRRELVM v, p_socket ps)
{
    SQ_FUNC_VARS_NO_TOP(v);
    struct linger li;                      /* obj, name, table */
    if(sq_gettype(v, 3) != OT_TABLE) return sq_throwerror(v, _SC("table expected as parameter 2"));
    sq_pushliteral(v, _SC("on"));
    sq_get(v, 3);
    if (!sq_gettype(v, -1) != OT_BOOL)
        return sq_throwerror(v, _SC("boolean 'on' field expected"));
    SQ_GET_BOOL(v, -1, bval);
    li.l_onoff = (u_short) bval == SQTrue;
    sq_pushliteral(v, _SC("timeout"));
    sq_get(v, 3);
    if (!sq_gettype(v, -1) != OT_INTEGER)
        return sq_throwerror(v, _SC("integer 'timeout' field expected"));
    SQ_GET_INTEGER(v, -1, ival);
    li.l_linger = (u_short) ival;
    return opt_set(v, ps, SOL_SOCKET, SO_LINGER, (char *) &li, sizeof(li));
}

static int opt_ip_multicast_ttl(HSQUIRRELVM v, p_socket ps)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 3, val); /* obj, name, int */
    return opt_set(v, ps, SOL_SOCKET, SO_LINGER, (char *) &val, sizeof(val));
}

static int opt_ip_add_membership(HSQUIRRELVM v, p_socket ps)
{
    return opt_setmembership(v, ps, IPPROTO_IP, IP_ADD_MEMBERSHIP);
}

static int opt_ip_drop_membersip(HSQUIRRELVM v, p_socket ps)
{
    return opt_setmembership(v, ps, IPPROTO_IP, IP_DROP_MEMBERSHIP);
}

/*=========================================================================*\
* Auxiliar functions
\*=========================================================================*/

/*-------------------------------------------------------------------------*\
* Some systems do not provide this so that we provide our own. It's not
* marvelously fast, but it works just fine.
\*-------------------------------------------------------------------------*/
#ifdef INET_ATON
static int inet_aton(const char *cp, struct in_addr *inp)
{
    unsigned int a = 0, b = 0, c = 0, d = 0;
    int n = 0, r;
    unsigned long int addr = 0;
    r = sscanf(cp, "%u.%u.%u.%u%n", &a, &b, &c, &d, &n);
    if (r == 0 || n == 0) return 0;
    cp += n;
    if (*cp) return 0;
    if (a > 255 || b > 255 || c > 255 || d > 255) return 0;
    if (inp) {
        addr += a; addr <<= 8;
        addr += b; addr <<= 8;
        addr += c; addr <<= 8;
        addr += d;
        inp->s_addr = htonl(addr);
    }
    return 1;
}
#endif

#ifdef _WIN32
//#define inet_aton inet_addr
//#define inet_aton(a,b) inet_pton(a,b, AF_INET)
#endif

static int opt_setmembership(HSQUIRRELVM v, p_socket ps, int level, int name)
{
    SQ_FUNC_VARS_NO_TOP(v);
    struct ip_mreq val;                   /* obj, name, table */
    if(sq_gettype(v, 3) != OT_TABLE) return sq_throwerror(v, _SC("table expected as parameter 2"));
    sq_pushliteral(v, _SC("multiaddr"));
    sq_get(v, 3);
    if (!sq_gettype(v, -1) != OT_STRING)
        return sq_throwerror(v, _SC("string 'multiaddr' field expected"));
    SQ_GET_STRING(v, -1, multiaddr);
    if (!inet_aton(multiaddr, &val.imr_multiaddr))
        return sq_throwerror(v, _SC("invalid 'multiaddr' ip address"));
    sq_pushliteral(v, _SC("interface"));
    sq_get(v, 3);
    if (!sq_gettype(v, -1) != OT_STRING)
        return sq_throwerror(v, _SC("string 'interface' field expected"));
    val.imr_interface.s_addr = htonl(INADDR_ANY);
    SQ_GET_STRING(v, -1, saddr);
    if (strcmp(saddr, "*") &&
            !inet_aton(saddr, &val.imr_interface))
        return sq_throwerror(v, _SC("invalid 'interface' ip address"));
    return opt_set(v, ps, level, name, (char *) &val, sizeof(val));
}

static
int opt_set(HSQUIRRELVM v, p_socket ps, int level, int name, void *val, int len)
{
    if (setsockopt(*ps, level, name, (char *) val, len) < 0) {
        return sq_throwerror(v, _SC("setsockopt failed"));
    }
    return 0;
}

static int opt_setboolean(HSQUIRRELVM v, p_socket ps, int level, int name)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_BOOL(v, 3, bval);
    int val = bval == SQTrue;             /* obj, name, bool */
    return opt_set(v, ps, level, name, (char *) &val, sizeof(val));
}

/*=========================================================================*\
* Global Lua functions
\*=========================================================================*/

/*-------------------------------------------------------------------------*\
* Sets timeout values for IO operations
* Lua Input: base, time [, mode]
*   time: time out value in seconds
*   mode: "b" for block timeout, "t" for total timeout. (default: b)
\*-------------------------------------------------------------------------*/
static int timeout_meth_settimeout(HSQUIRRELVM v, p_timeout tm) {
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, t, -1);
    SQ_OPT_STRING(v, 3, mode, "b");
    switch (*mode) {
        case _SC('b'):
            tm->block = t;
            break;
        case _SC('r'): case _SC('t'):
            tm->total = t;
            break;
        default:
            return sq_throwerror(v, _SC("invalid timeout mode"));
            break;
    }
    return 0;
}

/*=========================================================================*\
* Test support functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Returns the time the system has been up, in secconds.
\*-------------------------------------------------------------------------*/
static int timeout_lua_gettime(HSQUIRRELVM v)
{
    sq_pushinteger(v, lua_timeout_gettime());
    return 1;
}

/*-------------------------------------------------------------------------*\
* Sleep for n seconds.
\*-------------------------------------------------------------------------*/
static int timeout_lua_sleep(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FLOAT(v, 2, n);
#ifdef _WIN32
    Sleep((int)(n*1000));
#else
    struct timespec t, r;
    t.tv_sec = (int) n;
    n -= t.tv_sec;
    t.tv_nsec = (int) (n * 1000000000);
    if (t.tv_nsec >= 1000000000) t.tv_nsec = 999999999;
    while (nanosleep(&t, &r) != 0) {
        t.tv_sec = r.tv_sec;
        t.tv_nsec = r.tv_nsec;
    }
#endif
    return 0;
}

/*-------------------------------------------------------------------------*\
* Sleep for n Milliseconds.
* Clone of the above for murgaLua.
\*-------------------------------------------------------------------------*/
static int timeout_lua_sleepMillisecs(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FLOAT(v, 2, n);
#ifdef _WIN32
    Sleep((int)(n));
#else
    struct timespec t, r;
    n = n / 1000;
    t.tv_sec = (int) n;
    n -= t.tv_sec;
    t.tv_nsec = (int) (n * 1000000000);
    if (t.tv_nsec >= 1000000000) t.tv_nsec = 999999999;
    while (nanosleep(&t, &r) != 0) {
        t.tv_sec = r.tv_sec;
        t.tv_nsec = r.tv_nsec;
    }
#endif
    return 0;
}

/*=========================================================================*\
* Internal functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Passes all resolver information to Lua as a table
\*-------------------------------------------------------------------------*/
static void inet_pushresolved(HSQUIRRELVM v, struct hostent *hp)
{
    char **alias;
    struct in_addr **addr;
    int i, resolved;
    sq_newtable(v); resolved = sq_gettop(v);
    sq_pushliteral(v, _SC("name"));
    sq_pushstring(v, hp->h_name, -1);
    sq_rawset(v, resolved);
    sq_pushliteral(v, _SC("ip"));
    sq_pushliteral(v, _SC("alias"));
    i = 1;
    alias = hp->h_aliases;
    sq_newarray(v, 0);
    if (alias) {
        while (*alias) {
            sq_pushstring(v, *alias, -1);
            sq_arrayappend(v, -2);
            i++; alias++;
        }
    }
    sq_rawset(v, resolved);
    i = 1;
    sq_newarray(v, 0);
    addr = (struct in_addr **) hp->h_addr_list;
    if (addr) {
        while (*addr) {
            sq_pushstring(v, inet_ntoa(**addr), -1);
            sq_arrayappend(v, -2);
            i++; addr++;
        }
    }
    sq_rawset(v, resolved);
}

/*-------------------------------------------------------------------------*\
* Returns all information provided by the resolver given a host name
* or ip address
\*-------------------------------------------------------------------------*/
static int inet_gethost(const char *address, struct hostent **hp) {
    struct in_addr addr;
    if (inet_aton(address, &addr))
        return lua_socket_gethostbyaddr((char *) &addr, sizeof(addr), hp);
    else
        return lua_socket_gethostbyname(address, hp);
}

/*-------------------------------------------------------------------------*\
* Returns all information provided by the resolver given a host name
* or ip address
\*-------------------------------------------------------------------------*/
static int inet_global_tohostname(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, address);
    struct hostent *hp = NULL;
    int err = inet_gethost(address, &hp);
    if (err != IO_DONE) return sq_throwerror(v, lua_socket_hoststrerror(err));
    sq_newtable(v);
    sq_pushliteral(v, _SC("name"));
    sq_pushstring(v, hp->h_name, -1);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("resolved"));
    inet_pushresolved(v, hp);
    sq_rawset(v, -3);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Returns all information provided by the resolver given a host name
* or ip address
\*-------------------------------------------------------------------------*/
static int inet_global_toip(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, address);
    struct hostent *hp = NULL;
    int err = inet_gethost(address, &hp);
    if (err != IO_DONE) return sq_throwerror(v, lua_socket_hoststrerror(err));
    sq_newtable(v);
    sq_pushliteral(v, _SC("address"));
    sq_pushstring(v, inet_ntoa(*((struct in_addr *) hp->h_addr)), -1);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("resolved"));
    inet_pushresolved(v, hp);
    sq_rawset(v, -3);
    return 1;
}


/*-------------------------------------------------------------------------*\
* Gets the host name
\*-------------------------------------------------------------------------*/
static int inet_global_gethostname(HSQUIRRELVM v)
{
    char name[257];
    name[256] = '\0';
    if (gethostname(name, 256) < 0) return sq_throwerror(v, _SC("gethostname failed"));
    sq_pushstring(v, name, -1);
    return 1;
}



/*=========================================================================*\
* Lua methods
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Retrieves socket peer name
\*-------------------------------------------------------------------------*/
static int inet_meth_getpeername(HSQUIRRELVM v, p_socket ps)
{
    struct sockaddr_in peer;
    socklen_t peer_len = sizeof(peer);
    if (getpeername(*ps, (SA *) &peer, &peer_len) < 0) return sq_throwerror(v, _SC("getpeername failed"));
    sq_newtable(v);
    sq_pushliteral(v, _SC("address"));
    sq_pushstring(v, inet_ntoa(peer.sin_addr), -1);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("port"));
    sq_pushinteger(v, ntohs(peer.sin_port));
    sq_rawset(v, -3);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Retrieves socket local name
\*-------------------------------------------------------------------------*/
static int inet_meth_getsockname(HSQUIRRELVM v, p_socket ps)
{
    struct sockaddr_in local;
    socklen_t local_len = sizeof(local);
    if (getsockname(*ps, (SA *) &local, &local_len) < 0) return sq_throwerror(v, _SC("getsockname failed"));
    sq_newtable(v);
    sq_pushliteral(v, _SC("address"));
    sq_pushstring(v, inet_ntoa(local.sin_addr), -1);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("port"));
    sq_pushinteger(v, ntohs(local.sin_port));
    sq_rawset(v, -3);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Tries to create a new inet socket
\*-------------------------------------------------------------------------*/
static const char *inet_trycreate(p_socket ps, int type) {
    return lua_socket_strerror(lua_socket_create(ps, AF_INET, type, 0));
}

/*-------------------------------------------------------------------------*\
* Tries to connect to remote address (address, port)
\*-------------------------------------------------------------------------*/
static const char *inet_tryconnect(p_socket ps, const char *address,
        unsigned short port, p_timeout tm)
{
    struct sockaddr_in remote;
    int err;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
	if (strcmp(address, "*")) {
        if (!inet_aton(address, &remote.sin_addr)) {
            struct hostent *hp = NULL;
            struct in_addr **addr;
            err = lua_socket_gethostbyname(address, &hp);
            if (err != IO_DONE) return lua_socket_hoststrerror(err);
            addr = (struct in_addr **) hp->h_addr_list;
            memcpy(&remote.sin_addr, *addr, sizeof(struct in_addr));
        }
    } else remote.sin_family = AF_UNSPEC;
    err = lua_socket_connect(ps, (SA *) &remote, sizeof(remote), tm);
    return lua_socket_strerror(err);
}

/*-------------------------------------------------------------------------*\
* Tries to bind socket to (address, port)
\*-------------------------------------------------------------------------*/
static const char *inet_trybind(p_socket ps, const char *address, unsigned short port)
{
    struct sockaddr_in local;
    int err;
    memset(&local, 0, sizeof(local));
    /* address is either wildcard or a valid ip address */
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(port);
    local.sin_family = AF_INET;
    if (strcmp(address, "*") && !inet_aton(address, &local.sin_addr)) {
        struct hostent *hp = NULL;
        struct in_addr **addr;
        err = lua_socket_gethostbyname(address, &hp);
        if (err != IO_DONE) return lua_socket_hoststrerror(err);
        addr = (struct in_addr **) hp->h_addr_list;
        memcpy(&local.sin_addr, *addr, sizeof(struct in_addr));
    }
    err = lua_socket_bind(ps, (SA *) &local, sizeof(local));
    if (err != IO_DONE) lua_socket_destroy(ps);
    return lua_socket_strerror(err);
}

/*=========================================================================*\
* Input/Output interface for Lua programs
* LuaSocket toolkit
*
* Line patterns require buffering. Reading one character at a time involves
* too many system calls and is very slow. This module implements the
* LuaSocket interface for input/output on connected objects, as seen by
* Lua programs.
*
* Input is buffered. Output is *not* buffered because there was no simple
* way of making sure the buffered output data would ever be sent.
*
* The module is built on top of the I/O abstraction defined in io.h and the
* timeout management is done with the timeout.h interface.
*
* Adapted to Squirrel by Domingo Alvarez Duarte on 04/01/2013
*
* RCS ID: $Id: buffer.h,v 1.12 2005/10/07 04:40:59 diego Exp $
\*=========================================================================*/
/* buffer size in bytes */
#define BUF_SIZE 8192

/* buffer control structure */
typedef struct t_buffer_ {
    SQFloat birthday;        /* throttle support info: creation time, */
    SQInteger sent, received;  /* bytes sent, and bytes received */
    p_io io;                /* IO driver used for this buffer */
    p_timeout tm;           /* timeout management for this buffer */
#ifdef LUASOCKET_DEBUG
    int elapsed;
#endif
	SQInteger first, last;     /* index of first and last bytes of stored data */
	char data[BUF_SIZE];    /* storage space for buffer data */
} t_buffer;
typedef t_buffer *p_buffer;

/*=========================================================================*\
* Internal function prototypes
\*=========================================================================*/
static int recvraw(p_buffer buf, size_t wanted, SQBlob &b);
static int recvline(p_buffer buf, SQBlob &b);
static int recvall(p_buffer buf, SQBlob &b);
static int buffer_get(p_buffer buf, const char **data, size_t *count);
static void buffer_skip(p_buffer buf, size_t count);
static int sendraw(p_buffer buf, const char *data, size_t count, size_t *sent);

/* min and max macros */
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? x : y)
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? x : y)
#endif

/*=========================================================================*\
* Exported functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Initializes module
\*-------------------------------------------------------------------------*/
static int buffer_open(HSQUIRRELVM v) {
    (void) v;
    return 0;
}

/*-------------------------------------------------------------------------*\
* Initializes C structure
\*-------------------------------------------------------------------------*/
static void buffer_init(p_buffer buf, p_io io, p_timeout tm) {
	buf->first = buf->last = 0;
    buf->io = io;
    buf->tm = tm;
    buf->received = buf->sent = 0;
    buf->birthday = lua_timeout_gettime();
}

/*-------------------------------------------------------------------------*\
* object:getstats() interface
\*-------------------------------------------------------------------------*/
static int buffer_meth_getstats(HSQUIRRELVM v, p_buffer buf) {
    sq_newtable(v);
    sq_pushliteral(v, _SC("received"));
    sq_pushinteger(v, buf->received);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("sent"));
    sq_pushinteger(v, buf->sent);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("age"));
    sq_pushinteger(v, lua_timeout_gettime() - buf->birthday);
    sq_rawset(v, -3);
    return 1;
}

/*-------------------------------------------------------------------------*\
* object:setstats() interface
\*-------------------------------------------------------------------------*/
static int buffer_meth_setstats(HSQUIRRELVM v, p_buffer buf) {
    SQ_FUNC_VARS(v);
    if(sq_getinteger(v, 2, &buf->received) < 0
        || sq_getinteger(v, 3, &buf->sent) < 0) return SQ_ERROR;
    if(_top_ > 3){
        SQ_GET_INTEGER(v, 4, age);
        buf->birthday = lua_timeout_gettime() - age;
    }
    return 0;
}

/*-------------------------------------------------------------------------*\
* object:send() interface
\*-------------------------------------------------------------------------*/
static int buffer_meth_send(HSQUIRRELVM v, p_buffer buf) {
    SQ_FUNC_VARS(v);
    int err = IO_DONE;
    size_t sent = 0;
    SQ_GET_STRING(v, 2, data);
    SQ_OPT_INTEGER(v, 3, start, 0);
    SQ_OPT_INTEGER(v, 3, end, -1);
#ifdef LUASOCKET_DEBUG
    p_timeout tm = lua_timeout_markstart(buf->tm);
#endif
    if (start < 0) start = (data_size+start+1);
    if (end < 0) end = (data_size+end+1);
    if (start < 0) start = 0;
    if (end > data_size) end = data_size;
    if (start <= end) err = sendraw(buf, data+start, end-start, &sent);
    /* check if there was an error */
    if (err != IO_DONE) {
        return sq_throwerror(v, buf->io->error(buf->io->ctx, err));
    } else {
        sq_pushinteger(v, sent+start-1);
    }
#ifdef LUASOCKET_DEBUG
    /* push time elapsed during operation as the last return value */
    buf->elapsed = timeout_gettime() - timeout_getstart(tm);
#endif
    return 1;
}

/*-------------------------------------------------------------------------*\
* object:receive() interface
\*-------------------------------------------------------------------------*/
static int buffer_meth_receive(HSQUIRRELVM v, p_buffer buf) {
    SQ_FUNC_VARS(v);
    int err = IO_DONE;
    SQBlob b(0, 8192);
    SQ_OPT_STRING(v, 3, prefix, NULL);
#ifdef LUASOCKET_DEBUG
    p_timeout tm = lua_timeout_markstart(buf->tm);
#endif
    /* initialize buffer with optional extra prefix
     * (useful for concatenating previous partial results) */
    b.Write(prefix, prefix_size);
    /* receive new patterns */
    SQObjectType ptype = sq_gettype(v, 2);
    if (ptype == OT_STRING) {
        SQ_GET_STRING(v, 2, p);
        if (p[0] == _SC('*') && p[1] == _SC('l')) err = recvline(buf, b);
        else if (p[0] == _SC('*') && p[1] == _SC('a')) err = recvall(buf, b);
        else return sq_throwerror(v, _SC("invalid receive pattern"));
        /* get a fixed number of bytes (minus what was already partially
         * received) */
    }
    else
    {
        SQ_GET_INTEGER(v, 2, recv_size);
        err = recvraw(buf, (size_t) recv_size-prefix_size, b);
    }
    /* check if there was an error */
    if (err != IO_DONE) {
        return sq_throwerror(v, buf->io->error(buf->io->ctx, err));
        /* we can't push anyting in the stack before pushing the
         * contents of the buffer. this is the reason for the complication */

    } else {
        sq_pushstring(v, (const SQChar*)b.GetBuf(), b.Len());
    }
#ifdef LUASOCKET_DEBUG
    /* push time elapsed during operation as the last return value */
    buf->elapsed = timeout_gettime() - timeout_getstart(tm);
#endif
    return 1;
}

/*-------------------------------------------------------------------------*\
* Determines if there is any data in the read buffer
\*-------------------------------------------------------------------------*/
static int buffer_isempty(p_buffer buf) {
    return buf->first >= buf->last;
}

/*=========================================================================*\
* Internal functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Sends a block of data (unbuffered)
\*-------------------------------------------------------------------------*/
#define STEPSIZE 8192
static int sendraw(p_buffer buf, const char *data, size_t count, size_t *sent) {
    p_io io = buf->io;
    p_timeout tm = buf->tm;
    size_t total = 0;
    int err = IO_DONE;
    while (total < count && err == IO_DONE) {
        size_t done;
        size_t step = (count-total <= STEPSIZE)? count-total: STEPSIZE;
        err = io->send(io->ctx, data+total, step, &done, tm);
        total += done;
    }
    *sent = total;
    buf->sent += total;
    return err;
}

/*-------------------------------------------------------------------------*\
* Reads a fixed number of bytes (buffered)
\*-------------------------------------------------------------------------*/
static int recvraw(p_buffer buf, size_t wanted, SQBlob &b) {
    int err = IO_DONE;
    size_t total = 0;
    while (err == IO_DONE) {
        size_t count; const char *data;
        err = buffer_get(buf, &data, &count);
        count = MIN(count, wanted - total);
        b.Write(data, count);
        buffer_skip(buf, count);
        total += count;
        if (total >= wanted) break;
    }
    return err;
}

/*-------------------------------------------------------------------------*\
* Reads everything until the connection is closed (buffered)
\*-------------------------------------------------------------------------*/
static int recvall(p_buffer buf, SQBlob &b) {
    int err = IO_DONE;
    size_t total = 0;
    while (err == IO_DONE) {
        const char *data; size_t count;
        err = buffer_get(buf, &data, &count);
        total += count;
        b.Write(data, count);
        buffer_skip(buf, count);
    }
    if (err == IO_CLOSED) {
        if (total > 0) return IO_DONE;
        else return IO_CLOSED;
    } else return err;
}

/*-------------------------------------------------------------------------*\
* Reads a line terminated by a CR LF pair or just by a LF. The CR and LF
* are not returned by the function and are discarded from the buffer
\*-------------------------------------------------------------------------*/
static int recvline(p_buffer buf, SQBlob &b) {
    int err = IO_DONE;
    while (err == IO_DONE) {
        size_t count, pos; const char *data;
        err = buffer_get(buf, &data, &count);
        pos = 0;
        while (pos < count && data[pos] != '\n') {
            /* we ignore all \r's */
            if (data[pos] != '\r') b.WriteChar(data[pos]);
            pos++;
        }
        if (pos < count) { /* found '\n' */
            buffer_skip(buf, pos+1); /* skip '\n' too */
            break; /* we are done */
        } else /* reached the end of the buffer */
            buffer_skip(buf, pos);
    }
    return err;
}

/*-------------------------------------------------------------------------*\
* Skips a given number of bytes from read buffer. No data is read from the
* transport layer
\*-------------------------------------------------------------------------*/
static void buffer_skip(p_buffer buf, size_t count) {
    buf->received += count;
    buf->first += count;
    if (buffer_isempty(buf))
        buf->first = buf->last = 0;
}

/*-------------------------------------------------------------------------*\
* Return any data available in buffer, or get more data from transport layer
* if buffer is empty
\*-------------------------------------------------------------------------*/
static int buffer_get(p_buffer buf, const char **data, size_t *count) {
    int err = IO_DONE;
    p_io io = buf->io;
    p_timeout tm = buf->tm;
    if (buffer_isempty(buf)) {
        size_t got;
        err = io->recv(io->ctx, buf->data, BUF_SIZE, &got, tm);
        buf->first = 0;
        buf->last = got;
    }
    *count = buf->last - buf->first;
    *data = buf->data + buf->first;
    return err;
}

/*=========================================================================*\
* Lua methods
\*=========================================================================*/

#define TCP_TYPE_ANY 0
#define TCP_TYPE_CLIENT 1
#define TCP_TYPE_SERVER 2
#define TCP_TYPE_MASTER 3

typedef struct t_tcp_ {
    t_socket sock;
    t_io io;
    t_buffer buf;
    t_timeout tm;
    int type;
} t_tcp;

typedef t_tcp *p_tcp;

static const SQChar SQ_tcp_TAG[] = _SC("sq_tcp");
#define GET_tcp_INSTANCE_AT(v, idx) \
	p_tcp tcp; \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&tcp,(void*)SQ_tcp_TAG)) < 0) return _rc_;

#define GET_tcp_client_INSTANCE_AT(v, idx) GET_tcp_INSTANCE_AT(v, idx)\
	if(tcp->type != TCP_TYPE_CLIENT) return sq_throwerror(v, _SC("tcp client expected"));

#define GET_tcp_server_INSTANCE_AT(v, idx) GET_tcp_INSTANCE_AT(v, idx)\
	if(tcp->type != TCP_TYPE_SERVER) return sq_throwerror(v, _SC("tcp server expected"));

#define GET_tcp_master_INSTANCE_AT(v, idx) GET_tcp_INSTANCE_AT(v, idx)\
	if(tcp->type != TCP_TYPE_MASTER) return sq_throwerror(v, _SC("tcp master expected"));

/*-------------------------------------------------------------------------*\
* Just call buffered IO methods
\*-------------------------------------------------------------------------*/
/* socket tcp option handlers */
static t_opt tcp_opt[] = {
    {"keepalive",   opt_keepalive},
    {"reuseaddr",   opt_reuseaddr},
    {"tcp-nodelay", opt_tcp_nodelay},
    {"linger",      opt_linger},
    {NULL,          NULL}
};

static int tcp_meth_send(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_client_INSTANCE_AT(v, 1);
    return buffer_meth_send(v, &tcp->buf);
}

static int tcp_meth_receive(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_client_INSTANCE_AT(v, 1);
    return buffer_meth_receive(v, &tcp->buf);
}

static int tcp_meth_getstats(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_client_INSTANCE_AT(v, 1);
    return buffer_meth_getstats(v, &tcp->buf);
}

static int tcp_meth_setstats(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_client_INSTANCE_AT(v, 1);
    return buffer_meth_setstats(v, &tcp->buf);
}

/*-------------------------------------------------------------------------*\
* Just call option handler
\*-------------------------------------------------------------------------*/
static int tcp_meth_setoption(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    return opt_meth_setoption(v, tcp_opt, &tcp->sock);
}

/*-------------------------------------------------------------------------*\
* Select support methods
\*-------------------------------------------------------------------------*/
static int tcp_meth_getfd(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    sq_pushinteger(v, (int) tcp->sock);
    return 1;
}

/* this is very dangerous, but can be handy for those that are brave enough */
static int tcp_meth_setfd(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    SQ_GET_INTEGER(v, 2, fd);
    tcp->sock = (t_socket) fd;
    return 0;
}

static int tcp_meth_dirty(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    sq_pushbool(v, !buffer_isempty(&tcp->buf));
    return 1;
}

/*=========================================================================*\
* Library functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Creates a master tcp object
\*-------------------------------------------------------------------------*/
static SQRESULT tcp_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	p_tcp tcp = (p_tcp)p;
	if(tcp) {
	    lua_socket_destroy(&tcp->sock);
	    sq_free(tcp, sizeof(t_tcp));
	}
	return 0;
}

static int tcp_constructor_for_socket(HSQUIRRELVM v, int idx, t_socket sock, int type)
{
    /* allocate tcp object */
    p_tcp tcp = (p_tcp) sq_malloc(sizeof(t_tcp));
    /* set its type as master object */
    tcp->type = type;
    /* initialize remaining structure fields */
    lua_socket_setnonblocking(&sock);
    tcp->sock = sock;
    lua_io_init(&tcp->io, (p_send) lua_socket_send, (p_recv) lua_socket_recv,
            (p_error) lua_socket_ioerror, &tcp->sock);
    lua_timeout_init(&tcp->tm, -1, -1);
    buffer_init(&tcp->buf, &tcp->io, &tcp->tm);
    sq_setinstanceup(v, idx, tcp);
    sq_setreleasehook(v, idx, tcp_releasehook);
    return 1;
}

static int tcp_constructor(HSQUIRRELVM v)
{
    t_socket sock;
    const char *err = inet_trycreate(&sock, SOCK_STREAM);
    /* try to allocate a system socket */
    if (!err) {
        return tcp_constructor_for_socket(v, 1, sock, TCP_TYPE_MASTER);
    }
    return sq_throwerror(v, err);
}

/*-------------------------------------------------------------------------*\
* Waits for and returns a client object attempting connection to the
* server object
\*-------------------------------------------------------------------------*/
static int tcp_meth_accept(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_server_INSTANCE_AT(v, 1);
    p_timeout tm = lua_timeout_markstart(&tcp->tm);
    t_socket sock;
    int err = lua_socket_accept(&tcp->sock, &sock, NULL, NULL, tm);
    /* if successful, push client socket */
    if (err == IO_DONE) {
        sq_pushstring(v,_SC("socket"),-1);
        int rc = sq_getonroottable(v);
        if(rc < 0) return rc;
        sq_pushstring(v,_SC("tcp"),-1);
        rc = sq_rawget(v, -2);
        if(rc < 0) return rc;
        rc = sq_createinstance(v, -1);
        if(rc < 0) return rc;
        tcp_constructor_for_socket(v, sq_gettop(v), sock, TCP_TYPE_CLIENT);
    } else {
        return sq_throwerror(v, lua_socket_strerror(err));
    }
    return 1;
}

/*-------------------------------------------------------------------------*\
* Binds an object to an address
\*-------------------------------------------------------------------------*/
static int tcp_meth_bind(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_master_INSTANCE_AT(v, 1);
    SQ_GET_STRING(v, 2, address);
    SQ_GET_INTEGER(v, 3, port);
    const char *err = inet_trybind(&tcp->sock, address, port);
    if (err) return sq_throwerror(v, err);
    return 0;
}

/*-------------------------------------------------------------------------*\
* Turns a master tcp object into a client object.
\*-------------------------------------------------------------------------*/
static int tcp_meth_connect(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    SQ_GET_STRING(v, 2, address);
    SQ_GET_INTEGER(v, 3, port);
    p_timeout tm = lua_timeout_markstart(&tcp->tm);
    const char *err = inet_tryconnect(&tcp->sock, address, port, tm);
    /* have to set the class even if it failed due to non-blocking connects */
    if (err) return sq_throwerror(v, err);
    tcp->type = TCP_TYPE_CLIENT;
    return 0;
}

/*-------------------------------------------------------------------------*\
* Closes socket used by object
\*-------------------------------------------------------------------------*/
static int tcp_meth_close(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    lua_socket_destroy(&tcp->sock);
    return 0;
}

/*-------------------------------------------------------------------------*\
* Puts the sockt in listen mode
\*-------------------------------------------------------------------------*/
static int tcp_meth_listen(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_tcp_master_INSTANCE_AT(v, 1);
    SQ_OPT_INTEGER(v, 2, backlog, 32);
    int err = lua_socket_listen(&tcp->sock, backlog);
    if (err != IO_DONE) return sq_throwerror(v, lua_socket_strerror(err));
    /* turn master object into a server object */
    tcp->type = TCP_TYPE_SERVER;
    return 0;
}

/*-------------------------------------------------------------------------*\
* Shuts the connection down partially
\*-------------------------------------------------------------------------*/
static int tcp_meth_shutdown(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_tcp_client_INSTANCE_AT(v, 1);
    SQ_OPT_STRING(v, 2, how, _SC("both"));
    switch (how[0]) {
        case _SC('b'):
            if (strcmp(how, "both")) goto error;
            lua_socket_shutdown(&tcp->sock, 2);
            break;
        case _SC('s'):
            if (strcmp(how, "send")) goto error;
            lua_socket_shutdown(&tcp->sock, 1);
            break;
        case _SC('r'):
            if (strcmp(how, "receive")) goto error;
            lua_socket_shutdown(&tcp->sock, 0);
            break;
    }
    return 0;
error:
    return sq_throwerror(v, _SC("invalid shutdown method"));
}

/*-------------------------------------------------------------------------*\
* Just call inet methods
\*-------------------------------------------------------------------------*/
static int tcp_meth_getpeername(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    return inet_meth_getpeername(v, &tcp->sock);
}

static int tcp_meth_getsockname(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    return inet_meth_getsockname(v, &tcp->sock);
}

/*-------------------------------------------------------------------------*\
* Just call tm methods
\*-------------------------------------------------------------------------*/
static int tcp_meth_settimeout(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_tcp_INSTANCE_AT(v, 1);
    return timeout_meth_settimeout(v, &tcp->tm);
}


/*=========================================================================*\
* Lua udp methods
\*=========================================================================*/
/* can't be larger than wsocket.c MAXCHUNK!!! */
#define UDP_DATAGRAMSIZE 8192
#define UDP_TYPE_ANY 0
#define UDP_TYPE_CONNECTED 1
#define UDP_TYPE_UNCONNECTED 2

typedef struct t_udp_ {
    t_socket sock;
    t_timeout tm;
    int type;
} t_udp;
typedef t_udp *p_udp;

/* socket udp options */
static t_opt udp_opt[] = {
    {"dontroute",          opt_dontroute},
    {"broadcast",          opt_broadcast},
    {"reuseaddr",          opt_reuseaddr},
    {"ip-multicast-ttl",   opt_ip_multicast_ttl},
    {"ip-multicast-loop",  opt_ip_multicast_loop},
    {"ip-add-membership",  opt_ip_add_membership},
    {"ip-drop-membership", opt_ip_drop_membersip},
    {NULL,          NULL}
};

static const SQChar SQ_udp_TAG[] = _SC("sq_udp");
#define GET_udp_INSTANCE_AT(v, idx) \
	p_udp udp; \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&udp,(void*)SQ_udp_TAG)) < 0) return _rc_;

#define GET_udp_connected_INSTANCE_AT(v, idx) GET_udp_INSTANCE_AT(v, idx)\
	if(udp->type != UDP_TYPE_CONNECTED) return sq_throwerror(v, _SC("udp connected expected"));

#define GET_udp_unconnected_INSTANCE_AT(v, idx) GET_udp_INSTANCE_AT(v, idx)\
	if(udp->type != UDP_TYPE_UNCONNECTED) return sq_throwerror(v, _SC("udp unconnected expected"));



static const char *udp_strerror(int err) {
    /* a 'closed' error on an unconnected means the target address was not
     * accepted by the transport layer */
    if (err == IO_CLOSED) return "refused";
    else return lua_socket_strerror(err);
}

/*-------------------------------------------------------------------------*\
* Send data through connected udp socket
\*-------------------------------------------------------------------------*/
static int udp_meth_send(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_connected_INSTANCE_AT(v, 1);
    p_timeout tm = &udp->tm;
    size_t sent = 0;
    int err;
    SQ_GET_STRING(v, 2, data);
    lua_timeout_markstart(tm);
    err = lua_socket_send(&udp->sock, data, data_size, &sent, tm);
    if (err != IO_DONE) return sq_throwerror(v, udp_strerror(err));
    sq_pushinteger(v, sent);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Send data through unconnected udp socket
\*-------------------------------------------------------------------------*/
static int udp_meth_sendto(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_unconnected_INSTANCE_AT(v, 1);
    size_t sent = 0;
    SQ_GET_STRING(v, 2, data);
    SQ_GET_STRING(v, 3, ip);
    SQ_GET_INTEGER(v, 4, port);
    p_timeout tm = &udp->tm;
    struct sockaddr_in addr;
    int err;
    memset(&addr, 0, sizeof(addr));
    if (!inet_aton(ip, &addr.sin_addr))
        return sq_throwerror(v, _SC("invalid ip address"));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    lua_timeout_markstart(tm);
    err = lua_socket_sendto(&udp->sock, data, data_size, &sent,
            (SA *) &addr, sizeof(addr), tm);
    if (err != IO_DONE) return sq_throwerror(v, udp_strerror(err));
    sq_pushinteger(v, sent);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Receives data from a UDP socket
\*-------------------------------------------------------------------------*/
static int udp_meth_receive(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    GET_udp_INSTANCE_AT(v, 1);
    char buffer[UDP_DATAGRAMSIZE];
    SQ_OPT_INTEGER(v, 2, count, sizeof(buffer));
    size_t got;
    int err;
    p_timeout tm = &udp->tm;
    count = MIN(count, sizeof(buffer));
    lua_timeout_markstart(tm);
    err = lua_socket_recv(&udp->sock, buffer, count, &got, tm);
    if (err != IO_DONE) return sq_throwerror(v, udp_strerror(err));
    sq_pushstring(v, buffer, got);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Receives data and sender from a UDP socket
\*-------------------------------------------------------------------------*/
static int udp_meth_receivefrom(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    GET_udp_unconnected_INSTANCE_AT(v, 1);
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char buffer[UDP_DATAGRAMSIZE];
    size_t got;
    SQ_OPT_INTEGER(v, 2, count, sizeof(buffer));
    int err;
    p_timeout tm = &udp->tm;
    lua_timeout_markstart(tm);
    count = MIN(count, sizeof(buffer));
    err = lua_socket_recvfrom(&udp->sock, buffer, count, &got,
            (SA *) &addr, &addr_len, tm);
    if (err == IO_DONE) {
        sq_newtable(v);
        sq_pushliteral(v, _SC("data"));
        sq_pushstring(v, buffer, got);
        sq_rawset(v, -3);
        sq_pushliteral(v, _SC("address"));
        sq_pushstring(v, inet_ntoa(addr.sin_addr), -1);
        sq_rawset(v, -3);
        sq_pushliteral(v, _SC("port"));
        sq_pushinteger(v, ntohs(addr.sin_port));
        sq_rawset(v, -3);
        return 1;
    }
    else return sq_throwerror(v, udp_strerror(err));
}

/*-------------------------------------------------------------------------*\
* Select support methods
\*-------------------------------------------------------------------------*/
static int udp_meth_getfd(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    sq_pushinteger(v, (SQInteger) udp->sock);
    return 1;
}

/* this is very dangerous, but can be handy for those that are brave enough */
static int udp_meth_setfd(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    SQ_GET_INTEGER(v, 2, fd);
    udp->sock = (t_socket) fd;
    return 0;
}

static int udp_meth_dirty(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    (void) udp;
    sq_pushbool(v, SQFalse);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Just call inet methods
\*-------------------------------------------------------------------------*/
static int udp_meth_getpeername(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_connected_INSTANCE_AT(v, 1);
    return inet_meth_getpeername(v, &udp->sock);
}

static int udp_meth_getsockname(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    return inet_meth_getsockname(v, &udp->sock);
}

/*-------------------------------------------------------------------------*\
* Just call option handler
\*-------------------------------------------------------------------------*/
static int udp_meth_setoption(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    return opt_meth_setoption(v, udp_opt, &udp->sock);
}

/*-------------------------------------------------------------------------*\
* Just call tm methods
\*-------------------------------------------------------------------------*/
static int udp_meth_settimeout(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    return timeout_meth_settimeout(v, &udp->tm);
}

/*-------------------------------------------------------------------------*\
* Turns a master udp object into a client object.
\*-------------------------------------------------------------------------*/
static int udp_meth_setpeername(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    p_timeout tm = &udp->tm;
    SQ_GET_STRING(v, 2, address);
    int connecting = scstrcmp(address, _SC("*"));
    SQInteger port;
    if(connecting) sq_getinteger(v, 3, &port);
    else sq_optinteger(v, 3, &port, 0);
    const char *err = inet_tryconnect(&udp->sock, address, port, tm);
    if (err) return sq_throwerror(v,  err);
    /* change class to connected or unconnected depending on address */
    if (connecting)
    udp->type = connecting ? UDP_TYPE_CONNECTED : UDP_TYPE_UNCONNECTED;
    return 0;
}

/*-------------------------------------------------------------------------*\
* Closes socket used by object
\*-------------------------------------------------------------------------*/
static int udp_meth_close(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_INSTANCE_AT(v, 1);
    lua_socket_destroy(&udp->sock);
    return 0;
}

/*-------------------------------------------------------------------------*\
* Turns a master object into a server object
\*-------------------------------------------------------------------------*/
static int udp_meth_setsockname(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    GET_udp_unconnected_INSTANCE_AT(v, 1);
    SQ_GET_STRING(v, 2, address);
    SQ_GET_INTEGER(v, 3, port);
    const char *err = inet_trybind(&udp->sock, address, port);
    if (err) return sq_throwerror(v, err);
    return 0;
}

/*=========================================================================*\
* Library functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Creates a master udp object
\*-------------------------------------------------------------------------*/
static SQRESULT udp_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	p_udp udp = (p_udp)p;
	if(udp) {
	    lua_socket_destroy(&udp->sock);
	    sq_free(udp, sizeof(p_udp));
	}
	return 0;
}


static int udp_constructor(HSQUIRRELVM v) {
    t_socket sock;
    const char *err = inet_trycreate(&sock, SOCK_DGRAM);
    /* try to allocate a system socket */
    if (!err) {
        /* allocate tcp object */
        p_udp udp = (p_udp) sq_malloc(sizeof(t_udp));
        udp->type = UDP_TYPE_UNCONNECTED;
        /* initialize remaining structure fields */
        lua_socket_setnonblocking(&sock);
        udp->sock = sock;
        lua_timeout_init(&udp->tm, -1, -1);
        sq_setinstanceup(v, 1, udp);
        sq_setreleasehook(v, 1, udp_releasehook);
        return 1;
    }
    return sq_throwerror(v, err);
}

/*=========================================================================*\
* Internal Select function prototypes.
\*=========================================================================*/
static t_socket getfd(HSQUIRRELVM v);
static int dirty(HSQUIRRELVM v);
static t_socket collect_fd(HSQUIRRELVM v, int tab, t_socket max_fd,
        int itab, fd_set *set);
static int check_dirty(HSQUIRRELVM v, int tab, int dtab, fd_set *set);
static void return_fd(HSQUIRRELVM v, fd_set *set, t_socket max_fd,
        int itab, int tab, int start);
static void make_assoc(HSQUIRRELVM v, int tab);
static int global_select(HSQUIRRELVM v);

/*-------------------------------------------------------------------------*\
* Waits for a set of sockets until a condition is met or timeout.
\*-------------------------------------------------------------------------*/
static int global_select(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    int rtab, wtab, itab, ret, ndirty;
    t_socket max_fd;
    fd_set rset, wset;
    t_timeout tm;
    SQ_OPT_FLOAT(v, 4, t, -1);
    FD_ZERO(&rset); FD_ZERO(&wset);
    sq_settop(v, 4);
    sq_newarray(v, 0); itab = sq_gettop(v);
    sq_newarray(v, 0); rtab = sq_gettop(v);
    sq_newarray(v, 0); wtab = sq_gettop(v);
    max_fd = collect_fd(v, 2, SOCKET_INVALID, itab, &rset);
    ndirty = check_dirty(v, 2, rtab, &rset);
    t = ndirty > 0? 0.0: t;
    lua_timeout_init(&tm, t, -1);
    lua_timeout_markstart(&tm);
    max_fd = collect_fd(v, 3, max_fd, itab, &wset);
    ret = lua_socket_select(max_fd+1, &rset, &wset, NULL, &tm);
    if (ret > 0 || ndirty > 0) {
        return_fd(v, &rset, max_fd+1, itab, rtab, ndirty);
        return_fd(v, &wset, max_fd+1, itab, wtab, 0);
        make_assoc(v, rtab);
        make_assoc(v, wtab);
        sq_newarray(v, 2);
        sq_pushinteger(v, 0);
        sq_push(v, rtab);
        sq_rawset(v, -2);
        sq_pushinteger(v, 1);
        sq_push(v, wtab);
        sq_rawset(v, -2);
        return 1;
    } else if (ret == 0) {
        return sq_throwerror(v, _SC("timeout"));
    }
    return sq_throwerror(v, _SC("error"));
}

/*=========================================================================*\
* Internal functions
\*=========================================================================*/
static t_socket getfd(HSQUIRRELVM v) {
    t_socket fd = SOCKET_INVALID;
    SQInteger top = sq_gettop(v);
    sq_pushliteral(v, _SC("getfd"));
    if(sq_get(v, -2) == SQ_OK){
        SQObjectType ptype = sq_gettype(v, -1);
        if (ptype == OT_CLOSURE || ptype == OT_NATIVECLOSURE) {
            sq_push(v, -2);
            if(sq_call(v, 1, SQTrue, SQFalse) == SQ_OK){
                if (sq_gettype(v, -1) == OT_INTEGER){
                    SQInteger ifd;
                    sq_getinteger(v, -1, &ifd);
                    fd = (t_socket) ifd;
                }
            }
        }
    }
    sq_settop(v, top);
    return fd;
}

static int dirty(HSQUIRRELVM v) {
    int is = 0;
    SQInteger top = sq_gettop(v);
    sq_pushliteral(v, _SC("dirty"));
    if(sq_get(v, -2) == SQ_OK){
        SQObjectType ptype = sq_gettype(v, -1);
        if (ptype == OT_CLOSURE || ptype == OT_NATIVECLOSURE) {
            sq_push(v, -2);
            if(sq_call(v, 1, SQTrue, SQFalse) == SQ_OK){
                if (sq_gettype(v, -1) == OT_INTEGER){
                    SQBool bval;
                    sq_getbool(v, -1, &bval);
                    is = bval == SQTrue;
                }
            }
        }
        sq_pop(v, 1);
    }
    sq_settop(v, top);
    return is;
}

static t_socket collect_fd(HSQUIRRELVM v, int tab, t_socket max_fd,
        int itab, fd_set *set) {
    int i = 0;
    if (sq_gettype(v, tab) == OT_NULL)
        return max_fd;
    while (1) {
        t_socket fd;
        sq_pushinteger(v, i);
        if(sq_rawget(v, tab) == SQ_ERROR) break;
        fd = getfd(v);
        if (fd != SOCKET_INVALID) {
            FD_SET(fd, set);
            if (max_fd == SOCKET_INVALID || max_fd < fd)
                max_fd = fd;
            sq_pushinteger(v, fd);
            sq_push(v, -2);
            sq_rawset(v, itab);
        }
        sq_pop(v, 1);
        i = i + 1;
    }
    return max_fd;
}

static int check_dirty(HSQUIRRELVM v, int tab, int dtab, fd_set *set) {
    int ndirty = 0, i = 0;
    if (sq_gettype(v, tab) == OT_NULL)
        return 0;
    while (1) {
        t_socket fd;
        sq_pushinteger(v, i);
        if(sq_rawget(v, tab) == SQ_ERROR) break;
        fd = getfd(v);
        if (fd != SOCKET_INVALID && dirty(v)) {
            sq_pushinteger(v, ++ndirty);
            sq_push(v, -2);
            sq_rawset(v, dtab);
            FD_CLR(fd, set);
        }
        sq_pop(v, 1);
        i = i + 1;
    }
    return ndirty;
}

static void return_fd(HSQUIRRELVM v, fd_set *set, t_socket max_fd,
        int itab, int tab, int start) {
    t_socket fd;
    for (fd = 0; fd < max_fd; fd++) {
        if (FD_ISSET(fd, set)) {
            sq_pushinteger(v, ++start);
            sq_pushinteger(v, fd);
            sq_rawget(v, itab);
            sq_rawset(v, tab);
        }
    }
}

static void make_assoc(HSQUIRRELVM v, int tab) {
    int i = 0, atab;
    sq_newarray(v, 0); atab = sq_gettop(v);
    while (1) {
        sq_pushinteger(v, i);
        if(sq_rawget(v, tab) == SQ_ERROR) break;
        if (sq_gettype(v, -1) != OT_NULL) {
            sq_pushinteger(v, i);
            sq_push(v, -2);
            sq_rawset(v, atab);
            sq_pushinteger(v, i);
            sq_rawset(v, atab);
        } else {
            sq_pop(v, 1);
            break;
        }
        i = i+1;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_sq_socket(HSQUIRRELVM v)
    {
        sq_pushliteral(v,_SC("socket"));
        sq_newtable(v);
        sq_insertfunc(v, _SC("gethostname"), inet_global_gethostname, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("gettime"), timeout_lua_gettime, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("select"), global_select, -3, _SC(".aan"), SQTrue);
        sq_insertfunc(v, _SC("sleep"), timeout_lua_sleep, 2, _SC(".i"), SQTrue);
        sq_insertfunc(v, _SC("sleepMillisecs"), timeout_lua_sleepMillisecs, 2, _SC(".i"), SQTrue);
        sq_insertfunc(v, _SC("tohostname"), inet_global_tohostname, 2, _SC(".s"), SQTrue);
        sq_insertfunc(v, _SC("toip"), inet_global_toip, 2, _SC(".s"), SQTrue);

        sq_pushliteral(v, _SC("tcp"));
        sq_newclass(v, SQFalse);
        sq_settypetag(v,-1,(void*)SQ_tcp_TAG);
        sq_insertfunc(v, _SC("constructor"), tcp_constructor, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("accept"), tcp_meth_accept, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("bind"), tcp_meth_bind, 3, _SC("xsi"), SQFalse);
        sq_insertfunc(v, _SC("close"), tcp_meth_close, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("connect"), tcp_meth_connect, 3, _SC("xsi"), SQFalse);
        sq_insertfunc(v, _SC("dirty"), tcp_meth_dirty, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("getfd"), tcp_meth_getfd, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("getpeername"), tcp_meth_getpeername, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("getsockname"), tcp_meth_getsockname, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("getstats"), tcp_meth_getstats, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("listen"), tcp_meth_listen, -1, _SC("xi"), SQFalse);
        sq_insertfunc(v, _SC("receive"), tcp_meth_receive, -1, _SC("x i|s s"), SQFalse);
        sq_insertfunc(v, _SC("send"), tcp_meth_send, -2, _SC("xsii"), SQFalse);
        sq_insertfunc(v, _SC("setfd"), tcp_meth_setfd, 2, _SC("xi"), SQFalse);
        sq_insertfunc(v, _SC("setoption"), tcp_meth_setoption, -2, _SC("xs s|n|b"), SQFalse);
        sq_insertfunc(v, _SC("setstats"), tcp_meth_setstats, 4, _SC("xiii"), SQFalse);
        sq_insertfunc(v, _SC("settimeout"), tcp_meth_settimeout, -2, _SC("xns"), SQFalse);
        sq_insertfunc(v, _SC("shutdown"), tcp_meth_shutdown, 2, _SC("xs"), SQFalse);
        sq_newslot(v,-3,SQTrue);

        sq_pushliteral(v, _SC("udp"));
        sq_newclass(v, SQFalse);
        sq_settypetag(v,-1,(void*)SQ_udp_TAG);
        sq_insertfunc(v, _SC("constructor"), udp_constructor, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("close"), udp_meth_close, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("dirty"), udp_meth_dirty, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("getfd"), udp_meth_getfd, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("getpeername"), udp_meth_getpeername, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("getsockname"), udp_meth_getsockname, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("receive"), udp_meth_receive, -1, _SC("xi"), SQFalse);
        sq_insertfunc(v, _SC("receivefrom"), udp_meth_receivefrom, -1, _SC("xi"), SQFalse);
        sq_insertfunc(v, _SC("send"), udp_meth_send, 2, _SC("xs"), SQFalse);
        sq_insertfunc(v, _SC("sendto"), udp_meth_sendto, 4, _SC("xssi"), SQFalse);
        sq_insertfunc(v, _SC("setfd"), udp_meth_setfd, 2, _SC("xi"), SQFalse);
        sq_insertfunc(v, _SC("setpeername"), udp_meth_setpeername, -1, _SC("xsi"), SQFalse);
        sq_insertfunc(v, _SC("setsockname"), udp_meth_setsockname, 3, _SC("xsi"), SQFalse);
        sq_insertfunc(v, _SC("setoption"), udp_meth_setoption, -2, _SC("xs s|n|b"), SQFalse);
        sq_insertfunc(v, _SC("settimeout"), udp_meth_settimeout, 2, _SC("xn"), SQFalse);
        sq_newslot(v,-3,SQTrue);

        sq_newslot(v,-3,SQTrue); //push socket table
        return 0;
    }

#ifdef __cplusplus
}
#endif
