#ifdef SQ_USE_PCRE

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

#include <pcre.h>

////////
#include "dynamic_library.h"

/*SquiLu

local library_functions = [
    ["pcre *", "pcre_compile", "const char *, int, const char **, int *, const unsigned char *"],
    ["pcre *", "pcre_compile2", "const char *, int, int *, const char **, int *, const unsigned char *"],
    ["int", "pcre_config", "int, void *"],
    ["int", "pcre_fullinfo", "const pcre *, const pcre_extra *, int, void *"],
    ["int", "pcre_dfa_exec", "const pcre *, const pcre_extra *, const char *, int, int, int, int *, int , int *, int"],
    ["int", "pcre_exec", "const pcre *, const pcre_extra *, PCRE_SPTR, int, int, int, int *, int"],
    ["pcre_extra*", "pcre_study", "const pcre *, int, const char **"],
    ["void", "pcre_free_study", "pcre_extra *"],

    //next entry should be the last one
    //to make valid the test made on load_library function
    ["const char *", "pcre_version", "void"],
];

function write_library_functions_declaration(){
    foreach(k,v in library_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_library_functions_load(){
    foreach(k,v in library_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libdyn.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary libdyn;

//@write_library_functions_declaration();
// generated-code:begin
typedef pcre * (*pcre_compile_t)(const char *, int, const char **, int *, const unsigned char *);
static pcre_compile_t dlpcre_compile = 0;
typedef pcre * (*pcre_compile2_t)(const char *, int, int *, const char **, int *, const unsigned char *);
static pcre_compile2_t dlpcre_compile2 = 0;
typedef int (*pcre_config_t)(int, void *);
static pcre_config_t dlpcre_config = 0;
typedef int (*pcre_fullinfo_t)(const pcre *, const pcre_extra *, int, void *);
static pcre_fullinfo_t dlpcre_fullinfo = 0;
typedef int (*pcre_dfa_exec_t)(const pcre *, const pcre_extra *, const char *, int, int, int, int *, int , int *, int);
static pcre_dfa_exec_t dlpcre_dfa_exec = 0;
typedef int (*pcre_exec_t)(const pcre *, const pcre_extra *, PCRE_SPTR, int, int, int, int *, int);
static pcre_exec_t dlpcre_exec = 0;
typedef pcre_extra* (*pcre_study_t)(const pcre *, int, const char **);
static pcre_study_t dlpcre_study = 0;
typedef void (*pcre_free_study_t)(pcre_extra *);
static pcre_free_study_t dlpcre_free_study = 0;
typedef const char * (*pcre_version_t)(void);
static pcre_version_t dlpcre_version = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libpcre);

static bool load_library(const char *libname)
{
    if(dlpcre_version) return true;
    if(libdyn.open(libname))
    {
        //@write_library_functions_load();
// generated-code:begin
dlpcre_compile = (pcre_compile_t) libdyn.dlsym("pcre_compile");
if(!dlpcre_compile) return false;
dlpcre_compile2 = (pcre_compile2_t) libdyn.dlsym("pcre_compile2");
if(!dlpcre_compile2) return false;
dlpcre_config = (pcre_config_t) libdyn.dlsym("pcre_config");
if(!dlpcre_config) return false;
dlpcre_fullinfo = (pcre_fullinfo_t) libdyn.dlsym("pcre_fullinfo");
if(!dlpcre_fullinfo) return false;
dlpcre_dfa_exec = (pcre_dfa_exec_t) libdyn.dlsym("pcre_dfa_exec");
if(!dlpcre_dfa_exec) return false;
dlpcre_exec = (pcre_exec_t) libdyn.dlsym("pcre_exec");
if(!dlpcre_exec) return false;
dlpcre_study = (pcre_study_t) libdyn.dlsym("pcre_study");
if(!dlpcre_study) return false;
dlpcre_free_study = (pcre_free_study_t) libdyn.dlsym("pcre_free_study");
if(!dlpcre_free_study) return false;
dlpcre_version = (pcre_version_t) libdyn.dlsym("pcre_version");
if(!dlpcre_version) return false;
// generated-code:end

        return true;
    }
    return false;
}

static SQInteger calc_new_size_by_max_len(SQInteger start_pos, SQInteger max_len, SQInteger curr_size)
{
    SQInteger new_size;
    if(start_pos < 0)
    {
        new_size = curr_size + start_pos;
        start_pos = new_size < 0 ? 0 : new_size;
    }
    if(max_len > 0) new_size = start_pos + max_len;
    else new_size = curr_size + max_len;
    if( (new_size < curr_size) && (new_size > start_pos) )
    {
        return new_size;
    }
    return curr_size;
}

struct sqpcre_st {
    pcre *re;
    pcre_extra *re_extra;
    size_t ovector_size;
    int ovector[1];
};

static const SQChar PCRE_Tag[]   = _SC("sqpcre");
#define GET_pcre_INSTANCE() SQ_GET_INSTANCE(v, 1, sqpcre_st, PCRE_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("sqpcre object already closed"));

static SQRESULT sq_pcre_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	sqpcre_st *self = ((sqpcre_st *)p);
	if(self)
    {
        free(self->re);
        if(self->re_extra) dlpcre_free_study(self->re_extra);
        sq_free(self, sizeof(sqpcre_st));
    }
	return 1;
}

static SQRESULT sq_pcre_constructor(HSQUIRRELVM v)
{
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libpcre !"));
	SQ_FUNC_VARS(v);
	SQ_GET_STRING(v, 2, pattern);
	SQ_OPT_INTEGER(v, 3, options, 0);
	const SQChar *error;
	int erroffset;
	pcre *re = dlpcre_compile(
            pattern,              /* the pattern */
            options,              /* 0 = default options */
            &error,               /* for error message */
            &erroffset,           /* for error offset */
            NULL);                /* use default character tables */
	if(!re) return sq_throwerror(v,_SC("PCRE compilation failed at offset %d: %s"), erroffset, error);

    int back_ref_max;
    int capture_count;
    int rc = dlpcre_fullinfo(re, NULL, PCRE_INFO_BACKREFMAX, &back_ref_max);
    rc = dlpcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &capture_count);
    //printf("%d : %d : %d\n", __LINE__, back_ref_max, capture_count);

    size_t ovector_size = back_ref_max + capture_count + 1;
    //NP1_ASSERT(ovector_size < NP1_SIZE_T_MAX/3, "Too many back references and/or captures");
    ovector_size *= 3;
    size_t alloc_size = sizeof(sqpcre_st) + (ovector_size * sizeof(int));

	sqpcre_st *sqpcre = (sqpcre_st *)sq_malloc(alloc_size);
	sqpcre->re = re;
	sqpcre->re_extra = NULL;
	sqpcre->ovector_size = ovector_size;

	sq_setinstanceup(v,1,sqpcre);
	sq_setreleasehook(v,1,sq_pcre_releasehook);
	return 0;
}

static SQRESULT sq_pcre_study(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre_INSTANCE();
	SQ_OPT_INTEGER(v, 2, options, 0);
    const char *errptr = 0;
    self->re_extra = dlpcre_study(self->re, options, &errptr);

	sq_pushbool(v, self->re_extra != NULL);
	return 1;
}

#define OVECCOUNT 30    /* should be a multiple of 3 */

static SQRESULT sq_pcre_exec(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 4, start_offset, 0);
    SQ_OPT_INTEGER(v, 5, options, 0);
    SQ_OPT_INTEGER(v, 6, max_len, 0);

    if(max_len)
    {
        subject_size = calc_new_size_by_max_len(start_offset, max_len, subject_size);
    }

    int rc = dlpcre_exec(
        self->re,             /* the compiled pattern */
        self->re_extra,       /* no extra data - we didn't study the pattern */
        subject,              /* the subject string */
        subject_size,         /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->ovector,        /* output vector for substring information */
        self->ovector_size);  /* number of elements in the output vector */

    const int array_pos = 3;
    SQInteger rtype = sq_gettype(v, array_pos);

    /* The output vector wasn't big enough */

    if (rc > 0)
    {
        if(rtype == OT_ARRAY)
        {
            int nelms = rc*2;
            sq_clear(v, array_pos);
            for (int i = 0; i < nelms; i++)
            {
                SQInteger pos = self->ovector[i];
                if(pos < 0) continue; //forget defined subroutines
                sq_pushinteger(v, pos);
                sq_arrayappend(v, array_pos);
            }
        }
    }

    sq_pushinteger(v, rc);
    return 1;
}

static SQRESULT sq_pcre_match(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_pcre_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 3, start_offset, 0);
    SQ_OPT_INTEGER(v, 4, options, 0);
    SQ_OPT_INTEGER(v, 5, max_len, 0);

    if(max_len)
    {
        subject_size = calc_new_size_by_max_len(start_offset, max_len, subject_size);
    }

    int rc = dlpcre_exec(
        self->re,             /* the compiled pattern */
        self->re_extra,       /* no extra data - we didn't study the pattern */
        subject,              /* the subject string */
        subject_size,         /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->ovector,        /* output vector for substring information */
        self->ovector_size);  /* number of elements in the output vector */

    if(rc > 0)
    {
        SQInteger start_pos = self->ovector[0], end_pos = self->ovector[1];
        if(start_pos == end_pos) sq_pushinteger(v, start_pos);
        else sq_pushstring(v, subject + start_pos, end_pos - start_pos);
        return 1;
    }
    if(rc < -2) //only no matching errore
    {
        return sq_throwerror(v, _SC("pcre2_match error %d"), (int)rc);
    }
    return 0;
}

static SQRESULT sq_pcre_gmatch(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_pcre_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 4, start_offset, 0);
    SQ_OPT_INTEGER(v, 5, options, 0);
    SQ_OPT_INTEGER(v, 6, max_len, 0);

    if(max_len)
    {
        subject_size = calc_new_size_by_max_len(start_offset, max_len, subject_size);
    }

    SQInteger rc;
    bool isFirst = true;

    while( (rc = dlpcre_exec(
        self->re,             /* the compiled pattern */
        self->re_extra,       /* no extra data - we didn't study the pattern */
        subject,              /* the subject string */
        subject_size,         /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->ovector,        /* output vector for substring information */
        self->ovector_size)) > 0)   /* number of elements in the output vector */
    {
        if(isFirst)
        {
            sq_push(v, 3); //push the function
            isFirst = false;
        }
        sq_pushroottable(v); //this
        SQInteger start_pos, end_pos, ov_offset = 0, i = 0,
            param_count = 1; //root table already on the stack
        for(;i < rc; i++) {
            ov_offset = i*2;
            start_pos = self->ovector[ov_offset];
            if(start_pos < 0) continue;
            end_pos = self->ovector[ov_offset+1];
            if(start_pos == end_pos) sq_pushinteger(v, start_pos);
            else sq_pushstring(v, subject + start_pos, end_pos - start_pos);
            ++param_count;
        }
        i = sq_call(v, rc+1, SQTrue, SQTrue);
        if(i < 0) return i;

        SQObjectType rtype = sq_gettype(v, -1);
        SQBool keep_matching = SQFalse;
        if(rtype == OT_BOOL) {
            sq_getbool(v, -1, &keep_matching);
        }
        sq_poptop(v); //remove return parameter

        if(!keep_matching) break;

        start_offset = self->ovector[(rc*2)-1]; //the last match + 1
    }
    if(rc < -2) //only no matching errore
    {
        return sq_throwerror(v, _SC("pcre2_match error %d"), (int)rc);
    }
    return 0;
}

#include "sqstdblobimpl.h"
static SQRESULT sq_pcre_gsub(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre_INSTANCE();
    SQ_GET_STRING(v, 2, str);
    SQ_OPT_INTEGER(v, 4, options, 0);
    SQ_OPT_INTEGER(v, 5, max_sub, 0);
    if(max_sub < 0) return sq_throwerror(v, _SC("max substitutions can't be less than zero"));

	SQBlob blob(0,8192);
	const int replacement_idx = 3;
	SQObjectType ptype = sq_gettype(v, replacement_idx);
    const SQChar *replacement;
    SQInteger replacement_size, start_offset=0;

    SQInteger rc;
    bool isFirst = true;

    while( (rc = dlpcre_exec(
        self->re,             /* the compiled pattern */
        self->re_extra,       /* no extra data - we didn't study the pattern */
        str,              /* the subject string */
        str_size,         /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->ovector,        /* output vector for substring information */
        self->ovector_size)) > 0)   /* number of elements in the output vector */
    {
        SQInteger i, ov_offset, start_pos, end_pos;
	    blob.Write(str+start_offset, self->ovector[0]-start_offset);
	    switch(ptype){
	        case OT_CLOSURE:{
                if(isFirst)
                {
                    sq_push(v, replacement_idx); //push the function
                    isFirst = false;
                }
                sq_pushroottable(v); //this
                SQInteger param_count = 1; //root table
                for(i=0; i < rc; i++) {
                    ov_offset = i*2;
                    start_pos = self->ovector[ov_offset];
                    if(start_pos < 0) continue; //defined subroutines not pushed as parameter
                    end_pos = self->ovector[ov_offset+1];
                    if(start_pos == end_pos) sq_pushinteger(v, start_pos);
                    else sq_pushstring(v, str + start_pos, end_pos - start_pos);
                    ++param_count;
                }
                i = sq_call(v, param_count, SQTrue, SQTrue);
                if(i < 0) return i;
                if(sq_gettype(v, -1) == OT_STRING){
                    const SQChar *svalue;
                    sq_getstring(v, -1, &svalue);
                    blob.Write(svalue, sq_getsize(v, -1));
                }
                sq_poptop(v);
	        }
	        break;
	        case OT_ARRAY:{
                SQInteger array_idx = 0;
                for(i=0; i < rc; i++) {
                    ov_offset = i*2;
                    SQInteger pos = self->ovector[ov_offset];
                    if(pos < 0) continue; //forget defined subroutines
                    sq_pushinteger(v, array_idx++);
                    if(SQ_SUCCEEDED(sq_get(v, replacement_idx)) &&
                            SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &replacement, &replacement_size))){
                        blob.Write(replacement, replacement_size);
                        sq_pop(v, 1); //remove value
                    }
                }
	        }
	        break;
	        case OT_TABLE:{
                for(i=0; i < rc; i++) {
                    ov_offset = i*2;
                    start_pos = self->ovector[ov_offset];
                    if(start_pos < 0) continue;
                    end_pos = self->ovector[ov_offset+1];
                    sq_pushstring(v, str + start_pos, end_pos - start_pos);
                    if(SQ_SUCCEEDED(sq_get(v, replacement_idx)) &&
                            SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &replacement, &replacement_size))){
                        blob.Write(replacement, replacement_size);
                        sq_pop(v, 1); //remove value
                    }
                }
	        }
	        break;
	        case OT_STRING:{
	            sq_getstr_and_size(v, replacement_idx, &replacement, &replacement_size);

                for(i=0; i < replacement_size; i++) {
                    SQInteger c = replacement[i];
                    switch(c)
                    {
                    case '$':
                        ++i;
                        if(i < replacement_size)
                        {
                            SQInteger idx = replacement[i] - '0', match_idx = 0;
                            for(int j=0; j < rc; j++) {
                                ov_offset = j*2;
                                start_pos = self->ovector[ov_offset];
                                if(start_pos < 0) continue;
                                if(match_idx == idx)
                                {
                                    end_pos = self->ovector[ov_offset+1];
                                    blob.Write(str+start_pos, end_pos-start_pos);
                                    break;
                                }
                                ++match_idx;
                            }
                            if(idx != match_idx)
                            {
                                return sq_throwerror(v, _SC("there is no match for replacement $" _PRINT_INT_FMT), idx);
                            }
                            continue;
                        }
                        else
                        {
                            return sq_throwerror(v, _SC("unexpected end of replacement string"));
                        }
                        break;
                    case '\\':
                        ++i;
                        if(i < replacement_size)
                        {
                            blob.WriteChar(replacement[i]);
                            continue;
                        }
                        //falthrough last character on replacement string
                    default:
                        blob.WriteChar(c);
                    }
                }
	        }
	        break;
	        default:
                return sq_throwerror(v, _SC("gsub only works with closure, array, table for replacement"));
	    }
		start_offset = self->ovector[(rc*2)-1]; //the last match + 1
        if(max_sub)
        {
            if(--max_sub == 0) break;
        }
	}

    if(rc < -2) //only no matching errore
    {
        return sq_throwerror(v, _SC("pcre_match error %d"), (int)rc);
    }

    if(str_size) blob.Write(str+start_offset, str_size-start_offset);
	sq_pushstring(v, (const SQChar *)blob.GetBuf(), blob.Len());
	return 1;
}

static SQRESULT sq_pcre__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC("sqpcre"),-1);
	return 1;
}

static SQRESULT sq_pcre_version(HSQUIRRELVM v)
{
	sq_pushstring(v, dlpcre_version(),-1);
	return 1;
}

static SQRESULT sq_pcre_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, libname);
    sq_pushbool(v, load_library(libname));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_pcre_##name,nparams,tycheck}
static SQRegFunction sq_pcre_methods[] =
{
	_DECL_FUNC(constructor,-2,_SC(".sn")),
	_DECL_FUNC(study,-1,_SC("xn")),
	_DECL_FUNC(exec,-3,_SC("xsannn")),
	_DECL_FUNC(match,-2,_SC("xsnnn")),
	_DECL_FUNC(gmatch,-3,_SC("xscnnn")),
	_DECL_FUNC(gsub,-3,_SC("xs s|c|a|t nn")),
	_DECL_FUNC(_typeof,1,_SC("x")),
    _DECL_FUNC(version,1,_SC(".")),
    _DECL_FUNC(loadlib,2,_SC(".s")),
    {0,0}
};
#undef _DECL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType sqpcre_constants[] = {
    #define MK_CONST(c) {_SC(#c), PCRE_##c}
    #define MK_CONST2(c) {_SC(c), (SQInteger)PCRE_##c}
    //MK_CONST(SSL_SESSION_ID_SIZE),

	MK_CONST(ANCHORED),
	MK_CONST(NOTBOL),
	MK_CONST(NOTEOL),
	MK_CONST(NOTEMPTY),
	MK_CONST(NOTEMPTY_ATSTART),
	MK_CONST(NO_START_OPTIMIZE),
	MK_CONST(PARTIAL_HARD),
	MK_CONST(PARTIAL_SOFT),
	MK_CONST(NEWLINE_CR),
	MK_CONST(NEWLINE_LF),
	MK_CONST(NEWLINE_CRLF),
	MK_CONST(NEWLINE_ANYCRLF),
	MK_CONST(NEWLINE_ANY),
	MK_CONST(NO_UTF8_CHECK),
	MK_CONST(STUDY_JIT_COMPILE),
	MK_CONST(STUDY_JIT_PARTIAL_HARD_COMPILE),
	MK_CONST(STUDY_JIT_PARTIAL_SOFT_COMPILE),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_pcre(HSQUIRRELVM v)
{
    sq_pushstring(v,PCRE_Tag,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PCRE_Tag);
    sq_insert_reg_funcs(v, sq_pcre_methods);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = sqpcre_constants; KeyIntPtr->Str; KeyIntPtr++) {
		sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
		sq_pushinteger(v, KeyIntPtr->Val);       //then the value
		sq_newslot(v, -3, SQFalse);              //store then
	}

    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif //SQ_USE_PCRE
