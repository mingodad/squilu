/* see copyright notice in squirrel.h */
#include <squirrel.h>
#include <math.h>
#include <stdlib.h>
#include <sqstdmath.h>

#define SINGLE_ARG_FUNC(_funcname) static SQInteger math_##_funcname(HSQUIRRELVM v){ \
	SQFloat f; \
	sq_getfloat(v,2,&f); \
	sq_pushfloat(v,(SQFloat)_funcname(f)); \
	return 1; \
}

#define TWO_ARGS_FUNC(_funcname) static SQInteger math_##_funcname(HSQUIRRELVM v){ \
	SQFloat p1,p2; \
	sq_getfloat(v,2,&p1); \
	sq_getfloat(v,3,&p2); \
	sq_pushfloat(v,(SQFloat)_funcname(p1,p2)); \
	return 1; \
}

static SQInteger math_srand(HSQUIRRELVM v)
{
	SQInteger i;
	if(SQ_FAILED(sq_getinteger(v,2,&i)))
		return sq_throwerror(v,_SC("invalid param"));
	srand((unsigned int)i);
	return 0;
}

static SQInteger math_rand(HSQUIRRELVM v)
{
	sq_pushinteger(v,rand());
	return 1;
}

static SQInteger math_abs(HSQUIRRELVM v)
{
	SQInteger n;
	sq_getinteger(v,2,&n);
	sq_pushinteger(v,(SQInteger)abs((int)n));
	return 1;
}

SINGLE_ARG_FUNC(sqrt)
SINGLE_ARG_FUNC(fabs)
SINGLE_ARG_FUNC(sin)
SINGLE_ARG_FUNC(cos)
SINGLE_ARG_FUNC(asin)
SINGLE_ARG_FUNC(acos)
SINGLE_ARG_FUNC(log)
SINGLE_ARG_FUNC(log10)
SINGLE_ARG_FUNC(tan)
SINGLE_ARG_FUNC(atan)
TWO_ARGS_FUNC(atan2)
TWO_ARGS_FUNC(pow)
SINGLE_ARG_FUNC(floor)
SINGLE_ARG_FUNC(ceil)
SINGLE_ARG_FUNC(exp)

//DAD start
#include <string.h>
#include <stdio.h>
#include <ctype.h>
SQ_OPT_STRING_STRLEN();

/* mathB_roundf: rounds real value x to the d-th digit; patched Dec 22, 2007 for negative values */
static SQRESULT math_roundf (HSQUIRRELVM v) {
  SQ_FUNC_VARS(v);
  SQ_GET_FLOAT(v, 2, x);
  SQ_OPT_INTEGER(v, 3, dec_places, 0);
  SQFloat dec_factor;
  if( dec_places>15 || dec_places<0 ) return sq_throwerror(v, "decimal places out of range 0-15");
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
  if( dec_places>15 || dec_places<0 ) return sq_throwerror(v, "decimal places out of range 0-15");
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

static char math_number_format_dec_point[2] = ".";
static char math_number_format_thousand_sep[2] = ",";

static SQRESULT math_number_format_get_dec_point(HSQUIRRELVM v) {
	sq_pushstring(v, math_number_format_dec_point, -1);
	return 1;
}

static SQRESULT math_number_format_set_dec_point(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, dec_point);
	math_number_format_dec_point[0] = *dec_point;
	math_number_format_dec_point[1] = '\0';
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
    SQ_OPT_STRING(v, 4, thousand_sep, math_number_format_thousand_sep);
	char tmpbuf[64], resbuf[64];
	char *s, *t;  /* source, target */
	char *dp;
	int integral;
	int tmplen, reslen=0;
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

	tmplen = scsnprintf(tmpbuf, sizeof(tmpbuf), "%.*f", dec, d);

	resbuf[0] = '\0';

	if (isdigit((int)tmpbuf[0])) {

		/* find decimal point, if expected */
		if (dec) {
			dp = strpbrk(tmpbuf, ".,");
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

//DAD end

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),math_##name,nparams,tycheck}
static SQRegFunction mathlib_funcs[] = {
	_DECL_FUNC(sqrt,2,_SC(".n")),
	_DECL_FUNC(sin,2,_SC(".n")),
	_DECL_FUNC(cos,2,_SC(".n")),
	_DECL_FUNC(asin,2,_SC(".n")),
	_DECL_FUNC(acos,2,_SC(".n")),
	_DECL_FUNC(log,2,_SC(".n")),
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
	_DECL_FUNC(fabs,2,_SC(".n")),
	_DECL_FUNC(abs,2,_SC(".n")),
	_DECL_FUNC(roundf,-2,_SC(".ni")),
	_DECL_FUNC(broundf,-2,_SC(".ni")),
	_DECL_FUNC(number_format,-2,_SC(".niss")),
	_DECL_FUNC(number_format_get_dec_point,1,_SC(".")),
	_DECL_FUNC(number_format_set_dec_point,2,_SC(".s")),
	_DECL_FUNC(number_format_get_thousand_sep,1,_SC(".")),
	_DECL_FUNC(number_format_set_thousand_sep,2,_SC(".s")),
	{0,0},
};
#undef _DECL_FUNC

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

SQRESULT sqstd_register_mathlib(HSQUIRRELVM v)
{
    sq_pushstring(v,_SC("math"),-1);
    sq_newclass(v,SQFalse);

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
	sq_pushstring(v,_SC("PI"),-1);
	sq_pushfloat(v,(SQFloat)M_PI);
	sq_newslot(v,-3,SQFalse);

	sq_newslot(v,-3,SQTrue); //insert math
	return SQ_OK;
}
