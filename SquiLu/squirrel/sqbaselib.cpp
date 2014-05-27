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
#include "sqclass.h"
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

SQ_OPT_STRING_STRLEN();

bool str2num(const SQChar *s,SQObjectPtr &res, SQInteger base=10)
{
	SQChar *end;
	const SQChar *e = s;
	SQBool isfloat = SQFalse;
	SQChar c;
	while((c = *e) != _SC('\0'))
	{
		if(c == _SC('.') || c == _SC('E')|| c == _SC('e')) { //e and E is for scientific notation
			isfloat = SQTrue;
			break;
		}
		e++;
	}
	if(isfloat){
		SQFloat r = SQFloat(scstrtod(s,&end));
		if(s == end) return false;
		res = r;
	}
	else{
		SQInteger r = SQInteger(scstrtol(s,&end, base));
		if(s == end) return false;
		res = r;
	}
	return true;
}

static SQRESULT base_dummy(HSQUIRRELVM v)
{
	return 0;
}

#ifndef NO_GARBAGE_COLLECTOR
static SQRESULT base_collectgarbage(HSQUIRRELVM v)
{
	sq_pushinteger(v, sq_collectgarbage(v));
	return 1;
}
static SQRESULT base_resurectureachable(HSQUIRRELVM v)
{
	sq_resurrectunreachable(v);
	return 1;
}
static SQRESULT base_getrefcount(HSQUIRRELVM v)
{
    SQObjectPtr &o=stack_get(v,2);
    if(!ISREFCOUNTED(type(o))) sq_pushinteger(v,0);
    else sq_pushinteger(v, o._unVal.pRefCounted->_uiRef - 1);
	return 1;
}
static SQRESULT base_check_delayed_release_hooks(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
	if(_top_ > 1){
		SQ_GET_BOOL(v, 2, bval);
		v->_check_delayed_relase_hooks = bval;
		return 0;
	}
    sq_pushbool(v, v->_check_delayed_relase_hooks);
	return 1;
}
static SQRESULT base_call_delayed_release_hooks(HSQUIRRELVM v)
{
    v->_sharedstate->CallDelayedReleaseHooks(v);
	return 0;
}
#endif

static SQRESULT base_getroottable(HSQUIRRELVM v)
{
	v->Push(v->_roottable);
	return 1;
}

static SQRESULT base_getconsttable(HSQUIRRELVM v)
{
	v->Push(_ss(v)->_consts);
	return 1;
}


static SQRESULT base_setroottable(HSQUIRRELVM v)
{
	SQObjectPtr o = v->_roottable;
	if(SQ_FAILED(sq_setroottable(v))) return SQ_ERROR;
	v->Push(o);
	return 1;
}

static SQRESULT base_setconsttable(HSQUIRRELVM v)
{
	SQObjectPtr o = _ss(v)->_consts;
	if(SQ_FAILED(sq_setconsttable(v))) return SQ_ERROR;
	v->Push(o);
	return 1;
}

static SQRESULT base_seterrorhandler(HSQUIRRELVM v)
{
	sq_seterrorhandler(v);
	return 0;
}

static SQRESULT base_geterrorhandler(HSQUIRRELVM v)
{
	return sq_geterrorhandler(v);
}

static SQRESULT base_setatexithandler(HSQUIRRELVM v)
{
	sq_setatexithandler(v);
	return 0;
}

static SQRESULT base_getatexithandler(HSQUIRRELVM v)
{
	return sq_getatexithandler(v);
}

static SQRESULT base_setdebughook(HSQUIRRELVM v)
{
	sq_setdebughook(v);
	return 0;
}

static SQRESULT base_enabledebuginfo(HSQUIRRELVM v)
{
	SQObjectPtr &o=stack_get(v,2);

	sq_enabledebuginfo(v,SQVM::IsFalse(o)?SQFalse:SQTrue);
	return 0;
}

static SQRESULT __getcallstackinfos(HSQUIRRELVM v,SQInteger level)
{
	SQStackInfos si;
	SQInteger seq = 0;
	const SQChar *name = NULL;

	if (SQ_SUCCEEDED(sq_stackinfos(v, level, &si)))
	{
		const SQChar *fn = _SC("unknown");
		const SQChar *src = _SC("unknown");
		if(si.funcname)fn = si.funcname;
		if(si.source)src = si.source;
		sq_newtable(v);
		sq_pushstring(v, _SC("func"), -1);
		sq_pushstring(v, fn, -1);
		sq_newslot(v, -3, SQFalse);
		sq_pushstring(v, _SC("src"), -1);
		sq_pushstring(v, src, -1);
		sq_newslot(v, -3, SQFalse);
		sq_pushstring(v, _SC("line"), -1);
		sq_pushinteger(v, si.line);
		sq_newslot(v, -3, SQFalse);
		sq_pushstring(v, _SC("locals"), -1);
		sq_newtable(v);
		seq=0;
		while ((name = sq_getlocal(v, level, seq))) {
			sq_pushstring(v, name, -1);
			sq_push(v, -2);
			sq_newslot(v, -4, SQFalse);
			sq_pop(v, 1);
			seq++;
		}
		sq_newslot(v, -3, SQFalse);
		return 1;
	}

	return 0;
}
static SQRESULT base_getstackinfos(HSQUIRRELVM v)
{
	SQInteger level;
	sq_getinteger(v, -1, &level);
	return __getcallstackinfos(v,level);
}

static SQRESULT base_getstacktop(HSQUIRRELVM v)
{
    sq_pushinteger(v, sq_getfulltop(v));
	return 1;
}

static SQRESULT base_gettypetag(HSQUIRRELVM v)
{
    SQUserPointer bklass = 0;
    if(sq_gettypetag(v, 2, &bklass) == SQ_OK && bklass){
        //lets see if it can be a string
        const SQChar *tag = (const SQChar*)bklass;
        int i=0;
        for(; i<32; ++i){
            if(!tag[i]) break;
        }
        if(i > 0 && i < 32) sq_pushstring(v, tag, i);
        else sq_pushuserpointer(v, bklass);
    }
    else sq_pushnull(v);
	return 1;
}

static SQRESULT base_getdefaultdelegate(HSQUIRRELVM v)
{
    if(sq_getdefaultdelegate(v, sq_gettype(v, 2)) != SQ_OK)
        sq_pushnull(v);
	return 1;
}

static SQRESULT base_assert(HSQUIRRELVM v)
{
	if(SQVM::IsFalse(stack_get(v,2))){
		return sq_throwerror(v,_SC("assertion failed"));
	}
	return 0;
}

static SQRESULT get_slice_params(HSQUIRRELVM v,SQInteger &sidx,SQInteger &eidx,SQObjectPtr &o)
{
	SQInteger top = sq_gettop(v);
	sidx=0;
	eidx=0;
	o=stack_get(v,1);
	SQObjectPtr &start=stack_get(v,2);
	if(type(start)!=OT_NULL && sq_isnumeric(start)){
		sidx=tointeger(start);
	}
	if(top>2){
		SQObjectPtr &end=stack_get(v,3);
		if(sq_isnumeric(end)){
			eidx=tointeger(end);
		}
	}
	else {
		eidx = sq_getsize(v,1);
	}
	return 1;
}

static SQRESULT base_print1(HSQUIRRELVM v)
{
	const SQChar *str;
	sq_tostring(v,2);
	sq_getstring(v,-1,&str);
	if(_ss(v)->_printfunc) _ss(v)->_printfunc(v,_SC("%s"),str);
	return 0;
}

static SQRESULT base_print(HSQUIRRELVM v)
{
    if(_ss(v)->_printfunc){
        SQPRINTFUNCTION sqprint = _ss(v)->_printfunc;
        const SQChar *str;
        SQInteger nargs=sq_gettop(v);
        for(int i=2; i<=nargs; ++i){
            if(i>2) sqprint(v,_SC("\t"));
            sq_tostring(v,i);
            sq_getstring(v,-1,&str);
            sqprint(v,_SC("%s"),str);
            sq_poptop(v); //remove converted string
        }
        sqprint(v,_SC("\n"));
    }
	return 0;
}

static SQRESULT base_error(HSQUIRRELVM v)
{
	const SQChar *str;
	sq_tostring(v,2);
	sq_getstring(v,-1,&str);
	if(_ss(v)->_errorfunc) _ss(v)->_errorfunc(v,_SC("%s"),str);
	return 0;
}

static SQRESULT base_get_last_error(HSQUIRRELVM v)
{
    sq_getlasterror(v);
	return 1;
}

static SQRESULT base_get_last_stackinfo(HSQUIRRELVM v)
{
    sq_getlaststackinfo(v);
	return 1;
}

static SQRESULT base_compilestring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, src);
    SQ_OPT_STRING(v, 3, name, _SC("unnamedbuffer"));
    SQ_OPT_BOOL(v, 4, show_wanings, SQTrue);
    if(SQ_SUCCEEDED(sq_compilebuffer(v,src,src_size,name,SQFalse, show_wanings)))
        return 1;
    else
        return SQ_ERROR;
}

static SQRESULT base_newthread(HSQUIRRELVM v)
{
	SQObjectPtr &func = stack_get(v,2);
	SQInteger stksize = (_closure(func)->_function->_stacksize << 1) +2;
	HSQUIRRELVM newv = sq_newthread(v, (stksize < MIN_STACK_OVERHEAD + 2)? MIN_STACK_OVERHEAD + 2 : stksize);
	sq_move(newv,v,-2);
	return 1;
}

static SQRESULT base_suspend(HSQUIRRELVM v)
{
	return sq_suspendvm(v);
}

static SQRESULT base_array(HSQUIRRELVM v)
{
	SQArray *a;
	SQObject &size = stack_get(v,2);
	if(sq_gettop(v) > 2) {
		a = SQArray::Create(_ss(v),0);
		a->Resize(tointeger(size),stack_get(v,3));
	}
	else {
		a = SQArray::Create(_ss(v),tointeger(size));
	}
	v->Push(a);
	return 1;
}

static SQRESULT base_type(HSQUIRRELVM v)
{
	SQObjectPtr &o = stack_get(v,2);
	v->Push(SQString::Create(_ss(v),GetTypeName(o),-1));
	return 1;
}

static SQRESULT base_callee(HSQUIRRELVM v)
{
	if(v->_callsstacksize > 1)
	{
		v->Push(v->_callsstack[v->_callsstacksize - 2]._closure);
		return 1;
	}
	return sq_throwerror(v,_SC("no closure in the calls stack"));
}


static SQRESULT base_str_from_chars (HSQUIRRELVM v) {
  SQ_FUNC_VARS_NO_TOP(v);
  SQInteger n = sq_gettop(v);  /* number of arguments */
  int i;
  SQChar *data = sq_getscratchpad(v, n);
  for (i=2; i<=n; ++i) {
    SQ_GET_INTEGER(v, i, c);
    if(uchar(c) != c){
        return sq_throwerror(v, _SC("invalid value for parameter %d"), i);
    }
    data[i-2] = uchar(c);
  }
  sq_pushstring(v, data, n-1);
  return 1;
}


static SQRegFunction base_funcs[]={
	//generic
	{_SC("setatexithandler"),base_setatexithandler,2, _SC(".c")},
	{_SC("getatexithandler"),base_getatexithandler,1, NULL},
	{_SC("seterrorhandler"),base_seterrorhandler,2, _SC(".c")},
	{_SC("geterrorhandler"),base_geterrorhandler,1, NULL},
	{_SC("setdebughook"),base_setdebughook,2, NULL},
	{_SC("enabledebuginfo"),base_enabledebuginfo,2, NULL},
	{_SC("getstackinfos"),base_getstackinfos,2, _SC(".n")},
	{_SC("getstacktop"),base_getstacktop,1, _SC(".")},
	{_SC("gettypetag"),base_gettypetag,2, _SC("..")},
	{_SC("getdefaultdelegate"),base_getdefaultdelegate,2, _SC("..")},
	{_SC("getroottable"),base_getroottable,1, NULL},
	{_SC("setroottable"),base_setroottable,2, NULL},
	{_SC("getconsttable"),base_getconsttable,1, NULL},
	{_SC("setconsttable"),base_setconsttable,2, NULL},
	{_SC("assert"),base_assert,2, NULL},
	{_SC("print1"),base_print1,2, NULL},
	{_SC("print"),base_print,-2, NULL},
	{_SC("error"),base_error,2, NULL},
	{_SC("get_last_error"),base_get_last_error,1, NULL},
	{_SC("get_last_stackinfo"),base_get_last_stackinfo,1, NULL},
	{_SC("compilestring"),base_compilestring,-2, _SC(".ssb")},
	{_SC("newthread"),base_newthread,2, _SC(".c")},
	{_SC("suspend"),base_suspend,-1, NULL},
	{_SC("array"),base_array,-2, _SC(".n")},
	{_SC("type"),base_type,2, NULL},
	{_SC("callee"),base_callee,0,NULL},
	{_SC("dummy"),base_dummy,0,NULL},
#ifndef NO_GARBAGE_COLLECTOR
	{_SC("collectgarbage"),base_collectgarbage,0, NULL},
	{_SC("resurrectunreachable"),base_resurectureachable,0, NULL},
	{_SC("getrefcount"),base_getrefcount,2, _SC("..")},
	{_SC("check_delayed_release_hooks"),base_check_delayed_release_hooks,-1, _SC(".b")},
	{_SC("call_delayed_release_hooks"),base_call_delayed_release_hooks,1, NULL},
#endif
	{_SC("str_from_chars"),base_str_from_chars,-2, _SC(".i")},
	{0,0}
};

void sq_base_register(HSQUIRRELVM v)
{
	SQInteger i=0;
	sq_pushroottable(v);
	while(base_funcs[i].name!=0) {
		sq_pushstring(v,base_funcs[i].name,-1);
		sq_newclosure(v,base_funcs[i].f,0);
		sq_setnativeclosurename(v,-1,base_funcs[i].name);
		sq_setparamscheck(v,base_funcs[i].nparamscheck,base_funcs[i].typemask);
		sq_newslot(v,-3, SQFalse);
		i++;
	}

	sq_pushstring(v,_SC("_versionnumber_"),-1);
	sq_pushinteger(v,SQUIRREL_VERSION_NUMBER);
	sq_newslot(v,-3, SQFalse);
	sq_pushstring(v,_SC("_version_"),-1);
	sq_pushstring(v,SQUIRREL_VERSION,-1);
	sq_newslot(v,-3, SQFalse);
	sq_pushstring(v,_SC("_charsize_"),-1);
	sq_pushinteger(v,sizeof(SQChar));
	sq_newslot(v,-3, SQFalse);
	sq_pushstring(v,_SC("_intsize_"),-1);
	sq_pushinteger(v,sizeof(SQInteger));
	sq_newslot(v,-3, SQFalse);
	sq_pushstring(v,_SC("_floatsize_"),-1);
	sq_pushinteger(v,sizeof(SQFloat));
	sq_newslot(v,-3, SQFalse);
	sq_pop(v,1);
}

static SQRESULT default_delegate_len(HSQUIRRELVM v)
{
	v->Push(SQInteger(sq_getsize(v,1)));
	return 1;
}

static SQRESULT default_delegate_tofloat(HSQUIRRELVM v)
{
	SQObjectPtr &o=stack_get(v,1);
	switch(type(o)){
	case OT_STRING:{
		SQObjectPtr res;
		if(str2num(_stringval(o),res)){
			v->Push(SQObjectPtr(tofloat(res)));
			break;
		}}
		return sq_throwerror(v, _SC("cannot convert the string"));
		break;
	case OT_INTEGER:case OT_FLOAT:
		v->Push(SQObjectPtr(tofloat(o)));
		break;
	case OT_BOOL:
		v->Push(SQObjectPtr((SQFloat)(_integer(o)?1:0)));
		break;
	default:
		v->PushNull();
		break;
	}
	return 1;
}

static SQRESULT default_delegate_tointeger(HSQUIRRELVM v)
{
	SQObjectPtr &o=stack_get(v,1);
	switch(type(o)){
	case OT_STRING:{
		SQObjectPtr res;
		SQInteger base;
		if(sq_gettop(v) > 1){
		    if(sq_getinteger(v, 2, &base) < 0) return sq_throwerror(v, _SC("parameter integer expected (2-36)"));
		    if(base < 2 || base > 36) return sq_throwerror(v, _SC("invalid base \"%d\" to tointeger (2-36)"), base);
		}
		else base = 10;
		if(str2num(_stringval(o),res, base)){
			v->Push(SQObjectPtr(tointeger(res)));
			break;
		}}
		return sq_throwerror(v, _SC("cannot convert the string"));
		break;
	case OT_INTEGER:case OT_FLOAT:
		v->Push(SQObjectPtr(tointeger(o)));
		break;
	case OT_BOOL:
		v->Push(SQObjectPtr(_integer(o)?(SQInteger)1:(SQInteger)0));
		break;
	default:
		v->PushNull();
		break;
	}
	return 1;
}

static SQRESULT default_delegate_tostring(HSQUIRRELVM v)
{
	sq_tostring(v,1);
	return 1;
}

static SQRESULT obj_delegate_weakref(HSQUIRRELVM v)
{
	sq_weakref(v,1);
	return 1;
}

static SQRESULT obj_clear(HSQUIRRELVM v)
{
	return sq_clear(v,-1);
}


static SQRESULT number_delegate_tochar(HSQUIRRELVM v)
{
	SQObject &o=stack_get(v,1);
	SQChar c = (SQChar)tointeger(o);
	v->Push(SQString::Create(_ss(v),(const SQChar *)&c,1));
	return 1;
}



/////////////////////////////////////////////////////////////////
//TABLE DEFAULT DELEGATE

static SQRESULT table_rawdelete(HSQUIRRELVM v)
{
	if(SQ_FAILED(sq_rawdeleteslot(v,1,SQTrue)))
		return SQ_ERROR;
	return 1;
}


static SQRESULT container_rawexists(HSQUIRRELVM v)
{
	sq_pushbool(v, sq_rawexists(v,-2));
	return 1;
}

static SQRESULT container_rawset(HSQUIRRELVM v)
{
	return sq_rawset(v,-3);
}

static SQRESULT container_rawget(HSQUIRRELVM v)
{
    switch(sq_gettop(v)){
        case 2: return SQ_SUCCEEDED(sq_rawget(v,-2))?1:SQ_ERROR;break;
        case 3: {
            sq_push(v, 2); //copy key to top
            sq_rawget(v,-4); //if it fail pop the key and default value is on top
            return 1;
        }
        break;
    }
    return sq_throwerror(v, _SC("invalid number of parameters"));
}

static SQRESULT container_get(HSQUIRRELVM v)
{
    switch(sq_gettop(v)){
        case 2: return SQ_SUCCEEDED(sq_get(v,-2))?1:SQ_ERROR;break;
        case 3: {
            sq_push(v, 2); //copy key to top
            sq_get(v,-4); //if it fail pop the key and default value is on top
            return 1;
        }
        break;
    }
    return sq_throwerror(v, _SC("invalid number of parameters"));
}

static SQRESULT table_setdelegate(HSQUIRRELVM v)
{
	if(SQ_FAILED(sq_setdelegate(v,-2)))
		return SQ_ERROR;
	sq_push(v,-1); // -1 because sq_setdelegate pops 1
	return 1;
}

static SQRESULT table_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdelegate(v,-1))?1:SQ_ERROR;
}

SQRegFunction SQSharedState::_table_default_delegate_funcz[]={
	{_SC("len"),default_delegate_len,1, _SC("t")},
	{_SC("size"),default_delegate_len,1, _SC("t")},
	{_SC("get"),container_get,-2, _SC("t")},
	{_SC("rawget"),container_rawget,-2, _SC("t")},
	{_SC("rawset"),container_rawset,3, _SC("t")},
	{_SC("rawdelete"),table_rawdelete,2, _SC("t")},
	{_SC("rawin"),container_rawexists,2, _SC("t")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("clear"),obj_clear,1, _SC(".")},
	{_SC("setdelegate"),table_setdelegate,2, _SC(".t|o")},
	{_SC("getdelegate"),table_getdelegate,1, _SC(".")},
	{0,0}
};

//ARRAY DEFAULT DELEGATE///////////////////////////////////////

static SQRESULT array_append(HSQUIRRELVM v)
{
	return sq_arrayappend(v,-2);
}

static SQRESULT array_extend(HSQUIRRELVM v)
{
	_array(stack_get(v,1))->Extend(_array(stack_get(v,2)));
	return 0;
}

static SQRESULT array_reverse(HSQUIRRELVM v)
{
	return sq_arrayreverse(v,-1);
}

static SQRESULT array_pop(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_arraypop(v,1,SQTrue))?1:SQ_ERROR;
}

static SQRESULT array_top(HSQUIRRELVM v)
{
	SQObject &o=stack_get(v,1);
	if(_array(o)->Size()>0){
		v->Push(_array(o)->Top());
		return 1;
	}
	else return sq_throwerror(v,_SC("top() on a empty array"));
}

static SQRESULT array_insert(HSQUIRRELVM v)
{
	SQObject &o=stack_get(v,1);
	SQObject &idx=stack_get(v,2);
	SQObject &val=stack_get(v,3);
	if(!_array(o)->Insert(tointeger(idx),val))
		return sq_throwerror(v,_SC("index out of range"));
	return 0;
}

static SQRESULT array_set(HSQUIRRELVM v)
{
	SQObject &o=stack_get(v,1);
	SQObject &idx=stack_get(v,2);
	SQObject &val=stack_get(v,3);
	if(!_array(o)->Set(tointeger(idx),val))
		return sq_throwerror(v,_SC("index out of range"));
	return 0;
}

static SQRESULT array_remove(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v, 1);
	SQObject &idx = stack_get(v, 2);
	if(!sq_isnumeric(idx)) return sq_throwerror(v, _SC("wrong type"));
	SQObjectPtr val;
	if(_array(o)->Get(tointeger(idx), val)) {
		_array(o)->Remove(tointeger(idx));
		v->Push(val);
		return 1;
	}
	return sq_throwerror(v, _SC("idx out of range"));
}

static SQRESULT array_resize(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v, 1);
	SQObject &nsize = stack_get(v, 2);
	SQObjectPtr fill;
	if(sq_isnumeric(nsize)) {
		if(sq_gettop(v) > 2)
			fill = stack_get(v, 3);
		_array(o)->Resize(tointeger(nsize),fill);
		return 0;
	}
	return sq_throwerror(v, _SC("size must be a number"));
}

static SQRESULT __map_array(SQArray *dest,SQArray *src,HSQUIRRELVM v) {
	SQObjectPtr temp;
	SQInteger size = src->Size();
	for(SQInteger n = 0; n < size; n++) {
		src->Get(n,temp);
		v->Push(src);
		v->Push(temp);
		if(SQ_FAILED(sq_call(v,2,SQTrue,SQFalse))) {
			return SQ_ERROR;
		}
		dest->Set(n,v->GetUp(-1));
		v->Pop();
	}
	return 0;
}

static SQRESULT array_map(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v,1);
	SQInteger size = _array(o)->Size();
	SQObjectPtr ret = SQArray::Create(_ss(v),size);
	if(SQ_FAILED(__map_array(_array(ret),_array(o),v)))
		return SQ_ERROR;
	v->Push(ret);
	return 1;
}

static SQRESULT array_apply(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v,1);
	if(SQ_FAILED(__map_array(_array(o),_array(o),v)))
		return SQ_ERROR;
	return 0;
}

static SQRESULT array_reduce(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v,1);
	SQArray *a = _array(o);
	SQInteger size = a->Size();
	if(size == 0) {
		return 0;
	}
	SQObjectPtr res;
	a->Get(0,res);
	if(size > 1) {
		SQObjectPtr other;
		for(SQInteger n = 1; n < size; n++) {
			a->Get(n,other);
			v->Push(o);
			v->Push(res);
			v->Push(other);
			if(SQ_FAILED(sq_call(v,3,SQTrue,SQFalse))) {
				return SQ_ERROR;
			}
			res = v->GetUp(-1);
			v->Pop();
		}
	}
	v->Push(res);
	return 1;
}

static SQRESULT array_filter(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v,1);
	SQArray *a = _array(o);
	SQObjectPtr ret = SQArray::Create(_ss(v),0);
	SQInteger size = a->Size();
	SQObjectPtr val;
	for(SQInteger n = 0; n < size; n++) {
		a->Get(n,val);
		v->Push(o);
		v->Push(n);
		v->Push(val);
		if(SQ_FAILED(sq_call(v,3,SQTrue,SQFalse))) {
			return SQ_ERROR;
		}
		if(!SQVM::IsFalse(v->GetUp(-1))) {
			_array(ret)->Append(val);
		}
		v->Pop();
	}
	v->Push(ret);
	return 1;
}

static SQRESULT array_find(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v,1);
	SQObjectPtr &val = stack_get(v,2);
	SQArray *a = _array(o);
	SQInteger size = a->Size();
	SQObjectPtr temp;
	for(SQInteger n = 0; n < size; n++) {
		a->Get(n,temp);
		if(SQVM::IsEqual(temp,val)) {
			v->Push(n);
			return 1;
		}
	}
	return 0;
}


static SQRESULT array_bsearch(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v,1);
	SQObjectPtr &val = stack_get(v,2);
	SQArray *a = _array(o);
	SQObjectPtr temp;
	SQInteger imid, imin = 0, imax = a->Size()-1;
	while(imax >= imin) {
	    /* calculate the midpoint for roughly equal partition */
        imid = (imin + imax) / 2;

        // determine which subarray to search
		SQInteger res = 0;
		a->Get(imid,temp);

		if(v->ObjCmp(temp,val,res)) {
            if(res <  0)
                // change min index to search upper subarray
                imin = imid + 1;
            else if(res > 0 )
                // change max index to search lower subarray
                imax = imid - 1;
            else{
                // key found at index imid
                sq_pushinteger(v, imid);
                return 1;
            }
		}
		else break;
	}
	sq_pushinteger(v, imid > 0 ? -imid : -1);
	return 1;
}


bool _sort_compare(HSQUIRRELVM v,SQObjectPtr &a,SQObjectPtr &b,SQInteger func,SQInteger &ret)
{
	if(func < 0) {
		if(!v->ObjCmp(a,b,ret)) return false;
	}
	else {
		SQInteger top = sq_gettop(v);
		sq_push(v, func);
		sq_pushroottable(v);
		v->Push(a);
		v->Push(b);
		if(SQ_FAILED(sq_call(v, 3, SQTrue, SQFalse))) {
			if(!sq_isstring( v->_lasterror))
				v->Raise_Error(_SC("compare func failed"));
			return false;
		}
		if(SQ_FAILED(sq_getinteger(v, -1, &ret))) {
			v->Raise_Error(_SC("numeric value expected as return value of the compare function"));
			return false;
		}
		sq_settop(v, top);
		return true;
	}
	return true;
}

bool _hsort_sift_down(HSQUIRRELVM v,SQArray *arr, SQInteger root, SQInteger bottom, SQInteger func)
{
	SQInteger maxChild;
	SQInteger done = 0;
	SQInteger ret;
	SQInteger root2;
	while (((root2 = root * 2) <= bottom) && (!done))
	{
		if (root2 == bottom) {
			maxChild = root2;
		}
		else {
			if(!_sort_compare(v,arr->_values[root2],arr->_values[root2 + 1],func,ret))
				return false;
			if (ret > 0) {
				maxChild = root2;
			}
			else {
				maxChild = root2 + 1;
			}
		}

		if(!_sort_compare(v,arr->_values[root],arr->_values[maxChild],func,ret))
			return false;
		if (ret < 0) {
			if (root == maxChild) {
                v->Raise_Error(_SC("inconsistent compare function"));
                return false; // We'd be swapping ourselve. The compare function is incorrect
            }

			_Swap(arr->_values[root],arr->_values[maxChild]);
			root = maxChild;
		}
		else {
			done = 1;
		}
	}
	return true;
}

bool _hsort(HSQUIRRELVM v,SQObjectPtr &arr, SQInteger l, SQInteger r,SQInteger func)
{
	SQArray *a = _array(arr);
	SQInteger i;
	SQInteger array_size = a->Size();
	for (i = (array_size / 2); i >= 0; i--) {
		if(!_hsort_sift_down(v,a, i, array_size - 1,func)) return false;
	}

	for (i = array_size-1; i >= 1; i--)
	{
		_Swap(a->_values[0],a->_values[i]);
		if(!_hsort_sift_down(v,a, 0, i-1,func)) return false;
	}
	return true;
}

static SQRESULT array_sort(HSQUIRRELVM v)
{
	SQInteger func = -1;
	SQObjectPtr &o = stack_get(v,1);
	if(_array(o)->Size() > 1) {
		if(sq_gettop(v) == 2) func = 2;
		if(!_hsort(v, o, 0, _array(o)->Size()-1, func))
			return SQ_ERROR;

	}
	return 0;
}

static SQRESULT array_slice(HSQUIRRELVM v)
{
	SQInteger sidx,eidx;
	SQObjectPtr o;
	if(get_slice_params(v,sidx,eidx,o)==-1)return -1;
	SQInteger alen = _array(o)->Size();
	if(sidx < 0)sidx = alen + sidx;
	if(eidx < 0)eidx = alen + eidx;
	if(eidx < sidx)return sq_throwerror(v,_SC("wrong indexes"));
	if(eidx > alen)return sq_throwerror(v,_SC("slice out of range"));
	SQArray *arr=SQArray::Create(_ss(v),eidx-sidx);
	SQObjectPtr t;
	SQInteger count=0;
	for(SQInteger i=sidx;i<eidx;i++){
		_array(o)->Get(i,t);
		arr->Set(count++,t);
	}
	v->Push(arr);
	return 1;

}

//DAD start
#include <sqstdio.h>
#include <sqstdblob.h>
#include "sqstdstream.h"
#include "sqstdblobimpl.h"

static SQRESULT array_concat0 (HSQUIRRELVM v, int allowAll) {
    SQ_FUNC_VARS(v);
    SQObjectPtr &arobj = stack_get(v,1);
    SQObjectPtrVec &aryvec = _array(arobj)->_values;
    SQInteger last = aryvec.size();
    if(last == 0){
        sq_pushstring(v, _SC(""), 0);
        return 1;
    }
    SQ_OPT_STRING(v, 2, sep, _SC(""));
    SQ_OPT_INTEGER(v, 3, i, 0);
    SQ_OPT_INTEGER(v, 4, opt_last, last);

  last = opt_last < last ? opt_last : last;
  opt_last = last -1;
  SQBlob blob(0, 8192);

  for (; i < last; ++i) {
      SQObjectPtr str, &o = aryvec[i];
      switch(type(o)){
          case OT_STRING:
              break;
          case OT_INTEGER:
          case OT_FLOAT:
          case OT_NULL:
              if(!v->ToString(o,str)) return SQ_ERROR;
              break;
          case OT_USERDATA:
            if(allowAll){
              if(!v->ToString(o,str)) return SQ_ERROR;
              break;
            }
          default:
              return sq_throwerror(v, _SC("Invalid type \"%s\" at position %d for array concat !"),
								GetTypeName(o), i);
      }

      const SQChar *value;
      SQInteger value_size;
      if(type(o) == OT_STRING) {
		value = _stringval(o);
		value_size = _string(o)->_len;
      }
      else
      {
		value = _stringval(str);
		value_size = _string(str)->_len;
      }
      blob.Write((void*)value, value_size);
      if(i != opt_last && sep_size) blob.Write((void*)sep, sep_size);
  }
  sq_pushstring(v, (SQChar*)blob.GetBuf(), blob.Len());
  return 1;
}

static SQRESULT array_concat (HSQUIRRELVM v) {
  return array_concat0(v, 0);
}

static SQRESULT array_concat2 (HSQUIRRELVM v) {
  return array_concat0(v, 1);
}

static SQRESULT array_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_ARRAY))?1:SQ_ERROR;
}

static SQRESULT array_empty(HSQUIRRELVM v)
{
	sq_pushbool(v,sq_getsize(v,1) == 0);
	return 1;
}

//DAD end

SQRegFunction SQSharedState::_array_default_delegate_funcz[]={
	{_SC("len"),default_delegate_len,1, _SC("a")},
	{_SC("size"),default_delegate_len,1, _SC("a")},
	{_SC("append"),array_append,2, _SC("a")},
	{_SC("extend"),array_extend,2, _SC("aa")},
	{_SC("push"),array_append,2, _SC("a")},
	{_SC("pop"),array_pop,1, _SC("a")},
	{_SC("top"),array_top,1, _SC("a")},
	{_SC("insert"),array_insert,3, _SC("an")},
	{_SC("remove"),array_remove,2, _SC("an")},
	{_SC("resize"),array_resize,-2, _SC("an")},
	{_SC("reverse"),array_reverse,1, _SC("a")},
	{_SC("sort"),array_sort,-1, _SC("ac")},
	{_SC("slice"),array_slice,-1, _SC("ann")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("clear"),obj_clear,1, _SC(".")},
	{_SC("map"),array_map,2, _SC("ac")},
	{_SC("apply"),array_apply,2, _SC("ac")},
	{_SC("reduce"),array_reduce,2, _SC("ac")},
	{_SC("filter"),array_filter,2, _SC("ac")},
	{_SC("find"),array_find,2, _SC("a.")},
	{_SC("bsearch"),array_bsearch,2, _SC("a.")},
	{_SC("concat"),array_concat,-1, _SC("as")},
	{_SC("concat2"),array_concat2,-1, _SC("as")},
	{_SC("getdelegate"),array_getdelegate,1, _SC(".")},
	{_SC("get"),container_rawget, -2, _SC("ai.")},
	{_SC("set"),array_set, 3, _SC("ai.")},
	{_SC("empty"),array_empty, 1, _SC("a")},
	{0,0}
};

//STRING DEFAULT DELEGATE//////////////////////////
static SQRESULT string_slice(HSQUIRRELVM v)
{
	SQInteger sidx,eidx;
	SQObjectPtr o;
	if(SQ_FAILED(get_slice_params(v,sidx,eidx,o)))return -1;
	SQInteger slen = _string(o)->_len;
	if(sidx < 0)sidx = slen + sidx;
	if(eidx < 0)eidx = slen + eidx;
	if(eidx < sidx)	return sq_throwerror(v,_SC("wrong indexes"));
	if(eidx > slen)	return sq_throwerror(v,_SC("slice out of range"));
	v->Push(SQString::Create(_ss(v),&_stringval(o)[sidx],eidx-sidx));
	return 1;
}

static SQRESULT string_substr(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_INTEGER(v, 2, start);
	SQObjectPtr &o = stack_get(v,1);
    SQInteger str_size = sq_getsize(v, 1);
    SQ_OPT_INTEGER(v, 3, len, str_size - start);
	if(start < 0)	return sq_throwerror(v,_SC("invalid start index %d"), start);
	if(len > (str_size - start))	return sq_throwerror(v,_SC("lenght out of range"));
	v->Push(SQString::Create(_ss(v),&_stringval(o)[start], len));
	return 1;
}

static SQRESULT string_find(HSQUIRRELVM v)
{
	SQInteger top,start_idx=0;
	const SQChar *str,*substr,*ret;
	if(((top=sq_gettop(v))>1) && SQ_SUCCEEDED(sq_getstring(v,1,&str)) && SQ_SUCCEEDED(sq_getstring(v,2,&substr))){
		if(top>2)sq_getinteger(v,3,&start_idx);
		if((sq_getsize(v,1)>start_idx) && (start_idx>=0)){
			ret=scstrstr(&str[start_idx],substr);
			if(ret){
				sq_pushinteger(v,(SQInteger)(ret-str));
				return 1;
			}
		}
		return 0;
	}
	return sq_throwerror(v,_SC("invalid param"));
}

#define STRING_TOFUNCZ(func) static SQRESULT string_##func(HSQUIRRELVM v) \
{ \
	SQObject str=stack_get(v,1); \
	SQInteger len=_string(str)->_len; \
	const SQChar *sThis=_stringval(str); \
	SQChar *sNew=(_ss(v)->GetScratchPad(rsl(len))); \
	for(SQInteger i=0;i<len;i++) sNew[i]=func(sThis[i]); \
	v->Push(SQString::Create(_ss(v),sNew,len)); \
	return 1; \
}


STRING_TOFUNCZ(tolower)
STRING_TOFUNCZ(toupper)

//DAD start
#include "lua-regex.h"

//on 64 bits there is an error SQRESULT/int
static int process_string_gsub(LuaMatchState *ms, void *udata, char_buffer_st **b) {
    const SQChar *str;
    SQInteger str_size;
    HSQUIRRELVM v = (HSQUIRRELVM)udata;
    SQObjectType rtype = sq_gettype(v, 3);
    SQInteger top = sq_gettop(v);
    SQInteger result = 1;
    switch(rtype){
        case OT_CLOSURE:{
            sq_push(v, 3); //push the function
            sq_pushroottable(v); //this
            int i=0;
            for(; i < ms->level; ++i){
                sq_pushstring(v, ms->capture[i].init, ms->capture[i].len);
            }
            int rc = sq_call(v, i+1, SQTrue, SQTrue);
            if(rc < 0) {
                ms->error = sq_getlasterror_str(v);
                return 0;
            }

            if(SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &str, &str_size))){
                if(!char_buffer_add_str(ms, b, str, str_size)) {
                    result = 0;
                    break;
                }
            }
        }
        break;
        case OT_ARRAY:{
            for(int i=0; i < ms->level; ++i){
                sq_pushinteger(v, i);
                if(SQ_SUCCEEDED(sq_get(v, 3)) && SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &str, &str_size))){
                    if(!char_buffer_add_str(ms, b, str, str_size)) {
                        result = 0;
                        break;
                    }
                    sq_pop(v, 1); //remove value
                }
            }
        }
        break;
        case OT_TABLE:{
            for(int i=0; i < ms->level; ++i){
                sq_pushstring(v, ms->capture[i].init, ms->capture[i].len);
                if(SQ_SUCCEEDED(sq_get(v, 3)) && SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &str, &str_size))){
                    if(!char_buffer_add_str(ms, b, str, str_size)) {
                        result = 0;
                        break;
                    }
                    sq_pop(v, 1); //remove value
                }
            }
        }
    }
    sq_settop(v, top); //restore the stack to it's original state
    return result; //returning non zero means continue
}

static SQRESULT string_gsub(HSQUIRRELVM v)
{
    const char *error_ptr;
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, src);
    SQ_GET_STRING(v, 2, pattern);
    SQ_OPT_INTEGER(v, 4, max_sub, 0);
    SQObjectType rtype = sq_gettype(v, 3);
    if(rtype == OT_STRING){
        SQ_GET_STRING(v, 3, replacement);
        char_buffer_st *buf = str_gsub (src, src_size, pattern, pattern_size,
                              replacement, replacement_size, max_sub, &error_ptr, 0, 0);
        if(buf){
            sq_pushstring(v, buf->buf, buf->used);
            free(buf);
            return 1;
        }
        return sq_throwerror(v,error_ptr);
    }
    else
    {
        switch(rtype){
            case OT_CLOSURE:
            case OT_ARRAY:
            case OT_TABLE:{
                char_buffer_st *buf = str_gsub (src, src_size, pattern, pattern_size,
                              0, 0, max_sub, &error_ptr, process_string_gsub, v);
                if(buf){
                    sq_pushstring(v, buf->buf, buf->used);
                    free(buf);
                    return 1;
                }
                return sq_throwerror(v,error_ptr);
            }
        }
    }
	return sq_throwerror(v,_SC("invalid type for parameter 3 function/table/array/string expected"));
}

static SQRESULT process_string_gmatch_find(LuaMatchState *ms, void *udata, char_buffer_st **b, bool isFind) {
    HSQUIRRELVM v = (HSQUIRRELVM)udata;
    SQInteger top = sq_gettop(v);
    SQInteger result = 1;
    int i=0;
    sq_push(v, 3); //push the function
    sq_pushroottable(v); //this en, function already on top of stack
    if(isFind){
        sq_pushinteger(v, ms->start_pos);
        sq_pushinteger(v, ms->end_pos);
    }
    for(; i < ms->level; ++i){
        sq_pushstring(v, ms->capture[i].init, ms->capture[i].len);
    }
    if(!isFind && i == 0){
        sq_pushstring(v, ms->src_init + ms->start_pos, ms->end_pos-ms->start_pos+1);
        i=1;
    }
    int rc = sq_call(v, i+1 + (isFind ? 2 : 0), SQTrue, SQTrue);
    if(rc < 0) {
        ms->error = sq_getlasterror_str(v);
        return 0;
    }
    SQObjectType rtype = sq_gettype(v, -1);
    if(rtype == OT_BOOL) {
        SQBool b;
        sq_getbool(v, -1, &b);
        result = b == SQTrue;
    }
    else result = rtype != OT_NULL;

    sq_settop(v, top); //restore the stack to it's original state
    return result; //returning non zero means continue
}

//on 64 bits there is an error SQRESULT/int
static int process_string_gmatch(LuaMatchState *ms, void *udata, char_buffer_st **b) {
    return process_string_gmatch_find(ms, udata, b, false);
}

static SQRESULT string_gmatch(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, src);
    SQ_GET_STRING(v, 2, pattern);
    LuaMatchState ms;
    memset(&ms, 0, sizeof(ms));

    if(_top_ > 2){
        SQInteger rtype = sq_gettype(v, 3);
        if(rtype == OT_CLOSURE){
            _rc_ = str_match(&ms, src, src_size, pattern, pattern_size,
                    0, 0, process_string_gmatch, v);
            if(ms.error) return sq_throwerror(v, ms.error);
            sq_pushinteger(v, _rc_);
            return 1;
        }
        return sq_throwerror(v,_SC("invalid type for parameter 3 function expected"));
    }
    _rc_ = str_match(&ms, src, src_size, pattern, pattern_size,
            0, 0, 0, 0);
    if(ms.error) return sq_throwerror(v, ms.error);
    if(ms.level){
        if(ms.level == 1) sq_pushstring(v, ms.capture[0].init, ms.capture[0].len);
        else {
            sq_newarray(v, ms.level);
            for(int i=0; i < ms.level; ++i){
                sq_pushinteger(v, i);
                sq_pushstring(v, ms.capture[i].init, ms.capture[i].len);
                sq_rawset(v, -3);
            }
        }
    } else {
        if(ms.end_pos > ms.start_pos) sq_pushstring(v, src + ms.start_pos, ms.end_pos-ms.start_pos+1);
        else sq_pushnull(v);
    }
    return 1;
}

static int process_string_find_lua(LuaMatchState *ms, void *udata, char_buffer_st **b) {
    return process_string_gmatch_find(ms, udata, b, true);
}

static SQRESULT string_find_lua(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, src);
    SQ_GET_STRING(v, 2, pattern);
    SQ_OPT_INTEGER(v, 4, start, 0);
    SQ_OPT_BOOL(v, 5, raw, SQFalse);
    SQInteger rtype = sq_gettype(v, 3);

    if(_top_ == 2){
        //only want to know if it exists
        LuaMatchState ms;
        memset(&ms, 0, sizeof(ms));
        int rc = str_find(&ms, src, src_size, pattern, pattern_size,
                start, raw == SQTrue, 0, 0);
        if(ms.error) return sq_throwerror(v, ms.error);
        sq_pushinteger(v, rc);
        return 1;
    }
    if(rtype == OT_CLOSURE){
        LuaMatchState ms;
        memset(&ms, 0, sizeof(ms));
        int rc = str_find(&ms, src, src_size, pattern, pattern_size,
                start, raw == SQTrue, process_string_find_lua, v);
        if(ms.error) return sq_throwerror(v, ms.error);
        sq_pushinteger(v, rc);
        return 1;
    }
    else if(rtype == OT_TABLE || rtype == OT_ARRAY){
        LuaMatchState ms;
        memset(&ms, 0, sizeof(ms));
        int rc = str_find(&ms, src, src_size, pattern, pattern_size,
                start, raw == SQTrue, 0, 0);
        if(ms.error) return sq_throwerror(v, ms.error);
        if(rtype == OT_TABLE){
            sq_pushstring(v, _SC("start_pos"), -1);
            sq_pushinteger(v, ms.start_pos);
            sq_rawset(v, 3);
            sq_pushstring(v, _SC("end_pos"), -1);
            sq_pushinteger(v, ms.end_pos);
            sq_rawset(v, 3);
        }
        else if(rtype == OT_ARRAY)
        {
            sq_pushinteger(v, 0);
            sq_pushinteger(v, ms.start_pos);
            sq_rawset(v, 3);
            sq_pushinteger(v, 1);
            sq_pushinteger(v, ms.end_pos);
            sq_rawset(v, 3);
        }
        sq_pushinteger(v, rc);
        return 1;
    }
	return sq_throwerror(v,_SC("invalid type for parameter 3 function expected"));
}

static const SQChar *lmemfind (const SQChar *s1, size_t l1,
                               const SQChar *s2, size_t l2) {
  if (l2 == 0) return s1;  /* empty strings are everywhere */
  else if (l2 > l1) return NULL;  /* avoids a negative `l1' */
  else {
    const SQChar *init;  /* to search for a `*s2' inside `s1' */
    l2--;  /* 1st char will be checked by `memchr' */
    l1 = l1-l2;  /* `s2' cannot be found after that */
    while (l1 > 0 && (init = (const SQChar *)memchr(s1, *s2, l1)) != NULL) {
      init++;   /* 1st char is already checked */
      if (memcmp(init, s2+1, l2) == 0)
        return init-1;
      else {  /* correct `l1' and `s1' to try again */
        l1 -= init-s1;
        s1 = init;
      }
    }
    return NULL;  /* not found */
  }
}

/*DAD */
static SQRESULT string_replace(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, src);
    SQ_GET_STRING(v, 2, p);
    SQ_GET_STRING(v, 3, p2);
    SQ_OPT_INTEGER(v, 4, count, 0);
    const SQChar *s2;
    int n = 0;
    int init = 0;

    SQBlob b(0, 8192);

    while (1) {
        s2 = lmemfind(src+init, src_size-init, p, p_size);
        if (s2) {
            b.Write(src+init, s2-(src+init));
            b.Write(p2, p2_size);
            init = init + (s2-(src+init)) + p_size;
            n++;
            if(count && (n >= count)) {
                b.Write(src+init, src_size-init);
                break;
            }
        } else {
            b.Write(src+init, src_size-init);
            break;
        }
    }
    sq_pushstring(v, (const SQChar*)b.GetBuf(), b.Len());
    return 1;
}

static SQRESULT string_endswith(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQ_GET_STRING(v, 2, token);

    SQInteger ti = token_size, si = str_size;
    SQBool end = SQTrue;
    if(token_size <= str_size){
        while(ti > 0) {
            if(str[--si] != token[--ti]){
                end = SQFalse;
                break;

            }
        }
    }
    else {
        end = SQFalse;
    }
    sq_pushbool(v, end);
    return 1;
}

static SQRESULT string_startswith(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQ_GET_STRING(v, 2, token);

    int i;
    SQBool start = SQTrue;
    // please make this less ugly...
    if(token_size <= str_size){
    	for(i = 0; i < token_size; ++i) {
            if(str[i] != token[i]){
                start = SQFalse;
                break;
            }
        }
    }
    else {
        start = SQFalse;
    }
    sq_pushbool(v, start);
    return 1;
}

static SQRESULT string_find_close_quote(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, src);
    SQ_OPT_INTEGER(v, 2, init, 0);
    if(init >= src_size) return sq_throwerror(v, _SC("invalid start position"));

    for(; init < src_size; ++init) {
        if(src[init] == '"'){
             if(src[init+1] == '"') ++init; //skip quoted quote
             else break;
        }
    }
    if(src[init] != '"') init = -1;
    sq_pushinteger(v, init);
    return 1;
}

static SQRESULT string_find_delimiter(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, src);
    SQ_GET_INTEGER(v, 2, delimiter);
    SQ_GET_INTEGER(v, 3, escape_char);
    SQ_GET_INTEGER(v, 4, init);
    if(init >= src_size) return sq_throwerror(v, _SC("invalid start position"));

    for(; init < src_size; ++init) {
        if(src[init] == delimiter){
             if(src[init-1] == escape_char) ++init; //skip quoted quote
             else break;
        }
    }
    if(src[init] != delimiter) init = -1;
    sq_pushinteger(v, init);
    return 1;
}

static SQRESULT string_reverse (HSQUIRRELVM v) {
  int i;
  SQ_FUNC_VARS_NO_TOP(v);
  SQ_GET_STRING(v, 1, s)
  SQChar *data = sq_getscratchpad(v,s_size);
  --s_size;
  for(i=0; i<=s_size ; ++i){
      data[i] = s[s_size-i];
  }
  sq_pushstring(v, data, s_size+1);
  return 1;
}


static SQRESULT string_rep (HSQUIRRELVM v) {
  int i;
  SQ_FUNC_VARS_NO_TOP(v);
  SQ_GET_STRING(v, 1, s)
  SQ_GET_INTEGER(v, 2, n);
  SQInteger nsize = n*s_size;
  SQChar *data = sq_getscratchpad(v, nsize);
  for(i=0; i<n ; ++i){
      memcpy(data+(i*s_size), s, s_size);
  }
  sq_pushstring(v, data, nsize);
  return 1;
}

static SQRESULT string_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_STRING))?1:SQ_ERROR;
}

//DAD end

static void __strip_l(const SQChar *str,const SQChar **start)
{
	const SQChar *t = str;
	while(((*t) != '\0') && scisspace(*t)){ t++; }
	*start = t;
}

static void __strip_r(const SQChar *str,SQInteger len,const SQChar **end)
{
	if(len == 0) {
		*end = str;
		return;
	}
	const SQChar *t = &str[len-1];
	while(t >= str && scisspace(*t)) { t--; }
	*end = t + 1;
}

static SQRESULT string_strip(HSQUIRRELVM v)
{
	const SQChar *str,*start,*end;
	sq_getstring(v,1,&str);
	SQInteger len = sq_getsize(v,1);
	__strip_l(str,&start);
	__strip_r(str,len,&end);
	sq_pushstring(v,start,end - start);
	return 1;
}

static SQRESULT string_lstrip(HSQUIRRELVM v)
{
	const SQChar *str,*start;
	sq_getstring(v,1,&str);
	__strip_l(str,&start);
	sq_pushstring(v,start,-1);
	return 1;
}

static SQRESULT string_rstrip(HSQUIRRELVM v)
{
	const SQChar *str,*end;
	sq_getstring(v,1,&str);
	SQInteger len = sq_getsize(v,1);
	__strip_r(str,len,&end);
	sq_pushstring(v,str,end - str);
	return 1;
}

static SQRESULT string_split_by_strtok(HSQUIRRELVM v)
{
	const SQChar *str,*seps;
	SQChar *stemp,*tok;
	sq_getstring(v,1,&str);
	sq_getstring(v,2,&seps);
	if(sq_getsize(v,2) == 0) return sq_throwerror(v,_SC("empty separators string"));
	SQInteger memsize = (sq_getsize(v,1)+1)*sizeof(SQChar);
	stemp = sq_getscratchpad(v,memsize);
	memcpy(stemp,str,memsize);
	tok = scstrtok(stemp,seps);
	sq_newarray(v,0);
	while( tok != NULL ) {
		sq_pushstring(v,tok,-1);
		sq_arrayappend(v,-2);
		tok = scstrtok( NULL, seps );
	}
	return 1;
}

static SQRESULT string_split(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQ_GET_INTEGER(v, 2, sep);
    const SQChar *token;
    sq_newarray(v,0);
    while ((token = scstrchr(str, sep)) != NULL) {
        sq_pushstring(v, str, token - str);
        sq_arrayappend(v, -2);
        str = token + 1;
    }
    if(*str){ //there is anything left ?
        sq_pushstring(v, str, -1);
        sq_arrayappend(v, -2);
    } else if( str_size && (*(str-1) == sep) ){ //last empty column ?
        sq_pushstring(v, _SC(""), 0);
        sq_arrayappend(v, -2);
    }
    return 1;
}

static SQRESULT string_empty(HSQUIRRELVM v)
{
	sq_pushbool(v,sq_getsize(v,1) == 0);
	return 1;
}

static SQRESULT string_isalpha(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQ_GET_INTEGER(v, 2, idx);
    if(idx >= str_size) {
        return sq_throwerror(v, _SC("index %d out of range"), idx);
    }
	sq_pushbool(v, scisalpha(str[idx]));
	return 1;
}

static SQRESULT string_isdigit(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQ_GET_INTEGER(v, 2, idx);
    if(idx >= str_size) {
        return sq_throwerror(v, _SC("index %d out of range"), idx);
    }
	sq_pushbool(v, scisdigit(str[idx]));
	return 1;
}

#ifdef SQ_SUBLATIN
#include "sublatin.h"

static SQRESULT string_sl_len (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    sq_pushinteger(v, strLenSubSetLatinUtf8(str));
    return 1;
}

static SQRESULT string_sl_lower (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQInteger size = str_size+sizeof(SQChar); //'\0' terminator
    SQChar *s = sq_getscratchpad(v, size);
    memcpy(s, str, size);
    toLowerSubSetLatinUtf8(s);
    sq_pushstring(v, s, -1);
    return 1;
}


static SQRESULT string_sl_upper (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQInteger size = str_size+sizeof(SQChar); //'\0' terminator
    SQChar *s = sq_getscratchpad(v, size);
    memcpy(s, str, size);
    toUpperSubSetLatinUtf8(s);
    sq_pushstring(v, s, -1);
    return 1;
}

static SQRESULT string_sl_deaccent (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQInteger size = str_size+sizeof(SQChar); //'\0' terminator
    SQChar *s = sq_getscratchpad(v, size);
    memcpy(s, str, size);
    deAccentSubSetLatinUtf8(s);
    sq_pushstring(v, s, -1);
    return 1;
}

static SQRESULT string_sl_lower_deaccent (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, str);
    SQInteger size = str_size+sizeof(SQChar); //'\0' terminator
    SQChar *s = sq_getscratchpad(v, size);
    memcpy(s, str, size);
    toLowerDeaccentSubSetLatinUtf8(s);
    sq_pushstring(v, s, -1);
    return 1;
}

static SQRESULT string_sl_icmp (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, sl);
    SQ_GET_STRING(v, 2, sr);
    sq_pushinteger(v, strICmpSubSetLatinUtf8(sl, sr));
    return 1;
}

static SQRESULT string_sl_icmp_noaccents (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, sl);
    SQ_GET_STRING(v, 2, sr);
    sq_pushinteger(v, strICmpSubSetLatinUtf8NoAccents(sl, sr));
    return 1;
}

static SQRESULT string_sl_cmp_noaccents (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 1, sl);
    SQ_GET_STRING(v, 2, sr);
    sq_pushinteger(v, strCmpSubSetLatinUtf8NoAccents(sl, sr));
    return 1;
}

static SQRESULT string_sl_like_cmp (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, sl);
    SQ_GET_STRING(v, 2, sr);
    SQ_OPT_INTEGER(v, 3, s_esc, _SC('%'));
    sq_pushbool(v, subLatinLikeCompare(sl, sr, s_esc) == 1);
    return 1;
}

static SQRESULT string_sl_like_cmp_noaccents (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 1, sl);
    SQ_GET_STRING(v, 2, sr);
    SQ_OPT_INTEGER(v, 3, s_esc, _SC('%'));
    sq_pushbool(v, subLatinLikeCompareNoAccents(sl, sr, s_esc) == 1);
    return 1;
}

#endif

SQRegFunction SQSharedState::_string_default_delegate_funcz[]={
	{_SC("len"),default_delegate_len,1, _SC("s")},
	{_SC("size"),default_delegate_len,1, _SC("s")},
	{_SC("tointeger"),default_delegate_tointeger,-1, _SC("sn")},
	{_SC("tofloat"),default_delegate_tofloat,1, _SC("s")},
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("slice"),string_slice,-1, _SC(" s n  n")},
	{_SC("substr"),string_substr,-2, _SC(" s n  n")},
	{_SC("replace"),string_replace,-3, _SC("sssi")},
	{_SC("find"),string_find,-2, _SC("s s n ")},
	{_SC("find_lua"),string_find_lua,-2, _SC("ss a|t|c nb")},
	{_SC("find_close_quote"),string_find_close_quote,-1, _SC("sn")},
	{_SC("find_delimiter"),string_find_delimiter,4, _SC("siin")},
	{_SC("gsub"),string_gsub,-3, _SC("s s s|a|t|c n")},
	{_SC("gmatch"),string_gmatch, 3, _SC("s s c")},
	{_SC("match"), string_gmatch, 2, _SC("s s")},
	{_SC("startswith"),string_startswith, 2, _SC("ss")},
	{_SC("endswith"),string_endswith, 2, _SC("ss")},
	{_SC("reverse"),string_reverse, 2, _SC("ss")},
	{_SC("rep"),string_rep, 2, _SC("si")},
	{_SC("tolower"),string_tolower,1, _SC("s")},
	{_SC("toupper"),string_toupper,1, _SC("s")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("getdelegate"),string_getdelegate,1, _SC(".")},
	{_SC("strip"),string_strip,1, _SC("s")},
	{_SC("lstrip"),string_lstrip,1, _SC("s")},
	{_SC("rstrip"),string_rstrip,1, _SC("s")},
	{_SC("split"),string_split,2, _SC("si")},
	{_SC("split_by_strtok"),string_split_by_strtok,2, _SC("ss")},
	{_SC("empty"),string_empty,1, _SC("s")},
	{_SC("isalpha"),string_isalpha,2, _SC("si")},
	{_SC("isdigit"),string_isdigit,2, _SC("si")},
#ifdef SQ_SUBLATIN
	{_SC("sl_len"),string_sl_len,1, _SC("s")},
	{_SC("sl_lower"),string_sl_lower,1, _SC("s")},
	{_SC("sl_upper"),string_sl_upper,1, _SC("s")},
	{_SC("sl_deaccent"),string_sl_deaccent,1, _SC("s")},
	{_SC("sl_lower_deaccent"),string_sl_lower_deaccent,1, _SC("s")},
	{_SC("sl_icmp"),string_sl_icmp,2, _SC("ss")},
	{_SC("sl_icmp_noaccents"),string_sl_icmp_noaccents,2, _SC("ss")},
	{_SC("sl_cmp_noaccents"),string_sl_cmp_noaccents, 2, _SC("ss")},
	{_SC("sl_like_cmp"),string_sl_like_cmp, -2, _SC("ssi")},
	{_SC("sl_like_cmp_noaccents"),string_sl_like_cmp_noaccents, -2, _SC("ssi")},
#endif
	{0,0}
};

//NUMBER DEFAULT DELEGATE//////////////////////////
static SQRESULT number_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_INTEGER))?1:SQ_ERROR;
}

SQRegFunction SQSharedState::_number_default_delegate_funcz[]={
	{_SC("tointeger"),default_delegate_tointeger,1, _SC("n|b")},
	{_SC("tofloat"),default_delegate_tofloat,1, _SC("n|b")},
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("tochar"),number_delegate_tochar,1, _SC("n|b")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("getdelegate"),number_getdelegate,1, _SC(".")},
	{0,0}
};

//CLOSURE DEFAULT DELEGATE//////////////////////////
static SQRESULT closure_pcall(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_call(v,sq_gettop(v)-1,SQTrue,SQFalse))?1:SQ_ERROR;
}

static SQRESULT closure_call(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_call(v,sq_gettop(v)-1,SQTrue,SQTrue))?1:SQ_ERROR;
}

static SQRESULT _closure_acall(HSQUIRRELVM v,SQBool raiseerror, SQBool v2)
{
	SQArray *aparams=_array(stack_get(v, v2 ? 3 : 2));
	SQInteger nparams=aparams->Size();
	v->Push(stack_get(v,1));
	if(v2) v->Push(stack_get(v,2));
	for(SQInteger i=0;i<nparams;i++)v->Push(aparams->_values[i]);
	return SQ_SUCCEEDED(sq_call(v,nparams + (v2 ? 1 : 0),SQTrue,raiseerror))?1:SQ_ERROR;
}

static SQRESULT closure_acall(HSQUIRRELVM v)
{
	return _closure_acall(v,SQTrue, SQFalse);
}

static SQRESULT closure_acall2(HSQUIRRELVM v)
{
	return _closure_acall(v,SQTrue, SQTrue);
}

static SQRESULT closure_pacall(HSQUIRRELVM v)
{
	return _closure_acall(v,SQFalse, SQFalse);
}

static SQRESULT closure_bindenv(HSQUIRRELVM v)
{
	if(SQ_FAILED(sq_bindenv(v,1)))
		return SQ_ERROR;
	return 1;
}

static SQRESULT closure_setenv(HSQUIRRELVM v)
{
	if(SQ_FAILED(sq_setfenv(v,1, SQFalse)))
		return SQ_ERROR;
	return 0;
}
static SQRESULT closure_getenv(HSQUIRRELVM v)
{
	if(SQ_FAILED(sq_getfenv(v,1, SQFalse)))
		return SQ_ERROR;
	return 1;
}

static SQRESULT closure_getinfos(HSQUIRRELVM v) {
	SQObject o = stack_get(v,1);
	SQTable *res = SQTable::Create(_ss(v),4);
	if(type(o) == OT_CLOSURE) {
		SQFunctionProto *f = _closure(o)->_function;
		SQInteger nparams = f->_nparameters + (f->_varparams?1:0);
		SQObjectPtr params = SQArray::Create(_ss(v),nparams);
    SQObjectPtr defparams = SQArray::Create(_ss(v),f->_ndefaultparams);
		for(SQInteger n = 0; n<f->_nparameters; n++) {
			_array(params)->Set((SQInteger)n,f->_parameters[n]);
		}
    for(SQInteger j = 0; j<f->_ndefaultparams; j++) {
			_array(defparams)->Set((SQInteger)j,_closure(o)->_defaultparams[j]);
		}
		if(f->_varparams) {
			_array(params)->Set(nparams-1,SQString::Create(_ss(v),_SC("..."),-1));
		}
		res->NewSlot(SQString::Create(_ss(v),_SC("native"),-1),false);
		res->NewSlot(SQString::Create(_ss(v),_SC("name"),-1),f->_name);
		res->NewSlot(SQString::Create(_ss(v),_SC("src"),-1),f->_sourcename);
		res->NewSlot(SQString::Create(_ss(v),_SC("parameters"),-1),params);
		res->NewSlot(SQString::Create(_ss(v),_SC("varargs"),-1),f->_varparams);
    res->NewSlot(SQString::Create(_ss(v),_SC("defparams"),-1),defparams);
	}
	else { //OT_NATIVECLOSURE
		SQNativeClosure *nc = _nativeclosure(o);
		res->NewSlot(SQString::Create(_ss(v),_SC("native"),-1),true);
		res->NewSlot(SQString::Create(_ss(v),_SC("name"),-1),nc->_name);
		res->NewSlot(SQString::Create(_ss(v),_SC("paramscheck"),-1),nc->_nparamscheck);
		SQObjectPtr typecheck;
		if(nc->_typecheck.size() > 0) {
			typecheck =
				SQArray::Create(_ss(v), nc->_typecheck.size());
			for(SQUnsignedInteger n = 0; n<nc->_typecheck.size(); n++) {
					_array(typecheck)->Set((SQInteger)n,nc->_typecheck[n]);
			}
		}
		res->NewSlot(SQString::Create(_ss(v),_SC("typecheck"),-1),typecheck);
	}
	v->Push(res);
	return 1;
}

//CLOSURE DEFAULT DELEGATE//////////////////////////
static SQRESULT closure_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_CLOSURE))?1:SQ_ERROR;
}


SQRegFunction SQSharedState::_closure_default_delegate_funcz[]={
	{_SC("call"),closure_call,-1, _SC("c")},
	{_SC("pcall"),closure_pcall,-1, _SC("c")},
	{_SC("acall"),closure_acall,2, _SC("ca")},
	{_SC("acall2"),closure_acall2,3, _SC("c.a")},
	{_SC("pacall"),closure_pacall,2, _SC("ca")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("bindenv"),closure_bindenv,2, _SC("c x|y|t|a")},
	{_SC("setenv"),closure_setenv,2, _SC("c x|y|t")},
	{_SC("getenv"),closure_getenv,1, _SC("c")},
	{_SC("getinfos"),closure_getinfos,1, _SC("c")},
	{_SC("getdelegate"),closure_getdelegate,1, _SC(".")},
	{0,0}
};

//GENERATOR DEFAULT DELEGATE
static SQRESULT generator_getstatus(HSQUIRRELVM v)
{
	SQObject &o=stack_get(v,1);
	switch(_generator(o)->_state){
		case SQGenerator::eSuspended:v->Push(SQString::Create(_ss(v),_SC("suspended")));break;
		case SQGenerator::eRunning:v->Push(SQString::Create(_ss(v),_SC("running")));break;
		case SQGenerator::eDead:v->Push(SQString::Create(_ss(v),_SC("dead")));break;
	}
	return 1;
}

//GENERATOR DEFAULT DELEGATE//////////////////////////
static SQRESULT generator_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_GENERATOR))?1:SQ_ERROR;
}

SQRegFunction SQSharedState::_generator_default_delegate_funcz[]={
	{_SC("getstatus"),generator_getstatus,1, _SC("g")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("getdelegate"),generator_getdelegate,1, _SC(".")},
	{0,0}
};

//THREAD DEFAULT DELEGATE
static SQRESULT thread_call(HSQUIRRELVM v)
{
	SQObjectPtr o = stack_get(v,1);
	if(type(o) == OT_THREAD) {
		SQInteger nparams = sq_gettop(v);
		_thread(o)->Push(_thread(o)->_roottable);
		for(SQInteger i = 2; i<(nparams+1); i++)
			sq_move(_thread(o),v,i);
		if(SQ_SUCCEEDED(sq_call(_thread(o),nparams,SQTrue,SQTrue))) {
			sq_move(v,_thread(o),-1);
			sq_pop(_thread(o),1);
			return 1;
		}
		v->_lasterror = _thread(o)->_lasterror;
		return SQ_ERROR;
	}
	return sq_throwerror(v,_SC("wrong parameter"));
}

static SQRESULT thread_wakeup(HSQUIRRELVM v)
{
	SQObjectPtr o = stack_get(v,1);
	if(type(o) == OT_THREAD) {
		SQVM *thread = _thread(o);
		SQInteger state = sq_getvmstate(thread);
		if(state != SQ_VMSTATE_SUSPENDED) {
			switch(state) {
				case SQ_VMSTATE_IDLE:
					return sq_throwerror(v,_SC("cannot wakeup a idle thread"));
				break;
				case SQ_VMSTATE_RUNNING:
					return sq_throwerror(v,_SC("cannot wakeup a running thread"));
				break;
			}
		}

		SQInteger wakeupret = sq_gettop(v)>1?1:0;
		if(wakeupret) {
			sq_move(thread,v,2);
		}
		if(SQ_SUCCEEDED(sq_wakeupvm(thread,wakeupret,SQTrue,SQTrue,SQFalse))) {
			sq_move(v,thread,-1);
			sq_pop(thread,1); //pop retval
			if(sq_getvmstate(thread) == SQ_VMSTATE_IDLE) {
				sq_settop(thread,1); //pop roottable
			}
			return 1;
		}
		sq_settop(thread,1);
		v->_lasterror = thread->_lasterror;
		return SQ_ERROR;
	}
	return sq_throwerror(v,_SC("wrong parameter"));
}

static SQRESULT thread_getstatus(HSQUIRRELVM v)
{
	SQObjectPtr &o = stack_get(v,1);
	switch(sq_getvmstate(_thread(o))) {
		case SQ_VMSTATE_IDLE:
			sq_pushstring(v,_SC("idle"),-1);
		break;
		case SQ_VMSTATE_RUNNING:
			sq_pushstring(v,_SC("running"),-1);
		break;
		case SQ_VMSTATE_SUSPENDED:
			sq_pushstring(v,_SC("suspended"),-1);
		break;
		default:
			return sq_throwerror(v,_SC("internal VM error"));
	}
	return 1;
}

static SQRESULT thread_getstackinfos(HSQUIRRELVM v)
{
	SQObjectPtr o = stack_get(v,1);
	if(type(o) == OT_THREAD) {
		SQVM *thread = _thread(o);
		SQInteger threadtop = sq_gettop(thread);
		SQInteger level;
		sq_getinteger(v,-1,&level);
		SQRESULT res = __getcallstackinfos(thread,level);
		if(SQ_FAILED(res))
		{
			sq_settop(thread,threadtop);
			if(type(thread->_lasterror) == OT_STRING) {
				sq_throwerror(v,_stringval(thread->_lasterror));
			}
			else {
				sq_throwerror(v,_SC("unknown error"));
			}
		}
		if(res > 0) {
			//some result
			sq_move(v,thread,-1);
			sq_settop(thread,threadtop);
			return 1;
		}
		//no result
		sq_settop(thread,threadtop);
		return 0;

	}
	return sq_throwerror(v,_SC("wrong parameter"));
}

//THREAD DEFAULT DELEGATE//////////////////////////
static SQRESULT thread_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_THREAD))?1:SQ_ERROR;
}


SQRegFunction SQSharedState::_thread_default_delegate_funcz[] = {
	{_SC("call"), thread_call, -1, _SC("v")},
	{_SC("wakeup"), thread_wakeup, -1, _SC("v")},
	{_SC("getstatus"), thread_getstatus, 1, _SC("v")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("getstackinfos"),thread_getstackinfos,2, _SC("vn")},
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("getdelegate"),thread_getdelegate,1, _SC(".")},
	{0,0},
};

static SQRESULT class_getattributes(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getattributes(v,-2))?1:SQ_ERROR;
}

static SQRESULT class_setattributes(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_setattributes(v,-3))?1:SQ_ERROR;
}

static SQRESULT class_instance(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_createinstance(v,-1))?1:SQ_ERROR;
}

static SQRESULT class_getbase(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getbase(v,-1))?1:SQ_ERROR;
}

static SQRESULT class_newmember(HSQUIRRELVM v)
{
	SQInteger top = sq_gettop(v);
	SQBool bstatic = SQFalse;
	if(top == 5)
	{
		sq_tobool(v,-1,&bstatic);
		sq_pop(v,1);
	}

	if(top < 4) {
		sq_pushnull(v);
	}
	return SQ_SUCCEEDED(sq_newmember(v,-4,bstatic))?1:SQ_ERROR;
}

static SQRESULT class_rawnewmember(HSQUIRRELVM v)
{
	SQInteger top = sq_gettop(v);
	SQBool bstatic = SQFalse;
	if(top == 5)
	{
		sq_tobool(v,-1,&bstatic);
		sq_pop(v,1);
	}

	if(top < 4) {
		sq_pushnull(v);
	}
	return SQ_SUCCEEDED(sq_rawnewmember(v,-4,bstatic))?1:SQ_ERROR;
}

static SQRESULT class_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_CLASS))?1:SQ_ERROR;
}

SQRegFunction SQSharedState::_class_default_delegate_funcz[] = {
	{_SC("getattributes"), class_getattributes, 2, _SC("y.")},
	{_SC("setattributes"), class_setattributes, 3, _SC("y..")},
	{_SC("get"),container_get,-2, _SC("y")},
	{_SC("rawget"),container_rawget,-2, _SC("y")},
	{_SC("rawset"),container_rawset,3, _SC("y")},
	{_SC("rawin"),container_rawexists,2, _SC("y")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("instance"),class_instance,1, _SC("y")},
	{_SC("getbase"),class_getbase,1, _SC("y")},
	{_SC("newmember"),class_newmember,-3, _SC("y")},
	{_SC("rawnewmember"),class_rawnewmember,-3, _SC("y")},
	{_SC("getdelegate"),class_getdelegate,1, _SC(".")},
	{0,0}
};


static SQRESULT instance_getclass(HSQUIRRELVM v)
{
	if(SQ_SUCCEEDED(sq_getclass(v,1)))
		return 1;
	return SQ_ERROR;
}

static SQRESULT instance_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_INSTANCE))?1:SQ_ERROR;
}

SQRegFunction SQSharedState::_instance_default_delegate_funcz[] = {
	{_SC("getclass"), instance_getclass, 1, _SC("x")},
	{_SC("get"),container_get,-2, _SC("x")},
	{_SC("rawget"),container_rawget,-2, _SC("x")},
	{_SC("rawset"),container_rawset,3, _SC("x")},
	{_SC("rawin"),container_rawexists,2, _SC("x")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("getdelegate"),instance_getdelegate,1, _SC(".")},
	{0,0}
};

static SQRESULT weakref_ref(HSQUIRRELVM v)
{
	if(SQ_FAILED(sq_getweakrefval(v,1)))
		return SQ_ERROR;
	return 1;
}

static SQRESULT weakref_getdelegate(HSQUIRRELVM v)
{
	return SQ_SUCCEEDED(sq_getdefaultdelegate(v,OT_WEAKREF))?1:SQ_ERROR;
}

SQRegFunction SQSharedState::_weakref_default_delegate_funcz[] = {
	{_SC("ref"),weakref_ref,1, _SC("r")},
	{_SC("weakref"),obj_delegate_weakref,1, NULL },
	{_SC("tostring"),default_delegate_tostring,1, _SC(".")},
	{_SC("getdelegate"),weakref_getdelegate,1, _SC(".")},
	{0,0}
};

