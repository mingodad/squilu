/*
* lbase64.c
* base64 encoding and decoding for Lua 5.1
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 27 Jun 2007 19:04:40
* This code is hereby placed in the public domain.
* Ported to Squrrel by Domingo Alvarez Duarte
*/

#include "squirrel.h"
#include <string.h>
#include "sqstdblobimpl.h"

#define MYNAME		_SC("base64")
#define MYVERSION	_SC(MYNAME " library for " LUA_VERSION " / Jun 2007")

#define uint unsigned int

static const char code[]=
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void encode(SQBlob &b, uint c1, uint c2, uint c3, int n)
{
    unsigned long tuple=c3+256UL*(c2+256UL*c1);
    int i;
    char s[4];
    for (i=0; i<4; i++)
    {
        s[3-i] = code[tuple % 64];
        tuple /= 64;
    }
    for (i=n+1; i<4; i++) s[i]='=';
    b.Write(s,4);
}

static SQRESULT base64_encode(HSQUIRRELVM v)		/** encode(s) */
{
    SQInteger l;
    const unsigned char *s;
    SQRESULT rc;
    if((rc = sq_getstr_and_size(v,2, (const char **)&s, &l)) != SQ_OK) return rc;
    SQBlob b(0, l*2);
    int n;
    for (n=l/3; n--; s+=3) encode(b,s[0],s[1],s[2],3);
    switch (l%3)
    {
    case 1:
        encode(b,s[0],0,0,1);
        break;
    case 2:
        encode(b,s[0],s[1],0,2);
        break;
    }
    sq_pushstring(v, (SQChar*)b.GetBuf(), b.Len());
    return 1;
}

static void decode(SQBlob &b, int c1, int c2, int c3, int c4, int n)
{
    unsigned long tuple=c4+64L*(c3+64L*(c2+64L*c1));
    char s[3];
    switch (--n)
    {
    case 3:
        s[2]=tuple;
    case 2:
        s[1]=tuple >> 8;
    case 1:
        s[0]=tuple >> 16;
    }
    b.Write(s,n);
}

static SQRESULT base64_decode(HSQUIRRELVM v)		/** decode(s) */
{
    SQInteger l;
    const unsigned char *s;
    SQRESULT rc;
    if((rc = sq_getstr_and_size(v,2, (const char **)&s, &l)) != SQ_OK) return rc;
    SQBlob b(0, l*2);
    int n=0;
    char t[4];
    for (;;)
    {
        int c=*s++;
        switch (c)
        {
            const char *p;
        default:
            p=strchr(code,c);
            if (p==NULL) return 0;
            t[n++]= p-code;
            if (n==4)
            {
                decode(b,t[0],t[1],t[2],t[3],4);
                n=0;
            }
            break;
        case '=':
            switch (n)
            {
            case 1:
                decode(b,t[0],0,0,0,1);
                break;
            case 2:
                decode(b,t[0],t[1],0,0,2);
                break;
            case 3:
                decode(b,t[0],t[1],t[2],0,3);
                break;
            }
        case 0:
            sq_pushstring(v, (SQChar*)b.GetBuf(), b.Len());
            return 1;
        case '\n':
        case '\r':
        case '\t':
        case ' ':
        case '\f':
        case '\b':
            break;
        }
    }
    return 0;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),base64_##name,nparams,tycheck}
static SQRegFunction base64_methods[] =
{
    _DECL_FUNC(encode,2,_SC(".s")),
    _DECL_FUNC(decode,2,_SC(".s")),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_base64(HSQUIRRELVM v)
{
    sq_pushstring(v,_SC("base64"),-1);
    sq_newtable(v);
    sq_insert_reg_funcs(v, base64_methods);
    sq_newslot(v,-3,SQTrue);
    return 1;
}

#ifdef __cplusplus
}
#endif
