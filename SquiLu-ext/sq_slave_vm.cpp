/*
** Rings: Multiple Lua States
** $Id: rings.c,v 1.24 2008/06/30 17:52:31 carregal Exp $
** See Copyright Notice in license.html
*/

#include <string.h>
#include <stdio.h>

#include "squirrel.h"
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>

/*
** Copies values from State src to State dst.
*/
static SQRESULT copy_values_between_vms (HSQUIRRELVM dst, HSQUIRRELVM src, int argc, int argIdx)
{
    SQRESULT _rc_;
    sq_reservestack(dst, argc + 20);
    argc += argIdx; //we will work with argc args starting at argIdx
    for (; argIdx < argc; argIdx++)
    {
        switch (sq_gettype(src, argIdx))
        {
        case OT_INTEGER:
            SQ_GET_INTEGER(src, argIdx, vint);
            sq_pushinteger(dst, vint);
            break;

        case OT_FLOAT:
            SQ_GET_FLOAT(src, argIdx, vfloat);
            sq_pushfloat (dst, vfloat);
            break;

        case OT_BOOL:
            SQ_GET_BOOL(src, argIdx, vbool);
            sq_pushbool (dst, vbool);
            break;

        case OT_STRING:
        {
            SQ_GET_STRING(src, argIdx, vstr)
            sq_pushstring (dst, vstr, vstr_size);
        }
        break;

        case OT_ARRAY:
        {
            SQInteger size = sq_getsize(src, argIdx);
            sq_newarray(dst, size);
            for(SQInteger i=0; i<size; ++i)
            {
                sq_pushinteger(src, i);
                sq_get(src, -2);
                sq_pushinteger(dst, i);
                if(copy_values_between_vms(dst, src, 1, sq_gettop(src)) != SQ_OK) return SQ_ERROR;
                sq_poptop(src);
                sq_set(dst, -3);
            }
        }
        break;

        case OT_TABLE:
        {
            sq_newtable(dst);
            sq_pushnull(src);
            while(sq_next(src, -2) == SQ_OK)
            {
                SQInteger src_top = sq_gettop(src);
                if(copy_values_between_vms(dst, src, 1, src_top-1) != SQ_OK
                        || copy_values_between_vms(dst, src, 1, src_top) != SQ_OK) return SQ_ERROR;
                sq_newslot(dst, -3, SQFalse);
                sq_pop(src, 2);
            }
            sq_pop(src,1);
        }
        break;

        case OT_USERPOINTER:
        {
            SQUserPointer ptr;
            sq_getuserpointer(src, argIdx, &ptr);
            sq_pushuserpointer(dst, ptr);
        }
        break;

        case OT_NULL:
            sq_pushnull(dst);
            break;

        default:
            return SQ_ERROR;
        }
    }
    return SQ_OK;
}


static const SQChar sq_slave_vm_TAG[] = _SC("SlaveVM");

static SQRESULT get_slave_vm_instance(HSQUIRRELVM v, SQInteger idx, HSQUIRRELVM *vm)
{
    if(sq_getinstanceup(v, idx, (SQUserPointer*)vm, (void*)sq_slave_vm_TAG) != SQ_OK) return SQ_ERROR;
    if(!vm) return sq_throwerror(v, _SC("slave vm already closed"));
    return SQ_OK;
}

#define GET_sq_slave_vm_INSTANCE(v, idx) \
    HSQUIRRELVM self=NULL; if(get_slave_vm_instance(v, idx, &self) != SQ_OK) return SQ_ERROR;

static SQRESULT sq_slave_vm__tostring (HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    sq_pushfstring (v, _SC("Squirrel vm (%p)"), self);
    return 1;
}

static SQRESULT sq_slave_vm_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
    HSQUIRRELVM self = (HSQUIRRELVM)p;
    if(self) sq_close(self);
    return 0;
}

/*
** Creates a new SQuirrel vm.
*/
static SQRESULT sq_slave_vm_constructor (HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, stack_size, 1024);
    HSQUIRRELVM self = sq_open(stack_size);

    /* Initialize environment */
    sq_setprintfunc(self,sq_getprintfunc(v),sq_geterrorfunc(v));

    /* load base libraries */
    sq_pushroottable(self);

    sqstd_register_bloblib(self);
    sqstd_register_iolib(self);
    sqstd_register_systemlib(self);
    sqstd_register_mathlib(self);
    sqstd_register_stringlib(self);
    sq_poptop(self); //remove root table

    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v, 1, sq_slave_vm_release_hook);

    return 1;
}

static SQRESULT sq_slave_vm_close(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    sq_close(self);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "vm is closed"
    return 0;
}

static SQRESULT sq_slave_vm_call(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQ_GET_BOOL(v, 2, has_ret_val);
    SQ_GET_STRING(v, 3, func_name);
    SQInteger top = sq_gettop(self);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(self);
    sq_pushstring(self, func_name, func_name_size);
    if(sq_get(self, -2) == SQ_OK)
    {
        switch(sq_gettype(self, -1))
        {
        case OT_CLOSURE:
        case OT_NATIVECLOSURE:
        case OT_CLASS:
        {
            sq_pushroottable(self);
            int argc = sq_gettop(v) - 3;
            if(argc && copy_values_between_vms(self, v, argc, 4) != SQ_OK)
            {
                sq_throwerror(v, sq_getlasterror_str(self));
                goto cleanup;
            }
            if(sq_call(self, argc+1, has_ret_val, SQFalse) != SQ_OK)
            {
                sq_throwerror(v, sq_getlasterror_str(self));
                goto cleanup;
            }
            if(has_ret_val)
            {
                if(copy_values_between_vms(v, self, 1, sq_gettop(self)) == SQ_OK) result = 1;
            }
            else result = SQ_OK;
        }
        }
    }
    else
    {
        sq_throwerror(v, sq_getlasterror_str(self));
    }
cleanup:
    sq_settop(self, top);
    return result;
}

static SQRESULT sq_slave_vm_set(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQInteger top = sq_gettop(self);
    SQRESULT result;
    sq_pushroottable(self);
    if(copy_values_between_vms(self, v, 1, 2) == SQ_OK
            && copy_values_between_vms(self, v, 1, 3) == SQ_OK)
    {
        result = sq_newslot(self, -3, SQFalse);
        if(result == SQ_ERROR) sq_throwerror(v, sq_getlasterror_str(self));
    }
    else result = SQ_ERROR;
    sq_settop(self, top);
    return result;
}

static SQRESULT sq_slave_vm_get(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQInteger top = sq_gettop(self);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(self);
    if(copy_values_between_vms(self, v, 1, 2) == SQ_OK)
    {
        if(sq_get(self, -2) == SQ_OK
                && copy_values_between_vms(v, self, 1, sq_gettop(self)) == SQ_OK)
        {
            result = 1;
        }
        else
        {
            if(sq_gettop(v) == 3) result = 1; //we have a default value
            else sq_throwerror(v, sq_getlasterror_str(self));
        }
    }
    sq_settop(self, top);
    return result;
}

static SQRESULT sq_slave_vm_dofile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, file_name);
    SQ_OPT_BOOL(v, 3, retval, false);
    SQ_OPT_BOOL(v, 4, printerror, false);
    SQ_OPT_BOOL(v, 5, show_warnings, false);
    SQInteger top = sq_gettop(self);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(self); //important always push the root table, because sqstd_dofile will try to do a sq_push(v, -2)
    if(sqstd_dofile(self, file_name, retval, printerror, show_warnings) >= 0)
    {
        if(retval)
        {
            if(copy_values_between_vms(v, self, 1, sq_gettop(self)) == SQ_OK) result = 1;
        }
        else result = SQ_OK;
    }
    else sq_throwerror(v, sq_getlasterror_str(self));
    sq_settop(self, top);
    return result;
}

static SQRESULT sq_slave_vm_loadfile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, func_name);
    SQ_GET_STRING(v, 3, file_name);
    SQ_OPT_BOOL(v, 4, printerror, false);
    SQ_OPT_BOOL(v, 5, show_warnings, false);
    SQInteger top = sq_gettop(self);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(self);
    sq_pushstring(self, func_name, func_name_size);
    if(sqstd_loadfile(self, file_name, printerror, show_warnings) >= 0)
    {
        result = sq_newslot(self, -3, SQFalse);
    }
    sq_settop(self, top);
    return result;
}

static SQRESULT sq_slave_vm_compilestring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, func_name);
    SQ_GET_STRING(v, 3, str_script);
    SQ_OPT_BOOL(v, 4, printerror, false);
    SQ_OPT_BOOL(v, 5, show_warnings, false);
    SQInteger top = sq_gettop(self);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(self);
    sq_pushstring(self, func_name, func_name_size);
    if(sq_compilebuffer(self, str_script, str_script_size, func_name, printerror, show_warnings) >= 0)
    {
        result = sq_newslot(self, -3, SQFalse);
    }
    sq_settop(self, top);
    return result;
}

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_sq_slave_vm(HSQUIRRELVM v)
    {
        const SQChar get_set_validation_mask[] = _SC("x s|n|p s|n|b|a|t|p|o");
        sq_pushstring(v,sq_slave_vm_TAG, -1);
        sq_newclass(v, SQFalse);
        sq_settypetag(v,-1,(void*)sq_slave_vm_TAG);
        sq_insertfunc(v, _SC("constructor"), sq_slave_vm_constructor, -1, _SC("xi"), SQFalse);
        sq_insertfunc(v, _SC("_tostring"), sq_slave_vm__tostring, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("close"), sq_slave_vm_close, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("set"), sq_slave_vm_set, 3, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("_set"), sq_slave_vm_set, 3, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("get"), sq_slave_vm_get, -2, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("_get"), sq_slave_vm_get, -2, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("dofile"), sq_slave_vm_dofile, -2, _SC("xsbbb"), SQFalse);
        sq_insertfunc(v, _SC("loadfile"), sq_slave_vm_loadfile, -3, _SC("xssbb"), SQFalse);
        sq_insertfunc(v, _SC("compilestring"), sq_slave_vm_compilestring, -3, _SC("xssbb"), SQFalse);
        sq_insertfunc(v, _SC("call"), sq_slave_vm_call, -3, _SC("xbs"), SQFalse);

        sq_newslot(v,-3,SQTrue); //push sq_slave_vm class
        return 0;
    }

#ifdef __cplusplus
}
#endif
