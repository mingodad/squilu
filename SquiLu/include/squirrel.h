/*
Copyright (c) 2003-2017 Alberto Demichelis
Forked by Domingo Alvarez Duarte

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef _SQUIRREL_H_
#define _SQUIRREL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>

#ifndef SQUIRREL_API
#define SQUIRREL_API extern //__attribute__ ((visibility ("default")))
#endif

#if (defined(_WIN64) || defined(_LP64))
#ifndef _SQ64
#define _SQ64
#endif
#endif


#define SQTrue	(1)
#define SQFalse	(0)

struct SQVM;
struct SQTable;
struct SQArrayBase;
struct SQArray;
struct SQFloat64Array;
struct SQFloat32Array;
struct SQInt64Array;
struct SQInt32Array;
struct SQInt16Array;
struct SQInt8Array;
struct SQString;
struct SQClosure;
struct SQGenerator;
struct SQNativeClosure;
struct SQUserData;
struct SQFunctionProto;
struct SQRefCounted;
struct SQClass;
struct SQInstance;
struct SQDelegable;
struct SQOuter;

#if defined(_UNICODE) && !defined(ONLY_ASCII)
#define SQUNICODE
#endif

#include "sqconfig.h"

#define SQUIRREL_VERSION	_SC("SquiLu based on Squirrel 3.1 stable and Lua 5.1.5")
#define SQUIRREL_COPYRIGHT	_SC("Copyright (C) 2003-2017 Alberto Demichelis, Domingo Alvarez Duarte")
#define SQUIRREL_AUTHOR		_SC("Alberto Demichelis, Domingo Alvarez Duarte")
#define SQUIRREL_VERSION_NUMBER	310

#define SQ_VMSTATE_IDLE			0
#define SQ_VMSTATE_RUNNING		1
#define SQ_VMSTATE_SUSPENDED	2

#define SQUIRREL_EOB 0
#define SQ_BYTECODE_STREAM_TAG	0xFAFA

#define SQOBJECT_REF_COUNTED	0x08000000
#define SQOBJECT_NUMERIC		0x04000000
#define SQOBJECT_DELEGABLE		0x02000000
#define SQOBJECT_CANBEFALSE		0x01000000

#define SQ_MATCHTYPEMASKSTRING (-99999)

#define _RT_MASK 0x00FFFFFF
#define _RAW_TYPE(type) (type&_RT_MASK)

#define _RT_NULL			0x00000001
#define _RT_INTEGER			0x00000002
#define _RT_FLOAT			0x00000004
#define _RT_BOOL			0x00000008
#define _RT_STRING			0x00000010
#define _RT_TABLE			0x00000020
#define _RT_ARRAY			0x00000040
#define _RT_USERDATA		0x00000080
#define _RT_CLOSURE			0x00000100
#define _RT_NATIVECLOSURE	0x00000200
#define _RT_GENERATOR		0x00000400
#define _RT_USERPOINTER		0x00000800
#define _RT_THREAD			0x00001000
#define _RT_FUNCPROTO		0x00002000
#define _RT_CLASS			0x00004000
#define _RT_INSTANCE		0x00008000
#define _RT_WEAKREF			0x00010000
#define _RT_OUTER			0x00020000

typedef enum tagSQObjectType{
	OT_NULL =			(_RT_NULL|SQOBJECT_CANBEFALSE),
	OT_INTEGER =		(_RT_INTEGER|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE),
	OT_FLOAT =			(_RT_FLOAT|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE),
	OT_BOOL =			(_RT_BOOL|SQOBJECT_CANBEFALSE),
	OT_STRING =			(_RT_STRING|SQOBJECT_REF_COUNTED),
	OT_TABLE =			(_RT_TABLE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE),
	OT_ARRAY =			(_RT_ARRAY|SQOBJECT_REF_COUNTED),
	OT_USERDATA =		(_RT_USERDATA|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE),
	OT_CLOSURE =		(_RT_CLOSURE|SQOBJECT_REF_COUNTED),
	OT_NATIVECLOSURE =	(_RT_NATIVECLOSURE|SQOBJECT_REF_COUNTED),
	OT_GENERATOR =		(_RT_GENERATOR|SQOBJECT_REF_COUNTED),
	OT_USERPOINTER =	_RT_USERPOINTER,
	OT_THREAD =			(_RT_THREAD|SQOBJECT_REF_COUNTED) ,
	OT_FUNCPROTO =		(_RT_FUNCPROTO|SQOBJECT_REF_COUNTED), //internal usage only
	OT_CLASS =			(_RT_CLASS|SQOBJECT_REF_COUNTED),
	OT_INSTANCE =		(_RT_INSTANCE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE),
	OT_WEAKREF =		(_RT_WEAKREF|SQOBJECT_REF_COUNTED),
	OT_OUTER =			(_RT_OUTER|SQOBJECT_REF_COUNTED) //internal usage only
}SQObjectType;

#define ISREFCOUNTED(t) (t&SQOBJECT_REF_COUNTED)


typedef union tagSQObjectValue
{
	struct SQTable *pTable;
	struct SQArrayBase *pArrayBase;
	struct SQArray *pArray;
	struct SQFloat64Array *pFloat64Array;
	struct SQFloat32Array *pFloat32Array;
	struct SQInt64Array *pInt64Array;
	struct SQInt32Array *pInt32Array;
	struct SQInt16Array *pInt16Array;
	struct SQInt8Array *pInt8Array;
	struct SQClosure *pClosure;
	struct SQOuter *pOuter;
	struct SQGenerator *pGenerator;
	struct SQNativeClosure *pNativeClosure;
	struct SQString *pString;
	struct SQUserData *pUserData;
	SQInteger nInteger;
	SQFloat fFloat;
	SQUserPointer pUserPointer;
	struct SQFunctionProto *pFunctionProto;
	struct SQRefCounted *pRefCounted;
	struct SQDelegable *pDelegable;
	struct SQVM *pThread;
	struct SQClass *pClass;
	struct SQInstance *pInstance;
	struct SQWeakRef *pWeakRef;
	SQRawObjectVal raw;
}SQObjectValue;


typedef struct tagSQObject
{
	SQObjectType _type;
	SQObjectValue _unVal;
}SQObject;

typedef struct  tagSQMemberHandle{
	SQBool _static;
	SQInteger _index;
}SQMemberHandle;

typedef struct tagSQStackInfos{
	const SQChar* funcname;
	const SQChar* source;
	SQInteger line;
}SQStackInfos;

typedef struct SQVM* HSQUIRRELVM;
typedef SQObject HSQOBJECT;
typedef SQMemberHandle HSQMEMBERHANDLE;
typedef SQRESULT (*SQFUNCTION)(HSQUIRRELVM);
#ifdef SQ_WITH_DELAYED_RELEASE_HOOKS
typedef SQRESULT (*SQRELEASEHOOK)(SQUserPointer ptr, SQInteger size, HSQUIRRELVM v);
#else
typedef SQRESULT (*SQRELEASEHOOK)(SQUserPointer ptr, SQInteger size, void *ep);
#endif // SQ_WITH_DELAYED_RELEASE_HOOKS
typedef void (*SQCOMPILERERROR)(HSQUIRRELVM,const SQChar * /*desc*/,const SQChar * /*source*/,SQInteger /*line*/,SQInteger /*column*/);
typedef void (*SQPRINTFUNCTION)(HSQUIRRELVM,const SQChar * ,...);
typedef void (*SQDEBUGHOOK)(HSQUIRRELVM /*v*/, SQInteger /*type*/, const SQChar * /*sourcename*/, SQInteger /*line*/, const SQChar * /*funcname*/);
typedef SQInteger (*SQWRITEFUNC)(SQUserPointer,SQUserPointer,SQInteger);
typedef SQInteger (*SQREADFUNC)(SQUserPointer,SQUserPointer,SQInteger);

typedef SQInteger (*SQLEXREADFUNC)(SQUserPointer);

typedef struct tagSQRegFunction{
	const SQChar *name;
	SQFUNCTION f;
	SQInteger nparamscheck;
	const SQChar *typemask;
	SQBool isStatic;
}SQRegFunction;

typedef struct tagSQFunctionInfo {
	SQUserPointer funcid;
	const SQChar *name;
	const SQChar *source;
	SQInteger line;
}SQFunctionInfo;

#if !defined(sq_assert)
#define sq_assert(x)	((void)0)
#endif

#define SQ_EXTENSIONS_KEY _SC("sq__Extensions")
typedef struct {
    const char* module_name;
    SQFUNCTION module_load_func;
} sq_modules_preload_st;

#define SQUIRREL_API_FUNC(fret, fname, fargs) SQUIRREL_API fret sq_##fname fargs;
#include "sqapi.h"
#undef SQUIRREL_API_FUNC

/*UTILITY MACRO*/
#define sq_isnumeric(o) ((o)._type&SQOBJECT_NUMERIC)
#define sq_istable(o) ((o)._type==OT_TABLE)
#define sq_isarray(o) ((o)._type==OT_ARRAY)
#define sq_isfunction(o) ((o)._type==OT_FUNCPROTO)
#define sq_isclosure(o) ((o)._type==OT_CLOSURE)
#define sq_isgenerator(o) ((o)._type==OT_GENERATOR)
#define sq_isnativeclosure(o) ((o)._type==OT_NATIVECLOSURE)
#define sq_isstring(o) ((o)._type==OT_STRING)
#define sq_isinteger(o) ((o)._type==OT_INTEGER)
#define sq_isfloat(o) ((o)._type==OT_FLOAT)
#define sq_isuserpointer(o) ((o)._type==OT_USERPOINTER)
#define sq_isuserdata(o) ((o)._type==OT_USERDATA)
#define sq_isthread(o) ((o)._type==OT_THREAD)
#define sq_isnull(o) ((o)._type==OT_NULL)
#define sq_isclass(o) ((o)._type==OT_CLASS)
#define sq_isinstance(o) ((o)._type==OT_INSTANCE)
#define sq_isbool(o) ((o)._type==OT_BOOL)
#define sq_isweakref(o) ((o)._type==OT_WEAKREF)
#define sq_type(o) ((o)._type)

/* deprecated */
#define sq_createslot(v,n) sq_newslot(v,n,SQFalse)

#define SQ_OK (0)
#define SQ_ERROR (-1)

#define SQ_FAILED(res) (res<0)
#define SQ_SUCCEEDED(res) (res>=0)
#define SQ_RETURN_IF_ERROR(res) if(SQ_FAILED(res)) return res

#ifdef __GNUC__
# define SQ_UNUSED_ARG(x) x __attribute__((unused))
#else
# define SQ_UNUSED_ARG(x) x
#endif

/*DAD*/
#define SQ_FUNC_VARS(v) \
    SQRESULT _rc_; SQInteger _top_=sq_gettop(v);

#define SQ_FUNC_VARS_NO_TOP(v) \
    SQRESULT _rc_;

#define KLASS_VAR(Klass, Var) Klass *Var

#define SQ_GET_INSTANCE_VAR(v, idx, Klass, Var, Klass_tag) \
	KLASS_VAR(Klass, Var); \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&Var,(void*)Klass_tag)) < 0) return _rc_;

#define SQ_GET_INSTANCE(v, idx, Klass, Klass_tag) SQ_GET_INSTANCE_VAR(v, idx, Klass, self, Klass_tag)
#define SQ_GET_INSTANCE2(v, idx, Var, Klass, Klass_tag) SQ_GET_INSTANCE_VAR(v, idx, Klass, Var, Klass_tag)

#define SQ_GET_STRING_NVD(v, idx, var)\
    if((_rc_ = sq_getstr_and_size(v,idx, &var, &var##_size)) < 0) return _rc_;

#define SQ_GET_STRING(v, idx, var)\
    const SQChar *var;\
    SQInteger var##_size;\
    SQ_GET_STRING_NVD(v, idx, var)

#define SQ_OPT_STRING_STRLEN() static inline size_t sq_opt_strlen(const SQChar *v) {return v ? scstrlen(v) : 0;}

#define SQ_OPT_STRING_NVD(v, idx, var, dflt)\
    if(_top_ >= idx)\
        {if((_rc_ = sq_getstr_and_size(v,idx, &var, &var##_size)) < 0) return _rc_;}\
    else {var=dflt; var##_size = sq_opt_strlen(dflt);}

#define SQ_OPT_STRING(v, idx, var, dflt)\
    const SQChar *var;\
    SQInteger var##_size;\
    SQ_OPT_STRING_NVD(v, idx, var, dflt)

#define SQ_GET_BOOL_NVD(v, idx, var)\
    if((_rc_ = sq_getbool(v,idx, &var)) < 0) return _rc_;

#define SQ_GET_BOOL(v, idx, var)\
    SQBool var;\
    SQ_GET_BOOL_NVD(v, idx, var)

#define SQ_OPT_BOOL_NVD(v, idx, var, dflt)\
    if(_top_ >= idx)\
        {if((_rc_ = sq_getbool(v,idx, &var)) < 0) return _rc_;}\
    else {var=dflt;}

#define SQ_OPT_BOOL(v, idx, var, dflt)\
    SQBool var;\
    SQ_OPT_BOOL_NVD(v, idx, var, dflt)

#define SQ_GET_INTEGER_NVD(v, idx, var)\
    if((_rc_ = sq_getinteger(v,idx, &var)) < 0) return _rc_;

#define SQ_GET_INTEGER(v, idx, var)\
    SQInteger var;\
    SQ_GET_INTEGER_NVD(v, idx, var)

#define SQ_OPT_INTEGER_NVD(v, idx, var, dflt)\
    if(_top_ >= idx)\
        {if((_rc_ = sq_getinteger(v,idx, &var)) < 0) return _rc_;}\
    else {var=dflt;}

#define SQ_OPT_INTEGER(v, idx, var, dflt)\
    SQInteger var;\
    SQ_OPT_INTEGER_NVD(v, idx, var, dflt)

#define SQ_GET_FLOAT_NVD(v, idx, var)\
    if((_rc_ = sq_getfloat(v,idx, &var)) < 0) return _rc_;

#define SQ_GET_FLOAT(v, idx, var)\
    SQFloat var;\
    SQ_GET_FLOAT_NVD(v, idx, var)

#define SQ_OPT_FLOAT_NVD(v, idx, var, dflt)\
    if(_top_ >= idx)\
        {if((_rc_ = sq_getfloat(v,idx, &var)) < 0) return _rc_;}\
    else {var=dflt;}

#define SQ_OPT_FLOAT(v, idx, var, dflt)\
    SQFloat var;\
    SQ_OPT_FLOAT_NVD(v, idx, var, dflt)

#define sq_pushliteral(v, str) sq_pushstring(v, str, sizeof(str)-1)

#define SQ_GET_USERPOINTER_NVD(v, idx, var)\
    if((_rc_ = sq_getuserpointer(v,idx, &var)) < 0) return _rc_;

#define SQ_GET_USERPOINTER(v, idx, var)\
    SQUserPointer var;\
    SQ_GET_USERPOINTER_NVD(v, idx, var)

#define SQ_CHECK_TYPE(v, idx, tp) if(sq_gettype(v, idx) != OT_##tp) return sq_throwerror(v, _SC( #tp " expected for parameter (%d)"), idx-1);
#define SQ_CHECK_ARRAY(v, idx) SQ_CHECK_TYPE(v, idx, ARRAY)
#define SQ_CHECK_TABLE(v, idx) SQ_CHECK_TYPE(v, idx, TABLE)

#define SIZEOF_SQCHAR_STRING(str) ((sizeof(str)/sizeof(SQChar))-1)


struct SQStrBufState{
	const SQChar *buf;
	SQInteger ptr;
	SQInteger size;
};

SQInteger sq_strbuf_lexfeed(SQUserPointer file);
SQRESULT sqext_register_SQLexer (HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQUIRREL_H_*/
