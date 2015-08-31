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
*/
#include "xjd1Int.h"

/*
** A structure used to store the definition of a query language function
** or aggregate.
*/
struct Function {
  int nMinArg;
  int nMaxArg;
  const char *zName;
  JsonNode *(*xFunc)(int nArg, JsonNode **apArg);
  int (*xStep)(int nArg, JsonNode **apArg, void **, int *pbSave);
  JsonNode *(*xFinal)(void *);
};

/*************************************************************************
** Start of implementation of aggregate functions:
**
**   count()
**     This aggregate may be invoked with zero or one argument. If no
**     arguments are specified, it returns the number of rows visited. If
**     one argument is supplied, it returns the number of rows visited
**     for which the argument was anything other than a NULL. In all cases,
**     this function returns a non-negative integer value.
**
**   min()
**     This aggregate is invoked with exactly one argument. If no rows are
**     visited, NULL is returned. Otherwise, it returns a copy of the
**     smallest argument it is passed. The comparisons used to determine
**     which argument is the smallest are performed using the same rules
**     as for the < and > operators.
**
**   max()
**     This aggregate is invoked with exactly one argument. If no rows are
**     visited, NULL is returned. Otherwise, it returns a copy of the
**     largest argument it is passed. The comparisons used to determine
**     which argument is the largest are performed using the same rules
**     as for the < and > operators.
**
**   array()
**     This aggregate is invoked with exactly one argument. If no rows are
**     visited, an empty array is returned. Otherwise, an array containing
**     the argument passed to this aggregate for each row visited is returned.
**     The order in which the values appear in the returned array is not
**     defined.
**
**   sum()
**     This aggregate is invoked with exactly one argument. If no rows are
**     visited, the value 0.0 (a number) is returned. Otherwise, each argument
**     is converted to a number and the sum of all converted arguments
**     returned. Any value that cannot be converted to a number (i.e. a struct,
**     an array or any string value that does not look like a number) is
**     treated as 0.0.
**
**   avg()
**     This aggregate is invoked with exactly one argument. If no rows are
**     visited, NULL is returned. Otherwise, the expression avg(X) is
**     equivalent to (sum(X)/count()).
*/

/*
** Aggregate function count().
*/
static int xCountStep(int nArg, JsonNode **apArg, void **pp, int *pbSave){
  int *pnCount = (int *)*pp;
  if( pnCount==0 ){
    pnCount = xjd1MallocZero(sizeof(int));
    *pp = (void *)pnCount;
  }
  if( nArg==0 || apArg[0]->eJType!=XJD1_NULL ){
    (*pnCount)++;
  }
  return XJD1_OK;
}
static JsonNode *xCountFinal(void *p){
  JsonNode *pRet;
  pRet = xjd1JsonNew(0);
  pRet->eJType = XJD1_REAL;
  if( p ){
    pRet->u.r = (double)*(int *)p;
    xjd1_free(p);
  }
  return pRet;
}

/*
** Aggregate functions min() and max().
*/
static int xMinStep(int nArg, JsonNode **apArg, void **pp, int *pbSave){
  JsonNode *pArg;
  JsonNode *pBest = *pp;
  assert( nArg==1 );
  pArg = apArg[0];
  if( !pBest || xjd1JsonCompare(pArg, pBest, 0)<0 ){
    xjd1JsonFree(pBest);
    *pp = (void *)xjd1JsonRef(pArg);
    *pbSave = 1;
  }
  return XJD1_OK;
}
static int xMaxStep(int nArg, JsonNode **apArg, void **pp, int *pbSave){
  JsonNode *pArg;
  JsonNode *pBest = *pp;
  assert( nArg==1 );
  pArg = apArg[0];
  if( !pBest || xjd1JsonCompare(pArg, pBest, 0)>0 ){
    xjd1JsonFree(pBest);
    *pp = (void *)xjd1JsonRef(pArg);
    *pbSave = 1;
  }
  return XJD1_OK;
}
static JsonNode *xMinMaxFinal(void *p){
  JsonNode *pRet;
  if( p ){
    pRet = (JsonNode *)p;
  }else{
    pRet = xjd1JsonNew(0);
    pRet->eJType = XJD1_NULL;
  }
  return pRet;
}

/*
** Aggregate function array().
*/
static int xArrayStep(int nArg, JsonNode **apArg, void **pp, int *pbSave){
  JsonNode *pArray = (JsonNode *)*pp;
  JsonNode *pArg = apArg[0];
  int nNew;
  if( !pArray ){
    pArray = xjd1JsonNew(0);
    pArray->eJType = XJD1_ARRAY;
    *pp = (void *)pArray;
  }
  nNew = pArray->u.ar.nElem+1;
  pArray->u.ar.apElem = (JsonNode **)xjd1_realloc(
      pArray->u.ar.apElem, nNew*sizeof(JsonNode *)
  );
  pArray->u.ar.nElem = nNew;
  pArray->u.ar.apElem[nNew-1] = xjd1JsonRef(pArg);
  return XJD1_OK;
}
static JsonNode *xArrayFinal(void *p){
  JsonNode *pArray = (JsonNode *)p;
  if( !pArray ){
    pArray = xjd1JsonNew(0);
    pArray->eJType = XJD1_ARRAY;
  }
  return pArray;
}

/*
** Aggregate function sum()
*/
static int xSumStep(int nArg, JsonNode **apArg, void **pp, int *pbSave){
  JsonNode *pVal = (JsonNode *)*pp;
  double rVal = 0.0;
  if( !pVal ){
    pVal = xjd1JsonNew(0);
    pVal->eJType = XJD1_REAL;
    *pp = (void *)pVal;
  }
  if( XJD1_OK==xjd1JsonToReal(apArg[0], &rVal) ){
    pVal->u.r += rVal;
  }
  return XJD1_OK;
}
static JsonNode *xSumFinal(void *p){
  JsonNode *pVal = (JsonNode *)p;
  if( !pVal ){
    pVal = xjd1JsonNew(0);
    pVal->eJType = XJD1_REAL;
  }
  return pVal;
}

/*
** Aggregate function avg()
*/
typedef struct AvgCtx AvgCtx;
struct AvgCtx {
  int nRow;
  double rSum;
};
static int xAvgStep(int nArg, JsonNode **apArg, void **pp, int *pbSave){
  AvgCtx *pCtx = (AvgCtx *)*pp;
  double rVal = 0.0;
  if( !pCtx ){
    pCtx = xjd1MallocZero(sizeof(AvgCtx));
    *pp = (void *)pCtx;
  }
  pCtx->nRow++;
  if( XJD1_OK==xjd1JsonToReal(apArg[0], &rVal) ){
    pCtx->rSum += rVal;
  }
  return XJD1_OK;
}
static JsonNode *xAvgFinal(void *p){
  AvgCtx *pCtx = (AvgCtx *)p;
  JsonNode *pRet;

  pRet = xjd1JsonNew(0);
  if( pCtx ){
    pRet->eJType = XJD1_REAL;
    pRet->u.r = pCtx->rSum/(double)pCtx->nRow;
    xjd1_free(pCtx);
  }else{
    pRet->eJType = XJD1_NULL;
  }

  return pRet;
}

/*
** End of implementation of aggregates.
*************************************************************************/

/*************************************************************************
** Start of implementation of scalar functions:
**
**   length()
**     This function is called with exactly one argument. It converts
**     that argument to a string, and returns the number of characters
**     in that string.
**
*/

/*
** Scalar function "length(x)".
*/
static JsonNode *xLength(int nArg, JsonNode **apArg){
  JsonNode *pRet;
  JsonNode *pStr;
  int nRet;
  assert( nArg==1 );

  pStr = apArg[0];
  if( pStr->eJType==XJD1_STRING ){
    nRet = xjd1Strlen30(pStr->u.z);
  }else{
    String str;
    xjd1StringInit(&str, 0, 0);
    xjd1JsonToString(pStr, &str);
    nRet = str.nUsed;
    xjd1StringClear(&str);
  }

  pRet = xjd1JsonNew(0);
  pRet->eJType = XJD1_REAL;
  pRet->u.r = (double)nRet;

  return pRet;
}

/*
** End of implementation of scalar functions.
*************************************************************************/

int xjd1AggregateInit(xjd1_stmt *pStmt, Query *pQuery, Expr *p){
  Aggregate *pAgg;

  assert( pQuery->eQType==TK_SELECT );
  pAgg = pQuery->u.simple.pAgg;
  if( pAgg==0 ){
    pAgg = (Aggregate *)xjd1PoolMallocZero(&pStmt->sPool, sizeof(Aggregate));
    if( pAgg==0 ) return XJD1_NOMEM;
    pQuery->u.simple.pAgg = pAgg;
  }

  if( p ){
    static const int ARRAY_ALLOC_INCR = 8;

    if( (pAgg->nExpr % ARRAY_ALLOC_INCR)==0 ){
      int nByte;                    /* Size of new allocation in bytes */
      int nCopy;                    /* Size of old allocation in bytes */
      AggExpr *aNew;                /* Pointer to new allocation */

      nByte = (pAgg->nExpr + ARRAY_ALLOC_INCR) * sizeof(AggExpr);
      nCopy = pAgg->nExpr * sizeof(AggExpr);
      aNew = (AggExpr *)xjd1PoolMallocZero(&pStmt->sPool, nByte);

      if( aNew==0 ) return XJD1_NOMEM;
      memcpy(aNew, pAgg->aAggExpr, nCopy);
      pAgg->aAggExpr = aNew;
    }

    p->u.func.iAgg = pAgg->nExpr;
    pAgg->aAggExpr[pAgg->nExpr++].pExpr = p;
  }

  return XJD1_OK;
}

/*
** The expression passed as the first argument is of type TK_FUNCTION.
** This function initializes the expression object. If successful, XJD1_OK
** is returned. Otherwise, an error code is returned and an error left in
** the pStmt statement handle.
*/
int xjd1FunctionInit(Expr *p, xjd1_stmt *pStmt, Query *pQuery, int eExpr){
  char *zName;
  int nArg;
  int nByte;
  int i;
  int bAggOk;
  int bWrongNumArgs = 0;

  static Function aFunc[] = {
    {  1,1, "length", xLength, 0, 0 },

    {  0,1, "count", 0, xCountStep, xCountFinal  },
    {  1,1, "min",   0, xMinStep,   xMinMaxFinal },
    {  1,1, "max",   0, xMaxStep,   xMinMaxFinal },
    {  1,1, "array", 0, xArrayStep, xArrayFinal  },
    {  1,1, "sum",   0, xSumStep,   xSumFinal    },
    {  1,1, "avg",   0, xAvgStep,   xAvgFinal    },
  };

  assert( p->eType==TK_FUNCTION && p->eClass==XJD1_EXPR_FUNC );
  assert( pQuery || eExpr==0 );

  /* Set bAggOk to true if aggregate functions may be used in this context. */
  bAggOk = (pQuery && pQuery->eQType==TK_SELECT
        && (eExpr==XJD1_EXPR_RESULT || eExpr==XJD1_EXPR_GROUPBY
         || eExpr==XJD1_EXPR_HAVING || eExpr==XJD1_EXPR_ORDERBY
  ));

  zName = p->u.func.zFName;
  nArg = p->u.func.args->nEItem;

  for(i=0; i<ArraySize(aFunc); i++){
    Function *pFunc = &aFunc[i];
    assert( (pFunc->xFunc==0)==(pFunc->xStep && pFunc->xFinal) );
    if( !strcmp(pFunc->zName, zName) ){
      if( nArg<=pFunc->nMaxArg && nArg>=pFunc->nMinArg ){
        if( pFunc->xStep ){
          int rc;
          if( bAggOk==0 ){
            const char *zErrMsg = "illegal use of aggregate function: %s";
            xjd1StmtError(pStmt, XJD1_ERROR, zErrMsg, zName);
            return XJD1_ERROR;
          }

          /* Add this aggregate function to the Query.pAgg->apExpr[] array. */
          rc = xjd1AggregateInit(pStmt, pQuery, p);
          if( rc!=XJD1_OK ) return rc;
        }
        p->u.func.pFunction = pFunc;
      }else{
        bWrongNumArgs = 1;
      }
      break;
    }
  }

  if( !p->u.func.pFunction ){
    if( bWrongNumArgs ){
      xjd1StmtError(pStmt, XJD1_ERROR,
          "wrong number of arguments to function %s()", zName);
    }else{
      xjd1StmtError(pStmt, XJD1_ERROR, "no such function: %s", zName);
    }
    return XJD1_ERROR;
  }

  nByte = sizeof(JsonNode *) * nArg;
  p->u.func.apArg = (JsonNode **)xjd1PoolMallocZero(&pStmt->sPool, nByte);
  if( !p->u.func.apArg ){
    return XJD1_NOMEM;
  }

  return XJD1_OK;
}

static int aggExprStep(AggExpr *pAggExpr, int *pbSave){
  Expr *p = pAggExpr->pExpr;
  Function *pFunc = p->u.func.pFunction;
  int i;
  int nItem = p->u.func.args->nEItem;

  assert( p->eType==TK_FUNCTION && p->eClass==XJD1_EXPR_FUNC );
  assert( pFunc->xStep && pFunc->xFinal && pFunc->xFunc==0 );

  for(i=0; i<nItem; i++){
    p->u.func.apArg[i] = xjd1ExprEval(p->u.func.args->apEItem[i].pExpr);
  }

  pFunc->xStep(nItem, p->u.func.apArg, &pAggExpr->pAggCtx, pbSave);
  for(i=0; i<nItem; i++){
    xjd1JsonFree(p->u.func.apArg[i]);
  }

  return XJD1_OK;
}

int xjd1AggregateStep(
  Aggregate *pAgg,
  int *pbSave                     /* OUT: True if this row should be saved */
){
  int i;                /* Used to iterate through aggregate functions */
  for(i=0; i<pAgg->nExpr; i++){
    int rc = aggExprStep(&pAgg->aAggExpr[i], pbSave);
    if( rc!=XJD1_OK ) return rc;
  }
  return XJD1_OK;;
}

int xjd1AggregateFinalize(Aggregate *pAgg){
  int i;
  for(i=0; i<pAgg->nExpr; i++){
    AggExpr *pAggExpr = &pAgg->aAggExpr[i];
    Expr *p = pAggExpr->pExpr;

    assert( i==p->u.func.iAgg );
    xjd1JsonFree(pAggExpr->pValue);
    pAggExpr->pValue = p->u.func.pFunction->xFinal(pAggExpr->pAggCtx);
    pAggExpr->pAggCtx = 0;
  }

  return XJD1_OK;
}

/*
** Call any outstanding xFinal() functions for aggregate functions in the
** query. This is required to reset the aggregate contexts when a query is
** rewound following an error.
*/
void xjd1AggregateClear(Query *pQuery){
  Aggregate *pAgg = pQuery->u.simple.pAgg;

  assert( pQuery->eQType==TK_SELECT );
  if( pAgg ){
    int i;
    for(i=0; i<pAgg->nExpr; i++){
      AggExpr *pAggExpr = &pAgg->aAggExpr[i];
      Expr *p = pAggExpr->pExpr;

      xjd1JsonFree( p->u.func.pFunction->xFinal(pAggExpr->pAggCtx) );
      xjd1JsonFree( pAggExpr->pValue );
      pAggExpr->pAggCtx = 0;
      pAggExpr->pValue = 0;
    }
  }
}

JsonNode *xjd1FunctionEval(Expr *p){
  JsonNode *pRet;
  Function *pFunc = p->u.func.pFunction;

  assert( p->eType==TK_FUNCTION && p->eClass==XJD1_EXPR_FUNC );

  if( pFunc->xFunc ){
    /* A scalar function. */
    int i;
    int nItem = p->u.func.args->nEItem;
    for(i=0; i<nItem; i++){
      p->u.func.apArg[i] = xjd1ExprEval(p->u.func.args->apEItem[i].pExpr);
    }

    pRet = pFunc->xFunc(nItem, p->u.func.apArg);
    for(i=0; i<nItem; i++){
      xjd1JsonFree(p->u.func.apArg[i]);
    }
  }else{
    AggExpr *pAggExpr = &p->pQuery->u.simple.pAgg->aAggExpr[p->u.func.iAgg];
    pRet = xjd1JsonRef(pAggExpr->pValue);
  }

  return pRet;
}

