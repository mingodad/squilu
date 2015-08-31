/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 46 "parse.y"

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

#line 70 "parse.y"

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
#line 134 "parse.y"

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
#line 350 "parse.y"

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
#line 461 "parse.y"

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
#line 690 "parse.y"

  static Command *makePrag(Parse *p, Token *pName, Expr *pValue){
    Command *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
    if( pNew ){
      pNew->eCmdType = TK_PRAGMA;
      pNew->u.prag.zName = tokenStr(p, pName);
      pNew->u.prag.pValue = pValue;
    }
    return pNew;
  }
#line 347 "parse.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    xjd1ParserTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is xjd1ParserTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    xjd1ParserARG_SDECL     A static variable declaration for the %extra_argument
**    xjd1ParserARG_PDECL     A parameter declaration for the %extra_argument
**    xjd1ParserARG_STORE     Code to store %extra_argument into yypParser
**    xjd1ParserARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 120
#define YYACTIONTYPE unsigned short int
#define xjd1ParserTOKENTYPE Token
typedef union {
  int yyinit;
  xjd1ParserTOKENTYPE yy0;
  LimitOffset yy17;
  ExprList* yy28;
  int yy60;
  Query* yy89;
  ExprItem yy90;
  Command* yy156;
  JsonNode* yy181;
  GroupByHaving yy186;
  Expr* yy216;
  DataSrc* yy225;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define xjd1ParserARG_SDECL Parse *p;
#define xjd1ParserARG_PDECL ,Parse *p
#define xjd1ParserARG_FETCH Parse *p = yypParser->p
#define xjd1ParserARG_STORE yypParser->p = p
#define YYNSTATE 219
#define YYNRULE 114
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
#define YY_ACTTAB_COUNT (787)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    31,   43,   46,   38,   39,   40,   33,   32,   41,   41,
 /*    10 */    41,   41,   34,   34,   42,   42,   42,   42,   37,   37,
 /*    20 */    37,   36,   36,   35,   35,   35,  163,   47,  115,   38,
 /*    30 */    39,   40,   33,   32,   41,   41,   41,   41,   34,   34,
 /*    40 */    42,   42,   42,   42,   37,   37,   37,   36,   36,   35,
 /*    50 */    35,   35,  202,  201,  115,   35,   35,   35,  146,  109,
 /*    60 */   115,   31,   43,   46,   38,   39,   40,   33,   32,   41,
 /*    70 */    41,   41,   41,   34,   34,   42,   42,   42,   42,   37,
 /*    80 */    37,   37,   36,   36,   35,   35,   35,  219,  162,  115,
 /*    90 */     4,    7,    6,  211,   31,   43,   46,   38,   39,   40,
 /*   100 */    33,   32,   41,   41,   41,   41,   34,   34,   42,   42,
 /*   110 */    42,   42,   37,   37,   37,   36,   36,   35,   35,   35,
 /*   120 */   165,  162,  115,   36,   36,   35,   35,   35,  212,   30,
 /*   130 */   115,   31,   43,   46,   38,   39,   40,   33,   32,   41,
 /*   140 */    41,   41,   41,   34,   34,   42,   42,   42,   42,   37,
 /*   150 */    37,   37,   36,   36,   35,   35,   35,   59,   45,  115,
 /*   160 */    37,   37,   37,   36,   36,   35,   35,   35,  205,  206,
 /*   170 */   115,  210,  119,   86,  209,  116,  117,   20,   31,   43,
 /*   180 */    46,   38,   39,   40,   33,   32,   41,   41,   41,   41,
 /*   190 */    34,   34,   42,   42,   42,   42,   37,   37,   37,   36,
 /*   200 */    36,   35,   35,   35,  115,    9,  115,   31,   43,   46,
 /*   210 */    38,   39,   40,   33,   32,   41,   41,   41,   41,   34,
 /*   220 */    34,   42,   42,   42,   42,   37,   37,   37,   36,   36,
 /*   230 */    35,   35,   35,   24,  186,  115,  187,   34,   34,   42,
 /*   240 */    42,   42,   42,   37,   37,   37,   36,   36,   35,   35,
 /*   250 */    35,  110,  151,  115,  147,  137,  109,   12,   13,    2,
 /*   260 */    23,   31,   43,   46,   38,   39,   40,   33,   32,   41,
 /*   270 */    41,   41,   41,   34,   34,   42,   42,   42,   42,   37,
 /*   280 */    37,   37,   36,   36,   35,   35,   35,  208,    6,  115,
 /*   290 */    14,   44,  210,  119,   76,  210,  119,   91,  156,  167,
 /*   300 */   199,   53,  111,  199,  197,  152,  196,  108,   31,   43,
 /*   310 */    46,   38,   39,   40,   33,   32,   41,   41,   41,   41,
 /*   320 */    34,   34,   42,   42,   42,   42,   37,   37,   37,   36,
 /*   330 */    36,   35,   35,   35,   56,    8,  115,   46,   38,   39,
 /*   340 */    40,   33,   32,   41,   41,   41,   41,   34,   34,   42,
 /*   350 */    42,   42,   42,   37,   37,   37,   36,   36,   35,   35,
 /*   360 */    35,  163,   47,  115,  135,   22,  218,  217,  216,  215,
 /*   370 */   214,  213,   40,   33,   32,   41,   41,   41,   41,   34,
 /*   380 */    34,   42,   42,   42,   42,   37,   37,   37,   36,   36,
 /*   390 */    35,   35,   35,   26,   27,  115,   21,  135,   28,   29,
 /*   400 */    18,  164,  184,   10,  145,   55,  163,   47,  207,    3,
 /*   410 */   166,  334,  210,  119,   91,   57,  193,   49,  182,   68,
 /*   420 */   199,   53,  111,  194,  192,  210,  119,   91,  191,  133,
 /*   430 */   132,  131,  130,  199,  197,  198,  140,  127,  125,   61,
 /*   440 */   142,  207,  122,  189,  139,   16,  169,  168,  120,  193,
 /*   450 */    39,   40,   33,   32,   41,   41,   41,   41,   34,   34,
 /*   460 */    42,   42,   42,   42,   37,   37,   37,   36,   36,   35,
 /*   470 */    35,   35,  188,  142,  115,   33,   32,   41,   41,   41,
 /*   480 */    41,   34,   34,   42,   42,   42,   42,   37,   37,   37,
 /*   490 */    36,   36,   35,   35,   35,  183,   19,  115,   67,  181,
 /*   500 */   218,  217,  216,  215,  214,  213,  176,  210,  119,   91,
 /*   510 */   210,  119,   91,  141,  128,  199,   53,  111,  199,  197,
 /*   520 */   136,  210,  119,   91,  272,  272,  272,   26,   27,  199,
 /*   530 */   197,  153,   28,   29,  180,  164,  155,   10,  179,   55,
 /*   540 */   210,  119,   90,    3,   65,  134,  174,  149,    5,   57,
 /*   550 */    63,  218,  217,  216,  215,  214,  213,  210,  119,   91,
 /*   560 */    50,  291,  291,  170,   17,  199,   53,  111,  210,  119,
 /*   570 */    90,   58,  121,  148,  204,  149,  291,  112,   26,   27,
 /*   580 */   210,  119,   52,   28,   29,   70,  164,   48,   10,   25,
 /*   590 */    55,  210,  119,   74,    3,  143,  113,  143,  113,   69,
 /*   600 */    57,  195,   20,  154,  150,  190,   71,  210,  119,   78,
 /*   610 */   138,   54,  114,  210,  119,   96,  210,  119,   85,  210,
 /*   620 */   119,   94,  210,  119,   95,  210,  119,  104,   66,  177,
 /*   630 */    15,  129,  178,  218,  217,  216,  215,  214,  213,   64,
 /*   640 */   210,  119,  102,  210,  119,   99,  210,  119,   98,  126,
 /*   650 */   210,  119,   97,  175,  210,  119,  105,  210,  119,  106,
 /*   660 */    26,   27,  210,  119,  161,   28,   29,   62,  164,  173,
 /*   670 */    10,  185,   55,  210,  119,  103,    3,  124,  123,  218,
 /*   680 */   217,  216,  215,  214,  213,  210,  119,  101,  171,  210,
 /*   690 */   119,  100,  210,  119,   77,  210,  119,   93,  210,  119,
 /*   700 */   160,  210,  119,  159,  107,  172,   26,   27,  210,  119,
 /*   710 */   158,   28,   29,    1,  164,  118,   10,   60,   55,   11,
 /*   720 */   203,  144,    3,  210,  119,  157,   72,  335,   57,  335,
 /*   730 */   200,  218,  217,  216,  215,  214,  213,  210,  119,   75,
 /*   740 */   210,  119,   92,  210,  119,   51,  210,  119,   89,  210,
 /*   750 */   119,   88,  210,  119,   87,  210,  119,   84,   26,   27,
 /*   760 */   210,  119,   83,   28,   29,  335,  164,  335,   10,  335,
 /*   770 */    55,  210,  119,   82,    3,  210,  119,   81,  210,  119,
 /*   780 */    80,  210,  119,   79,  210,  119,   73,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*    10 */    18,   19,   20,   21,   22,   23,   24,   25,   26,   27,
 /*    20 */    28,   29,   30,   31,   32,   33,   38,   39,   36,   11,
 /*    30 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*    40 */    22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
 /*    50 */    32,   33,   60,   61,   36,   31,   32,   33,  108,  109,
 /*    60 */    36,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*    70 */    17,   18,   19,   20,   21,   22,   23,   24,   25,   26,
 /*    80 */    27,   28,   29,   30,   31,   32,   33,    0,    4,   36,
 /*    90 */    47,   48,   49,   40,    8,    9,   10,   11,   12,   13,
 /*   100 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   110 */    24,   25,   26,   27,   28,   29,   30,   31,   32,   33,
 /*   120 */     1,   37,   36,   29,   30,   31,   32,   33,   37,   43,
 /*   130 */    36,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   140 */    17,   18,   19,   20,   21,   22,   23,   24,   25,   26,
 /*   150 */    27,   28,   29,   30,   31,   32,   33,   44,   43,   36,
 /*   160 */    26,   27,   28,   29,   30,   31,   32,   33,   46,   46,
 /*   170 */    36,   88,   89,   90,   42,   92,   44,   64,    8,    9,
 /*   180 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   190 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*   200 */    30,   31,   32,   33,   36,   45,   36,    8,    9,   10,
 /*   210 */    11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
 /*   220 */    21,   22,   23,   24,   25,   26,   27,   28,   29,   30,
 /*   230 */    31,   32,   33,   63,   50,   36,   52,   20,   21,   22,
 /*   240 */    23,   24,   25,   26,   27,   28,   29,   30,   31,   32,
 /*   250 */    33,   37,   53,   36,  107,  108,  109,   57,   45,   45,
 /*   260 */    44,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   270 */    17,   18,   19,   20,   21,   22,   23,   24,   25,   26,
 /*   280 */    27,   28,   29,   30,   31,   32,   33,   40,   49,   36,
 /*   290 */    77,   44,   88,   89,   90,   88,   89,   90,   94,   46,
 /*   300 */    96,   97,   98,   96,   97,   98,   37,   89,    8,    9,
 /*   310 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   320 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*   330 */    30,   31,   32,   33,  116,   57,   36,   10,   11,   12,
 /*   340 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   350 */    23,   24,   25,   26,   27,   28,   29,   30,   31,   32,
 /*   360 */    33,   38,   39,   36,    4,   44,    2,    3,    4,    5,
 /*   370 */     6,    7,   13,   14,   15,   16,   17,   18,   19,   20,
 /*   380 */    21,   22,   23,   24,   25,   26,   27,   28,   29,   30,
 /*   390 */    31,   32,   33,   29,   30,   36,   58,   37,   34,   35,
 /*   400 */    77,   37,   42,   39,   55,   41,   38,   39,    4,   45,
 /*   410 */    86,   87,   88,   89,   90,   51,    4,   44,   94,   45,
 /*   420 */    96,   97,   98,   46,   37,   88,   89,   90,   40,   65,
 /*   430 */    66,   67,   68,   96,   97,   98,   46,   73,   74,   75,
 /*   440 */     4,   37,  118,   37,   53,   77,   82,   83,   84,   37,
 /*   450 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   460 */    22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
 /*   470 */    32,   33,   37,   37,   36,   14,   15,   16,   17,   18,
 /*   480 */    19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
 /*   490 */    29,   30,   31,   32,   33,   46,   43,   36,   69,   37,
 /*   500 */     2,    3,    4,    5,    6,    7,   72,   88,   89,   90,
 /*   510 */    88,   89,   90,   94,   71,   96,   97,   98,   96,   97,
 /*   520 */    98,   88,   89,   90,   47,   48,   49,   29,   30,   96,
 /*   530 */    97,   98,   34,   35,   37,   37,   59,   39,   37,   41,
 /*   540 */    88,   89,   90,   45,   69,   93,   72,   95,   50,   51,
 /*   550 */    54,    2,    3,    4,    5,    6,    7,   88,   89,   90,
 /*   560 */    76,   38,   39,   94,   79,   96,   97,   98,   88,   89,
 /*   570 */    90,   80,   79,   93,  101,   95,   53,   37,   29,   30,
 /*   580 */    88,   89,   90,   34,   35,  104,   37,   54,   39,   62,
 /*   590 */    41,   88,   89,   90,   45,   38,   39,   38,   39,  105,
 /*   600 */    51,  106,   64,  111,   56,   37,  103,   88,   89,   90,
 /*   610 */    53,  109,   53,   88,   89,   90,   88,   89,   90,   88,
 /*   620 */    89,   90,   88,   89,   90,   88,   89,   90,  113,   37,
 /*   630 */    81,   70,  114,    2,    3,    4,    5,    6,    7,  115,
 /*   640 */    88,   89,   90,   88,   89,   90,   88,   89,   90,   70,
 /*   650 */    88,   89,   90,  114,   88,   89,   90,   88,   89,   90,
 /*   660 */    29,   30,   88,   89,   90,   34,   35,  114,   37,  105,
 /*   670 */    39,   40,   41,   88,   89,   90,   45,  114,   78,    2,
 /*   680 */     3,    4,    5,    6,    7,   88,   89,   90,  117,   88,
 /*   690 */    89,   90,   88,   89,   90,   88,   89,   90,   88,   89,
 /*   700 */    90,   88,   89,   90,   89,   37,   29,   30,   88,   89,
 /*   710 */    90,   34,   35,  114,   37,   91,   39,  105,   41,  102,
 /*   720 */   112,  110,   45,   88,   89,   90,  100,  119,   51,  119,
 /*   730 */   112,    2,    3,    4,    5,    6,    7,   88,   89,   90,
 /*   740 */    88,   89,   90,   88,   89,   90,   88,   89,   90,   88,
 /*   750 */    89,   90,   88,   89,   90,   88,   89,   90,   29,   30,
 /*   760 */    88,   89,   90,   34,   35,  119,   37,  119,   39,  119,
 /*   770 */    41,   88,   89,   90,   45,   88,   89,   90,   88,   89,
 /*   780 */    90,   88,   89,   90,   88,   89,   90,
};
#define YY_SHIFT_USE_DFLT (-13)
#define YY_SHIFT_COUNT (166)
#define YY_SHIFT_MIN   (-12)
#define YY_SHIFT_MAX   (729)
static const short yy_shift_ofst[] = {
 /*     0 */   364,  549,  677,  677,  498,  677,  677,  677,  729,  729,
 /*    10 */   631,  729,  729,  729,  729,  729,  729,  729,  729,  729,
 /*    20 */   729,  729,  729,  729,  729,  729,  729,  729,  729,  729,
 /*    30 */   729,  729,  729,  729,  729,  729,  729,  729,  729,  729,
 /*    40 */   729,  729,  729,  729,  729,  729,  729,  729,  214,  214,
 /*    50 */   668,   -8,   -8,  477,  559,  360,  113,  184,  592,  668,
 /*    60 */   600,  592,  538,  592,  592,  579,  592,  561,  568,  548,
 /*    70 */   538,  533,  527,  253,  199,  170,  123,   86,   53,  300,
 /*    80 */   300,  300,  300,  300,  300,  300,  300,  300,  300,  300,
 /*    90 */   300,  300,  300,  300,  300,  327,   18,  438,  359,  461,
 /*   100 */   217,  217,  217,  134,  134,   94,   24,  368,  323,  557,
 /*   110 */   523,   43,  213,  436,  412,  404,  247,   84,  132,  -12,
 /*   120 */   540,  491,  493,  485,  484,  496,  474,  475,  434,  443,
 /*   130 */   429,  501,  497,  462,  449,  453,  239,  349,  435,  406,
 /*   140 */   391,  390,  388,  387,  377,  374,  349,  373,  338,  321,
 /*   150 */   278,  269,  239,  239,  216,  200,  122,  168,  168,  168,
 /*   160 */   168,  168,  115,   91,  160,   87,  119,
};
#define YY_REDUCE_USE_DFLT (-51)
#define YY_REDUCE_COUNT (72)
#define YY_REDUCE_MIN   (-50)
#define YY_REDUCE_MAX   (696)
static const short yy_reduce_ofst[] = {
 /*     0 */   324,  469,  419,  204,  433,  422,  337,  207,  480,  452,
 /*    10 */    83,  503,  492,  696,  693,  690,  687,  683,  672,  667,
 /*    20 */   664,  661,  658,  655,  652,  649,  635,  620,  613,  610,
 /*    30 */   607,  604,  601,  597,  585,  574,  569,  566,  562,  558,
 /*    40 */   555,  552,  537,  534,  531,  528,  525,  519,  147,  -50,
 /*    50 */   218,  618,  608,  626,  611,  624,  612,  617,  599,  615,
 /*    60 */   571,  563,  564,  553,  539,  524,  518,  515,  502,  495,
 /*    70 */   494,  481,  473,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   327,  333,  333,  333,  333,  333,  333,  333,  262,  262,
 /*    10 */   333,  278,  333,  333,  333,  333,  333,  333,  333,  333,
 /*    20 */   333,  333,  333,  333,  333,  333,  333,  333,  333,  333,
 /*    30 */   333,  333,  333,  333,  333,  333,  333,  333,  333,  333,
 /*    40 */   333,  333,  333,  333,  333,  333,  333,  333,  333,  333,
 /*    50 */   333,  303,  303,  297,  289,  333,  307,  275,  333,  333,
 /*    60 */   323,  333,  307,  333,  333,  318,  333,  313,  333,  294,
 /*    70 */   307,  281,  304,  333,  279,  305,  273,  333,  333,  331,
 /*    80 */   325,  321,  324,  322,  235,  236,  237,  308,  296,  264,
 /*    90 */   263,  273,  306,  260,  238,  241,  240,  246,  245,  244,
 /*   100 */   252,  251,  243,  250,  242,  247,  248,  333,  333,  333,
 /*   110 */   286,  333,  330,  333,  333,  333,  333,  333,  333,  229,
 /*   120 */   333,  333,  333,  333,  333,  333,  333,  333,  333,  333,
 /*   130 */   333,  333,  333,  333,  333,  333,  269,  283,  333,  333,
 /*   140 */   333,  333,  333,  333,  333,  333,  284,  282,  295,  261,
 /*   150 */   333,  333,  270,  268,  298,  333,  333,  256,  255,  254,
 /*   160 */   253,  249,  333,  333,  226,  333,  333,  332,  329,  328,
 /*   170 */   326,  320,  226,  319,  317,  316,  314,  315,  312,  311,
 /*   180 */   310,  309,  265,  239,  232,  234,  277,  276,  287,  285,
 /*   190 */   291,  293,  292,  290,  288,  274,  280,  272,  271,  267,
 /*   200 */   299,  302,  301,  300,  266,  257,  258,  259,  233,  231,
 /*   210 */   230,  228,  227,  225,  224,  223,  222,  221,  220,
};

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  xjd1ParserARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void xjd1ParserTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "SEMI",          "INTEGER",       "FLOAT",       
  "STRING",        "TRUE",          "FALSE",         "NULL",        
  "QM",            "OR",            "AND",           "BITOR",       
  "BITXOR",        "BITAND",        "ILIKEOP",       "LIKEOP",      
  "NE",            "EQEQ",          "EQ3",           "NE3",         
  "WITHIN",        "IN",            "GT",            "LE",          
  "LT",            "GE",            "LSHIFT",        "RSHIFT",      
  "URSHIFT",       "PLUS",          "MINUS",         "STAR",        
  "SLASH",         "REM",           "BITNOT",        "BANG",        
  "COLLATE",       "ID",            "DOT",           "LB",          
  "RB",            "LC",            "RC",            "COLON",       
  "COMMA",         "LP",            "RP",            "UNION",       
  "EXCEPT",        "INTERSECT",     "ALL",           "SELECT",      
  "DISTINCT",      "AS",            "FROM",          "FLATTENOP",   
  "GROUP",         "BY",            "HAVING",        "ORDER",       
  "ASCENDING",     "DESCENDING",    "LIMIT",         "OFFSET",      
  "WHERE",         "BEGIN",         "ROLLBACK",      "COMMIT",      
  "CREATE",        "COLLECTION",    "IF",            "NOT",         
  "EXISTS",        "DROP",          "DELETE",        "UPDATE",      
  "SET",           "EQ",            "ELSE",          "INSERT",      
  "INTO",          "VALUE",         "ASYNC",         "SYNC",        
  "PRAGMA",        "error",         "cmd",           "input",       
  "jvalue",        "lvalue",        "expr",          "structlist",  
  "arraylist",     "exprlist",      "select",        "nexprlist",   
  "selectcore",    "compound",      "esel",          "x",           
  "orderby_opt",   "limit_opt",     "dist_opt",      "sel_result",  
  "from",          "where_opt",     "groupby_opt",   "fromlist",    
  "fromitem",      "path",          "eachalias",     "sortlist",    
  "sortorder",     "ifnotexists",   "tabname",       "ifexists",    
  "setlist",       "upsert_opt",    "async",       
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "input ::= cmd SEMI",
 /*   1 */ "jvalue ::= INTEGER",
 /*   2 */ "jvalue ::= FLOAT",
 /*   3 */ "jvalue ::= STRING",
 /*   4 */ "jvalue ::= TRUE",
 /*   5 */ "jvalue ::= FALSE",
 /*   6 */ "jvalue ::= NULL",
 /*   7 */ "lvalue ::= ID",
 /*   8 */ "lvalue ::= lvalue DOT ID",
 /*   9 */ "lvalue ::= lvalue LB expr RB",
 /*  10 */ "expr ::= lvalue",
 /*  11 */ "expr ::= jvalue",
 /*  12 */ "expr ::= LC structlist RC",
 /*  13 */ "expr ::= LC RC",
 /*  14 */ "expr ::= LB arraylist RB",
 /*  15 */ "expr ::= LB RB",
 /*  16 */ "structlist ::= ID|STRING COLON expr",
 /*  17 */ "structlist ::= structlist COMMA ID|STRING COLON expr",
 /*  18 */ "arraylist ::= expr",
 /*  19 */ "arraylist ::= arraylist COMMA expr",
 /*  20 */ "expr ::= ID LP exprlist RP",
 /*  21 */ "expr ::= expr AND expr",
 /*  22 */ "expr ::= expr OR expr",
 /*  23 */ "expr ::= expr LT|GT|GE|LE expr",
 /*  24 */ "expr ::= expr EQEQ|NE|EQ3|NE3 expr",
 /*  25 */ "expr ::= expr BITAND expr",
 /*  26 */ "expr ::= expr BITXOR expr",
 /*  27 */ "expr ::= expr BITOR expr",
 /*  28 */ "expr ::= expr LSHIFT|RSHIFT|URSHIFT expr",
 /*  29 */ "expr ::= expr PLUS|MINUS expr",
 /*  30 */ "expr ::= expr STAR|SLASH|REM expr",
 /*  31 */ "expr ::= expr IN|WITHIN expr",
 /*  32 */ "expr ::= expr ILIKEOP expr",
 /*  33 */ "expr ::= expr LIKEOP expr",
 /*  34 */ "expr ::= BANG expr",
 /*  35 */ "expr ::= BITNOT expr",
 /*  36 */ "expr ::= MINUS expr",
 /*  37 */ "expr ::= PLUS expr",
 /*  38 */ "expr ::= LP select RP",
 /*  39 */ "expr ::= LP expr RP",
 /*  40 */ "expr ::= expr COLLATE ID|STRING",
 /*  41 */ "expr ::= expr QM expr COLON expr",
 /*  42 */ "exprlist ::= nexprlist",
 /*  43 */ "exprlist ::=",
 /*  44 */ "nexprlist ::= expr",
 /*  45 */ "nexprlist ::= nexprlist COMMA expr",
 /*  46 */ "cmd ::= select",
 /*  47 */ "select ::= compound orderby_opt limit_opt",
 /*  48 */ "compound ::= selectcore",
 /*  49 */ "compound ::= esel UNION esel",
 /*  50 */ "compound ::= esel UNION ALL esel",
 /*  51 */ "compound ::= esel EXCEPT esel",
 /*  52 */ "compound ::= esel INTERSECT esel",
 /*  53 */ "esel ::= compound",
 /*  54 */ "esel ::= expr",
 /*  55 */ "selectcore ::= SELECT dist_opt sel_result from where_opt groupby_opt",
 /*  56 */ "dist_opt ::=",
 /*  57 */ "dist_opt ::= DISTINCT",
 /*  58 */ "dist_opt ::= ALL",
 /*  59 */ "sel_result ::=",
 /*  60 */ "sel_result ::= expr",
 /*  61 */ "sel_result ::= expr AS ID",
 /*  62 */ "from ::=",
 /*  63 */ "from ::= FROM fromlist",
 /*  64 */ "fromlist ::= fromitem",
 /*  65 */ "fromlist ::= fromlist COMMA fromitem",
 /*  66 */ "fromitem ::= LP select RP AS ID",
 /*  67 */ "fromitem ::= ID",
 /*  68 */ "fromitem ::= path AS ID",
 /*  69 */ "fromitem ::= fromitem FLATTENOP LP path eachalias RP",
 /*  70 */ "eachalias ::=",
 /*  71 */ "eachalias ::= AS ID|STRING",
 /*  72 */ "path ::= ID",
 /*  73 */ "path ::= path DOT ID",
 /*  74 */ "path ::= path LB ID|STRING RB",
 /*  75 */ "groupby_opt ::=",
 /*  76 */ "groupby_opt ::= GROUP BY exprlist",
 /*  77 */ "groupby_opt ::= GROUP BY exprlist HAVING expr",
 /*  78 */ "orderby_opt ::=",
 /*  79 */ "orderby_opt ::= ORDER BY sortlist",
 /*  80 */ "sortlist ::= sortlist COMMA expr sortorder",
 /*  81 */ "sortlist ::= expr sortorder",
 /*  82 */ "sortorder ::= ASCENDING",
 /*  83 */ "sortorder ::= DESCENDING",
 /*  84 */ "sortorder ::=",
 /*  85 */ "limit_opt ::=",
 /*  86 */ "limit_opt ::= LIMIT expr",
 /*  87 */ "limit_opt ::= LIMIT expr OFFSET expr",
 /*  88 */ "where_opt ::=",
 /*  89 */ "where_opt ::= WHERE expr",
 /*  90 */ "cmd ::= BEGIN ID",
 /*  91 */ "cmd ::= ROLLBACK ID",
 /*  92 */ "cmd ::= COMMIT ID",
 /*  93 */ "cmd ::= CREATE COLLECTION ifnotexists tabname",
 /*  94 */ "ifnotexists ::=",
 /*  95 */ "ifnotexists ::= IF NOT EXISTS",
 /*  96 */ "tabname ::= ID",
 /*  97 */ "cmd ::= DROP COLLECTION ifexists tabname",
 /*  98 */ "ifexists ::= IF EXISTS",
 /*  99 */ "ifexists ::=",
 /* 100 */ "cmd ::= DELETE FROM tabname where_opt",
 /* 101 */ "cmd ::= UPDATE tabname SET setlist where_opt upsert_opt",
 /* 102 */ "setlist ::= setlist COMMA lvalue EQ expr",
 /* 103 */ "setlist ::= lvalue EQ expr",
 /* 104 */ "upsert_opt ::=",
 /* 105 */ "upsert_opt ::= ELSE INSERT expr",
 /* 106 */ "cmd ::= async INSERT INTO tabname VALUE expr",
 /* 107 */ "cmd ::= async INSERT INTO tabname select",
 /* 108 */ "async ::=",
 /* 109 */ "async ::= ASYNC",
 /* 110 */ "async ::= SYNC",
 /* 111 */ "cmd ::= PRAGMA ID",
 /* 112 */ "cmd ::= PRAGMA ID EQ expr",
 /* 113 */ "cmd ::= PRAGMA ID LP expr RP",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to xjd1Parser and xjd1ParserFree.
*/
void *xjd1ParserAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  xjd1ParserARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 86: /* cmd */
{
#line 64 "parse.y"
(void)p;
#line 1040 "parse.c"
}
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from xjd1ParserAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void xjd1ParserFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int xjd1ParserStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_COUNT
   || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   xjd1ParserARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   xjd1ParserARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 87, 2 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 89, 1 },
  { 89, 3 },
  { 89, 4 },
  { 90, 1 },
  { 90, 1 },
  { 90, 3 },
  { 90, 2 },
  { 90, 3 },
  { 90, 2 },
  { 91, 3 },
  { 91, 5 },
  { 92, 1 },
  { 92, 3 },
  { 90, 4 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 2 },
  { 90, 2 },
  { 90, 2 },
  { 90, 2 },
  { 90, 3 },
  { 90, 3 },
  { 90, 3 },
  { 90, 5 },
  { 93, 1 },
  { 93, 0 },
  { 95, 1 },
  { 95, 3 },
  { 86, 1 },
  { 94, 3 },
  { 97, 1 },
  { 97, 3 },
  { 97, 4 },
  { 97, 3 },
  { 97, 3 },
  { 98, 1 },
  { 98, 1 },
  { 96, 6 },
  { 102, 0 },
  { 102, 1 },
  { 102, 1 },
  { 103, 0 },
  { 103, 1 },
  { 103, 3 },
  { 104, 0 },
  { 104, 2 },
  { 107, 1 },
  { 107, 3 },
  { 108, 5 },
  { 108, 1 },
  { 108, 3 },
  { 108, 6 },
  { 110, 0 },
  { 110, 2 },
  { 109, 1 },
  { 109, 3 },
  { 109, 4 },
  { 106, 0 },
  { 106, 3 },
  { 106, 5 },
  { 100, 0 },
  { 100, 3 },
  { 111, 4 },
  { 111, 2 },
  { 112, 1 },
  { 112, 1 },
  { 112, 0 },
  { 101, 0 },
  { 101, 2 },
  { 101, 4 },
  { 105, 0 },
  { 105, 2 },
  { 86, 2 },
  { 86, 2 },
  { 86, 2 },
  { 86, 4 },
  { 113, 0 },
  { 113, 3 },
  { 114, 1 },
  { 86, 4 },
  { 115, 2 },
  { 115, 0 },
  { 86, 4 },
  { 86, 6 },
  { 116, 5 },
  { 116, 3 },
  { 117, 0 },
  { 117, 3 },
  { 86, 6 },
  { 86, 5 },
  { 118, 0 },
  { 118, 1 },
  { 118, 1 },
  { 86, 2 },
  { 86, 4 },
  { 86, 5 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  xjd1ParserARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* input ::= cmd SEMI */
#line 65 "parse.y"
{p->pCmd = yymsp[-1].minor.yy156;}
#line 1449 "parse.c"
        break;
      case 1: /* jvalue ::= INTEGER */
      case 2: /* jvalue ::= FLOAT */ yytestcase(yyruleno==2);
#line 113 "parse.y"
{yygotominor.yy181 = jsonReal(p,&yymsp[0].minor.yy0);}
#line 1455 "parse.c"
        break;
      case 3: /* jvalue ::= STRING */
#line 115 "parse.y"
{yygotominor.yy181 = jsonString(p,&yymsp[0].minor.yy0);}
#line 1460 "parse.c"
        break;
      case 4: /* jvalue ::= TRUE */
#line 116 "parse.y"
{yygotominor.yy181 = jsonType(p,XJD1_TRUE);}
#line 1465 "parse.c"
        break;
      case 5: /* jvalue ::= FALSE */
#line 117 "parse.y"
{yygotominor.yy181 = jsonType(p,XJD1_FALSE);}
#line 1470 "parse.c"
        break;
      case 6: /* jvalue ::= NULL */
#line 118 "parse.y"
{yygotominor.yy181 = jsonType(p,XJD1_NULL);}
#line 1475 "parse.c"
        break;
      case 7: /* lvalue ::= ID */
#line 278 "parse.y"
{yygotominor.yy216 = idExpr(p,&yymsp[0].minor.yy0);}
#line 1480 "parse.c"
        break;
      case 8: /* lvalue ::= lvalue DOT ID */
#line 279 "parse.y"
{yygotominor.yy216 = lvalueExpr(p,yymsp[-2].minor.yy216,&yymsp[0].minor.yy0);}
#line 1485 "parse.c"
        break;
      case 9: /* lvalue ::= lvalue LB expr RB */
#line 280 "parse.y"
{yygotominor.yy216 = biExpr(p,yymsp[-3].minor.yy216,TK_LB,yymsp[-1].minor.yy216);}
#line 1490 "parse.c"
        break;
      case 10: /* expr ::= lvalue */
      case 89: /* where_opt ::= WHERE expr */ yytestcase(yyruleno==89);
      case 105: /* upsert_opt ::= ELSE INSERT expr */ yytestcase(yyruleno==105);
#line 283 "parse.y"
{yygotominor.yy216 = yymsp[0].minor.yy216;}
#line 1497 "parse.c"
        break;
      case 11: /* expr ::= jvalue */
#line 284 "parse.y"
{yygotominor.yy216 = jsonExpr(p,yymsp[0].minor.yy181);}
#line 1502 "parse.c"
        break;
      case 12: /* expr ::= LC structlist RC */
#line 285 "parse.y"
{yygotominor.yy216 = stExpr(p,yymsp[-1].minor.yy28);}
#line 1507 "parse.c"
        break;
      case 13: /* expr ::= LC RC */
#line 286 "parse.y"
{yygotominor.yy216 = stExpr(p,0);}
#line 1512 "parse.c"
        break;
      case 14: /* expr ::= LB arraylist RB */
#line 287 "parse.y"
{yygotominor.yy216 = arExpr(p,yymsp[-1].minor.yy28);}
#line 1517 "parse.c"
        break;
      case 15: /* expr ::= LB RB */
#line 288 "parse.y"
{yygotominor.yy216 = arExpr(p,0);}
#line 1522 "parse.c"
        break;
      case 16: /* structlist ::= ID|STRING COLON expr */
#line 291 "parse.y"
{yygotominor.yy28 = apndExpr(p,0,yymsp[0].minor.yy216,&yymsp[-2].minor.yy0);}
#line 1527 "parse.c"
        break;
      case 17: /* structlist ::= structlist COMMA ID|STRING COLON expr */
#line 293 "parse.y"
{yygotominor.yy28 = apndExpr(p,yymsp[-4].minor.yy28,yymsp[0].minor.yy216,&yymsp[-2].minor.yy0);}
#line 1532 "parse.c"
        break;
      case 18: /* arraylist ::= expr */
      case 44: /* nexprlist ::= expr */ yytestcase(yyruleno==44);
#line 295 "parse.y"
{yygotominor.yy28 = apndExpr(p,0,yymsp[0].minor.yy216,0);}
#line 1538 "parse.c"
        break;
      case 19: /* arraylist ::= arraylist COMMA expr */
      case 45: /* nexprlist ::= nexprlist COMMA expr */ yytestcase(yyruleno==45);
#line 296 "parse.y"
{yygotominor.yy28 = apndExpr(p,yymsp[-2].minor.yy28,yymsp[0].minor.yy216,0);}
#line 1544 "parse.c"
        break;
      case 20: /* expr ::= ID LP exprlist RP */
#line 300 "parse.y"
{yygotominor.yy216 = funcExpr(p,&yymsp[-3].minor.yy0,yymsp[-1].minor.yy28);}
#line 1549 "parse.c"
        break;
      case 21: /* expr ::= expr AND expr */
      case 22: /* expr ::= expr OR expr */ yytestcase(yyruleno==22);
      case 23: /* expr ::= expr LT|GT|GE|LE expr */ yytestcase(yyruleno==23);
      case 24: /* expr ::= expr EQEQ|NE|EQ3|NE3 expr */ yytestcase(yyruleno==24);
      case 25: /* expr ::= expr BITAND expr */ yytestcase(yyruleno==25);
      case 26: /* expr ::= expr BITXOR expr */ yytestcase(yyruleno==26);
      case 27: /* expr ::= expr BITOR expr */ yytestcase(yyruleno==27);
      case 28: /* expr ::= expr LSHIFT|RSHIFT|URSHIFT expr */ yytestcase(yyruleno==28);
      case 29: /* expr ::= expr PLUS|MINUS expr */ yytestcase(yyruleno==29);
      case 30: /* expr ::= expr STAR|SLASH|REM expr */ yytestcase(yyruleno==30);
      case 31: /* expr ::= expr IN|WITHIN expr */ yytestcase(yyruleno==31);
      case 32: /* expr ::= expr ILIKEOP expr */ yytestcase(yyruleno==32);
      case 33: /* expr ::= expr LIKEOP expr */ yytestcase(yyruleno==33);
#line 301 "parse.y"
{yygotominor.yy216 = biExpr(p,yymsp[-2].minor.yy216,yymsp[-1].major,yymsp[0].minor.yy216);}
#line 1566 "parse.c"
        break;
      case 34: /* expr ::= BANG expr */
      case 35: /* expr ::= BITNOT expr */ yytestcase(yyruleno==35);
      case 36: /* expr ::= MINUS expr */ yytestcase(yyruleno==36);
      case 37: /* expr ::= PLUS expr */ yytestcase(yyruleno==37);
#line 315 "parse.y"
{yygotominor.yy216 = biExpr(p,yymsp[0].minor.yy216,yymsp[-1].major,0);}
#line 1574 "parse.c"
        break;
      case 38: /* expr ::= LP select RP */
#line 319 "parse.y"
{yygotominor.yy216 = subqExpr(p,yymsp[-1].minor.yy89);}
#line 1579 "parse.c"
        break;
      case 39: /* expr ::= LP expr RP */
#line 320 "parse.y"
{yygotominor.yy216 = yymsp[-1].minor.yy216;}
#line 1584 "parse.c"
        break;
      case 40: /* expr ::= expr COLLATE ID|STRING */
#line 321 "parse.y"
{yygotominor.yy216 = yymsp[-2].minor.yy216;}
#line 1589 "parse.c"
        break;
      case 41: /* expr ::= expr QM expr COLON expr */
#line 322 "parse.y"
{yygotominor.yy216 = triExpr(p,yymsp[-4].minor.yy216,yymsp[-2].minor.yy216,yymsp[0].minor.yy216);}
#line 1594 "parse.c"
        break;
      case 42: /* exprlist ::= nexprlist */
      case 79: /* orderby_opt ::= ORDER BY sortlist */ yytestcase(yyruleno==79);
#line 326 "parse.y"
{yygotominor.yy28 = yymsp[0].minor.yy28;}
#line 1600 "parse.c"
        break;
      case 43: /* exprlist ::= */
      case 78: /* orderby_opt ::= */ yytestcase(yyruleno==78);
#line 327 "parse.y"
{yygotominor.yy28 = 0;}
#line 1606 "parse.c"
        break;
      case 46: /* cmd ::= select */
#line 333 "parse.y"
{
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_SELECT;
    pNew->u.q.pQuery = yymsp[0].minor.yy89;
  }
  yygotominor.yy156 = pNew;
}
#line 1618 "parse.c"
        break;
      case 47: /* select ::= compound orderby_opt limit_opt */
#line 407 "parse.y"
{
  yymsp[-2].minor.yy89->pOrderBy = yymsp[-1].minor.yy28;
  yymsp[-2].minor.yy89->pLimit = yymsp[0].minor.yy17.pLimit;
  yymsp[-2].minor.yy89->pOffset = yymsp[0].minor.yy17.pOffset;
  yygotominor.yy89 = yymsp[-2].minor.yy89;
}
#line 1628 "parse.c"
        break;
      case 48: /* compound ::= selectcore */
      case 53: /* esel ::= compound */ yytestcase(yyruleno==53);
#line 413 "parse.y"
{yygotominor.yy89 = yymsp[0].minor.yy89;}
#line 1634 "parse.c"
        break;
      case 49: /* compound ::= esel UNION esel */
      case 51: /* compound ::= esel EXCEPT esel */ yytestcase(yyruleno==51);
      case 52: /* compound ::= esel INTERSECT esel */ yytestcase(yyruleno==52);
#line 414 "parse.y"
{yygotominor.yy89=compoundQuery(p,yymsp[-2].minor.yy89,yymsp[-1].major,yymsp[0].minor.yy89);}
#line 1641 "parse.c"
        break;
      case 50: /* compound ::= esel UNION ALL esel */
#line 415 "parse.y"
{yygotominor.yy89=compoundQuery(p,yymsp[-3].minor.yy89,yymsp[-1].major,yymsp[0].minor.yy89);}
#line 1646 "parse.c"
        break;
      case 54: /* esel ::= expr */
#line 419 "parse.y"
{
  /* TODO: Fix these error messages */
  if( p->errCode==XJD1_OK ){
    if( yymsp[0].minor.yy216->eClass!=XJD1_EXPR_Q ){
      xjd1ParseError(p, XJD1_SYNTAX,
        "syntax error near \"%.*s\"", p->sTok.n, p->sTok.z
      );
    }else{
      yygotominor.yy89 = yymsp[0].minor.yy216->u.subq.p;
      if( yygotominor.yy89->pOrderBy || yygotominor.yy89->pLimit ){
        xjd1ParseError(p, XJD1_SYNTAX,
          "syntax error near \"%.*s\"", p->sTok.n, p->sTok.z
        );
      }
    }
  }
}
#line 1667 "parse.c"
        break;
      case 55: /* selectcore ::= SELECT dist_opt sel_result from where_opt groupby_opt */
#line 439 "parse.y"
{yygotominor.yy89 = simpleQuery(p,yymsp[-4].minor.yy60,yymsp[-3].minor.yy90.pExpr,yymsp[-3].minor.yy90.zAs,yymsp[-2].minor.yy225,yymsp[-1].minor.yy216,&yymsp[0].minor.yy186);}
#line 1672 "parse.c"
        break;
      case 56: /* dist_opt ::= */
      case 58: /* dist_opt ::= ALL */ yytestcase(yyruleno==58);
      case 94: /* ifnotexists ::= */ yytestcase(yyruleno==94);
      case 99: /* ifexists ::= */ yytestcase(yyruleno==99);
#line 443 "parse.y"
{yygotominor.yy60 = 0;}
#line 1680 "parse.c"
        break;
      case 57: /* dist_opt ::= DISTINCT */
      case 95: /* ifnotexists ::= IF NOT EXISTS */ yytestcase(yyruleno==95);
      case 98: /* ifexists ::= IF EXISTS */ yytestcase(yyruleno==98);
#line 444 "parse.y"
{yygotominor.yy60 = 1;}
#line 1687 "parse.c"
        break;
      case 59: /* sel_result ::= */
#line 452 "parse.y"
{yygotominor.yy90.pExpr=0;yygotominor.yy90.zAs=0;}
#line 1692 "parse.c"
        break;
      case 60: /* sel_result ::= expr */
#line 453 "parse.y"
{yygotominor.yy90.pExpr=yymsp[0].minor.yy216;yygotominor.yy90.zAs=0;}
#line 1697 "parse.c"
        break;
      case 61: /* sel_result ::= expr AS ID */
#line 454 "parse.y"
{yygotominor.yy90.pExpr=yymsp[-2].minor.yy216;yygotominor.yy90.zAs=tokenStr(p, &yymsp[0].minor.yy0);}
#line 1702 "parse.c"
        break;
      case 62: /* from ::= */
#line 536 "parse.y"
{yygotominor.yy225 = nullDataSrc(p);}
#line 1707 "parse.c"
        break;
      case 63: /* from ::= FROM fromlist */
      case 64: /* fromlist ::= fromitem */ yytestcase(yyruleno==64);
#line 537 "parse.y"
{yygotominor.yy225 = yymsp[0].minor.yy225;}
#line 1713 "parse.c"
        break;
      case 65: /* fromlist ::= fromlist COMMA fromitem */
#line 539 "parse.y"
{yygotominor.yy225 = joinDataSrc(p,yymsp[-2].minor.yy225,yymsp[0].minor.yy225);}
#line 1718 "parse.c"
        break;
      case 66: /* fromitem ::= LP select RP AS ID */
#line 540 "parse.y"
{yygotominor.yy225 = subqDataSrc(p,yymsp[-3].minor.yy89,&yymsp[0].minor.yy0);}
#line 1723 "parse.c"
        break;
      case 67: /* fromitem ::= ID */
#line 542 "parse.y"
{yygotominor.yy225 = pathDataSrc(p,0,&yymsp[0].minor.yy0);}
#line 1728 "parse.c"
        break;
      case 68: /* fromitem ::= path AS ID */
#line 543 "parse.y"
{yygotominor.yy225 = pathDataSrc(p,yymsp[-2].minor.yy216,&yymsp[0].minor.yy0);}
#line 1733 "parse.c"
        break;
      case 69: /* fromitem ::= fromitem FLATTENOP LP path eachalias RP */
#line 545 "parse.y"
{
  yygotominor.yy225 = flattenDataSrc(p,yymsp[-5].minor.yy225,&yymsp[-4].minor.yy0,yymsp[-2].minor.yy216,yymsp[-1].minor.yy216);
}
#line 1740 "parse.c"
        break;
      case 70: /* eachalias ::= */
#line 550 "parse.y"
{yygotominor.yy216=0;}
#line 1745 "parse.c"
        break;
      case 71: /* eachalias ::= AS ID|STRING */
#line 551 "parse.y"
{yygotominor.yy216=idExpr(p,&yymsp[0].minor.yy0);}
#line 1750 "parse.c"
        break;
      case 72: /* path ::= ID */
#line 554 "parse.y"
{yygotominor.yy216 = idExpr(p, &yymsp[0].minor.yy0);        }
#line 1755 "parse.c"
        break;
      case 73: /* path ::= path DOT ID */
#line 555 "parse.y"
{yygotominor.yy216 = lvalueExpr(p, yymsp[-2].minor.yy216, &yymsp[0].minor.yy0); }
#line 1760 "parse.c"
        break;
      case 74: /* path ::= path LB ID|STRING RB */
#line 556 "parse.y"
{yygotominor.yy216 = lvalueExpr(p, yymsp[-3].minor.yy216, &yymsp[-1].minor.yy0); }
#line 1765 "parse.c"
        break;
      case 75: /* groupby_opt ::= */
#line 559 "parse.y"
{yygotominor.yy186.pGroupBy=0; yygotominor.yy186.pHaving=0;}
#line 1770 "parse.c"
        break;
      case 76: /* groupby_opt ::= GROUP BY exprlist */
#line 560 "parse.y"
{yygotominor.yy186.pGroupBy=yymsp[0].minor.yy28; yygotominor.yy186.pHaving=0;}
#line 1775 "parse.c"
        break;
      case 77: /* groupby_opt ::= GROUP BY exprlist HAVING expr */
#line 562 "parse.y"
{yygotominor.yy186.pGroupBy=yymsp[-2].minor.yy28; yygotominor.yy186.pHaving=yymsp[0].minor.yy216;}
#line 1780 "parse.c"
        break;
      case 80: /* sortlist ::= sortlist COMMA expr sortorder */
#line 569 "parse.y"
{yygotominor.yy28 = apndExpr(p,yymsp[-3].minor.yy28,yymsp[-1].minor.yy216,yymsp[0].minor.yy0.n?&yymsp[0].minor.yy0:0);}
#line 1785 "parse.c"
        break;
      case 81: /* sortlist ::= expr sortorder */
#line 570 "parse.y"
{yygotominor.yy28 = apndExpr(p,0,yymsp[-1].minor.yy216,yymsp[0].minor.yy0.n?&yymsp[0].minor.yy0:0);}
#line 1790 "parse.c"
        break;
      case 82: /* sortorder ::= ASCENDING */
      case 83: /* sortorder ::= DESCENDING */ yytestcase(yyruleno==83);
      case 96: /* tabname ::= ID */ yytestcase(yyruleno==96);
#line 571 "parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 1797 "parse.c"
        break;
      case 84: /* sortorder ::= */
#line 573 "parse.y"
{yygotominor.yy0.z=""; yygotominor.yy0.n=0;}
#line 1802 "parse.c"
        break;
      case 85: /* limit_opt ::= */
#line 576 "parse.y"
{yygotominor.yy17.pLimit=0; yygotominor.yy17.pOffset=0;}
#line 1807 "parse.c"
        break;
      case 86: /* limit_opt ::= LIMIT expr */
#line 577 "parse.y"
{yygotominor.yy17.pLimit=yymsp[0].minor.yy216; yygotominor.yy17.pOffset=0;}
#line 1812 "parse.c"
        break;
      case 87: /* limit_opt ::= LIMIT expr OFFSET expr */
#line 578 "parse.y"
{yygotominor.yy17.pLimit=yymsp[-2].minor.yy216; yygotominor.yy17.pOffset=yymsp[0].minor.yy216;}
#line 1817 "parse.c"
        break;
      case 88: /* where_opt ::= */
      case 104: /* upsert_opt ::= */ yytestcase(yyruleno==104);
#line 581 "parse.y"
{yygotominor.yy216 = 0;}
#line 1823 "parse.c"
        break;
      case 93: /* cmd ::= CREATE COLLECTION ifnotexists tabname */
#line 592 "parse.y"
{
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_CREATECOLLECTION;
    pNew->u.crtab.ifExists = yymsp[-1].minor.yy60;
    pNew->u.crtab.zName = tokenStr(p, &yymsp[0].minor.yy0);
  }
  yygotominor.yy156 = pNew;
}
#line 1836 "parse.c"
        break;
      case 97: /* cmd ::= DROP COLLECTION ifexists tabname */
#line 608 "parse.y"
{
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_DROPCOLLECTION;
    pNew->u.crtab.ifExists = yymsp[-1].minor.yy60;
    pNew->u.crtab.zName = tokenStr(p, &yymsp[0].minor.yy0);
  }
  yygotominor.yy156 = pNew;
}
#line 1849 "parse.c"
        break;
      case 100: /* cmd ::= DELETE FROM tabname where_opt */
#line 624 "parse.y"
{
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_DELETE;
    pNew->u.del.zName = tokenStr(p, &yymsp[-1].minor.yy0);
    pNew->u.del.pWhere = yymsp[0].minor.yy216;
  }
  yygotominor.yy156 = pNew;
}
#line 1862 "parse.c"
        break;
      case 101: /* cmd ::= UPDATE tabname SET setlist where_opt upsert_opt */
#line 636 "parse.y"
{
  Command *pNew = xjd1PoolMalloc(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_UPDATE;
    pNew->u.update.zName = tokenStr(p, &yymsp[-4].minor.yy0);
    pNew->u.update.pWhere = yymsp[-1].minor.yy216;
    pNew->u.update.pChng = yymsp[-2].minor.yy28;
    pNew->u.update.pUpsert = yymsp[0].minor.yy216;
  }
  yygotominor.yy156 = pNew;
}
#line 1877 "parse.c"
        break;
      case 102: /* setlist ::= setlist COMMA lvalue EQ expr */
#line 649 "parse.y"
{
   yygotominor.yy28 = apndExpr(p,yymsp[-4].minor.yy28,yymsp[-2].minor.yy216,0);
   yygotominor.yy28 = apndExpr(p,yygotominor.yy28,yymsp[0].minor.yy216,0);
}
#line 1885 "parse.c"
        break;
      case 103: /* setlist ::= lvalue EQ expr */
#line 653 "parse.y"
{
   yygotominor.yy28 = apndExpr(p,0,yymsp[-2].minor.yy216,0);
   yygotominor.yy28 = apndExpr(p,yygotominor.yy28,yymsp[0].minor.yy216,0);
}
#line 1893 "parse.c"
        break;
      case 106: /* cmd ::= async INSERT INTO tabname VALUE expr */
#line 666 "parse.y"
{
  Command *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_INSERT;
    pNew->u.ins.zName = tokenStr(p, &yymsp[-2].minor.yy0);
    pNew->u.ins.pValue = yymsp[0].minor.yy216;
  }
  yygotominor.yy156 = pNew;
}
#line 1906 "parse.c"
        break;
      case 107: /* cmd ::= async INSERT INTO tabname select */
#line 675 "parse.y"
{
  Command *pNew = xjd1PoolMallocZero(p->pPool, sizeof(*pNew));
  if( pNew ){
    pNew->eCmdType = TK_INSERT;
    pNew->u.ins.zName = tokenStr(p, &yymsp[-1].minor.yy0);
    pNew->u.ins.pQuery = yymsp[0].minor.yy89;
  }
  yygotominor.yy156 = pNew;
}
#line 1919 "parse.c"
        break;
      case 111: /* cmd ::= PRAGMA ID */
#line 701 "parse.y"
{yygotominor.yy156 = makePrag(p,&yymsp[0].minor.yy0,0);}
#line 1924 "parse.c"
        break;
      case 112: /* cmd ::= PRAGMA ID EQ expr */
#line 702 "parse.y"
{yygotominor.yy156 = makePrag(p,&yymsp[-2].minor.yy0,yymsp[0].minor.yy216);}
#line 1929 "parse.c"
        break;
      case 113: /* cmd ::= PRAGMA ID LP expr RP */
#line 703 "parse.y"
{yygotominor.yy156 = makePrag(p,&yymsp[-3].minor.yy0,yymsp[-1].minor.yy216);}
#line 1934 "parse.c"
        break;
      default:
      /* (90) cmd ::= BEGIN ID */ yytestcase(yyruleno==90);
      /* (91) cmd ::= ROLLBACK ID */ yytestcase(yyruleno==91);
      /* (92) cmd ::= COMMIT ID */ yytestcase(yyruleno==92);
      /* (108) async ::= */ yytestcase(yyruleno==108);
      /* (109) async ::= ASYNC */ yytestcase(yyruleno==109);
      /* (110) async ::= SYNC */ yytestcase(yyruleno==110);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  xjd1ParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  xjd1ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  xjd1ParserARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 37 "parse.y"

  xjd1ParseError(p, XJD1_SYNTAX,
      "syntax error near \"%.*s\"", p->sTok.n, p->sTok.z
  );
#line 2007 "parse.c"
  xjd1ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  xjd1ParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  xjd1ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "xjd1ParserAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void xjd1Parser(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  xjd1ParserTOKENTYPE yyminor       /* The value for the token */
  xjd1ParserARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  xjd1ParserARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
