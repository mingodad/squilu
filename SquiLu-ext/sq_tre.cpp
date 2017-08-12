#if defined(SQ_USE_TRE) || defined(SQ_USE_TRE_STATIC)

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

#include "tre/tre.h"

////////
#include "dynamic_library.h"

/*SquiLu

local library_functions = [
    ["int", "tre_regncomp", "regex_t *preg, const char *regex, size_t len, int cflags"],
    //["int", "tre_regwncomp", "regex_t *preg, const wchar_t *regex, size_t len, int cflags"],
    ["int", "tre_regnexec", "const regex_t *preg, const char *string, size_t len, size_t nmatch, regmatch_t pmatch[], int eflags"],
    ["int", "tre_reganexec", "const regex_t *preg, const char *string, size_t len, regamatch_t *match, regaparams_t params, int eflags"],
    //["int", "tre_regwnexec", "const regex_t *preg, const wchar_t *string, size_t len, size_t nmatch, regmatch_t pmatch[], int eflags"],
    //["int", "tre_regawnexec", "const regex_t *preg, const wchar_t *string, size_t len, regamatch_t *match, regaparams_t params, int eflags"],
    ["void", "tre_regaparams_default", "regaparams_t *params"],
    ["int", "tre_config", "int query, void *result"],
    ["int", "tre_have_backrefs", "const regex_t *preg"],
    ["int", "tre_have_approx", "const regex_t *preg"],
    //["int", "tre_reguexec", "const regex_t *preg, const tre_str_source *string, size_t nmatch, regmatch_t pmatch[], int eflags"],
    ["size_t", "tre_regerror", "int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size"],
    ["void", "tre_regfree", "regex_t *preg"],

    //next entry should be the last one
    //to make valid the test made on load_library function
    ["char *", "tre_version", "void"],
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

function write_library_functions_static_defines(){
    foreach(k,v in library_functions){
        putsnl("#define dl" + v[1] + " " + v[1]);
    }
}
SquiLu*/

#ifdef SQ_USE_TRE_STATIC

#define load_library(x) true

//@write_library_functions_static_defines()
// generated-code:begin
#define dltre_regncomp tre_regncomp
#define dltre_regnexec tre_regnexec
#define dltre_reganexec tre_reganexec
#define dltre_regaparams_default tre_regaparams_default
#define dltre_config tre_config
#define dltre_have_backrefs tre_have_backrefs
#define dltre_have_approx tre_have_approx
#define dltre_regerror tre_regerror
#define dltre_regfree tre_regfree
#define dltre_version tre_version
// generated-code:end

#else

static DynamicLibrary libdyn;

//@write_library_functions_declaration();
// generated-code:begin
typedef int (*tre_regncomp_t)(regex_t *preg, const char *regex, size_t len, int cflags);
static tre_regncomp_t dltre_regncomp = 0;
typedef int (*tre_regnexec_t)(const regex_t *preg, const char *string, size_t len, size_t nmatch, regmatch_t pmatch[], int eflags);
static tre_regnexec_t dltre_regnexec = 0;
typedef int (*tre_reganexec_t)(const regex_t *preg, const char *string, size_t len, regamatch_t *match, regaparams_t params, int eflags);
static tre_reganexec_t dltre_reganexec = 0;
typedef void (*tre_regaparams_default_t)(regaparams_t *params);
static tre_regaparams_default_t dltre_regaparams_default = 0;
typedef int (*tre_config_t)(int query, void *result);
static tre_config_t dltre_config = 0;
typedef int (*tre_have_backrefs_t)(const regex_t *preg);
static tre_have_backrefs_t dltre_have_backrefs = 0;
typedef int (*tre_have_approx_t)(const regex_t *preg);
static tre_have_approx_t dltre_have_approx = 0;
typedef size_t (*tre_regerror_t)(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size);
static tre_regerror_t dltre_regerror = 0;
typedef void (*tre_regfree_t)(regex_t *preg);
static tre_regfree_t dltre_regfree = 0;
typedef char * (*tre_version_t)(void);
static tre_version_t dltre_version = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libtre);

static bool load_library(const char *libname)
{
    if(dltre_version) return true;
    if(libdyn.open(libname))
    {
        //@write_library_functions_load();
// generated-code:begin
dltre_regncomp = (tre_regncomp_t) libdyn.dlsym("tre_regncomp");
if(!dltre_regncomp) return false;
dltre_regnexec = (tre_regnexec_t) libdyn.dlsym("tre_regnexec");
if(!dltre_regnexec) return false;
dltre_reganexec = (tre_reganexec_t) libdyn.dlsym("tre_reganexec");
if(!dltre_reganexec) return false;
dltre_regaparams_default = (tre_regaparams_default_t) libdyn.dlsym("tre_regaparams_default");
if(!dltre_regaparams_default) return false;
dltre_config = (tre_config_t) libdyn.dlsym("tre_config");
if(!dltre_config) return false;
dltre_have_backrefs = (tre_have_backrefs_t) libdyn.dlsym("tre_have_backrefs");
if(!dltre_have_backrefs) return false;
dltre_have_approx = (tre_have_approx_t) libdyn.dlsym("tre_have_approx");
if(!dltre_have_approx) return false;
dltre_regerror = (tre_regerror_t) libdyn.dlsym("tre_regerror");
if(!dltre_regerror) return false;
dltre_regfree = (tre_regfree_t) libdyn.dlsym("tre_regfree");
if(!dltre_regfree) return false;
dltre_version = (tre_version_t) libdyn.dlsym("tre_version");
if(!dltre_version) return false;
// generated-code:end

        return true;
    }
    return false;
}

#endif // SQ_USE_TRE_STATIC


struct sqtre_st {
    regex_t re;
    regamatch_t amatch;
    regaparams_t aparams;

    /* associated squirrel vm */
    //HSQUIRRELVM v;
};

static const SQChar TRE_Tag[]   = _SC("sqtre");
#define GET_tre_INSTANCE() SQ_GET_INSTANCE(v, 1, sqtre_st, TRE_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("sqtre object already closed"));

static SQRESULT generate_error (HSQUIRRELVM v, const sqtre_st *sqtre, int errcode) {
    char errbuf[80];
    tre_regerror (errcode, &sqtre->re, errbuf, sizeof (errbuf));
    return sq_throwerror(v, _SC("%s"), errbuf);
}

static SQRESULT sq_tre_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	sqtre_st *self = ((sqtre_st *)p);
	if(self)
    {
        if(self->amatch.pmatch)
        {
            sq_free(self->amatch.pmatch, self->amatch.nmatch * sizeof(*self->amatch.pmatch));
        }
        dltre_regfree(&self->re);
        sq_free(self, sizeof(*self));
    }
	return 1;
}

static SQRESULT sq_tre_constructor(HSQUIRRELVM v)
{
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libtre !"));
	SQ_FUNC_VARS(v);
	SQ_GET_STRING(v, 2, pattern);
	SQ_OPT_INTEGER(v, 3, eflags, REG_EXTENDED);

	sqtre_st *sqtre = (sqtre_st *)sq_malloc(sizeof(*sqtre));
	memset(sqtre, 0, sizeof(sqtre_st));
	//sqtre->v = v;

	int rc = dltre_regncomp(
            &sqtre->re,
            pattern,              /* the pattern */
            pattern_size,
            eflags);              /* 0 = default eflags */

	if(rc)
    {
        SQRESULT res = generate_error(v, sqtre, rc);
        sq_free(sqtre, sizeof(sqtre_st));
        return res;
    }

    if (eflags & REG_NOSUB) sqtre->re.re_nsub = 0;
    else
    {
        sqtre->amatch.nmatch = sqtre->re.re_nsub+1; //+1 fror the whole match at 0
        sqtre->amatch.pmatch = (regmatch_t *) sq_malloc(sqtre->amatch.nmatch * sizeof (regmatch_t));
    }

    /*
    sq_resetobject(&sqtre->callout_cb);
    sq_resetobject(&sqtre->callout_cb_udata);
    */

	sq_setinstanceup(v,1,sqtre);
	sq_setreleasehook(v,1,sq_tre_releasehook);
	return 0;
}

#define GET_EXTRA_MATCH_PARAMS(stack_pos) \
    SQ_OPT_INTEGER(v, stack_pos, start_offset, 0); \
    SQ_OPT_INTEGER(v, stack_pos+1, eflags, 0); \
    SQ_OPT_INTEGER(v, stack_pos+2, max_len, 0); \
    \
    if(start_offset < 0) return sq_throwerror(v, _SC("start_offset can't be negative")); \
    SQInteger subject_str_size = subject_size - start_offset; \
    if(subject_str_size < 0) return sq_throwerror(v, _SC("str size - start_offset can't be negative")); \
    const SQChar *subject_str = subject + start_offset; \
    if(max_len && (max_len < subject_str_size)) subject_str_size = max_len;
static SQRESULT sq_tre_exec(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_tre_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    GET_EXTRA_MATCH_PARAMS(4);

    int rc = dltre_reganexec(
        &self->re,          /* the compiled pattern */
        subject_str,        /* the subject string */
        subject_str_size,   /* the length of the subject */
        &self->amatch,
        self->aparams,
        eflags);            /* 0 = default eflags */

    const int array_pos = 3;
    SQInteger rtype = sq_gettype(v, array_pos);

    if (rc == 0)
    {
        if(rtype == OT_ARRAY)
        {
            int nelms = self->amatch.nmatch;
            regmatch_t *pmatch = self->amatch.pmatch;
            sq_clear(v, array_pos);
            for (int i = 0; i < nelms; i++)
            {
                SQInteger pos = pmatch[i].rm_so + start_offset;
                sq_pushinteger(v, pos);
                sq_arrayappend(v, array_pos);
                pos = pmatch[i].rm_eo + start_offset;
                sq_pushinteger(v, pos);
                sq_arrayappend(v, array_pos);
            }
        }
    }

    sq_pushinteger(v, rc);
    return 1;
}

static SQRESULT sq_tre_match(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_tre_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    GET_EXTRA_MATCH_PARAMS(3);

    int rc = dltre_reganexec(
        &self->re,          /* the compiled pattern */
        subject_str,        /* the subject string */
        subject_str_size,   /* the length of the subject */
        &self->amatch,
        self->aparams,
        eflags);            /* 0 = default eflags */

    if(rc == 0)
    {
        regmatch_t *pmatch = self->amatch.pmatch;
        SQInteger start_pos = pmatch[0].rm_so, end_pos = pmatch[0].rm_eo;
        if(start_pos == end_pos) sq_pushinteger(v, start_pos + start_offset); //empty match return it's position
        else sq_pushstring(v, subject_str + start_pos, end_pos - start_pos);
        return 1;
    }
    if(rc == REG_ESPACE) //only no matching errore
    {
        return sq_throwerror(v, _SC("tre_match error %d"), (int)rc);
    }
    return 0;
}

static SQRESULT sq_tre_gmatch(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_tre_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    GET_EXTRA_MATCH_PARAMS(4);

    SQInteger rc;
    bool isFirst = true;


    regmatch_t *pmatch = self->amatch.pmatch;
    SQInteger nmatch = self->amatch.nmatch;

    while( (rc = dltre_reganexec(
        &self->re,             /* the compiled pattern */
        subject_str,           /* the subject string */
        subject_str_size,      /* the length of the subject */
        &self->amatch,
        self->aparams,
        eflags)) == 0)         /* use default match context */
    {
        if(isFirst)
        {
            sq_push(v, 3); //push the function
            isFirst = false;
        }
        sq_pushroottable(v); //this
        SQInteger start_pos, end_pos = 0, i = 0,
            param_count = 1; //root table already on the stack
        for(;i < nmatch; i++) {
            start_pos = pmatch[i].rm_so;
            end_pos = pmatch[i].rm_eo;
            if(start_pos == end_pos) sq_pushinteger(v, start_pos + start_offset); //empty match return it's position
            else sq_pushstring(v, subject_str + start_pos, end_pos - start_pos);
            ++param_count;
        }
        i = sq_call(v, param_count, SQTrue, SQTrue);
        if(i < 0) return i;
        SQObjectType rtype = sq_gettype(v, -1);
        SQBool keep_matching = SQFalse;
        if(rtype == OT_BOOL) {
            sq_getbool(v, -1, &keep_matching);
        }
        sq_poptop(v); //remove return parameter

        if(!keep_matching) break;

        end_pos = pmatch[0].rm_eo;
        subject_str_size -= end_pos;
        if(subject_str_size <= 0) break;
        start_offset += end_pos;
        subject_str += end_pos; //the last match + 1
    }
    if(rc == REG_ESPACE) //only no matching errore
    {
        return sq_throwerror(v, _SC("tre_match error %d"), (int)rc);
    }
    return 0;
}

#include "sqstdblobimpl.h"
static SQRESULT sq_tre_gsub(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_tre_INSTANCE();
    SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 4, eflags, 0);
    SQ_OPT_INTEGER(v, 5, max_sub, 0);
    if(max_sub < 0) return sq_throwerror(v, _SC("max substitutions can't be less than zero"));

	SQBlob blob(0,8192);
	const int replacement_idx = 3;
	SQObjectType ptype = sq_gettype(v, replacement_idx);
    const SQChar *replacement;
    SQInteger replacement_size, start_offset=0;

    SQInteger rc;
    bool isFirst = true;
    regmatch_t *pmatch = self->amatch.pmatch;
    SQInteger nmatch = self->amatch.nmatch;
    if(ptype == OT_STRING)
    {
        sq_getstr_and_size(v, replacement_idx, &replacement, &replacement_size);
    }

    while( (rc = dltre_reganexec(
        &self->re,                  /* the compiled pattern */
        subject,                /* the subject string */
        subject_size,  /* the length of the subject */
        &self->amatch,
        self->aparams,
        eflags)) == 0)              /* use default match context */
    {
        SQInteger i, start_pos, end_pos;
	    blob.Write(subject, pmatch[0].rm_so);
	    switch(ptype){
	        case OT_CLOSURE:{
                if(isFirst)
                {
                    sq_push(v, replacement_idx); //push the function
                    isFirst = false;
                }
                sq_pushroottable(v); //this
                SQInteger param_count = 1; //root table
                for(i=0; i < nmatch; i++) {
                    start_pos = pmatch[i].rm_so;
                    end_pos = pmatch[i].rm_eo;
                    if(start_pos == end_pos) sq_pushinteger(v, start_pos + start_offset);
                    else sq_pushstring(v, subject + start_pos, end_pos - start_pos);
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
                for(i=0; i < nmatch; i++) {
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
                for(i=0; i < nmatch; i++) {
                    start_pos = pmatch[i].rm_so;
                    end_pos = pmatch[i].rm_eo;
                    sq_pushstring(v, subject + start_pos, end_pos - start_pos);
                    if(SQ_SUCCEEDED(sq_get(v, replacement_idx)) &&
                            SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &replacement, &replacement_size))){
                        blob.Write(replacement, replacement_size);
                        sq_pop(v, 1); //remove value
                    }
                }
	        }
	        break;
	        case OT_STRING:{
                for(i=0; i < replacement_size; i++) {
                    SQInteger c = replacement[i];
                    switch(c)
                    {
                    case '$':
                        ++i;
                        if(i < replacement_size)
                        {
                            SQInteger idx = replacement[i] - '0', match_idx = 0;
                            for(int j=0; j < nmatch; j++) {
                                if(match_idx == idx)
                                {
                                    start_pos = pmatch[j].rm_so;
                                    end_pos = pmatch[j].rm_eo;
                                    blob.Write(subject + start_pos, end_pos-start_pos);
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
        end_pos = pmatch[0].rm_eo;
        subject_size -= end_pos;
        if(subject_size <= 0) break;
        subject += end_pos; //the last match + 1
        if(max_sub)
        {
            if(--max_sub == 0) break;
        }
	}

    if(rc == REG_ESPACE) //only no matching errore
    {
        return sq_throwerror(v, _SC("tre_match error %d"), (int)rc);
    }

    if(subject_size > 0) blob.Write(subject+start_offset, subject_size-start_offset);
	sq_pushstring(v, (const SQChar *)blob.GetBuf(), blob.Len());
	return 1;
}

static SQRESULT sq_tre_have_backrefs(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_tre_INSTANCE();
    sq_pushinteger(v, dltre_have_backrefs(&self->re));
	return 1;
}

static SQRESULT sq_tre_have_approx(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_tre_INSTANCE();
    sq_pushinteger(v, dltre_have_approx(&self->re));
	return 1;
}

static SQRESULT sq_tre__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC(TRE_Tag),-1);
	return 1;
}

static SQRESULT sq_tre_version(HSQUIRRELVM v)
{
    const char *buf = dltre_version();
	sq_pushstring(v, (const SQChar*)buf, -1);
	return 1;
}

static SQRESULT sq_tre_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, libname);
    sq_pushbool(v, load_library(libname));
	return 1;
}

static SQRESULT sq_tre_regaparams_default(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_tre_INSTANCE();
    dltre_regaparams_default(&self->aparams);
	return 0;
}

static SQRESULT sq_tre_regamatch(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_tre_INSTANCE();
    SQ_GET_STRING(v, 2, pkey);

#define GET_PKEY(skey) \
    else if(strcmp(pkey, #skey) == 0) \
    { \
        sq_pushinteger(v, self->amatch.skey); \
    }

    if(0){}
    GET_PKEY(cost)
    GET_PKEY(num_ins)
    GET_PKEY(num_del)
    GET_PKEY(num_subst)
    GET_PKEY(nmatch)
    else return sq_throwerror(v, _SC("unknown parameter '%s'"), pkey);
	return 1;
}

static SQRESULT sq_tre_regaparams(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_tre_INSTANCE();
    SQ_GET_STRING(v, 2, pkey);
    SQ_OPT_INTEGER(v, 3, pvalue, 0);
    SQBool doSet = (_top_ == 3);
#define GET_SET_PKEY(skey) \
    else if(strcmp(pkey, #skey) == 0) \
    { \
        sq_pushinteger(v, self->aparams.skey); \
        if(doSet) self->aparams.skey = pvalue; \
    }

    if(0){}
    GET_SET_PKEY(max_cost)
    GET_SET_PKEY(cost_ins)
    GET_SET_PKEY(cost_del)
    GET_SET_PKEY(cost_subst)
    GET_SET_PKEY(max_ins)
    GET_SET_PKEY(max_del)
    GET_SET_PKEY(max_subst)
    GET_SET_PKEY(max_err)
    else return sq_throwerror(v, _SC("unknown parameter '%s'"), pkey);
	return 1;
}

static SQRESULT sq_tre_config(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, option);
    void *config_result;
    switch(option)
    {
        case TRE_CONFIG_APPROX:
        case TRE_CONFIG_WCHAR:
        case TRE_CONFIG_MULTIBYTE:
        case TRE_CONFIG_SYSTEM_ABI:
            tre_config(option, &config_result);
            sq_pushinteger(v, (SQInteger)config_result);
            break;
        case TRE_CONFIG_VERSION:
            tre_config(option, &config_result);
            sq_pushstring(v, (const SQChar*)config_result, -1);
            break;
        default:
            return sq_throwerror(v, _SC("unknown config option '%d'"), option);
    }
	return 1;
}
#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_tre_##name,nparams,tycheck}
static SQRegFunction sq_tre_methods[] =
{
	_DECL_FUNC(constructor,-2,_SC(".sn")),
	_DECL_FUNC(config,2,_SC("xi")),
	_DECL_FUNC(regaparams_default,1,_SC("x")),
	_DECL_FUNC(regaparams,-2,_SC("xsi")),
	_DECL_FUNC(regamatch,2,_SC("xs")),
	_DECL_FUNC(exec,-3,_SC("xsannn")),
	_DECL_FUNC(match,-2,_SC("xsnnn")),
	_DECL_FUNC(gmatch,-3,_SC("xscnnn")),
	_DECL_FUNC(gsub,-3,_SC("xs s|c|a|t nn")),
	_DECL_FUNC(_typeof,1,_SC("x")),
    _DECL_FUNC(version,1,_SC(".")),
	_DECL_FUNC(have_approx,1,_SC("x")),
	_DECL_FUNC(have_backrefs,1,_SC("x")),
    _DECL_FUNC(loadlib,2,_SC(".s")),
    {0,0}
};
#undef _DECL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType sqtre_constants[] = {
    #define MK_CONST(c) {_SC(#c), TRE_##c}
    #define MK_CONST2(c) {_SC(#c), (SQInteger)TRE_##c}
    #define MK_CONST_REG(c) {_SC(#c), (SQInteger)REG_##c}
    //MK_CONST(SSL_SESSION_ID_SIZE),

	MK_CONST2(CONFIG_VERSION),
	MK_CONST2(CONFIG_APPROX),
	MK_CONST2(CONFIG_WCHAR),
	MK_CONST2(CONFIG_MULTIBYTE),
	MK_CONST2(CONFIG_SYSTEM_ABI),

	MK_CONST_REG(OK),
	MK_CONST_REG(NOMATCH),
	MK_CONST_REG(BADPAT),
	MK_CONST_REG(ECOLLATE),
	MK_CONST_REG(ECTYPE),
	MK_CONST_REG(EESCAPE),
	MK_CONST_REG(ESUBREG),
	MK_CONST_REG(EBRACK),
	MK_CONST_REG(EPAREN),
	MK_CONST_REG(EBRACE),
	MK_CONST_REG(BADBR),
	MK_CONST_REG(ERANGE),
	MK_CONST_REG(ESPACE),
	MK_CONST_REG(BADRPT),

	MK_CONST_REG(EXTENDED),
	MK_CONST_REG(ICASE),
	MK_CONST_REG(NEWLINE),
	MK_CONST_REG(NOSUB),
	MK_CONST_REG(BASIC),
	MK_CONST_REG(LITERAL),
	MK_CONST_REG(RIGHT_ASSOC),
	MK_CONST_REG(UNGREEDY),
	MK_CONST_REG(NOTBOL),
	MK_CONST_REG(NOTEOL),
	MK_CONST_REG(APPROX_MATCHER),
	MK_CONST_REG(BACKTRACKING_MATCHER),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_tre(HSQUIRRELVM v)
{
    sq_pushstring(v,TRE_Tag,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)TRE_Tag);
    sq_insert_reg_funcs(v, sq_tre_methods);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = sqtre_constants; KeyIntPtr->Str; KeyIntPtr++) {
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

#endif //SQ_USE_TRE

