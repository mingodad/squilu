#include "mpdecimal.h"
#include "squirrel.h"
#include <string.h>
SQ_OPT_STRING_STRLEN();

static const SQChar sq_decimal_ctx_TAG[] = _SC("DecimalCtx");
static const SQChar sq_decimal_TAG[] = _SC("Decimal");

#define GET_DecimalCtx_INSTANCE(v, idx) SQ_GET_INSTANCE_VAR(v, idx, mpd_context_t, ctx, sq_decimal_ctx_TAG)
#define GET_Decimal_INSTANCE(v, idx) SQ_GET_INSTANCE_VAR(v, idx, mpd_t, dec, sq_decimal_TAG)
#define GET_Decimal_INSTANCE2(v, idx) SQ_GET_INSTANCE_VAR(v, idx, mpd_t, dec2, sq_decimal_TAG)


static SQRESULT sq_DecimalCtx_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v) {
    mpd_context_t *ctx = (mpd_context_t *)p;
    if(ctx) sq_free(ctx, sizeof(mpd_context_t));
    return 0;
}

/*
** Creates a new DecimalCtx.
*/
static SQRESULT sq_DecimalCtx_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    SQ_OPT_INTEGER(v, 2, prec, 8);
    if(prec < 0) return sq_throwerror(v, _SC("invalid precision (%d)"), prec);
    ctx = (mpd_context_t *)sq_malloc(sizeof(mpd_context_t));
    mpd_init(ctx, prec);
    sq_setinstanceup(v, 1, ctx);
    sq_setreleasehook(v, 1, sq_DecimalCtx_release_hook);
    return 1;
}

static SQRESULT sq_DecimalCtx_prec(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, prec);
        if(prec < 0) return sq_throwerror(v, _SC("invalid precision (%d)"), prec);
        mpd_qsetprec(ctx, prec);
        return 0;
    }
    else sq_pushinteger(v, mpd_getprec(ctx));
	return 1;
}

static SQRESULT sq_DecimalCtx_emax(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, emax);
        if(emax < 0) return sq_throwerror(v, _SC("invalid emax (%d)"), emax);
        mpd_qsetemax(ctx, emax);
        return 0;
    }
    else sq_pushinteger(v, mpd_getemax(ctx));
	return 1;
}

static SQRESULT sq_DecimalCtx_emin(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, emin);
        if(emin < 0) return sq_throwerror(v, _SC("invalid emin (%d)"), emin);
        mpd_qsetemin(ctx, emin);
        return 0;
    }
    else sq_pushinteger(v, mpd_getemin(ctx));
	return 1;
}

static SQRESULT sq_DecimalCtx_round(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, round);
        if(round < 0) return sq_throwerror(v, _SC("invalid round (%d)"), round);
        mpd_qsetround(ctx, round);
        return 0;
    }
    else sq_pushinteger(v, mpd_getround(ctx));
	return 1;
}

static SQRESULT sq_DecimalCtx_traps(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, flags);
        if(flags < 0) return sq_throwerror(v, _SC("invalid traps (%d)"), flags);
        mpd_qsettraps(ctx, flags);
        return 0;
    }
    else sq_pushinteger(v, mpd_gettraps(ctx));
	return 1;
}

static SQRESULT sq_DecimalCtx_status(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, status);
        if(status < 0) return sq_throwerror(v, _SC("invalid status (%d)"), status);
        mpd_qsetstatus(ctx, status);
        return 0;
    }
    else sq_pushinteger(v, mpd_getstatus(ctx));
	return 1;
}

static SQRESULT sq_DecimalCtx_clamp(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, clamp);
        if(clamp < 0) return sq_throwerror(v, _SC("invalid clamp (%d)"), clamp);
        mpd_qsetclamp(ctx, clamp);
        return 0;
    }
    else sq_pushinteger(v, mpd_getclamp(ctx));
	return 1;
}

static SQRESULT sq_DecimalCtx_cr(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_DecimalCtx_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, cr);
        if(cr < 0) return sq_throwerror(v, _SC("invalid cr (%d)"), cr);
        mpd_qsetcr(ctx, cr);
        return 0;
    }
    else sq_pushinteger(v, mpd_getcr(ctx));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_DecimalCtx_##name,nparams,tycheck}
static SQRegFunction DecimalCtx_methods[] =
{
    _DECL_FUNC(constructor,-1,_SC("xi")),
    _DECL_FUNC(prec,-1,_SC("xi")),
    _DECL_FUNC(emax,-1,_SC("xi")),
    _DECL_FUNC(emin,-1,_SC("xi")),
    _DECL_FUNC(round,-1,_SC("xi")),
    _DECL_FUNC(traps,-1,_SC("xi")),
    _DECL_FUNC(status,-1,_SC("xi")),
    _DECL_FUNC(clamp,-1,_SC("xi")),
    _DECL_FUNC(cr,-1,_SC("xi")),
    {0,0}
};
#undef _DECL_FUNC

static mpd_context_t * sq_get_global_ctx(HSQUIRRELVM v, SQInteger idx)
{
    sq_pushliteral(v, _SC("gctx"));
    sq_get(v, idx);
    mpd_context_t *ctx = 0;
    sq_getinstanceup(v, -1, (void**)&ctx, (void*)sq_decimal_ctx_TAG);
    sq_poptop(v);
	return ctx;
}

static SQRESULT sq_Decimal_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v) {
    mpd_t *dec = (mpd_t *)p;
    if(dec) mpd_del(dec);
    return 0;
}

/*
** Creates a new Decimal.
*/
static SQRESULT sq_Decimal_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    GET_Decimal_INSTANCE(v, 1);
    if(_top_ > 1){
        switch(sq_gettype(v, 2)){
            case OT_INSTANCE:{
                GET_DecimalCtx_INSTANCE(v, 2);
                dec = mpd_new(ctx);
            }
            case OT_INTEGER:{
                SQ_GET_INTEGER(v, 2, iparam);
                mpd_context_t *ctx = sq_get_global_ctx(v, 1);
                dec = mpd_new(ctx);
                uint32_t status;
                mpd_qset_i32(dec, iparam, ctx, &status);
            }
            break;
            case OT_STRING:{
                SQ_GET_STRING(v, 2, str);
                mpd_context_t *ctx = sq_get_global_ctx(v, 1);
                dec = mpd_new(ctx);
                uint32_t status;
                mpd_qset_string(dec, str, ctx, &status);
            }
            break;
            case OT_FLOAT:{
                SQ_GET_FLOAT(v, 2, fparam);
                mpd_context_t *ctx = sq_get_global_ctx(v, 1);
                dec = mpd_new(ctx);
                uint32_t status;
                char buf[32];
                snprintf(buf, sizeof(buf), "%f", fparam);
                mpd_qset_string(dec, buf, ctx, &status);
            }
            break;
        }
    }
    else dec = mpd_qnew();
    sq_setinstanceup(v, 1, dec);
    sq_setreleasehook(v, 1, sq_Decimal_release_hook);
    return 1;
}

static SQRESULT sq_Decimal_error(HSQUIRRELVM v, uint32_t status) {
    SQChar *error = _SC("MPD_??");
#define CASE_ERROR(n) case n: error = #n; break;
    switch(status){
        CASE_ERROR(MPD_Clamped);
        CASE_ERROR(MPD_Conversion_syntax);
        CASE_ERROR(MPD_Division_by_zero );
        CASE_ERROR(MPD_Division_impossible);
        CASE_ERROR(MPD_Division_undefined);
        CASE_ERROR(MPD_Fpu_error);
        CASE_ERROR(MPD_Inexact);
        CASE_ERROR(MPD_Invalid_context);
        CASE_ERROR(MPD_Invalid_operation);
        CASE_ERROR(MPD_Malloc_error);
        CASE_ERROR(MPD_Not_implemented);
        CASE_ERROR(MPD_Overflow);
        CASE_ERROR(MPD_Rounded);
        CASE_ERROR(MPD_Subnormal);
        CASE_ERROR(MPD_Underflow);
    }
#undef CASE_ERROR
    return sq_throwerror(v, error);
}

static SQRESULT sq_Decimal_new_for_dec (HSQUIRRELVM v, mpd_t *dec, mpd_context_t *ctx, uint32_t status) {
    //mpd_addstatus_raise(ctx, status);
	ctx->status |= status;
	if (status&ctx->traps) return sq_Decimal_error(v, status);

    sq_pushstring(v, sq_decimal_TAG, -1);
    sq_getonroottable(v);
    sq_createinstance(v, -1);
    sq_setinstanceup(v, -1, dec);
    sq_setreleasehook(v, -1, sq_Decimal_release_hook);
    return 1;
}

static SQRESULT sq_Decimal_tostring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    char *decstring =  mpd_to_eng(dec, 1);
    sq_pushstring(v, decstring, -1);
    if (decstring) mpd_free(decstring);
	return 1;
}

static SQRESULT sq_Decimal__add(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qadd(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal__sub(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qsub(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal__mul(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qmul(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal__div(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qdiv(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal__modulo(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qrem(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal__unm(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qminus(result, dec, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal__cmp(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    sq_pushinteger(v, mpd_cmp(dec, dec2, ctx));
	return 1;
}

static SQRESULT sq_Decimal_abs(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qabs(result, dec, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal_max(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qmax(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal_min(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qmin(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal_next_minus(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qnext_minus(result, dec, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal_next_plus(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qnext_plus(result, dec, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

static SQRESULT sq_Decimal_next_toward(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_Decimal_INSTANCE(v, 1);
    GET_Decimal_INSTANCE2(v, 2);
    mpd_context_t *ctx = sq_get_global_ctx(v, 1);
    mpd_t *result = mpd_new(ctx);
    uint32_t status = 0;
    mpd_qnext_toward(result, dec, dec2, ctx, &status);
	return sq_Decimal_new_for_dec(v, result, ctx, status);
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_Decimal_##name,nparams,tycheck}
static SQRegFunction Decimal_methods[] =
{
    _DECL_FUNC(constructor,-1,_SC("x x|n|s")),
    {_SC("_tostring"),sq_Decimal_tostring, 1,_SC("x")},
    _DECL_FUNC(tostring,1,_SC("x")),
    _DECL_FUNC(_add, 2,_SC("xx")),
    _DECL_FUNC(_sub, 2,_SC("xx")),
    _DECL_FUNC(_mul, 2,_SC("xx")),
    _DECL_FUNC(_div, 2,_SC("xx")),
    _DECL_FUNC(_modulo, 2,_SC("xx")),
    _DECL_FUNC(_unm, 1,_SC("x")),
    _DECL_FUNC(_cmp, 2,_SC("xx")),
    _DECL_FUNC(abs, 1,_SC("x")),
    _DECL_FUNC(max, 2,_SC("xx")),
    _DECL_FUNC(min, 2,_SC("xx")),
    _DECL_FUNC(next_minus, 1,_SC("x")),
    _DECL_FUNC(next_plus, 1,_SC("x")),
    _DECL_FUNC(next_toward, 2,_SC("xx")),
    {0,0}
};
#undef _DECL_FUNC


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_decimal(HSQUIRRELVM v)
{
    sq_pushstring(v,sq_decimal_ctx_TAG,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_decimal_ctx_TAG);
    sq_insert_reg_funcs(v, DecimalCtx_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,sq_decimal_TAG,-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)sq_decimal_TAG);

	sq_pushliteral(v, _SC("gctx"));
	sq_pushstring(v,sq_decimal_ctx_TAG,-1);
	sq_getonroottable(v);
	sq_pushroottable(v);
	sq_call(v, 1, SQTrue, SQFalse);
	sq_remove(v, -2);
    sq_newslot(v,-3,SQTrue);

    sq_insert_reg_funcs(v, Decimal_methods);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif
