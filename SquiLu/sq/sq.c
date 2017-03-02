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


#define MAXINPUT 1024

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
	(void)v; /* UNUSED */
}

void errorfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stderr, s, vl);
	va_end(vl);
	(void)v; /* UNUSED */
}

void PrintVersionInfos()
{
	scfprintf(stdout,_SC("%s %s (%d bits)\n"),SQUIRREL_VERSION,SQUIRREL_COPYRIGHT,((int)(sizeof(SQInteger)*8)));
}

SQInteger push_program_args(HSQUIRRELVM v,SQInteger arg, SQInteger argc, char* argv[], int asArray){
    SQInteger i, callargs = 0;
    if(asArray)
    {
        sq_newarray(v, 0);
    }
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
        if(asArray) sq_arrayappend(v, -2);
        callargs++;
        //sq_arrayappend(v,-2);
    }
    return callargs;
}

void PrintUsage()
{
	scfprintf(stderr,_SC("usage: sq <options> <scriptpath [args]>.\n")
		_SC("Available options are:\n")
		_SC("   -s              compiles the file to bytecode(default output 'out.cnut')\n")
		_SC("   -o              specifies output file for the -c option\n")
		_SC("   -c              compiles only\n")
		_SC("   -d              generates debug infos\n")
		_SC("   -v              displays version infos\n")
		_SC("   -p              preload given script file\n")
		_SC("   -h              prints help\n"));
}

void loadDefaultScript(HSQUIRRELVM v, const char *script)
{
    FILE *fb = scfopen(script, "rb");
    if (!fb) return;
    fclose(fb);

    SQChar srcBoot[256];
    scsprintf(srcBoot, sizeof(srcBoot), _SC("dofile(\"%s\", false);"), script);

    if(SQ_SUCCEEDED(sq_compilebuffer(v,srcBoot, strlen(srcBoot), _SC("defaultScript"), SQTrue, SQTrue, SQ_MAX_INCLUDE_FILES))) {
        int callargs = 1;
        sq_pushroottable(v);
        callargs += push_program_args(v, 0, sq_main_argc, sq_main_argv, 0);
        sq_call(v, callargs,SQFalse, SQTrue);
    }
}

#define _INTERACTIVE 0
#define _DONE 2
#define _ERROR 3
//<<FIXME>> this func is a mess
int getargs(HSQUIRRELVM v,int argc, char* argv[],SQInteger *retval)
{
	int compiles_only = 0;
	int compiles_as_source_only = 0;
#ifdef SQUNICODE
	static SQChar temp[500];
#endif
	const char * output = NULL;
	const char * preload = NULL;
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
				case 'p':
					if(arg < argc) {
						arg++;
						preload = argv[arg];
                        loadDefaultScript(v, preload);
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

			//arg++; //commenting this line to pass the script filename as first parameter in vargv

			//sq_pushstring(v,_SC("ARGS"),-1);
			//sq_newarray(v,0);

			//sq_createslot(v,-3);
			//sq_pop(v,1);
			if(compiles_only) {
				if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,SQTrue,SQTrue))){
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
				if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,SQTrue,SQTrue))){
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
				if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,SQTrue,SQTrue))) {
					int callargs = 1;
					sq_pushroottable(v);
					callargs += push_program_args(v, arg, argc, argv, 0);
					if(SQ_SUCCEEDED(sq_call(v,callargs,SQTrue,SQTrue))) {
						SQObjectType type = sq_gettype(v,-1);
						if(type == OT_INTEGER) {
							*retval = type;
							sq_getinteger(v,-1,retval);
						}
						//if there is a function called "main" we call it like in C/C++
						sq_pushliteral(v, _SC("main"));
						if(sq_getonroottable(v) == SQ_OK)
                        {
                            if(sq_gettype(v, -1) == OT_CLOSURE)
                            {
                                sq_pushroottable(v);
                                sq_pushinteger(v, 1);
                                callargs = push_program_args(v, arg, argc, argv, 1);
                                sq_pushinteger(v, callargs);
                                sq_replace(v, -3);
                                if(SQ_SUCCEEDED(sq_call(v,3,SQTrue,SQTrue))) {
                                    SQObjectType type = sq_gettype(v,-1);
                                    if(type == OT_INTEGER) {
                                        *retval = type;
                                        sq_getinteger(v,-1,retval);
                                    }
                                }
                            }
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
			scsprintf(sq_getscratchpad(v,MAXINPUT),MAXINPUT,_SC("return (%s)"),&buffer[1]);
			memcpy(buffer,sq_getscratchpad(v,-1),(scstrlen(sq_getscratchpad(v,-1))+1)*sizeof(SQChar));
			retval=1;
		}
		i=scstrlen(buffer);
		if(i>0){
			SQInteger oldtop=sq_gettop(v);
			if(SQ_SUCCEEDED(sq_compilebuffer(v,buffer,i,_SC("interactive console"),SQTrue, SQTrue, SQ_MAX_INCLUDE_FILES))){
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

#if 0
static SQInteger LoadFrozenScript0(HSQUIRRELVM v, const SQChar* filename, int only_check)
{
    SQInteger retval;
    // lots of debugging to make sure that everything is ok
    //printf("%s\n", filename);
    FILE *f = scfopen(filename, "rb");

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

    if(SQ_SUCCEEDED(sq_compilebuffer(v,script, script_len, _SC("frozenScript"), SQTrue))) {
        int callargs = 1;
        sq_pushroottable(v);
        callargs += push_program_args(v, 0, sq_main_argc, sq_main_argv, 0);
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
#endif

static char *chngChar (char *str, char oldChar, char newChar) {
    char *strPtr = str;
    while ((strPtr = strchr (strPtr, oldChar)) != NULL)
        *strPtr++ = newChar;
    return str;
}

static SQInteger LoadFrozenScript(HSQUIRRELVM v, const SQChar* filename, int only_check)
{
#ifndef SQUILU_ALONE
    // lots of debugging to make sure that everything is ok
    //printf("%s\n", filename);
    FILE *f = scfopen(filename, "rb");

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

    fclose(f);
    SQChar srcBoot[256];
    SQInteger scr_len = scsprintf(srcBoot, sizeof(srcBoot),
            _SC("local __fd=file(\"%s\", \"rb\");__fd.seek(%d, 'b');local __zsrc=__fd.read(%d);__fd.close();__zsrc=compilestring(zlib.inflate(__zsrc),\"zsrc\",false);__zsrc.acall2(this, vargv);"),
            filename, (int)(fileSize-END_TAG_LEN - script_len), (int)script_len);

    chngChar(srcBoot, '\\', '/');

    if(SQ_SUCCEEDED(sq_compilebuffer(v,srcBoot, scr_len, _SC("bootScript"), SQTrue, SQTrue, SQ_MAX_INCLUDE_FILES))) {
        int callargs = 1;
        sq_pushroottable(v);
        callargs += push_program_args(v, 0, sq_main_argc, sq_main_argv, 0);
        if(SQ_SUCCEEDED(sq_call(v, callargs,SQFalse, SQTrue))) {
            return _DONE;
        }
        else{
            return _ERROR;
        }
    }
#endif //SQUILU_ALONE
    return 0;
}


SQRESULT sqext_register_sqfs(HSQUIRRELVM v);
SQRESULT sqext_register_sq_zmq3(HSQUIRRELVM v);
SQRESULT sqext_register_sq_socket(HSQUIRRELVM v);
SQRESULT sqext_register_sq_zlib(HSQUIRRELVM v);
SQRESULT sqext_register_sq_miniz(HSQUIRRELVM v);
SQRESULT sqext_register_mongoose(HSQUIRRELVM v);
SQRESULT sqrat_register_importlib(HSQUIRRELVM v);
SQRESULT sqext_register_sq_slave_vm(HSQUIRRELVM v);
SQRESULT sqext_register_axtls (HSQUIRRELVM v);
SQRESULT sqext_register_openssl (HSQUIRRELVM v);
SQRESULT sqext_register_base64(HSQUIRRELVM v);
SQRESULT sqext_register_Sq_Fpdf(HSQUIRRELVM v);
SQRESULT sqext_register_SQLite3(HSQUIRRELVM v);
SQRESULT sqext_register_mix (HSQUIRRELVM sqvm);
SQRESULT sqext_register_rs232(HSQUIRRELVM v);
SQRESULT sqext_register_tinyxml2(HSQUIRRELVM v);
SQRESULT sqext_register_decimal(HSQUIRRELVM v);
SQRESULT sqext_register_markdown(HSQUIRRELVM v);
SQRESULT sqext_register_PostgreSQL(HSQUIRRELVM v);
SQRESULT sqext_register_MySQL(HSQUIRRELVM v);
SQRESULT sqext_register_Java(HSQUIRRELVM v);
SQRESULT sqext_register_ThreadObjects(HSQUIRRELVM v);
SQRESULT sqext_register_csv_parser (HSQUIRRELVM v);
SQRESULT sqext_register_fltklib(HSQUIRRELVM v);
SQRESULT sqext_register_dad_utils(HSQUIRRELVM v);
SQRESULT sqext_register_gumbo(HSQUIRRELVM v);
SQRESULT sqext_register_sys(HSQUIRRELVM v);
SQRESULT sqext_register_DNS_SD(HSQUIRRELVM v);
SQRESULT sqext_register_ffi(HSQUIRRELVM v);
SQRESULT sqext_register_xjd1(HSQUIRRELVM v);
SQRESULT sqext_register_libclang(HSQUIRRELVM v);
SQRESULT sqext_register_EasyCurl (HSQUIRRELVM v);
SQRESULT sqext_register_tweetnacl(HSQUIRRELVM v);
SQRESULT sqext_register_ipc (HSQUIRRELVM v);
SQRESULT sqext_register_BitVector (HSQUIRRELVM v); //depends on sqlite3
SQRESULT sqext_register_pack(HSQUIRRELVM v);
SQRESULT sqext_register_sq_blosc(HSQUIRRELVM v);
SQRESULT sqext_register_pcre(HSQUIRRELVM v);
SQRESULT sqext_register_pcre2(HSQUIRRELVM v);
SQRESULT sqext_register_freetype(HSQUIRRELVM v);

static sq_modules_preload_st modules_preload[] = {
    {"blob", sqstd_register_bloblib},
    {"io", sqstd_register_iolib},
    {"math", sqstd_register_mathlib},
    {"sys", sqstd_register_systemlib},
    {"string", sqstd_register_stringlib},
#ifdef WITH_DAD_EXTRAS
#ifndef SQUILU_ALONE
    {"sqlexer", sqext_register_SQLexer},
    {"gumbo", sqext_register_gumbo},
    {"base64", sqext_register_base64},
    {"fpdf", sqext_register_Sq_Fpdf},
    {"sqlite3", sqext_register_SQLite3},
    {"bitvector", sqext_register_BitVector}, //depends on sqlite3
    //{"xdj1", sqext_register_xjd1},
    {"mix", sqext_register_mix},
    {"sqfs", sqext_register_sqfs},
    //{"ipc", sqext_register_ipc},
    {"socket", sqext_register_sq_socket},
    {"tweetnacl", sqext_register_tweetnacl},
    {"pack", sqext_register_pack},
#ifndef TARGET_IOS
#ifdef SQ_USE_PCRE
    {"pcre", sqext_register_pcre},
#endif // SQ_USE_PCRE
#if defined(SQ_USE_PCRE2) || defined(SQ_USE_PCRE2_STATIC)
    {"pcre2", sqext_register_pcre2},
#endif // SQ_USE_PCRE2
#ifdef SQ_USE_FREETYPE
    {"freetype", sqext_register_freetype},
#endif // SQ_USE_FREETYPE
#ifdef SQ_USE_BLOSC
    {"blosc", sqext_register_sq_blosc},
#endif // SQ_USE_BLOSC
#endif
#ifdef USE_AXTLS
    {"axtls", sqext_register_axtls},
#endif
#ifdef WITH_FFI
    {"ffi", sqext_register_ffi},
#endif
#ifdef USE_OPENSSL
    {"openssl", sqext_register_openssl},
#endif
    {"zlib", sqext_register_sq_zlib},
    //{"miniz", sqext_register_sq_miniz},
    {"mongoose", sqext_register_mongoose},
    {"importlib", sqrat_register_importlib},
    {"tinyxml2", sqext_register_tinyxml2},
#ifndef _WIN32_WCE
#ifdef WITH_MPDECIMAL
    {"decimal", sqext_register_decimal},
#endif
    {"markdown", sqext_register_markdown},
#endif
    {"slave_vm", sqext_register_sq_slave_vm},
    //{"thread", sqext_register_ThreadObjects},
    {"dad_utils", sqext_register_dad_utils},
    //{"sys_extra", sqext_register_sys},
#ifdef SQ_USE_EASYCURL
    {"easycurl", sqext_register_EasyCurl},
#endif // SQ_USE_EASYCURL

#ifdef WITH_FULL_DAD_EXTRAS
    {"csv_parser", sqext_register_csv_parser},
    //{"zmq3", sqext_register_sq_zmq3},
    //{"java", sqext_register_Java},
#endif
#ifdef WITH_LIBCLANG
    {"libclang", sqext_register_libclang},
#endif
#ifdef WITH_POSTGRESQL
    {"postgresql", sqext_register_PostgreSQL},
#endif
#ifdef WITH_MYSQL
    {"mysql", sqext_register_MySQL},
#endif
    {"rs232", sqext_register_rs232},
#ifdef WITH_FLTK
    {"fltk", sqext_register_fltklib},
#endif
#ifdef WITH_DNS_SD
    {"dns_sd", sqext_register_DNS_SD},
#endif

#endif //SQUILU_ALONE
#endif
    {NULL, NULL}
};

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

#ifndef SQ_OPEN_VM_SIZE
#define SQ_OPEN_VM_SIZE 1024
#endif // SQ_OPEN_VM_SIZE
	v=sq_open(SQ_OPEN_VM_SIZE);
	sq_setprintfunc(v,printfunc,errorfunc);

	sq_pushroottable(v);
/*
    sq_pushstring(v, _SC("sq_xinterpreter"), -1);
    sq_pushstring(v, argv[0], -1);
    sq_newslot(v,-3,SQFalse);
*/
    sq_modules_preload_st *mp = modules_preload;
    sq_preload_modules(v, mp);

    while(mp->module_load_func){
      mp->module_load_func(v);
      ++mp;
    }

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

