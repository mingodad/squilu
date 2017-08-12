/* see copyright notice in squirrel.h */
#include <squirrel.h>
#include <sqstdstring.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stddef.h>

#ifdef _WIN32_WCE
extern "C" {
int _wtoi (const wchar_t *);
}
#endif // _WINCE

#define MAX_FORMAT_LEN	20
#define MAX_WFORMAT_LEN	3
#define ADDITIONAL_FORMAT_SPACE (100*sizeof(SQChar))

static SQBool isfmtchr(SQChar ch)
{
	switch(ch) {
	case '-': case '+': case ' ': case '#': case '0': return SQTrue;
	}
	return SQFalse;
}

static SQRESULT validate_format(HSQUIRRELVM v, SQChar *fmt, const SQChar *src, SQInteger n,SQInteger &width)
{
	SQChar *dummy;
	SQChar swidth[MAX_WFORMAT_LEN];
	SQInteger wc = 0;
	SQInteger start = n;
	fmt[0] = _SC('%');
	while (isfmtchr(src[n])) n++;
	while (scisdigit(src[n])) {
		swidth[wc] = src[n];
		n++;
		wc++;
		if(wc>=MAX_WFORMAT_LEN)
			return sq_throwerror(v,_SC("width format too long"));
	}
	swidth[wc] = _SC('\0');
	if(wc > 0) {
		width = scstrtol(swidth,&dummy,10);
	}
	else
		width = 0;
	if (src[n] == _SC('.')) {
	    n++;

		wc = 0;
		while (scisdigit(src[n])) {
			swidth[wc] = src[n];
			n++;
			wc++;
			if(wc>=MAX_WFORMAT_LEN)
				return sq_throwerror(v,_SC("precision format too long"));
		}
		swidth[wc] = _SC('\0');
		if(wc > 0) {
			width += scstrtol(swidth,&dummy,10);
		}
	}
	if (n-start > MAX_FORMAT_LEN )
		return sq_throwerror(v,_SC("format too long"));
	memcpy(&fmt[1],&src[start],((n-start)+1)*sizeof(SQChar));
	fmt[(n-start)+2] = _SC('\0');
	return n;
}

SQRESULT sqstd_format(HSQUIRRELVM v,SQInteger nformatstringidx,SQInteger *outlen,SQChar **output)
{
	const SQChar *format;
	SQChar *dest;
	SQChar fmt[MAX_FORMAT_LEN];
	SQRESULT res = sq_getstring(v,nformatstringidx,&format);
	if (SQ_FAILED(res)) {
		return res; // propagate the error
	}
	SQInteger format_size = sq_getsize(v,nformatstringidx);
	SQInteger allocated = (format_size+2)*sizeof(SQChar);
	dest = sq_getscratchpad(v,allocated);
	SQInteger n = 0,i = 0, nparam = nformatstringidx+1, w = 0;
	while(n < format_size) {
		if(format[n] != _SC('%')) {
			assert(i < allocated);
			dest[i++] = format[n];
			n++;
		}
		else if(format[n+1] == _SC('%')) { //handles %%
				dest[i++] = _SC('%');
				n += 2;
		}
		else {
			n++;
			if( nparam > sq_gettop(v) )
				return sq_throwerror(v,_SC("not enough paramters for the given format string"));
			n = validate_format(v,fmt,format,n,w);
			if(n < 0) return -1;
			SQInteger addlen = 0;
			SQInteger valtype = 0;
			const SQChar *ts = NULL;
			SQInteger ti = 0;
			SQFloat tf = 0.0;
			SQInteger fc = format[n];
			switch(fc) {
            case _SC('q'):
			case _SC('s'):
				if(SQ_FAILED(sq_getstring(v,nparam,&ts)))
					return sq_throwerror(v,_SC("string expected for the specified format"));
                if(fc == _SC('q')){
                    addlen = 2; //quotes before and after
                    SQInteger size = sq_getsize(v,nparam);
                    SQChar *ts2 = (SQChar*)ts;
                      while (size--) {
                        ++addlen;
                        if (*ts2 == _SC('\r') && *(ts2+1) == _SC('\n') ) {
                          ++addlen;
                          ++ts2; --size;//eat \r and output only \n
                        }
                        else if (*ts2 == _SC('"') || *ts2 == _SC('\\') || *ts2 == _SC('\n')  || *ts2 == _SC('\t')) {
                          ++addlen;
                        }
                        else if (*ts2 == _SC('\0') || sciscntrl(SQUChar(*ts2))) {
                          SQChar buff[10];
                          addlen += scsprintf(buff, sizeof(buff), _SC("\\x%x"), (int)SQUChar(*ts2))-1; //-1 because we already added the original char to the sum
                        }
                        ++ts2;
                      }

                      ts2 = (SQChar*)i; //save the i position using pointer as integer
                      i += addlen;
                      allocated += addlen;
                      dest = sq_getscratchpad(v,allocated);
                      size = sq_getsize(v,nparam);

                      ts2 = &dest[(ptrdiff_t)ts2]; //use saved i position saved on pointer as integer
                      *ts2++ = _SC('"');
                      while (size--) {
                        if (*ts == _SC('\r') && *(ts+1) == _SC('\n') ) {
                          ++ts; --size;//eat \r and output only \n
                        }
                        if (*ts == _SC('"') || *ts == _SC('\\')) {
                            *ts2++ = _SC('\\');
                            *ts2++ = *ts;
                        }
                        else if (*ts == _SC('\n')) {
                            *ts2++ = _SC('\\');
                            *ts2++ = _SC('n');
                        }
                        else if (*ts == _SC('\t')) {
                            *ts2++ = _SC('\\');
                            *ts2++ = _SC('t');
                        }
                        else if (*ts == _SC('\0') || sciscntrl(SQUChar(*ts))) {
                          SQChar buff[10];
                          int iw;
                          iw = scsprintf(buff, sizeof(buff), _SC("\\x%x"), (int)SQUChar(*ts));
                          for(int i=0; i< iw; ++i) *ts2++ = buff[i];
                        }
                        else
                            *ts2++ = *ts;
                        ++ts;
                      }
                      *ts2++ = _SC('"');

                      ++n;
                      ++nparam;
                      continue;
                }
                else
                {
                    addlen = (sq_getsize(v,nparam)*sizeof(SQChar))+((w+1)*sizeof(SQChar));
                    valtype = _SC('s');
                }
				break;
			case _SC('i'): case _SC('d'): case _SC('o'): case _SC('u'):  case _SC('x'):  case _SC('X'):
#ifdef _SQ64
				{
				size_t flen = scstrlen(fmt);
				SQInteger fpos = flen - 1;
				SQChar f = fmt[fpos];
				SQChar *prec = (SQChar *)_PRINT_INT_PREC;
				while(*prec != _SC('\0')) {
					fmt[fpos++] = *prec++;
				}
				fmt[fpos++] = f;
				fmt[fpos++] = _SC('\0');
				}
#endif
			case _SC('c'):
				if(SQ_FAILED(sq_getinteger(v,nparam,&ti)))
					return sq_throwerror(v,_SC("integer expected for the specified format"));
				addlen = (ADDITIONAL_FORMAT_SPACE)+((w+1)*sizeof(SQChar));
				valtype = _SC('i');
				break;
			case _SC('f'): case _SC('g'): case _SC('G'): case _SC('e'):  case _SC('E'):
				if(SQ_FAILED(sq_getfloat(v,nparam,&tf)))
					return sq_throwerror(v,_SC("float expected for the specified format"));
				addlen = (ADDITIONAL_FORMAT_SPACE)+((w+1)*sizeof(SQChar));
				valtype = _SC('f');
				break;
			default:
				return sq_throwerror(v,_SC("invalid format"));
			}
			n++;
			allocated += addlen + sizeof(SQChar);
			dest = sq_getscratchpad(v,allocated);
			switch(valtype) {
            case 's': i += scsprintf(&dest[i],allocated,fmt,ts); break;
            case 'i': i += scsprintf(&dest[i],allocated,fmt,ti); break;
            case 'f': i += scsprintf(&dest[i],allocated,fmt,tf); break;
			};
			nparam ++;
		}
	}
	*outlen = i;
	dest[i] = _SC('\0');
	*output = dest;
	return SQ_OK;
}

static SQRESULT _string_format(HSQUIRRELVM v)
{
	SQChar *dest = NULL;
	SQInteger length = 0;
	if(SQ_FAILED(sqstd_format(v,2,&length,&dest)))
		return -1;
	sq_pushstring(v,dest,length);
	return 1;
}

static SQRESULT _string_printf(HSQUIRRELVM v)
{
	SQChar *dest = NULL;
	SQInteger length = 0;
	SQPRINTFUNCTION sqprint = sq_getprintfunc(v);
    if(sqprint){
        if(SQ_FAILED(sqstd_format(v,2,&length,&dest)))
            return -1;
        sqprint(v,_SC("%s"),dest);
        sq_pushinteger(v, length);
        return 1;
    }
	return 0;
}

static SQInteger _string_escape(HSQUIRRELVM v)
{
    const SQChar *str;
    SQChar *dest,*resstr;
    SQInteger size;
    sq_getstring(v,2,&str);
    size = sq_getsize(v,2);
    if(size == 0) {
        sq_push(v,2);
        return 1;
    }
#ifdef SQUNICODE
#if WCHAR_SIZE == 2
    const SQChar *escpat = _SC("\\x%04x");
    const SQInteger maxescsize = 6;
#else //WCHAR_SIZE == 4
    const SQChar *escpat = _SC("\\x%08x");
    const SQInteger maxescsize = 10;
#endif
#else
    const SQChar *escpat = _SC("\\x%02x");
    const SQInteger maxescsize = 4;
#endif
    SQInteger destcharsize = (size * maxescsize); //assumes every char could be escaped
    resstr = dest = (SQChar *)sq_getscratchpad(v,destcharsize * sizeof(SQChar));
    SQChar c;
    SQChar escch;
    SQInteger escaped = 0;
    for(int n = 0; n < size; n++){
        c = *str++;
        escch = 0;
        if(scisprint(c) || c == 0) {
            switch(c) {
            case '\a': escch = 'a'; break;
            case '\b': escch = 'b'; break;
            case '\t': escch = 't'; break;
            case '\n': escch = 'n'; break;
            case '\v': escch = 'v'; break;
            case '\f': escch = 'f'; break;
            case '\r': escch = 'r'; break;
            case '\\': escch = '\\'; break;
            case '\"': escch = '\"'; break;
            case '\'': escch = '\''; break;
            case 0: escch = '0'; break;
            }
            if(escch) {
                *dest++ = '\\';
                *dest++ = escch;
                escaped++;
            }
            else {
                *dest++ = c;
            }
        }
        else {

            dest += scsprintf(dest, destcharsize, escpat, c);
            escaped++;
        }
    }

    if(escaped) {
        sq_pushstring(v,resstr,dest - resstr);
    }
    else {
        sq_push(v,2); //nothing escaped
    }
    return 1;
}

#define SETUP_REX(v) \
	SQRex *self = NULL; \
	sq_getinstanceup(v,1,(SQUserPointer *)&self,0);

static SQRESULT _rexobj_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	SQRex *self = ((SQRex *)p);
	sqstd_rex_free(self);
	return 1;
}

#define GET_EXTRA_MATCH_PARAMS(stack_pos) \
    SQ_OPT_INTEGER(v, stack_pos, start_offset, 0); \
    SQ_OPT_INTEGER(v, stack_pos+1, max_len, 0); \
    \
    if(start_offset < 0) return sq_throwerror(v, _SC("start_offset can't be negative")); \
    SQInteger subject_str_size = subject_size - start_offset; \
    if(subject_str_size < 0) return sq_throwerror(v, _SC("str size - start_offset can't be negative")); \
    const SQChar *subject_str = subject + start_offset; \
    if(max_len && (max_len < subject_str_size)) subject_str_size = max_len;
static SQRESULT _regexp_match(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
	SETUP_REX(v);
	SQ_GET_STRING(v, 2, subject);
	GET_EXTRA_MATCH_PARAMS(3);

	const SQChar *begin,*end;
	if(sqstd_rex_searchrange(self,subject_str, subject_str + subject_str_size,&begin,&end)){
	    SQInteger n = sqstd_rex_getsubexpcount(self);
	    SQRexMatch match;
        sqstd_rex_getsubexp(self, (n ? 1 : 0),&match);
        if(match.len == -1) sq_pushinteger(v, match.begin - subject); //on empty capture push position
        else sq_pushstring(v, match.begin, match.len);
	}
	else sq_pushnull(v);
	return 1;
}

static SQRESULT _regexp_gmatch(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
	SETUP_REX(v);
	SQ_GET_STRING(v, 2, subject);
	GET_EXTRA_MATCH_PARAMS(4);

    bool isFirst = true;
	const SQChar *begin,*end;
	while(sqstd_rex_searchrange(self,subject_str, subject_str + subject_str_size,&begin,&end)){
	    SQInteger n = sqstd_rex_getsubexpcount(self);
	    SQRexMatch match;
        if(isFirst)
        {
            sq_push(v, 3); //push the function
            isFirst = false;
        }
	    sq_pushroottable(v); //this
	    SQInteger i = 0;
	    for(;i < n; i++) {
            sqstd_rex_getsubexp(self,i,&match);
            if(i > 0){ //skip whole match
                if(match.len == -1) sq_pushinteger(v, match.begin - subject); //on empty capture push position
                else sq_pushstring(v, match.begin, match.len);
            }
		}
		i = sq_call(v, n, SQFalse, SQTrue);
		if(i < 0) return i;
		subject_str_size -= end-subject_str;
		//if(subject_str_size <= 0) break;
		subject_str = end;
	}
	sq_pushbool(v,SQFalse);
	return 1;
}

#include "sqstdblobimpl.h"
static SQRESULT _regexp_gsub(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
	SETUP_REX(v);
	SQ_GET_STRING(v, 2, subject);
    SQ_OPT_INTEGER(v, 4, max_sub, 0);
    if(max_sub < 0) return sq_throwerror(v, _SC("max substitutions can't be less than zero"));

    bool isFirst = true;
	const SQChar *begin,*end;
	SQBlob blob(0,8192);
	SQObjectType ptype = sq_gettype(v, 3);
    const SQChar *replacement;
    SQInteger replacement_size;
    if(ptype == OT_STRING)
    {
        sq_getstr_and_size(v, 3, &replacement, &replacement_size);
    }
	while(sqstd_rex_searchrange(self,subject, subject+subject_size,&begin,&end)){
	    blob.Write(subject, begin-subject);
	    SQInteger n = sqstd_rex_getsubexpcount(self);
	    SQRexMatch match;
        SQInteger i;
	    switch(ptype){
	        case OT_CLOSURE:{
                if(isFirst)
                {
                    sq_push(v, 3); //push the function
                    isFirst = false;
                }
                sq_pushroottable(v); //this
                for(i=0; i < n; i++) {
                    sqstd_rex_getsubexp(self,i,&match);
                    if(i > 0){ //skip whole match
                        if(match.len == 0) sq_pushinteger(v, match.begin - subject); //on empty capture push position
                        else sq_pushstring(v, match.begin, match.len);
                    }
                }
                i = sq_call(v, n, SQTrue, SQTrue);
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
                for(i=0; i < n; i++) {
                    sqstd_rex_getsubexp(self,i,&match);
                    if(i > 0){ //skip whole match
                        sq_pushinteger(v, i-1);
                        if(SQ_SUCCEEDED(sq_get(v, 3)) &&
                                SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &replacement, &replacement_size))){
                            blob.Write(replacement, replacement_size);
                            sq_pop(v, 1); //remove value
                        }
                    }
                }
	        }
	        break;
	        case OT_TABLE:{
                for(i=0; i < n; i++) {
                    sqstd_rex_getsubexp(self,i,&match);
                    if(i > 0){ //skip whole match
                        sq_pushstring(v, match.begin, match.len);
                        if(SQ_SUCCEEDED(sq_get(v, 3)) &&
                                SQ_SUCCEEDED(sq_getstr_and_size(v, -1, &replacement, &replacement_size))){
                            blob.Write(replacement, replacement_size);
                            sq_pop(v, 1); //remove value
                        }
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
                            SQInteger idx = replacement[i] - '0';
                            if(idx < n)
                            {
                                sqstd_rex_getsubexp(self,idx,&match);
                                blob.Write(match.begin, match.len);
                            }
                            else
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
		subject_size -= end-subject;
		subject = end;
        if(max_sub)
        {
            if(--max_sub == 0) break;
        }
	}
    if(subject_size > 0) blob.Write(subject, subject_size);
	sq_pushstring(v, (const SQChar *)blob.GetBuf(), blob.Len());
	return 1;
}

static void _addrexmatch(HSQUIRRELVM v,const SQChar *str,const SQChar *begin,const SQChar *end)
{
	sq_newtable(v);
	sq_pushstring(v,_SC("begin"),-1);
	sq_pushinteger(v,begin - str);
	sq_rawset(v,-3);
	sq_pushstring(v,_SC("end"),-1);
	sq_pushinteger(v,end - str);
	sq_rawset(v,-3);
}

static SQRESULT _regexp_search(HSQUIRRELVM v)
{
	SETUP_REX(v);
	const SQChar *str,*begin,*end;
	SQInteger start = 0;
	sq_getstring(v,2,&str);
	if(sq_gettop(v) > 2) sq_getinteger(v,3,&start);
	if(sqstd_rex_search(self,str+start,&begin,&end) == SQTrue) {
		_addrexmatch(v,str,begin,end);
		return 1;
	}
	return 0;
}

static SQRESULT _regexp_capture(HSQUIRRELVM v)
{
	SETUP_REX(v);
	const SQChar *str,*begin,*end;
	SQInteger start = 0;
	sq_getstring(v,2,&str);
	if(sq_gettop(v) > 2) sq_getinteger(v,3,&start);
	if(sqstd_rex_search(self,str+start,&begin,&end) == SQTrue) {
		SQInteger n = sqstd_rex_getsubexpcount(self);
		SQRexMatch match;
		sq_newarray(v,0);
		for(SQInteger i = 0;i < n; i++) {
			sqstd_rex_getsubexp(self,i,&match);
			if(match.len > 0)
				_addrexmatch(v,str,match.begin,match.begin+match.len);
			else
				_addrexmatch(v,str,str,str); //empty match
			sq_arrayappend(v,-2);
		}
		return 1;
	}
	return 0;
}

static SQRESULT _regexp_xcapture(HSQUIRRELVM v)
{
	SETUP_REX(v);
	const SQChar *str,*begin,*end;
	SQInteger start = 0;
	sq_getstring(v,2,&str);
	if(sq_gettop(v) > 2) sq_getinteger(v,3,&start);
	if(sqstd_rex_search(self,str+start,&begin,&end) == SQTrue) {
	    sq_pushbool(v, SQTrue);
		return 1;
	}
	return 0;
}

static SQRESULT _regexp_getxcapture(HSQUIRRELVM v)
{
	SETUP_REX(v);
	SQInteger n, start;
	const SQChar *str;
	sq_getstring(v,2,&str);
	sq_getinteger(v,3,&n);
	SQRexMatch match;
    sqstd_rex_getsubexp(self,n,&match);
    if(match.len > 0){
        start = match.begin-str;
        sq_pushinteger(v, start);
	    sq_arrayappend(v,-2);
        sq_pushinteger(v, start+match.len);
	    sq_arrayappend(v,-2);
	    sq_pushbool(v, SQTrue);
		return 1;
    }
	return 0;
}

static SQRESULT _regexp_subexpcount(HSQUIRRELVM v)
{
	SETUP_REX(v);
	sq_pushinteger(v,sqstd_rex_getsubexpcount(self));
	return 1;
}

static SQRESULT _regexp_constructor(HSQUIRRELVM v)
{
	const SQChar *error,*pattern;
	sq_getstring(v,2,&pattern);
	SQRex *rex = sqstd_rex_compile(pattern,&error);
	if(!rex) return sq_throwerror(v,error);
	sq_setinstanceup(v,1,rex);
	sq_setreleasehook(v,1,_rexobj_releasehook);
	return 0;
}

static SQRESULT _regexp__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC("regexp"),-1);
	return 1;
}

#define _DECL_REX_FUNC(name,nparams,pmask) {_SC(#name),_regexp_##name,nparams,pmask}
static SQRegFunction rexobj_funcs[]={
	_DECL_REX_FUNC(constructor,2,_SC(".s")),
	_DECL_REX_FUNC(search,-2,_SC("xsn")),
	_DECL_REX_FUNC(match,-2,_SC("xsnn")),
	_DECL_REX_FUNC(gmatch,-3,_SC("xscnn")),
	_DECL_REX_FUNC(gsub,-3,_SC("xs s|c|a|t n")),
	_DECL_REX_FUNC(capture,-2,_SC("xsn")),
	_DECL_REX_FUNC(xcapture,-2,_SC("xsn")),
	_DECL_REX_FUNC(getxcapture,4,_SC("xsna")),
	_DECL_REX_FUNC(subexpcount,1,_SC("x")),
	_DECL_REX_FUNC(_typeof,1,_SC("x")),
	{NULL,(SQFUNCTION)0,0,NULL}
};
#undef _DECL_REX_FUNC

#define _DECL_FUNC(name,nparams,pmask) {_SC(#name),_string_##name,nparams,pmask}
static SQRegFunction stringlib_funcs[]={
	_DECL_FUNC(printf,-2,_SC(".s")),
	_DECL_FUNC(format,-2,_SC(".s")),
    _DECL_FUNC(escape,2,_SC(".s")),
	{NULL,(SQFUNCTION)0,0,NULL}
};
#undef _DECL_FUNC


SQInteger sqstd_register_stringlib(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC("regexp"),-1);
	sq_newclass(v,SQFalse);
	SQInteger i = 0;
	while(rexobj_funcs[i].name != 0) {
		SQRegFunction &f = rexobj_funcs[i];
		sq_pushstring(v,f.name,-1);
		sq_newclosure(v,f.f,0);
		sq_setparamscheck(v,f.nparamscheck,f.typemask);
		sq_setnativeclosurename(v,-1,f.name);
		sq_newslot(v,-3,SQFalse);
		i++;
	}
	sq_newslot(v,-3,SQFalse);

	i = 0;
	while(stringlib_funcs[i].name!=0)
	{
		sq_pushstring(v,stringlib_funcs[i].name,-1);
		sq_newclosure(v,stringlib_funcs[i].f,0);
		sq_setparamscheck(v,stringlib_funcs[i].nparamscheck,stringlib_funcs[i].typemask);
		sq_setnativeclosurename(v,-1,stringlib_funcs[i].name);
		sq_newslot(v,-3,SQFalse);
		i++;
	}
	return 1;
}
