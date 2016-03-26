#ifdef WITH_FFI

#include "squirrel.h"
#include <ffi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"
#include "sqstdblob.h"

#ifdef WIN32
#include <windows.h>
#define CLOSE_LIB(lib) (FreeLibrary((HINSTANCE)lib) != 0)
#ifdef _WIN32_WCE
#define OPEN_LIB(lib) (void*)LoadLibraryA(lib)
#define GET_SYM(lib, sn) GetProcAddressA((HINSTANCE)lib, sn)
#else
#define OPEN_LIB(lib) (void*)LoadLibrary(lib)
#define GET_SYM(lib, sn) (void*)GetProcAddress((HINSTANCE)lib, sn)
#endif
typedef void* (WINAPI*cPtrFuncVarArg)(...);
#else
#include <dlfcn.h>
#define CLOSE_LIB(lib) (::dlclose(lib) == 0)
#define OPEN_LIB(lib) ::dlopen(lib,RTLD_LAZY)
#define GET_SYM(lib, sn) ::dlsym(lib, sn)
typedef void*(*cPtrFuncVarArg)(...);
#endif

static const SQChar *FFI_LIB_TAG = _SC("FFI_LIB");
static const SQChar *FFI_LIB_LIB_TAG = _SC("FFI_LIB_LIB");
static const SQChar *FFI_LIB_FUNC_TAG = _SC("FFI_LIB_FUNC");
static const SQChar *FFI_LIB_VAR_TAG = _SC("FFI_LIB_VAR");

//Code adapted from https://github.com/pfalcon/squirrel-modules

/** Stores a delegate table on registry by key */
static inline void sq_create_delegate_table(HSQUIRRELVM vm, SQRegFunction *methods, const SQChar *key)
{
    sq_pushstring(vm, key, -1);
    sq_newtable(vm);
    sq_insert_reg_funcs(vm, methods);
    sq_setonregistrytable(vm);
}

static inline void sq_push_delegate_table(HSQUIRRELVM vm, const SQChar *key)
{
    sq_pushstring(vm, key, -1);
    sq_getonregistrytable(vm);
}

static SQInteger sq_ffi_load(HSQUIRRELVM v)
{
    const SQChar *fname;
    sq_getstring(v, 2, &fname);
    void *mod = OPEN_LIB(fname);
    if (!mod)
        return sq_throwerror(v, "Cannot load library");
    sq_pushuserpointer(v, mod);
    void **p = (void**)sq_newuserdata(v, sizeof(mod));
    *p = mod;
    sq_push_delegate_table(v, FFI_LIB_LIB_TAG);
    sq_setdelegate(v, -2);
    return 1;
}

static SQInteger sq_ffi_sym(HSQUIRRELVM v)
{
    void *mod;
    const SQChar *symname;
    sq_getuserpointer(v, 2, &mod);
    sq_getstring(v, 3, &symname);
    void *sym = GET_SYM(mod, symname);
    printf("GET_SYM(%s) = %p\n", symname, sym);
    if (!sym)
        return sq_throwerror(v, "Cannot find symbol");
    void **p = (void**)sq_newuserdata(v, sizeof(sym));
    *p = sym;
    return 1;
}

static ffi_type *char2ffi_type(char c)
{
    switch (c) {
    case 'b':
        return &ffi_type_schar;
    case 'B':
        return &ffi_type_uchar;
    case 'i':
        return &ffi_type_sint;
    case 'I':
        return &ffi_type_uint;
    case 'l':
        return &ffi_type_slong;
    case 'L':
        return &ffi_type_ulong;
    case 'P':
    case 's':
        return &ffi_type_pointer;
    default:
        return NULL;
    }
}

static ffi_type *get_ffi_type(HSQUIRRELVM v, int idx)
{
    int type = sq_gettype(v, idx);
    void *p;
    const SQChar *s;
    ffi_type *t;

    switch (type) {
    case OT_USERPOINTER:
        sq_getuserpointer(v, idx, &p);
        return (ffi_type*)p;
    case OT_STRING:
        sq_getstring(v, idx, &s);
        t = char2ffi_type(*s);
        if (t)
            return t;
    default:
        sq_throwerror(v, "Type spec must be string or ffi_type");
        return NULL;
    }
}

typedef struct FFIFunc
{
    void *func;
    char rettype;
    ffi_cif cif;
    ffi_type *params[0];
} FFIFunc;

typedef struct FFIVar
{
    void *var;
    char type;
} FFIVar;

static SQInteger sq_lib_bind_func(HSQUIRRELVM v)
{
    void **modbuf;
    void *mod;
    void *sym;
    const SQChar *symname;
    const char *rettype;

    sq_getuserdata(v, 1, (void**)&modbuf, NULL);
    mod = *modbuf;

    sq_getstring(v, 2, &rettype);

    sq_getstring(v, 3, &symname);
    sym = GET_SYM(mod, symname);
    if (!sym)
        return sq_throwerror(v, "Cannot find symbol");

    int nparam = sq_getsize(v, 4);

    int size = sizeof(FFIFunc) + sizeof(ffi_type*) * nparam;
    FFIFunc *ffibuf = (FFIFunc*)sq_newuserdata(v, size);
    sq_push_delegate_table(v, FFI_LIB_FUNC_TAG);
    sq_setdelegate(v, -2);

//    printf("Allocated %d bytes at %p\n", size, ffibuf);
    ffibuf->func = sym;
    ffibuf->rettype = *rettype;

    int i;
    for (i = 0; i < nparam; i++) {
        sq_pushinteger(v, i);
        sq_get(v, 4);
        ffibuf->params[i] = get_ffi_type(v, -1);
        if (!ffibuf->params[i])
            return SQ_ERROR;
        sq_poptop(v);
    }
    int res = ffi_prep_cif(&ffibuf->cif, FFI_DEFAULT_ABI, nparam, char2ffi_type(*rettype), ffibuf->params);
    if (res != FFI_OK)
        return sq_throwerror(v, "Error in ffi_prep_cif");
    return 1;
}

static SQInteger sq_lib_bind_var(HSQUIRRELVM v)
{
    void **modbuf;
    void *mod;
    void *sym;
    const SQChar *symname;
    const char *type;

    sq_getuserdata(v, 1, (void**)&modbuf, NULL);
    mod = *modbuf;

    sq_getstring(v, 2, &type);

    sq_getstring(v, 3, &symname);
    sym = GET_SYM(mod, symname);
    if (!sym)
        return sq_throwerror(v, "Cannot find symbol");

    FFIVar *ffibuf = (FFIVar*)sq_newuserdata(v, sizeof(FFIVar));
    sq_push_delegate_table(v, FFI_LIB_VAR_TAG);
    sq_setdelegate(v, -2);

    ffibuf->var = sym;
    ffibuf->type = *type;

    return 1;
}

#define METAMETHOD
#ifdef METAMETHOD
// For metamethod, we have userdata, then delegate table
#define EXTRA_PARAMS 2
#else
// For normal method, there's only userdata
#define EXTRA_PARAMS 1
#endif

static void return_ffi_value(HSQUIRRELVM v, ptrdiff_t val, char type)
{
    switch (type) {
    case 's':
        sq_pushstring(v, (char*)val, -1);
        break;
    default:
        sq_pushinteger(v, val);
    }
}

static SQInteger sq_func__call(HSQUIRRELVM v)
{
    FFIFunc *ffibuf;
    sq_getuserdata(v, 1, (void**)&ffibuf, NULL);
    int top = sq_gettop(v);
//    printf("ffibuf %p top %d\n", ffibuf, top);

    if (ffibuf->cif.nargs != ((unsigned)(top - EXTRA_PARAMS)))
        return sq_throwerror(v, "Wrong number of args");

    SQInteger values[top - EXTRA_PARAMS];
    void *valueptrs[top - EXTRA_PARAMS];
    int i;
    for (i = EXTRA_PARAMS + 1; i <= top; i++) {
        #define pi (i - (EXTRA_PARAMS + 1))
        switch (sq_gettype(v, i)) {
        case OT_INTEGER:
            sq_getinteger(v, i, (SQInteger*)&values[pi]);
            break;
        case OT_STRING:
            sq_getstring(v, i, (const char**)&values[pi]);
            break;
        case OT_INSTANCE: {
            if (SQ_FAILED(sqstd_getblob(v, i, (SQUserPointer*)&values[pi])))
                return SQ_ERROR;
            break;
        }
        default:
            return sq_throwerror(v, "Unimplemented type");
        }
        valueptrs[pi] = &values[pi];
    }

    ptrdiff_t rc;
//    printf("Before call, %p\n", ffibuf->func);
    ffi_call(&ffibuf->cif, (void(*)())ffibuf->func, &rc, valueptrs);
    return_ffi_value(v, rc, ffibuf->rettype);
    return 1;
}

static SQInteger sq_var_get(HSQUIRRELVM v)
{
    FFIVar *ffibuf;
    sq_getuserdata(v, 1, (void**)&ffibuf, NULL);
    void *pval = ffibuf->var;
    switch (ffibuf->type) {
    case 's':
        sq_pushstring(v, *(char**)pval, -1);
        break;
    case 'i':
        sq_pushinteger(v, *(int*)pval);
        break;
    case 'I':
        sq_pushinteger(v, *(unsigned int*)pval);
        break;
    case 'h':
        sq_pushinteger(v, *(short*)pval);
        break;
    case 'H':
        sq_pushinteger(v, *(unsigned short*)pval);
        break;
    case 'l':
        sq_pushinteger(v, *(long*)pval);
        break;
    case 'L':
        sq_pushinteger(v, *(unsigned long*)pval);
        break;
    case 'b':
        sq_pushinteger(v, *(char*)pval);
        break;
    case 'B':
        sq_pushinteger(v, *(unsigned char*)pval);
        break;
    default:
        sq_pushinteger(v, *(int*)pval);
    }
    return 1;
}

static SQInteger sq_ffi_setdelegate(HSQUIRRELVM v)
{
    if (SQ_FAILED(sq_setdelegate(v, 2)))
        return SQ_ERROR;
    return 0;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_ffi_##name,nparams,tycheck}
static SQRegFunction sq_ffi_methods[] =
{
	_DECL_FUNC(load,  2, _SC(".s")),
	_DECL_FUNC(sym,  3, _SC(".ps")),
	_DECL_FUNC(setdelegate,  3, _SC("..t")),
	{0,0}
};
#undef _DECL_FUNC

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_lib_##name,nparams,tycheck}
static SQRegFunction sq_lib_methods[] =
{
	_DECL_FUNC(bind_func,  4, _SC("ussa")),
	_DECL_FUNC(bind_var,  3, _SC("uss")),
	{0,0}
};
#undef _DECL_FUNC

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_func_##name,nparams,tycheck}
static SQRegFunction sq_func_methods[] =
{
	_DECL_FUNC(_call,  -1, _SC("u")),
	{0,0}
};
#undef _DECL_FUNC

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_var_##name,nparams,tycheck}
static SQRegFunction sq_var_methods[] =
{
	_DECL_FUNC(get,  1, _SC("u")),
	{0,0}
};
#undef _DECL_FUNC

struct FFI_type_name {
    const char* name;
    ffi_type *type;
};

static struct FFI_type_name ffi_types_wrap[] = {
    {"void",    &ffi_type_void},
    {"schar",   &ffi_type_schar},
    {"uchar",   &ffi_type_uchar},
    {"sshort",  &ffi_type_sshort},
    {"ushort",  &ffi_type_ushort},
    {"sint",    &ffi_type_sint},
    {"uint",    &ffi_type_uint},
    {"slong",   &ffi_type_slong},
    {"ulong",   &ffi_type_ulong},
    {"float",   &ffi_type_float},
    {"double",  &ffi_type_double},
    {"pointer", &ffi_type_pointer},
    {NULL}
};


#ifdef __cplusplus
extern "C" {
#endif

static inline void sq_create_delegate_table(HSQUIRRELVM vm, SQRegFunction *methods, HSQOBJECT *handle)
{
    sq_newtable(vm);
    sq_insert_reg_funcs(vm, methods);
    sq_resetobject(handle);
    sq_getstackobj(vm, -1, handle);
    sq_addref(vm, handle);
}

SQRESULT sqext_register_ffi(HSQUIRRELVM v)
{
    int saved_top = sq_gettop(v);
    //add a namespace ffi
	sq_pushstring(v,_SC("ffi"),-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)FFI_LIB_TAG);
    sq_insert_reg_funcs(v, sq_ffi_methods);

    int i;
    for (i = 0; ffi_types_wrap[i].name != 0; i++) {
        struct FFI_type_name *e = &ffi_types_wrap[i];
        sq_pushstring(v, e->name, -1);
        sq_pushuserpointer(v, e->type);
        sq_newslot(v, -3, SQFalse);
    }
    sq_newslot(v,-3,SQTrue); //add ffi table to the root table


    sq_create_delegate_table(v, sq_lib_methods, FFI_LIB_LIB_TAG);
    sq_create_delegate_table(v, sq_func_methods, FFI_LIB_FUNC_TAG);
    sq_create_delegate_table(v, sq_var_methods, FFI_LIB_VAR_TAG);

    sq_settop(v, saved_top);

    return SQ_OK;
}

#ifdef __cplusplus
}
#endif

#endif //WITH_FFI
