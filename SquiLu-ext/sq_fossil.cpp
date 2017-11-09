#include "squirrel.h"
#include "sqstdblobimpl.h"

#define MixInteger SQInteger

#include "sqlite3.h"
#define USE_MG_MD5
#include "mongoose.h"
#include "fossil-delta.h"

static SQRESULT sq_fossil_delta_create(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, str_from);
    SQ_GET_STRING(v, 3, str_to);
    SQChar *delta = sq_getscratchpad(v, str_to_size + DELTA_OVERFLOW);
    int delta_size = delta_create(str_from, str_from_size, str_to, str_to_size, delta);
    sq_pushstring(v, delta, delta_size);
    return 1;
}

static SQRESULT sq_fossil_delta_apply(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, str_from);
    SQ_GET_STRING(v, 3, str_delta);
    int str_to_size = delta_output_size(str_delta, str_delta_size);
    if(str_to_size < 0) return sq_throwerror(v, _SC("invalid delta string"));
    SQChar *str_to = sq_getscratchpad(v, str_to_size);
    str_to_size = delta_apply(str_from, str_from_size, str_delta, str_delta_size, str_to);
    if(str_to_size < 0) return sq_throwerror(v, _SC("error applying delta"));
    sq_pushstring(v, str_to, str_to_size);
    return 1;
}

static SQRESULT sq_fossil_delta_output_size(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, str_delta);
    int str_to_size = delta_output_size(str_delta, str_delta_size);
    sq_pushinteger(v, str_to_size);
    return 1;
}

static SQRESULT sq_fossil_delta_analyze(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, str_delta);
    int pnCopy, pnInsert;
    int rc = delta_analyze(str_delta, str_delta_size, &pnCopy, &pnInsert);
    if(rc < 0) return sq_throwerror(v, _SC("invalid delta string"));
    sq_newarray(v, 2);
    sq_pushinteger(v, pnCopy);
    sq_arrayset(v, -2, 0);
    sq_pushinteger(v, pnInsert);
    sq_arrayset(v, -2, 1);
    return 1;
}

extern "C" {
typedef sqlite3_uint64 u64;
/*
** State structure for a SHA3 hash in progress
*/
typedef struct SHA3Context SHA3Context;
struct SHA3Context {
  union {
    u64 s[25];                /* Keccak state. 5x5 lines of 64 bits each */
    unsigned char x[1600];    /* ... or 1600 bytes */
  } u;
  unsigned nRate;        /* Bytes of input accepted per Keccak iteration */
  unsigned nLoaded;      /* Input bytes loaded into u.x[] so far this cycle */
  unsigned ixMask;       /* Insert next input into u.x[nLoaded^ixMask]. */
};
void xsqlite3_SHA3Init(SHA3Context*, int);
void xsqlite3_SHA3Update(SHA3Context*, const unsigned char*, unsigned);
const unsigned char* xsqlite3_SHA3Final(SHA3Context*);

#define SHA3Init xsqlite3_SHA3Init
#define SHA3Update xsqlite3_SHA3Update
#define SHA3Final xsqlite3_SHA3Final

/* Context for the SHA1 hash */
typedef struct SHA1Context SHA1Context;
struct SHA1Context {
  unsigned int state[5];
  unsigned int count[2];
  unsigned char buffer[64];
};
void xsqlite3_SHA1Init(SHA1Context*);
void xsqlite3_SHA1Update(SHA1Context*, const unsigned char*, unsigned);
void xsqlite3_SHA1Final(SHA1Context*, char*);

#define SHA1Init xsqlite3_SHA1Init
#define SHA1Update xsqlite3_SHA1Update
#define SHA1Final xsqlite3_SHA1Final
}


/*
** Convert a digest into base-16.  digest should be declared as
** "unsigned char digest[20]" in the calling function.  The SHA3
** digest is stored in the first 20 bytes.  zBuf should
** be "char zBuf[41]".
*/
static void DigestToBase16(const unsigned char *digest, char *zBuf, int nByte){
  static const char zEncode[] = "0123456789abcdef";
  int ix;

  for(ix=0; ix<nByte; ix++){
    *zBuf++ = zEncode[(*digest>>4)&0xf];
    *zBuf++ = zEncode[*digest++ & 0xf];
  }
  *zBuf = '\0';
}

static SQRESULT sq_fossil_sha3sum(HSQUIRRELVM v) {
	SQ_FUNC_VARS(v);
	SQ_GET_INTEGER(v, 2, iSize);
	switch(iSize)
	{
    case 0:
        iSize = 256;
    case 224:
    case 256:
    case 384:
    case 512:
        break;
    default:
        return sq_throwerror(v, _SC("invalid hash size expected one of [0, 224, 256, 384, 512]"));
	}

    SHA3Context ctx;
    SHA3Init(&ctx, iSize);

    for (int i = 3; i <= _top_; ++i) {
        SQ_GET_STRING(v, i, p);
        SHA3Update(&ctx, (const unsigned char *) p, p_size);
    }

    SQInteger buf_size = iSize/4;
    SQChar *buf = sq_getscratchpad(v, buf_size);
    DigestToBase16(SHA3Final(&ctx), buf, buf_size/2);
    sq_pushstring(v, buf, buf_size);
    return 1;
}

static SQRESULT sq_fossil_sha1sum(HSQUIRRELVM v) {
	SQ_FUNC_VARS(v);

    const int hash_size = 20;
    const int buf_size = hash_size*2;
    char buf[buf_size+1];
    SHA1Context ctx;
    SHA1Init(&ctx);

    for (int i = 2; i <= _top_; ++i) {
        SQ_GET_STRING(v, i, p);
        SHA1Update(&ctx, (const unsigned char *) p, p_size);
    }
    SHA1Final(&ctx, buf);
    sq_pushstring(v, buf, buf_size);
    return 1;
}

static SQRESULT sq_fossil_md5sum(HSQUIRRELVM v) {
	SQ_FUNC_VARS(v);

    const int hash_size = 16;
    const int buf_size = hash_size*2;
    char buf[buf_size+1];
    unsigned char hash[hash_size];
    MD5_CTX ctx;
    MD5Init(&ctx);

    for (int i = 2; i <= _top_; ++i) {
        SQ_GET_STRING(v, i, p);
        MD5Update(&ctx, (const unsigned char *) p, p_size);
    }
    MD5Final(hash, &ctx);
    DigestToBase16(hash, buf, hash_size);
    sq_pushstring(v, buf, buf_size);
    return 1;
}

#define _DECL_FUNC(name,nparams,pmask) {_SC(#name), sq_fossil_##name,nparams,pmask}
static SQRegFunction fossil_obj_funcs[]={
	_DECL_FUNC(delta_create,3, _SC(".ss")),
	_DECL_FUNC(delta_apply,3, _SC(".ss")),
	_DECL_FUNC(delta_output_size,2, _SC(".s")),
	_DECL_FUNC(delta_analyze,2, _SC(".s")),
	_DECL_FUNC(sha3sum,-3, _SC(".is")),
	_DECL_FUNC(sha1sum,-2, _SC(".s")),
	_DECL_FUNC(md5sum,-2, _SC(".s")),
	{0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif
/* This defines a function that opens up your library. */
SQRESULT sqext_register_fossil (HSQUIRRELVM sqvm) {
    //add a namespace sqmix
	sq_pushstring(sqvm,_SC("sqfossil"),-1);
	sq_newclass(sqvm,SQFalse);
    sq_insert_reg_funcs(sqvm, fossil_obj_funcs);
	sq_newslot(sqvm,-3,SQTrue); //add sq_fossil table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}
#endif
