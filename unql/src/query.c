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
** This file contains code used to implement query processing.
*/
#include "xjd1Int.h"


struct ResultItem {
  JsonNode **apKey;               /* Array of JSON objects */
  ResultItem *pNext;              /* Next element in list */
};

static int addToResultList(
  ResultList *pList,              /* List to append to */
  JsonNode **apKey                /* Array of values to add to list */
){
  ResultItem *pNew;               /* Newly allocated ResultItem */
  int nByte;                      /* Bytes to allocate for new node */
  int i;                          /* Used to iterate through apKey[] */

  nByte = sizeof(ResultItem) + sizeof(JsonNode*) * pList->nKey;
  pNew = (ResultItem *)xjd1PoolMalloc(pList->pPool, nByte);
  if( !pNew ){
    for(i=0; i<pList->nKey; i++){
      xjd1JsonFree(apKey[i]);
    }
    return XJD1_NOMEM;
  }

  pNew->apKey = (JsonNode **)&pNew[1];
  memcpy(pNew->apKey, apKey, pList->nKey * sizeof(JsonNode *));
  pNew->pNext = pList->pItem;
  pList->pItem = pNew;

  return XJD1_OK;
}

static int cmpResultItem(ResultItem *p1, ResultItem *p2, ExprList *pEList){
  int i;
  int c = 0;
  int nEItem = (pEList ? pEList->nEItem : 1);
  for(i=0; i<nEItem; i++){
    if( 0!=(c=xjd1JsonCompare(p1->apKey[i], p2->apKey[i], 0)) ) break;
  }
  if( c && pEList ){
    char const *zDir = pEList->apEItem[i].zAs;
    if( zDir && zDir[0]=='D' ){
      c = c*-1;
    }
  }
  return c;
}

static ResultItem *mergeResultItems(
  ExprList *pEList,              /* Used for ASC/DESC of each key */
  ResultItem *p1,                /* First list to merge */
  ResultItem *p2                 /* Second list to merge */
){
  ResultItem *pRet = 0;
  ResultItem **ppNext;

  ppNext = &pRet;
  while( p1 || p2 ){
    if( !p1 ){
      *ppNext = p2;
      p2 = 0;
    }else if( !p2 ){
      *ppNext = p1;
      p1 = 0;
    }else{
      int c = cmpResultItem(p1, p2, pEList);
      if( c<=0 ){
        *ppNext = p1;
        ppNext = &p1->pNext;
        p1 = p1->pNext;
      }else{
        *ppNext = p2;
        ppNext = &p2->pNext;
        p2 = p2->pNext;
      }
    }
  }

  return pRet;
}

static void sortResultList(ResultList *pList, ExprList *pEList, int uniq){
  int i;                          /* Used to iterate through aList[] */
  ResultItem *aList[40];          /* Array of slots for merge sort */
  ResultItem *pHead;

  memset(aList, 0, sizeof(aList));
  pHead = pList->pItem;

  while( pHead ){
    ResultItem *pNext = pHead->pNext;
    pHead->pNext = 0;
    for(i=0; aList[i]; i++){
      assert( i<ArraySize(aList) );
      pHead = mergeResultItems(pEList, pHead, aList[i]);
      aList[i] = 0;
    }
    aList[i] = pHead;
    pHead = pNext;
  }

  pHead = aList[0];
  for(i=1; i<ArraySize(aList); i++){
    pHead = mergeResultItems(pEList, pHead, aList[i]);
  }
  pList->pItem = pHead;

  if( uniq && pHead ){
    ResultItem *pPrev = pHead;
    ResultItem *p;
    for(p=pPrev->pNext; p; p=p->pNext){
      if( cmpResultItem(pPrev, p, pEList)==0 ){
        pPrev->pNext = p->pNext;
      }else{
        pPrev = p;
      }
    }
  }

#if 1
  while( pHead ){
    ResultItem *pNext = pHead->pNext;
    assert( pNext==0 || cmpResultItem(pHead, pNext, pEList)<=(uniq?-1:0) );
    pHead = pNext;
  }
#endif
}

static void freeResultListItem(ResultList *pList, ResultItem *pItem){
  int i;
  for(i=0; i<pList->nKey; i++){
    xjd1JsonFree(pItem->apKey[i]);
  }
}

static void popResultList(ResultList *pList){
  ResultItem *pItem;
  pItem = pList->pItem;
  if( pItem ){
    pList->pItem = pItem->pNext;
    freeResultListItem(pList, pItem);
  }
}

/*
** These two are used GROUP BY processing.
*/
static void saveResultList(ResultList *pList){
  if( pList->pSaved ){
    freeResultListItem(pList, pList->pSaved);
  }
  pList->pSaved = pList->pItem;
  pList->pItem = pList->pItem->pNext;
}
static void restoreResultList(ResultList *pList){
  if( pList->pSaved ){
    popResultList(pList);
    pList->pSaved->pNext = pList->pItem;
    pList->pItem = pList->pSaved;
    pList->pSaved = 0;
  }
}


static void clearResultList(ResultList *pList){
  while( pList->pItem ) popResultList(pList);
  xjd1PoolDelete(pList->pPool);
  memset(pList, 0, sizeof(ResultList));
}

/*
** Called after statement parsing to initalize every Query object
** within the statement.
*/
int xjd1QueryInit(
  Query *p,                       /* Query to initialize */
  xjd1_stmt *pStmt,               /* Statement this query is part of */
  void *pCtx                      /* Context for expression initialization */
){
  int rc;
  if( p==0 ) return XJD1_OK;
  p->pStmt = pStmt;
  if( p->eQType==TK_SELECT ){
    rc = xjd1ExprInit(p->u.simple.pRes, pStmt, p, XJD1_EXPR_RESULT, pCtx);
    if( !rc ){
      rc = xjd1DataSrcInit(p->u.simple.pFrom, p, pCtx);
    }
    if( !rc ){
      rc = xjd1ExprInit(p->u.simple.pWhere, pStmt, p, XJD1_EXPR_WHERE, pCtx);
    }
    if( !rc ){
      rc = xjd1ExprListInit(
          p->u.simple.pGroupBy, pStmt, p, XJD1_EXPR_GROUPBY, pCtx
      );
    }
    if( !rc ){
      rc = xjd1ExprInit(p->u.simple.pHaving, pStmt, p, XJD1_EXPR_HAVING, pCtx);
    }
    if( !rc && p->u.simple.pGroupBy ){
      rc = xjd1AggregateInit(pStmt, p, 0);
    }
  }else{
    rc = xjd1QueryInit(p->u.compound.pLeft, pStmt, pCtx);
    if( !rc ) rc = xjd1QueryInit(p->u.compound.pRight, pStmt, pCtx);
  }

  if( !rc ){
    rc = xjd1ExprListInit(p->pOrderBy, pStmt, p, XJD1_EXPR_ORDERBY, pCtx);
  }
  if( !rc ){
    rc = xjd1ExprInit(p->pLimit, pStmt, p, XJD1_EXPR_LIMIT, pCtx);
  }
  if( !rc ){
    rc = xjd1ExprInit(p->pOffset, pStmt, p, XJD1_EXPR_OFFSET, pCtx);
  }
  return rc;
}

/*
** Rewind a query so that it is pointing at the first row.
*/
int xjd1QueryRewind(Query *p){
  if( p==0 ) return XJD1_OK;
  if( p->eQType==TK_SELECT ){
    xjd1DataSrcRewind(p->u.simple.pFrom);
    clearResultList(&p->u.simple.grouped);
    clearResultList(&p->u.simple.distincted);
    xjd1AggregateClear(p);
  }else{
    xjd1QueryRewind(p->u.compound.pLeft);
    xjd1QueryRewind(p->u.compound.pRight);
    p->u.compound.doneLeft = 0;
    clearResultList(&p->u.compound.left);
    clearResultList(&p->u.compound.right);
    xjd1JsonFree(p->u.compound.pOut);
    p->u.compound.pOut = 0;
  }
  clearResultList(&p->ordered);
  p->eDocFrom = XJD1_FROM_DATASRC;
  p->bLimitValid = 0;
  return XJD1_OK;
}

/*
** Advance to the next row of the TK_SELECT query passed as the first
** argument, disregarding any ORDER BY, OFFSET or LIMIT clause.
**
** Return XJD1_ROW if there is such a row, or XJD1_DONE at EOF. Or return
** an error code if an error occurs.
*/
static int selectStepWhered(Query *p){
  int rc;                         /* Return code */
  assert( p->eQType==TK_SELECT );
  do{
    rc = xjd1DataSrcStep(p->u.simple.pFrom);
  }while(
    rc==XJD1_ROW
    && (p->u.simple.pWhere!=0 && !xjd1ExprTrue(p->u.simple.pWhere))
  );
  return rc;
}

static int selectStepGrouped(Query *p){
  Aggregate *pAgg = p->u.simple.pAgg;
  int rc;

  if( pAgg ){
    ExprList *pGroupBy = p->u.simple.pGroupBy;

    rc = XJD1_OK;
    if( pGroupBy==0 ){
      /* An aggregate query with no GROUP BY clause. If there is no GROUP BY,
      ** then exactly one row is returned, which makes ORDER BY and DISTINCT
      ** no-ops. And it is not possible to have a HAVING clause without a
      ** GROUP BY, so no need to worry about that either.
      */
      assert( p->u.simple.pHaving==0 );

      if( p->u.simple.grouped.pPool==0 ){
        JsonNode **apSrc;
        int nSrc;
        int saved = 0;
        Pool *pPool;

        pPool = p->u.simple.grouped.pPool = xjd1PoolNew();
        if( !pPool ) return XJD1_NOMEM;

        p->u.simple.grouped.nKey = nSrc = xjd1DataSrcCount(p->u.simple.pFrom);
        apSrc = (JsonNode **)xjd1PoolMallocZero(pPool, nSrc*sizeof(JsonNode *));
        if( !apSrc ) return XJD1_NOMEM;

        /* Call the xStep() of each aggregate in the query for each row
        ** matched by the query WHERE clause. */
        while( rc==XJD1_OK && XJD1_ROW==(rc = selectStepWhered(p) ) ){
          int saveThisRow = 0;
          rc = xjd1AggregateStep(pAgg, &saveThisRow);
          if( saved==0 || saveThisRow ){
            xjd1DataSrcCacheSave(p->u.simple.pFrom, apSrc);
            saved = 1;
          }
        }
        assert( rc!=XJD1_OK );
        if( rc==XJD1_DONE ){
          rc = addToResultList(&p->u.simple.grouped, apSrc);
        }
        if( rc==XJD1_OK ){
          rc = xjd1AggregateFinalize(pAgg);
        }
        if( rc==XJD1_OK ){
          rc = XJD1_ROW;
          p->eDocFrom = XJD1_FROM_GROUPED;
        }

      }else{
        rc = XJD1_DONE;
      }

      /* TODO: If this is a top-level query, then xQueryDoc(p, 0) will be
      ** called exactly once to retrieve the query result. When any
      ** aggregate expressions within the query result are evaluated, the
      ** xFinal function is evaluated. But this will only work once - if
      ** xQueryDoc(p, 0) is called more than once it will break. Find out if
      ** this is a problem!
      */

    }else{

      /* An aggregate with a GROUP BY clause. There may also be a DISTINCT
      ** qualifier.
      **
      ** Use a ResultList to sort all the rows matched by the WHERE
      ** clause. The apKey[] array consists of each of the expressions
      ** in the GROUP BY clause, followed by each document in the FROM
      ** clause. */
      do {
        ResultItem *pItem;

        if( p->u.simple.grouped.pPool==0 ){
          DataSrc *pFrom = p->u.simple.pFrom;
          JsonNode **apKey;
          int nByte;
          Pool *pPool;

          /* Allocate the memory pool for this ResultList. And apKey. */
          pPool = p->u.simple.grouped.pPool = xjd1PoolNew();
          p->u.simple.grouped.nKey = pGroupBy->nEItem + xjd1DataSrcCount(pFrom);
          if( !pPool ) return XJD1_NOMEM;
          nByte = p->u.simple.grouped.nKey * sizeof(JsonNode *);
          apKey = (JsonNode **)xjd1PoolMallocZero(pPool, nByte);
          if( !apKey ) return XJD1_NOMEM;

          while( rc==XJD1_OK && XJD1_ROW==(rc = selectStepWhered(p) ) ){
            int i;
            for(i=0; i<pGroupBy->nEItem; i++){
              apKey[i] = xjd1ExprEval(pGroupBy->apEItem[i].pExpr);
            }
            xjd1DataSrcCacheSave(pFrom, &apKey[i]);
            rc = addToResultList(&p->u.simple.grouped, apKey);
            memset(apKey, 0, nByte);
          }
          if( rc!=XJD1_DONE ) return rc;
          sortResultList(&p->u.simple.grouped, pGroupBy, 0);
        }else{
          popResultList(&p->u.simple.grouped);
        }

        p->eDocFrom = XJD1_FROM_GROUPED;
        pItem = p->u.simple.grouped.pItem;
        if( pItem==0 ){
          rc = XJD1_DONE;
        }else{
          while( 1 ){
            int saveThisRow = 0;
            ResultItem *pNext = pItem->pNext;
            rc = xjd1AggregateStep(pAgg, &saveThisRow);
            if( saveThisRow ) saveResultList(&p->u.simple.grouped);
            if( rc || !pNext || cmpResultItem(pItem, pNext, pGroupBy) ){
              restoreResultList(&p->u.simple.grouped);
              break;
            }
            if( !saveThisRow ) popResultList(&p->u.simple.grouped);
            pItem = p->u.simple.grouped.pItem;
          }

          if( XJD1_OK==rc && XJD1_OK==(rc = xjd1AggregateFinalize(pAgg)) ){
            rc = XJD1_ROW;
          }
        }

      }while( rc==XJD1_ROW
           && p->u.simple.pHaving && !xjd1ExprTrue(p->u.simple.pHaving)
      );
    }

  }else{
    /* Non-aggregate query */
    rc = selectStepWhered(p);
  }

  return rc;
}

static int selectStepDistinct(Query *p){
  int rc;
  if( p->u.simple.isDistinct ){
    if( p->u.simple.distincted.pPool==0 ){
      Pool *pPool;
      JsonNode **apKey;
      int nKey;

      nKey = 1 + xjd1DataSrcCount(p->u.simple.pFrom);
      pPool = p->u.simple.distincted.pPool = xjd1PoolNew();
      if( !pPool ) return XJD1_NOMEM;
      p->u.simple.distincted.nKey = nKey;
      apKey = xjd1PoolMallocZero(pPool, nKey * sizeof(JsonNode *));
      if( !apKey ) return XJD1_NOMEM;

      while( XJD1_ROW==(rc = selectStepGrouped(p) ) ){
        apKey[0] = xjd1QueryDoc(p, 0);
        xjd1DataSrcCacheSave(p->u.simple.pFrom, &apKey[1]);
        rc = addToResultList(&p->u.simple.distincted, apKey);
        memset(apKey, 0, nKey * sizeof(JsonNode *));
        if( rc!=XJD1_OK ) break;
      }
      if( rc==XJD1_DONE ){
        sortResultList(&p->u.simple.distincted, 0, 1);
        p->eDocFrom = XJD1_FROM_DISTINCTED;
        rc = XJD1_ROW;
      }
    }else{
      popResultList(&p->u.simple.distincted);
      rc = p->u.simple.distincted.pItem ? XJD1_ROW : XJD1_DONE;
    }
  }else{
    rc = selectStepGrouped(p);


  }
  return rc;
}


static int selectStepCompounded(Query *);
static int cacheQuery(ResultList *pList, Query *p){
  Pool *pPool;
  int rc;

  pList->nKey = 1;
  pPool = pList->pPool = xjd1PoolNew();
  if( !pPool ) return XJD1_NOMEM;

  while( XJD1_ROW==(rc = selectStepCompounded(p) ) ){
    JsonNode *pDoc = xjd1QueryDoc(p, 0);
    rc = addToResultList(pList, &pDoc);
    if( rc!=XJD1_OK ) break;
  }

  if( rc==XJD1_DONE ){
    sortResultList(pList, 0, 1);
    rc = XJD1_OK;
  }
  return rc;
}


static int selectStepCompounded(Query *p){
  int rc;
  if( p->eQType==TK_SELECT ){
    rc = selectStepDistinct(p);
  }else{
    JsonNode *pOut = 0;
    if( p->eQType==TK_ALL ){
      rc = XJD1_DONE;
      if( p->u.compound.doneLeft==0 ){
        rc = selectStepCompounded(p->u.compound.pLeft);
        if( rc==XJD1_ROW ){
          pOut = xjd1QueryDoc(p->u.compound.pLeft, 0);
        }
      }
      if( rc==XJD1_DONE ){
        p->u.compound.doneLeft = 1;
        rc = selectStepCompounded(p->u.compound.pRight);
        if( rc==XJD1_ROW ){
          pOut = xjd1QueryDoc(p->u.compound.pRight, 0);
        }
      }
    }else{

      ResultList *p1;
      ResultList *p2;

      if( p->u.compound.left.pPool==0 ){
        rc = cacheQuery(&p->u.compound.left, p->u.compound.pLeft);
        if( rc!=XJD1_OK ) return rc;
        rc = cacheQuery(&p->u.compound.right, p->u.compound.pRight);
        if( rc!=XJD1_OK ) return rc;
      }

      p1 = &p->u.compound.left;
      p2 = &p->u.compound.right;
      rc = XJD1_DONE;

      switch( p->eQType ){
        case TK_UNION: {
          if( (p1->pItem || p2->pItem) ){
            int c;
            if( p1->pItem==0 ){
              c = 1;
            }else if( p2->pItem==0 ){
              c = -1;
            }else{
              c = cmpResultItem(p1->pItem, p2->pItem, 0);
            }

            if( c<0 ){
              pOut = xjd1JsonRef(p1->pItem->apKey[0]);
            }else{
              pOut = xjd1JsonRef(p2->pItem->apKey[0]);
            }
            if( c<=0 ) popResultList(p1);
            if( c>=0 ) popResultList(p2);
            rc = XJD1_ROW;
          }
          break;
        }

        case TK_INTERSECT:
          while( rc==XJD1_DONE && p1->pItem && p2->pItem ){
            int c = cmpResultItem(p1->pItem, p2->pItem, 0);
            if( c==0 ){
              pOut = xjd1JsonRef(p1->pItem->apKey[0]);
              rc = XJD1_ROW;
            }
            if( c<=0 ) popResultList(p1);
            if( c>=0 ) popResultList(p2);
          }
          break;

        case TK_EXCEPT:
          while( rc==XJD1_DONE && p1->pItem ){
            int c = p2->pItem ? cmpResultItem(p1->pItem, p2->pItem, 0) : -1;
            if( c<=0 ){
              if( c ){
                pOut = xjd1JsonRef(p1->pItem->apKey[0]);
                rc = XJD1_ROW;
              }
              popResultList(p1);
            }else{
              popResultList(p2);
            }
          }
          break;
      }
    }

    xjd1JsonFree(p->u.compound.pOut);
    p->u.compound.pOut = pOut;
  }
  return rc;
}

/*
** Advance to the next row of the TK_SELECT query passed as the first
** argument, disregarding any OFFSET or LIMIT clause.
**
** Return XJD1_ROW if there is such a row, or XJD1_EOF if there is not. Or
** return an error code if an error occurs.
*/
static int selectStepOrdered(Query *p){
  int rc = XJD1_OK;                    /* Return Code */
  ExprList *pOrderBy = p->pOrderBy;    /* ORDER BY clause (or NULL) */

  if( pOrderBy ){

    /* A non-aggregate with an ORDER BY clause. */
    if( p->ordered.pPool==0 ){
      int nKey = pOrderBy->nEItem + 1;
      Pool *pPool;
      JsonNode **apKey;

      p->ordered.nKey = nKey;
      pPool = p->ordered.pPool = xjd1PoolNew();
      if( !pPool ) return XJD1_NOMEM;
      apKey = xjd1PoolMallocZero(pPool, nKey * sizeof(JsonNode *));
      if( !apKey ) return XJD1_NOMEM;

      while( XJD1_ROW==(rc = selectStepCompounded(p) ) ){
        int i;
        for(i=0; i<pOrderBy->nEItem; i++){
          apKey[i] = xjd1ExprEval(pOrderBy->apEItem[i].pExpr);
        }
        apKey[i] = xjd1QueryDoc(p, 0);

        rc = addToResultList(&p->ordered, apKey);
        if( rc!=XJD1_OK ) break;
      }
      if( rc!=XJD1_DONE ) return rc;

      sortResultList(&p->ordered, pOrderBy, 0);
      p->eDocFrom = XJD1_FROM_ORDERED;
    }else{
      assert( p->eDocFrom==XJD1_FROM_ORDERED );
      popResultList(&p->ordered);
    }

    rc = p->ordered.pItem ? XJD1_ROW : XJD1_DONE;
  }else{
    /* No ORDER BY clause. */
    rc = selectStepCompounded(p);
  }

  return rc;
}

/*
** Advance a query to the next row.  Return XDJ1_DONE if there is no
** next row, or XJD1_ROW if the step was successful.
*/
int xjd1QueryStep(Query *p){
  int rc = XJD1_ROW;
  if( p==0 ) return XJD1_DONE;

  /* Query is either a simple SELECT, or a compound of some type. */
  assert( p->eQType==TK_SELECT || p->eQType==TK_UNION || p->eQType==TK_ALL
       || p->eQType==TK_EXCEPT || p->eQType==TK_INTERSECT
  );

  if( p->bLimitValid==0 ){
    /* Calculate the values, if any, of the LIMIT and OFFSET clauses.
    **
    ** TBD: Should this throw an exception if the result of evaluating
    ** either of these clauses cannot be converted to a number?
    */
    Expr *pLimit = p->pLimit;     /* The LIMIT expression, or NULL */
    Expr *pOffset = p->pOffset;   /* The OFFSET expression, or NULL */

    if( pOffset ){
      JsonNode *pVal;             /* Result of evaluating expression pOffset */
      double rOffset;             /* pVal converted to a number */

      pVal = xjd1ExprEval(pOffset);
      if( 0==xjd1JsonToReal(pVal, &rOffset) ){
        int nOffset;
        for(nOffset=(int)rOffset; nOffset>0; nOffset--){
          rc = selectStepOrdered(p);
          if( rc!=XJD1_ROW ) break;
        }
      }
      xjd1JsonFree(pVal);
    }

    p->nLimit = -1;
    if( p->pLimit ){
      double rLimit;
      JsonNode *pVal;

      pVal = xjd1ExprEval(pLimit);
      if( 0==xjd1JsonToReal(pVal, &rLimit) ){
        p->nLimit = (int)rLimit;
      }
      xjd1JsonFree(pVal);
    }
    p->bLimitValid = 1;
  }

  if( rc==XJD1_ROW ){
    if( p->nLimit==0 ){
      rc = XJD1_DONE;
    }else{
      rc = selectStepOrdered(p);
      if( p->nLimit>0 ) p->nLimit--;
    }
  }

  return rc;
}

/*
** Return a document currently referenced by a query.  If zDocName==0 then
** return the constructed result set of the query.
**
** The caller must invoke JsonFree() when it is done with this value.
*/
JsonNode *xjd1QueryDoc(Query *p, int iDoc){
  JsonNode *pOut = 0;
  if( p ){
    if( p->eDocFrom==XJD1_FROM_ORDERED ){
      assert( iDoc==0 && p->ordered.pItem );
      pOut = xjd1JsonRef(p->ordered.pItem->apKey[p->ordered.nKey-1]);
    }else if( p->eQType==TK_SELECT ){
      switch( p->eDocFrom ){
        case XJD1_FROM_DISTINCTED:
          pOut = xjd1JsonRef(p->u.simple.distincted.pItem->apKey[iDoc]);
          break;

        case XJD1_FROM_GROUPED:
          if( iDoc==0 && p->u.simple.pRes ){
            pOut = xjd1ExprEval(p->u.simple.pRes);
          }else{
            JsonNode **apSrc = p->u.simple.grouped.pItem->apKey;
            if( p->u.simple.pGroupBy ){
              apSrc = &apSrc[p->u.simple.pGroupBy->nEItem];
            }
            pOut = xjd1JsonRef(apSrc[(iDoc?iDoc-1:0)]);
          }
          break;

        case XJD1_FROM_DATASRC:
          if( iDoc==0 && p->u.simple.pRes ){
            pOut = xjd1ExprEval(p->u.simple.pRes);
          }else{
            pOut = xjd1DataSrcRead(p->u.simple.pFrom, (iDoc ? iDoc : 1));
          }
          break;
      }
    }else{
      assert( iDoc==0 );
      pOut = xjd1JsonRef(p->u.compound.pOut);
    }


  }
  return pOut;
}


/*
** The destructor for a Query object.
*/
int xjd1QueryClose(Query *pQuery){
  int rc = XJD1_OK;
  if( pQuery==0 ) return rc;
  if( pQuery->eQType==TK_SELECT ){
    clearResultList(&pQuery->ordered);
    clearResultList(&pQuery->u.simple.grouped);
    clearResultList(&pQuery->u.simple.distincted);
    xjd1ExprClose(pQuery->u.simple.pRes);
    xjd1DataSrcClose(pQuery->u.simple.pFrom);
    xjd1ExprClose(pQuery->u.simple.pWhere);
    xjd1ExprListClose(pQuery->u.simple.pGroupBy);
    xjd1ExprClose(pQuery->u.simple.pHaving);
  }else{
    xjd1QueryClose(pQuery->u.compound.pLeft);
    xjd1QueryClose(pQuery->u.compound.pRight);
    clearResultList(&pQuery->u.compound.left);
    clearResultList(&pQuery->u.compound.right);
    xjd1JsonFree(pQuery->u.compound.pOut);
    pQuery->u.compound.pOut = 0;
  }
  xjd1ExprListClose(pQuery->pOrderBy);
  xjd1ExprClose(pQuery->pLimit);
  xjd1ExprClose(pQuery->pOffset);
  return rc;
}

