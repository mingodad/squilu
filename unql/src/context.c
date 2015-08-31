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
** Execution context
*/
#include "xjd1Int.h"


int xjd1_context_new(xjd1_context **ppNew){
  xjd1_context *p;

  *ppNew = p = xjd1_malloc( sizeof(*p) );
  if( p ){
    memset(p, 0, sizeof(*p));
    return XJD1_OK;
  }else{
    return XJD1_NOMEM;
  }
}
int xjd1_context_config(xjd1_context *p, int op, ...){
  return XJD1_UNKNOWN;
}
int xjd1_context_delete(xjd1_context *p){
  if( p==0 ) return XJD1_OK;
  p->isDying = 1;
  if( p->nRef>0 ) return XJD1_OK;
  xjd1_free(p);
  return XJD1_OK;
}

PRIVATE void xjd1ContextUnref(xjd1_context *p){
  if( p==0 ) return;
  p->nRef--;
  if( p->nRef<=0 && p->isDying ) xjd1_context_delete(p);
}
