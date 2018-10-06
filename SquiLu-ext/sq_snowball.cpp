#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_SNOWBALL

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

SQ_OPT_STRING_STRLEN();

#include "libstemmer.h"


static const SQChar SQ_LIBNAME[] = _SC("Snowball");


static const SQChar SNOWBALL_Tag[]   = _SC("sq_Snowball");
#define GET_Snowball_INSTANCE() SQ_GET_INSTANCE(v, 1, sb_stemmer, SNOWBALL_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("Snowball object already closed"));


static SQRESULT Snowball_release_hook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	sb_stemmer *self = (sb_stemmer*)p;
	if(self) sb_stemmer_delete(self);
	return 0;
}

static SQRESULT sq_Snowball_constructor(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
	SQ_GET_STRING(v, 2, algorithm);
	SQ_OPT_STRING(v, 3, charenc, NULL);

	sb_stemmer *Snowball = sb_stemmer_new(algorithm, charenc);
    if(!Snowball)
        return sq_throwerror(v, _SC("Could'nt create an Snowball object."));

    SQInteger rc = sq_setinstanceup(v, 1, Snowball);
    sq_setreleasehook(v, 1, Snowball_release_hook);
    return rc;
}


//	int  sb_stemmer_length(struct sb_stemmer * stemmer)
static SQRESULT sq_Snowball_length(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_Snowball_INSTANCE();

	sq_pushinteger(v, sb_stemmer_length(self));
	return 1;
}

//	const sb_symbol *   sb_stemmer_stem(struct sb_stemmer * stemmer, const sb_symbol * word, int size)
static SQRESULT sq_Snowball_stem(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_Snowball_INSTANCE();
	SQ_GET_STRING(v, 2, str_word);

	const sb_symbol *res =  sb_stemmer_stem(self,
        (const sb_symbol*)str_word, str_word_size);
	if(res) sq_pushstring(v, (const SQChar*)res, -1);
	else sq_pushnull(v);
	return 1;
}

//	const char ** sb_stemmer_list(void)
static SQRESULT sq_Snowball_list(HSQUIRRELVM v){
    const char ** list = sb_stemmer_list();
    if (*list == 0) sq_pushnull(v);
    else
    {
        SQInteger lcount = 0;
        const char **ptr = list;
        while(*(ptr++)) ++lcount;
        ptr = list;
        sq_newarray(v, lcount);
        for(SQInteger i=0; i < lcount; ++i)
        {
            sq_pushstring(v, *(ptr++), -1);
            sq_arrayset(v, -2, i);
        }
    }
    return 1;
}

#define _DECL_SNOWBALL_FUNC(name,nparams,pmask) {_SC(#name),sq_Snowball_##name,nparams,pmask}
static SQRegFunction Snowball_obj_funcs[]={

	_DECL_SNOWBALL_FUNC(constructor, -2, _SC("xss")),
	_DECL_SNOWBALL_FUNC(length, 1, _SC("x")),
	_DECL_SNOWBALL_FUNC(stem, 2, _SC("xs")),
	_DECL_SNOWBALL_FUNC(list, 1, _SC(".")),
	{0,0}
};
#undef _DECL_SNOWBALL_FUNC


/* This defines a function that opens up your library. */
SQRESULT sqext_register_Snowball (HSQUIRRELVM v) {
	//add a namespace Snowball
	sq_pushstring(v, SQ_LIBNAME, -1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(SQUserPointer)SNOWBALL_Tag);

	sq_insert_reg_funcs(v, Snowball_obj_funcs);

	sq_newslot(v,-3,SQFalse); //add Snowball table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}

#endif //USE_SNOWBALL

#endif
