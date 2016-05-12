//__max_print_stack_str_size <- 256;

#include "sq_lexer_tokens.nut"
#include "sqtypedefs.nut"

const SQFalse = 0;
const SQTrue = 1;
const SQUIRREL_EOB = -1;
const EOF = -1;
const MAX_CHAR = 0xFF;

const MAX_HEX_DIGITS = 12;
const TINT =1;
const TFLOAT = 2;
const THEX = 3;
const TSCIENTIFIC = 4;
const TOCTAL = 5;

SQInteger isexponent(SQInteger c)
{
    return c == 'e' || c=='E';
}
SQInteger scisdigit(SQInteger c)
{
    return c >= '0' && c <= '9';
}
SQInteger scdigitvalue(SQInteger c)
{
    if(c >= '0' && c <= '9') return c - '0';
    assert(0);
    return 0;
}
SQInteger scisodigit(SQInteger c)
{
    return c >= '0' && c <= '7';
}
SQInteger scodigitvalue(SQInteger c)
{
    if(c >= '0' && c <= '7') return c - '0';
    assert(0);
    return 0;
}
SQInteger scisxdigit(SQInteger c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
SQInteger scxdigitvalue(SQInteger c)
{
    if(c >= '0' && c <= '9') return (c - '0');
    if(c >= 'a' && c <= 'f') return (c - 'a') + 10;
    if(c >= 'A' && c <= 'F') return (c - 'A') + 10;
    assert(0);
    return 0;
}

SQInteger scisalpha(SQInteger c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

SQInteger scisalnum(SQInteger c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

SQInteger sctoupper(SQInteger c)
{
	if((c >= 'a' && c <= 'z')) return c - ('a'-'A');
	return c;
}

SQUnsignedInteger LexHexadecimal(const SQChar_ptr_t s, SQInteger sz)
{
    SQUnsignedInteger res = 0;
    //for(char c : s)
    foreach(c in s)
    {
        res = res * 16 + scxdigitvalue(c);
    }
    return res;
}

SQUnsignedInteger LexInteger(const SQChar_ptr_t s, SQInteger sz)
{
    SQUnsignedInteger res = 0;
    //for(char c : s)
    foreach(c in s)
    {
        res = res * 10 + scdigitvalue(c);
    }
    return res;
}

SQUnsignedInteger LexOctal(const SQChar_ptr_t s, SQInteger sz)
{
    SQUnsignedInteger res = 0;
    //for(char c : s)
    foreach(c in s)
    {
        res = res * 8 + scodigitvalue(c);
    }
    return res;
}


class SQSharedState
{
};

struct FILE
{
    string_t data;
    int_t cursor, size;
    FILE(string_t dt)
    {
        data = dt;
        cursor = 0;
	size = data.size();
    }
};
typedef class_ptr_t FILE_ptr_t;

SQInteger fgetc(FILE_ptr_t fp)
{
    if(fp->cursor < fp->size)
    {
        return fp->data[fp->cursor++];
    }
    return EOF;
}

static SQInteger compilerReadFunc(SQUserPointer fp)
{
    SQInteger c = fgetc(/*(FILE *)*/fp);
    if(c == EOF)
    {
        return 0;
    }
    return c;
}

struct SQString
{
    static SQString_ptr_t Create(SQSharedState_ptr_t ss, string_t s)
    {
        return s;
    }
};

struct SQTable
{
    table_t _tbl;

    SQTable()
    {
        _tbl = {};
    }

    static SQTable_ptr_t Create(SQSharedState_ptr_t ss, SQInteger n)
    {
        return SQTable();
    }
    bool NewSlot(const SQObjectPtr &key,const SQObjectPtr &val)
    {
        table_rawset(_tbl, key, val);
        return true;
    }
    inline bool GetStr(const SQChar_ptr_t key,SQInteger keylen,SQObjectPtr &val)
    {
	val = table_rawget(_tbl, key, NULL);
	return val;
    }
    
    SQInteger Next(bool getweakrefs,const SQObjectPtr &refpos, SQObjectPtr &outkey, SQObjectPtr &outval)
    {
	    return -1;
    }
};

struct SQLexerNut
{
    SQInteger CUR_CHAR;

    SQLexerNut()
    {
        _keywords=0;
        _longstr = blob(0, 8192);
    }
    virtual ~SQLexerNut()
    {
        _keywords->Release();
    }

    void INIT_TEMP_STRING()
    {
        _longstr.clear();
    }
    
    void  TERMINATE_BUFFER()
    {
	   // _longstr.push_back('\0'));
    }

    void APPEND_CHAR(SQInteger c)
    {
        _longstr.writen(c, 'c');
    }

    SQInteger NEXT()
    {
        SQInteger rc = Next();if(rc < 0) return rc;_currentcolumn++;
    }
    
    SQInteger RETURN_TOKEN(SQInteger t)
    {
	     _prevtoken = _curtoken; _curtoken = t; return t;
    }
    
    bool IS_EOB(){return CUR_CHAR <= SQUIRREL_EOB;}

    SQInteger Init(SQSharedState_ptr_t ss,SQLEXREADFUNC rg,SQUserPointer up,
                   CompilerErrorFunc efunc,void_ptr_t ed, SQBool want_comments=SQFalse)
    {
        _want_comments = want_comments;
        //_lasterror[0] = '\0';
        _svalue = NULL;
        _errfunc = efunc;
        _errtarget = ed;
        _sharedstate = ss;
        if(_keywords) _keywords->Release();
        _keywords = GetKeywords();
        return ResetReader(rg, up, 1);
    }

    SQInteger ResetReader(SQLEXREADFUNC rg, SQUserPointer up, SQInteger line)
    {
        _readf = rg;
        _up = up;
        _lasttokenline = _currentline = line;
        _lasttokencolumn = 0;
        _currentcolumn = 0;
        _prevtoken = -1;
        _reached_eof = SQFalse;
        return Next();
    }

    void ADD_KEYWORD(SQTable_ptr_t tbl, string_t &key, SQInteger id)
    {
        tbl->NewSlot( SQString.Create(_sharedstate, key), id);
    }

    SQTable_ptr_t GetKeywords()
    {
        SQTable_ptr_t tbl = SQTable.Create(_sharedstate, (TK_LAST_ENUM_TOKEN - TK_FIRST_ENUM_TOKEN - 1) /*26*/);
        ADD_KEYWORD(tbl, "any_t", TK_LOCAL_ANY_T);
        ADD_KEYWORD(tbl, "array_t", TK_LOCAL_ARRAY_T);
        ADD_KEYWORD(tbl, "as", TK_AS);
        ADD_KEYWORD(tbl, "auto", TK_LOCAL);
        ADD_KEYWORD(tbl, "base", TK_BASE);
        ADD_KEYWORD(tbl, "bool_t", TK_LOCAL_BOOL_T);
        ADD_KEYWORD(tbl, "break", TK_BREAK);
        ADD_KEYWORD(tbl, "case", TK_CASE);
        ADD_KEYWORD(tbl, "catch", TK_CATCH);
        ADD_KEYWORD(tbl, "char_t", TK_LOCAL_CHAR_T);
        ADD_KEYWORD(tbl, "class",TK_CLASS);
        ADD_KEYWORD(tbl, "clone", TK_CLONE);
        ADD_KEYWORD(tbl, "constructor",TK_CONSTRUCTOR);
        ADD_KEYWORD(tbl, "const",TK_CONST);
        ADD_KEYWORD(tbl, "constexpr",TK_CONSTEXPR);
        ADD_KEYWORD(tbl, "continue", TK_CONTINUE);
        ADD_KEYWORD(tbl, "declare", TK_DECLARE);
        ADD_KEYWORD(tbl, "default", TK_DEFAULT);
        ADD_KEYWORD(tbl, "delete", TK_DELETE);
        ADD_KEYWORD(tbl, "destructor",TK_DESTRUCTOR);
        ADD_KEYWORD(tbl, "do", TK_DO);
        ADD_KEYWORD(tbl, "double_t", TK_LOCAL_DOUBLE_T);
        ADD_KEYWORD(tbl, "else", TK_ELSE);
        ADD_KEYWORD(tbl, "enum",TK_ENUM);
        ADD_KEYWORD(tbl, "extends",TK_EXTENDS);
        ADD_KEYWORD(tbl, "extern",TK_EXTERN);
        ADD_KEYWORD(tbl, "false",TK_FALSE);
        ADD_KEYWORD(tbl, "__FILE__",TK___FILE__);
        ADD_KEYWORD(tbl, "float_t", TK_LOCAL_FLOAT_T);
        ADD_KEYWORD(tbl, "foreach", TK_FOREACH);
        ADD_KEYWORD(tbl, "for", TK_FOR);
        ADD_KEYWORD(tbl, "friend", TK_FRIEND);
        ADD_KEYWORD(tbl, "function", TK_FUNCTION);
        ADD_KEYWORD(tbl, "__FUNCTION__",TK___FUNCTION__);
        ADD_KEYWORD(tbl, "if", TK_IF);
        ADD_KEYWORD(tbl, "instanceof",TK_INSTANCEOF);
        ADD_KEYWORD(tbl, "int16_t", TK_LOCAL_INT16_T);
        ADD_KEYWORD(tbl, "int32_t", TK_LOCAL_INT32_T);
        ADD_KEYWORD(tbl, "int64_t", TK_LOCAL_INT64_T);
        ADD_KEYWORD(tbl, "int8_t", TK_LOCAL_INT8_T);
        ADD_KEYWORD(tbl, "in", TK_IN);
        ADD_KEYWORD(tbl, "inline", TK_INLINE);
        ADD_KEYWORD(tbl, "int_t", TK_LOCAL_INT_T);
        ADD_KEYWORD(tbl, "let", TK_LOCAL);
        ADD_KEYWORD(tbl, "__LINE__",TK___LINE__);
        ADD_KEYWORD(tbl, "local", TK_LOCAL);
        ADD_KEYWORD(tbl, "long_double_t", TK_LOCAL_LONG_DOUBLE_T);
        ADD_KEYWORD(tbl, "new",TK_IGNORE);
        ADD_KEYWORD(tbl, "noexcept",TK_NOEXCEPT);
        ADD_KEYWORD(tbl, "number_t", TK_LOCAL_NUMBER_T);
        ADD_KEYWORD(tbl, "null", TK_NULL);
        ADD_KEYWORD(tbl, "NULL", TK_NULL);
        ADD_KEYWORD(tbl, "private",TK_PRIVATE);
        ADD_KEYWORD(tbl, "public",TK_PUBLIC);
        ADD_KEYWORD(tbl, "resume", TK_RESUME);
        ADD_KEYWORD(tbl, "return", TK_RETURN);
        ADD_KEYWORD(tbl, "size_t",TK_LOCAL_SIZE_T);
        ADD_KEYWORD(tbl, "ssize_t",TK_LOCAL_SSIZE_T);
        ADD_KEYWORD(tbl, "static",TK_STATIC);
        ADD_KEYWORD(tbl, "string_t", TK_LOCAL_STRING_T);
        ADD_KEYWORD(tbl, "struct",TK_STRUCT);
        ADD_KEYWORD(tbl, "switch", TK_SWITCH);
        ADD_KEYWORD(tbl, "table_t", TK_LOCAL_TABLE_T);
        ADD_KEYWORD(tbl, "this", TK_THIS);
        ADD_KEYWORD(tbl, "throw", TK_THROW);
        ADD_KEYWORD(tbl, "typedef", TK_TYPEDEF);
        ADD_KEYWORD(tbl, "true",TK_TRUE);
        ADD_KEYWORD(tbl, "try", TK_TRY);
        ADD_KEYWORD(tbl, "typeof", TK_TYPEOF);
        ADD_KEYWORD(tbl, "uint16_t", TK_LOCAL_UINT16_T);
        ADD_KEYWORD(tbl, "uint32_t", TK_LOCAL_UINT32_T);
        ADD_KEYWORD(tbl, "uint64_t", TK_LOCAL_UINT64_T);
        ADD_KEYWORD(tbl, "uint8_t", TK_LOCAL_UINT8_T);
        ADD_KEYWORD(tbl, "uint_t", TK_LOCAL_UINT_T);
        ADD_KEYWORD(tbl, "unsafe", TK_UNSAFE);
        ADD_KEYWORD(tbl, "using", TK_USING);
        ADD_KEYWORD(tbl, "var", TK_LOCAL);
        ADD_KEYWORD(tbl, "virtual", TK_VIRTUAL);
        ADD_KEYWORD(tbl, "void_ptr_t", TK_LOCAL_VOIDPTR_T);
        ADD_KEYWORD(tbl, "void", TK_VOID);
        ADD_KEYWORD(tbl, "volatile", TK_VOLATILE);
        ADD_KEYWORD(tbl, "wchar_t", TK_LOCAL_WCHAR_T);
        ADD_KEYWORD(tbl, "weakref_t", TK_LOCAL_WEAKREF_T);
        ADD_KEYWORD(tbl, "while", TK_WHILE);
        ADD_KEYWORD(tbl, "yield", TK_YIELD);

        return tbl;
    }

    SQInteger Error(const SQChar_ptr_t err, ...)
    {
	_lasterror = err;
	if(0)
	{
	throw err;
        va_list vl;
        va_start(vl, fmt);
        scvsprintf(_lasterror, sizeof(_lasterror), fmt, vl);
        va_end(vl);
	}
        if(_errfunc) _errfunc(_errtarget,_lasterror);
        return -1;
    }

    SQInteger Lex()
    {
//print("Lex", __LINE__, _currentline, _currentcolumn, CUR_CHAR);
        _lasttokenline = _currentline;
        _lasttokencolumn = _currentcolumn;
        while(CUR_CHAR != SQUIRREL_EOB)
        {
            switch(CUR_CHAR)
            {
            case '\t':
            case '\r':
            case ' ':
                if(Next()) return -1;
                continue;
            case '\n':
                _currentline++;
                _prevtoken=_curtoken;
                _curtoken='\n';
                if(Next()) return -1;
                _currentcolumn=1;
                continue;
            case '#':
                if(Next()) return -1;
                if(CUR_CHAR == '!') //shell shebang
                {
                    if(LexLineComment()) return -1;
                    if(_want_comments) return RETURN_TOKEN(TK_COMMENT_LINE)
                        continue;
                }
                return RETURN_TOKEN(TK_PRAGMA);
                continue;
            case '/':
                if(Next()) return -1;
                switch(CUR_CHAR)
                {
                case '*':
                    if(LexBlockComment()) return -1;
                    if(_want_comments) return RETURN_TOKEN(TK_COMMENT_BLOCK)
                        continue;
                case '/':
                    if(LexLineComment()) return -1;
                    if(_want_comments) return RETURN_TOKEN(TK_COMMENT_LINE)
                        continue;
                case '=':
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_DIVEQ);
                    continue;
                case '>':
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_ATTR_CLOSE);
                    continue;
                default:
                    return RETURN_TOKEN('/');
                }
            case '=':
                if(Next()) return -1;
                if (CUR_CHAR != '=')
                {
                    return RETURN_TOKEN('=')
                }
                else
                {
                    if(Next()) return -1;
                    if (CUR_CHAR == '=')
                    {
                        if(Next()) return -1;
                        return RETURN_TOKEN(TK_EQ_IDENTITY)
                    }
                    else
                    {
                        return RETURN_TOKEN(TK_EQ);
                    }
                }
            case '<':
                if(Next()) return -1;
                switch(CUR_CHAR)
                {
                case '=':
                    if(Next()) return -1;
                    if(CUR_CHAR == '>')
                    {
                        if(Next()) return -1;
                        return RETURN_TOKEN(TK_3WAYSCMP);
                    }
                    return RETURN_TOKEN(TK_LE)
                    break;
                case '-':
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_NEWSLOT);
                    break;
                case '<':
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_SHIFTL);
                    break;
                case '/':
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_ATTR_OPEN);
                    break;
                }
                return RETURN_TOKEN('<');
            case '>':
                if(Next()) return -1;
                if (CUR_CHAR == '=')
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_GE);
                }
                else if(CUR_CHAR == '>')
                {
                    if(Next()) return -1;
                    if(CUR_CHAR == '>')
                    {
                        if(Next()) return -1;
                        return RETURN_TOKEN(TK_USHIFTR);
                    }
                    return RETURN_TOKEN(TK_SHIFTR);
                }
                else
                {
                    return RETURN_TOKEN('>')
                }
            case '!':
                if(Next()) return -1;
                if (CUR_CHAR != '=')
                {
                    return RETURN_TOKEN('!')
                }
                else
                {
                    if(Next()) return -1;
                    if (CUR_CHAR == '=')
                    {
                        if(Next()) return -1;
                        return RETURN_TOKEN(TK_NE_IDENTITY)
                    }
                    else
                    {
                        return RETURN_TOKEN(TK_NE);
                    }
                }
            case '@':
            {
                SQInteger stype;
                if(Next()) return -1;
                if(CUR_CHAR != '"')
                {
                    return RETURN_TOKEN('@');
                }
                if((stype=ReadString('"',true))!=-1)
                {
                    return RETURN_TOKEN(stype);
                }
                return Error("error parsing the string");
            }
            case '"':
            case '\'':
            {
                SQInteger stype;
                if((stype=ReadString(CUR_CHAR,false))!=-1)
                {
                    return RETURN_TOKEN(stype);
                }
                return Error("error parsing the string");
            }
            case '{':
            case '}':
            case '(':
            case ')':
            case '[':
            case ']':
            case ';':
            case ',':
            case '?':
            case '~':
            {
                SQInteger ret = CUR_CHAR;
                if(Next()) return -1;
                if((ret == '[' || ret == '{' || ret == '(') && CUR_CHAR == '=')
                {
                    //lets try lua literal delimiters
                    SQInteger stype;
                    if((stype=ReadString(ret,true))!=-1)
                    {
                        return RETURN_TOKEN(stype);
                    }
                    return Error("error parsing the string");
                }
                else return RETURN_TOKEN(ret);
            }
            case '.':
                if(Next()) return -1;
                if (CUR_CHAR != '.')
                {
                    return RETURN_TOKEN('.')
                }
                if(Next()) return -1;
                if (CUR_CHAR != '.')
                {
                    return Error("invalid token '..'");
                }
                if(Next()) return -1;
                return RETURN_TOKEN(TK_VARPARAMS);
            case '^':
                if(Next()) return -1;
                //if (CUR_CHAR == '='){ if(Next()) return -1; return RETURN_TOKEN(TK_BIT_XOR_EQ);}
                return RETURN_TOKEN('^');
            case '&':
                if(Next()) return -1;
                //if (CUR_CHAR == '='){ if(Next()) return -1; return RETURN_TOKEN(TK_BIT_AND_EQ);}
                if (CUR_CHAR != '&')
                {
                    return RETURN_TOKEN('&')
                }
                else
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_AND);
                }
            case '|':
                if(Next()) return -1;
                //if (CUR_CHAR == '='){ if(Next()) return -1; return RETURN_TOKEN(TK_BIT_OR_EQ);}
                if (CUR_CHAR != '|')
                {
                    return RETURN_TOKEN('|')
                }
                else
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_OR);
                }
            case ':':
                if(Next()) return -1;
                if (CUR_CHAR != ':')
                {
                    return RETURN_TOKEN(':')
                }
                else
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_DOUBLE_COLON);
                }
            case '*':
                if(Next()) return -1;
                if (CUR_CHAR == '=')
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_MULEQ);
                }
                else return RETURN_TOKEN('*');
            case '%':
                if(Next()) return -1;
                if (CUR_CHAR == '=')
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_MODEQ);
                }
                else return RETURN_TOKEN('%');
            case '-':
                if(Next()) return -1;
                if (CUR_CHAR == '=')
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_MINUSEQ);
                }
                else if  (CUR_CHAR == '-')
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_MINUSMINUS);
                }
                else if  (CUR_CHAR == '>')
                {
                    if(Next()) return -1;    //accept C/C++ like pointers
                    return RETURN_TOKEN('.');
                }
                else return RETURN_TOKEN('-');
            case '+':
                if(Next()) return -1;
                if (CUR_CHAR == '=')
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_PLUSEQ);
                }
                else if (CUR_CHAR == '+')
                {
                    if(Next()) return -1;
                    return RETURN_TOKEN(TK_PLUSPLUS);
                }
                else return RETURN_TOKEN('+');
            case SQUIRREL_EOB:
                return 0;
            default:
            {
                if (scisdigit(CUR_CHAR))
                {
                    SQInteger ret = ReadNumber();
                    if(ret < 0) return -1;
                    return RETURN_TOKEN(ret);
                }
                else if (scisalpha(CUR_CHAR) || CUR_CHAR == '_')
                {
                    SQInteger t = ReadID();
                    if(t < 0) return -1;
                    return RETURN_TOKEN(t);
                }
                else
                {
                    SQInteger c = CUR_CHAR;
                    if (sciscntrl((int)c)) return Error("unexpected character(control)");
                    if(Next()) return -1;
                    return RETURN_TOKEN(c);
                }
                return RETURN_TOKEN(0);
            }
            }
        }
        return 0;
    }

    const SQChar_ptr_t Tok2Str(SQInteger tok)
    {
	foreach(k,v in _keywords->_tbl)
	{
		if(v == tok) return k;
	}
	if(0)
	{
		SQObjectPtr itr, key, val;
		SQInteger nitr;
		while((nitr = _keywords->Next(false,itr, key, val)) != -1)
		{
		    itr = /*(SQInteger)*/nitr;
		    if(/*((SQInteger)_integer(*/val/*))*/ == tok)
			return /*_stringval(*/key/*)*/;
		}
	}
        return NULL;
    }

    const SQChar_ptr_t GetTokenName(int tk_code)
    {
	foreach(k,v in getconsttable())
	{
		if(v == tk_code) return k;
	}

	    const SQChar_ptr_t str_tk;

        switch(tk_code)
        {
        //#define ENUM_TK(a) case TK_##a: str_tk = "TK_" #a); break;
        //SQ_KEYWORDS_LIST()
        //#undef ENUM_TK
        default:
            str_tk = "()";
        }
        return str_tk;
    }

//private:
    SQInteger GetIDType(const SQChar_ptr_t s,SQInteger len)
    {
	foreach(k,v in _keywords->_tbl)
	{
		if(k == s) return v;
	}
	    
        SQObjectPtr t;
        if(_keywords->GetStr(s,len, t))
        {
            return /*SQInteger(_integer(*/t/*))*/;
        }
        return TK_IDENTIFIER;
    }

    SQInteger ReadString(SQInteger ndelim,bool verbatim)
    {
        INIT_TEMP_STRING();
        SQInteger start_equals = 0;
        SQChar cdelim1, cdelim2;
        if(ndelim == '{')
        {
            cdelim1 = '{';
            cdelim2 = '}';
        }
        else if(ndelim == '(')
        {
            cdelim1 = '(';
            cdelim2 = ')';
        }
        else
        {
            cdelim1 = '[';
            cdelim2 = ']';
        }
        if(CUR_CHAR == '=')
        {
            //lua like literal
            while(!IS_EOB() && CUR_CHAR == '=')
            {
                ++start_equals;
                if(Next()) return -1;
            }
            if(CUR_CHAR != cdelim1)
            {
                //it's not a lua literal delimiter
                return Error("expect '%c' on literal delimiter", cdelim1);
            }
            ndelim = cdelim2;
        }
        if(Next()) return -1;
        if(IS_EOB()) return -1;
        if(start_equals)
        {
            int cr_nl = CUR_CHAR == '\r';
            if(cr_nl) if(Next()) return -1;
            cr_nl = CUR_CHAR == '\n';
            if(cr_nl) if(Next()) return -1;
            if(cr_nl)  //if a new line follows the start of delimiter drop it
            {
                ++_currentline;
                if(IS_EOB())
                {
                    return Error("unfinished string");
                }
            }
        }
        for(;;)
        {
            while(CUR_CHAR != ndelim)
            {
                SQInteger x = CUR_CHAR;
                switch(x)
                {
                case SQUIRREL_EOB:
                    return Error("unfinished string");
                case '\n':
                    if(!verbatim) return Error("newline in a constant");
                    APPEND_CHAR(CUR_CHAR);
                    if(Next()) return -1;
                    _currentline++;
                    break;
                case '\\':
                    if(verbatim)
                    {
                        APPEND_CHAR('\\');
                        if(Next()) return -1;
                    }
                    else
                    {
                        if(Next()) return -1;
                        switch(CUR_CHAR)
                        {
                        case 'x':
                        {
                            const SQInteger maxdigits = sizeof(SQChar) * 2;
                            SQChar temp; //[maxdigits + 1];
                            if(ProcessStringHexEscape(temp, maxdigits) < 0) return -1;
                            SQChar_ptr_t stemp;
                            APPEND_CHAR((SQChar)scstrtoul(temp, /*&*/stemp, 16));
                        }
                        break;
                        case 'U':
                        case 'u':
                        {
                            const SQInteger maxdigits = x == 'u' ? 4 : 8;
                            SQChar temp; //[8 + 1];
                            if(ProcessStringHexEscape(temp, maxdigits) < 0) return -1;
                            SQChar_ptr_t stemp;
                            /*
                            #ifdef SQUNICODE
                            #if WCHAR_SIZE == 2
                            			AddUTF16(scstrtoul(temp, &stemp, 16));
                            #else
                            			ADD_CHAR((SQChar)scstrtoul(temp, &stemp, 16));
                            #endif
                            #else
                            */
                            AddUTF8(scstrtoul(temp, /*&*/stemp, 16));
                            //#endif
                        }
                        break;
                        case 't':
                            APPEND_CHAR('\t');
                            if(Next()) return -1;
                            break;
                        case 'a':
                            APPEND_CHAR('\a');
                            if(Next()) return -1;
                            break;
                        case 'b':
                            APPEND_CHAR('\b');
                            if(Next()) return -1;
                            break;
                        case 'n':
                            APPEND_CHAR('\n');
                            if(Next()) return -1;
                            break;
                        case 'r':
                            APPEND_CHAR('\r');
                            if(Next()) return -1;
                            break;
                        case 'v':
                            APPEND_CHAR('\v');
                            if(Next()) return -1;
                            break;
                        case 'f':
                            APPEND_CHAR('\f');
                            if(Next()) return -1;
                            break;
                        case '0':
                            APPEND_CHAR('\0');
                            if(Next()) return -1;
                            break;
                        case '\\':
                            APPEND_CHAR('\\');
                            if(Next()) return -1;
                            break;
                        case '"':
                            APPEND_CHAR('"');
                            if(Next()) return -1;
                            break;
                        case '\'':
                            APPEND_CHAR('\'');
                            if(Next()) return -1;
                            break;
                        default:
                            return Error("unrecognised escaper char");
                            break;
                        }
                    }
                    break;
                default:
                    APPEND_CHAR(CUR_CHAR);
                    if(Next()) return -1;
                }
            }
            if(Next()) return -1;
            if(start_equals)
            {
                bool lastBraceAdded = false;
                if(CUR_CHAR == '=')
                {
                    SQInteger end_equals = start_equals;
                    if(Next()) return -1;
                    if(CUR_CHAR == '=' || CUR_CHAR == cdelim2)
                    {
                        --end_equals;
                        while(!IS_EOB() && CUR_CHAR == '=')
                        {
                            --end_equals;
                            if(Next()) return -1;
                        }
                        if(end_equals) return Error("expect same number of '=' on literal delimiter");
                        if(CUR_CHAR != cdelim2) return Error("expect '%c' to close literal delimiter", cdelim2);
                        if(Next()) return -1;
                        break;
                    }
                    APPEND_CHAR(cdelim2); //the first NEXT() after break the while loop
                    APPEND_CHAR('=');
                    lastBraceAdded = true;
                }
                if(!lastBraceAdded) APPEND_CHAR(cdelim2); //the first NEXT() after break the while loop
                APPEND_CHAR(CUR_CHAR);
                if(Next()) return -1;
            }
            else if(verbatim && CUR_CHAR == '"')   //double quotation
            {
                APPEND_CHAR(CUR_CHAR);
                if(Next()) return -1;
            }
            else
            {
                break;
            }
        }
        TERMINATE_BUFFER();
        SQInteger len = _longstr.size()/*-1*/;
        if(ndelim == '\'')
        {
            if(len == 0) return Error("empty constant");
            if(len > 1) return Error("constant too long");
            _nvalue = _longstr.tostring()/*[0]*/;
            return TK_INTEGER;
        }
        _svalue = /*&*/_longstr.tostring()/*[0]*/;
        return TK_STRING_LITERAL;
    }

    //#define MAX_HEX_DIGITS (sizeof(SQInteger)*2)
    SQInteger ReadNumber()
    {
        //#define TINT 1
        //#define TFLOAT 2
        //#define THEX 3
        //#define TSCIENTIFIC 4
        //#define TOCTAL 5
        SQInteger type = TINT, firstchar = CUR_CHAR;
        SQUnsignedInteger itmp=0;
        SQChar_ptr_t sTemp;
        INIT_TEMP_STRING();
        if(Next()) return -1;
        if(firstchar == '0' && (sctoupper(CUR_CHAR) == 'X' || scisodigit(CUR_CHAR)) )
        {
            if(scisodigit(CUR_CHAR))
            {
                type = TOCTAL;
                while(scisodigit(CUR_CHAR))
                {
                    APPEND_CHAR(CUR_CHAR);
                    if(Next()) return -1;
                }
                if(scisdigit(CUR_CHAR)) return Error("invalid octal number");
            }
            else
            {
                if(Next()) return -1;
                type = THEX;
                while(scisxdigit(CUR_CHAR))
                {
                    APPEND_CHAR(CUR_CHAR);
                    if(Next()) return -1;
                }
                if(_longstr.size() > MAX_HEX_DIGITS) return Error("too many digits for an Hex number");
            }
        }
        else
        {
            APPEND_CHAR(/*(int)*/firstchar);
            while (CUR_CHAR == '.' || scisdigit(CUR_CHAR) || isexponent(CUR_CHAR))
            {
                if(CUR_CHAR == '.' || isexponent(CUR_CHAR)) type = TFLOAT;
                if(isexponent(CUR_CHAR))
                {
                    if(type != TFLOAT) return Error("invalid numeric format");
                    type = TSCIENTIFIC;
                    APPEND_CHAR(CUR_CHAR);
                    if(Next()) return -1;
                    if(CUR_CHAR == '+' || CUR_CHAR == '-')
                    {
                        APPEND_CHAR(CUR_CHAR);
                        if(Next()) return -1;
                    }
                    if(!scisdigit(CUR_CHAR)) return Error("exponent expected");
                }

                APPEND_CHAR(CUR_CHAR);
                if(Next()) return -1;
            }
        }
        TERMINATE_BUFFER();
        switch(type)
        {
        case TSCIENTIFIC:
        case TFLOAT:
            _fvalue = /*(SQFloat)*/scstrtod(/*&*/_longstr.tostring()/*[0]*/,/*&*/sTemp);
            return TK_FLOAT;
        case TINT:
            itmp = LexInteger(/*&*/_longstr.tostring()/*[0]*/,/*&*/itmp);
            break;
        case THEX:
            itmp = LexHexadecimal(/*&*/_longstr.tostring()/*[0]*/,/*&*/itmp);
            break;
        case TOCTAL:
            itmp = LexOctal(/*&*/_longstr.tostring()/*[0]*/,/*&*/itmp);
            break;
        }
        switch(type)
        {
        case TINT:
        case THEX:
        case TOCTAL:
            //to allow 64 bits integers comment bellow
            //if(itmp > INT_MAX) return Error("integer overflow %ulld %d"));
            _nvalue = /*(SQInteger)*/ itmp;
            return TK_INTEGER;
        }
        return 0;
    }

    SQInteger LexBlockComment()
    {
        /*
            if(CUR_CHAR == '*')
            {
        	NEXT();
        	if(CUR_CHAR != '*'){ //document comment
        	    printf("Doument comment found at line %d\n", _currentline);
        	}
            }
        */
        bool done = false;
        if(_want_comments) INIT_TEMP_STRING();
        if(Next()) return -1; //remove the comment token '*'
        while(!done)
        {
            switch(CUR_CHAR)
            {
            case '*':
            {
                if(Next()) return -1;
                if(CUR_CHAR == '/')
                {
                    done = true;
                    if(Next()) return -1;
                    continue;
                }
            };
            break;
            case '\n':
                _currentline++;
                break;
            case SQUIRREL_EOB:
                return Error("missing \"*/\" in comment");
            }
            if(_want_comments) APPEND_CHAR(CUR_CHAR);
            if(Next()) return -1;
        }
        if(_want_comments)
        {
            TERMINATE_BUFFER();
            if(_longstr.size() > 0) _longstr./*pop_back()*/setLen(_longstr.size()-1); //remove the last '*'
            _svalue = /*&*/_longstr.tostring()/*[0]*/;
        }
        return 0;
    }

    SQInteger LexLineComment()
    {
        if(_want_comments) INIT_TEMP_STRING();
        if(Next()) return -1; //remove the comment token
        while (CUR_CHAR != '\n' && (!IS_EOB()))
        {
            if(_want_comments) APPEND_CHAR(CUR_CHAR);
            if(Next()) return -1;
        }
        if(_want_comments)
        {
            TERMINATE_BUFFER();
            _svalue = /*&*/_longstr.tostring()/*[0]*/;
        }
        return 0;
    }

    SQInteger ReadID()
    {
        SQInteger res;
        INIT_TEMP_STRING();
        do
        {
            APPEND_CHAR(CUR_CHAR);
            if(Next()) return -1;
        }
        while(scisalnum(CUR_CHAR) || CUR_CHAR == '_');
        TERMINATE_BUFFER();	
        res = GetIDType(/*&*/_longstr.tostring()/*[0]*/,_longstr.size() - 1);
        if(res == TK_IDENTIFIER || res == TK_CONSTRUCTOR || res == TK_DESTRUCTOR)
        {
            _svalue = /*&*/_longstr.tostring()/*[0]*/;
        }
        return res;
    }

    SQInteger Next()
    {
        SQInteger t = _readf(_up);
        if(t > MAX_CHAR) return Error("Invalid character");
        if(t != 0)
        {
            CUR_CHAR = _currdata = /*(LexChar)*/t;
	    ++_currentcolumn;
            return 0;
        }
        CUR_CHAR = _currdata = SQUIRREL_EOB;
        _reached_eof = SQTrue;
        return 0;
    }

    /*
    #ifdef SQUNICODE
    #if WCHAR_SIZE == 2
        SQInteger AddUTF16(SQUnsignedInteger ch);
    #endif
    #else
        SQInteger AddUTF8(SQUnsignedInteger ch);
    #endif
    */
    SQInteger AddUTF8(SQUnsignedInteger ch)
    {
        if (ch < 0x80)
        {
            APPEND_CHAR((char)ch);
            return 1;
        }
        if (ch < 0x800)
        {
            APPEND_CHAR((SQChar)((ch >> 6) | 0xC0));
            APPEND_CHAR((SQChar)((ch & 0x3F) | 0x80));
            return 2;
        }
        if (ch < 0x10000)
        {
            APPEND_CHAR((SQChar)((ch >> 12) | 0xE0));
            APPEND_CHAR((SQChar)(((ch >> 6) & 0x3F) | 0x80));
            APPEND_CHAR((SQChar)((ch & 0x3F) | 0x80));
            return 3;
        }
        if (ch < 0x110000)
        {
            APPEND_CHAR((SQChar)((ch >> 18) | 0xF0));
            APPEND_CHAR((SQChar)(((ch >> 12) & 0x3F) | 0x80));
            APPEND_CHAR((SQChar)(((ch >> 6) & 0x3F) | 0x80));
            APPEND_CHAR((SQChar)((ch & 0x3F) | 0x80));
            return 4;
        }
        return 0;
    }

    SQInteger ProcessStringHexEscape(SQChar_ptr_t dest, SQInteger maxdigits)
    {
        if(Next()) return -1;
        if (!isxdigit(CUR_CHAR)) return Error("hexadecimal number expected");
        SQInteger n = 0;
        while (isxdigit(CUR_CHAR) && n < maxdigits)
        {
            dest[n] = CUR_CHAR;
            n++;
            if(Next()) return -1;
        }
        dest[n] = 0;
        return n;
    }

    SQInteger _curtoken;
    SQTable_ptr_t _keywords;
    SQBool _reached_eof;
//public:
    SQInteger _prevtoken;
    SQInteger _currentline;
    SQInteger _lasttokenline;
    SQInteger _lasttokencolumn;
    SQInteger _currentcolumn;
    const SQChar_ptr_t _svalue;
    SQInteger _nvalue;
    SQFloat _fvalue;
    SQLEXREADFUNC _readf;
    SQUserPointer _up;
    LexChar _currdata;
    SQSharedState_ptr_t _sharedstate;
    sqvector_SQChar _longstr;
    CompilerErrorFunc _errfunc;
    void_ptr_t _errtarget;
    SQChar_ptr_t _lasterror/*[256]*/;
    SQBool _want_comments;
};

SQSharedState ss;
SQUserPointer up;
CompilerErrorFunc efunc;
void_ptr_t ed;

string_t cpp_code = readfile(__FILE__);
print(__FILE__, cpp_code.len());
//cpp_code = "int main(){return 0;}";

FILE source = FILE(cpp_code);

SQLexerNut lex = SQLexerNut();
lex.Init(ss, compilerReadFunc, source, efunc, ed, SQTrue);

int_t tok;

double_t start_time = os.clock();
while((tok = lex.Lex()) > 0)
{
	//printf("Token = %d\n", tok);

	string_t tkstr;
	if(tok > TK_FIRST_ENUM_TOKEN && tok < TK_LAST_ENUM_TOKEN) tkstr = lex.Tok2Str(tok);
	else tkstr = tok.tochar();
	
	print(
		tok,
		tkstr || "",
		lex.GetTokenName(tok),
		lex._svalue || "",
		lex._nvalue,
		//lex._lasttokenline,
		//lex._lasttokencolumn,
		lex._currentline,
		lex._currentcolumn
		);

}

print("Time spent", os.clock() - start_time);
