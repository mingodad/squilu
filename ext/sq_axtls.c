#ifdef __cplusplus
extern "C" {
#endif

#include "squirrel.h"
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

#define GET_INT(idx, var) int var; sq_getinteger(sqvm, idx, &var)
#define GET_STR(idx, var) const SQChar *var; sq_getstring(sqvm, idx, &var)

#include "ssl.h"

#define SQ_NETLIBNAME "axtlsl"

typedef struct {
	SSL_CTX *ptr;
} SSL_CTX_ptr;

static const char *SSL_CTX_metaTag   = "sq_axtls";

static SSL_CTX_ptr *sq_ssl_get_ctx(HSQUIRRELVM sqvm, int index) {
    SSL_CTX_ptr *ssl_ctx_ptr = (SSL_CTX_ptr*)luaL_checkudata(sqvm, index, SSL_CTX_metaTag);
    if (ssl_ctx_ptr == NULL)
        luaL_argerror(sqvm, index, "bad SSL CONTEXT");
    if (ssl_ctx_ptr->ptr == NULL)
        luaL_argerror(sqvm, index, "SSL CONTEXT has NULL value");
    return ssl_ctx_ptr;
}


typedef struct {
	SSL *ptr;
	int free_ptr_on_gc;
} SSL_ptr;

static const char *SSL_metaTag   = "axtls.ssl";

static SSL_ptr *sq_ssl_get_ssl(HSQUIRRELVM sqvm, int index) {
    SSL_ptr *ssl_ptr = (SSL_ptr*)luaL_checkudata(sqvm, index, SSL_metaTag);
    if (ssl_ptr == NULL)
        luaL_argerror(sqvm, index, "bad SSL");
    if (ssl_ptr->ptr == NULL)
        luaL_argerror(sqvm, index, "SSL has NULL value");
    return ssl_ptr;
}


static int sq_ssl_ctx_new(HSQUIRRELVM sqvm){
	int options = sq_getinteger(sqvm, 1);
	int num_sessions = sq_getinteger(sqvm, 2);
    SSL_CTX *ssl_ctx = ssl_ctx_new(options, num_sessions);
    if(ssl_ctx) {
        SSL_CTX_ptr *ssl_ctx_ptr = (SSL_CTX_ptr*)sq_newuserdata(sqvm, sizeof(SSL_CTX_ptr));
        ssl_ctx_ptr->ptr = ssl_ctx;
        luaL_newmetatable(sqvm, SSL_CTX_metaTag);
        sq_setmetatable(sqvm, -2);        /* set metatable */
    } else sq_pushnil(L);

	return 1;
}

static int sq_ssl_ctx_free(HSQUIRRELVM sqvm){
	SSL_CTX_ptr *ssl_ctx_ptr = sq_ssl_get_ctx(sqvm, 1);
	if(ssl_ctx_ptr->ptr){
		ssl_ctx_free(ssl_ctx_ptr->ptr);
		ssl_ctx_ptr->ptr = NULL;
	}
	return 0;
}

static int sq_ssl_server_new(HSQUIRRELVM sqvm){
	SSL_CTX_ptr *ssl_ctx_ptr = sq_ssl_get_ctx(sqvm, 1);
	int client_fd = sq_getinteger(sqvm, 2);
    SSL *ssl = ssl_server_new(ssl_ctx_ptr->ptr, client_fd);

    if(ssl) {
        SSL_ptr *ssl_ptr = (SSL_ptr*)sq_newuserdata(sqvm, sizeof(SSL_ptr));
        ssl_ptr->ptr = ssl;
        ssl_ptr->free_ptr_on_gc = 1;

        luaL_newmetatable(sqvm, SSL_metaTag);
        sq_setmetatable(sqvm, -2);        /* set metatable */
    } else sq_pushnil(L);

	return 1;
}

static int sq_ssl_client_new(HSQUIRRELVM sqvm){
	SSL_CTX_ptr *ssl_ctx_ptr = sq_ssl_get_ctx(sqvm, 1);
	int client_fd = sq_getinteger(sqvm, 2);
	uint8_t *session_id = (uint8_t *)luaL_optstring(sqvm, 3, NULL);
	uint8_t sess_id_size= luaL_optint(sqvm, 4, 0);
	SSL *ssl = ssl_client_new(ssl_ctx_ptr->ptr, client_fd, session_id, sess_id_size);

    if(ssl) {
        SSL_ptr *ssl_ptr = (SSL_ptr*)sq_newuserdata(sqvm, sizeof(SSL_ptr));
        ssl_ptr->ptr = ssl;
        ssl_ptr->free_ptr_on_gc = 1;

        luaL_newmetatable(sqvm, SSL_metaTag);
        sq_setmetatable(sqvm, -2);        /* set metatable */
    } else sq_pushnil(L);

	return 1;
}

static int sq_ssl_free(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	if(ssl_ptr->ptr && ssl_ptr->free_ptr_on_gc){
		ssl_free(ssl_ptr->ptr);
		ssl_ptr->ptr = NULL;
	}
	return 0;
}

static int sq_ssl_read(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	uint8_t *in_data;
	int result = ssl_read(ssl_ptr->ptr, &in_data);
	sq_pushinteger(sqvm, result);
	int return_params = 1;
	if (result > SSL_OK) {
	  sq_pushlstring(sqvm, (char *)in_data, result);
      return_params++;
    }
	return return_params;
}

static int sq_ssl_write(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	uint8_t *out_data;
	sq_getstring(sqvm, 2, &out_data);
	int out_len;
	if(sq_gettop(sqvm) > 2) sq_getinteger(sqvm, 3, &out_len);
	else out_len = sq_getsize(sqvm, 2);
	sq_pushinteger(sqvm, ssl_write(ssl_ptr->ptr, out_data, out_len));
	return 1;
}

static int sq_ssl_find(HSQUIRRELVM sqvm){
	SSL_CTX_ptr *ssl_ctx_ptr = sq_ssl_get_ctx(sqvm, 1);
	GET_INT(2, client_fd);
    SSL *ssl = ssl_find(ssl_ctx_ptr->ptr, client_fd);

    if(ssl){
        SSL_ptr *ssl_ptr = (SSL_ptr*)sq_newuserdata(sqvm, sizeof(SSL_ptr));
        ssl_ptr->ptr = ssl;
        //we don't want to free this SSL when collecting garbage
        ssl_ptr->free_ptr_on_gc = 0;

        luaL_newmetatable(sqvm, SSL_metaTag);
        sq_setmetatable(sqvm, -2);        /* set metatable */
    } else sq_pushnil(L);

	return 1;
}

static int sq_ssl_get_session_id(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	const uint8_t * result = ssl_get_session_id(ssl_ptr->ptr);
    sq_pushlstring(sqvm, (char *)result, ssl_get_session_id_size(ssl_ptr->ptr));
	return 1;
}

static int sq_ssl_get_session_id_size(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	uint8_t result = ssl_get_session_id_size(ssl_ptr->ptr);
	sq_pushinteger(sqvm, result);
	return 1;
}

static int sq_ssl_get_cipher_id(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	uint8_t result = ssl_get_cipher_id(ssl_ptr->ptr);
	sq_pushinteger(sqvm, result);
	return 1;
}

static int sq_ssl_handshake_status(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	int result = ssl_handshake_status(ssl_ptr->ptr);
	sq_pushinteger(sqvm, result);
	return 1;
}

static int sq_ssl_get_config(HSQUIRRELVM sqvm){
    GET_INT(1, info);
	sq_pushinteger(sqvm, ssl_get_config(info));
	return 1;
}

static int sq_ssl_display_error(HSQUIRRELVM sqvm){
    GET_INT(1, error);
	ssl_display_error(error);
	return 0;
}

static int sq_ssl_verify_cert(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	int result = ssl_verify_cert(ssl_ptr->ptr);
	sq_pushinteger(sqvm, result);
	return 1;
}

static int sq_ssl_get_cert_dn(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	GET_INT(2, component);
	const char* result = ssl_get_cert_dn(ssl_ptr->ptr, component);
	sq_pushstring(sqvm, result);
	return 1;
}

static int sq_ssl_get_cert_subject_alt_dnsname(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	GET_INT(2, dnsindex);
	const char* result = ssl_get_cert_subject_alt_dnsname(ssl_ptr->ptr, dnsindex);
	sq_pushstring(sqvm, result);
	return 1;
}

static int sq_ssl_renegotiate(HSQUIRRELVM sqvm){
	SSL_ptr *ssl_ptr = sq_ssl_get_ssl(sqvm, 1);
	int result = ssl_renegotiate(ssl_ptr->ptr);
	sq_pushinteger(sqvm, result);
	return 1;
}

static int sq_ssl_obj_load(HSQUIRRELVM sqvm){
	SSL_CTX_ptr *ssl_ctx_ptr = sq_ssl_get_ctx(sqvm, 1);
	GET_INT(2, obj_type);
	GET_STR(3, filename);
	GET_STR(4, password);
	int result = ssl_obj_load(ssl_ctx_ptr->ptr, obj_type,filename,password);
	sq_pushinteger(sqvm, result);
	return 1;
}

static int sq_ssl_obj_memory_load(HSQUIRRELVM sqvm){
	SSL_CTX_ptr *ssl_ctx_ptr = sq_ssl_get_ctx(sqvm, 1);
	GET_INT(2, obj_type);
	uint8_t *data;
	sq_getstring(sqvm, 3, &data);
	GET_INT(4, data_len);
	GET_STR(5, password);
	int result = ssl_obj_memory_load(ssl_ctx_ptr->ptr, obj_type,data,data_len, password);
	sq_pushinteger(sqvm, result);
	return 1;
}

static int sq_ssl_version(HSQUIRRELVM sqvm){
	sq_pushstring(sqvm,(const char*)ssl_version());
	return 1;
}

static SQInteger ssl_ctx_release_hook(SQUserPointer p, SQInteger size)
{
	ssl_ctx_free((SSL_CTX *)p);
	return 1;
}

static SQInteger ssl_ctx_constructor(HSQUIRRELVM sqvm)
{
	SQInteger options, num_sessions;
    sq_getinteger(sqvm, 2, &options);
    sq_getinteger(sqvm, 3, &num_sessions);

	SSL_CTX *ssl_ctx = ssl_ctx_new(options, num_sessions);
    if(!ssl_ctx)
        return sq_throwerror(sqvm, _SC("Could'nt create an ssl context."))

    sq_setinstanceup(sqvm, 1, ssl_ctx);
    RELEASE_HOOK(sq_setreleasehook(sqvm,1, ssl_ctx_release_hook));
	return 1;
}

static SQInteger ssl_release_hook(SQUserPointer p, SQInteger size)
{
	ssl_free((SSL *)p);
	return 1;
}

static SQInteger ssl_constructor(HSQUIRRELVM sqvm)
{
	SQInteger socket_fd, num_sessions;
    sq_getinteger(sqvm, 2, &options);
    sq_getinteger(sqvm, 3, &num_sessions);

	SSL *ssl = ssl_new(options, num_sessions);
    if(!ssl)
        return sq_throwerror(sqvm, _SC("Could'nt create an ssl server/client."))

    sq_setinstanceup(sqvm, 1, ssl);
    RELEASE_HOOK(sq_setreleasehook(sqvm,1, ssl_release_hook));
	return 1;
}

#define _DECL_FL_FUNC(name,nparams,pmask) {_SC(#name),sq_##name,nparams,pmask}
static SQRegFunction ssl_ctx_obj_funcs[]={
	_DECL_FL_BOX_FUNC(constructor,3,_SC("xii")),
	_DECL_FL_FUNC(server_new,2,_SC("xx")),
	_DECL_FL_FUNC(client_new,2,_SC("xxii")),
	_DECL_FL_FUNC(find,2,_SC("tu")),
	_DECL_FL_FUNC(get_session_id,2,_SC("tu")),
	_DECL_FL_FUNC(get_session_id_size,2,_SC("tu")),
	_DECL_FL_FUNC(get_cipher_id,2,_SC("tu")),
	_DECL_FL_FUNC(handshake_status,2,_SC("tu")),
	_DECL_FL_FUNC(get_config,2,_SC("tu")),
	_DECL_FL_FUNC(display_error,2,_SC("tu")),
	_DECL_FL_FUNC(verify_cert,2,_SC("tu")),
	_DECL_FL_FUNC(get_cert_dn,2,_SC("tu")),
	_DECL_FL_FUNC(get_cert_subject_alt_dnsname,2,_SC("tu")),
	_DECL_FL_FUNC(renegotiate,2,_SC("tu")),
	_DECL_FL_FUNC(obj_load,2,_SC("tu")),
	_DECL_FL_FUNC(obj_memory_load,2,_SC("tu")),
	_DECL_FL_FUNC(version,2,_SC("tu")),
	{0,0}
};
#undef _DECL_FL_FUNC

#define _DECL_FL_FUNC(name,nparams,pmask) {_SC(#name),sq_##name,nparams,pmask}
static SQRegFunction ssl_obj_funcs[]={
	_DECL_FL_FUNC(ctx_new,3,_SC("tii")),
	_DECL_FL_FUNC(ctx_free,2,_SC("tu")),
	_DECL_FL_FUNC(server_new,2,_SC("tu")),
	_DECL_FL_FUNC(client_new,2,_SC("tu")),
	_DECL_FL_FUNC(free,2,_SC("tu")),
	_DECL_FL_FUNC(read,2,_SC("tu")),
	_DECL_FL_FUNC(write,2,_SC("tu")),
	_DECL_FL_FUNC(find,2,_SC("tu")),
	_DECL_FL_FUNC(get_session_id,2,_SC("tu")),
	_DECL_FL_FUNC(get_session_id_size,2,_SC("tu")),
	_DECL_FL_FUNC(get_cipher_id,2,_SC("tu")),
	_DECL_FL_FUNC(handshake_status,2,_SC("tu")),
	_DECL_FL_FUNC(get_config,2,_SC("tu")),
	_DECL_FL_FUNC(display_error,2,_SC("tu")),
	_DECL_FL_FUNC(verify_cert,2,_SC("tu")),
	_DECL_FL_FUNC(get_cert_dn,2,_SC("tu")),
	_DECL_FL_FUNC(get_cert_subject_alt_dnsname,2,_SC("tu")),
	_DECL_FL_FUNC(renegotiate,2,_SC("tu")),
	_DECL_FL_FUNC(obj_load,2,_SC("tu")),
	_DECL_FL_FUNC(obj_memory_load,2,_SC("tu")),
	_DECL_FL_FUNC(version,2,_SC("tu")),
	{0,0}
};
#undef _DECL_FL_FUNC

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
SQRESULT sq_register_axtlsl (HSQUIRRELVM sqvm) {
    //add constants
    sq_pushconsttable(sqvm);    //get the constants table first
    for (KeyIntPtrType KeyIntPtr = axtls_constants; KeyIntPtr->Str; KeyIntPtr++) {
        sq_pushstring(sqvm, KeyIntPtr->Str, -1);    //first the key
        sq_pushinteger(sqvm, KeyIntPtr->Val);       //then the value
        sq_newslot(sqvm, -3, SQFalse);              //store then
    }
    sq_poptop(sqvm); //pop remove constants table when we finished

    //add a namespace axtls
	sq_pushstring(sqvm,_SC("axtls"),-1);
	sq_newtable(sqvm);

    //now create the SSL Context class
	sq_pushstring(sqvm,_SC("ssl_ctx"),-1);
	sq_newclass(sqvm,SQFalse);
	sq_settypetag(sqvm,-1,(void*)SSL_CTX_TAG(Fl));
	insertFuncs(sqvm, ssl_ctx_obj_funcs);
	sq_newslot(sqvm,-3,SQFalse);

    //now create the SSL class
	sq_pushstring(sqvm,_SC("ssl"),-1);
	sq_newclass(sqvm,SQFalse);
	sq_settypetag(sqvm,-1,(void*)SSL_TAG(Fl));
	insertFuncs(sqvm, ssl_obj_funcs);
	sq_newslot(sqvm,-3,SQFalse);

	sq_newslot(sqvm,-3,SQFalse); //add axtls table to the root table
    sq_poptop(sqvm); //removes axtls table

    return SQ_OK;
}

#ifdef __cplusplus
}
#endif
