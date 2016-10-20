#ifdef SQ_USE_CBLOSC

#include "squirrel.h"
#include "sqstdblobimpl.h"
#include <stdio.h>
#include <string.h>
#include <blosc.h>

static SQRESULT sq_blosc_version(HSQUIRRELVM v)
{
    sq_pushstring(v, blosc_get_version_string(), -1);
    return 1;
}

static SQRESULT sq_blosc_init(HSQUIRRELVM v)
{
    blosc_init();
    return 0;
}

static SQRESULT sq_blosc_destroy(HSQUIRRELVM v)
{
    blosc_destroy();
    return 0;
}

static SQRESULT sq_blosc_free_resources(HSQUIRRELVM v)
{
    sq_pushinteger(v, blosc_free_resources());
    return 1;
}

static SQRESULT sq_blosc_list_compressors(HSQUIRRELVM v)
{
    sq_pushstring(v, blosc_list_compressors(), -1);
    return 1;
}

static SQRESULT sq_blosc_cbuffer_sizes(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, cbuffer);
    size_t nbytes, cbytes, blocksize;
    blosc_cbuffer_sizes((const void *)cbuffer, &nbytes, &cbytes, &blocksize);
    sq_newtable(v);

    sq_pushliteral(v, _SC("nbytes"));
    sq_pushinteger(v, nbytes);
    sq_rawset(v, -3);

    sq_pushliteral(v, _SC("cbytes"));
    sq_pushinteger(v, cbytes);
    sq_rawset(v, -3);

    sq_pushliteral(v, _SC("blocksize"));
    sq_pushinteger(v, blocksize);
    sq_rawset(v, -3);

    return 1;
}

static SQRESULT sq_blosc_cbuffer_metainfo(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, cbuffer);
    size_t typesize;
    int flags;
    blosc_cbuffer_metainfo((const void *)cbuffer, &typesize, &flags);
    sq_newtable(v);

    sq_pushliteral(v, _SC("typesize"));
    sq_pushinteger(v, typesize);
    sq_rawset(v, -3);

    sq_pushliteral(v, _SC("flags"));
    sq_pushinteger(v, flags);
    sq_rawset(v, -3);

    return 1;
}

static SQRESULT sq_blosc_cbuffer_complib(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, cbuffer);
    sq_pushstring(v, blosc_cbuffer_complib(cbuffer), -1);
    return 1;
}

static SQRESULT sq_blosc_blocksize(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    if(_top_ == 1)
    {
        sq_pushinteger(v, blosc_get_blocksize());
        return 1;
    }
    SQ_GET_INTEGER(v, 2, new_size);
    blosc_set_blocksize(new_size);
    return 0;
}

static SQRESULT sq_blosc_compcode_to_compname(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, compcode);
    char *compname;
    int rc = blosc_compcode_to_compname(compcode, &compname);
    if(rc > -1) sq_pushstring(v, compname, -1);
    else sq_pushnull(v);
    return 1;
}

static SQRESULT sq_blosc_compname_to_compcode(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, compname);
    sq_pushinteger(v, blosc_compname_to_compcode(compname));
    return 1;
}

static SQRESULT sq_blosc_compressor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    if(_top_ == 1) sq_pushstring(v, blosc_get_compressor(), -1);
    else
    {
        SQ_GET_STRING(v, 2, compname);
        sq_pushinteger(v, blosc_set_compressor(compname));
    }
    return 1;
}

static SQRESULT sq_blosc_nthreads(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    if(_top_ == 1) sq_pushinteger(v, blosc_get_nthreads());
    else
    {
        SQ_GET_INTEGER(v, 2, nthreads);
        sq_pushinteger(v, blosc_set_nthreads(nthreads));
    }
    return 1;
}

static SQRESULT sq_blosc_compress0(HSQUIRRELVM v, int isCtx)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, clevel);
    SQ_GET_INTEGER(v, 3, doshuffle);
    SQ_GET_INTEGER(v, 4, typesize);
    SQ_GET_STRING(v, 5, src);

    size_t destsize = src_size + BLOSC_MAX_OVERHEAD;
    SQChar *dest = sq_getscratchpad(v, destsize);
    int csize;
    if(isCtx)
    {
        SQ_GET_STRING(v, 6, compname);
        SQ_GET_INTEGER(v, 7, blocksize);
        SQ_GET_INTEGER(v, 8, nthreads);

        csize = blosc_compress_ctx(clevel, doshuffle, typesize,
				src_size, (const void *)src, (void *)dest, destsize,
                compname, blocksize, nthreads);
    }
    else
    {
        csize = blosc_compress(clevel, doshuffle, typesize,
				src_size, (const void *)src, (void *)dest, destsize);
    }

    if(csize > 0) sq_pushstring(v, dest, csize);
    else sq_pushinteger(v, csize);
    return 1;
}

static SQRESULT sq_blosc_compress(HSQUIRRELVM v)
{
    return sq_blosc_compress0(v, 0);
}

static SQRESULT sq_blosc_compress_ctx(HSQUIRRELVM v)
{
    return sq_blosc_compress0(v, 1);
}

static SQRESULT sq_blosc_decompress0(HSQUIRRELVM v, int isCtx)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, cbuffer);

    size_t nbytes, cbytes, blocksize;
    blosc_cbuffer_sizes((const void *)cbuffer, &nbytes, &cbytes, &blocksize);

    SQChar *dest = sq_getscratchpad(v, nbytes);
    int dsize;
    if(isCtx)
    {
        SQ_GET_INTEGER(v, 3, nthreads);
        dsize = blosc_decompress_ctx((const void *)cbuffer, (void *)dest, nbytes, nthreads);
    }
    else dsize = blosc_decompress((const void *)cbuffer, (void *)dest, nbytes);
    if(dsize > 0) sq_pushstring(v, dest, dsize);
    else sq_pushinteger(v, dsize);

    return 1;
}

static SQRESULT sq_blosc_decompress(HSQUIRRELVM v)
{
     return sq_blosc_decompress0(v, 0);
}

static SQRESULT sq_blosc_decompress_ctx(HSQUIRRELVM v)
{
     return sq_blosc_decompress0(v, 1);
}

static SQRESULT sq_blosc_getitem(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, cbuffer);
    SQ_GET_INTEGER(v, 3, start_pos)
    SQ_GET_INTEGER(v, 4, nitems)

    size_t typesize;
    int flags;
    blosc_cbuffer_metainfo((const void *)cbuffer, &typesize, &flags);

    SQChar *dest = sq_getscratchpad(v, nitems * typesize);

    int dsize = blosc_getitem((const void *)cbuffer, start_pos, nitems, (void *)dest);
    if(dsize > 0) sq_pushstring(v, dest, dsize);
    else sq_pushinteger(v, dsize);

    return 1;
}

#ifdef __cplusplus
extern "C" {
#endif
#define INT_CONST(v,num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);
#define STR_CONST(v,str) 	sq_pushstring(v,_SC(#str),-1);sq_pushstring(v,str, -1);sq_newslot(v,-3,SQTrue);


    SQRESULT sqext_register_sq_blosc(HSQUIRRELVM v)
    {
        sq_pushliteral(v,_SC("blosc"));
        sq_newtable(v);
        INT_CONST(v, BLOSC_MIN_HEADER_LENGTH);
        INT_CONST(v, BLOSC_MAX_OVERHEAD);
        INT_CONST(v, BLOSC_MAX_BUFFERSIZE);
        INT_CONST(v, BLOSC_MAX_TYPESIZE);
        INT_CONST(v, BLOSC_MAX_THREADS);
        INT_CONST(v, BLOSC_NOSHUFFLE);
        INT_CONST(v, BLOSC_SHUFFLE);
        INT_CONST(v, BLOSC_BITSHUFFLE);

        STR_CONST(v, BLOSC_BLOSCLZ_COMPNAME);
        STR_CONST(v, BLOSC_LZ4_COMPNAME);
        STR_CONST(v, BLOSC_LZ4HC_COMPNAME);
        STR_CONST(v, BLOSC_SNAPPY_COMPNAME);
        STR_CONST(v, BLOSC_ZLIB_COMPNAME);
        STR_CONST(v, BLOSC_ZSTD_COMPNAME);

        sq_insertfunc(v, _SC("version"), sq_blosc_version, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("init"), sq_blosc_init, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("destroy"), sq_blosc_destroy, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("free_resources"), sq_blosc_free_resources, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("list_compressors"), sq_blosc_list_compressors, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("cbuffer_sizes"), sq_blosc_cbuffer_sizes, 2, _SC(".s"), SQTrue);
        sq_insertfunc(v, _SC("cbuffer_metainfo"), sq_blosc_cbuffer_metainfo, 2, _SC(".s"), SQTrue);
        sq_insertfunc(v, _SC("cbuffer_complib"), sq_blosc_cbuffer_complib, 2, _SC(".s"), SQTrue);
        sq_insertfunc(v, _SC("compcode_to_compname"), sq_blosc_compcode_to_compname, 2, _SC(".i"), SQTrue);
        sq_insertfunc(v, _SC("compname_to_compcode"), sq_blosc_compname_to_compcode, 2, _SC(".s"), SQTrue);
        sq_insertfunc(v, _SC("blocksize"), sq_blosc_blocksize, -1, _SC(".i"), SQTrue);
        sq_insertfunc(v, _SC("compressor"), sq_blosc_compressor, -1, _SC(".s"), SQTrue);
        sq_insertfunc(v, _SC("nthreads"), sq_blosc_nthreads, -1, _SC(".i"), SQTrue);
        sq_insertfunc(v, _SC("compress"), sq_blosc_compress, 5, _SC(".iiis"), SQTrue);
        sq_insertfunc(v, _SC("compress_ctx"), sq_blosc_compress_ctx, 8, _SC(".iiissii"), SQTrue);
        sq_insertfunc(v, _SC("decompress"), sq_blosc_decompress, 2, _SC(".s"), SQTrue);
        sq_insertfunc(v, _SC("decompress_ctx"), sq_blosc_decompress_ctx, 3, _SC(".si"), SQTrue);
        sq_insertfunc(v, _SC("getitem"), sq_blosc_getitem, 4, _SC(".sii"), SQTrue);

        sq_newslot(v,-3,SQTrue); //push blosc table
        return 0;
    }

#ifdef __cplusplus
}
#endif

#endif // SQ_USE_CBLOSC
