#if defined(SQ_USE_PCRE2) || defined(SQ_USE_PCRE2_STATIC)

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

////////
#include "dynamic_library.h"

/*SquiLu

local library_functions = [
    ["pcre2_code *", "pcre2_compile", "PCRE2_SPTR, PCRE2_SIZE, uint32_t, int *, PCRE2_SIZE *, pcre2_compile_context *"],
    ["int", "pcre2_jit_compile", "pcre2_code *, uint32_t"],
    ["void", "pcre2_code_free", "pcre2_code *"],
    ["pcre2_match_data *", "pcre2_match_data_create_from_pattern", "const pcre2_code *, pcre2_general_context *"],
    ["PCRE2_SIZE *", "pcre2_get_ovector_pointer", "pcre2_match_data *match_data"],
    ["uint32_t", "pcre2_get_ovector_count", "pcre2_match_data *match_data"],
    ["void", "pcre2_match_data_free", "pcre2_match_data *"],
    ["int", "pcre2_match", "const pcre2_code *, PCRE2_SPTR, PCRE2_SIZE, PCRE2_SIZE, uint32_t, pcre2_match_data *, pcre2_match_context *"],
    ["int", "pcre2_dfa_match", "const pcre2_code *code, PCRE2_SPTR subject, PCRE2_SIZE length, PCRE2_SIZE startoffset, uint32_t options, pcre2_match_data *match_data, pcre2_match_context *mcontext, int *workspace, PCRE2_SIZE wscount"],
    ["int", "pcre2_get_error_message", "int, PCRE2_UCHAR *, PCRE2_SIZE"],
    ["pcre2_match_context *", "pcre2_match_context_create", "pcre2_general_context *gcontext"],
    ["pcre2_match_context *", "pcre2_match_context_copy", "pcre2_match_context *"],
    ["void", "pcre2_match_context_free", "pcre2_match_context *"],
    ["int", "pcre2_set_match_limit", "pcre2_match_context *mcontext, uint32_t value"],
    ["int", "pcre2_set_recursion_limit", "pcre2_match_context *mcontext, uint32_t value"],
    ["int", "pcre2_set_callout", "pcre2_match_context *, int (*)(pcre2_callout_block *, void *), void *"],
    ["int", "pcre2_substitute", "const pcre2_code *, PCRE2_SPTR, PCRE2_SIZE, PCRE2_SIZE, uint32_t, pcre2_match_data *, pcre2_match_context *, PCRE2_SPTR, PCRE2_SIZE, PCRE2_UCHAR *, PCRE2_SIZE *"],

    //next entry should be the last one
    //to make valid the test made on load_library function
    ["int", "pcre2_config", "uint32_t what, void *where"],
];

function write_library_functions_declaration(){
    foreach(k,v in library_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_library_functions_load(chsz){
    foreach(k,v in library_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libdyn.dlsym(\"" + v[1] + chsz + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}

function write_library_functions_static_defines(){
    foreach(k,v in library_functions){
        putsnl("#define dl" + v[1] + " " + v[1]);
    }
}
SquiLu*/

#ifdef SQ_USE_PCRE2_STATIC

#define load_library(x) true

//@write_library_functions_static_defines()
// generated-code:begin
#define dlpcre2_compile pcre2_compile
#define dlpcre2_jit_compile pcre2_jit_compile
#define dlpcre2_code_free pcre2_code_free
#define dlpcre2_match_data_create_from_pattern pcre2_match_data_create_from_pattern
#define dlpcre2_get_ovector_pointer pcre2_get_ovector_pointer
#define dlpcre2_get_ovector_count pcre2_get_ovector_count
#define dlpcre2_match_data_free pcre2_match_data_free
#define dlpcre2_match pcre2_match
#define dlpcre2_dfa_match pcre2_dfa_match
#define dlpcre2_get_error_message pcre2_get_error_message
#define dlpcre2_match_context_create pcre2_match_context_create
#define dlpcre2_match_context_copy pcre2_match_context_copy
#define dlpcre2_match_context_free pcre2_match_context_free
#define dlpcre2_set_match_limit pcre2_set_match_limit
#define dlpcre2_set_recursion_limit pcre2_set_recursion_limit
#define dlpcre2_set_callout pcre2_set_callout
#define dlpcre2_substitute pcre2_substitute
#define dlpcre2_config pcre2_config
// generated-code:end

#else

static DynamicLibrary libdyn;

//@write_library_functions_declaration();
// generated-code:begin
typedef pcre2_code * (*pcre2_compile_t)(PCRE2_SPTR, PCRE2_SIZE, uint32_t, int *, PCRE2_SIZE *, pcre2_compile_context *);
static pcre2_compile_t dlpcre2_compile = 0;
typedef int (*pcre2_jit_compile_t)(pcre2_code *, uint32_t);
static pcre2_jit_compile_t dlpcre2_jit_compile = 0;
typedef void (*pcre2_code_free_t)(pcre2_code *);
static pcre2_code_free_t dlpcre2_code_free = 0;
typedef pcre2_match_data * (*pcre2_match_data_create_from_pattern_t)(const pcre2_code *, pcre2_general_context *);
static pcre2_match_data_create_from_pattern_t dlpcre2_match_data_create_from_pattern = 0;
typedef PCRE2_SIZE * (*pcre2_get_ovector_pointer_t)(pcre2_match_data *match_data);
static pcre2_get_ovector_pointer_t dlpcre2_get_ovector_pointer = 0;
typedef uint32_t (*pcre2_get_ovector_count_t)(pcre2_match_data *match_data);
static pcre2_get_ovector_count_t dlpcre2_get_ovector_count = 0;
typedef void (*pcre2_match_data_free_t)(pcre2_match_data *);
static pcre2_match_data_free_t dlpcre2_match_data_free = 0;
typedef int (*pcre2_match_t)(const pcre2_code *, PCRE2_SPTR, PCRE2_SIZE, PCRE2_SIZE, uint32_t, pcre2_match_data *, pcre2_match_context *);
static pcre2_match_t dlpcre2_match = 0;
typedef int (*pcre2_dfa_match_t)(const pcre2_code *code, PCRE2_SPTR subject, PCRE2_SIZE length, PCRE2_SIZE startoffset, uint32_t options, pcre2_match_data *match_data, pcre2_match_context *mcontext, int *workspace, PCRE2_SIZE wscount);
static pcre2_dfa_match_t dlpcre2_dfa_match = 0;
typedef int (*pcre2_get_error_message_t)(int, PCRE2_UCHAR *, PCRE2_SIZE);
static pcre2_get_error_message_t dlpcre2_get_error_message = 0;
typedef pcre2_match_context * (*pcre2_match_context_create_t)(pcre2_general_context *gcontext);
static pcre2_match_context_create_t dlpcre2_match_context_create = 0;
typedef pcre2_match_context * (*pcre2_match_context_copy_t)(pcre2_match_context *);
static pcre2_match_context_copy_t dlpcre2_match_context_copy = 0;
typedef void (*pcre2_match_context_free_t)(pcre2_match_context *);
static pcre2_match_context_free_t dlpcre2_match_context_free = 0;
typedef int (*pcre2_set_match_limit_t)(pcre2_match_context *mcontext, uint32_t value);
static pcre2_set_match_limit_t dlpcre2_set_match_limit = 0;
typedef int (*pcre2_set_recursion_limit_t)(pcre2_match_context *mcontext, uint32_t value);
static pcre2_set_recursion_limit_t dlpcre2_set_recursion_limit = 0;
typedef int (*pcre2_set_callout_t)(pcre2_match_context *, int (*)(pcre2_callout_block *, void *), void *);
static pcre2_set_callout_t dlpcre2_set_callout = 0;
typedef int (*pcre2_substitute_t)(const pcre2_code *, PCRE2_SPTR, PCRE2_SIZE, PCRE2_SIZE, uint32_t, pcre2_match_data *, pcre2_match_context *, PCRE2_SPTR, PCRE2_SIZE, PCRE2_UCHAR *, PCRE2_SIZE *);
static pcre2_substitute_t dlpcre2_substitute = 0;
typedef int (*pcre2_config_t)(uint32_t what, void *where);
static pcre2_config_t dlpcre2_config = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libpcre2-8);

static bool load_library(const char *libname)
{
    if(dlpcre2_config) return true;
    if(libdyn.open(libname))
    {
        //@write_library_functions_load("_8");
// generated-code:begin
dlpcre2_compile = (pcre2_compile_t) libdyn.dlsym("pcre2_compile_8");
if(!dlpcre2_compile) return false;
dlpcre2_jit_compile = (pcre2_jit_compile_t) libdyn.dlsym("pcre2_jit_compile_8");
if(!dlpcre2_jit_compile) return false;
dlpcre2_code_free = (pcre2_code_free_t) libdyn.dlsym("pcre2_code_free_8");
if(!dlpcre2_code_free) return false;
dlpcre2_match_data_create_from_pattern = (pcre2_match_data_create_from_pattern_t) libdyn.dlsym("pcre2_match_data_create_from_pattern_8");
if(!dlpcre2_match_data_create_from_pattern) return false;
dlpcre2_get_ovector_pointer = (pcre2_get_ovector_pointer_t) libdyn.dlsym("pcre2_get_ovector_pointer_8");
if(!dlpcre2_get_ovector_pointer) return false;
dlpcre2_get_ovector_count = (pcre2_get_ovector_count_t) libdyn.dlsym("pcre2_get_ovector_count_8");
if(!dlpcre2_get_ovector_count) return false;
dlpcre2_match_data_free = (pcre2_match_data_free_t) libdyn.dlsym("pcre2_match_data_free_8");
if(!dlpcre2_match_data_free) return false;
dlpcre2_match = (pcre2_match_t) libdyn.dlsym("pcre2_match_8");
if(!dlpcre2_match) return false;
dlpcre2_dfa_match = (pcre2_dfa_match_t) libdyn.dlsym("pcre2_dfa_match_8");
if(!dlpcre2_dfa_match) return false;
dlpcre2_get_error_message = (pcre2_get_error_message_t) libdyn.dlsym("pcre2_get_error_message_8");
if(!dlpcre2_get_error_message) return false;
dlpcre2_match_context_create = (pcre2_match_context_create_t) libdyn.dlsym("pcre2_match_context_create_8");
if(!dlpcre2_match_context_create) return false;
dlpcre2_match_context_copy = (pcre2_match_context_copy_t) libdyn.dlsym("pcre2_match_context_copy_8");
if(!dlpcre2_match_context_copy) return false;
dlpcre2_match_context_free = (pcre2_match_context_free_t) libdyn.dlsym("pcre2_match_context_free_8");
if(!dlpcre2_match_context_free) return false;
dlpcre2_set_match_limit = (pcre2_set_match_limit_t) libdyn.dlsym("pcre2_set_match_limit_8");
if(!dlpcre2_set_match_limit) return false;
dlpcre2_set_recursion_limit = (pcre2_set_recursion_limit_t) libdyn.dlsym("pcre2_set_recursion_limit_8");
if(!dlpcre2_set_recursion_limit) return false;
dlpcre2_set_callout = (pcre2_set_callout_t) libdyn.dlsym("pcre2_set_callout_8");
if(!dlpcre2_set_callout) return false;
dlpcre2_substitute = (pcre2_substitute_t) libdyn.dlsym("pcre2_substitute_8");
if(!dlpcre2_substitute) return false;
dlpcre2_config = (pcre2_config_t) libdyn.dlsym("pcre2_config_8");
if(!dlpcre2_config) return false;
// generated-code:end

        return true;
    }
    return false;
}

#endif // SQ_USE_PCRE2_STATIC

typedef int (*pre2_callout_func_t)(pcre2_callout_block *, void *);

struct sqpcre2_st {
    pcre2_code *re;
    pcre2_match_context *match_context;
    pcre2_match_data *match_data;
    PCRE2_SIZE *ovector;

    /* associated squirrel vm */
    HSQUIRRELVM v;

    HSQOBJECT callout_cb;
    HSQOBJECT callout_cb_udata;
};

static const SQChar PCRE2_Tag[]   = _SC("sqpcre2");
#define GET_pcre2_INSTANCE() SQ_GET_INSTANCE(v, 1, sqpcre2_st, PCRE2_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("sqpcre2 object already closed"));

static SQRESULT sq_pcre2_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	sqpcre2_st *self = ((sqpcre2_st *)p);
	if(self)
    {
        if(self->match_data) dlpcre2_match_data_free(self->match_data);
        dlpcre2_code_free(self->re);
        if(self->match_context)
        {
            sq_release(self->v, &self->callout_cb);
            sq_release(self->v, &self->callout_cb_udata);
            dlpcre2_match_context_free(self->match_context);
        }

        sq_free(self, sizeof(*self));
    }
	return 1;
}

static SQRESULT sq_pcre2_constructor(HSQUIRRELVM v)
{
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libpcre !"));
	SQ_FUNC_VARS(v);
	SQ_GET_STRING(v, 2, pattern);
	SQ_OPT_INTEGER(v, 3, options, 0);
	PCRE2_SIZE erroroffset;
	int errornumber;
	pcre2_code *re = dlpcre2_compile(
            (PCRE2_SPTR)pattern,              /* the pattern */
            PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
            options,              /* 0 = default options */
            &errornumber,         /* for error message */
            &erroroffset,         /* for error offset */
            NULL);                /* use default character tables */
	if(!re)
    {
        PCRE2_UCHAR buffer[256];
        dlpcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        return sq_throwerror(v,_SC("PCRE compilation failed at offset %d: %s"), (int)erroroffset, buffer);
    }

	sqpcre2_st *sqpcre2 = (sqpcre2_st *)sq_malloc(sizeof(*sqpcre2));
	sqpcre2->re = re;
	sqpcre2->match_data = dlpcre2_match_data_create_from_pattern(re, NULL);
	sqpcre2->ovector = dlpcre2_get_ovector_pointer(sqpcre2->match_data);
	sqpcre2->v = v;
    sqpcre2->match_context = NULL;
    /*
    sq_resetobject(&sqpcre2->callout_cb);
    sq_resetobject(&sqpcre2->callout_cb_udata);
    */

	sq_setinstanceup(v,1,sqpcre2);
	sq_setreleasehook(v,1,sq_pcre2_releasehook);
	return 0;
}

static SQRESULT sq_pcre2_exec(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 4, start_offset, 0);
    SQ_OPT_INTEGER(v, 5, options, 0);

    int rc = dlpcre2_match(
        self->re,             /* the compiled pattern */
        (PCRE2_SPTR)subject,                  /* the subject string */
        subject_size,             /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->match_data,     /* block for storing the result */
        self->match_context);                /* use default match context */

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

static SQRESULT sq_pcre2_match(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 3, start_offset, 0);
    SQ_OPT_INTEGER(v, 4, options, 0);

    int rc = dlpcre2_match(
        self->re,             /* the compiled pattern */
        (PCRE2_SPTR)subject,                  /* the subject string */
        subject_size,             /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->match_data,     /* block for storing the result */
        self->match_context);                /* use default match context */

    if(rc > 0)
    {
        SQInteger start_pos = self->ovector[0], end_pos = self->ovector[1];
        if(start_pos == end_pos) sq_pushinteger(v, start_pos);
        else sq_pushstring(v, subject + start_pos, end_pos - start_pos);
        return 1;
    }
    sq_pushbool(v,SQFalse);
    return 1;
}

static SQRESULT sq_pcre2_gmatch(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 4, start_offset, 0);
    SQ_OPT_INTEGER(v, 5, options, 0);

    SQInteger rc;
    bool isFirst = true;

    while( (rc = dlpcre2_match(
        self->re,             /* the compiled pattern */
        (PCRE2_SPTR)subject,                  /* the subject string */
        subject_size,             /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->match_data,     /* block for storing the result */
        self->match_context)) > 0)           /* use default match context */
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
        i = sq_call(v, param_count, SQFalse, SQTrue);
        if(i < 0) return i;
        start_offset = self->ovector[(rc*2)-1]; //the last match + 1
    }
    sq_pushbool(v,SQFalse);
    return 1;
}

#include "sqstdblobimpl.h"
static SQRESULT sq_pcre2_gsub(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();
    SQ_GET_STRING(v, 2, str);
    SQ_OPT_INTEGER(v, 4, start_offset, 0);
    SQ_OPT_INTEGER(v, 5, options, 0);

	SQBlob blob(0,8192);
	const int replacement_idx = 3;
	SQObjectType ptype = sq_gettype(v, replacement_idx);
    const SQChar *replacement;
    SQInteger replacement_size;

    SQInteger rc;
    bool isFirst = true;

    while( (rc = dlpcre2_match(
        self->re,             /* the compiled pattern */
        (PCRE2_SPTR)str,                  /* the subject string */
        str_size,             /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->match_data,     /* block for storing the result */
        self->match_context)) > 0)           /* use default match context */
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
                                return sq_throwerror(v, _SC("there is no match for replacement $%d"), idx);
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
	}

    if(str_size) blob.Write(str+start_offset, str_size-start_offset);
	sq_pushstring(v, (const SQChar *)blob.GetBuf(), blob.Len());
	return 1;
}

static SQRESULT sq_pcre2_jit_compile(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();
    SQ_OPT_INTEGER(v, 2, options, 0);
    sq_pushinteger(v, dlpcre2_jit_compile(self->re, options));
	return 1;
}

static SQRESULT sq_pcre2__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC(PCRE2_Tag),-1);
	return 1;
}

static SQRESULT sq_pcre2_version(HSQUIRRELVM v)
{
    PCRE2_UCHAR8 buf[32];
    int rc = dlpcre2_config(PCRE2_CONFIG_VERSION, buf);
	sq_pushstring(v, (const SQChar*)buf, -1);
	return 1;
}

static SQRESULT sq_pcre2_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, libname);
    sq_pushbool(v, load_library(libname));
	return 1;
}

/*
The external callout function returns an integer to PCRE2.
If the value is zero, matching proceeds as normal.
If the value is greater than zero, matching fails at the current point,
but the testing of other matching possibilities goes ahead,
just as if a lookahead assertion had failed.
If the value is less than zero, the match is abandoned,
and the matching function returns the negative value.
*/
static int sq_pcre2_callout_callback(pcre2_callout_block *pcb, void *udata)
{
    SQInteger result = -1; /* abort by default */
    sqpcre2_st *sqpcre2 = (sqpcre2_st*)udata;
    HSQUIRRELVM v = sqpcre2->v;
    int top = sq_gettop(v);

    sq_pushobject(v, sqpcre2->callout_cb);
    sq_pushroottable(v); //this
    sq_pushinteger(v, pcb->callout_number);
    if(pcb->callout_string_length) sq_pushstring(v,
            (const SQChar*)pcb->callout_string, pcb->callout_string_length);
    else sq_pushnull(v);
    sq_pushinteger(v, pcb->start_match);
    sq_pushinteger(v, pcb->current_position);
    sq_pushobject(v, sqpcre2->callout_cb_udata);

    /* call squilu function */
    if (sq_call(v, 6, SQTrue, SQFalse) == SQ_OK)
        sq_getinteger(v, -1, &result);

    sq_settop(v, top);
    return result;
}

static SQRESULT sq_pcre2_set_callout(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();

    if(!self->match_context)
    {
        self->match_context = dlpcre2_match_context_create(NULL);
        sq_resetobject(&self->callout_cb);
        sq_resetobject(&self->callout_cb_udata);
    }
    else
    {
        /* clear progress handler */
        dlpcre2_set_callout(self->match_context, NULL, NULL);
    }

    sq_release(v, &self->callout_cb);
    sq_release(v, &self->callout_cb_udata);

    sq_resetobject(&self->callout_cb);
    sq_resetobject(&self->callout_cb_udata);

    SQObjectType otype = sq_gettype(v, 2);
    if (_top_ > 1 && (otype != OT_NULL))
    {
        if(sq_gettype(v, 2) != OT_CLOSURE)
            return sq_throwerror(v, _SC("invalid second parameter expected closure"));

        sq_getstackobj(v, 2, &self->callout_cb);
        sq_addref(v, &self->callout_cb);
        if(_top_ > 2)
        {
            sq_getstackobj(v, 3, &self->callout_cb_udata);
            sq_addref(v, &self->callout_cb_udata);
        }

        /* set progress callback */
        dlpcre2_set_callout(self->match_context, sq_pcre2_callout_callback, self);
    }
	return 0;
}

static SQRESULT sq_pcre2_set_callout_param(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();

    if(!self->match_context) return sq_throwerror(v, _SC("callout not set till now"));

    sq_release(v, &self->callout_cb_udata);
    sq_resetobject(&self->callout_cb_udata);

    SQObjectType otype = sq_gettype(v, 2);
    if (_top_ > 1 && (otype != OT_NULL))
    {
        sq_getstackobj(v, 2, &self->callout_cb_udata);
        sq_addref(v, &self->callout_cb_udata);
    }
	return 0;
}

#if 0
static SQRESULT sq_pcre2_substitute(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_pcre2_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_GET_STRING(v, 3, replacement);

    PCRE2_UCHAR *out_str;
    PCRE2_SIZE out_str_size;

    int rc = dlpcre2_substitute(
        self->re,             /* the compiled pattern */
        (PCRE2_SPTR)subject,  /* the subject string */
        str_size,             /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,              /* 0 = default options */
        self->match_data,     /* block for storing the result */
        self->match_context,  /* use default match context */
        (PCRE2_SPTR)replacement,
        replacement_size,
        out_str,
        &out_str_size);

    if(rc)
    {
        sq_pushstring(v, (const SQChar*)out_str, out_str_size);

    }
    else sq_pushnull(v);

	return 1;
}
#endif

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_pcre2_##name,nparams,tycheck}
static SQRegFunction sq_pcre2_methods[] =
{
	_DECL_FUNC(constructor,-2,_SC(".sn")),
	_DECL_FUNC(jit_compile,-1,_SC("xi")),
	_DECL_FUNC(exec,-3,_SC("xsann")),
	_DECL_FUNC(match,-2,_SC("xsnn")),
	_DECL_FUNC(gmatch,-3,_SC("xscnn")),
	_DECL_FUNC(gsub,-3,_SC("xs s|c|a|t nn")),
	_DECL_FUNC(_typeof,1,_SC("x")),
    _DECL_FUNC(version,1,_SC(".")),
    _DECL_FUNC(loadlib,2,_SC(".s")),
    _DECL_FUNC(set_callout,-2,_SC("xc.")),
    _DECL_FUNC(set_callout_param,-1,_SC("x.")),
    {0,0}
};
#undef _DECL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType sqpcre2_constants[] = {
    #define MK_CONST(c) {_SC(#c), PCRE2_##c}
    #define MK_CONST2(c) {_SC(c), (SQInteger)PCRE2_##c}
    //MK_CONST(SSL_SESSION_ID_SIZE),

	MK_CONST(ANCHORED),
	MK_CONST(NOTBOL),
	MK_CONST(NOTEOL),
	MK_CONST(NOTEMPTY),
	MK_CONST(NOTEMPTY_ATSTART),
	MK_CONST(NO_START_OPTIMIZE),
	MK_CONST(PARTIAL_HARD),
	MK_CONST(PARTIAL_SOFT),
	MK_CONST(NO_AUTO_POSSESS),
	MK_CONST(NO_START_OPTIMIZE),
	MK_CONST(NO_DOTSTAR_ANCHOR),
	MK_CONST(DOTALL),
	MK_CONST(CASELESS),
	MK_CONST(MULTILINE),
	MK_CONST(EXTENDED),
	MK_CONST(NEWLINE_CR),
	MK_CONST(NEWLINE_LF),
	MK_CONST(NEWLINE_CRLF),
	MK_CONST(NEWLINE_ANYCRLF),
	MK_CONST(NEWLINE_ANY),
	MK_CONST(INFO_NEWLINE),
	MK_CONST(INFO_ALLOPTIONS),
	MK_CONST(CONFIG_VERSION),
	MK_CONST(UTF),
	MK_CONST(NO_UTF_CHECK),
	MK_CONST(ERROR_UTF8_ERR1),
	MK_CONST(CONFIG_JIT),
	MK_CONST(JIT_COMPLETE),
	MK_CONST(JIT_PARTIAL_HARD),
	MK_CONST(JIT_PARTIAL_SOFT),
	MK_CONST(AUTO_CALLOUT),
	MK_CONST(SUBSTITUTE_GLOBAL),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_pcre2(HSQUIRRELVM v)
{
    sq_pushstring(v,PCRE2_Tag,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PCRE2_Tag);
    sq_insert_reg_funcs(v, sq_pcre2_methods);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = sqpcre2_constants; KeyIntPtr->Str; KeyIntPtr++) {
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

#endif //SQ_USE_PCRE2
