/* see copyright notice in squirrel.h */
#include <new>
#include <stdio.h>
#include <string.h>
#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdfile.h>
#include "sqstdstream.h"

#if 0
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#endif // _WIN32
#endif // 0

//only to make the SQFile vtable be generated here
void SQFile::DummyPinVtable(){}

//#define SQSTD_FILE_TYPE_TAG (SQSTD_STREAM_TYPE_TAG | 0x00000001)
const SQChar  SQSTD_FILE_TYPE_TAG[] = _SC("std_stream_file");
const SQChar  SQSTD_FILE_CLASS_TYPE_TAG[] = _SC("std_file");

//basic API
SQFILE sqstd_fopen(const SQChar *filename ,const SQChar *mode)
{
	return (SQFILE)scfopen(filename,mode);
}

SQInteger sqstd_fread(void* buffer, SQInteger size, SQInteger count, SQFILE file)
{
	return (SQInteger)fread(buffer,size,count,(FILE *)file);
}

SQChar *sqstd_fgets(SQChar* buffer, SQInteger count, SQFILE file)
{
	return (SQChar*)fgets(buffer,count,(FILE *)file);
}

SQInteger sqstd_fwrite(const SQUserPointer buffer, SQInteger size, SQInteger count, SQFILE file)
{
	return (SQInteger)fwrite(buffer,size,count,(FILE *)file);
}

SQInteger sqstd_fseek(SQFILE file, SQInteger offset, SQInteger origin)
{
	SQInteger realorigin;
	switch(origin) {
		case SQ_SEEK_CUR: realorigin = SEEK_CUR; break;
		case SQ_SEEK_END: realorigin = SEEK_END; break;
		case SQ_SEEK_SET: realorigin = SEEK_SET; break;
		default: return -1; //failed
	}
	return fseek((FILE *)file,(long)offset,(int)realorigin);
}

SQInteger sqstd_ftell(SQFILE file)
{
	return ftell((FILE *)file);
}

SQInteger sqstd_fflush(SQFILE file)
{
	return fflush((FILE *)file);
}

SQInteger sqstd_fclose(SQFILE file)
{
	return fclose((FILE *)file);
}

SQInteger sqstd_feof(SQFILE file)
{
	return feof((FILE *)file);
}

struct SQPopen : public SQFile {
	SQPopen():SQFile() {}
	SQPopen(SQFILE file, bool owns):SQFile(file, owns) {}
	bool Open(const SQChar *filename ,const SQChar *mode) {
		Close();
		if( (_handle = popen(filename,mode)) ) {
			_owns = true;
			return true;
		}
		return false;
	}
	int PClose() {
	    int result = 0;
		if(_handle && _owns) {
			result = pclose((FILE*)_handle);
			_handle = NULL;
			_owns = false;
		}
		return result;
	}
	void Close() {
	    PClose();
	}
	void DummyPinVtable();
};

//only to make the SQPopen vtable be generated here
void SQPopen::DummyPinVtable(){}

static SQRESULT _popen__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC("popen"),-1);
	return 1;
}

static SQRESULT _popen_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	SQPopen *self = (SQPopen*)p;
	self->~SQPopen(); //on osx it doesn't seem to work
	sq_free(self,sizeof(SQFile));
	return 1;
}

static SQRESULT _popen_constructor(HSQUIRRELVM v)
{
	const SQChar *filename,*mode;
	bool owns = true;
	SQPopen *f;
	SQFILE newf;
	if(sq_gettype(v,2) == OT_STRING && sq_gettype(v,3) == OT_STRING) {
		sq_getstring(v, 2, &filename);
		sq_getstring(v, 3, &mode);
		newf = popen(filename, mode);
		if(!newf) return sq_throwerror(v, _SC("cannot open file"));
	} else {
		return sq_throwerror(v,_SC("wrong parameter"));
	}

	f = new (sq_malloc(sizeof(SQPopen)))SQPopen(newf,owns);
	if(SQ_FAILED(sq_setinstanceup(v,1,f))) {
		f->~SQPopen();
		sq_free(f,sizeof(SQPopen));
		return sq_throwerror(v, _SC("cannot create popen"));
	}
	sq_setreleasehook(v,1,_popen_releasehook);
	return 0;
}

static SQRESULT _popen_close(HSQUIRRELVM v)
{
	SQPopen *self = NULL;
	if(SQ_SUCCEEDED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(SQUserPointer)SQSTD_FILE_TYPE_TAG,SQTrue))
		&& self != NULL)
	{
		sq_pushinteger(v, self->PClose());
		return 1;
	}
	return 0;
}

#define _DECL_FILE_FUNC(name,nparams,typecheck) {_SC(#name),_popen_##name,nparams,typecheck,false}
static const SQRegFunction _popen_methods[] = {
    _DECL_FILE_FUNC(constructor,3,_SC("x")),
    _DECL_FILE_FUNC(_typeof,1,_SC("x")),
    _DECL_FILE_FUNC(close,1,_SC("x")),
    {NULL,(SQFUNCTION)0,0,NULL,false}
};
#undef _DECL_FILE_FUNC

static SQRESULT _file__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC("file"),-1);
	return 1;
}

static SQRESULT _file_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	SQFile *self = (SQFile*)p;
	self->~SQFile();
	sq_free(self,sizeof(SQFile));
	return 1;
}

static SQRESULT _file_constructor(HSQUIRRELVM v)
{
	const SQChar *filename,*mode;
	bool owns = true;
	SQFile *f;
	SQFILE newf;
	if(sq_gettype(v,2) == OT_STRING && sq_gettype(v,3) == OT_STRING) {
		sq_getstring(v, 2, &filename);
		sq_getstring(v, 3, &mode);
		newf = sqstd_fopen(filename, mode);
		if(!newf) return sq_throwerror(v, _SC("cannot open file"));
	} else if(sq_gettype(v,2) == OT_USERPOINTER) {
		owns = !(sq_gettype(v,3) == OT_NULL);
		sq_getuserpointer(v,2,&newf);
	} else {
		return sq_throwerror(v,_SC("wrong parameter"));
	}

	f = new (sq_malloc(sizeof(SQFile)))SQFile(newf,owns);
	if(SQ_FAILED(sq_setinstanceup(v,1,f))) {
		f->~SQFile();
		sq_free(f,sizeof(SQFile));
		return sq_throwerror(v, _SC("cannot create blob with negative size"));
	}
	sq_setreleasehook(v,1,_file_releasehook);
	return 0;
}

static SQRESULT _file_close(HSQUIRRELVM v)
{
	SQFile *self = NULL;
	if(SQ_SUCCEEDED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(SQUserPointer)SQSTD_FILE_TYPE_TAG,SQTrue))
		&& self != NULL)
	{
		self->Close();
	}
	return 0;
}
#if 0
static SQRESULT _file_nonblock(HSQUIRRELVM v)
{
	SQFile *self = NULL;
#ifdef _WIN32
    return sq_throwerror(v, _SC("nonblock not supported on win32"));
#else
	if(SQ_SUCCEEDED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(SQUserPointer)SQSTD_FILE_TYPE_TAG))
		&& self != NULL)
	{
	    SQFILE handle = self->GetHandle();
	    if(handle)
        {
            int fd = fileno((FILE*)handle);
            if(fd > STDERR_FILENO)
            {
                int flags = fcntl(fd, F_GETFL, 0);

                if(sq_gettop(v) > 1)
                {
                    //set nonblocking
                    SQBool isNonBlocking;
                    sq_getbool(v, 2, &isNonBlocking);
                    if (flags >= 0) {
                        int rc = fcntl(fd, F_SETFL,
                                    isNonBlocking ? flags | O_NONBLOCK
                                    : flags ^ O_NONBLOCK);
                        sq_pushbool(v, rc == 0);
                    }
                    else return 0;
                }
                else
                {
                    sq_pushbool(v, flags & O_NONBLOCK);
                }
                return 1;
            }
        }
	}
	return 0;
#endif // _WIN32
}
#endif // 0
//bindings
#define _DECL_FILE_FUNC(name,nparams,typecheck) {_SC(#name),_file_##name,nparams,typecheck,false}
static const SQRegFunction _file_methods[] = {
    _DECL_FILE_FUNC(constructor,3,_SC("x")), //TODO if we change "x" to "xss" it stops working
    _DECL_FILE_FUNC(_typeof,1,_SC("x")),
    _DECL_FILE_FUNC(close,1,_SC("x")),
    //_DECL_FILE_FUNC(nonblock,-1,_SC("xb")),
    {NULL,(SQFUNCTION)0,0,NULL,false}
};


SQRESULT sqstd_createfile(HSQUIRRELVM v, SQFILE file,SQBool own)
{
	SQInteger top = sq_gettop(v);
	sq_pushregistrytable(v);
	sq_pushstring(v,_SC("std_file"),-1);
	if(SQ_SUCCEEDED(sq_get(v,-2))) {
		sq_remove(v,-2); //removes the registry
		sq_pushroottable(v); // push the this
		sq_pushuserpointer(v,file); //file
		if(own){
			sq_pushinteger(v,1); //true
		}
		else{
			sq_pushnull(v); //false
		}
		if(SQ_SUCCEEDED( sq_call(v,3,SQTrue,SQFalse) )) {
			sq_remove(v,-2);
			return SQ_OK;
		}
	}
	sq_settop(v,top);
	return SQ_ERROR;
}

SQRESULT sqstd_getfile(HSQUIRRELVM v, SQInteger idx, SQFILE *file)
{
	SQFile *fileobj = NULL;
	if(SQ_SUCCEEDED(sq_getinstanceup(v,idx,(SQUserPointer*)&fileobj,(SQUserPointer)SQSTD_FILE_TYPE_TAG,SQFalse))) {
		*file = fileobj->GetHandle();
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("not a file"));
}



#define IO_BUFFER_SIZE 2048
struct IOBuffer {
    unsigned char buffer[IO_BUFFER_SIZE];
    SQInteger size;
    SQInteger ptr;
    SQFILE file;
};

static SQInteger _read_byte(IOBuffer *iobuffer)
{
	if(iobuffer->ptr < iobuffer->size) {

		SQInteger ret = iobuffer->buffer[iobuffer->ptr];
		iobuffer->ptr++;
		return ret;
	}
	else {
		if( (iobuffer->size = sqstd_fread(iobuffer->buffer,1,IO_BUFFER_SIZE,iobuffer->file )) > 0 )
		{
			SQInteger ret = iobuffer->buffer[0];
			iobuffer->ptr = 1;
			return ret;
		}
	}

	return 0;
}

static SQInteger _read_two_bytes(IOBuffer *iobuffer)
{
    if(iobuffer->ptr < iobuffer->size) {
        if(iobuffer->size < 2) return 0;
        SQInteger ret = *((const wchar_t*)&iobuffer->buffer[iobuffer->ptr]);
        iobuffer->ptr += 2;
        return ret;
    }
    else {
        if( (iobuffer->size = sqstd_fread(iobuffer->buffer,1,IO_BUFFER_SIZE,iobuffer->file )) > 0 )
        {
            if(iobuffer->size < 2) return 0;
            SQInteger ret = *((const wchar_t*)&iobuffer->buffer[0]);
            iobuffer->ptr = 2;
            return ret;
        }
    }

	return 0;
}

static SQInteger _io_file_lexfeed_PLAIN(SQUserPointer iobuf)
{
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
	return _read_byte(iobuffer);

}

#ifdef SQUNICODE
static SQInteger _io_file_lexfeed_UTF8(SQUserPointer iobuf)
{
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
#define READ(iobuf) \
	if((inchar = (unsigned char)_read_byte(iobuf)) == 0) \
		return 0;

    static const SQInteger utf8_lengths[16] =
    {
        1,1,1,1,1,1,1,1,        /* 0000 to 0111 : 1 byte (plain ASCII) */
        0,0,0,0,                /* 1000 to 1011 : not valid */
        2,2,                    /* 1100, 1101 : 2 bytes */
        3,                      /* 1110 : 3 bytes */
        4                       /* 1111 :4 bytes */
    };
    static const unsigned char byte_masks[5] = {0,0,0x1f,0x0f,0x07};
    unsigned char inchar;
    SQInteger c = 0;
    READ(iobuffer);
    c = inchar;
    //
    if(c >= 0x80) {
        SQInteger tmp;
        SQInteger codelen = utf8_lengths[c>>4];
        if(codelen == 0)
            return 0;
            //"invalid UTF-8 stream";
        tmp = c&byte_masks[codelen];
        for(SQInteger n = 0; n < codelen-1; n++) {
            tmp<<=6;
            READ(iobuffer);
            tmp |= inchar & 0x3F;
        }
        c = tmp;
    }
    return c;
}
#endif

static SQInteger _io_file_lexfeed_UCS2_LE(SQUserPointer iobuf)
{
	SQInteger ret;
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
	if( (ret = _read_two_bytes(iobuffer)) > 0 )
		return ret;
	return 0;
}

static SQInteger _io_file_lexfeed_UCS2_BE(SQUserPointer iobuf)
{
	SQInteger c;
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
	if( (c = _read_two_bytes(iobuffer)) > 0 ) {
		c = ((c>>8)&0x00FF)| ((c<<8)&0xFF00);
		return c;
	}
	return 0;
}

static SQInteger file_read(SQUserPointer file,SQUserPointer buf,SQInteger size)
{
	SQInteger ret;
	if( ( ret = sqstd_fread(buf,1,size,(SQFILE)file ))!=0 )return ret;
	return -1;
}

static SQInteger file_write(SQUserPointer file,SQUserPointer p,SQInteger size)
{
	return sqstd_fwrite(p,1,size,(SQFILE)file);
}

SQRESULT sqstd_loadfile(HSQUIRRELVM v,const SQChar *filename,SQBool printerror,SQBool show_warnings)
{
	SQFILE file = sqstd_fopen(filename,_SC("rb"));
	SQInteger ret;
	unsigned short us;
	unsigned char uc;
	SQLEXREADFUNC func = _io_file_lexfeed_PLAIN;
	if(file){
		ret = sqstd_fread(&us,1,2,file);
		if(ret != 2) {
			//probably an empty file
			us = 0;
		}
		if(us == SQ_BYTECODE_STREAM_TAG) { //BYTECODE
			sqstd_fseek(file,0,SQ_SEEK_SET);
			if(SQ_SUCCEEDED(sq_readclosure(v,file_read,file))) {
				sqstd_fclose(file);
				return SQ_OK;
			}
		}
		else { //SCRIPT
			switch(us)
			{
				//gotta swap the next 2 lines on BIG endian machines
				case 0xFFFE: func = _io_file_lexfeed_UCS2_BE; break;//UTF-16 little endian;
				case 0xFEFF: func = _io_file_lexfeed_UCS2_LE; break;//UTF-16 big endian;
				case 0xBBEF:
					if(sqstd_fread(&uc,1,sizeof(uc),file) == 0) {
						sqstd_fclose(file);
						return sq_throwerror(v,_SC("io error"));
					}
					if(uc != 0xBF) {
						sqstd_fclose(file);
						return sq_throwerror(v,_SC("Unrecognozed ecoding"));
					}
#ifdef SQUNICODE
					func = _io_file_lexfeed_UTF8;
#else
					func = _io_file_lexfeed_PLAIN;
#endif
					break;//UTF-8 ;
				default: sqstd_fseek(file,0,SQ_SEEK_SET); break; // ascii
			}

			IOBuffer buffer;
			buffer.ptr = 0;
			buffer.size = 0;
			buffer.file = file;

			if(SQ_SUCCEEDED(sq_compile(v,func,&buffer,filename,printerror,show_warnings,
                              SQ_MAX_INCLUDE_FILES))){
				sqstd_fclose(file);
				return SQ_OK;
			}
		}
		sqstd_fclose(file);
		return SQ_ERROR;
	}
	return sq_throwerror(v,_SC("cannot open the file"));
}


SQRESULT sqstd_dofile(HSQUIRRELVM v,const SQChar *filename,SQBool retval,SQBool printerror,SQBool show_warnings)
{
	if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,printerror,show_warnings))) {
		sq_push(v,-2);
		if(SQ_SUCCEEDED(sq_call(v,1,retval,SQTrue))) {
			sq_remove(v,retval?-2:-1); //removes the closure
			return 1;
		}
		sq_pop(v,1); //removes the closure
	}
	return SQ_ERROR;
}

SQRESULT sqstd_writeclosuretofile(HSQUIRRELVM v,const SQChar *filename)
{
	SQFILE file = sqstd_fopen(filename,_SC("wb+"));
	if(!file) return sq_throwerror(v,_SC("cannot open the file"));
	if(SQ_SUCCEEDED(sq_writeclosure(v,file_write,file))) {
		sqstd_fclose(file);
		return SQ_OK;
	}
	sqstd_fclose(file);
	return SQ_ERROR; //forward the error
}

SQRESULT sqstd_writeclosuretofile_as_source(HSQUIRRELVM v,const SQChar *filename)
{
	SQFILE file = sqstd_fopen(filename,_SC("wb+"));
	if(!file) return sq_throwerror(v,_SC("cannot open the file"));
	if(SQ_SUCCEEDED(sq_writeclosure_as_source(v,file_write,file))) {
		sqstd_fclose(file);
		return SQ_OK;
	}
	sqstd_fclose(file);
	return SQ_ERROR; //forward the error
}

SQInteger _g_io_loadfile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, filename);
    SQ_OPT_BOOL(v, 3, printerror, SQFalse);
    SQ_OPT_BOOL(v, 4, show_warnings, SQTrue);
	if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,printerror,show_warnings)))
		return 1;
	return SQ_ERROR; //propagates the error
}

SQInteger _g_io_writeclosuretofile(HSQUIRRELVM v)
{
	const SQChar *filename;
	sq_getstring(v,2,&filename);
	if(SQ_SUCCEEDED(sqstd_writeclosuretofile(v,filename)))
		return 1;
	return SQ_ERROR; //propagates the error
}

#include "sqstdblobimpl.h"
SQInteger blob_write(SQUserPointer file,SQUserPointer p,SQInteger size);
SQInteger _g_io_dumpclosure(HSQUIRRELVM v)
{
	SQBlob b(0,8192);
	if(SQ_SUCCEEDED(sq_writeclosure(v,blob_write,&b))) {
		sq_pushstring(v, (const SQChar*)b.GetBuf(), b.Len());
		return 1;
	}
	return SQ_ERROR; //forward the error
}

SQInteger blob_read(SQUserPointer file,SQUserPointer p,SQInteger size);
SQInteger _g_io_loadstring(HSQUIRRELVM v)
{
    if(sq_gettype(v, 2) != OT_STRING) return sq_throwerror(v, _SC("string expected as parameter"));
    SQInteger rc, size = sq_getsize(v, 2);
    const SQChar *dump;
    sq_getstring(v, 2, &dump);
    unsigned short tag = *((unsigned short*)dump);
    if(tag == SQ_BYTECODE_STREAM_TAG){
        SQBlob b(0, size);
        b.Write(dump, size);
        b.Seek(0, SQ_SEEK_SET);
        rc = sq_readclosure(v, blob_read, &b);
    }
    else
    {
        rc = sq_compilebuffer(v, dump, size, _SC("loadstring"), SQFalse, SQFalse, SQ_MAX_INCLUDE_FILES);
    }
	return rc < 0 ? rc : 1;
}

//used by sq_slave_vm
SQInteger _g_io_dostring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQInteger rc = _g_io_loadstring(v);
    if(rc < 0) return rc;
    SQ_OPT_BOOL(v, 3, retval, SQFalse);
    sq_push(v,1); //this environment
    if(SQ_SUCCEEDED(sq_call(v,1,retval,SQTrue))) {
        sq_remove(v,retval?-2:-1); //removes the closure
        return 1;
    }
    sq_pop(v,1); //removes the closure
	return SQ_ERROR; //forward the error
}

SQInteger _g_io_dofile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, filename);
    SQ_OPT_BOOL(v, 3, printerror, SQFalse);
    SQ_OPT_BOOL(v, 4, show_warnings, SQTrue);
	sq_push(v,1); //repush the this
	if(SQ_SUCCEEDED(sqstd_dofile(v,filename,SQTrue,printerror,show_warnings)))
		return 1;
	return SQ_ERROR; //propagates the error
}

SQInteger _g_io_existsfile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, filename);
    SQFile fs;
    sq_pushbool(v, fs.Open(filename, "rb"));
    return 1;
}

SQInteger _g_io_readfile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, filename);
    SQFile fs;
    if(fs.Open(filename, "rb"))
    {
        SQInteger size,res;
        SQChar *data;
        size = fs.Len();
        data = sq_getscratchstr(v,size);
        res = fs.Read(data,size);
        if(res != size)
        {
            sq_delscratchstr(v, data);
            if(res <= 0)
                return sq_throwerror(v,_SC("no data left to read"));
            return sq_throwerror(v,_SC("could not read whole file"));
        }
        sq_pushscratchstr(v);
        return 1;
    }
	return sq_throwerror(v,_SC("could not open file %s"), filename);
}

SQInteger _g_io_writefile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, filename);
    SQ_GET_STRING(v, 3, data);
    SQFile fs;
    if(fs.Open(filename, "wb"))
    {
        SQInteger res = fs.Write(data, data_size);
        if(res != data_size)
            return sq_throwerror(v,_SC("could not write to file"));
        return 0;
    }
	return sq_throwerror(v,_SC("could not open file %s"), filename);
}

#define _DECL_GLOBALIO_FUNC(name,nparams,typecheck) {_SC(#name),_g_io_##name,nparams,typecheck,false}
static const SQRegFunction iolib_funcs[]={
	_DECL_GLOBALIO_FUNC(loadfile,-2,_SC(".sbb")),
	_DECL_GLOBALIO_FUNC(dofile,-2,_SC(".sbb")),
	_DECL_GLOBALIO_FUNC(writeclosuretofile,3,_SC(".sc")),
	_DECL_GLOBALIO_FUNC(dostring,-2,_SC(".sb")),
	_DECL_GLOBALIO_FUNC(loadstring,2,_SC(".s")),
	_DECL_GLOBALIO_FUNC(dumpclosure,3,_SC(".sc")),
	_DECL_GLOBALIO_FUNC(existsfile,2,_SC(".s")),
	_DECL_GLOBALIO_FUNC(readfile,2,_SC(".s")),
	_DECL_GLOBALIO_FUNC(writefile,3,_SC(".ss")),
	{NULL,(SQFUNCTION)0,0,NULL,false}
};

SQRESULT sqstd_register_iolib(HSQUIRRELVM v)
{
	//create delegate
	declare_stream(v,_SC("popen"),(SQUserPointer)SQSTD_FILE_TYPE_TAG,SQSTD_FILE_CLASS_TYPE_TAG,_popen_methods,iolib_funcs);
	declare_stream(v,_SC("file"),(SQUserPointer)SQSTD_FILE_TYPE_TAG,SQSTD_FILE_CLASS_TYPE_TAG,_file_methods,iolib_funcs);
	sq_pushstring(v,_SC("stdout"),-1);
	sqstd_createfile(v,stdout,SQFalse);
	sq_newslot(v,-3,SQFalse);
	sq_pushstring(v,_SC("stdin"),-1);
	sqstd_createfile(v,stdin,SQFalse);
	sq_newslot(v,-3,SQFalse);
	sq_pushstring(v,_SC("stderr"),-1);
	sqstd_createfile(v,stderr,SQFalse);
	sq_newslot(v,-3,SQFalse);
	return SQ_OK;
}
