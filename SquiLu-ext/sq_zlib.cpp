#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <unzip.h>
#include "squirrel.h"
#include "sqstdblobimpl.h"
SQ_OPT_STRING_STRLEN();

#define LZ_BUFFER_SIZE 8192
#define LZ_MAX_ZIP_NAME_SIZE 256
#define ZIP_DOS_DIR_ATTRIBUTE_BITFLAG 0x10

static const SQChar sq_minizip_unzip_TAG[] = _SC("sq_minizip_unzip_tag");

#define GET_minizip_unzip_INSTANCE_VAR_AT(idx, Var) \
    SQ_GET_INSTANCE_VAR(v, idx, unzFile, Var, sq_minizip_unzip_TAG);\
    if(!Var) return sq_throwerror(v, _SC("miniz_ziparchive already destroyed"));
#define GET_minizip_unzip_INSTANCE() GET_minizip_unzip_INSTANCE_VAR_AT(1, self)

static SQRESULT sq_minizip_unzip_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	unzFile self = ((unzFile)p);
	if(self) {
	    unzClose(self);
	}
	return 0;
}

static SQRESULT sq_minizip_unzip_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, zip_fname);
    unzFile zip_archive = unzOpen64(zip_fname);
    if(!zip_archive)
    {
        return sq_throwerror(v, _SC("Cannot open %s\n"),zip_fname);
    }
    sq_setinstanceup(v, 1, zip_archive);
    sq_setreleasehook(v,1, sq_minizip_unzip_releasehook);
	return 1;
}

static SQRESULT sq_minizip_unzip_get_num_files(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_minizip_unzip_INSTANCE();
    unz_global_info64 gi;
    int err = unzGetGlobalInfo64(self,&gi);
    if(err != UNZ_OK)
    {
        return sq_throwerror(v, _SC("error %d with zipfile in unzGetGlobalInfo \n"), err);
    }
    sq_pushinteger(v, gi.number_entry);
	return 1;
}

static SQRESULT sq_minizip_unzip_goto_first_file(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_minizip_unzip_INSTANCE();
    sq_pushbool(v, unzGoToFirstFile(self) == UNZ_OK);
	return 1;
}

static SQRESULT sq_minizip_unzip_goto_next_file(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_minizip_unzip_INSTANCE();
    sq_pushbool(v, unzGoToNextFile(self) == UNZ_OK);
	return 1;
}

static SQRESULT sq_minizip_unzip_get_file_info(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_minizip_unzip_INSTANCE();
    char filename_inzip[LZ_MAX_ZIP_NAME_SIZE];
    unz_file_info64 file_info;
    int err = unzGetCurrentFileInfo64(self,&file_info,filename_inzip,LZ_MAX_ZIP_NAME_SIZE,NULL,0,NULL,0);
    if (err!=UNZ_OK)
    {
        return sq_throwerror(v, _SC("error %d with zipfile in unzGetCurrentFileInfo\n"),err);
    }
    sq_newtable(v);
    sq_pushliteral(v, "name");
    sq_pushstring(v, filename_inzip, -1);
    sq_rawset(v, -3);
    sq_pushliteral(v, "size");
    sq_pushinteger(v, file_info.uncompressed_size);
    sq_rawset(v, -3);
    sq_pushliteral(v, "is_dir");
    sq_pushbool(v, file_info.external_fa & ZIP_DOS_DIR_ATTRIBUTE_BITFLAG);
    sq_rawset(v, -3);

	return 1;
}

static SQRESULT sq_minizip_unzip_extract_file(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_minizip_unzip_INSTANCE();
    int err = unzOpenCurrentFile(self);
    if (err!=UNZ_OK)
    {
        return sq_throwerror(v, _SC("error %d with zipfile in unzGetCurrentFileInfo\n"), err);
    }
    SQBlob b(0, 8192);
    char buf[8192];
    uInt size_buf = sizeof(buf);
    do
    {
        err = unzReadCurrentFile(self,buf,size_buf);
        if (err<0)
        {
            return sq_throwerror(v, _SC("error %d with zipfile in unzReadCurrentFile\n"), err);
        }
        if (err>0)
        {
            b.Write(buf, err);
        }
    }
    while (err>0);
    err = unzCloseCurrentFile(self);
    sq_pushstring(v, (SQChar*)b.GetBuf(), b.Len());
	return 1;
}

static SQRESULT sq_minizip_unzip_destroy(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_minizip_unzip_INSTANCE();
    int rc = sq_minizip_unzip_releasehook(self, 0, v);
    if(rc == 0) sq_setinstanceup(v, 1, 0);
    sq_pushinteger(v, rc);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_minizip_unzip_##name,nparams,tycheck}
static SQRegFunction sq_minizip_unzip_methods[] =
{
	_DECL_FUNC(constructor,  2, _SC("xs")),
	_DECL_FUNC(get_num_files,  1, _SC("x")),
	_DECL_FUNC(goto_first_file,  1, _SC("x")),
	_DECL_FUNC(goto_next_file,  1, _SC("x")),
	_DECL_FUNC(get_file_info,  1, _SC("x")),
	_DECL_FUNC(extract_file,  1, _SC("x")),
	_DECL_FUNC(destroy,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC


static SQRESULT sq_check_result(HSQUIRRELVM v, int result, const z_stream* stream) {
    /* Both of these are "normal" return codes: */
    if ( result == Z_OK || result == Z_STREAM_END ) return SQ_OK;
    switch ( result ) {
    case Z_NEED_DICT:
        return sq_throwerror(v, _SC("RequiresDictionary: input stream requires a dictionary to be deflated (%s)"),
                        stream->msg);
        break;
    case Z_STREAM_ERROR:
        return sq_throwerror(v, _SC("InternalError: inconsistent internal zlib stream (%s)"),
                        stream->msg);
        break;
    case Z_DATA_ERROR:
        return sq_throwerror(v, _SC("InvalidInput: input string does not conform to zlib format or checksum"));
        break;
    case Z_MEM_ERROR:
        return sq_throwerror(v, _SC("OutOfMemory: not enough memory (%s)"), stream->msg);
        break;
    case Z_BUF_ERROR:
        return sq_throwerror(v, _SC("InternalError: no progress possible (%s)"), stream->msg);
        break;
    case Z_VERSION_ERROR:
        return sq_throwerror(v, _SC("IncompatibleLibrary: built with version %s, but dynamically linked with version %s (%s)"),
                        ZLIB_VERSION,  zlibVersion(), stream->msg);
        break;
    default:
        return sq_throwerror(v, _SC("ZLibError: unknown code %d (%s)"), result, stream->msg);
    }
    return SQ_OK;
}

static SQRESULT sq_zlib_deflate(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, data);
    SQ_OPT_INTEGER(v, 3, level, Z_DEFAULT_COMPRESSION);
    /*  Allocate the stream: */
    z_stream stream;

    stream.zalloc = Z_NULL;
    stream.zfree  = Z_NULL;
    if(sq_check_result(v, deflateInit(&stream, level), &stream) != SQ_OK) return SQ_ERROR;

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
        int result = deflate(&stream, Z_FINISH);
        if (result == Z_STREAM_END )
            break;
        if (result != Z_OK)
        {
            sq_check_result(v, result, &stream);
            deflateEnd(&stream);
            return SQ_ERROR;
        }
        b.GrowBufOf(LZ_BUFFER_SIZE);
    } while ( stream.avail_out == 0 );

    SQInteger total_out = stream.total_out;
    /*  Close the stream: */
    if(sq_check_result(v, deflateEnd(&stream), &stream) == SQ_ERROR) return SQ_ERROR;

    /*  Need to do this before we alter the stack: */
    sq_pushstring(v, (const SQChar*)b.GetBuf(), total_out);

    return 1;
}

static SQRESULT sq_zlib_inflate(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, data);
    /*  By default, we will do gzip header detection w/ max window size */
    SQ_OPT_INTEGER(v, 3, window_size, MAX_WBITS + 32);
    /*  Allocate the stream: */
    z_stream stream;

    stream.zalloc = Z_NULL;
    stream.zfree  = Z_NULL;
    if(sq_check_result(v, inflateInit2(&stream, window_size), &stream) != SQ_OK) return SQ_ERROR;

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
        int result = inflate(&stream, Z_FINISH);
        if ( Z_BUF_ERROR != result ) {
            /* Ignore Z_BUF_ERROR since that just indicates that we
             * need a larger buffer in order to proceed.  Thanks to
             * Tobias Markmann for finding this bug!
             */
            if(sq_check_result(v, result, &stream) == SQ_OK) break;
            else {
                inflateEnd(&stream);
                return SQ_ERROR;
            }
        }
        b.GrowBufOf(LZ_BUFFER_SIZE);
    } while ( stream.avail_out == 0 );

    SQInteger total_out = stream.total_out;
    /*  Close the stream: */
    if(sq_check_result(v, inflateEnd(&stream), &stream) == SQ_ERROR) return SQ_ERROR;

    /*  Need to do this before we alter the stack: */
    sq_pushstring(v, (const SQChar*)b.GetBuf(), total_out);

    return 1;
}

static SQRESULT sq_zlib_version(HSQUIRRELVM v)
{
    sq_pushstring(v, zlibVersion(), -1);
    return 1;
}

#ifdef __cplusplus
extern "C" {
#endif
#define INT_CONST(v,num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);


    SQRESULT sqext_register_sq_zlib(HSQUIRRELVM v)
    {
        sq_pushliteral(v,_SC("zlib"));
        sq_newtable(v);
        INT_CONST(v, Z_DEFAULT_COMPRESSION);
        INT_CONST(v, Z_BEST_SPEED);
        INT_CONST(v, Z_BEST_COMPRESSION);

        sq_insertfunc(v, _SC("version"), sq_zlib_version, 1, _SC("."), SQTrue);
        sq_insertfunc(v, _SC("deflate"), sq_zlib_deflate, -2, _SC(".si"), SQTrue);
        sq_insertfunc(v, _SC("inflate"), sq_zlib_inflate, -2, _SC(".si"), SQTrue);

        sq_pushliteral(v,_SC("Unzip"));
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)sq_minizip_unzip_TAG);
        sq_insert_reg_funcs(v, sq_minizip_unzip_methods);
        sq_newslot(v,-3,SQTrue);

        sq_newslot(v,-3,SQTrue); //push zlib table
        return 0;
    }

#ifdef __cplusplus
}
#endif
