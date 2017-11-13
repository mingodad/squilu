#ifdef USE_BITVECTOR

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#ifdef _WIN32
#include <malloc.h>  /* for alloc */
#endif
#include <assert.h>  /* for a few sanity tests */

#include "sqlite3.h"

//copy from sqliteInt.h
#ifndef UINT32_TYPE
# ifdef HAVE_UINT32_T
#  define UINT32_TYPE uint32_t
# else
#  define UINT32_TYPE unsigned int
# endif
#endif
#ifndef UINT16_TYPE
# ifdef HAVE_UINT16_T
#  define UINT16_TYPE uint16_t
# else
#  define UINT16_TYPE unsigned short int
# endif
#endif
#ifndef INT16_TYPE
# ifdef HAVE_INT16_T
#  define INT16_TYPE int16_t
# else
#  define INT16_TYPE short int
# endif
#endif
#ifndef UINT8_TYPE
# ifdef HAVE_UINT8_T
#  define UINT8_TYPE uint8_t
# else
#  define UINT8_TYPE unsigned char
# endif
#endif
#ifndef INT8_TYPE
# ifdef HAVE_INT8_T
#  define INT8_TYPE int8_t
# else
#  define INT8_TYPE signed char
# endif
#endif
#ifndef LONGDOUBLE_TYPE
# define LONGDOUBLE_TYPE long double
#endif
//typedef sqlite_int64 i64;          /* 8-byte signed integer */
//typedef sqlite_uint64 u64;         /* 8-byte unsigned integer */
typedef UINT32_TYPE u32;           /* 4-byte unsigned integer */
typedef UINT16_TYPE u16;           /* 2-byte unsigned integer */
typedef INT16_TYPE i16;            /* 2-byte signed integer */
typedef UINT8_TYPE u8;             /* 1-byte unsigned integer */
typedef INT8_TYPE i8;              /* 1-byte signed integer */

void *sqlite3MallocZero(size_t size)
{
    void *p = sq_malloc(size);
    memset(p, 0, size);
    return p;
}

/*
** SQLITE_MAX_U32 is a u64 constant that is the maximum u64 value
** that can be stored in a u32 without loss of data.  The value
** is 0x00000000ffffffff.  But because of quirks of some compilers, we
** have to specify the value in the less intuitive manner shown:
*/
#define SQLITE_MAX_U32  ((((u64)1)<<32)-1)

/*
** On systems with ample stack space and that support alloca(), make
** use of alloca() to obtain space for large automatic objects.  By default,
** obtain space from malloc().
**
** The alloca() routine never returns NULL.  This will cause code paths
** that deal with sqlite3StackAlloc() failures to be unreachable.
*/
# define sqlite3StackAllocRaw(D,N)   alloca(N)
# define sqlite3StackAllocZero(D,N)  memset(alloca(N), 0, N)
# define sqlite3StackFree(D,P)

# define SQLITE_NOMEM_BKPT SQLITE_NOMEM
# define SQLITE_IOERR_NOMEM_BKPT SQLITE_IOERR_NOMEM

/*
** 2008 February 16
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file implements an object that represents a fixed-length
** bitmap.  Bits are numbered starting with 1.
**
** A bitmap is used to record which pages of a database file have been
** journalled during a transaction, or which pages have the "dont-write"
** property.  Usually only a few pages are meet either condition.
** So the bitmap is usually sparse and has low cardinality.
** But sometimes (for example when during a DROP of a large table) most
** or all of the pages in a database can get journalled.  In those cases,
** the bitmap becomes dense with high cardinality.  The algorithm needs
** to handle both cases well.
**
** The size of the bitmap is fixed when the object is created.
**
** All bits are clear when the bitmap is created.  Individual bits
** may be set or cleared one at a time.
**
** Test operations are about 100 times more common that set operations.
** Clear operations are exceedingly rare.  There are usually between
** 5 and 500 set operations per Bitvec object, though the number of sets can
** sometimes grow into tens of thousands or larger.  The size of the
** Bitvec object is the number of pages in the database file at the
** start of a transaction, and is thus usually less than a few thousand,
** but can be as large as 2 billion for a really big database.
*/
//#include "sqliteInt.h"

/* Size of the Bitvec structure in bytes. */
#define BITVEC_SZ        512

/* Round the union size down to the nearest pointer boundary, since that's how
** it will be aligned within the Bitvec struct. */
#define BITVEC_USIZE \
    (((BITVEC_SZ-(3*sizeof(u32)))/sizeof(Bitvec*))*sizeof(Bitvec*))

/* Type of the array "element" for the bitmap representation.
** Should be a power of 2, and ideally, evenly divide into BITVEC_USIZE.
** Setting this to the "natural word" size of your CPU may improve
** performance. */
#define BITVEC_TELEM     u8
/* Size, in bits, of the bitmap element. */
#define BITVEC_SZELEM    8
/* Number of elements in a bitmap array. */
#define BITVEC_NELEM     (BITVEC_USIZE/sizeof(BITVEC_TELEM))
/* Number of bits in the bitmap array. */
#define BITVEC_NBIT      (BITVEC_NELEM*BITVEC_SZELEM)

/* Number of u32 values in hash table. */
#define BITVEC_NINT      (BITVEC_USIZE/sizeof(u32))
/* Maximum number of entries in hash table before
** sub-dividing and re-hashing. */
#define BITVEC_MXHASH    (BITVEC_NINT/2)
/* Hashing function for the aHash representation.
** Empirical testing showed that the *37 multiplier
** (an arbitrary prime)in the hash function provided
** no fewer collisions than the no-op *1. */
#define BITVEC_HASH(X)   (((X)*1)%BITVEC_NINT)

#define BITVEC_NPTR      (BITVEC_USIZE/sizeof(Bitvec *))


/*
** A bitmap is an instance of the following structure.
**
** This bitmap records the existence of zero or more bits
** with values between 1 and iSize, inclusive.
**
** There are three possible representations of the bitmap.
** If iSize<=BITVEC_NBIT, then Bitvec.u.aBitmap[] is a straight
** bitmap.  The least significant bit is bit 1.
**
** If iSize>BITVEC_NBIT and iDivisor==0 then Bitvec.u.aHash[] is
** a hash table that will hold up to BITVEC_MXHASH distinct values.
**
** Otherwise, the value i is redirected into one of BITVEC_NPTR
** sub-bitmaps pointed to by Bitvec.u.apSub[].  Each subbitmap
** handles up to iDivisor separate values of i.  apSub[0] holds
** values between 1 and iDivisor.  apSub[1] holds values between
** iDivisor+1 and 2*iDivisor.  apSub[N] holds values between
** N*iDivisor+1 and (N+1)*iDivisor.  Each subbitmap is normalized
** to hold deal with values between 1 and iDivisor.
*/
struct Bitvec {
  u32 iSize;      /* Maximum bit index.  Max iSize is 4,294,967,296. */
  u32 nSet;       /* Number of bits that are set - only valid for aHash
                  ** element.  Max is BITVEC_NINT.  For BITVEC_SZ of 512,
                  ** this would be 125. */
  u32 iDivisor;   /* Number of bits handled by each apSub[] entry. */
                  /* Should >=0 for apSub element. */
                  /* Max iDivisor is max(u32) / BITVEC_NPTR + 1.  */
                  /* For a BITVEC_SZ of 512, this would be 34,359,739. */
  union {
    BITVEC_TELEM aBitmap[BITVEC_NELEM];    /* Bitmap representation */
    u32 aHash[BITVEC_NINT];      /* Hash table representation */
    Bitvec *apSub[BITVEC_NPTR];  /* Recursive representation */
  } u;
};

/*
** Create a new bitmap object able to handle bits between 0 and iSize,
** inclusive.  Return a pointer to the new object.  Return NULL if
** malloc fails.
*/
static Bitvec *sqlite3BitvecCreate(u32 iSize){
  Bitvec *p;
  assert( sizeof(*p)==BITVEC_SZ );
  p = (Bitvec*)sqlite3MallocZero( sizeof(*p) );
  if( p ){
    p->iSize = iSize;
  }
  return p;
}

/*
** Check to see if the i-th bit is set.  Return true or false.
** If p is NULL (if the bitmap has not been created) or if
** i is out of range, then return false.
*/
static int sqlite3BitvecTestNotNull(Bitvec *p, u32 i){
  assert( p!=0 );
  i--;
  if( i>=p->iSize ) return 0;
  while( p->iDivisor ){
    u32 bin = i/p->iDivisor;
    i = i%p->iDivisor;
    p = p->u.apSub[bin];
    if (!p) {
      return 0;
    }
  }
  if( p->iSize<=BITVEC_NBIT ){
    return (p->u.aBitmap[i/BITVEC_SZELEM] & (1<<(i&(BITVEC_SZELEM-1))))!=0;
  } else{
    u32 h = BITVEC_HASH(i++);
    while( p->u.aHash[h] ){
      if( p->u.aHash[h]==i ) return 1;
      h = (h+1) % BITVEC_NINT;
    }
    return 0;
  }
}
static int sqlite3BitvecTest(Bitvec *p, u32 i){
  return p!=0 && sqlite3BitvecTestNotNull(p,i);
}

/*
** Set the i-th bit.  Return 0 on success and an error code if
** anything goes wrong.
**
** This routine might cause sub-bitmaps to be allocated.  Failing
** to get the memory needed to hold the sub-bitmap is the only
** that can go wrong with an insert, assuming p and i are valid.
**
** The calling function must ensure that p is a valid Bitvec object
** and that the value for "i" is within range of the Bitvec object.
** Otherwise the behavior is undefined.
*/
static int sqlite3BitvecSet(Bitvec *p, u32 i){
  u32 h;
  if( p==0 ) return SQLITE_OK;
  assert( i>0 );
  assert( i<=p->iSize );
  i--;
  while((p->iSize > BITVEC_NBIT) && p->iDivisor) {
    u32 bin = i/p->iDivisor;
    i = i%p->iDivisor;
    if( p->u.apSub[bin]==0 ){
      p->u.apSub[bin] = sqlite3BitvecCreate( p->iDivisor );
      if( p->u.apSub[bin]==0 ) return SQLITE_NOMEM_BKPT;
    }
    p = p->u.apSub[bin];
  }
  if( p->iSize<=BITVEC_NBIT ){
    p->u.aBitmap[i/BITVEC_SZELEM] |= 1 << (i&(BITVEC_SZELEM-1));
    return SQLITE_OK;
  }
  h = BITVEC_HASH(i++);
  /* if there wasn't a hash collision, and this doesn't */
  /* completely fill the hash, then just add it without */
  /* worring about sub-dividing and re-hashing. */
  if( !p->u.aHash[h] ){
    if (p->nSet<(BITVEC_NINT-1)) {
      goto bitvec_set_end;
    } else {
      goto bitvec_set_rehash;
    }
  }
  /* there was a collision, check to see if it's already */
  /* in hash, if not, try to find a spot for it */
  do {
    if( p->u.aHash[h]==i ) return SQLITE_OK;
    h++;
    if( h>=BITVEC_NINT ) h = 0;
  } while( p->u.aHash[h] );
  /* we didn't find it in the hash.  h points to the first */
  /* available free spot. check to see if this is going to */
  /* make our hash too "full".  */
bitvec_set_rehash:
  if( p->nSet>=BITVEC_MXHASH ){
    unsigned int j;
    int rc;
    u32 *aiValues = (u32*)sqlite3StackAllocRaw(0, sizeof(p->u.aHash));
    if( aiValues==0 ){
      return SQLITE_NOMEM_BKPT;
    }else{
      memcpy(aiValues, p->u.aHash, sizeof(p->u.aHash));
      memset(p->u.apSub, 0, sizeof(p->u.apSub));
      p->iDivisor = (p->iSize + BITVEC_NPTR - 1)/BITVEC_NPTR;
      rc = sqlite3BitvecSet(p, i);
      for(j=0; j<BITVEC_NINT; j++){
        if( aiValues[j] ) rc |= sqlite3BitvecSet(p, aiValues[j]);
      }
      sqlite3StackFree(0, aiValues);
      return rc;
    }
  }
bitvec_set_end:
  p->nSet++;
  p->u.aHash[h] = i;
  return SQLITE_OK;
}

/*
** Clear the i-th bit.
**
** pBuf must be a pointer to at least BITVEC_SZ bytes of temporary storage
** that BitvecClear can use to rebuilt its hash table.
*/
static void sqlite3BitvecClear(Bitvec *p, u32 i, void *pBuf){
  if( p==0 ) return;
  assert( i>0 );
  i--;
  while( p->iDivisor ){
    u32 bin = i/p->iDivisor;
    i = i%p->iDivisor;
    p = p->u.apSub[bin];
    if (!p) {
      return;
    }
  }
  if( p->iSize<=BITVEC_NBIT ){
    p->u.aBitmap[i/BITVEC_SZELEM] &= ~(1 << (i&(BITVEC_SZELEM-1)));
  }else{
    unsigned int j;
    u32 *aiValues = (u32*)pBuf;
    memcpy(aiValues, p->u.aHash, sizeof(p->u.aHash));
    memset(p->u.aHash, 0, sizeof(p->u.aHash));
    p->nSet = 0;
    for(j=0; j<BITVEC_NINT; j++){
      if( aiValues[j] && aiValues[j]!=(i+1) ){
        u32 h = BITVEC_HASH(aiValues[j]-1);
        p->nSet++;
        while( p->u.aHash[h] ){
          h++;
          if( h>=BITVEC_NINT ) h = 0;
        }
        p->u.aHash[h] = aiValues[j];
      }
    }
  }
}

/*
** Destroy a bitmap object.  Reclaim all memory used.
*/
static void sqlite3BitvecDestroy(Bitvec *p){
  if( p==0 ) return;
  if( p->iDivisor ){
    unsigned int i;
    for(i=0; i<BITVEC_NPTR; i++){
      sqlite3BitvecDestroy(p->u.apSub[i]);
    }
  }
  sq_free(p, 0);
}

/*
** Return the value of the iSize parameter specified when Bitvec *p
** was created.
*/
static u32 sqlite3BitvecSize(Bitvec *p){
  return p->iSize;
}

/*
** Return the value of the BITVEC_SZ.
*/
static u32 sqlite3BITVEC_SZ(){
  return BITVEC_SZ;
}


//SQ_OPT_STRING_STRLEN();

static const SQChar SQ_LIBNAME[] = _SC("BitVector");

static const SQChar BitVector_Tag[]   = _SC("BitVector_TAG");
#define GET_BitVector_INSTANCE() SQ_GET_INSTANCE(v, 1, Bitvec, BitVector_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("BitVector object already closed"));


static SQRESULT BitVector_release_hook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	Bitvec *self = (Bitvec*)p;
	if(self) sqlite3BitvecDestroy(self);
	return 0;
}

/*
static SQRESULT BitVector_free(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	GET_BitVector_INSTANCE();
	BitVector_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0);
	return 0;
}
*/

static SQRESULT sq_BitVector_constructor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_INTEGER(v, 2, int_size);
//	Bitvec *sqlite3BitvecCreate(u32)
    Bitvec *bv = sqlite3BitvecCreate((u32)int_size);
    SQInteger rc = sq_setinstanceup(v, 1, bv);
    sq_setreleasehook(v,1, BitVector_release_hook);
	return rc;
}

#define BV_CHECK_RANGE() \
    	if(int_pos < 1 || int_pos > self->iSize) return sq_throwerror(v, _SC("index out of range '%u'"), (u32)int_pos);
static SQRESULT sq_BitVector_clear(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_BitVector_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_pos);
	BV_CHECK_RANGE();
//	void sqlite3BitvecClear(Bitvec*, u32, void*)
    SQChar *bv_buf = sq_getscratchpad(v, sqlite3BITVEC_SZ());

    sqlite3BitvecClear(self, (u32)int_pos, bv_buf);
	return 0;
}

static SQRESULT sq_BitVector_set(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_BitVector_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_pos);
	BV_CHECK_RANGE();
//	int sqlite3BitvecSet(Bitvec*, u32)
    sq_pushinteger(v, sqlite3BitvecSet(self, (u32)int_pos));
	return 1;
}

static SQRESULT sq_BitVector_size(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_BitVector_INSTANCE();

//	u32 sqlite3BitvecSize(Bitvec*)
    sq_pushinteger(v, sqlite3BitvecSize(self));
	return 1;
}

static SQRESULT sq_BitVector_test(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_BitVector_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_pos);
	BV_CHECK_RANGE();
//	int sqlite3BitvecTest(Bitvec*, u32)
    sq_pushinteger(v, sqlite3BitvecTest(self, (u32)int_pos));
	return 1;
}

static SQRESULT sq_BitVector_test_not_null(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_BitVector_INSTANCE();

	SQ_GET_INTEGER(v, 2, int_pos);
	BV_CHECK_RANGE();
//	int sqlite3BitvecTestNotNull(Bitvec*, u32)
    sq_pushinteger(v, sqlite3BitvecTestNotNull(self, (u32)int_pos));
	return 1;
}

#define _DECL_BITVECTOR_FUNC(name,nparams,pmask) {_SC(#name),sq_BitVector_##name,nparams,pmask}
static SQRegFunction BitVector_obj_funcs[]={

	_DECL_BITVECTOR_FUNC(constructor, 2, _SC("xi")),
	_DECL_BITVECTOR_FUNC(clear, 2, _SC("xi")),
	_DECL_BITVECTOR_FUNC(set, 2, _SC("xi")),
	_DECL_BITVECTOR_FUNC(size, 1, _SC("x")),
	_DECL_BITVECTOR_FUNC(test, 2, _SC("xi")),
	_DECL_BITVECTOR_FUNC(test_not_null, 2, _SC("xi")),
	{0,0}
};
#undef _DECL_BITVECTOR_FUNC

extern "C" {

    /* This defines a function that opens up your library. */
    SQRESULT sqext_register_BitVector (HSQUIRRELVM v) {
        //add a namespace BitVector
        sq_pushstring(v, SQ_LIBNAME, -1);
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(SQUserPointer)BitVector_Tag);

        sq_insert_reg_funcs(v, BitVector_obj_funcs);

        sq_newslot(v,-3,SQFalse); //add BitVector table to the root table

        return SQ_OK;
    }
}

#endif //USE_BITVECTOR
