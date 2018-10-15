/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#include "sqvm.h"
#include "sqstring.h"
#include "sqtable.h"
#include "sqarray.h"
#include "sqfuncproto.h"
#include "sqclosure.h"
#include "squserdata.h"
#include "sqcompiler.h"
#include "sqfuncstate.h"
#include "sqclass.h"
#include <stdarg.h>

static void sq_raise_type_error(HSQUIRRELVM v, SQObjectType type,SQObjectPtr &o)
{
    SQObjectPtr oval = v->PrintObjVal(o);
    v->Raise_Error(_SC("wrong argument type, expected '%s' got '%.50s'"),IdType2Name(type),_stringval(oval));
}
#define _CHECK_OBJ_TYPE(v,otype,o) if(sq_type(o) != otype) {sq_raise_type_error(v, otype, o); return SQ_ERROR;}

static bool sq_aux_gettypedarg(HSQUIRRELVM v,SQInteger idx,SQObjectType type,SQObjectPtr **o)
{
	*o = &stack_get(v,idx);
	if(sq_type(**o) != type){
		SQObjectPtr oval = v->PrintObjVal(**o);
		v->Raise_Error(_SC("wrong argument type, expected '%s' got '%.50s'"),IdType2Name(type),_stringval(oval));
		return false;
	}
	return true;
}

#define _GETSAFE_OBJ(v,idx,type,o) { if(!sq_aux_gettypedarg(v,idx,type,&o)) return SQ_ERROR; }

#define sq_aux_paramscheck(v,count) \
{ \
	if(sq_gettop(v) < count){ v->Raise_Error(_SC("not enough params in the stack")); return SQ_ERROR; }\
}


static SQInteger sq_aux_invalidtype(HSQUIRRELVM v,SQObjectType type)
{
	return sq_throwerror(v, _SC("unexpected type %s"), IdType2Name(type));
}

HSQUIRRELVM sq_open(SQInteger initialstacksize)
{
	SQSharedState *ss;
	SQVM *v;
	sq_new(ss, SQSharedState);
	ss->Init();
	v = (SQVM *)SQ_MALLOC(sizeof(SQVM));
	new (v) SQVM(ss);
	ss->_root_vm = v;
	if(v->Init(NULL, initialstacksize)) {
		return v;
	}
	sq_delete(v, SQVM);
	return NULL;
}

HSQUIRRELVM sq_newthread(HSQUIRRELVM friendvm, SQInteger initialstacksize)
{
	SQSharedState *ss;
	SQVM *v;
	ss=_ss(friendvm);

	v= (SQVM *)SQ_MALLOC(sizeof(SQVM));
	new (v) SQVM(ss);

	if(v->Init(friendvm, initialstacksize)) {
		friendvm->Push(v);
		return v;
	} else {
		sq_delete(v, SQVM);
		return NULL;
	}
}

SQInteger sq_getvmstate(HSQUIRRELVM v)
{
	if(v->_suspended)
		return SQ_VMSTATE_SUSPENDED;
	else {
		if(v->_callsstacksize != 0) return SQ_VMSTATE_RUNNING;
		else return SQ_VMSTATE_IDLE;
	}
}

void sq_seterrorhandler(HSQUIRRELVM v)
{
	SQObjectPtr &o = stack_get(v, -1);
	if(sq_isclosure(o) || sq_isnativeclosure(o) || sq_isnull(o)) {
		v->_errorhandler = o;
		v->Pop();
	}
}

SQRESULT sq_geterrorhandler(HSQUIRRELVM v)
{
    v->Push(v->_errorhandler);
	return 1;
}

void sq_setatexithandler(HSQUIRRELVM v)
{
	SQObjectPtr &o = stack_get(v, -1);
	if(sq_isclosure(o) || sq_isnativeclosure(o) || sq_isnull(o)) {
		v->_atexithandler = o;
		v->Pop();
	}
}

SQRESULT sq_getatexithandler(HSQUIRRELVM v)
{
    v->Push(v->_atexithandler);
	return 1;
}

void sq_setnativedebughook(HSQUIRRELVM v,SQDEBUGHOOK hook)
{
	v->_debughook_native = hook;
	v->_debughook_closure.Null();
	v->_debughook = hook?true:false;
}

void sq_setdebughook(HSQUIRRELVM v)
{
    SQObjectPtr &o = stack_get(v,-1);
    if(sq_isclosure(o) || sq_isnativeclosure(o) || sq_isnull(o)) {
        v->_debughook_closure = o;
        v->_debughook_native = NULL;
        v->_debughook = !sq_isnull(o);
        v->Pop();
    }
}

void sq_close(HSQUIRRELVM v)
{
	SQSharedState *ss = _ss(v);
	_thread(ss->_root_vm)->Finalize();
	sq_delete(ss, SQSharedState);
}

SQInteger sq_getversion()
{
	return SQUIRREL_VERSION_NUMBER;
}

void sq_set_include_path(HSQUIRRELVM v, const SQChar *include_path)
{
	v->SetIncludePath(include_path);
}

const SQChar * sq_get_include_path(HSQUIRRELVM v)
{
	return v->GetIncludePath();
}

SQBool sq_set_define_name(HSQUIRRELVM v, const SQChar *define_name)
{
	return v->AddDefined(define_name);
}

void sq_remove_define_name(HSQUIRRELVM v, const SQChar *define_name)
{
	v->RemoveDefined(define_name);
}

SQBool sq_exists_define_name(HSQUIRRELVM v, const SQChar *define_name)
{
	return v->IsDefined(define_name);
}

SQRESULT sq_compile(HSQUIRRELVM v,SQLEXREADFUNC read,SQUserPointer p,const SQChar *sourcename
                    ,SQBool raiseerror, SQBool show_warnings, SQInteger max_nested_includes)
{
	SQObjectPtr o;
#ifndef SQ_NO_COMPILER
	if(Compile(v, read, p, sourcename, o, raiseerror?true:false, _ss(v)->_debuginfo, show_warnings,
                max_nested_includes)) {
		v->Push(SQClosure::Create(_ss(v), _funcproto(o), _table(v->_roottable)->GetWeakRef(OT_TABLE)));
		return SQ_OK;
	}
	return SQ_ERROR;
#else
	return sq_throwerror(v,_SC("this is a no compiler build"));
#endif
}

void sq_enabledebuginfo(HSQUIRRELVM v, SQBool enable)
{
	_ss(v)->_debuginfo = enable?true:false;
}

void sq_notifyallexceptions(HSQUIRRELVM v, SQBool enable)
{
	_ss(v)->_notifyallexceptions = enable?true:false;
}

void sq_addref(HSQUIRRELVM v,HSQOBJECT *po)
{
	if(!ISREFCOUNTED(sq_type(*po))) return;
#ifdef NO_GARBAGE_COLLECTOR
	__AddRef(po->_type,po->_unVal);
#else
	_ss(v)->_refs_table.AddRef(*po);
#endif
}

SQUnsignedInteger sq_getrefcount(HSQUIRRELVM v,HSQOBJECT *po)
{
	if(!ISREFCOUNTED(sq_type(*po))) return 0;
#ifdef NO_GARBAGE_COLLECTOR
   return po->_unVal.pRefCounted->_uiRef;
#else
   return _ss(v)->_refs_table.GetRefCount(*po);
#endif
}

SQBool sq_release(HSQUIRRELVM v,HSQOBJECT *po)
{
	if(!ISREFCOUNTED(sq_type(*po))) return SQTrue;
#ifdef NO_GARBAGE_COLLECTOR
	bool ret = (po->_unVal.pRefCounted->_uiRef <= 1) ? SQTrue : SQFalse;
	__Release(po->_type,po->_unVal);
	return ret; //the ret val doesn't work(and cannot be fixed)
#else
	return v->_closing ? SQ_OK : _ss(v)->_refs_table.Release(*po);
#endif
}

SQUnsignedInteger sq_getvmrefcount(HSQUIRRELVM SQ_UNUSED_ARG(v), const HSQOBJECT *po)
{
    if (!ISREFCOUNTED(sq_type(*po))) return 0;
    return po->_unVal.pRefCounted->_uiRef;
}

const SQChar *sq_objtostring(const HSQOBJECT *o)
{
	if(sq_type(*o) == OT_STRING) {
		return _stringval(*o);
	}
	return NULL;
}

SQInteger sq_objtointeger(const HSQOBJECT *o)
{
	if(sq_isnumeric(*o)) {
		return tointeger(*o);
	}
	return 0;
}

SQFloat sq_objtofloat(const HSQOBJECT *o)
{
	if(sq_isnumeric(*o)) {
		return tofloat(*o);
	}
	return 0;
}

SQBool sq_objtobool(const HSQOBJECT *o)
{
	if(sq_isbool(*o)) {
		return _integer(*o);
	}
	return SQFalse;
}

SQUserPointer sq_objtouserpointer(const HSQOBJECT *o)
{
	if(sq_isuserpointer(*o)) {
		return _userpointer(*o);
	}
	return 0;
}

void sq_pushnull(HSQUIRRELVM v)
{
	v->PushNull();
}

void sq_pushstring(HSQUIRRELVM v,const SQChar *s,SQInteger len)
{
	if(s)
		v->Push(SQObjectPtr(SQString::Create(_ss(v), s, len)));
	else v->PushNull();
}

void sq_pushfstring(HSQUIRRELVM v,const SQChar *fmt, ...)
{
    if(fmt){
        SQChar str[1024];
        va_list vl;
        va_start(vl, fmt);
        SQInteger len = scvsprintf(str, sizeof(str), fmt, vl);
        va_end(vl);
        v->Push(SQObjectPtr(SQString::Create(_ss(v), str, len)));
    }
	else v->PushNull();
}

void sq_pushinteger(HSQUIRRELVM v,SQInteger n)
{
	v->Push(n);
}

void sq_pushbool(HSQUIRRELVM v,SQBool b)
{
	v->Push(b?true:false);
}

void sq_pushfloat(HSQUIRRELVM v,SQFloat n)
{
	v->Push(n);
}

void sq_pushuserpointer(HSQUIRRELVM v,SQUserPointer p)
{
    v->Push(p);
}

void sq_pushthread(HSQUIRRELVM v, HSQUIRRELVM thread)
{
    v->Push(thread);
}

SQUserPointer sq_newuserdata(HSQUIRRELVM v,SQUnsignedInteger size)
{
	SQUserData *ud = SQUserData::Create(_ss(v), size + SQ_ALIGNMENT);
	v->Push(ud);
	return (SQUserPointer)sq_aligning(ud + 1);
}

void sq_newtable(HSQUIRRELVM v)
{
	v->Push(SQTable::Create(_ss(v), 0));
}

void sq_newtableex(HSQUIRRELVM v,SQInteger initialcapacity)
{
	v->Push(SQTable::Create(_ss(v), initialcapacity));
}

void sq_newarray(HSQUIRRELVM v,SQInteger size)
{
	v->Push(SQArray::Create(_ss(v), size));
}

SQRESULT sq_newclass(HSQUIRRELVM v,SQBool hasbase)
{
	SQClass *baseclass = NULL;
	if(hasbase) {
		SQObjectPtr &base = stack_get(v,-1);
		if(sq_type(base) != OT_CLASS)
			return sq_throwerror(v,_SC("invalid base type"));
		baseclass = _class(base);
	}
	SQClass *newclass = SQClass::Create(_ss(v), baseclass);
	if(baseclass) v->Pop();
	v->Push(newclass);
	return SQ_OK;
}

SQRESULT sq_pushnewclass(HSQUIRRELVM v, const SQChar *className,
                          const SQChar *parentName,
                          void *classTag, SQRegFunction *methods,
                          SQBool leaveOnTop){
    if(!className || !classTag)
        return sq_throwerror(v, _SC("Missing class name or class tag."));
    SQBool hasBase = parentName ? SQTrue : SQFalse;
    SQInteger top = sq_gettop(v);
    HSQOBJECT obj;

	sq_pushstring(v,className,-1);
	if(hasBase){
        sq_pushstring(v, parentName,-1);
        if (SQ_FAILED(sq_getonroottable(v)) || sq_gettype(v, -1) != OT_CLASS){
            sq_settop(v, top); //leave stack as we got it
            return sq_throwerror(v, _SC("Missing base class \"%s\" for \"%s\"."), parentName, className);
        }
	}
	sq_newclass(v, hasBase);
	sq_settypetag(v,-1,classTag);
	if(methods) sq_insert_reg_funcs(v, methods);

	if(leaveOnTop){
	    sq_resetobject(&obj);
	    sq_getstackobj(v, -1, &obj);
	}

	if(SQ_FAILED(sq_newslot(v, -3, SQFalse))){
            sq_settop(v, top); //leave stack as we got it
            return SQ_ERROR;
    }
    if(leaveOnTop) sq_pushobject(v, obj);
	return SQ_OK;
}

SQBool sq_instanceof(HSQUIRRELVM v)
{
	SQObjectPtr &inst = stack_get(v,-1);
	SQObjectPtr &cl = stack_get(v,-2);
	if(sq_type(inst) != OT_INSTANCE || sq_type(cl) != OT_CLASS)
		return sq_throwerror(v,_SC("invalid param type"));
	return _instance(inst)->InstanceOf(_class(cl))?SQTrue:SQFalse;
}

#define CHECK_ARRAY_AT(check_n, arr_var) \
	sq_aux_paramscheck(v,check_n);\
	SQObjectPtr &arr_var = stack_get(v,idx);\
	_CHECK_OBJ_TYPE(v, OT_ARRAY, arr_var);

SQRESULT sq_arrayappend(HSQUIRRELVM v,SQInteger idx)
{
    CHECK_ARRAY_AT(2, arr);
    bool rc = _array(arr)->Append(v->GetUp(-1));
	v->Pop();
	if(!rc)
        return sq_throwerror(v, _SC("appending error"));
	return SQ_OK;
}

SQRESULT sq_arraypop(HSQUIRRELVM v,SQInteger idx,SQBool pushval)
{
    CHECK_ARRAY_AT(1, arr);
	if(_array(arr)->Size() > 0) {
        if(pushval != 0){
            v->PushNull();
            SQObjectPtr &o = stack_get(v, -1);
            _array(arr)->Top(o);
            //v->Push(_array(arr)->Top());
        }
		_array(arr)->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v, _SC("empty array"));
}

static inline SQRESULT sq_arrayresize_base(HSQUIRRELVM v,SQArrayBase *arr,SQInteger newsize)
{
	if(newsize >= 0) {
		arr->Resize(newsize);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("negative size"));
}

SQRESULT sq_arrayresize(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger newsize)
{
    CHECK_ARRAY_AT(1, arr);
    return sq_arrayresize_base(v, _array(arr), newsize);
}

SQRESULT sq_arrayminsize(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger minsize)
{
    CHECK_ARRAY_AT(1, arr);
	if(_array(arr)->Size() < minsize) {
        return sq_arrayresize_base(v, _array(arr), minsize);
	}
	return SQ_OK;
}

SQRESULT sq_arrayreverse(HSQUIRRELVM v,SQInteger idx)
{
    CHECK_ARRAY_AT(1, o);
	SQArrayBase *arr = _array(o);
	if(arr->Size() > 0) {
		SQObjectPtr t;
		SQInteger size = arr->Size();
		SQInteger n = size >> 1; size -= 1;
		for(SQInteger i = 0; i < n; i++) arr->_swap(i, size-i);
		return SQ_OK;
	}
	return SQ_OK;
}

SQRESULT sq_arrayremove(HSQUIRRELVM v,SQInteger idx,SQInteger itemidx)
{
    CHECK_ARRAY_AT(1, arr);
	return _array(arr)->Remove(itemidx) ? SQ_OK : sq_throwerror(v,_SC("index out of range"));
}

SQRESULT sq_arrayinsert(HSQUIRRELVM v,SQInteger idx,SQInteger destpos)
{
    CHECK_ARRAY_AT(1, arr);
	SQRESULT ret = _array(arr)->Insert(destpos, v->GetUp(-1)) ? SQ_OK : sq_throwerror(v,_SC("index out of range"));
	v->Pop();
	return ret;
}

SQRESULT sq_arrayset(HSQUIRRELVM v,SQInteger idx,SQInteger destpos)
{
    CHECK_ARRAY_AT(1, arr);
	SQRESULT ret = _array(arr)->Set(destpos, v->GetUp(-1)) ? SQ_OK : sq_throwerror(v,_SC("index out of range"));
	v->Pop();
	return ret;
}

SQRESULT sq_arrayget(HSQUIRRELVM v,SQInteger idx,SQInteger pos)
{
    CHECK_ARRAY_AT(1, arr);
	v->PushNull();
	if(!_array(arr)->Get(pos, v->GetUp(-1)))
	{
        v->Pop();
        return sq_throwerror(v,_SC("index out of range"));
	}
	return SQ_OK;
}

SQRESULT sq_arraygetsizeof(HSQUIRRELVM v,SQInteger idx)
{
    CHECK_ARRAY_AT(idx, arr);
    sq_pushinteger(v, _array(arr)->SizeOf());
	return SQ_OK;
}

void sq_newclosure(HSQUIRRELVM v,SQFUNCTION func,SQUnsignedInteger nfreevars)
{
	SQNativeClosure *nc = SQNativeClosure::Create(_ss(v), func,nfreevars);
	nc->_nparamscheck = 0;
	for(SQUnsignedInteger i = 0; i < nfreevars; i++) {
		nc->_outervalues[i] = v->Top();
		v->Pop();
	}
	v->Push(SQObjectPtr(nc));
}

SQRESULT sq_getclosureinfo(HSQUIRRELVM v,SQInteger idx,SQInteger *nparams,SQInteger *nfreevars)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_type(o) == OT_CLOSURE) {
		SQClosure *c = _closure(o);
		SQFunctionProto *proto = c->_function;
		*nparams = (SQInteger)proto->_nparameters;
		*nfreevars = (SQInteger)proto->_noutervalues;
		return SQ_OK;
	}
	else if(sq_type(o) == OT_NATIVECLOSURE)
	{
		SQNativeClosure *c = _nativeclosure(o);
		*nparams = (SQInteger)c->_nparamscheck;
		*nfreevars = c->_noutervalues;
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("the object is not a closure"));
}

SQRESULT sq_setnativeclosurename(HSQUIRRELVM v,SQInteger idx,const SQChar *name)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isnativeclosure(o)) {
		SQNativeClosure *nc = _nativeclosure(o);
		nc->_name = SQString::Create(_ss(v),name);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("the object is not a nativeclosure"));
}

SQRESULT sq_setparamscheck(HSQUIRRELVM v,SQInteger nparamscheck,const SQChar *typemask)
{
	SQObjectPtr &o = stack_get(v, -1);
	if(!sq_isnativeclosure(o))
		return sq_throwerror(v, _SC("native closure expected"));
	SQNativeClosure *nc = _nativeclosure(o);
	nc->_nparamscheck = nparamscheck;
	if(typemask) {
		SQIntVec res;
		if(!CompileTypemask(res, typemask))
			return sq_throwerror(v, _SC("invalid typemask"));
		nc->_typecheck.copy(res);
	}
	else {
		nc->_typecheck.resize(0);
	}
	if(nparamscheck == SQ_MATCHTYPEMASKSTRING) {
		nc->_nparamscheck = nc->_typecheck.size();
	}
	return SQ_OK;
}

SQRESULT sq_setfenv(HSQUIRRELVM v,SQInteger idx, SQBool cloning)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(!sq_isnativeclosure(o) &&
		!sq_isclosure(o))
		return sq_throwerror(v,_SC("the target is not a closure"));
    SQObjectPtr &env = stack_get(v,-1);
	if(!sq_istable(env) &&
		!sq_isarray(env) &&
		!sq_isclass(env) &&
		!sq_isinstance(env))
		return sq_throwerror(v,_SC("invalid environment"));
	SQWeakRef *w = _refcounted(env)->GetWeakRef(sq_type(env));
	SQObjectPtr ret;
	if(sq_isclosure(o)) {
		SQClosure *c = cloning ? _closure(o)->Clone() : _closure(o);
		__ObjRelease(c->_env);
		c->_env = w;
		__ObjAddRef(c->_env);
		if(_closure(o)->_base) {
			c->_base = _closure(o)->_base;
			__ObjAddRef(c->_base);
		}
		ret = c;
	}
	else { //then must be a native closure
		SQNativeClosure *c = cloning ? _nativeclosure(o)->Clone() : _nativeclosure(o);
		__ObjRelease(c->_env);
		c->_env = w;
		__ObjAddRef(c->_env);
		ret = c;
	}
	v->Pop();
	if(cloning) v->Push(ret);
	return SQ_OK;
}

SQRESULT sq_getfenv(HSQUIRRELVM v,SQInteger idx, SQBool roottable_when_null)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(!sq_isnativeclosure(o) &&
		!sq_isclosure(o))
		return sq_throwerror(v,_SC("the source is not a closure"));
	if(sq_isclosure(o)) {
	    if(_closure(o)->_env) v->Push(_closure(o)->_env);
	    else {
	        if(roottable_when_null) sq_pushroottable(v);
	        else sq_pushnull(v);
	    }
	}
	else { //then must be a native closure
	    if(_nativeclosure(o)->_env) v->Push(_nativeclosure(o)->_env);
	    else {
	        if(roottable_when_null) sq_pushroottable(v);
	        else sq_pushnull(v);
	    }
	}
	return SQ_OK;
}

SQRESULT sq_bindenv(HSQUIRRELVM v,SQInteger idx)
{
    return sq_setfenv(v, idx, SQTrue);
}

SQRESULT sq_getclosurename(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);

	if(!sq_isnativeclosure(o) &&
		!sq_isclosure(o))
		return sq_throwerror(v,_SC("the target is not a closure"));
	if(sq_isnativeclosure(o))
	{
		v->Push(_nativeclosure(o)->_name);
	}
	else { //closure
		v->Push(_closure(o)->_function->_name);
	}
	return SQ_OK;
}

SQRESULT sq_setclosureroot(HSQUIRRELVM v,SQInteger idx)
{
    SQObjectPtr &c = stack_get(v,idx);
    SQObject o = stack_get(v, -1);
    if(!sq_isclosure(c)) return sq_throwerror(v, _SC("closure expected"));
    if(sq_istable(o)) {
        _closure(c)->SetRoot(_table(o)->GetWeakRef(OT_TABLE));
        v->Pop();
        return SQ_OK;
    }
    return sq_throwerror(v, _SC("invalid type"));
}

SQRESULT sq_getclosureroot(HSQUIRRELVM v,SQInteger idx)
{
    SQObjectPtr &c = stack_get(v,idx);
    if(!sq_isclosure(c)) return sq_throwerror(v, _SC("closure expected"));
    v->Push(_closure(c)->_root->_obj);
    return SQ_OK;
}

SQRESULT sq_clear(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o=stack_get(v,idx);
	switch(sq_type(o)) {
		case OT_TABLE: _table(o)->Clear();	break;
		case OT_ARRAY: _array(o)->Resize(0); break;
		default:
			return sq_throwerror(v, _SC("clear only works on table and array"));

	}
	return SQ_OK;
}

void sq_pushroottable(HSQUIRRELVM v)
{
	v->Push(v->_roottable);
}

SQRESULT sq_getonroottable(HSQUIRRELVM v)
{
    SQObjectPtr &obj = v->GetUp(-1);
	if(_table(v->_roottable)->Get(obj,obj))
		return SQ_OK;
	v->Pop();
	return SQ_ERROR;
}

SQRESULT sq_setonroottable(HSQUIRRELVM v)
{
    SQObjectPtr &key = v->GetUp(-2);
	if(sq_type(key) == OT_NULL) {
		v->Pop(2);
		return sq_throwerror(v, _SC("null key"));
	}
    _table(v->_roottable)->NewSlot(key, v->GetUp(-1));
    v->Pop(2);
    return SQ_OK;
}

void sq_pushregistrytable(HSQUIRRELVM v)
{
	v->Push(_ss(v)->_registry);
}

SQRESULT sq_getonregistrytable(HSQUIRRELVM v)
{
    SQObjectPtr &obj = v->GetUp(-1);
	if(_table(_ss(v)->_registry)->Get(obj,obj))
		return SQ_OK;
	v->Pop();
	return SQ_ERROR;
}

SQRESULT sq_setonregistrytable(HSQUIRRELVM v)
{
    SQObjectPtr &key = v->GetUp(-2);
	if(sq_type(key) == OT_NULL) {
		v->Pop(2);
		return sq_throwerror(v, _SC("null key"));
	}
    _table(_ss(v)->_registry)->NewSlot(key, v->GetUp(-1));
    v->Pop(2);
    return SQ_OK;
}

SQRESULT sq_delete_on_registry_table(HSQUIRRELVM v, SQUserPointer uptr)
{
    SQObjectPtr key = uptr;
	if(sq_type(key) == OT_NULL) {
		return sq_throwerror(v, _SC("null key"));
	}
	if(v->_closing) return SQ_OK;
	SQTable *registry = _table(_ss(v)->_registry);
    if(registry) //when closing the vm releasehooks are called with NULL registry
    {
        registry->Remove(key);
        return SQ_OK;
    }
    return SQ_ERROR;
}

SQRESULT sq_preload_modules(HSQUIRRELVM v, const sq_modules_preload_st *modules){
    SQRESULT result = 0;
    SQInteger saved_top = sq_gettop(v);
    sq_pushliteral(v, SQ_EXTENSIONS_KEY);
    if(sq_getonregistrytable(v) != SQ_OK){
        //create a table for Extensions
        sq_newtable(v);
        sq_pushstring(v, SQ_EXTENSIONS_KEY, -1);
        sq_push(v, -2);
        sq_setonregistrytable(v);
    }

    while(modules && modules->module_name){
      //lua_checkstack(L, 20);
      sq_pushstring(v, modules->module_name, -1);
      sq_pushuserpointer(v, (SQUserPointer)modules->module_load_func);
      sq_rawset(v, -3);
      ++modules;
      ++result;
    }

    sq_settop(v, saved_top);
    return result;
}

SQFUNCTION sq_get_preload_module_func(HSQUIRRELVM v, const SQChar *module_name)
{
    SQFUNCTION result = NULL;
    SQInteger saved_top = sq_gettop(v);
    sq_pushliteral(v, SQ_EXTENSIONS_KEY);
    if(sq_getonregistrytable(v) == SQ_OK){
      sq_pushstring(v, module_name, -1);
      if(sq_rawget(v, -2) == SQ_OK)
      {
          sq_getuserpointer(v, -1, (SQUserPointer*)&result);
      }
    }
    sq_settop(v, saved_top);
    return result;
}

void sq_pushconsttable(HSQUIRRELVM v)
{
	v->Push(_ss(v)->_consts);
}

SQRESULT sq_setroottable(HSQUIRRELVM v)
{
	SQObjectPtr &o = stack_get(v, -1);
	if(sq_istable(o) || sq_isnull(o)) {
		v->_roottable = o;
		v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v, _SC("ivalid type"));
}

SQRESULT sq_setconsttable(HSQUIRRELVM v)
{
	SQObjectPtr &o = stack_get(v, -1);
	if(sq_istable(o)) {
		_ss(v)->_consts = o;
		v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v, _SC("invalid type, expected table"));
}

void sq_setforeignptr(HSQUIRRELVM v,SQUserPointer p)
{
    v->_foreignptr = p;
}

SQUserPointer sq_getforeignptr(HSQUIRRELVM v)
{
    return v->_foreignptr;
}

void sq_setsharedforeignptr(HSQUIRRELVM v,SQUserPointer p)
{
    _ss(v)->_foreignptr = p;
}

SQUserPointer sq_getsharedforeignptr(HSQUIRRELVM v)
{
    return _ss(v)->_foreignptr;
}

void sq_setvmreleasehook(HSQUIRRELVM v,SQRELEASEHOOK hook)
{
    v->_releasehook = hook;
}

SQRELEASEHOOK sq_getvmreleasehook(HSQUIRRELVM v)
{
    return v->_releasehook;
}

void sq_setsharedreleasehook(HSQUIRRELVM v,SQRELEASEHOOK hook)
{
    _ss(v)->_releasehook = hook;
}

SQRELEASEHOOK sq_getsharedreleasehook(HSQUIRRELVM v)
{
    return _ss(v)->_releasehook;
}

void sq_push(HSQUIRRELVM v,SQInteger idx)
{
	v->Push(stack_get(v, idx));
}

SQObjectType sq_gettype(HSQUIRRELVM v,SQInteger idx)
{
	return sq_type(stack_get(v, idx));
}

SQUIRREL_API const SQChar *sq_gettypename(HSQUIRRELVM v,SQInteger idx){
	SQObjectPtr &o = stack_get(v, idx);
    return GetTypeName(o);
}

SQRESULT sq_typeof(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v, idx);
	SQObjectPtr res;
	if(!v->TypeOf(o,res)) {
		return SQ_ERROR;
	}
	v->Push(res);
	return SQ_OK;
}

SQRESULT sq_tostring(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v, idx);
	SQObjectPtr res;
	if(!v->ToString(o,res)) {
		return SQ_ERROR;
	}
	v->Push(res);
	return SQ_OK;
}

SQRESULT sq_tobool(HSQUIRRELVM v, SQInteger idx)
{
	SQObjectPtr res;
	SQBool b;
	SQ_RETURN_IF_ERROR(sq_getbool(v,idx, &b));
	_integer(res) = b;
	v->Push(res);
	return SQ_OK;
}

SQRESULT sq_tointeger(HSQUIRRELVM v, SQInteger idx)
{
	SQObjectPtr res;
	SQInteger i;
	SQ_RETURN_IF_ERROR(sq_getinteger(v,idx, &i));
	_integer(res) = i;
	v->Push(res);
	return SQ_OK;
}

SQRESULT sq_tofloat(HSQUIRRELVM v, SQInteger idx)
{
	SQObjectPtr res;
	SQFloat f;
	SQ_RETURN_IF_ERROR(sq_getfloat(v,idx, &f));
	_float(res) = f;
	v->Push(res);
	return SQ_OK;
}

SQRESULT sq_getinteger(HSQUIRRELVM v,SQInteger idx,SQInteger *i)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isnumeric(o)) {
		*i = tointeger(o);
		return SQ_OK;
	}
    if(sq_isbool(o)) {
        *i = SQVM::IsFalse(o)?SQFalse:SQTrue;
        return SQ_OK;
    }
	return SQ_ERROR;
}

SQRESULT sq_getinteger_ptr(HSQUIRRELVM v,SQInteger idx,SQInteger **i)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isinteger(o)) {
		*i = &_integer(o);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getfloat(HSQUIRRELVM v,SQInteger idx,SQFloat *f)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isnumeric(o)) {
		*f = tofloat(o);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getfloat_ptr(HSQUIRRELVM v,SQInteger idx,SQFloat **f)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isfloat(o)) {
		*f = &_float(o);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getbool(HSQUIRRELVM v,SQInteger idx,SQBool *b)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isbool(o)) {
		*b = _integer(o);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getstring(HSQUIRRELVM v,SQInteger idx,const SQChar **c)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_STRING, o);
	*c = _stringval(o);
	return SQ_OK;
}

SQRESULT sq_getstr_and_size(HSQUIRRELVM v,SQInteger idx,const SQChar **c, SQInteger *size)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_STRING, o);
	*c = _stringval(o);
	*size = _string(o)->_len;
	return SQ_OK;
}

SQRESULT sq_getthread(HSQUIRRELVM v,SQInteger idx,HSQUIRRELVM *thread)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_THREAD, o);
	*thread = _thread(o);
	return SQ_OK;
}

SQRESULT sq_clone(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	v->PushNull();
	if(!v->Clone(o, stack_get(v, -1))){
		v->Pop();
		return SQ_ERROR;
	}
	return SQ_OK;
}

SQInteger sq_getsize(HSQUIRRELVM v, SQInteger idx)
{
	SQObjectPtr &o = stack_get(v, idx);
	SQObjectType type = sq_type(o);
	switch(type) {
	case OT_STRING:		return _string(o)->_len;
	case OT_TABLE:		return _table(o)->CountUsed();
	case OT_ARRAY:		return _array(o)->Size();
	case OT_USERDATA:	return _userdata(o)->_size;
	case OT_INSTANCE:	return _instance(o)->_class->_udsize;
	case OT_CLASS:		return _class(o)->_udsize;
	default:
		return sq_aux_invalidtype(v, type);
	}
}

SQHash sq_gethash(HSQUIRRELVM v, SQInteger idx)
{
	SQObjectPtr &o = stack_get(v, idx);
	return HashObj(o);
}

SQRESULT sq_getuserdata(HSQUIRRELVM v,SQInteger idx,SQUserPointer *p,SQUserPointer *typetag)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_USERDATA, o);
	(*p) = _userdataval(o);
	if(typetag) *typetag = _userdata(o)->_typetag;
	return SQ_OK;
}

SQRESULT sq_settypetag(HSQUIRRELVM v,SQInteger idx,SQUserPointer typetag)
{
	SQObjectPtr &o = stack_get(v,idx);
	switch(sq_type(o)) {
		case OT_USERDATA:	_userdata(o)->_typetag = typetag;	break;
		case OT_CLASS:		_class(o)->_typetag = typetag;		break;
		default:			return sq_throwerror(v,_SC("invalid object type"));
	}
	return SQ_OK;
}

SQRESULT sq_getobjtypetag(const HSQOBJECT *o,SQUserPointer * typetag)
{
  switch(sq_type(*o)) {
    case OT_INSTANCE: *typetag = _instance(*o)->_class->_typetag; break;
    case OT_USERDATA: *typetag = _userdata(*o)->_typetag; break;
    case OT_CLASS:    *typetag = _class(*o)->_typetag; break;
    default: return SQ_ERROR;
  }
  return SQ_OK;
}

SQRESULT sq_gettypetag(HSQUIRRELVM v,SQInteger idx,SQUserPointer *typetag)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(SQ_FAILED(sq_getobjtypetag(&o,typetag)))
		return SQ_ERROR;// this is not an error it should be a bool but would break backward compatibility
	return SQ_OK;
}

SQRESULT sq_getuserpointer(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_USERPOINTER, o);
	(*p) = _userpointer(o);
	return SQ_OK;
}

SQUserPointer sq_get_as_userpointer(HSQUIRRELVM v,SQInteger idx)
{
    SQObjectPtr &o = stack_get(v,idx);
    return _userpointer(o);
}

SQRESULT sq_setinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer p)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sq_type(o) != OT_INSTANCE) return sq_throwerror(v,_SC("the object is not a class instance"));
	_instance(o)->_userpointer = p;
	return SQ_OK;
}

SQRESULT sq_setclassudsize(HSQUIRRELVM v, SQInteger idx, SQInteger udsize)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sq_type(o) != OT_CLASS) return sq_throwerror(v,_SC("the object is not a class"));
	if(_class(o)->_locked) return sq_throwerror(v,_SC("the class is locked"));
	_class(o)->_udsize = udsize;
	return SQ_OK;
}


SQRESULT sq_getinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p,SQUserPointer typetag)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sq_type(o) != OT_INSTANCE) return sq_throwerror(v,_SC("the object is not a class instance"));
	(*p) = _instance(o)->_userpointer;
	if(typetag != 0) {
		SQClass *cl = _instance(o)->_class;
		do{
			if(cl->_typetag == typetag)
				return SQ_OK;
			cl = cl->_base;
		}while(cl != NULL);
		return sq_throwerror(v,_SC("invalid type tag"));
	}
	return SQ_OK;
}

SQInteger sq_getfulltop(HSQUIRRELVM v)
{
	return v->_top;
}

SQInteger sq_gettop(HSQUIRRELVM v)
{
	return (v->_top) - v->_stackbase;
}

void sq_settop(HSQUIRRELVM v, SQInteger newtop)
{
	SQInteger top = sq_gettop(v);
	if(top > newtop)
		sq_pop(v, top - newtop);
	else
		while(top++ < newtop) sq_pushnull(v);
}

void sq_pop(HSQUIRRELVM v, SQInteger nelemstopop)
{
	assert(v->_top >= nelemstopop);
	v->Pop(nelemstopop);
}

void sq_poptop(HSQUIRRELVM v)
{
	assert(v->_top >= 1);
    v->Pop();
}


void sq_remove(HSQUIRRELVM v, SQInteger idx)
{
	v->Remove(idx);
}

void sq_insert(HSQUIRRELVM v, SQInteger idx)
{
	v->Insert(idx);
}

void sq_replace(HSQUIRRELVM v, SQInteger idx)
{
	v->Replace(idx);
}

SQInteger sq_compare(HSQUIRRELVM v, SQInteger idx1, SQInteger idx2)
{
	SQInteger res;
	v->ObjCmp(stack_get(v, idx1), stack_get(v, idx2),res);
	return res;
}

SQInteger sq_cmp(HSQUIRRELVM v)
{
	SQInteger res;
	v->ObjCmp(stack_get(v, -1), stack_get(v, -2),res);
	return res;
}

SQRESULT sq_newslot(HSQUIRRELVM v, SQInteger idx, SQBool bstatic)
{
	sq_aux_paramscheck(v, 3);
	SQObjectPtr &self = stack_get(v, idx);
	if(sq_type(self) == OT_TABLE || sq_type(self) == OT_CLASS) {
		SQObjectPtr &key = v->GetUp(-2);
		if(sq_type(key) == OT_NULL) return sq_throwerror(v, _SC("null is not a valid key"));
		v->NewSlot(self, key, v->GetUp(-1),bstatic?true:false);
		v->Pop(2);
	}
	return SQ_OK;
}

SQRESULT sq_deleteslot(HSQUIRRELVM v,SQInteger idx,SQBool pushval)
{
	sq_aux_paramscheck(v, 2);
	SQObjectPtr &self = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_TABLE, self);
	SQObjectPtr &key = v->GetUp(-1);
	if(sq_type(key) == OT_NULL) return sq_throwerror(v, _SC("null is not a valid key"));
	SQObjectPtr res;
	if(!v->DeleteSlot(self, key, res)){
		v->Pop();
		return SQ_ERROR;
	}
	if(pushval)	v->GetUp(-1) = res;
	else v->Pop();
	return SQ_OK;
}

SQRESULT sq_set(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self = stack_get(v, idx);
	if(v->Set(self, v->GetUp(-2), v->GetUp(-1),DONT_FALL_BACK)) {
		v->Pop(2);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_rawset(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self = stack_get(v, idx);
	SQObjectPtr &key = v->GetUp(-2);
	if(sq_type(key) == OT_NULL) {
		v->Pop(2);
		return sq_throwerror(v, _SC("null key"));
	}
	switch(sq_type(self)) {
	case OT_TABLE:
		_table(self)->NewSlot(key, v->GetUp(-1));
		v->Pop(2);
		return SQ_OK;

	case OT_CLASS:
		_class(self)->NewSlot(_ss(v), key, v->GetUp(-1),false);
		v->Pop(2);
		return SQ_OK;

	case OT_INSTANCE:
		if(_instance(self)->Set(key, v->GetUp(-1))) {
			v->Pop(2);
			return SQ_OK;
		}
	break;
	case OT_ARRAY:
		if(v->Set(self, key, v->GetUp(-1),false)) {
			v->Pop(2);
			return SQ_OK;
		}
	break;
	default:
		v->Pop(2);
		return sq_throwerror(v, _SC("rawset works only on array/table/class and instance"));
	}
	v->Raise_IdxError(v->GetUp(-2));return SQ_ERROR;
}

SQRESULT sq_newmember(HSQUIRRELVM v,SQInteger idx,SQBool bstatic)
{
	SQObjectPtr &self = stack_get(v, idx);
	if(sq_type(self) != OT_CLASS) return sq_throwerror(v, _SC("new member only works with classes"));
	SQObjectPtr &key = v->GetUp(-3);
	if(sq_type(key) == OT_NULL) return sq_throwerror(v, _SC("null key"));
	if(!v->NewSlotA(self,key,v->GetUp(-2),v->GetUp(-1),bstatic?true:false,false)) {
         v->Pop(3);
         return SQ_ERROR;
    }
    v->Pop(3);
	return SQ_OK;
}

SQRESULT sq_rawnewmember(HSQUIRRELVM v,SQInteger idx,SQBool bstatic)
{
	SQObjectPtr &self = stack_get(v, idx);
	if(sq_type(self) != OT_CLASS) return sq_throwerror(v, _SC("new member only works with classes"));
	SQObjectPtr &key = v->GetUp(-3);
	if(sq_type(key) == OT_NULL) return sq_throwerror(v, _SC("null key"));
	if(!v->NewSlotA(self,key,v->GetUp(-2),v->GetUp(-1),bstatic?true:false,true)) {
         v->Pop(3);
         return SQ_ERROR;
    }
    v->Pop(3);
	return SQ_OK;
}

SQRESULT sq_setdelegate(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self = stack_get(v, idx);
	SQObjectPtr &mt = v->GetUp(-1);
	SQObjectType type = sq_type(self);
	switch(type) {
	case OT_TABLE:
		if(sq_type(mt) == OT_TABLE) {
			if(!_table(self)->SetDelegate(_table(mt))) {
                return sq_throwerror(v, _SC("delagate cycle"));
            }
			v->Pop();
        }
		else if(sq_type(mt)==OT_NULL) {
			_table(self)->SetDelegate(NULL); v->Pop(); }
		else return sq_aux_invalidtype(v,type);
		break;
	case OT_USERDATA:
		if(sq_type(mt)==OT_TABLE) {
			_userdata(self)->SetDelegate(_table(mt)); v->Pop(); }
		else if(sq_type(mt)==OT_NULL) {
			_userdata(self)->SetDelegate(NULL); v->Pop(); }
		else return sq_aux_invalidtype(v, type);
		break;
	default:
			return sq_aux_invalidtype(v, type);
	}
	return SQ_OK;
}

SQRESULT sq_rawdeleteslot(HSQUIRRELVM v,SQInteger idx,SQBool pushval)
{
	sq_aux_paramscheck(v, 2);
	SQObjectPtr &self = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_TABLE, self);
	SQObjectPtr &key = v->GetUp(-1);
	SQObjectPtr t;
	if(_table(self)->Get(key,t)) {
		_table(self)->Remove(key);
	}
	if(pushval != 0)
		v->GetUp(-1) = t;
	else
		v->Pop();
	return SQ_OK;
}

SQRESULT sq_getdelegate(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	switch(sq_type(self)){
	case OT_TABLE:
	case OT_USERDATA:
		if(!_delegable(self)->_delegate){
			v->PushNull();
			break;
		}
		v->Push(SQObjectPtr(_delegable(self)->_delegate));
		break;
	default: return sq_throwerror(v,_SC("wrong type"));
	}
	return SQ_OK;

}

SQRESULT sq_get(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	SQObjectPtr &obj = v->GetUp(-1);
	if(v->Get(self,obj,obj,false,DONT_FALL_BACK))
		return SQ_OK;
	v->Pop();
	return SQ_ERROR;
}

SQRESULT sq_getbyname(HSQUIRRELVM v,SQInteger idx, const SQChar *key, SQInteger key_len)
{
	SQObjectPtr &self=stack_get(v,idx);
	sq_pushstring(v, key, key_len);
	SQObjectPtr &obj = v->GetUp(-1);
	switch(sq_type(self)) {
	case OT_TABLE:
		if(_table(self)->Get(obj,obj))
			return SQ_OK;
		break;
	case OT_CLASS:
		if(_class(self)->Get(obj,obj))
			return SQ_OK;
		break;
	case OT_INSTANCE:
		if(_instance(self)->Get(obj,obj))
			return SQ_OK;
		break;
	default:
		sq_poptop(v);
		return sq_throwerror(v,_SC("sq_getbyname works only on table/instance and class"));
	}
	sq_poptop(v);
	return sq_throwerror(v,_SC("the index doesn't exist"));
}

SQRESULT sq_rawget(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	SQObjectPtr &obj = v->GetUp(-1);
	switch(sq_type(self)) {
	case OT_TABLE:
		if(_table(self)->Get(obj,obj))
			return SQ_OK;
		break;
	case OT_CLASS:
		if(_class(self)->Get(obj,obj))
			return SQ_OK;
		break;
	case OT_INSTANCE:
		if(_instance(self)->Get(obj,obj))
			return SQ_OK;
		break;
	case OT_ARRAY:{
		if(sq_isnumeric(obj)){
			if(_array(self)->Get(tointeger(obj),obj)) {
				return SQ_OK;
			}
		}
		else {
			v->Pop();
			return sq_throwerror(v,_SC("invalid index type for an array"));
		}
				  }
		break;
	default:
		v->Pop();
		return sq_throwerror(v,_SC("rawget works only on array/table/instance and class"));
	}
	v->Pop();
	return sq_throwerror(v,_SC("the index doesn't exist"));
}

SQBool sq_rawexists(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	SQObjectPtr &obj = v->GetUp(-1);
	SQBool result = SQFalse;
	switch(sq_type(self)) {
	case OT_TABLE:
		result = _table(self)->Exists(obj);
		break;
	case OT_CLASS:
		result = _class(self)->Exists(obj);
		break;
	case OT_INSTANCE:
		result = _instance(self)->Exists(obj);
		break;
	case OT_ARRAY:
		if(sq_isnumeric(obj)){
			result = _array(self)->Exists(tointeger(obj));
		}
		break;
	default:
		break;
	}
	v->Pop();
	return result;
}

SQRESULT sq_getstackobj(HSQUIRRELVM v,SQInteger idx,HSQOBJECT *po)
{
	*po=stack_get(v,idx);
	return SQ_OK;
}

const SQChar *sq_getlocal(HSQUIRRELVM v,SQUnsignedInteger level,SQUnsignedInteger idx)
{
	SQUnsignedInteger cstksize=v->_callsstacksize;
	SQUnsignedInteger lvl=(cstksize-level)-1;
	SQInteger stackbase=v->_stackbase;
	if(lvl<cstksize){
		for(SQUnsignedInteger i=0;i<level;i++){
			SQVM::CallInfo &ci=v->_callsstack[(cstksize-i)-1];
			stackbase-=ci._prevstkbase;
		}
		SQVM::CallInfo &ci=v->_callsstack[lvl];
		if(sq_type(ci._closure)!=OT_CLOSURE)
			return NULL;
		SQClosure *c=_closure(ci._closure);
		SQFunctionProto *func=c->_function;
		if(func->_noutervalues > (SQInteger)idx) {
			v->Push(*_outer(c->_outervalues[idx])->_valptr);
			return _stringval(func->_outervalues[idx]._name);
		}
		idx -= func->_noutervalues;
		return func->GetLocal(v,stackbase,idx,(SQInteger)(ci._ip-func->_instructions)-1);
	}
	return NULL;
}

void sq_pushobject(HSQUIRRELVM v,HSQOBJECT obj)
{
	v->Push(SQObjectPtr(obj));
}

void sq_resetobject(HSQOBJECT *po)
{
	po->_unVal.pUserPointer=NULL;po->_type=OT_NULL;
}

SQRESULT sq_throwerror(HSQUIRRELVM v,const SQChar *fmt, ...)
{
    if(fmt)
    {
        SQChar err[256];
        va_list vl;
        va_start(vl, fmt);
        scvsprintf(err, sizeof(err), fmt, vl);
        va_end(vl);
        v->_lasterror=SQString::Create(_ss(v),err);
    }
    else v->_lasterror.Null();
	return SQ_ERROR;
}

SQRESULT sq_throwobject(HSQUIRRELVM v)
{
	v->_lasterror = v->GetUp(-1);
	v->Pop();
	return SQ_ERROR;
}


void sq_reseterror(HSQUIRRELVM v)
{
	v->_lasterror.Null();
}

void sq_getlasterror(HSQUIRRELVM v)
{
	v->Push(v->_lasterror);
}

void sq_getlaststackinfo(HSQUIRRELVM v)
{
    sq_newtable(v);
    sq_pushliteral(v, _SC("line"));
    sq_pushinteger(v, v->_lasterror_stackinfo.line);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("source"));
    sq_pushstring(v, v->_lasterror_stackinfo.source, -1);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("funcname"));
    sq_pushstring(v, v->_lasterror_stackinfo.funcname, -1);
    sq_rawset(v, -3);
}

const SQChar *sq_getlasterror_str(HSQUIRRELVM v)
{
	return _stringval(v->_lasterror);
}

void sq_getlasterror_line_col(HSQUIRRELVM v, SQInteger *line, SQInteger *column)
{
    *line = v->_lasterror_line;
    *column = v->_lasterror_column;
}

SQRESULT sq_reservestack(HSQUIRRELVM v,SQInteger nsize)
{
	if (((SQUnsignedInteger)v->_top + nsize) > v->_stack.size()) {
		if(v->_nmetamethodscall) {
			return sq_throwerror(v,_SC("cannot resize stack while in  a metamethod"));
		}
		v->_stack.resize(v->_stack.size() + ((v->_top + nsize) - v->_stack.size()));
	}
	return SQ_OK;
}

SQRESULT sq_resume(HSQUIRRELVM v,SQBool retval,SQBool raiseerror)
{
	if (sq_type(v->GetUp(-1)) == OT_GENERATOR)
	{
		v->PushNull(); //retval
		if (!v->Execute(v->GetUp(-2), 0, v->_top, v->GetUp(-1), raiseerror, SQVM::ET_RESUME_GENERATOR))
		{v->Raise_Error(v->_lasterror); return SQ_ERROR;}
		if(!retval)
			v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("only generators can be resumed"));
}

SQRESULT sq_call(HSQUIRRELVM v,SQInteger params,SQBool retval,SQBool raiseerror)
{
	SQObjectPtr res;
	if(!v->Call(v->GetUp(-(params+1)),params,v->_top-params,res,raiseerror?true:false)){
        v->Pop(params); //pop args
		return SQ_ERROR;
	}
	if(!v->_suspended)
		v->Pop(params); //pop args
     if(retval)
        v->Push(res); // push result
    return SQ_OK;
}

SQRESULT sq_tailcall(HSQUIRRELVM v, SQInteger nparams)
{
	SQObjectPtr &res = v->GetUp(-(nparams + 1));
	if (sq_type(res) != OT_CLOSURE) {
		return sq_throwerror(v, _SC("only closure can be tail called"));
	}
	SQClosure *clo = _closure(res);
	if (clo->_function->_bgenerator)
	{
		return sq_throwerror(v, _SC("generators cannot be tail called"));
	}

	SQInteger stackbase = (v->_top - nparams) - v->_stackbase;
	if (!v->TailCall(clo, stackbase, nparams)) {
		return SQ_ERROR;
	}
	return SQ_TAILCALL_FLAG;
}

SQRESULT sq_suspendvm(HSQUIRRELVM v)
{
	return v->Suspend();
}

SQRESULT sq_wakeupvm(HSQUIRRELVM v,SQBool wakeupret,SQBool retval,SQBool raiseerror,SQBool throwerror)
{
	SQObjectPtr ret;
	if(!v->_suspended)
		return sq_throwerror(v,_SC("cannot resume a vm that is not running any code"));
	SQInteger target = v->_suspended_target;
	if(wakeupret) {
		if(target != -1) {
			v->GetAt(v->_stackbase+v->_suspended_target)=v->GetUp(-1); //retval
		}
		v->Pop();
	} else if(target != -1) { v->GetAt(v->_stackbase+v->_suspended_target).Null(); }
	SQObjectPtr dummy;
	if(!v->Execute(dummy,-1,-1,ret,raiseerror,throwerror?SQVM::ET_RESUME_THROW_VM : SQVM::ET_RESUME_VM)) {
		return SQ_ERROR;
	}
	if(retval)
		v->Push(ret);
	return SQ_OK;
}

void sq_setreleasehook(HSQUIRRELVM v,SQInteger idx,SQRELEASEHOOK hook)
{
	if(sq_gettop(v) >= 1){
		SQObjectPtr &ud=stack_get(v,idx);
		switch( sq_type(ud) ) {
		case OT_USERDATA:	_userdata(ud)->_hook = hook;	break;
		case OT_INSTANCE:	_instance(ud)->_hook = hook;	break;
		case OT_CLASS:		_class(ud)->_hook = hook;		break;
		default: break; //shutup compiler
		}
	}
}

SQRELEASEHOOK sq_getreleasehook(HSQUIRRELVM v,SQInteger idx)
{
    if(sq_gettop(v) >= 1){
        SQObjectPtr &ud=stack_get(v,idx);
        switch( sq_type(ud) ) {
        case OT_USERDATA:   return _userdata(ud)->_hook;
        case OT_INSTANCE:   return _instance(ud)->_hook;
        case OT_CLASS:      return _class(ud)->_hook;
        default: break; //shutup compiler
        }
    }
    return NULL;
}

void sq_setcompilererrorhandler(HSQUIRRELVM v,SQCOMPILERERROR f)
{
	_ss(v)->_compilererrorhandler = f;
}

SQRESULT sq_writeclosure(HSQUIRRELVM v,SQWRITEFUNC w,SQUserPointer up)
{
	SQObjectPtr &o = stack_get(v,-1);
	_CHECK_OBJ_TYPE(v, OT_CLOSURE, o);
	unsigned short tag = SQ_BYTECODE_STREAM_TAG;
	if(_closure(o)->_function->_noutervalues)
		return sq_throwerror(v,_SC("a closure with free valiables bound cannot be serialized"));
	if(w(up,&tag,2) != 2)
		return sq_throwerror(v,_SC("io error"));
	if(!_closure(o)->Save(v,up,w))
		return SQ_ERROR;
	return SQ_OK;
}

SQRESULT sq_writeclosure_as_source(HSQUIRRELVM v,SQWRITEFUNC w,SQUserPointer up)
{
	SQObjectPtr &o = stack_get(v,-1);
	_CHECK_OBJ_TYPE(v, OT_CLOSURE, o);
	if(_closure(o)->_function->_noutervalues)
		return sq_throwerror(v,_SC("a closure with free valiables bound cannot be serialized"));
    const SQChar decl[] = _SC("local bytecode = ");
	if(w(up, (void*)decl, scstrlen(decl)) != (int)scstrlen(decl))
		return sq_throwerror(v,_SC("io error"));
	if(!_closure(o)->SaveAsSource(v,up,w))
		return SQ_ERROR;
	return SQ_OK;
}

SQRESULT sq_readclosure(HSQUIRRELVM v,SQREADFUNC r,SQUserPointer up)
{
	SQObjectPtr closure;

	unsigned short tag;
	if(r(up,&tag,2) != 2)
		return sq_throwerror(v,_SC("io error"));
	if(tag != SQ_BYTECODE_STREAM_TAG)
		return sq_throwerror(v,_SC("invalid stream"));
	if(!SQClosure::Load(v,up,r,closure))
		return SQ_ERROR;
	v->Push(closure);
	return SQ_OK;
}

SQChar *sq_getscratchpad(HSQUIRRELVM v,SQInteger minsize)
{
	return _ss(v)->GetScratchPad(minsize);
}

SQRESULT sq_resurrectunreachable(HSQUIRRELVM v)
{
#ifndef NO_GARBAGE_COLLECTOR
	_ss(v)->ResurrectUnreachable(v);
	return SQ_OK;
#else
	return sq_throwerror(v,_SC("sq_resurrectunreachable requires a garbage collector build"));
#endif
}

SQInteger sq_collectgarbage(HSQUIRRELVM v)
{
#ifdef NO_GARBAGE_COLLECTOR
#ifdef SQ_WITH_DELAYED_RELEASE_HOOKS
    _ss(v)->CallDelayedReleaseHooks(v);
#endif // SQ_WITH_DELAYED_RELEASE_HOOKS
	return _ss(v)->CollectGarbage(v);
#else
	return -1;
#endif
}

SQRESULT sq_getcallee(HSQUIRRELVM v)
{
	if(v->_callsstacksize > 1)
	{
		v->Push(v->_callsstack[v->_callsstacksize - 2]._closure);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("no closure in the calls stack"));
}

const SQChar *sq_getfreevariable(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger nval)
{
	SQObjectPtr &self=stack_get(v,idx);
	const SQChar *name = NULL;
	switch(sq_type(self))
	{
	case OT_CLOSURE:{
		SQClosure *clo = _closure(self);
		SQFunctionProto *fp = clo->_function;
		if(((SQUnsignedInteger)fp->_noutervalues) > nval) {
			v->Push(*(_outer(clo->_outervalues[nval])->_valptr));
			SQOuterVar &ov = fp->_outervalues[nval];
			name = _stringval(ov._name);
		}
					}
		break;
	case OT_NATIVECLOSURE:{
		SQNativeClosure *clo = _nativeclosure(self);
		if(clo->_noutervalues > nval) {
			v->Push(clo->_outervalues[nval]);
			name = _SC("@NATIVE");
		}
						  }
		break;
	default: break; //shutup compiler
	}
	return name;
}

SQRESULT sq_setfreevariable(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger nval)
{
	SQObjectPtr &self=stack_get(v,idx);
	switch(sq_type(self))
	{
	case OT_CLOSURE:{
		SQFunctionProto *fp = _closure(self)->_function;
		if(((SQUnsignedInteger)fp->_noutervalues) > nval){
			*(_outer(_closure(self)->_outervalues[nval])->_valptr) = stack_get(v,-1);
		}
		else return sq_throwerror(v,_SC("invalid free var index"));
					}
		break;
	case OT_NATIVECLOSURE:
		if(_nativeclosure(self)->_noutervalues > nval){
			_nativeclosure(self)->_outervalues[nval] = stack_get(v,-1);
		}
		else return sq_throwerror(v,_SC("invalid free var index"));
		break;
	default:
		return sq_aux_invalidtype(v,sq_type(self));
	}
	v->Pop();
	return SQ_OK;
}

SQRESULT sq_setattributes(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_CLASS, o);
	SQObjectPtr &key = stack_get(v,-2);
	SQObjectPtr &val = stack_get(v,-1);
	SQObjectPtr attrs;
	if(sq_type(key) == OT_NULL) {
		attrs = _class(o)->_attributes;
		_class(o)->_attributes = val;
		v->Pop(2);
		v->Push(attrs);
		return SQ_OK;
	}else if(_class(o)->GetAttributes(key,attrs)) {
		_class(o)->SetAttributes(key,val);
		v->Pop(2);
		v->Push(attrs);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("wrong index"));
}

SQRESULT sq_getattributes(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_CLASS, o);
	SQObjectPtr &key = stack_get(v,-1);
	SQObjectPtr attrs;
	if(sq_type(key) == OT_NULL) {
		attrs = _class(o)->_attributes;
		v->Pop();
		v->Push(attrs);
		return SQ_OK;
	}
	else if(_class(o)->GetAttributes(key,attrs)) {
		v->Pop();
		v->Push(attrs);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("wrong index"));
}

SQRESULT sq_getmemberhandle(HSQUIRRELVM v,SQInteger idx,HSQMEMBERHANDLE *handle)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_CLASS, o);
	SQObjectPtr &key = stack_get(v,-1);
	SQTable *m = _class(o)->_members;
	SQObjectPtr val;
	if(m->Get(key,val)) {
		handle->_static = _isfield(val) ? SQFalse : SQTrue;
		handle->_index = _member_idx(val);
		v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("wrong index"));
}

SQRESULT _getmemberbyhandle(HSQUIRRELVM v,SQObjectPtr &self,const HSQMEMBERHANDLE *handle,SQObjectPtr *&val)
{
	switch(sq_type(self)) {
		case OT_INSTANCE: {
				SQInstance *i = _instance(self);
				if(handle->_static) {
					SQClass *c = i->_class;
					val = &c->_methods[handle->_index].val;
				}
				else {
					val = &i->_values[handle->_index];

				}
			}
			break;
		case OT_CLASS: {
				SQClass *c = _class(self);
				if(handle->_static) {
					val = &c->_methods[handle->_index].val;
				}
				else {
					val = &c->_defaultvalues[handle->_index].val;
				}
			}
			break;
		default:
			return sq_throwerror(v,_SC("wrong type(expected class or instance)"));
	}
	return SQ_OK;
}

SQRESULT sq_getbyhandle(HSQUIRRELVM v,SQInteger idx,const HSQMEMBERHANDLE *handle)
{
	SQObjectPtr &self = stack_get(v,idx);
	SQObjectPtr *val = NULL;
	if(SQ_FAILED(_getmemberbyhandle(v,self,handle,val))) {
		return SQ_ERROR;
	}
	v->Push(_realval(*val));
	return SQ_OK;
}

SQRESULT sq_setbyhandle(HSQUIRRELVM v,SQInteger idx,const HSQMEMBERHANDLE *handle)
{
	SQObjectPtr &self = stack_get(v,idx);
	SQObjectPtr &newval = stack_get(v,-1);
	SQObjectPtr *val = NULL;
	if(SQ_FAILED(_getmemberbyhandle(v,self,handle,val))) {
		return SQ_ERROR;
	}
	*val = newval;
	v->Pop();
	return SQ_OK;
}

SQRESULT sq_getbase(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_CLASS, o);
	if(_class(o)->_base)
		v->Push(SQObjectPtr(_class(o)->_base));
	else
		v->PushNull();
	return SQ_OK;
}

SQRESULT sq_getclass(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_INSTANCE, o);
	v->Push(SQObjectPtr(_instance(o)->_class));
	return SQ_OK;
}

SQRESULT sq_createinstance(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	_CHECK_OBJ_TYPE(v, OT_CLASS, o);
	v->Push(_class(o)->CreateInstance());
	return SQ_OK;
}

void sq_weakref(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o=stack_get(v,idx);
	if(ISREFCOUNTED(sq_type(o))) {
		v->Push(_refcounted(o)->GetWeakRef(sq_type(o)));
		return;
	}
	v->Push(o);
}

SQRESULT sq_getweakrefval(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sq_type(o) != OT_WEAKREF) {
		return sq_throwerror(v,_SC("the object must be a weakref"));
	}
	v->Push(_weakref(o)->_obj);
	return SQ_OK;
}

SQRESULT sq_getdefaultdelegate(HSQUIRRELVM v,SQObjectType t)
{
	SQSharedState *ss = _ss(v);
	switch(t) {
	case OT_TABLE: v->Push(ss->_table_default_delegate); break;
	case OT_ARRAY: v->Push(ss->_array_default_delegate); break;
	case OT_STRING: v->Push(ss->_string_default_delegate); break;
	case OT_INTEGER: case OT_FLOAT: v->Push(ss->_number_default_delegate); break;
	case OT_GENERATOR: v->Push(ss->_generator_default_delegate); break;
	case OT_CLOSURE: case OT_NATIVECLOSURE: v->Push(ss->_closure_default_delegate); break;
	case OT_THREAD: v->Push(ss->_thread_default_delegate); break;
	case OT_CLASS: v->Push(ss->_class_default_delegate); break;
	case OT_INSTANCE: v->Push(ss->_instance_default_delegate); break;
	case OT_WEAKREF: v->Push(ss->_weakref_default_delegate); break;
	default: return sq_throwerror(v,_SC("the type doesn't have a default delegate"));
	}
	return SQ_OK;
}

SQRESULT sq_next(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o=stack_get(v,idx),&refpos = stack_get(v,-1),realkey,val;
	if(sq_type(o) == OT_GENERATOR) {
		return sq_throwerror(v,_SC("cannot iterate a generator"));
	}
	int faketojump;
	if(!v->FOREACH_OP(o,realkey,val,refpos,0,666,faketojump))
		return SQ_ERROR;
	if(faketojump != 666) {
		v->Push(realkey);
		v->Push(val);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQInteger sq_strbuf_lexfeed(SQUserPointer file)
{
	SQStrBufState *buf=(SQStrBufState*)file;
	if(buf->size<(buf->ptr+1))
		return 0;
	return buf->buf[buf->ptr++];
}

SQRESULT sq_compilebuffer(HSQUIRRELVM v,const SQChar *s,SQInteger size,const SQChar *sourcename,
                          SQBool raiseerror, SQBool show_warnings, SQInteger max_nested_includes) {
	SQStrBufState buf;
	buf.buf = s;
	buf.size = size;
	buf.ptr = 0;
	return sq_compile(v, sq_strbuf_lexfeed, &buf, sourcename, raiseerror, show_warnings,
                   max_nested_includes);
}

void sq_move(HSQUIRRELVM dest,HSQUIRRELVM src,SQInteger idx)
{
	dest->Push(stack_get(src,idx));
}

void sq_setprintfunc(HSQUIRRELVM v, SQPRINTFUNCTION printfunc,SQPRINTFUNCTION errfunc)
{
	_ss(v)->_printfunc = printfunc;
	_ss(v)->_errorfunc = errfunc;
}

SQPRINTFUNCTION sq_getprintfunc(HSQUIRRELVM v)
{
	return _ss(v)->_printfunc;
}

SQPRINTFUNCTION sq_geterrorfunc(HSQUIRRELVM v)
{
	return _ss(v)->_errorfunc;
}

void *sq_malloc(SQUnsignedInteger size)
{
	return SQ_MALLOC(size);
}

void *sq_realloc(void* p,SQUnsignedInteger oldsize,SQUnsignedInteger newsize)
{
	return SQ_REALLOC(p,oldsize,newsize);
}

void sq_free(void *p,SQUnsignedInteger size)
{
	SQ_FREE(p,size);
}


SQRESULT sq_optinteger(HSQUIRRELVM sqvm, SQInteger idx, SQInteger *value, SQInteger default_value){
    if(sq_gettop(sqvm) >= idx){
        return sq_getinteger(sqvm, idx, value);
    }
    *value = default_value;
    return SQ_OK;
}

static SQRESULT sq_optstr_and_size(HSQUIRRELVM sqvm, SQInteger idx, const SQChar **value, const SQChar *dflt, SQInteger *size){
    if(sq_gettop(sqvm) >= idx){
        return sq_getstr_and_size(sqvm, idx, value, size);
    }
    *value = dflt;
    *size = scstrlen(dflt);
    return SQ_OK;
}

void sq_insertfunc(HSQUIRRELVM sqvm, const SQChar *fname, SQFUNCTION func,
                        SQInteger nparamscheck, const SQChar *typemask, SQBool isStatic){
    sq_pushstring(sqvm,fname,-1);
    sq_newclosure(sqvm,func,0);
    sq_setparamscheck(sqvm,nparamscheck,typemask);
    sq_setnativeclosurename(sqvm,-1,fname);
    sq_newslot(sqvm,-3,isStatic);
}

void sq_insert_reg_funcs(HSQUIRRELVM sqvm, SQRegFunction *obj_funcs){
	SQInteger i = 0;
	while(obj_funcs[i].name != 0) {
		SQRegFunction &f = obj_funcs[i];
		sq_pushstring(sqvm,f.name,-1);
		sq_newclosure(sqvm,f.f,0);
		sq_setparamscheck(sqvm,f.nparamscheck,f.typemask);
		sq_setnativeclosurename(sqvm,-1,f.name);
		sq_newslot(sqvm,-3,f.isStatic);
		i++;
	}
}

#define DONE_AND_RETURN(x) {ret_val =x; goto done_and_return;}
#define CHECK_OK(v) if((ret_val = v) < 0) goto done_and_return;

#define DONE_AND_RETURN(x) {ret_val =x; goto done_and_return;}
#define CHECK_OK(v) if((ret_val = v) < 0) goto done_and_return;

SQRESULT sq_call_va_vl(HSQUIRRELVM v, SQBool reset_stack, SQInteger idx, const SQChar *func_name, SQInteger idx_this, const SQChar *sig, va_list vl)
{
    int narg; // nres;  /* number of arguments and results */
    SQRESULT ret_val = 0;
    SQObjectType toptype;
    SQInteger top = sq_gettop(v);

    //if func_name not null assume global function name
    //else function already on stack
    if(func_name)
    {
        sq_pushstring(v, func_name, -1);
        CHECK_OK(sq_get(v, idx > 0 ? idx : idx-1));
        toptype = sq_gettype(v, -1);
        if(!(toptype == OT_CLOSURE || toptype == OT_NATIVECLOSURE)) DONE_AND_RETURN(-100);
    }

    if(idx_this == 0) sq_pushroottable(v);
    else sq_push(v, idx_this > 0 ? idx_this : idx_this-1);
    /* push arguments */
    narg = 1;
    while (*sig)    /* push arguments */
    {
        switch (*sig++)
        {
        case _SC('d'):  /* double argument */
            sq_pushfloat(v, va_arg(vl, double));
            break;

        case _SC('i'):  /* int argument */
            sq_pushinteger(v, va_arg(vl, int));
            break;

        case _SC('s'):  /* string argument */
            sq_pushstring(v, va_arg(vl, SQChar *), -1);
            break;

        case _SC('b'):  /* bool argument */
            sq_pushbool(v, va_arg(vl, int));
            break;

        case _SC('n'):  /* null argument */
            va_arg(vl, void *);
            sq_pushnull(v);
            break;

        case _SC('p'):  /* pointer argument */
            sq_pushuserpointer(v, va_arg(vl, void *));
            break;

        case _SC('>'):
            goto endwhile;

        default:
            DONE_AND_RETURN(-200);
        }
        narg++;
        //sq_checkstack(v, 1, "too many arguments");
    }
endwhile:

    /* do the call */
    //nres = strlen(sig);  /* number of expected results */
    if (sq_call(v, narg, *sig, SQTrue) != SQ_OK)  /* do the call */
        DONE_AND_RETURN(-300);

    /* retrieve results */
    //nres = -nres;  /* stack index of first result */
    if (*sig)    /* get results */
    {
        SQObjectPtr &o = stack_get(v,-1);
        switch (*sig)
        {

        case _SC('d'):  /* double result */
            if (!sq_isnumeric(o))  DONE_AND_RETURN(-1000);
            *va_arg(vl, double *) = tofloat(o);
            break;

        case _SC('i'):  /* int result */
            if (!sq_isnumeric(o))  DONE_AND_RETURN(-1100);
            *va_arg(vl, int *) = tointeger(o);
            break;
        //string do not work with stack reset
        case _SC('s'):  /* string result */
            if(reset_stack) DONE_AND_RETURN(-1250);
            if (!sq_isstring(o)) DONE_AND_RETURN(-1200);
            *va_arg(vl, const SQChar **) = _stringval(o);
            break;
        case _SC('b'):  /* bool result */
            if (!sq_isbool(o)) DONE_AND_RETURN(-1300);
            *va_arg(vl, int *) = tointeger(o);
            break;

        case _SC('p'):  /* user pointer result */
            if (!sq_isuserpointer(o)) DONE_AND_RETURN(-1400);
            *va_arg(vl, void **) = _userpointer(o);
            break;

        default:
            DONE_AND_RETURN(-500);
        }
        //nres++;
    }
done_and_return:
    if(reset_stack) sq_settop(v, top);
    return ret_val;
}

SQRESULT sq_call_va(HSQUIRRELVM v, SQBool reset_stack, SQInteger idx, const SQChar *func, SQInteger idx_this, const SQChar *sig, ...)
{
    va_list vl;
    va_start(vl, sig);
    SQRESULT ret_val = sq_call_va_vl(v, reset_stack, idx, func, idx_this, sig, vl);
    va_end(vl);
    return ret_val;
}

SQRESULT sq_checkoption (HSQUIRRELVM v, SQInteger narg, const SQChar *def,
                                 const SQChar *const lst[]) {
  const SQChar *name;
  if(def && sq_gettop(v) >= narg && sq_getstring(v, narg, &name) != SQ_OK) name = def;
  else if(sq_getstring(v, narg, &name) != SQ_OK) return SQ_ERROR;
  int i;
  for (i=0; lst[i]; i++)
    if (scstrcmp(lst[i], name) == 0)
      return i;
  return sq_throwerror(v, _SC("invalid option [%d] [%s]"), (int)narg-1, name);
}
