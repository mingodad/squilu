#ifdef __cplusplus
extern "C" {
#endif

//#ifdef USE_NANOMSG

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

#include "NanoMsg.h"

static const SQChar SQ_LIBNAME[] = _SC("NanoMsg");

SQ_OPT_STRING_STRLEN();

static const SQChar SSL_CTX_Tag[]   = _SC("sq_NanoMsg_ctx");

static SQRESULT get_NanoMsg_instance(HSQUIRRELVM v, SQInteger idx, sq_sqlite3_sdb **sdb)
{
    SQRESULT _rc_;
    if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)sdb,(void*)SQLite3_TAG)) < 0) return _rc_;
    if(!*sdb) return sq_throwerror(v, _SC("database is closed"));
    return _rc_;
}

#define GET_NanoMsg_INSTANCE() SQ_GET_INSTANCE(v, 1, NANOMSG, NANOMSG_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("NanoMsg object already closed"));

#define GET_NanoMsg_INSTANCE_at(idx) SQ_GET_INSTANCE(v, idx, NANOMSG, NANOMSG_Tag)


static SQRESULT NanoMsg_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	NANOMSG *self = (NANOMSG*)p;
	if(self) NanoMsg_free(self);
	return 0;
}

static SQRESULT NanoMsg_free(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	GET_ssl_INSTANCE();
	NanoMsg_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0);
	return 0;
}

static SQRESULT NanoMsg_constructor(HSQUIRRELVM v, NANOMSG *NanoMsg, int free_on_gc)
{
    if(!NanoMsg)
        return sq_throwerror(v, _SC("Could'nt create an NanoMsg object."));

    sq_pushstring(v, SQ_LIBNAME, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, NanoMsg_NAME, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, ssl);
                if(free_on_gc) sq_setreleasehook(v,-1, NanoMsg_release_hook);
                return 1;
            }
        }
    }
	return SQ_ERROR;
}


static SQRESULT sq_NanoMsg_allocmsg(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_size);
	SQ_GET_INTEGER(v, 3, int_the_type);
//	void *nn_allocmsg (size_t size, int type)

	return 0;
}

static SQRESULT sq_NanoMsg_bind(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_STRING(v, 2, str_addr);
//	int nn_bind (int s, const char *addr)
    sq_pushinteger(v, nn_bind (self->sock, str_addr));

	return 1;
}

static SQRESULT sq_NanoMsg_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

//	int nn_close (int s)
    sq_pushinteger(v, nn_close (self->sock));

	return 1;
}

static SQRESULT sq_NanoMsg_connect(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_STRING(v, 2, str_addr);
//	int nn_connect (int s, const char *addr)
    sq_pushinteger(v, nn_connect (self->sock, str_addr));

	return 1;
}

static SQRESULT sq_NanoMsg_constructor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_domain);
	SQ_GET_INTEGER(v, 3, int_protocol);
//	int nn_socket (int domain, int protocol)

	return 0;
}

static SQRESULT sq_NanoMsg_destructor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

//	int nn_close (int s)

	return 0;
}

static SQRESULT sq_NanoMsg_device(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	GET_NanoMsg_INSTANCE_at(v, 2);
	GET_NanoMsg_INSTANCE_at(v, 3);
//	int nn_device (int s1, int s2)
    sq_pushinteger(v, nn_device (self->sock, str_addr));

	return 1;
}

static SQRESULT sq_NanoMsg_errno(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

//	int nn_errno (void)

	return 0;
}

static SQRESULT sq_NanoMsg_freemsg(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_STRING(v, 2, msg);
//	int nn_freemsg (void *msg)

	return 0;
}

static SQRESULT sq_NanoMsg_getsockopt(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_level);
	SQ_GET_INTEGER(v, 3, int_option);
	SQ_GET_STRING(v, 4, optval);
	SQ_GET_STRING(v, 5, optvallen);
//	int nn_getsockopt (int s, int level, int option, void *optval, size_t *optvallen)

	return 0;
}

static SQRESULT sq_NanoMsg_pollfd(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	GET_NanoMsg_INSTANCE_at(v, 2, fds);
	GET_NanoMsg_INSTANCE_at(v, 3, int_timeout);
//	int nn_poll (struct nn_pollfd *fds, int nfds, int timeout)

	return 0;
}

static SQRESULT sq_NanoMsg_reallocmsg(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_STRING(v, 2, msg);
	SQ_GET_STRING(v, 3, size);
//	void *nn_reallocmsg (void *msg, size_t size)

	return 0;
}

static SQRESULT sq_NanoMsg_recv(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_flags);
//	int nn_recv (int s, void *buf, size_t len, int flags)

	return 0;
}

static SQRESULT sq_NanoMsg_recvmsg(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_flags);
//	int nn_recvmsg (int s, struct nn_msghdr *msghdr, int flags)

	return 0;
}

static SQRESULT sq_NanoMsg_send(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_STRING(v, 2, str_buf);
	SQ_GET_INTEGER(v, 3, int_flags);
//	int nn_send (int s, const void *buf, size_t len, int flags)

	return 0;
}

static SQRESULT sq_NanoMsg_sendmsg(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_STRING(v, 2, msghdr);
	SQ_GET_STRING(v, 3, flags);
//	int nn_sendmsg (int s, const struct nn_msghdr *msghdr, int flags)

	return 0;
}

static SQRESULT sq_NanoMsg_setsockopt(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_level);
	SQ_GET_INTEGER(v, 3, int_option);
	SQ_GET_STRING(v, 4, optval);
	SQ_GET_STRING(v, 5, optvallen);
//	int nn_setsockopt (int s, int level, int option, const void *optval, size_t optvallen)

	return 0;
}

static SQRESULT sq_NanoMsg_shutdown(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_how);
//	int nn_shutdown (int s, int how)

	return 0;
}

static SQRESULT sq_NanoMsg_socket(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_domain);
	SQ_GET_INTEGER(v, 3, int_protocol);
//	int nn_socket (int domain, int protocol)

	return 0;
}

static SQRESULT sq_NanoMsg_strerror(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_errnum);
//	const char *nn_strerror (int errnum)

	return 0;
}

static SQRESULT sq_NanoMsg_symbol(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_i);
	SQ_GET_STRING(v, 3, value);
//	const char *nn_symbol (int i, int *value)

	return 0;
}

static SQRESULT sq_NanoMsg_symbol_info(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_i);
//	int nn_symbol_info (int i, struct nn_symbol_properties *buf, int buflen)

	return 0;
}

static SQRESULT sq_NanoMsg_tcpmuxd(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_port);
//	int nn_tcpmuxd (int port)

	return 0;
}

static SQRESULT sq_NanoMsg_term(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_NanoMsg_INSTANCE();

//	void nn_term (void)

	return 0;
}

#define _DECL_NANOMSG_FUNC(name,nparams,pmask) {_SC(#name),sq_NanoMsg_##name,nparams,pmask}
static SQRegFunction NanoMsg_obj_funcs[]={

	_DECL_NANOMSG_FUNC(allocmsg, 3, _SC(".ii")),
	_DECL_NANOMSG_FUNC(bind, 2, _SC(".s")),
	_DECL_NANOMSG_FUNC(close, 1, _SC(".")),
	_DECL_NANOMSG_FUNC(connect, 2, _SC(".s")),
	_DECL_NANOMSG_FUNC(constructor, 3, _SC(".ii")),
	_DECL_NANOMSG_FUNC(destructor, 1, _SC(".")),
	_DECL_NANOMSG_FUNC(device, 3, _SC(".ii")),
	_DECL_NANOMSG_FUNC(errno, 1, _SC(".")),
	_DECL_NANOMSG_FUNC(freemsg, 2, _SC("..")),
	_DECL_NANOMSG_FUNC(getsockopt, 5, _SC(".ii..")),
	_DECL_NANOMSG_FUNC(pollfd, 3, _SC("..i")),
	_DECL_NANOMSG_FUNC(reallocmsg, 3, _SC("...")),
	_DECL_NANOMSG_FUNC(recv, 2, _SC(".i")),
	_DECL_NANOMSG_FUNC(recvmsg, 2, _SC(".i")),
	_DECL_NANOMSG_FUNC(send, 3, _SC(".si")),
	_DECL_NANOMSG_FUNC(sendmsg, 3, _SC("...")),
	_DECL_NANOMSG_FUNC(setsockopt, 5, _SC(".ii..")),
	_DECL_NANOMSG_FUNC(shutdown, 2, _SC(".i")),
	_DECL_NANOMSG_FUNC(socket, 3, _SC(".ii")),
	_DECL_NANOMSG_FUNC(strerror, 2, _SC(".i")),
	_DECL_NANOMSG_FUNC(symbol, 3, _SC(".i.")),
	_DECL_NANOMSG_FUNC(symbol_info, 2, _SC(".i")),
	_DECL_NANOMSG_FUNC(tcpmuxd, 2, _SC(".i")),
	_DECL_NANOMSG_FUNC(term, 1, _SC(".")),
	{0,0}
};
#undef _DECL_NANOMSG_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType NanoMsg_constants[] = {
    #define MK_CONST(c) {_SC(#c), c}
    //MK_CONST(SSL_SESSION_ID_SIZE),

	MK_CONST(v, NN_NS_LIMIT);
	MK_CONST(v, NN_SNDPRIO);
	MK_CONST(v, NN_NS_OPTION_LEVEL);
	MK_CONST(v, NN_NS_TRANSPORT_OPTION);
	MK_CONST(v, NN_UNIT_MILLISECONDS);
	MK_CONST(v, NN_RECONNECT_IVL_MAX);
	MK_CONST(v, NN_NS_SOCKET_OPTION);
	MK_CONST(v, NN_TYPE_STR);
	MK_CONST(v, NN_SOL_SOCKET);
	MK_CONST(v, NN_NS_FLAG);
	MK_CONST(v, NN_RCVFD);
	MK_CONST(v, NN_LINGER);
	MK_CONST(v, NN_SNDFD);
	MK_CONST(v, SP_HDR);
	MK_CONST(v, NN_TYPE_INT);
	MK_CONST(v, NN_POLLIN);
	MK_CONST(v, NN_RCVBUF);
	MK_CONST(v, NN_SNDBUF);
	MK_CONST(v, NN_NS_TRANSPORT);
	MK_CONST(v, NN_IPV4ONLY);
	MK_CONST(v, NN_DOMAIN);
	MK_CONST(v, NN_NS_EVENT);
	MK_CONST(v, NN_SOCKADDR_MAX);
	MK_CONST(v, NN_RCVTIMEO);
	MK_CONST(v, NN_NS_OPTION_UNIT);
	MK_CONST(v, NN_NS_PROTOCOL);
	MK_CONST(v, NN_SNDTIMEO);
	MK_CONST(v, NN_NS_DOMAIN);
	MK_CONST(v, NN_SOCKET_NAME);
	MK_CONST(v, NN_PROTOCOL);
	MK_CONST(v, NN_NS_VERSION);
	MK_CONST(v, PROTO_SP);
	MK_CONST(v, NN_TYPE_NONE);
	MK_CONST(v, NN_NS_OPTION_TYPE);
	MK_CONST(v, NN_NS_ERROR);
	MK_CONST(v, NN_POLLOUT);
	MK_CONST(v, NN_DONTWAIT);
	MK_CONST(v, NN_MSG);
	MK_CONST(v, NN_UNIT_PRIORITY);
	MK_CONST(v, NN_UNIT_NONE);
	MK_CONST(v, NN_RECONNECT_IVL);
	MK_CONST(v, NN_RCVPRIO);
	MK_CONST(v, AF_SP_RAW);
	MK_CONST(v, NN_UNIT_BYTES);
	MK_CONST(v, NN_UNIT_BOOLEAN);
	MK_CONST(v, NN_NS_NAMESPACE);
	MK_CONST(v, AF_SP);
    {0,0}
};

/* This defines a function that opens up your library. */
SQRESULT sqext_register_NanoMsg (HSQUIRRELVM v) {
	//add a namespace NanoMsg
	sq_pushstring(v, SQ_LIBNAME, -1);
	sq_newtable(v);

	sq_insert_reg_funcs(v, NanoMsg_obj_funcs);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = NanoMsg_constants; KeyIntPtr->Str; KeyIntPtr++) {
		sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
		sq_pushinteger(v, KeyIntPtr->Val);       //then the value
		sq_newslot(v, -3, SQFalse);              //store then
	}

	sq_newslot(v,-3,SQFalse); //add NanoMsg table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}

#endif //USE_NANOMSG

#endif
