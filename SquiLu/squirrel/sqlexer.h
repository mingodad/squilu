/*	see copyright notice in squirrel.h */
#ifndef _SQLEXER_H_
#define _SQLEXER_H_

#ifdef SQUNICODE
typedef SQChar LexChar;
#else
typedef	unsigned char LexChar;
#endif

struct SQLexer
{
	SQLexer();
	virtual ~SQLexer();
	SQInteger Init(SQSharedState *ss,SQLEXREADFUNC rg,SQUserPointer up,
                CompilerErrorFunc efunc,void *ed, SQBool want_comments=SQFalse);
	SQInteger ResetReader(SQLEXREADFUNC rg, SQUserPointer up, SQInteger line);
	SQTable * GetKeywords();
	SQInteger Error(const SQChar *err, ...);
	SQInteger Lex();
	const SQChar *Tok2Str(SQInteger tok);
	const SQChar *GetTokenName(int tk_code);
private:
	SQInteger GetIDType(const SQChar *s,SQInteger len);
	SQInteger ReadString(SQInteger ndelim,bool verbatim);
	SQInteger ReadNumber();
	SQInteger LexBlockComment();
	SQInteger LexLineComment();
	SQInteger ReadID();
	SQInteger Next();
#ifdef SQUNICODE
#if WCHAR_SIZE == 2
    SQInteger AddUTF16(SQUnsignedInteger ch);
#endif
#else
    SQInteger AddUTF8(SQUnsignedInteger ch);
#endif
    SQInteger ProcessStringHexEscape(SQChar *dest, SQInteger maxdigits);
	SQInteger _curtoken;
	SQTable *_keywords;
	SQBool _reached_eof;
public:
	SQInteger _prevtoken;
	SQInteger _currentline;
	SQInteger _lasttokenline;
	SQInteger _lasttokencolumn;
	SQInteger _currentcolumn;
	const SQChar *_svalue;
	SQInteger _nvalue;
	SQFloat _fvalue;
	SQLEXREADFUNC _readf;
	SQUserPointer _up;
	LexChar _currdata;
	SQSharedState *_sharedstate;
	sqvector<SQChar> _longstr;
	CompilerErrorFunc _errfunc;
	void *_errtarget;
	SQChar _lasterror[256];
	SQBool _want_comments;
};

#endif
