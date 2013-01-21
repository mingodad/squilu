#ifdef __cplusplus
extern "C" {
#endif

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

#include "ssl.h"

static const SQChar SQ_LIBNAME[] = _SC("axtls");
static const SQChar ssl_ctx_NAME[] = _SC("ssl_ctx");
static const SQChar ssl_NAME[] = _SC("ssl");

SQ_OPT_STRING_STRLEN();

static const SQChar SSL_CTX_Tag[]   = _SC("sq_axtls_ssl_ctx");
#define GET_ssl_ctx_INSTANCE() SQ_GET_INSTANCE(v, 1, SSL_CTX, SSL_CTX_Tag) \
    if(self == NULL) return sq_throwerror(v, _SC("ssl_ctx object already closed"));


static const SQChar SSL_Tag[]   = _SC("sq_axtls_ssl");
#define GET_ssl_INSTANCE() SQ_GET_INSTANCE(v, 1, SSL, SSL_Tag) \
    if(self == NULL) return sq_throwerror(v, _SC("ssl object already closed"));

static SQInteger ssl_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
    SSL *self = (SSL*)p;
    if(self) ssl_free(self);
	return 0;
}

static SQInteger sq_ssl_free(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
    ssl_release_hook(self, 0, v);
    sq_setinstanceup(v, 1, 0);
	return 0;
}

static SQInteger ssl_constructor(HSQUIRRELVM v, SSL *ssl, int free_on_gc)
{
    if(!ssl)
        return sq_throwerror(v, _SC("Could'nt create an ssl object."));

    sq_pushstring(v, SQ_LIBNAME, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, ssl_NAME, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, ssl);
                if(free_on_gc) sq_setreleasehook(v,-1, ssl_release_hook);
                return 1;
            }
        }
    }
	return SQ_ERROR;
}

static int sq_ssl_read(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
	uint8_t *in_data;
	int result = ssl_read(self, &in_data);
	if (result > SSL_OK) sq_pushstring(v, (const SQChar*)in_data, result);
	else sq_pushinteger(v, result);
	return 1;
}

static int sq_ssl_write(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_ssl_INSTANCE();
    SQ_GET_STRING(v, 2, out_data);
	if(_top_ > 2) {
	    SQ_GET_INTEGER(v, 3, size);
	    if(size > out_data_size) return sq_throwerror(v, _SC("parameter 2 size bigger than data size"));
	    out_data_size = size;
	}
	sq_pushinteger(v, ssl_write(self, (const uint8_t *)out_data, out_data_size));
	return 1;
}

static int sq_ssl_get_session_id(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
	const uint8_t * result = ssl_get_session_id(self);
    sq_pushstring(v, (char *)result, ssl_get_session_id_size(self));
	return 1;
}

static int sq_ssl_get_session_id_size(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
	uint8_t result = ssl_get_session_id_size(self);
	sq_pushinteger(v, result);
	return 1;
}

static int sq_ssl_get_cipher_id(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
	uint8_t result = ssl_get_cipher_id(self);
	sq_pushinteger(v, result);
	return 1;
}

static int sq_ssl_handshake_status(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
	int result = ssl_handshake_status(self);
	sq_pushinteger(v, result);
	return 1;
}

static int sq_ssl_verify_cert(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
	int result = ssl_verify_cert(self);
	sq_pushinteger(v, result);
	return 1;
}

static int sq_ssl_get_cert_dn(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
    SQ_GET_INTEGER(v, 2, component);
	const char* result = ssl_get_cert_dn(self, component);
	sq_pushstring(v, result, -1);
	return 1;
}

static int sq_ssl_get_cert_subject_alt_dnsname(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
    SQ_GET_INTEGER(v, 2, dnsindex);
	const char* result = ssl_get_cert_subject_alt_dnsname(self, dnsindex);
	sq_pushstring(v, result, -1);
	return 1;
}

static int sq_ssl_renegotiate(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_INSTANCE();
	int result = ssl_renegotiate(self);
	sq_pushinteger(v, result);
	return 1;
}

static int sq_ssl_ctx_server_new(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_ctx_INSTANCE();
    SQ_GET_INTEGER(v, 2, client_fd);
    SSL *ssl = ssl_server_new(self, client_fd);
    SQRESULT rc = ssl_constructor(v, ssl, 1);
    if(rc == SQ_ERROR && ssl){
        ssl_free(ssl);
    }
    return rc;
}

static int sq_ssl_ctx_client_new(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_ssl_ctx_INSTANCE();
    SQ_GET_INTEGER(v, 2, client_fd);
    SQ_OPT_STRING(v, 3, session_id, NULL);
    SQ_OPT_INTEGER(v, 4, size, -1);
	SSL *ssl = ssl_client_new(self, client_fd, (const uint8_t *)session_id,
                           size >= 0 ? size : session_id_size);
    SQRESULT rc = ssl_constructor(v, ssl, 1);
    if(rc == SQ_ERROR && ssl){
        ssl_free(ssl);
    }
    return rc;
}

static int sq_ssl_ctx_find(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_ctx_INSTANCE();
    SQ_GET_INTEGER(v, 2, client_fd);
    SSL *ssl = ssl_find(self, client_fd);
    if(ssl) return ssl_constructor(v, ssl, 0);
    else sq_pushnull(v);
    return 1;
}

static int sq_ssl_ctx_obj_load(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_ctx_INSTANCE();
	SQ_GET_INTEGER(v, 2, obj_type);
	SQ_GET_STRING(v, 3, filename);
	SQ_GET_STRING(v, 4, password);
	int result = ssl_obj_load(self, obj_type, filename, password);
	sq_pushinteger(v, result);
	return 1;
}

static int sq_ssl_ctx_obj_memory_load(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_ctx_INSTANCE();
	SQ_GET_INTEGER(v, 2, obj_type);
	SQ_GET_STRING(v, 3, data);
	SQ_GET_STRING(v, 4, password);
	int result = ssl_obj_memory_load(self, obj_type, (const uint8_t *)data, data_size, password);
	sq_pushinteger(v, result);
	return 1;
}

static int sq_axtls_version(HSQUIRRELVM v){
	sq_pushstring(v,(const char*)ssl_version(), -1);
	return 1;
}

static int sq_axtls_get_config(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, info);
	sq_pushinteger(v, ssl_get_config(info));
	return 1;
}

static int sq_axtls_display_error(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, error);
	ssl_display_error(error);
	return 0;
}

static SQInteger ssl_ctx_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
    SSL_CTX *self = (SSL_CTX*)p;
    if(self) ssl_ctx_free(self);
	return 0;
}

static SQInteger sq_ssl_ctx_free(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_ssl_ctx_INSTANCE();
    ssl_ctx_release_hook(self, 0, v);
    sq_setinstanceup(v, 1, 0);
	return 0;
}


static SQInteger sq_ssl_ctx_constructor(HSQUIRRELVM v)
{
	SQInteger options, num_sessions;
    sq_getinteger(v, 2, &options);
    sq_getinteger(v, 3, &num_sessions);

	SSL_CTX *ssl_ctx = ssl_ctx_new(options, num_sessions);
    if(!ssl_ctx)
        return sq_throwerror(v, _SC("Could'nt create an ssl context."));

    sq_setinstanceup(v, 1, ssl_ctx);
    sq_setreleasehook(v,1, ssl_ctx_release_hook);
	return 1;
}

#define _DECL_AXTLS_FUNC(name,nparams,pmask) {_SC(#name),sq_axtls_##name,nparams,pmask}
static SQRegFunction axtls_obj_funcs[]={
	_DECL_AXTLS_FUNC(get_config,2,_SC(".i")),
	_DECL_AXTLS_FUNC(display_error,2,_SC(".i")),
	_DECL_AXTLS_FUNC(version,2,_SC(".")),
	{0,0}
};
#undef _DECL_AXTLS_FUNC

#define _DECL_SSL_CTX_FUNC(name,nparams,pmask) {_SC(#name),sq_ssl_ctx_##name,nparams,pmask}
static SQRegFunction ssl_ctx_obj_funcs[]={
	_DECL_SSL_CTX_FUNC(constructor,3,_SC("xii")),
	_DECL_SSL_CTX_FUNC(free,1,_SC("x")),
	_DECL_SSL_CTX_FUNC(server_new,2,_SC("xi")),
	_DECL_SSL_CTX_FUNC(client_new,-2,_SC("xisi")),
	_DECL_SSL_CTX_FUNC(find,2,_SC("xs")),
	_DECL_SSL_CTX_FUNC(obj_load,2,_SC("xs")),
	_DECL_SSL_CTX_FUNC(obj_memory_load,2,_SC("xs")),
	{0,0}
};
#undef _DECL_SSL_CTX_FUNC

#define _DECL_SSL_FUNC(name,nparams,pmask) {_SC(#name),sq_ssl_##name,nparams,pmask}
static SQRegFunction ssl_obj_funcs[]={
	_DECL_SSL_FUNC(free,1,_SC("x")),
	_DECL_SSL_FUNC(read,1,_SC("x")),
	_DECL_SSL_FUNC(write,-2,_SC("xsi")),
	_DECL_SSL_FUNC(get_session_id,1,_SC("x")),
	_DECL_SSL_FUNC(get_session_id_size,1,_SC("x")),
	_DECL_SSL_FUNC(get_cipher_id,1,_SC("x")),
	_DECL_SSL_FUNC(handshake_status,1,_SC("x")),
	_DECL_SSL_FUNC(verify_cert,1,_SC("x")),
	_DECL_SSL_FUNC(get_cert_dn,2,_SC("xi")),
	_DECL_SSL_FUNC(get_cert_subject_alt_dnsname,2,_SC("xi")),
	_DECL_SSL_FUNC(renegotiate,1,_SC("x")),
	{0,0}
};
#undef _DECL_SSL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType axtls_constants[] = {
    #define MK_CONST(c) {_SC(#c), c}
    MK_CONST(SSL_SESSION_ID_SIZE),
    MK_CONST(SSL_CLIENT_AUTHENTICATION),
    MK_CONST(SSL_SERVER_VERIFY_LATER),
    MK_CONST(SSL_NO_DEFAULT_KEY),
    MK_CONST(SSL_DISPLAY_STATES),
    MK_CONST(SSL_DISPLAY_BYTES),
    MK_CONST(SSL_DISPLAY_CERTS),
    MK_CONST(SSL_DISPLAY_RSA),
    MK_CONST(SSL_CONNECT_IN_PARTS),
    MK_CONST(SSL_OK),
    MK_CONST(SSL_NOT_OK),
    MK_CONST(SSL_ERROR_DEAD),
    MK_CONST(SSL_CLOSE_NOTIFY),
    MK_CONST(SSL_ERROR_CONN_LOST),
    MK_CONST(SSL_ERROR_SOCK_SETUP_FAILURE),
    MK_CONST(SSL_ERROR_INVALID_HANDSHAKE),
    MK_CONST(SSL_ERROR_INVALID_PROT_MSG),
    MK_CONST(SSL_ERROR_INVALID_HMAC),
    MK_CONST(SSL_ERROR_INVALID_VERSION),
    MK_CONST(SSL_ERROR_INVALID_SESSION),
    MK_CONST(SSL_ERROR_NO_CIPHER),
    MK_CONST(SSL_ERROR_BAD_CERTIFICATE),
    MK_CONST(SSL_ERROR_INVALID_KEY),
    MK_CONST(SSL_ERROR_FINISHED_INVALID),
    MK_CONST(SSL_ERROR_NO_CERT_DEFINED),
    MK_CONST(SSL_ERROR_NO_CLIENT_RENOG),
    MK_CONST(SSL_ERROR_NOT_SUPPORTED),
    MK_CONST(SSL_X509_OFFSET),
    MK_CONST(SSL_ALERT_TYPE_WARNING),
    MK_CONST(SLL_ALERT_TYPE_FATAL),
    MK_CONST(SSL_ALERT_CLOSE_NOTIFY),
    MK_CONST(SSL_ALERT_UNEXPECTED_MESSAGE),
    MK_CONST(SSL_ALERT_BAD_RECORD_MAC),
    MK_CONST(SSL_ALERT_HANDSHAKE_FAILURE),
    MK_CONST(SSL_ALERT_BAD_CERTIFICATE),
    MK_CONST(SSL_ALERT_ILLEGAL_PARAMETER),
    MK_CONST(SSL_ALERT_DECODE_ERROR),
    MK_CONST(SSL_ALERT_DECRYPT_ERROR),
    MK_CONST(SSL_ALERT_INVALID_VERSION),
    MK_CONST(SSL_ALERT_NO_RENEGOTIATION),
    MK_CONST(SSL_AES128_SHA),
    MK_CONST(SSL_AES256_SHA),
    MK_CONST(SSL_RC4_128_SHA),
    MK_CONST(SSL_RC4_128_MD5),
    MK_CONST(SSL_BUILD_SKELETON_MODE),
    MK_CONST(SSL_BUILD_SERVER_ONLY),
    MK_CONST(SSL_BUILD_ENABLE_VERIFICATION),
    MK_CONST(SSL_BUILD_ENABLE_CLIENT),
    MK_CONST(SSL_BUILD_FULL_MODE),
    MK_CONST(SSL_BUILD_MODE),
    MK_CONST(SSL_MAX_CERT_CFG_OFFSET),
    MK_CONST(SSL_MAX_CA_CERT_CFG_OFFSET),
    MK_CONST(SSL_HAS_PEM),
    MK_CONST(SSL_DEFAULT_SVR_SESS),
    MK_CONST(SSL_DEFAULT_CLNT_SESS),
    MK_CONST(SSL_X509_CERT_COMMON_NAME),
    MK_CONST(SSL_X509_CERT_ORGANIZATION),
    MK_CONST(SSL_X509_CERT_ORGANIZATIONAL_NAME),
    MK_CONST(SSL_X509_CA_CERT_COMMON_NAME),
    MK_CONST(SSL_X509_CA_CERT_ORGANIZATION),
    MK_CONST(SSL_X509_CA_CERT_ORGANIZATIONAL_NAME),
    MK_CONST(SSL_OBJ_X509_CERT),
    MK_CONST(SSL_OBJ_X509_CACERT),
    MK_CONST(SSL_OBJ_RSA_KEY),
    MK_CONST(SSL_OBJ_PKCS8),
    MK_CONST(SSL_OBJ_PKCS12),
    {0,0}
};

/* This defines a function that opens up your library. */
SQRESULT sqext_register_axtls (HSQUIRRELVM v) {
    //add a namespace axtls
	sq_pushstring(v, SQ_LIBNAME, -1);
	sq_newtable(v);

	sq_insert_reg_funcs(v, axtls_obj_funcs);

    //add constants
    KeyIntPtrType KeyIntPtr;
    for (KeyIntPtr = axtls_constants; KeyIntPtr->Str; KeyIntPtr++) {
        sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
        sq_pushinteger(v, KeyIntPtr->Val);       //then the value
        sq_newslot(v, -3, SQFalse);              //store then
    }

    //now create the SSL Context class
	sq_pushstring(v,ssl_ctx_NAME,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)SSL_CTX_Tag);
	sq_insert_reg_funcs(v, ssl_ctx_obj_funcs);
	sq_newslot(v,-3,SQFalse);

    //now create the SSL class
	sq_pushstring(v,ssl_NAME,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)SSL_Tag);
	sq_insert_reg_funcs(v, ssl_obj_funcs);
	sq_newslot(v,-3,SQFalse);

	sq_newslot(v,-3,SQFalse); //add axtls table to the root table

    return SQ_OK;
}

#ifdef __cplusplus
}
#endif
