#include "squirrel.h"
#include "sqstdblobimpl.h"

#define MixInteger SQInteger
#include "code_mix_prep.c"

/* Generic loader function. Load the data found in the state in the lua engine
 */
static SQRESULT mix_loadbuffer(HSQUIRRELVM sqvm, mix_state_t *S, const SQChar *name) {
	SQRESULT res;
	S->pos = 0;

	if (S->size > 0 && S->buffer[0] == '#') {
		while (S->pos < S->size && S->buffer[S->pos] != '\n')
			++S->pos;
		++S->pos;
		S->token = tok_sh;
	} else {
		S->token = tok_code_end;
	}

	S->error = NULL;
	res = sq_compile(sqvm, sq_mix_reader_char, S, name, SQTrue);
	if (S->error != NULL) {
		return sq_throwerror(sqvm, S->error);
	} else if (res != 0) {
		sq_pushnull(sqvm);
		//lua_insert(sqvm, -2);
		return 2;
	}
	return 1;
}

SQ_OPT_STRING_STRLEN();

/* Read the options common to mix_loadbuffer and mix_loadfile and store them in
 * the state.
 */
static SQRESULT mix_stateopt(HSQUIRRELVM sqvm, mix_state_t *S) {
    SQ_FUNC_VARS(sqvm);

	SQ_OPT_STRING(sqvm, 3, code_start, "{%");
	S->code_start = code_start;
	S->code_startsize = code_start_size;
	if (S->code_startsize == 0)
		return sq_throwerror(sqvm, _SC("code_start separator cannot be empty"));

	SQ_OPT_STRING(sqvm, 4, code_end, "%}");
	S->code_end = code_end;
	S->code_endsize = code_end_size;
	if (S->code_endsize == 0)
		return sq_throwerror(sqvm, _SC("code_end separator cannot be empty"));

	SQ_OPT_STRING(sqvm, 5, expr_code, "=");
	S->expr = expr_code;
	S->exprsize = expr_code_size;
    if (S->exprsize == 0)
            return sq_throwerror(sqvm, _SC("expr separator cannot be empty"));

	SQ_OPT_STRING(sqvm, 6, print_code, "mix_write");
    if (print_code_size == 0)
            return sq_throwerror(sqvm, _SC("mix_write function name cannot be empty"));


    snprintf(S->print_out, sizeof(S->print_out), "%s(\"", print_code);
	S->print_outsize = strlen(S->print_out);
	S->result_size = 0;
	return 0;
}

static SQRESULT mix_loadfile(HSQUIRRELVM sqvm) {
	const SQChar  *filename;
	FILE        *file;
	mix_state_t  S;
    sq_mix_init(&S, 0, 0, 0,0,0,0);

	sq_getstring(sqvm, 2, &filename);
	SQRESULT rc = mix_stateopt(sqvm, &S);
	if(rc) return rc;

	file = fopen(filename, _SC("r"));
	if (file == NULL) {
		return sq_throwerror(sqvm, _SC("cannot open file <%s>"), filename);
	}

	fseek(file, 0, SEEK_END);
	S.size = ftell(file);
	fseek(file, 0, SEEK_SET);

	SQBlob buffer(0, S.size);
	if (fread(buffer.GetBuf(), S.size, 1, file) != 1) {
		fclose(file);
		return sq_throwerror(sqvm, _SC("cannot read file <%s>"), filename);
	}
	S.buffer = (const char*)buffer.GetBuf();
	fclose(file);

	return mix_loadbuffer(sqvm, &S, filename);
}

static SQRESULT mix_loadstring(HSQUIRRELVM sqvm) {
	mix_state_t S;
	sq_mix_init(&S, 0, 0, 0,0,0,0);

	sq_getstring(sqvm, 2, &S.buffer);
	S.size = sq_getsize(sqvm, 2);
	mix_stateopt(sqvm, &S);

	return mix_loadbuffer(sqvm, &S, "chunk");
}

#define _DECL_MIX_FUNC(name,nparams,pmask) {_SC(#name), mix_##name,nparams,pmask}
static SQRegFunction mix_obj_funcs[]={
	_DECL_MIX_FUNC(loadfile,-2,_SC(".sssss")),
	_DECL_MIX_FUNC(loadstring,-2,_SC(".sssss")),
	{0,0}
};
#undef _DECL_MIX_FUNC

#ifdef __cplusplus
extern "C" {
#endif
/* This defines a function that opens up your library. */
SQRESULT sq_register_mix (HSQUIRRELVM sqvm) {
    //add a namespace sqmix
	sq_pushstring(sqvm,_SC("sqmix"),-1);
	sq_newclass(sqvm,SQFalse);
    sq_insert_reg_funcs(sqvm, mix_obj_funcs);
	sq_newslot(sqvm,-3,SQTrue); //add sqmix table to the root table

	return SQ_OK;
}

#ifdef __cplusplus
}
#endif
