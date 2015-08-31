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
*****************************************************************************
** Language grammar
*/

// All token codes are small integers with #defines that begin with "TK_"
%token_prefix TK_

// The type of the data attached to each token is Token.  This is also the
// default type for non-terminals.
//
%token_type {Token}
%default_type {Token}

// The generated parser function takes a 4th argument as follows:
%extra_argument {Parse *p}

// The name of the generated procedure that implements the parser
// is as follows:
%name xjd1Parser

// Handle syntax errors.
%syntax_error {
  xjd1ParseError(p, XJD1_SYNTAX,
      "syntax error near \"%.*s\"", p->sTok.n, p->sTok.z
  );
}

// The following text is included near the beginning of the C source
// code file that implements the parser.
//
%include {
#include "xjd1Int.h"

/*
** Disable all error recovery processing in the parser push-down
** automaton.
*/
#define YYNOERRORRECOVERY 1

/*
** Make yytestcase() be a no-op
*/
#define yytestcase(X)

} // end %include

// Input is a single XJD1 command
%type cmd {Command*}
%destructor cmd {(void)p;}
input ::= cmd(X) SEMI.   {p->pCmd = X;}

/////////////////////////// Expression Processing /////////////////////////////
//

%include {
  /* A JSON literal for a real number */
  static JsonNode *jsonReal(Parse *p, Token *pTok){
    JsonNode *pNew = xjd1JsonNew(p->pPool);
    if( pNew ){
      pNew->eJType = XJD1_REAL;
      pNew->u.r = atof(pTok->z);
    }
    return pNew;
  }

  /* Convert a token into a zero-terminated string */
  static char *tokenStr(Parse *p, Token *pTok){
    char *z;
    if( pTok ){
      z = xjd1PoolDup(p->pPool, pTok->z, pTok->n);
      if( z && z[0]=='"' ) xjd1DequoteString(z, pTok->n);
    }else{
      z = 0;
    }
    return z;
  }

  /* A JSON literal for a string */
  static JsonNode *jsonString(Parse *p, Token *pTok){
    JsonNode *pNew = xjd1JsonNew(p->pPool);
    if( pNew ){
      pNew->eJType = XJD1_STRING;
      pNew->u.z = tokenStr(p, pTok);
    }
    return pNew;
  }

  /* A JSON literal for a boolean or NULL */
  static JsonNode *jsonType(Parse *p, int eJType){
    JsonNode *pNew = xjd1JsonNew(p->pPool);
    if( pNew ){
      pNew->eJType = eJType;
    }
    return pNew;
  }
}
%type jvalue {JsonNode*}
jvalue(A) ::= INTEGER(X).              {A = jsonReal(p,&X);}
jvalue(A) ::= FLOAT(X).                {A = jsonReal(p,&X);}
jvalue(A) ::= STRING(X).               {A = jsonString(p,&X);}
jvalue(A) ::= TRUE.                    {A = jsonType(p,XJD1_TRUE);}
jvalue(A) ::= FALSE.                   {A = jsonType(p,XJD1_FALSE);}
jvalue(A) ::= NULL.                    {A = jsonType(p,XJD1_NULL);}

%right QM.
%left OR.
%left AND.
%left BITOR.
%left BITXOR.
%left BITAND.
%left ILIKEOP LIKEOP NE EQEQ EQ3 NE3.
%left WITHIN IN GT LE LT GE.
%left LSHIFT RSHIFT URSHIFT.
%left PLUS MINUS.
%left STAR SLASH REM.
%right BITNOT BANG.
%left COLLATE.

%include {
  /* Generate an Expr object from an identifer token */
  static Expr *idExpr(Parse *p, Token *pTok){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_ID;
      pNew->eClass = XJD1_EXPR_TK;
      pNew->u.id.zId = tokenStr(p, pTok);
    }
    return pNew;
  }

  /* Generate an Expr object that is a binary operator on two
  ** other Expr objects. */
  static Expr *biExpr(Parse *p, Expr *pLeft, int eOp, Expr *pRight){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = eOp; 
      pNew->eClass = XJD1_EXPR_BI;
      pNew->u.bi.pLeft = pLeft;
      pNew->u.bi.pRight = pRight;
    }
    return pNew;
  }

  /* Generate an Expr object that is a tertiary operator */
  static Expr *triExpr(Parse *p, Expr *pTest, Expr *pTrue, Expr *pFalse){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_QM; 
      pNew->eClass = XJD1_EXPR_TRI;
      pNew->u.tri.pTest = pTest;
      pNew->u.tri.pIfTrue = pTrue;
      pNew->u.tri.pIfFalse = pFalse;
    }
    return pNew;
  }

  /* Generate an Expr object for an lvalue */
  static Expr *lvalueExpr(Parse *p, Expr *pLeft, Token *pId){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_DOT; 
      pNew->eClass = XJD1_EXPR_LVALUE;
      pNew->u.lvalue.pLeft = pLeft;
      pNew->u.lvalue.zId = tokenStr(p, pId);
    }
    return pNew;
  }

  /* Generate an Expr object that is a function call. */
  static Expr *funcExpr(Parse *p, Token *pFName, ExprList *pArgs){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_FUNCTION; 
      pNew->eClass = XJD1_EXPR_FUNC;
      pNew->u.func.zFName = tokenStr(p, pFName);
      if( pArgs==0 ){
        pArgs = xjd1PoolMallocZero(p->pPool, sizeof(ExprList));
      }
      pNew->u.func.args = pArgs;
    }
    return pNew;
  }

  /* Generate an Expr object that is a subquery. */
  static Expr *subqExpr(Parse *p, Query *pQuery){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_SELECT;
      pNew->eClass = XJD1_EXPR_Q;
      pNew->u.subq.p = pQuery;
    }
    return pNew;
  }

  /* Generate an Expr object that is a structure */
  static Expr *stExpr(Parse *p, ExprList *pList){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_STRUCT;
      pNew->eClass = XJD1_EXPR_STRUCT;
      if( pList ){
        pNew->u.st = pList;
      }else{
        pNew->u.st = (ExprList *)xjd1PoolMallocZero(p->pPool, sizeof(ExprList));
      }
    }
    return pNew;
  }

  /* Generate an Expr object that is an array7 */
  static Expr *arExpr(Parse *p, ExprList *pList){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_ARRAY;
      pNew->eClass = XJD1_EXPR_ARRAY;
      if( pList ){
        pNew->u.ar = pList;
      }else{
        pNew->u.ar = (ExprList *)xjd1PoolMallocZero(p->pPool, sizeof(ExprList));
      }
    }
    return pNew;
  }

  /* Generate an Expr object that is JSON value literal */
  static Expr *jsonExpr(Parse *p, JsonNode *pNode){
    Expr *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eType = TK_JVALUE;
      pNew->eClass = XJD1_EXPR_JSON;
      pNew->u.json.p = pNode;
    }
    return pNew;
  }

  /* Append a new expression to an expression list.  Allocate the
  ** expression list object if necessary. */
  static ExprList *apndExpr(Parse *p, ExprList *pList, Expr *pExpr, Token *pT){
    ExprItem *pItem;
    if( pList==0 ){
      pList = xjd1PoolMallocZero(p->pPool, sizeof(*pList)+4*sizeof(ExprItem));
      if( pList==0 ) return 0;
      pList->nEAlloc = 0;
    }
    if( pList->nEAlloc<=pList->nEItem ){
      ExprItem *pNew;
      int n = pList->nEAlloc*4;
      if( n==0 ) n = 10;
      pNew = xjd1PoolMalloc(p->pPool, sizeof(ExprItem)*n);
      if( pNew==0 ) return pList;
      memcpy(pNew, pList->apEItem, pList->nEItem*sizeof(ExprItem));
      pList->nEAlloc = n;
      pList->apEItem = pNew;
    }
    pItem = &pList->apEItem[pList->nEItem++];
    pItem->zAs = tokenStr(p, pT);
    pItem->pExpr = pExpr;
    return pList;
  }
}

%type lvalue {Expr*}
lvalue(A) ::= ID(X).                   {A = idExpr(p,&X);}
lvalue(A) ::= lvalue(X) DOT ID(Y).     {A = lvalueExpr(p,X,&Y);}
lvalue(A) ::= lvalue(X) LB expr(Y) RB. {A = biExpr(p,X,TK_LB,Y);}

%type expr {Expr*}
expr(A) ::= lvalue(X).               {A = X;}
expr(A) ::= jvalue(X).               {A = jsonExpr(p,X);}
expr(A) ::= LC structlist(X) RC.     {A = stExpr(p,X);}
expr(A) ::= LC RC.                   {A = stExpr(p,0);}
expr(A) ::= LB arraylist(X) RB.      {A = arExpr(p,X);}
expr(A) ::= LB RB.                   {A = arExpr(p,0);}

%type structlist {ExprList*}
structlist(A) ::= ID|STRING(Y) COLON expr(Z).    {A = apndExpr(p,0,Z,&Y);}
structlist(A) ::= structlist(X) COMMA ID|STRING(Y) COLON expr(Z).
                                                 {A = apndExpr(p,X,Z,&Y);}
%type arraylist {ExprList*}
arraylist(A) ::= expr(Y).                        {A = apndExpr(p,0,Y,0);}
arraylist(A) ::= arraylist(X) COMMA expr(Y).     {A = apndExpr(p,X,Y,0);}



expr(A) ::= ID(X) LP exprlist(Y) RP.  {A = funcExpr(p,&X,Y);}
expr(A) ::= expr(X) AND(OP) expr(Y).  {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) OR(OP) expr(Y).              {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) LT|GT|GE|LE(OP) expr(Y).     {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) EQEQ|NE|EQ3|NE3(OP) expr(Y). {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) BITAND(OP) expr(Y).          {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) BITXOR(OP) expr(Y).          {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) BITOR(OP) expr(Y).           {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) LSHIFT|RSHIFT|URSHIFT(OP) expr(Y).
                                                 {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) PLUS|MINUS(OP) expr(Y).      {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) STAR|SLASH|REM(OP) expr(Y).  {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) IN|WITHIN(OP) expr(Y).       {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) ILIKEOP(OP) expr(Y).          {A = biExpr(p,X,@OP,Y);}
expr(A) ::= expr(X) LIKEOP(OP) expr(Y).          {A = biExpr(p,X,@OP,Y);}
expr(A) ::= BANG(OP) expr(X).                    {A = biExpr(p,X,@OP,0);}
expr(A) ::= BITNOT(OP) expr(X).                  {A = biExpr(p,X,@OP,0);}
expr(A) ::= MINUS(OP) expr(X). [BITNOT]          {A = biExpr(p,X,@OP,0);}
expr(A) ::= PLUS(OP) expr(X). [BITNOT]           {A = biExpr(p,X,@OP,0);}
expr(A) ::= LP select(X) RP.                     {A = subqExpr(p,X);}
expr(A) ::= LP expr(X) RP.                       {A = X;}
expr(A) ::= expr(X) COLLATE ID|STRING.           {A = X;}
expr(A) ::= expr(X) QM expr(Y) COLON expr(Z).    {A = triExpr(p,X,Y,Z);}

%type exprlist {ExprList*}
%type nexprlist {ExprList*}
exprlist(A) ::= nexprlist(X).     {A = X;}
exprlist(A) ::= .                 {A = 0;}
nexprlist(A) ::= expr(X).                     {A = apndExpr(p,0,X,0);}
nexprlist(A) ::= nexprlist(X) COMMA expr(Y).  {A = apndExpr(p,X,Y,0);}

//////////////////////// The SELECT statement /////////////////////////////////
//
cmd(A) ::= select(X).  {
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_SELECT;
    pNew->u.q.pQuery = X;
  }
  A = pNew;
}

%left UNION EXCEPT.
%left INTERSECT.
%type select {Query*}
%type selectcore {Query*}
%type compound {Query*}
%type esel {Query*}
%type x {Query*}

%include {
  /* The value of a LIMIT ... OFFSET ... clause. */
  typedef struct LimitOffset {
    Expr *pLimit;
    Expr *pOffset;
  } LimitOffset;

  /* The value of a GROUP BY ... HAVING ... clause */
  typedef struct GroupByHaving {
    ExprList *pGroupBy;
    Expr *pHaving;
  } GroupByHaving;

  /* Construct a simple query object */
  static Query *simpleQuery(
    Parse *p,
    int isDistinct,
    Expr *pRes,
    const char *zAs,
    DataSrc *pFrom,
    Expr *pWhere,
    GroupByHaving *pGroupBy
  ){
    Query *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eQType = TK_SELECT;
      pNew->u.simple.isDistinct = isDistinct;
      pNew->u.simple.pRes = pRes;
      pNew->zAs = zAs;
      pNew->u.simple.pFrom = pFrom;
      pNew->u.simple.pWhere = pWhere;
      pNew->u.simple.pGroupBy = pGroupBy ? pGroupBy->pGroupBy : 0;
      pNew->u.simple.pHaving = pGroupBy ? pGroupBy->pHaving : 0;
    }
    return pNew;
  }

  /* Construct a compound query object */
  static Query *compoundQuery(
    Parse *p,
    Query *pLeft,
    int eOp,
    Query *pRight
  ){
    Query *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eQType = eOp;
      pNew->u.compound.pLeft = pLeft;
      pNew->u.compound.pRight = pRight;
      pNew->zAs = pLeft->zAs;
    }
    return pNew;
  }
}



select(A) ::= compound(Q) orderby_opt(O) limit_opt(L). {
  Q->pOrderBy = O;
  Q->pLimit = L.pLimit;
  Q->pOffset = L.pOffset;
  A = Q;
}
compound(A) ::= selectcore(X).                 {A = X;}
compound(A) ::= esel(X) UNION(OP) esel(Y).     {A=compoundQuery(p,X,@OP,Y);}
compound(A) ::= esel(X) UNION ALL(OP) esel(Y). {A=compoundQuery(p,X,@OP,Y);}
compound(A) ::= esel(X) EXCEPT(OP) esel(Y).    {A=compoundQuery(p,X,@OP,Y);}
compound(A) ::= esel(X) INTERSECT(OP) esel(Y). {A=compoundQuery(p,X,@OP,Y);}
esel(A) ::= compound(X).                       {A = X;}
esel(A) ::= expr(X). {
  /* TODO: Fix these error messages */
  if( p->errCode==XJD1_OK ){
    if( X->eClass!=XJD1_EXPR_Q ){
      xjd1ParseError(p, XJD1_SYNTAX,
        "syntax error near \"%.*s\"", p->sTok.n, p->sTok.z
      );
    }else{
      A = X->u.subq.p;
      if( A->pOrderBy || A->pLimit ){
        xjd1ParseError(p, XJD1_SYNTAX,
          "syntax error near \"%.*s\"", p->sTok.n, p->sTok.z
        );
      }
    }
  }
}

selectcore(A) ::= SELECT 
  dist_opt(D) sel_result(S) from(F) where_opt(W) groupby_opt(G).
  {A = simpleQuery(p,D,S.pExpr,S.zAs,F,W,&G);}


%type dist_opt {int}
dist_opt(A) ::= .                    {A = 0;}
dist_opt(A) ::= DISTINCT.            {A = 1;}
dist_opt(A) ::= ALL.                 {A = 0;}

// The result set of an expression can be either an JSON expression
// or nothing. If it is a JSON expression, it may be given an alias
// using the "AS <id>" syntax.
//
%type sel_result {ExprItem}
sel_result(A) ::= .                       {A.pExpr=0;A.zAs=0;}
sel_result(A) ::= expr(X).                {A.pExpr=X;A.zAs=0;}
sel_result(A) ::= expr(X) AS ID(Y).       {A.pExpr=X;A.zAs=tokenStr(p, &Y);}

// A complete FROM clause.
//
%type from {DataSrc*}
%type fromlist {DataSrc*}
%type fromitem {DataSrc*}
%include {
  /* Create a new data source that is a named table */
  static DataSrc *pathDataSrc(Parse *p, Expr *pPath, Token *pAs){
    DataSrc *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      if( pPath ){
        if( pPath->eType==TK_ID ){
          pNew->eDSType = TK_ID;
          pNew->u.tab.zName = pPath->u.id.zId;
        }else{
          pNew->eDSType = TK_DOT;
          pNew->u.path.pPath = pPath;
        }
        pNew->zAs = tokenStr(p, pAs);
      }else{
        pNew->eDSType = TK_ID;
        pNew->u.tab.zName = tokenStr(p,pAs);
      }
    }
    return pNew;
  }

  /* Create a new data source that is a join */
  static DataSrc *joinDataSrc(Parse *p, DataSrc *pLeft, DataSrc *pRight){
    DataSrc *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eDSType = TK_COMMA;
      pNew->u.join.pLeft = pLeft;
      pNew->u.join.pRight = pRight;
    }
    return pNew;
  }

  /* Create a new subquery data source */
  static DataSrc *subqDataSrc(Parse *p, Query *pSubq, Token *pAs){
    DataSrc *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eDSType = TK_SELECT;
      pNew->u.subq.q = pSubq;
      pNew->zAs = tokenStr(p, pAs);
    }
    return pNew;
  }

  /* Create a new data source that is a FLATTEN or EACH operator */
  static DataSrc *flattenDataSrc(
    Parse *p,
    DataSrc *pLeft,
    Token *pOp,
    Expr *pPath,
    Expr *pAs
  ){
    DataSrc *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eDSType = TK_FLATTENOP;
      pNew->u.flatten.pNext = pLeft;
      pNew->u.flatten.cOpName = pOp->z[0];
      pNew->u.flatten.pExpr = pPath;
      pNew->u.flatten.pAs = (pAs ? pAs : pPath);
    }
    return pNew;
  }

  /* Create a new data source that represents an empty FROM clause.
  ** This is used for queries of the form "SELECT <expr>". It returns a
  ** single object with no properties.  
  */
  static DataSrc *nullDataSrc(Parse *p){
    DataSrc *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eDSType = TK_NULL;
    }
    return pNew;
  }
}
from(A) ::= .                                    {A = nullDataSrc(p);}
from(A) ::= FROM fromlist(X).                    {A = X;}
fromlist(A) ::= fromitem(X).                     {A = X;}
fromlist(A) ::= fromlist(X) COMMA fromitem(Y).   {A = joinDataSrc(p,X,Y);}
fromitem(A) ::= LP select(X) RP AS ID(Y).        {A = subqDataSrc(p,X,&Y);}

fromitem(A) ::= ID(X).                           {A = pathDataSrc(p,0,&X);}
fromitem(A) ::= path(X) AS ID(Y).                {A = pathDataSrc(p,X,&Y);}

fromitem(A) ::= fromitem(W) FLATTENOP(X) LP path(Y) eachalias(Z) RP. {
  A = flattenDataSrc(p,W,&X,Y,Z);
}

%type eachalias {Expr*}
eachalias(A) ::= .                 {A=0;}
eachalias(A) ::= AS ID|STRING(Y).  {A=idExpr(p,&Y);}

%type path {Expr*}
path(A) ::= ID(Y).                               {A = idExpr(p, &Y);        }
path(A) ::= path(X) DOT ID(Y).                   {A = lvalueExpr(p, X, &Y); }
path(A) ::= path(X) LB ID|STRING(Y) RB.          {A = lvalueExpr(p, X, &Y); }

%type groupby_opt {GroupByHaving}
groupby_opt(A) ::= .                            {A.pGroupBy=0; A.pHaving=0;}
groupby_opt(A) ::= GROUP BY exprlist(X).        {A.pGroupBy=X; A.pHaving=0;}
groupby_opt(A) ::= GROUP BY exprlist(X) HAVING expr(Y).
                                                {A.pGroupBy=X; A.pHaving=Y;}

%type orderby_opt {ExprList*}
%type sortlist {ExprList*}
orderby_opt(A) ::= .                          {A = 0;}
orderby_opt(A) ::= ORDER BY sortlist(X).      {A = X;}
sortlist(A) ::= sortlist(X) COMMA expr(Y) sortorder(Z).
                                              {A = apndExpr(p,X,Y,Z.n?&Z:0);}
sortlist(A) ::= expr(Y) sortorder(Z).         {A = apndExpr(p,0,Y,Z.n?&Z:0);}
sortorder(A) ::= ASCENDING(X).    {A = X;}
sortorder(A) ::= DESCENDING(X).   {A = X;}
sortorder(A) ::= .                {A.z=""; A.n=0;}

%type limit_opt {LimitOffset}
limit_opt(A) ::= .                             {A.pLimit=0; A.pOffset=0;}
limit_opt(A) ::= LIMIT expr(X).                {A.pLimit=X; A.pOffset=0;}
limit_opt(A) ::= LIMIT expr(X) OFFSET expr(Y). {A.pLimit=X; A.pOffset=Y;}

%type where_opt {Expr*}
where_opt(A) ::= .                  {A = 0;}
where_opt(A) ::= WHERE expr(X).     {A = X;}

///////////////////// TRANSACTIONS ////////////////////////////
//
cmd ::= BEGIN ID.
cmd ::= ROLLBACK ID.
cmd ::= COMMIT ID.

///////////////////// The CREATE COLLECTION statement ////////////////////////
//
cmd(A) ::= CREATE COLLECTION ifnotexists(B) tabname(N). {
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_CREATECOLLECTION;
    pNew->u.crtab.ifExists = B;
    pNew->u.crtab.zName = tokenStr(p, &N);
  }
  A = pNew;
}
%type ifnotexists {int}
ifnotexists(A) ::= .                    {A = 0;}
ifnotexists(A) ::= IF NOT EXISTS.       {A = 1;}
tabname(A) ::= ID(X).                   {A = X;}

////////////////////////// The DROP COLLECTION ///////////////////////////////
//
cmd(A) ::= DROP COLLECTION ifexists(B) tabname(N). {
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_DROPCOLLECTION;
    pNew->u.crtab.ifExists = B;
    pNew->u.crtab.zName = tokenStr(p, &N);
  }
  A = pNew;
}
%type ifexists {int}
ifexists(A) ::= IF EXISTS.  {A = 1;}
ifexists(A) ::= .           {A = 0;}


/////////////////////////// The DELETE statement /////////////////////////////
//
cmd(A) ::= DELETE FROM tabname(N) where_opt(W). {
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_DELETE;
    pNew->u.del.zName = tokenStr(p, &N);
    pNew->u.del.pWhere = W;
  }
  A = pNew;
}

////////////////////////// The UPDATE command ////////////////////////////////
//
cmd(A) ::= UPDATE tabname(N) SET setlist(L) where_opt(W) upsert_opt(U). {
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_UPDATE;
    pNew->u.update.zName = tokenStr(p, &N);
    pNew->u.update.pWhere = W;
    pNew->u.update.pChng = L;
    pNew->u.update.pUpsert = U;
  }
  A = pNew;
}

%type setlist {ExprList*}
setlist(A) ::= setlist(X) COMMA lvalue(Y) EQ expr(Z). {
   A = apndExpr(p,X,Y,0);
   A = apndExpr(p,A,Z,0);
}
setlist(A) ::= lvalue(Y) EQ expr(Z). {
   A = apndExpr(p,0,Y,0);
   A = apndExpr(p,A,Z,0);
}

%type upsert_opt {Expr*}
upsert_opt(A) ::= .                       {A = 0;}
upsert_opt(A) ::= ELSE INSERT expr(X).    {A = X;}



////////////////////////// The INSERT command /////////////////////////////////
//
cmd(A) ::= async INSERT INTO tabname(N) VALUE expr(V). {
  Command *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_INSERT;
    pNew->u.ins.zName = tokenStr(p, &N);
    pNew->u.ins.pValue = V;
  }
  A = pNew;
}
cmd(A) ::= async INSERT INTO tabname(N) select(Q). {
  Command *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_INSERT;
    pNew->u.ins.zName = tokenStr(p, &N);
    pNew->u.ins.pQuery = Q;
  }
  A = pNew;
}
async ::= .
async ::= ASYNC.
async ::= SYNC.

////////////////////////// The PRAGMA command /////////////////////////////////
//
%include {
  static Command *makePrag(Parse *p, Token *pName, Expr *pValue){
    Command *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eCmdType = TK_PRAGMA;
      pNew->u.prag.zName = tokenStr(p, pName);
      pNew->u.prag.pValue = pValue;
    }
    return pNew;
  }
}
cmd(A) ::= PRAGMA ID(N).                {A = makePrag(p,&N,0);}
cmd(A) ::= PRAGMA ID(N) EQ expr(V).     {A = makePrag(p,&N,V);}
cmd(A) ::= PRAGMA ID(N) LP expr(V) RP.  {A = makePrag(p,&N,V);}
