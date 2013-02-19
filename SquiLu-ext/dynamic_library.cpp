// dynamic_library.cpp
//

#include "dynamic_library.h"
#include <stdarg.h>

#ifdef WIN32
#include <windows.h>
#define CLOSE_LIB(lib) (FreeLibrary((HINSTANCE)lib) != 0)
#define OPEN_LIB(lib) (void*)LoadLibrary(lib)
#define GET_SYM(lib, sn) GetProcAddress((HINSTANCE)lib, sn)
typedef void* (WINAPI*cPtrFuncVarArg)(...);
#else
#include <dlfcn.h>
#define CLOSE_LIB(lib) (::dlclose(lib) == 0)
#define OPEN_LIB(lib) ::dlopen(lib,RTLD_LAZY)
#define GET_SYM(lib, sn) ::dlsym(lib, sn)
typedef void*(*cPtrFuncVarArg)(...);
#endif

DynamicLibrary::DynamicLibrary ()
{
    handleLib=0;
}

DynamicLibrary::~ DynamicLibrary ()
{
    close();
}

bool DynamicLibrary::open (char const * libname)
{
    //should first close the opened library if any
    if(handleLib) return false;
    handleLib=OPEN_LIB(libname);
    return (handleLib != 0);
}

bool DynamicLibrary::close ()
{
    if(handleLib)
    {
        if( CLOSE_LIB(handleLib) )
        {
            handleLib = 0;
            return true;
        }
    }
    return false;
}

void * DynamicLibrary::dlsym (char const * sym_name)
{
    if(handleLib) return (void*)GET_SYM(handleLib,sym_name);
    else return 0;
}
