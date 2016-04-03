const TK_FIRST_ENUM_TOKEN = 258;
const TK_3WAYSCMP = 259;
const TK_AND = 260;
const TK_ARROW = 261;
const TK_AS = 262;
const TK_ATTR_CLOSE = 263;
const TK_ATTR_OPEN = 264;
const TK_BASE = 265;
const TK_BIT_AND_EQ = 266;
const TK_BIT_OR_EQ = 267;
const TK_BIT_SHIFT_LEFT_EQ = 268;
const TK_BIT_SHIFT_RIGHT_EQ = 269;
const TK_BIT_XOR_EQ = 270;
const TK_BREAK = 271;
const TK_CASE = 272;
const TK_CATCH = 273;
const TK_CLASS = 274;
const TK_CLONE = 275;
const TK_CONST = 276;
const TK_COMMENT_BLOCK = 277;
const TK_COMMENT_LINE = 278;
const TK_CONSTRUCTOR = 279;
const TK_CONTINUE = 280;
const TK_DECLARE = 281;
const TK_DEFAULT = 282;
const TK_DELETE = 283;
const TK_DESTRUCTOR = 284;
const TK_DIVEQ = 285;
const TK_DO = 286;
const TK_DOUBLE_COLON = 287;
const TK_ELSE = 288;
const TK_ENUM = 289;
const TK_EQ = 290;
const TK_EQ_IDENTITY = 291;
const TK_EXTENDS = 292;
const TK_EXTERN = 293;
const TK_FALSE = 294;
const TK___FILE__ = 295;
const TK_FLOAT = 296;
const TK_FOR = 297;
const TK_FOREACH = 298;
const TK_FRIEND = 299;
const TK___FUNCTION__ = 300;
const TK_FUNCTION = 301;
const TK_GE = 302;
const TK_IDENTIFIER = 303;
const TK_IF = 304;
const TK_IGNORE = 305;
const TK_IN = 306;
const TK_INSTANCEOF = 307;
const TK_INTEGER = 308;
const TK_LE = 309;
const TK___LINE__ = 310;
const TK_LOCAL = 311;
const TK_LOCAL_ANY_T = 312;
const TK_LOCAL_ARRAY_T = 313;
const TK_LOCAL_BOOL_T = 314;
const TK_LOCAL_CHAR_T = 315;
const TK_LOCAL_DOUBLE_T = 316;
const TK_LOCAL_FLOAT_T = 317;
const TK_LOCAL_INT16_T = 318;
const TK_LOCAL_INT32_T = 319;
const TK_LOCAL_INT64_T = 320;
const TK_LOCAL_INT8_T = 321;
const TK_LOCAL_INT_T = 322;
const TK_LOCAL_LONG_DOUBLE_T = 323;
const TK_LOCAL_NUMBER_T = 324;
const TK_LOCAL_SIZE_T = 325;
const TK_LOCAL_SSIZE_T = 326;
const TK_LOCAL_STRING_T = 327;
const TK_LOCAL_TABLE_T = 328;
const TK_LOCAL_UINT16_T = 329;
const TK_LOCAL_UINT32_T = 330;
const TK_LOCAL_UINT64_T = 331;
const TK_LOCAL_UINT8_T = 332;
const TK_LOCAL_UINT_T = 333;
const TK_LOCAL_VOIDPTR_T = 334;
const TK_LOCAL_WCHAR_T = 335;
const TK_LOCAL_WEAKREF_T = 336;
const TK_MINUSEQ = 337;
const TK_MINUSMINUS = 338;
const TK_MODEQ = 339;
const TK_MODULO = 340;
const TK_MULEQ = 341;
const TK_NE = 342;
const TK_NE_IDENTITY = 343;
const TK_NEWSLOT = 344;
const TK_NOEXCEPT = 345;
const TK_NULL = 346;
const TK_OR = 347;
const TK_PLUSEQ = 348;
const TK_PLUSPLUS = 349;
const TK_PRAGMA = 350;
const TK_PRIVATE = 351;
const TK_PUBLIC = 352;
const TK_RESUME = 353;
const TK_RETURN = 354;
const TK_SHIFTL = 355;
const TK_SHIFTR = 356;
const TK_STATIC = 357;
const TK_STRING_LITERAL = 358;
const TK_STRUCT = 359;
const TK_SWITCH = 360;
const TK_THIS = 361;
const TK_THROW = 362;
const TK_TYPEDEF = 363;
const TK_TRUE = 364;
const TK_TRY = 365;
const TK_TYPEOF = 366;
const TK_UMINUS = 367;
const TK_UNSAFE = 368;
const TK_USING = 369;
const TK_USHIFTR = 370;
const TK_VARPARAMS = 371;
const TK_VIRTUAL = 372;
const TK_VOID = 373;
const TK_VOLATILE = 374;
const TK_WHILE = 375;
const TK_YIELD = 376;
const TK_INLINE = 377;
const TK_CONSTEXPR = 378;
const TK_LAST_ENUM_TOKEN = 379;

//typedefs to allow parse/compile
typedef char_ptr_t SQChar_ptr_t;
typedef class_t SQObject;
typedef var_t SQObjectPtr;
typedef bool_t bool;
typedef int_t int;
typedef int_t SQInteger;
typedef class_t SQLex;
typedef enum_t SQOpcode;
typedef float_t SQFloat;
typedef void_ptr_t SQObjectPtr_ptr_t;
typedef class_t SQFuncState;
typedef void_ptr_t SQFuncState_ptr_t;
typedef class_t SQVM;
typedef class_ptr_t SQVM_ptr_t;
typedef struct_t jmp_buf;
typedef array_t SQObjectPtrVec;
typedef void_ptr_t SQUserPointer;
typedef array_t SQInstructionVec;
typedef param_t T;
typedef func_ptr_t SQLEXREADFUNC;
typedef class_ptr_t SQCompiler_ptr_t;
typedef void_ptr_t va_list;
typedef enum_t SQObjectType;


//const SQUIRREL_VERSION	= _SC("SquiLu based on Squirrel 3.1 stable and Lua 5.1.5");
//const SQUIRREL_COPYRIGHT	= _SC("Copyright (C) 2003-2016 Alberto Demichelis, Domingo Alvarez Duarte");
//const SQUIRREL_AUTHOR	= _SC("Alberto Demichelis, Domingo Alvarez Duarte");
const SQUIRREL_VERSION_NUMBER	= 310;

const SQ_VMSTATE_IDLE			= 0;
const SQ_VMSTATE_RUNNING		= 1;
const SQ_VMSTATE_SUSPENDED	= 2;

const SQUIRREL_EOB = 0;
const SQ_BYTECODE_STREAM_TAG	= 0xFAFA;

const SQOBJECT_REF_COUNTED	= 0x08000000;
const SQOBJECT_NUMERIC		= 0x04000000;
const SQOBJECT_DELEGABLE		= 0x02000000;
const SQOBJECT_CANBEFALSE		= 0x01000000;

const SQ_MATCHTYPEMASKSTRING = /*(*/-99999;

const _RT_MASK = 0x00FFFFFF;
//const _RAW_TYPE(type) (type&_RT_MASK);

const _RT_NULL			= 0x00000001;
const _RT_INTEGER			= 0x00000002;
const _RT_FLOAT			= 0x00000004;
const _RT_BOOL			= 0x00000008;
const _RT_STRING			= 0x00000010;
const _RT_TABLE			= 0x00000020;
const _RT_ARRAY			= 0x00000040;
const _RT_USERDATA		= 0x00000080;
const _RT_CLOSURE			= 0x00000100;
const _RT_NATIVECLOSURE	= 0x00000200;
const _RT_GENERATOR		= 0x00000400;
const _RT_USERPOINTER		= 0x00000800;
const _RT_THREAD			= 0x00001000;
const _RT_FUNCPROTO		= 0x00002000;
const _RT_CLASS			= 0x00004000;
const _RT_INSTANCE		= 0x00008000;
const _RT_WEAKREF		= 0x00010000;
const _RT_OUTER			= 0x00020000;

/*
//typedef 
//enum tagSQObjectType {
int_t OT_NULL =			(_RT_NULL|SQOBJECT_CANBEFALSE);
int_t OT_INTEGER =		(_RT_INTEGER|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE);
int_t OT_FLOAT =			(_RT_FLOAT|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE);
int_t OT_BOOL =			(_RT_BOOL|SQOBJECT_CANBEFALSE);
int_t OT_STRING =			(_RT_STRING|SQOBJECT_REF_COUNTED);
int_t OT_TABLE =			(_RT_TABLE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE);
int_t OT_ARRAY =			(_RT_ARRAY|SQOBJECT_REF_COUNTED);
int_t OT_USERDATA =		(_RT_USERDATA|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE);
int_t OT_CLOSURE =		(_RT_CLOSURE|SQOBJECT_REF_COUNTED);
int_t OT_NATIVECLOSURE =	(_RT_NATIVECLOSURE|SQOBJECT_REF_COUNTED);
int_t OT_GENERATOR =		(_RT_GENERATOR|SQOBJECT_REF_COUNTED);
int_t OT_USERPOINTER =	_RT_USERPOINTER;
int_t OT_THREAD =			(_RT_THREAD|SQOBJECT_REF_COUNTED);
int_t OT_FUNCPROTO =		(_RT_FUNCPROTO|SQOBJECT_REF_COUNTED); //internal usage only
int_t OT_CLASS =			(_RT_CLASS|SQOBJECT_REF_COUNTED);
int_t OT_INSTANCE =		(_RT_INSTANCE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE);
int_t OT_WEAKREF =		(_RT_WEAKREF|SQOBJECT_REF_COUNTED);
int_t OT_OUTER =			(_RT_OUTER|SQOBJECT_REF_COUNTED); //internal usage only
//}SQObjectType;
*/

const OT_NULL	= 1;
const OT_INTEGER	= 2;
const OT_FLOAT	= 3;
const OT_BOOL	= 4;
const OT_STRING	= 5;
const OT_TABLE	= 6;
const OT_ARRAY	= 7;
const OT_USERDATA	= 8;
const OT_CLOSURE	= 9;
const OT_NATIVECLOSURE	= 10;
const OT_GENERATOR	= 11;
const OT_USERPOINTER	= 12;
const OT_THREAD	= 13;
const OT_FUNCPROTO	= 14;
const OT_CLASS	= 15;
const OT_INSTANCE	= 16;
const OT_WEAKREF	= 17;
const OT_OUTER	= 18;

enum eFunctionType
{
    fn_global = 1,
    fn_local,
    fn_lambda,
    fn_member,
    fn_anonymous
};

const EXPR =  1;
const OBJECT = 2;
const BASE =  3;
const LOCAL = 4;
const OUTER = 5;
const EXPR_STATEMENT = 6; //we start from statement state

struct SQExpState
{
    SQInteger  etype;       /* expr. type; one of EXPR, OBJECT, BASE, OUTER, LOCAL or EXPR_STATEMENT */
    SQInteger  epos;        /* expr. location on stack; -1 for OBJECT and BASE */
    bool       donot_get;   /* signal not to deref the next value */
};

const MAX_COMPILER_ERROR_LEN = 256;

struct SQScope
{
    SQInteger outers;
    SQInteger stacksize;
    SQInteger nested;
};

static SQInteger compilerReadFunc(SQUserPointer fp)
{
    SQInteger c = fgetc(/*(FILE *)*/fp);
    if(c == EOF)
    {
        return 0;
    }
    return c;
}

class SQCompiler
{
//private:
    SQInteger _token;
    SQFuncState_ptr_t _fs;
    SQObjectPtr _sourcename;
    SQLex _lex;
    bool _lineinfo;
    bool _raiseerror;
    bool _show_warnings;
    bool _is_parsing_extern;
    SQInteger _debugline;
    SQInteger _debugop;
    SQExpState   _es;
    SQScope _scope;
    SQChar_ptr_t _compilererror;
    jmp_buf _errorjmp;
    SQVM_ptr_t _vm;
    SQObjectPtrVec _scope_consts;
    SQObjectPtr _globals;
    SQObjectPtr _type_names; //to allow C/C++ style instance declarations
    SQObjectPtr _extern_names; //to allow C/C++ style extern declarations
    //SQChar error_buf[MAX_COMPILER_ERROR_LEN];
    SQInteger _max_nested_includes, _nested_includes_count;

//public:

    SQCompiler()
    {
        _vm=v;
        _lex.Init(_ss(v), rg, up,ThrowError,this);
        //_sourcename = SQString::Create(_ss(v), sourcename);
        _lineinfo = lineinfo;
        _raiseerror = raiseerror;
        _show_warnings = show_warnings;
        _scope.outers = 0;
        _scope.stacksize = 0;
        _scope.nested = 0;
        _compilererror = NULL;
        _globals = {}; //SQTable::Create(_ss(_vm),0);
        _type_names = {}; //SQTable::Create(_ss(_vm),0);
        _extern_names = {}; //SQTable::Create(_ss(_vm),0);
        _max_nested_includes = max_nested_includes;
        _nested_includes_count = 0;
        _is_parsing_extern = false;
    }
    ~SQCompiler()
    {
        //_table(_globals)->Finalize();
        //_globals.Null();
    }

    static void ThrowError(void_ptr_t ud, const SQChar_ptr_t s)
    {
        SQCompiler_ptr_t c = /*(SQCompiler *)*/ud;
        c->Error(s);
    }

    void Error(const SQChar_ptr_t s, ...) //__attribute__((format(printf, 2, 3)))
    {
        va_list vl;
        va_start(vl, s);
        scvsprintf(error_buf, sizeof(error_buf), s, vl);
        va_end(vl);
        _compilererror = error_buf;
        longjmp(_errorjmp,1);
    }

    void Warning(const SQChar_ptr_t s, ...) //__attribute__((format(printf, 2, 3)))
    {
        if(!_show_warnings) return;
        va_list vl;
        va_start(vl, s);
        scvfprintf(stderr, s, vl);
        va_end(vl);
    }

    bool CheckNameIsType(const SQObject &name)
    {
        return _table(_type_names)->Exists(name);
    }

    bool CheckTypeName(const SQObject &name, bool addIfNotExists=false)
    {
        bool found = _table(_type_names)->Exists(name);
        if(addIfNotExists && !found)
        {
            SQObjectPtr oname = name, otrue = true;
            _table(_type_names)->NewSlot(oname, otrue);
        }
        return found;
    }

    bool TypesGet(const SQObjectPtr &key,SQObjectPtr &val)
    {
        return _table(_type_names)->Get(key,val);
    }

    bool TypesNewSlot(const SQObjectPtr &key, const SQObjectPtr &val)
    {
        return _table(_type_names)->NewSlot(key,val);
    }

    int CheckExternName(const SQObject &name, bool addIfNotExists=false)
    {
        SQObjectPtr orefcount;
        int found = _table(_extern_names)->Get(name, orefcount);
        if(addIfNotExists && !found)
        {
            SQObjectPtr oname = name;
            //_integer(orefcount) = 1; //1 means only declared, 2 declared and assigned
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
            //_integer(orefcount) = ref_count = 2;
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
        /*const*/ SQChar_ptr_t found = NULL;
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
                             _stringval(_sourcename), _lex._currentline, _lex._currentcolumn,
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
                        _stringval(_sourcename), _lex._currentline, _lex._currentcolumn, _stringval(name));
        }
        else
        {
            found = _fs->FindOuterVariable(name);
            if(found >= 0) Warning(_SC("%s:%d:%d warning outer variable '%s' already declared will be shadowed\n"),
                                       _stringval(_sourcename), _lex._currentline, _lex._currentcolumn, _stringval(name));
        }
        if(checkGlobals) CheckGlobalName(name, false, false);
        SQObjectPtr strongid = name;
        CheckConstsExists(strongid);
    }

    bool IsConstant(const SQObject &name,SQObject &e)
    {
        SQObjectPtr val;
        for(int i=_scope.nested-1; i >= 0; --i)
        {
            if(_table(_scope_consts[i])->Get(name,val))
            {
                e = val;
                return true;
            }
        }
        if(_table(_ss(_vm)->_consts)->Get(name,val))
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
            if(type(constant) == OT_TABLE)
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
            SQObjectType ctype = type(constval);
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
                                   _stringval(_sourcename), _lex._currentline, _lex._currentcolumn,  _stringval(key));
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

    SQObjectPtr GetTokenObject(SQInteger tok)
    {
        SQObjectPtr ret;
        switch(tok)
        {
        case TK_IDENTIFIER:
            ret = _fs->CreateString(_lex._svalue);
            break;
        case TK_STRING_LITERAL:
            ret = _fs->CreateString(_lex._svalue,_lex._longstr.size()-1);
            break;
        case TK_INTEGER:
            ret = SQObjectPtr(_lex._nvalue);
            break;
        case TK_FLOAT:
            ret = SQObjectPtr(_lex._fvalue);
            break;
        default:
            ret = _fs->CreateString(_lex.GetTokenName(_token));
        }
        Lex();
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
                /*const*/ SQChar_ptr_t etypename;
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
                    case TK_INTEGER:
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

    SQObject Expect(SQInteger tok)
    {
        ErrorIfNotToken(tok);
        return GetTokenObject(tok);
    }

    SQObject ExpectTypeToken()
    {
        switch(_token)
        {
        //CASE_TK_LOCAL_TYPES:
        case TK_IDENTIFIER:
            break;
        default:
            ErrorIfNotToken(TK_IDENTIFIER);
        }
        return GetTokenObject(TK_IDENTIFIER);
    }

    bool IsEndOfStatement()
    {
        return ((_lex._prevtoken == '\n') || (_token == SQUIRREL_EOB) || (_token == '}') || (_token == ';'));
    }

    void OptionalSemicolon()
    {
        if(_token == ';')
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
        int line = _lex._currentline;
        //int column = _lex._currentcolumn;
        Lex();
        SQObject id = Expect(TK_IDENTIFIER);
        if(scstrcmp(_stringval(id), _SC("include")) == 0)
        {
//#if SQ_ENABLE_INCLUDES
            SQInteger nested_count = _nested_includes_count + 1;
            if((_max_nested_includes <= 0) || (nested_count > _max_nested_includes))
            {
                Error(_SC("Error: too many nested includes %d %s\n"), nested_count, _stringval(id));
            }

            id = Expect(TK_STRING_LITERAL);
            //Warning(_SC("%s:%d:%d warning pragma include %s\n"),
            //        _stringval(_sourcename), line, column, _stringval(id));

            FILE *fp = scfopen(_stringval(id), "r");
            if(fp != NULL)
            {
                //increment nested count
                ++_nested_includes_count;
                //save current source file and lex state
                SQUserPointer saved_up = _lex._up; //current userpointer
                SQLEXREADFUNC saved_readf = _lex._readf; //current readfunction
                SQInteger saved_line = _lex._currentline;
                SQInteger saved_column = _lex._currentcolumn;
                SQInteger saved_curdata = _lex._currdata;
                SQInteger saved_prevtoken = _lex._prevtoken;
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
                    if(_lex._prevtoken != '}' && _lex._prevtoken != ';') OptionalSemicolon();
                }

                //close file
                fclose(fp);
                //restore saved source file and lex state
                _fs->_sourcename = saved_source_name;
                _sourcename = saved_source_name;
                _token = saved_token;
                _lex._currdata = saved_curdata;
                _lex._prevtoken = saved_prevtoken;
                _lex._currentcolumn = saved_column;
                _lex._currentline = saved_line;
                _lex._readf = saved_readf;
                _lex._up = saved_up;

                --_nested_includes_count;
                //done let's continue working

            }
            else
            {
                Error(_SC("Error: opening include file %s\n"), _stringval(id));
            }
//#else
//                Error(_SC("Error: includes are not enabled\n"));
//#endif
        }
        else
        {
            _lex._currentline = line;
            Error(_SC("Error: unknown pragma %s\n"), _stringval(id));
        }
    }

    bool Compile(SQObjectPtr &o)
    {
        _debugline = 1;
        _debugop = 0;

        //SQFuncState funcstate(_ss(_vm), NULL,ThrowError,this);
        funcstate._name = SQString_Create(_ss(_vm), _SC("main"));
        _fs = /*&*/funcstate;
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
                if(_lex._prevtoken != '}' && _lex._prevtoken != ';') OptionalSemicolon();
            }
            _fs->SetStackSize(stacksize);
            _fs->AddLineInfos(_lex._currentline, _lineinfo, true);
            _fs->AddInstruction(_OP_RETURN, 0xFF);
            _fs->SetStackSize(0);
            o =_fs->BuildProto();
//#ifdef _DEBUG_DUMP
//            _fs->Dump(_funcproto(o));
//#endif
        }
        else
        {
            if(_raiseerror && _ss(_vm)->_compilererrorhandler)
            {
                _ss(_vm)->_compilererrorhandler(_vm, _compilererror, type(_sourcename) == OT_STRING?_stringval(_sourcename):_SC("unknown"),
                                                _lex._currentline, _lex._currentcolumn);
            }
            _vm->_lasterror = SQString_Create(_ss(_vm), _compilererror, -1);
            _vm->_lasterror_line = _lex._currentline;
            _vm->_lasterror_column = _lex._currentcolumn;
            return false;
        }
        return true;
    }

    void Statements()
    {
        while(_token != '}' && _token != TK_DEFAULT && _token != TK_CASE)
        {
            Statement();
            if(_lex._prevtoken != '}' && _lex._prevtoken != ';') OptionalSemicolon();
        }
    }

    void Statement(bool closeframe = true)
    {
        _es.etype = EXPR_STATEMENT;
        SQObject id;
        _fs->AddLineInfos(_lex._currentline, _lineinfo);
        //start_again:
        switch(_token)
        {
        case ';':
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
            /*CASE_TK_LOCAL_TYPES:*/
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
        case '{':
        {
            BEGIN_SCOPE();
            Lex();
            Statements();
            Expect('}');
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
            id = Expect(TK_IDENTIFIER);
            if(_token == ':')
            {
                //type specifier like typescript
                Lex();
                ExpectTypeToken(); //ignore for now
            }
            Expect('=');
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
        //goto start_again;
        case TK_STATIC:
            if(_scope.nested)
            {
                Warning(_SC("%s:%d:%d warning static cualifier is ignored\n"),
                        _stringval(_sourcename), _lex._currentline, _lex._currentcolumn);
            }
            Lex(); //ignore it only to allow run some C/C++ code
            //goto start_again;

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
            Expect(';');
            TypesNewSlot(strongid,SQObjectPtr(type_val));
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
            Expect(';');
            TypesNewSlot(strongid,SQObjectPtr(type_val));
        }
        break;

        case TK_PRAGMA:
            Pragma();
            break;

        case TK_UNSAFE:
            Lex(); //ignore for now
            //goto start_again;
            break;

        case TK_IDENTIFIER:
            id = _fs->CreateString(_lex._svalue);
            if(CheckTypeName(id)) //C/C++ type declaration;
            {
                LocalDeclStatement();
                break;
            }

        default:
            CommaExpr();
            _fs->DiscardTarget();
            /*
            //Fagiano says that this is not a bug
            //and with this modification stack grow by one lement all the time
                        if(_token == TK_IDENTIFIER){
                            CommaExpr();
                            if(_token == TK_IDENTIFIER){
                                Error(_SC(" '=' expected near '%s'"), _lex._svalue);
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
            SQInteger p2 = _fs->PopTarget(); //src in OP_GET
            SQInteger p1 = _fs->PopTarget(); //key in OP_GET
            _fs->PushTarget(p1);
            //EmitCompArithLocal(tok, p1, p1, p2);
            _fs->AddInstruction(ChooseArithOpByToken(tok),p1, p2, p1, 0);
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
        //for(Expression(warningAssign); _token == ','; _fs->PopTarget(), Lex(), CommaExpr(warningAssign));
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
            id = _fs->CreateString(_lex._svalue);
        }
        //else id = {}; //old compilers do not allow this

        LogicalOrExp();
        switch(_token)
        {
        case '=':
        case TK_NEWSLOT:
        case TK_MINUSEQ:
        case TK_PLUSEQ:
        case TK_MULEQ:
        case TK_DIVEQ:
        case TK_MODEQ:
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
                            && (type(id) == OT_STRING) ) AddGlobalName(id);
                }
                else //if _derefstate != DEREF_NO_DEREF && DEREF_FIELD so is the index of a local
                    Error(_SC("can't 'create' a local slot"));
                break;
            case '=': //ASSIGN
                if(warningAssign) Warning(_SC("%s:%d:%d warning making assignment, maybe it's not what you want\n"),
                                              _stringval(_sourcename), _lex._currentline, _lex._currentcolumn);
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
                EmitCompoundArith(op, ds, pos);
                break;
            }
        }
        break;
        case '?':
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
            Expect(':');
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

    //template<typename T>
    void INVOKE_EXP(T f)
    {
        SQExpState es = _es;
        _es.etype     = EXPR;
        _es.epos      = -1;
        _es.donot_get = false;
        //(this->*f)();
        _es = es;
    }
    //template<typename T>
    void BIN_EXP(SQOpcode op, T f,SQInteger op3 = 0)
    {
        Lex();
        INVOKE_EXP(f);
        SQInteger op1 = _fs->PopTarget();
        SQInteger op2 = _fs->PopTarget();
        _fs->AddInstruction(op, _fs->PushTarget(), op1, op2, op3);
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
                INVOKE_EXP(/*&*/SQCompiler_LogicalOrExp);
                _fs->SnoozeOpt();
                SQInteger second_exp = _fs->PopTarget();
                if(trg != second_exp) _fs->AddInstruction(_OP_MOVE, trg, second_exp);
                _fs->SnoozeOpt();
                _fs->SetIntructionParam(jpos, 1, (_fs->GetCurrentPos() - jpos));
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
                INVOKE_EXP(/*&*/SQCompiler_LogicalAndExp);
                _fs->SnoozeOpt();
                SQInteger second_exp = _fs->PopTarget();
                if(trg != second_exp) _fs->AddInstruction(_OP_MOVE, trg, second_exp);
                _fs->SnoozeOpt();
                _fs->SetIntructionParam(jpos, 1, (_fs->GetCurrentPos() - jpos));
                break;
            }

            default:
                return;
            }
    }

    void BitwiseOrExp()
    {
        BitwiseXorExp();
        for(;;) if(_token == '|')
            {
                BIN_EXP(_OP_BITW,/*&*/SQCompiler_BitwiseXorExp,BW_OR);
            }
            else return;
    }

    void BitwiseXorExp()
    {
        BitwiseAndExp();
        for(;;) if(_token == '^')
            {
                BIN_EXP(_OP_BITW,/*&*/SQCompiler_BitwiseAndExp,BW_XOR);
            }
            else return;
    }

    void BitwiseAndExp()
    {
        EqExp();
        for(;;) if(_token == '&')
            {
                BIN_EXP(_OP_BITW,/*&*/SQCompiler_EqExp,BW_AND);
            }
            else return;
    }

    void EqExp()
    {
        CompExp();
        for(;;) switch(_token)
            {
            case TK_EQ:
                BIN_EXP(_OP_EQ,/*&*/SQCompiler_CompExp);
                break;
            case TK_EQ_IDENTITY :
                BIN_EXP(_OP_EQI,/*&*/SQCompiler_CompExp);
                break;
            case TK_NE:
                BIN_EXP(_OP_NE,/*&*/SQCompiler_CompExp);
                break;
            case TK_NE_IDENTITY:
                BIN_EXP(_OP_NEI,/*&*/SQCompiler_CompExp);
                break;
            case TK_3WAYSCMP:
                BIN_EXP(_OP_CMP,/*&*/SQCompiler_CompExp,CMP_3W);
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
            case '>':
                BIN_EXP(_OP_CMP, /*&*/SQCompiler_ShiftExp,CMP_G);
                break;
            case '<':
                BIN_EXP(_OP_CMP, /*&*/SQCompiler_ShiftExp,CMP_L);
                break;
            case TK_GE:
                BIN_EXP(_OP_CMP, /*&*/SQCompiler_ShiftExp,CMP_GE);
                break;
            case TK_LE:
                BIN_EXP(_OP_CMP, /*&*/SQCompiler_ShiftExp,CMP_LE);
                break;
            case TK_IN:
                BIN_EXP(_OP_EXISTS, /*&*/SQCompiler_ShiftExp);
                break;
            case TK_INSTANCEOF:
                BIN_EXP(_OP_INSTANCEOF, /*&*/SQCompiler_ShiftExp);
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
                BIN_EXP(_OP_BITW, /*&*/SQCompiler_PlusExp,BW_USHIFTR);
                break;
            case TK_SHIFTL:
                BIN_EXP(_OP_BITW, /*&*/SQCompiler_PlusExp,BW_SHIFTL);
                break;
            case TK_SHIFTR:
                BIN_EXP(_OP_BITW, /*&*/SQCompiler_PlusExp,BW_SHIFTR);
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
        case '+':
            return _OP_ADD;
        case TK_MINUSEQ:
        case '-':
            return _OP_SUB;
        case TK_MULEQ:
        case '*':
            return _OP_MUL;
        case TK_DIVEQ:
        case '/':
            return _OP_DIV;
        case TK_MODEQ:
        case '%':
            return _OP_MOD;
        default:
            assert(0);
        }
        return _OP_ADD;
    }

    SQInteger ChooseCompArithCharByToken(SQInteger tok)
    {
        SQInteger oper;
        switch(tok)
        {
        case TK_MINUSEQ:
            oper = '-';
            break;
        case TK_PLUSEQ:
            oper = '+';
            break;
        case TK_MULEQ:
            oper = '*';
            break;
        case TK_DIVEQ:
            oper = '/';
            break;
        case TK_MODEQ:
            oper = '%';
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
            case '+':
            case '-':
                BIN_EXP(ChooseArithOpByToken(_token), /*&*/SQCompiler_MultExp);
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
            case '*':
            case '/':
            case '%':
                BIN_EXP(ChooseArithOpByToken(_token), /*&*/SQCompiler_PrefixedExpr);
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
            case '.':
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
            case '[':
                if(_lex._prevtoken == '\n') Error(_SC("cannot brake deref/or comma needed after [exp]=exp slot declaration"));
                Lex();
                Expression();
                Expect(']');
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
            case '(':
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

    SQInteger Factor() {}

    void EmitLoadConstInt(SQInteger value,SQInteger target)
    {
        if(target < 0)
        {
            target = _fs->PushTarget();
        }
        //with the line bellow we get wrong result for -1
        //if(value <= INT_MAX && value > INT_MIN) { //does it fit in 32 bits?
        if((value & (~(/*(SQInteger)*/0xFFFFFFFF))) == 0)   //does it fit in 32 bits?
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
            _fs->AddInstruction(_OP_LOADFLOAT, target,/**((SQInt32 *)&*/value);
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
        case '=':
        case '(':
        case TK_NEWSLOT:
        case TK_MODEQ:
        case TK_MULEQ:
        case TK_DIVEQ:
        case TK_MINUSEQ:
        case TK_PLUSEQ:
            return false;
            1
        case TK_PLUSPLUS:
        case TK_MINUSMINUS:
            if (!IsEndOfStatement())
            {
                return false;
            }
            break;
        }
        return (!_es.donot_get || ( _es.donot_get && (_token == '.' || _token == '[')));
    }

    void FunctionCallArgs()
    {
        SQInteger nargs = 1;//this
        while(_token != ')')
        {
            Expression();
            MoveIfCurrentTargetIsLocal();
            nargs++;
            if(_token == ',')
            {
                Lex();
                if(_token == ')') Error(_SC("expression expected, found ')'"));
            }
        }
        Lex();
        for(SQInteger i = 0; i < (nargs - 1); i++) _fs->PopTarget();
        SQInteger stackbase = _fs->PopTarget();
        SQInteger closure = _fs->PopTarget();
        _fs->AddInstruction(_OP_CALL, _fs->PushTarget(), closure, stackbase, nargs);
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

    //void CheckClassMemberExists(SQObjectPtr &member_names, const SQChar_ptr_t name){}

    void ParseTableOrClass(SQInteger separator,SQInteger terminator, SQObjectPtr_ptr_t class_name=NULL) {}

    void ExternDeclStatement()
    {
        _is_parsing_extern = true;
        Lex();
        LocalDeclStatement();
        _is_parsing_extern = false;
    }

    void LocalDeclStatement() {}

    void IfBlock()
    {
        if (_token == '{')
        {
            BEGIN_SCOPE();
            Lex();
            Statements();
            Expect('}');
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
            if (_lex._prevtoken != '}' && _lex._prevtoken != ';') OptionalSemicolon();
            //END_SCOPE();
        }
    }

    void IfStatement()
    {
        SQInteger jmppos;
        bool haselse = false;
        Lex();
        Expect('(');
        CommaExpr(true);
        Expect(')');
        _fs->AddInstruction(_OP_JZ, _fs->PopTarget());
        SQInteger jnepos = _fs->GetCurrentPos();
        BEGIN_SCOPE();

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
        Statement();
        //
        if(_token != '}' && _token != TK_ELSE && _token != TK_IDENTIFIER) OptionalSemicolon();

        END_SCOPE();
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
        Expect('(');
        CommaExpr(true);
        Expect(')');

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
        _fs->AddLineInfos(_lex._currentline, _lineinfo, true);
        Expect(TK_WHILE);
        SQInteger continuetrg = _fs->GetCurrentPos();
        Expect('(');
        CommaExpr(true);
        Expect(')');
        _fs->AddInstruction(_OP_JZ, _fs->PopTarget(), 1);
        _fs->AddInstruction(_OP_JMP, 0, jmptrg - _fs->GetCurrentPos() - 1);
        END_BREAKBLE_BLOCK(continuetrg);
    }

    void ForStatement()
    {
        Lex();
        BEGIN_SCOPE();
        Expect('(');
        switch(_token)
        {
        case TK_IDENTIFIER:
        //ignore custom types for now
        //fallthrough as local declaration
        //CASE_TK_LOCAL_TYPES:
        case TK_LOCAL:
            LocalDeclStatement();
            break;
        default:
            if(_token != ';')
            {
                CommaExpr();
                _fs->PopTarget();
            }
        }

        Expect(';');
        _fs->SnoozeOpt();
        SQInteger jmppos = _fs->GetCurrentPos();
        SQInteger jzpos = -1;
        if(_token != ';')
        {
            CommaExpr();
            _fs->AddInstruction(_OP_JZ, _fs->PopTarget());
            jzpos = _fs->GetCurrentPos();
        }
        Expect(';');
        _fs->SnoozeOpt();
        SQInteger expstart = _fs->GetCurrentPos() + 1;
        if(_token != ')')
        {
            CommaExpr();
            _fs->PopTarget();
        }
        Expect(')');
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
        END_SCOPE();

        END_BREAKBLE_BLOCK(continuetrg);
    }

    void ForEachStatement()
    {
        SQObject idxname, valname;
        Lex();
        Expect('(');
        valname = Expect(TK_IDENTIFIER);
        if(_token == ',')
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
        Expect(')');
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
        Expect('(');
        CommaExpr(true);
        Expect(')');
        Expect('{');
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
            Expect(':');
            SQInteger trg = _fs->PopTarget();
            SQInteger eqtarget = trg;
            bool vlocal = _fs->IsLocal(trg);
            if(vlocal)
            {
                eqtarget = _fs->PushTarget(); //we need to allocate a extra reg
            }
            _fs->AddInstruction(_OP_EQ, eqtarget, trg, expr);
            _fs->AddInstruction(_OP_JZ, eqtarget, 0);
            if(vlocal)
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
            Expect(':');
            BEGIN_SCOPE();
            Statements();
            END_SCOPE();
        }
        Expect('}');
        _fs->PopTarget();
        __nbreaks__ = _fs->_unresolvedbreaks.size() - __nbreaks__;
        if(__nbreaks__ > 0)ResolveBreaks(_fs, __nbreaks__);
        _fs->_breaktargets.pop_back();
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
        Expect('(');
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
            class_name = SQString_Create(_ss(_vm), _lex._svalue);
            CheckGlobalName(class_name, true);
            CheckTypeName(class_name, true); //to allow C/C++ style instance declarations
        }
        es = _es;
        _es.donot_get = true;
        PrefixedExpr();
        if(_token == ';') //class forward declaration
        {
            //return;
        }
        if(_es.etype == EXPR)
        {
            Error(_SC("invalid class name"));
        }
        else if(_es.etype == OBJECT || _es.etype == BASE)
        {
            ClassExp(/*&*/class_name);
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
        case TK_INTEGER:
            val._type = OT_INTEGER;
            val._unVal.nInteger = _lex._nvalue;
            break;
        case TK_FLOAT:
            val._type = OT_FLOAT;
            val._unVal.fFloat = _lex._fvalue;
            break;
        case TK_STRING_LITERAL:
            val = _fs->CreateString(_lex._svalue,_lex._longstr.size()-1);
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
            case TK_INTEGER:
                val._type = OT_INTEGER;
                val._unVal.nInteger = -_lex._nvalue;
                break;
            case TK_FLOAT:
                val._type = OT_FLOAT;
                val._unVal.fFloat = -_lex._fvalue;
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
        case TK_INTEGER:
            EmitLoadConstInt(_lex._nvalue,-1);
            break;
        case TK_FLOAT:
            EmitLoadConstFloat(_lex._fvalue,-1);
            break;
        case TK_STRING_LITERAL:
            _fs->AddInstruction(_OP_LOAD, _fs->PushTarget(), _fs->GetConstant(_fs->CreateString(_lex._svalue,_lex._longstr.size()-1)));
            break;
        case TK_IDENTIFIER:
        {
            SQObject id = _fs->CreateString(_lex._svalue);
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
            case TK_INTEGER:
                EmitLoadConstInt(-_lex._nvalue,-1);
                break;
            case TK_FLOAT:
                EmitLoadConstFloat(-_lex._fvalue,-1);
                break;
            default:
                Error(_SC("scalar expected : integer, float"));
            }
            break;
        default:
            //goto error;
        }
        Lex();
        return tk_type;
//error:
        Error(_SC("constant or scalar expected : integer, float or string"));
        return 0;
    }

    void EnumStatement()
    {
        Lex();
        SQObject id = Expect(TK_IDENTIFIER);
        Expect('{');

        //CheckLocalNameScope(id, _scope.nested);
        SQObjectPtr strongid = id;
        CheckLocalNameScope(id, _scope.nested);
        SQObject table = _fs->CreateTable();
        //_fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(),0,NOT_TABLE);
        SQInteger nval = 0;
        while(_token != '}')
        {
            SQObject key = Expect(TK_IDENTIFIER);
            SQObject val;
            if(_token == '=')
            {
                Lex();
                val = ExpectScalar();
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
        //if(_fs->_breaktargets.size()) _fs->_breaktargets.top()++;
        //if(_fs->_continuetargets.size()) _fs->_continuetargets.top()++;
        SQInteger trappos = _fs->GetCurrentPos();
        {
            BEGIN_SCOPE();
            Statement();
            END_SCOPE();
        }
        _fs->_traps--;
        _fs->AddInstruction(_OP_POPTRAP, 1, 0);
        //if(_fs->_breaktargets.size()) _fs->_breaktargets.top()--;
        //if(_fs->_continuetargets.size()) _fs->_continuetargets.top()--;
        _fs->AddInstruction(_OP_JMP, 0, 0);
        SQInteger jmppos = _fs->GetCurrentPos();
        _fs->SetIntructionParam(trappos, 1, (_fs->GetCurrentPos() - trappos));
        Expect(TK_CATCH);
        Expect('(');
        exid = Expect(TK_IDENTIFIER);
        Expect(')');
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
        Expect('(');
        SQObjectPtr dummy;
        CreateFunction(dummy, lambda ? eFunctionType_lambda : eFunctionType_anonymous);
        _fs->AddInstruction(_OP_CLOSURE, _fs->PushTarget(), _fs->_functions.size() - 1, ftype == TK_FUNCTION?0:1);
    }

    void ClassExp(SQObjectPtr_ptr_t class_name)
    {
        SQInteger vbase = -1;
        SQInteger attrs = -1;
        bool hasInheritance = false;
        switch(_token)
        {
        case ':': //C++ style class derivation
        case TK_EXTENDS: //squirrel style class derivation
            Lex();
            hasInheritance = true;
            switch(_token)
            {
            case TK_PRIVATE:
            case TK_PUBLIC:
                Lex(); //ignore, accepted only to compile a subset of C++
            }
        }
        if(hasInheritance)
        {
            Expression();
            vbase = _fs->TopTarget();
        }
        if(_token == TK_ATTR_OPEN)
        {
            Lex();
            _fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(),0,NOT_TABLE);
            ParseTableOrClass(',',TK_ATTR_CLOSE);
            attrs = _fs->TopTarget();
        }
        Expect('{');
        if(attrs != -1) _fs->PopTarget();
        if(vbase != -1) _fs->PopTarget();
        _fs->AddInstruction(_OP_NEWOBJ, _fs->PushTarget(), vbase, attrs,NOT_CLASS);
        ParseTableOrClass(';','}', class_name);
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

    void CreateFunction(SQObject &name, int ftype, int stack_offset=0)
    {
        SQFuncState_ptr_t funcstate = _fs->PushChildState(_ss(_vm));
        funcstate->_name = name;
        SQObject paramname, type_name;
        funcstate->AddParameter(_fs->CreateString(_SC("this")), _scope.nested+1);
        funcstate->_sourcename = _sourcename;
        SQInteger defparams = 0;
        bool is_reference_declaration = 0;
        /*const*/ SQChar_ptr_t param_type_name = 0;
        bool isVoid = false;
        while(_token!=')')
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
                if(_token != ')') Error(_SC("expected ')'"));
                break;
            }
            else
            {
                if(_token == TK_CONST) Lex(); //C/C++ const cualifiers for now
                switch(_token)
                {
                    //CASE_TK_LOCAL_TYPES: //accept C/C++ type parameter declarations
                    //        param_type_name = _lex.GetTokenName(_token);
                    //        Lex();
                }
                CHECK_REF_DECLARATION(_token);
                paramname = Expect(TK_IDENTIFIER);
                if(CheckNameIsType(paramname))
                {
                    //ignore custom types for now
                    param_type_name = _stringval(paramname);
                    CHECK_REF_DECLARATION(_token);
                    paramname = Expect(TK_IDENTIFIER);
                }
                funcstate->AddParameter(paramname, _scope.nested+1, is_reference_declaration ? _VAR_REFERENCE : _VAR_ANY);
                if(param_type_name)
                {
                    funcstate->AddParameterTypeName(param_type_name);
                }
                if(_token == '=')
                {
                    if(is_reference_declaration) Error(_SC("parameter passed by reference can't have default value"));
                    Lex();
                    if(_token == '[' || _token == '{') Error(_SC("default parameter with array/table values not supported"));
                    Expression();
                    //stack_offset to compensate for local functions been relocated to allow recursion
                    funcstate->AddDefaultParam(_fs->TopTarget()+stack_offset);
                    defparams++;
                }
                else if(_token == ':')
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
                if(_token == ',') Lex();
                else if(_token != ')') Error(_SC("expected ')' or ','"));
            }
        }
        Expect(')');
        switch(_token)
        {
        case ':':
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
                if(  (scstrcmp(_lex._svalue, _SC("final")) == 0) ||
                        (scstrcmp(_lex._svalue, _SC("override")) == 0))
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
            if(_token == ';') //to parse thinscript
            {
                _fs->PopChildState();
                return;
            }
            else _is_parsing_extern = false;
        }

        SQFuncState_ptr_t currchunk = _fs;
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
        funcstate->AddLineInfos(_lex._prevtoken == '\n'?_lex._lasttokenline:_lex._currentline, _lineinfo, true);
        funcstate->AddInstruction(_OP_RETURN, -1);
        funcstate->SetStackSize(0);

        SQFunctionProto *func = funcstate->BuildProto();
//#ifdef _DEBUG_DUMP
//		funcstate->Dump(func);
//#endif
        _fs = currchunk;
        _fs->_functions.push_back(func);
        _fs->PopChildState();
    }

    void ResolveBreaks(SQFuncState_ptr_t funcstate, SQInteger ntoresolve) {}

    void ResolveContinues(SQFuncState_ptr_t funcstate, SQInteger ntoresolve, SQInteger targetpos) {}
//private:
};

string_t txt = readfile("test-raii.nut")
               var lex = new SQLexer(txt, true /*we want comments*/);

var first_enum_token = lex.first_enum_token();
var last_enum_token = lex.last_enum_token();

var nest_level = 0;
var tok =  lex.lex(); //lex.lex();
while(tok > 0)
{
    local tkstr;
    if(tok > first_enum_token && tok < last_enum_token) tkstr = lex.tok2str(tok); //lex.tok2str(tok);
    else tkstr = tok.tochar();
    /*
    print(
    	tok,
    	tkstr || "",
    	lex.token_name(tok), //lex.token_name(tok),
    	lex.svalue() || "",
    	lex.lasttokenline(),
    	lex.lasttokencolumn(),
    	lex.currentline(),
    	lex.currentcolumn()
    	);
    */

    switch(tok)
    {
    case '{':
    case '}':
    default:
        print(tkstr);
    }

    tok = lex.lex();
}
