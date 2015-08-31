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
** Main interfaces
*/
#include "xjd1Int.h"

/*
** Open a new database connection
*/
int xjd1_open_with_db(xjd1_context *pContext, sqlite3 *db, xjd1 **ppNewConn){
  xjd1 *pConn;

  *ppNewConn = pConn = xjd1_malloc( sizeof(*pConn) );
  if( pConn==0 ) return XJD1_NOMEM;
  memset(pConn, 0, sizeof(*pConn));
  pConn->pContext = pContext;
  pConn->db = db;
  pConn->isSQLite3Borrowed = 1;
  return XJD1_OK;
}

int xjd1_open(xjd1_context *pContext, const char *zURI, xjd1 **ppNewConn){
  sqlite3 *db;
  int rc;

  rc = sqlite3_open_v2(zURI, &db,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI, 0);
  if( rc ){
    sqlite3_close(db);
    return XJD1_ERROR_OPEN_DB;
  }
  rc = xjd1_open_with_db(pContext, db, ppNewConn);
  if(rc != XJD1_OK){
    sqlite3_close(db);
    return rc;
  }
  (*ppNewConn)->isSQLite3Borrowed = 0;
  return rc;
}


/* Configure a database connection */
int xjd1_config(xjd1 *pConn, int op, ...){
  int rc = XJD1_UNKNOWN;
  va_list ap;
  va_start(ap, op);
  switch( op ){
    case XJD1_CONFIG_PARSERTRACE: {
      pConn->parserTrace = va_arg(ap, int);
      rc = XJD1_OK;
      break;
    }
    default: {
      break;
    }
  }
  va_end(ap);
  return rc;
}

/* Close a database connection.  The close does not actually
** occur until all references to the connection also close.  This
** means that any prepared statements must also be closed.
*/
int xjd1_close(xjd1 *pConn){
  if( pConn==0 ) return XJD1_OK;
  pConn->isDying = 1;
  if( pConn->nRef>0 ) return XJD1_OK;
  xjd1ContextUnref(pConn->pContext);
  if(!pConn->isSQLite3Borrowed) sqlite3_close(pConn->db);
  xjd1StringClear(&pConn->errMsg);
  xjd1_free(pConn);
  return XJD1_OK;
}

/*
** Report the most recent error.
*/
int xjd1_errcode(xjd1 *pConn){
  return pConn ? pConn->errCode : XJD1_ERROR;
}
const char *xjd1_errmsg(xjd1 *pConn){
  if( pConn==0 ) return "out of memory";
  return xjd1StringText(&pConn->errMsg);
}

const char *xjd1_errcode_name(xjd1 *pConn){
  const char *z = "???";
  switch( xjd1_errcode(pConn) ){
    case XJD1_OK:        z = "OK";         break;
    case XJD1_ERROR:     z = "ERROR";      break;
    case XJD1_MISUSE:    z = "MISUSE";     break;
    case XJD1_NOMEM:     z = "NOMEM";      break;
    case XJD1_UNKNOWN:   z = "UNKNOWN";    break;
    case XJD1_SYNTAX:    z = "SYNTAX";     break;
    case XJD1_ROW:       z = "ROW";        break;
    case XJD1_DONE:      z = "DONE";       break;
  }
  return z;
}

/* Remove a reference to a database connection.  When the last reference
** is removed and the database is closed, then memory is deallocated.
*/
PRIVATE void xjd1Unref(xjd1 *pConn){
  pConn->nRef--;
  if( pConn->nRef<=0 && pConn->isDying ) xjd1_close(pConn);
}

/*
** Change the error message and error code.
*/
PRIVATE void xjd1Error(xjd1 *pConn, int errCode, const char *zFormat, ...){
  va_list ap;
  if( pConn==0 ) return;
  if( !pConn->appendErr ){
    xjd1StringTruncate(&pConn->errMsg);
  }else if( xjd1StringLen(&pConn->errMsg) ){
    xjd1StringAppend(&pConn->errMsg, "\n", 1);
  }
  pConn->errCode = errCode;
  if( zFormat ){
    va_start(ap, zFormat);
    xjd1StringVAppendF(&pConn->errMsg, zFormat, ap);
    va_end(ap);
  }else{
    const char *z;
    switch( errCode ){
      case XJD1_NOMEM: z = "out of memory";          break;
      default:         z = xjd1_errcode_name(pConn); break;
    }
    xjd1StringAppend(&pConn->errMsg, z, -1);
  }
}
