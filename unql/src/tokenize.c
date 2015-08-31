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
** The tokenizer
*/
#include "xjd1Int.h"
#ifndef NDEBUG
#include <stdio.h>
#endif

/*
** The following 256 byte lookup table is used to support built-in
** equivalents to the following standard library functions:
**
**   isspace()                        0x01
**   isalpha()                        0x02
**   isdigit()                        0x04
**   isalnum()                        0x06
**   isxdigit()                       0x08
**   identifier character             0x40
**
** Bit 0x20 is set if the mapped character requires translation to upper
** case. i.e. if the character is a lower-case ASCII character.
** If x is a lower-case ASCII character, then its upper-case equivalent
** is (x - 0x20). Therefore toupper() can be implemented as:
**
**   (x & ~(map[x]&0x20))
**
** Bit 0x40 is set if the character non-alphanumeric and can be used in an
** identifier.  Identifiers are alphanumerics, "_" and any
** non-ASCII UTF character. Hence the test for whether or not a character is
** part of an identifier is 0x46.
*/
const unsigned char xjd1CtypeMap[256] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 00..07    ........ */
  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,  /* 08..0f    ........ */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 10..17    ........ */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 18..1f    ........ */
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 20..27     !"#$%&' */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 28..2f    ()*+,-./ */
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,  /* 30..37    01234567 */
  0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 38..3f    89:;<=>? */

  0x00, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x02,  /* 40..47    @ABCDEFG */
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,  /* 48..4f    HIJKLMNO */
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,  /* 50..57    PQRSTUVW */
  0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x40,  /* 58..5f    XYZ[\]^_ */
  0x00, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x22,  /* 60..67    `abcdefg */
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,  /* 68..6f    hijklmno */
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,  /* 70..77    pqrstuvw */
  0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 78..7f    xyz{|}~. */

  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 80..87    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 88..8f    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 90..97    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* 98..9f    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* a0..a7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* a8..af    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* b0..b7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* b8..bf    ........ */

  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* c0..c7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* c8..cf    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* d0..d7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* d8..df    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* e0..e7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* e8..ef    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  /* f0..f7    ........ */
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40   /* f8..ff    ........ */
};

/**********************************************************************
** The following code is automatically generated
** by ../tool/mkkeywordhash.c
*/
/* Hash score: 61 */
static int keywordCode(const char *z, int n){
  /* zText[] encodes 333 bytes of keywords in 224 bytes */
  /*   BEGINTORDEROLLBACKELSELECTGROUPDATEACHAVINGLOBYWITHINSERTALL       */
  /*   IMITASCENDINGASYNCHRONOUSCOLLATEXISTSCOLLECTIONULLCREATEXCEPT      */
  /*   DELETEDESCENDINGDROPRAGMAFLATTENOTIFROMILIKEUNIONVALUEWHEREin      */
  /*   ullCOMMITDISTINCTINTERSECTOFFSETfalsetrue                          */
  static const char zText[223] = {
    'B','E','G','I','N','T','O','R','D','E','R','O','L','L','B','A','C','K',
    'E','L','S','E','L','E','C','T','G','R','O','U','P','D','A','T','E','A',
    'C','H','A','V','I','N','G','L','O','B','Y','W','I','T','H','I','N','S',
    'E','R','T','A','L','L','I','M','I','T','A','S','C','E','N','D','I','N',
    'G','A','S','Y','N','C','H','R','O','N','O','U','S','C','O','L','L','A',
    'T','E','X','I','S','T','S','C','O','L','L','E','C','T','I','O','N','U',
    'L','L','C','R','E','A','T','E','X','C','E','P','T','D','E','L','E','T',
    'E','D','E','S','C','E','N','D','I','N','G','D','R','O','P','R','A','G',
    'M','A','F','L','A','T','T','E','N','O','T','I','F','R','O','M','I','L',
    'I','K','E','U','N','I','O','N','V','A','L','U','E','W','H','E','R','E',
    'i','n','u','l','l','C','O','M','M','I','T','D','I','S','T','I','N','C',
    'T','I','N','T','E','R','S','E','C','T','O','F','F','S','E','T','f','a',
    'l','s','e','t','r','u','e',
  };
  static const unsigned char aHash[97] = {
       0,  34,   1,   0,   7,   0,  25,  26,   0,  39,   0,   0,  20,
       0,  42,  37,  35,  46,  43,   0,   0,   0,  40,   0,   0,   8,
      21,   0,   0,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,
       0,   0,   0,  44,   0,  13,   0,   0,  50,   0,   0,   6,   0,
       0,   0,  45,  41,   0,  52,  22,   0,   0,   0,   0,   0,  24,
      28,  49,  36,  19,  16,   0,   0,   0,   2,  17,  32,   0,  48,
       0,   0,   0,  51,   0,   0,  27,  30,   0,   0,   0,  31,  14,
       5,   0,   0,  23,  15,  47,
  };
  static const unsigned char aNext[52] = {
       0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      11,   0,   0,   0,   0,   9,   0,   0,   0,   0,   0,   0,   0,
      18,   0,   0,   0,   0,   3,   0,  12,   0,   0,   0,   0,  29,
      38,   0,  10,   0,   0,   0,   0,  33,   0,   0,   0,   0,   0,
  };
  static const unsigned char aLen[52] = {
       5,   4,   5,   8,   4,   6,   5,   6,   4,   6,   4,   2,   6,
       6,   3,   5,   3,   9,   5,  12,   2,  11,   4,   7,   6,  10,
       4,   6,   6,   6,   4,  10,   4,   6,   7,   3,   2,   4,   5,
       4,   5,   5,   5,   2,   4,   6,   8,   9,   6,   3,   5,   4,
  };
  static const unsigned short int aOffset[52] = {
       0,   3,   6,  10,  18,  20,  26,  29,  34,  37,  42,  45,  47,
      51,  57,  59,  64,  64,  73,  73,  73,  74,  74,  85,  91,  97,
     106, 110, 115, 121, 127, 127, 137, 140, 146, 152, 155, 156, 160,
     161, 165, 170, 175, 180, 181, 185, 191, 199, 208, 211, 214, 219,
  };
  static const unsigned char aCode[52] = {
    TK_BEGIN,      TK_INTO,       TK_ORDER,      TK_ROLLBACK,   TK_ELSE,       
    TK_SELECT,     TK_GROUP,      TK_UPDATE,     TK_FLATTENOP,  TK_HAVING,     
    TK_LIKEOP,     TK_BY,         TK_WITHIN,     TK_INSERT,     TK_ALL,        
    TK_LIMIT,      TK_ASCENDING,  TK_ASCENDING,  TK_ASYNC,      TK_ASYNC,      
    TK_AS,         TK_SYNC,       TK_SYNC,       TK_COLLATE,    TK_EXISTS,     
    TK_COLLECTION, TK_NULL,       TK_CREATE,     TK_EXCEPT,     TK_DELETE,     
    TK_DESCENDING, TK_DESCENDING, TK_DROP,       TK_PRAGMA,     TK_FLATTENOP,  
    TK_NOT,        TK_IF,         TK_FROM,       TK_ILIKEOP,    TK_LIKEOP,     
    TK_UNION,      TK_VALUE,      TK_WHERE,      TK_IN,         TK_NULL,       
    TK_COMMIT,     TK_DISTINCT,   TK_INTERSECT,  TK_OFFSET,     TK_SET,        
    TK_FALSE,      TK_TRUE,       
  };
  int h, i;
  if( n<2 ) return TK_ID;
  h = (z[0]*4 ^ z[n-1]*3 ^ n) % 97;
  for(i=((int)aHash[h])-1; i>=0; i=((int)aNext[i])-1){
    if( aLen[i]==n && memcmp(&zText[aOffset[i]],z,n)==0 ){
      return aCode[i];
    }
  }
  return TK_ID;
}
#define XJD1_N_KEYWORD 52

/* End of the automatically generated hash code
*********************************************************************/

/*
** Return the length of the token that begins at z[0].
** Store the token type in *tokenType before returning.
*/
int xjd1GetToken(const unsigned char *z, int *tokenType){
  int i, c;
  switch( *z ){
    case ' ': case '\t': case '\n': case '\f': case '\r': {
      for(i=1; xjd1Isspace(z[i]); i++){}
      *tokenType = TK_SPACE;
      return i;
    }
    case '-': {
      if( z[1]=='-' ){
        for(i=2; (c=z[i])!=0 && c!='\n'; i++){}
        *tokenType = TK_SPACE;
        return i;
      }
      *tokenType = TK_MINUS;
      return 1;
    }
    case '?': {
      *tokenType = TK_QM;
      return 1;
    }
    case '(': {
      *tokenType = TK_LP;
      return 1;
    }
    case ')': {
      *tokenType = TK_RP;
      return 1;
    }
    case '[': {
      *tokenType = TK_LB;
      return 1;
    }
    case ']': {
      *tokenType = TK_RB;
      return 1;
    }
    case '{': {
      *tokenType = TK_LC;
      return 1;
    }
    case '}': {
      *tokenType = TK_RC;
      return 1;
    }
    case ';': {
      *tokenType = TK_SEMI;
      return 1;
    }
    case '+': {
      *tokenType = TK_PLUS;
      return 1;
    }
    case '*': {
      *tokenType = TK_STAR;
      return 1;
    }
    case '/': {
      if( z[1]!='*' || z[2]==0 ){
        *tokenType = TK_SLASH;
        return 1;
      }
      for(i=3, c=z[2]; (c!='*' || z[i]!='/') && (c=z[i])!=0; i++){}
      if( c ) i++;
      *tokenType = TK_SPACE;
      return i;
    }
    case '%': {
      *tokenType = TK_REM;
      return 1;
    }
    case '=': {
      if( z[1]!='=' ){
        *tokenType = TK_EQ;
        return 1;
      }else if( z[2]=='=' ){
        *tokenType = TK_EQ3;
        return 3;
      }else{
        *tokenType = TK_EQEQ;
        return 2;
      }
    }
    case '<': {
      if( (c=z[1])=='=' ){
        *tokenType = TK_LE;
        return 2;
      }else if( c=='>' ){
        *tokenType = TK_NE;
        return 2;
      }else if( c=='<' ){
        *tokenType = TK_LSHIFT;
        return 2;
      }else{
        *tokenType = TK_LT;
        return 1;
      }
    }
    case '>': {
      if( (c=z[1])=='=' ){
        *tokenType = TK_GE;
        return 2;
      }else if( c!='>' ){
        *tokenType = TK_GT;
        return 1;
      }else if( z[2]=='>' ){
        *tokenType = TK_URSHIFT;
        return 3;
      }else{
        *tokenType = TK_RSHIFT;
        return 2;
      }
    }
    case '!': {
      if( z[1]!='=' ){
        *tokenType = TK_BANG;
        return 1;
      }else if( z[2]=='=' ){
        *tokenType = TK_NE3;
        return 3;
      }else{
        *tokenType = TK_NE;
        return 2;
      }
    }
    case '|': {
      if( z[1]!='|' ){
        *tokenType = TK_BITOR;
        return 1;
      }else{
        *tokenType = TK_OR;
        return 2;
      }
    }
    case ',': {
      *tokenType = TK_COMMA;
      return 1;
    }
    case ':': {
      *tokenType = TK_COLON;
      return 1;
    }
    case '&': {
      if( z[1]=='&' ){
        *tokenType = TK_AND;
        return 2;
      }else{
        *tokenType = TK_BITAND;
        return 1;
      }
    }
    case '~': {
      *tokenType = TK_BITNOT;
      return 1;
    }
    case '"': {
      int delim = z[0];
      for(i=1; (c=z[i])!=0; ++i){
        if(c == '\\'){
            switch(z[i+1])
            {
            case '\\':
            case '"':
            case 'n':
            case 'r':
            case 'f':
            case 't':
            case 'b':
                    ++i;
                break;
            }
        }
        if( c==delim ) {
            break;
        }
      }
      *tokenType = TK_STRING;
      return i+1;
    }
    case '.': {
      if( !xjd1Isdigit(z[1]) ){
        *tokenType = TK_DOT;
        return 1;
      }
      /* If the next character is a digit, this is a floating point
      ** number that begins with ".".  Fall thru into the next case */
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
      *tokenType = TK_INTEGER;
      for(i=0; xjd1Isdigit(z[i]); i++){}
      if( z[i]=='.' ){
        i++;
        while( xjd1Isdigit(z[i]) ){ i++; }
        *tokenType = TK_FLOAT;
      }
      if( (z[i]=='e' || z[i]=='E') &&
           ( xjd1Isdigit(z[i+1])
            || ((z[i+1]=='+' || z[i+1]=='-') && xjd1Isdigit(z[i+2]))
           )
      ){
        i += 2;
        while( xjd1Isdigit(z[i]) ){ i++; }
        *tokenType = TK_FLOAT;
      }
      while( xjd1Isident(z[i]) ){
        *tokenType = TK_ILLEGAL;
        i++;
      }
      return i;
    }
    default: {
      if( !xjd1Isident(*z) ){
        break;
      }
      for(i=1; xjd1Isident(z[i]); i++){}
      *tokenType = keywordCode((char*)z, i);
      return i;
    }
  }
  *tokenType = TK_ILLEGAL;
  return 1;
}

static void *parserAlloc(size_t N){
  return xjd1_malloc((int)N);
}

/*
** Run the parser on the given SQL string.  The parser structure is
** passed in.  An SQLITE_ status code is returned.  If an error occurs
** then an and attempt is made to write an error message into
** memory obtained from xjd1_malloc() and to make *pzErrMsg point to that
** error message.
*/
int xjd1RunParser(
  xjd1 *pConn,
  xjd1_stmt *pStmt,
  const char *zCode,
  int *pN
){
  int i = 0;
  Parse sParse;
  int tokenType;
  void *pEngine;
  int lastTokenParsed = 0;
  int nToken = 0;
  extern void *xjd1ParserAlloc(void*(*)(size_t));
  extern void xjd1ParserFree(void*, void(*)(void*));
  extern void xjd1Parser(void*,int,Token,Parse*);

#ifndef NDEBUG
  if( pConn->parserTrace ){
    extern void xjd1ParserTrace(FILE*, char*);
    xjd1ParserTrace(stdout, "parser> ");
  }
#endif

  pEngine = xjd1ParserAlloc(parserAlloc);
  memset(&sParse, 0, sizeof(sParse));
  if( pEngine==0 ){
    sParse.errCode = XJD1_NOMEM;
    goto abort_parse;
  }

  sParse.pConn = pConn;
  sParse.pPool = &pStmt->sPool;
  xjd1StringInit(&sParse.errMsg, &pStmt->sPool, 0);
  while( zCode[i] && sParse.errCode==0 ){
    assert( i>=0 );
    sParse.sTok.z = &zCode[i];
    sParse.sTok.n = xjd1GetToken((unsigned char*)&zCode[i], &tokenType);
    i += sParse.sTok.n;
    switch( tokenType ){
      case TK_SPACE: {
        break;
      }
      case TK_ILLEGAL: {
        xjd1ParseError(&sParse, XJD1_ERROR,
            "unrecognized token: \"%.*s\"", sParse.sTok.n, sParse.sTok.z
        );
        goto abort_parse;
      }
      default: {
        nToken++;
        xjd1Parser(pEngine, tokenType, sParse.sTok, &sParse);
        lastTokenParsed = tokenType;
        if( sParse.errCode || tokenType==TK_SEMI ) goto abort_parse;
        break;
      }
    }
  }

abort_parse:
  if( sParse.errCode==XJD1_OK && nToken>0 ){
    if( lastTokenParsed!=TK_SEMI ){
      sParse.sTok.z = ";";
      sParse.sTok.n = 1;
      xjd1Parser(pEngine, TK_SEMI, sParse.sTok, &sParse);
    }
    xjd1Parser(pEngine, 0, sParse.sTok, &sParse);
  }
  xjd1ParserFree(pEngine, xjd1_free);

  if( sParse.errCode!=XJD1_OK ){
    xjd1Error(pConn, sParse.errCode, "%s", sParse.errMsg.zBuf);
  }
  pStmt->pCmd = sParse.pCmd;
  *pN = i;

  return sParse.errCode;
}

/*
** Set the error code and message for the Parse object passed as the
** first argument.
*/
void xjd1ParseError(Parse *p, int errCode, const char *zFormat, ...){
  va_list ap;
  if( p->errCode==XJD1_OK ){
    p->errCode = errCode;
    va_start(ap, zFormat);
    xjd1StringVAppendF(&p->errMsg, zFormat, ap);
    va_end(ap);
  }
}
