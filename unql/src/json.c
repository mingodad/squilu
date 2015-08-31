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
** This file contains code used to parse and render JSON strings.
*/
#include "xjd1Int.h"
#include <ctype.h>


/*
** Change a JsonNode to be a NULL.  Any substructure is deleted.
*/
void xjd1JsonToNull(JsonNode *p){
  if( p==0 ) return;
  switch( p->eJType ){
    case XJD1_STRING: {
      xjd1_free(p->u.z);
      break;
    }
    case XJD1_ARRAY: {
      int i;
      for(i=0; i<p->u.ar.nElem; i++){
        xjd1JsonFree(p->u.ar.apElem[i]);
      }
      xjd1_free(p->u.ar.apElem);
      break;
    }
    case XJD1_STRUCT: {
      JsonStructElem *pElem, *pNext;
      for(pElem=p->u.st.pFirst; pElem; pElem=pNext){
        pNext = pElem->pNext;
        xjd1_free(pElem->zLabel);
        xjd1JsonFree(pElem->pValue);
        xjd1_free(pElem);
      }
      break;
    }
  }
  p->eJType = XJD1_NULL;
}

/*
** Reclaim memory used by JsonNode objects
*/
void xjd1JsonFree(JsonNode *p){
  if( p && (--p->nRef)<=0 ){
    xjd1JsonToNull(p);
    xjd1_free(p);
  }
}

/*
** Allocate a new Json node.
*/
JsonNode *xjd1JsonNew(Pool *pPool){
  JsonNode *p;
  if( pPool ){
    p = xjd1PoolMalloc(pPool, sizeof(*p));
    if( p ) p->nRef = 10000;
  }else{
    p = xjd1_malloc( sizeof(*p) );
    if( p ){
      memset(p, 0, sizeof(*p));
      p->nRef = 1;
    }
  }
  return p;
}

/*
** Increase the reference count on a JSON object.
**
** The object is freed when its reference count reaches zero.
*/
JsonNode *xjd1JsonRef(JsonNode *p){
  if( p ){
    p->nRef++;
  }
  return p;
}


/*
** Return a deep copy of a JSON object.
*/
JsonNode *xjd1JsonDeepCopy(JsonNode *p){
  JsonNode *pNew;
  if( p==0 ) return 0;
  pNew = xjd1JsonNew(0);
  if( pNew==0 ) return 0;
  pNew->eJType = p->eJType;
  pNew->u = p->u;
  switch( pNew->eJType ){
    case XJD1_STRING: {
      pNew->u.z = xjd1PoolDup(0, p->u.z, -1);
      break;
    }
    case XJD1_ARRAY: {
      JsonNode **ap;
      pNew->u.ar.apElem = ap = xjd1_malloc( sizeof(JsonNode*)*p->u.ar.nElem );
      if( ap==0 ){
        pNew->eJType = XJD1_NULL;
      }else{
        int i;
        pNew->u.ar.nElem = p->u.ar.nElem;
        for(i=0; i<p->u.ar.nElem; i++){
          ap[i] = xjd1JsonDeepCopy(p->u.ar.apElem[i]);
        }
      }
      break;
    }
    case XJD1_STRUCT: {
      JsonStructElem *pSrc, *pDest, **ppPrev;
      ppPrev = &pNew->u.st.pFirst;
      for(pSrc=p->u.st.pFirst; pSrc; pSrc=pSrc->pNext){
        pNew->u.st.pLast = pDest = xjd1_malloc( sizeof(*pDest) );
        if( pDest==0 ) break;
        memset(pDest, 0, sizeof(*pDest));
        *ppPrev = pDest;
        ppPrev = &pDest->pNext;
        pDest->zLabel = xjd1PoolDup(0, pSrc->zLabel, -1);
        pDest->pValue = xjd1JsonDeepCopy(pSrc->pValue);
      }
      break;
    }
  }
  return pNew;
}

/*
** Return an editable JSON object.  A JSON object is editable if its
** reference count is exactly 1.  If the input JSON object has a reference
** count greater than 1, then make a copy and return the copy.
*/
JsonNode *xjd1JsonEdit(JsonNode *p){
  if( p==0 ) return 0;
  if( p->nRef==1 ) return p;
  return xjd1JsonDeepCopy(p);
}


/* Render a string as a string literal.
*/
void renderString(String *pOut, const char *z){
  int n, i, j, c;
  char *zOut;
  for(i=n=0; (c=z[i])!=0; i++, n++){
    switch(c)
    {
    case '"':
    case '\\':
    case '\n':
    case '\r':
    case '\t':
    case '\f':
    case '\b':
        ++n;
        break;
    }
  }
  xjd1StringAppend(pOut, 0, n+3);
  zOut = xjd1StringText(pOut);
  if( zOut ){
    zOut += xjd1StringLen(pOut);
    zOut[0] = '"';
    for(i=0, j=1; (c=z[i])!=0; i++){
      switch(c)
      {
        case '"':
        case '\\':
          zOut[j++] = '\\';
          break;
        case '\n':
          zOut[j++] = '\\';
          zOut[j++] = 'n';
          continue;
          break;
        case '\r':
          zOut[j++] = '\\';
          zOut[j++] = 'r';
          continue;
          break;
        case '\t':
          zOut[j++] = '\\';
          zOut[j++] = 't';
          continue;
          break;
        case '\f':
          zOut[j++] = '\\';
          zOut[j++] = 'f';
          continue;
          break;
        case '\b':
          zOut[j++] = '\\';
          zOut[j++] = 'b';
          continue;
          break;
      }
      zOut[j++] = c;
    }
    zOut[j++] = '"';
    zOut[j] = 0;
    pOut->nUsed += j;
  }
}


/*
** Append the text for a JSON string.
*/
void xjd1JsonRender(String *pOut, const JsonNode *p){
  if( p==0 ){
    xjd1StringAppend(pOut, "null", 4);
  }else{
    switch( p->eJType ){
      case XJD1_FALSE: {
        xjd1StringAppend(pOut, "false", 5);
        break;
      }
      case XJD1_TRUE: {
        xjd1StringAppend(pOut, "true", 4);
        break;
      }
      case XJD1_NULL: {
        xjd1StringAppend(pOut, "null", 4);
        break;
      }
      case XJD1_REAL: {
        xjd1StringAppendF(pOut, "%.17g", p->u.r);
        break;
      }
      case XJD1_STRING: {
        renderString(pOut, p->u.z);
        break;
      }
      case XJD1_ARRAY: {
        char cSep = '[';
        int i;
        if( p->u.ar.nElem==0 ) xjd1StringAppend(pOut, &cSep, 1);
        for(i=0; i<p->u.ar.nElem; i++){
          xjd1StringAppend(pOut, &cSep, 1);
          cSep = ',';
          xjd1JsonRender(pOut, p->u.ar.apElem[i]);
        }
        xjd1StringAppend(pOut, "]", 1);
        break;
      }
      case XJD1_STRUCT: {
        char cSep = '{';
        JsonStructElem *pElem;
        if( p->u.st.pFirst==0 ) xjd1StringAppend(pOut, &cSep, 1);
        for(pElem=p->u.st.pFirst; pElem; pElem=pElem->pNext){
          xjd1StringAppend(pOut, &cSep, 1);
          cSep = ',';
          renderString(pOut, pElem->zLabel);
          xjd1StringAppend(pOut, ":", 1);
          xjd1JsonRender(pOut, pElem->pValue);
        }
        xjd1StringAppend(pOut, "}", 1);
        break;
      }
    }
  }
}

/*
** Attempt to convert a JSON object into a real number.  Return 0
** if successful and 1 if there is an error.
*/
int xjd1JsonToReal(const JsonNode *p, double *pRes){
  double x = 0.0;
  *pRes = 0.0/x;
  if( p==0 ) return 1;
  switch( p->eJType ){
    case XJD1_FALSE: {
      *pRes = 0.0;
      return 0;
    }
    case XJD1_TRUE: {
      *pRes = 1.0;
      return 0;
    }
    case XJD1_REAL: {
      *pRes = p->u.r;
      return 0;
    }
    case XJD1_STRING: {
      char *zEnd;
      if( isspace(p->u.z[0]) ){
        return 1;
      }else{
        *pRes = strtod(p->u.z, &zEnd);
        if( zEnd[0]!=0 ){
          return 1;
        }
        return 0;
      }
    }
  }
  return 1;
}

/*
** Attempt to convert a JSON object into a string.  Return 0
** if successful and 1 if there is an error.
*/
int xjd1JsonToString(const JsonNode *p, String *pOut){
  if( p==0 ){
    xjd1StringAppend(pOut, "null", 4);
    return 1;
  }
  switch( p->eJType ){
    case XJD1_FALSE: {
      xjd1StringAppend(pOut, "false", 5);
      break;
    }
    case XJD1_TRUE: {
      xjd1StringAppend(pOut, "true", 4);
      break;
    }
    case XJD1_REAL: {
      xjd1StringAppendF(pOut, "%.17g", p->u.r);
      break;
    }
    case XJD1_NULL: {
      xjd1StringAppend(pOut, "null", 4);
      break;
    }
    case XJD1_STRING: {
      xjd1StringAppend(pOut, p->u.z, -1);
      break;
    }
    case XJD1_ARRAY: {
      xjd1JsonRender(pOut, p);
      break;
    }
    case XJD1_STRUCT: {
      xjd1JsonRender(pOut, p);
      break;
    }
  }
  return 0;
}

/*
** Compare to JSON objects.  Return negative, zero, or positive if the
** first is less than, equal to, or greater than the second.
*/
int xjd1JsonCompare(const JsonNode *pLeft, const JsonNode *pRight, int insensitive){
  if( pLeft==0 ){
    return pRight ? -1 : 0;
  }
  if( pRight==0 ){
    return 1;
  }
  if( pLeft->eJType!=pRight->eJType ){
    return pLeft->eJType - pRight->eJType;
  }
  switch( pLeft->eJType ){
    case XJD1_REAL: {
      if( pLeft->u.r<pRight->u.r ){
        return -1;
      }
      if( pLeft->u.r>pRight->u.r ){
        return 1;
      }
      return 0;
    }
    case XJD1_STRING: {
      if(insensitive) return strcasecmp(pLeft->u.z, pRight->u.z);
      else return strcmp(pLeft->u.z, pRight->u.z);
    }
    case XJD1_ARRAY: {
      int i, mx, c;
      mx = pLeft->u.ar.nElem;
      if( mx>pRight->u.ar.nElem ) mx = pRight->u.ar.nElem;
      for(i=0; i<mx; i++){
        c = xjd1JsonCompare(pLeft->u.ar.apElem[i], pRight->u.ar.apElem[i], 0);
        if( c ) return c;
      }
      return pLeft->u.ar.nElem - pRight->u.ar.nElem;
    }
    case XJD1_STRUCT: {
      JsonStructElem *pA = pLeft->u.st.pFirst;
      JsonStructElem *pB = pRight->u.st.pFirst;
      int c = 0;
      while( pA && pB ){
        c = strcmp(pA->zLabel, pB->zLabel);
        if( c ) return c;
        c = xjd1JsonCompare(pA->pValue, pB->pValue, 0);
        if( c ) return c;
        pA = pA->pNext;
        pB = pB->pNext;
      }
      if( pA ){
        return 1;
      }
      if( pB ){
        return -1;
      }
      return 0;
    }
  }
  return 0;
}


/* JSON parser token types */
#define JSON_FALSE          XJD1_FALSE
#define JSON_TRUE           XJD1_TRUE
#define JSON_REAL           XJD1_REAL
#define JSON_NULL           XJD1_NULL
#define JSON_STRING         XJD1_STRING
#define JSON_BEGIN_ARRAY    XJD1_ARRAY
#define JSON_BEGIN_STRUCT   XJD1_STRUCT

#define JSON_END_ARRAY     20
#define JSON_COMMA         21
#define JSON_END_STRUCT    22
#define JSON_COLON         23
#define JSON_EOF           24
#define JSON_ERROR         25

/* State of a JSON string tokenizer */
typedef struct JsonStr JsonStr;
struct JsonStr {
  const char *zIn;        /* Complete input string */
  int mxIn;               /* Number of characters in input string */
  int iCur;               /* First character of current token */
  int n;                  /* Number of charaters in current token */
  int eType;              /* Type of current token */
};

/* Return the type of the current token */
#define tokenType(X)  ((X)->eType)

/* Return a pointer to the string of a token. */
#define tokenString(X) (&(X)->zIn[(X)->iCur])

/* Return TRUE if z[0..n-1] is the beginning of engineering notation:
**
**      [eE](+|-)[0-9]+
*/
static int isExp(const char *z, int n){
  if( n<2 ) return 0;
  if( z[0]!='e' && z[0]!='E' ) return 0;
  if( xjd1Isdigit(z[1]) ) return 1;
  if( n<3 || (z[1]!='-' && z[1]!='+') ) return 0;
  if( xjd1Isdigit(z[2]) ) return 1;
  return 0;
}

/* Advance to the next token */
static void tokenNext(JsonStr *p){
  int i, n;
  const char *z = p->zIn;
  int mx = p->mxIn;
  char c;

  i = p->n + p->iCur;
  while( i<mx && xjd1Isspace(z[i]) ){ i++; }
  if( i>=mx ) goto token_eof;
  p->iCur = i;
  switch( i<mx ? z[i] : 0 ){
    case 0: {
      goto token_eof;
    }
    case '"': {
     for(n=1; i+n<mx && (c = z[i+n])!=0 && c!='"'; n++){
        if( c=='\\' ) n++;
      }
      if( c=='"' ) n++;
      if( i+n>mx ){ n = mx - i; c = 0; }
      p->n = n;
      p->eType = (c=='"' ? JSON_STRING : JSON_ERROR);
      break;
    }
    case '{': {
      p->n = 1;
      p->eType = JSON_BEGIN_STRUCT;
      break;
    }
    case '}': {
      p->n = 1;
      p->eType = JSON_END_STRUCT;
      break;
    }
    case '[': {
      p->n = 1;
      p->eType = JSON_BEGIN_ARRAY;
      break;
    }
    case ']': {
      p->n = 1;
      p->eType = JSON_END_ARRAY;
      break;
    }
    case ',': {
      p->n = 1;
      p->eType = JSON_COMMA;
      break;
    }
    case ':': {
      p->n = 1;
      p->eType = JSON_COLON;
      break;
    }
    case 't': {
      if( i+4<=mx && memcmp(&z[i],"true",4)==0
       && (i+4==mx || xjd1Isident(z[i+4])==0)
      ){
        p->n = 4;
        p->eType = JSON_TRUE;
      }else{
        p->n = 1;
        p->eType = JSON_ERROR;
      }
      break;
    }
    case 'f': {
      if( i+5<=mx && memcmp(&z[i],"false",5)==0
       && (i+5==mx || xjd1Isident(z[i+5])==0)
      ){
        p->n = 5;
        p->eType = JSON_FALSE;
      }else{
        p->n = 1;
        p->eType = JSON_ERROR;
      }
      break;
    }
    case 'n': {
      if( i+4<=mx && memcmp(&z[i],"null",4)==0
       && (i+4==mx || xjd1Isident(z[i+4])==0)
      ){
        p->n = 4;
        p->eType = JSON_NULL;
      }else{
        p->n = 1;
        p->eType = JSON_ERROR;
      }
      break;
    }
    case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
      n = 0;
      if( z[i]=='-' ){
        n = 1;
        if( i+1>=mx || !xjd1Isdigit(z[i+1]) ){
          p->n = 1;
          p->eType = JSON_ERROR;
          break;
        }
      }
      if( z[i+n]=='0' ){
        n++;
      }else{
        while( i+n<mx && xjd1Isdigit(z[i+n]) ) n++;
      }
      if( i+n+1<mx && z[i+n]=='.' && xjd1Isdigit(z[i+n+1]) ){
        n++;
        while( i+n<mx && xjd1Isdigit(z[i+n]) ) n++;
      }
      if( isExp(&z[i+n], mx-(i+n)) ){
        n += 2;
        while( i+n<mx && xjd1Isdigit(z[i+n]) ) n++;
      }
      p->n = n;
      p->eType = JSON_REAL;
      break;
    }
    default: {
      p->n = 1;
      p->eType = JSON_ERROR;
      break;
    }
  }
  return;

token_eof:
  p->n = 0;
  p->eType = JSON_EOF;
  return;
}

/*
** Dequote a string in-place.
*/
void xjd1DequoteString(char *z, int n){
  int i, j;
  char c;
  assert( n>=2 );
  assert( z[0]=='"' && z[n-1]=='"' );
  for(i=1, j=0; i<n-1; i++){
    if( z[i]!='\\' ){
      z[j++] = z[i];
    }else{
      i++;
      c = z[i];
      if( c=='b' ){
        z[j++] = '\b';
      }else if( c=='f' ){
        z[j++] = '\f';
      }else if( c=='n' ){
        z[j++] = '\n';
      }else if( c=='r' ){
        z[j++] = '\r';
      }else if( c=='t' ){
        z[j++] = '\t';
      }else if( c=='u' && i<n-4 ){
        /* FIX ME */
      }else{
        z[j++] = c;
      }
    }
  }
  z[j] = 0;
}

/* Convert the current token (which must be a string) into a true
** string (resolving all of the backslash escapes) and return a pointer
** to the true string.  Space is obtained from xjd1_malloc().
*/
static char *tokenDequoteString(JsonStr *pIn){
  const char *zIn;
  char *zOut;
  //int n;
  zIn = &pIn->zIn[pIn->iCur];
  zOut = xjd1_malloc( pIn->n );
  if( zOut==0 ) return 0;
  assert( zIn[0]=='"' && zIn[pIn->n-1]=='"' );
  //n = pIn->n-1;
  memcpy(zOut, zIn, pIn->n);
  xjd1DequoteString(zOut, pIn->n);
  return zOut;
}


/* Enter point to the first token of the JSON object.
** Exit pointing to the first token past end end of the
** JSON object.
*/
static JsonNode *parseJson(JsonStr *pIn){
  JsonNode *pNew;
  pNew = xjd1JsonNew(0);
  if( pNew==0 ) return 0;
  pNew->eJType = tokenType(pIn);
  switch( pNew->eJType ){
    case JSON_BEGIN_STRUCT: {
      JsonStructElem **ppTail;
      tokenNext(pIn);
      if( tokenType(pIn)==JSON_END_STRUCT ) break;
      ppTail = &pNew->u.st.pFirst;
      while( 1 ){
        JsonStructElem *pElem;
        if( tokenType(pIn)!=JSON_STRING ){
          goto json_error;
        }
        pElem = xjd1_malloc( sizeof(*pElem) );
        if( pElem==0 ) goto json_error;
        memset(pElem, 0, sizeof(*pElem));
        *ppTail = pElem;
        pNew->u.st.pLast = pElem;
        ppTail = &pElem->pNext;
        pElem->zLabel = tokenDequoteString(pIn);
        tokenNext(pIn);
        if( tokenType(pIn)!=JSON_COLON ){
          goto json_error;
        }
        tokenNext(pIn);
        pElem->pValue = parseJson(pIn);
        if( tokenType(pIn)==JSON_COMMA ){
          tokenNext(pIn);
        }else if( tokenType(pIn)==JSON_END_STRUCT ){
          tokenNext(pIn);
          break;
        }else{
          goto json_error;
        }
      }
      break;
    }
    case JSON_BEGIN_ARRAY: {
      int nAlloc = 0;
      tokenNext(pIn);
      if( tokenType(pIn)==JSON_END_ARRAY ){
        tokenNext(pIn);
        break;
      }
      while( 1 ){
        if( pNew->u.ar.nElem>=nAlloc ){
          JsonNode **pNewArray;
          nAlloc = nAlloc*2 + 5;
          pNewArray = xjd1_realloc(pNew->u.ar.apElem,
                              sizeof(JsonNode*)*nAlloc);
          if( pNewArray==0 ) goto json_error;
          pNew->u.ar.apElem = pNewArray;
        }
        pNew->u.ar.apElem[pNew->u.ar.nElem++] = parseJson(pIn);
        if( tokenType(pIn)==JSON_COMMA ){
          tokenNext(pIn);
        }else if( tokenType(pIn)==JSON_END_ARRAY ){
          tokenNext(pIn);
          break;
        }else{
          goto json_error;
        }
      }
      break;
    }
    case JSON_STRING: {
      pNew->u.z = tokenDequoteString(pIn);
      tokenNext(pIn);
      break;
    }
    case JSON_REAL: {
      pNew->u.r = atof(tokenString(pIn));
      tokenNext(pIn);
      break;
    }
    case JSON_TRUE:
    case JSON_FALSE:
    case JSON_NULL: {
      tokenNext(pIn);
      break;
    }
    default: {
      xjd1_free(pNew);
      pNew = 0;
      break;
    }
  }
  return pNew;

json_error:
  xjd1JsonFree(pNew);
  return 0;
}

/*
** Parse up a JSON string
*/
JsonNode *xjd1JsonParse(const char *zIn, int mxIn){
  JsonStr x;
  x.zIn = zIn;
  x.mxIn = mxIn>0 ? mxIn : xjd1Strlen30(zIn);
  x.iCur = 0;
  x.n = 0;
  x.eType = 0;
  tokenNext(&x);
  return parseJson(&x);
}

/*
** This function is used by the XJD1 shell in test mode. It assumes that
** the string zIn contains a list of white-space separated JSON values.
** It appends the contents of zIn to string pOut, making the following
** edits:
**
**   1. All white-space that appears inside of struct or array values
**      is removed. A single space characer is left between each top
**      level value.
**
**   2. Double quotes are added to any unquoted sequences of alphabetic
**      characters (which are illegal in JSON).
**
** For example, the following input:
**
**      {a:1,  "b":[1, 3, 4]}   [1,2]
**
** is transformed to:
**
**      {"a":1,"b":[1,3,4]} [1,2]
**
** before it is appended to pOut.
*/
int xjd1JsonTidy(String *pOut, const char *zIn){
  int nOpen = 0;                  /* Number of open {} or [] brackets */
  JsonStr x;                      /* Wrapper around zIn */

  /* Set up the string wrapper to tokenize zIn */
  x.zIn = zIn;
  x.mxIn = xjd1Strlen30(zIn);
  x.iCur = 0;
  x.n = 0;
  x.eType = 0;

  while( 1 ){
    int ePrev = x.eType;
    tokenNext(&x);
    if( x.eType==JSON_EOF ) break;
    if( ePrev && nOpen==0 ) xjd1StringAppend(pOut, " ", 1);
    if( x.eType==JSON_BEGIN_ARRAY || x.eType==JSON_BEGIN_STRUCT ) nOpen++;
    if( x.eType==JSON_END_ARRAY   || x.eType==JSON_END_STRUCT ) nOpen--;
    if( x.eType==JSON_ERROR && xjd1Isalpha(zIn[x.iCur]) ){
      while( xjd1Isalpha(zIn[x.iCur+x.n]) ) x.n++;
      xjd1StringAppend(pOut, "\"", 1);
      xjd1StringAppend(pOut, &zIn[x.iCur], x.n);
      xjd1StringAppend(pOut, "\"", 1);
    }else{
      xjd1StringAppend(pOut, &zIn[x.iCur], x.n);
    }
  }while( x.eType!=JSON_EOF );

  return XJD1_OK;
}

/*
** The JsonNode passed as the first argument must be of type XJD1_STRUCT.
** This function adds a property to the object. The name of the new
** property is passed as the second argument to this function. The third
** argument is used as the initial value. If the property already exists,
** it is replaced.
**
** This function decrements the reference count of the JsonNode passed
** as the third argument. This means it is possible to do:
**
**     pVal = xjd1JsonNew();            // nRef==1
**     xjd1JsonInsert(p, "x", pVal);    // Object p assumes ownership of pVal
**
** XJD1_OK is returned if the operation is completed successfully. Or
** XJD1_NOMEM if an OOM error is encountered.
*/
int xjd1JsonInsert(JsonNode *p, const char *zLabel, JsonNode *pVal){
  JsonStructElem *pElem;

  assert( p && p->eJType==XJD1_STRUCT && p->nRef==1 );
  for(pElem=p->u.st.pFirst; pElem; pElem=pElem->pNext){
    if( strcmp(zLabel, pElem->zLabel)==0 ) break;
  }
  if( pElem ){
    xjd1_free(pElem->zLabel);
    xjd1JsonFree(pElem->pValue);
  }else{
    pElem = xjd1_malloc(sizeof(*pElem));
    if( !pElem ){
      xjd1JsonFree(pVal);
      return XJD1_NOMEM;
    }

    if( p->u.st.pLast ){
      p->u.st.pLast->pNext = pElem;
    }else{
      p->u.st.pFirst = pElem;
    }
    pElem->pNext = 0;
    p->u.st.pLast = pElem;
  }

  pElem->pValue = pVal;
  pElem->zLabel = xjd1PoolDup(0, zLabel, -1);
  return (pElem->zLabel ? XJD1_OK : XJD1_NOMEM);
}

