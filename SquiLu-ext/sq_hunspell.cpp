#if defined(SQ_USE_HUNSPELL) || defined(SQ_USE_HUNSPELL_STATIC)

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

#include "hunspell.h"

////////
#include "dynamic_library.h"

/*SquiLu

local library_functions = [
    ["Hunhandle*", "Hunspell_create_key", "const char* affpath, const char* dpath, const char* key"],
    ["void", "Hunspell_destroy", "Hunhandle* pHunspell"],
    ["int", "Hunspell_add_dic", "Hunhandle* pHunspell, const char* dpath"],
    ["int", "Hunspell_spell", "Hunhandle* pHunspell, const char*"],
    ["char *", "Hunspell_get_dic_encoding", "Hunhandle* pHunspell"],
    ["int", "Hunspell_suggest", "Hunhandle* pHunspell, char*** slst, const char* word"],
    ["int", "Hunspell_analyze", "Hunhandle* pHunspell, char*** slst, const char* word"],
    ["int", "Hunspell_stem", "Hunhandle* pHunspell, char*** slst, const char* word"],
    ["int", "Hunspell_generate", "Hunhandle* pHunspell, char*** slst, const char* word, const char* word2"],
    ["int", "Hunspell_generate2", "Hunhandle* pHunspell, char*** slst, const char* word, char** desc, int n"],
    ["int", "Hunspell_add", "Hunhandle* pHunspell, const char* word"],
    ["int", "Hunspell_add_with_affix", "Hunhandle* pHunspell, const char* word, const char* example"],
    ["int", "Hunspell_remove", "Hunhandle* pHunspell, const char* word"],

    //next entry should be the last one
    //to make valid the test made on load_library function
    ["void", "Hunspell_free_list", "Hunhandle* pHunspell, char*** slst, int n"],
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

#ifdef SQ_USE_HUNSPELL_STATIC

#define load_library(x) true

//@write_library_functions_static_defines()
// generated-code:begin
#define dlHunspell_create_key Hunspell_create_key
#define dlHunspell_destroy Hunspell_destroy
#define dlHunspell_add_dic Hunspell_add_dic
#define dlHunspell_spell Hunspell_spell
#define dlHunspell_get_dic_encoding Hunspell_get_dic_encoding
#define dlHunspell_suggest Hunspell_suggest
#define dlHunspell_analyze Hunspell_analyze
#define dlHunspell_stem Hunspell_stem
#define dlHunspell_generate Hunspell_generate
#define dlHunspell_generate2 Hunspell_generate2
#define dlHunspell_add Hunspell_add
#define dlHunspell_add_with_affix Hunspell_add_with_affix
#define dlHunspell_remove Hunspell_remove
#define dlHunspell_free_list Hunspell_free_list
// generated-code:end

#else

static DynamicLibrary libdyn;

//@write_library_functions_declaration();
// generated-code:begin
typedef Hunhandle* (*Hunspell_create_key_t)(const char* affpath, const char* dpath, const char* key);
static Hunspell_create_key_t dlHunspell_create_key = 0;
typedef void (*Hunspell_destroy_t)(Hunhandle* pHunspell);
static Hunspell_destroy_t dlHunspell_destroy = 0;
typedef int (*Hunspell_add_dic_t)(Hunhandle* pHunspell, const char* dpath);
static Hunspell_add_dic_t dlHunspell_add_dic = 0;
typedef int (*Hunspell_spell_t)(Hunhandle* pHunspell, const char*);
static Hunspell_spell_t dlHunspell_spell = 0;
typedef char * (*Hunspell_get_dic_encoding_t)(Hunhandle* pHunspell);
static Hunspell_get_dic_encoding_t dlHunspell_get_dic_encoding = 0;
typedef int (*Hunspell_suggest_t)(Hunhandle* pHunspell, char*** slst, const char* word);
static Hunspell_suggest_t dlHunspell_suggest = 0;
typedef int (*Hunspell_analyze_t)(Hunhandle* pHunspell, char*** slst, const char* word);
static Hunspell_analyze_t dlHunspell_analyze = 0;
typedef int (*Hunspell_stem_t)(Hunhandle* pHunspell, char*** slst, const char* word);
static Hunspell_stem_t dlHunspell_stem = 0;
typedef int (*Hunspell_generate_t)(Hunhandle* pHunspell, char*** slst, const char* word, const char* word2);
static Hunspell_generate_t dlHunspell_generate = 0;
typedef int (*Hunspell_generate2_t)(Hunhandle* pHunspell, char*** slst, const char* word, char** desc, int n);
static Hunspell_generate2_t dlHunspell_generate2 = 0;
typedef int (*Hunspell_add_t)(Hunhandle* pHunspell, const char* word);
static Hunspell_add_t dlHunspell_add = 0;
typedef int (*Hunspell_add_with_affix_t)(Hunhandle* pHunspell, const char* word, const char* example);
static Hunspell_add_with_affix_t dlHunspell_add_with_affix = 0;
typedef int (*Hunspell_remove_t)(Hunhandle* pHunspell, const char* word);
static Hunspell_remove_t dlHunspell_remove = 0;
typedef void (*Hunspell_free_list_t)(Hunhandle* pHunspell, char*** slst, int n);
static Hunspell_free_list_t dlHunspell_free_list = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libhunspell-1.6);

static bool load_library(const char *libname)
{
    if(dlHunspell_free_list) return true;
    if(libdyn.open(libname))
    {
        //@write_library_functions_load();
// generated-code:begin
dlHunspell_create_key = (Hunspell_create_key_t) libdyn.dlsym("Hunspell_create_key");
if(!dlHunspell_create_key) return false;
dlHunspell_destroy = (Hunspell_destroy_t) libdyn.dlsym("Hunspell_destroy");
if(!dlHunspell_destroy) return false;
dlHunspell_add_dic = (Hunspell_add_dic_t) libdyn.dlsym("Hunspell_add_dic");
if(!dlHunspell_add_dic) return false;
dlHunspell_spell = (Hunspell_spell_t) libdyn.dlsym("Hunspell_spell");
if(!dlHunspell_spell) return false;
dlHunspell_get_dic_encoding = (Hunspell_get_dic_encoding_t) libdyn.dlsym("Hunspell_get_dic_encoding");
if(!dlHunspell_get_dic_encoding) return false;
dlHunspell_suggest = (Hunspell_suggest_t) libdyn.dlsym("Hunspell_suggest");
if(!dlHunspell_suggest) return false;
dlHunspell_analyze = (Hunspell_analyze_t) libdyn.dlsym("Hunspell_analyze");
if(!dlHunspell_analyze) return false;
dlHunspell_stem = (Hunspell_stem_t) libdyn.dlsym("Hunspell_stem");
if(!dlHunspell_stem) return false;
dlHunspell_generate = (Hunspell_generate_t) libdyn.dlsym("Hunspell_generate");
if(!dlHunspell_generate) return false;
dlHunspell_generate2 = (Hunspell_generate2_t) libdyn.dlsym("Hunspell_generate2");
if(!dlHunspell_generate2) return false;
dlHunspell_add = (Hunspell_add_t) libdyn.dlsym("Hunspell_add");
if(!dlHunspell_add) return false;
dlHunspell_add_with_affix = (Hunspell_add_with_affix_t) libdyn.dlsym("Hunspell_add_with_affix");
if(!dlHunspell_add_with_affix) return false;
dlHunspell_remove = (Hunspell_remove_t) libdyn.dlsym("Hunspell_remove");
if(!dlHunspell_remove) return false;
dlHunspell_free_list = (Hunspell_free_list_t) libdyn.dlsym("Hunspell_free_list");
if(!dlHunspell_free_list) return false;
// generated-code:end

        return true;
    }
    return false;
}

#endif // SQ_USE_HUNSPELL_STATIC

SQ_OPT_STRING_STRLEN();

static const SQChar HUNSPELL_Tag[]   = _SC("sqhunspell");
#define GET_hunspell_INSTANCE() SQ_GET_INSTANCE(v, 1, Hunhandle, HUNSPELL_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("sqhunspell object already closed"));

static SQRESULT sq_hunspell_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	Hunhandle *self = ((Hunhandle *)p);
	if(self)
    {
        dlHunspell_destroy(self);
    }
	return 1;
}

static SQRESULT sq_hunspell_constructor(HSQUIRRELVM v)
{
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libhunspell !"));
	SQ_FUNC_VARS(v);
	SQ_GET_STRING(v, 2, aff);
	SQ_GET_STRING(v, 3, dic);
	SQ_OPT_STRING(v, 4, key, NULL);

    Hunhandle *self = dlHunspell_create_key(aff, dic, key);

	sq_setinstanceup(v,1,self);
	sq_setreleasehook(v,1,sq_hunspell_releasehook);
	return 0;
}

static SQRESULT sq_hunspell_add_dic(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, dpath);
	sq_pushinteger(v, dlHunspell_add_dic(self, dpath));
	return 1;
}

static SQRESULT sq_hunspell_list(HSQUIRRELVM v, Hunhandle* pHunspell, int nelms, char **list)
{
    sq_newarray(v, nelms);
    for (int i = 0; i < nelms; i++)
    {
        sq_pushstring(v, list[i], -1);
        sq_arrayset(v, -2, i);
    }
    dlHunspell_free_list(pHunspell, &list, nelms);
    return 1;
}

static SQRESULT sq_hunspell_analyze(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, word);
	char **list = NULL;

	int nelms = dlHunspell_analyze(self, &list, word);
	return sq_hunspell_list(v, self, nelms, list);
}

static SQRESULT sq_hunspell_generate(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, word);
    char **list;
    const SQChar *str;
    int nelms;

	switch(sq_gettype(v, 3))
	{
    case OT_STRING:
        {
            SQ_GET_STRING(v, 3, example);
            nelms = dlHunspell_generate(self, &list, word, example);
            return sq_hunspell_list(v, self, nelms, list);
        }
        break;
    case OT_ARRAY:
        {
            int howmany = sq_getsize(v, 3);
            char **desc = (char**)sq_getscratchpad(v, howmany);
            for (int i=0; i<howmany; i++) {
                sq_arrayget(v, 3, i);
                if(sq_gettype(v, -1) != OT_STRING) return sq_throwerror(v, _SC("only strings are allowed in the supplied array"));
                sq_getstring(v, -1, &str);
                desc[i] = (char*)str; // nasty
                sq_poptop(v);
            }
            nelms = dlHunspell_generate2(self, &list, word, desc, howmany);
            return sq_hunspell_list(v, self, nelms, list);
        }
        break;
    default:
        return sq_throwerror(v, _SC("string or array expected"));
	}
	return 0;
}

static SQRESULT sq_hunspell_spell(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, word);
	int ret = dlHunspell_spell(self, word);
	sq_pushbool(v, ret);
	return 1;
}

static SQRESULT sq_hunspell_stem(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, word);
	char **list;

	int nelms = dlHunspell_stem(self, &list, word);
	return sq_hunspell_list(v, self, nelms, list);
}

static SQRESULT sq_hunspell_suggest(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, word);
	SQ_OPT_INTEGER(v, 3, max_suggestion, 0);
	char **list;

	int nelms = dlHunspell_suggest(self, &list, word);
	if(max_suggestion)
    {
        if(max_suggestion == 1)
        {
            if(nelms)
            {
                sq_pushstring(v, list[0], -1);
                dlHunspell_free_list(self, &list, nelms);
            }
            else sq_pushnull(v);
            return 1;
        }
        if(nelms > max_suggestion) nelms = max_suggestion;
    }
	return sq_hunspell_list(v, self, nelms, list);
}

static SQRESULT sq_hunspell_add_word(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, word);
	sq_pushinteger(v, dlHunspell_add(self, word));
	return 1;
}

static SQRESULT sq_hunspell_remove_word(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	SQ_GET_STRING(v, 2, word);
	sq_pushinteger(v, dlHunspell_remove(self, word));
	return 1;
}

static SQRESULT sq_hunspell_get_dic_encoding(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_hunspell_INSTANCE();
	char* encoding = dlHunspell_get_dic_encoding(self);
	sq_pushstring(v, encoding, -1);
	return 1;
}

static SQRESULT sq_hunspell__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC(HUNSPELL_Tag),-1);
	return 1;
}


static SQRESULT sq_hunspell_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, libname);
    sq_pushbool(v, load_library(libname));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_hunspell_##name,nparams,tycheck}
static SQRegFunction sq_hunspell_methods[] =
{
	_DECL_FUNC(constructor,-3,_SC(".sss")),
	_DECL_FUNC(add_dic,2,_SC("xs")),
	_DECL_FUNC(analyze,2,_SC("xs")),
	_DECL_FUNC(generate,-2,_SC("x s|a")),
	_DECL_FUNC(spell,2,_SC("xs")),
	_DECL_FUNC(stem,2,_SC("xs")),
	_DECL_FUNC(suggest,-2,_SC("xsi")),
	_DECL_FUNC(add_word,2,_SC("xs")),
	_DECL_FUNC(remove_word,2,_SC("xs")),
    _DECL_FUNC(get_dic_encoding,1,_SC("x")),
	_DECL_FUNC(_typeof,1,_SC("x")),
    _DECL_FUNC(loadlib,2,_SC(".s")),
    {0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_hunspell(HSQUIRRELVM v)
{
    sq_pushstring(v,HUNSPELL_Tag,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)HUNSPELL_Tag);
    sq_insert_reg_funcs(v, sq_hunspell_methods);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif //SQ_USE_HUNSPELL


