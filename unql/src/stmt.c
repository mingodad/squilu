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
** Interfaces to prepared statment objects.
*/
#include "xjd1Int.h"

/*
** Create a new prepared statement for database connection pConn.  The
** program code to be parsed is zStmt.  Return the new statement in *ppNew.
** Write the number of zStmt bytes read into *pN.
*/
int xjd1_stmt_new(xjd1 *pConn, const char *zStmt, xjd1_stmt **ppNew, int *pN){
  xjd1_stmt *p;
  int dummy;
  Command *pCmd;
  int rc;

  if( pN==0 ) pN = &dummy;
  *pN = strlen(zStmt);
  *ppNew = p = xjd1_malloc( sizeof(*p) );
  if( p==0 ) return XJD1_NOMEM;
  memset(p, 0, sizeof(*p));
  p->pConn = pConn;
  p->pNext = pConn->pStmt;
  pConn->pStmt = p;
  p->zCode = xjd1PoolDup(&p->sPool, zStmt, -1);
  xjd1StringInit(&p->retValue, &p->sPool, 0);
  xjd1StringInit(&p->errMsg, &p->sPool, 0);

  rc = xjd1RunParser(pConn, p, p->zCode, pN);
  pCmd = p->pCmd;
  assert( rc==XJD1_OK || (pCmd==0 && pConn->errCode==rc) );

  if( pCmd ){
    switch( pCmd->eCmdType ){
      case TK_SELECT: {
        rc = xjd1QueryInit(pCmd->u.q.pQuery, p, 0);
        break;
      }
      case TK_INSERT: {
        xjd1QueryInit(pCmd->u.ins.pQuery, p, 0);
        break;
      }
      case TK_DELETE: {
        xjd1ExprInit(pCmd->u.del.pWhere, p, 0, 0, 0);
        break;
      }
      case TK_UPDATE: {
        xjd1ExprInit(pCmd->u.update.pWhere, p, 0, 0, 0);
        xjd1ExprListInit(pCmd->u.update.pChng, p, 0, 0, 0);
        xjd1ExprInit(pCmd->u.update.pUpsert, p, 0, 0, 0);
        break;
      }
    }

    if( p->errCode ){
      xjd1Error(pConn, p->errCode, "%s", p->errMsg.zBuf);
      rc = p->errCode;
    }else if( rc!=XJD1_OK ){
      xjd1Error(pConn, rc, 0);
    }
  }

  if( rc!=XJD1_OK ){
    xjd1_stmt_delete(p);
    *ppNew = 0;
  }
  return rc;
}

/*
** Configure a prepared statement.
*/
int xdj1_stmt_config(xjd1_stmt *pStmt, int op, ...){
  return XJD1_UNKNOWN;
}

/*
** Delete a prepared statement.
*/
int xjd1_stmt_delete(xjd1_stmt *pStmt){
  Command *pCmd;
  if( pStmt==0 ) return XJD1_OK;
  pStmt->isDying = 1;
  if( pStmt->nRef>0 ) return XJD1_OK;

  pCmd = pStmt->pCmd;
  if( pCmd ){
    switch( pCmd->eCmdType ){
      case TK_SELECT: {
        xjd1QueryClose(pCmd->u.q.pQuery);
        break;
      }
      case TK_INSERT: {
        xjd1QueryClose(pCmd->u.ins.pQuery);
        break;
      }
      case TK_DELETE: {
        xjd1ExprClose(pCmd->u.del.pWhere);
        break;
      }
      case TK_UPDATE: {
        xjd1ExprClose(pCmd->u.update.pWhere);
        xjd1ExprListClose(pCmd->u.update.pChng);
        xjd1ExprClose(pCmd->u.update.pUpsert);
        break;
      }
    }
  }

  if( pStmt->pPrev ){
    pStmt->pPrev->pNext = pStmt->pNext;
  }else{
    //assert( pStmt->pConn->pStmt==pStmt ); wrong assumption
    pStmt->pConn->pStmt = pStmt->pNext;
  }
  if( pStmt->pNext ){
    pStmt->pNext->pPrev = pStmt->pPrev;
  }
  xjd1Unref(pStmt->pConn);
  xjd1PoolClear(&pStmt->sPool);
  xjd1StringClear(&pStmt->retValue);
  xjd1_free(pStmt);
  return XJD1_OK;
}

/*
** Execute a prepared statement up to its next return value or until
** it completes.
*/
int xjd1_stmt_step(xjd1_stmt *pStmt){
  Command *pCmd;
  int rc = XJD1_DONE;
  if( pStmt==0 ) return rc;
  pCmd = pStmt->pCmd;
  if( pCmd==0 ) return rc;
  switch( pCmd->eCmdType ){
    case TK_CREATECOLLECTION: {
      char *zSql;
      int res;
      char *zErr = 0;
      zSql = sqlite3_mprintf("CREATE TABLE %s \"%w\"(x)",
                 pCmd->u.crtab.ifExists ? "IF NOT EXISTS" : "",
                 pCmd->u.crtab.zName);
      res = sqlite3_exec(pStmt->pConn->db, zSql, 0, 0, &zErr);
      if( zErr ){
        xjd1Error(pStmt->pConn, XJD1_ERROR, "%s", zErr);
        sqlite3_free(zErr);
        rc = XJD1_ERROR;
      }
      sqlite3_free(zSql);
      break;
    }
    case TK_DROPCOLLECTION: {
      char *zSql;
      int res;
      char *zErr = 0;
      zSql = sqlite3_mprintf("DROP TABLE %s \"%w\"",
                 pCmd->u.crtab.ifExists ? "IF EXISTS" : "",
                 pCmd->u.crtab.zName);
      res = sqlite3_exec(pStmt->pConn->db, zSql, 0, 0, &zErr);
      if( zErr ){
        xjd1Error(pStmt->pConn, XJD1_ERROR, "%s", zErr);
        sqlite3_free(zErr);
        rc = XJD1_ERROR;
      }
      sqlite3_free(zSql);
      break;
    }
    case TK_INSERT: {
      JsonNode *pNode;
      String json;
      int res;
      char *zErr;
      char *zSql;
      if( pCmd->u.ins.pQuery ){
        xjd1Error(pStmt->pConn, XJD1_ERROR, 
                 "INSERT INTO ... SELECT not yet implemented");
        break;
      }
      pNode = xjd1ExprEval(pCmd->u.ins.pValue);
      if( pNode==0 ) break;
      xjd1StringInit(&json,0,0);
      xjd1JsonRender(&json, pNode);
      xjd1JsonFree(pNode);
      zSql = sqlite3_mprintf("INSERT INTO \"%w\" VALUES('%q')",
                  pCmd->u.ins.zName,
                  xjd1StringText(&json));
      xjd1StringClear(&json);
      res = sqlite3_exec(pStmt->pConn->db, zSql, 0, 0, &zErr);
      if( zErr ){
        xjd1Error(pStmt->pConn, XJD1_ERROR, "%s", zErr);
        sqlite3_free(zErr);
        rc = XJD1_ERROR;
      }
      sqlite3_free(zSql);
      break;
    }
    case TK_SELECT: {
      Query *pQuery = pCmd->u.q.pQuery;
      rc = xjd1QueryStep(pQuery);
      xjd1StringClear(&pStmt->retValue);
      if( rc==XJD1_ROW ){
        JsonNode *pValue = xjd1QueryDoc(pQuery, 0);
        xjd1JsonRender(&pStmt->retValue, pValue);
        xjd1JsonFree(pValue);
        pStmt->okValue = 1;
      }else{
        pStmt->okValue = 0;
      }
      break;
    }
    case TK_DELETE: {
      rc = xjd1DeleteStep(pStmt);
      break;
    }
    case TK_UPDATE: {
      rc = xjd1UpdateStep(pStmt);
      break;
    }
    case TK_PRAGMA: {
      rc = xjd1PragmaStep(pStmt);
      break;
    }
  }
  return rc;
}

/*
** Rewind a prepared statement back to the beginning.
*/
int xjd1_stmt_rewind(xjd1_stmt *pStmt){
  Command *pCmd = pStmt->pCmd;
  if( pCmd ){
    switch( pCmd->eCmdType ){
      case TK_SELECT: {
        xjd1QueryRewind(pCmd->u.q.pQuery);
        xjd1StringTruncate(&pStmt->retValue);
        pStmt->okValue = 0;
        break;
      }
      case TK_INSERT: {
        xjd1QueryRewind(pCmd->u.ins.pQuery);
        break;
      }
    }
  }
  return XJD1_OK;
}

/*
** Return the output value of a prepared statement resulting from
** its most recent xjd1_stmt_step() call.
*/
int xjd1_stmt_value(xjd1_stmt *pStmt, const char **pzValue){
  if( pStmt==0 ) return XJD1_MISUSE;
  *pzValue = pStmt->retValue.zBuf;
  return XJD1_OK;
}

/*
** Construct a human-readable listing of a prepared statement showing
** its internal structure.  Used for debugging and analysis only.
**
** The return string is obtained from xjd1_malloc and must be freed by
** the caller.
*/
char *xjd1_stmt_debug_listing(xjd1_stmt *p){
  String x;
  xjd1StringInit(&x, 0, 0);
  xjd1TraceCommand(&x, 0, p->pCmd);
  return x.zBuf;
}

/*
** Return the current value for a particular document in the given
** statement.
**
** The caller is responsible for invoking xjd1JsonFree() on the result.
*/
JsonNode *xjd1StmtDoc(xjd1_stmt *pStmt){
  Command *pCmd;
  JsonNode *pRes = 0;
  if( pStmt==0 ) return 0;
  pCmd = pStmt->pCmd;
  if( pCmd==0 ) return 0;
  switch( pCmd->eCmdType ){
    case TK_SELECT: {
      pRes = xjd1QueryDoc(pCmd->u.q.pQuery, 0);
      break;
    }
    case TK_UPDATE:
    case TK_DELETE: {
      pRes = xjd1JsonRef(pStmt->pDoc);
      break;
    }
  }
  return pRes;
}

void xjd1StmtError(xjd1_stmt *pStmt, int errCode, const char *zFormat, ...){
  va_list ap;
  pStmt->errCode = errCode;
  va_start(ap, zFormat);
  xjd1StringVAppendF(&pStmt->errMsg, zFormat, ap);
  va_end(ap);
}

