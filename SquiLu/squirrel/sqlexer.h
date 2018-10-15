/*	see copyright notice in squirrel.h */
#ifndef _SQLEXER_H_
#define _SQLEXER_H_

#ifdef SQUNICODE
typedef SQChar LexChar;
#else
typedef	unsigned char LexChar;
#endif

struct SQLexerData
{
    SQInteger curtoken;
    SQInteger prevtoken;
    SQInteger currentline;
    SQInteger currentcolumn;
    SQInteger lasttokenline;
    SQInteger lasttokencolumn;
    const SQChar *svalue;
    sqvector<SQChar> longstr;
    SQInteger nvalue;
    SQFloat fvalue;
    LexChar currdata;
    SQInteger readcount;
    SQBool reached_eof;
    SQChar lasterror[256];
    SQLexerData()
    {
        clear();
    }
    SQLexerData(SQLexerData *src)
    {
        copy(src);
    }
    void copy(SQLexerData *src)
    {
        curtoken = src->curtoken;
        reached_eof = src->reached_eof;
        prevtoken = src->prevtoken;
        currentline = src->currentline;
        currentcolumn = src->currentcolumn;
        lasttokenline = src->lasttokenline;
        lasttokencolumn = src->lasttokencolumn;
        longstr.resize(src->longstr.size());
        memcpy(longstr._vals, src->longstr._vals, src->longstr.size());
        svalue = &longstr[0];
        nvalue = src->nvalue;
        fvalue = src->fvalue;
        currdata = src->currdata;
        readcount = src->readcount;
        scstrcpy(lasterror, src->lasterror);
    }
    void clear()
    {
        curtoken = 0;
        reached_eof = SQFalse;
        prevtoken = -1;
        currentline = 0;
        currentcolumn = 0;
        lasttokenline = 0;
        lasttokencolumn = 0;
        longstr.resize(0);
        svalue = NULL;
        nvalue = 0;
        fvalue = 0.0;
        currdata = 0;
        lasterror[0] = '\0';
        readcount = 0;
    }
};

struct SQLexer
{
	SQLexer();
	virtual ~SQLexer();
	SQInteger Init(SQSharedState *ss,SQLEXREADFUNC rg,SQUserPointer up,
                CompilerErrorFunc efunc,void *ed, SQBool want_comments=SQFalse);
	SQInteger ResetReader(SQLEXREADFUNC rg, SQUserPointer up, SQInteger line);
	virtual SQTable * GetKeywords();
	SQInteger Error(const SQChar *err, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
	SQInteger Lex();
	SQInteger LookaheadLex();
	const SQChar *Tok2Str(SQInteger tok);
	const SQChar *GetTokenName(SQInteger tk_code);
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
    SQLexerData _data, _data_lookahead;
	SQTable *_keywords;
public:
    SQLexerData *data;
	SQLEXREADFUNC _readf;
	SQUserPointer _up;
	SQSharedState *_sharedstate;
	CompilerErrorFunc _errfunc;
	void *_errtarget;
	SQBool _want_comments;
};

#endif
