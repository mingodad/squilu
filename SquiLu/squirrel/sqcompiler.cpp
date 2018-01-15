/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#ifndef NO_COMPILER
#include <stdarg.h>
#include <setjmp.h>
#include "sqopcodes.h"
#include "sqstring.h"
#include "sqfuncproto.h"
#include "sqcompiler.h"
#include "sqfuncstate.h"
#include "sqlexer.h"
#include "sqvm.h"
#include "sqtable.h"

enum
{
    eFunctionType_global = 1,
    eFunctionType_local,
    eFunctionType_lambda,
    eFunctionType_member,
    eFunctionType_anonymous
};

#define EXPR   1
#define OBJECT 2
#define BASE   3
#define LOCAL  4
#define OUTER  5
#define EXPR_STATEMENT  6 //we start from statement state

struct SQExpState
{
    SQInteger  etype;       /* expr. type; one of EXPR, OBJECT, BASE, OUTER, LOCAL or EXPR_STATEMENT */
    SQInteger  epos;        /* expr. location on stack; -1 for OBJECT and BASE */
    bool       donot_get;   /* signal not to deref the next value */
};

#define MAX_COMPILER_ERROR_LEN 256

struct SQScope
{
    SQInt32 stacksize;
    SQInt16 outers;
    SQInt16 nested;
};

#define BEGIN_SCOPE() SQScope __oldscope__ = _scope; \
    ++_scope.nested; \
    _scope.outers = _fs->_outers; \
    _scope.stacksize = _fs->GetStackSize();\
    _scope_types.push_back(SQTable::Create(_ss(_vm),0)); \
    _scope_consts.push_back(SQTable::Create(_ss(_vm),0));

#define RESOLVE_OUTERS() if(_fs->GetStackSize() != _scope.stacksize) { \
        if(_fs->CountOuters(_scope.stacksize)) { \
            _fs->AddInstruction(_OP_CLOSE,0,_scope.stacksize); \
        } \
    }

#define END_SCOPE_NO_CLOSE() {	if(_fs->GetStackSize() != _scope.stacksize) { \
            _fs->SetStackSize(_scope.stacksize); \
        } \
        _scope = __oldscope__; \
        _scope_types.pop_back();\
        _scope_consts.pop_back();\
    }

#define END_SCOPE() {	SQInteger oldouters = _fs->_outers;\
        if(_fs->GetStackSize() != _scope.stacksize) { \
            _fs->SetStackSize(_scope.stacksize); \
            if(oldouters != _fs->_outers) { \
                _fs->AddInstruction(_OP_CLOSE,0,_scope.stacksize); \
            } \
        } \
        _scope = __oldscope__; \
    }

#define BEGIN_BREAKBLE_BLOCK()	SQInteger __nbreaks__=_fs->_unresolvedbreaks.size(); \
    SQInteger __ncontinues__=_fs->_unresolvedcontinues.size(); \
    _fs->_breaktargets.push_back(0);_fs->_continuetargets.push_back(0);

#define END_BREAKBLE_BLOCK(continue_target) {__nbreaks__=_fs->_unresolvedbreaks.size()-__nbreaks__; \
    __ncontinues__=_fs->_unresolvedcontinues.size()-__ncontinues__; \
    if(__ncontinues__>0)ResolveContinues(_fs,__ncontinues__,continue_target); \
    if(__nbreaks__>0)ResolveBreaks(_fs,__nbreaks__); \
    _fs->_breaktargets.pop_back();_fs->_continuetargets.pop_back();}

#define CASE_TK_LOCAL_CHAR_TYPES \
		case TK_LOCAL_STRING_T: \
		case TK_LOCAL_CHAR_T: \
		case TK_LOCAL_WCHAR_T

#define CASE_TK_LOCAL_INT_TYPES \
		case TK_LOCAL_INT8_T: \
		case TK_LOCAL_INT16_T: \
		case TK_LOCAL_INT32_T: \
		case TK_LOCAL_INT64_T: \
		case TK_LOCAL_INT_T: \
		case TK_LOCAL_UINT8_T: \
		case TK_LOCAL_UINT16_T: \
		case TK_LOCAL_UINT32_T: \
		case TK_LOCAL_UINT64_T: \
		case TK_LOCAL_UINT_T: \
		case TK_LOCAL_SIZE_T: \
		case TK_LOCAL_SSIZE_T

#define CASE_TK_LOCAL_FLOAT_TYPES \
		case TK_LOCAL_FLOAT_T: \
		case TK_LOCAL_DOUBLE_T: \
		case TK_LOCAL_LONG_DOUBLE_T

#define CASE_TK_NUMBER_TYPES \
		CASE_TK_LOCAL_FLOAT_TYPES: \
		CASE_TK_LOCAL_INT_TYPES: \
		case TK_LOCAL_NUMBER_T

#define CASE_TK_LOCAL_TYPES \
		CASE_TK_LOCAL_CHAR_TYPES: \
		CASE_TK_NUMBER_TYPES: \
		case TK_LOCAL_BOOL_T: \
		case TK_LOCAL_TABLE_T: \
		case TK_LOCAL_ANY_T: \
		case TK_LOCAL_ARRAY_T: \
		case TK_VOID: \
		case TK_LOCAL_VOIDPTR_T: \
		case TK_LOCAL_WEAKREF_T

#define CASE_TK_INTEGER \
        case TK_INTEGER: \
        case TK_UNSIGNED_INTEGER: \
        case TK_SHORT_INTEGER: \
        case TK_UNSIGNED_SHORT_INTEGER: \
        case TK_LONG_INTEGER: \
        case TK_UNSIGNED_LONG_INTEGER: \
        case TK_LONG_LONG_INTEGER: \
        case TK_UNSIGNED_LONG_LONG_INTEGER
static SQInteger compilerReadFunc(SQUserPointer fp)
{
    SQInteger c = fgetc((FILE *)fp);
    if(c == EOF)
    {
        return 0;
    }
    return c;
}

class SQCompiler
{
public:
    SQCompiler(SQVM *v, SQLEXREADFUNC rg, SQUserPointer up, const SQChar* sourcename,
               bool raiseerror, bool lineinfo, bool show_warnings, SQInteger max_nested_includes)
    {
        _vm=v;
        _lex.Init(_ss(v), rg, up,ThrowError,this);
        _sourcename = SQString::Create(_ss(v), sourcename);
        _lineinfo = lineinfo;
        _raiseerror = raiseerror;
        _show_warnings = show_warnings;
        _scope.outers = 0;
        _scope.stacksize = 0;
        _scope.nested = 0;
        _compilererror = NULL;
        _globals = SQTable::Create(_ss(_vm),0);
        _type_names = SQTable::Create(_ss(_vm),0);
        _extern_names = SQTable::Create(_ss(_vm),0);
        _max_nested_includes = max_nested_includes;
        _nested_includes_count = 0;
        _is_parsing_extern = false;
        _ifdef_exclude = 0;
        _ifdef_line = 0;
        _inside_ifdef = 0;
        squilu_lib_path = NULL;
    }
    ~SQCompiler()
    {
        _table(_globals)->Finalize();
        _globals.Null();
    }

    static void ThrowError(void *ud, const SQChar *s)
    {
        SQCompiler *c = (SQCompiler *)ud;
        c->Error(s);
    }

    void Error(const SQChar *s, ...) //__attribute__((format(printf, 2, 3)))
    {
        va_list vl;
        va_start(vl, s);
        scvsprintf(error_buf, sizeof(error_buf), s, vl);
        va_end(vl);
        _compilererror = error_buf;
        longjmp(_errorjmp,1);
    }

    void Warning(const SQChar *s, ...) //__attribute__((format(printf, 2, 3)))
    {
        if(!_show_warnings) return;
        va_list vl;
        va_start(vl, s);
        scvfprintf(stderr, s, vl);
        va_end(vl);
    }

    bool CheckNameIsType(const SQObject &name)
    {
        for(int i=_scope.nested-1; i >= 0; --i)
        {
            if(_table(_scope_types[i])->Exists(name)) return true;
        }
        return _table(_type_names)->Exists(name);
    }

    bool CheckTypeName(const SQObject &name, bool addIfNotExists=false)
    {
        bool found = CheckNameIsType(name);
        if(addIfNotExists && !found)
        {
            SQObjectPtr oname = name, otrue = true;
            _table(_type_names)->NewSlot(oname, otrue);
        }
        return found;
    }

    bool TypesGet(const SQObjectPtr &key,SQObjectPtr &val)
    {
        for(int i=_scope.nested-1; i >= 0; --i)
        {
            if(_table(_scope_types[i])->Get(key,val)) return true;
        }
        return _table(_type_names)->NewSlot(key,val);
    }

    bool TypesNewSlot(const SQObjectPtr &key, const SQObjectPtr &val)
    {
        if(_scope.nested) return _table(_scope_types[_scope.nested-1])->NewSlot(key,val);
        return _table(_type_names)->NewSlot(key,val);
    }

    int CheckExternName(const SQObject &name, bool addIfNotExists=false)
    {
        SQObjectPtr orefcount;
        int found = _table(_extern_names)->Get(name, orefcount);
        if(addIfNotExists && !found)
        {
            SQObjectPtr oname = name;
            _integer(orefcount) = 1; //1 means only declared, 2 declared and assigned
            _table(_extern_names)->NewSlot(oname, orefcount);
            _table(_globals)->NewSlot(oname, orefcount);
        }
        return found ? _integer(orefcount) : 0;
    }
    int ExternNameSetRefCount(const SQObject &name)
    {
        int ref_count = CheckExternName(name);
        if(ref_count == 1)
        {
            SQObjectPtr orefcount;
            _integer(orefcount) = ref_count = 2;
            _table(_extern_names)->Set(name, orefcount);
        }
        return ref_count;
    }

    void AddGlobalName(const SQObject &name)
    {
        SQObjectPtr oname = name, otrue = true;
        _table(_globals)->NewSlot(oname, otrue);
    }

    void CheckGlobalName(const SQObject &name, bool addIfNotExists=false, bool checkLocals=true)
    {
        const SQChar *found = NULL;
        if(CheckExternName(name))
        {
            found = _SC("extern");
        }
        else if(_table(_globals)->Exists(name))
        {
            found = _SC("global");
        }
        if(found)
        {
            if(checkLocals) Error(_SC("%s '%s' already declared"), found, _stringval(name));
            else Warning(_SC("%s:%d:%d warning %s '%s' already declared will be shadowed\n"),
                             _stringval(_sourcename), _lex.data->currentline, _lex.data->currentcolumn,
                             found, _stringval(name));
        }
        else if(checkLocals) CheckLocalNameScope(name, -1, false);
        if(addIfNotExists)
        {
            AddGlobalName(name);
        }
    }

    void CheckLocalNameScope(const SQObject &name, SQInteger scope, bool checkGlobals=true)
    {
        SQInteger found = _fs->GetLocalVariable(name);
        if(found >= 0)
        {
            SQLocalVarInfo &lvi = _fs->_vlocals[found];
            if(lvi._scope == scope)
                Error(_SC("local '%s' already declared"), _stringval(name));
            else
                Warning(_SC("%s:%d:%d warning local '%s' already declared will be shadowed\n"),
                        _stringval(_sourcename), _lex.data->currentline, _lex.data->currentcolumn, _stringval(name));
        }
        else
        {
            found = _fs->FindOuterVariable(name);
            if(found >= 0) Warning(_SC("%s:%d:%d warning outer variable '%s' already declared will be shadowed\n"),
                                       _stringval(_sourcename), _lex.data->currentline, _lex.data->currentcolumn, _stringval(name));
        }
        if(checkGlobals) CheckGlobalName(name, false, false);
        SQObjectPtr strongid = name;
        CheckConstsExists(strongid);
    }

    bool IsConstant(const SQObject &name,SQObject &e)
    {
        SQObjectPtr val;
        if(ConstsGet(name, val))
        {
            e = val;
            return true;
        }
        return false;
    }

    SQInteger ExpressionConstant(SQObject id)
    {
        SQObject constant;
        SQInteger epos;
        if(IsConstant(id, constant))
        {
            /* Handle named constant */
            SQObjectPtr constval;
            SQObject    constid;
            if(sq_type(constant) == OT_TABLE)
            {
                Expect('.');
                constid = Expect(TK_IDENTIFIER);
                if(!_table(constant)->Get(constid, constval))
                {
                    constval.Null();
                    Error(_SC("invalid constant [%s.%s]"), _stringval(id), _stringval(constid));
                }
            }
            else
            {
                constval = constant;
            }
            epos = _fs->PushTarget();

            /* generate direct or literal function depending on size */
            SQObjectType ctype = sq_type(constval);
            switch(ctype)
            {
            case OT_INTEGER:
                EmitLoadConstInt(_integer(constval),epos);
                break;
            case OT_FLOAT:
                EmitLoadConstFloat(_float(constval),epos);
                break;
            default:
                _fs->AddInstruction(_OP_LOAD,epos,_fs->GetConstant(constval));
                break;
            }
        }
        else
        {
            Error(_SC("invalid constant [%s]"), _stringval(id));
        }
        return epos;
    }

    void CheckConstsExists(const SQObjectPtr &key)
    {
        int found = -1;
        for(int i=_scope.nested-1; i >= 0; --i)
        {
            if(_table(_scope_consts[i])->Exists(key))
            {
                found = i+1;
                break;
            }
        }
        if(found < 0 && _table(_ss(_vm)->_consts)->Exists(key)) found = 0;
        if(found == _scope.nested)
        {
            Error(_SC("constant '%s' already exists\n"), _stringval(key));
        }
        if(found >= 0) Warning(_SC("%s:%d:%d warning an already defined constant '%s' will be shadowed\n"),
                                   _stringval(_sourcename), _lex.data->currentline, _lex.data->currentcolumn,  _stringval(key));
    }

    bool ConstsGet(const SQObjectPtr &key,SQObjectPtr &val)
    {
        for(int i=_scope.nested-1; i >= 0; --i)
        {
            if(_table(_scope_consts[i])->Get(key,val)) return true;
        }
        return _table(_ss(_vm)->_consts)->Get(key,val);
    }

    bool ConstsNewSlot(const SQObjectPtr &key, const SQObjectPtr &val)
    {
        if(_scope.nested) return _table(_scope_consts[_scope.nested-1])->NewSlot(key,val);
        return _table(_ss(_vm)->_consts)->NewSlot(key,val);
    }

    void Lex()
    {
        _token = _lex.Lex();
    }
    SQObjectPtr GetTokenObject(SQInteger tok, bool doLex=true)
    {
        SQObjectPtr ret;
        switch(tok)
        {
        case TK_IDENTIFIER:
            ret = _fs->CreateString(_lex.data->svalue);
            break;
        case TK_STRING_LITERAL:
            ret = _fs->CreateString(_lex.data->svalue,_lex.data->longstr.size()-1);
            break;
        CASE_TK_INTEGER:
            ret = SQObjectPtr(_lex.data->nvalue);
            break;
        case TK_FLOAT:
            ret = SQObjectPtr(_lex.data->fvalue);
            break;
        default:
            ret = _fs->CreateString(_lex.GetTokenName(_token));
        }
        if(doLex) Lex();
        return ret;
    }
    void ErrorIfNotToken(SQInteger tok)
    {
        if(_token != tok)
        {
            if(((_token == TK_CONSTRUCTOR) || (_token == TK_DESTRUCTOR)) && tok == TK_IDENTIFIER)
            {
                //do nothing
            }
            else
            {
                const SQChar *etypename;
                if(tok > 255)
                {
                    switch(tok)
                    {
                    case TK_IDENTIFIER:
                        etypename = _SC("IDENTIFIER");
                        break;
                    case TK_STRING_LITERAL:
                        etypename = _SC("STRING_LITERAL");
                        break;
                    CASE_TK_INTEGER:
                        etypename = _SC("INTEGER");
                        break;
                    case TK_FLOAT:
                        etypename = _SC("FLOAT");
                        break;
                    default:
                        etypename = _lex.Tok2Str(tok);
                    }
                    Error(_SC("expected '%s'"), etypename);
                }
                Error(_SC("expected '%c'"), tok);
            }
        }
    }
    SQObject Expect(SQInteger tok, bool doLex=true)
    {
        ErrorIfNotToken(tok);
        return GetTokenObject(tok, doLex);
    }
    SQObject ExpectTypeToken()
    {
        switch(_token)
        {
        CASE_TK_LOCAL_TYPES:
            return GetTokenObject(_token);
            break;
        case TK_IDENTIFIER:
            break;
        default:
            ErrorIfNotToken(TK_IDENTIFIER);
        }
        return GetTokenObject(TK_IDENTIFIER);
    }
    bool IsEndOfStatement()
    {
        return ((_lex.data->prevtoken == _SC('\n')) || (_token == SQUIRREL_EOB) || (_token == _SC('}')) || (_token == _SC(';')));
    }
    void OptionalSemicolon()
    {
        if(_token == _SC(';'))
        {
            Lex();
            return;
        }
        if(!IsEndOfStatement())
        {
            Error(_SC("end of statement expected (; or lf) found (%d)"), _token);
        }
    }
    void MoveIfCurrentTargetIsLocal()
    {
        SQInteger trg = _fs->TopTarget();
        if(_fs->IsLocal(trg))
        {
            trg = _fs->PopTarget(); //pops the target and moves it
            _fs->AddInstruction(_OP_MOVE, _fs->PushTarget(), trg);
        }
    }

    void Pragma()
    {
        int line = _lex.data->currentline;
        int column = _lex.data->currentcolumn;
        Lex();
        if(_token == TK_ELSE)
        {
            if(!_inside_ifdef) Error(_SC("'#else' without '#ifdef'"));
            Lex();
            //if we were not been excluding we start excluding
            if(_ifdef_exclude == 0) ++_ifdef_exclude;
            else if(_ifdef_exclude == 1) --_ifdef_exclude;
            return;
        }
        SQObject id = Expect(TK_IDENTIFIER);
        if(scstrcmp(_stringval(id), _SC("include")) == 0)
        {
#if SQ_ENABLE_INCLUDES
            SQInteger nested_count = _nested_includes_count + 1;
            if((_max_nested_includes <= 0) || (nested_count > _max_nested_includes))
            {
                Error(_SC("Error: too many nested includes %d %s\n"), nested_count, _stringval(id));
            }

            //do not call Lex() since next token can try search for id defined in the include
            //and then go global instead of local, we'll call it after including
            id = Expect(TK_STRING_LITERAL, false);
            //Warning(_SC("%s:%d:%d warning pragma include %s\n"),
            //        _stringval(_sourcename), line, column, _stringval(id));

            const SQChar *lib_path = NULL;
            //first try the filename alone
            FILE *fp = scfopen(_stringval(id), "r");
            while(!fp)
            {
                //now if we have an environment path let's try it
                if(!lib_path)
                {
                    if(!squilu_lib_path) squilu_lib_path = _vm->GetIncludePath();
                    lib_path = squilu_lib_path;
                    if(!lib_path) break;
                }
                while (*lib_path == *SQUILU_PATH_SEP) lib_path++;  /* skip separators */
                if (*lib_path == '\0') break;  /* no more templates */
                const SQChar *l = scstrchr(lib_path, *SQUILU_PATH_SEP);  /* find next separator */
                if (l == NULL) l = lib_path + scstrlen(lib_path);
                size_t path_len = l - lib_path;
                SQChar file_path_name[SQUILU_MAX_PATH];
                if(path_len < sizeof(file_path_name)) //enough room to work ?
                {
                    scsprintf(file_path_name, sizeof(file_path_name), "%s", lib_path);
                    scsprintf(file_path_name + path_len, sizeof(file_path_name) - path_len,
                              "%s%s", SQUILU_DIRSEP, _stringval(id));
                    //printf("file_path_name = %s\n", file_path_name);
                    fp = scfopen(file_path_name, "r");
                    lib_path += path_len; //move the pointer to the next path
                }
            }
            if(fp != NULL)
            {
                //increment nested count
                ++_nested_includes_count;
                //save current source file and lex state
                SQUserPointer saved_up = _lex._up; //current userpointer
                SQLEXREADFUNC saved_readf = _lex._readf; //current readfunction
                SQInteger saved_line = _lex.data->currentline;
                SQInteger saved_column = _lex.data->currentcolumn;
                SQInteger saved_curdata = _lex.data->currdata;
                SQInteger saved_prevtoken = _lex.data->prevtoken;
                SQInteger saved_token = _token;
                SQObjectPtr saved_source_name = _sourcename;

                //set new source file
                _fs->_sourcename = id;
                _sourcename = id;
                _lex.ResetReader(compilerReadFunc, fp, 1);

                //compile the include file
                Lex();
                while(_token > 0)
                {
                    Statement();
                    if(_lex.data->prevtoken != _SC('}') && _lex.data->prevtoken != _SC(';')) OptionalSemicolon();
                }

                //close file
                fclose(fp);
                if(_inside_ifdef) Error(_SC("unterminated #ifdef starting on line %d"), _ifdef_line);
                //restore saved source file and lex state
                _fs->_sourcename = saved_source_name;
                _sourcename = saved_source_name;
                _token = saved_token;
                _lex.data->currdata = saved_curdata;
                _lex.data->prevtoken = saved_prevtoken;
                _lex.data->currentcolumn = saved_column;
                _lex.data->currentline = saved_line;
                _lex._readf = saved_readf;
                _lex._up = saved_up;

                --_nested_includes_count;
                //done let's continue working

                //Now we do the Lex() call skiped before compile the include file
                Lex();

            }
            else
            {
                Error(_SC("Error: opening include file %s\n"), _stringval(id));
            }
#else
            Error(_SC("Error: includes are not enabled\n"));
#endif
        }
        else if(scstrcmp(_stringval(id), _SC("endif")) == 0)
        {
            if(!_inside_ifdef) Error(_SC("'#endif' without '#ifdef'"));
            if(_ifdef_exclude) --_ifdef_exclude;
            --_inside_ifdef;
        }
        else if((scstrcmp(_stringval(id), _SC("ifdef")) == 0)
                || (scstrcmp(_stringval(id), _SC("ifndef")) == 0))
        {
            ++_inside_ifdef;
            bool isIfndef = _stringval(id)[2] == _SC('n');
            id = Expect(TK_IDENTIFIER);
            if(_ifdef_exclude) ++_ifdef_exclude;
            else
            {
                _ifdef_line = line;
                bool isDefined = _vm->IsDefined(_stringval(id));
                if(isIfndef) isDefined = !isDefined;
                _ifdef_exclude = (isDefined ? 0 : 1);
            }
        }
        else if(scstrcmp(_stringval(id), _SC("define")) == 0)
        {
            id = Expect(TK_IDENTIFIER);
            if(_ifdef_exclude == 0)
            {
                if(_vm->IsDefined(_stringval(id)))
                    Warning(_SC("%s:%d:%d warning '%s' redefined\n"),
                        _stringval(_sourcename), line, column, _stringval(id));
                _vm->AddDefined(_stringval(id));
            }
        }
        else if(scstrcmp(_stringval(id), _SC("undef")) == 0)
        {
            id = Expect(TK_IDENTIFIER);
            if(_ifdef_exclude == 0) _vm->RemoveDefined(_stringval(id));
        }
        else
        {
            _lex.data->currentline = line;
            Error(_SC("Error: unknown pragma %s\n"), _stringval(id));
        }
    }

    bool Compile(SQObjectPtr &o)
    {
        _debugline = 1;
        _debugop = 0;

        SQFuncState funcstate(_ss(_vm), NULL,ThrowError,this);
        funcstate._name = SQString::Create(_ss(_vm), _SC("main"));
        _fs = &funcstate;
        _fs->AddParameter(_fs->CreateString(_SC("this")), _scope.nested+1);
        _fs->AddParameter(_fs->CreateString(_SC("vargv")), _scope.nested+1);
        _fs->_varparams = true;
        _fs->_sourcename = _sourcename;

        SQInteger stacksize = _fs->GetStackSize();
        if(setjmp(_errorjmp) == 0)
        {
            Lex();
            while(_token > 0)
            {
                Statement();
                if(_lex.data->prevtoken != _SC('}') && _lex.data->prevtoken != _SC(';')) OptionalSemicolon();
            }
            if(_inside_ifdef) Error(_SC("unterminated #ifdef starting on line %d"), _ifdef_line);
            _fs->SetStackSize(stacksize);
            _fs->AddLineInfos(_lex.data->currentline, _lineinfo, true);
            _fs->AddInstruction(_OP_RETURN, 0xFF);
            _fs->SetStackSize(0);
            o =_fs->BuildProto();
#ifdef _DEBUG_DUMP
            _fs->Dump(_funcproto(o));
#endif
        }
        else
        {
            if(_raiseerror && _ss(_vm)->_compilererrorhandler)
            {
                _ss(_vm)->_compilererrorhandler(_vm, _compilererror, sq_type(_sourcename) == OT_STRING?_stringval(_sourcename):_SC("unknown"),
                                                _lex.data->currentline, _lex.data->currentcolumn);
            }
            _vm->_lasterror = SQString::Create(_ss(_vm), _compilererror, -1);
            _vm->_lasterror_line = _lex.data->currentline;
            _vm->_lasterror_column = _lex.data->currentcolumn;
            return false;
        }
        return true;
    }
    void Statements()
    {
        while(_token != _SC('}') && _token != TK_DEFAULT && _token != TK_CASE)
        {
            Statement();
            if(_lex.data->prevtoken != _SC('}') && _lex.data->prevtoken != _SC(';')) OptionalSemicolon();
        }
    }
    void Statement(bool closeframe = true)
    {
        _es.etype = EXPR_STATEMENT;
        SQObject id;
        _fs->AddLineInfos(_lex.data->currentline, _lineinfo);
        while(_ifdef_exclude && (_token != TK_PRAGMA))
        {
            Lex();
            if(_token <= 0) Error(_SC("'#endif' expected to close '#ifdef' started at %d"), _ifdef_line);
        }
start_again:
        switch(_token)
        {
        case _SC(';'):
            Lex();
            break;
        case TK_IF:
            IfStatement();
            break;
        case TK_WHILE:
            WhileStatement();
            break;
        case TK_DO:
            DoWhileStatement();
            break;
        case TK_FOR:
            ForStatement();
            break;
        case TK_FOREACH:
            ForEachStatement();
            break;
        case TK_SWITCH:
            SwitchStatement();
            break;

        case TK_LOCAL:
            //case TK_CONST:
        CASE_TK_LOCAL_TYPES:
            LocalDeclStatement();
            break;
        case TK_RETURN:
        case TK_YIELD:
        {
            SQOpcode op;
            if(_token == TK_RETURN)
            {
                op = _OP_RETURN;
            }
            else
            {
                op = _OP_YIELD;
                _fs->_bgenerator = true;
            }
            Lex();
            if(!IsEndOfStatement())
            {
                SQInteger retexp = _fs->GetCurrentPos()+1;
                CommaExpr(true);
                if(op == _OP_RETURN && _fs->_traps > 0)
                    _fs->AddInstruction(_OP_POPTRAP, _fs->_traps, 0);
                _fs->_returnexp = retexp;
                _fs->AddInstruction(op, 1, _fs->PopTarget(),_fs->GetStackSize());
            }
            else
            {
                if(op == _OP_RETURN && _fs->_traps > 0)
                    _fs->AddInstruction(_OP_POPTRAP, _fs->_traps ,0);
                _fs->_returnexp = -1;
                _fs->AddInstruction(op, 0xFF,0,_fs->GetStackSize());
            }
            break;
        }
        case TK_GOTO:
        {
            //error if outside any function
            if(!_fs->_parent) Error(_SC("'goto' has to be in a function block"));
            Lex(); //ignore for now
            id = Expect(TK_IDENTIFIER);
            SQGotoLabelsInfo info;
            info.name = id;
            info.line = _lex.data->currentline; //need to get line number here
            info.traps = _fs->_traps;
            info.nested = _scope.nested;
            //Expect(_SC(';'));

            if(info.traps) _fs->AddInstruction(_OP_NOP, 0, 0); //for _OP_POPTRAP
            RESOLVE_OUTERS();
            _fs->AddInstruction(_OP_JMP, 0, -1234);
            //instruction pos
            info.pos = _fs->GetCurrentPos();
            _fs->_unresolvedgotos.push_back(info);
        }
        break;
        case TK_BREAK:
            if(_fs->_breaktargets.size() <= 0)Error(_SC("'break' has to be in a loop block"));
            if(_fs->_breaktargets.top() > 0)
            {
                _fs->AddInstruction(_OP_POPTRAP, _fs->_breaktargets.top(), 0);
            }
            RESOLVE_OUTERS();
            _fs->AddInstruction(_OP_JMP, 0, -1234);
            _fs->_unresolvedbreaks.push_back(_fs->GetCurrentPos());
            Lex();
            break;
        case TK_CONTINUE:
            if(_fs->_continuetargets.size() <= 0)Error(_SC("'continue' has to be in a loop block"));
            if(_fs->_continuetargets.top() > 0)
            {
                _fs->AddInstruction(_OP_POPTRAP, _fs->_continuetargets.top(), 0);
            }
            RESOLVE_OUTERS();
            _fs->AddInstruction(_OP_JMP, 0, -1234);
            _fs->_unresolvedcontinues.push_back(_fs->GetCurrentPos());
            Lex();
            break;
        case TK_FUNCTION:
            FunctionStatement();
            break;
        case TK_CLASS:
        case TK_STRUCT:
            ClassStatement();
            break;
        case TK_ENUM:
            EnumStatement();
            break;
        case _SC('{'):
        {
            BEGIN_SCOPE();
            Lex();
            Statements();
            Expect(_SC('}'));
            if(closeframe)
            {
                END_SCOPE();
            }
            else
            {
                END_SCOPE_NO_CLOSE();
            }
        }
        break;
        case TK_TRY:
            TryCatchStatement();
            break;
        case TK_THROW:
            Lex();
            CommaExpr();
            _fs->AddInstruction(_OP_THROW, _fs->PopTarget());
            break;

        case TK_CONST:
        {
            Lex();
            if(_token == TK_IDENTIFIER)
            {
                id = _fs->CreateString(_lex.data->svalue);
                if(CheckTypeName(id)) //C/C++ type declaration;
                {
                    goto start_again;
                }
                Lex();
            }
            else id = Expect(TK_IDENTIFIER);
            if(_token == _SC(':'))
            {
                //type specifier like typescript
                Lex();
                ExpectTypeToken(); //ignore for now
            }
            Expect(_SC('='));
            SQObjectPtr strongid = id;
            CheckLocalNameScope(id, _scope.nested);
            //CheckConstsExists(strongid);
            SQObject val = ExpectScalar();
            OptionalSemicolon();
            ConstsNewSlot(strongid,SQObjectPtr(val));
        }
        break;

        case TK_INLINE: //accept and ignore
        case TK_CONSTEXPR: //accept and ignore
        case TK_FRIEND:
        case TK_VOLATILE:
            Lex();
            goto start_again;
        case TK_STATIC:
            if(_scope.nested)
            {
                Warning(_SC("%s:%d:%d warning static cualifier is ignored\n"),
                        _stringval(_sourcename), _lex.data->currentline, _lex.data->currentcolumn);
            }
            Lex(); //ignore it only to allow run some C/C++ code
            goto start_again;

        case TK_DECLARE: //parse as extern
        case TK_EXTERN:
            ExternDeclStatement();
            break;

        case TK_TYPEDEF:
        {
            Lex();
            SQObject type_val = ExpectTypeToken();
            id = ExpectTypeToken();
            SQObjectPtr strongid = id;
            CheckLocalNameScope(id, _scope.nested);
            //Expect(_SC(';'));
            TypesNewSlot(strongid,SQObjectPtr(type_val));
            if(_token == _SC('[')) //fixed size array
            {
                Lex();
                Expect(TK_INTEGER);
                Expect(_SC(']'));
            }
        }
        break;
        case TK_USING:
        {
            Lex();
            id = Expect(TK_IDENTIFIER);
            Expect('=');
            SQObjectPtr strongid = id;
            CheckLocalNameScope(id, _scope.nested);
            SQObject type_val = ExpectTypeToken();
            //Expect(_SC(';'));
            TypesNewSlot(strongid,SQObjectPtr(type_val));
        }
        break;

        case TK_PRAGMA:
            Pragma();
            break;

        case TK_UNSAFE:
            Lex(); //ignore for now
            goto start_again;
            break;

        case TK_TEMPLATE:
            TemplateStatement();
            break;

        case TK_IDENTIFIER:
        {
            id = _fs->CreateString(_lex.data->svalue);
            SQInteger lhtk = _lex.LookaheadLex();
            if(lhtk == _SC(':'))
            {
                if(!_fs->_parent) Error(_SC("'label' has to be inside a function block"));
                if(!_fs->AddGotoTarget(id, _lex.data->currentline, _fs->_traps, _scope.nested))
                {
                    Error(_SC("Label already declared"));
                }
                Lex(); //eat ':'
                Lex();
                break;
            }
            if(CheckTypeName(id)) //C/C++ type declaration;
            {
                if(lhtk != _SC('.'))
                {
                    LocalDeclStatement();
                    break;
                }
            }
        }

        default:
            CommaExpr();
            _fs->DiscardTarget();
            /*
            //Fagiano says that this is not a bug
            //and with this modification the stack grow by one lement all the time
            if(_token == TK_IDENTIFIER){
                CommaExpr();
                if(_token == TK_IDENTIFIER){
                    Error(_SC(" '=' expected near '%s'"), _lex.data->svalue);
                }
            }
            */
            //_fs->PopTarget();
            break;
        }
        _fs->SnoozeOpt();
    }
    void EmitDerefOp(SQOpcode op)
    {
        SQInteger val = _fs->PopTarget();
        SQInteger key = _fs->PopTarget();
        SQInteger src = _fs->PopTarget();
        _fs->AddInstruction(op,_fs->PushTarget(),src,key,val);
    }
    void Emit2ArgsOP(SQOpcode op, SQInteger p3 = 0)
    {
        SQInteger p2 = _fs->PopTarget(); //src in OP_GET
        SQInteger p1 = _fs->PopTarget(); //key in OP_GET
        _fs->AddInstruction(op,_fs->PushTarget(), p1, p2, p3);
    }
    void EmitCompoundArith(SQInteger tok, SQInteger etype, SQInteger pos)
    {
        /* Generate code depending on the expression type */
        switch(etype)
        {
        case LOCAL:
        {
            SQOpcode op = ChooseArithOpByToken(tok);
            SQInteger p3 = 0;
            SQInteger p2 = _fs->PopTarget(); //src in OP_GET
            SQInteger p1 = _fs->PopTarget(); //key in OP_GET
            _fs->PushTarget(p1);
            //EmitCompArithLocal(tok, p1, p1, p2);
            if(op == _OP_BITW)
            {
                p3 = ChooseBitwOpByToken(tok);
            }
            _fs->AddInstruction(op,p1, p2, p1, p3);
            _fs->SnoozeOpt(); //FIX: stop optimizer in retargeting opcode
        }
        break;
        case OBJECT:
        case BASE:
        {
            SQInteger val = _fs->PopTarget();
            SQInteger key = _fs->PopTarget();
            SQInteger src = _fs->PopTarget();
            /* _OP_COMPARITH mixes dest obj and source val in the arg1 */
            _fs->AddInstruction(_OP_COMPARITH, _fs->PushTarget(), (src<<16)|val, key, ChooseCompArithCharByToken(tok));
        }
        break;
        case OUTER:
        {
            SQInteger val = _fs->TopTarget();
            SQInteger tmp = _fs->PushTarget();
            _fs->AddInstruction(_OP_GETOUTER,   tmp, pos);
            _fs->AddInstruction(ChooseArithOpByToken(tok), tmp, val, tmp, 0);
            _fs->PopTarget();
            _fs->PopTarget();
            _fs->AddInstruction(_OP_SETOUTER, _fs->PushTarget(), pos, tmp);
        }
        break;
        }
    }
    void CommaExpr(bool warningAssign=false)
    {
        for(Expression(warningAssign); _token == ','; _fs->PopTarget(), Lex(), CommaExpr(warningAssign));
    }
    void ErrorIfConst()
    {
        SQLocalVarInfo &vsrc = _fs->_vlocals[_fs->TopTarget()];
//printf("%d %d %d %d %s\n", __LINE__, vsrc._scope, vsrc._type, vsrc._pos, vsrc._name._unVal.pString ? _stringval(vsrc._name) : "?");
        if(vsrc._type & _VAR_CONST) Error(_SC("can't assign to a const variable"));
    }
    void Expression(bool warningAssign=false)
    {
        SQExpState es = _es;
        _es.etype     = EXPR;
        _es.epos      = -1;
        _es.donot_get = false;
        SQObject id;
        if((_token == TK_IDENTIFIER) && (es.etype == EXPR_STATEMENT))
        {
            id = _fs->CreateString(_lex.data->svalue);
        }
        //else id = {}; //old compilers do not allow this
        else id._type = OT_NULL; //segfault without it with gcc -O3

        LogicalOrExp();
        switch(_token)
        {
        case _SC('='):
        case TK_NEWSLOT:
        case TK_MINUSEQ:
        case TK_PLUSEQ:
        case TK_MULEQ:
        case TK_DIVEQ:
        case TK_MODEQ:
        case TK_BIT_XOR_EQ:
        case TK_BIT_AND_EQ:
        case TK_BIT_OR_EQ:
        case TK_BIT_SHIFT_LEFT_EQ:
        case TK_BIT_SHIFT_RIGHT_EQ:
        {
            SQInteger op = _token;
            SQInteger ds = _es.etype;
            SQInteger pos = _es.epos;

            if(ds == EXPR) Error(_SC("can't assign expression"));
            else if(ds == BASE) Error(_SC("'base' cannot be modified"));
            Lex();
            Expression();

            switch(op)
            {
            case TK_NEWSLOT:
                if(ds == OBJECT || ds == BASE)
                {
                    EmitDerefOp(_OP_NEWSLOT);
                    if((_es.epos == -1) && (es.etype == EXPR_STATEMENT)
                            && (sq_type(id) == OT_STRING) ) AddGlobalName(id);
                }
                else //if _derefstate != DEREF_NO_DEREF && DEREF_FIELD so is the index of a local
                    Error(_SC("can't 'create' a local slot"));
                break;
            case _SC('='): //ASSIGN
                if(warningAssign) Warning(_SC("%s:%d:%d warning making assignment, maybe it's not what you want\n"),
                                              _stringval(_sourcename), _lex.data->currentline, _lex.data->currentcolumn);
                switch(ds)
                {
                case LOCAL:
                {
                    SQInteger src = _fs->PopTarget();
                    SQInteger dst = _fs->TopTarget();
                    _fs->AddInstruction(_OP_MOVE, dst, src);
                }
                break;
                case OBJECT:
                case BASE:
                    EmitDerefOp(_OP_SET);
                    break;
                case OUTER:
                {
                    SQInteger src = _fs->PopTarget();
                    SQInteger dst = _fs->PushTarget();
                    _fs->AddInstruction(_OP_SETOUTER, dst, pos, src);
                }
                }
                break;
            case TK_MINUSEQ:
            case TK_PLUSEQ:
            case TK_MULEQ:
            case TK_DIVEQ:
            case TK_MODEQ:
            case TK_BIT_XOR_EQ:
            case TK_BIT_AND_EQ:
            case TK_BIT_OR_EQ:
            case TK_BIT_SHIFT_LEFT_EQ:
            case TK_BIT_SHIFT_RIGHT_EQ:
                EmitCompoundArith(op, ds, pos);
                break;
            }
        }
        break;
        case _SC('?'):
        {
            Lex();
            _fs->AddInstruction(_OP_JZ, _fs->PopTarget());
            SQInteger jzpos = _fs->GetCurrentPos();
            SQInteger trg = _fs->PushTarget();
            Expression();
            SQInteger first_exp = _fs->PopTarget();
            if(trg != first_exp) _fs->AddInstruction(_OP_MOVE, trg, first_exp);
            SQInteger endfirstexp = _fs->GetCurrentPos();
            _fs->AddInstruction(_OP_JMP, 0, 0);
            Expect(_SC(':'));
            SQInteger jmppos = _fs->GetCurrentPos();
            Expression();
            SQInteger second_exp = _fs->PopTarget();
            if(trg != second_exp) _fs->AddInstruction(_OP_MOVE, trg, second_exp);
            _fs->SetIntructionParam(jmppos, 1, _fs->GetCurrentPos() - jmppos);
            _fs->SetIntructionParam(jzpos, 1, endfirstexp - jzpos + 1);
            _fs->SnoozeOpt();
        }
        break;
        }
        _es = es;
    }
    template<typename T> void INVOKE_EXP(T f)
    {
        SQExpState es = _es;
        _es.etype     = EXPR;
        _es.epos      = -1;
        _es.donot_get = false;
        (this->*f)();
        _es = es;
    }
    template<typename T> void BIN_EXP(SQOpcode op, T f,SQInteger op3 = 0)
    {
        Lex();
        INVOKE_EXP(f);
        SQInteger op1 = _fs->PopTarget();
        SQInteger op2 = _fs->PopTarget();
        _fs->AddInstruction(op, _fs->PushTarget(), op1, op2, op3);
        _es.etype = EXPR;
    }
    void LogicalOrExp()
    {
        LogicalAndExp();
        for(;;) if(_token == TK_OR)
            {
                SQInteger first_exp = _fs->PopTarget();
                SQInteger trg = _fs->PushTarget();
                _fs->AddInstruction(_OP_OR, trg, 0, first_exp, 0);
                SQInteger jpos = _fs->GetCurrentPos();
                if(trg != first_exp) _fs->AddInstruction(_OP_MOVE, trg, first_exp);
                Lex();
                INVOKE_EXP(&SQCompiler::LogicalOrExp);
                _fs->SnoozeOpt();
                SQInteger second_exp = _fs->PopTarget();
                if(trg != second_exp) _fs->AddInstruction(_OP_MOVE, trg, second_exp);
                _fs->SnoozeOpt();
                _fs->SetIntructionParam(jpos, 1, (_fs->GetCurrentPos() - jpos));
                _es.etype = EXPR;
                break;
            }
            else return;
    }
    void LogicalAndExp()
    {
        BitwiseOrExp();
        for(;;) switch(_token)
            {
            case TK_AND:
            {
                SQInteger first_exp = _fs->PopTarget();
                SQInteger trg = _fs->PushTarget();
                _fs->AddInstruction(_OP_AND, trg, 0, first_exp, 0);
                SQInteger jpos = _fs->GetCurrentPos();
                if(trg != first_exp) _fs->AddInstruction(_OP_MOVE, trg, first_exp);
                Lex();
                INVOKE_EXP(&SQCompiler::LogicalAndExp);
                _fs->SnoozeOpt();
                SQInteger second_exp = _fs->PopTarget();
                if(trg != second_exp) _fs->AddInstruction(_OP_MOVE, trg, second_exp);
                _fs->SnoozeOpt();
                _fs->SetIntructionParam(jpos, 1, (_fs->GetCurrentPos() - jpos));
                _es.etype = EXPR;
                break;
            }

            default:
                return;
            }
    }
    void BitwiseOrExp()
    {
        BitwiseXorExp();
        for(;;) if(_token == _SC('|'))
            {
                BIN_EXP(_OP_BITW, &SQCompiler::BitwiseXorExp,BW_OR);
            }
            else return;
    }
    void BitwiseXorExp()
    {
        BitwiseAndExp();
        for(;;) if(_token == _SC('^'))
            {
                BIN_EXP(_OP_BITW, &SQCompiler::BitwiseAndExp,BW_XOR);
            }
            else return;
    }
    void BitwiseAndExp()
    {
        EqExp();
        for(;;) if(_token == _SC('&'))
            {
                BIN_EXP(_OP_BITW, &SQCompiler::EqExp,BW_AND);
            }
            else return;
    }
    void EqExp()
    {
        CompExp();
        for(;;) switch(_token)
            {
            case TK_EQ:
                BIN_EXP(_OP_EQ, &SQCompiler::CompExp);
                break;
            case TK_EQ_IDENTITY :
                BIN_EXP(_OP_EQI, &SQCompiler::CompExp);
                break;
            case TK_NE:
                BIN_EXP(_OP_NE, &SQCompiler::CompExp);
                break;
            case TK_NE_IDENTITY:
                BIN_EXP(_OP_NEI, &SQCompiler::CompExp);
                break;
            case TK_3WAYSCMP:
                BIN_EXP(_OP_CMP, &SQCompiler::CompExp,CMP_3W);
                break;
            default:
                return;
            }
    }
    void CompExp()
    {
        ShiftExp();
        for(;;) switch(_token)
            {
            case _SC('>'):
                BIN_EXP(_OP_CMP, &SQCompiler::ShiftExp,CMP_G);
                break;
            case _SC('<'):
                BIN_EXP(_OP_CMP, &SQCompiler::ShiftExp,CMP_L);
                break;
            case TK_GE:
                BIN_EXP(_OP_CMP, &SQCompiler::ShiftExp,CMP_GE);
                break;
            case TK_LE:
                BIN_EXP(_OP_CMP, &SQCompiler::ShiftExp,CMP_LE);
                break;
            case TK_IN:
                BIN_EXP(_OP_EXISTS, &SQCompiler::ShiftExp);
                break;
            case TK_INSTANCEOF:
                BIN_EXP(_OP_INSTANCEOF, &SQCompiler::ShiftExp);
                break;
            default:
                return;
            }
    }
    void ShiftExp()
    {
        PlusExp();
        for(;;) switch(_token)
            {
            case TK_USHIFTR:
                BIN_EXP(_OP_BITW, &SQCompiler::PlusExp,BW_USHIFTR);
                break;
            case TK_SHIFTL:
                BIN_EXP(_OP_BITW, &SQCompiler::PlusExp,BW_SHIFTL);
                break;
            case TK_SHIFTR:
                BIN_EXP(_OP_BITW, &SQCompiler::PlusExp,BW_SHIFTR);
                break;
            default:
                return;
            }
    }
    SQOpcode ChooseArithOpByToken(SQInteger tok)
    {
        switch(tok)
        {
        case TK_PLUSEQ:
        case _SC('+'):
            return _OP_ADD;
        case TK_MINUSEQ:
        case _SC('-'):
            return _OP_SUB;
        case TK_MULEQ:
        case _SC('*'):
            return _OP_MUL;
        case TK_DIVEQ:
        case _SC('/'):
            return _OP_DIV;
        case TK_MODEQ:
        case _SC('%'):
            return _OP_MOD;
        case TK_BIT_XOR_EQ:
        case TK_BIT_AND_EQ:
        case TK_BIT_OR_EQ:
        case TK_BIT_SHIFT_LEFT_EQ:
        case TK_BIT_SHIFT_RIGHT_EQ:
            return _OP_BITW;
        default:
            assert(0);
        }
        return _OP_ADD;
    }
    BitWiseOP ChooseBitwOpByToken(SQInteger tok)
    {
        switch(tok)
        {
        case TK_BIT_XOR_EQ:
            return BW_XOR;
        case TK_BIT_AND_EQ:
            return BW_AND;
        case TK_BIT_OR_EQ:
            return BW_OR;
        case TK_BIT_SHIFT_LEFT_EQ:
            return BW_SHIFTL;
        case TK_BIT_SHIFT_RIGHT_EQ:
            return BW_SHIFTR;
        default:
            Error(_SC("unknown bitwise token"));
        }
        return BW_AND;
    }
    SQInteger ChooseCompArithCharByToken(SQInteger tok)
    {
        SQInteger oper;
        switch(tok)
        {
        case TK_MINUSEQ:
            oper = _SC('-');
            break;
        case TK_PLUSEQ:
            oper = _SC('+');
            break;
        case TK_MULEQ:
            oper = _SC('*');
            break;
        case TK_DIVEQ:
            oper = _SC('/');
            break;
        case TK_MODEQ:
            oper = _SC('%');
            break;
            break;
        default:
            oper = 0; //shut up compiler
            assert(0);
            break;
        };
        return oper;
    }
    void PlusExp()
    {
        MultExp();
        for(;;) switch(_token)
            {
            case _SC('+'):
            case _SC('-'):
                BIN_EXP(ChooseArithOpByToken(_token), &SQCompiler::MultExp);
                break;
            default:
                return;
            }
    }

    void MultExp()
    {
        PrefixedExpr();
        for(;;) switch(_token)
            {
            case _SC('*'):
            case _SC('/'):
            case _SC('%'):
                BIN_EXP(ChooseArithOpByToken(_token), &SQCompiler::PrefixedExpr);
                break;
            default:
                return;
            }
    }
    //if 'pos' != -1 the previous variable is a local variable
    void PrefixedExpr()
    {
        SQInteger pos = Factor();
        for(;;)
        {
            switch(_token)
            {
            case _SC('.'):
                pos = -1;
                Lex();

                _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(Expect(TK_IDENTIFIER)));
                if(_es.etype==BASE)
                {
                    Emit2ArgsOP(_OP_GET);
                    pos = _fs->TopTarget();
                    _es.etype = EXPR;
                    _es.epos   = pos;
                }
                else
                {
                    if(NeedGet())
                    {
                        Emit2ArgsOP(_OP_GET);
                    }
                    _es.etype = OBJECT;
                }
                break;
            case _SC('['):
                if(_lex.data->prevtoken == _SC('\n')) Error(_SC("cannot brake deref/or comma needed after [exp]=exp slot declaration"));
                Lex();
                Expression();
                Expect(_SC(']'));
                pos = -1;
                if(_es.etype==BASE)
                {
                    Emit2ArgsOP(_OP_GET);
                    pos = _fs->TopTarget();
                    _es.etype = EXPR;
                    _es.epos   = pos;
                }
                else
                {
                    if(NeedGet())
                    {
                        Emit2ArgsOP(_OP_GET);
                    }
                    _es.etype = OBJECT;
                }
                break;
            case TK_MINUSMINUS:
            case TK_PLUSPLUS:
            {
                if(IsEndOfStatement()) return;
                SQInteger diff = (_token==TK_MINUSMINUS) ? -1 : 1;
                Lex();
                switch(_es.etype)
                {
                case EXPR:
                    Error(_SC("can't '++' or '--' an expression"));
                    break;
                case OBJECT:
                case BASE:
                    if(_es.donot_get == true)
                    {
                        Error(_SC("can't '++' or '--' an expression"));    //mmh dor this make sense?
                        break;
                    }
                    Emit2ArgsOP(_OP_PINC, diff);
                    break;
                case LOCAL:
                {
                    SQInteger src = _fs->PopTarget();
                    _fs->AddInstruction(_OP_PINCL, _fs->PushTarget(), src, 0, diff);
                }
                break;
                case OUTER:
                {
                    SQInteger tmp1 = _fs->PushTarget();
                    SQInteger tmp2 = _fs->PushTarget();
                    _fs->AddInstruction(_OP_GETOUTER, tmp2, _es.epos);
                    _fs->AddInstruction(_OP_PINCL,    tmp1, tmp2, 0, diff);
                    _fs->AddInstruction(_OP_SETOUTER, tmp2, _es.epos, tmp2);
                    _fs->PopTarget();
                }
                }
            }
            return;
            break;
            case _SC('('):
                switch(_es.etype)
                {
                case OBJECT:
                {
                    SQInteger key     = _fs->PopTarget();  /* location of the key */
                    SQInteger table   = _fs->PopTarget();  /* location of the object */
                    SQInteger closure = _fs->PushTarget(); /* location for the closure */
                    SQInteger ttarget = _fs->PushTarget(); /* location for 'this' pointer */
                    _fs->AddInstruction(_OP_PREPCALL, closure, key, table, ttarget);
                }
                break;
                case BASE:
                    //Emit2ArgsOP(_OP_GET);
                    _fs->AddInstruction(_OP_MOVE, _fs->PushTarget(), 0);
                    break;
                case OUTER:
                    _fs->AddInstruction(_OP_GETOUTER, _fs->PushTarget(), _es.epos);
                    _fs->AddInstruction(_OP_MOVE,     _fs->PushTarget(), 0);
                    break;
                default:
                    _fs->AddInstruction(_OP_MOVE, _fs->PushTarget(), 0);
                }
                _es.etype = EXPR;
                Lex();
                FunctionCallArgs();
                break;

            case TK_AS:
            {
                Lex();
                ExpectTypeToken(); //ignore for now
            }

            default:
                return;
            }
        }
    }
    SQInteger Factor()
    {
        _es.etype = EXPR;
        switch(_token)
        {
        case TK_STRING_LITERAL:
        {
            SQInteger lhtk = _lex.LookaheadLex();
            if(lhtk == TK_STRING_LITERAL) //C/C++ two consecutive strings
            {
                SQCharBuf buf;
                //save the current string
                buf.append(_lex.data->svalue,_lex.data->longstr.size()-1);
                while(lhtk == TK_STRING_LITERAL)
                {
                    Lex(); //get lookahead token
                    buf.append(_lex.data->svalue,_lex.data->longstr.size()-1);
                    lhtk = _lex.LookaheadLex();
                }
                _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(
                                        _fs->CreateString(buf.data(),buf.size())));
            }
            else
            {
                _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(
                                        _fs->CreateString(_lex.data->svalue,_lex.data->longstr.size()-1)));
            }
            Lex();
        }
        break;
        case TK_BASE:
            Lex();
            _fs->AddInstruction(_OP_GETBASE, _fs->PushTarget());
            _es.etype  = BASE;
            _es.epos   = _fs->TopTarget();
            return (_es.epos);
            break;
        case TK_IDENTIFIER:
        case TK_CONSTRUCTOR:
        case TK_DESTRUCTOR:
        case TK_THIS:
        {
            SQObject id;
            SQObject constant;

            switch(_token)
            {
            case TK_IDENTIFIER:
                id = _fs->CreateString(_lex.data->svalue);
                break;
            case TK_THIS:
                id = _fs->CreateString(_SC("this"), 4);
                break;
            case TK_CONSTRUCTOR:
                id = _fs->CreateString(_SC("constructor"), 11);
                break;
            case TK_DESTRUCTOR:
                id = _fs->CreateString(_SC("destructor"), 10);
                break;
            }

            SQInteger pos = -1;
            Lex();
            if((pos = _fs->GetLocalVariable(id)) != -1)
            {
                /* Handle a local variable (includes 'this') */
                _fs->PushTarget(pos);
                _es.etype  = LOCAL;
                _es.epos   = pos;
            }

            else if((pos = _fs->GetOuterVariable(id)) != -1)
            {
                /* Handle a free var */
                if(NeedGet())
                {
                    _es.epos  = _fs->PushTarget();
                    _fs->AddInstruction(_OP_GETOUTER, _es.epos, pos);
                    /* _es.etype = EXPR; already default value */
                }
                else
                {
                    _es.etype = OUTER;
                    _es.epos  = pos;
                }
            }

            else if(IsConstant(id, constant))
            {
                /* Handle named constant */
                SQObjectPtr constval;
                SQObject    constid;
                if(sq_type(constant) == OT_TABLE)
                {
                    Expect('.');
                    constid = Expect(TK_IDENTIFIER);
                    if(!_table(constant)->Get(constid, constval))
                    {
                        constval.Null();
                        Error(_SC("invalid constant [%s.%s]"), _stringval(id), _stringval(constid));
                    }
                }
                else
                {
                    constval = constant;
                }
                _es.epos = _fs->PushTarget();

                /* generate direct or literal function depending on size */
                SQObjectType ctype = sq_type(constval);
                switch(ctype)
                {
                case OT_INTEGER:
                    EmitLoadConstInt(_integer(constval),_es.epos);
                    break;
                case OT_FLOAT:
                    EmitLoadConstFloat(_float(constval),_es.epos);
                    break;
                case OT_BOOL:
                    _fs->AddInstruction(_OP_LOADBOOL, _es.epos, _integer(constval));
                    break;
                default:
                    _fs->AddInstruction(_OP_LOAD,_es.epos,_fs->GetConstant(constval));
                    break;
                }
                _es.etype = EXPR;
            }
            else
            {
                /* Handle a non-local variable, aka a field. Push the 'this' pointer on
                * the virtual stack (always found in offset 0, so no instruction needs to
                * be generated), and push the key next. Generate an _OP_LOAD instruction
                * for the latter. If we are not using the variable as a dref expr, generate
                * the _OP_GET instruction.
                */
                if(CheckNameIsType(id)) EatTemplateInitialization();
                _fs->PushTarget(0);
                _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(id));
                if(NeedGet())
                {
                    Emit2ArgsOP(_OP_GET);
                }
                _es.etype = OBJECT;
            }
            return _es.epos;
        }
        break;
        case TK_DOUBLE_COLON:  // "::"
            _fs->AddInstruction(_OP_LOADROOT, _fs->PushTarget());
            _es.etype = OBJECT;
            _token = _SC('.'); /* hack: drop into PrefixExpr, case '.'*/
            _es.epos = -1;
            return _es.epos;
            break;
        case TK_NULL:
            _fs->AddInstruction(_OP_LOADNULLS, _fs->PushTarget(),1);
            Lex();
            break;
        CASE_TK_INTEGER:
            EmitLoadConstInt(_lex.data->nvalue,-1);
            Lex();
            break;
        case TK_FLOAT:
            EmitLoadConstFloat(_lex.data->fvalue,-1);
            Lex();
            break;
        case TK_TRUE:
        case TK_FALSE:
            _fs->AddInstruction(_OP_LOADBOOL, _fs->PushTarget(),_token == TK_TRUE?1:0);
            Lex();
            break;
        case _SC('['):
        {
            _fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(),0,0,NOT_ARRAY);
            SQInteger apos = _fs->GetCurrentPos(),key = 0;
            Lex();
            while(_token != _SC(']'))
            {
                Expression();
                if(_token == _SC(',')) Lex();
                SQInteger val = _fs->PopTarget();
                SQInteger array = _fs->TopTarget();
                _fs->AddInstruction(_OP_APPENDARRAY, array, val, AAT_STACK);
                key++;
            }
            _fs->SetIntructionParam(apos, 1, key);
            Lex();
        }
        break;
        case _SC('{'):
            _fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(),0,NOT_TABLE);
            Lex();
            ParseTableOrClass(_SC(','),_SC('}'));
            break;
        case TK_FUNCTION:
            FunctionExp(_token);
            break;
        case _SC('@'):
            FunctionExp(_token,true);
            break;
        case TK_STRUCT:
        case TK_CLASS:
            Lex();
            ClassExp(NULL);
            break;
        case _SC('-'):
            Lex();
            switch(_token)
            {
            CASE_TK_INTEGER:
                EmitLoadConstInt(-_lex.data->nvalue,-1);
                Lex();
                break;
            case TK_FLOAT:
                EmitLoadConstFloat(-_lex.data->fvalue,-1);
                Lex();
                break;
            default:
                UnaryOP(_OP_NEG);
            }
            break;
        case _SC('!'):
            Lex();
            UnaryOP(_OP_NOT);
            break;
        case _SC('~'):
            Lex();
            bool isInteger;
            switch(_token)
            {
            CASE_TK_INTEGER:
                isInteger = true;
            default:
                isInteger = false;
            }
            if(isInteger)
            {
                EmitLoadConstInt(~_lex.data->nvalue,-1);
                Lex();
                break;
            }
            UnaryOP(_OP_BWNOT);
            break;
        case TK_TYPEOF :
            Lex() ;
            UnaryOP(_OP_TYPEOF);
            break;
        case TK_RESUME :
            Lex();
            UnaryOP(_OP_RESUME);
            break;
        case TK_CLONE :
            Lex();
            UnaryOP(_OP_CLONE);
            break;
        case TK_RAWCALL:
            Lex();
            Expect('(');
            FunctionCallArgs(true);
            break;
        case TK_MINUSMINUS :
        case TK_PLUSPLUS :
            PrefixIncDec(_token);
            break;
        case TK_DELETE :
            DeleteExpr();
            break;
        case _SC('('):
            Lex();
            if(_token == TK_IDENTIFIER)
            {
                //check C/C++ cast
                SQObject id = _fs->CreateString(_lex.data->svalue);
                if(CheckTypeName(id)) //C/C++ type declaration;
                {
                    SQInteger lhtk = _lex.LookaheadLex();
                    if(lhtk == _SC(')'))
                    {
                        Lex(); //eat TK_IDENTIFIER
                        Lex(); //eat ')'
                        CommaExpr();
                        break;
                    }
                }
            }
            CommaExpr();
            Expect(_SC(')'));
            break;
        case TK___LINE__:
            EmitLoadConstInt(_lex.data->currentline,-1);
            Lex();
            break;
        case TK___FILE__:
            _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(_sourcename));
            Lex();
            break;
        case TK___FUNCTION__:
            _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(_fs->_name));
            Lex();
            break;
        case TK_IGNORE:
            //Warning("Keyword ignored \"%s\" at line %d:%d\n", _lex.Tok2Str(_token),
            //        _lex.data->currentline, _lex.data->currentcolumn);
            Lex();
            Factor();
            break;
        default:
            Error(_SC("expression expected"));
        }
        return -1;
    }
    void EmitLoadConstInt(SQInteger value,SQInteger target)
    {
        if(target < 0)
        {
            target = _fs->PushTarget();
        }
        //with the line bellow we get wrong result for -1
        //if(value <= INT_MAX && value > INT_MIN) { //does it fit in 32 bits?
        if((value & (~((SQInteger)0xFFFFFFFF))) == 0)   //does it fit in 32 bits?
        {
            _fs->AddInstruction(_OP_LOADINT, target,value);
        }
        else
        {
            _fs->AddInstruction(_OP_LOAD, target, _fs->GetNumericConstant(value));
        }
    }
    void EmitLoadConstFloat(SQFloat value,SQInteger target)
    {
        if(target < 0)
        {
            target = _fs->PushTarget();
        }
        if(sizeof(SQFloat) == sizeof(SQInt32))
        {
            _fs->AddInstruction(_OP_LOADFLOAT, target,*((SQInt32 *)&value));
        }
        else
        {
            _fs->AddInstruction(_OP_LOAD, target, _fs->GetNumericConstant(value));
        }
    }
    void UnaryOP(SQOpcode op)
    {
        PrefixedExpr();
        SQInteger src = _fs->PopTarget();
        _fs->AddInstruction(op, _fs->PushTarget(), src);
    }
    bool NeedGet()
    {
        switch(_token)
        {
        case _SC('='):
        case _SC('('):
        case TK_NEWSLOT:
        case TK_MODEQ:
        case TK_MULEQ:
        case TK_DIVEQ:
        case TK_MINUSEQ:
        case TK_PLUSEQ:
        case TK_BIT_XOR_EQ:
        case TK_BIT_AND_EQ:
        case TK_BIT_OR_EQ:
        case TK_BIT_SHIFT_LEFT_EQ:
        case TK_BIT_SHIFT_RIGHT_EQ:
            return false;
        case TK_PLUSPLUS:
        case TK_MINUSMINUS:
            if (!IsEndOfStatement())
            {
                return false;
            }
            break;
        }
        return (!_es.donot_get || ( _es.donot_get && (_token == _SC('.') || _token == _SC('['))));
    }
    void FunctionCallArgs(bool rawcall = false)
    {
        SQInteger nargs = 1;//this
        while(_token != _SC(')'))
        {
            Expression();
            MoveIfCurrentTargetIsLocal();
            nargs++;
            if(_token == _SC(','))
            {
                Lex();
                if(_token == ')') Error(_SC("expression expected, found ')'"));
            }
        }
        Lex();
        if (rawcall)
        {
            if (nargs < 3) Error(_SC("rawcall requires at least 2 parameters (callee and this)"));
            nargs -= 2; //removes callee and this from count
        }
        for(SQInteger i = 0; i < (nargs - 1); i++) _fs->PopTarget();
        SQInteger stackbase = _fs->PopTarget();
        SQInteger closure = _fs->PopTarget();
        SQInteger target = _fs->PushTarget();
        assert(target >= -1);
        assert(target < 255);
        _fs->AddInstruction(_OP_CALL, target, closure, stackbase, nargs);
    }
    void AddClassMemberExists(SQObjectPtr &member_names, SQObject &name)
    {
        SQObjectPtr oname = name, otrue = true;
        _table(member_names)->NewSlot(oname, otrue);
    }
    void CheckClassMemberExists(SQObjectPtr &member_names, SQObject &name, bool addIfNotExists=true)
    {
        if(_table(member_names)->Exists(name))
        {
            Error(_SC("class already has a member named: %s"), _stringval(name));
        }
        if(addIfNotExists) AddClassMemberExists(member_names, name);
    }
    void CheckClassMemberExists(SQObjectPtr &member_names, const SQChar *name)
    {
        SQObject oname = _fs->CreateString(name);
        CheckClassMemberExists(member_names, oname);
    }
    void ParseTableOrClass(SQInteger separator,SQInteger terminator, SQObjectPtr *class_name=NULL)
    {
        SQObjectPtr member_names;
        SQInteger saved_tok, tpos = _fs->GetCurrentPos(),nkeys = 0;
        SQObject type_name, obj_id;
        bool isClass = separator == ';'; //hack recognizes a table/class from the separator
        if(isClass)
        {
            member_names = SQTable::Create(_ss(_vm),0);
        }
        int addClassMember = 0;
        while(_token != terminator)
        {
            bool hasattrs = false;
            bool isstatic = false;
            bool cppDestructor = false;
            //bool isvirtual = false;
            //bool isprivate = false;
            const SQChar *membertypename = 0;
            SQInteger member_type_token = 0;
            //check if is an attribute
            if(isClass)
            {
                if(_token == TK_ATTR_OPEN)
                {
                    _fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(),0,NOT_TABLE);
                    Lex();
                    ParseTableOrClass(',',TK_ATTR_CLOSE);
                    hasattrs = true;
                }
                if(_token == TK_STATIC)
                {
                    isstatic = true;
                    Lex();
                }
                if(_token == TK_VIRTUAL)
                {
                    //isvirtual = true;
                    Lex();
                }
                else if(_token == TK_PUBLIC)
                {
                    Lex();
                }
                else if(_token == TK_PRIVATE)
                {
                    //isprivate = true;
                    Lex();
                }
                else if(_token == TK_PROTECTED)
                {
                    //isprivate = true;
                    Lex();
                }
                else if(_token == TK_INLINE)
                {
                    Lex();
                }
                else if(_token == TK_CONST)
                {
                    Lex();
                }
                else if(_token == TK_TEMPLATE)
                {
                    TemplateStatement();
                }
            }
member_has_type:
            switch(_token)
            {
            case TK_FUNCTION:
            case TK_CONSTRUCTOR:
            case TK_DESTRUCTOR:
            {
                saved_tok = _token;
                Lex();
                obj_id = saved_tok == TK_FUNCTION ? Expect(TK_IDENTIFIER) :
                         _fs->CreateString(saved_tok == TK_CONSTRUCTOR ? _SC("constructor") : _SC("destructor"));
                if(sq_type(member_names) == OT_TABLE) CheckClassMemberExists(member_names, obj_id);
                else Error(_SC("unexpected error in class declaration"));
                Expect(_SC('('));
function_params_decl:
                _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(obj_id));
                CreateFunction(obj_id, eFunctionType_member);
                _fs->AddInstruction(_OP_CLOSURE, _fs->PushTarget(), _fs->_functions.size() - 1, 0);
            }
            break;
            case _SC('['):
                Lex();
                CommaExpr();
                Expect(_SC(']'));
                Expect(_SC('='));
                Expression();
                break;

            case TK_STRING_LITERAL: //JSON
                if(isClass)
                {
                    Error(_SC("unexpected string literal in class declaration"));
                }
            case TK_IDENTIFIER:  //JSON
            {
                obj_id = GetTokenObject(_token);
                SQInteger next_token = _SC('=');
                if(isClass)
                {
                    CheckClassMemberExists(member_names, obj_id, false);
                    addClassMember = 0;
                    EatTemplateInitialization();
                    switch(_token)
                    {
                    case _SC('('): //C/C++ style function declaration
                        Lex();
                        if(class_name)
                        {
                            //printf("ClassMember %d : %s : %s\n", (int)cppDestructor, _stringval(*class_name), _stringval(obj_id));
                            if(memcmp(_stringval(*class_name), _stringval(obj_id), _string(*class_name)->_len) == 0)
                            {
                                //C++ style constructor/destructor declaration
                                obj_id = _fs->CreateString(cppDestructor ? _SC("destructor") : _SC("constructor"));
                                cppDestructor = false;
                            }
                        }
                        AddClassMemberExists(member_names, obj_id);
                        goto function_params_decl;
                        break;
                    case _SC(':'): //typescript field with type annotation
                        if(membertypename)
                        {
                            Error(_SC("member type already declared before %s"), _stringval(obj_id));
                        }
                        Lex();
                        type_name = ExpectTypeToken();
                        addClassMember = 1;
                        break;
                    case _SC(','):
                        ++addClassMember;
                    case _SC(';'): //member variable declaration without explicit initialization
                        Lex();
                        ++addClassMember;
                        break;
                    case TK_IDENTIFIER: //if 2 identifier found the first should be a type
                        if(CheckNameIsType(obj_id)) //Struct/Class/Typedef names
                        {
                            membertypename = _stringval(obj_id);
                            goto member_has_type;
                        }

                    }
                    if(addClassMember)
                    {
                        AddClassMemberExists(member_names, obj_id);
                        if(_token != _SC('='))
                        {
                            _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(obj_id));
                            if(member_type_token) GetVarTypeDeclaration(member_type_token, _fs->PushTarget());
                            else _fs->AddInstruction(_OP_LOADNULLS, _fs->PushTarget(), 1);
                            break;
                        }
                    }
                }
                if(_token == _SC(':'))
                {
                    next_token = _token;
                }
                Expect(next_token);
                _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(obj_id));
                Expression();
                break;
            }
            CASE_TK_LOCAL_TYPES: //class member variables;
            case TK_LOCAL:
                if(isClass)
                {
                    member_type_token = _token;
                    membertypename = _lex.GetTokenName(_token);
                    Lex();
                    goto member_has_type;
                }
            case _SC('~'): //C++ style destructor declaration
                if(isClass)
                {
                    cppDestructor = true;
                    Lex();
                    goto member_has_type;
                }
            case TK_CONSTEXPR:
                if(isClass)
                {
                    Lex();
                    continue;
                }
            //else fallthrough
            default :
                ErrorIfNotToken(TK_IDENTIFIER);
            }

            if(_token == separator) Lex();//optional comma/semicolon
            nkeys++;
            SQInteger val = _fs->PopTarget();
            SQInteger key = _fs->PopTarget();
            SQInteger attrs = hasattrs ? _fs->PopTarget():-1;
            assert((hasattrs && (attrs == key-1)) || !hasattrs);
            (void)attrs; // UNUSED
            unsigned char flags = (hasattrs?NEW_SLOT_ATTRIBUTES_FLAG:0)|(isstatic?NEW_SLOT_STATIC_FLAG:0);
            SQInteger table = _fs->TopTarget(); //<<BECAUSE OF THIS NO COMMON EMIT FUNC IS POSSIBLE
            if(isClass)
            {
                _fs->AddInstruction(_OP_NEWSLOTA, flags, table, key, val); //this for classes only as it invokes _newmember
                if(addClassMember > 1)
                {
                    addClassMember = 0;
                    goto member_has_type;
                }
            }
            else
            {
                _fs->AddInstruction(_OP_NEWSLOT, 0xFF, table, key, val);
            }
        }
        if(separator == _SC(',')) //hack recognizes a table from the separator
            _fs->SetIntructionParam(tpos, 1, nkeys);
        Lex();
    }
    void ExternDeclStatement()
    {
        _is_parsing_extern = true;
        Lex();
        LocalDeclStatement();
        _is_parsing_extern = false;
    }
    void EatTemplateInitialization()
    {
        //C++/Java template instantiation
        if(_token == _SC('<'))
        {
            Lex();
            ExpectTypeToken();
            if(_token != _SC('>')) Error(_SC("template instantiation declaration expected <TYPE>"));
            Lex();
        }
    }
    SQInteger GetVarTypeDeclaration(SQInteger declType, SQInteger dest=-1)
    {
        bool doAddInstruction = (dest >= 0);
        switch(declType)
        {
        CASE_TK_LOCAL_CHAR_TYPES:
            if(doAddInstruction) _fs->AddInstruction(_OP_LOADNULLS, dest,1);
            declType = _VAR_STRING;
            break;

        case TK_LOCAL_BOOL_T:
            //default value false
            if(doAddInstruction) _fs->AddInstruction(_OP_LOADBOOL, dest,0);
            declType = _VAR_BOOL;
            break;

        case TK_LOCAL_TABLE_T:
            if(doAddInstruction) _fs->AddInstruction(_OP_LOADNULLS, dest,1);
            declType = _VAR_TABLE;
            break;
        case TK_LOCAL_ARRAY_T:
            if(doAddInstruction) _fs->AddInstruction(_OP_LOADNULLS, dest,1);
            declType = _VAR_ARRAY;
            break;

        CASE_TK_LOCAL_INT_TYPES:
            //default value 0
            if(doAddInstruction) _fs->AddInstruction(_OP_LOADINT, dest,0);
            declType = _VAR_INTEGER;
            break;
        CASE_TK_LOCAL_FLOAT_TYPES:
        case TK_LOCAL_NUMBER_T: //start numbers as floats
            //default value 0.0
            if(doAddInstruction)
            {
                SQFloat value = 0.0;
                EmitLoadConstFloat(value, dest);
            }
            declType = _VAR_FLOAT;
            break;
        //case TK_LOCAL:
        default:
            //default value null
            if(doAddInstruction) _fs->AddInstruction(_OP_LOADNULLS, dest,1);
            declType = _VAR_ANY;
        }
        return declType;
    }
#define CHECK_REF_DECLARATION(tk) if(tk == _SC('&')){is_reference_declaration = true;Lex();}
    void LocalDeclStatement()
    {
        SQObject varname;
        SQObject type_name = GetTokenObject(_token, false);
        //SQChar *param_type_name;
        bool is_void_declaration = _token == TK_VOID;
        bool is_const_declaration = _token == TK_CONST;
        bool is_reference_declaration = false;
        //bool is_instance_declaration = _token == TK_IDENTIFIER;
        SQInteger declType = _token;
        Lex();
        EatTemplateInitialization();
        if( _token == TK_FUNCTION)
        {
            Lex();
            varname = Expect(TK_IDENTIFIER);
            CheckLocalNameScope(varname, _scope.nested);
            Expect(_SC('('));
function_params_decl:
#if 1
            //doing this way works but prevents garbage collection when doing multiple reloads on the same vm
            //the following is an attempt to allow local declared functions be called recursivelly
            SQInteger old_pos = _fs->GetCurrentPos(); //save current instructions position
            _fs->PushLocalVariable(varname, _scope.nested, _VAR_CLOSURE); //add function name to find it as outer var if needed
            if(sq_type(type_name) == OT_STRING) _fs->AddParameterTypeName(type_name);
            //-1 to compensate default parameters when relocating
            CreateFunction(varname, eFunctionType_local, -1, declType);
            if(_is_parsing_extern)
            {
                if(_token == _SC(';')) //to parse thinscript
                {
                    //Expect(_SC(';'));
                    CheckExternName(varname, true);
                    return;
                }
                else _is_parsing_extern = false;
            }
            _fs->AddInstruction(_OP_CLOSURE, _fs->PushTarget(), _fs->_functions.size() - 1, 0);
            //rellocate any stack operation (default parameters & _OP_Closure)
            for(int i=old_pos+1, curr_pos = _fs->GetCurrentPos(); i <= curr_pos; ++i)
            {
                SQInstruction & inst = _fs->GetInstruction(i);
                _fs->SetIntructionParam(i, 0, inst._arg0 -1);
            }
            _fs->PopTarget();
#else
            CreateFunction(varname,eFunctionType::local);
            _fs->AddInstruction(_OP_CLOSURE, _fs->PushTarget(), _fs->_functions.size() - 1, 0);
            _fs->PopTarget();
            _fs->PushLocalVariable(varname, _scope.nested, _VAR_CLOSURE);
#endif
            return;
        }

        do
        {
            //param_type_name = NULL;
            CHECK_REF_DECLARATION(_token);
            varname = Expect(TK_IDENTIFIER);
            if(CheckNameIsType(varname))
            {
                //ignore custom types for now
                //param_type_name = _stringval(varname);
                declType = _VAR_ANY;
                CHECK_REF_DECLARATION(_token);
                varname = Expect(TK_IDENTIFIER);
            }

            CheckLocalNameScope(varname, _scope.nested);
            EatTemplateInitialization();
            if(_token == _SC('('))
            {
                declType = GetVarTypeDeclaration(declType);
                //C/C++ style function declaration
                Lex();
                goto function_params_decl;
            }
            else if(_token == _SC(':'))
            {
                //type specifier like typescript
                Lex();
                declType = _token;
                type_name = ExpectTypeToken(); //ignore for now
            }
            if(is_void_declaration)
            {
                Error(_SC("void type is invalid here"));
            }
            else if(_token == _SC('='))
            {
                if(_is_parsing_extern)
                {
                    Error(_SC("can not make assignment in external declarations"));
                }
                Lex();
                Expression();
                SQInteger src = _fs->PopTarget();
                SQInteger dest = _fs->PushTarget();
                if(dest != src) _fs->AddInstruction(_OP_MOVE, dest, src);
                declType = GetVarTypeDeclaration(declType);
            }
            else if(is_const_declaration || is_reference_declaration)
                Error(_SC("const/reference '%s' need an initializer"), _stringval(varname));
            else
            {
                //check_param_type:
                if(!_is_parsing_extern)
                {
                    SQInteger dest = _fs->PushTarget();
                    declType = GetVarTypeDeclaration(declType, dest);
                }
            }
            if(_is_parsing_extern)
            {
                if(CheckExternName(varname, true))
                {
                    Error(_SC("extern %s already declared"), varname);
                }
            }
            else
            {
                _fs->PopTarget();
                _fs->PushLocalVariable(varname, _scope.nested, (is_const_declaration ? _VAR_CONST : declType)
                                       | (is_reference_declaration ? _VAR_REFERENCE : 0));
                if(sq_type(type_name) == OT_STRING) _fs->AddParameterTypeName(type_name);
            }
            if(_token == _SC(',')) Lex();
            else break;
        }
        while(1);
    }
    void IfBlock()
    {
        if (_token == _SC('{'))
        {
            BEGIN_SCOPE();
            Lex();
            Statements();
            Expect(_SC('}'));
            if (true)
            {
                END_SCOPE();
            }
            else
            {
                END_SCOPE_NO_CLOSE();
            }
        }
        else
        {
            //BEGIN_SCOPE();
            Statement();
            if (_lex.data->prevtoken != _SC('}') && _lex.data->prevtoken != _SC(';')) OptionalSemicolon();
            //END_SCOPE();
        }
    }
    void IfStatement()
    {
        SQInteger jmppos;
        bool haselse = false;
        Lex();
        Expect(_SC('('));
        CommaExpr(true);
        Expect(_SC(')'));
        _fs->AddInstruction(_OP_JZ, _fs->PopTarget());
        SQInteger jnepos = _fs->GetCurrentPos();

        IfBlock();
        //there is a situation where the if statement has a statement enclosed by {}
        //and after the closing "}" there is no newline or semicolom
        //it's a valid construction but the compiler was complaining about it
        //for now added "&& _token != TK_IDENTIFIER" to the check after "Statement()" call
        /*
        local color = "blue";
        if(color == "yellow"){
        	print(color);
        } print("Waht color is it ?");
        */

        SQInteger endifblock = _fs->GetCurrentPos();
        if(_token == TK_ELSE)
        {
            haselse = true;
            BEGIN_SCOPE();
            _fs->AddInstruction(_OP_JMP);
            jmppos = _fs->GetCurrentPos();
            Lex();
            Statement();
            if(_token != TK_IDENTIFIER) OptionalSemicolon();
            END_SCOPE();
            _fs->SetIntructionParam(jmppos, 1, _fs->GetCurrentPos() - jmppos);
        }
        _fs->SetIntructionParam(jnepos, 1, endifblock - jnepos + (haselse?1:0));
    }
    void WhileStatement()
    {
        SQInteger jzpos, jmppos;
        jmppos = _fs->GetCurrentPos();
        Lex();
        Expect(_SC('('));
        CommaExpr(true);
        Expect(_SC(')'));

        BEGIN_BREAKBLE_BLOCK();
        _fs->AddInstruction(_OP_JZ, _fs->PopTarget());
        jzpos = _fs->GetCurrentPos();
        BEGIN_SCOPE();

        Statement();

        END_SCOPE();
        _fs->AddInstruction(_OP_JMP, 0, jmppos - _fs->GetCurrentPos() - 1);
        _fs->SetIntructionParam(jzpos, 1, _fs->GetCurrentPos() - jzpos);

        END_BREAKBLE_BLOCK(jmppos);
    }
    void DoWhileStatement()
    {
        Lex();
        SQInteger jmptrg = _fs->GetCurrentPos();
        BEGIN_BREAKBLE_BLOCK()
        BEGIN_SCOPE();
        Statement();
        END_SCOPE();
        //fix proposed by frosch to correct line number info in stack dumps
        _fs->AddLineInfos(_lex.data->currentline, _lineinfo, true);
        Expect(TK_WHILE);
        SQInteger continuetrg = _fs->GetCurrentPos();
        Expect(_SC('('));
        CommaExpr(true);
        Expect(_SC(')'));
        _fs->AddInstruction(_OP_JZ, _fs->PopTarget(), 1);
        _fs->AddInstruction(_OP_JMP, 0, jmptrg - _fs->GetCurrentPos() - 1);
        END_BREAKBLE_BLOCK(continuetrg);
    }
    void ForStatement()
    {
        Lex();
        BEGIN_SCOPE();
        Expect(_SC('('));
        switch(_token)
        {
        case TK_IDENTIFIER:
        {
            SQObject id = _fs->CreateString(_lex.data->svalue);
            if(!CheckTypeName(id))
            {
                SQInteger lhtk = _lex.LookaheadLex();
                if(lhtk == TK_IDENTIFIER) Error(_SC("unknown type specifier"));
                goto lbl_commaexpr;//not a C/C++ type declaration;
            }
        }
            //ignore custom types for now
            //fallthrough as local declaration
        CASE_TK_LOCAL_TYPES:
        case TK_LOCAL:
            LocalDeclStatement();
            break;
        default:
lbl_commaexpr:
            if(_token != _SC(';'))
            {
                CommaExpr();
                _fs->PopTarget();
            }
        }

        Expect(_SC(';'));
        _fs->SnoozeOpt();
        SQInteger jmppos = _fs->GetCurrentPos();
        SQInteger jzpos = -1;
        if(_token != _SC(';'))
        {
            CommaExpr();
            _fs->AddInstruction(_OP_JZ, _fs->PopTarget());
            jzpos = _fs->GetCurrentPos();
        }
        Expect(_SC(';'));
        _fs->SnoozeOpt();
        SQInteger expstart = _fs->GetCurrentPos() + 1;
        if(_token != _SC(')'))
        {
            CommaExpr();
            _fs->PopTarget();
        }
        Expect(_SC(')'));
        _fs->SnoozeOpt();
        SQInteger expend = _fs->GetCurrentPos();
        SQInteger expsize = (expend - expstart) + 1;
        SQInstructionVec exp;
        if(expsize > 0)
        {
            for(SQInteger i = 0; i < expsize; i++)
                exp.push_back(_fs->GetInstruction(expstart + i));
            _fs->PopInstructions(expsize);
        }
        BEGIN_BREAKBLE_BLOCK()
        Statement();
        SQInteger continuetrg = _fs->GetCurrentPos();
        if(expsize > 0)
        {
            for(SQInteger i = 0; i < expsize; i++)
                _fs->AddInstruction(exp[i]);
        }
        _fs->AddInstruction(_OP_JMP, 0, jmppos - _fs->GetCurrentPos() - 1, 0);
        if(jzpos>  0) _fs->SetIntructionParam(jzpos, 1, _fs->GetCurrentPos() - jzpos);

        END_BREAKBLE_BLOCK(continuetrg);
        END_SCOPE();
    }
    void ForEachStatement()
    {
        SQObject idxname, valname;
        Lex();
        Expect(_SC('('));
        valname = Expect(TK_IDENTIFIER);
        if(_token == _SC(','))
        {
            idxname = valname;
            Lex();
            valname = Expect(TK_IDENTIFIER);
        }
        else
        {
            idxname = _fs->CreateString(_SC("@INDEX@"));
        }
        Expect(TK_IN);

        //save the stack size
        BEGIN_SCOPE();
        //put the table in the stack(evaluate the table expression)
        Expression();
        Expect(_SC(')'));
        SQInteger container = _fs->TopTarget();
        //push the index local var
        SQInteger indexpos = _fs->PushLocalVariable(idxname, _scope.nested);
        _fs->AddInstruction(_OP_LOADNULLS, indexpos,1);
        //push the value local var
        SQInteger valuepos = _fs->PushLocalVariable(valname, _scope.nested);
        _fs->AddInstruction(_OP_LOADNULLS, valuepos,1);
        //push reference index
        SQInteger itrpos = _fs->PushLocalVariable(_fs->CreateString(_SC("@ITERATOR@")), _scope.nested); //use invalid id to make it inaccessible
        _fs->AddInstruction(_OP_LOADNULLS, itrpos,1);
        SQInteger jmppos = _fs->GetCurrentPos();
        _fs->AddInstruction(_OP_FOREACH, container, 0, indexpos);
        SQInteger foreachpos = _fs->GetCurrentPos();
        _fs->AddInstruction(_OP_POSTFOREACH, container, 0, indexpos);
        //generate the statement code
        BEGIN_BREAKBLE_BLOCK()
        Statement();
        _fs->AddInstruction(_OP_JMP, 0, jmppos - _fs->GetCurrentPos() - 1);
        _fs->SetIntructionParam(foreachpos, 1, _fs->GetCurrentPos() - foreachpos);
        _fs->SetIntructionParam(foreachpos + 1, 1, _fs->GetCurrentPos() - foreachpos);
        END_BREAKBLE_BLOCK(foreachpos - 1);
        //restore the local variable stack(remove index,val and ref idx)
        _fs->PopTarget();
        END_SCOPE();
    }
    void SwitchStatement()
    {
        Lex();
        Expect(_SC('('));
        CommaExpr(true);
        Expect(_SC(')'));
        Expect(_SC('{'));
        SQInteger expr = _fs->TopTarget();
        bool bfirst = true;
        SQInteger tonextcondjmp = -1;
        SQInteger skipcondjmp = -1;
        SQInteger __nbreaks__ = _fs->_unresolvedbreaks.size();
        _fs->_breaktargets.push_back(0);
        while(_token == TK_CASE)
        {
            if(!bfirst)
            {
                _fs->AddInstruction(_OP_JMP, 0, 0);
                skipcondjmp = _fs->GetCurrentPos();
                _fs->SetIntructionParam(tonextcondjmp, 1, _fs->GetCurrentPos() - tonextcondjmp);
            }
            //condition
            Lex();
            ExpressionScalar() /*Expression()*/;
            Expect(_SC(':'));
            SQInteger trg = _fs->PopTarget();
            SQInteger eqtarget = trg;
            bool local = _fs->IsLocal(trg);
            if(local)
            {
                eqtarget = _fs->PushTarget(); //we need to allocate a extra reg
            }
            _fs->AddInstruction(_OP_EQ, eqtarget, trg, expr);
            _fs->AddInstruction(_OP_JZ, eqtarget, 0);
            if(local)
            {
                _fs->PopTarget();
            }

            //end condition
            if(skipcondjmp != -1)
            {
                _fs->SetIntructionParam(skipcondjmp, 1, (_fs->GetCurrentPos() - skipcondjmp));
            }
            tonextcondjmp = _fs->GetCurrentPos();
            BEGIN_SCOPE();
            Statements();
            END_SCOPE();
            bfirst = false;
        }
        if(tonextcondjmp != -1)
            _fs->SetIntructionParam(tonextcondjmp, 1, _fs->GetCurrentPos() - tonextcondjmp);
        if(_token == TK_DEFAULT)
        {
            Lex();
            Expect(_SC(':'));
            BEGIN_SCOPE();
            Statements();
            END_SCOPE();
        }
        Expect(_SC('}'));
        _fs->PopTarget();
        __nbreaks__ = _fs->_unresolvedbreaks.size() - __nbreaks__;
        if(__nbreaks__ > 0)ResolveBreaks(_fs, __nbreaks__);
        _fs->_breaktargets.pop_back();
    }
    void TemplateStatement()
    {
        Lex(); //ignore for now
        Expect(_SC('<'));
        int nest_count = 1;
        while(_token > 0 && nest_count > 0)
        {
            Lex();
            switch(_token)
            {
            case _SC('>'):
                --nest_count;
                break;
            case _SC('<'):
                nest_count++;
            }
        }
        if(nest_count == 0) Lex(); //last '>' ignore for now
    }
    void FunctionStatement()
    {
        SQObject id;
        Lex();
        id = Expect(TK_IDENTIFIER);
        _fs->PushTarget(0);
        _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(id));
        if(_token == TK_DOUBLE_COLON) Emit2ArgsOP(_OP_GET);
        else CheckGlobalName(id, true);

        while(_token == TK_DOUBLE_COLON)
        {
            Lex();
            id = Expect(TK_IDENTIFIER);
            //todo check if class function already exists
            _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(id));
            if(_token == TK_DOUBLE_COLON) Emit2ArgsOP(_OP_GET);
        }
        Expect(_SC('('));
        CreateFunction(id, eFunctionType_global);
        _fs->AddInstruction(_OP_CLOSURE, _fs->PushTarget(), _fs->_functions.size() - 1, 0);
        EmitDerefOp(_OP_NEWSLOT);
        _fs->PopTarget();
    }
    void ClassStatement()
    {
        SQExpState es;
        SQObjectPtr class_name;
        Lex();
        if(_token == TK_IDENTIFIER)
        {
            class_name = SQString::Create(_ss(_vm), _lex.data->svalue);
            CheckGlobalName(class_name, true);
            CheckTypeName(class_name, true); //to allow C/C++ style instance declarations
        }
        es = _es;
        _es.donot_get = true;
        PrefixedExpr();
        if(_token == _SC(';')) //class forward declaration
        {
            //return;
        }
        if(_es.etype == EXPR)
        {
            Error(_SC("invalid class name"));
        }
        else if(_es.etype == OBJECT || _es.etype == BASE)
        {
            ClassExp(&class_name);
            EmitDerefOp(_OP_NEWSLOT);
            _fs->PopTarget();
        }
        else
        {
            Error(_SC("cannot create a class in a local with the syntax(class <local>)"));
        }
        _es = es;
    }
    SQObject ExpectScalar()
    {
        SQObject val;
        val._type = OT_NULL;
        val._unVal.nInteger = 0; //shut up GCC 4.x
        switch(_token)
        {
        CASE_TK_INTEGER:
            val._type = OT_INTEGER;
            val._unVal.nInteger = _lex.data->nvalue;
            break;
        case TK_FLOAT:
            val._type = OT_FLOAT;
            val._unVal.fFloat = _lex.data->fvalue;
            break;
        case TK_STRING_LITERAL:
            val = _fs->CreateString(_lex.data->svalue,_lex.data->longstr.size()-1);
            break;
        case TK_TRUE:
        case TK_FALSE:
            val._type = OT_BOOL;
            val._unVal.nInteger = _token == TK_TRUE ? 1 : 0;
            break;
        case '-':
            Lex();
            switch(_token)
            {
            CASE_TK_INTEGER:
                val._type = OT_INTEGER;
                val._unVal.nInteger = -_lex.data->nvalue;
                break;
            case TK_FLOAT:
                val._type = OT_FLOAT;
                val._unVal.fFloat = -_lex.data->fvalue;
                break;
            default:
                Error(_SC("scalar expected : integer, float"));
            }
            break;
        default:
            Error(_SC("scalar expected : integer, float or string"));
        }
        Lex();
        return val;
    }
    SQInteger ExpressionScalar()
    {
        SQInteger tk_type = _token;
        switch(_token)
        {
        CASE_TK_INTEGER:
            EmitLoadConstInt(_lex.data->nvalue,-1);
            break;
        case TK_FLOAT:
            EmitLoadConstFloat(_lex.data->fvalue,-1);
            break;
        case TK_STRING_LITERAL:
            _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(_fs->CreateString(_lex.data->svalue,_lex.data->longstr.size()-1)));
            break;
        case TK_IDENTIFIER:
        {
            SQObject id = _fs->CreateString(_lex.data->svalue);
            Lex();
            ExpressionConstant(id);
            return tk_type;
        }
        break;
        case '-':
            Lex();
            tk_type = _token;
            switch(_token)
            {
            CASE_TK_INTEGER:
                EmitLoadConstInt(-_lex.data->nvalue,-1);
                break;
            case TK_FLOAT:
                EmitLoadConstFloat(-_lex.data->fvalue,-1);
                break;
            default:
                Error(_SC("scalar expected : integer, float"));
            }
            break;
        default:
            goto error;
        }
        Lex();
        return tk_type;
error:
        Error(_SC("constant or scalar expected : integer, float or string"));
        return 0;
    }
    void EnumStatement()
    {
        Lex();
        SQObject id = Expect(TK_IDENTIFIER);
        Expect(_SC('{'));

        //CheckLocalNameScope(id, _scope.nested);
        SQObjectPtr strongid = id;
        CheckLocalNameScope(id, _scope.nested);
        TypesNewSlot(strongid,strongid);
        SQObject table = _fs->CreateTable();
        //_fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(),0,NOT_TABLE);
        SQInteger nval = 0;
        while(_token != _SC('}'))
        {
            SQObject key = Expect(TK_IDENTIFIER);
            SQObject val;
            if(_token == _SC('='))
            {
                Lex();
                val = ExpectScalar();
                nval = _integer(val)+1;
            }
            else
            {
                val._type = OT_INTEGER;
                val._unVal.nInteger = nval++;
            }
            //SQInteger table = _fs->TopTarget(); //<<BECAUSE OF THIS NO COMMON EMIT FUNC IS POSSIBLE
            //_fs->AddInstruction(_OP_NEWSLOT, 0xFF, table, key, val);

            _table(table)->NewSlot(SQObjectPtr(key),SQObjectPtr(val));
            if(_token == ',') Lex();
        }

        ConstsNewSlot(SQObjectPtr(strongid),SQObjectPtr(table));
        strongid.Null();
        Lex();
    }
    void TryCatchStatement()
    {
        SQObject exid;
        Lex();
        _fs->AddInstruction(_OP_PUSHTRAP,0,0);
        _fs->_traps++;
        if(_fs->_breaktargets.size()) _fs->_breaktargets.top()++;
        if(_fs->_continuetargets.size()) _fs->_continuetargets.top()++;
        SQInteger trappos = _fs->GetCurrentPos();
        {
            BEGIN_SCOPE();
            Statement();
            END_SCOPE();
        }
        _fs->_traps--;
        _fs->AddInstruction(_OP_POPTRAP, 1, 0);
        if(_fs->_breaktargets.size()) _fs->_breaktargets.top()--;
        if(_fs->_continuetargets.size()) _fs->_continuetargets.top()--;
        _fs->AddInstruction(_OP_JMP, 0, 0);
        SQInteger jmppos = _fs->GetCurrentPos();
        _fs->SetIntructionParam(trappos, 1, (_fs->GetCurrentPos() - trappos));
        Expect(TK_CATCH);
        Expect(_SC('('));
        exid = Expect(TK_IDENTIFIER);
        Expect(_SC(')'));
        {
            BEGIN_SCOPE();
            SQInteger ex_target = _fs->PushLocalVariable(exid, _scope.nested);
            _fs->SetIntructionParam(trappos, 0, ex_target);
            Statement();
            _fs->SetIntructionParams(jmppos, 0, (_fs->GetCurrentPos() - jmppos), 0);
            END_SCOPE();
        }
    }
    void FunctionExp(SQInteger ftype,bool lambda = false)
    {
        Lex();
        Expect(_SC('('));
        SQObjectPtr dummy;
        CreateFunction(dummy, lambda ? eFunctionType_lambda : eFunctionType_anonymous);
        _fs->AddInstruction(_OP_CLOSURE, _fs->PushTarget(), _fs->_functions.size() - 1, ftype == TK_FUNCTION?0:1);
    }
    void ClassExp(SQObjectPtr *class_name)
    {
        SQInteger base = -1;
        SQInteger attrs = -1;
        bool hasInheritance = false;
        switch(_token)
        {
        case _SC(':'): //C++ style class derivation
        case TK_EXTENDS: //squirrel style class derivation
            Lex();
            hasInheritance = true;
            switch(_token)
            {
            case TK_PRIVATE:
            case TK_PROTECTED:
            case TK_PUBLIC:
                Lex(); //ignore, accepted only to compile a subset of C++
            }
        }
        if(hasInheritance)
        {
            Expression();
            base = _fs->TopTarget();
        }
        if(_token == TK_ATTR_OPEN)
        {
            Lex();
            _fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(),0,NOT_TABLE);
            ParseTableOrClass(_SC(','),TK_ATTR_CLOSE);
            attrs = _fs->TopTarget();
        }
        Expect(_SC('{'));
        if(attrs != -1) _fs->PopTarget();
        if(base != -1) _fs->PopTarget();
        _fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(), base, attrs,NOT_CLASS);
        ParseTableOrClass(_SC(';'),_SC('}'), class_name);
    }
    void DeleteExpr()
    {
        SQExpState es;
        Lex();
        es = _es;
        _es.donot_get = true;
        PrefixedExpr();
        if(_es.etype==EXPR) Error(_SC("can't delete an expression"));
        if(_es.etype==OBJECT || _es.etype==BASE)
        {
            Emit2ArgsOP(_OP_DELETE);
        }
        else
        {
            Error(_SC("cannot delete an (outer) local"));
        }
        _es = es;
    }
    void PrefixIncDec(SQInteger token)
    {
        SQExpState  es;
        SQInteger diff = (token==TK_MINUSMINUS) ? -1 : 1;
        Lex();
        es = _es;
        _es.donot_get = true;
        PrefixedExpr();
        if(_es.etype==EXPR)
        {
            Error(_SC("can't '++' or '--' an expression"));
        }
        else if(_es.etype==OBJECT || _es.etype==BASE)
        {
            Emit2ArgsOP(_OP_INC, diff);
        }
        else if(_es.etype==LOCAL)
        {
            SQInteger src = _fs->TopTarget();
            _fs->AddInstruction(_OP_INCL, src, src, 0, diff);

        }
        else if(_es.etype==OUTER)
        {
            SQInteger tmp = _fs->PushTarget();
            _fs->AddInstruction(_OP_GETOUTER, tmp, _es.epos);
            _fs->AddInstruction(_OP_INCL,     tmp, tmp, 0, diff);
            _fs->AddInstruction(_OP_SETOUTER, tmp, _es.epos, tmp);
        }
        _es = es;
    }
    void CreateFunction(SQObject &name, int ftype, int stack_offset=0, int fdeclType=0)
    {
        SQFuncState *funcstate = _fs->PushChildState(_ss(_vm));
        funcstate->_name = name;
        SQObject paramname, type_name;
        funcstate->AddParameter(_fs->CreateString(_SC("this")), _scope.nested+1);
        funcstate->_sourcename = _sourcename;
        SQInteger defparams = 0;
        bool is_reference_declaration = 0;
        const SQChar *param_type_name = 0;
        bool isVoid = false;
        if(fdeclType)
        {
            funcstate->SetReturnTypeName(SQGetVarTypeName(fdeclType));
        }
        while(_token!=_SC(')'))
        {
            if(isVoid)
            {
                Error(_SC("void type is invalid here"));
            }
            is_reference_declaration = 0; //reset is_reference_declaration
            param_type_name = 0; //rest for each parameter
            if(_token == TK_VOID)
            {
                isVoid = true;
                Lex();
                continue;
            }
            if(_token == TK_VARPARAMS)
            {
                if(defparams > 0) Error(_SC("function with default parameters cannot have variable number of parameters"));
                funcstate->AddParameter(_fs->CreateString(_SC("vargv")), _scope.nested+1);
                funcstate->_varparams = true;
                Lex();
                if(_token != _SC(')')) Error(_SC("expected ')'"));
                break;
            }
            else
            {
                if(_token == TK_CONST) Lex(); //C/C++ const cualifiers for now
                switch(_token)
                {
                CASE_TK_LOCAL_TYPES: //accept C/C++ type parameter declarations
                    param_type_name = _lex.GetTokenName(_token);
                    Lex();
                }
                CHECK_REF_DECLARATION(_token);
                paramname = Expect(TK_IDENTIFIER);
                if(CheckNameIsType(paramname))
                {
                    //ignore custom types for now
                    param_type_name = _stringval(paramname);
                    CHECK_REF_DECLARATION(_token);
                    EatTemplateInitialization();
                    paramname = Expect(TK_IDENTIFIER);
                }
                funcstate->AddParameter(paramname, _scope.nested+1, is_reference_declaration ? _VAR_REFERENCE : _VAR_ANY);
                if(param_type_name)
                {
                    funcstate->AddParameterTypeName(param_type_name);
                }
                if(_token == _SC('='))
                {
                    if(is_reference_declaration) Error(_SC("parameter passed by reference can't have default value"));
                    Lex();
                    if(_token == _SC('[') || _token == _SC('{')) Error(_SC("default parameter with array/table values not supported"));
                    Expression();
                    //stack_offset to compensate for local functions been relocated to allow recursion
                    funcstate->AddDefaultParam(_fs->TopTarget()+stack_offset);
                    defparams++;
                }
                else if(_token == _SC(':'))
                {
                    //param type specifier like typescript
                    if(param_type_name)
                    {
                        Error(_SC("parameter type already declared before %s"), _string(paramname));
                    }
                    Lex();
                    type_name = ExpectTypeToken();
                    funcstate->AddParameterTypeName(type_name);
                    //printf("%d %s\n", __LINE__, _stringval(type_name));
                }
                else
                {
                    if(defparams > 0) Error(_SC("expected '='"));
                }
                if(_token == _SC(',')) Lex();
                else if(_token != _SC(')')) Error(_SC("expected ')' or ','"));
            }
        }
        Expect(_SC(')'));
        switch(_token)
        {
        case _SC(':'):
        {
            //return type specifier like typescript
            Lex();
            type_name = ExpectTypeToken();
            funcstate->_return_type = type_name;
            //printf("%d %s\n", __LINE__, _stringval(type_name));
        }
        break;

        case TK_CONST:
        case TK_NOEXCEPT:
        case TK_VOLATILE:
            Lex(); //accept and ignore
            break;

        case TK_IDENTIFIER:
            if(ftype == eFunctionType_member)
            {
                if(  (scstrcmp(_lex.data->svalue, _SC("final")) == 0) ||
                        (scstrcmp(_lex.data->svalue, _SC("override")) == 0))
                {
                    Lex(); //accept but ignore then
                }
            }
        }

        for(SQInteger n = 0; n < defparams; n++)
        {
            _fs->PopTarget();
        }

        if(_is_parsing_extern)
        {
            if(_token == _SC(';')) //to parse thinscript
            {
                _fs->PopChildState();
                return;
            }
            else _is_parsing_extern = false;
        }

        SQFuncState *currchunk = _fs;
        _fs = funcstate;
        if(ftype == eFunctionType_lambda)
        {
            Expression();
            _fs->AddInstruction(_OP_RETURN, 1, _fs->PopTarget());
        }
        else
        {
            Statement(false);
        }
        ResolveGotos();
        funcstate->AddLineInfos(_lex.data->prevtoken == _SC('\n')?_lex.data->lasttokenline:_lex.data->currentline, _lineinfo, true);
        funcstate->AddInstruction(_OP_RETURN, -1);
        funcstate->SetStackSize(0);

        SQFunctionProto *func = funcstate->BuildProto();
#ifdef _DEBUG_DUMP
        funcstate->Dump(func);
#endif
        _fs = currchunk;
        _fs->_functions.push_back(func);
        _fs->PopChildState();
    }
    void AdjustGotoInstruction(const SQGotoLabelsInfo &goto_info, const SQGotoLabelsInfo &label)
    {
        SQInteger target_pos = label.pos;
        SQInteger jump_pos = target_pos - goto_info.pos;
        //set the jmp instruction

        SQInteger poptraps = goto_info.traps - label.traps;
        SQInt32 ioffset = 1;
        while(poptraps)
        {
            SQInstruction &i_op_poptrap = _fs->GetInstruction(goto_info.pos-ioffset);
            if(i_op_poptrap.op != _OP_NOP)
            {
                if(ioffset < 2)
                {
                    ++ioffset;
                    continue;
                }
                Error(_SC("Compiler: This should not happen i_op_poptrap.op != _OP_NOP"));
            }
            //printf("poptraps for %s = %d : %d\n", _stringval(label.name), (int)poptraps, (int)i_op_poptrap._arg0);
            i_op_poptrap.op = _OP_POPTRAP;
            i_op_poptrap._arg0 = poptraps;
            break;
        }

        _fs->SetIntructionParams(goto_info.pos, 0, jump_pos, 0);
    }
    void ResolveGotos()
    {
        //first we walk through the labels and when we come back
        //nested labels we remove then because we only allow
        //goto/joump out of blocks/scopes
        SQInt16 last_nested = -1;
        for(SQUnsignedInteger idxlabel = 0; idxlabel < _fs->_gototargets.size(); ++idxlabel)
        {
            SQGotoLabelsInfo &label = _fs->_gototargets[idxlabel];
            SQUnsignedInteger resolved = 0;
            for(SQUnsignedInteger idxGoto = 0; idxGoto < _fs->_unresolvedgotos.size(); ++idxGoto)
            {
                SQGotoLabelsInfo &goto_info = _fs->_unresolvedgotos[idxGoto];
                if(_string(label.name) == _string(goto_info.name))
                {
                    if(label.nested > goto_info.nested)
                    {
                        _lex.data->currentline = goto_info.line;
                        Error(_SC("Goto can not jump inside nested block/scope '%s'"), _stringval(goto_info.name));
                    }
                    AdjustGotoInstruction(goto_info, label);
                    //we dcrease idxGoto here to compensate the removal inside the for loop
                    _fs->_unresolvedgotos.remove(idxGoto--);
                    ++resolved;
                }
            }
            if(!resolved)
            {
                //if a label isn't resolved we have an unused label
                _lex.data->currentline = label.line;
                Error(_SC("Label not resolved at this point '%s'"), _stringval(label.name));
            }
            if(label.nested < last_nested)
            {
                //remove any previous nested label to prevent goto/jump inside blocks/scopes
                SQUnsignedInteger i = idxlabel - 1;
                for(; i > 0; --i)
                {
                    if(_fs->_gototargets[i].nested <= label.nested) break;
                    _fs->_gototargets.remove(i);
                    //we decrease the index idxlabel here due to remove before
                    //the current idxlabel
                    --idxlabel;
                }
            }
            last_nested = label.nested;
        }

        if(_fs->_unresolvedgotos.size())
        {
            //if we still have any unresolved goto it's an error, let's show the first
            SQGotoLabelsInfo &goto_info = _fs->_unresolvedgotos[0];
            _lex.data->currentline = goto_info.line;
            Error(_SC("Goto can not jump to undeclared label '%s'"), _stringval(goto_info.name));
        }
    }
    void ResolveBreaks(SQFuncState *funcstate, SQInteger ntoresolve)
    {
        while(ntoresolve > 0)
        {
            SQInteger pos = funcstate->_unresolvedbreaks.back();
            funcstate->_unresolvedbreaks.pop_back();
            //set the jmp instruction
            funcstate->SetIntructionParams(pos, 0, funcstate->GetCurrentPos() - pos, 0);
            ntoresolve--;
        }
    }
    void ResolveContinues(SQFuncState *funcstate, SQInteger ntoresolve, SQInteger targetpos)
    {
        while(ntoresolve > 0)
        {
            SQInteger pos = funcstate->_unresolvedcontinues.back();
            funcstate->_unresolvedcontinues.pop_back();
            //set the jmp instruction
            funcstate->SetIntructionParams(pos, 0, targetpos - pos, 0);
            ntoresolve--;
        }
    }
private:
    SQInteger _token;
    SQFuncState *_fs;
    SQObjectPtr _sourcename;
    SQLexer _lex;
    bool _lineinfo;
    bool _raiseerror;
    bool _show_warnings;
    bool _is_parsing_extern;
    SQInteger _debugline;
    SQInteger _debugop;
    SQExpState   _es;
    SQScope _scope;
    SQChar *_compilererror;
    jmp_buf _errorjmp;
    SQVM *_vm;
    SQObjectPtrVec _scope_consts;
    SQObjectPtrVec _scope_types;
    SQObjectPtr _globals;
    SQObjectPtr _type_names; //to allow C/C++ style instance declarations
    SQObjectPtr _extern_names; //to allow C/C++ style extern declarations
    SQChar error_buf[MAX_COMPILER_ERROR_LEN];
    SQInteger _max_nested_includes, _nested_includes_count;
    SQInteger _ifdef_exclude, _ifdef_line, _inside_ifdef;
    const SQChar *squilu_lib_path;
};

bool Compile(SQVM *vm,SQLEXREADFUNC rg, SQUserPointer up, const SQChar *sourcename, SQObjectPtr &out,
             bool raiseerror, bool lineinfo, bool show_warnings, SQInteger max_nested_includes)
{
    SQCompiler p(vm, rg, up, sourcename, raiseerror, lineinfo, show_warnings, max_nested_includes);
    return p.Compile(out);
}

#endif
