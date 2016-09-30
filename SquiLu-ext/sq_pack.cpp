/*
* lpack.c
* a Lua library for packing and unpacking binary data
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 29 Jun 2007 19:27:20
* This code is hereby placed in the public domain.
* with contributions from Ignacio Castaño <castanyo@yahoo.es> and
* Roberto Ierusalimschy <roberto@inf.puc-rio.br>.
*
* Ported to SquiLu by Domingo Alvarez Duarte mingodad@gmail.com on 21/09/2016
*/

#define	OP_ZSTRING	'z'		/* zero-terminated string */
#define	OP_BSTRING	'p'		/* string preceded by length byte */
#define	OP_WSTRING	'P'		/* string preceded by length word */
#define	OP_SSTRING	'a'		/* string preceded by length size_t */
#define	OP_STRING	'A'		/* string */
#define	OP_FLOAT		'f'		/* float */
#define	OP_DOUBLE	'd'		/* double */
#define	OP_CHAR		'c'		/* char */
#define	OP_BYTE		'b'		/* byte = unsigned char */
#define	OP_SHORT	'h'		/* short */
#define	OP_USHORT	'H'		/* unsigned short */
#define	OP_INT		'i'		/* int */
#define	OP_UINT		'I'		/* unsigned int */
#define	OP_LONG		'l'		/* long */
#define	OP_ULONG	'L'		/* unsigned long */
#define	OP_LITTLEENDIAN	'<'		/* little endian */
#define	OP_BIGENDIAN	'>'		/* big endian */
#define	OP_NATIVE	'='		/* native endian */

#include <ctype.h>
#include <string.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

SQ_OPT_STRING_STRLEN();

static SQRESULT badcode(HSQUIRRELVM v, int c)
{
	SQChar s[]=_SC("bad code '?'");
	s[sizeof(s)-(3*sizeof(SQChar))]=c;
	return sq_throwerror(v,s);
}

static int doendian(int c)
{
	int x=1;
	int e=*(char*)&x;
	if (c==OP_LITTLEENDIAN) return !e;
	if (c==OP_BIGENDIAN) return e;
	if (c==OP_NATIVE) return 0;
	return 0;
}

static void doswap(int swap, void *p, size_t n)
{
	if (swap)
	{
		char *a=(char*)p;
		int i,j;
		for (i=0, j=n-1, n=n/2; n--; i++, j--)
		{
			char t=a[i]; a[i]=a[j]; a[j]=t;
		}
	}
}

#define UNPACKNUMBER_T(OP,T, SQ_P, SQ_T)	\
	case OP: \
	{ \
		T a; \
		int m=sizeof(a); \
		if (i+m>s_size) goto done; \
		memcpy(&a,s+i,m); \
		i+=m; \
		doswap(swap,&a,m); \
		SQ_P(v,(SQ_T)a); \
		++n; \
		break; \
	}

#define UNPACKFLOAT(OP, T) UNPACKNUMBER_T(OP, T, sq_pushfloat, SQFloat)
#define UNPACKINTEGER(OP, T) UNPACKNUMBER_T(OP, T, sq_pushinteger, SQInteger)

#define UNPACKSTRING(OP,T) \
	case OP: \
	{ \
		T l; \
		int m=sizeof(l); \
		if (i+m>s_size) goto done; \
		memcpy(&l,s+i,m); \
		doswap(swap,&l,m); \
		if (i+m+l>s_size) goto done; \
		i+=m; \
		sq_pushstring(v,s+i,l); \
		i+=l; \
		++n; \
		break; \
	}

static SQRESULT sq_unpack(HSQUIRRELVM v) 		/** unpack(s,f,[init]) */
{
	SQ_FUNC_VARS(v);
	SQ_GET_STRING(v, 2, s);
	SQ_GET_STRING(v, 3, f);
	SQ_OPT_INTEGER(v,4, i, 0);
	SQInteger n=0;
	int swap=0;
	sq_pushnull(v);
	while (*f)
	{
		int c=*f++;
		int N=1;
		if (isdigit(*f))
		{
			N=0;
			while (isdigit(*f)) N=10*N+(*f++)-'0';
			if (N==0 && c==OP_STRING) { sq_pushliteral(v,_SC("")); ++n; }
		}
		while (N--) switch (c)
		{
			case OP_LITTLEENDIAN:
			case OP_BIGENDIAN:
			case OP_NATIVE:
			{
				swap=doendian(c);
				N=0;
				break;
			}
			case OP_STRING:
			{
				++N;
				if (i+N>s_size) goto done;
				sq_pushstring(v,s+i,N);
				i+=N;
				++n;
				N=0;
				break;
			}
			case OP_ZSTRING:
			{
				size_t l;
				if (i>=s_size) goto done;
				l=strlen(s+i);
				sq_pushstring(v,s+i,l);
				i+=l+1;
				++n;
				break;
			}
			UNPACKSTRING(OP_BSTRING, unsigned char)
			UNPACKSTRING(OP_WSTRING, unsigned short)
			UNPACKSTRING(OP_SSTRING, size_t)
			UNPACKFLOAT(OP_DOUBLE, double)
			UNPACKFLOAT(OP_FLOAT, float)
			UNPACKINTEGER(OP_CHAR, char)
			UNPACKINTEGER(OP_BYTE, unsigned char)
			UNPACKINTEGER(OP_SHORT, short)
			UNPACKINTEGER(OP_USHORT, unsigned short)
			UNPACKINTEGER(OP_INT, int)
			UNPACKINTEGER(OP_UINT, unsigned int)
			UNPACKINTEGER(OP_LONG, long)
			UNPACKINTEGER(OP_ULONG, unsigned long)
			case ' ': case ',':
			break;
			default:
				return badcode(v,c);
			break;
		}
	}
done:
	SQInteger nelm = sq_gettop(v) - _top_ - 1; //-1 because first sq_pushnull
	if(nelm > 0)
	{
		SQInteger arraypos = _top_ +1;
		sq_newarray(v, nelm);
		sq_replace(v, arraypos);
		for(SQInteger i=nelm-1; i >= 0; --i)
		{
			sq_arrayset(v, arraypos, i);
		}
	}
	return 1;
}

#define PACKINTEGER(OP,T) \
	case OP: \
	{ \
		SQ_GET_INTEGER(v, i++, inum); \
		T a=(T)inum; \
		doswap(swap,&a,sizeof(a)); \
		b.Write((void*)&a,sizeof(a)); \
		break; \
	}

#define PACKFLOAT(OP,T) \
	case OP: \
	{ \
		SQ_GET_FLOAT(v, i++, fnum); \
		T a=(T)fnum; \
		doswap(swap,&a,sizeof(a)); \
		b.Write((void*)&a,sizeof(a)); \
		break; \
	}

#define PACKSTRING(OP,T) \
	case OP: \
	{ \
		SQ_GET_STRING(v, i++, str); \
		T ll=(T)str_size; \
		doswap(swap,&ll,sizeof(ll)); \
		b.Write((void*)&ll,sizeof(ll)); \
		b.Write(str, str_size); \
		break; \
	}

static SQRESULT sq_pack(HSQUIRRELVM v) 		/** pack(f,...) */
{
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, f);
	int i=3;
	int swap=0;
	SQBlob b(0, 8192);
	while (*f)
	{
		int c=*f++;
		int N=1;
		if (isdigit(*f))
		{
			N=0;
			while (isdigit(*f)) N=10*N+(*f++)-'0';
		}
		while (N--) switch (c)
		{
			case OP_LITTLEENDIAN:
			case OP_BIGENDIAN:
			case OP_NATIVE:
			{
				swap=doendian(c);
				N=0;
				break;
			}
			case OP_STRING:
			case OP_ZSTRING:
			{
				SQ_GET_STRING(v, i++, str);
				b.Write(str, str_size+(c==OP_ZSTRING));
				break;
			}
			PACKSTRING(OP_BSTRING, unsigned char)
			PACKSTRING(OP_WSTRING, unsigned short)
			PACKSTRING(OP_SSTRING, size_t)
			PACKFLOAT(OP_DOUBLE, double)
			PACKFLOAT(OP_FLOAT, float)
			PACKINTEGER(OP_CHAR, char)
			PACKINTEGER(OP_BYTE, unsigned char)
			PACKINTEGER(OP_SHORT, short)
			PACKINTEGER(OP_USHORT, unsigned short)
			PACKINTEGER(OP_INT, int)
			PACKINTEGER(OP_UINT, unsigned int)
			PACKINTEGER(OP_LONG, long)
			PACKINTEGER(OP_ULONG, unsigned long)
			case ' ': case ',':
			break;
			default:
				return badcode(v,c);
			break;
		}
	}
	sq_pushstring(v, (SQChar*)b.GetBuf(), b.Len());
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_##name,nparams,tycheck}
static SQRegFunction sq_pack_methods[] =
{
    _DECL_FUNC(pack,-3,_SC(".s.")),
    _DECL_FUNC(unpack,-3,_SC(".ssi")),
    {0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_pack(HSQUIRRELVM v)
{
    sq_pushstring(v,_SC("sqpack"),-1);
    sq_newtable(v);
    sq_insert_reg_funcs(v, sq_pack_methods);
    sq_newslot(v,-3,SQTrue);
    return 0;
}

#ifdef __cplusplus
}
#endif
