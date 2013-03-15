#include "squirrel.h"
#include "mysql.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"

#include "dynamic_library.h"

/*SquiLu
local mysql_functions = [
    ["MYSQL *", "mysql_init", "MYSQL *mysql"],
    ["MYSQL *", "mysql_real_connect", @"MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned long clientflag"],
    ["void", "mysql_close", "MYSQL *sock"],
    ["const char *", "mysql_error", "MYSQL *mysql"],
    ["unsigned long", "mysql_real_escape_string", @"MYSQL *mysql,
					       char *to,const char *from,
					       unsigned long length"],
    ["unsigned long", "mysql_get_server_version", "MYSQL *mysql"],
    ["unsigned long", "mysql_insert_id", "MYSQL *mysql"],
    ["void", "mysql_data_seek", "MYSQL_RES *result, my_ulonglong offset"],
    ["MYSQL_RES *", "mysql_store_result", "MYSQL *mysql"],
    ["int", "mysql_real_query", @"MYSQL *mysql, const char *q,
					unsigned long length"],
    ["my_ulonglong", "mysql_num_rows", "MYSQL_RES *res"],
    ["my_ulonglong", "mysql_affected_rows", "MYSQL *mysql"],
    ["unsigned int", "mysql_num_fields", "MYSQL_RES *res"],
    ["MYSQL_FIELD *", "mysql_fetch_fields", "MYSQL_RES *res"],
    ["MYSQL_FIELD *", "mysql_fetch_field_direct", "MYSQL_RES *res, unsigned int fieldnr"],
    ["MYSQL_ROW", "mysql_fetch_row", "MYSQL_RES *res"],
    ["unsigned long *", "mysql_fetch_lengths", "MYSQL_RES *res"],
    ["void", "mysql_free_result", "MYSQL_RES *result"],
    ["my_bool", "mysql_eof", "MYSQL_RES *res"],

    //prepared statements
    ["MYSQL_STMT *", "mysql_stmt_init", "MYSQL *mysql"],
    ["int", "mysql_stmt_prepare", @"MYSQL_STMT *stmt, const char *query,
                               unsigned long length"],
    ["int", "mysql_stmt_execute", "MYSQL_STMT *stmt"],
    ["unsigned long", "mysql_stmt_param_count", "MYSQL_STMT * stmt"],
    ["my_bool", "mysql_stmt_bind_param", "MYSQL_STMT * stmt, MYSQL_BIND * bnd"],
    ["my_bool", "mysql_stmt_reset", "MYSQL_STMT * stmt"],
    ["my_bool", "mysql_stmt_free_result", "MYSQL_STMT * stmt"],
    ["my_bool", "mysql_stmt_close", "MYSQL_STMT * stmt"],
    ["const char *", "mysql_stmt_error", "MYSQL_STMT * stmt"],
];

function write_mysql_functions_declaration(){
    foreach(k,v in mysql_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_mysql_functions_load(){
    foreach(k,v in mysql_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libmysqlclient.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary libmysqlclient;

//@write_mysql_functions_declaration();
// generated-code:begin
typedef MYSQL * (*mysql_init_t)(MYSQL *mysql);
static mysql_init_t dlmysql_init = 0;
typedef MYSQL * (*mysql_real_connect_t)(MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned long clientflag);
static mysql_real_connect_t dlmysql_real_connect = 0;
typedef void (*mysql_close_t)(MYSQL *sock);
static mysql_close_t dlmysql_close = 0;
typedef const char * (*mysql_error_t)(MYSQL *mysql);
static mysql_error_t dlmysql_error = 0;
typedef unsigned long (*mysql_real_escape_string_t)(MYSQL *mysql,
					       char *to,const char *from,
					       unsigned long length);
static mysql_real_escape_string_t dlmysql_real_escape_string = 0;
typedef unsigned long (*mysql_get_server_version_t)(MYSQL *mysql);
static mysql_get_server_version_t dlmysql_get_server_version = 0;
typedef unsigned long (*mysql_insert_id_t)(MYSQL *mysql);
static mysql_insert_id_t dlmysql_insert_id = 0;
typedef void (*mysql_data_seek_t)(MYSQL_RES *result, my_ulonglong offset);
static mysql_data_seek_t dlmysql_data_seek = 0;
typedef MYSQL_RES * (*mysql_store_result_t)(MYSQL *mysql);
static mysql_store_result_t dlmysql_store_result = 0;
typedef int (*mysql_real_query_t)(MYSQL *mysql, const char *q,
					unsigned long length);
static mysql_real_query_t dlmysql_real_query = 0;
typedef my_ulonglong (*mysql_num_rows_t)(MYSQL_RES *res);
static mysql_num_rows_t dlmysql_num_rows = 0;
typedef my_ulonglong (*mysql_affected_rows_t)(MYSQL *mysql);
static mysql_affected_rows_t dlmysql_affected_rows = 0;
typedef unsigned int (*mysql_num_fields_t)(MYSQL_RES *res);
static mysql_num_fields_t dlmysql_num_fields = 0;
typedef MYSQL_FIELD * (*mysql_fetch_fields_t)(MYSQL_RES *res);
static mysql_fetch_fields_t dlmysql_fetch_fields = 0;
typedef MYSQL_FIELD * (*mysql_fetch_field_direct_t)(MYSQL_RES *res, unsigned int fieldnr);
static mysql_fetch_field_direct_t dlmysql_fetch_field_direct = 0;
typedef MYSQL_ROW (*mysql_fetch_row_t)(MYSQL_RES *res);
static mysql_fetch_row_t dlmysql_fetch_row = 0;
typedef unsigned long * (*mysql_fetch_lengths_t)(MYSQL_RES *res);
static mysql_fetch_lengths_t dlmysql_fetch_lengths = 0;
typedef void (*mysql_free_result_t)(MYSQL_RES *result);
static mysql_free_result_t dlmysql_free_result = 0;
typedef my_bool (*mysql_eof_t)(MYSQL_RES *res);
static mysql_eof_t dlmysql_eof = 0;
typedef MYSQL_STMT * (*mysql_stmt_init_t)(MYSQL *mysql);
static mysql_stmt_init_t dlmysql_stmt_init = 0;
typedef int (*mysql_stmt_prepare_t)(MYSQL_STMT *stmt, const char *query,
                               unsigned long length);
static mysql_stmt_prepare_t dlmysql_stmt_prepare = 0;
typedef int (*mysql_stmt_execute_t)(MYSQL_STMT *stmt);
static mysql_stmt_execute_t dlmysql_stmt_execute = 0;
typedef unsigned long (*mysql_stmt_param_count_t)(MYSQL_STMT * stmt);
static mysql_stmt_param_count_t dlmysql_stmt_param_count = 0;
typedef my_bool (*mysql_stmt_bind_param_t)(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
static mysql_stmt_bind_param_t dlmysql_stmt_bind_param = 0;
typedef my_bool (*mysql_stmt_reset_t)(MYSQL_STMT * stmt);
static mysql_stmt_reset_t dlmysql_stmt_reset = 0;
typedef my_bool (*mysql_stmt_free_result_t)(MYSQL_STMT * stmt);
static mysql_stmt_free_result_t dlmysql_stmt_free_result = 0;
typedef my_bool (*mysql_stmt_close_t)(MYSQL_STMT * stmt);
static mysql_stmt_close_t dlmysql_stmt_close = 0;
typedef const char * (*mysql_stmt_error_t)(MYSQL_STMT * stmt);
static mysql_stmt_error_t dlmysql_stmt_error = 0;
// generated-code:end
#ifdef WIN32
#define LIBPQ_NAME "libmysqlclient.dll"
#else
#define LIBPQ_NAME "libmysqlclient.so"
#endif

static bool load_libmysqlclient()
{
    if(dlmysql_init) return true;
    if(libmysqlclient.open(LIBPQ_NAME))
    {
        //@write_mysql_functions_load();
// generated-code:begin
dlmysql_init = (mysql_init_t) libmysqlclient.dlsym("mysql_init");
if(!dlmysql_init) return false;
dlmysql_real_connect = (mysql_real_connect_t) libmysqlclient.dlsym("mysql_real_connect");
if(!dlmysql_real_connect) return false;
dlmysql_close = (mysql_close_t) libmysqlclient.dlsym("mysql_close");
if(!dlmysql_close) return false;
dlmysql_error = (mysql_error_t) libmysqlclient.dlsym("mysql_error");
if(!dlmysql_error) return false;
dlmysql_real_escape_string = (mysql_real_escape_string_t) libmysqlclient.dlsym("mysql_real_escape_string");
if(!dlmysql_real_escape_string) return false;
dlmysql_get_server_version = (mysql_get_server_version_t) libmysqlclient.dlsym("mysql_get_server_version");
if(!dlmysql_get_server_version) return false;
dlmysql_insert_id = (mysql_insert_id_t) libmysqlclient.dlsym("mysql_insert_id");
if(!dlmysql_insert_id) return false;
dlmysql_data_seek = (mysql_data_seek_t) libmysqlclient.dlsym("mysql_data_seek");
if(!dlmysql_data_seek) return false;
dlmysql_store_result = (mysql_store_result_t) libmysqlclient.dlsym("mysql_store_result");
if(!dlmysql_store_result) return false;
dlmysql_real_query = (mysql_real_query_t) libmysqlclient.dlsym("mysql_real_query");
if(!dlmysql_real_query) return false;
dlmysql_num_rows = (mysql_num_rows_t) libmysqlclient.dlsym("mysql_num_rows");
if(!dlmysql_num_rows) return false;
dlmysql_affected_rows = (mysql_affected_rows_t) libmysqlclient.dlsym("mysql_affected_rows");
if(!dlmysql_affected_rows) return false;
dlmysql_num_fields = (mysql_num_fields_t) libmysqlclient.dlsym("mysql_num_fields");
if(!dlmysql_num_fields) return false;
dlmysql_fetch_fields = (mysql_fetch_fields_t) libmysqlclient.dlsym("mysql_fetch_fields");
if(!dlmysql_fetch_fields) return false;
dlmysql_fetch_field_direct = (mysql_fetch_field_direct_t) libmysqlclient.dlsym("mysql_fetch_field_direct");
if(!dlmysql_fetch_field_direct) return false;
dlmysql_fetch_row = (mysql_fetch_row_t) libmysqlclient.dlsym("mysql_fetch_row");
if(!dlmysql_fetch_row) return false;
dlmysql_fetch_lengths = (mysql_fetch_lengths_t) libmysqlclient.dlsym("mysql_fetch_lengths");
if(!dlmysql_fetch_lengths) return false;
dlmysql_free_result = (mysql_free_result_t) libmysqlclient.dlsym("mysql_free_result");
if(!dlmysql_free_result) return false;
dlmysql_eof = (mysql_eof_t) libmysqlclient.dlsym("mysql_eof");
if(!dlmysql_eof) return false;
dlmysql_stmt_init = (mysql_stmt_init_t) libmysqlclient.dlsym("mysql_stmt_init");
if(!dlmysql_stmt_init) return false;
dlmysql_stmt_prepare = (mysql_stmt_prepare_t) libmysqlclient.dlsym("mysql_stmt_prepare");
if(!dlmysql_stmt_prepare) return false;
dlmysql_stmt_execute = (mysql_stmt_execute_t) libmysqlclient.dlsym("mysql_stmt_execute");
if(!dlmysql_stmt_execute) return false;
dlmysql_stmt_param_count = (mysql_stmt_param_count_t) libmysqlclient.dlsym("mysql_stmt_param_count");
if(!dlmysql_stmt_param_count) return false;
dlmysql_stmt_bind_param = (mysql_stmt_bind_param_t) libmysqlclient.dlsym("mysql_stmt_bind_param");
if(!dlmysql_stmt_bind_param) return false;
dlmysql_stmt_reset = (mysql_stmt_reset_t) libmysqlclient.dlsym("mysql_stmt_reset");
if(!dlmysql_stmt_reset) return false;
dlmysql_stmt_free_result = (mysql_stmt_free_result_t) libmysqlclient.dlsym("mysql_stmt_free_result");
if(!dlmysql_stmt_free_result) return false;
dlmysql_stmt_close = (mysql_stmt_close_t) libmysqlclient.dlsym("mysql_stmt_close");
if(!dlmysql_stmt_close) return false;
dlmysql_stmt_error = (mysql_stmt_error_t) libmysqlclient.dlsym("mysql_stmt_error");
if(!dlmysql_stmt_error) return false;
// generated-code:end

        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

static const SQChar *MySQL_TAG = _SC("MySQL");

static SQRESULT get_mysql_instance(HSQUIRRELVM v, SQInteger idx, MYSQL **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)MySQL_TAG)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("database is closed"));
	return _rc_;
}

#define GET_mysql_INSTANCE_AT(idx) \
	MYSQL *self=NULL; \
	if((_rc_ = get_mysql_instance(v,idx,&self)) < 0) return _rc_;

#define GET_mysql_INSTANCE() GET_mysql_INSTANCE_AT(1)

static const SQChar *MySQL_Result_TAG = _SC("MySQL_Result");
static const SQChar *_curr_row_key = _SC("_curr_row");

static SQRESULT get_mysql_result_instance(HSQUIRRELVM v, SQInteger idx, MYSQL_RES **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)MySQL_Result_TAG)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("MYSQL_RES is closed"));
	return _rc_;
}

#define GET_mysql_result_INSTANCE_AT(idx) \
	MYSQL_RES *self=NULL; \
	if((_rc_ = get_mysql_result_instance(v,idx,&self)) < 0) return _rc_;

#define GET_mysql_result_INSTANCE() GET_mysql_result_INSTANCE_AT(1)

static SQRESULT sq_mysql_result_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	MYSQL_RES *self = ((MYSQL_RES *)p);
	if (self) dlmysql_free_result(self);
	return 0;
}

static SQRESULT sq_mysql_result_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
	dlmysql_free_result(self);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "Pgresult is closed"
	return 0;
}

static SQRESULT sq_mysql_result_col_count(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
	sq_pushinteger(v, (SQInteger)dlmysql_num_fields(self));
	return 1;
}

static SQRESULT sq_mysql_result_row_count(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
	sq_pushinteger(v, (SQInteger)dlmysql_num_rows(self));
	return 1;
}

static SQRESULT sq_mysql_result_col_name(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
	SQ_GET_INTEGER(v, 2, col);
	if(col < 0 || col > dlmysql_num_fields(self)-1)
		return sq_throwerror(v, _SC("invalid col number (%d)"), col);
	MYSQL_FIELD *fields = dlmysql_fetch_fields(self);
	sq_pushstring(v, fields[col].name, -1);
	return 1;
}

static int get_field_idx(MYSQL_RES *self, const SQChar *name){
	MYSQL_FIELD *fields = dlmysql_fetch_fields(self);
	unsigned int count = dlmysql_num_fields(self);
	for(int i=0; i<count; ++i){
		if(scstrcmp(name, fields[i].name) == 0){
			return i;
		}
	}
	return -1;
}

static SQRESULT sq_mysql_result_col_index(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
	SQ_GET_STRING(v, 2, name);
	sq_pushinteger(v, get_field_idx(self, name));
	return 1;
}

static SQRESULT sq_mysql_result_eof(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
    sq_pushstring(v, _curr_row_key, -1);
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        sq_pushbool(v, curr_row < dlmysql_num_rows(self));
    }
	else sq_pushbool(v, SQTrue);
	return 1;
}

static SQRESULT sq_mysql_result_next_row(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
    sq_pushstring(v, _curr_row_key, -1);
    sq_push(v, -1); //make a copy
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        if(++curr_row < dlmysql_num_rows(self)){
            sq_poptop(v);
            sq_pushinteger(v, curr_row);
            sq_set(v, 1);
            sq_pushbool(v, SQTrue);
            return 1;
        }
    }
	sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_mysql_result_col_value(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_result_INSTANCE();
	SQObjectType ptype = sq_gettype(v, 2);
	int col = -1;
	if(ptype == OT_STRING){
	    SQ_GET_STRING(v, 2, col_name);
	    col = get_field_idx(self, col_name);
	}
	else
	{
	    SQ_GET_INTEGER(v, 2, idx);
	    col = idx;
	}
	if(col < 0) return sq_throwerror(v, _SC("invalid col index/name"));

    sq_pushstring(v, _curr_row_key, -1);
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        if(curr_row < dlmysql_num_rows(self)){
        	dlmysql_data_seek(self, curr_row);
        	const MYSQL_ROW res_row = dlmysql_fetch_row(self);
			unsigned long *lengths = dlmysql_fetch_lengths(self);
            sq_pushstring(v, (const SQChar*)res_row[col], lengths[col]);
            return 1;
        }
    }
	return SQ_ERROR;
}

static SQRESULT sq_mysql_result_row_as_array(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_mysql_result_INSTANCE();
	SQ_OPT_INTEGER(v, 2, row, -1);
    if(row < 0){
        sq_pushstring(v, _curr_row_key, -1);
        if(sq_get(v, 1) == SQ_OK){
            sq_getinteger(v, -1, &row);
        }
    }
    int row_count = dlmysql_num_rows(self);
    if(row < 0 || row >= row_count) return sq_throwerror(v, _SC("invalid row (%d)"), row);

    int col_count = dlmysql_num_fields(self);
    sq_newarray(v, col_count);
    dlmysql_data_seek(self, row);
    const MYSQL_ROW res_row = dlmysql_fetch_row(self);
    unsigned long *lengths = dlmysql_fetch_lengths(self);
    for(int i=0; i < col_count; ++i){
        sq_pushinteger(v, i);
        sq_pushstring(v, (const SQChar*)res_row[i], lengths[i]);
        sq_rawset(v, -3);
    }
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_mysql_result_##name,nparams,tycheck}
static SQRegFunction sq_mysql_result_methods[] =
{
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(eof,  1, _SC("x")),
	_DECL_FUNC(next_row,  1, _SC("x")),
	_DECL_FUNC(col_count,  1, _SC("x")),
	_DECL_FUNC(row_count,  1, _SC("x")),
	_DECL_FUNC(col_name,  2, _SC("xi")),
	_DECL_FUNC(col_index,  2, _SC("xs")),
	_DECL_FUNC(col_value,  2, _SC("x i|s")),
	_DECL_FUNC(row_as_array,  -1, _SC("xi")),
	{0,0}
};
#undef _DECL_FUNC

static const SQChar *MySQL_Statement_TAG = _SC("MySQL_Statement");

static SQRESULT get_mysql_statement_instance(HSQUIRRELVM v, SQInteger idx, MYSQL_STMT **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)MySQL_Statement_TAG)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("MySql statement is closed"));
	return _rc_;
}

#define GET_mysql_statement_INSTANCE_AT(idx) \
	MYSQL_STMT *self=NULL; \
	if((_rc_ = get_mysql_statement_instance(v,idx,&self)) < 0) return _rc_;

#define GET_mysql_statement_INSTANCE() GET_mysql_statement_INSTANCE_AT(1)

static SQRESULT sq_mysql_statement_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	MYSQL_STMT *self = ((MYSQL_STMT *)p);
	if (self) dlmysql_stmt_close(self);
	return 0;
}

static SQRESULT sq_mysql_statement_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_statement_INSTANCE();
	sq_mysql_statement_releasehook(self, 0, v);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "Pgstatement is closed"
	return 0;
}


#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_mysql_statement_##name,nparams,tycheck}
static SQRegFunction sq_mysql_statement_methods[] =
{
	_DECL_FUNC(close,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_mysql_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	MYSQL *self = ((MYSQL *)p);
	if (self) dlmysql_close(self);
	return 0;
}

static SQRESULT sq_mysql_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, host);
    SQ_GET_STRING(v, 3, username);
    SQ_GET_STRING(v, 4, password);
    SQ_GET_STRING(v, 5, sourcename);
    SQ_OPT_INTEGER(v, 6, port, 3306);
    MYSQL *self=0;

    if(load_libmysqlclient())
    {
        self = dlmysql_init(NULL);
		if (!self) return sq_throwerror(v, _SC("error connecting: Out of memory."));
		if (!dlmysql_real_connect(self, host, username, password, sourcename, port, NULL, 0))
		{
			SQRESULT res = sq_throwerror(v, _SC("error connecting to database. MySQL: %s"), dlmysql_error(self));
			dlmysql_close (self); /* Close conn if connect failed */
			return res;
		}
    }
    else return sq_throwerror(v, _SC("Failed to load libmysqlclient !"));

    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, sq_mysql_releasehook);

    //save a weakref to allow statement return it's db
    sq_pushuserpointer(v, self);
    sq_weakref(v, 1);
    sq_setonregistrytable(v);

    return 1;
}

static SQRESULT sq_mysql_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
	dlmysql_close(self);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "database is closed"
	return 0;
}

static SQRESULT sq_mysql_exec_dml(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);

	if (dlmysql_real_query(self, szSQL, szSQL_size))
	/* error executing query */
		return sq_throwerror(v, _SC("error executing query. MySQL: %s"), dlmysql_error(self));

    sq_pushinteger(v, (SQInteger)dlmysql_affected_rows(self));
	return 1;
}

static SQRESULT sq_mysql_exec_scalar(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);
	int result = 0;

	if (dlmysql_real_query(self, szSQL, szSQL_size))
		/* error executing query */
		return sq_throwerror(v, _SC("error executing query. MySQL: %s"), dlmysql_error(self));

	MYSQL_RES *qres = dlmysql_store_result(self);

    bool is_ok = (dlmysql_num_rows(qres) == 1) && (dlmysql_num_fields(qres) > 0);

    if (is_ok) {
    	MYSQL_ROW row = dlmysql_fetch_row(qres);
    	unsigned long *lengths = dlmysql_fetch_lengths(qres);
    	result = lengths[0] && row[0] ? atoi(row[0]) : 0;
    }
    dlmysql_free_result(qres);

    if (!is_ok) return sq_throwerror(v, _SC("invalid scalar query (%s)"), szSQL);

    sq_pushinteger(v, result);
	return 1;
}

static SQRESULT sq_mysql_exec_query(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);

	if (dlmysql_real_query(self, szSQL, szSQL_size))
		/* error executing query */
		return sq_throwerror(v, _SC("error executing query. MySQL: %s"), dlmysql_error(self));

    MYSQL_RES *qres = dlmysql_store_result(self);

	sq_pushroottable(v);
	sq_pushstring(v, MySQL_Result_TAG, -1);
	if(sq_get(v, -2) == SQ_OK){
		if(sq_createinstance(v, -1) == SQ_OK){
			sq_setinstanceup(v, -1, qres);
			sq_setreleasehook(v, -1, sq_mysql_result_releasehook);
			sq_pushstring(v, _curr_row_key, -1);
			sq_pushinteger(v, -1);
			sq_set(v, -3);
			return 1;
		}
	}
    return SQ_ERROR;
}

static SQRESULT sq_mysql_prepare(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);

    MYSQL_STMT *stmt = dlmysql_stmt_init(self);
    if (!stmt) return sq_throwerror(v, _SC(" mysql_stmt_init(), out of memory\n"));

    if (!dlmysql_stmt_prepare(stmt, szSQL, szSQL_size)) {
    	SQRESULT res = sq_throwerror(v, dlmysql_stmt_error(stmt));
    	dlmysql_stmt_close(stmt);
    	return res;
    }

	sq_pushroottable(v);
	sq_pushstring(v, MySQL_Statement_TAG, -1);
	if(sq_get(v, -2) == SQ_OK){
		if(sq_createinstance(v, -1) == SQ_OK){
			sq_setinstanceup(v, -1, stmt);
			sq_setreleasehook(v, -1, sq_mysql_statement_releasehook);
			return 1;
		}
    }
    return SQ_ERROR;
}

static SQRESULT sq_mysql_error_message(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
	sq_pushstring(v, dlmysql_error(self), -1);
	return 1;
}

static SQRESULT sq_mysql_version(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
	sq_pushinteger(v, (SQInteger)dlmysql_get_server_version(self));
	return 1;
}

static SQRESULT sq_mysql_last_insert_id(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
	sq_pushinteger(v, (SQInteger)dlmysql_insert_id(self));
	return 1;
}

static SQRESULT sq_mysql_escape_string(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_mysql_INSTANCE();
	SQ_GET_STRING(v, 2, str);
	SQInteger to_size = (str_size+1) * sizeof(SQChar);
	SQChar *to = sq_getscratchpad(v, to_size);
	if(to) {
		SQInteger new_size = (SQInteger)dlmysql_real_escape_string(self, to, str, str_size);
		sq_pushstring(v, to, new_size);
		return 1;
	}
	return sq_throwerror(v, _SC("could not allocate escaped string"));
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_mysql_##name,nparams,tycheck}
static SQRegFunction sq_mysql_methods[] =
{
	_DECL_FUNC(constructor,  -5, _SC("xssssi")),
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(exec_dml,  2, _SC("xs")),
	_DECL_FUNC(exec_scalar,  2, _SC("xs")),
	_DECL_FUNC(exec_query,  2, _SC("xs")),
	_DECL_FUNC(prepare,  2, _SC("xs")),
	_DECL_FUNC(error_message,  1, _SC("x")),
	_DECL_FUNC(version,  1, _SC("x")),
	_DECL_FUNC(last_insert_id,  1, _SC("x")),
	_DECL_FUNC(escape_string,  1, _SC("xs")),
	{0,0}
};
#undef _DECL_FUNC


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_MySQL(HSQUIRRELVM v)
{
    sq_pushstring(v,MySQL_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)MySQL_TAG);
    sq_insert_reg_funcs(v, sq_mysql_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,MySQL_Statement_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)MySQL_Statement_TAG);
    sq_insert_reg_funcs(v, sq_mysql_statement_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,MySQL_Result_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)MySQL_Result_TAG);
    sq_insert_reg_funcs(v, sq_mysql_result_methods);
    sq_pushstring(v, _curr_row_key, -1);
    sq_pushnull(v);
    sq_newslot(v, -3, SQFalse);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

