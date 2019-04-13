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
void operator delete(void *p, unsigned long len)
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
