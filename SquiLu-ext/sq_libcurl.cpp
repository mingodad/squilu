#ifdef SQ_USE_EASYCURL

#ifdef __cplusplus
extern "C" {
#endif


#include "squirrel.h"
#include "sqstdblobimpl.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

#include "curl/curl.h"

////////
#include "dynamic_library.h"

/*SquiLu
local library_functions = [
    ["CURL *", "curl_easy_init", "void"],
    ["CURLcode", "curl_easy_setopt", "CURL *curl, CURLoption option, ..."],
    ["CURLcode", "curl_easy_getinfo", "CURL *curl, CURLINFO info, ..."],
    ["CURLcode", "curl_easy_perform", "CURL *curl"],
    ["CURLcode", "curl_easy_pause", "CURL *handle, int bitmask"],
    ["CURL *", "curl_easy_duphandle", "CURL *curl"],
    ["void", "curl_easy_reset", "CURL *curl"],
    ["CURLcode", "curl_easy_recv", "CURL *curl, void *buffer, size_t buflen, size_t *n"],
    ["CURLcode", "curl_easy_send", "CURL *curl, const void *buffer, size_t buflen, size_t *n"],
    ["const char *", "curl_easy_escape", "CURL *handle, const char *string, int length"],
    ["const char *", "curl_easy_unescape", "CURL *handle, const char *string, int length"],
    ["const char *", "curl_easy_strerror", "CURLcode errornum"],
    ["struct curl_slist *", "curl_slist_append", "struct curl_slist *, const char *"],
    ["void", "curl_slist_free_all", "struct curl_slist *"],

    //next entry should be the last one
    //to make valid the test made on load_library function
    ["void", "curl_easy_cleanup", "CURL *curl"],
];

function write_library_functions_declaration(){
    foreach(k,v in library_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_library_functions_load(){
    foreach(k,v in library_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libdyn.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary libdyn;

//@write_library_functions_declaration();
// generated-code:begin
typedef CURL * (*curl_easy_init_t)(void);
static curl_easy_init_t dlcurl_easy_init = 0;
typedef CURLcode (*curl_easy_setopt_t)(CURL *curl, CURLoption option, ...);
static curl_easy_setopt_t dlcurl_easy_setopt = 0;
typedef CURLcode (*curl_easy_getinfo_t)(CURL *curl, CURLINFO info, ...);
static curl_easy_getinfo_t dlcurl_easy_getinfo = 0;
typedef CURLcode (*curl_easy_perform_t)(CURL *curl);
static curl_easy_perform_t dlcurl_easy_perform = 0;
typedef CURLcode (*curl_easy_pause_t)(CURL *handle, int bitmask);
static curl_easy_pause_t dlcurl_easy_pause = 0;
typedef CURL * (*curl_easy_duphandle_t)(CURL *curl);
static curl_easy_duphandle_t dlcurl_easy_duphandle = 0;
typedef void (*curl_easy_reset_t)(CURL *curl);
static curl_easy_reset_t dlcurl_easy_reset = 0;
typedef CURLcode (*curl_easy_recv_t)(CURL *curl, void *buffer, size_t buflen, size_t *n);
static curl_easy_recv_t dlcurl_easy_recv = 0;
typedef CURLcode (*curl_easy_send_t)(CURL *curl, const void *buffer, size_t buflen, size_t *n);
static curl_easy_send_t dlcurl_easy_send = 0;
typedef const char * (*curl_easy_escape_t)(CURL *handle, const char *string, int length);
static curl_easy_escape_t dlcurl_easy_escape = 0;
typedef const char * (*curl_easy_unescape_t)(CURL *handle, const char *string, int length);
static curl_easy_unescape_t dlcurl_easy_unescape = 0;
typedef const char * (*curl_easy_strerror_t)(CURLcode errornum);
static curl_easy_strerror_t dlcurl_easy_strerror = 0;
typedef struct curl_slist * (*curl_slist_append_t)(struct curl_slist *, const char *);
static curl_slist_append_t dlcurl_slist_append = 0;
typedef void (*curl_slist_free_all_t)(struct curl_slist *);
static curl_slist_free_all_t dlcurl_slist_free_all = 0;
typedef void (*curl_easy_cleanup_t)(CURL *curl);
static curl_easy_cleanup_t dlcurl_easy_cleanup = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libcurl);

static bool load_library(const char *libname)
{
    if(dlcurl_easy_cleanup) return true;
    if(libdyn.open(libname))
    {
        //@write_library_functions_load();
// generated-code:begin
dlcurl_easy_init = (curl_easy_init_t) libdyn.dlsym("curl_easy_init");
if(!dlcurl_easy_init) return false;
dlcurl_easy_setopt = (curl_easy_setopt_t) libdyn.dlsym("curl_easy_setopt");
if(!dlcurl_easy_setopt) return false;
dlcurl_easy_getinfo = (curl_easy_getinfo_t) libdyn.dlsym("curl_easy_getinfo");
if(!dlcurl_easy_getinfo) return false;
dlcurl_easy_perform = (curl_easy_perform_t) libdyn.dlsym("curl_easy_perform");
if(!dlcurl_easy_perform) return false;
dlcurl_easy_pause = (curl_easy_pause_t) libdyn.dlsym("curl_easy_pause");
if(!dlcurl_easy_pause) return false;
dlcurl_easy_duphandle = (curl_easy_duphandle_t) libdyn.dlsym("curl_easy_duphandle");
if(!dlcurl_easy_duphandle) return false;
dlcurl_easy_reset = (curl_easy_reset_t) libdyn.dlsym("curl_easy_reset");
if(!dlcurl_easy_reset) return false;
dlcurl_easy_recv = (curl_easy_recv_t) libdyn.dlsym("curl_easy_recv");
if(!dlcurl_easy_recv) return false;
dlcurl_easy_send = (curl_easy_send_t) libdyn.dlsym("curl_easy_send");
if(!dlcurl_easy_send) return false;
dlcurl_easy_escape = (curl_easy_escape_t) libdyn.dlsym("curl_easy_escape");
if(!dlcurl_easy_escape) return false;
dlcurl_easy_unescape = (curl_easy_unescape_t) libdyn.dlsym("curl_easy_unescape");
if(!dlcurl_easy_unescape) return false;
dlcurl_easy_strerror = (curl_easy_strerror_t) libdyn.dlsym("curl_easy_strerror");
if(!dlcurl_easy_strerror) return false;
dlcurl_slist_append = (curl_slist_append_t) libdyn.dlsym("curl_slist_append");
if(!dlcurl_slist_append) return false;
dlcurl_slist_free_all = (curl_slist_free_all_t) libdyn.dlsym("curl_slist_free_all");
if(!dlcurl_slist_free_all) return false;
dlcurl_easy_cleanup = (curl_easy_cleanup_t) libdyn.dlsym("curl_easy_cleanup");
if(!dlcurl_easy_cleanup) return false;
// generated-code:end

        return true;
    }
    return false;
}

////////

struct sq_easycurl_st
{
    CURL *curl;
    HSQUIRRELVM vm;
    HSQOBJECT reader_cb;
    HSQOBJECT reader_cb_udata;
    HSQOBJECT writer_cb;
    HSQOBJECT writer_cb_udata;
    HSQOBJECT slist_array;
    SQInteger last_reader_pos;
    struct curl_slist *slist; //only one linked list
};

static const SQChar SQ_LIBNAME[] = _SC("EasyCurl");

//SQ_OPT_STRING_STRLEN();

static const SQChar EasyCurl_Tag[]   = _SC("EasyCurl_TAG");
#define GET_EasyCurl_INSTANCE() SQ_GET_INSTANCE(v, 1, sq_easycurl_st, EasyCurl_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("EasyCurl object already closed"));

/* Auxiliary function that waits on the socket. */
static int wait_on_socket(curl_socket_t sockfd, int for_recv, long timeout_ms)
{
  struct timeval tv;
  fd_set infd, outfd, errfd;
  int res;

  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec= (timeout_ms % 1000) * 1000;

  FD_ZERO(&infd);
  FD_ZERO(&outfd);
  FD_ZERO(&errfd);

  FD_SET(sockfd, &errfd); /* always check for error */

  if(for_recv) {
    FD_SET(sockfd, &infd);
  }
  else {
    FD_SET(sockfd, &outfd);
  }

  /* select() returns the number of signalled sockets or -1 */
  res = select(sockfd + 1, &infd, &outfd, &errfd, &tv);
  return res;
}

static SQRESULT EasyCurl_release_hook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	sq_easycurl_st *self = (sq_easycurl_st*)p;
	if(self)
    {
        dlcurl_easy_cleanup(self->curl);
        sq_release(self->vm, &self->reader_cb);
        sq_release(self->vm, &self->reader_cb_udata);
        sq_release(self->vm, &self->writer_cb);
        sq_release(self->vm, &self->writer_cb_udata);
        if(self->slist)
        {
            sq_release(self->vm, &self->slist_array);
            dlcurl_slist_free_all(self->slist);
        }
        sq_free(self, sizeof(*self));
    }
	return 0;
}

static SQRESULT EasyCurl_constructor(HSQUIRRELVM v, int idx, CURL *EasyCurl, int free_on_gc)
{
    if(!EasyCurl)
        return sq_throwerror(v, _SC("Could'nt create an EasyCurl object."));

    sq_easycurl_st *self = (sq_easycurl_st*)sq_malloc(sizeof(*self));
    memset(self, 0, sizeof(*self));
    sq_resetobject(&self->reader_cb);
    sq_resetobject(&self->reader_cb_udata);
    sq_resetobject(&self->writer_cb);
    sq_resetobject(&self->writer_cb_udata);
    sq_resetobject(&self->slist_array);
    self->vm = v;
    self->curl = EasyCurl;

    SQInteger rc = sq_setinstanceup(v, idx, self);
    if(free_on_gc) sq_setreleasehook(v,-1, EasyCurl_release_hook);
	return rc;
}

static SQRESULT EasyCurl_create_instance(HSQUIRRELVM v, CURL *EasyCurl, int free_on_gc)
{
    sq_pushstring(v, SQ_LIBNAME, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, EasyCurl_Tag, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                return EasyCurl_constructor(v, -1, EasyCurl, free_on_gc);
            }
        }
    }
	return SQ_ERROR;
}

static SQRESULT sq_EasyCurl_constructor(HSQUIRRELVM v){
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libcurl !"));
    CURL *curl = dlcurl_easy_init();
	return EasyCurl_constructor(v, 1, curl, 1);
}

static SQRESULT sq_EasyCurl_duphandle(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

    CURL *curl = dlcurl_easy_duphandle(self->curl);

	return EasyCurl_create_instance(v, curl, 1);
}

static SQRESULT sq_EasyCurl_getinfo(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

    CURLcode rc = CURL_LAST;

	SQ_GET_INTEGER(v, 2, option);
	SQ_GET_INTEGER(v, 3, option_type);

	switch(option_type)
	{
    case 's':
        const SQChar *str_value;
        rc = dlcurl_easy_getinfo(self->curl, (CURLINFO)option, &str_value);
        if(rc == CURLE_OK) sq_pushstring(v, str_value, -1);
        break;
    case 'i':
        long int_value;
        rc = dlcurl_easy_getinfo(self->curl, (CURLINFO)option, &int_value);
        if(rc == CURLE_OK) sq_pushinteger(v, int_value);
        break;
    case 'f':
        double float_value;
        rc = dlcurl_easy_getinfo(self->curl, (CURLINFO)option, &float_value);
        if(rc == CURLE_OK) sq_pushfloat(v, float_value);
        break;
    default:
        return sq_throwerror(v, _SC("invalid option value type"));
	}

    if(rc != CURLE_OK) sq_pushinteger(v, CURL_LAST);

	return 1;
}

static SQRESULT sq_EasyCurl_setopt(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	CURLcode rc = CURLE_OK;

	SQ_GET_INTEGER(v, 2, option);
	switch(sq_gettype(v, 3))
	{
    case OT_STRING:
        SQ_GET_STRING(v, 3, str_value);
        rc = dlcurl_easy_setopt(self->curl, (CURLoption)option, str_value);
        break;
    case OT_INTEGER:
        SQ_GET_INTEGER(v, 3, int_value);
        rc = dlcurl_easy_setopt(self->curl, (CURLoption)option, int_value);
        break;
    case OT_FLOAT:
        SQ_GET_FLOAT(v, 3, float_value);
        rc = dlcurl_easy_setopt(self->curl, (CURLoption)option, float_value);
        break;
    case OT_ARRAY:
        {
        if(self->slist)
        {
            return sq_throwerror(v, _SC("single list already used, only one right now"));
        }
        sq_getstackobj(v, 3, &self->slist_array);
        sq_addref(v, &self->slist_array);
        const SQChar *str;
        SQInteger asize = sq_getsize(v, 3);
        for(int i=0; i<asize; ++i)
        {
            sq_pushinteger(v, i);
            sq_get(v, 3);
            sq_getstring(v, -1, &str);
            self->slist = dlcurl_slist_append(self->slist, str);
            sq_poptop(v);
        }
        rc = dlcurl_easy_setopt(self->curl, (CURLoption)option, self->slist);
        }
        break;
    default:
        return sq_throwerror(v, _SC("invalid option value type"));
	}

    sq_pushinteger(v, rc);

	return 1;
}

static SQRESULT sq_EasyCurl_recv(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_INTEGER(v, 2, try_receive_n);

    SQChar *buffer = sq_getscratchpad(v, try_receive_n);
    size_t recv_size = 0;
    CURLcode rc = dlcurl_easy_recv(self->curl, (void *)buffer, try_receive_n, &recv_size);
    if(rc != CURLE_OK) sq_pushinteger(v, -rc);
    else sq_pushinteger(v, recv_size);
	return 1;
}

static SQRESULT sq_EasyCurl_send(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_STRING(v, 2, buffer);

	size_t send_size = 0;
    CURLcode rc = dlcurl_easy_send(self->curl, (const void *)buffer, buffer_size, &send_size);
    if(rc != CURLE_OK) sq_pushinteger(v, -rc);
    else sq_pushinteger(v, send_size);
	return 1;
}

static SQRESULT sq_EasyCurl_reset(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

    dlcurl_easy_reset(self->curl);

	return 0;
}

static SQRESULT sq_EasyCurl_perform(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

    CURLcode rc = dlcurl_easy_perform(self->curl);
    sq_pushinteger(v, rc);

	return 1;
}

static SQRESULT sq_EasyCurl_pause(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_INTEGER(v, 2, flag_how);

    CURLcode rc = dlcurl_easy_pause(self->curl, flag_how);
    sq_pushinteger(v, rc);

	return 1;
}

static SQRESULT sq_EasyCurl_cleanup(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

    dlcurl_easy_cleanup(self->curl);
    sq_setinstanceup(v, 1, 0); //invalidate this curl handle
	return 0;
}

static SQRESULT sq_EasyCurl_strerror(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_INTEGER(v, 2, error_code);

    sq_pushstring(v, dlcurl_easy_strerror((CURLcode)error_code), -1);

	return 1;
}

static SQRESULT sq_EasyCurl_escape(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_STRING(v, 2, str);

    sq_pushstring(v, dlcurl_easy_escape(self->curl, str, str_size), -1);

	return 1;
}

static SQRESULT sq_EasyCurl_unescape(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_STRING(v, 2, str);

    sq_pushstring(v, dlcurl_easy_unescape(self->curl, str, str_size), -1);

	return 1;
}


static size_t sq_EasyCurl_reader_writer_callback(char *bufptr, size_t size, size_t nitems, void *userp, int isReader)
{
    SQInteger result = 0; /* abort by default */
    size_t data_size = size * nitems;
    if(data_size == 0) return result;
    sq_easycurl_st *self = (sq_easycurl_st*)userp;
    HSQUIRRELVM v = self->vm;
    int top = sq_gettop(v);

    HSQOBJECT &rw_cb = (isReader ? self->reader_cb : self->writer_cb);
    HSQOBJECT &rw_cb_udata = (isReader ? self->reader_cb_udata : self->writer_cb_udata);

    sq_pushobject(v, rw_cb);

    SQObjectType otype = sq_gettype(v, -1);
    switch(otype)
    {
    case OT_INSTANCE:
        {
            SQBlob *blob = NULL;
            if(SQ_FAILED(sq_getinstanceup(v,-1,(SQUserPointer*)&blob,(SQUserPointer)SQBlob::SQBlob_TAG)))
                break;
            if(!blob || !blob->IsValid())
                break;
            if(isReader)
            {
                SQInteger blen = blob->Len();
                if(blen < self->last_reader_pos)
                {
                    if(blen < data_size)
                    {
                        memcpy(bufptr, blob->GetBuf(), blen);
                        self->last_reader_pos += blen;
                        result = blen;
                    }
                    else
                    {
                        memcpy(bufptr, blob->GetBuf() + self->last_reader_pos, data_size);
                        self->last_reader_pos += data_size;
                        result = data_size;
                    }
                }
            }
            else
            {
                blob->Write(bufptr, data_size);
                result = data_size;
            }
        }
        break;
    case OT_CLOSURE:
        sq_pushroottable(v);
        if(!isReader)
        {
            sq_pushstring(v, (const SQChar*)bufptr, data_size);
        }
        sq_pushinteger(v, size);
        sq_pushinteger(v, nitems);
        sq_pushobject(v, rw_cb_udata);

        /* call squilu function */
        if (sq_call(v, isReader ? 4 : 5, SQTrue, SQFalse) == SQ_OK)
        {
            switch(sq_gettype(v, -1))
            {
                case OT_INTEGER:
                    sq_getinteger(v, -1, &result);
                    break;
                case OT_STRING:
                    if(isReader)
                    {
                        const SQChar *str;
                        if(sq_getstr_and_size(v,-1, &str, &result) == SQ_OK)
                        {
                            memcpy(bufptr, str, result);
                        }
                    }
                    break;
            }
        }


        break;
    }

    sq_settop(v, top);
    return result;
}

static size_t sq_EasyCurl_reader_callback(char *bufptr, size_t size, size_t nitems, void *userp)
{
    return sq_EasyCurl_reader_writer_callback(bufptr, size, nitems, userp, 1);
}

static size_t sq_EasyCurl_writer_callback(char *bufptr, size_t size, size_t nitems, void *userp)
{
    return sq_EasyCurl_reader_writer_callback(bufptr, size, nitems, userp, 0);
}

static SQRESULT sq_EasyCurl_set_reader_writer(HSQUIRRELVM v, int isReader){
	SQ_FUNC_VARS(v);
	GET_EasyCurl_INSTANCE();

    CURLcode rc;
    HSQOBJECT &rw_cb = (isReader ? self->reader_cb : self->writer_cb);
    HSQOBJECT &rw_cb_udata = (isReader ? self->reader_cb_udata : self->writer_cb_udata);
    CURLoption rw_fnc_option = (isReader ? CURLOPT_READFUNCTION : CURLOPT_WRITEFUNCTION);

    /* clear reader handler */
    rc = dlcurl_easy_setopt(self->curl, rw_fnc_option, NULL);


    sq_release(v, &rw_cb);
    sq_release(v, &rw_cb_udata);

    sq_resetobject(&rw_cb);
    sq_resetobject(&rw_cb_udata);
    self->last_reader_pos = 0;

    SQObjectType otype = sq_gettype(v, 2);
    if(_top_ > 1 && otype != OT_NULL)
    {
        switch(otype)
        {
        case OT_INSTANCE:
            {
            SQBlob *blob = NULL;
            if(SQ_FAILED(sq_getinstanceup(v,2,(SQUserPointer*)&blob,(SQUserPointer)SQBlob::SQBlob_TAG)))
                return sq_throwerror(v,_SC("expect a blob as second parameter"));
            if(!blob || !blob->IsValid())
                return sq_throwerror(v,_SC("the blob is invalid"));
            }
            break;
        case OT_CLOSURE:
                break;
        default:
                return sq_throwerror(v, _SC("invalid fisrt parameter expected closure/blob"));
        }

        sq_getstackobj(v, 2, &rw_cb);
        sq_addref(v, &rw_cb);
        if(_top_ > 2)
        {
            sq_getstackobj(v, 3, &rw_cb_udata);
            sq_addref(v, &rw_cb_udata);
        }

        /* set authorizer_hook callback */
        rc = dlcurl_easy_setopt(self->curl, rw_fnc_option,
                              (isReader ? sq_EasyCurl_reader_callback : sq_EasyCurl_writer_callback));
        if(rc == CURLE_OK) rc = dlcurl_easy_setopt(self->curl,
                                                 (isReader ? CURLOPT_READDATA : CURLOPT_WRITEDATA), self);
    }

    sq_pushinteger(v, rc);
    return 1;
}

static SQRESULT sq_EasyCurl_set_reader(HSQUIRRELVM v){
	return sq_EasyCurl_set_reader_writer(v, 1);
}

static SQRESULT sq_EasyCurl_set_writer(HSQUIRRELVM v){
    return sq_EasyCurl_set_reader_writer(v, 0);
}

static SQRESULT sq_EasyCurl_wait_on_socket(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_EasyCurl_INSTANCE();

	SQ_GET_BOOL(v, 2, for_recv);
	SQ_GET_INTEGER(v, 3, timeout_ms);

    long sockextr;
    CURLcode rc = dlcurl_easy_getinfo(self->curl, CURLINFO_LASTSOCKET, &sockextr);
    if(rc == CURLE_OK)
    {
        curl_socket_t sockfd = (curl_socket_t)sockextr;
        /* wait for the socket to become ready for sending */
        if(!wait_on_socket(sockfd, (for_recv ? 1 : 0), timeout_ms)) {
          return sq_throwerror(v, _SC("Error: timeout."));
        }
    }

    return 0;
}

static SQRESULT sq_EasyCurl_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, libname);
    sq_pushbool(v, load_library(libname));
	return 1;
}


#define _DECL_EASYCURL_FUNC(name,nparams,pmask) {_SC(#name),sq_EasyCurl_##name,nparams,pmask}
static SQRegFunction EasyCurl_obj_funcs[]={

	_DECL_EASYCURL_FUNC(constructor, 1, _SC(".")),
	_DECL_EASYCURL_FUNC(duphandle, 1, _SC("x")),
	_DECL_EASYCURL_FUNC(getinfo, 3, _SC("xii")),
	_DECL_EASYCURL_FUNC(recv, 2, _SC("xi")),
	_DECL_EASYCURL_FUNC(reset, 1, _SC("x")),
	_DECL_EASYCURL_FUNC(send, 2, _SC("xs")),
	_DECL_EASYCURL_FUNC(setopt, 3, _SC("xi s|n|a")),
	_DECL_EASYCURL_FUNC(perform, 1, _SC("x")),
	_DECL_EASYCURL_FUNC(pause, 2, _SC("xi")),
	_DECL_EASYCURL_FUNC(cleanup, 1, _SC("x")),
	_DECL_EASYCURL_FUNC(strerror, 2, _SC(".i")),
	_DECL_EASYCURL_FUNC(set_reader, -1, _SC("x c|x .")),
	_DECL_EASYCURL_FUNC(set_writer, -1, _SC("x c|x .")),
	_DECL_EASYCURL_FUNC(wait_on_socket, 3, _SC("x bi")),
	_DECL_EASYCURL_FUNC(escape, 2, _SC("xs")),
	_DECL_EASYCURL_FUNC(unescape, 2, _SC("xs")),
	_DECL_EASYCURL_FUNC(loadlib,2,_SC(".s")),
	{0,0}
};
#undef _DECL_EASYCURL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType EasyCurl_constants[] = {
    #define MK_CONST(c) {_SC(#c), c}
    #define MK_CONST2(c) {_SC(#c), (SQInteger)c}
    //MK_CONST(SSL_SESSION_ID_SIZE),

	MK_CONST(CURLPROTO_DICT),
	MK_CONST(CURLSSH_AUTH_ANY),
	MK_CONST(CURLAUTH_NTLM_WB),
	MK_CONST(CURLSSH_AUTH_KEYBOARD),
	MK_CONST(CURLPROTO_SMTP),
	MK_CONST(CURLPROTO_RTMP),
	MK_CONST(CURLSSH_AUTH_PUBLICKEY),
	MK_CONST(CURLPROTO_POP3),
	//MK_CONST(CURLAUTH_NEGOTIATE),
	MK_CONST(CURLPROTO_POP3S),
	//MK_CONST(CURLHEADER_SEPARATE),
	MK_CONST(CURLPROTO_RTMPTE),
	MK_CONST(CURLAUTH_DIGEST_IE),
	MK_CONST(CURLPROTO_ALL),
	MK_CONST(CURLSSH_AUTH_HOST),
	MK_CONST(CURLPROTO_HTTP),
	MK_CONST(CURLPROTO_HTTPS),
	MK_CONST(CURLOPT_SSL_VERIFYPEER),
	MK_CONST(CURLOPT_SSL_VERIFYHOST),
	MK_CONST(CURLOPT_ACCEPT_ENCODING),
	MK_CONST(CURLAUTH_BASIC),
	MK_CONST(CURLPROTO_IMAPS),
	MK_CONST(CURLPROTO_RTMPTS),
	MK_CONST(CURLSSH_AUTH_NONE),
	MK_CONST(CURLGSSAPI_DELEGATION_NONE),
	MK_CONST(CURLAUTH_DIGEST),
	MK_CONST(CURLGSSAPI_DELEGATION_FLAG),
	MK_CONST(CURLPROTO_LDAP),
	MK_CONST(CURLAUTH_NONE),
	MK_CONST(CURLPROTO_SMTPS),
	MK_CONST(CURLPROTO_GOPHER),
	MK_CONST(CURLSSH_AUTH_DEFAULT),
	//MK_CONST(CURLPROTO_SMB),
	MK_CONST(CURLPROTO_RTSP),
	MK_CONST(CURLSSH_AUTH_PASSWORD),
	//MK_CONST(CURLPROTO_SMBS),
	MK_CONST2(CURLAUTH_ANYSAFE),
	MK_CONST2(CURLPROTO_LDAPS),
	MK_CONST2(CURLGSSAPI_DELEGATION_POLICY_FLAG),
	MK_CONST2(CURLPROTO_TFTP),
	MK_CONST2(CURLPROTO_SFTP),
	//MK_CONST2(CURLHEADER_UNIFIED),
	MK_CONST2(CURLAUTH_GSSNEGOTIATE),
	MK_CONST2(CURLPROTO_FILE),
	MK_CONST2(CURLAUTH_NTLM),
	MK_CONST2(CURLSSH_AUTH_AGENT),
	MK_CONST2(CURLPROTO_HTTPS),
	MK_CONST2(CURLPROTO_RTMPS),
	MK_CONST2(CURLAUTH_ONLY),
	MK_CONST2(CURLPROTO_TELNET),
	MK_CONST2(CURLAUTH_ANY),
	MK_CONST2(CURLOPT_VERBOSE),

	MK_CONST(CURLPROTO_FTPS),
	MK_CONST(CURLPROTO_IMAP),
	MK_CONST(CURLPROTO_RTMPE),
	MK_CONST(CURLPROTO_RTMPT),
	MK_CONST(CURLPROTO_FTP),
	MK_CONST(CURLPROTO_SCP),
	MK_CONST(CURLOPT_URL),
	MK_CONST(CURLOPT_FOLLOWLOCATION),
	MK_CONST(CURLOPT_HTTPGET),
	MK_CONST(CURLOPT_PUT),
	MK_CONST2(CURLOPT_KEYPASSWD),
	MK_CONST2(CURLOPT_USERPWD),
	MK_CONST2(CURLOPT_USERNAME),
	MK_CONST2(CURLOPT_PASSWORD),
	MK_CONST2(CURLOPT_TLSAUTH_USERNAME),
	MK_CONST2(CURLOPT_TLSAUTH_PASSWORD),
	MK_CONST2(CURLOPT_TLSAUTH_TYPE),
	MK_CONST(CURLOPT_POST),
	MK_CONST(CURLOPT_POSTFIELDS),
	MK_CONST(CURLOPT_HTTPHEADER),
	MK_CONST(CURLOPT_POSTFIELDS),
	MK_CONST(CURLINFO_EFFECTIVE_URL),
	MK_CONST(CURLOPT_MAIL_FROM),
	MK_CONST(CURLOPT_MAIL_RCPT),
	MK_CONST(CURLOPT_UPLOAD),
	MK_CONST(CURLE_OK),
	MK_CONST(CURLPAUSE_RECV),
	MK_CONST(CURLPAUSE_RECV_CONT),
	MK_CONST(CURLPAUSE_SEND),
	MK_CONST(CURLPAUSE_SEND_CONT),
	MK_CONST(CURLPAUSE_ALL),
	MK_CONST(CURLPAUSE_CONT),

	MK_CONST(CURLOPT_APPEND),
	MK_CONST(CURLOPT_COOKIE),
	MK_CONST(CURLOPT_COOKIEFILE),
	MK_CONST(CURLOPT_COOKIESESSION),
	MK_CONST(CURLOPT_COOKIEJAR),
	MK_CONST(CURLOPT_FILE),
	MK_CONST(CURLOPT_REFERER),
	MK_CONST(CURLOPT_AUTOREFERER),
	MK_CONST(CURLOPT_TIMEOUT),
	MK_CONST(CURLOPT_USERAGENT),
	MK_CONST(CURLOPT_HEADER),
	MK_CONST(CURLOPT_NOBODY),
	MK_CONST(CURLOPT_MAXREDIRS),
	MK_CONST(CURLOPT_FOLLOWLOCATION),

	MK_CONST(CURLINFO_CONTENT_TYPE),
	MK_CONST(CURLINFO_COOKIELIST),
	MK_CONST(CURLINFO_EFFECTIVE_URL),
	MK_CONST(CURLINFO_LOCAL_IP),
	MK_CONST(CURLINFO_LOCAL_PORT),
	MK_CONST(CURLINFO_PRIMARY_IP),
	MK_CONST(CURLINFO_PRIMARY_PORT),
	MK_CONST(CURLINFO_REDIRECT_COUNT),
	MK_CONST(CURLINFO_REDIRECT_URL),
	MK_CONST(CURLINFO_RESPONSE_CODE),
	MK_CONST(CURLINFO_HTTP_CODE),

	MK_CONST(CURL_LAST),
    {0,0}
};

/* This defines a function that opens up your library. */
SQRESULT sqext_register_EasyCurl (HSQUIRRELVM v) {
	//add a namespace EasyCurl
	sq_pushstring(v, SQ_LIBNAME, -1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(SQUserPointer)EasyCurl_Tag);

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
