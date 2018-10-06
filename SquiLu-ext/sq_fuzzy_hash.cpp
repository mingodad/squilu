#ifdef SQ_USE_FUZZY_HASH

#include "squirrel.h"
#include "sqstdblobimpl.h"

#define MixInteger SQInteger

#include "fuzzy.h"

static SQRESULT sq_fuzzy_hash_buf0(HSQUIRRELVM v, SQBool isSmall) {
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, buf);

    SQChar *result = sq_getscratchpad(v, FUZZY_MAX_RESULT);
    int rc = fuzzy_hash_buf((const unsigned char*)buf, buf_size, result, isSmall);
    if(rc) sq_pushnull(v);
    else sq_pushstring(v, result, -1);
    return 1;
}

static SQRESULT sq_fuzzy_hash_buf(HSQUIRRELVM v) {
    return sq_fuzzy_hash_buf0(v, SQFalse);
}

static SQRESULT sq_fuzzy_hash_buf_small(HSQUIRRELVM v) {
    return sq_fuzzy_hash_buf0(v, SQTrue);
}

static SQRESULT sq_fuzzy_hash_filename(HSQUIRRELVM v) {
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, fname);

    SQChar *result = sq_getscratchpad(v, FUZZY_MAX_RESULT);
    int rc = fuzzy_hash_filename(fname, result);
    if(rc) sq_pushnull(v);
    else sq_pushstring(v, result, -1);
    return 1;
}

static SQRESULT sq_fuzzy_compare(HSQUIRRELVM v) {
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, sig1);
	SQ_GET_STRING(v, 3, sig2);

    sq_pushinteger(v, fuzzy_compare(sig1, sig2));
    return 1;
}

static SQRESULT sq_fuzzy_compare_small(HSQUIRRELVM v) {
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, sig1);
	SQ_GET_STRING(v, 3, sig2);

    sq_pushinteger(v, fuzzy_compare_small(sig1, sig2));
    return 1;
}

#define _DECL_FUNC(name,nparams,pmask) {_SC(#name), sq_##name,nparams,pmask}
static SQRegFunction fuzzy_hash_obj_funcs[]={
	_DECL_FUNC(fuzzy_hash_buf,2, _SC(".s")),
	_DECL_FUNC(fuzzy_hash_buf_small,2, _SC(".s")),
	_DECL_FUNC(fuzzy_hash_filename,2, _SC(".s")),
	_DECL_FUNC(fuzzy_compare,3, _SC(".ss")),
	_DECL_FUNC(fuzzy_compare_small,3, _SC(".ss")),
	{0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif
/* This defines a function that opens up your library. */
SQRESULT sqext_register_fuzzy_hash (HSQUIRRELVM sqvm) {
    //add a namespace sqmix
	sq_pushstring(sqvm,_SC("sqfuzzy_hash"),-1);
	sq_newclass(sqvm,SQFalse);
    sq_insert_reg_funcs(sqvm, fuzzy_hash_obj_funcs);
	sq_newslot(sqvm,-3,SQTrue); //add sq_fossil table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}
#endif

#endif

