#include "squirrel.h"
#include <stdio.h>

typedef struct {
    HSQOBJECT func_to_call;
    HSQOBJECT param;
} gc_scope_alert_st;

static const SQChar *gc_scope_alert_TAG = _SC("gc_scope_alert");

static SQRESULT gc_scope_alert_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	gc_scope_alert_st *self = ((gc_scope_alert_st *)p);
//printf("%p %p\n", p, v);
	if(self){
	    if(v){
            SQInteger top = sq_gettop(v);
            sq_reservestack(v, 20);
            sq_pushobject(v, self->func_to_call);
            sq_pushroottable(v);
            sq_pushobject(v, self->param);
            sq_call(v, 2, SQFalse, SQTrue);
            sq_release(v, &self->func_to_call);
            sq_release(v, &self->param);
            sq_settop(v, top);
	    }
	    //else
	    sq_free(self, sizeof(gc_scope_alert_st));
	}
	return 0;
}

static SQRESULT gc_scope_alert_constructor(HSQUIRRELVM v)
{
    SQInteger rc;
    gc_scope_alert_st proto;
    sq_resetobject(&proto.func_to_call);
    sq_resetobject(&proto.param);
    if((rc = sq_getstackobj(v, 2, &proto.func_to_call)) < 0) return rc;
    if(sq_gettop(v) > 2){
         if((rc = sq_getstackobj(v, 3, &proto.param)) < 0) return rc;
    }
    gc_scope_alert_st *self = (gc_scope_alert_st*)sq_malloc(sizeof(gc_scope_alert_st));
    if(!self) return sq_throwerror(v, "Failed to create %s", gc_scope_alert_TAG);
    sq_addref(v, &proto.func_to_call);
    sq_addref(v, &proto.param);
    *self = proto;
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, gc_scope_alert_releasehook);
    return 1;
}

static SQRESULT spectralnorm_A(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP();
	SQ_GET_INTEGER(v, 2, i);
	SQ_GET_INTEGER(v, 3, j);
	SQInteger ij = j + i++;
	sq_pushfloat(v, 1.0/(ij * (ij+1)/2.0+i));
    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  gc_scope_alert_##name,nparams,tycheck}
static SQRegFunction gc_scope_alert_methods[] =
{
	_DECL_FUNC(constructor,  -2, _SC("xc.")),
	{0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_dad_utils(HSQUIRRELVM v)
    {
    	sq_insertfunc(v, _SC("spectralnorm_A"), spectralnorm_A, 3, _SC(".ii"), true);

        sq_pushstring(v,_SC("dad_utils"),-1);
        sq_newtable(v);

        sq_pushstring(v,gc_scope_alert_TAG,-1);
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)gc_scope_alert_TAG);
        sq_insert_reg_funcs(v, gc_scope_alert_methods);
        sq_rawset(v,-3);

        sq_rawset(v,-3);//insert dad_utils
        return 1;
    }

#ifdef __cplusplus
}
#endif
