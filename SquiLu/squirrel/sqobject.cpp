/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#include "sqvm.h"
#include "sqstring.h"
#include "sqarray.h"
#include "sqtable.h"
#include "squserdata.h"
#include "sqfuncproto.h"
#include "sqclass.h"
#include "sqclosure.h"


const SQChar *IdType2Name(SQObjectType type)
{
	switch(_RAW_TYPE(type))
	{
	case _RT_NULL:return _SC("null");
	case _RT_INTEGER:return _SC("integer");
	case _RT_FLOAT:return _SC("float");
	case _RT_BOOL:return _SC("bool");
	case _RT_STRING:return _SC("string");
	case _RT_TABLE:return _SC("table");
	case _RT_ARRAY:return _SC("array");
	case _RT_GENERATOR:return _SC("generator");
	case _RT_CLOSURE:
	case _RT_NATIVECLOSURE:
		return _SC("function");
	case _RT_USERDATA:
	case _RT_USERPOINTER:
		return _SC("userdata");
	case _RT_THREAD: return _SC("thread");
	case _RT_FUNCPROTO: return _SC("function");
	case _RT_CLASS: return _SC("class");
	case _RT_INSTANCE: return _SC("instance");
	case _RT_WEAKREF: return _SC("weakref");
	case _RT_OUTER: return _SC("outer");
	default:
		return NULL;
	}
}

const SQChar *GetTypeName(const SQObjectPtr &obj1)
{
	return IdType2Name(sq_type(obj1));
}

SQObjectPtr SQObjectPtr::operator[](SQInteger nidx) {
    SQObjectPtr val;
    if(sq_type(*this) == OT_ARRAY) _array(*this)->Get(nidx, val);
    return val;
}

SQObjectPtr SQObjectPtr::operator[](const SQChar *key) {
    SQObjectPtr val;
    switch(sq_type(*this))
    {
        case OT_TABLE: _table(*this)->Get(key, val); break;
        case OT_CLASS: _class(*this)->Get(key, val); break;
        case OT_INSTANCE: _instance(*this)->Get(key, val); break;
	default:
		break;
    }
    return val;
}

SQString *SQString::Create(SQSharedState *ss,const SQChar *s,SQInteger len)
{
	SQString *str=ADD_STRING(ss,s,len);
	return str;
}

void SQString::Release()
{
	REMOVE_STRING(_sharedstate,this);
}

SQInteger SQString::Next(const SQObjectPtr &refpos, SQObjectPtr &outkey, SQObjectPtr &outval)
{
	SQInteger idx = (SQInteger)SQTranslateIndex(refpos);
	while(idx < _len){
		outkey = (SQInteger)idx;
		outval = (SQInteger)((SQUnsignedInteger)_val[idx]);
		//return idx for the next iteration
		return ++idx;
	}
	//nothing to iterate anymore
	return -1;
}

SQUnsignedInteger SQTranslateIndex(const SQObjectPtr &idx)
{
	switch(sq_type(idx)){
		case OT_NULL:
			return 0;
		case OT_INTEGER:
			return (SQUnsignedInteger)_integer(idx);
		default: assert(0); break;
	}
	return 0;
}

SQWeakRef *SQRefCounted::GetWeakRef(SQObjectType type)
{
    if(!_weakref) {
        sq_new(_weakref,SQWeakRef);
#if defined(SQUSEDOUBLE) && !defined(_SQ64)
        _weakref->_obj._unVal.raw = 0; //clean the whole union on 32 bits with double
#endif
        _weakref->_obj._type = type;
        _weakref->_obj._unVal.pRefCounted = this;
    }
    return _weakref;
}

SQRefCounted::~SQRefCounted()
{
	if(_weakref) {
		_weakref->_obj._type = OT_NULL;
		_weakref->_obj._unVal.pRefCounted = NULL;
	}
}

void SQWeakRef::Release() {
	if(ISREFCOUNTED(_obj._type)) {
		_obj._unVal.pRefCounted->_weakref = NULL;
	}
	sq_delete(this,SQWeakRef);
}

bool SQDelegable::GetMetaMethod(SQVM *v,SQMetaMethod mm,SQObjectPtr &res) {
	if(_delegate) {
		return _delegate->Get((*_ss(v)->_metamethods)[mm],res);
	}
	return false;
}

bool SQDelegable::SetDelegate(SQTable *mt)
{
	SQTable *temp = mt;
	if(temp == this) return false;
	while (temp) {
		if (temp->_delegate == this) return false; //cycle detected
		temp = temp->_delegate;
	}
	if (mt)	__ObjAddRef(mt);
	__ObjRelease(_delegate);
	_delegate = mt;
	return true;
}

bool SQGenerator::Yield(SQVM *v,SQInteger target)
{
	if(_state==eSuspended) { v->Raise_Error(_SC("internal vm error, yielding dead generator"));  return false;}
	if(_state==eDead) { v->Raise_Error(_SC("internal vm error, yielding a dead generator")); return false; }
	SQInteger size = v->_top-v->_stackbase;

	_stack.resize(size);
	SQObject _this = v->_stack[v->_stackbase];
	_stack._vals[0] = ISREFCOUNTED(sq_type(_this)) ? SQObjectPtr(_refcounted(_this)->GetWeakRef(sq_type(_this))) : _this;
	for(SQInteger n =1; n<target; n++) {
		_stack._vals[n] = v->_stack[v->_stackbase+n];
	}
	for(SQInteger j =0; j < size; j++)
	{
		v->_stack[v->_stackbase+j].Null();
	}

	_ci = *v->ci;
	_ci._generator=NULL;
	for(SQInteger i=0;i<_ci._etraps;i++) {
		_etraps.push_back(v->_etraps.top());
		v->_etraps.pop_back();
		// store relative stack base and size in case of resume to other _top
		SQExceptionTrap &et = _etraps.back();
		et._stackbase -= v->_stackbase;
		et._stacksize -= v->_stackbase;
	}
	_state=eSuspended;
	return true;
}

bool SQGenerator::Resume(SQVM *v,SQObjectPtr &dest)
{
	if(_state==eDead){ v->Raise_Error(_SC("resuming dead generator")); return false; }
	if(_state==eRunning){ v->Raise_Error(_SC("resuming active generator")); return false; }
	SQInteger size = _stack.size();
	SQInteger target = &dest - &(v->_stack._vals[v->_stackbase]);
	assert(target>=0 && target<=255);
	SQInteger newbase = v->_top;
	if(!v->EnterFrame(v->_top, v->_top + size, false))
		return false;
	v->ci->_generator   = this;
	v->ci->_target      = (SQInt32)target;
	v->ci->_closure     = _ci._closure;
	v->ci->_ip          = _ci._ip;
	v->ci->_literals    = _ci._literals;
	v->ci->_ncalls      = _ci._ncalls;
	v->ci->_etraps      = _ci._etraps;
	v->ci->_root        = _ci._root;


	for(SQInteger i=0;i<_ci._etraps;i++) {
		v->_etraps.push_back(_etraps.top());
		_etraps.pop_back();
		SQExceptionTrap &et = v->_etraps.back();
		// restore absolute stack base and size
		et._stackbase += newbase;
		et._stacksize += newbase;
	}
	SQObject _this = _stack._vals[0];
	v->_stack[v->_stackbase] = sq_type(_this) == OT_WEAKREF ? _weakref(_this)->_obj : _this;

	for(SQInteger n = 1; n<size; n++) {
		v->_stack[v->_stackbase+n] = _stack._vals[n];
		_stack._vals[n].Null();
	}

	_state=eRunning;
	if (v->_debughook)
		v->CallDebugHook(_SC('c'));

	return true;
}

void SQArray::Extend(const SQArray *a){
	SQInteger xlen;
	if((xlen=a->Size()))
		for(SQInteger i=0;i<xlen;i++)
			Append(a->_values[i]);
}

const SQChar* SQFunctionProto::GetLocal(SQVM *vm,SQUnsignedInteger stackbase,SQUnsignedInteger nseq,SQUnsignedInteger nop)
{
	SQUnsignedInteger nvars=_nlocalvarinfos;
	const SQChar *res=NULL;
	if(nvars>=nseq){
 		for(SQUnsignedInteger i=0;i<nvars;i++){
			if(_localvarinfos[i]._start_op<=nop && _localvarinfos[i]._end_op>=nop)
			{
				if(nseq==0){
					vm->Push(vm->_stack[stackbase+_localvarinfos[i]._pos]);
					res=_stringval(_localvarinfos[i]._name);
					break;
				}
				nseq--;
			}
		}
	}
	return res;
}


SQInteger SQFunctionProto::GetLine(SQInstruction *curr)
{
	SQInteger op = (SQInteger)(curr-_instructions);
    SQInteger line=_lineinfos[0]._line;
    SQInteger low = 0;
    SQInteger high = _nlineinfos - 1;
    SQInteger mid = 0;
    while(low <= high)
    {
        mid = low + ((high - low) >> 1);
        SQInteger curop = _lineinfos[mid]._op;
        if(curop > op)
        {
            high = mid - 1;
        }
        else if(curop < op) {
            if(mid < (_nlineinfos - 1)
                && _lineinfos[mid + 1]._op >= op) {
                break;
            }
            low = mid + 1;
        }
        else { //equal
            break;
        }
    }

    while(mid > 0 && _lineinfos[mid]._op >= op) mid--; //I've changed this one

    line = _lineinfos[mid]._line;

    return line;
}

SQClosure::~SQClosure()
{
    __ObjRelease(_root);
    __ObjRelease(_env);
    __ObjRelease(_base);
    REMOVE_FROM_CHAIN(&_ss(this)->_gc_chain,this);
}

#define _CHECK_IO(exp)  { if(!exp)return false; }
static bool SafeWrite(HSQUIRRELVM v,SQWRITEFUNC write,SQUserPointer up,SQUserPointer dest,SQInteger size)
{
	if(write(up,dest,size) != size) {
		v->Raise_Error(_SC("io error (write function failure)"));
		return false;
	}
	return true;
}

static bool SafeWriteFmt(HSQUIRRELVM v,SQWRITEFUNC write,SQUserPointer up, const SQChar *fmt, ...)
{
    if(fmt){
        SQChar str[8192];
        va_list vl;
        va_start(vl, fmt);
        SQInteger len = scvsprintf(str, sizeof(str), fmt, vl);
        va_end(vl);
        return SafeWrite(v, write, up, str, len);
    }
	return false;
}

static bool SafeRead(HSQUIRRELVM v,SQWRITEFUNC read,SQUserPointer up,SQUserPointer dest,SQInteger size)
{
	if(size && read(up,dest,size) != size) {
		v->Raise_Error(_SC("io error, read function failure, the origin stream could be corrupted/trucated"));
		return false;
	}
	return true;
}

static bool WriteTag(HSQUIRRELVM v,SQWRITEFUNC write,SQUserPointer up,SQUnsignedInteger32 tag)
{
	return SafeWrite(v,write,up,&tag,sizeof(tag));
}

static bool CheckTag(HSQUIRRELVM v,SQWRITEFUNC read,SQUserPointer up,SQUnsignedInteger32 tag)
{
	SQUnsignedInteger32 t;
	_CHECK_IO(SafeRead(v,read,up,&t,sizeof(t)));
	if(t != tag){
		v->Raise_Error(_SC("invalid or corrupted closure stream"));
		return false;
	}
	return true;
}

static bool WriteObjectAsCode(HSQUIRRELVM v,SQUserPointer up,SQWRITEFUNC write,SQObjectPtr &o, bool withQuotes=true)
{
	SQChar buf[32];
	SQInteger sz;
	switch(sq_type(o)){
	case OT_STRING:{
            SQInteger str_size = _string(o)->_len;
            if(str_size){
                const SQChar *str = _stringval(o);
                bool needQuote = false;
                for(int i=0; i < str_size; ++i){
                    if(str[i] == '"' || str[i] == '\n'){
                        needQuote = true;
                        break;
                    }
                }
                if(!needQuote) withQuotes = true;
                const SQChar *d1 = withQuotes ? _SC("\"") : _SC("[==[");
                const SQChar *d2 = withQuotes ? _SC("\"") : _SC("]==]");
                _CHECK_IO(SafeWrite(v,write,up, (void*)d1, scstrlen(d1)));
                _CHECK_IO(SafeWrite(v,write,up, (void*)str,rsl(_string(o)->_len)));
                _CHECK_IO(SafeWrite(v,write,up, (void*)d2, scstrlen(d2)));
            }
            else
            {
                const SQChar *str = _SC("null");
                _CHECK_IO(SafeWrite(v,write,up, (void*)str, scstrlen(str)));
            }
        }
		break;
	case OT_INTEGER:
        sz = scsprintf(buf, sizeof(buf), _PRINT_INT_FMT, _integer(o));
		_CHECK_IO(SafeWrite(v,write,up,buf,sz));break;
	case OT_FLOAT:
        sz = scsprintf(buf, sizeof(buf), "%f", _float(o));
		_CHECK_IO(SafeWrite(v,write,up,buf,sz));break;
	case OT_NULL:{
            const SQChar str[] = _SC("null");
            _CHECK_IO(SafeWrite(v,write,up, (void*)str, scstrlen(str)));
        }
		break;
	default:
		v->Raise_Error(_SC("cannot serialize a %s"),GetTypeName(o));
		return false;
	}
	return true;
}

static bool WriteObject(HSQUIRRELVM v,SQUserPointer up,SQWRITEFUNC write,SQObjectPtr &o)
{
	SQUnsignedInteger32 _type = (SQUnsignedInteger32)sq_type(o);
	_CHECK_IO(SafeWrite(v,write,up,&_type,sizeof(_type)));
	switch(sq_type(o)){
	case OT_STRING:
		_CHECK_IO(SafeWrite(v,write,up,&_string(o)->_len,sizeof(SQInteger)));
		_CHECK_IO(SafeWrite(v,write,up,_stringval(o),rsl(_string(o)->_len)));
		break;
	case OT_BOOL:
	case OT_INTEGER:
		_CHECK_IO(SafeWrite(v,write,up,&_integer(o),sizeof(SQInteger)));break;
	case OT_FLOAT:
		_CHECK_IO(SafeWrite(v,write,up,&_float(o),sizeof(SQFloat)));break;
	case OT_NULL:
		break;
	default:
		v->Raise_Error(_SC("cannot serialize a %s"),GetTypeName(o));
		return false;
	}
	return true;
}

static bool ReadObject(HSQUIRRELVM v,SQUserPointer up,SQREADFUNC read,SQObjectPtr &o)
{
	SQUnsignedInteger32 _type;
	_CHECK_IO(SafeRead(v,read,up,&_type,sizeof(_type)));
	SQObjectType t = (SQObjectType)_type;
	switch(t){
	case OT_STRING:{
		SQInteger len;
		_CHECK_IO(SafeRead(v,read,up,&len,sizeof(SQInteger)));
		_CHECK_IO(SafeRead(v,read,up,_ss(v)->GetScratchPad(sq_rsl(len)),sq_rsl(len)));
		o=SQString::Create(_ss(v),_ss(v)->GetScratchPad(-1),len);
				   }
		break;
	case OT_INTEGER:{
		SQInteger i;
		_CHECK_IO(SafeRead(v,read,up,&i,sizeof(SQInteger))); o = i; break;
					}
	case OT_BOOL:{
		SQInteger i;
		_CHECK_IO(SafeRead(v,read,up,&i,sizeof(SQInteger))); o._type = OT_BOOL; o._unVal.nInteger = i; break;
					}
	case OT_FLOAT:{
		SQFloat f;
		_CHECK_IO(SafeRead(v,read,up,&f,sizeof(SQFloat))); o = f; break;
				  }
	case OT_NULL:
		o.Null();
		break;
	default:
		v->Raise_Error(_SC("cannot serialize a %s"),IdType2Name(t));
		return false;
	}
	return true;
}

bool SQClosure::Save(SQVM *v,SQUserPointer up,SQWRITEFUNC write)
{
	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_HEAD));
	_CHECK_IO(WriteTag(v,write,up,sizeof(SQChar)));
	_CHECK_IO(WriteTag(v,write,up,sizeof(SQInteger)));
	_CHECK_IO(WriteTag(v,write,up,sizeof(SQFloat)));
	_CHECK_IO(_function->Save(v,up,write));
	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_TAIL));
	return true;
}

bool SQClosure::SaveAsSource(SQVM *v,SQUserPointer up,SQWRITEFUNC write)
{
	_CHECK_IO(_function->SaveAsSource(v,up,write));
	return true;
}

bool SQClosure::Load(SQVM *v,SQUserPointer up,SQREADFUNC read,SQObjectPtr &ret)
{
	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_HEAD));
	_CHECK_IO(CheckTag(v,read,up,sizeof(SQChar)));
	_CHECK_IO(CheckTag(v,read,up,sizeof(SQInteger)));
	_CHECK_IO(CheckTag(v,read,up,sizeof(SQFloat)));
	SQObjectPtr func;
	_CHECK_IO(SQFunctionProto::Load(v,up,read,func));
	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_TAIL));
    ret = SQClosure::Create(_ss(v),_funcproto(func),_table(v->_roottable)->GetWeakRef(OT_TABLE));
    //FIXME: load an root for this closure
	return true;
}

SQFunctionProto::SQFunctionProto(SQSharedState *ss)
{
	_stacksize=0;
	_bgenerator=false;
	INIT_CHAIN();ADD_TO_CHAIN(&_ss(this)->_gc_chain,this);
}

SQFunctionProto::~SQFunctionProto()
{
	REMOVE_FROM_CHAIN(&_ss(this)->_gc_chain,this);
}

bool SQFunctionProto::Save(SQVM *v,SQUserPointer up,SQWRITEFUNC write)
{
	SQInteger i,nliterals = _nliterals,nparameters = _nparameters;
	SQInteger noutervalues = _noutervalues,nlocalvarinfos = _nlocalvarinfos;
	SQInteger nlineinfos=_nlineinfos,ninstructions = _ninstructions,nfunctions=_nfunctions;
	SQInteger ndefaultparams = _ndefaultparams;
	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(WriteObject(v,up,write,_sourcename));
	_CHECK_IO(WriteObject(v,up,write,_name));
	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeWrite(v,write,up,&nliterals,sizeof(nliterals)));
	_CHECK_IO(SafeWrite(v,write,up,&nparameters,sizeof(nparameters)));
	_CHECK_IO(SafeWrite(v,write,up,&noutervalues,sizeof(noutervalues)));
	_CHECK_IO(SafeWrite(v,write,up,&nlocalvarinfos,sizeof(nlocalvarinfos)));
	_CHECK_IO(SafeWrite(v,write,up,&nlineinfos,sizeof(nlineinfos)));
	_CHECK_IO(SafeWrite(v,write,up,&ndefaultparams,sizeof(ndefaultparams)));
	_CHECK_IO(SafeWrite(v,write,up,&ninstructions,sizeof(ninstructions)));
	_CHECK_IO(SafeWrite(v,write,up,&nfunctions,sizeof(nfunctions)));
	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	for(i=0;i<nliterals;i++){
		_CHECK_IO(WriteObject(v,up,write,_literals[i]));
	}

	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	for(i=0;i<nparameters;i++){
		_CHECK_IO(WriteObject(v,up,write,_parameters[i]));
	}

	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	for(i=0;i<noutervalues;i++){
		_CHECK_IO(SafeWrite(v,write,up,&_outervalues[i]._type,sizeof(SQUnsignedInteger)));
		_CHECK_IO(WriteObject(v,up,write,_outervalues[i]._src));
		_CHECK_IO(WriteObject(v,up,write,_outervalues[i]._name));
	}

	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	for(i=0;i<nlocalvarinfos;i++){
		SQLocalVarInfo &lvi=_localvarinfos[i];
		_CHECK_IO(WriteObject(v,up,write,lvi._name));
		_CHECK_IO(SafeWrite(v,write,up,&lvi._pos,sizeof(SQUnsignedInteger)));
		_CHECK_IO(SafeWrite(v,write,up,&lvi._start_op,sizeof(SQUnsignedInteger)));
		_CHECK_IO(SafeWrite(v,write,up,&lvi._end_op,sizeof(SQUnsignedInteger)));
	}

	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeWrite(v,write,up,_lineinfos,sizeof(SQLineInfo)*nlineinfos));

	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeWrite(v,write,up,_defaultparams,sizeof(SQInteger)*ndefaultparams));

	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeWrite(v,write,up,_instructions,sizeof(SQInstruction)*ninstructions));

	_CHECK_IO(WriteTag(v,write,up,SQ_CLOSURESTREAM_PART));
	for(i=0;i<nfunctions;i++){
		_CHECK_IO(_funcproto(_functions[i])->Save(v,up,write));
	}
	_CHECK_IO(SafeWrite(v,write,up,&_stacksize,sizeof(_stacksize)));
	_CHECK_IO(SafeWrite(v,write,up,&_bgenerator,sizeof(_bgenerator)));
	_CHECK_IO(SafeWrite(v,write,up,&_varparams,sizeof(_varparams)));
	return true;
}

const SQChar *SQGetBitwiseOpName(int it){
#define SCASE(x) case x: return _SC(#x); break;
    switch(it){
        SCASE(BW_AND)
        SCASE(BW_OR)
        SCASE(BW_XOR)
        SCASE(BW_SHIFTL)
        SCASE(BW_SHIFTR)
        SCASE(BW_USHIFTR)
        default: return _SC("?");
    }
#undef SCASE
}

const SQChar *SQGetCmpOpName(int it){
#define SCASE(x) case x: return _SC(#x); break;
    switch(it){
        SCASE(CMP_G)
        SCASE(CMP_GE)
        SCASE(CMP_L)
        SCASE(CMP_LE)
        SCASE(CMP_3W)
        default: return _SC("?");
    }
#undef SCASE
}

const SQChar *SQGetArrayAppendTypeName(int it){
#define SCASE(x) case x: return _SC(#x); break;
    switch(it){
        SCASE(AAT_STACK)
        SCASE(AAT_LITERAL)
        SCASE(AAT_INT)
        SCASE(AAT_FLOAT)
        SCASE(AAT_BOOL)
        default: return _SC("?");
    }
#undef SCASE
}

const SQChar *SQGetNewObjTypeName(int it){
#define SCASE(x) case x: return _SC(#x); break;
    switch(it){
        SCASE(NOT_TABLE)
        SCASE(NOT_ARRAY)
        SCASE(NOT_CLASS)
        default: return _SC("?");
    }
#undef SCASE
}

const SQChar *SQGetArithOpName(int it){
#define SCASE(x, z) case x: return _SC(#z); break;
    switch(it){
        SCASE(_OP_ADD, +)
        SCASE(_OP_SUB, -)
        SCASE(_OP_MUL, *)
        SCASE(_OP_DIV, /)
        SCASE(_OP_MOD, %)
        default: return _SC("?");
    }
#undef SCASE
}

const SQChar *SQGetOpName(int op_code) {
	const SQChar *str_op;

    switch(op_code){
#define ENUM_OP(a,b) case a: str_op = _SC(#a); break;
        SQ_OP_CODE_LIST()
#undef ENUM_OP
        default:
            str_op = _SC("???");
    }
    return str_op;
}

bool SQFunctionProto::SaveAsSource(SQVM *v,SQUserPointer up,SQWRITEFUNC write)
{
	SQInteger i,nliterals = _nliterals,nparameters = _nparameters;
	SQInteger noutervalues = _noutervalues,nlocalvarinfos = _nlocalvarinfos;
	SQInteger nlineinfos=_nlineinfos,ninstructions = _ninstructions,nfunctions=_nfunctions;
	SQInteger ndefaultparams = _ndefaultparams;
	SafeWriteFmt(v,write,up,"{\n");
	SafeWriteFmt(v,write,up,"\tsource_name = ");
	_CHECK_IO(WriteObjectAsCode(v,up,write,_sourcename));
	SafeWriteFmt(v,write,up,",\n\tfunction_name = ");
	_CHECK_IO(WriteObjectAsCode(v,up,write,_name));
	SafeWriteFmt(v,write,up,"\n");

	SafeWriteFmt(v,write,up,"\tliterals = [\n");
	for(i=0;i<nliterals;i++){
		SafeWriteFmt(v,write,up,"\t\t/*%d*/ ", i);
		_CHECK_IO(WriteObjectAsCode(v,up,write,_literals[i], false));
		SafeWriteFmt(v,write,up,",\n");
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tparameters = [\n");
	for(i=0;i<nparameters;i++){
		SafeWriteFmt(v,write,up,"\t\t/*%d*/ ", i);
		_CHECK_IO(WriteObjectAsCode(v,up,write,_parameters[i]));
		SafeWriteFmt(v,write,up,",\n");
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\toutervalues = [\n\t\t//[type, src, name],\n");
	for(i=0;i<noutervalues;i++){
		SafeWriteFmt(v,write,up,"\t\t/*%d*/ [%d, ", i, _outervalues[i]._type);
		_CHECK_IO(WriteObjectAsCode(v,up,write,_outervalues[i]._src));
		SafeWriteFmt(v,write,up,", ");
		_CHECK_IO(WriteObjectAsCode(v,up,write,_outervalues[i]._name));
		SafeWriteFmt(v,write,up,"],\n");
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tlocalvarinfos = [\n\t\t//[pos, name, start_op, end_op, scope, type],\n");
	for(i=0;i<nlocalvarinfos;i++){
	    SQLocalVarInfo &lvi=_localvarinfos[i];
		SafeWriteFmt(v,write,up,"\t\t/*%d*/ [", i);
		SafeWriteFmt(v,write,up,"%d, ", lvi._pos);
		_CHECK_IO(WriteObjectAsCode(v,up,write,lvi._name));
		SafeWriteFmt(v,write,up,", %d", lvi._start_op);
		SafeWriteFmt(v,write,up,", %d", lvi._end_op);
		SafeWriteFmt(v,write,up,", %d", lvi._scope);
		SafeWriteFmt(v,write,up,", %d],\n", lvi._type);
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tlineinfos = [\n\t\t//[op, line],\n");
	for(i=0;i<nlineinfos;i++){
	    SQLineInfo &li=_lineinfos[i];
		SafeWriteFmt(v,write,up,"\t\t/*%d*/ [%d, %d], /*%s*/\n", i, li._op, li._line, SQGetOpName(li._op));
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tdefaultparams = [");
	for(i=0;i<ndefaultparams;i++){
		SafeWriteFmt(v,write,up,"%d,", _defaultparams[i]);
	}
    SafeWriteFmt(v,write,up,"],\n");

	SafeWriteFmt(v,write,up,"\tinstructions = [ /*stack*/\n");
    SafeWriteFmt(v,write,up,"\t\t//[op_str, op, arg0, arg1, arg2, arg3],\n");
	const SQChar *str_op;
	for(i=0;i<ninstructions;i++){
	    SQInstruction &inst = _instructions[i];
        str_op = SQGetOpName(inst.op);
		SafeWriteFmt(v,write,up,"\t\t/*%d*/ [\"%s\", %d, %d, %d, %d, %d],", i, str_op, inst.op, inst._arg0, inst._arg1, inst._arg2, inst._arg3);

        switch(inst.op){
            case _OP_LOAD:
            case _OP_DLOAD:
            case _OP_GETK:{
                SQInteger lidx = inst._arg1;
                if(lidx >= 0xFFFFFFFF) SafeWriteFmt(v,write,up,"\t\t/* stk[%d] <- null */", inst._arg0);
                else
                {
                    SafeWriteFmt(v,write,up,"\t\t/* stk[%d] <- literals[%d] */", inst._arg0, lidx);
                    //_CHECK_IO(WriteObjectAsCode(v,up,write,_literals[lidx], false));
                    //SafeWriteFmt(v,write,up," */");
                }
                if(inst.op == _OP_DLOAD) {
                    lidx = inst._arg3;
                    if(lidx >= 0xFFFFFFFF)  SafeWriteFmt(v,write,up," /* stk[%d] <- null */", inst._arg2);
                    else {
                        SafeWriteFmt(v,write,up," /* stk[%d] <- literals[%d] ", inst._arg2, lidx);
                        _CHECK_IO(WriteObjectAsCode(v,up,write,_literals[lidx], false));
                        SafeWriteFmt(v,write,up," */");
                    }
                }
            }
            break;
            case _OP_GET:
                    SafeWriteFmt(v,write,up,"\t/* stk_at_arg0[%d] = stk_at_arg1[%d].get(stk_at_arg2(%d)) */",
                                 inst._arg0, inst._arg1, inst._arg2);
            break;
            case _OP_SET:
                    SafeWriteFmt(v,write,up,"\t/* stk_at_arg1[%d].set(stk_at_arg2(%d), stk_at_arg3(%d)) */",
                                 inst._arg1, inst._arg2, inst._arg3);
            break;
            case _OP_PREPCALLK:
            case _OP_PREPCALL:
                    SafeWriteFmt(v,write,up,"\t/* closure_at_stk[%d], stk[%d].get(%s[%d]) -> stk[%d] */",
                                 inst._arg0, inst._arg2, inst.op == _OP_PREPCALLK ? "literals" : "stk", inst._arg1, inst._arg3);
            break;
            case _OP_LOADFLOAT:
                    SafeWriteFmt(v,write,up,"\t/* %f */", *((SQFloat*)&inst._arg1));
            break;
            case _OP_GETOUTER:
                        SafeWriteFmt(v,write,up,"\t/* stk[%d] <- outervalues[%d] == (%s) */",
                                     inst._arg0, inst._arg1, _stringval(_outervalues[inst._arg1]._name));
            break;
            case _OP_CALL:
                        SafeWriteFmt(v,write,up,"\t\t/* target[%d], closure_at_stk[%d], stackbase(%d), nargs(%d) */",
                                     inst._arg0, inst._arg1, inst._arg2, inst._arg3);
            break;
            case _OP_MOVE:
                        SafeWriteFmt(v,write,up,"\t\t/* stk[%d] <- stk[%d] */", inst._arg0, inst._arg1);
            break;
            case _OP_DMOVE:
                        SafeWriteFmt(v,write,up,"\t/* stk[%d] <- stk[%d], stk[%d] <- stk[%d] */",
                                     inst._arg0, inst._arg1, inst._arg2, inst._arg3);
            break;
            case _OP_LOADINT:
                        SafeWriteFmt(v,write,up,"\t/* stk[%d] <- arg1(%d) */", inst._arg0, inst._arg1);
            break;
            case _OP_EQ:
                        SafeWriteFmt(v,write,up,"\t\t/* stk_at_arg0[%d] = stk_at_arg2[%d] == (arg3(%d) !=0 ? literals_at_arg1[%d] : stk_at_arg1[%d]) */",
                                     inst._arg0, inst._arg2, inst._arg3, inst._arg1, inst._arg1);
            break;
            case _OP_NOT:
                        SafeWriteFmt(v,write,up,"\t\t/* stk_at_arg0[%d] = IsFalse(stk_at_arg1[%d]) */",
                                     inst._arg0, inst._arg1);
            break;
            case _OP_AND:
            case _OP_OR:
                        SafeWriteFmt(v,write,up,"\t\t/* %s(stk_at_arg2[%d]) {stk_at_arg0[%d] = stk[%d]; goto[%d]} */",
                                     _OP_OR ? "IsTrue" : "IsFalse",
                                     inst._arg2, inst._arg0, inst._arg2, i + inst._arg1 + 1);
            break;
            case _OP_JZ:
                        SafeWriteFmt(v,write,up,"\t\t/* IsFalse(STK(%d) (ci->_ip+=(%d) -> goto[%d]) */",
                                     inst._arg0, inst._arg1, i + inst._arg1 + 1);
            break;
            case _OP_JCMP:
                        SafeWriteFmt(v,write,up,"\t\t/* IsFalse(STK(%d) %s STK(%d)) (ci->_ip+=(%d) -> goto[%d]) */",
                                     inst._arg2, SQGetCmpOpName(inst._arg3), inst._arg0, inst._arg1, i + inst._arg1 + 1);
            break;
            case _OP_JMP:
                        SafeWriteFmt(v,write,up,"\t\t/* (ci->_ip+=(%d)) -> goto[%d] */",
                                     inst._arg1, i + inst._arg1 + 1);
            break;
            case _OP_RETURN:
                        SafeWriteFmt(v,write,up,"\t/* _arg0(%d) != 255 ? stk[%d] : null */",
                                     inst._arg0, inst._arg1);
            break;
            case _OP_NEWOBJ:
                        SafeWriteFmt(v,write,up,"\t/* stk[%d], len(%d), %s(%d) */",
                                     inst._arg0, inst._arg1, SQGetNewObjTypeName(inst._arg3), inst._arg3);
            break;
            case _OP_APPENDARRAY:
                        SafeWriteFmt(v,write,up,"\t/* array_at_stk(%d), %s(%d), sq_type(%d) */",
                                     inst._arg0, SQGetArrayAppendTypeName(inst._arg2), inst._arg1, inst._arg2);
            break;
            case _OP_NEWSLOT:
            case _OP_NEWSLOTA:
                        SafeWriteFmt(v,write,up,"\t/* flags(%d), table_at_stk(%d),  key_at_stk(%d), val_at_stk(%d) */",
                                     inst._arg0, inst._arg1, inst._arg2, inst._arg3);
            break;
            case _OP_LOADBOOL:
                        SafeWriteFmt(v,write,up,"\t/* stk[%d] <- bool(%d) */",
                                     inst._arg0, inst._arg2);
            break;
            case _OP_LOADNULLS:
                        SafeWriteFmt(v,write,up,"\t/* stk_at_arg0[%d .. (arg0 + arg1(%d))] = null */",
                                     inst._arg0, inst._arg1);
            break;
            case _OP_LOADROOT:
                        SafeWriteFmt(v,write,up,"\t/* stk_at_arg0[%d] <- roottable */",
                                     inst._arg0);
            break;
            case _OP_CLOSURE:
                        SafeWriteFmt(v,write,up,"\t/* stk_at_arg0[%d] <- functions[%d] == (",
                                     inst._arg0, inst._arg1);
                        _CHECK_IO(WriteObjectAsCode(v,up,write,_funcproto(_functions[inst._arg1])->_name));
                        SafeWriteFmt(v,write,up,"), isLanbda(%d) */", inst._arg2);
            break;
            case _OP_ADD:
            case _OP_SUB:
            case _OP_DIV:
            case _OP_MUL:
            case _OP_MOD:
                        SafeWriteFmt(v,write,up,"\t\t/* stk[%d] = stk[%d] %s stk[%d] */",
                                     inst._arg0, inst._arg1, SQGetArithOpName(inst.op), inst._arg2);
            break;
            case _OP_INCL:
                        SafeWriteFmt(v,write,up,"\t\t/* stk[%d] = stk[%d] + sarg3(%d) */",
                                     inst._arg1, inst._arg1, ((char)inst._arg3));
            break;
            case _OP_PINCL:
                        SafeWriteFmt(v,write,up,"\t\t/* target = stk(%d); stk[%d] = stk[%d] + sarg3(%d) */",
                                     inst._arg1, inst._arg1, inst._arg1, ((char)inst._arg3));
            break;
            case _OP_BITW:
                        SafeWriteFmt(v,write,up,"\t\t/* stk[%d] = stk[%d] %s stk[%d] */",
                                     inst._arg0, inst._arg1, SQGetBitwiseOpName(inst._arg3), inst._arg2);
            break;
            //default:
        }
        SafeWriteFmt(v,write,up,"\n");
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tfunctions = [\n");
	for(i=0;i<nfunctions;i++){
	    SafeWriteFmt(v,write,up,"/*function %d*/", i);
	    _CHECK_IO(_funcproto(_functions[i])->SaveAsSource(v,up,write));
	    SafeWriteFmt(v,write,up,",\n");
	}
    SafeWriteFmt(v,write,up,"],\n");

	SafeWriteFmt(v,write,up,"\tstacksize = %d,\n", _stacksize);
	SafeWriteFmt(v,write,up,"\tbgenerator = %d,\n", _bgenerator);
	SafeWriteFmt(v,write,up,"\tvarparams = %d,\n", _varparams);
	SafeWriteFmt(v,write,up,"}");
	return true;
}

bool SQFunctionProto::Load(SQVM *v,SQUserPointer up,SQREADFUNC read,SQObjectPtr &ret)
{
	SQInteger i, nliterals,nparameters;
	SQInteger noutervalues ,nlocalvarinfos ;
	SQInteger nlineinfos,ninstructions ,nfunctions,ndefaultparams ;
	SQObjectPtr sourcename, name;
	SQObjectPtr o;
	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(ReadObject(v, up, read, sourcename));
	_CHECK_IO(ReadObject(v, up, read, name));

	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeRead(v,read,up, &nliterals, sizeof(nliterals)));
	_CHECK_IO(SafeRead(v,read,up, &nparameters, sizeof(nparameters)));
	_CHECK_IO(SafeRead(v,read,up, &noutervalues, sizeof(noutervalues)));
	_CHECK_IO(SafeRead(v,read,up, &nlocalvarinfos, sizeof(nlocalvarinfos)));
	_CHECK_IO(SafeRead(v,read,up, &nlineinfos, sizeof(nlineinfos)));
	_CHECK_IO(SafeRead(v,read,up, &ndefaultparams, sizeof(ndefaultparams)));
	_CHECK_IO(SafeRead(v,read,up, &ninstructions, sizeof(ninstructions)));
	_CHECK_IO(SafeRead(v,read,up, &nfunctions, sizeof(nfunctions)));


	SQFunctionProto *f = SQFunctionProto::Create(_opt_ss(v),ninstructions,nliterals,nparameters,
			nfunctions,noutervalues,nlineinfos,nlocalvarinfos,ndefaultparams);
	SQObjectPtr proto = f; //gets a ref in case of failure
	f->_sourcename = sourcename;
	f->_name = name;

	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));

	for(i = 0;i < nliterals; i++){
		_CHECK_IO(ReadObject(v, up, read, o));
		f->_literals[i] = o;
	}
	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));

	for(i = 0; i < nparameters; i++){
		_CHECK_IO(ReadObject(v, up, read, o));
		f->_parameters[i] = o;
	}
	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));

	for(i = 0; i < noutervalues; i++){
		SQUnsignedInteger type;
		SQObjectPtr name;
		_CHECK_IO(SafeRead(v,read,up, &type, sizeof(SQUnsignedInteger)));
		_CHECK_IO(ReadObject(v, up, read, o));
		_CHECK_IO(ReadObject(v, up, read, name));
		f->_outervalues[i] = SQOuterVar(name,o, (SQOuterType)type);
	}
	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));

	for(i = 0; i < nlocalvarinfos; i++){
		SQLocalVarInfo lvi;
		_CHECK_IO(ReadObject(v, up, read, lvi._name));
		_CHECK_IO(SafeRead(v,read,up, &lvi._pos, sizeof(SQUnsignedInteger)));
		_CHECK_IO(SafeRead(v,read,up, &lvi._start_op, sizeof(SQUnsignedInteger)));
		_CHECK_IO(SafeRead(v,read,up, &lvi._end_op, sizeof(SQUnsignedInteger)));
		f->_localvarinfos[i] = lvi;
	}
	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeRead(v,read,up, f->_lineinfos, sizeof(SQLineInfo)*nlineinfos));

	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeRead(v,read,up, f->_defaultparams, sizeof(SQInteger)*ndefaultparams));

	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));
	_CHECK_IO(SafeRead(v,read,up, f->_instructions, sizeof(SQInstruction)*ninstructions));

	_CHECK_IO(CheckTag(v,read,up,SQ_CLOSURESTREAM_PART));
	for(i = 0; i < nfunctions; i++){
		_CHECK_IO(_funcproto(o)->Load(v, up, read, o));
		f->_functions[i] = o;
	}
	_CHECK_IO(SafeRead(v,read,up, &f->_stacksize, sizeof(f->_stacksize)));
	_CHECK_IO(SafeRead(v,read,up, &f->_bgenerator, sizeof(f->_bgenerator)));
	_CHECK_IO(SafeRead(v,read,up, &f->_varparams, sizeof(f->_varparams)));

	ret = f;
	return true;
}

#ifndef NO_GARBAGE_COLLECTOR

#define START_MARK() 	if(!(_uiRef&MARK_FLAG)){ \
		_uiRef|=MARK_FLAG;

#define END_MARK() RemoveFromChain(&_sharedstate->_gc_chain, this); \
		AddToChain(chain, this); }

void SQVM::Mark(SQCollectable **chain)
{
	START_MARK()
		SQSharedState::MarkObject(_lasterror,chain);
		SQSharedState::MarkObject(_errorhandler,chain);
		SQSharedState::MarkObject(_debughook_closure,chain);
		SQSharedState::MarkObject(_roottable, chain);
		SQSharedState::MarkObject(temp_reg, chain);
		for(SQUnsignedInteger i = 0; i < _stack.size(); i++) SQSharedState::MarkObject(_stack[i], chain);
		for(SQInteger k = 0; k < _callsstacksize; k++) SQSharedState::MarkObject(_callsstack[k]._closure, chain);
	END_MARK()
}

void SQArray::Mark(SQCollectable **chain)
{
	START_MARK()
		SQInteger len = _values.size();
		for(SQInteger i = 0;i < len; i++) SQSharedState::MarkObject(_values[i], chain);
	END_MARK()
}
void SQTable::Mark(SQCollectable **chain)
{
	START_MARK()
		if(_delegate) _delegate->Mark(chain);
		SQInteger len = _numofnodes;
		for(SQInteger i = 0; i < len; i++){
			SQSharedState::MarkObject(_nodes[i].key, chain);
			SQSharedState::MarkObject(_nodes[i].val, chain);
		}
	END_MARK()
}

void SQClass::Mark(SQCollectable **chain)
{
	START_MARK()
		_members->Mark(chain);
		if(_base) _base->Mark(chain);
		SQSharedState::MarkObject(_attributes, chain);
		for(SQUnsignedInteger i =0; i< _defaultvalues.size(); i++) {
			SQSharedState::MarkObject(_defaultvalues[i].val, chain);
			SQSharedState::MarkObject(_defaultvalues[i].attrs, chain);
		}
		for(SQUnsignedInteger j =0; j< _methods.size(); j++) {
			SQSharedState::MarkObject(_methods[j].val, chain);
			SQSharedState::MarkObject(_methods[j].attrs, chain);
		}
		for(SQUnsignedInteger k =0; k< MT_LAST; k++) {
			SQSharedState::MarkObject(_metamethods[k], chain);
		}
	END_MARK()
}

void SQInstance::Mark(SQCollectable **chain)
{
	START_MARK()
		_class->Mark(chain);
		SQUnsignedInteger nvalues = _class->_defaultvalues.size();
		for(SQUnsignedInteger i =0; i< nvalues; i++) {
			SQSharedState::MarkObject(_values[i], chain);
		}
	END_MARK()
}

void SQGenerator::Mark(SQCollectable **chain)
{
	START_MARK()
		for(SQUnsignedInteger i = 0; i < _stack.size(); i++) SQSharedState::MarkObject(_stack[i], chain);
		SQSharedState::MarkObject(_closure, chain);
	END_MARK()
}

void SQFunctionProto::Mark(SQCollectable **chain)
{
	START_MARK()
		for(SQInteger i = 0; i < _nliterals; i++) SQSharedState::MarkObject(_literals[i], chain);
		for(SQInteger k = 0; k < _nfunctions; k++) SQSharedState::MarkObject(_functions[k], chain);
	END_MARK()
}

void SQClosure::Mark(SQCollectable **chain)
{
	START_MARK()
		if(_base) _base->Mark(chain);
		SQFunctionProto *fp = _function;
		fp->Mark(chain);
		for(SQInteger i = 0; i < fp->_noutervalues; i++) SQSharedState::MarkObject(_outervalues[i], chain);
		for(SQInteger k = 0; k < fp->_ndefaultparams; k++) SQSharedState::MarkObject(_defaultparams[k], chain);
	END_MARK()
}

void SQNativeClosure::Mark(SQCollectable **chain)
{
	START_MARK()
		for(SQUnsignedInteger i = 0; i < _noutervalues; i++) SQSharedState::MarkObject(_outervalues[i], chain);
	END_MARK()
}

void SQOuter::Mark(SQCollectable **chain)
{
	START_MARK()
    /* If the valptr points to a closed value, that value is alive */
    if(_valptr == &_value) {
      SQSharedState::MarkObject(_value, chain);
    }
	END_MARK()
}

void SQUserData::Mark(SQCollectable **chain){
	START_MARK()
		if(_delegate) _delegate->Mark(chain);
	END_MARK()
}

void SQCollectable::UnMark() { _uiRef&=~MARK_FLAG; }

#endif

