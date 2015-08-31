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
** This file contains code used to evaluate expressions at run-time.
*/
#include "xjd1Int.h"
#include <math.h>


/*
** Instances of the following structure are used when resolving document
** references in expressions.
*/
typedef struct ResolveCtx ResolveCtx;
struct ResolveCtx {
  xjd1_stmt *pStmt;               /* Statement expressions are part of */
  Query *pQuery;                  /* Query expressions are part of */
  int eExpr;                      /* Role of expression in pQuery */
  ResolveCtx *pParent;            /* NULL or parent of pQuery */
};

/* forward reference */
static int walkExpr(Expr*, int (*)(Expr *,void *), void *);

/*
** Walk an expression list
*/
static int walkExprList(ExprList *p, int (*xFunc)(Expr *,void *), void *pCtx){
  if( p ){
    int i;
    for(i=0; i<p->nEItem; i++){
      walkExpr(p->apEItem[i].pExpr, xFunc, pCtx);
    }
  }
  return XJD1_OK;
}

/*
** Walk an expression tree
*/
static int walkExpr(Expr *p, int (*xFunc)(Expr *,void *), void *pCtx){
  int rc = XJD1_OK;
  if( p==0 ) return XJD1_OK;
  rc = xFunc(p, pCtx);
  switch( p->eClass ){
    case XJD1_EXPR_BI: {
      walkExpr(p->u.bi.pLeft, xFunc, pCtx);
      walkExpr(p->u.bi.pRight, xFunc, pCtx);
      break;
    }
    case XJD1_EXPR_TRI: {
      walkExpr(p->u.tri.pTest, xFunc, pCtx);
      walkExpr(p->u.tri.pIfTrue, xFunc, pCtx);
      walkExpr(p->u.tri.pIfFalse, xFunc, pCtx);
      break;
    }
    case XJD1_EXPR_TK: {
      /* Nothing to do */
      break;
    }
    case XJD1_EXPR_FUNC: {
      walkExprList(p->u.func.args, xFunc, pCtx);
      break;
    }
    case XJD1_EXPR_JSON: {
      /* Nothing to do */
      break;
    }
    case XJD1_EXPR_ARRAY: {
      walkExprList(p->u.ar, xFunc, pCtx);
      break;
    }
    case XJD1_EXPR_STRUCT: {
      walkExprList(p->u.st, xFunc, pCtx);
      break;
    }
    case XJD1_EXPR_LVALUE: {
      walkExpr(p->u.lvalue.pLeft, xFunc, pCtx);
      break;
    }
  }
  return rc;
}

static int exprResolve(Expr *p, ResolveCtx *pCtx){
  Command *pCmd = pCtx->pStmt->pCmd;
  const char *zDoc;

  zDoc = p->u.id.zId;
  switch( pCmd->eCmdType ){
    case TK_DELETE:
      if( 0==strcmp(zDoc, pCmd->u.del.zName) ) return XJD1_OK;
      break;

    case TK_UPDATE:
      if( 0==strcmp(zDoc, pCmd->u.update.zName) ) return XJD1_OK;
      break;

    case TK_SELECT: {
      ResolveCtx *pTest;
      for(pTest=pCtx; pTest; pTest=pTest->pParent){
        Query *pQuery = pTest->pQuery;
        if( pQuery ){
          int eExpr = pTest->eExpr;
          int bFound = 0;

          assert( pQuery->eQType==TK_SELECT || eExpr==XJD1_EXPR_ORDERBY );
          assert( eExpr>0 );

          /* Search the FROM clause. */
          if( pQuery->eQType==TK_SELECT && (
                eExpr==XJD1_EXPR_RESULT  || eExpr==XJD1_EXPR_WHERE ||
                eExpr==XJD1_EXPR_GROUPBY || eExpr==XJD1_EXPR_HAVING ||
                eExpr==XJD1_EXPR_ORDERBY
                )){
            int iDatasrc = xjd1DataSrcResolve(pQuery->u.simple.pFrom, zDoc);
            if( iDatasrc ){
              p->u.id.iDatasrc = iDatasrc;
              bFound = 1;
            }
          }

          /* Match against any 'AS' alias on the query result */
          if( bFound==0 && pQuery->zAs ){
            if( eExpr==XJD1_EXPR_ORDERBY
                || eExpr==XJD1_EXPR_HAVING
                || (eExpr==XJD1_EXPR_WHERE && pQuery->u.simple.pAgg==0)
              ){
              if( 0==strcmp(zDoc, pQuery->zAs) ){
                bFound = 1;
              }
            }
          }

          if( bFound ){
            p->u.id.pQuery = pQuery;
            return XJD1_OK;
          }
        }
      }
      break;
    }

    default:
      assert( 0 );
      break;
  }

  xjd1StmtError(pCtx->pStmt, XJD1_ERROR, "no such object: %s", zDoc);
  return XJD1_ERROR;
}

/*
** Callback for query expressions
*/
static int walkInitCallback(Expr *p, void *pArg){
  int rc = XJD1_OK;
  ResolveCtx *pCtx = (ResolveCtx *)pArg;
  assert( p );
  p->pStmt = pCtx->pStmt;
  p->pQuery = pCtx->pQuery;
  switch( p->eClass ){
    case XJD1_EXPR_Q:
      rc = xjd1QueryInit(p->u.subq.p, pCtx->pStmt, pArg);
      break;

    case XJD1_EXPR_FUNC: {
      rc = xjd1FunctionInit(p, pCtx->pStmt, pCtx->pQuery, pCtx->eExpr);
      break;
    }

    case XJD1_EXPR_TK: {
      rc = exprResolve(p, pCtx);
      break;
    }

    default:
      break;
  }

  return rc;
}


/*
** Initialize an expression in preparation for evaluation of a
** statement.
*/
int xjd1ExprInit(
  Expr *p,                        /* Expression to initialize */
  xjd1_stmt *pStmt,               /* Statement expression belongs to */
  Query *pQuery,                  /* Query expression belongs to (or NULL) */
  int eExpr,                      /* How the expression features in the query */
  void *pCtx                      /* Parent resolution context */
){
  ResolveCtx sCtx;
  assert( pQuery==0 || pQuery->pStmt==pStmt );
  sCtx.pStmt = pStmt;
  sCtx.pQuery = pQuery;
  sCtx.eExpr = eExpr;
  sCtx.pParent = (ResolveCtx *)pCtx;
  return walkExpr(p, walkInitCallback, (void *)&sCtx);
}

/*
** Initialize a list of expression in preparation for evaluation of a
** statement.
*/
int xjd1ExprListInit(
  ExprList *p,                    /* List of expressions to initialize */
  xjd1_stmt *pStmt,               /* Statement expressions belong to */
  Query *pQuery,                  /* Query expressions belong to (or NULL) */
  int eExpr,                      /* How the expressions feature in the query */
  void *pCtx                      /* Parent resolution context */
){
  ResolveCtx sCtx;
  assert( pQuery==0 || pQuery->pStmt==pStmt );
  sCtx.pStmt = pStmt;
  sCtx.pQuery = pQuery;
  sCtx.eExpr = eExpr;
  sCtx.pParent = (ResolveCtx *)pCtx;
  return walkExprList(p, walkInitCallback, (void *)&sCtx);
}


/* Walker callback for ExprClose() */
static int walkCloseQueryCallback(Expr *p, void *pCtx){
  int rc = XJD1_OK;
  if( p->eType==TK_SELECT ){
    rc = xjd1QueryClose(p->u.subq.p);
  }
  return rc;
}

/*
** Close all subqueries in an expression.
*/
int xjd1ExprClose(Expr *p){
  return walkExpr(p, walkCloseQueryCallback, 0);
}

/*
** Close all subqueries in an expression list.
*/
int xjd1ExprListClose(ExprList *p){
  return walkExprList(p, walkCloseQueryCallback, 0);
}

/*
** Return true if the JSON object is a string
*/
static int isStr(const JsonNode *p){
  if( p==0 ) return 0;
  switch( p->eJType ){
    case XJD1_TRUE:
    case XJD1_FALSE:
    case XJD1_NULL:
    case XJD1_REAL:
      return 0;
  }
  return 1;
}

/*
** Return non-zero if the JSON object passed should be considered TRUE in a
** boolean context. For example in the result of a WHERE or HAVING clause.
**
** XJD1 uses the same rules as Javascript does to determine which
** values are considered TRUE:
**
**   1. Arrays and objects are always considered true.
**   2. Strings are false if they are zero bytes in length, otherwise true.
**   3. Numbers are false if equal to zero, or true otherwise.
**   4. NULL values are false.
**   5. "true" and "false" are "true" and "false". Respectively.
*/
static int isTrue(const JsonNode *p){
  int res = 0;                    /* Return value */
  switch( p->eJType ){
    case XJD1_REAL:
      res = p->u.r!=0.0;
      break;

    case XJD1_STRING:
      res = p->u.z[0]!='\0';
      break;

    case XJD1_ARRAY:
    case XJD1_STRUCT:
    case XJD1_TRUE:
      res = 1;
      break;
  }
  assert( res==1 || res==0 );
  return res;
}

/*
** Allocate a NULL JSON object.
*/
static JsonNode *nullJson(void){
  JsonNode *pRes = xjd1JsonNew(0);
  if( pRes ) pRes->eJType = XJD1_NULL;
  return pRes;
}

/*
** If the JSON node passed as the first argument is of type XJD1_STRUCT,
** attempt to return a pointer to property zProperty.
**
** If zProperty is not defined, or if pStruct is not of type XJD1_STRUCT,
** return a pointer to a NULL value.
*/
static JsonNode *getProperty(JsonNode *pStruct, const char *zProperty){
  JsonStructElem *pElem;
  JsonNode *pRes = 0;

  if( pStruct && pStruct->eJType==XJD1_STRUCT ){
    for(pElem=pStruct->u.st.pFirst; pElem; pElem=pElem->pNext){
      if( strcmp(pElem->zLabel, zProperty)==0 ){
        pRes = xjd1JsonRef(pElem->pValue);
        break;
      }
    }
  }

  if( pRes==0 ){
    pRes = nullJson();
  }
  return pRes;
}

/*
** Return TRUE if and only if all of the following are true:
**
**   (1)  pA exists
**   (2)  pB exists and is either an array or a structure
**   (3)  pA is a label within pB
*/
static int inOperator(JsonNode *pA, JsonNode *pB){
  char *zLHS;
  int rc = 0;
  char zBuf[100];
  if( pA==0 ) return 0;
  if( pB==0 ) return 0;
  switch( pA->eJType ){
    case XJD1_FALSE:
    case XJD1_TRUE:
    case XJD1_NULL:
    case XJD1_ARRAY:
    case XJD1_STRUCT:
      return 0;
    case XJD1_REAL:
      sqlite3_snprintf(sizeof(zBuf), zBuf, "%g", pA->u.r);
      zLHS = zBuf;
      break;
    case XJD1_STRING:
      zLHS = pA->u.z;
      break;
  }
  switch( pB->eJType ){
    case XJD1_ARRAY: {
      char *zTail;
      int x = strtol(zLHS, &zTail, 0);
      if( zTail[0]!=0 ) return 0;
      rc = (x>=0 && x<pB->u.ar.nElem);
      break;
    }
    case XJD1_STRUCT: {
      JsonStructElem *p;
      for(p=pB->u.st.pFirst; p; p=p->pNext){
        if( strcmp(p->zLabel, zLHS)==0 ){
          rc = 1;
          break;
        }
      }
      break;
    }
    default: {
      rc = 0;
    }
  }
  return rc;
}

/*
** Evaluate an expression.  Return the result as a JSON object.
** Return TRUE if and only if all of the following are true:
**
**   (1)  pA exists
**   (2)  pB exists and is an array or structure.
**   (3)  pA is value contained within pB
*/
static int withinOperator(JsonNode *pA, JsonNode *pB){
  int rc = 0;
  if( pA==0 ) return 0;
  if( pB==0 ) return 0;
  switch( pB->eJType ){
    case XJD1_ARRAY: {
      int i;
      for(i=0; i<pB->u.ar.nElem; i++){
        if( xjd1JsonCompare(pA, pB->u.ar.apElem[i], 0)==0 ){
          rc = 1;
          break;
        }
      }
      break;
    }
    case XJD1_STRUCT: {
      JsonStructElem *p;
      for(p=pB->u.st.pFirst; p; p=p->pNext){
        if( xjd1JsonCompare(pA, p->pValue, 0)==0 ){
          rc = 1;
          break;
        }
      }
      break;
    }
    default: {
      rc = 0;
    }
  }
  return rc;
}

/*
** Assuming zIn points to the first byte of a UTF-8 character,
** advance zIn to point to the first byte of the next UTF-8 character.
*/
#define XJD1_SKIP_UTF8(zIn) {                          \
    if( (*(zIn++))>=0xc0 ){                            \
          while( (*zIn & 0xc0)==0x80 ){ zIn++; }       \
    }                                                  \
}


/*
** Evaluate an expression.  Return the result as a JSON object.
**
** The caller must free the returned JSON by a call xjdJsonFree().
*/
JsonNode *xjd1ExprEval(Expr *p){
  JsonNode *pRes;
  double rLeft, rRight;
  int c;
  JsonNode *pJLeft, *pJRight;

  if( p==0 ) return nullJson();
  switch( p->eType ){
    case TK_JVALUE: {
      return xjd1JsonRef(p->u.json.p);
    }

    case TK_DOT: {
      JsonNode *pBase = xjd1ExprEval(p->u.lvalue.pLeft);
      pRes = getProperty(pBase, p->u.lvalue.zId);
      xjd1JsonFree(pBase);
      return pRes;
    }

    /* The x[y] operator. The result depends on the type of value x.
    **
    ** If x is of type XJD1_STRUCT, then expression y is converted to
    ** a string. The value returned is the value of property y of
    ** object x.
    **
    ** If x is of type XJD1_ARRAY, then expression y is converted to
    ** a number. If that number is an integer, then it is the index of
    ** the array element to return.
    **
    ** If x is of type XJD1_STRING, then it is treated as an array of
    ** characters. Processing proceeds as for XJD1_ARRAY.
    */
    case TK_LB: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      pJRight = xjd1ExprEval(p->u.bi.pRight);
      pRes = 0;

      switch( pJLeft->eJType ){
        case XJD1_STRUCT: {
          String idx;
          xjd1StringInit(&idx, 0, 0);
          xjd1JsonToString(pJRight, &idx);
          pRes = getProperty(pJLeft, idx.zBuf);
          xjd1StringClear(&idx);
          break;
        }

        case XJD1_ARRAY: {
          int iIdx;
          if( xjd1JsonToReal(pJRight, &rRight) ) break;
          iIdx = (int)rRight;
          if( (double)iIdx==rRight && iIdx>=0 && iIdx<pJLeft->u.ar.nElem ){
            pRes = xjd1JsonRef(pJLeft->u.ar.apElem[iIdx]);
          }
          break;
        }

        case XJD1_STRING: {
          int iIdx;
          if( xjd1JsonToReal(pJRight, &rRight) ) break;
          iIdx = (int)rRight;
          if( (double)iIdx==rRight && iIdx>=0 ){
            unsigned char *z = (unsigned char*)pJLeft->u.z;
            for(; *z && iIdx!=0; iIdx--){
              XJD1_SKIP_UTF8(z);
            }
            if( *z ){
              String x;
              unsigned char *zEnd = (unsigned char*)z;
              pRes = xjd1JsonNew(0);
              if( pRes ){
                XJD1_SKIP_UTF8(zEnd);
                xjd1StringInit(&x, 0, 0);
                xjd1StringAppend(&x, (char*)z, zEnd-(unsigned char*)z);
                pRes->eJType = XJD1_STRING;
                pRes->u.z = xjd1StringGet(&x);
              }
            }
          }
          break;
        }

        default:
          break;
      }

      xjd1JsonFree(pJLeft);
      xjd1JsonFree(pJRight);
      if( pRes==0 ) pRes = nullJson();
      return pRes;
    }

    case TK_ID: {
      if( p->u.id.pQuery ){
        assert( p->pStmt->pCmd->eCmdType==TK_SELECT );
        return xjd1QueryDoc(p->u.id.pQuery, p->u.id.iDatasrc);
      }else{
        assert( p->pStmt->pCmd->eCmdType==TK_DELETE
             || p->pStmt->pCmd->eCmdType==TK_UPDATE
        );
        return xjd1StmtDoc(p->pStmt);
      }
    }

    /* The following two logical operators work in the same way as their
    ** javascript counterparts. i.e.
    **
    **    1. "x AND y" is equivalent to "x ? y : x"
    **    2. "x OR y" is equivalent to "x ? x : y"
    */
    case TK_AND:
    case TK_OR: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      if( isTrue(pJLeft)==(p->eType==TK_OR) ){
        pRes = pJLeft;
      }else{
        xjd1JsonFree(pJLeft);
        pRes = xjd1ExprEval(p->u.bi.pRight);
      }
      return pRes;
    }

    case TK_FUNCTION: {
      pRes = xjd1FunctionEval(p);
      return pRes;
    }

    /* A scalar sub-query. The result of this is the first object
    ** returned by executing the query. Or, if the query returns zero
    ** rows, a NULL value.
    **
    ** TODO: Handle correlated and uncorrelated sub-queries differently.
    */
    case TK_SELECT: {
      Query *pQuery = p->u.subq.p;
      int rc;
      rc = xjd1QueryStep(pQuery);
      if( rc==XJD1_ROW ){
        pRes = xjd1QueryDoc(pQuery, 0);
      }else if( rc==XJD1_DONE ){
        pRes = xjd1JsonNew(0);
        if( pRes ) pRes->eJType = XJD1_NULL;
      }
      xjd1QueryRewind(pQuery);
      return pRes;
    }
  }


  pRes = xjd1JsonNew(0);
  if( pRes==0 ) return 0;
  pRes->eJType = XJD1_NULL;
  switch( p->eType ){
    case TK_STRUCT: {
      int i;
      JsonStructElem *pElem, **ppPrev;
      ExprList *pList = p->u.st;
      ppPrev = &pRes->u.st.pFirst;
      pRes->eJType = XJD1_STRUCT;
      for(i=0; i<pList->nEItem; i++){
        ExprItem *pItem = &pList->apEItem[i];
        pElem = xjd1_malloc( sizeof(*pElem) );
        if( pElem==0 ) break;
        *ppPrev = pRes->u.st.pLast = pElem;
        ppPrev = &pElem->pNext;
        memset(pElem, 0, sizeof(*pElem));
        pElem->zLabel = xjd1PoolDup(0, pItem->zAs, -1);
        pElem->pValue = xjd1ExprEval(pItem->pExpr);
      }
      break;
    }
    case TK_ARRAY: {
      int i;
      ExprList *pList = p->u.st;
      pRes->u.ar.apElem = xjd1_malloc( pList->nEItem*sizeof(JsonNode*) );
      if( pRes->u.ar.apElem ){
        pRes->u.ar.nElem = pList->nEItem;
        pRes->eJType = XJD1_ARRAY;
        for(i=0; i<pList->nEItem; i++){
          ExprItem *pItem = &pList->apEItem[i];
          pRes->u.ar.apElem[i] = xjd1ExprEval(pItem->pExpr);
        }
      }
      break;
    }
    case TK_LIKEOP:
    case TK_ILIKEOP:
    case TK_EQEQ:
    case TK_NE:
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      pJRight = xjd1ExprEval(p->u.bi.pRight);
      c = xjd1JsonCompare(pJLeft, pJRight, (p->eType == TK_ILIKEOP));
      xjd1JsonFree(pJLeft);
      xjd1JsonFree(pJRight);
      switch( p->eType ){
	case TK_ILIKEOP:
	case TK_LIKEOP:
        case TK_EQEQ: c = c==0;   break;
        case TK_NE:   c = c!=0;   break;
        case TK_LT:   c = c<0;    break;
        case TK_LE:   c = c<=0;   break;
        case TK_GT:   c = c>0;    break;
        case TK_GE:   c = c>=0;   break;
      }
      pRes->eJType = c ? XJD1_TRUE : XJD1_FALSE;
      break;
    }
    case TK_PLUS: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      pJRight = xjd1ExprEval(p->u.bi.pRight);
      if( isStr(pJLeft) || isStr(pJRight) ){
        String x;
        xjd1StringInit(&x, 0, 0);
        xjd1JsonToString(pJLeft, &x);
        xjd1JsonToString(pJRight, &x);
        pRes->eJType = XJD1_STRING;
        pRes->u.z = xjd1StringGet(&x);
      }else{
        xjd1JsonToReal(pJLeft, &rLeft);
        xjd1JsonToReal(pJRight, &rRight);
        pRes->u.r = rLeft+rRight;
        pRes->eJType = XJD1_REAL;
      }
      xjd1JsonFree(pJLeft);
      xjd1JsonFree(pJRight);
      break;
    }


    case TK_STAR:
    case TK_SLASH:
    case TK_MINUS: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      xjd1JsonToReal(pJLeft, &rLeft);
      if( p->u.bi.pRight==0 ){
        assert( p->eType==TK_MINUS );
        pRes->u.r = -1.0 * rLeft;
      }else{
        pJRight = xjd1ExprEval(p->u.bi.pRight);
        xjd1JsonToReal(pJRight, &rRight);

        switch( p->eType ){
          case TK_MINUS:
            pRes->u.r = rLeft-rRight;
            break;

          case TK_SLASH:
            if( rRight!=0.0 ){
              pRes->u.r = rLeft / rRight;
            }
            break;

          case TK_STAR:
            pRes->u.r = rLeft * rRight;
            break;
        }
        xjd1JsonFree(pJRight);
      }
      pRes->eJType = XJD1_REAL;
      xjd1JsonFree(pJLeft);
      break;
    }

    case TK_BANG: {
      if( xjd1ExprTrue(p->u.bi.pLeft) ){
        pRes->eJType = XJD1_FALSE;
      }else{
        pRes->eJType = XJD1_TRUE;
      }
      break;
    }

    /* Bitwise operators: &, |, <<, >> and ~.
    **
    ** These follow the javascript conventions. Arguments are converted to
    ** 64-bit real numbers, and then to 32-bit signed integers. The bitwise
    ** operation is performed and the result converted back to a 64-bit
    ** real number.
    **
    ** TBD: When XJD1 is enhance to feature an arbitrary precision integer
    ** type, these will have to change somehow.
    **
    ** This block also contains the implementation of the modulo operator.
    ** As it requires the same 32-bit integer conversions as the bitwise
    ** operators.
    */
    case TK_REM:
    case TK_RSHIFT:
    case TK_LSHIFT:
    case TK_BITAND:
    case TK_BITOR: {
      int iLeft;
      int iRight;

      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      pJRight = xjd1ExprEval(p->u.bi.pRight);
      xjd1JsonToReal(pJLeft, &rLeft);
      xjd1JsonToReal(pJRight, &rRight);
      iLeft = rLeft;
      iRight = rRight;

      pRes->eJType = XJD1_REAL;
      switch( p->eType ){
        case TK_RSHIFT:
          if( iRight>=32 ){
            pRes->u.r = (double)(iLeft<0 ? -1 : 0);
          }else{
            pRes->u.r = (double)(iLeft >> iRight);
          }
          break;

        case TK_LSHIFT:
          if( iRight>=32 ){
            pRes->u.r = 0;
          }else{
            pRes->u.r = (double)(iLeft << iRight);
          }
          break;

        case TK_BITAND: pRes->u.r = (double)(iLeft & iRight); break;
        case TK_BITOR:  pRes->u.r = (double)(iLeft | iRight); break;
        case TK_REM:    pRes->u.r = (double)(iLeft % iRight); break;
      }
      break;
    }

    case TK_BITNOT: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      xjd1JsonToReal(pJLeft, &rLeft);
      pRes->eJType = XJD1_REAL;
      pRes->u.r = (double)(~((int)rLeft));
      break;
    }

    case TK_QM: {
      if( xjd1ExprTrue(p->u.tri.pTest) ){
        pRes = xjd1ExprEval(p->u.tri.pIfTrue);
      }else{
        pRes = xjd1ExprEval(p->u.tri.pIfFalse);
      }
      break;
    }

    case TK_IN: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      pJRight = xjd1ExprEval(p->u.bi.pRight);
      pRes->eJType = inOperator(pJLeft, pJRight) ? XJD1_TRUE : XJD1_FALSE;
      xjd1JsonFree(pJLeft);
      xjd1JsonFree(pJRight);
      break;
    }

    case TK_WITHIN: {
      pJLeft = xjd1ExprEval(p->u.bi.pLeft);
      pJRight = xjd1ExprEval(p->u.bi.pRight);
      pRes->eJType = withinOperator(pJLeft, pJRight) ? XJD1_TRUE : XJD1_FALSE;
      xjd1JsonFree(pJLeft);
      xjd1JsonFree(pJRight);
      break;
    }

    default: {
      pRes->eJType = XJD1_NULL;
      break;
    }
  }
  return pRes;
}

/*
** Return non-zero if the evaluation of the given expression should be
** considered TRUE in a boolean context. For example in result of a
** WHERE or HAVING clause.
*/
int xjd1ExprTrue(Expr *p){
  int rc = 0;
  JsonNode *pValue = xjd1ExprEval(p);
  if( pValue ){
    rc = isTrue(pValue);
    assert( rc==1 || rc==0 );
    xjd1JsonFree(pValue);
  }
  return rc;
}

