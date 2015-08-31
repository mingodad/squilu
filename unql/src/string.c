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
** String manipulation routines.
*/
#include "xjd1Int.h"

/*
** Compute a string length that is limited to what can be stored in
** lower 30 bits of a 32-bit signed integer.
**
** The value returned will never be negative.  Nor will it ever be greater
** than the actual length of the string.  For very long strings (greater
** than 1GiB) the value returned might be less than the true string length.
*/
PRIVATE int xjd1Strlen30(const char *z){
  const char *z2 = z;
  if( z==0 ) return 0;
  while( *z2 ){ z2++; }
  return 0x3fffffff & (int)(z2 - z);
}

/*
** Change the allocation size for a string to newSize bytes.  Return
** 1 if an OOM error occurs or 0 on success.
*/
static int resizeString(String *pStr, int newSize){
  char *zNew;
  if( pStr->pPool ){
    zNew = xjd1PoolMalloc(pStr->pPool, newSize);
    if( zNew==0 ) return 1;
    memcpy(zNew, pStr->zBuf, pStr->nUsed);
  }else{
    zNew = xjd1_realloc(pStr->zBuf, newSize);
    if( zNew==0 ) return 1;
  }
  pStr->nAlloc = newSize;
  pStr->zBuf = zNew;
  return 0;
}

/*
** Truncate a string to zero length
*/
void xjd1StringTruncate(String *pStr){
  if( pStr ){
    pStr->nUsed = 0;
    if( pStr->zBuf ) pStr->zBuf[0] = 0;
  }
}

/*
** Return the value of a string.
*/
char *xjd1StringGet(String *pStr){
  char *z;
  if( pStr==0 ) return 0;
  z = pStr->zBuf;
  memset(pStr, 0, sizeof(*pStr));
  return z;
}

/*
** Append text in z to a string.  If n>=0 then append exactly
** n bytes.  If n<0 then append all of z[] up to the zero terminator.
**
** Return the number of bytes appended.  0 is returned on an OOM
** error.
*/
PRIVATE int xjd1StringAppend(String *pStr, const char *z, int n){
  if( n<0 ) n = xjd1Strlen30(z);
  if( pStr->nUsed + n + 1 >= pStr->nAlloc ){
    if( resizeString(pStr, pStr->nAlloc*2 + n + 100) ) return 0;
  }
  if( z ){
    memcpy(&pStr->zBuf[pStr->nUsed], z, n);
    pStr->nUsed += n;
    pStr->zBuf[pStr->nUsed] = 0;
  }
  return n;
}

/*
** Initialize a string object that has been previously allocated.
** In other words, turn bulk memory into a string object.
*/
PRIVATE void xjd1StringInit(String *pStr, Pool *pPool, int initSize){
  memset(pStr, 0, sizeof(*pStr));
  pStr->pPool = pPool;
  if( initSize>0 ){
    resizeString(pStr, initSize);
  }
}

/*
** Allocate a new string object.  Initialize the string buffer to
** initSize bytes.  initSize can be zero.
**
** If pPool is not NULL then all string memory allocations including
** the allocation of the String object itself, come from the identified
** memory pool.  If pPool is NULL, then xjd1_malloc()/xjd1_free() are used.
*/
PRIVATE String *xjd1StringNew(Pool *pPool, int initSize){
  String *pStr;
  if( pPool ){
    pStr = xjd1PoolMalloc(pPool, sizeof(*pStr));
  }else{
    pStr = xjd1_malloc( sizeof(*pStr) );
  }
  if( pStr) xjd1StringInit(pStr, pPool, initSize);
  return pStr;
}

/*
** Free the content of a string but not the String object itself.
*/
PRIVATE void xjd1StringClear(String *pStr){
  if( pStr ){
    if( pStr->pPool==0 ) xjd1_free(pStr->zBuf);
    memset(pStr, 0, sizeof(*pStr));
  }
}

/*
** Free a string previously allocated using xjd1StringNew().
*/
PRIVATE void xjd1StringDelete(String *pStr){
  if( pStr && pStr->pPool==0 ){
    xjd1_free(pStr->zBuf);
    xjd1_free(pStr);
  }
}

/*
** Consume the first N characters from the front of the string.
*/
PRIVATE void xjd1StringRemovePrefix(String *pStr, int N){
  if( pStr==0 ) return;
  if( pStr->nUsed<=N ){
    xjd1StringTruncate(pStr);
  }else{
    memmove(pStr->zBuf, &pStr->zBuf[N], (pStr->nUsed+1)-N);
    pStr->nUsed -= N;
  }
}

/*
** Conversion types fall into various categories as defined by the
** following enumeration.
*/
#define etRADIX       1 /* Integer types.  %d, %x, %o, and so forth */
#define etFLOAT       2 /* Floating point.  %f */
#define etEXP         3 /* Exponentional notation. %e and %E */
#define etGENERIC     4 /* Floating or exponential, depending on exponent. %g */
#define etSIZE        5 /* Return number of characters processed so far. %n */
#define etSTRING      6 /* Strings. %s */
#define etDYNSTRING   7 /* Dynamically allocated strings. %z */
#define etPERCENT     8 /* Percent symbol. %% */
#define etCHARX       9 /* Characters. %c */
#define etERROR      10 /* Used to indicate no such conversion type */
#define etPOINTER    11 /* Pointer value.  %p */
/* The rest are extensions, not normally found in printf() */
#define etSTRESCAPE  12 /* Strings with escapes. %q */

/*
** An "etByte" is an 8-bit unsigned value.
*/
typedef unsigned char etByte;
typedef long long int i64;
typedef unsigned long long int u64;

/*
** Each builtin conversion character (ex: the 'd' in "%d") is described
** by an instance of the following structure
*/
typedef struct et_info {   /* Information about each format field */
  char fmttype;            /* The format field code letter */
  etByte base;             /* The base for radix conversion */
  etByte flags;            /* One or more of FLAG_ constants below */
  etByte type;             /* Conversion paradigm */
  etByte charset;          /* Offset into aDigits[] of the digits string */
  etByte prefix;           /* Offset into aPrefix[] of the prefix string */
} et_info;

/*
** Allowed values for et_info.flags
*/
#define FLAG_SIGNED  1     /* True if the value to convert is signed */
#define FLAG_INTERN  2     /* True if for internal use only */
#define FLAG_STRING  4     /* Allow infinity precision */


/*
** The following table is searched linearly, so it is good to put the
** most frequently used conversion types first.
*/
static const char aDigits[] = "0123456789ABCDEF0123456789abcdef";
static const char aPrefix[] = "-x0\000X0";
static const et_info fmtinfo[] = {
  {  'd', 10, 1, etRADIX,      0,  0 },
  {  's',  0, 4, etSTRING,     0,  0 },
  {  'g',  0, 1, etGENERIC,    30, 0 },
  {  'q',  0, 4, etSTRESCAPE,  0,  0 },
  {  'c',  0, 0, etCHARX,      0,  0 },
  {  'o',  8, 0, etRADIX,      0,  2 },
  {  'u', 10, 0, etRADIX,      0,  0 },
  {  'x', 16, 0, etRADIX,      16, 1 },
  {  'X', 16, 0, etRADIX,      0,  4 },
  {  'f',  0, 1, etFLOAT,      0,  0 },
  {  'e',  0, 1, etEXP,        30, 0 },
  {  'E',  0, 1, etEXP,        14, 0 },
  {  'G',  0, 1, etGENERIC,    14, 0 },
  {  'i', 10, 1, etRADIX,      0,  0 },
  {  'n',  0, 0, etSIZE,       0,  0 },
  {  '%',  0, 0, etPERCENT,    0,  0 },
  {  'p', 16, 0, etPOINTER,    0,  1 },
};
#define etNINFO  (sizeof(fmtinfo)/sizeof(fmtinfo[0]))

/*
** "*val" is a double such that 0.1 <= *val < 10.0
** Return the ascii code for the leading digit of *val, then
** multiply "*val" by 10.0 to renormalize.
**
** Example:
**     input:     *val = 3.14159
**     output:    *val = 1.4159    function return = '3'
**
** The counter *cnt is incremented each time.  After counter exceeds
** 16 (the number of significant digits in a 64-bit float) '0' is
** always returned.
*/
static int et_getdigit(long double *val, int *cnt){
  int digit;
  long double d;
  if( (*cnt)++ >= 16 ) return '0';
  digit = (int)*val;
  d = digit;
  digit += '0';
  *val = (*val - d)*10.0;
  return digit;
}

/*
** Size of temporary conversion buffer.
*/
#define etBUFSIZE 500

/*
** Find the length of a string as long as that length does not
** exceed N bytes.  If no zero terminator is seen in the first
** N bytes then return N.  If N is negative, then this routine
** is an alias for strlen().
*/
static int StrNLen32(const char *z, int N){
  int n = 0;
  while( (N-- != 0) && *(z++)!=0 ){ n++; }
  return n;
}


/*
** INPUTS:
**   pStr   Append output to this string.
**
**   fmt    This is the format string, as in the usual print.
**
**   ap     This is a pointer to a list of arguments.  Same as in
**          vfprint.
**
** OUTPUTS:
**          The return value is the total number of characters appended.
**          Returns -1 on a error.
**
** Note that the order in which automatic variables are declared below
** seems to make a big difference in determining how fast this beast
** will run.
*/
PRIVATE int xjd1StringVAppendF(
  String *pStr,                      /* Append output to this string */
  const char *fmt,                   /* Format string */
  va_list ap                         /* arguments */
){
  int c;                     /* Next character in the format string */
  char *bufpt;               /* Pointer to the conversion buffer */
  int precision;             /* Precision of the current field */
  int length;                /* Length of the field */
  int idx;                   /* A general purpose loop counter */
  int count;                 /* Total number of characters output */
  int width;                 /* Width of the current field */
  etByte flag_leftjustify;   /* True if "-" flag is present */
  etByte flag_plussign;      /* True if "+" flag is present */
  etByte flag_blanksign;     /* True if " " flag is present */
  etByte flag_alternateform; /* True if "#" flag is present */
  etByte flag_altform2;      /* True if "!" flag is present */
  etByte flag_zeropad;       /* True if field width constant starts with zero */
  etByte flag_long;          /* True if "l" flag is present */
  etByte flag_longlong;      /* True if the "ll" flag is present */
  etByte done;               /* Loop termination flag */
  u64 longvalue;             /* Value for integer types */
  long double realvalue;     /* Value for real types */
  const et_info *infop;      /* Pointer to the appropriate info structure */
  char buf[etBUFSIZE];       /* Conversion buffer */
  char prefix;               /* Prefix character.  "+" or "-" or " " or '\0'. */
  etByte errorflag = 0;      /* True if an error is encountered */
  etByte xtype;              /* Conversion paradigm */
  char *zExtra;              /* Extra memory used for etTCLESCAPE conversions */
  static const char spaces[] =
   "                                                                         ";
#define etSPACESIZE (sizeof(spaces)-1)
  int  exp, e2;              /* exponent of real numbers */
  double rounder;            /* Used for rounding floating point values */
  etByte flag_dp;            /* True if decimal point should be shown */
  etByte flag_rtz;           /* True if trailing zeros should be removed */
  etByte flag_exp;           /* True to force display of the exponent */
  int nsd;                   /* Number of significant digits returned */

  count = length = 0;
  bufpt = 0;
  for(; (c=(*fmt))!=0; ++fmt){
    if( c!='%' ){
      int amt;
      bufpt = (char *)fmt;
      amt = 1;
      while( (c=(*++fmt))!='%' && c!=0 ) amt++;
      xjd1StringAppend(pStr,bufpt,amt);
      count += amt;
      if( c==0 ) break;
    }
    if( (c=(*++fmt))==0 ){
      errorflag = 1;
      xjd1StringAppend(pStr,"%",1);
      count++;
      break;
    }
    /* Find out what flags are present */
    flag_leftjustify = flag_plussign = flag_blanksign = 
     flag_alternateform = flag_altform2 = flag_zeropad = 0;
    done = 0;
    do{
      switch( c ){
        case '-':   flag_leftjustify = 1;     break;
        case '+':   flag_plussign = 1;        break;
        case ' ':   flag_blanksign = 1;       break;
        case '#':   flag_alternateform = 1;   break;
        case '!':   flag_altform2 = 1;        break;
        case '0':   flag_zeropad = 1;         break;
        default:    done = 1;                 break;
      }
    }while( !done && (c=(*++fmt))!=0 );
    /* Get the field width */
    width = 0;
    if( c=='*' ){
      width = va_arg(ap,int);
      if( width<0 ){
        flag_leftjustify = 1;
        width = -width;
      }
      c = *++fmt;
    }else{
      while( c>='0' && c<='9' ){
        width = width*10 + c - '0';
        c = *++fmt;
      }
    }
    if( width > etBUFSIZE-10 ){
      width = etBUFSIZE-10;
    }
    /* Get the precision */
    if( c=='.' ){
      precision = 0;
      c = *++fmt;
      if( c=='*' ){
        precision = va_arg(ap,int);
        if( precision<0 ) precision = -precision;
        c = *++fmt;
      }else{
        while( c>='0' && c<='9' ){
          precision = precision*10 + c - '0';
          c = *++fmt;
        }
      }
    }else{
      precision = -1;
    }
    /* Get the conversion type modifier */
    if( c=='l' ){
      flag_long = 1;
      c = *++fmt;
      if( c=='l' ){
        flag_longlong = 1;
        c = *++fmt;
      }else{
        flag_longlong = 0;
      }
    }else{
      flag_long = flag_longlong = 0;
    }
    /* Fetch the info entry for the field */
    infop = 0;
    xtype = etERROR;
    for(idx=0; idx<etNINFO; idx++){
      if( c==fmtinfo[idx].fmttype ){
        infop = &fmtinfo[idx];
        xtype = infop->type;
        break;
      }
    }
    zExtra = 0;

    /* Limit the precision to prevent overflowing buf[] during conversion */
    if( precision>etBUFSIZE-40 && (infop->flags & FLAG_STRING)==0 ){
      precision = etBUFSIZE-40;
    }

    /*
    ** At this point, variables are initialized as follows:
    **
    **   flag_alternateform          TRUE if a '#' is present.
    **   flag_altform2               TRUE if a '!' is present.
    **   flag_plussign               TRUE if a '+' is present.
    **   flag_leftjustify            TRUE if a '-' is present or if the
    **                               field width was negative.
    **   flag_zeropad                TRUE if the width began with 0.
    **   flag_long                   TRUE if the letter 'l' (ell) prefixed
    **                               the conversion character.
    **   flag_longlong               TRUE if the letter 'll' (ell ell) prefixed
    **                               the conversion character.
    **   flag_blanksign              TRUE if a ' ' is present.
    **   width                       The specified field width.  This is
    **                               always non-negative.  Zero is the default.
    **   precision                   The specified precision.  The default
    **                               is -1.
    **   xtype                       The class of the conversion.
    **   infop                       Pointer to the appropriate info struct.
    */
    switch( xtype ){
      case etPOINTER:
        flag_longlong = sizeof(char*)==sizeof(i64);
        flag_long = sizeof(char*)==sizeof(long int);
        /* Fall through into the next case */
      case etRADIX:
        if( infop->flags & FLAG_SIGNED ){
          i64 v;
          if( flag_longlong )   v = va_arg(ap,i64);
          else if( flag_long )  v = va_arg(ap,long int);
          else                  v = va_arg(ap,int);
          if( v<0 ){
            longvalue = -v;
            prefix = '-';
          }else{
            longvalue = v;
            if( flag_plussign )        prefix = '+';
            else if( flag_blanksign )  prefix = ' ';
            else                       prefix = 0;
          }
        }else{
          if( flag_longlong )   longvalue = va_arg(ap,u64);
          else if( flag_long )  longvalue = va_arg(ap,unsigned long int);
          else                  longvalue = va_arg(ap,unsigned int);
          prefix = 0;
        }
        if( longvalue==0 ) flag_alternateform = 0;
        if( flag_zeropad && precision<width-(prefix!=0) ){
          precision = width-(prefix!=0);
        }
        bufpt = &buf[etBUFSIZE-1];
        {
          register const char *cset;      /* Use registers for speed */
          register int base;
          cset = &aDigits[infop->charset];
          base = infop->base;
          do{                                           /* Convert to ascii */
            *(--bufpt) = cset[longvalue%base];
            longvalue = longvalue/base;
          }while( longvalue>0 );
        }
        length = &buf[etBUFSIZE-1]-bufpt;
        for(idx=precision-length; idx>0; idx--){
          *(--bufpt) = '0';                             /* Zero pad */
        }
        if( prefix ) *(--bufpt) = prefix;               /* Add sign */
        if( flag_alternateform && infop->prefix ){      /* Add "0" or "0x" */
          const char *pre;
          char x;
          pre = &aPrefix[infop->prefix];
          if( *bufpt!=pre[0] ){
            for(; (x=(*pre))!=0; pre++) *(--bufpt) = x;
          }
        }
        length = &buf[etBUFSIZE-1]-bufpt;
        break;
      case etFLOAT:
      case etEXP:
      case etGENERIC:
        realvalue = va_arg(ap,double);
        if( precision<0 ) precision = 6;         /* Set default precision */
        if( precision>etBUFSIZE/2-10 ) precision = etBUFSIZE/2-10;
        if( realvalue<0.0 ){
          realvalue = -realvalue;
          prefix = '-';
        }else{
          if( flag_plussign )          prefix = '+';
          else if( flag_blanksign )    prefix = ' ';
          else                         prefix = 0;
        }
        if( xtype==etGENERIC && precision>0 ) precision--;
#if 0
        /* Rounding works like BSD when the constant 0.4999 is used.  Wierd! */
        for(idx=precision, rounder=0.4999; idx>0; idx--, rounder*=0.1);
#else
        /* It makes more sense to use 0.5 */
        for(idx=precision, rounder=0.5; idx>0; idx--, rounder*=0.1);
#endif
        if( xtype==etFLOAT ) realvalue += rounder;
        /* Normalize realvalue to within 10.0 > realvalue >= 1.0 */
        exp = 0;
        if( realvalue>0.0 ){
          while( realvalue>=1e32 && exp<=350 ){ realvalue *= 1e-32; exp+=32; }
          while( realvalue>=1e8 && exp<=350 ){ realvalue *= 1e-8; exp+=8; }
          while( realvalue>=10.0 && exp<=350 ){ realvalue *= 0.1; exp++; }
          while( realvalue<1e-8 && exp>=-350 ){ realvalue *= 1e8; exp-=8; }
          while( realvalue<1.0 && exp>=-350 ){ realvalue *= 10.0; exp--; }
          if( exp>350 || exp<-350 ){
            bufpt = "NaN";
            length = 3;
            break;
          }
        }
        bufpt = buf;
        /*
        ** If the field type is etGENERIC, then convert to either etEXP
        ** or etFLOAT, as appropriate.
        */
        flag_exp = xtype==etEXP;
        if( xtype!=etFLOAT ){
          realvalue += rounder;
          if( realvalue>=10.0 ){ realvalue *= 0.1; exp++; }
        }
        if( xtype==etGENERIC ){
          flag_rtz = !flag_alternateform;
          if( exp<-4 || exp>precision ){
            xtype = etEXP;
          }else{
            precision = precision - exp;
            xtype = etFLOAT;
          }
        }else{
          flag_rtz = 0;
        }
        if( xtype==etEXP ){
          e2 = 0;
        }else{
          e2 = exp;
        }
        nsd = 0;
        flag_dp = (precision>0) | flag_alternateform | flag_altform2;
        /* The sign in front of the number */
        if( prefix ){
          *(bufpt++) = prefix;
        }
        /* Digits prior to the decimal point */
        if( e2<0 ){
          *(bufpt++) = '0';
        }else{
          for(; e2>=0; e2--){
            *(bufpt++) = et_getdigit(&realvalue,&nsd);
          }
        }
        /* The decimal point */
        if( flag_dp ){
          *(bufpt++) = '.';
        }
        /* "0" digits after the decimal point but before the first
        ** significant digit of the number */
        for(e2++; e2<0 && precision>0; precision--, e2++){
          *(bufpt++) = '0';
        }
        /* Significant digits after the decimal point */
        while( (precision--)>0 ){
          *(bufpt++) = et_getdigit(&realvalue,&nsd);
        }
        /* Remove trailing zeros and the "." if no digits follow the "." */
        if( flag_rtz && flag_dp ){
          while( bufpt[-1]=='0' ) *(--bufpt) = 0;
          assert( bufpt>buf );
          if( bufpt[-1]=='.' ){
            if( flag_altform2 ){
              *(bufpt++) = '0';
            }else{
              *(--bufpt) = 0;
            }
          }
        }
        /* Add the "eNNN" suffix */
        if( flag_exp || (xtype==etEXP && exp) ){
          *(bufpt++) = aDigits[infop->charset];
          if( exp<0 ){
            *(bufpt++) = '-'; exp = -exp;
          }else{
            *(bufpt++) = '+';
          }
          if( exp>=100 ){
            *(bufpt++) = (exp/100)+'0';                /* 100's digit */
            exp %= 100;
          }
          *(bufpt++) = exp/10+'0';                     /* 10's digit */
          *(bufpt++) = exp%10+'0';                     /* 1's digit */
        }
        *bufpt = 0;

        /* The converted number is in buf[] and zero terminated. Output it.
        ** Note that the number is in the usual order, not reversed as with
        ** integer conversions. */
        length = bufpt-buf;
        bufpt = buf;

        /* Special case:  Add leading zeros if the flag_zeropad flag is
        ** set and we are not left justified */
        if( flag_zeropad && !flag_leftjustify && length < width){
          int i;
          int nPad = width - length;
          for(i=width; i>=nPad; i--){
            bufpt[i] = bufpt[i-nPad];
          }
          i = prefix!=0;
          while( nPad-- ) bufpt[i++] = '0';
          length = width;
        }
        break;
      case etSIZE:
        *(va_arg(ap,int*)) = count;
        length = width = 0;
        break;
      case etPERCENT:
        buf[0] = '%';
        bufpt = buf;
        length = 1;
        break;
      case etCHARX:
        c = buf[0] = (xtype==etCHARX ? va_arg(ap,int) : *++fmt);
        if( precision>=0 ){
          for(idx=1; idx<precision; idx++) buf[idx] = c;
          length = precision;
        }else{
          length =1;
        }
        bufpt = buf;
        break;
      case etSTRING: {
        int limit = flag_alternateform ? va_arg(ap,int) : -1;
        bufpt = va_arg(ap,char*);
        if( bufpt==0 ){
          bufpt = "";
        }else if( xtype==etDYNSTRING ){
          zExtra = bufpt;
        }
        length = StrNLen32(bufpt, limit);
        if( precision>=0 && precision<length ) length = precision;
        break;
      }
      case etSTRESCAPE: {
        int i, j, n, ch, isnull;
        int limit = flag_alternateform ? va_arg(ap,int) : -1;
        char *escarg = va_arg(ap,char*);
        isnull = escarg==0;
        if( isnull ) escarg = "(NULL)";
        if( limit<0 ) limit = strlen(escarg);
        for(i=n=0; i<limit; i++){
          if( escarg[i]=='"' || escarg[i]=='\\' )  n++;
        }
        n += i + 1;
        if( n>etBUFSIZE ){
          bufpt = zExtra = xjd1_malloc( n );
        }else{
          bufpt = buf;
        }
        j = 0;
        for(i=0; i<limit; i++){
          ch = escarg[i];
          if( ch=='"' || ch=='\\' ) bufpt[j++] = '\\';
          bufpt[j++] = ch;
        }
        bufpt[j] = 0;
        length = j;
        if( precision>=0 && precision<length ) length = precision;
        break;
      }
      case etERROR:
        buf[0] = '%';
        buf[1] = c;
        errorflag = 0;
        idx = 1+(c!=0);
        xjd1StringAppend(pStr,"%",idx);
        count += idx;
        if( c==0 ) fmt--;
        break;
    }/* End switch over the format type */
    /*
    ** The text of the conversion is pointed to by "bufpt" and is
    ** "length" characters long.  The field width is "width".  Do
    ** the output.
    */
    if( !flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        count += nspace;
        while( nspace>=etSPACESIZE ){
          xjd1StringAppend(pStr,spaces,etSPACESIZE);
          nspace -= etSPACESIZE;
        }
        if( nspace>0 ) xjd1StringAppend(pStr,spaces,nspace);
      }
    }
    if( length>0 ){
      xjd1StringAppend(pStr,bufpt,length);
      count += length;
    }
    if( flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        count += nspace;
        while( nspace>=etSPACESIZE ){
          xjd1StringAppend(pStr,spaces,etSPACESIZE);
          nspace -= etSPACESIZE;
        }
        if( nspace>0 ) xjd1StringAppend(pStr,spaces,nspace);
      }
    }
    if( zExtra ){
      xjd1_free(zExtra);
    }
  }/* End for loop over the format string */
  return errorflag ? -1 : count;
} /* End of function */

/*
** Append formatted text to a string.  Return the number of bytes appended.
*/
PRIVATE int xjd1StringAppendF(String *pStr, const char *zFormat, ...){
  va_list ap;
  int rc;
  va_start(ap, zFormat);
  rc = xjd1StringVAppendF(pStr, zFormat, ap);
  va_end(ap);
  return rc;
}
