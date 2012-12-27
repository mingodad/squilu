#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "squirrel.h"
SQ_OPT_STRING_STRLEN();

#ifdef USE_SQ_SQLITE3
#include "lsqlite3.h"
#endif

#ifdef USE_AXTLS
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
    char *buf;
    size_t len;
    size_t size;
    const char *name;
};

struct SQ_Mg_Context {
    HSQUIRRELVM v;
    struct mg_context *ctx;
    SQ_MG_Callback master_plugin;
    SQ_MG_Callback master_plugin_exit;
    SQ_MG_Callback user_callback;
    SQ_MG_Callback user_callback_setup;
    SQ_MG_Callback user_callback_exit;
};

static const char SQ_MG_CONN_TAG[] = "sq_mg_conn_tag";

static SQRESULT sq_mg_conn_releasehook(SQUserPointer p, SQInteger size)
{
	return 1;
}

static SQRESULT sq_mg_conn_constructor(HSQUIRRELVM v)
{
    //SQ_FUNC_VARS_NO_TOP(v);
    struct mg_connection *conn = 0;

    sq_setinstanceup(v, 1, conn);
    sq_setreleasehook(v,1, sq_mg_conn_releasehook);
	return 1;
}

#define GET_MG_CONNECION() \
    struct mg_connection *conn; \
    if((_rc_ = sq_getinstanceup(v,1,(SQUserPointer*)&conn,(void*)SQ_MG_CONN_TAG)) < 0) return _rc_;

static SQRESULT
sq_mg_conn_print(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_MG_CONNECION();
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
sq_mg_conn_vm_print(HSQUIRRELVM v)
{
    GET_MG_CONNECION();
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
sq_mg_conn_write(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, buf);
    SQ_OPT_INTEGER(v, 3, write_size, buf_size);
    sq_pushinteger(v, mg_write(conn, buf, write_size));
    return 1;
}

static SQRESULT
sq_mg_conn_read(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_MG_CONNECION();
    SQ_OPT_INTEGER(v, 2, n, 1024*2000);
    size_t rlen;  /* how much to read */
    size_t nr;  /* number of chars actually read */
    rlen = 8192;  /* try to read that much each time */
#if 0
    SQBlob blob(0, rlen);
    if (rlen > n) rlen = n;  /* cannot read more than asked */
    char *p = sq_getscratchpad(v,rlen);
    do {
        nr = mg_read(conn, p, rlen);
        blob.Write(p, nr);
        n -= nr;  /* still have to read `n' chars */
    } while (n > 0 && nr == rlen);  /* until end of count or eof */
    sq_pushstring(v, (const SQchar *)blob.GetBuf(), blob.Len());  /* close buffer */
#endif
    return 1;
}

static SQRESULT
sq_mg_conn_send_file(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, file_path);
    mg_send_file(conn, file_path);
    return 0;
}

static SQRESULT
sq_mg_conn_write_blob(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
#if 0
    SQInteger saved_top = sq_gettop(v);
    memfile_raw_data(v);
    const void *buf = sq_touserdata(v, -2);
    size_t buf_len = sq_tointeger(v, -1);
    sq_settop(v, saved_top);
    sq_pushinteger(v, mg_write(conn, buf, buf_len));
#endif
    return 1;
}

static SQRESULT
sq_mg_conn_get_header(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, name);
    sq_pushstring(v, mg_get_header(conn, name), -1);
    return 1;
}

static SQRESULT
sq_mg_conn_get_cookie(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, cookie_name);

    const char *start;
    int len = mg_find_cookie(conn, cookie_name, &start);

    if(len > 0) sq_pushstring(v, start, len);
    else sq_pushnull(v);
    return 1;
}

static SQRESULT
sq_mg_conn_get_var(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
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
sq_mg_conn_handle_cgi_request(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, prog);

    mg_handle_cgi_request(conn, prog);

    return 0;
}

static SQRESULT
sq_mg_conn_get_option(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, name);
    SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) mg_get_user_data(conn);
    sq_pushstring(v, mg_get_option(sq_mg_ctx->ctx, name), -1);

    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_mg_conn_##name,nparams,tycheck}
static SQRegFunction mg_conn_methods[] =
{
	_DECL_FUNC(constructor,  1, _SC("x")),

	_DECL_FUNC(print,  -2, _SC("x n|s")),
	_DECL_FUNC(read,  2, _SC("xi")),
	_DECL_FUNC(write,  3, _SC("xsi")),
	_DECL_FUNC(write_blob,  -2, _SC("xxi")),
	_DECL_FUNC(get_var,  2, _SC("xs")),
	_DECL_FUNC(get_cookie,  2, _SC("xs")),
	_DECL_FUNC(get_header,  2, _SC("xs")),
	_DECL_FUNC(send_file,  2, _SC("xs")),
	_DECL_FUNC(handle_cgi_request,  2, _SC("xs")),
	_DECL_FUNC(get_option,  2, _SC("xs")),
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
        { "access_control_list", NULL },
        { "max_request_size", "16384" },
        { "extra_mime_types", NULL },
        { "listening_ports", "8080" },
        { "document_root",  "." },
        { "ssl_certificate", NULL },
        { "num_threads", "10" },
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

static const char SQ_MONGOOSE_TAG[] = "sq_mongoose_tag";
static SQBool show_errors_on_stdout = SQFalse;

static SQRESULT sq_mongoose_releasehook(SQUserPointer p, SQInteger size)
{
    SQ_Mg_Context *sq_mg_ctx = (    SQ_Mg_Context *)p;
    sq_free(sq_mg_ctx, sizeof(SQ_Mg_Context));
	return 1;
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
    if((_rc_ = sq_getinstanceup(v,1,(SQUserPointer*)&self,(void*)SQ_MONGOOSE_TAG)) < 0) return _rc_;

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

// creates a reference dispatching callbacks to squirrel functions
static SQRESULT
fetchcallback(HSQUIRRELVM v, const char *key, SQ_MG_Callback *dump)
{
    if(sq_gettype(v, -1) != OT_TABLE) return sq_throwerror(v, "table expected to fetch callbacks");
    sq_pushstring(v, key, -1);
    sq_rawget(v, -2);

    dump->buf = NULL;
    dump->len = 0;
    dump->size = 0;
    dump->name = key;
#if 0
    if (sq_gettype(v, -1) == OT_CLOSURE){
        SQBlob b(0, 8192);
        if (sqstd_writeclosure(v, blob_write, &b) != 0)
            return sq_throwerror(v, "unable to dump given function");
        dump->buf = malloc(b.Len());
        if(dump->buf) {
            dump->len = b.Len();
            memcpy(dump->buf, b.GetBuf(),  dump->len);
        }
    }
#endif
    sq_pop(v, 1);
    return 1;
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

    // store the Lua state for use in callback proxies
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

        if(self->user_callback.buf) {
            sq_free(self->user_callback.buf, self->user_callback.size);
            self->user_callback.buf = NULL;
        }
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
	_DECL_FUNC(show_errors_on_stdout,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC

static SQRESULT
sq_mg_url_decode_base(HSQUIRRELVM v, SQInteger is_form_url_encoded)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
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
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, src);

    char *dst = mg_url_encode(src);

    sq_pushstring(v, dst, -1);
    free(dst);
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
    GET_MG_CONNECION();
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

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_mg_##name,nparams,tycheck}
static SQRegFunction sq_mg_methods[] =
{
	_DECL_FUNC(url_decode,  2, _SC(".s")),
	_DECL_FUNC(uri_decode,  2, _SC(".s")),
	_DECL_FUNC(url_encode,  2, _SC(".s")),
	_DECL_FUNC(md5,  -2, _SC("xs")),
#ifdef JNI_ENABLE_LOG
	_DECL_FUNC(jniLog,  -2, _SC("xs")),
#endif
	_DECL_FUNC(debug_print,  -2, _SC("xs")),
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

    sq_newtable(v);
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
}

/*
** Copies values from State src to State dst.
*/
static int copy_values (HSQUIRRELVM dst, HSQUIRRELVM src, int i, int top) {
  int _rc_;
  sq_reservestack(dst, top - i + 1);
  for (; i <= top; i++) {
    switch (sq_gettype(src, i)) {
      case OT_INTEGER:
        SQ_GET_INTEGER(src, i, vint);
        sq_pushinteger(dst, vint);
        break;
      case OT_FLOAT:
        SQ_GET_FLOAT(src, i, vfloat);
        sq_pushfloat (dst, vfloat);
        break;
      case OT_BOOL:
        SQ_GET_BOOL(src, i, vbool);
        sq_pushbool (dst, vbool);
        break;
      case OT_STRING: {
        SQ_GET_STRING(src, i, vstr)
        sq_pushstring (dst, vstr, vstr_size);
        break;
      }
//      case sq_TLIGHTUSERDATA: {
//        sq_pushlightuserdata (dst, sq_touserdata (src, i));
//        break;
//      }
      case OT_NULL:
      default:
        sq_pushnull(dst);
        break;
    }
  }
}

static SQInteger
sq_mg_pcall_master_plugin(HSQUIRRELVM v)
{
#if 0
    SQ_FUNC_VARS_NO_TOP(v);
    GET_MG_CONNECION();
    SQ_GET_STRING(v, 2, func_name);
    const SQChar *error_message = _SC("No master plugin installed !");
    HSQUIRRELVM master_plugin = (HSQUIRRELVM) mg_lock_master_plugin(conn);
    if(master_plugin){
        int master_plugin_saved_top = sq_gettop(master_plugin);
        sq_pushcfunction(master_plugin, traceback);  /* push traceback function */
        int error_func = sq_gettop(master_plugin);

        sq_getglobal(master_plugin, func_name);
        if(sq_isfunction(master_plugin, -1)){
            int arg_top = sq_gettop (v);
            /* Push arguments to dst stack */
            int idx = 4;
            copy_values (master_plugin, v, idx, arg_top);
            if (sq_pcall (master_plugin, arg_top-idx+1, sq_MULTRET, error_func) == 0) {
              /* run OK? */
              int ret_top = sq_gettop (master_plugin);
              /* Push status = OK */
              sq_pushbool (v, SQTrue);
              /* Return values to src */
              copy_values (v, master_plugin, master_plugin_saved_top+2, ret_top);
              /* pops debug.traceback and result from dst state */
              sq_settop(master_plugin, master_plugin_saved_top);
              /*unlock master plugin*/
              mg_unlock_master_plugin(conn);
              /* Return true (success) plus return values */
              return (ret_top-master_plugin_saved_top);
            }
            error_message = sq_tostring (master_plugin, -1);
        } else error_message = "Attempt to call an invalid function on master plugin !";

        /* pops debug.traceback and result from dst state */
        sq_settop(master_plugin, master_plugin_saved_top);
        /*unlock master plugin*/
        mg_unlock_master_plugin(conn);
    }

    return sq_throwerror(v, error_message);
#endif
}

static void write_error_message(struct mg_connection *conn,
                        const char * error_msg, SQInteger error_len){
#define PRE_TAG_OPEN "<pre>"
#define PRE_TAG_CLOSE "</pre>"
    mg_write(conn, PRE_TAG_OPEN, sizeof(PRE_TAG_OPEN));
    mg_write(conn, error_msg, error_len);
    mg_write(conn, PRE_TAG_CLOSE, sizeof(PRE_TAG_CLOSE));
}

#ifdef __cplusplus
extern "C" {
#endif
SQUIRREL_API SQRESULT sqstd_register_SQLite3(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_base64(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sq_register_mix(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_bloblib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_iolib(HSQUIRRELVM v);
SQUIRREL_API SQInteger sqstd_register_systemlib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_mathlib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_stringlib(HSQUIRRELVM v);
SQUIRREL_API SQRESULT sqstd_register_Sq_Fpdf(HSQUIRRELVM v);
//SQUIRREL_API SQRESULT sqopen_lfs(HSQUIRRELVM v);
SQUIRREL_API void sqstd_seterrorhandlers(HSQUIRRELVM v);
SQUIRREL_API void sqstd_printcallstack(HSQUIRRELVM v);
#ifdef __cplusplus
} /*extern "C"*/
#endif

void sq_printfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	vfprintf(stdout, s, vl);
	va_end(vl);
}

void sq_errorfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	vfprintf(stderr, s, vl);
	va_end(vl);
}


static HSQUIRRELVM my_new_squirrel(struct mg_context *ctx) {
    HSQUIRRELVM v = sq_open(1024);
    if(!v) return 0;
    sq_pushroottable(v);

	sqstd_register_bloblib(v);
	sqstd_register_iolib(v);
	sqstd_register_systemlib(v);
	sqstd_register_mathlib(v);
	sqstd_register_stringlib(v);
	sqstd_register_base64(v);
	sqstd_register_Sq_Fpdf(v);
	sqstd_register_SQLite3(v);
	sq_register_mix(v);

	sqstd_seterrorhandlers(v); //registers the default error handlers
	sq_setprintfunc(v, sq_printfunc, sq_errorfunc); //sets the print function

    sq_pushliteral(v,_SC("mg_connection"));
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)SQ_MG_CONN_TAG);
    sq_insert_reg_funcs(v, mg_conn_methods);
    sq_newslot(v,-3,SQTrue);

    sq_insert_reg_funcs(v, sq_mg_methods);

    sq_pushliteral(v, "APP_ROOT_FOLDER");
    sq_pushstring(v, mg_get_option(ctx, "document_root"), -1);
    sq_newslot(v, -3, SQFalse);

    sq_poptop(v); //remove roottable

    return v;
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
            SQ_MG_Callback *dump = &sq_mg_ctx->master_plugin;
            v = my_new_squirrel((struct mg_context *)ri);
            if(!v) return 0;
            if(dump->buf){
                if (sq_compilebuffer(v, dump->buf, dump->len, dump->name, SQFalse) == SQ_OK) {
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
                SQ_MG_Callback *dump = &sq_mg_ctx->master_plugin_exit;
                if(dump->buf){
                    if (sq_compilebuffer(v, dump->buf, dump->len, dump->name, SQFalse) == SQ_OK) {
                        sq_pushroottable(v);
                        if(sq_call(v, 1, SQFalse, SQTrue) != SQ_OK){
                            sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                        }
                        sq_poptop(v); //remove function from stack
                    }
                }
                sq_close(v);
            }
            return 0;
        break;

        case MG_NEW_PLUGIN:{
            SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) mg_get_user_data(conn);
            if(!sq_mg_ctx) return 0;
            SQ_MG_Callback *dump = &sq_mg_ctx->user_callback;
            if (dump->buf) {
                v = my_new_squirrel((struct mg_context *)ri);
                if(!v) return 0;
                sq_pushroottable(v);

                sq_pushregistrytable(v);
                sq_pushstring(v, SQ_MG_CONN_TAG,-1);
                int rc = sq_rawget(v, -2);
                sq_remove(v, -2); //remove registrytable
                sq_pushroottable(v);
                rc = sq_call(v, 1, SQTrue, SQFalse);
                sq_remove(v, -2); //class
                rc = sq_getinstanceup(v, -1, (void**)sq_mg_ctx, (void*)SQ_MG_CONN_TAG);



                //_g_io_dostring(v, dump->buf, dump->len, dump->name);
                //sq_setglobal(v, "__manage_conn___");
                dump = &sq_mg_ctx->user_callback_setup;
                if(dump->buf){
                    if (sq_compilebuffer(v, dump->buf, dump->len, dump->name, SQFalse) == SQ_OK) {
                        sq_pushroottable(v);
                        if(sq_call(v, 1, SQFalse, SQTrue) != SQ_OK){
                            sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                        }
                        sq_poptop(v); //remove function from stack
                    }
                }
                sq_poptop(v); //remove root table
                return v;
            }
        }
        return 0;

        case MG_FREE_PLUGIN:
            v = (HSQUIRRELVM)mg_get_plugin(conn);
            if(v){
                SQ_Mg_Context *sq_mg_ctx = (SQ_Mg_Context *) ri;
                if(sq_mg_ctx){
                    SQ_MG_Callback *dump = &sq_mg_ctx->user_callback_exit;

                    if(dump->buf){
                        if (sq_compilebuffer(v, dump->buf, dump->len, dump->name, SQFalse) == SQ_OK) {
                            sq_pushroottable(v);
                            if(sq_call(v, 1, SQFalse, SQTrue) != SQ_OK){
                                sq_errorfunc(v, "sq_call failed %d\n%s", __LINE__, sq_getlasterror_str(v));
                            }
                            sq_poptop(v); //remove function from stack
                        }
                    }
                }
                sq_close(v);
            }
        return NULL;

        case MG_INIT_SSL:
#ifdef USE_AXTLS
            //lua_pushstring(L, "MG_INIT_SSL");
            *((void**)conn) = SSL_CTX_new(0);
            return 0;
#endif
        case MG_NEW_REQUEST:
        case MG_HTTP_ERROR:
        case MG_EVENT_LOG:{
            v = (HSQUIRRELVM) mg_get_plugin(conn);

            if (v) {
                SQInteger saved_top = sq_gettop(v);
                //set_mg_conn(v, conn);

                //sq_pushcfunction(v, traceback);  /* push traceback function */
                SQInteger error_func = sq_gettop(v);

                //sq_getglobal(v, "__manage_conn___");
                //if the SQIntegererpreter is in a mess state what to do ?
                if(sq_gettype(v,-1) != OT_CLOSURE) {
                    const char *error_msg = "Lua SQIntegererpreter lost manage_conn";
                    write_error_message(conn, error_msg, sizeof(error_msg));
                };

                switch (event) {
                case MG_NEW_REQUEST:    sq_pushliteral(v, "MG_NEW_REQUEST");    break;
                case MG_HTTP_ERROR:     sq_pushliteral(v, "MG_HTTP_ERROR");     break;
                case MG_EVENT_LOG:      sq_pushliteral(v, "MG_EVENT_LOG");      break;
                case MG_INIT_SSL:       sq_pushliteral(v, "MG_INIT_SSL");       break;
                default:                sq_pushnull(v);                         break;
                }
                push_request(v, ri);
        //        sq_call(v, 2, 1);
        //        e = sq_toboolean(v, -1) ? 1 : 0;
#if 0
                if(sq_call(v, 2, 1, error_func) != SQ_OK) {
                    size_t error_len;
                    const char *error_msg = sq_tolstring(v, -1, &error_len);
                    //printf("%s\n", error_msg);
                    write_error_message(conn, error_msg, error_len);
                    e = 0;
                } else {
                    e = sq_toboolean(v, -1) ? 1 : 0;
                }
#endif
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

    SQRESULT sqstd_register_mongoose(HSQUIRRELVM v)
    {
        sq_insert_reg_funcs(v, sq_mg_methods);
        sq_pushliteral(v,_SC("mongoose"));
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)SQ_MONGOOSE_TAG);
        sq_insert_reg_funcs(v, sq_mongoose_methods);
        sq_newslot(v,-3,SQTrue);
        return 1;
    }

#ifdef __cplusplus
}
#endif

