/*
    see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#ifndef SQ_EXCLUDE_DEFAULT_MEMFUNCTIONS
#ifdef SQ_DEBUG_MEMORY
int _sq_total_malloc = 0;
int _sq_total_realloc = 0;
int _sq_total_free = 0;
#endif
void *sq_vm_malloc(SQUnsignedInteger size){
#ifdef SQ_DEBUG_MEMORY
    ++_sq_total_malloc;
#endif
    return malloc(size);
}

void *sq_vm_realloc(void *p, SQUnsignedInteger SQ_UNUSED_ARG(oldsize), SQUnsignedInteger size){
#ifdef SQ_DEBUG_MEMORY
    ++_sq_total_realloc;
#endif
    return realloc(p, size);
}

void sq_vm_free(void *p, SQUnsignedInteger SQ_UNUSED_ARG(size)){
#ifdef SQ_DEBUG_MEMORY
    ++_sq_total_free;
#endif
    free(p);
}
#endif

#ifdef CUSTOM_DELETE_OPERATOR
//made public to allow link without libstdc++
void operator delete(void *p, unsigned int len)
{
	printf("DELETE SHOULD NOT BE CALLED %p : %d\n", p, (int)len);
	assert(0);
}
void operator delete(void *p, unsigned long len)
{
	printf("DELETE SHOULD NOT BE CALLED %p : %d\n", p, (int)len);
	assert(0);
}
void operator delete(void *p, unsigned long long len)
{
	printf("DELETE SHOULD NOT BE CALLED %p : %d\n", p, (int)len);
	assert(0);
}
void operator delete(void *p)
{
	printf("DELETE SHOULD NOT BE CALLED %p\n", p);
	assert(0);
}
#endif

#ifdef WITH_SQSTDCPP
#include <stdio.h>
#include <stdlib.h>

// MSVC uses __cdecl calling convention for new/delete :-O
#ifdef _MSC_VER
#  define NEWDEL_CALL __cdecl
#else
#  define NEWDEL_CALL
#endif

extern "C" void __cxa_pure_virtual ()
{
    puts("__cxa_pure_virtual called\n");
    abort ();
}

void * NEWDEL_CALL operator new (size_t size)
{
    void *p = malloc (size);
    if(!p)
    {
        puts("not enough memory\n");
        abort ();
    }
    return p;
}

void * NEWDEL_CALL operator new [] (size_t size)
{
    return ::operator new(size);
}

void NEWDEL_CALL operator delete (void *p)
{
    if (p) free (p);
}

void NEWDEL_CALL operator delete [] (void *p)
{
    if (p) free (p);
}

void NEWDEL_CALL operator delete (void *p, size_t)
{
    if (p) free (p);
}
#endif // WITH_MYSTDCPP
