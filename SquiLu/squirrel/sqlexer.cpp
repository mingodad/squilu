/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "sqtable.h"
#include "sqstring.h"
#include "sqcompiler.h"
#include "sqlexer.h"

#define CUR_CHAR (data->currdata)
#define RETURN_TOKEN(t) { data->prevtoken = data->curtoken; data->curtoken = t; return t;}
#define IS_EOB() (CUR_CHAR <= SQUIRREL_EOB)
//#define NEXT() {SQInteger rc = Next(); if(rc < 0) return rc; data->currentcolumn++;}
#define NEXT() {if(Next()) return -1;}
#define INIT_TEMP_STRING() { data->longstr.resize(0);}
#define APPEND_CHAR(c) { data->longstr.push_back(c);}
#define TERMINATE_BUFFER() {data->longstr.push_back(_SC('\0'));}
#define ADD_KEYWORD(key,id) tbl->NewSlot( SQString::Create(_sharedstate, _SC(#key)) ,SQInteger(id))

SQLexer::SQLexer(){_keywords=0;}
SQLexer::~SQLexer()
{
	if(_keywords) _keywords->Release();
}

SQInteger SQLexer::Init(SQSharedState *ss, SQLEXREADFUNC rg,
                        SQUserPointer up,CompilerErrorFunc efunc,void *ed,
                        SQBool want_comments, SQBool want_stringSingleAndDoubleQuotes)
{
    _want_comments = want_comments;
    _want_stringSingleAndDoubleQuotes = want_stringSingleAndDoubleQuotes;
    data = &_data;
    _data_lookahead.currentline = -1;
	_errfunc = efunc;
	_errtarget = ed;
	_sharedstate = ss;
	if(_keywords) _keywords->Release();
    _keywords = GetKeywords();
	return ResetReader(rg, up, 1);
}

SQInteger SQLexer::ResetReader(SQLEXREADFUNC rg, SQUserPointer up, SQInteger line)
{
	_readf = rg;
	_up = up;
	data->lasttokenline = data->currentline = line;
	data->lasttokencolumn = 0;
	data->currentcolumn = 0;
	data->prevtoken = -1;
	data->readcount = 0;
	data->reached_eof = SQFalse;
	data->isCharacter = SQFalse;
	return Next();
}

SQTable * SQLexer::GetKeywords()
{
	SQTable *tbl = SQTable::Create(_sharedstate, (TK_LAST_ENUM_TOKEN - TK_FIRST_ENUM_TOKEN - 1) /*26*/);
	ADD_KEYWORD(any_t, TK_LOCAL_ANY_T);
	ADD_KEYWORD(array_t, TK_LOCAL_ARRAY_T);
	ADD_KEYWORD(as, TK_AS);
	ADD_KEYWORD(auto, TK_LOCAL);
	ADD_KEYWORD(base, TK_BASE);
	ADD_KEYWORD(bool_t, TK_LOCAL_BOOL_T);
	ADD_KEYWORD(break, TK_BREAK);
	ADD_KEYWORD(case, TK_CASE);
	ADD_KEYWORD(catch, TK_CATCH);
	ADD_KEYWORD(char_t, TK_LOCAL_CHAR_T);
	ADD_KEYWORD(class,TK_CLASS);
	ADD_KEYWORD(clone, TK_CLONE);
	ADD_KEYWORD(constructor,TK_CONSTRUCTOR);
	ADD_KEYWORD(const,TK_CONST);
	ADD_KEYWORD(constexpr,TK_CONSTEXPR);
	ADD_KEYWORD(continue, TK_CONTINUE);
	ADD_KEYWORD(declare, TK_DECLARE);
	ADD_KEYWORD(default, TK_DEFAULT);
	ADD_KEYWORD(delete, TK_DELETE);
	ADD_KEYWORD(destructor,TK_DESTRUCTOR);
	ADD_KEYWORD(do, TK_DO);
	ADD_KEYWORD(double_t, TK_LOCAL_DOUBLE_T);
	ADD_KEYWORD(else, TK_ELSE);
	ADD_KEYWORD(enum,TK_ENUM);
	ADD_KEYWORD(extends,TK_EXTENDS);
	ADD_KEYWORD(extern,TK_EXTERN);
	ADD_KEYWORD(false,TK_FALSE);
	ADD_KEYWORD(__FILE__,TK___FILE__);
	ADD_KEYWORD(float_t, TK_LOCAL_FLOAT_T);
	ADD_KEYWORD(foreach, TK_FOREACH);
	ADD_KEYWORD(for, TK_FOR);
	ADD_KEYWORD(friend, TK_FRIEND);
	ADD_KEYWORD(function, TK_FUNCTION);
	ADD_KEYWORD(__FUNCTION__,TK___FUNCTION__);
	ADD_KEYWORD(goto, TK_GOTO);
	ADD_KEYWORD(if, TK_IF);
	ADD_KEYWORD(instanceof,TK_INSTANCEOF);
	ADD_KEYWORD(int16_t, TK_LOCAL_INT16_T);
	ADD_KEYWORD(int32_t, TK_LOCAL_INT32_T);
	ADD_KEYWORD(int64_t, TK_LOCAL_INT64_T);
	ADD_KEYWORD(int8_t, TK_LOCAL_INT8_T);
	ADD_KEYWORD(in, TK_IN);
	ADD_KEYWORD(inline, TK_INLINE);
	ADD_KEYWORD(interface, TK_INTERFACE);
	ADD_KEYWORD(int_t, TK_LOCAL_INT_T);
	ADD_KEYWORD(let, TK_LOCAL);
	ADD_KEYWORD(__LINE__,TK___LINE__);
	ADD_KEYWORD(local, TK_LOCAL);
	ADD_KEYWORD(long_double_t, TK_LOCAL_LONG_DOUBLE_T);
	ADD_KEYWORD(new,TK_IGNORE);
	ADD_KEYWORD(noexcept,TK_NOEXCEPT);
	ADD_KEYWORD(number_t, TK_LOCAL_NUMBER_T);
	ADD_KEYWORD(null, TK_NULL);
	ADD_KEYWORD(NULL, TK_NULL);
	ADD_KEYWORD(private,TK_PRIVATE);
	ADD_KEYWORD(protected,TK_PROTECTED);
	ADD_KEYWORD(public,TK_PUBLIC);
	ADD_KEYWORD(rawcall, TK_RAWCALL);
	ADD_KEYWORD(resume, TK_RESUME);
	ADD_KEYWORD(return, TK_RETURN);
	ADD_KEYWORD(size_t,TK_LOCAL_SIZE_T);
	ADD_KEYWORD(ssize_t,TK_LOCAL_SSIZE_T);
	ADD_KEYWORD(static,TK_STATIC);
	ADD_KEYWORD(string_t, TK_LOCAL_STRING_T);
	ADD_KEYWORD(struct,TK_STRUCT);
	ADD_KEYWORD(switch, TK_SWITCH);
	ADD_KEYWORD(table_t, TK_LOCAL_TABLE_T);
	ADD_KEYWORD(template, TK_TEMPLATE);
	ADD_KEYWORD(this, TK_THIS);
	ADD_KEYWORD(throw, TK_THROW);
	ADD_KEYWORD(typedef, TK_TYPEDEF);
	ADD_KEYWORD(true,TK_TRUE);
	ADD_KEYWORD(try, TK_TRY);
	ADD_KEYWORD(typeof, TK_TYPEOF);
	ADD_KEYWORD(uint16_t, TK_LOCAL_UINT16_T);
	ADD_KEYWORD(uint32_t, TK_LOCAL_UINT32_T);
	ADD_KEYWORD(uint64_t, TK_LOCAL_UINT64_T);
	ADD_KEYWORD(uint8_t, TK_LOCAL_UINT8_T);
	ADD_KEYWORD(uint_t, TK_LOCAL_UINT_T);
	ADD_KEYWORD(unsafe, TK_UNSAFE);
	ADD_KEYWORD(using, TK_USING);
	ADD_KEYWORD(var, TK_LOCAL);
	ADD_KEYWORD(virtual, TK_VIRTUAL);
	ADD_KEYWORD(void_ptr_t, TK_LOCAL_VOIDPTR_T);
	ADD_KEYWORD(void, TK_VOID);
	ADD_KEYWORD(volatile, TK_VOLATILE);
	ADD_KEYWORD(wchar_t, TK_LOCAL_WCHAR_T);
	ADD_KEYWORD(weakref_t, TK_LOCAL_WEAKREF_T);
	ADD_KEYWORD(while, TK_WHILE);
	ADD_KEYWORD(yield, TK_YIELD);

	return tbl;
}

SQInteger SQLexer::Error(const SQChar *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    scvsprintf(data->lasterror, sizeof(data->lasterror), fmt, vl);
    va_end(vl);
	if(_errfunc) _errfunc(_errtarget,data->lasterror);
	return -1;
}

SQInteger SQLexer::Next()
{
	SQInteger t = _readf(_up);
	if(t > MAX_CHAR) return Error(_SC("Invalid character"));
	if(t != 0) {
		data->currdata = (LexChar)t;
		++data->currentcolumn;
		++data->readcount;
		return 0;
	}
	data->currdata = SQUIRREL_EOB;
	data->reached_eof = SQTrue;
	return 0;
}

const SQChar *SQLexer::Tok2Str(SQInteger tok)
{
	SQObjectPtr itr, key, val;
	SQInteger nitr;
	while((nitr = _keywords->Next(false,itr, key, val)) != -1) {
		itr = (SQInteger)nitr;
		if(((SQInteger)_integer(val)) == tok)
			return _stringval(key);
	}
	return NULL;
}

const SQChar *SQLexer::GetTokenName(SQInteger tk_code) {
	const SQChar *str_tk;

    switch(tk_code){
#define ENUM_TK(a) case TK_##a: str_tk = _SC("TK_" #a); break;
        SQ_KEYWORDS_LIST()
#undef ENUM_TK
        default:
            str_tk = _SC("()");
    }
    return str_tk;
}

SQInteger SQLexer::LexBlockComment()
{
/*
    if(CUR_CHAR == _SC('*'))
    {
        NEXT();
        if(CUR_CHAR != _SC('*')){ //document comment
            printf("Doument comment found at line %d\n", data->currentline);
        }
    }
*/
	bool done = false;
	SQInteger nested = 0;
	if(_want_comments) INIT_TEMP_STRING();
	NEXT(); //remove the comment token '*'
	while(!done) {
		switch(CUR_CHAR) {
			case _SC('/'): {
				if(_want_comments) APPEND_CHAR(CUR_CHAR);
				NEXT();
				if(CUR_CHAR == _SC('*'))	++nested;
				else continue; //reevaluate, when it's a \n it'll be incremented line bellow
			}
			break;
			case _SC('*'): {
				NEXT();
				if(CUR_CHAR == _SC('/')) {
					if(nested) {
						--nested;
						if(_want_comments) APPEND_CHAR(_SC('*'));
						break;
					}
					else
					{
						done = true; NEXT(); continue;
					}
				}
				else if(_want_comments) APPEND_CHAR(_SC('*'));
				continue; //reevaluate, when it's a \n it'll be incremented line bellow
			}
			break;
			case _SC('\n'): data->currentline++; break;
			case SQUIRREL_EOB: return Error(_SC("missing \"*/\" in comment"));
		}
		if(_want_comments) APPEND_CHAR(CUR_CHAR);
		NEXT();
	}
    if(_want_comments)
    {
        TERMINATE_BUFFER();
        if(data->longstr.size() > 0) data->longstr.pop_back(); //remove the last '*'
        data->svalue = &data->longstr[0];
    }
	return 0;
}

SQInteger SQLexer::LexLineComment()
{
    if(_want_comments) INIT_TEMP_STRING();
    NEXT(); //remove the comment token
	while (CUR_CHAR != _SC('\n') && (!IS_EOB())) {if(_want_comments) APPEND_CHAR(CUR_CHAR); NEXT();}
    if(_want_comments)
    {
        TERMINATE_BUFFER();
        data->svalue = &data->longstr[0];
    }
	return 0;
}

SQInteger SQLexer::LookaheadLex()
{
    if(CUR_CHAR == SQUIRREL_EOB) return 0;
    if(_data_lookahead.currentline >= 0)
    {
        return Error(_SC("lex lookahead already done"));
    }
    _data_lookahead.copy(&_data);
    data = &_data_lookahead;
    Lex();
    data = &_data;
    return _data_lookahead.curtoken;
}

//dontThrowIntegerOverflow for when in 32bits parsing 64bits integer inside excluded ifdef
SQInteger SQLexer::Lex(bool dontThrowIntegerOverflow)
{
    if(_data_lookahead.currentline >= 0 && data != &_data_lookahead)
    {
        //we did a lookahead before, reuse it now
        _data.copy(&_data_lookahead);
        _data_lookahead.currentline = -1;
        return _data.curtoken;
    }
	data->lasttokenline = data->currentline;
	data->lasttokencolumn = data->currentcolumn;
	data->svalue = NULL;
	while(CUR_CHAR != SQUIRREL_EOB) {
		switch(CUR_CHAR){
		case _SC('\t'): case _SC('\r'): case _SC(' '): NEXT(); continue;
		case _SC('\n'):
			data->currentline++;
			data->prevtoken=data->curtoken;
			data->curtoken=_SC('\n');
			NEXT();
			data->currentcolumn=1;
			continue;
		case _SC('#'):
		    NEXT();
		    if(CUR_CHAR == _SC('!')) //shell shebang
            {
                if(LexLineComment()) return -1;
                if(_want_comments) RETURN_TOKEN(TK_COMMENT_LINE)
                continue;
            }
            RETURN_TOKEN(TK_PRAGMA);
		case _SC('/'):
			NEXT();
			switch(CUR_CHAR){
			case _SC('*'):
				if(LexBlockComment()) return -1;
                if(_want_comments) RETURN_TOKEN(TK_COMMENT_BLOCK)
				continue;
			case _SC('/'):
				if(LexLineComment()) return -1;
                if(_want_comments) RETURN_TOKEN(TK_COMMENT_LINE)
				continue;
			case _SC('='):
				NEXT();
				RETURN_TOKEN(TK_DIVEQ);
			case _SC('>'):
				NEXT();
				RETURN_TOKEN(TK_ATTR_CLOSE);
			default:
				RETURN_TOKEN('/');
			}
		case _SC('='):
			NEXT();
			if (CUR_CHAR != _SC('=')){ RETURN_TOKEN('=') }
			else {
                NEXT();
                if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_EQ_IDENTITY) }
                else { RETURN_TOKEN(TK_EQ); }
            }
		case _SC('<'):
			NEXT();
			switch(CUR_CHAR) {
			case _SC('='):
				NEXT();
				if(CUR_CHAR == _SC('>')) {
					NEXT();
					RETURN_TOKEN(TK_3WAYSCMP);
				}
				RETURN_TOKEN(TK_LE)
			case _SC('-'): NEXT(); RETURN_TOKEN(TK_NEWSLOT); break;
			case _SC('<'):
			    NEXT();
			    if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_BIT_SHIFT_LEFT_EQ);}
			    RETURN_TOKEN(TK_SHIFTL); break;
			case _SC('/'): NEXT(); RETURN_TOKEN(TK_ATTR_OPEN); break;
			}
			RETURN_TOKEN('<');
		case _SC('>'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_GE);}
			else if(CUR_CHAR == _SC('>')){
				NEXT();
				if(CUR_CHAR == _SC('>')){
					NEXT();
					RETURN_TOKEN(TK_USHIFTR);
				} else if (CUR_CHAR == _SC('=')){
				    NEXT();
				    RETURN_TOKEN(TK_BIT_SHIFT_RIGHT_EQ);
                }
				RETURN_TOKEN(TK_SHIFTR);
			}
			else { RETURN_TOKEN('>') }
		case _SC('!'):
			NEXT();
			if (CUR_CHAR != _SC('=')){ RETURN_TOKEN(_SC('!'))}
			else {
                NEXT();
                if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_NE_IDENTITY)}
                else { RETURN_TOKEN(TK_NE); }
            }
		case _SC('@'): {
			SQInteger stype;
			NEXT();
			if(CUR_CHAR != _SC('"')) {
				RETURN_TOKEN(_SC('@'));
			}
			if((stype=ReadString(_SC('"'),true))!=-1) {
				RETURN_TOKEN(stype);
			}
			return Error(_SC("error parsing the string"));
					   }
		case _SC('"'):
		case _SC('\''): {
			SQInteger stype;
			if((stype=ReadString(CUR_CHAR,false))!=-1){
				RETURN_TOKEN(stype);
			}
			return Error(_SC("error parsing the string"));
			}
		case _SC('{'): case _SC('}'): case _SC('('): case _SC(')'): case _SC('['): case _SC(']'):
		case _SC(';'): case _SC(','): case _SC('?'): case _SC('~'):
			{
			    SQInteger ret = CUR_CHAR;
                NEXT();
                if((ret == _SC('[') || ret == _SC('{') || ret == _SC('(')) && CUR_CHAR == _SC('=')){
                    //lets try lua literal delimiters
                    SQInteger stype;
                    if((stype=ReadString(ret,true))!=-1){
                        RETURN_TOKEN(stype);
                    }
                    return Error(_SC("error parsing the string"));
                }
                else RETURN_TOKEN(ret);
            }
		case _SC('.'):
			NEXT();
			if (CUR_CHAR != _SC('.')){
			    if (scisdigit(CUR_CHAR)) {
				SQInteger ret = ReadNumber(_SC('.'), dontThrowIntegerOverflow);
				if(ret < 0) return -1;
				RETURN_TOKEN(ret);
			    }
			    RETURN_TOKEN(_SC('.'));
			}
			NEXT();
			if (CUR_CHAR != _SC('.')){ return Error(_SC("invalid token '..'")); }
			NEXT();
			RETURN_TOKEN(TK_VARPARAMS);
		case _SC('^'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_BIT_XOR_EQ);}
			RETURN_TOKEN(_SC('^'));
		case _SC('&'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_BIT_AND_EQ);}
			if (CUR_CHAR != _SC('&')){ RETURN_TOKEN(_SC('&')) }
			else { NEXT(); RETURN_TOKEN(TK_AND); }
		case _SC('|'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_BIT_OR_EQ);}
			if (CUR_CHAR != _SC('|')){ RETURN_TOKEN(_SC('|')) }
			else { NEXT(); RETURN_TOKEN(TK_OR); }
		case _SC(':'):
			NEXT();
			if (CUR_CHAR != _SC(':')){ RETURN_TOKEN(_SC(':')) }
			else { NEXT(); RETURN_TOKEN(TK_DOUBLE_COLON); }
		case _SC('*'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_MULEQ);}
			else RETURN_TOKEN(_SC('*'));
		case _SC('%'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_MODEQ);}
			else RETURN_TOKEN(_SC('%'));
		case _SC('-'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_MINUSEQ);}
			else if  (CUR_CHAR == _SC('-')){ NEXT(); RETURN_TOKEN(TK_MINUSMINUS);}
			else if  (CUR_CHAR == _SC('>')){ NEXT(); RETURN_TOKEN(_SC('.'));} //accept C/C++ like pointers
			else RETURN_TOKEN(_SC('-'));
		case _SC('+'):
			NEXT();
			if (CUR_CHAR == _SC('=')){ NEXT(); RETURN_TOKEN(TK_PLUSEQ);}
			else if (CUR_CHAR == _SC('+')){ NEXT(); RETURN_TOKEN(TK_PLUSPLUS);}
			else RETURN_TOKEN(_SC('+'));
		case SQUIRREL_EOB:
			return 0;
		default:{
				if (scisdigit(CUR_CHAR)) {
					SQInteger ret = ReadNumber(0, dontThrowIntegerOverflow);
					if(ret < 0) return -1;
					RETURN_TOKEN(ret);
				}
				else if (scisalpha(CUR_CHAR) || CUR_CHAR == _SC('_')) {
					SQInteger t = ReadID();
					if(t < 0) return -1;
					RETURN_TOKEN(t);
				}
				else {
					SQInteger c = CUR_CHAR;
					if (sciscntrl((int)c)) return Error(_SC("unexpected character(control)"));
					NEXT();
					RETURN_TOKEN(c);
				}
				RETURN_TOKEN(0);
			}
		}
	}
	return 0;
}

SQInteger SQLexer::GetIDType(const SQChar *s,SQInteger len)
{
	SQObjectPtr t;
	if(_keywords->GetStr(s,len, t)) {
		return SQInteger(_integer(t));
	}
	return TK_IDENTIFIER;
}

#ifdef SQUNICODE
#if WCHAR_SIZE == 2
SQInteger SQLexer::AddUTF16(SQUnsignedInteger ch)
{
    if (ch >= 0x10000)
    {
        SQUnsignedInteger code = (ch - 0x10000);
        APPEND_CHAR((SQChar)(0xD800 | (code >> 10)));
        APPEND_CHAR((SQChar)(0xDC00 | (code & 0x3FF)));
        return 2;
    }
    else {
        APPEND_CHAR((SQChar)ch);
        return 1;
    }
}
#endif
#else
SQInteger SQLexer::AddUTF8(SQUnsignedInteger ch)
{
    if (ch < 0x80) {
        APPEND_CHAR((char)ch);
        return 1;
    }
    if (ch < 0x800) {
        APPEND_CHAR((SQChar)((ch >> 6) | 0xC0));
        APPEND_CHAR((SQChar)((ch & 0x3F) | 0x80));
        return 2;
    }
    if (ch < 0x10000) {
        APPEND_CHAR((SQChar)((ch >> 12) | 0xE0));
        APPEND_CHAR((SQChar)(((ch >> 6) & 0x3F) | 0x80));
        APPEND_CHAR((SQChar)((ch & 0x3F) | 0x80));
        return 3;
    }
    if (ch < 0x110000) {
        APPEND_CHAR((SQChar)((ch >> 18) | 0xF0));
        APPEND_CHAR((SQChar)(((ch >> 12) & 0x3F) | 0x80));
        APPEND_CHAR((SQChar)(((ch >> 6) & 0x3F) | 0x80));
        APPEND_CHAR((SQChar)((ch & 0x3F) | 0x80));
        return 4;
    }
    return 0;
}
#endif

SQInteger SQLexer::ProcessStringHexEscape(SQChar *dest, SQInteger maxdigits)
{
    NEXT();
    if (!isxdigit(CUR_CHAR)) return Error(_SC("hexadecimal number expected"));
    SQInteger n = 0;
    while (isxdigit(CUR_CHAR) && n < maxdigits) {
        dest[n] = CUR_CHAR;
        n++;
        NEXT();
    }
    dest[n] = 0;
    return n;
}

SQInteger scisodigit(SQInteger c) { return c >= _SC('0') && c <= _SC('7'); }

SQInteger SQLexer::ReadString(SQInteger ndelim,bool verbatim)
{
	INIT_TEMP_STRING();
	SQInteger start_equals = 0;
	SQChar cpp_delimin[32], cdelim1, cdelim2, saved_ndelim = ndelim;
	if(ndelim == _SC('{')){
	    cdelim1 = _SC('{');
	    cdelim2 = _SC('}');
	}
	else if(ndelim == _SC('(')){
	    cdelim1 = _SC('(');
	    cdelim2 = _SC(')');
	}
	else if(ndelim == _SC('[')){
	    cdelim1 = _SC('[');
	    cdelim2 = _SC(']');
	}
	else if(ndelim == _SC('R')){
	    cdelim1 = _SC('(');
	    cdelim2 = _SC(')');
	    ndelim = cdelim2;
	    if(CUR_CHAR != _SC('"'))
        {
            return Error(_SC("expect '\"' on literal delimiter"));
        }
        NEXT();
        size_t i=0;
        for(;(i < sizeof(cpp_delimin)-1) && (CUR_CHAR != _SC('(')); ++i)
        {
            cpp_delimin[i] = CUR_CHAR;
            NEXT();
        }
        cpp_delimin[i] = _SC('\0');
	}
	else
    {
        cdelim1 = cdelim2 = _SC('\0');
    }
	if((cdelim1 == saved_ndelim) && (CUR_CHAR == _SC('='))){
	    //lua like literal
	    while(!IS_EOB() && CUR_CHAR == _SC('=')) {
	        ++start_equals;
	        NEXT();
	    }
	    if(CUR_CHAR != cdelim1){
	        //it's not a lua literal delimiter
	        return Error(_SC("expect '%c' on literal delimiter"), cdelim1);
	    }
	    ndelim = cdelim2;
	}
	NEXT();
	if(IS_EOB()) return -1;
	if(start_equals) {
	    int cr_nl = CUR_CHAR == _SC('\r');
        if(cr_nl) NEXT();
        cr_nl = CUR_CHAR == _SC('\n');
        if(cr_nl) NEXT();
        if(cr_nl) {//if a new line follows the start of delimiter drop it
            ++data->currentline;
            if(IS_EOB())
            {
                return Error(_SC("unfinished string"));
            }
        }
	}
	for(;;) {
try_again:
		while(CUR_CHAR != ndelim) {
            SQInteger x = CUR_CHAR;
			switch(x) {
			case SQUIRREL_EOB:
				return Error(_SC("unfinished string"));
			case _SC('\n'):
				if(!verbatim) return Error(_SC("newline in a constant"));
				APPEND_CHAR(CUR_CHAR); NEXT();
				data->currentline++;
				break;
			case _SC('\\'):
				if(verbatim) {
					APPEND_CHAR(_SC('\\')); NEXT();
				}
				else {
					NEXT();
					switch(CUR_CHAR) {
                    case _SC('x'):  {
                        const SQInteger maxdigits = sizeof(SQChar) * 2;
                        SQChar temp[maxdigits + 1];
                        if(ProcessStringHexEscape(temp, maxdigits) < 0) return -1;
                        SQChar *stemp;
                        APPEND_CHAR((SQChar)scstrtoul(temp, &stemp, 16));
                    }
                    break;
                    case _SC('U'):
                    case _SC('u'):  {
                        const SQInteger maxdigits = CUR_CHAR == _SC('u') ? 4 : 8;
                        SQChar temp[8 + 1];
                        if(ProcessStringHexEscape(temp, maxdigits) < 0) return -1;
                        SQChar *stemp;
#ifdef SQUNICODE
#if WCHAR_SIZE == 2
                        AddUTF16(scstrtoul(temp, &stemp, 16));
#else
                        ADD_CHAR((SQChar)scstrtoul(temp, &stemp, 16));
#endif
#else
                        AddUTF8(scstrtoul(temp, &stemp, 16));
#endif
					}
				    break;
				    //end of string continuation
					case _SC('\n'):
					    if(ndelim == _SC('"')){
                            data->currentline++;
                            NEXT();
                            break;
                        }
                        //falthrough
					case _SC('t'): APPEND_CHAR(_SC('\t')); NEXT(); break;
					case _SC('a'): APPEND_CHAR(_SC('\a')); NEXT(); break;
					case _SC('b'): APPEND_CHAR(_SC('\b')); NEXT(); break;
					case _SC('n'): APPEND_CHAR(_SC('\n')); NEXT(); break;
					case _SC('r'): APPEND_CHAR(_SC('\r')); NEXT(); break;
					case _SC('v'): APPEND_CHAR(_SC('\v')); NEXT(); break;
					case _SC('f'): APPEND_CHAR(_SC('\f')); NEXT(); break;
					case _SC('\\'): APPEND_CHAR(_SC('\\')); NEXT(); break;
					case _SC('"'): APPEND_CHAR(_SC('"')); NEXT(); break;
					case _SC('\''): APPEND_CHAR(_SC('\'')); NEXT(); break;
					case _SC('0'):
					case _SC('1'):
					case _SC('2'):
					case _SC('3'):
					case _SC('4'):
					case _SC('5'):
					case _SC('6'):
					case _SC('7'):
					    {
						int ndigits = 0;
						int octal_char = 0;
						while(scisodigit(CUR_CHAR)) {
							octal_char = (octal_char)*8+(CUR_CHAR-_SC('0'));
							NEXT();
							if(++ndigits == 3) break;
						}
						if(octal_char > 0xff) return Error(_SC("max value of embedded octal digits is \377"));
						APPEND_CHAR(octal_char);
						goto try_again;
					    }
					    break;
					case _SC('/'): APPEND_CHAR(CUR_CHAR); NEXT();
					    break;
					default:
						return Error(_SC("unrecognised escaper char"));
					}
				}
				break;
			default:
				APPEND_CHAR(CUR_CHAR);
				NEXT();
			}
		}
		NEXT();
		if(start_equals){
		    bool lastBraceAdded = false;
		    if(CUR_CHAR == _SC('=')){
                SQInteger end_equals = start_equals;
                NEXT();
                if(CUR_CHAR == _SC('=') || CUR_CHAR == cdelim2){
                    --end_equals;
                    while(!IS_EOB() && CUR_CHAR == _SC('=')) {
                        --end_equals;
                        NEXT();
                    }
                    if(end_equals) return Error(_SC("expect same number of '=' on literal delimiter"));
                    if(CUR_CHAR != cdelim2) return Error(_SC("expect '%c' to close literal delimiter"), cdelim2);
                    NEXT();
                    break;
                }
                APPEND_CHAR(cdelim2); //the first NEXT() after break the while loop
                APPEND_CHAR(_SC('='));
                lastBraceAdded = true;
		    }
		    if(!lastBraceAdded) APPEND_CHAR(cdelim2); //the first NEXT() after break the while loop
		    APPEND_CHAR(CUR_CHAR);
		    NEXT();
		}
		else if(saved_ndelim == _SC('R')) {
            if(CUR_CHAR == ndelim)
            {
                APPEND_CHAR(ndelim);
                goto try_again;
            }
            size_t i = 0;
            for(;(i < sizeof(cpp_delimin)-1) && (CUR_CHAR != _SC('"')) && cpp_delimin[i]; ++i)
            {
                if(CUR_CHAR != cpp_delimin[i])
                {
                    //false positive append all chars till here and continue
                    APPEND_CHAR(ndelim);
                    for(size_t j=0; j < i; ++j) APPEND_CHAR(cpp_delimin[j]); //recover already eaten chars from buffer
                    APPEND_CHAR(CUR_CHAR); //append the last one that mismatch
                    if(CUR_CHAR == _SC('\n')) data->currentline++;
                    NEXT();
                    goto try_again;
                    //return Error(_SC("expect \"%s\" to close literal delimiter"), cpp_delimin);
                }
                NEXT();
            }
            if(CUR_CHAR != _SC('"')) return Error(_SC("expect '\"' to close literal delimiter"));
            NEXT(); //eat last '"'
            break;
		}
		else if(verbatim && CUR_CHAR == _SC('"')) { //double quotation
			APPEND_CHAR(CUR_CHAR);
			NEXT();
		}
		else {
			break;
		}
	}
	TERMINATE_BUFFER();
	SQInteger len = data->longstr.size()-1;
	if(ndelim == _SC('\'') && !_want_stringSingleAndDoubleQuotes) {
		if(len == 0) return Error(_SC("empty constant"));
		if(len > 1) return Error(_SC("constant too long"));
		data->nvalue = data->longstr[0];
		data->isCharacter = SQTrue;
		return TK_INTEGER;
	}
	data->svalue = &data->longstr[0];
	return TK_STRING_LITERAL;
}

#define MAXBY10		(SQUnsignedInteger)(SQ_INT_MAX / 10)
#define MAXLASTD	(SQUnsignedInteger)(SQ_INT_MAX % 10)

static int isneg (const SQChar **s) {
  if (**s == _SC('-')) { (*s)++; return 1; }
  else if (**s == _SC('+')) (*s)++;
  return 0;
}

#define ADD_CHECK_DIGIT(dig, base) \
	    if (a >= MAXBY10 && (a > MAXBY10 || d > ((int)MAXLASTD + neg)))  /* overflow? */ \
            return false;  /* do not accept it (as integer) */ \
		a = a*base+dig;


static bool LexHexadecimal(const SQChar *s,SQUnsignedInteger *res)
{
	SQUnsignedInteger a = 0;
	int d = 0, neg = isneg(&s);
	while(*s != 0)
	{
		if(scisdigit(*s)) d = (*s++)-_SC('0');
		else if(scisxdigit(*s)) d = toupper(*s++)-_SC('A')+10;
		else { assert(0); }
		ADD_CHECK_DIGIT(d, 16);
	}
	*res = a;
	return true;
}

static bool LexInteger(const SQChar *s,SQUnsignedInteger *res)
{
	SQUnsignedInteger a = 0;
	int d = 0, neg = isneg(&s);
	while(*s != 0)
	{
	    d = (*s++)-_SC('0');
		ADD_CHECK_DIGIT(d, 10);
	}
	*res = neg ? (((SQUnsignedInteger)0)-a) : a;
	return true;
}

static bool LexOctal(const SQChar *s,SQUnsignedInteger *res)
{
	SQUnsignedInteger a = 0;
	int d = 0, neg = isneg(&s);
	while(*s != 0)
	{
		if(scisodigit(*s)) d = (*s++)-_SC('0');
		else { assert(0); }
		ADD_CHECK_DIGIT(d, 8);
	}
	*res = neg ? (((SQUnsignedInteger)0)-a) : a;
	return true;
}

static SQInteger isexponent(SQInteger c) { return c == _SC('e') || c==_SC('E'); }


#define MAX_HEX_DIGITS (sizeof(SQInteger)*2)
//dontThrowIntegerOverflow for when in 32bits parsing 64bits integer inside excluded ifdef
SQInteger SQLexer::ReadNumber(SQInteger startChar, bool dontThrowIntegerOverflow)
{
#define TINT 1
#define TFLOAT 2
#define THEX 3
#define TSCIENTIFIC 4
#define TOCTAL 5
	SQInteger rtype, type = TINT, firstchar = startChar ? startChar : CUR_CHAR;
	SQUnsignedInteger itmp=0;
	SQChar *sTemp;
	INIT_TEMP_STRING();
	if(!startChar) NEXT();
	if(firstchar == _SC('0') && (toupper(CUR_CHAR) == _SC('X') || scisodigit(CUR_CHAR)) ) {
		if(scisodigit(CUR_CHAR)) {
			type = TOCTAL;
			while(scisodigit(CUR_CHAR)) {
				APPEND_CHAR(CUR_CHAR);
				NEXT();
			}
			if(scisdigit(CUR_CHAR)) return Error(_SC("invalid octal number"));
		}
		else {
			NEXT();
			type = THEX;
			while(isxdigit(CUR_CHAR)) {
				APPEND_CHAR(CUR_CHAR);
				NEXT();
			}
			if((data->longstr.size() > MAX_HEX_DIGITS)  && !dontThrowIntegerOverflow)
                return Error(_SC("too many digits for an Hex number"));
		}
	}
	else {
		APPEND_CHAR((int)firstchar);
		while (CUR_CHAR == _SC('.') || scisdigit(CUR_CHAR) || isexponent(CUR_CHAR)) {
            if(CUR_CHAR == _SC('.') || isexponent(CUR_CHAR)) type = TFLOAT;
			if(isexponent(CUR_CHAR)) {
				if(type != TFLOAT) return Error(_SC("invalid numeric format"));
				type = TSCIENTIFIC;
				APPEND_CHAR(CUR_CHAR);
				NEXT();
				if(CUR_CHAR == _SC('+') || CUR_CHAR == _SC('-')){
					APPEND_CHAR(CUR_CHAR);
					NEXT();
				}
				if(!scisdigit(CUR_CHAR)) return Error(_SC("exponent expected"));
			}

			APPEND_CHAR(CUR_CHAR);
			NEXT();
		}
	}
	TERMINATE_BUFFER();
	bool okNumber = true;
	switch(type) {
	case TINT:
		okNumber = LexInteger(&data->longstr[0],&itmp);
		if(okNumber) break;
		//fallthrough
	case TSCIENTIFIC:
	case TFLOAT:
		data->fvalue = (SQFloat)scstrtod(&data->longstr[0],&sTemp);
		if(CUR_CHAR == _SC('f')) NEXT(); //0.0f C/C++ notation
		return TK_FLOAT;
	case THEX:
		okNumber = LexHexadecimal(&data->longstr[0],&itmp);
		break;
	case TOCTAL:
		okNumber = LexOctal(&data->longstr[0],&itmp);
		break;
	}

	if(!okNumber && !dontThrowIntegerOverflow) Error(_SC("integer overflow %s"), &data->longstr[0]);

	rtype = TK_INTEGER;
	data->isCharacter = SQFalse;
	switch(type) {
	case TINT:
	    switch(CUR_CHAR)
	    {
        case _SC('u'):
        case _SC('U'):
            rtype = TK_UNSIGNED_INTEGER;
            NEXT();
            break;
	    }

	    switch(CUR_CHAR)
	    {
        case _SC('s'):
        case _SC('S'):
            rtype = (rtype == TK_UNSIGNED_INTEGER) ? TK_UNSIGNED_SHORT_INTEGER : TK_SHORT_INTEGER;
            NEXT();
            break;

        case _SC('l'):
        case _SC('L'):
            rtype = (rtype == TK_UNSIGNED_INTEGER) ? TK_UNSIGNED_LONG_INTEGER : TK_LONG_INTEGER;
            NEXT();
            if((CUR_CHAR == _SC('l')) || (CUR_CHAR == _SC('L')) )
            {
                switch(rtype)
                {
                case TK_UNSIGNED_INTEGER:
                case TK_UNSIGNED_LONG_INTEGER:
                    rtype = TK_UNSIGNED_LONG_LONG_INTEGER;
                    break;
                default:
                    rtype = TK_LONG_LONG_INTEGER;
                }
                NEXT();
            }
            break;
	    }
	case THEX:
	case TOCTAL:
	    //to allow 64 bits integers comment bellow
	    //if(itmp > INT_MAX) return Error(_SC("integer overflow %ulld %d"));
	    data->nvalue = (SQInteger) itmp;
	    return rtype;
	}
	return 0;
}

SQInteger SQLexer::ReadID()
{
	SQInteger res;
	INIT_TEMP_STRING();
	do {
		APPEND_CHAR(CUR_CHAR);
		NEXT();
	} while(scisalnum(CUR_CHAR) || CUR_CHAR == _SC('_'));
	TERMINATE_BUFFER();
	if(data->longstr.size() == 2)
	{
		if(CUR_CHAR == _SC('"'))
		{
		    if(data->longstr[0] == _SC('R'))
		    {
			//C++ multiline string
			return ReadString(_SC('R'),true);
		    }
		    if(data->longstr[0] == _SC('L'))
		    {
			//C++ wchar string
			return ReadString(CUR_CHAR,false);
		    }
		}
		if(CUR_CHAR == _SC('\''))
		{
		    if(data->longstr[0] == _SC('L'))
		    {
			//C++ wchar string
			return ReadString(CUR_CHAR,false);
		    }
		}
	}
	res = GetIDType(&data->longstr[0],data->longstr.size() - 1);
	if(res == TK_IDENTIFIER || res == TK_CONSTRUCTOR || res == TK_DESTRUCTOR) {
		data->svalue = &data->longstr[0];
	}
	return res;
}
