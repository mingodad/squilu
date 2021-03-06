/*	see copyright notice in squirrel.h */
#ifndef _SQSTDIO_H_
#define _SQSTDIO_H_

#ifdef __cplusplus

//#define SQSTD_STREAM_TYPE_TAG 0x80000000

struct SQStream {
    virtual ~SQStream() {}
	ABSTRACT_METHOD(virtual SQInteger Read(void *buffer, SQInteger size), {return 0;})
	ABSTRACT_METHOD(virtual SQInteger Gets(char *buffer, SQInteger size), {return 0;})
	ABSTRACT_METHOD(virtual SQInteger Write(const void *buffer, SQInteger size), {return 0;})
	ABSTRACT_METHOD(virtual SQInteger Flush(), {return 0;})
	ABSTRACT_METHOD(virtual SQInteger Tell(), {return 0;})
	ABSTRACT_METHOD(virtual SQInteger Len(), {return 0;})
	ABSTRACT_METHOD(virtual SQInteger Seek(SQInteger offset, SQInteger origin), {return 0;})
	ABSTRACT_METHOD(virtual bool IsValid(), {return false;})
	ABSTRACT_METHOD(virtual bool EOS(), {return true;})
	virtual void DummyPinVtable();
};

extern "C" {
#endif

#define SQ_SEEK_CUR 0
#define SQ_SEEK_END 1
#define SQ_SEEK_SET 2

typedef void* SQFILE;

SQUIRREL_API SQFILE sqstd_fopen(const SQChar *,const SQChar *);
SQUIRREL_API SQInteger sqstd_fread(SQUserPointer, SQInteger, SQInteger, SQFILE);
SQUIRREL_API SQChar *sqstd_fgets(SQChar*, SQInteger, SQFILE);
SQUIRREL_API SQInteger sqstd_fwrite(const SQUserPointer, SQInteger, SQInteger, SQFILE);
SQUIRREL_API SQInteger sqstd_fseek(SQFILE , SQInteger , SQInteger);
SQUIRREL_API SQInteger sqstd_ftell(SQFILE);
SQUIRREL_API SQInteger sqstd_fflush(SQFILE);
SQUIRREL_API SQInteger sqstd_fclose(SQFILE);
SQUIRREL_API SQInteger sqstd_feof(SQFILE);

SQUIRREL_API SQRESULT sqstd_createfile(HSQUIRRELVM v, SQFILE file,SQBool own);
SQUIRREL_API SQRESULT sqstd_getfile(HSQUIRRELVM v, SQInteger idx, SQFILE *file);

//compiler helpers
SQUIRREL_API SQRESULT sqstd_loadfile_include_path(HSQUIRRELVM v,const SQChar *filename,SQBool printerror,
                                                  SQBool show_warnings, const SQChar *include_path);
SQUIRREL_API SQRESULT sqstd_loadfile(HSQUIRRELVM v,const SQChar *filename,SQBool printerror,SQBool show_warnings);
SQUIRREL_API SQRESULT sqstd_dofile(HSQUIRRELVM v,const SQChar *filename,SQBool retval,SQBool printerror,SQBool show_warnings);
SQUIRREL_API SQRESULT sqstd_writeclosuretofile(HSQUIRRELVM v,const SQChar *filename);
SQUIRREL_API SQRESULT sqstd_writeclosuretofile_as_source(HSQUIRRELVM v,const SQChar *filename);

SQUIRREL_API SQRESULT sqstd_register_iolib(HSQUIRRELVM v);

extern const SQChar SQSTD_FILE_TYPE_TAG[];
extern const SQChar SQSTD_FILE_CLASS_TYPE_TAG[];


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQSTDIO_H_*/

