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
** This file contains code used to implement methods for the DataSrc object.
*/
#include "xjd1Int.h"


/*
** Called after statement parsing to initalize every DataSrc object.
*/
int xjd1DataSrcInit(DataSrc *p, Query *pQuery, void *pOuterCtx){
  int rc = XJD1_OK;
  p->pQuery = pQuery;
  switch( p->eDSType ){
    case TK_COMMA: {
      xjd1DataSrcInit(p->u.join.pLeft, pQuery, pOuterCtx);
      xjd1DataSrcInit(p->u.join.pRight, pQuery, pOuterCtx);
      break;
    }
    case TK_SELECT: {
      xjd1QueryInit(p->u.subq.q, pQuery->pStmt, 0);
      break;
    }
    case TK_ID: {
      char *zSql = sqlite3_mprintf("SELECT x FROM \"%w\"", p->u.tab.zName);
//printf("\n%d:%s:%s\n", __LINE__, __FILE__, zSql);
      sqlite3_prepare_v2(pQuery->pStmt->pConn->db, zSql, -1,
                         &p->u.tab.pStmt, 0);
      sqlite3_free(zSql);
      break;
    }
    case TK_FLATTENOP: {
      xjd1DataSrcInit(p->u.flatten.pNext, pQuery, pOuterCtx);
      break;
    }
    case TK_DOT: {
      rc = xjd1ExprInit(p->u.path.pPath, pQuery->pStmt, 0, 0, pOuterCtx);
      break;
    }
    case TK_NULL:                 /* Initializing a NULL DS is a no-op */
      assert( p->u.null.isDone==0 );
      break;
  }
  return rc;
}

static JsonNode *newIntValue(int i){
  JsonNode *pRet = xjd1JsonNew(0);
  pRet->eJType = XJD1_REAL;
  pRet->u.r = (double)i;
  return pRet;
}

static JsonNode *newStringValue(const char *z){
  JsonNode *pRet = xjd1JsonNew(0);
  pRet->eJType = XJD1_STRING;
  pRet->u.z = xjd1PoolDup(0, z, -1);
  return pRet;
}

/*
** Argument pPath is a an expression consisting entirely of TK_ID
** and TK_DOT noddes. e.g. "a.b.c.d". If value pVal contains the
** identified property, a pointer to the JsonStructElem that contains
** it is returned.
**
** If bCreate is not true and object pVal does not contain the specified
** property, NULL is returned. Or, if bCreate is true and pVal contains all
** but the rightmost component of the path, a new element is added to the
** object and a pointer to it returned.
*/
static JsonStructElem *findStructElem(JsonNode *pVal, Expr *pPath, int bCreate){
  JsonStructElem *pRet = 0;       /* Return value */

  assert( bCreate==0 || pVal->nRef==1 );
  assert( pPath->eType==TK_ID || pPath->eType==TK_DOT );

  if( pVal && pVal->eJType==XJD1_STRUCT ){
    JsonNode *p = 0;
    const char *zAs;
    if( pPath->eType==TK_DOT ){
      JsonStructElem *pElem;
      pElem = findStructElem(pVal, pPath->u.lvalue.pLeft, bCreate);
      if( pElem ){
        p = pElem->pValue;
        zAs = pPath->u.lvalue.zId;
      }
    }else{
      p = pVal;
      zAs = pPath->u.id.zId;
    }

    if( p && p->eJType==XJD1_STRUCT ){
      for(pRet=p->u.st.pFirst; pRet; pRet=pRet->pNext){
        if( 0==strcmp(pRet->zLabel, zAs) ) break;
      }
      if( pRet==0 && bCreate ){
        pRet = xjd1MallocZero(sizeof(*pRet));
        pRet->zLabel = xjd1PoolDup(0, zAs, -1);
        if( p->u.st.pLast ){
          p->u.st.pLast->pNext = pRet;
        }else{
          p->u.st.pFirst = pRet;
        }
        p->u.st.pLast = pRet;
      }
    }
  }

  return pRet;
}

/*
** An iterator of this type is used by FLATTEN and EACH datasources. It
** is allocated, accessed and deleted only by the functions:
**
**     flattenIterNew()
**     flattenIterNext()
**     flattenIterEntry()
**     flattenIterFree()
*/
struct FlattenIter {
  int nAlloc;                     /* Allocated size of aIter[] array */
  int nIter;                      /* Number of aIter[] array elements in use */
  int isRecursive;                /* True for FLATTEN, false for EACH */
  struct FlattenIterElem {
    JsonNode *pVal;               /* Struct or List to iterate through. */
    union {
      JsonStructElem *pElem;      /* If pVal->eJType==XJD1_STRUCT */
      int iElem;                  /* If pVal->eJType==XJD1_ARRAY */
    } current;
  } aIter[1];
};

/*
** Allocate a new iterator to iterate through value pVal. If parameter
** isRecursive is true, then the iteration descends into any contained structs
** or arrays (for the FLATTEN operator). Otherwise, the iteration is not
** recursive (used by the EACH operator).
*/
static FlattenIter *flattenIterNew(
  JsonNode *pBase,                /* Base object */
  Expr *pPath,                    /* Path to flatten or each on */
  int isRecursive                 /* True for FLATTEN, false for EACH */
){
  FlattenIter *pNew = 0;          /* New iterator object */
  JsonStructElem *pElem;

  pElem = findStructElem(pBase, pPath, 0);
  if( pElem ){
    JsonNode *pVal = pElem->pValue;
    if( pVal->eJType==XJD1_STRUCT || pVal->eJType==XJD1_ARRAY ){
      pNew = (FlattenIter *)xjd1MallocZero(sizeof(FlattenIter));
      if( pNew ){
        pNew->nAlloc = 1;
        pNew->nIter = 1;
        pNew->isRecursive = isRecursive;
        pNew->aIter[0].pVal = xjd1JsonRef(pVal);
      }
    }
  }

  return pNew;
}

static int flattenIterEntry(
  FlattenIter *pIter,             /* Iterator handle */
  JsonNode **ppKey,               /* OUT: Current key value */
  JsonNode **ppVal                /* OUT: Current payload value */
){
  struct FlattenIterElem *p = &pIter->aIter[pIter->nIter-1];

  assert( p->pVal->eJType==XJD1_STRUCT || p->pVal->eJType==XJD1_ARRAY );
  if( p->pVal->eJType==XJD1_STRUCT ){
    *ppVal = xjd1JsonRef(p->current.pElem->pValue);
  }else{
    *ppVal = xjd1JsonRef(p->pVal->u.ar.apElem[p->current.iElem-1]);
  }

  if( ppKey ){
    JsonNode *pKey = 0;
    JsonNode *pList = 0;
    int i;

    if( pIter->isRecursive ){
      pList = xjd1JsonNew(0);
      pList->eJType = XJD1_ARRAY;
      pList->u.ar.nElem = pIter->nIter;
      pList->u.ar.apElem = xjd1MallocZero(pIter->nIter*sizeof(JsonNode *));
    }

    for(i=pIter->nIter-1; i>=0; i--){
      p = &pIter->aIter[i];
      if( p->pVal->eJType==XJD1_STRUCT ){
        pKey = newStringValue(p->current.pElem->zLabel);
      }else{
        pKey = newIntValue(p->current.iElem-1);
      }
      if( pIter->isRecursive==0 ) break;
      pList->u.ar.apElem[i] = pKey;
    }

    *ppKey = (pIter->isRecursive ? pList : pKey);
  }
  return XJD1_OK;
}

static int flattenIterNext(FlattenIter **ppIter){
  FlattenIter *pIter = *ppIter;
  int rc = XJD1_DONE;
  if( pIter ){
    while( rc==XJD1_DONE && pIter->nIter ){
      struct FlattenIterElem *p = &(*ppIter)->aIter[pIter->nIter-1];
      if( p->pVal->eJType==XJD1_STRUCT ){
        if( p->current.pElem==0 ){
          p->current.pElem = p->pVal->u.st.pFirst;
        }else{
          p->current.pElem = p->current.pElem->pNext;
        }
        if( p->current.pElem ){
          rc = XJD1_ROW;
        }else{
          pIter->nIter--;
        }
      }else{
        assert( p->pVal->eJType==XJD1_ARRAY );
        p->current.iElem++;
        if( p->current.iElem<=p->pVal->u.ar.nElem ){
          rc = XJD1_ROW;
        }else{
          pIter->nIter--;
        }
      }

      if( pIter->isRecursive && rc==XJD1_ROW ){
        JsonNode *pVal;
        flattenIterEntry(pIter, 0, &pVal);
        if( pVal->eJType==XJD1_STRUCT || pVal->eJType==XJD1_ARRAY ){
          assert( pIter->nIter<=pIter->nAlloc );
          if( pIter->nIter==pIter->nAlloc ){
            int nNew = sizeof(FlattenIter) + (pIter->nAlloc*2-1)*sizeof(*p);
            FlattenIter *pNew;

            pNew = (FlattenIter *)xjd1_realloc(pIter, nNew);
            pNew->nAlloc = pNew->nAlloc * 2;
            *ppIter = pNew;
            pIter = pNew;
          }

          pIter->aIter[pIter->nIter].pVal = pVal;
          pIter->aIter[pIter->nIter].current.pElem = 0;
          pIter->aIter[pIter->nIter].current.iElem = 0;
          pIter->nIter++;
          rc = XJD1_DONE;
        }
      }
    }
  }
  return rc;
}

/*
** Free an iterator allocated by flattenIterNew().
*/
static void flattenIterFree(FlattenIter *pIter){
  if( pIter ){
    int i;
    for(i=0; i<pIter->nIter; i++){
      xjd1JsonFree(pIter->aIter[i].pVal);
    }
    xjd1_free(pIter);
  }
}



/*
** This function makes a copy of the JSON value (type XJD1_STRUCT) passed as
** the first object, adds a property to it, and returns a pointer to the copy.
** The ref-count of the returned object is 1.
**
** The name of the property added is passed as the zAs argument. The property
** is set to a structure containing two fields, "k" (value pKey) and "v"
** (value pValue). i.e. if the arguments are as follows:
**
**     pBase   = {a:a, b:b}
**     pKey    = "abc"
**     pValue  = "xyz"
**     zAs     = "c"
**
** then the returned object is:
**
**     {a:a, b:b, c:{k:"abc", v:"xyz"}}
**
** If object pBase already has a field named "zAs", then it is replaced in
** the returned copy.
**
** This function decrements the ref-count of arguments pKey and pValue. But
** not pBase.
*/
static JsonNode *flattenedObject(
  JsonNode *pBase,                /* Base object */
  JsonNode *pKey,
  JsonNode *pValue,
  Expr *pPath
){
  JsonNode *pRet;                 /* Value to return */
  JsonNode *pKV;                  /* New value for property zAs */
  JsonStructElem *pElem;

  pKV = xjd1JsonNew(0);
  pKV->eJType = XJD1_STRUCT;
  xjd1JsonInsert(pKV, "k", pKey);
  xjd1JsonInsert(pKV, "v", pValue);

  pRet = xjd1JsonEdit(xjd1JsonRef(pBase));
  pElem = findStructElem(pRet, pPath, 1);
  xjd1JsonFree(pElem->pValue);
  pElem->pValue = pKV;

  assert( pRet->nRef==1 && pRet!=pBase );
  return pRet;
}

/*
** Advance a data source to the next row. Return XJD1_DONE if the data
** source is at EOF or XJD1_ROW if the step results in a row of content
** being available.
*/
int xjd1DataSrcStep(DataSrc *p){
  int rc= XJD1_DONE;
  if( p==0 ) return XJD1_DONE;
  switch( p->eDSType ){
    case TK_COMMA: {

      if( p->u.join.bStart==0 ){
        p->u.join.bStart = 1;
        rc = xjd1DataSrcStep(p->u.join.pLeft);
        if( rc!=XJD1_ROW ) break;
      }

      rc = xjd1DataSrcStep(p->u.join.pRight);
      if( rc==XJD1_DONE ){
        rc = xjd1DataSrcStep(p->u.join.pLeft);
        if( rc==XJD1_ROW ) {
          xjd1DataSrcRewind(p->u.join.pRight);
          rc = xjd1DataSrcStep(p->u.join.pRight);
        }
      }
      break;
    }

    case TK_SELECT: {
      xjd1JsonFree(p->pValue);
      p->pValue = 0;
      rc = xjd1QueryStep(p->u.subq.q);
      p->pValue = xjd1QueryDoc(p->u.subq.q, 0);
      break;
    }

    case TK_ID: {
      rc = sqlite3_step(p->u.tab.pStmt);
      xjd1JsonFree(p->pValue);
      p->pValue = 0;
      if( rc==SQLITE_ROW ){
        const char *zJson = (const char*)sqlite3_column_text(p->u.tab.pStmt, 0);
        p->pValue = xjd1JsonParse(zJson, -1);
        rc = XJD1_ROW;
      }else{
        p->u.tab.eofSeen = 1;
        rc = XJD1_DONE;
      }
      break;
    }

    case TK_FLATTENOP: {
      xjd1JsonFree(p->pValue);
      p->pValue = 0;

      while( XJD1_ROW!=(rc = flattenIterNext(&p->u.flatten.pIter)) ){
        flattenIterFree(p->u.flatten.pIter);
        p->u.flatten.pIter = 0;

        rc = xjd1DataSrcStep(p->u.flatten.pNext);
        if( rc!=XJD1_ROW ){
          break;
        }else{
          int isRecursive = (p->u.flatten.cOpName=='F');
          JsonNode *pBase = p->u.flatten.pNext->pValue;
          Expr *pPath = p->u.flatten.pExpr;

          p->u.flatten.pIter = flattenIterNew(pBase, pPath, isRecursive);
        }
      }

      if( rc==XJD1_ROW ){
        JsonNode *pKey = 0;
        JsonNode *pValue = 0;
        JsonNode *pBase = p->u.flatten.pNext->pValue;
        flattenIterEntry(p->u.flatten.pIter, &pKey, &pValue);
        p->pValue = flattenedObject(pBase, pKey, pValue, p->u.flatten.pAs);
      }

      break;
    }

    case TK_DOT: {
      JsonNode *pArray = p->u.path.pArray;

      xjd1JsonFree(p->pValue);
      p->pValue = 0;

      if( pArray==0 ){
        pArray = p->u.path.pArray = xjd1ExprEval(p->u.path.pPath);
      }
      if( pArray
       && pArray->eJType==XJD1_ARRAY
       && p->u.path.iNext<pArray->u.ar.nElem
      ){
        rc = XJD1_ROW;
        p->pValue = xjd1JsonRef(pArray->u.ar.apElem[p->u.path.iNext++]);
      }

      break;
    }

    case TK_NULL: {
      rc = (p->u.null.isDone ? XJD1_DONE : XJD1_ROW);
      p->u.null.isDone = 1;
      break;
    }
  }
  return rc;
}

/*
** Return the document that this data source is current pointing to
** if the AS name of the document is zDocName or if zDocName==0.
** The AS name is important since a join might have multiple documents.
**
** xjd1JsonRef() has been called on the returned string.  The caller
** must invoke xjd1JsonFree().
*/
JsonNode *xjd1DataSrcDoc(DataSrc *p, const char *zDocName){
  JsonNode *pRes = 0;
  if( p==0 ) return 0;
  if( zDocName && p->zAs && 0==strcmp(p->zAs, zDocName) ){
    return xjd1JsonRef(p->pValue);
  }
  switch( p->eDSType ){
    case TK_COMMA: {
      pRes = xjd1DataSrcDoc(p->u.join.pLeft, zDocName);
      if( pRes==0 ) pRes = xjd1DataSrcDoc(p->u.join.pRight, zDocName);
      break;
    }
    case TK_SELECT: {
      assert( p->zAs );
      if( zDocName==0 ){
        pRes = xjd1JsonRef(p->pValue);
      }
      break;
    }
    case TK_FLATTENOP:
    case TK_ID: {
      if( zDocName==0 || (p->zAs==0 && strcmp(p->u.tab.zName, zDocName)==0) ){
        pRes = xjd1JsonRef(p->pValue);
      }
      break;
    }
    default: {
      pRes = xjd1JsonRef(p->pValue);
      break;
    }
  }
  return pRes;
}

/*
** Rewind a data source so that the next call to DataSrcStep() will cause
** it to point to the first row.
*/
int xjd1DataSrcRewind(DataSrc *p){
  if( p==0 ) return XJD1_DONE;
  xjd1JsonFree(p->pValue);  p->pValue = 0;
  switch( p->eDSType ){
    case TK_COMMA: {
      p->u.join.bStart = 0;
      xjd1DataSrcRewind(p->u.join.pLeft);
      xjd1DataSrcRewind(p->u.join.pRight);
      break;
    }
    case TK_SELECT: {
      xjd1QueryRewind(p->u.subq.q);
      break;
    }
    case TK_ID: {
      sqlite3_reset(p->u.tab.pStmt);
      break;
    }
    case TK_DOT: {
      xjd1JsonFree(p->u.path.pArray);
      p->u.path.pArray = 0;
      p->u.path.iNext = 0;
      break;
    }
    case TK_FLATTENOP: {
      xjd1DataSrcRewind(p->u.flatten.pNext);
      flattenIterFree(p->u.flatten.pIter);
      p->u.flatten.pIter = 0;
      break;
    }
    case TK_NULL: {
      p->u.null.isDone = 0;
      break;
    }
  }
  return XJD1_DONE;
}

/*
** The destructor for a Query object.
*/
int xjd1DataSrcClose(DataSrc *p){
  if( p==0 ) return XJD1_OK;
  xjd1JsonFree(p->pValue);  p->pValue = 0;
  switch( p->eDSType ){
    case TK_ID: {
//printf("\n%d:%s:%s\n", __LINE__, __FILE__, sqlite3_sql(p->u.tab.pStmt));
      sqlite3_finalize(p->u.tab.pStmt);
      break;
    }
    case TK_FLATTENOP: {
      xjd1DataSrcClose(p->u.flatten.pNext);
      break;
    }
    case TK_DOT: {
      xjd1JsonFree(p->u.path.pArray);
      p->u.path.pArray = 0;
      break;
    }
  }
  return XJD1_OK;
}

int xjd1DataSrcCount(DataSrc *p){
  int n = 1;
  if( p->eDSType==TK_COMMA ){
    n = xjd1DataSrcCount(p->u.join.pLeft) + xjd1DataSrcCount(p->u.join.pRight);
  }
  return n;
}

static void cacheSaveRecursive(DataSrc *p, JsonNode ***papNode){
  if( p->eDSType==TK_COMMA ){
    cacheSaveRecursive(p->u.join.pLeft, papNode);
    cacheSaveRecursive(p->u.join.pRight, papNode);
  }else{
    xjd1JsonFree(**papNode);
    **papNode = xjd1JsonRef(p->pValue);
    (*papNode)++;
  }
}

void xjd1DataSrcCacheSave(DataSrc *p, JsonNode **apNode){
  JsonNode **pp = apNode;
  cacheSaveRecursive(p, &pp);
}

static int datasrcResolveRecursive(
  DataSrc *p,
  int *piEntry,
  const char *zDocname
){
  int ret = 0;
  if( p->eDSType==TK_COMMA ){
    ret = datasrcResolveRecursive(p->u.join.pLeft, piEntry, zDocname);
    if( 0==ret ){
      ret = datasrcResolveRecursive(p->u.join.pRight, piEntry, zDocname);
    }
  }else if( p->eDSType==TK_FLATTENOP ){
    ret = datasrcResolveRecursive(p->u.flatten.pNext, piEntry, zDocname);
  }else{
    if( (p->zAs && 0==strcmp(zDocname, p->zAs))
     || (p->zAs==0 && p->eDSType==TK_ID && strcmp(p->u.tab.zName, zDocname)==0)
    ){
      ret = *piEntry;
    }
    (*piEntry)++;
  }
  return ret;
}
int xjd1DataSrcResolve(DataSrc *p, const char *zDocname){
  int iEntry = 1;
  return datasrcResolveRecursive(p, &iEntry, zDocname);
}

static JsonNode *datasrcReadRecursive(
  DataSrc *p,
  int *piEntry,
  int iDoc
){
  JsonNode *pRet = 0;
  if( p->eDSType==TK_COMMA ){
    pRet = datasrcReadRecursive(p->u.join.pLeft, piEntry, iDoc);
    if( 0==pRet ){
      pRet = datasrcReadRecursive(p->u.join.pRight, piEntry, iDoc);
    }
  }else{
    if( *piEntry==iDoc ){
      pRet = xjd1JsonRef(p->pValue);
    }
    (*piEntry)++;
  }
  return pRet;
}
JsonNode *xjd1DataSrcRead(DataSrc *p, int iDoc){
  int iEntry = 1;
  assert( iDoc>=1 );
  return datasrcReadRecursive(p, &iEntry, iDoc);
}
