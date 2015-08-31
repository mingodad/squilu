/*
** Copyright (c) 2011 D. Richard Hipp
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the Simplified BSD License (also
** known as the "2-Clause License" or "FreeBSD License".)
**
** This program is distributed in the hope that it will be useful,
** but without any warranty; without even the implied warranty of
** merchantability or fitness for a particular purpose.
**
** Author contact information:
**   drh@hwaci.com
**   http://www.hwaci.com/drh/
**
*************************************************************************
** Pooled memory allocation
*/
#include "xjd1Int.h"

static void *dflt_malloc(int N){ return malloc(N); }
static void dflt_free(void *p){ free(p); }
static void *dflt_realloc(void *p, int N){ return realloc(p, N); }

static struct {
  void *(*xMalloc)(int);
  void *(*xRealloc)(void *, int);
  void (*xFree)(void *);
} global = {
  dflt_malloc,
  dflt_realloc,
  dflt_free
};

void xjd1_configure_malloc(
  void *(*xMalloc)(int),
  void *(*xRealloc)(void *, int),
  void (*xFree)(void *)
){
  global.xMalloc = xMalloc;
  global.xRealloc = xRealloc;
  global.xFree = xFree;
}

void *xjd1_malloc(int N){
  return global.xMalloc(N);
}
void xjd1_free(void *p){
  global.xFree(p);
}
void *xjd1_realloc(void *p, int N){
  return global.xRealloc(p, N);
}

void *xjd1MallocZero(int N){
  void *pRet;
  pRet = global.xMalloc(N);
  if( pRet ) memset(pRet, 0, N);
  return pRet;
}


/*
** Create a new memory allocation pool.  Return a pointer to the
** memory pool or NULL on OOM error.
*/
Pool *xjd1PoolNew(void){
  Pool *p = xjd1_malloc( sizeof(*p) );
  if( p ) memset(p, 0, sizeof(*p));
  return p;
}

/*
** Free a memory allocation pool allocated using xjd1PoolNew().
*/
void xjd1PoolDelete(Pool *p){
  if( p ){
    xjd1PoolClear(p);
    xjd1_free(p);
  }
}

/*
** Clear a memory allocation pool.  That is to say, xjd1_free all the
** memory allocations associated with the pool, though do not xjd1_free
** the memory pool itself.
*/
void xjd1PoolClear(Pool *p){
  PoolChunk *pChunk, *pNext;
  for(pChunk = p->pChunk; pChunk; pChunk = pNext){
    pNext = pChunk->pNext;
    xjd1_free(pChunk);
  }
  memset(p, 0, sizeof(*p));
}

/*
** Allocate N bytes of memory from the memory allocation pool.
*/
#define POOL_CHUNK_SIZE 3000
void *xjd1PoolMalloc(Pool *p, int N){
  N = (N+7)&~7;
  if( N>POOL_CHUNK_SIZE/4 ){
    PoolChunk *pChunk = xjd1_malloc( N + 8 );
    if( pChunk==0 ) return 0;
    pChunk->pNext = p->pChunk;
    p->pChunk = pChunk;
    return &((char*)pChunk)[8];
  }else{
    void *x;
    if( p->nSpace<N ){
      PoolChunk *pChunk = xjd1_malloc( POOL_CHUNK_SIZE + 8 );
      if( pChunk==0 ) return 0;
      pChunk->pNext = p->pChunk;
      p->pChunk = pChunk;
      p->pSpace = (char*)pChunk;
      p->pSpace += 8;
      p->nSpace = POOL_CHUNK_SIZE;
    }
    x = p->pSpace;
    p->pSpace += N;
    p->nSpace -= N;
    return x;
  }
}
void *xjd1PoolMallocZero(Pool *p, int N){
  void *x = xjd1PoolMalloc(p,N);
  if( x ) memset(x, 0, (N+7)&~7);
  return x;
}

/*
** Create a duplicate of a string using a memory pool.  Or if pPool==0
** obtain the memory for the duplicate from xjd1_malloc().
*/
char *xjd1PoolDup(Pool *pPool, const char *z, int n){
  char *zOut;
  if( n<0 ) n = xjd1Strlen30(z);
  if( pPool ){
    zOut = xjd1PoolMalloc(pPool, n+1);
  }else{
    zOut = xjd1_malloc( n+1 );
  }
  if( zOut ){
     memcpy(zOut, z, n);
     zOut[n] = 0;
  }
  return zOut;
}
