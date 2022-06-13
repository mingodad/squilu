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

#ifdef SLAVE_VM_WITH_OS_THREADS
#include "mongoose.h"
#endif // SLAVE_VM_WITH_OS_THREADS

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

struct SlaveVM_st {
    HSQUIRRELVM v;
    HSQUIRRELVM master_vm;
#ifdef SLAVE_VM_WITH_OS_THREADS
    mg_thread_mutex_t mutex;
    mg_thread_cond_t cond_var;
    mg_thread_t thread_id;
    int slave_state, master_state;
    bool isRunningAsThread;
#endif // SLAVE_VM_WITH_OS_THREADS
};

static const SQChar sq_slave_vm_TAG[] = _SC("SlaveVM");

static SQRESULT get_slave_vm_instance(HSQUIRRELVM v, SQInteger idx, SlaveVM_st **svm_st)
{
    if(sq_getinstanceup(v, idx, (SQUserPointer*)svm_st, (void*)sq_slave_vm_TAG,SQFalse) != SQ_OK) return SQ_ERROR;
    if(!*svm_st) return sq_throwerror(v, _SC("%s"), _SC("slave vm already closed"));
    return SQ_OK;
}

#define GET_sq_slave_vm_INSTANCE(v, idx) \
    SlaveVM_st *self=NULL; HSQUIRRELVM svm; \
    if(get_slave_vm_instance(v, idx, &self) != SQ_OK) return SQ_ERROR; \
    svm = self->v;

static SQRESULT sq_slave_vm__tostring (HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    sq_pushfstring (v, _SC("Squirrel vm (%p)"), svm);
    return 1;
}

#ifdef SLAVE_VM_WITH_OS_THREADS
static int releaseSlaveThread(SlaveVM_st *self)
{
    if(self->isRunningAsThread)
    {
        mg_thread_cond_destroy(&self->cond_var);
        mg_thread_mutex_destroy(&self->mutex);
    }
    return 0;
}
#endif // SLAVE_VM_WITH_OS_THREADS

static SQRESULT sq_slave_vm_release_hook(SQUserPointer p, SQInteger size, void */*ep*/)
{
    SlaveVM_st *self = (SlaveVM_st*)p;
    if(self && self->v)
    {
#ifdef SLAVE_VM_WITH_OS_THREADS
        releaseSlaveThread(self);
#endif // SLAVE_VM_WITH_OS_THREADS
        sq_close(self->v);
        self->v = NULL;
        sq_free(self, sizeof(*self));
    }
    return 0;
}

#ifdef SLAVE_VM_WITH_OS_THREADS

#define SQ_getforeignptr_slave(v) \
    SlaveVM_st *self = (SlaveVM_st*)sq_getforeignptr(v);\
    if(!self) return sq_throwerror(v, _SC("%s"), _SC("No slavevm foreignptr found"));

static SQRESULT sq_vm_thread_lock(HSQUIRRELVM v)
{
    SQ_getforeignptr_slave(v);
    sq_pushinteger(v, mg_thread_mutex_lock(&self->mutex));
    return 1;
}

/*
static SQRESULT sq_vm_thread_unlock(HSQUIRRELVM v)
{
    SQ_getforeignptr_slave(v);
    sq_pushinteger(v, mg_thread_mutex_unlock(&self->mutex));
    return 1;
}
*/

static SQRESULT sq_vm_thread_cond_wait(HSQUIRRELVM v)
{
    SQ_getforeignptr_slave(v);
    sq_pushinteger(v, mg_thread_cond_wait(&self->cond_var, &self->mutex));
    return 1;
}

static SQRESULT sq_vm_thread_master_state(HSQUIRRELVM v)
{
    SQ_getforeignptr_slave(v);
    sq_pushinteger(v, self->master_state);
    return 1;
}

static SQRESULT sq_vm_thread_slave_state(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_getforeignptr_slave(v);
    SQ_OPT_INTEGER(v, 2, state, 0);
    sq_pushinteger(v, self->slave_state);
    if(sq_gettop(v) > 1) self->slave_state = state;
    return 1;
}
#endif // SLAVE_VM_WITH_OS_THREADS


/*
** Creates a new SQuirrel vm.
*/
static SQRESULT sq_slave_vm_constructor (HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, stack_size, 1024);
    SQ_OPT_BOOL(v, 3, with_libraries, false);
    SQ_OPT_BOOL(v, 4, run_as_thread, false);
    HSQUIRRELVM self = sq_open(stack_size);

    /* Initialize environment */
    sq_setprintfunc(self,sq_getprintfunc(v),sq_geterrorfunc(v));

    if(with_libraries)
    {
        /* load base libraries */
        sq_pushroottable(self);

        sqstd_register_bloblib(self);
        sqstd_register_iolib(self);
        sqstd_register_systemlib(self);
        sqstd_register_mathlib(self);
        sqstd_register_stringlib(self);
        sq_poptop(self); //remove root table
    }

    SlaveVM_st *svm_st = (SlaveVM_st *)sq_malloc(sizeof(SlaveVM_st));
    memset(svm_st, 0, sizeof(*svm_st));
#ifdef SLAVE_VM_WITH_OS_THREADS
    sq_setforeignptr(self, svm_st);
    if(run_as_thread)
    {
        svm_st->isRunningAsThread = true;
        svm_st->master_vm = v;
        mg_thread_mutex_init(&svm_st->mutex, NULL);
        mg_thread_cond_init(&svm_st->cond_var, NULL);

        sq_pushroottable(self);
        //sq_insertfunc(self, _SC("slavevm_thread_lock"), sq_vm_thread_lock, 1, _SC("."), SQFalse);
        //sq_insertfunc(self, _SC("slavevm_unlock"), sq_vm_thread_unlock, 1, _SC("."), SQFalse);
        sq_insertfunc(self, _SC("slavevm_thread_cond_wait"), sq_vm_thread_cond_wait, 1, _SC("."), SQFalse);
        sq_insertfunc(self, _SC("slavevm_thread_master_state"), sq_vm_thread_master_state, 1, _SC("."), SQFalse);
        sq_insertfunc(self, _SC("slavevm_thread_slave_state"), sq_vm_thread_slave_state, -1, _SC(".i"), SQFalse);
        sq_poptop(self); //remove root table
    }
#endif // SLAVE_VM_WITH_OS_THREADS
    svm_st->v = self;
    svm_st->master_vm = v;

    sq_setinstanceup(v, 1, svm_st);
    sq_setreleasehook(v, 1, sq_slave_vm_release_hook);

    return 1;
}

static SQRESULT sq_slave_vm_preload_lib (HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, module_name);

    SQFUNCTION sq_func = sq_get_preload_module_func(v, module_name);
    SQBool rc =  sq_func != NULL;
    if(rc)
    {
        sq_pushroottable(svm);
        sq_func(svm);
        sq_poptop(svm); //remove root table
    }

    sq_pushbool(v, rc);
    return 1;
}

#ifdef SLAVE_VM_WITH_OS_THREADS
static SQRESULT checkTryLock(HSQUIRRELVM v, SlaveVM_st *self)
{
    if(self->isRunningAsThread && mg_thread_mutex_trylock(&self->mutex))
    {
        return sq_throwerror(v, _SC("%s"), _SC("Could not aquire slave lock"));
    }
    return SQ_OK;
}
#endif // SLAVE_VM_WITH_OS_THREADS

static SQRESULT sq_slave_vm_close(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
    releaseSlaveThread(self);
#endif // SLAVE_VM_WITH_OS_THREADS
    sq_close(svm);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "vm is closed"
    return 0;
}

static SQRESULT sq_slave_vm_set_include_path(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
    releaseSlaveThread(self);
#endif // SLAVE_VM_WITH_OS_THREADS
    SQ_GET_STRING(v, 2, include_path);
    sq_set_include_path(svm, include_path);
    return 0;
}

static SQRESULT sq_slave_vm_get_include_path(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
    releaseSlaveThread(self);
#endif // SLAVE_VM_WITH_OS_THREADS
    const SQChar *include_path = sq_get_include_path(svm);
    if(include_path) sq_pushstring(v, include_path, -1);
    else sq_pushnull(v);
    return 1;
}

static SQRESULT sq_slave_vm_call(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQ_GET_BOOL(v, 2, has_ret_val);
    SQ_GET_STRING(v, 3, func_name);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
#endif // SLAVE_VM_WITH_OS_THREADS
    SQInteger top = sq_gettop(svm);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(svm);
    sq_pushstring(svm, func_name, func_name_size);
    if(sq_get(svm, -2) == SQ_OK)
    {
        switch(sq_gettype(svm, -1))
        {
        case OT_CLOSURE:
        case OT_NATIVECLOSURE:
        case OT_CLASS:
        {
            sq_pushroottable(svm);
            int argc = sq_gettop(v) - 3;
            if(argc && copy_values_between_vms(svm, v, argc, 4) != SQ_OK)
            {
                result = sq_throwerror(v, _SC("%s"), sq_getlasterror_str(svm));
                goto cleanup;
            }
            if(sq_call(svm, argc+1, has_ret_val, SQFalse) != SQ_OK)
            {
                result = sq_throwerror(v, _SC("%s"), sq_getlasterror_str(svm));
                goto cleanup;
            }
            if(has_ret_val)
            {
                if(copy_values_between_vms(v, svm, 1, sq_gettop(svm)) == SQ_OK) result = 1;
            }
            else result = SQ_OK;
        }
        }
    }
    else
    {
        result = sq_throwerror(v, _SC("%s"), sq_getlasterror_str(svm));
    }
cleanup:
    sq_settop(svm, top);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(self->isRunningAsThread) mg_thread_mutex_unlock(&self->mutex);
#endif // SLAVE_VM_WITH_OS_THREADS
    return result;
}

static SQRESULT sq_slave_vm_set(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
#endif // SLAVE_VM_WITH_OS_THREADS
    SQInteger top = sq_gettop(svm);
    SQRESULT result;
    sq_pushroottable(svm);
    if(copy_values_between_vms(svm, v, 1, 2) == SQ_OK
            && copy_values_between_vms(svm, v, 1, 3) == SQ_OK)
    {
        result = sq_newslot(svm, -3, SQFalse);
        if(result == SQ_ERROR) sq_throwerror(v, _SC("%s"), sq_getlasterror_str(svm));
    }
    else result = SQ_ERROR;
    sq_settop(svm, top);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(self->isRunningAsThread) mg_thread_mutex_unlock(&self->mutex);
#endif // SLAVE_VM_WITH_OS_THREADS
    return result;
}

static SQRESULT sq_slave_vm_get(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
#endif // SLAVE_VM_WITH_OS_THREADS
    SQInteger top = sq_gettop(svm);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(svm);
    if(copy_values_between_vms(svm, v, 1, 2) == SQ_OK)
    {
        if(sq_get(svm, -2) == SQ_OK
                && copy_values_between_vms(v, svm, 1, sq_gettop(svm)) == SQ_OK)
        {
            result = 1;
        }
        else
        {
            if(sq_gettop(v) == 3) result = 1; //we have a default value
            else sq_throwerror(v, _SC("%s"), sq_getlasterror_str(svm));
        }
    }
    sq_settop(svm, top);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(self->isRunningAsThread) mg_thread_mutex_unlock(&self->mutex);
#endif // SLAVE_VM_WITH_OS_THREADS
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
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
#endif // SLAVE_VM_WITH_OS_THREADS
    SQInteger top = sq_gettop(svm);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(svm); //important always push the root table, because sqstd_dofile will try to do a sq_push(v, -2)
    if(sqstd_dofile(svm, file_name, retval, printerror, show_warnings) >= 0)
    {
        if(retval)
        {
            if(copy_values_between_vms(v, svm, 1, sq_gettop(svm)) == SQ_OK) result = 1;
        }
        else result = SQ_OK;
    }
    else sq_throwerror(v, _SC("%s"), sq_getlasterror_str(svm));
    sq_settop(svm, top);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(self->isRunningAsThread) mg_thread_mutex_unlock(&self->mutex);
#endif // SLAVE_VM_WITH_OS_THREADS
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
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
#endif // SLAVE_VM_WITH_OS_THREADS
    SQInteger top = sq_gettop(svm);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(svm);
    sq_pushstring(svm, func_name, func_name_size);
    if(sqstd_loadfile(svm, file_name, printerror, show_warnings) >= 0)
    {
        result = sq_newslot(svm, -3, SQFalse);
    }
    sq_settop(svm, top);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(self->isRunningAsThread) mg_thread_mutex_unlock(&self->mutex);
#endif // SLAVE_VM_WITH_OS_THREADS
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
    SQ_OPT_INTEGER(v, 6, max_nested_includes, 0);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
#endif // SLAVE_VM_WITH_OS_THREADS
    SQInteger top = sq_gettop(svm);
    SQRESULT result = SQ_ERROR;
    sq_pushroottable(svm);
    sq_pushstring(svm, func_name, func_name_size);
    if(sq_compilebuffer(svm, str_script, str_script_size, func_name, printerror, show_warnings, max_nested_includes) >= 0)
    {
        result = sq_newslot(svm, -3, SQFalse);
    }
    else
    {
        const SQChar *last_error = sq_getlasterror_str(svm);
        if(last_error) {
            SQInteger line, column;
            sq_getlasterror_line_col(svm, &line, &column);
            result = sq_throwerror(v, _SC("compilestring %s %d:%d: %s"), func_name, (int)line, (int)column, last_error);
        }
    }
    sq_settop(svm, top);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(self->isRunningAsThread) mg_thread_mutex_unlock(&self->mutex);
#endif // SLAVE_VM_WITH_OS_THREADS
    return result;
}

extern SQInteger _g_io_dostring(HSQUIRRELVM v);

static SQRESULT sq_slave_vm_dostring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, str);
    SQ_OPT_BOOL(v, 3, retval, false);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(checkTryLock(v, self)) return SQ_ERROR;
#endif // SLAVE_VM_WITH_OS_THREADS
    SQInteger top = sq_gettop(svm);
    sq_pushroottable(svm); //important always push the root table, because sqstd_dofile will try to do a sq_push(v, -2)
    sq_pushstring(svm, str, str_size);
    sq_pushbool(svm, retval);
    SQRESULT result = _g_io_dostring(svm);
    if(result == SQ_ERROR) sq_settop(svm, top);
#ifdef SLAVE_VM_WITH_OS_THREADS
    if(self->isRunningAsThread) mg_thread_mutex_unlock(&self->mutex);
#endif // SLAVE_VM_WITH_OS_THREADS
    return result;
}

static SQRESULT sq_slave_vm_is_thread_idle(HSQUIRRELVM v)
{
#ifdef SLAVE_VM_WITH_OS_THREADS
    GET_sq_slave_vm_INSTANCE(v, 1);
    int rc = mg_thread_mutex_trylock(&self->mutex) == 0;
    if(rc) mg_thread_mutex_unlock(&self->mutex);
    sq_pushbool(v, rc);
#else
    sq_pushbool(v, SQTrue);
#endif
    return 1;
}

static SQRESULT sq_slave_vm_is_runing_as_thread(HSQUIRRELVM v)
{
#ifdef SLAVE_VM_WITH_OS_THREADS
    GET_sq_slave_vm_INSTANCE(v, 1);
    sq_pushbool(v, self->isRunningAsThread);
#else
    sq_pushbool(v, SQFalse);
#endif
    return 1;
}

#ifdef SLAVE_VM_WITH_OS_THREADS

#define check_run_as_thread(v) \
    if(!self->isRunningAsThread) return sq_throwerror(v, _SC("%s"), _SC("Slave VM not running as thread"))

static void *slaveThreadTask(void *p)
{
    SlaveVM_st *self = (SlaveVM_st*)p;
    self->thread_id = mg_thread_self();
    HSQUIRRELVM svm = self->v;
    int rc;
    SQInteger top, argc;
    rc = sq_getinteger(svm, -1, &top) == SQ_OK;
    if(rc)
    {
        rc = sq_getinteger(svm, -2, &argc) == SQ_OK;
        if(rc)
        {
            sq_pop(svm, 2); //remove top & argc
            rc = sq_call(svm, argc+1, SQFalse, SQFalse);
        }
        sq_settop(svm, top);
    }
    rc = mg_thread_mutex_unlock(&self->mutex);
    self->thread_id = 0;
    //printf("%d:%d:%s\n", __LINE__, rc, __FILE__);
    return NULL;
}

extern void sq_system_sleep(int n);

static SQRESULT sq_slave_vm_thread_run(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    check_run_as_thread(v);

    int rc;
    const SQChar *func_name;
    SQInteger func_name_size;
    if(self->thread_id) return sq_throwerror(v, _SC("%s"), _SC("slavevm already running in a thread"));
    SQInteger top = sq_gettop(svm);
    rc = mg_thread_mutex_trylock(&self->mutex) == 0;
    if(rc)
    {
        sq_getstr_and_size(v, 2, &func_name, &func_name_size);
        sq_pushstring(svm, func_name, func_name_size);
        sq_getonroottable(svm);
        //printf("%d:%s\n", __LINE__, __FILE__);
        rc = (sq_gettype(svm, -1) == OT_CLOSURE);
        if( rc )
        {
            //printf("%d:%s\n", __LINE__, __FILE__);
            sq_pushroottable(svm);
            int argc = sq_gettop(v) - 2;
            rc = ( (argc == 0) || (copy_values_between_vms(svm, v, argc, 3) == SQ_OK));
            if(rc)
            {
                sq_pushinteger(svm, argc);
                sq_pushinteger(svm, top);
                rc = mg_start_thread(slaveThreadTask, self) == 0;
                if(rc) goto done;
            }
            //printf("%d:%d:%s\n", __LINE__, rc, __FILE__);
        }
        sq_settop(svm, top); //something went wrong reset slave stack
    }
done:
    sq_pushbool(v, rc);
    return 1;
}

static SQRESULT sq_slave_vm_thread_trylock(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    check_run_as_thread(v);
    sq_pushinteger(v, mg_thread_mutex_trylock(&self->mutex));
    return 1;
}

static SQRESULT sq_slave_vm_thread_unlock(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    check_run_as_thread(v);
    sq_pushinteger(v, mg_thread_mutex_unlock(&self->mutex));
    return 1;
}

static SQRESULT sq_slave_vm_thread_cond_signal(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    check_run_as_thread(v);
    sq_pushinteger(v, mg_thread_cond_signal(&self->cond_var));
    return 1;
}

static SQRESULT sq_slave_vm_thread_master_state(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_sq_slave_vm_INSTANCE(v, 1);
    check_run_as_thread(v);
    SQ_OPT_INTEGER(v, 2, state, 0);
    sq_pushinteger(v, self->master_state);
    if(sq_gettop(v) > 1) self->master_state = state;
    return 1;
}

static SQRESULT sq_slave_vm_thread_slave_state(HSQUIRRELVM v)
{
    GET_sq_slave_vm_INSTANCE(v, 1);
    check_run_as_thread(v);
    sq_pushinteger(v, self->slave_state);
    return 1;
}
#endif // SLAVE_VM_WITH_OS_THREADS

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_sq_slave_vm(HSQUIRRELVM v)
    {
        const SQChar get_set_validation_mask[] = _SC("x s|n|p s|n|b|a|t|p|o");
        sq_pushstring(v,sq_slave_vm_TAG, -1);
        sq_newclass(v, SQFalse);
        sq_settypetag(v,-1,(void*)sq_slave_vm_TAG);
        sq_insertfunc(v, _SC("constructor"), sq_slave_vm_constructor, -1, _SC("xibb"), SQFalse);
        sq_insertfunc(v, _SC("_tostring"), sq_slave_vm__tostring, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("close"), sq_slave_vm_close, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("set_include_path"), sq_slave_vm_set_include_path, 2, _SC("xs"), SQFalse);
        sq_insertfunc(v, _SC("get_include_path"), sq_slave_vm_get_include_path, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("set"), sq_slave_vm_set, 3, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("_set"), sq_slave_vm_set, 3, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("get"), sq_slave_vm_get, -2, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("_get"), sq_slave_vm_get, -2, get_set_validation_mask, SQFalse);
        sq_insertfunc(v, _SC("dofile"), sq_slave_vm_dofile, -2, _SC("xsbbb"), SQFalse);
        sq_insertfunc(v, _SC("loadfile"), sq_slave_vm_loadfile, -3, _SC("xssbb"), SQFalse);
        sq_insertfunc(v, _SC("dostring"), sq_slave_vm_dostring, -2, _SC("xsb"), SQFalse);
        sq_insertfunc(v, _SC("compilestring"), sq_slave_vm_compilestring, -3, _SC("xssbbi"), SQFalse);
        sq_insertfunc(v, _SC("call"), sq_slave_vm_call, -3, _SC("xbs"), SQFalse);
        sq_insertfunc(v, _SC("is_thread_idle"), sq_slave_vm_is_thread_idle, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("is_runing_as_thread"), sq_slave_vm_is_runing_as_thread, 1, _SC("x"), SQFalse);
#ifdef SLAVE_VM_WITH_OS_THREADS
        sq_insertfunc(v, _SC("thread_run"), sq_slave_vm_thread_run, -2, _SC("xs"), SQFalse);
        sq_insertfunc(v, _SC("thread_trylock"), sq_slave_vm_thread_trylock, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("thread_unlock"), sq_slave_vm_thread_unlock, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("thread_cond_signal"), sq_slave_vm_thread_cond_signal, 1, _SC("x"), SQFalse);
        sq_insertfunc(v, _SC("thread_master_state"), sq_slave_vm_thread_master_state, -1, _SC("xi"), SQFalse);
        sq_insertfunc(v, _SC("thread_slave_state"), sq_slave_vm_thread_slave_state, 1, _SC("x"), SQFalse);
#endif // SLAVE_VM_WITH_OS_THREADS
        sq_insertfunc(v, _SC("preload_lib"), sq_slave_vm_preload_lib, 2, _SC("xs"), SQFalse);

        sq_newslot(v,-3,SQTrue); //push sq_slave_vm class
        return 0;
    }

#ifdef __cplusplus
}
#endif
