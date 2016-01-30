
function generateCppClassWrapper(klass, klass_name="klass")
{
	local dbg = function(...)
	{
		return;
		foreach(k, v in vargv)
		{
			if(k) stdout.write("\t");
			stdout.write(v);
		}
		stdout.write("\n");
	}
	
	local rename_code = function(code)
	{
		return code.gsub("KLASS", klass_name.toupper()).gsub("klass", klass_name);
	}
	
	local print_code = function(code)
	{
		print(rename_code(code));
	}
	
	local get_var_type = function(var_name, to_declare=false)
	{
		if(var_name.startswith("int_"))
		{
			return to_declare ? "i" : "INTEGER";
		}
		if(var_name.startswith("float_"))
		{
			return to_declare ? "f" : "FLOAT";
		}
		if(var_name.startswith("str_"))
		{
			return to_declare ? "s" : "STRING";
		}
		return to_declare ? "." : "STRING";
	}
	
	local members = [];

	foreach(k,v in klass)
	{
		members.push(k);
	}
	
	members.sort();
	
	print_code([==[
#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_KLASS

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */

#include "klass.h"

static const SQChar SQ_LIBNAME[] = _SC("klass");

SQ_OPT_STRING_STRLEN();

static const SQChar SSL_CTX_Tag[]   = _SC("sq_klass_ctx");
#define GET_klass_INSTANCE() SQ_GET_INSTANCE(v, 1, KLASS, KLASS_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("klass object already closed"));


static SQRESULT klass_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	KLASS *self = (KLASS*)p;
	if(self) klass_free(self);
	return 0;
}

static SQRESULT klass_free(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	GET_ssl_INSTANCE();
	klass_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0);
	return 0;
}

static SQRESULT klass_constructor(HSQUIRRELVM v, KLASS *klass, int free_on_gc)
{
    if(!klass)
        return sq_throwerror(v, _SC("Could'nt create an klass object."));

    sq_pushstring(v, SQ_LIBNAME, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, klass_NAME, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, ssl);
                if(free_on_gc) sq_setreleasehook(v,-1, klass_release_hook);
                return 1;
            }
        }
    }
	return SQ_ERROR;
}

]==]);
	
	foreach(k,v in members)
	{
		dbg(k,v);
		v = klass[v];
		local v_type = type(v);
		if(v_type == "function")
		{
			local info = v.getinfos();
			local ndefparams = info.defparams.len();
			print_code(format("static SQRESULT sq_klass_%s(HSQUIRRELVM v){", info.name));
			if(ndefparams) print("\tSQ_FUNC_VARS(v);");
			else print("\tSQ_FUNC_VARS_NO_TOP(v);");
			print_code("\tGET_klass_INSTANCE();\n");
			
			local nparams = info.parameters.len();
			if(nparams > 1)
			{
				local firstDefParam = nparams - ndefparams;
				for(local i=1; i < nparams; ++i)
				{
					local hasDefParam = ndefparams && (i >= firstDefParam);
					local vtype = get_var_type(info.parameters[i]);
					if(hasDefParam) print(format("\tSQ_OPT_%s(v, %d, %s, %q);", vtype, i+1, info.parameters[i], info.defparams[i - firstDefParam].tostring()));
					else print(format("\tSQ_GET_%s(v, %d, %s);", vtype, i+1, info.parameters[i]));
				}
			}
			
			foreach(k2,v2 in info)
			{
				dbg("", k2, v2);
				if(type(v2) == "array")
				{
					foreach(k3,v3 in v2)
					{
						dbg("","", k3, v3);
					}
				}
			}
			//local return_type = 
			local attr = klass.getattributes(info.name);
			if(attr && attr.rawin("cfunc")) print("//", attr.rawget("cfunc"));
			print("\n\treturn 0;\n}\n");
		}
	}
	
	print_code( [==[
#define _DECL_KLASS_FUNC(name,nparams,pmask) {_SC(#name),sq_klass_##name,nparams,pmask}
static SQRegFunction klass_obj_funcs[]={
]==]);
	
	foreach(k,v in members)
	{
		v = klass[v];
		if(type(v) == "function")
		{
			local info = v.getinfos();
			local ndefparams = info.defparams.len();
			local nparams = info.parameters.len();
			local nreqparams = nparams - ndefparams;
			if(ndefparams) nreqparams *= -1;
			stdout.write(rename_code(format("\t_DECL_KLASS_FUNC(%s, %d, _SC(\".", info.name, nreqparams)));
			
			for(local i=1; i < nparams; ++i)
			{
				stdout.write(get_var_type(info.parameters[i], true));
			}
			stdout.write("\")),\n");
		}
	}
	
	print_code([==[
	{0,0}
};
#undef _DECL_KLASS_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType klass_constants[] = {
    #define MK_CONST(c) {_SC(#c), c}
    //MK_CONST(SSL_SESSION_ID_SIZE),
]==]);
	foreach(k,v in klass)
	{
		//print(k, v);
		local v_type = type(v);
		if(v_type == "integer")
		{
			if(k.startswith("const_"))
			{
				local const_name = k.replace("const_", "");
				print(format("\tMK_CONST(v, %s);", const_name));
			}
		}
	}
	print_code([==[
    {0,0}
};

/* This defines a function that opens up your library. */
SQRESULT sqext_register_klass (HSQUIRRELVM v) {
	//add a namespace klass
	sq_pushstring(v, SQ_LIBNAME, -1);
	sq_newtable(v);

	sq_insert_reg_funcs(v, klass_obj_funcs);

	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = klass_constants; KeyIntPtr->Str; KeyIntPtr++) {
		sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
		sq_pushinteger(v, KeyIntPtr->Val);       //then the value
		sq_newslot(v, -3, SQFalse);              //store then
	}

	sq_newslot(v,-3,SQFalse); //add klass table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}

#endif //USE_KLASS

#endif

]==]);
	
}
