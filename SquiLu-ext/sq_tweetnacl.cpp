#include <squirrel.h>
#include <stdint.h>
#include <string.h>
extern "C" {
#include "tweetnacl.h"
#include "randombytes.h"
}

typedef unsigned char* puc;
typedef const puc cpuc;

/* API: a = crypto_onetimeauth(m,k); */
static SQRESULT sq_crypto_onetimeauth(HSQUIRRELVM v) {
	const SQChar *m, *k;
	SQInteger msize = 0, ksize = 0;
	SQRESULT rc = 0;
	if((rc = sq_getstr_and_size(v,2, &m, &msize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &k, &ksize)) != SQ_OK) return rc;

	if (ksize != crypto_onetimeauth_KEYBYTES) return sq_throwerror(v, _SC("incorrect key length"));

	SQChar *buffer = sq_getscratchpad(v, crypto_onetimeauth_BYTES);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));
	crypto_onetimeauth((puc)buffer, (cpuc)m, msize, (cpuc)k);
	sq_pushstring(v, buffer, crypto_onetimeauth_BYTES);
	return 1;
}
/*
const char pycrypto_onetimeauth__doc__[]=
"crypto_onetimeauth(m,k) -> a\n\n\
The crypto_onetimeauth function authenticates a message m\n\
using a secret key k. The function returns an authenticator a.\n\
The authenticator length is always crypto_onetimeauth_BYTES.\n\
The function raises an exception if len(k) is not crypto_onetimeauth_KEYBYTES.\n\
This uses Poly1305.\n\
";
*/

/* API: crypto_onetimeauth_verify(a,m,k); */
static SQRESULT sq_crypto_onetimeauth_verify(HSQUIRRELVM v) {

	const SQChar *m, *k, *a;
	SQInteger msize = 0, ksize = 0, alen = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &a, &alen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &m, &msize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &k, &ksize)) != SQ_OK) return rc;

	if (alen != crypto_onetimeauth_BYTES) return sq_throwerror(v, _SC("incorrect authenticator length"));
	if (ksize != crypto_onetimeauth_KEYBYTES) return sq_throwerror(v, _SC("incorrect key length"));
	if (crypto_onetimeauth_verify((cpuc)a, (cpuc)m, msize, (cpuc)k) != 0) return sq_throwerror(v, _SC("invalid authenticator"));

	sq_pushbool(v, SQTrue);
	return 1;
}
/*
const char pycrypto_onetimeauth_verify__doc__[]=
"crypto_onetimeauth_verify(a,m,k)\n\n\
The crypto_onetimeauth_verify function checks that:\n\
  len(k) is crypto_onetimeauth_KEYBYTES;\n\
  len(a) is crypto_onetimeauth_BYTES;\n\
  and a is a correct authenticator of a message m under the secret key k.\n\
If any of these checks fail, the function raises an exception.\n\
This uses Poly1305.\n\
";
*/

/* API: h = crypto_hash(m); */
static SQRESULT sq_crypto_hash(HSQUIRRELVM v) {

	const SQChar *m;
	SQInteger msize = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &m, &msize)) != SQ_OK) return rc;

	SQChar *buffer = sq_getscratchpad(v, crypto_hash_BYTES);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	crypto_hash((puc)buffer, (cpuc)m, msize);

	sq_pushstring(v, buffer, crypto_hash_BYTES);
	return 1;
}
/*
const char pycrypto_hash__doc__[]=
"crypto_hash(m) -> h\n\n\
The crypto_hash function hashes a message m.\n\
It returns a hash h. The output length len(h) is always crypto_hash_BYTES.\n\
This uses SHA512.\n\
";
*/

/* API: crypto_verify_16(x,y); */
static SQRESULT sq_crypto_verify_16(HSQUIRRELVM v) {

	const SQChar *x, *y;
	SQInteger xlen = 0, ylen = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &x, &xlen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &y, &ylen)) != SQ_OK) return rc;

	if (xlen != crypto_verify_16_BYTES) return sq_throwerror(v, _SC("incorrect x-string length"));
	if (ylen != crypto_verify_16_BYTES) return sq_throwerror(v, _SC("incorrect y-string length"));
	if (crypto_verify_16((cpuc)x, (cpuc)y) != 0) return sq_throwerror(v, _SC("strings doesn't match"));

	sq_pushbool(v, SQTrue);
	return 1;
}
/*
const char pycrypto_verify_16__doc__[]=
"crypto_verify_16(x,y)\n\n\
The crypto_verify_16 function checks that:\n\
  len(x) is crypto_verify_16_BYTES;\n\
  len(y) is crypto_verify_16_BYTES;\n\
  and check if strings x and y has same content.\n\
If any of these checks fail, the function raises an exception.\n\
The time taken by crypto_verify_16 is independent of the contents of x and y.\n\
";
*/

/* API: crypto_verify_32(x,y); */
static SQRESULT sq_crypto_verify_32(HSQUIRRELVM v) {

	const SQChar *x, *y;
	SQInteger xlen = 0, ylen = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &x, &xlen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &y, &ylen)) != SQ_OK) return rc;

	if (xlen != crypto_verify_32_BYTES) return sq_throwerror(v, _SC("incorrect x-string length"));
	if (ylen != crypto_verify_32_BYTES) return sq_throwerror(v, _SC("incorrect y-string length"));
	if (crypto_verify_32((cpuc)x, (cpuc)y) != 0) return sq_throwerror(v, _SC("strings doesn't match"));

	sq_pushbool(v, SQTrue);
	return 1;
}
/*
const char pycrypto_verify_32__doc__[]=
"crypto_verify_32(x,y)\n\n\
The crypto_verify_32 function checks that:\n\
  len(x) is crypto_verify_32_BYTES;\n\
  len(y) is crypto_verify_32_BYTES;\n\
  and check if strings x and y has same content.\n\
If any of these checks fail, the function raises an exception.\n\
The time taken by crypto_verify_32 is independent of the contents of x and y.\n\
";
*/

/* API: crypto_scalarmult(n,p); */
static SQRESULT sq_crypto_scalarmult(HSQUIRRELVM v) {

	const SQChar *n, *p;
	SQInteger nlen = 0, plen = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &n, &nlen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &p, &plen)) != SQ_OK) return rc;

	if (nlen != crypto_scalarmult_SCALARBYTES) return sq_throwerror(v, _SC("incorrect scalar length"));
	if (plen != crypto_scalarmult_BYTES) return sq_throwerror(v, _SC("incorrect element length"));

	SQChar *buffer = sq_getscratchpad(v, crypto_scalarmult_BYTES);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	crypto_scalarmult((puc)buffer, (cpuc)n, (cpuc)p);

	sq_pushstring(v, buffer, crypto_scalarmult_BYTES);
	return 1;
}
/*
const char pycrypto_scalarmult__doc__[]=
"crypto_scalarmult(n,p) -> q\n\n\
This function multiplies a group element p by an integer n.\n\
It returns the resulting group element q of length crypto_scalarmult_BYTES.\n\
The function raises an exception if len(p) is not crypto_scalarmult_BYTES.\n\
It also raises an exception if len(n) is not crypto_scalarmult_SCALARBYTES.\n\
This uses Curve25519.\n\
";
*/

/* API: crypto_scalarmult_base(n); */
static SQRESULT sq_crypto_scalarmult_base(HSQUIRRELVM v) {

	const SQChar *n;
	SQInteger nlen = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &n, &nlen)) != SQ_OK) return rc;

	if (nlen != crypto_scalarmult_SCALARBYTES) return sq_throwerror(v, _SC("incorrect scalar length"));

	SQChar *buffer = sq_getscratchpad(v, crypto_scalarmult_BYTES);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	crypto_scalarmult_base((puc)buffer, (cpuc)n);
	sq_pushstring(v, buffer, crypto_scalarmult_BYTES);
	return 1;
}
/*
const char pycrypto_scalarmult_base__doc__[]=
"crypto_scalarmult_base(n,p) -> q\n\n\
The crypto_scalarmult_base function computes\n\
the scalar product of a standard group element and an integer n.\n\
It returns the resulting group element q of length crypto_scalarmult_BYTES.\n\
It raises an exception if len(n) is not crypto_scalarmult_SCALARBYTES.\n\
This uses Curve25519 and the standard base point '9'.\n\
";
*/

/* API: c = crypto_stream(clen,n,k); */
static SQRESULT sq_crypto_stream(HSQUIRRELVM v) {

	const SQChar *n, *k;
	SQInteger nsize = 0, ksize = 0, clen = 0;
	SQRESULT rc = 0;

	if((rc = sq_getinteger(v,2, &clen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &k, &ksize)) != SQ_OK) return rc;

	if (nsize != crypto_stream_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (ksize != crypto_stream_KEYBYTES) return sq_throwerror(v, _SC("incorrect key length"));
	if (clen < 0) return sq_throwerror(v, _SC("incorrect clen"));

	SQChar *buffer = sq_getscratchpad(v, clen);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	crypto_stream((puc)buffer, clen, (cpuc)n, (cpuc)k);

	sq_pushstring(v, buffer, clen);
	return 1;
}
/*
const char pycrypto_stream__doc__[]=
"crypto_stream(clen,n,k) -> c\n\n\
The crypto_stream function produces a clen-byte stream c\n\
as a function of a secret key k and a nonce n.\n\
The function raises an exception:\n\
  if len(k) is not crypto_stream_KEYBYTES;\n\
  if len(n) is not crypto_stream_NONCEBYTES;\n\
  if clen is smaller than 0.\n\
This uses XSalsa20, with a 24-byte nonce.\n\
";
*/

/* API: c = crypto_stream_xor(m,n,k); */
static SQRESULT sq_crypto_stream_xor(HSQUIRRELVM v) {

	const SQChar *m, *n, *k;
	SQInteger msize = 0, nsize = 0, ksize = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &m, &msize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &k, &ksize)) != SQ_OK) return rc;

	if (nsize != crypto_stream_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (ksize != crypto_stream_KEYBYTES) return sq_throwerror(v, _SC("incorrect key length"));

	SQChar *buffer = sq_getscratchpad(v, msize);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	crypto_stream_xor((puc)buffer, (cpuc)m, msize, (cpuc)n, (cpuc)k);
	sq_pushstring(v, buffer, msize);
	return 1;
}
/*
const char pycrypto_stream_xor__doc__[]=
"crypto_stream_xor(m,n,k) -> c\n\n\
The crypto_stream_xor function encrypts a message m using a secret key k\n\
and a nonce n. The crypto_stream_xor function returns the ciphertext c.\n\
The function raises an exception:\n\
  if len(k) is not crypto_stream_KEYBYTES;\n\
  if len(n) is not crypto_stream_NONCEBYTES.\n\
This uses XSalsa20, with a 24-byte nonce.\n\
";
*/

/* API: sm = crypto_sign(m,sk); */
static SQRESULT sq_crypto_sign(HSQUIRRELVM v) {

	SQInteger mlen = 0, sksize = 0;
	const SQChar *sk, *m;
	unsigned long long smlen;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &m, &mlen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &sk, &sksize)) != SQ_OK) return rc;

	if (sksize != crypto_sign_SECRETKEYBYTES) return sq_throwerror(v, _SC("incorrect secret-key length"));

	SQChar *buffer = sq_getscratchpad(v, mlen + crypto_sign_BYTES);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	if (crypto_sign((puc)buffer, &smlen, (cpuc)m, mlen, (cpuc)sk) != 0)
		return sq_throwerror(v, _SC("crypto_sign returns nonzero"));

	sq_pushstring(v, buffer, smlen);
	return 1;
}
/*
const char pycrypto_sign__doc__[]=
"crypto_sign(m,sk) -> sm\n\n\
The crypto_sign function signs a message m using the sender's secret key sk.\n\
The crypto_sign function returns the resulting signed message sm.\n\
The function raises an exception if len(sk) is not crypto_sign_SECRETKEYBYTES.\n\
This uses Ed25519.\n\
";
*/

/* API: m = crypto_sign_open(sm,pk); */
static SQRESULT sq_crypto_sign_open(HSQUIRRELVM v) {

	const SQChar *sm, *pk;
	SQInteger smlen=0, pksize=0;
	unsigned long long mlen;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &sm, &smlen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &pk, &pksize)) != SQ_OK) return rc;

	if (pksize != crypto_sign_PUBLICKEYBYTES) return sq_throwerror(v, _SC("incorrect public-key length"));

	SQChar *buffer = sq_getscratchpad(v, smlen);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	if (crypto_sign_open((puc)buffer, &mlen, (cpuc)sm, smlen, (cpuc)pk) != 0)
		return sq_throwerror(v, _SC("ciphertext fails verification"));

	sq_pushstring(v, buffer, mlen);
	return 1;
}
/*
const char pycrypto_sign_open__doc__[]=
"crypto_sign_open(sm,pk) -> m\n\n\
The crypto_sign_open function verifies the signature in\n\
sm using the receiver's secret key sk.\n\
The crypto_sign_open function returns the message m.\n\n\
If the signature fails verification, crypto_sign_open raises an exception.\n\
The function also raises an exception if len(pk) is not crypto_sign_PUBLICKEYBYTES.\n\
This uses Ed25519.\n\
";
*/

/* API: (pk,sk) = crypto_sign_keypair(); */
static SQRESULT sq_crypto_sign_keypair(HSQUIRRELVM v) {

    const SQChar *seed;
    SQInteger seedsize = 0;
    unsigned char  *pk, *sk;

    if(sq_gettop(v) > 1)
    {
        SQRESULT rc = 0;
	    if((rc = sq_getstr_and_size(v,2, &seed, &seedsize)) != SQ_OK) return rc;
	    if (seedsize != crypto_sign_SECRETKEYBYTES) return sq_throwerror(v, _SC("incorrect seed length"));
    }

    pk = (unsigned char *)sq_malloc(crypto_sign_PUBLICKEYBYTES);
    if (!pk) return sq_throwerror(v, _SC("could not allocate memory"));

    sk = (unsigned char *)sq_malloc(crypto_sign_SECRETKEYBYTES);
    if (!sk) {
        sq_free(pk, crypto_sign_PUBLICKEYBYTES);
        return sq_throwerror(v, _SC("could not allocate memory"));
    }

    if(seedsize) memcpy(sk, seed, seedsize);

    crypto_sign_keypair(pk, sk);
    sq_newarray(v, 2);
    sq_pushstring(v, (const SQChar*)pk, crypto_sign_PUBLICKEYBYTES);
    sq_arrayset(v, -2, 0);
    sq_pushstring(v, (const SQChar*)sk, crypto_sign_SECRETKEYBYTES);
    sq_arrayset(v, -2, 1);

    sq_free(pk, crypto_sign_PUBLICKEYBYTES);
    sq_free(sk, crypto_sign_SECRETKEYBYTES);

    return 1;
}
/*
const char pycrypto_sign_keypair__doc__[]=
"crypto_sign_keypair() -> (pk,sk)\n\n\
The crypto_sign_keypair function randomly generates a secret key and\n\
a corresponding public key. It returns tuple containing the secret key in sk and\n\
public key in pk.\n\
It guarantees that sk has crypto_sign_SECRETKEYBYTES bytes\n\
and that pk has crypto_sign_PUBLICKEYBYTES bytes.\n\
This uses Ed25519.\n\
";
*/

static SQRESULT sq_crypto_sign_keypair_from_seed(HSQUIRRELVM v) {

	return sq_crypto_sign_keypair(v);
}
/*
const char pycrypto_sign_keypair_from_seed__doc__[]=
"crypto_sign_keypair_from_seed(seed) -> (pk,sk)\n\n\
The crypto_sign_keypair_from_seed function generates a secret key and\n\
a corresponding public key from a user-provded seed. It returns\n\
a tuple containing the secret key in sk and\n\
public key in pk.\n\
It guarantees that sk has crypto_sign_SECRETKEYBYTES bytes\n\
and that pk has crypto_sign_PUBLICKEYBYTES bytes.\n\
This uses Ed25519.\n\
";
*/

/* API: c = crypto_secretbox(m,n,k); */
static SQRESULT sq_crypto_secretbox(HSQUIRRELVM v) {

	const SQChar *m, *n, *k;
	SQInteger msize = 0, nsize = 0, ksize = 0;
	SQInteger i;
	//    unsigned long long mlen;
	SQInteger mlen;
	unsigned char *mpad;
	unsigned char *cpad;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &m, &msize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &k, &ksize)) != SQ_OK) return rc;


	if (nsize != crypto_secretbox_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (ksize != crypto_secretbox_KEYBYTES) return sq_throwerror(v, _SC("incorrect key length"));

	mlen = msize + crypto_secretbox_ZEROBYTES;
	mpad = (unsigned char *)sq_malloc(mlen);
	if (!mpad) return sq_throwerror(v, _SC("could not allocate memory"));
	cpad = (unsigned char *)sq_malloc(mlen);
	if (!cpad) {
		sq_free(mpad, mlen);
		return sq_throwerror(v, _SC("could not allocate memory"));
	}

	for (i = 0; i < crypto_secretbox_ZEROBYTES; ++i) mpad[i] = 0;
	for (i = crypto_secretbox_ZEROBYTES; i < mlen; ++i) mpad[i] = m[i - crypto_secretbox_ZEROBYTES];

	crypto_secretbox(cpad, mpad, mlen, (cpuc)n, (cpuc)k);

	sq_pushstring(v, (const SQChar*)cpad + crypto_secretbox_BOXZEROBYTES, mlen - crypto_secretbox_BOXZEROBYTES);

	sq_free(mpad, mlen);
	sq_free(cpad, mlen);
	return 1;
}
/*
const char pycrypto_secretbox__doc__[]=
"crypto_secretbox(m,n,k) -> c\n\n\
The crypto_secretbox function encrypts and authenticates\n\
a message m using a secret key k and a nonce n. \n\
The crypto_secretbox function returns the resulting ciphertext c. \n\
The function raises an exception if len(k) is not crypto_secretbox_KEYBYTES.\n\
The function also raises an exception if len(n) is not crypto_secretbox_NONCEBYTES.\n\
This uses XSalsa20 (with a 24-byte nonce) and a 16-byte Poly1305 MAC.\n\
";
*/

/* API: m = crypto_secretbox_open(c,n,k); */
static SQRESULT sq_crypto_secretbox_open(HSQUIRRELVM v) {

	const SQChar *c, *n, *k;
	SQInteger csize = 0, nsize = 0, ksize = 0;
	long long i;
	SQInteger clen;
	unsigned char *mpad;
	unsigned char *cpad;
	SQRESULT rc = 1;

	if((rc = sq_getstr_and_size(v,2, &c, &csize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &k, &ksize)) != SQ_OK) return rc;

	if (nsize != crypto_secretbox_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (ksize != crypto_secretbox_KEYBYTES) return sq_throwerror(v, _SC("incorrect key length"));

	clen = csize + crypto_secretbox_BOXZEROBYTES;

	mpad = (unsigned char *)sq_malloc(clen);
	if (!mpad) return sq_throwerror(v, _SC("could not allocate memory"));
	cpad = (unsigned char *)sq_malloc(clen);
	if (!cpad) {
		sq_free(mpad, clen);
		return sq_throwerror(v, _SC("could not allocate memory"));
	}

	for (i = 0; i < crypto_secretbox_BOXZEROBYTES; ++i) cpad[i] = 0;
	for (i = crypto_secretbox_BOXZEROBYTES; i < clen; ++i) cpad[i] = c[i - crypto_secretbox_BOXZEROBYTES];

	if (crypto_secretbox_open(mpad, cpad, clen, (cpuc)n, (cpuc)k) != 0) {
		rc = sq_throwerror(v, _SC("ciphertext fails verification"));
		goto done;
	}
	if (clen < crypto_secretbox_ZEROBYTES) {
		rc =  sq_throwerror(v, _SC("ciphertext too short"));
		goto done;
	}

	sq_pushstring(v, (const SQChar*)mpad + crypto_secretbox_BOXZEROBYTES, clen - crypto_secretbox_BOXZEROBYTES);
	rc = 1;

done:
	sq_free(mpad, clen);
	sq_free(cpad, clen);
	return rc;
}
/*
const char pycrypto_secretbox_open__doc__[]=
"crypto_secretbox_open(c,n,k) -> m\n\n\
The crypto_secretbox_open function verifies and decrypts \n\
a ciphertext c using a secret key k and a nonce n.\n\
The crypto_secretbox_open function returns the resulting plaintext m.\n\
If the ciphertext fails verification, crypto_secretbox_open raises an exception.\n\
The function also raises an exception if len(k) is not crypto_secretbox_KEYBYTES,\n\
or if len(n) is not crypto_secretbox_NONCEBYTES.\n\
This uses XSalsa20 (with a 24-byte nonce) and a 16-byte Poly1305 MAC.\n\
";
*/

/* C API: c = crypto_box(m,n,pk,sk); */
static SQRESULT sq_crypto_box(HSQUIRRELVM v) {

	const SQChar *m, *n, *pk, *sk;
	SQInteger msize = 0, nsize = 0, pksize = 0, sksize = 0;
	long long i;
	SQInteger mlen;
	unsigned char *mpad;
	unsigned char *cpad;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &m, &msize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &pk, &pksize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,5, &sk, &sksize)) != SQ_OK) return rc;

	if (nsize != crypto_box_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (pksize != crypto_box_PUBLICKEYBYTES) return sq_throwerror(v, _SC("incorrect public-key length"));
	if (sksize != crypto_box_SECRETKEYBYTES) return sq_throwerror(v, _SC("incorrect secret-key length"));

	mlen = msize + crypto_box_ZEROBYTES;
	mpad = (unsigned char *)sq_malloc(mlen);
	if (!mpad) return sq_throwerror(v, _SC("could not allocate memory"));
	cpad = (unsigned char *)sq_malloc(mlen);
	if (!cpad) {
		sq_free(mpad, mlen);
		return sq_throwerror(v, _SC("could not allocate memory"));
	}

	for (i = 0; i < crypto_box_ZEROBYTES; ++i) mpad[i] = 0;
	for (i = crypto_box_ZEROBYTES; i < mlen; ++i) mpad[i] = m[i - crypto_box_ZEROBYTES];

	crypto_box(cpad, mpad, mlen, (cpuc)n, (cpuc)pk, (cpuc)sk);

	sq_pushstring(v, (const SQChar*)cpad + crypto_box_BOXZEROBYTES, mlen - crypto_box_BOXZEROBYTES);

	sq_free(mpad, mlen);
	sq_free(cpad, mlen);
	return 1;
}
/*
const char pycrypto_box__doc__[]=
"crypto_box(m,n,pk,sk) -> c\n\n\
The crypto_box function encrypts and authenticates a message m\n\
using the sender's secret key sk, the receiver's public key pk,\n\
and a nonce n. The crypto_box function returns the resulting ciphertext c.\n\
The function raises an exception if len(sk) is not crypto_box_SECRETKEYBYTES\n\
or if len(pk) is not crypto_box_PUBLICKEYBYTES\n\
or if len(n) is not crypto_box_NONCEBYTES.\n\
This uses Curve25519, XSalsa20 (with a 24-byte nonce) and a 16-byte Poly1305 MAC.\n\
";
*/

/* API: m = crypto_box_open(c,n,pk,sk); */
static SQRESULT sq_crypto_box_open(HSQUIRRELVM v) {

	const SQChar *c, *n, *pk, *sk;
	SQInteger csize = 0, nsize = 0, pksize = 0, sksize = 0;
	long long i;
	SQInteger clen;
	unsigned char *mpad;
	unsigned char *cpad;
	SQRESULT rc = 1;

	if((rc = sq_getstr_and_size(v,2, &c, &csize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &pk, &pksize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,5, &sk, &sksize)) != SQ_OK) return rc;

	if (nsize != crypto_box_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (pksize != crypto_box_PUBLICKEYBYTES) return sq_throwerror(v, _SC("incorrect public-key length"));
	if (sksize != crypto_box_SECRETKEYBYTES) return sq_throwerror(v, _SC("incorrect secret-key length"));

	clen = csize + crypto_box_BOXZEROBYTES;
	mpad = (unsigned char *)sq_malloc(clen);
	if (!mpad) return sq_throwerror(v, _SC("could not allocate memory"));
	cpad = (unsigned char *)sq_malloc(clen);
	if (!cpad) {
		sq_free(mpad, clen);
		return sq_throwerror(v, _SC("could not allocate memory"));
	}

	for (i = 0; i < crypto_box_BOXZEROBYTES; ++i) cpad[i] = 0;
	for (i = crypto_box_BOXZEROBYTES; i < clen; ++i) cpad[i] = c[i - crypto_box_BOXZEROBYTES];

	if (crypto_box_open(mpad, cpad, clen, (cpuc)n, (cpuc)pk, (cpuc)sk) != 0) {
		rc = sq_throwerror(v, _SC("ciphertext fails verification"));
		goto done;
	}
	if (clen < crypto_box_ZEROBYTES) {
		rc =  sq_throwerror(v, _SC("ciphertext too short"));
		goto done;
	}

	sq_pushstring(v, (const SQChar*)mpad + crypto_box_ZEROBYTES, clen - crypto_box_ZEROBYTES);
	rc = 1;

done:
	sq_free(mpad, clen);
	sq_free(cpad, clen);
	return rc;
}
/*
const char pycrypto_box_open__doc__[]=
"crypto_box_open(c,n,pk,sk) -> m\n\n\
The crypto_box_open function verifies and decrypts\n\
a ciphertext c using the receiver's secret key sk,\n\
the sender's public key pk, and a nonce n.\n\
The crypto_box_open function returns the resulting plaintext m.\n\
The function raises an exception if len(sk) is not crypto_box_SECRETKEYBYTES\n\
or if len(pk) is not crypto_box_PUBLICKEYBYTES\n\
or if len(n) is not crypto_box_NONCEBYTES.\n\
This uses Curve25519, XSalsa20 (with a 24-byte nonce) and a 16-byte Poly1305 MAC.\n\
";
*/

/* API: (pk,sk) = crypto_box_keypair(); */
static SQRESULT sq_crypto_box_keypair(HSQUIRRELVM v) {

	SQChar *pk, *sk;

	pk = (SQChar*)sq_malloc(crypto_box_PUBLICKEYBYTES);
	if (!pk) return sq_throwerror(v, _SC("could not allocate memory"));
	sk = (SQChar *)sq_malloc(crypto_box_SECRETKEYBYTES);
	if (!sk) {
		sq_free(pk, crypto_box_PUBLICKEYBYTES);
		return sq_throwerror(v, _SC("could not allocate memory"));
	}

	crypto_box_keypair((puc)pk, (puc)sk);

	sq_newarray(v, 2);
	sq_pushstring(v, pk, crypto_box_PUBLICKEYBYTES);
	sq_arrayset(v, -2, 0);
	sq_pushstring(v, sk, crypto_box_SECRETKEYBYTES);
	sq_arrayset(v, -2, 1);

	sq_free(pk, crypto_box_PUBLICKEYBYTES);
	sq_free(sk, crypto_box_SECRETKEYBYTES);

	return 1;
}
/*
const char pycrypto_box_keypair__doc__[]=
"crypto_box_keypair() -> (pk,sk)\n\n\
The crypto_box_keypair function randomly generates a secret key and\n\
a corresponding public key. It returns tuple containing the secret key in sk and\n\
public key in pk.\n\
It guarantees that sk has crypto_box_SECRETKEYBYTES bytes\n\
and that pk has crypto_box_PUBLICKEYBYTES bytes.\n\
This uses Curve25519.\n\
";
*/

/* API: s = crypto_box_beforenm(pk,sk); */
static SQRESULT sq_crypto_box_beforenm(HSQUIRRELVM v) {

	const SQChar *pk, *sk, *ret;
	SQInteger pklen=0, sklen=0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &pk, &pklen)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &sk, &sklen)) != SQ_OK) return rc;

	if (pklen != crypto_box_PUBLICKEYBYTES) return sq_throwerror(v, _SC("incorrect public-key length"));
	if (sklen != crypto_box_SECRETKEYBYTES) return sq_throwerror(v, _SC("incorrect secret-key length"));

	ret = sq_getscratchpad(v, crypto_box_BEFORENMBYTES);
	if (!ret) return sq_throwerror(v, _SC("could not allocate memory"));

	crypto_box_beforenm((puc)ret, (cpuc)pk, (cpuc)sk);

	sq_pushstring(v, ret, crypto_box_BEFORENMBYTES);

	return 1;
}
/*
const char pycrypto_box_beforenm__doc__[]=
"crypto_box_beforenm(pk,sk) -> s\n\n\
Function crypto_box_beforenm computes a shared secret s from \n\
public key pk and secret key sk\n\
The function raises an exception if len(sk) is not crypto_box_SECRETKEYBYTES.\n\
It also raises an exception if len(pk) is not crypto_box_PUBLICKEYBYTES.\n\
This uses Curve25519, XSalsa20 (with a 24-byte nonce) and a 16-byte Poly1305 MAC.\n\
";
*/

/* API: c = crypto_box_afternm(m,n,k); */
static SQRESULT sq_crypto_box_afternm(HSQUIRRELVM v) {

	const SQChar *m, *n, *k;
	SQInteger msize = 0, nsize = 0, ksize = 0;
	long long i;
	SQInteger mlen;
	unsigned char *mpad;
	unsigned char *cpad;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &m, &msize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &k, &ksize)) != SQ_OK) return rc;

	if (nsize != crypto_box_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (ksize != crypto_box_BEFORENMBYTES) return sq_throwerror(v, _SC("incorrect key length"));

	mlen = msize + crypto_box_ZEROBYTES;
	mpad = (unsigned char *)sq_malloc(mlen);
	if (!mpad) return sq_throwerror(v, _SC("could not allocate memory"));
	cpad = (unsigned char *)sq_malloc(mlen);
	if (!cpad) {
		sq_free(mpad, mlen);
		return sq_throwerror(v, _SC("could not allocate memory"));
	}

	for (i = 0; i < crypto_box_ZEROBYTES; ++i) mpad[i] = 0;
	for (i = crypto_box_ZEROBYTES; i < mlen; ++i) mpad[i] = m[i - crypto_box_ZEROBYTES];

	crypto_box_afternm(cpad, mpad, mlen, (cpuc)n, (cpuc)k);

	sq_pushstring(v, (const SQChar*)cpad + crypto_box_BOXZEROBYTES, mlen - crypto_box_BOXZEROBYTES);

	sq_free(mpad, mlen);
	sq_free(cpad, mlen);
	return 1;
}
/*
const char pycrypto_box_afternm__doc__[]=
"crypto_box_afternm(m,n,k) -> c\n\n\
The crypto_box_afternm function encrypts and authenticates\n\
a message m using a secret key k and a nonce n. \n\
The crypto_box_afternm function returns the resulting ciphertext c. \n\
The function raises an exception if len(k) is not crypto_box_BEFORENMBYTES.\n\
The function also raises an exception if len(n) is not crypto_box_NONCEBYTES.\n\
This uses Curve25519, XSalsa20 (with a 24-byte nonce) and a 16-byte Poly1305 MAC.\n\
";
*/

/* API: m = crypto_box_open_afternm(c,n,k); */
static SQRESULT sq_crypto_box_open_afternm(HSQUIRRELVM v) {

	const SQChar *c, *n, *k;
	SQInteger csize = 0, nsize = 0, ksize = 0;
	long long i;
	SQInteger clen;
	unsigned char *mpad;
	unsigned char *cpad;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &c, &csize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,3, &n, &nsize)) != SQ_OK) return rc;
	if((rc = sq_getstr_and_size(v,4, &k, &ksize)) != SQ_OK) return rc;

	if (nsize != crypto_box_NONCEBYTES) return sq_throwerror(v, _SC("incorrect nonce length"));
	if (ksize != crypto_box_BEFORENMBYTES) return sq_throwerror(v, _SC("incorrect key length"));

	clen = csize + crypto_box_BOXZEROBYTES;
	mpad = (unsigned char *)sq_malloc(clen);
	if (!mpad) return sq_throwerror(v, _SC("could not allocate memory"));
	cpad = (unsigned char *)sq_malloc(clen);
	if (!cpad) {
		sq_free(mpad, clen);
		return sq_throwerror(v, _SC("could not allocate memory"));
	}

	for (i = 0; i < crypto_box_BOXZEROBYTES; ++i) cpad[i] = 0;
	for (i = crypto_box_BOXZEROBYTES; i < clen; ++i) cpad[i] = c[i - crypto_box_BOXZEROBYTES];

	if (crypto_box_open_afternm(mpad, cpad, clen, (cpuc)n, (cpuc)k) != 0) {
		rc = sq_throwerror(v, _SC("ciphertext fails verification"));
		goto done;
	}
	if (clen < crypto_box_ZEROBYTES) {
		rc =  sq_throwerror(v, _SC("ciphertext too short"));
		goto done;
	}

	sq_pushstring(v, (const SQChar*)mpad + crypto_box_ZEROBYTES, clen - crypto_box_ZEROBYTES);
    rc = 1;

done:
	sq_free(mpad, clen);
	sq_free(cpad, clen);
	return rc;
}
/*
const char pycrypto_box_open_afternm__doc__[]=
"crypto_box_open_afternm(c,n,k) -> m\n\n\
The crypto_box_open_afternm function verifies and decrypts \n\
a ciphertext c using a secret key k and a nonce n.\n\
The crypto_box_open_afternm function returns the resulting plaintext m.\n\
If the ciphertext fails verification, crypto_box_open_afternm raises an exception.\n\
The function also raises an exception if len(k) is not crypto_box_BEFORENMBYTES,\n\
or if len(n) is not crypto_box_NONCEBYTES.\n\
This uses Curve25519, XSalsa20 (with a 24-byte nonce) and a 16-byte Poly1305 MAC.\n\
";
*/

static SQRESULT sq_crypto_spk2epk(HSQUIRRELVM v) {

	const SQChar *spk;
	SQInteger spksize = 0;
	SQRESULT rc = 0;

	if((rc = sq_getstr_and_size(v,2, &spk, &spksize)) != SQ_OK) return rc;

	if (spksize != crypto_sign_PUBLICKEYBYTES) return sq_throwerror(v, _SC("incorrect public key length"));

	SQChar *buffer = sq_getscratchpad(v, crypto_box_PUBLICKEYBYTES);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	spk2epk((unsigned char *)buffer, (unsigned char *)spk);

	sq_pushstring(v, buffer, crypto_box_PUBLICKEYBYTES);
	return 1;
}
/*
const char pyspk2epk__doc__[]=
"spk2epk(spk) -> epk\n\n\
The spk2epk function converts an ed25519 key (signing public key)\n\
 to a corresponding curve25519 public key (encryption public key).";
*/

static SQRESULT sq_crypto_randombytes(HSQUIRRELVM v) {

	SQInteger rsize = 0;
	SQRESULT rc = 0;

	if((rc = sq_getinteger(v,2, &rsize)) != SQ_OK) return rc;

	SQChar *buffer = sq_getscratchpad(v, rsize);
	if (!buffer) return sq_throwerror(v, _SC("could not allocate memory"));

	randombytes((unsigned char *)buffer, rsize);

	sq_pushstring(v, buffer, rsize);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC("crypto_" #name),  sq_crypto_##name,nparams,tycheck}
static SQRegFunction sq_tweetnacl_methods[] =
{
    _DECL_FUNC(onetimeauth,  3, _SC(".ss")),
    _DECL_FUNC(onetimeauth_verify,  4, _SC(".sss")),
    _DECL_FUNC(hash,  2, _SC(".s")),
    _DECL_FUNC(verify_16,  3, _SC(".ss")),
    _DECL_FUNC(verify_32,  3, _SC(".ss")),
    _DECL_FUNC(scalarmult,  3, _SC(".ss")),
    _DECL_FUNC(scalarmult_base,  3, _SC(".ss")),
    _DECL_FUNC(stream,  4, _SC(".iss")),
    _DECL_FUNC(stream_xor,  4, _SC(".sss")),
    _DECL_FUNC(sign,  3, _SC(".ss")),
    _DECL_FUNC(sign_open,  3, _SC(".ss")),
    _DECL_FUNC(sign_keypair,  1, _SC(".")),
    _DECL_FUNC(sign_keypair_from_seed,  2, _SC(".s")),
    _DECL_FUNC(secretbox,  4, _SC(".sss")),
    _DECL_FUNC(secretbox_open,  4, _SC(".sss")),
    _DECL_FUNC(box,  5, _SC(".ssss")),
    _DECL_FUNC(box_open,  5, _SC(".ssss")),
    _DECL_FUNC(box_keypair,  1, _SC(".")),
    _DECL_FUNC(box_beforenm,  3, _SC(".ss")),
    _DECL_FUNC(box_afternm,  4, _SC(".sss")),
    _DECL_FUNC(box_open_afternm,  4, _SC(".sss")),
    _DECL_FUNC(spk2epk,  1, _SC(".s")),
    _DECL_FUNC(randombytes,  2, _SC(".i")),
    {0,0}
};

#undef _DECL_FUNC

#define INT_CONST(v,num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);
#define STR_CONST(v,str) 	sq_pushstring(v,_SC(#str),-1);sq_pushliteral(v,str);sq_newslot(v,-3,SQTrue);

static void add_constants(HSQUIRRELVM v) {
    STR_CONST(v, crypto_onetimeauth_PRIMITIVE);
    STR_CONST(v, crypto_onetimeauth_IMPLEMENTATION);
    STR_CONST(v, crypto_onetimeauth_VERSION);
    INT_CONST(v, crypto_onetimeauth_BYTES);
    INT_CONST(v, crypto_onetimeauth_KEYBYTES);
    STR_CONST(v, crypto_hash_PRIMITIVE);
    STR_CONST(v, crypto_hash_IMPLEMENTATION);
    STR_CONST(v, crypto_hash_VERSION);
    INT_CONST(v, crypto_hash_BYTES);
    INT_CONST(v, crypto_verify_16_BYTES);
    STR_CONST(v, crypto_verify_16_IMPLEMENTATION);
    STR_CONST(v, crypto_verify_16_VERSION);
    INT_CONST(v, crypto_verify_32_BYTES);
    STR_CONST(v, crypto_verify_32_IMPLEMENTATION);
    STR_CONST(v, crypto_verify_32_VERSION);
    STR_CONST(v, crypto_scalarmult_PRIMITIVE);
    STR_CONST(v, crypto_scalarmult_IMPLEMENTATION);
    STR_CONST(v, crypto_scalarmult_VERSION);
    INT_CONST(v, crypto_scalarmult_BYTES);
    INT_CONST(v, crypto_scalarmult_SCALARBYTES);
    STR_CONST(v, crypto_stream_PRIMITIVE);
    STR_CONST(v, crypto_stream_IMPLEMENTATION);
    STR_CONST(v, crypto_stream_VERSION);
    INT_CONST(v, crypto_stream_KEYBYTES);
    INT_CONST(v, crypto_stream_NONCEBYTES);
    STR_CONST(v, crypto_sign_PRIMITIVE);
    STR_CONST(v, crypto_sign_IMPLEMENTATION);
    STR_CONST(v, crypto_sign_VERSION);
    INT_CONST(v, crypto_sign_BYTES);
    INT_CONST(v, crypto_sign_PUBLICKEYBYTES);
    INT_CONST(v, crypto_sign_SECRETKEYBYTES);
    STR_CONST(v, crypto_secretbox_PRIMITIVE);
    STR_CONST(v, crypto_secretbox_IMPLEMENTATION);
    STR_CONST(v, crypto_secretbox_VERSION);
    INT_CONST(v, crypto_secretbox_KEYBYTES);
    INT_CONST(v, crypto_secretbox_NONCEBYTES);
    INT_CONST(v, crypto_secretbox_ZEROBYTES);
    INT_CONST(v, crypto_secretbox_BOXZEROBYTES);
    STR_CONST(v, crypto_box_PRIMITIVE);
    STR_CONST(v, crypto_box_IMPLEMENTATION);
    STR_CONST(v, crypto_box_VERSION);
    INT_CONST(v, crypto_box_PUBLICKEYBYTES);
    INT_CONST(v, crypto_box_SECRETKEYBYTES);
    INT_CONST(v, crypto_box_BEFORENMBYTES);
    INT_CONST(v, crypto_box_NONCEBYTES);
    INT_CONST(v, crypto_box_ZEROBYTES);
    INT_CONST(v, crypto_box_BOXZEROBYTES);
    return;
}

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_tweetnacl(HSQUIRRELVM v)
    {
        sq_pushstring(v,_SC("tweetnacl"),-1);
        sq_newtable(v);
	add_constants(v);
	sq_insert_reg_funcs(v, sq_tweetnacl_methods);

        sq_rawset(v,-3);//insert tweetnacl
        return 1;
    }

#ifdef __cplusplus
}
#endif
