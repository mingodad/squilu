#ifdef SQ_USE_LMDB

#ifdef __cplusplus
extern "C" {
#endif


#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

SQ_OPT_STRING_STRLEN();

#include "lmdb.h"

static const SQChar SQ_LMDB_ENV[] = _SC("LmDB_Env");

static const SQChar LMDB_ENV_Tag[]   = _SC("sq_LmDB_Env_ctx");
#define GET_LmDB_Env_INSTANCE() SQ_GET_INSTANCE(v, 1, MDB_env, LMDB_ENV_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("LmDB_Env object already closed"));


static SQRESULT LmDB_Env_release_hook(SQUserPointer p, SQInteger size, void*)
{
	MDB_env *self = (MDB_env*)p;
	if(self) mdb_env_close(self);
	return 0;
}

static SQRESULT LmDB_Env_constructor(HSQUIRRELVM v, MDB_env *LmDB_Env, int free_on_gc)
{
    if(!LmDB_Env)
        return sq_throwerror(v, _SC("Could'nt create an LmDB_Env object."));

    sq_pushstring(v, SQ_LMDB_ENV, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, LMDB_ENV_Tag, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, LmDB_Env);
                if(free_on_gc) sq_setreleasehook(v,-1, LmDB_Env_release_hook);
                return 1;
            }
        }
    }
	return SQ_ERROR;
}


static SQRESULT sq_LmDB_Env_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();
	LmDB_Env_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0);
	return 0;
}

static SQRESULT sq_LmDB_Env_copy(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	SQ_GET_STRING(v, 2, path);
//	int  mdb_env_copy(MDB_env *env, const char *path)
    sq_pushinteger(v, mdb_env_copy(self, path));
	return 1;
}

static SQRESULT sq_LmDB_Env_dbi_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

//	void mdb_dbi_close(MDB_env *env, MDB_dbi dbi)
    //mdb_dbi_close(self, MDB_dbi dbi);
	return 0;
}

static SQRESULT sq_LmDB_Env_get_flags(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	unsigned int int_flags;
//	int  mdb_env_get_flags(MDB_env *env, unsigned int *flags)
	mdb_env_get_flags(self, &int_flags);
	sq_pushinteger(v, (SQInteger)int_flags);
	return 1;
}

static SQRESULT sq_LmDB_Env_get_path(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

//	int  mdb_env_get_path(MDB_env *env, const char **path)
	const char *path;
	int err = mdb_env_get_path(self, &path);
	if(err) sq_pushinteger(v, err);
	else sq_pushstring(v, path, -1);
	return 1;
}

static SQRESULT sq_LmDB_Env_info(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

//	int  mdb_env_info(MDB_env *env, MDB_envinfo *stat)
	MDB_envinfo info;
	mdb_env_info(self,&info);
	sq_newtable(v);

	sq_pushliteral(v, _SC("me_mapsize"));
	sq_pushinteger(v,info.me_mapsize);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("ms_last_pgno"));
	sq_pushinteger(v,info.me_last_pgno);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("me_last_txnid"));
	sq_pushinteger(v,info.me_last_txnid);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("me_maxreaders"));
	sq_pushinteger(v,info.me_maxreaders);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("me_numreaders"));
	sq_pushinteger(v,info.me_numreaders);
	sq_rawset(v, -3);

	return 1;
}

static SQRESULT sq_LmDB_Env_open(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	SQ_GET_STRING(v, 2, path);
	SQ_GET_INTEGER(v, 3, int_flags);
	SQ_GET_INTEGER(v, 4, int_mode);
//	int  mdb_env_open(MDB_env *env, const char *path, unsigned int flags, mdb_mode_t mode)
	sq_pushinteger(v, mdb_env_open(self, path, int_flags, int_mode));
	return 1;
}

static SQRESULT sq_LmDB_Env_set_flags(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_flags);
	SQ_GET_INTEGER(v, 3, int_onOff);
//	int  mdb_env_set_flags(MDB_env *env, unsigned int flags, int onoff)
	sq_pushinteger(v, mdb_env_set_flags(self, int_flags, int_onOff));
	return 1;
}

static SQRESULT sq_LmDB_Env_set_mapsize(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_size);
//	int  mdb_env_set_mapsize(MDB_env *env, mdb_size_t size)
	sq_pushinteger(v, mdb_env_set_mapsize(self, int_size));
	return 1;
}

static SQRESULT sq_LmDB_Env_set_maxdbs(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_dbs);
//	int  mdb_env_set_maxdbs(MDB_env *env, MDB_dbi dbs)
	sq_pushinteger(v, mdb_env_set_maxdbs(self, int_dbs));
	return 1;
}

static SQRESULT sq_LmDB_Env_set_maxreaders(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_readers);
//	int  mdb_env_set_maxreaders(MDB_env *env, unsigned int readers)
	sq_pushinteger(v, mdb_env_set_maxreaders(self, int_readers));
	return 1;
}

static SQRESULT stat_to_table(HSQUIRRELVM v, MDB_stat *stat){
	sq_newtable(v);
	sq_pushliteral(v, _SC("ms_psize"));
	sq_pushinteger(v,stat->ms_psize);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("ms_depth"));
	sq_pushinteger(v,stat->ms_depth);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("ms_branch_pages"));
	sq_pushinteger(v,stat->ms_branch_pages);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("ms_leaf_pages"));
	sq_pushinteger(v,stat->ms_leaf_pages);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("ms_overflow_pages"));
	sq_pushinteger(v,stat->ms_overflow_pages);
	sq_rawset(v, -3);
	sq_pushliteral(v, _SC("ms_entries"));
	sq_pushinteger(v,stat->ms_entries);
	sq_rawset(v, -3);
	return 1;
}


static SQRESULT sq_LmDB_Env_stat(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

//	int  mdb_env_stat(MDB_env *env, MDB_stat *stat)
	MDB_stat stat;
	mdb_env_stat(self, &stat);
	return stat_to_table(v, &stat);
}

static SQRESULT sq_LmDB_Env_sync(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_force);
//	int  mdb_env_sync(MDB_env *env, int force)
	sq_pushinteger(v, mdb_env_sync(self, int_force));
	return 1;
}

static SQRESULT sq_LmDB_Env_txn_begin(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Env_INSTANCE();

//	int  mdb_txn_begin(MDB_env *env, MDB_txn *parent, unsigned int flags, MDB_txn **txn)

	return 0;
}

static SQRESULT sq_LmDB_Env_strerror(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);

	SQ_GET_INTEGER(v, 2, int_error);
//	char *mdb_strerror(int err)
	sq_pushstring(v, mdb_strerror(int_error), -1);
	return 1;
}

static SQRESULT sq_LmDB_Env_version(HSQUIRRELVM v){

//	char *mdb_version(int *major, int *minor, int *patch)
	int major, minor, patch;
	sq_pushstring(v, mdb_version(&major, &minor, &patch), -1);
	return 1;
}


#define _DECL_LMDB_ENV_FUNC(name,nparams,pmask) {_SC(#name),sq_LmDB_Env_##name,nparams,pmask}
static SQRegFunction LmDB_Env_obj_funcs[]={

	_DECL_LMDB_ENV_FUNC(close, 1, _SC("x")),
	_DECL_LMDB_ENV_FUNC(copy, 2, _SC("x.")),
	_DECL_LMDB_ENV_FUNC(dbi_close, 1, _SC("x")),
	_DECL_LMDB_ENV_FUNC(get_flags, 2, _SC("xi")),
	_DECL_LMDB_ENV_FUNC(get_path, 1, _SC("x")),
	_DECL_LMDB_ENV_FUNC(info, 1, _SC("x")),
	_DECL_LMDB_ENV_FUNC(open, 4, _SC("xsii")),
	_DECL_LMDB_ENV_FUNC(set_flags, 3, _SC("xii")),
	_DECL_LMDB_ENV_FUNC(set_mapsize, 2, _SC("xi")),
	_DECL_LMDB_ENV_FUNC(set_maxdbs, 2, _SC("xi")),
	_DECL_LMDB_ENV_FUNC(set_maxreaders, 2, _SC("xi")),
	_DECL_LMDB_ENV_FUNC(stat, 1, _SC("x")),
	_DECL_LMDB_ENV_FUNC(sync, 2, _SC("xi")),
	_DECL_LMDB_ENV_FUNC(txn_begin, 1, _SC("x")),
	_DECL_LMDB_ENV_FUNC(strerror, 2, _SC(".i")),
	_DECL_LMDB_ENV_FUNC(version, 1, _SC(".")),
	{0,0}
};
#undef _DECL_LMDB_ENV_FUNC

static const SQChar SQ_LMDB_TRANSACTION[] = _SC("LmDB_Transaction");

static const SQChar LMDB_TRANSACTION_Tag[]   = _SC("sq_LmDB_Transaction_ctx");
#define GET_LmDB_Transaction_INSTANCE() SQ_GET_INSTANCE(v, 1, SQ_LMDB_TRANSACTION, LMDB_TRANSACTION_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("LmDB_Transaction object already closed"));


static SQRESULT LmDB_Transaction_release_hook(SQUserPointer p, SQInteger size, void*)
{
	LMDB_TRANSACTION *self = (LMDB_TRANSACTION*)p;
	if(self) LmDB_Transaction_free(self);
	return 0;
}

static SQRESULT LmDB_Transaction_free(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();
	LmDB_Transaction_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0);
	return 0;
}

static SQRESULT LmDB_Transaction_constructor(HSQUIRRELVM v, LMDB_TRANSACTION *LmDB_Transaction, int free_on_gc)
{
    if(!LmDB_Transaction)
        return sq_throwerror(v, _SC("Could'nt create an LmDB_Transaction object."));

    sq_pushstring(v, SQ_LMDB_TRANSACTION, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, LMDB_TRANSACTION_Tag, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, LmDB_Transaction);
                if(free_on_gc) sq_setreleasehook(v,-1, LmDB_Transaction_release_hook);
                return 1;
            }
        }
    }
	return SQ_ERROR;
}


static SQRESULT sq_LmDB_Transaction_abort(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

//	void mdb_txn_abort(MDB_txn *txn)
	mdb_txn_abort(self);
	return 0;
}

static MDB_val* pop_val(HSQUIRRELVM v,int index, MDB_val* val) {
	if ( sq_gettype(v,index) == OT_NULL ) {
		return NULL;
	}
	SQInteger sz;
	sq_getstr_and_size(v, index, (const SQChar**)&val->mv_data, &sz);
	val->mv_size = sz;
	return val;
}

static SQRESULT txn_cmp_helper(HSQUIRRELVM v,int mode) {
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();
	SQ_GET_INTEGER(v, 2, dbi);
	MDB_val key, value;
	pop_val(v, 3, &key);
	pop_val(v, 4, &value);

	if ( mode==MDB_DUPSORT )
		return sq_pushinteger(v, mdb_dcmp(self,dbi, &key, &value));
	return sq_pushinteger(v, mdb_cmp(self,dbi, &key, &value));
}


static SQRESULT sq_LmDB_Transaction_cmp(HSQUIRRELVM v){
//	int  mdb_cmp(MDB_txn *txn, MDB_dbi dbi, const MDB_val *a, const MDB_val *b)
	return txn_cmp_helper(v, 0);
}

static SQRESULT sq_LmDB_Transaction_cursor_open(HSQUIRRELVM v){
//	int  mdb_cursor_open(MDB_txn *txn, MDB_dbi dbi, MDB_cursor **cursor)
	return txn_cmp_helper(v, 1);
}

static SQRESULT sq_LmDB_Transaction_cursor_renew(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

//	int  mdb_cursor_renew(MDB_txn *txn, MDB_cursor *cursor)
    //sq_pushinteger(v, mdb_cursor_renew(self, ));
	return 1;
}

static SQRESULT sq_LmDB_Transaction_dbi_drop(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

	SQ_GET_INTEGER(v, 2, dbi);
	SQ_GET_INTEGER(v, 3, del);
//	int  mdb_drop(MDB_txn *txn, MDB_dbi dbi, int del)
	sq_pushinteger(v, mdb_drop(self, dbi, del));
	return 1;
}

static SQRESULT sq_LmDB_Transaction_dbi_open(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

	SQ_GET_STRING(v, 2, name);
	SQ_GET_INTEGER(v, 3, int_flags);
//	int  mdb_dbi_open(MDB_txn *txn, const char *name, unsigned int flags, MDB_dbi *dbi)

	return 0;
}

static SQRESULT sq_LmDB_Transaction_dcmp(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

//	int  mdb_dcmp(MDB_txn *txn, MDB_dbi dbi, const MDB_val *a, const MDB_val *b)

	return 0;
}

static SQRESULT sq_LmDB_Transaction_del(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

	SQ_GET_STRING(v, 2, key);
//	int  mdb_del(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data)
	sq_pushinteger(v, mdb_del(self, dbi, del));
	return 1;
}

static SQRESULT sq_LmDB_Transaction_get(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

	SQ_GET_STRING(v, 2, key);
//	int  mdb_get(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data)

	return 0;
}

static SQRESULT sq_LmDB_Transaction_int_commit(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

//	int  mdb_txn_commit(MDB_txn *txn)
	sq_pushinteger(v, mdb_txn_commit(self));
	return 1;
}

static SQRESULT sq_LmDB_Transaction_put(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

	MDB_val key, value;
	pop_val(v, 2, &key);
	pop_val(v, 3, &value);
	SQ_GET_INTEGER(v, 4, int_flags);

//	int  mdb_put(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data, unsigned int flags)
	sq_pushinteger(v, mdb_put(self, &key, &value, int_flags));
	return 1;
	SQ_GET_STRING(v, 2, key);
	SQ_GET_STRING(v, 3, value);
	SQ_GET_INTEGER(v, 4, int_flags);

	return 0;
}

static SQRESULT sq_LmDB_Transaction_renew(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

//	int  mdb_txn_renew(MDB_txn *txn)
	sq_pushinteger(v, mdb_txn_renew(self));
	return 1;
}

static SQRESULT sq_LmDB_Transaction_reset(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

//	void mdb_txn_reset(MDB_txn *txn)
	sq_pushinteger(v, mdb_txn_reset(self));
	return 1;
}

static SQRESULT sq_LmDB_Transaction_stat(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Transaction_INSTANCE();

//	int  mdb_stat(MDB_txn *txn, MDB_dbi dbi, MDB_stat *stat)

	return 0;
}

#define _DECL_LMDB_TRANSACTION_FUNC(name,nparams,pmask) {_SC(#name),sq_LmDB_Transaction_##name,nparams,pmask}
static SQRegFunction LmDB_Transaction_obj_funcs[]={

	_DECL_LMDB_TRANSACTION_FUNC(abort, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(cmp, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(cursor_open, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(cursor_renew, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(dbi_drop, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(dbi_open, 3, _SC("x.i")),
	_DECL_LMDB_TRANSACTION_FUNC(dcmp, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(del, 2, _SC("x.")),
	_DECL_LMDB_TRANSACTION_FUNC(get, 2, _SC("x.")),
	_DECL_LMDB_TRANSACTION_FUNC(int_commit, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(put, 4, _SC("x..i")),
	_DECL_LMDB_TRANSACTION_FUNC(renew, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(reset, 1, _SC("x")),
	_DECL_LMDB_TRANSACTION_FUNC(stat, 1, _SC("x")),
	{0,0}
};
#undef _DECL_LMDB_TRANSACTION_FUNC

static const SQChar LMDB_CURSOR[] = _SC("LmDB_Cursor");

static const SQChar LMDB_CURSOR_Tag[]   = _SC("sq_LmDB_Cursor_ctx");
#define GET_LmDB_Cursor_INSTANCE() SQ_GET_INSTANCE(v, 1, LMDB_CURSOR, LMDB_CURSOR_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("LmDB_Cursor object already closed"));


static SQRESULT LmDB_Cursor_release_hook(SQUserPointer p, SQInteger size, void*)
{
	MDB_cursor *self = (MDB_cursor*)p;
	if(self) mdb_cursor_close(self);
	return 0;
}

static SQRESULT LmDB_Cursor_constructor(HSQUIRRELVM v, MDB_cursor *lmdb_Cursor, int free_on_gc)
{
	if(!lmdb_Cursor)
		return sq_throwerror(v, _SC("Could'nt create an LmDB_Cursor object."));

	sq_pushstring(v, LMDB_CURSOR, -1);
	if(sq_getonroottable(v) == SQ_OK){
		sq_pushstring(v, LMDB_CURSOR_Tag, -1);
		if(sq_get(v, -2) == SQ_OK){
			if(sq_createinstance(v, -1) == SQ_OK){
				sq_setinstanceup(v, -1, lmdb_Cursor);
				if(free_on_gc) sq_setreleasehook(v,-1, LmDB_Cursor_release_hook);
				return 1;
			}
		}
	}
	return SQ_ERROR;
}

static SQRESULT sq_LmDB_Cursor_constructor(HSQUIRRELVM v){
	// if(!load_library()) return sq_throwerror(v, _SC("Failed to load libcurl !"));
	//int  mdb_cursor_open(MDB_txn *txn, MDB_dbi dbi, MDB_cursor **cursor);
	MDB_cursor *lmdb_Cursor = NULL;
	int rc = mdb_cursor_open(txn, dbi, &lmdb_Cursor);
	return LmDB_Cursor_constructor(v, 1, lmdb_Cursor, 1);
}

static SQRESULT sq_LmDB_Cursor_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_ssl_INSTANCE();
	LmDB_Cursor_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0);
	return 0;
}

static SQRESULT sq_LmDB_Cursor_count(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Cursor_INSTANCE();

	mdb_size_t count;
	mdb_cursor_count(self, &count);
	sq_pushinteger(v, (SQInteger)count);
	return 1;
}

static SQRESULT sq_LmDB_Cursor_del(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Cursor_INSTANCE();

	MDB_val key, value;
	pop_val(v, 2, &key);
	pop_val(v, 3, &value);
	SQ_GET_INTEGER(v, 4, int_flags);

	sq_pushinteger(v, mdb_cursor_del(self, &key, &value, int_flags));
	return 1;
}

static SQRESULT sq_LmDB_Cursor_get(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Cursor_INSTANCE();

	MDB_val key, value;
	pop_val(v, 2, &key);
	pop_val(v, 3, &value);
	SQ_GET_INTEGER(v, 4, op);

	sq_pushinteger(v, mdb_cursor_get(self, &key, &value, op));
	return 1;
}

static SQRESULT sq_LmDB_Cursor_put(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_LmDB_Cursor_INSTANCE();

	MDB_val key, value;
	pop_val(v, 2, &key);
	pop_val(v, 3, &value);
	SQ_GET_INTEGER(v, 4, int_flags);

	sq_pushinteger(v, mdb_cursor_put(self, &key, &value, int_flags));
	return 1;
}

#define _DECL_LMDB_CURSOR_FUNC(name,nparams,pmask) {_SC(#name),sq_LmDB_Cursor_##name,nparams,pmask}
static SQRegFunction LmDB_Cursor_obj_funcs[]={

	_DECL_LMDB_CURSOR_FUNC(close, 1, _SC(".")),
	_DECL_LMDB_CURSOR_FUNC(count, 1, _SC(".")),
	_DECL_LMDB_CURSOR_FUNC(del, 2, _SC(".i")),
	_DECL_LMDB_CURSOR_FUNC(get, 2, _SC("..")),
	_DECL_LMDB_CURSOR_FUNC(put, 4, _SC("...i")),
	{0,0}
};
#undef _DECL_LMDB_CURSOR_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType LmDB_constants[] = {
    #define MK_CONST(c) {_SC(#c), MDB_##c}

	MK_CONST(TLS_FULL);
	MK_CONST(NEXT);
	MK_CONST(WRITEMAP);
	MK_CONST(PAGE_FULL);
	MK_CONST(MULTIPLE);
	MK_CONST(PREV_DUP);
	MK_CONST(DBS_FULL);
	MK_CONST(NOSYNC);
	MK_CONST(SUCCESS);
	MK_CONST(SET);
	MK_CONST(SET_RANGE);
	MK_CONST(NOSUBDIR);
	MK_CONST(SET_KEY);
	MK_CONST(INCOMPATIBLE);
	MK_CONST(FIXEDMAP);
	MK_CONST(PREV_NODUP);
	MK_CONST(NEXT_MULTIPLE);
	MK_CONST(FIRST_DUP);
	MK_CONST(APPEND);
	MK_CONST(GET_MULTIPLE);
	MK_CONST(READERS_FULL);
	MK_CONST(CURSOR_FULL);
	MK_CONST(INTEGERKEY);
	MK_CONST(PANIC);
	MK_CONST(NEXT_DUP);
	MK_CONST(REVERSEKEY);
	MK_CONST(MAP_RESIZED);
	MK_CONST(LAST_DUP);
	MK_CONST(NEXT_NODUP);
	MK_CONST(NOMETASYNC);
	MK_CONST(GET_CURRENT);
	MK_CONST(GET_BOTH_RANGE);
	MK_CONST(MAP_FULL);
	MK_CONST(GET_BOTH);
	MK_CONST(PREV);
	MK_CONST(NOTLS);
	MK_CONST(FIRST);
	MK_CONST(CORRUPTED);
	MK_CONST(MAPASYNC);
	MK_CONST(DUPFIXED);
	MK_CONST(NORDAHEAD);
	MK_CONST(APPENDDUP);
	MK_CONST(NOOVERWRITE);
	MK_CONST(INVALID);
	MK_CONST(NOLOCK);
	MK_CONST(KEYEXIST);
	MK_CONST(NOTFOUND);
	MK_CONST(REVERSEDUP);
	MK_CONST(CREATE);
	MK_CONST(RESERVE);
	MK_CONST(TXN_FULL);
	MK_CONST(LAST);
	MK_CONST(PAGE_NOTFOUND);
	MK_CONST(DUPSORT);
	MK_CONST(RDONLY);
	MK_CONST(VERSION_MISMATCH);
	MK_CONST(CURRENT);
	MK_CONST(NOMEMINIT);
	MK_CONST(NODUPDATA);
	MK_CONST(INTEGERDUP);
    {0,0}
};

/* This defines a function that opens up your library. */
SQRESULT sqext_register_LmDB (HSQUIRRELVM v) {
	//add a namespace LmDB
	sq_pushstring(v, SQ_LMDB_ENV, -1);
	sq_newtable(v);

	sq_insert_reg_funcs(v, LmDB_obj_funcs);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = LmDB_constants; KeyIntPtr->Str; KeyIntPtr++) {
		sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
		sq_pushinteger(v, KeyIntPtr->Val);       //then the value
		sq_newslot(v, -3, SQFalse);              //store then
	}

	sq_newslot(v,-3,SQFalse); //add LmDB table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}
#endif

#endif //SQ_USE_LMDB
