#include "squirrel.h"
//#include "sqstdblobimpl.h"
#include "zmq.h"
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

SQ_OPT_STRING_STRLEN();

static SQRESULT zmq_throwerror(HSQUIRRELVM v){
    return sq_throwerror(v, zmq_strerror(zmq_errno()));
}

static SQRESULT table_set_int(HSQUIRRELVM v, const SQChar *key, SQInteger value){
    sq_pushliteral(v, key);
    sq_pushinteger(v, value);
    return sq_rawset(v, -3);
}

static const SQChar sq_zmq3_ctx_TAG[] = _SC("sq_zmq3_ctx_tag");

#define GET_zmq3_ctx_INSTANCE_VAR_AT(idx, Var) \
    SQ_GET_INSTANCE_VAR(v, idx, SQUserPointer, Var, sq_zmq3_ctx_TAG);\
    if(!Var) return sq_throwerror(v, _SC("zmq_ctx already destroyed"));
#define GET_zmq3_ctx_INSTANCE() GET_zmq3_ctx_INSTANCE_VAR_AT(1, self)

static const SQChar sq_zmq3_socket_TAG[] = _SC("sq_zmq3_socket_tag");

#define GET_zmq3_socket_INSTANCE_VAR_AT(idx, Var) \
    SQ_GET_INSTANCE_VAR(v, idx, zmq_msg_t, Var, sq_zmq3_socket_TAG);\
    if(!Var) return sq_throwerror(v, _SC("socket already closed"));
#define GET_zmq3_socket_INSTANCE() GET_zmq3_socket_INSTANCE_VAR_AT(1, self)

static const SQChar sq_zmq3_msg_TAG[] = _SC("sq_zmq3_msg_tag");

#define GET_zmq3_msg_INSTANCE_VAR_AT(idx, Var) \
    SQ_GET_INSTANCE_VAR(v, idx, zmq_msg_t, Var, sq_zmq3_msg_TAG);\
    if(!Var) return sq_throwerror(v, _SC("message already closed"));
#define GET_zmq3_msg_INSTANCE() GET_zmq3_msg_INSTANCE_VAR_AT(1, self)

static SQRESULT sq_zmq3_msg_releasehook(SQUserPointer p, SQInteger size)
{
	zmq_msg_t *self = ((zmq_msg_t *)p);
	if(self) {
	    int rc = zmq_msg_close(self);
	    if(rc != 0) return rc;
	    sq_free(self, sizeof(zmq_msg_t));
	}
	return 0;
}

static SQRESULT sq_zmq3_msg_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    zmq_msg_t *msg = (zmq_msg_t*)sq_malloc(sizeof(zmq_msg_t));
    int rc;
    switch(_top_){
        case 1:
            rc = zmq_msg_init(msg);
        break;
        case 2:{
            SQ_GET_INTEGER(v, 2, size);
            rc = zmq_msg_init_size(msg, size);
        }
        break;
        default:
            return sq_throwerror(v, _SC("zmq_msg accepts between 0 and 1 parameters"));
    }
    if(rc) return zmq_throwerror(v);
    sq_setinstanceup(v, 1, msg);
    sq_setreleasehook(v,1, sq_zmq3_msg_releasehook);
	return 1;
}

static SQRESULT sq_zmq3_msg_send(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    GET_zmq3_socket_INSTANCE_VAR_AT(2, socket);
    SQ_GET_INTEGER(v, 3, flags);
    sq_pushinteger(v, zmq_msg_send(self, socket, flags));
	return 1;
}

static SQRESULT sq_zmq3_msg_recv(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    GET_zmq3_socket_INSTANCE_VAR_AT(2, socket);
    SQ_GET_INTEGER(v, 3, flags);
    sq_pushinteger(v, zmq_msg_recv(self, socket, flags));
	return 1;
}

static SQRESULT sq_zmq3_msg_close(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    int rc = sq_zmq3_msg_releasehook(self, 0);
    if(rc == 0) sq_setinstanceup(v, 1, 0); //invalidate the message
    sq_pushinteger(v, rc);
	return 1;
}

static SQRESULT sq_zmq3_msg_copy(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    GET_zmq3_msg_INSTANCE_VAR_AT(2, src);
    sq_pushinteger(v, zmq_msg_copy(self, src));
	return 1;
}

static SQRESULT sq_zmq3_msg_data(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_zmq3_msg_INSTANCE();
    if(_top_ > 1){
        SQ_GET_STRING(v, 2, data);
        if(data_size > zmq_msg_size(self)) return sq_throwerror(v, _SC("data size bigger than available space"));
        memcpy(zmq_msg_data(self), data, data_size);
        return 0;
    } else {
        sq_pushstring(v, (const SQChar*)zmq_msg_data(self), zmq_msg_size(self));
    }
	return 1;
}

static SQRESULT sq_zmq3_msg_size(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    sq_pushinteger(v, zmq_msg_size(self));
	return 1;
}

static SQRESULT sq_zmq3_msg_more(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    sq_pushinteger(v, zmq_msg_more(self));
	return 1;
}

static SQRESULT sq_zmq3_msg_set(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    SQ_GET_INTEGER(v, 2, option);
    SQ_GET_INTEGER(v, 3, optval);
    sq_pushinteger(v, zmq_msg_set(self, option, optval));
	return 1;
}

static SQRESULT sq_zmq3_msg_get(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_msg_INSTANCE();
    SQ_GET_INTEGER(v, 2, option);
    sq_pushinteger(v, zmq_msg_get(self, option));
	return 1;
}


#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_zmq3_msg_##name,nparams,tycheck}
static SQRegFunction sq_zmq3_msg_methods[] =
{
	_DECL_FUNC(constructor,  -1, _SC("x i|s i")),
	_DECL_FUNC(send,  3, _SC("xxi")),
	_DECL_FUNC(recv,  3, _SC("xxi")),
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(copy,  2, _SC("xx")),
	_DECL_FUNC(data,  -1, _SC("xs")),
	_DECL_FUNC(size,  1, _SC("x")),
	_DECL_FUNC(more,  1, _SC("x")),
	_DECL_FUNC(set,  3, _SC("xii")),
	_DECL_FUNC(get,  2, _SC("xi")),
	{0,0}
};
#undef _DECL_FUNC

//socket

static SQRESULT sq_zmq3_socket_releasehook(SQUserPointer p, SQInteger size)
{
	void *self = ((void *)p);
	if(self) {
	    return zmq_close(self);
	}
	return 0;
}

static SQRESULT sq_zmq3_socket_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_ctx_INSTANCE_VAR_AT(2, ctx);
    SQ_GET_INTEGER(v, 3, type);
    switch(type){
        case ZMQ_PAIR :
        case ZMQ_PUB :
        case ZMQ_SUB :
        case ZMQ_REQ :
        case ZMQ_REP :
        case ZMQ_DEALER :
        case ZMQ_ROUTER :
        case ZMQ_PULL :
        case ZMQ_PUSH :
        case ZMQ_XPUB :
        case ZMQ_XSUB :
        break;
        default:
            return sq_throwerror(v, _SC("invalid socket type %d"), type);
    }
    void *socket = zmq_socket(ctx, type);
    if(!socket) return zmq_throwerror(v);
    sq_setinstanceup(v, 1, socket);
    sq_setreleasehook(v,1, sq_zmq3_socket_releasehook);
	return 1;
}

static SQRESULT sq_zmq3_socket_close(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    int rc = sq_zmq3_socket_releasehook(self, 0);
    if(rc == 0) sq_setinstanceup(v, 1, 0);
    sq_pushinteger(v, rc);
	return 1;
}


static int check_socket_option(int option){
    switch(option){
        case ZMQ_AFFINITY :
        case ZMQ_IDENTITY :
        case ZMQ_SUBSCRIBE :
        case ZMQ_UNSUBSCRIBE :
        case ZMQ_RATE :
        case ZMQ_RECOVERY_IVL :
        case ZMQ_SNDBUF :
        case ZMQ_RCVBUF :
        case ZMQ_RCVMORE :
        case ZMQ_FD :
        case ZMQ_EVENTS :
        case ZMQ_TYPE :
        case ZMQ_LINGER :
        case ZMQ_RECONNECT_IVL :
        case ZMQ_BACKLOG :
        case ZMQ_RECONNECT_IVL_MAX :
        case ZMQ_MAXMSGSIZE :
        case ZMQ_SNDHWM :
        case ZMQ_RCVHWM :
        case ZMQ_MULTICAST_HOPS :
        case ZMQ_RCVTIMEO :
        case ZMQ_SNDTIMEO :
        case ZMQ_IPV4ONLY :
        case ZMQ_LAST_ENDPOINT :
        case ZMQ_ROUTER_MANDATORY :
        case ZMQ_TCP_KEEPALIVE :
        case ZMQ_TCP_KEEPALIVE_CNT :
        case ZMQ_TCP_KEEPALIVE_IDLE :
        case ZMQ_TCP_KEEPALIVE_INTVL :
        case ZMQ_TCP_ACCEPT_FILTER :
        case ZMQ_DELAY_ATTACH_ON_CONNECT :
        case ZMQ_XPUB_VERBOSE :
        break;
        default:
            return -1;
    }
    return 0;
}

static SQRESULT sq_zmq3_socket_getsetsockopt(HSQUIRRELVM v, bool isSet)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    SQ_GET_INTEGER(v, 2, option);

    if(check_socket_option(option)) return sq_throwerror(v, _SC("invalid socket option %d"), option);
    SQInteger rc;

    switch (option) {
    case ZMQ_AFFINITY:
    case ZMQ_MAXMSGSIZE:
        {
            SQ_GET_INTEGER(v, 3, ioptval);
            int64_t optval = (int64_t) ioptval;
            size_t optvallen = sizeof(int64_t);
            if(isSet) rc = zmq_setsockopt(self, option, (void *) &optval, optvallen);
            else
            {
                rc = zmq_getsockopt(self, option, (void *) &optval, &optvallen);
                if(rc == 0) sq_pushinteger(v, optval);
            }
        }
        break;
    case ZMQ_TYPE:
    case ZMQ_RCVMORE:
    case ZMQ_EVENTS:
    case ZMQ_SNDHWM:
    case ZMQ_RCVHWM:
    case ZMQ_RATE:
    case ZMQ_RECOVERY_IVL:
    case ZMQ_SNDBUF:
    case ZMQ_RCVBUF:
    case ZMQ_LINGER:
    case ZMQ_RECONNECT_IVL:
    case ZMQ_RECONNECT_IVL_MAX:
    case ZMQ_BACKLOG:
    case ZMQ_MULTICAST_HOPS:
    case ZMQ_RCVTIMEO:
    case ZMQ_SNDTIMEO:
    case ZMQ_IPV4ONLY:
    case ZMQ_DELAY_ATTACH_ON_CONNECT:
    case ZMQ_TCP_KEEPALIVE:
    case ZMQ_TCP_KEEPALIVE_CNT:
    case ZMQ_TCP_KEEPALIVE_IDLE:
    case ZMQ_TCP_KEEPALIVE_INTVL:
        {
            if(isSet){
                SQ_GET_INTEGER(v, 3, ioptval);
                rc = zmq_setsockopt(self, option, (void *) &ioptval, sizeof(ioptval));
            }
            else
            {
                int ioptval;
                rc = zmq_setsockopt(self, option, (void *) &ioptval, sizeof(ioptval));
                if(rc == 0) sq_pushinteger(v, ioptval);
            }
        }
        break;
    case ZMQ_IDENTITY:
    case ZMQ_TCP_ACCEPT_FILTER:
    case ZMQ_SUBSCRIBE:
    case ZMQ_UNSUBSCRIBE:
        {
            SQ_GET_STRING(v, 3, optval);
            if(isSet) rc = zmq_setsockopt(self, option, (void *) optval, optval_size);
            else
            {
                char buf[256];
                size_t optvallen = sizeof(buf);
                memset(buf, 0, optvallen);
                rc = zmq_getsockopt(self, option, (void *)buf, &optvallen);
                buf[sizeof(buf)-1] = '\0';
                if (rc == 0) {
                    sq_pushstring(v, buf, -1);
                    return 1;
                }
            }
        }
        break;
    //case case ZMQ_EVENTS:
    //case case ZMQ_FD:
    //case case ZMQ_LAST_ENDPOINT:
    default:
        rc = -1;
        errno = EINVAL;
    }

    if (rc != 0) return sq_throwerror(v, zmq_strerror(rc));
	return 0;
}

static SQRESULT sq_zmq3_socket_setsockopt(HSQUIRRELVM v)
{
    return sq_zmq3_socket_getsetsockopt(v, true);
}

static SQRESULT sq_zmq3_socket_getsockopt(HSQUIRRELVM v)
{
    return sq_zmq3_socket_getsetsockopt(v, false);
}

static SQRESULT sq_zmq3_socket_bind(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    SQ_GET_STRING(v, 2, addr);
    sq_pushinteger(v, zmq_bind(self, addr));
	return 1;
}

static SQRESULT sq_zmq3_socket_unbind(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    SQ_GET_STRING(v, 2, addr);
    sq_pushinteger(v, zmq_unbind(self, addr));
	return 1;
}

static SQRESULT sq_zmq3_socket_connect(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    SQ_GET_STRING(v, 2, addr);
    sq_pushinteger(v, zmq_connect(self, addr));
	return 1;
}

static SQRESULT sq_zmq3_socket_disconnect(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    SQ_GET_STRING(v, 2, addr);
    sq_pushinteger(v, zmq_disconnect(self, addr));
	return 1;
}

static SQRESULT sq_zmq3_socket_send(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_zmq3_socket_INSTANCE();
    SQ_GET_STRING(v, 2, data);
/*
    SQ_OPT_INTEGER(v, 3, data_len, data_size);
    SQ_OPT_INTEGER(v, 4, flags, 0);
    sq_pushinteger(v, zmq_send(self, data, data_len ? data_len : data_size, flags));
*/
    SQ_OPT_INTEGER(v, 3, flags, 0);
    zmq_msg_t msg;
    if(zmq_msg_init_size(&msg, data_size)) return zmq_throwerror(v);
    memcpy(zmq_msg_data(&msg), data, data_size);

    int rc = zmq_sendmsg(self, &msg, flags);

    if(zmq_msg_close(&msg)) return zmq_throwerror(v);

    sq_pushinteger(v, rc);

	return 1;
}

static SQRESULT sq_zmq3_socket_sendmsg(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    GET_zmq3_msg_INSTANCE_VAR_AT(2, msg)
    SQ_GET_INTEGER(v, 3, flags);
    sq_pushinteger(v, zmq_sendmsg(self, msg, flags));
	return 1;
}

static SQRESULT sq_zmq3_socket_recv(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_zmq3_socket_INSTANCE();
/*
    SQ_GET_INSTANCE_VAR(v, 2, SQBlob, blob, SQBlob::SQBlob_TAG);
    SQ_OPT_INTEGER(v, 3, size);
    SQ_GET_INTEGER(v, 4, flags);
    if(!blob->SetLen(size)) sq_throwerror(v, _SC("could not set blob size to %d"), size);
    SQInteger nbytes = zmq_recv(self, blob->GetBuf(), size, flags);
    blob->SetLen(nbytes);
    sq_pushinteger(v, nbytes);
*/
    SQ_OPT_INTEGER(v, 2, flags, 0);

    zmq_msg_t msg;
    if(zmq_msg_init(&msg)) return zmq_throwerror(v);

    int nbytes = zmq_recvmsg(self, &msg, flags);
    if(nbytes < 0) {
        // Best we can do in this case is try to close and hope for the best.
        zmq_msg_close(&msg);
        return zmq_throwerror(v);
    }

    sq_pushstring(v, (const SQChar*)zmq_msg_data(&msg), zmq_msg_size(&msg));

    if(zmq_msg_close(&msg)) {
        // Above string will be poped from the stack by the normalising code
        // upon sucessful return.
        return zmq_throwerror(v);
    }
	return 1;
}

static SQRESULT sq_zmq3_socket_recvmsg(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_socket_INSTANCE();
    GET_zmq3_msg_INSTANCE_VAR_AT(2, msg)
    SQ_GET_INTEGER(v, 3, flags);
    sq_pushinteger(v, zmq_recvmsg(self, msg, flags));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_zmq3_socket_##name,nparams,tycheck}
static SQRegFunction sq_zmq3_socket_methods[] =
{
	_DECL_FUNC(constructor,  3, _SC("xxi")),
	_DECL_FUNC(setsockopt,  3, _SC("xi s|i")),
	_DECL_FUNC(getsockopt,  2, _SC("xi")),
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(bind,  2, _SC("xs")),
	_DECL_FUNC(unbind,  2, _SC("xs")),
	_DECL_FUNC(connect,  2, _SC("xs")),
	_DECL_FUNC(disconnect,  2, _SC("xs")),
	_DECL_FUNC(send,  -2, _SC("xsi")),
	_DECL_FUNC(sendmsg,  3, _SC("xxi")),
	_DECL_FUNC(recv,  -1, _SC("xi")),
	_DECL_FUNC(recvmsg,  3, _SC("xxi")),
	{0,0}
};
#undef _DECL_FUNC


//zmq3 context

static SQRESULT sq_zmq3_ctx_releasehook(SQUserPointer p, SQInteger size)
{
	void *self = ((void *)p);
	if(self) {
	    return zmq_ctx_destroy(self);
	}
	return 0;
}

static SQRESULT sq_zmq3_ctx_constructor(HSQUIRRELVM v)
{
    void *self = zmq_ctx_new();
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, sq_zmq3_ctx_releasehook);
	return 1;
}

static SQRESULT sq_zmq3_ctx_version(HSQUIRRELVM v)
{
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    sq_newtable(v);
    table_set_int(v, _SC("major"), major);
    table_set_int(v, _SC("minor"), minor);
    table_set_int(v, _SC("patch"), patch);
	return 1;
}

static SQRESULT sq_zmq3_ctx_errno(HSQUIRRELVM v)
{
    sq_pushinteger(v, zmq_errno());
	return 1;
}

static SQRESULT sq_zmq3_ctx_strerror(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_ctx_INSTANCE();
    SQ_GET_INTEGER(v, 2, errnum);
    sq_pushstring(v, zmq_strerror(errnum), -1);
	return 1;
}

static int check_ctx_options(int option){
    switch(option){
        case ZMQ_IO_THREADS:
        case ZMQ_MAX_SOCKETS:
            return 0;
    };
    return -1;
}

static SQRESULT sq_zmq3_ctx_getset(HSQUIRRELVM v, bool isSet)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_ctx_INSTANCE();
    SQ_GET_INTEGER(v, 2, option);
    SQ_GET_INTEGER(v, 3, optval);
    if(check_ctx_options(option)) return sq_throwerror(v, _SC("invalid context option \"%d\""), option);
    if(isSet) sq_pushinteger(v, zmq_ctx_set(self, option, optval));
    else sq_pushinteger(v, zmq_ctx_get(self, option));
	return 1;
}

static SQRESULT sq_zmq3_ctx_set(HSQUIRRELVM v)
{
    return sq_zmq3_ctx_getset(v, true);
}

static SQRESULT sq_zmq3_ctx_get(HSQUIRRELVM v)
{
    return sq_zmq3_ctx_getset(v, false);
}

static SQRESULT sq_zmq3_ctx_socket(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_ctx_INSTANCE();
    SQ_GET_INTEGER(v, 2, type);
    sq_pushroottable(v);
    if(sq_getbyname(v, -1, _SC("zmq_socket"), -1) == SQ_OK){
        sq_pushroottable(v); //this
        sq_push(v, 1); //zmq_ctx
        sq_push(v, 2); //socket type
        if(sq_call(v, 3, SQTrue, SQFalse) == SQ_OK) return 1;
    }
	return SQ_ERROR;
}

static SQRESULT sq_zmq3_ctx_destroy(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_zmq3_ctx_INSTANCE();
    int rc = sq_zmq3_ctx_releasehook(self, 0);
    if(rc == 0) sq_setinstanceup(v, 1, 0);
    sq_pushinteger(v, rc);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_zmq3_ctx_##name,nparams,tycheck}
static SQRegFunction sq_zmq3_ctx_methods[] =
{
	_DECL_FUNC(constructor,  1, _SC("x")),
	_DECL_FUNC(version,  1, _SC("x")),
	_DECL_FUNC(errno,  1, _SC("x")),
	_DECL_FUNC(strerror,  2, _SC("xi")),
	_DECL_FUNC(set,  3, _SC("xii")),
	_DECL_FUNC(get,  2, _SC("xi")),
	_DECL_FUNC(socket,  2, _SC("xi")),
	_DECL_FUNC(destroy,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC

#define INT_CONST(v,num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,ZMQ_##num);sq_newslot(v,-3,SQTrue);

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqstd_register_sq_zmq3(HSQUIRRELVM v)
    {
        sq_pushliteral(v,_SC("zmq_ctx"));
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)sq_zmq3_ctx_TAG);
        sq_insert_reg_funcs(v, sq_zmq3_ctx_methods);

        INT_CONST(v,IO_THREADS);
        INT_CONST(v,MAX_SOCKETS);
        INT_CONST(v,IO_THREADS_DFLT);
        INT_CONST(v,MAX_SOCKETS_DFLT);

        sq_newslot(v,-3,SQTrue);


        sq_pushliteral(v,_SC("zmq_socket"));
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)sq_zmq3_socket_TAG);
        sq_insert_reg_funcs(v, sq_zmq3_socket_methods);

        INT_CONST(v,PAIR);
        INT_CONST(v,PUB);
        INT_CONST(v,SUB);
        INT_CONST(v,REQ);
        INT_CONST(v,REP);
        INT_CONST(v,DEALER);
        INT_CONST(v,ROUTER);
        INT_CONST(v,PULL);
        INT_CONST(v,PUSH);
        INT_CONST(v,XPUB);
        INT_CONST(v,XSUB);

        INT_CONST(v,AFFINITY );
        INT_CONST(v,IDENTITY );
        INT_CONST(v,SUBSCRIBE );
        INT_CONST(v,UNSUBSCRIBE );
        INT_CONST(v,RATE );
        INT_CONST(v,RECOVERY_IVL );
        INT_CONST(v,SNDBUF );
        INT_CONST(v,RCVBUF );
        INT_CONST(v,RCVMORE );
        INT_CONST(v,FD );
        INT_CONST(v,EVENTS );
        INT_CONST(v,TYPE );
        INT_CONST(v,LINGER );
        INT_CONST(v,RECONNECT_IVL );
        INT_CONST(v,BACKLOG );
        INT_CONST(v,RECONNECT_IVL_MAX );
        INT_CONST(v,MAXMSGSIZE );
        INT_CONST(v,SNDHWM );
        INT_CONST(v,RCVHWM );
        INT_CONST(v,MULTICAST_HOPS );
        INT_CONST(v,RCVTIMEO );
        INT_CONST(v,SNDTIMEO );
        INT_CONST(v,IPV4ONLY );
        INT_CONST(v,LAST_ENDPOINT );
        INT_CONST(v,ROUTER_MANDATORY );
        INT_CONST(v,TCP_KEEPALIVE );
        INT_CONST(v,TCP_KEEPALIVE_CNT );
        INT_CONST(v,TCP_KEEPALIVE_IDLE );
        INT_CONST(v,TCP_KEEPALIVE_INTVL );
        INT_CONST(v,TCP_ACCEPT_FILTER );
        INT_CONST(v,DELAY_ATTACH_ON_CONNECT );
        INT_CONST(v,XPUB_VERBOSE );

        INT_CONST(v,EVENT_CONNECTED);
        INT_CONST(v,EVENT_CONNECT_DELAYED);
        INT_CONST(v,EVENT_CONNECT_RETRIED);
        INT_CONST(v,EVENT_LISTENING);
        INT_CONST(v,EVENT_BIND_FAILED);
        INT_CONST(v,EVENT_ACCEPTED);
        INT_CONST(v,EVENT_ACCEPT_FAILED);
        INT_CONST(v,EVENT_CLOSED);
        INT_CONST(v,EVENT_CLOSE_FAILED);
        INT_CONST(v,EVENT_DISCONNECTED);
        INT_CONST(v,EVENT_ALL);

        sq_newslot(v,-3,SQTrue);


        sq_pushliteral(v,_SC("zmq_msg"));
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)sq_zmq3_msg_TAG);
        sq_insert_reg_funcs(v, sq_zmq3_msg_methods);

        INT_CONST(v,MORE);
        INT_CONST(v,DONTWAIT);
        INT_CONST(v,SNDMORE);

        sq_newslot(v,-3,SQTrue);

        return 1;
    }

#ifdef __cplusplus
}
#endif
