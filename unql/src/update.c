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
** Code to evaluate an UPDATE statement
*/
#include "xjd1Int.h"


/*
** pBase is a JSON structure object.  If it is not, overwrite the current
** value with an empty structure object. [[TBD: If it is not, return 0;]]
** Otherwise, lookup or insert the zField element.
*/
static JsonNode *findStructElement(JsonNode *pBase, const char *zField){
  JsonStructElem *pElem;
  if( pBase==0 ) return 0;
  if( pBase->eJType!=XJD1_STRUCT ){
//    return 0;
    xjd1JsonToNull(pBase);
    pBase->eJType = XJD1_STRUCT;
    pBase->u.st.pFirst = 0;
    pBase->u.st.pLast = 0;
  }
  for(pElem=pBase->u.st.pFirst; pElem; pElem=pElem->pNext){
    if( strcmp(pElem->zLabel, zField)==0 ){
      return pElem->pValue;
    }
  }
  pElem = xjd1_malloc( sizeof(*pElem) );
  if( pElem==0 ) return 0;
  pElem->pNext = 0;
  if( pBase->u.st.pLast==0 ){
    pBase->u.st.pFirst = pElem;
  }else{
    pBase->u.st.pLast->pNext = pElem;
  }
  pBase->u.st.pLast = pElem;
  pElem->zLabel = xjd1PoolDup(0, zField, -1);
  pElem->pValue = xjd1JsonNew(0);
  return pElem->pValue;
}

/*
** The expression p is an L-value.  Find the corresponding JsonNode.
** Create it if necessary.
*/
static JsonNode *findOrCreateJsonNode(JsonNode *pRoot, Expr *p){
  if( p==0 ) return 0;
  switch( p->eType ){
    /* The x.y operator. Here y must be an identifier, and we use its
    ** name string. The reference node is property y of object x.
    */
    case TK_DOT: {
      JsonNode *pBase = findOrCreateJsonNode(pRoot, p->u.lvalue.pLeft);
      return findStructElement(pBase, p->u.lvalue.zId);
    }

    /* The x[y] operator. The result depends on the type of value x.
    **
    ** If x is of type XJD1_STRUCT, then expression y is converted to
    ** a string. The reference node is property y of object x.
    **
    ** If x is of type XJD1_ARRAY, then expression y is converted to
    ** a number. If that number is an integer, then the reference node
    ** is element y of array x. Grow the array as needed.
    **
    ** If x is of any other type the assignment is silently ignored.
    ** Note that individual string characters can be read, but not
    ** written using the x[y] syntax.
    ** TBD: should x===null throw an error?
    */
    case TK_LB: {
      JsonNode *pBase = findOrCreateJsonNode(pRoot, p->u.bi.pLeft);
      switch( pBase->eJType ){
        case XJD1_STRUCT: {
          JsonNode *pRes;
          String idx;
          xjd1StringInit(&idx, 0, 0);
          xjd1JsonToString(xjd1ExprEval(p->u.bi.pRight), &idx);
          pRes = findStructElement(pBase, idx.zBuf);
          xjd1StringClear(&idx);
          return pRes;
        }

        case XJD1_ARRAY: {
          double rRight;
          int iIdx;
          if( xjd1JsonToReal(xjd1ExprEval(p->u.bi.pRight), &rRight) ) break;
          iIdx = (int)rRight;
          if( (double)iIdx==rRight && iIdx>=0 ){
            if( iIdx<pBase->u.ar.nElem ){
              return pBase->u.ar.apElem[iIdx];
            }else{
              JsonNode **pNewArray;
              int i;
              /* Grow the array as needed. TBD: find a way to handle very
              ** large, sparse arrays. As a temporary drafting precaution,
              ** assert that the value of iIdx is less than 1024.
              */
              assert( iIdx<1024 );
              pNewArray = xjd1_realloc(pBase->u.ar.apElem, sizeof(JsonNode*)*(iIdx+1));
              if( pNewArray==0 ) break;
              pBase->u.ar.apElem = pNewArray;
              for(i=pBase->u.ar.nElem; i<=iIdx; i++) {
                pNewArray[i] = xjd1JsonNew(0);
                pNewArray[i]->eJType = XJD1_NULL;
              }
              pBase->u.ar.nElem = iIdx+1;
              return pBase->u.ar.apElem[iIdx];
            }
          }
          break;
        }

        case XJD1_NULL: {
          /* TBD: throw error? */
          break;
        }
      }
    }
    break;

    case TK_ID: {
      return pRoot;
    }
  }
  return 0;
}

/*
** Perform an edit on a JSON value.  Return the document after the change.
*/
static void reviseOneField(
  JsonNode *pDoc,        /* The document to be edited */
  Expr *pLvalue,         /* Definition of field in document to be changed */
  Expr *pValue           /* New value for the field */
){
  JsonNode *pNode;
  JsonNode *pX;

  pNode = findOrCreateJsonNode(pDoc, pLvalue);
  if( pNode ){
    pX = xjd1JsonEdit(xjd1ExprEval(pValue));
    xjd1JsonToNull(pNode);
    *pNode = *pX;
    xjd1_free(pX);
  }
}


/*
** Evaluate an UPDATE.
*/
int xjd1UpdateStep(xjd1_stmt *pStmt){
  Command *pCmd = pStmt->pCmd;
  int rc = XJD1_OK;
  int nUpdate = 0;
  sqlite3 *db = pStmt->pConn->db;
  sqlite3_stmt *pQuery, *pReplace;
  char *zSql;
  int inAutocommit = sqlite3_get_autocommit(db);

  assert( pCmd!=0 );
  assert( pCmd->eCmdType==TK_UPDATE );
  if(inAutocommit) sqlite3_exec(db, "BEGIN", 0, 0, 0);
  zSql = sqlite3_mprintf("SELECT rowid, x FROM \"%w\"", pCmd->u.update.zName);
  sqlite3_prepare_v2(db, zSql, -1, &pQuery, 0);
  sqlite3_free(zSql);
  zSql = sqlite3_mprintf("UPDATE \"%w\" SET x=?1 WHERE rowid=?2",
                         pCmd->u.update.zName);
  sqlite3_prepare_v2(db, zSql, -1, &pReplace, 0);
  sqlite3_free(zSql);
  if( pQuery && pReplace ){
    while( SQLITE_ROW==sqlite3_step(pQuery) ){
      const char *zJson = (const char*)sqlite3_column_text(pQuery, 1);
      pStmt->pDoc = xjd1JsonParse(zJson, -1);
      if( pCmd->u.update.pWhere==0 || xjd1ExprTrue(pCmd->u.update.pWhere) ){
        JsonNode *pNewDoc;  /* Revised document content */
        ExprList *pChng;    /* List of changes */
        String jsonNewDoc;  /* Text rendering of revised document */
        int i, n;

        pNewDoc = xjd1JsonEdit(xjd1JsonRef(pStmt->pDoc));
        pChng = pCmd->u.update.pChng;
        n = pChng->nEItem;
        for(i=0; i<n-1; i += 2){
          Expr *pLvalue = pChng->apEItem[i].pExpr;
          Expr *pExpr = pChng->apEItem[i+1].pExpr;
          reviseOneField(pNewDoc, pLvalue, pExpr);
        }
        xjd1StringInit(&jsonNewDoc, 0, 0);
        xjd1JsonRender(&jsonNewDoc, pNewDoc);
        sqlite3_bind_int64(pReplace, 2, sqlite3_column_int64(pQuery, 0));
        sqlite3_bind_text(pReplace, 1, xjd1StringText(&jsonNewDoc), -1,
                          SQLITE_STATIC);
        sqlite3_step(pReplace);
        sqlite3_reset(pReplace);
        xjd1StringClear(&jsonNewDoc);
        xjd1JsonFree(pNewDoc);
        nUpdate++;
      }
      xjd1JsonFree(pStmt->pDoc);
      pStmt->pDoc = 0;
    }
  }
  sqlite3_finalize(pQuery);
  sqlite3_finalize(pReplace);

  if( pCmd->u.update.pUpsert ){
    if( nUpdate==0 ){
      JsonNode *pToIns;
      String jsonToIns;
      pToIns = xjd1ExprEval(pCmd->u.update.pUpsert);
      xjd1StringInit(&jsonToIns, 0, 0);
      xjd1JsonRender(&jsonToIns, pToIns);
      xjd1JsonFree(pToIns);
      zSql = sqlite3_mprintf(
           "INSERT INTO \"%w\" VALUES('%q')",
           pCmd->u.update.zName, xjd1StringText(&jsonToIns));
      xjd1StringClear(&jsonToIns);
      sqlite3_exec(db, zSql, 0, 0, 0);
      sqlite3_free(zSql);
    }
  }
  if(inAutocommit) sqlite3_exec(db, "COMMIT", 0, 0, 0);
  return rc;
}
