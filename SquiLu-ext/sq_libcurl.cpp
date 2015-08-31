#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_EASYCURL

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

#include "EasyCurl.h"

static const SQChar SQ_LIBNAME[] = _SC("EasyCurl");

SQ_OPT_STRING_STRLEN();

static const SQChar SSL_CTX_Tag[]   = _SC("sq_EasyCurl_ctx");
#define GET_EasyCurl_INSTANCE() SQ_GET_INSTANCE(v, 1, EASYCURL, EASYCURL_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("EasyCurl object already closed"));


static SQRESULT EasyCurl_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	EASYCURL *self = (EASYCURL*)p;
	if(self) EasyCurl_free(self);
	return 0;
}

static SQRESULT EasyCurl_free(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	GET_ssl_INSTANCE();
	EasyCurl_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0);
	return 0;
}

static SQRESULT EasyCurl_constructor(HSQUIRRELVM v, EASYCURL *EasyCurl, int free_on_gc)
{
    if(!EasyCurl)
        return sq_throwerror(v, _SC("Could'nt create an EasyCurl object."));

    sq_pushstring(v, SQ_LIBNAME, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, EasyCurl_NAME, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, ssl);
                if(free_on_gc) sq_setreleasehook(v,-1, EasyCurl_release_hook);
                return 1;
            }
        }
    }
	return SQ_ERROR;
}


static SQRESULT sq_EasyCurl_cleanup(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

//	void curl_easy_cleanup(CURL *curl)

	return 0;
}

static SQRESULT sq_EasyCurl_constructor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();


	return 0;
}

static SQRESULT sq_EasyCurl_destructor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();


	return 0;
}

static SQRESULT sq_EasyCurl_easy_duphandle(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

//	CURL* curl_easy_duphandle(CURL *curl)

	return 0;
}

static SQRESULT sq_EasyCurl_easy_getinfo(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_STRING(v, 2, info);
	SQ_GET_STRING(v, 3, vargv);
	SQ_GET_STRING(v, 4, ...);
//	CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)

	return 0;
}

static SQRESULT sq_EasyCurl_easy_init(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

//	CURL *curl_easy_init(void)

	return 0;
}

static SQRESULT sq_EasyCurl_easy_recv(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

//	CURLcode curl_easy_recv(CURL *curl, void *buffer, size_t buflen, size_t *n)

	return 0;
}

static SQRESULT sq_EasyCurl_easy_reset(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

//	void curl_easy_reset(CURL *curl)

	return 0;
}

static SQRESULT sq_EasyCurl_easy_send(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_STRING(v, 2, buffer);
//	CURLcode curl_easy_send(CURL *curl, const void *buffer, size_t buflen, size_t *n)

	return 0;
}

static SQRESULT sq_EasyCurl_easy_setopt(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_STRING(v, 2, option);
	SQ_GET_STRING(v, 3, vargv);
	SQ_GET_STRING(v, 4, ...);
//	CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)

	return 0;
}

static SQRESULT sq_EasyCurl_perform(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

//	CURLcode curl_easy_perform(CURL *curl)

	return 0;
}

#define _DECL_EASYCURL_FUNC(name,nparams,pmask) {_SC(#name),sq_EasyCurl_##name,nparams,pmask}
static SQRegFunction EasyCurl_obj_funcs[]={

	_DECL_EASYCURL_FUNC(cleanup, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(constructor, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(destructor, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(easy_duphandle, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(easy_getinfo, 4, _SC("....")),
	_DECL_EASYCURL_FUNC(easy_init, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(easy_recv, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(easy_reset, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(easy_send, 2, _SC("..")),
	_DECL_EASYCURL_FUNC(easy_setopt, 4, _SC("....")),
	_DECL_EASYCURL_FUNC(perform, 1, _SC(".")),
	{0,0}
};
#undef _DECL_EASYCURL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType EasyCurl_constants[] = {
    #define MK_CONST(c) {_SC(#c), c}
    //MK_CONST(SSL_SESSION_ID_SIZE),

	MK_CONST(v, CURLPROTO_DICT);
	MK_CONST(v, CURLSSH_AUTH_ANY);
	MK_CONST(v, CURLAUTH_NTLM_WB);
	MK_CONST(v, CURLSSH_AUTH_KEYBOARD);
	MK_CONST(v, CURLPROTO_SMTP);
	MK_CONST(v, CURLPROTO_RTMP);
	MK_CONST(v, CURLSSH_AUTH_PUBLICKEY);
	MK_CONST(v, CURLPROTO_POP3);
	MK_CONST(v, CURLAUTH_NEGOTIATE);
	MK_CONST(v, CURLPROTO_POP3S);
	MK_CONST(v, CURLHEADER_SEPARATE);
	MK_CONST(v, CURLPROTO_RTMPTE);
	MK_CONST(v, CURLAUTH_DIGEST_IE);
	MK_CONST(v, CURLPROTO_ALL);
	MK_CONST(v, CURLSSH_AUTH_HOST);
	MK_CONST(v, CURLPROTO_HTTP);
	MK_CONST(v, CURLAUTH_BASIC);
	MK_CONST(v, CURLPROTO_IMAPS);
	MK_CONST(v, CURLPROTO_RTMPTS);
	MK_CONST(v, CURLSSH_AUTH_NONE);
	MK_CONST(v, CURLGSSAPI_DELEGATION_NONE);
	MK_CONST(v, CURLAUTH_DIGEST);
	MK_CONST(v, CURLGSSAPI_DELEGATION_FLAG);
	MK_CONST(v, CURLPROTO_LDAP);
	MK_CONST(v, CURLAUTH_NONE);
	MK_CONST(v, CURLPROTO_SMTPS);
	MK_CONST(v, CURLPROTO_GOPHER);
	MK_CONST(v, CURLSSH_AUTH_DEFAULT);
	MK_CONST(v, CURLPROTO_SMB);
	MK_CONST(v, CURLPROTO_RTSP);
	MK_CONST(v, CURLSSH_AUTH_PASSWORD);
	MK_CONST(v, CURLPROTO_SMBS);
	MK_CONST(v, CURLAUTH_ANYSAFE);
	MK_CONST(v, CURLPROTO_LDAPS);
	MK_CONST(v, CURLGSSAPI_DELEGATION_POLICY_FLAG);
	MK_CONST(v, CURLPROTO_TFTP);
	MK_CONST(v, CURLPROTO_SFTP);
	MK_CONST(v, CURLHEADER_UNIFIED);
	MK_CONST(v, CURLAUTH_GSSNEGOTIATE);
	MK_CONST(v, CURLPROTO_FILE);
	MK_CONST(v, CURLAUTH_NTLM);
	MK_CONST(v, CURLSSH_AUTH_AGENT);
	MK_CONST(v, CURLPROTO_HTTPS);
	MK_CONST(v, CURLPROTO_RTMPS);
	MK_CONST(v, CURLAUTH_ONLY);
	MK_CONST(v, CURLPROTO_TELNET);
	MK_CONST(v, CURLAUTH_ANY);
	MK_CONST(v, CURLPROTO_FTPS);
	MK_CONST(v, CURLPROTO_IMAP);
	MK_CONST(v, CURLPROTO_RTMPE);
	MK_CONST(v, CURLPROTO_RTMPT);
	MK_CONST(v, CURLPROTO_FTP);
	MK_CONST(v, CURLPROTO_SCP);
    {0,0}
};

/* This defines a function that opens up your library. */
SQRESULT sqext_register_EasyCurl (HSQUIRRELVM v) {
	//add a namespace EasyCurl
	sq_pushstring(v, SQ_LIBNAME, -1);
	sq_newtable(v);

	sq_insert_reg_funcs(v, EasyCurl_obj_funcs);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = EasyCurl_constants; KeyIntPtr->Str; KeyIntPtr++) {
		sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
		sq_pushinteger(v, KeyIntPtr->Val);       //then the value
		sq_newslot(v, -3, SQFalse);              //store then
	}

	sq_newslot(v,-3,SQFalse); //add EasyCurl table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}

#endif //USE_EASYCURL

#endif
