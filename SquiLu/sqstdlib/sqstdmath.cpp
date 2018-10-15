/* see copyright notice in squirrel.h */
#include <squirrel.h>
#include <math.h>
//#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <sqstdmath.h>

/* Some useful constants.  */

#ifndef M_E
# define M_E		2.7182818284590452354	/* e */
#endif

#ifndef M_LOG2E
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
#endif

#ifndef M_LOG10E
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
#endif

#ifndef M_LN2
# define M_LN2		0.69314718055994530942	/* log_e 2 */
#endif

#ifndef M_LN10
# define M_LN10		(2.30258509299404568402)	/* log_e 10 */
#endif

#ifndef M_PI
# define M_PI		(3.14159265358979323846)	/* pi */
#endif

#ifndef M_PI_2
# define M_PI_2		(1.57079632679489661923)	/* pi/2 */
#endif

#ifndef M_PI_4
# define M_PI_4		(0.78539816339744830962)	/* pi/4 */
#endif

#ifndef M_1_PI
# define M_1_PI		(0.31830988618379067154)	/* 1/pi */
#endif

#ifndef M_2_PI
# define M_2_PI		(0.63661977236758134308)	/* 2/pi */
#endif

#ifndef M_2_SQRTPI
# define M_2_SQRTPI	(1.12837916709551257390)	/* 2/sqrt(pi) */
#endif

#ifndef M_SQRT2
# define M_SQRT2	(1.41421356237309504880)	/* sqrt(2) */
#endif

#ifndef M_SQRT1_2
# define M_SQRT1_2	(0.70710678118654752440)	/* 1/sqrt(2) */
#endif

#define SINGLE_ARG_FUNC(_funcname) static SQRESULT math_##_funcname(HSQUIRRELVM v){ \
	SQFloat f; \
	sq_getfloat(v,2,&f); \
	sq_pushfloat(v,(SQFloat)_funcname(f)); \
	return 1; \
}

#define TWO_ARGS_FUNC(_funcname) static SQRESULT math_##_funcname(HSQUIRRELVM v){ \
	SQFloat p1,p2; \
	sq_getfloat(v,2,&p1); \
	sq_getfloat(v,3,&p2); \
	sq_pushfloat(v,(SQFloat)_funcname(p1,p2)); \
	return 1; \
}

#define BOOL_SINGLE_ARG_FUNC(_funcname) static SQRESULT math_##_funcname(HSQUIRRELVM v){ \
	SQFloat f; \
	sq_getfloat(v,2,&f); \
	sq_pushbool(v,(SQBool)_funcname(f)); \
	return 1; \
}

static SQRESULT math_srand(HSQUIRRELVM v)
{
	SQInteger i;
	if(SQ_FAILED(sq_getinteger(v,2,&i)))
		return sq_throwerror(v,_SC("invalid param"));
	srand((unsigned int)i);
	return 0;
}

static SQRESULT math_rand(HSQUIRRELVM v)
{
	sq_pushinteger(v,rand());
	return 1;
}

static SQRESULT math_random(HSQUIRRELVM v) {
  SQ_FUNC_VARS(v);
  SQInteger low, up;
  SQFloat r = (SQFloat)rand() * (1.0 / ((SQFloat)RAND_MAX + 1.0));
  switch (_top_) {  /* check number of arguments */
    case 1: {  /* no arguments */
      sq_pushfloat(v, r);  /* Number between 0 and 1 */
      return 1;
    }
    case 2: {  /* only upper limit */
      low = 1;
      SQ_GET_INTEGER_NVD(v, 2, up);
      break;
    }
    case 3: {  /* lower and upper limits */
      SQ_GET_INTEGER_NVD(v, 2, low);
      SQ_GET_INTEGER_NVD(v, 3, up);
      break;
    }
    default: return sq_throwerror(v, _SC("wrong number of arguments"));
  }
  /* random integer in the interval [low, up] */
  if(low >= up) return sq_throwerror(v, _SC("interval is empty"));
  if(low <= 0 && up >= SQ_INT_MAX + low)
    return sq_throwerror(v, _SC("interval too large"));
  r *= (SQFloat)(up - low) + 1.0;
  sq_pushinteger(v, (SQInteger)r + low);
  return 1;
}

static SQRESULT math_abs(HSQUIRRELVM v)
{
    SQInteger n;
    sq_getinteger(v,2,&n);
    sq_pushinteger(v,(SQInteger)::abs(n));
	return 1;
}

static SQRESULT math_log(HSQUIRRELVM v) {
  SQ_FUNC_VARS(v);
  SQ_GET_FLOAT(v, 2, num);
  switch (_top_) {  /* check number of arguments */
    case 2: {  /* only num */
      sq_pushfloat(v,log(num));
      break;
    }
    case 3: {  /* num and base */
      SQ_GET_FLOAT(v, 3, base);
      sq_pushfloat(v,log(num)/log(base));
      break;
    }
  }
  return 1;
}

SINGLE_ARG_FUNC(sqrt)
SINGLE_ARG_FUNC(fabs)
SINGLE_ARG_FUNC(sin)
SINGLE_ARG_FUNC(asin)
SINGLE_ARG_FUNC(cos)
SINGLE_ARG_FUNC(acos)
//SINGLE_ARG_FUNC(log)
SINGLE_ARG_FUNC(log10)
SINGLE_ARG_FUNC(tan)
SINGLE_ARG_FUNC(atan)
TWO_ARGS_FUNC(atan2)
TWO_ARGS_FUNC(pow)
SINGLE_ARG_FUNC(floor)
SINGLE_ARG_FUNC(ceil)
SINGLE_ARG_FUNC(exp)

SINGLE_ARG_FUNC(acosh)
SINGLE_ARG_FUNC(asinh)
SINGLE_ARG_FUNC(tanh)
SINGLE_ARG_FUNC(atanh)

BOOL_SINGLE_ARG_FUNC(isnan)
BOOL_SINGLE_ARG_FUNC(isfinite)

//DAD start
#include <string.h>
#include <stdio.h>
#include <ctype.h>
SQ_OPT_STRING_STRLEN()

/* mathB_roundf: rounds real value x to the d-th digit; patched Dec 22, 2007 for negative values */
static SQRESULT math_roundf (HSQUIRRELVM v) {
  SQ_FUNC_VARS(v);
  SQ_GET_FLOAT(v, 2, x);
  SQ_OPT_INTEGER(v, 3, dec_places, 0);
  SQFloat dec_factor;
  if( dec_places>15 || dec_places<0 ) return sq_throwerror(v, _SC("decimal places out of range 0-15"));
  switch(dec_places){
	case 0: dec_factor = 1.0; break;
	case 1: dec_factor = 10.0; break;
	case 2: dec_factor = 100.0; break;
	case 3: dec_factor = 1000.0; break;
	case 4: dec_factor = 10000.0; break;
	case 5: dec_factor = 100000.0; break;
	case 6: dec_factor = 1000000.0; break;
	default: dec_factor = pow((double)10.0, dec_places);
  }
  if (x < 0)
    sq_pushfloat(v, ceil(dec_factor*x-0.5) / dec_factor);
  else
    sq_pushfloat(v, floor(dec_factor*x+0.5) / dec_factor);

  return 1;
}

static SQRESULT math_broundf (HSQUIRRELVM v) {
  SQ_FUNC_VARS(v);
  SQ_GET_FLOAT(v, 2, num);
  SQ_OPT_INTEGER(v, 3, dec_places, 0);
  SQFloat dec_factor, tmp, itmp;
  int neg;
  if( dec_places>15 || dec_places<0 ) return sq_throwerror(v, _SC("decimal places out of range 0-15"));
  neg = num < 0;
  switch(dec_places){
	case 0: dec_factor = 1.0; break;
	case 1: dec_factor = 10.0; break;
	case 2: dec_factor = 100.0; break;
	case 3: dec_factor = 1000.0; break;
	case 4: dec_factor = 10000.0; break;
	case 5: dec_factor = 100000.0; break;
	case 6: dec_factor = 1000000.0; break;
	default: dec_factor = pow((double)10.0, dec_places);
  }
  tmp = num * dec_factor;
  itmp = floor(tmp + (neg ? -0.5 : 0.5));
  // Handle rounding of .5 in a special manner
  if(tmp - floor(tmp) == 0.5){
    tmp = itmp / 2;
    if(tmp != floor(tmp) ) { // Is itmp odd
	  // Reduce Magnitude by 1 to make even
	  if(neg) itmp++;
	  else itmp--;
    }
  }

  sq_pushfloat(v, itmp / dec_factor);
  return 1;
}

static SQChar math_number_format_dec_point[2] = _SC(".");
static SQChar math_number_format_thousand_sep[2] = _SC(",");

static SQRESULT math_number_format_get_dec_point(HSQUIRRELVM v) {
	sq_pushstring(v, math_number_format_dec_point, -1);
	return 1;
}

static SQRESULT math_number_format_set_dec_point(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, dec_point);
	math_number_format_dec_point[0] = *dec_point;
	math_number_format_dec_point[1] = _SC('\0');
	return 0;
}

static SQRESULT math_number_format_get_thousand_sep(HSQUIRRELVM v) {
	sq_pushstring(v, math_number_format_thousand_sep, -1);
	return 1;
}

static SQRESULT math_number_format_set_thousand_sep(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, thousand_sep);
	math_number_format_thousand_sep[0] = *thousand_sep;
	math_number_format_thousand_sep[1] = '\0';
	return 0;
}

static SQRESULT math_number_format(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_FLOAT(v, 2, d);
    SQ_OPT_INTEGER(v, 3, dec, 2);
    SQ_OPT_STRING(v, 4, dec_point, math_number_format_dec_point);
    SQ_OPT_STRING(v, 5, thousand_sep, math_number_format_thousand_sep);
	SQChar tmpbuf[64], resbuf[64];
	SQChar *s, *t;  /* source, target */
	SQChar *dp;
	int integral;
	size_t tmplen, reslen=0;
	int count=0;
	int is_negative=0;
	int trim_right_zeros=0;

	if (d < 0) {
		is_negative = 1;
		d = -d;
	}

	if (dec < 0) {
		trim_right_zeros = 1;
		dec = -dec;
	}

	tmplen = scsprintf(tmpbuf, sizeof(tmpbuf), _SC("%.*f"), (int)dec, d);

	resbuf[0] = _SC('\0');

	if (isdigit((int)tmpbuf[0])) {

		/* find decimal point, if expected */
		if (dec) {
			dp = scstrpbrk(tmpbuf, _SC(".,"));
		} else {
			dp = NULL;
		}

		/* calculate the length of the return buffer */
		if (dp) {
			integral = dp - tmpbuf;
		} else {
			/* no decimal point was found */
			integral = tmplen;
		}

		/* allow for thousand separators */
		if (*thousand_sep) {
			integral += (integral-1) / 3;
		}

		reslen = integral;

		if (dec) {
			reslen += dec;

			if (*dec_point) {
				reslen++;
			}
		}

		/* add a byte for minus sign */
		if (is_negative) {
			reslen++;
		}
		if(sizeof(resbuf) >= reslen+1) {

			s = tmpbuf+tmplen-1;
			t = resbuf+reslen;
			*t-- = '\0';

			/* copy the decimal places.
			 * Take care, as the sprintf implementation may return less places than
			 * we requested due to internal buffer limitations */
			if (dec) {
				int declen = dp ? s - dp : 0;
				int topad = dec > declen ? dec - declen : 0;

				/* pad with '0's */
				while (topad--) {
					*t-- = '0';
				}

				if (dp) {
					s -= declen + 1; /* +1 to skip the point */
					t -= declen;

					/* now copy the chars after the point */
					memcpy(t + 1, dp + 1, declen);
				}

				/* add decimal point */
				if (*dec_point) {
					*t-- = *dec_point;
				}
			}

			/* copy the numbers before the decimal point, adding thousand
			 * separator every three digits */
			while(s >= tmpbuf) {
				*t-- = *s--;
				if (*thousand_sep && (++count%3)==0 && s>=tmpbuf) {
					*t-- = *thousand_sep;
				}
			}

			/* and a minus sign, if needed */
			if (is_negative) {
				*t-- = '-';
			}

			/* trim right zeros */
			if (*dec_point && trim_right_zeros) {
				for(t=resbuf+reslen-1; *t != *dec_point; t--){
					if (*t == '0') *t = '\0';
					else break;
				}
				if (*t == *dec_point) *t = '\0';
			}
		}
	}

	sq_pushstring(v, resbuf, -1);
	return 1;
}

static SQRESULT math_ult(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, n1);
    SQ_GET_INTEGER(v, 3, n2);
	sq_pushbool(v,((SQUnsignedInteger)n1) < ((SQUnsignedInteger)n2));
	return 1;
}

static SQRESULT math_min (HSQUIRRELVM v) {
    SQInteger n = sq_gettop(v);  /* number of arguments */
    SQInteger imin = 2;  /* index of current minimum value */
    for (SQInteger i = 3; i <= n; i++) {
    if (sq_compare(v, i, imin) < 0)
      imin = i;
    }
    sq_push(v, imin);
    return 1;
}

static SQRESULT math_max (HSQUIRRELVM v) {
    SQInteger n = sq_gettop(v);  /* number of arguments */
    SQInteger imax = 2;  /* index of current minimum value */
    for (SQInteger i = 3; i <= n; i++) {
    if (sq_compare(v, i, imax) > 0)
      imax = i;
    }
    sq_push(v, imax);
    return 1;
}

static SQRESULT math_deg (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FLOAT(v, 2, n);
    sq_pushfloat(v, n * (180.0 / M_PI));
    return 1;
}

static SQRESULT math_rad (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FLOAT(v, 2, n);
    sq_pushfloat(v, n * (M_PI / 180.0));
    return 1;
}

//DAD end

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),math_##name,nparams,tycheck,false}
static const SQRegFunction mathlib_funcs[] = {
	_DECL_FUNC(sqrt,2,_SC(".n")),
	_DECL_FUNC(sin,2,_SC(".n")),
	_DECL_FUNC(cos,2,_SC(".n")),
	_DECL_FUNC(asin,2,_SC(".n")),
	_DECL_FUNC(acos,2,_SC(".n")),
	_DECL_FUNC(log,-2,_SC(".nn")),
	_DECL_FUNC(log10,2,_SC(".n")),
	_DECL_FUNC(tan,2,_SC(".n")),
	_DECL_FUNC(atan,2,_SC(".n")),
	_DECL_FUNC(atan2,3,_SC(".nn")),
	_DECL_FUNC(pow,3,_SC(".nn")),
	_DECL_FUNC(floor,2,_SC(".n")),
	_DECL_FUNC(ceil,2,_SC(".n")),
	_DECL_FUNC(exp,2,_SC(".n")),
	_DECL_FUNC(srand,2,_SC(".n")),
	_DECL_FUNC(rand,1,NULL),
	_DECL_FUNC(random,-1,_SC(".ii")),
	_DECL_FUNC(fabs,2,_SC(".n")),
	_DECL_FUNC(abs,2,_SC(".n")),
	_DECL_FUNC(isnan,2,_SC(".n")),
	_DECL_FUNC(isfinite,2,_SC(".n")),
	_DECL_FUNC(roundf,-2,_SC(".ni")),
	_DECL_FUNC(broundf,-2,_SC(".ni")),
	_DECL_FUNC(number_format,-2,_SC(".niss")),
	_DECL_FUNC(number_format_get_dec_point,1,_SC(".")),
	_DECL_FUNC(number_format_set_dec_point,2,_SC(".s")),
	_DECL_FUNC(number_format_get_thousand_sep,1,_SC(".")),
	_DECL_FUNC(number_format_set_thousand_sep,2,_SC(".s")),
	_DECL_FUNC(ult,3,_SC(".nn")),
	_DECL_FUNC(min,-3,_SC(".nn")),
	_DECL_FUNC(max,-3,_SC(".nn")),
	_DECL_FUNC(rad,2,_SC(".n")),
	_DECL_FUNC(deg,2,_SC(".n")),

	_DECL_FUNC(asinh,2,_SC(".n")),
	_DECL_FUNC(acosh,2,_SC(".n")),
	_DECL_FUNC(tanh,2,_SC(".n")),
	_DECL_FUNC(atanh,2,_SC(".n")),

	{NULL,(SQFUNCTION)0,0,NULL,false}
};
#undef _DECL_FUNC

static void installFloatConst(HSQUIRRELVM v, const SQChar *skey, SQFloat fv)
{
	sq_pushstring(v,skey,-1);
	sq_pushfloat(v,fv);
	sq_newslot(v,-3,SQFalse);
}

SQRESULT sqstd_register_mathlib(HSQUIRRELVM v)
{
    sq_pushstring(v,_SC("math"),-1);
    sq_newtable(v);

	SQInteger i=0;
	while(mathlib_funcs[i].name!=0)	{
		sq_pushstring(v,mathlib_funcs[i].name,-1);
		sq_newclosure(v,mathlib_funcs[i].f,0);
		sq_setparamscheck(v,mathlib_funcs[i].nparamscheck,mathlib_funcs[i].typemask);
		sq_setnativeclosurename(v,-1,mathlib_funcs[i].name);
		sq_newslot(v,-3,SQFalse);
		i++;
	}
	sq_pushstring(v,_SC("RAND_MAX"),-1);
	sq_pushinteger(v,RAND_MAX);
	sq_newslot(v,-3,SQFalse);
	installFloatConst(v, _SC("E"), (SQFloat)M_E);
	installFloatConst(v, _SC("LOG2E"), (SQFloat)M_LOG2E);
	installFloatConst(v, _SC("LOG10E"), (SQFloat)M_LOG10E);
	installFloatConst(v, _SC("LN2"), (SQFloat)M_LN2);
	installFloatConst(v, _SC("LN10"), (SQFloat)M_LN10);
	installFloatConst(v, _SC("PI"), (SQFloat)M_PI);
	installFloatConst(v, _SC("PI_2"), (SQFloat)M_PI_2);
	installFloatConst(v, _SC("PI_4"), (SQFloat)M_PI_4);
	installFloatConst(v, _SC("1_PI"), (SQFloat)M_1_PI);
	installFloatConst(v, _SC("2_PI"), (SQFloat)M_2_PI);
	installFloatConst(v, _SC("2_SQRTPI"), (SQFloat)M_2_SQRTPI);
	installFloatConst(v, _SC("SQRT2"), (SQFloat)M_SQRT2);
	installFloatConst(v, _SC("SQRT1_2"), (SQFloat)M_SQRT1_2);
	installFloatConst(v, _SC("HUGE"), (SQFloat)HUGE_VAL);
/*
	installFloatConst(v, SC("DBL_MAX"), (SQFloat)DBL_MAX);
	installFloatConst(v, SC("DBL_MIN"), (SQFloat)DBL_MIN);
*/
	sq_pushstring(v,_SC("INT_MAX"),-1);
	sq_pushinteger(v,SQ_INT_MAX);
	sq_newslot(v,-3,SQFalse);
	sq_pushstring(v,_SC("INT_MIN"),-1);
	sq_pushinteger(v,SQ_INT_MIN);
	sq_newslot(v,-3,SQFalse);

	sq_newslot(v,-3,SQTrue); //insert math
	return SQ_OK;
}
