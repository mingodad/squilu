#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"
#include "squtils.h"
SQ_OPT_STRING_STRLEN();

#ifdef USE_SQ_SQLITE3
#include "lsqlite3.h"
#endif

#if defined(USE_AXTLS) || defined(USE_OPENSSL)
#ifdef __cplusplus
extern "C" {
#endif
    void * SSL_CTX_new(void *);
#ifdef __cplusplus
}
#endif
#endif

#define USE_MG_MD5 1

#include "mongoose.h"
//#include "civetweb.h"

#define PRINT_FILE_LINE printf("%s %d\n", __FILE__,__LINE__);

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#define MARK()  do { \
    printf("%s:%d (%s)\n", __FILE__, __LINE__, __FUNCTION__); \
} while (0)

typedef struct SQ_Mg_Context SQ_Mg_Context;
typedef struct SQ_MG_Callback SQ_MG_Callback;

struct SQ_MG_Callback {
    size_t size;
    char buf[1];
};

static SQ_MG_Callback *SQ_MG_Callback_malloc(size_t x){
    SQ_MG_Callback *p = (SQ_MG_Callback *)sq_malloc(sizeof(SQ_MG_Callback)+x);
    if(p) p->size = x;
    return p;
}

static void SQ_MG_Callback_free(SQ_MG_Callback *p){
    if(p) sq_free(p, sizeof(SQ_MG_Callback)+p->size);
}

struct SQ_Mg_Context {
    HSQUIRRELVM v;
    struct mg_context *ctx;
    SQ_MG_Callback *master_plugin;
    SQ_MG_Callback *master_plugin_exit;
    SQ_MG_Callback *user_callback;
    SQ_MG_Callback *user_callback_setup;
    SQ_MG_Callback *user_callback_exit;
};

static const SQChar sq_mg_context_TAG[] = "sq_mg_conn_class";
#define GET_mg_context_INSTANCE() \
    SQ_Mg_Context *self; \
    if((_rc_ = sq_getinstanceup(v,1,(SQUserPointer*)&self,(void*)sq_mg_context_TAG)) < 0) return _rc_;

static const SQChar sq_mg_user_callback[] = "mg_user_callback";

static const SQChar sq_http_request_TAG[] = "HttpRequest";

#define GET_http_request_INSTANCE() \
    struct mg_connection *conn; \
    if((_rc_ = sq_getinstanceup(v,1,(SQUserPointer*)&conn,(void*)sq_http_request_TAG)) < 0) return _rc_;

static SQRESULT sq_http_request_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	return 1;
}

static SQRESULT sq_http_request_constructor(HSQUIRRELVM v)
{
    //SQ_FUNC_VARS_NO_TOP(v);
    struct mg_connection *conn = 0;

    sq_setinstanceup(v, 1, conn);
    sq_setreleasehook(v,1, sq_http_request_releasehook);
	return 1;
}

static SQRESULT
sq_http_request_print(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_http_request_INSTANCE();
    SQInteger i, write_count = 0;

    for (i = 2; i <= _top_; ++i) {
        sq_tostring(v, i);
        SQ_GET_STRING(v, -1, value);
        write_count += mg_write(conn, value, value_size);
        sq_poptop(v);
    }

    sq_pushinteger(v, write_count);
    return 1;
}

#ifdef USE_SQ_LSQLITE3
static SQRESULT
sq_http_request_vm_print(HSQUIRRELVM v)
{
    GET_http_request_INSTANCE();
    lsqlite3_sdb_vm *svm = (lsqlite3_sdb_vm *)luaL_checkudata(v, 1,
                                                              lsqlite3_sqlite_vm_meta);
    SQInteger idx = luaL_checkint(v,2);

    SQInteger len = sqlite3_column_bytes(svm->vm, idx);
    if(len) sq_pushinteger(v, mg_write(conn, sqlite3_column_text(svm->vm, idx), len));
    else sq_pushinteger(v, 0);
    return 1;
}
#endif

static SQRESULT
sq_http_request_write(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, buf);
    SQ_OPT_INTEGER(v, 3, write_size, buf_size);
    sq_pushinteger(v, mg_write(conn, buf, write_size));
    return 1;
}

static SQRESULT
sq_http_request_read(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_http_request_INSTANCE();
    SQ_OPT_INTEGER(v, 2, n, 1024*2000);
    if(n <= 0) return sq_throwerror(v, _SC("invalid size to read (%d)"), n);
    size_t rlen;  /* how much to read */
    size_t nr;  /* number of chars actually read */
    rlen = 8192;  /* try to read that much each time */

    SQBlob *blob = NULL;
    if(_top_ > 2) {
        if(SQ_FAILED(sq_getinstanceup(v,3,(SQUserPointer*)&blob,(SQUserPointer)SQBlob::SQBlob_TAG)))
            return sq_throwerror(v,_SC("expect a blob as second parameter"));
        if(!blob || !blob->IsValid())
            return sq_throwerror(v,_SC("the blob is invalid"));
    } else {
        sq_new(blob, SQBlob);
		blob->Reserve(rlen);
    }

    if (((ssize_t)rlen) > n) rlen = n;  /* cannot read more than asked */
    char *p = sq_getscratchpad(v,rlen);
    do {
        //there is a bug in axtls that can return a number bigger than the actual bytes transfered
        nr = mg_read(conn, p, rlen);
        blob->Write(p, nr);
        n -= nr;  /* still have to read `n' chars */
    } while (n > 0 && nr == rlen);  /* until end of count or eof */

    if(_top_ <= 2) {
        sq_pushstring(v, (const SQChar *)blob->GetBuf(), blob->Len());  /* close buffer */
        if(blob) sq_delete(blob, SQBlob);
        return 1;
    }
    return 0;
}

static SQRESULT
sq_http_request_send_file(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, file_path);
    mg_send_file(conn, file_path);
    return 0;
}

static SQRESULT
sq_http_request_write_blob(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
	SQBlob *blob = NULL;
	{ if(SQ_FAILED(sq_getinstanceup(v,2,(SQUserPointer*)&blob,(SQUserPointer)SQBlob::SQBlob_TAG)))
		return sq_throwerror(v,_SC("invalid type tag"));  }
	if(!blob || !blob->IsValid())
		return sq_throwerror(v,_SC("the blob is invalid"));

    sq_pushinteger(v, mg_write(conn, (const SQChar*)blob->GetBuf(), blob->Len()));
    return 1;
}

static SQRESULT
sq_http_request_get_header(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, name);
    sq_pushstring(v, mg_get_header(conn, name), -1);
    return 1;
}

static SQRESULT
sq_http_request_get_cookie(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, cookie_name);

    const char *start;
    int len = mg_find_cookie(conn, cookie_name, &start);

    if(len > 0) sq_pushstring(v, start, len-1);
    else sq_pushnull(v);
    return 1;
}

static SQRESULT
sq_http_request_get_var(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, data);
    SQ_GET_STRING(v, 3, name);

    const char *start;
    size_t buffer_len;
    int var_len = mg_find_var(data, data_size, name, &start);
    if(var_len > 0){
        buffer_len = var_len+1;
        char *buffer = sq_getscratchpad(v,buffer_len);
        if(buffer){
            var_len = mg_url_decode(start, var_len, buffer, buffer_len, 1);
            sq_pushstring(v, buffer, var_len);
            return 1;
        }
    }
    sq_pushnull(v);
    return 1;
}

static SQRESULT
sq_http_request_get_conn_buf(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();

    int buf_size;
    const char *buf = mg_get_conn_buf(conn, &buf_size);
    sq_pushstring(v, buf, buf_size);
    return 1;
}

#ifndef _WIN32_WCE
static SQRESULT
sq_http_request_handle_cgi_request(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, prog);

    mg_handle_cgi_request(conn, prog);

    return 0;
}
#endif

static SQRESULT
sq_http_request_get_option(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, name);
    SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) mg_get_user_data(conn);
    sq_pushstring(v, mg_get_option(sq_mg_ctx->ctx, name), -1);

    return 1;
}

//Digest authentication functions
#define MAX_USER_LEN  20
#define MAX_NONCE_LEN  36 //extra bytes for zero terminator
#define MAX_SESSIONS 25
#define SESSION_TTL 3600
#define SESSION_REQUEST_TTL 120

enum e_session_state {e_session_invalid, e_session_sent_request, e_session_authorized};

struct st_session_t
{
    int state;
    char nonce[MAX_NONCE_LEN];
    char user[MAX_USER_LEN];         // Authenticated user
    long last_nc;
    time_t last_access;       // last_access time
    long remote_ip;           // Client's IP address
};

static struct st_session_t sessions[MAX_SESSIONS];  // Current sessions

// Protects messages, sessions, last_message_id
static mg_thread_mutex_t session_rwlock;

//should be called only after locking session_rwlock
static struct st_session_t *my_get_session_only_after_lock(const char *nonce){
    int i;
    for(i=0; i < MAX_SESSIONS; ++i){
        //printf("%s : %s\n", sessions[i].nonce, ah->nonce);
        if(strcmp(sessions[i].nonce, nonce) == 0){
            return &sessions[i];
        }
    }
    return 0;
}

// Check the user's password, return 1 if OK
static SQBool my_check_password(const struct mg_request_info *request_info, const char *my_tmp_password)
{
    md5_buf_t ha2, expected_response;
    struct mg_auth_header *ah = request_info->ah;

    // Some of the parameters may be NULL
    if (request_info->request_method == NULL ||
            //strcmp(ri->request_method, "PUT") == 0 ||
            //strcmp(ri->request_method, "DELETE") == 0 ||
            ah == NULL ||
            ah->nonce == NULL || ah->nc == NULL ||
            ah->cnonce == NULL || ah->qop == NULL || ah->uri == NULL ||
            ah->response == NULL)
    {
        return SQFalse;
    }

    // NOTE(lsm): due to a bug in MSIE, we do not compare the URI
    // TODO(lsm): check for authentication timeout
    if (// strcmp(ah->uri, conn->request_info.uri) != 0 ||
        strlen(ah->response) != 32
    )
    {
        return SQFalse;
    }

    mg_md5(ha2, request_info->request_method, ":", ah->uri, NULL);
    mg_md5(expected_response, my_tmp_password /*ah->ha1*/, ":", ah->nonce, ":", ah->nc,
           ":", ah->cnonce, ":", ah->qop, ":", ha2, NULL);
    SQBool result = mg_strcasecmp(ah->response, expected_response) == 0;
    //printf("%s : %s : %s\n", my_tmp_password, ah->response, expected_response);

    if(result)  //lets check timeout and other issues
    {
        struct st_session_t *session_found = 0;
        result = SQFalse;

        mg_thread_mutex_lock(&session_rwlock);

        session_found = my_get_session_only_after_lock(ah->nonce);
        if(session_found)
        {
            do
            {
                //mobile ip is a pain for security
                if( /*(ses.remote_ip != request_info->remote_ip) ||*/
                        ((time(NULL) - session_found->last_access) > SESSION_TTL)
                  ) break;
                if(session_found->state == e_session_sent_request)
                {
                    session_found->state = e_session_authorized;
                    snprintf(session_found->user, sizeof(session_found->user), "%s", ah->user);
                }
                else if(strcmp(session_found->user, ah->user) != 0) break;

                long recived_nc = strtol(ah->nc, 0, 16);
                time_t received_time = time(NULL);
/*
//printf("%d : %lu : %lu : %lu : %lu\n", request_info->remote_port, ses.last_nc, recived_nc, ses.last_access, received_time);
                if((ses.last_nc+1) != recived_nc){
                    //lets see if we can manage out of order sent by some browsers
                    if((received_time - ses.last_access) > 2) break;
                    //inside a window of 2 seconds we tolerate nc out of order
                    if(ses.last_nc > recived_nc) recived_nc = ses.last_nc;
                }
*/
                session_found->last_access = received_time;
                session_found->last_nc = recived_nc;
                result = SQTrue;
            } while(0);

            if(!result){
                session_found->state = e_session_invalid;
                session_found->nonce[0] = '\0';
            }
       }
       else {
           //dbg_msg("Session not found ! : %s : %s\n", ah->nonce, request_info->uri);
       }

        mg_thread_mutex_unlock(&session_rwlock);
    }

    return result;
}

static SQRESULT
sq_http_request_check_password(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, my_tmp_password);
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    sq_pushbool(v, my_check_password(request_info, my_tmp_password));
    return 1;
}

// Close user session return 1 if closed
static SQBool my_close_session(const struct mg_request_info *request_info)
{
    SQBool result = SQFalse;
    struct st_session_t *session_found = 0;
    struct mg_auth_header *ah = request_info->ah;

    // Some of the parameters may be NULL
    if (request_info->request_method == NULL ||
            //strcmp(ri->request_method, "PUT") == 0 ||
            //strcmp(ri->request_method, "DELETE") == 0 ||
            ah == NULL ||
            ah->nonce == NULL || ah->nc == NULL ||
            ah->cnonce == NULL || ah->qop == NULL || ah->uri == NULL ||
            ah->response == NULL)
    {
        return 0;
    }

    mg_thread_mutex_lock(&session_rwlock);

    session_found = my_get_session_only_after_lock(ah->nonce);
    if(session_found) {
        session_found->state = e_session_invalid;
        session_found->nonce[0] = '\0';
        result = SQTrue;
    }
    mg_thread_mutex_unlock(&session_rwlock);
    return result;
}

static SQRESULT
sq_http_request_close_session(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    sq_pushbool(v, my_close_session(request_info));
    return 1;
}

typedef char buf_1024_t[1024];

static const char *my_encode_nonce(buf_1024_t buf, md5_buf_t md5_buf, unsigned long ip)
{
    snprintf(buf, sizeof(buf_1024_t), "%lu:%d:%d:%lu", (unsigned long) time(NULL), rand(), rand(), ip);
    mg_md5(md5_buf, buf, NULL);
    return md5_buf;
}

static void my_send_authorization_request(struct mg_connection *conn,
        const struct mg_request_info *request_info,
        const char *authentication_domain,
        const char *nonce)
{
    md5_buf_t md5_buf;
    buf_1024_t buf;
    int i;
    if (nonce == NULL)
    {
        nonce = my_encode_nonce(buf, md5_buf, (unsigned long) request_info->remote_ip);
    }
    struct st_session_t *available_session = NULL;

    mg_thread_mutex_lock(&session_rwlock);

    time_t now = time(NULL);

    for(int j=0; j<3; ++j)
    {
        for(i=0; i < MAX_SESSIONS; ++i){
            if(sessions[i].state == e_session_invalid){
                break;
            }
            if(sessions[i].state == e_session_sent_request){
                if((now - sessions[i].last_access) > SESSION_REQUEST_TTL){
                    //if session request bigger than 2 minutes reuse it
                    break;
                }
            }
            //on the second intent we will reuse idle authorized sessions
            if( (j > 0) && sessions[i].state == e_session_authorized){
                if((now - sessions[i].last_access) > (SESSION_REQUEST_TTL*5)){
                    //if session request bigger than 10 minutes reuse it
                    break;
                }
            }
        }

        if(i < MAX_SESSIONS){
            available_session = &sessions[i];
            available_session->state = e_session_sent_request;
            available_session->last_access = now;
            available_session->last_nc = 0;
            available_session->remote_ip = request_info->remote_ip;
            snprintf(available_session->nonce, sizeof(available_session->nonce),
                     "%s", nonce);
            break;
        }
        else
        {
            nonce = my_encode_nonce(buf, md5_buf, (unsigned long) request_info->remote_ip);
        }
    }

    mg_thread_mutex_unlock(&session_rwlock);

    if(available_session) {
        //warning android ics/jelly bean expect realm to follow digest
        //otherwise it will not connect
        i = snprintf(buf, sizeof(buf),
            "HTTP/1.1 401 Unauthorized\r\n"
            "WWW-Authenticate: Digest realm=\"%s\", qop=\"auth\", nonce=\"%s\"\r\n"
            "Content-Length: 0\r\n\r\n",
            authentication_domain, nonce);
    } else {
        i = snprintf(buf, sizeof(buf),
            "HTTP/1.1 503 Service Temporary Unavailable\r\n"
            "Content-Length: 0\r\n\r\n");
    }
    //printf("%s", buf);
    mg_write(conn, buf, i);
}

static SQRESULT
sq_http_request_send_authorization_request(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_http_request_INSTANCE();
    SQ_GET_STRING(v, 2, authentication_domain);
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    my_send_authorization_request(conn, request_info, authentication_domain, NULL);
    return 0;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_http_request_##name,nparams,tycheck}
static SQRegFunction mg_http_request_methods[] =
{
	_DECL_FUNC(constructor,  1, _SC("x")),

	_DECL_FUNC(print,  -2, _SC("x n|s")),
	_DECL_FUNC(read,  -2, _SC("xix")),
	_DECL_FUNC(write,  3, _SC("xsi")),
	_DECL_FUNC(write_blob,  -2, _SC("xxi")),
	_DECL_FUNC(get_var,  3, _SC("xs")),
	_DECL_FUNC(get_cookie,  2, _SC("xs")),
	_DECL_FUNC(get_header,  2, _SC("xs")),
	_DECL_FUNC(send_file,  2, _SC("xs")),
#ifndef _WIN32_WCE
	_DECL_FUNC(handle_cgi_request,  2, _SC("xs")),
#endif
	_DECL_FUNC(get_option,  2, _SC("xs")),
	_DECL_FUNC(check_password,  2, _SC("xs")),
	_DECL_FUNC(close_session,  1, _SC("x")),
	_DECL_FUNC(send_authorization_request,  2, _SC("xs")),
	_DECL_FUNC(get_conn_buf,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC

/***********/

// helper function to extract a single mg_config value from a Lua table
static void
fetchfield(HSQUIRRELVM v, int idx, const char *key, char **value, const char *d)
{
    const char *s;
    sq_pushstring(v, key, -1);
    if(sq_rawget(v, idx) == SQ_OK){
        if(sq_gettype(v, -1) != OT_NULL){
            sq_tostring(v, -1);
            sq_getstring(v, -1, &s);
            *value = mg_strdup(s);
            sq_pop(v, 2); //retrieved value and converted value
            return;
        }
    }
    *value = NULL;
}

// initializes an options string array from a Lua table
static SQRESULT
fetchoptions(HSQUIRRELVM v, int idx, const char **options)
{
    struct {
        const char *key;
        const char *value;
    } OPTIONS[] = {
        { "cgi_extensions", ".cgi,.pl,.php" },
        { "cgi_environment", NULL },
        { "put_delete_passwords_file", NULL },
        { "cgi_interpreter", NULL },
        { "protect_uri", NULL },
        { "authentication_domain", "mydomain.com" },
        { "ssi_extensions", ".shtml,.shtm" },
        { "access_log_file", NULL },
        { "ssl_chain_file", NULL },
        { "enable_directory_listing", "yes" },
        { "error_log_file", NULL },
        { "global_passwords_file", NULL },
        { "index_files", "index.html,index.htm,index.cgi" },
        { "enable_keep_alive", "no" },
        { "enable_tcp_nodelay", "no" },
        { "access_control_list", NULL },
        { "max_request_size", "16384" },
        { "extra_mime_types", NULL },
        { "listening_ports", "8080" },
        { "document_root",  "." },
        { "ssl_certificate", NULL },
        { "num_threads", "10" },
        { "request_timeout_ms", "30000" },
        { "run_as_user", NULL },
        { NULL, NULL }
    };
    char *value;
    int i, j;

    if(sq_gettype(v, idx) != OT_TABLE) return sq_throwerror(v, "a table is needed to retrieve options");

    for (i = 0, j = 0; OPTIONS[i].key; ++i) {
        fetchfield(v, idx, OPTIONS[i].key, &value, OPTIONS[i].value);
        if (NULL != value) {
            options[j++] = mg_strdup(OPTIONS[i].key);
            options[j++] = value;
        }
    }
    options[j] = NULL;
    return 0;
}

/***********/

static const char sq_mongoose_TAG[] = "Mongoose";
static SQBool show_errors_on_stdout = SQFalse;

static void sq_mongoose_release_references(SQ_Mg_Context *self){
    SQ_MG_Callback_free(self->master_plugin);
    SQ_MG_Callback_free(self->master_plugin_exit);
    SQ_MG_Callback_free(self->user_callback);
    SQ_MG_Callback_free(self->user_callback_setup);
    SQ_MG_Callback_free(self->user_callback_exit);
}

static SQRESULT sq_mongoose_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
    SQ_Mg_Context *self = (SQ_Mg_Context *)p;
    if(self){
        if(self->ctx){
            mg_stop(self->ctx);
            sq_mongoose_release_references(self);
            self->ctx = 0;
        }
        sq_free(self, sizeof(SQ_Mg_Context));
    }
	return 0;
}

static SQRESULT sq_mongoose_constructor(HSQUIRRELVM v)
{
    //SQ_FUNC_VARS_NO_TOP(v);
    SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *)sq_malloc(sizeof(SQ_Mg_Context));
    memset(sq_mg_ctx, 0, sizeof(SQ_Mg_Context));

    sq_setinstanceup(v, 1, sq_mg_ctx);
    sq_setreleasehook(v,1, sq_mongoose_releasehook);
	return 1;
}

#define GET_mongoose_INSTANCE() \
    SQ_Mg_Context *self; \
    if((_rc_ = sq_getinstanceup(v,1,(SQUserPointer*)&self,(void*)sq_mongoose_TAG)) < 0) return _rc_;

static SQRESULT
sq_mongoose_show_errors_on_stdout(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_BOOL(v, 2, onOff);
    show_errors_on_stdout = onOff;
    return 0;
}

static SQRESULT
sq_mongoose_modify_passwords_file(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, passwords_file_name);
	SQ_GET_STRING(v, 3, domain);
	SQ_GET_STRING(v, 4, user);
	SQ_GET_STRING(v, 5, password);

    sq_pushinteger(v, mg_modify_passwords_file(passwords_file_name,
                             domain, user, password));
    return 1;
}

static SQRESULT
sq_mongoose_version(HSQUIRRELVM v)
{
    sq_pushstring(v, mg_version(), -1);
    return 1;
}

SQInteger blob_write(SQUserPointer file,SQUserPointer p,SQInteger size);
// creates a reference dispatching callbacks to squirrel functions
static SQRESULT
fetchcallback(HSQUIRRELVM v, const char *key, SQ_MG_Callback **sq_cb)
{
    if(!sq_cb) return sq_throwerror(v, "inavlid SQ_MG_Callback parameter value (NULL)");
    if(sq_gettype(v, -1) != OT_TABLE) return sq_throwerror(v, "table expected to fetch callbacks");
    sq_pushstring(v, key, -1);
    sq_rawget(v, -2);

    if (sq_gettype(v, -1) == OT_CLOSURE){
        SQBlob b(0,8192);
        if(SQ_SUCCEEDED(sq_writeclosure(v,blob_write,&b))) {
            *sq_cb = SQ_MG_Callback_malloc(b.Len());
            if(*sq_cb) memcpy((*sq_cb)->buf, b.GetBuf(), b.Len());
            sq_poptop(v);
        }
    }
    else
    {
        sq_pop(v, 1);
        return sq_throwerror(v, "closure expected for callbacks");
    }
    return SQ_OK;
}

static void *
user_callback_proxy(enum mg_event event,
                    struct mg_connection *conn,
                    const struct mg_request_info *ri);

// creates a new server using a configuration table
static SQRESULT
sq_mongoose_start(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mongoose_INSTANCE();
	_rc_ = 1; //assumes succeed

    const char *options[64];
    SQInteger i;

    if(self->ctx) return sq_throwerror(v, _SC("mongoose already running or stoped incorrectly"));

    // store the Squirrel vm for use in callback proxies
    self->v = v;

    // prepare the mg_config structure from the squirrel table argument
    memset(options, 0, sizeof (options));
    fetchoptions(v, 2, options);

    fetchcallback(v, "master_plugin", &self->master_plugin);
    fetchcallback(v, "master_plugin_exit", &self->master_plugin_exit);

    fetchcallback(v, "user_callback", &self->user_callback);
    fetchcallback(v, "user_callback_setup", &self->user_callback_setup);
    fetchcallback(v, "user_callback_exit", &self->user_callback_exit);

    self->ctx = mg_start(user_callback_proxy, self, options);

    // free the options string list memory
    for (i = 0; options[i]; ++i)
        free((void *) options[i]);

    // throw an error if the server did not start
    if (self->ctx == NULL) {
        sq_mongoose_release_references(self);
        _rc_ = sq_throwerror(v, "could not start mongoose");
    }

    // return the context so it can be stopped later
    return _rc_;
}

static SQRESULT
sq_mongoose_stop(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_mongoose_INSTANCE();

    if(self->ctx){
        mg_stop(self->ctx);
        sq_mongoose_release_references(self);
        self->ctx = 0;
    }

    return 0;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_mongoose_##name,nparams,tycheck}
static SQRegFunction sq_mongoose_methods[] =
{
	_DECL_FUNC(constructor,  1, _SC("x")),

	_DECL_FUNC(start,  -1, _SC("x")),
	_DECL_FUNC(stop,  1, _SC("x")),
	_DECL_FUNC(modify_passwords_file,  1, _SC("x")),
	_DECL_FUNC(version,  1, _SC("x")),
	_DECL_FUNC(show_errors_on_stdout,  2, _SC("xb")),
	{0,0}
};
#undef _DECL_FUNC

static SQRESULT
sq_mg_url_decode_base(HSQUIRRELVM v, SQInteger is_form_url_encoded)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, src);

    int dst_len = src_size +1;
    char *dst = sq_getscratchpad(v,dst_len);
    dst_len = mg_url_decode(src, src_size, dst, dst_len, is_form_url_encoded);
    sq_pushstring(v, dst, dst_len);
    return 1;
}

static SQRESULT
sq_mg_url_decode(HSQUIRRELVM v)
{
    return sq_mg_url_decode_base(v, 1);
}

static SQRESULT
sq_mg_uri_decode(HSQUIRRELVM v)
{
    return sq_mg_url_decode_base(v, 0);
}

static SQRESULT
sq_mg_url_encode(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, src);

    char *dst = mg_url_encode(src);

    sq_pushstring(v, dst, -1);
    free(dst);
    return 1;
}

static SQRESULT
sq_mg_get_builtin_mime_type(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, src);

    const char *dst = mg_get_builtin_mime_type(src);

    sq_pushstring(v, dst, -1);
    return 1;
}

static SQRESULT
sq_mg_md5(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);

    char buf[32 + 1];
    unsigned char hash[16];
    MD5_CTX ctx;
    MD5Init(&ctx);

    for (int i = 2; i <= _top_; ++i) {
        SQ_GET_STRING(v, i, p);
        MD5Update(&ctx, (const unsigned char *) p, p_size);
    }

    MD5Final(hash, &ctx);
    mg_bin2str(buf, hash, sizeof(hash));
    sq_pushstring(v, buf, -1);
    return 1;
}

static SQRESULT
sq_mg_debug_print(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQInteger i, write_count = 0;

    for (i = 2; i <= _top_; ++i) {
        sq_tostring(v, i);
        SQ_GET_STRING(v, -1, value);
        write_count += fwrite(value, 1, value_size, stderr);
        sq_poptop(v);
    }

    sq_pushinteger(v, write_count);
    return 1;
}

#ifdef JNI_ENABLE_LOG
#include <android/log.h>

/*
** A callback for the android_log() SQIntegererface.
*/
static void jniLog(int iErrCode, const char *zMsg){
  __android_log_print(ANDROID_LOG_ERROR,"jniLuaServerLog","(%d) %s\n", iErrCode, zMsg);
}

static SQRESULT sq_mg_jniLog(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, code);
    SQ_GET_STRING(v, 3, error_message);
    jniLog(code, error_message);
    return 0;
}
#endif

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_mg_##name,nparams,tycheck, SQTrue}
static SQRegFunction sq_mg_methods[] =
{
	_DECL_FUNC(url_decode,  2, _SC(".s")),
	_DECL_FUNC(uri_decode,  2, _SC(".s")),
	_DECL_FUNC(url_encode,  2, _SC(".s")),
	_DECL_FUNC(get_builtin_mime_type,  2, _SC(".s")),
	_DECL_FUNC(md5,  -2, _SC(".s")),
#ifdef JNI_ENABLE_LOG
	_DECL_FUNC(jniLog,  -2, _SC(".s")),
#endif
	_DECL_FUNC(debug_print,  -2, _SC(".s")),
	{0,0}
};
#undef _DECL_FUNC

static void reg_string(HSQUIRRELVM v, const char *name, const char *val) {
  sq_pushstring(v, name, -1);
  if(val) sq_pushstring(v, val, -1);
  else sq_pushnull(v);
  sq_rawset(v, -3);
}

static void reg_integer(HSQUIRRELVM v, const char *name, int val) {
  sq_pushstring(v, name, -1);
  sq_pushinteger(v, val);
  sq_rawset(v, -3);
}

// pushes request info on the Lua stack as a table
static void
push_request(HSQUIRRELVM v, const struct mg_request_info *ri)
{
    int i;

    sq_pushliteral(v, _SC("info"));
    sq_get(v, -2);
#define NEWSLOT_STR(ks) reg_string(v, #ks, ri->ks);
    NEWSLOT_STR(request_method);
    NEWSLOT_STR(uri);
    NEWSLOT_STR(http_version);
    NEWSLOT_STR(query_string);
    NEWSLOT_STR(remote_user);
    NEWSLOT_STR(log_message);
#define NEWSLOT_INT(ks) reg_integer(v, #ks, ri->ks);
    NEWSLOT_INT(remote_ip);
    NEWSLOT_INT(remote_port);
    NEWSLOT_INT(status_code);
    NEWSLOT_INT(is_ssl);
#undef NEWSLOT_STR
#undef NEWSLOT_INT

    sq_pushliteral(v, "http_headers");
    sq_newtable(v);
    for (i = 0; i < ri->num_headers; ++i) {
        reg_string(v, ri->http_headers[i].name, ri->http_headers[i].value);
    }
    sq_newslot(v, -3, SQFalse);
    sq_poptop(v); //remove data table
}

#if 0
static SQRESULT
sq_mg_pcall_master_plugin(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, func_name);
    const SQChar *error_message = _SC("No master plugin installed !");
    HSQUIRRELVM master_plugin = (HSQUIRRELVM) mg_lock_master_plugin(conn);
    if(master_plugin){
        int master_plugin_saved_top = sq_gettop(master_plugin);
        //sq_pushcfunction(master_plugin, traceback);  /* push traceback function */
        //int error_func = sq_gettop(master_plugin);

        sq_pushstring(master_plugin, func_name, func_name_size);
        sq_getonroottable(master_plugin);
        if(sq_gettype(master_plugin, -1) == OT_CLOSURE){
            sq_pushroottable(master_plugin);
            int arg_top = sq_gettop (v);
            /* Push arguments to dst stack */
            int idx = 4;
            copy_values_between_vms (master_plugin, v, arg_top-idx, idx);
            if (sq_pcall (master_plugin, arg_top-idx+1, SQTrue, SQTrue) == SQ_OK) {
              /* run OK? */
              int ret_top = sq_gettop (master_plugin);
              /* Push status = OK */
              sq_pushbool (v, SQTrue);
              /* Return values to src */
              //copy_values_between_vms (v, master_plugin, master_plugin_saved_top+2, ret_top);
              /* pops debug.traceback and result from dst state */
              //sq_settop(master_plugin, master_plugin_saved_top);
              /*unlock master plugin*/
              mg_unlock_master_plugin(conn);
              /* Return true (success) plus return values */
              return 1;
            }
            error_message = sq_tostring (master_plugin, -1);
        } else error_message = "Attempt to call an invalid function on master plugin !";

        /* pops debug.traceback and result from dst state */
        sq_settop(master_plugin, master_plugin_saved_top);
        /*unlock master plugin*/
        mg_unlock_master_plugin(conn);
    }

    return sq_throwerror(v, error_message);
}
#endif

static void write_error_message(struct mg_connection *conn,
                        const char * error_msg, SQInteger error_len){
#define PRE_TAG_OPEN "<pre>"
#define PRE_TAG_CLOSE "</pre>"
    mg_write(conn, PRE_TAG_OPEN, sizeof(PRE_TAG_OPEN));
    if(error_len < 0) error_len = strlen(error_msg);
    mg_write(conn, error_msg, error_len);
    mg_write(conn, PRE_TAG_CLOSE, sizeof(PRE_TAG_CLOSE));
}

#ifdef __cplusplus
extern "C" {
#endif
SQUIRREL_API SQRESULT sqext_register_SQLite3(HSQUIRRELVM v);
SQRESULT sqext_register_MySQL(HSQUIRRELVM v);
SQRESULT sqext_register_PostgreSQL(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_base64(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_mix(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_bloblib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_iolib(HSQUIRRELVM v);
SQUIRREL_API SQInteger sqstd_register_systemlib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_mathlib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_stringlib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_Sq_Fpdf(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_sqfs(HSQUIRRELVM v);
SQUIRREL_API void sqstd_seterrorhandlers(HSQUIRRELVM v);
SQUIRREL_API void sqstd_printcallstack(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_sq_socket(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_sq_slave_vm(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_axtls (HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqext_register_openssl (HSQUIRRELVM v);
SQRESULT sqext_register_decimal(HSQUIRRELVM v);
SQRESULT sqext_register_sq_zlib(HSQUIRRELVM v);
SQRESULT sqext_register_sq_blosc(HSQUIRRELVM v);
SQRESULT sqext_register_pcre2(HSQUIRRELVM v);
SQRESULT sqext_register_markdown(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

static sq_modules_preload_st modules_preload[] = {
    {"blob", sqstd_register_bloblib},
    {"io", sqstd_register_iolib},
    {"math", sqstd_register_mathlib},
    {"sys", sqstd_register_systemlib},
    {"string", sqstd_register_stringlib},
    {"base64", sqext_register_base64},
#ifndef NO_SQ_PDF
    {"fpdf", sqext_register_Sq_Fpdf},
#endif
    {"sqlite3", sqext_register_SQLite3},
    {"zlib", sqext_register_sq_zlib},
    {"markdown", sqext_register_markdown},
#if defined(SQ_USE_PCRE2) || defined(SQ_USE_PCRE2_STATIC)
    {"pcre2", sqext_register_pcre2},
#endif
#ifdef SQ_USE_CBLOSC
    {"blosc", sqext_register_sq_blosc},
#endif // SQ_USE_CBLOSC
#ifdef WITH_MYSQL
    {"mysql", sqext_register_MySQL},
#endif
#ifdef WITH_POSTGRESQL
    {"postgresql", sqext_register_PostgreSQL},
#endif
    {"sqfs", sqext_register_sqfs},
    {"mix", sqext_register_mix},
    {"socket", sqext_register_sq_socket},
    {"slave_vm", sqext_register_sq_slave_vm},
#ifdef USE_AXTLS
    {"axtls", sqext_register_axtls},
#endif
#ifdef USE_OPENSSL
    {"openssl", sqext_register_openssl},
#endif
    {NULL, NULL}
};

void sq_printfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
#ifdef JNI_ENABLE_LOG
    char buf[2048];
    vsnprintf(buf, sizeof(buf), s, vl);
    jniLog(-1, buf);
#else
	vfprintf(stdout, s, vl);
#endif
	va_end(vl);
}

void sq_errorfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
#ifdef JNI_ENABLE_LOG
    char buf[2048];
    vsnprintf(buf, sizeof(buf), s, vl);
    jniLog(-2, buf);
#else
	vfprintf(stderr, s, vl);
#endif
	va_end(vl);
}

#define INT_CONST(v,num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);

static HSQUIRRELVM my_new_squirrel(struct mg_context *ctx) {
    HSQUIRRELVM v = sq_open(1024);
    if(!v) return 0;

	sqstd_seterrorhandlers(v); //registers the default error handlers
	sq_setprintfunc(v, sq_printfunc, sq_errorfunc); //sets the print function

    sq_pushroottable(v);

    sq_modules_preload_st *mp = modules_preload;
    sq_preload_modules(v, mp);

    while(mp->module_load_func){
      mp->module_load_func(v);
      ++mp;
    }

    sq_pushstring(v,sq_http_request_TAG, -1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)sq_http_request_TAG);
    sq_insert_reg_funcs(v, mg_http_request_methods);
    sq_pushstring(v, _SC("info"), -1);
    sq_newtable(v);
    sq_newslot(v,-3,SQFalse);
    sq_newslot(v,-3,SQFalse);

    sq_insert_reg_funcs(v, sq_mg_methods);

    sq_pushliteral(v, "APP_ROOT_FOLDER");
    sq_pushstring(v, mg_get_option(ctx, "document_root"), -1);
    sq_newslot(v, -3, SQFalse);

    sq_poptop(v); //remove roottable

    return v;
}

SQInteger blob_read(SQUserPointer file,SQUserPointer p,SQInteger size);
SQInteger loadstring(HSQUIRRELVM v, const char * bcode, SQInteger bcode_size)
{
    SQBlob b(0, bcode_size);
    b.Write(bcode, bcode_size);
    b.Seek(0, SQ_SEEK_SET);
    SQInteger rc = sq_readclosure(v, blob_read, &b);
	return rc < 0 ? rc : 1;
}

// dispatches a callback to a Lua function if one is registered
static void *
user_callback_proxy(enum mg_event event,
                    struct mg_connection *conn,
                    const struct mg_request_info *ri)
{
    HSQUIRRELVM v;
    int e;

    switch(event){
        case MG_NEW_MASTER_PLUGIN: {
            SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) conn;
            SQ_MG_Callback *cb = sq_mg_ctx->master_plugin;
            v = my_new_squirrel((struct mg_context *)ri);
            if(!v) return 0;

            if(cb){
                if (loadstring(v, cb->buf, cb->size)  == 1) {
                    sq_pushroottable(v);
                    if(sq_call(v, 1, SQFalse, SQTrue) != SQ_OK){
                        sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                    }
                    sq_poptop(v); //remove function from stack
                }
            }
            return v;
        }
        break;

        case MG_FREE_MASTER_PLUGIN:
            v = (HSQUIRRELVM)conn;
            if(v) {
                SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) ri;
                SQ_MG_Callback *cb = sq_mg_ctx->master_plugin_exit;
                if(cb){
                    if (loadstring(v, cb->buf, cb->size) == 1) {
                        sq_pushroottable(v);
                        if(sq_call(v, 1, SQFalse, SQFalse) != SQ_OK){
                            sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                        }
                        sq_poptop(v); //remove function from stack
                    }
                    else sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                }
                sq_close(v);
            }
            return 0;
        break;

        case MG_NEW_PLUGIN:{
            SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) mg_get_user_data(conn);
            if(!sq_mg_ctx) return 0;
            SQ_MG_Callback *cb = sq_mg_ctx->user_callback;
            if (cb) {
                v = my_new_squirrel((struct mg_context *)ri);
                if(!v) return 0;
                SQInteger top = sq_gettop(v);
                SQ_MG_Callback *cb_setup = sq_mg_ctx->user_callback_setup;
                if(cb_setup){
                    if (loadstring(v, cb_setup->buf, cb_setup->size) == 1) {
                        sq_pushroottable(v);
                        if(sq_call(v, 1, SQFalse, SQFalse) != SQ_OK){
                            sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                        }
                        sq_poptop(v); //remove function from stack
                    }
                    else sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                }
                sq_pushroottable(v);
                sq_pushstring(v, sq_mg_user_callback, -1);
                if (loadstring(v, cb->buf, cb->size) == 1) {
                    sq_newslot(v, -3, SQFalse);
                }
                else sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                sq_settop(v, top); //remove everithing left on stack while working here
                return v;
            }
        }
        return 0;

        case MG_FREE_PLUGIN:
            v = (HSQUIRRELVM)mg_get_plugin(conn);
            if(v){
                SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) ri;
                if(sq_mg_ctx){
                    SQ_MG_Callback *cb = sq_mg_ctx->user_callback_exit;
                    if(cb){
                        if (loadstring(v, cb->buf, cb->size) == 1) {
                            sq_pushroottable(v);
                            if(sq_call(v, 1, SQFalse, SQFalse) != SQ_OK){
                                sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                            }
                            sq_poptop(v); //remove function from stack
                        }
                        else sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                    }
                }
                sq_close(v);
            }
        return NULL;

        case MG_INIT_SSL:
#if defined(USE_AXTLS) || defined(USE_OPENSSL)
            //lua_pushstring(L, "MG_INIT_SSL");
            *((void**)conn) = SSL_CTX_new(0);
#endif
            return 0;
        case MG_NEW_REQUEST:
        case MG_HTTP_ERROR:
        case MG_EVENT_LOG:{
            v = (HSQUIRRELVM) mg_get_plugin(conn);

            if (v) {
                SQInteger saved_top = sq_gettop(v);
                //SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) mg_get_user_data(conn);
                sq_pushstring(v, sq_mg_user_callback, -1);
                if(sq_getonroottable(v) != SQ_OK) {
                    write_error_message(conn, "failed to find mg_user_callback", -1);
                    return NULL;
                }
                sq_pushroottable(v);

#define CASE(n) case n: sq_pushstring(v, #n, -1);break
                switch(event){
                    CASE(MG_INIT_SSL);
                    CASE(MG_HTTP_ERROR);
                    CASE(MG_EVENT_LOG);
                    CASE(MG_NEW_REQUEST);
                    default:
                        sq_pushnull(v);
                }

                sq_pushstring(v, sq_http_request_TAG, -1);
                if(sq_getonroottable(v) == SQ_OK){
                    sq_pushroottable(v);
                    if(sq_call(v, 1, SQTrue, SQFalse) == SQ_OK){
                        sq_remove(v, -2); //remove class
                        sq_setinstanceup(v, -1, conn);
                    }
                }
                push_request(v, ri);

                if(sq_call(v, 3, SQTrue, SQTrue) != SQ_OK) {
                    write_error_message(conn, sq_getlasterror_str(v), -1);
                    e = 0;
                } else {
                    SQBool bval;
                    if(sq_getbool(v, -1, &bval) == SQ_OK) e = bval == SQTrue ? 1 : 0;
                    else e = 0;
                }

                sq_settop(v, saved_top);
                return (void *) e;
            }
        }

        default:
            return NULL;
    }

    return NULL;
}

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_mongoose(HSQUIRRELVM v)
    {
        sq_insert_reg_funcs(v, sq_mg_methods);
        sq_pushstring(v,sq_mongoose_TAG, -1);
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)sq_mongoose_TAG);
        sq_insert_reg_funcs(v, sq_mongoose_methods);
        sq_newslot(v,-3,SQTrue);
        return 1;
    }

#ifdef __cplusplus
}
#endif

