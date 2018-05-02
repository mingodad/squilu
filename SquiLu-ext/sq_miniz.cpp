#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "squirrel.h"
#include "sqstdblobimpl.h"
SQ_OPT_STRING_STRLEN();

#define LZ_BUFFER_SIZE 8192

#include "vogl_miniz_zip.h"

static const SQChar sq_miniz_ziparchive_TAG[] = _SC("sq_miniz_ziparchive_tag");

#define GET_miniz_ziparchive_INSTANCE_VAR_AT(idx, Var) \
    SQ_GET_INSTANCE_VAR(v, idx, mz_zip_archive, Var, sq_miniz_ziparchive_TAG);\
    if(!Var) return sq_throwerror(v, _SC("miniz_ziparchive already destroyed"));
#define GET_miniz_ziparchive_INSTANCE() GET_miniz_ziparchive_INSTANCE_VAR_AT(1, self)

static SQRESULT sq_miniz_ziparchive_releasehook(SQUserPointer p, SQInteger size, void *v)
{
	mz_zip_archive *self = ((mz_zip_archive *)p);
	if(self) {
	    mz_zip_reader_end(self);
	    sq_free(self, sizeof(mz_zip_archive));
	}
	return 0;
}

static SQRESULT sq_miniz_ziparchive_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, zip_fname);
    mz_zip_archive *zip_archive = (mz_zip_archive*)sq_malloc(sizeof(mz_zip_archive));
    memset(zip_archive, 0, sizeof(mz_zip_archive));
    if(!mz_zip_reader_init_file(zip_archive, zip_fname, 0, 0, 0))
    {
        return sq_throwerror(v, _SC("mz_zip_reader_init_file() failed!"));
    }
    sq_setinstanceup(v, 1, zip_archive);
    sq_setreleasehook(v,1, sq_miniz_ziparchive_releasehook);
	return 1;
}

static SQRESULT sq_miniz_ziparchive_get_num_files(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_miniz_ziparchive_INSTANCE();
    sq_pushinteger(v, mz_zip_get_num_files(self));
	return 1;
}

static SQRESULT sq_miniz_ziparchive_file_stat(HSQUIRRELVM v, mz_zip_archive_file_stat *file_stat)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_miniz_ziparchive_INSTANCE();
    SQ_GET_INTEGER(v, 2, idx);
    if (!mz_zip_file_stat(self, idx, file_stat))
    {
        return sq_throwerror(v, _SC("mz_zip_file_stat() failed!"));
    }
	return SQ_OK;
}

static SQRESULT sq_miniz_ziparchive_get_file_name(HSQUIRRELVM v)
{
    mz_zip_archive_file_stat file_stat;
    SQRESULT rc = sq_miniz_ziparchive_file_stat(v, &file_stat);
    if(rc != SQ_OK) return rc;
    sq_pushstring(v, file_stat.m_filename, -1);
	return 1;
}

static SQRESULT sq_miniz_ziparchive_get_file_size(HSQUIRRELVM v)
{
    mz_zip_archive_file_stat file_stat;
    SQRESULT rc = sq_miniz_ziparchive_file_stat(v, &file_stat);
    if(rc != SQ_OK) return rc;
    sq_pushinteger(v, file_stat.m_uncomp_size);
	return 1;
}

static SQRESULT sq_miniz_ziparchive_is_file_a_directory(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_miniz_ziparchive_INSTANCE();
    SQ_GET_INTEGER(v, 2, idx);
    sq_pushbool(v, mz_zip_is_file_a_directory(self, idx));
	return 1;
}

static SQRESULT sq_miniz_ziparchive_extract_file(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_miniz_ziparchive_INSTANCE();
    SQ_GET_INTEGER(v, 2, idx);
    size_t uncomp_size;
    void *p = mz_zip_extract_to_heap(self, idx, &uncomp_size, 0);
    if (!p)
    {
        return sq_throwerror(v, _SC("mz_zip_extract_to_heap() failed!"));
    }
    sq_pushstring(v, (const SQChar*)p, uncomp_size);
    MZ_FREE(p);
	return 1;
}

static SQRESULT sq_miniz_ziparchive_destroy(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_miniz_ziparchive_INSTANCE();
    int rc = sq_miniz_ziparchive_releasehook(self, 0, v);
    if(rc == 0) sq_setinstanceup(v, 1, 0);
    sq_pushinteger(v, rc);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_miniz_ziparchive_##name,nparams,tycheck}
static SQRegFunction sq_miniz_ziparchive_methods[] =
{
	_DECL_FUNC(constructor,  2, _SC("xs")),
	_DECL_FUNC(get_num_files,  1, _SC("x")),
	_DECL_FUNC(get_file_name,  2, _SC("xi")),
	_DECL_FUNC(get_file_size,  2, _SC("xi")),
	_DECL_FUNC(is_file_a_directory,  2, _SC("xi")),
	_DECL_FUNC(extract_file,  2, _SC("xi")),
	_DECL_FUNC(destroy,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC


static SQRESULT sq_check_result(HSQUIRRELVM v, int result, const mz_stream_s* stream) {
    /* Both of these are "normal" return codes: */
    if ( result == MZ_OK || result == MZ_STREAM_END ) return SQ_OK;
    switch ( result ) {
    case MZ_NEED_DICT:
        return sq_throwerror(v, _SC("RequiresDictionary: input stream requires a dictionary to be deflated (%s)"),
                        stream->msg);
        break;
    case MZ_STREAM_ERROR:
        return sq_throwerror(v, _SC("InternalError: inconsistent internal zlib stream (%s)"),
                        stream->msg);
        break;
    case MZ_DATA_ERROR:
        return sq_throwerror(v, _SC("InvalidInput: input string does not conform to zlib format or checksum"));
        break;
    case MZ_MEM_ERROR:
        return sq_throwerror(v, _SC("OutOfMemory: not enough memory (%s)"), stream->msg);
        break;
    case MZ_BUF_ERROR:
        return sq_throwerror(v, _SC("InternalError: no progress possible (%s)"), stream->msg);
        break;
    case MZ_VERSION_ERROR:
        return sq_throwerror(v, _SC("IncompatibleLibrary: built with version %s, but dynamically linked with version %s (%s)"),
                        MZ_VERSION,  mz_version(), stream->msg);
        break;
    default:
        return sq_throwerror(v, _SC("ZLibError: unknown code %d (%s)"), result, stream->msg);
    }
    return SQ_OK;
}

static SQRESULT sq_miniz_deflate(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, data);
    SQ_OPT_INTEGER(v, 3, level, MZ_DEFAULT_COMPRESSION);
    /*  Allocate the stream: */
    mz_stream_s stream;

    stream.zalloc = NULL;
    stream.zfree  = NULL;
    if(sq_check_result(v, mz_deflateInit(&stream, level), &stream) != SQ_OK) return SQ_ERROR;

    SQBlob b(LZ_BUFFER_SIZE);

    /*  Do the actual deflate'ing: */
    stream.next_in = (unsigned char*)data;
    stream.avail_in = data_size;
    if ( ! stream.avail_in ) {
        /*  Passed empty string, make it a noop instead of erroring out. */
        sq_pushliteral(v, "");
        return 1;
    }

    int nchunks = 0;
    do {
        stream.next_out  = ((unsigned char*)b.GetBuf()) + (LZ_BUFFER_SIZE * nchunks++);
        stream.avail_out = LZ_BUFFER_SIZE;
        int result = mz_deflate(&stream, MZ_FINISH);
        if (result == MZ_STREAM_END )
            break;
        if (result != MZ_OK)
        {
            sq_check_result(v, result, &stream);
            mz_deflateEnd(&stream);
            return SQ_ERROR;
        }
        b.GrowBufOf(LZ_BUFFER_SIZE);
    } while ( stream.avail_out == 0 );

    SQInteger total_out = stream.total_out;
    /*  Close the stream: */
    if(sq_check_result(v, mz_deflateEnd(&stream), &stream) == SQ_ERROR) return SQ_ERROR;

    /*  Need to do this before we alter the stack: */
    sq_pushstring(v, (const SQChar*)b.GetBuf(), total_out);

    return 1;
}

static SQRESULT sq_miniz_inflate(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, data);
    /*  By default, we will do gzip header detection w/ max window size */
    //SQ_OPT_INTEGER(v, 3, window_size, MAX_WBITS + 32);
    /*  Allocate the stream: */
    mz_stream_s stream;

    stream.zalloc = NULL;
    stream.zfree  = NULL;
    //if(sq_check_result(v, inflateInit2(&stream, window_size), &stream) != SQ_OK) return SQ_ERROR;
    if(sq_check_result(v, mz_inflateInit(&stream), &stream) != SQ_OK) return SQ_ERROR;
    SQBlob b(LZ_BUFFER_SIZE);

    /*  Do the actual deflate'ing: */
    stream.next_in = (unsigned char*)data;
    stream.avail_in = data_size;
    if ( ! stream.avail_in ) {
        /*  Passed empty string, make it a noop instead of erroring out. */
        sq_pushliteral(v, "");
        return 1;
    }

    int nchunks = 0;
    do {
        stream.next_out  = ((unsigned char*)b.GetBuf()) + (LZ_BUFFER_SIZE * nchunks++);
        stream.avail_out = LZ_BUFFER_SIZE;
        int result = mz_inflate(&stream, MZ_FINISH);
        if ( MZ_BUF_ERROR != result ) {
            /* Ignore Z_BUF_ERROR since that just indicates that we
             * need a larger buffer in order to proceed.  Thanks to
             * Tobias Markmann for finding this bug!
             */
            if(sq_check_result(v, result, &stream) == SQ_OK) break;
            else {
                mz_inflateEnd(&stream);
                return SQ_ERROR;
            }
        }
        b.GrowBufOf(LZ_BUFFER_SIZE);
    } while ( stream.avail_out == 0 );

    SQInteger total_out = stream.total_out;
    /*  Close the stream: */
    if(sq_check_result(v, mz_inflateEnd(&stream), &stream) == SQ_ERROR) return SQ_ERROR;

    /*  Need to do this before we alter the stack: */
    sq_pushstring(v, (const SQChar*)b.GetBuf(), total_out);

    return 1;
}

static SQRESULT sq_miniz_version(HSQUIRRELVM v)
{
    sq_pushstring(v, mz_version(), -1);
    return 1;
}

#ifdef __cplusplus
extern "C" {
#endif
#define INT_CONST(v,num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);


    SQRESULT sqext_register_sq_miniz(HSQUIRRELVM v)
    {
        sq_pushliteral(v,_SC("miniz"));
        sq_newtable(v);
        INT_CONST(v, MZ_DEFAULT_COMPRESSION);
        INT_CONST(v, MZ_BEST_SPEED);
        INT_CONST(v, MZ_BEST_COMPRESSION);

        sq_insertfunc(v, _SC("version"), sq_miniz_version, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("deflate"), sq_miniz_deflate, -2, _SC(".si"), SQTrue);
        sq_insertfunc(v, _SC("inflate"), sq_miniz_inflate, -2, _SC(".s"), SQTrue);

        sq_pushliteral(v,_SC("ZipArchive"));
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)sq_miniz_ziparchive_TAG);
        sq_insert_reg_funcs(v, sq_miniz_ziparchive_methods);
        sq_newslot(v,-3,SQTrue);

        sq_newslot(v,-3,SQTrue); //push miniz table
        return 0;
    }

#ifdef __cplusplus
}
#endif

