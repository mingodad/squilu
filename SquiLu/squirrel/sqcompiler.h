/*	see copyright notice in squirrel.h */
#ifndef _SQCOMPILER_H_
#define _SQCOMPILER_H_

struct SQVM;

enum SQKeywordsEnum {
    TK_FIRST_ENUM_TOKEN = 258,
    /*
    the above token is only for internal purposes
    like calculate total enum_tokens = TK_LAST_ENUM_TOKEN - TK_FIRST_ENUM_TOKEN -1
    */
    TK_IDENTIFIER,
    TK_STRING_LITERAL,
    TK_INTEGER,
    TK_FLOAT,
    TK_BASE,
    TK_DELETE,
    TK_EQ,
    TK_EQ_IDENTITY,
    TK_NE,
    TK_NE_IDENTITY,
    TK_LE,
    TK_GE,
    TK_SWITCH,
    TK_ARROW,
    TK_AND,
    TK_OR,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_BREAK,
    TK_FOR,
    TK_DO,
    TK_NULL,
    TK_FOREACH,
    TK_IN,
    TK_NEWSLOT,
    TK_MODULO,
    TK_LOCAL,
    TK_CLONE,
    TK_FUNCTION,
    TK_RETURN,
    TK_TYPEOF,
    TK_UMINUS,
    TK_PLUSEQ,
    TK_MINUSEQ,
    TK_CONTINUE,
    TK_YIELD,
    TK_TRY,
    TK_CATCH,
    TK_THROW,
    TK_SHIFTL,
    TK_SHIFTR,
    TK_RESUME,
    TK_DOUBLE_COLON,
    TK_CASE,
    TK_DEFAULT,
    TK_THIS,
    TK_PLUSPLUS,
    TK_MINUSMINUS,
    TK_3WAYSCMP,
    TK_USHIFTR,
    TK_CLASS,
    TK_EXTENDS,
    TK_CONSTRUCTOR,
    TK_DESTRUCTOR,
    TK_INSTANCEOF,
    TK_VARPARAMS,
    //TK_VARGC,
    //TK_VARGV,
    TK_TRUE,
    TK_FALSE,
    TK_MULEQ,
    TK_DIVEQ,
    TK_MODEQ,
    TK_ATTR_OPEN,
    TK_ATTR_CLOSE,
    TK_STATIC,
    TK_ENUM,
    TK_CONST,
    TK___LINE__,
    TK___FUNCTION__,
    TK___FILE__,
    TK_PRIVATE,
    TK_PUBLIC,

    TK_IGNORE,

    TK_LOCAL_CHAR_T,
    TK_LOCAL_WCHAR_T,
    TK_LOCAL_BOOL_T,

    TK_LOCAL_TABLE_T,
    TK_LOCAL_ARRAY_T,

    TK_LOCAL_INT8_T,
    TK_LOCAL_INT16_T,
    TK_LOCAL_INT32_T,
    TK_LOCAL_INT64_T,
    TK_LOCAL_INT_T,
    TK_LOCAL_UINT8_T,
    TK_LOCAL_UINT16_T,
    TK_LOCAL_UINT32_T,
    TK_LOCAL_UINT64_T,
    TK_LOCAL_UINT_T,

    TK_LOCAL_FLOAT_T,
    TK_LOCAL_DOUBLE_T,
    TK_LOCAL_LONG_DOUBLE_T,

    TK_BIT_AND_EQ,
    TK_BIT_OR_EQ,
    TK_BIT_XOR_EQ,
    TK_BIT_SHIFT_LEFT_EQ,
    TK_BIT_SHIFT_RIGHT_EQ,
    /*
    the next token is only for internal purposes
    like calculate total enum_tokens = TK_LAST_ENUM_TOKEN - TK_FIRST_ENUM_TOKEN -1
    */
    TK_LAST_ENUM_TOKEN
};

typedef void(*CompilerErrorFunc)(void *ud, const SQChar *s);
bool Compile(SQVM *vm, SQLEXREADFUNC rg, SQUserPointer up, const SQChar *sourcename, SQObjectPtr &out,
             bool raiseerror, bool lineinfo, bool show_warnings);
#endif //_SQCOMPILER_H_
