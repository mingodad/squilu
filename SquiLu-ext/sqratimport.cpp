//
// SqratImport: Supports importing of squirrel modules
//

//
// Copyright (c) 2009 Brandon Jones
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//  claim that you wrote the original software. If you use this software
//  in a product, an acknowledgment in the product documentation would be
//  appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be
//  misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//  distribution.
//

#include "sqratimport.h"
#include "sqmodule.h"

//#include "sqratlib/sqratBase.h"
#include <stdio.h>
#include <sqstdio.h>

#if defined(_WIN32)

#include <windows.h>

#elif defined(__unix)

#include <dlfcn.h>

#endif

typedef SQRESULT (*SQMODULELOAD)(HSQUIRRELVM v, HSQAPI sq);

static HSQAPI sqapi = NULL;

// Create and populate the HSQAPI structure with function pointers
// If new functions are added to the Squirrel API, they should be added here too
static HSQAPI sqrat_newapi() {
    HSQAPI sq = (HSQAPI)sq_malloc(sizeof(sq_api));

#define SQUIRREL_API_FUNC(fret, fname, fargs) sq->sq_##fname = sq_##fname;
#include "sqapi.h"
#undef SQUIRREL_API_FUNC

    return sq;
}

static void sqrat_deleteapi(HSQAPI sq) {
    sq_free(sq, sizeof(sq_api));
}

static SQRESULT sqrat_importscript(HSQUIRRELVM v, const SQChar* moduleName) {
    if(SQ_FAILED(sqstd_loadfile(v, moduleName, true, true))) {
        char filename[1024];
        scsprintf(filename, sizeof(filename), "%s.nut", moduleName);
        if(SQ_FAILED(sqstd_loadfile(v, filename, true, true))) {
            return SQ_ERROR;
        }
    }
    sq_push(v, -2);
    sq_call(v, 1, false, true);
    return SQ_OK;
}

static SQRESULT sqrat_importbin(HSQUIRRELVM v, const SQChar* moduleName) {
    SQMODULELOAD modLoad = 0;

#if defined(_WIN32)
    HMODULE mod;
    mod = GetModuleHandleA(moduleName);
    if(mod == NULL) {
        mod = LoadLibraryA(moduleName);
        if(mod == NULL) {
            return SQ_ERROR;
        }
    }

    modLoad = (SQMODULELOAD)GetProcAddressA(mod, "sqmodule_load");
    if(modLoad == NULL) {
        FreeLibrary(mod);
        return SQ_ERROR;
    }
#elif defined(__unix)
    /* adding .so to moduleName? */
    void *mod = dlopen(moduleName, RTLD_NOW | RTLD_LOCAL
#ifndef ANDROID_BUILD
                       | RTLD_NOLOAD //RTLD_NOLOAD flag is not specified in POSIX.1-2001..so not the best solution :(
#endif
                       );
    if (mod == NULL) {
        mod = dlopen(moduleName, RTLD_NOW | RTLD_LOCAL);
        if (mod == NULL)
            return SQ_ERROR;
    }
    modLoad = (SQMODULELOAD) dlsym(mod, "sqmodule_load");
    if (modLoad == NULL) {
        dlclose(mod);
        return SQ_ERROR;
    }
#endif

    if(sqapi == NULL) {
        sqapi = sqrat_newapi(); // Caching this for multiple imports is probably a very good idea
    }

    SQRESULT res = modLoad(v, sqapi);

    return res;
}

SQRESULT sqrat_import(HSQUIRRELVM v) {
    const SQChar* moduleName;
    HSQOBJECT table;
    SQRESULT res = SQ_OK;

    //SQInteger top = sq_gettop(v);
    sq_getstring(v, -2, &moduleName);
    sq_getstackobj(v, -1, &table);
    sq_addref(v, &table);

    sq_settop(v, 0); // Clear Stack
    sq_pushobject(v, table); // Push the target table onto the stack

    if(SQ_FAILED(sqrat_importscript(v, moduleName))) {
        res = sqrat_importbin(v, moduleName);
    }

    sq_settop(v, 0); // Clean up the stack (just in case the module load leaves it messy)
    sq_pushobject(v, table); // return the target table
    sq_release(v, &table);

    return res;
}

static SQRESULT sqratbase_import(HSQUIRRELVM v) {
    SQInteger args = sq_gettop(v);
    switch(args) {
    case 2:
        sq_pushroottable(v);
        break;
    case 3:
        // should already have the desired table pushed onto the stack
        if(sq_gettype(v, 3) != OT_TABLE) return sq_throwerror(v, _SC("table expected as second parameter"));
        break;
    default:
        // Error, unexpected number of arguments
        return sq_throwerror(v, _SC("unexpected number of arguments"));
        break;
    }

    sqrat_import(v);

    return 1;
}

SQRESULT sqrat_register_importlib(HSQUIRRELVM v) {
    sq_pushroottable(v);

    sq_pushstring(v, _SC("import"), -1);
    sq_newclosure(v, &sqratbase_import, 0);
    sq_newslot(v, -3, 0);

    sq_pop(v, 1); // pop root table

    return SQ_OK;
}
