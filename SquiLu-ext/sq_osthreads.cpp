#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"
SQ_OPT_STRING_STRLEN();

#include "ThreadObjectThreadClass.h"

static const SQChar *ThreadMutexObject_TAG = _SC("ThreadMutexObject");
#define GET_thread_mutex_INSTANCE_AT(idx) \
	CMutexClass *self=NULL; \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&self,(void*)ThreadMutexObject_TAG)) < 0) return _rc_;

#define GET_thread_mutex_INSTANCE() GET_thread_mutex_INSTANCE_AT(1)

static SQRESULT sq_thread_mutex_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v){
	CMutexClass *self = ((CMutexClass *)p);
	if (self) delete self;
	return 0;
}

static SQRESULT sq_thread_mutex_constructor(HSQUIRRELVM v){
    CMutexClass *self = new CMutexClass();
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v, 1, sq_thread_mutex_releasehook);
    return 1;
}

static SQRESULT sq_thread_mutex_lock(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_mutex_INSTANCE();
    self->Lock();
    return 0;
}

static SQRESULT sq_thread_mutex_unlock(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_mutex_INSTANCE();
    self->Unlock();
    return 0;
}

#define _DECL_FUNC(name,nparams,tycheck,isStatic) {_SC(#name),  sq_thread_mutex_##name,nparams,tycheck,isStatic}
static SQRegFunction sq_thread_mutex_methods[] =
{
	_DECL_FUNC(constructor, 1, _SC("x"), SQFalse),
	_DECL_FUNC(lock, 1, _SC("x"), SQFalse),
	_DECL_FUNC(unlock, 1, _SC("x"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

static const SQChar *ThreadEventObject_TAG = _SC("ThreadEventObject");

#define GET_thread_event_INSTANCE_AT(idx) \
	CEventClass *self=NULL; \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&self,(void*)ThreadEventObject_TAG)) < 0) return _rc_;

#define GET_thread_event_INSTANCE() GET_thread_event_INSTANCE_AT(1)

static SQRESULT sq_thread_event_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v){
	CEventClass *self = ((CEventClass *)p);
	if (self) delete self;
	return 0;
}

static SQRESULT sq_thread_event_constructor(HSQUIRRELVM v){
    CEventClass *self = new CEventClass();
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v, 1, sq_thread_event_releasehook);
    return 1;
}

static SQRESULT sq_thread_event_set(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_event_INSTANCE();
    self->Set();
    return 0;
}

static SQRESULT sq_thread_event_reset(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_event_INSTANCE();
    self->Reset();
    return 0;
}

static SQRESULT sq_thread_event_wait(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_event_INSTANCE();
    sq_pushbool(v, self->Wait());
    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck,isStatic) {_SC(#name),  sq_thread_event_##name,nparams,tycheck,isStatic}
static SQRegFunction sq_thread_event_methods[] =
{
	_DECL_FUNC(constructor, 1, _SC("x"), SQFalse),
	_DECL_FUNC(set, 1, _SC("x"), SQFalse),
	_DECL_FUNC(reset, 1, _SC("x"), SQFalse),
	_DECL_FUNC(wait, 1, _SC("x"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

class SQ_Task : public CTask {
    public:
    SQ_Task():CTask(){}
    BOOL Task(){}
};

static const SQChar *ThreadTaskObject_TAG = _SC("ThreadTaskObject");

#define GET_thread_task_INSTANCE_AT(idx) \
	SQ_Task *self=NULL; \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&self,(void*)ThreadTaskObject_TAG)) < 0) return _rc_;

#define GET_thread_task_INSTANCE() GET_thread_task_INSTANCE_AT(1)

static SQRESULT sq_thread_task_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v){
	SQ_Task *self = ((SQ_Task *)p);
	if (self) delete self;
	return 0;
}

static SQRESULT sq_thread_task_constructor(HSQUIRRELVM v){
    SQ_Task *self = new SQ_Task();
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v, 1, sq_thread_task_releasehook);
    return 1;
}

static SQRESULT sq_thread_task_status(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_task_INSTANCE();
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, state);
        self->SetTaskStatus((TaskStatus_t)state);
        return 0;
    }
    sq_pushinteger(v, self->Status());
    return 1;
}

static SQRESULT sq_thread_task_id(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_task_INSTANCE();
    if(_top_ > 1){
        SQ_GET_USERPOINTER(v, 2, pid);
        self->SetId((ThreadId_t*)pid);
        return 0;
    }
    ThreadId_t pId;
    self->Thread(&pId);
    sq_pushuserpointer(v, pId);
    return 1;
}

static SQRESULT sq_thread_task_wait(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_task_INSTANCE();
    SQ_GET_INTEGER(v, 2, timeoutSeconds);
    sq_pushbool(v, self->Wait(timeoutSeconds));
    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck,isStatic) {_SC(#name),  sq_thread_task_##name,nparams,tycheck,isStatic}
static SQRegFunction sq_thread_task_methods[] =
{
	_DECL_FUNC(constructor, 1, _SC("x"), SQFalse),
	_DECL_FUNC(status, -1, _SC("xi"), SQFalse),
	_DECL_FUNC(id, -1, _SC("xp"), SQFalse),
	_DECL_FUNC(wait, 2, _SC("xn"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

static const SQChar *ThreadObject_TAG = _SC("ThreadObject");

#define GET_thread_INSTANCE_AT(idx) \
	CThread *self=NULL; \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&self,(void*)ThreadObject_TAG)) < 0) return _rc_;

#define GET_thread_INSTANCE() GET_thread_INSTANCE_AT(1)

static SQRESULT sq_thread_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v){
	CThread *self = ((CThread *)p);
	if (self) delete self;
	return 0;
}

static SQRESULT sq_thread_constructor(HSQUIRRELVM v){
    CThread *self = new CThread();
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v, 1, sq_thread_releasehook);
    return 1;
}

static SQRESULT sq_thread_on_task(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_INSTANCE();
    if(_top_ > 1){
        SQ_GET_USERPOINTER(v, 2, lpvData);
        self->OnTask(lpvData);
    }
    else self->OnTask();
    return 0;
}

static SQRESULT sq_thread_id(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    sq_pushuserpointer(v, CThread::ThreadId());
    return 1;
}

static SQRESULT sq_thread_from_same_thread(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_INSTANCE();
    sq_pushbool(v, self->FromSameThread());
    return 1;
}

static SQRESULT sq_thread_percent_capacity(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_INSTANCE();
    sq_pushfloat(v, self->PercentCapacity());
    return 1;
}

static SQRESULT sq_thread_wait_till_exit(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_INSTANCE();
    self->WaitTillExit();
    return 0;
}

static SQRESULT sq_thread_kernel_process(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_thread_INSTANCE();
    sq_pushbool(v, self->KernelProcess());
    return 1;
}

static SQRESULT sq_thread_event(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_INSTANCE();
    SQ_GET_USERPOINTER(v, 2, lpvData);
    sq_pushbool(v, self->Event(lpvData));
    return 1;
}

static SQRESULT sq_thread_set_on_stop_timeout(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_INSTANCE();
    SQ_GET_INTEGER(v, 2, seconds);
    self->SetOnStopTimeout(seconds);
    return 0;
}

static SQRESULT sq_thread_set_queue_size(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_INSTANCE();
    SQ_GET_INTEGER(v, 2, size);
    sq_pushbool(v, self->SetQueueSize(size));
    return 1;
}

static SQRESULT sq_thread_start(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_INSTANCE();
    sq_pushbool(v, self->Start());
    return 1;
}

static SQRESULT sq_thread_stop(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    GET_thread_INSTANCE();
    sq_pushbool(v, self->Stop());
    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck,isStatic) {_SC(#name),  sq_thread_##name,nparams,tycheck,isStatic}
static SQRegFunction sq_thread_methods[] =
{
	_DECL_FUNC(constructor, 1, _SC("x"), SQFalse),
	_DECL_FUNC(on_task, -1, _SC("xi"), SQFalse),
	_DECL_FUNC(id, 1, _SC("y"), SQTrue),
	_DECL_FUNC(from_same_thread, 1, _SC("x"), SQFalse),
	_DECL_FUNC(percent_capacity, 1, _SC("x"), SQFalse),
	_DECL_FUNC(wait_till_exit, 1, _SC("x"), SQFalse),
	_DECL_FUNC(kernel_process, 1, _SC("x"), SQFalse),
	_DECL_FUNC(event, 2, _SC("x."), SQFalse),
	_DECL_FUNC(set_on_stop_timeout, 2, _SC("xn"), SQFalse),
	_DECL_FUNC(set_queue_size, 2, _SC("xi"), SQFalse),
	_DECL_FUNC(start, 1, _SC("x"), SQFalse),
	_DECL_FUNC(stop, 1, _SC("x"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_ThreadObjects(HSQUIRRELVM v)
{
    sq_pushstring(v,ThreadMutexObject_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)ThreadMutexObject_TAG);
    sq_insert_reg_funcs(v, sq_thread_mutex_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,ThreadEventObject_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)ThreadEventObject_TAG);
    sq_insert_reg_funcs(v, sq_thread_event_methods);

    sq_pushstring(v,ThreadTaskObject_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)ThreadTaskObject_TAG);
    sq_insert_reg_funcs(v, sq_thread_task_methods);

    sq_pushstring(v,ThreadObject_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)ThreadObject_TAG);
    sq_insert_reg_funcs(v, sq_thread_methods);

    return 0;
}

#ifdef __cplusplus
}
#endif
