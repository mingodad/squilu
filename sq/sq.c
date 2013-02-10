/*	see copyright notice in squirrel.h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#include <conio.h>
#endif
#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdaux.h>

#ifdef SQUNICODE
#define scfprintf fwprintf
#define scfopen	_wfopen
#define scvprintf vfwprintf
#else
#define scfprintf fprintf
#define scfopen	fopen
#define scvprintf vfprintf
#endif

int sq_main_argc = 0;
char** sq_main_argv = 0;

void PrintVersionInfos();

#if defined(_MSC_VER) && defined(_DEBUG)
int MemAllocHook( int allocType, void *userData, size_t size, int blockType,
   long requestNumber, const unsigned char *filename, int lineNumber)
{
	//if(requestNumber==769)_asm int 3;
	return 1;
}
#endif


SQInteger quit(HSQUIRRELVM v)
{
	int *done;
	sq_getuserpointer(v,-1,(SQUserPointer*)&done);
	*done=1;
	return 0;
}

void printfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stdout, s, vl);
	va_end(vl);
}

void errorfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stderr, s, vl);
	va_end(vl);
}

void PrintVersionInfos()
{
	scfprintf(stdout,_SC("%s %s (%d bits)\n"),SQUIRREL_VERSION,SQUIRREL_COPYRIGHT,((int)(sizeof(SQInteger)*8)));
}

void PrintUsage()
{
	scfprintf(stderr,_SC("usage: sq <options> <scriptpath [args]>.\n")
		_SC("Available options are:\n")
		_SC("   -c              compiles the file to bytecode(default output 'out.cnut')\n")
		_SC("   -o              specifies output file for the -c option\n")
		_SC("   -c              compiles only\n")
		_SC("   -d              generates debug infos\n")
		_SC("   -v              displays version infos\n")
		_SC("   -h              prints help\n"));
}

#define _INTERACTIVE 0
#define _DONE 2
#define _ERROR 3
//<<FIXME>> this func is a mess
int getargs(HSQUIRRELVM v,int argc, char* argv[],SQInteger *retval)
{
	int i;
	int compiles_only = 0;
	int compiles_as_source_only = 0;
	//static SQChar temp[500];
	//const SQChar *ret=NULL;
	char * output = NULL;
	//int lineinfo=0;
	*retval = 0;
	if(argc>1)
	{
		int arg=1,exitloop=0;

		while(arg < argc && !exitloop)
		{

			if(argv[arg][0]=='-')
			{
				switch(argv[arg][1])
				{
				case 'd': //DEBUG(debug infos)
					sq_enabledebuginfo(v,1);
					break;
				case 'c':
					compiles_only = 1;
					break;
				case 's':
					compiles_as_source_only = 1;
					break;
				case 'o':
					if(arg < argc) {
						arg++;
						output = argv[arg];
					}
					break;
				case 'v':
					PrintVersionInfos();
					return _DONE;

				case 'h':
					PrintVersionInfos();
					PrintUsage();
					return _DONE;
				default:
					PrintVersionInfos();
					scprintf(_SC("unknown prameter '-%c'\n"),argv[arg][1]);
					PrintUsage();
					*retval = -1;
					return _ERROR;
				}
			}else break;
			arg++;
		}

		// src file

		if(arg<argc) {
			const SQChar *filename=NULL;
#ifdef SQUNICODE
			mbstowcs(temp,argv[arg],strlen(argv[arg]));
			filename=temp;
#else
			filename=argv[arg];
#endif

			arg++;

			//sq_pushstring(v,_SC("ARGS"),-1);
			//sq_newarray(v,0);

			//sq_createslot(v,-3);
			//sq_pop(v,1);
			if(compiles_only) {
				if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,SQTrue))){
					const SQChar *outfile = _SC("out.cnut");
					if(output) {
#ifdef SQUNICODE
						int len = (int)(strlen(output)+1);
						mbstowcs(sq_getscratchpad(v,len*sizeof(SQChar)),output,len);
						outfile = sq_getscratchpad(v,-1);
#else
						outfile = output;
#endif
					}
					if(SQ_SUCCEEDED(sqstd_writeclosuretofile(v,outfile)))
						return _DONE;
				}
			}
			else if(compiles_as_source_only) {
				if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,SQTrue))){
					const SQChar *outfile = _SC("out.nut");
					if(output) {
#ifdef SQUNICODE
						int len = (int)(strlen(output)+1);
						mbstowcs(sq_getscratchpad(v,len*sizeof(SQChar)),output,len);
						outfile = sq_getscratchpad(v,-1);
#else
						outfile = output;
#endif
					}
					if(SQ_SUCCEEDED(sqstd_writeclosuretofile_as_source(v,outfile)))
						return _DONE;
				}
			}
			else {
				//if(SQ_SUCCEEDED(sqstd_dofile(v,filename,SQFalse,SQTrue))) {
					//return _DONE;
				//}
				if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,SQTrue))) {
					int callargs = 1;
					sq_pushroottable(v);
					for(i=arg;i<argc;i++)
					{
						const SQChar *a;
#ifdef SQUNICODE
						int alen=(int)strlen(argv[i]);
						a=sq_getscratchpad(v,(int)(alen*sizeof(SQChar)));
						mbstowcs(sq_getscratchpad(v,-1),argv[i],alen);
						sq_getscratchpad(v,-1)[alen] = _SC('\0');
#else
						a=argv[i];
#endif
						sq_pushstring(v,a,-1);
						callargs++;
						//sq_arrayappend(v,-2);
					}
					if(SQ_SUCCEEDED(sq_call(v,callargs,SQTrue,SQTrue))) {
						SQObjectType type = sq_gettype(v,-1);
						if(type == OT_INTEGER) {
							*retval = type;
							sq_getinteger(v,-1,retval);
						}
						return _DONE;
					}
					else{
						return _ERROR;
					}

				}
			}
			//if this point is reached an error occured
			{
				const SQChar *err;
				sq_getlasterror(v);
				if(SQ_SUCCEEDED(sq_getstring(v,-1,&err))) {
					scprintf(_SC("Error [%s]\n"),err);
					*retval = -2;
					return _ERROR;
				}
			}

		}
	}

	return _INTERACTIVE;
}

void Interactive(HSQUIRRELVM v)
{

#define MAXINPUT 1024
	SQChar buffer[MAXINPUT];
	SQInteger blocks =0;
	SQInteger string=0;
	SQInteger retval=0;
	SQInteger done=0;
	PrintVersionInfos();

	sq_pushroottable(v);
	sq_pushstring(v,_SC("quit"),-1);
	sq_pushuserpointer(v,&done);
	sq_newclosure(v,quit,1);
	sq_setparamscheck(v,1,NULL);
	sq_newslot(v,-3,SQFalse);
	sq_pop(v,1);

    while (!done)
	{
		SQInteger i = 0;
		scprintf(_SC("\nsq>"));
		for(;;) {
			int c;
			if(done)return;
			c = getchar();
			if (c == _SC('\n')) {
				if (i>0 && buffer[i-1] == _SC('\\'))
				{
					buffer[i-1] = _SC('\n');
				}
				else if(blocks==0)break;
				buffer[i++] = _SC('\n');
			}
			else if (c==_SC('}')) {blocks--; buffer[i++] = (SQChar)c;}
			else if(c==_SC('{') && !string){
					blocks++;
					buffer[i++] = (SQChar)c;
			}
			else if(c==_SC('"') || c==_SC('\'')){
					string=!string;
					buffer[i++] = (SQChar)c;
			}
			else if (i >= MAXINPUT-1) {
				scfprintf(stderr, _SC("sq : input line too long\n"));
				break;
			}
			else{
				buffer[i++] = (SQChar)c;
			}
		}
		buffer[i] = _SC('\0');

		if(buffer[0]==_SC('=')){
			scsprintf(sq_getscratchpad(v,MAXINPUT),_SC("return (%s)"),&buffer[1]);
			memcpy(buffer,sq_getscratchpad(v,-1),(scstrlen(sq_getscratchpad(v,-1))+1)*sizeof(SQChar));
			retval=1;
		}
		i=scstrlen(buffer);
		if(i>0){
			SQInteger oldtop=sq_gettop(v);
			if(SQ_SUCCEEDED(sq_compilebuffer(v,buffer,i,_SC("interactive console"),SQTrue))){
				sq_pushroottable(v);
				if(SQ_SUCCEEDED(sq_call(v,1,retval,SQTrue)) &&	retval){
					scprintf(_SC("\n"));
					sq_pushroottable(v);
					sq_pushstring(v,_SC("print"),-1);
					sq_get(v,-2);
					sq_pushroottable(v);
					sq_push(v,-4);
					sq_call(v,2,SQFalse,SQTrue);
					retval=0;
					scprintf(_SC("\n"));
				}
			}

			sq_settop(v,oldtop);
		}
	}
}

#define SCRIPT_END_TAG "<DAD:%ld>ooOo(^.^)oOoo"
// end of file is 29: "<DAD:%010d>ooOo(^.^)oOoo", script_len
#define END_TAG_LEN 29

static SQInteger LoadFrozenScript(HSQUIRRELVM v, const SQChar* filename, int only_check)
{
    SQInteger retval;
    // lots of debugging to make sure that everything is ok
    //printf("%s\n", filename);
    FILE *f = fopen(filename, "rb");

    if (!f) return -1;


    if (fseek(f, 0, SEEK_END))
    {
        fclose(f);
        return -1;
    }

	int fileSize = ftell(f);
	//printf("%d\n", fileSize);
    if (fseek(f, fileSize-END_TAG_LEN, SEEK_SET ))
    {
        fclose(f);
        return -1;
    }

    // do some sanity checking before reading the script length

    char tag_buf[END_TAG_LEN+2] = {0};
    memset(tag_buf, 0, sizeof(char)*(END_TAG_LEN+2));
    long script_len = 0;

    if (fread((void*)tag_buf, 1, END_TAG_LEN, f) != END_TAG_LEN)
    {
        fclose(f);
        return -1;
    }

	//printf("%s\n", tag_buf);
    if (sscanf(tag_buf, SCRIPT_END_TAG, &script_len) != 1)
    {
        fclose(f);

        // they only wanted to know if the script exists, assume it's valid
        if (only_check) return -2;
        else return -1;
    }
    else if (fseek(f, fileSize-END_TAG_LEN - script_len, SEEK_SET ))
    {
        fclose(f);
        return -1;
    }

    // size should be valid from Seek statement
    char *script = (char*)sq_malloc(script_len+10);
    if (script)
    {
        memset(script, 0, sizeof(script_len+10));
        fread(script, 1, script_len, f);
        script[script_len] = 0;
		//printf("%s", script);
    }

    fclose(f);

    // we finally have our script!
    //fwrite(script, 1, finalSize, f);
    //fclose(f);

    if(SQ_SUCCEEDED(sq_compilebuffer(v,script, script_len, SQFalse, SQTrue))) {
        int i, callargs = 1;
        sq_pushroottable(v);
        for(i=0;i<sq_main_argc;i++)
        {
            const SQChar *a;
#ifdef SQUNICODE
            int alen=(int)strlen(sq_main_argv[i]);
            a=sq_getscratchpad(v,(int)(alen*sizeof(SQChar)));
            mbstowcs(sq_getscratchpad(v,-1),sq_main_argv[i],alen);
            sq_getscratchpad(v,-1)[alen] = _SC('\0');
#else
            a=sq_main_argv[i];
#endif
            sq_pushstring(v,a,-1);
            callargs++;
            //sq_arrayappend(v,-2);
        }
        if(SQ_SUCCEEDED(sq_call(v,callargs,SQTrue,SQTrue))) {
            SQObjectType type = sq_gettype(v,-1);
            if(type == OT_INTEGER) {
                sq_getinteger(v,-1, &retval);
            }
            return _DONE;
        }
        else{
            return _ERROR;
        }
    }

    free(script);

    return 0;
}


SQRESULT sqext_register_sqfs(HSQUIRRELVM v);
SQRESULT sqext_register_sq_zmq3(HSQUIRRELVM v);
SQRESULT sqext_register_sq_socket(HSQUIRRELVM v);
SQRESULT sqext_register_sq_zlib(HSQUIRRELVM v);
SQRESULT sqext_register_mongoose(HSQUIRRELVM v);
SQRESULT sqrat_register_importlib(HSQUIRRELVM v);
SQRESULT sqext_register_sq_slave_vm(HSQUIRRELVM v);
SQRESULT sqext_register_axtls (HSQUIRRELVM v);
SQRESULT sqext_register_base64(HSQUIRRELVM v);
SQRESULT sqext_register_Sq_Fpdf(HSQUIRRELVM v);
SQRESULT sqext_register_SQLite3(HSQUIRRELVM v);
SQRESULT sqext_register_mix (HSQUIRRELVM sqvm);
SQRESULT sqext_register_rs232(HSQUIRRELVM v);
SQRESULT sqext_register_tinyxml2(HSQUIRRELVM v);
SQRESULT sqext_register_decimal(HSQUIRRELVM v);
SQRESULT sqext_register_markdown(HSQUIRRELVM v);
SQRESULT sqext_register_PostgreSQL(HSQUIRRELVM v);
SQRESULT sqext_register_Java(HSQUIRRELVM v);

int main(int argc, char* argv[])
{
    sq_main_argc = argc;
    sq_main_argv = argv;

	HSQUIRRELVM v;
	SQInteger retval = 0;
	//const SQChar *filename=NULL;
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetAllocHook(MemAllocHook);
#endif

	v=sq_open(1024);
	sq_setprintfunc(v,printfunc,errorfunc);

	sq_pushroottable(v);

	sqstd_register_bloblib(v);
	sqstd_register_iolib(v);
	sqstd_register_systemlib(v);
	sqstd_register_mathlib(v);
	sqstd_register_stringlib(v);

#ifdef WITH_DAD_EXTRAS
	sqext_register_base64(v);
	sqext_register_Sq_Fpdf(v);
	sqext_register_SQLite3(v);
	sqext_register_PostgreSQL(v);
	//sqext_register_dad_utils(v);
	sqext_register_mix(v);
	sqext_register_sqfs(v);
	sqext_register_sq_socket(v);
	sqext_register_axtls(v);
	sqext_register_sq_zlib(v);
	sqext_register_mongoose(v);
	sqrat_register_importlib(v);
	sqext_register_sq_slave_vm(v);
	sqext_register_tinyxml2(v);
	sqext_register_decimal(v);
	sqext_register_markdown(v);

	sqext_register_sq_zmq3(v);
	sqext_register_Java(v);

	sqext_register_rs232(v);
#ifdef WITH_FLTK
	sqext_register_fltklib(v);
#endif
#endif
	//aux library
	//sets error handlers
	sqstd_seterrorhandlers(v);

    //frozen script executed ?
    if(LoadFrozenScript(v, argv[0], 0) == _DONE) return 0;

	//gets arguments
	switch(getargs(v,argc,argv,&retval))
	{
	case _INTERACTIVE:
		Interactive(v);
		break;
	case _DONE:
	case _ERROR:
	default:
		break;
	}

	sq_close(v);

#if defined(_MSC_VER) && defined(_DEBUG)
	_getch();
	_CrtMemDumpAllObjectsSince( NULL );
#endif
	return retval;
}

