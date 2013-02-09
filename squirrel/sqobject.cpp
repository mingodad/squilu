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
	return IdType2Name(type(obj1));
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
	SQInteger idx = (SQInteger)TranslateIndex(refpos);
	while(idx < _len){
		outkey = (SQInteger)idx;
		outval = (SQInteger)((SQUnsignedInteger)_val[idx]);
		//return idx for the next iteration
		return ++idx;
	}
	//nothing to iterate anymore
	return -1;
}

SQUnsignedInteger TranslateIndex(const SQObjectPtr &idx)
{
	switch(type(idx)){
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
	_stack._vals[0] = ISREFCOUNTED(type(_this)) ? SQObjectPtr(_refcounted(_this)->GetWeakRef(type(_this))) : _this;
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
	}
	SQObject _this = _stack._vals[0];
	v->_stack[v->_stackbase] = type(_this) == OT_WEAKREF ? _weakref(_this)->_obj : _this;

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

    while(_lineinfos[mid]._op >= op && mid >= 0) mid--;

    return _lineinfos[mid]._line;
}

SQClosure::~SQClosure()
{
	__ObjRelease(_env);
	__ObjRelease(_base);
	REMOVE_FROM_CHAIN(&_ss(this)->_gc_chain,this);
}

#define _CHECK_IO(exp)  { if(!exp)return false; }
bool SafeWrite(HSQUIRRELVM v,SQWRITEFUNC write,SQUserPointer up,SQUserPointer dest,SQInteger size)
{
	if(write(up,dest,size) != size) {
		v->Raise_Error(_SC("io error (write function failure)"));
		return false;
	}
	return true;
}

bool SafeWriteFmt(HSQUIRRELVM v,SQWRITEFUNC write,SQUserPointer up, const SQChar *fmt, ...)
{
    if(fmt){
        SQChar str[8192];
        va_list vl;
        va_start(vl, fmt);
        SQInteger len = scvsnprintf(str, sizeof(str), fmt, vl);
        va_end(vl);
        return SafeWrite(v, write, up, str, len);
    }
	return false;
}

bool SafeRead(HSQUIRRELVM v,SQWRITEFUNC read,SQUserPointer up,SQUserPointer dest,SQInteger size)
{
	if(size && read(up,dest,size) != size) {
		v->Raise_Error(_SC("io error, read function failure, the origin stream could be corrupted/trucated"));
		return false;
	}
	return true;
}

bool WriteTag(HSQUIRRELVM v,SQWRITEFUNC write,SQUserPointer up,SQUnsignedInteger32 tag)
{
	return SafeWrite(v,write,up,&tag,sizeof(tag));
}

bool CheckTag(HSQUIRRELVM v,SQWRITEFUNC read,SQUserPointer up,SQUnsignedInteger32 tag)
{
	SQUnsignedInteger32 t;
	_CHECK_IO(SafeRead(v,read,up,&t,sizeof(t)));
	if(t != tag){
		v->Raise_Error(_SC("invalid or corrupted closure stream"));
		return false;
	}
	return true;
}

bool WriteObjectAsCode(HSQUIRRELVM v,SQUserPointer up,SQWRITEFUNC write,SQObjectPtr &o, bool withQuotes=true)
{
	SQChar buf[32];
	SQInteger sz;
	switch(type(o)){
	case OT_STRING:{
            const SQChar *d1 = withQuotes ? _SC("\"") : _SC("[==[");
            const SQChar *d2 = withQuotes ? _SC("\"") : _SC("]==]");
            _CHECK_IO(SafeWrite(v,write,up, (void*)d1, scstrlen(d1)));
            _CHECK_IO(SafeWrite(v,write,up, _stringval(o),rsl(_string(o)->_len)));
            _CHECK_IO(SafeWrite(v,write,up, (void*)d2, scstrlen(d2)));
        }
		break;
	case OT_INTEGER:
        sz = scsnprintf(buf, sizeof(buf), "%d", _integer(o));
		_CHECK_IO(SafeWrite(v,write,up,buf,sz));break;
	case OT_FLOAT:
        sz = scsnprintf(buf, sizeof(buf), "%f", _float(o));
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

bool WriteObject(HSQUIRRELVM v,SQUserPointer up,SQWRITEFUNC write,SQObjectPtr &o)
{
	SQUnsignedInteger32 _type = (SQUnsignedInteger32)type(o);
	_CHECK_IO(SafeWrite(v,write,up,&_type,sizeof(_type)));
	switch(type(o)){
	case OT_STRING:
		_CHECK_IO(SafeWrite(v,write,up,&_string(o)->_len,sizeof(SQInteger)));
		_CHECK_IO(SafeWrite(v,write,up,_stringval(o),rsl(_string(o)->_len)));
		break;
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

bool ReadObject(HSQUIRRELVM v,SQUserPointer up,SQREADFUNC read,SQObjectPtr &o)
{
	SQUnsignedInteger32 _type;
	_CHECK_IO(SafeRead(v,read,up,&_type,sizeof(_type)));
	SQObjectType t = (SQObjectType)_type;
	switch(t){
	case OT_STRING:{
		SQInteger len;
		_CHECK_IO(SafeRead(v,read,up,&len,sizeof(SQInteger)));
		_CHECK_IO(SafeRead(v,read,up,_ss(v)->GetScratchPad(rsl(len)),rsl(len)));
		o=SQString::Create(_ss(v),_ss(v)->GetScratchPad(-1),len);
				   }
		break;
	case OT_INTEGER:{
		SQInteger i;
		_CHECK_IO(SafeRead(v,read,up,&i,sizeof(SQInteger))); o = i; break;
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
	ret = SQClosure::Create(_ss(v),_funcproto(func));
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

bool SQFunctionProto::SaveAsSource(SQVM *v,SQUserPointer up,SQWRITEFUNC write)
{
	SQInteger i,nliterals = _nliterals,nparameters = _nparameters;
	SQInteger noutervalues = _noutervalues,nlocalvarinfos = _nlocalvarinfos;
	SQInteger nlineinfos=_nlineinfos,ninstructions = _ninstructions,nfunctions=_nfunctions;
	SQInteger ndefaultparams = _ndefaultparams;
	SafeWriteFmt(v,write,up,"{\n");
	SafeWriteFmt(v,write,up,"\tsource_name = \"%s\",\n", _stringval(_sourcename));
	SafeWriteFmt(v,write,up,"\tfunction_name = \"%s\",\n", _stringval(_name));

	SafeWriteFmt(v,write,up,"\tliterals = [\n");
	for(i=0;i<nliterals;i++){
		SafeWriteFmt(v,write,up,"\t\t/*%d*/", i);
		_CHECK_IO(WriteObjectAsCode(v,up,write,_literals[i], false));
		SafeWriteFmt(v,write,up,",\n");
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tparameters = [\n");
	for(i=0;i<nparameters;i++){
		SafeWriteFmt(v,write,up,"\t\t/*%d*/", i);
		_CHECK_IO(WriteObjectAsCode(v,up,write,_parameters[i]));
		SafeWriteFmt(v,write,up,",\n");
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\toutervalues = [\n");
	for(i=0;i<noutervalues;i++){
		SafeWriteFmt(v,write,up,"\t\t/*%d*/{type=%d,\n", i, _outervalues[i]._type);
		SafeWriteFmt(v,write,up,"\t\tsrc=");
		_CHECK_IO(WriteObjectAsCode(v,up,write,_outervalues[i]._src));
		SafeWriteFmt(v,write,up,",\n");
		SafeWriteFmt(v,write,up,"\t\tname=");
		_CHECK_IO(WriteObjectAsCode(v,up,write,_outervalues[i]._name));
		SafeWriteFmt(v,write,up,"},\n");
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tlocalvarinfos = [\n");
	for(i=0;i<nlocalvarinfos;i++){
	    SQLocalVarInfo &lvi=_localvarinfos[i];
		SafeWriteFmt(v,write,up,"\t\t/*%d*/{name=", i);
		_CHECK_IO(WriteObjectAsCode(v,up,write,lvi._name));
		SafeWriteFmt(v,write,up,", pos=%d", lvi._pos);
		SafeWriteFmt(v,write,up,", start_op=%d", lvi._start_op);
		SafeWriteFmt(v,write,up,", end_op=%d", lvi._end_op);
		SafeWriteFmt(v,write,up,", scope=%d", lvi._scope);
		SafeWriteFmt(v,write,up,", type=%d},\n", lvi._type);
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tlineinfos = [\n");
	for(i=0;i<nlineinfos;i++){
	    SQLineInfo &li=_lineinfos[i];
		SafeWriteFmt(v,write,up,"\t\t/*%d*/{line=%d, op=%d},\n", i, li._line, li._op);
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tdefaultparams = [");
	for(i=0;i<ndefaultparams;i++){
		SafeWriteFmt(v,write,up,"%d,", _defaultparams[i]);
	}
    SafeWriteFmt(v,write,up,"],\n");

	SafeWriteFmt(v,write,up,"\tinstructions = [\n");
    SafeWriteFmt(v,write,up,"\t\t//[op_str, op, arg0, arg1, arg2, arg3],\n");
	const SQChar *str_op;
#define CASE_OP(v) case v: str_op = _SC(#v); break;
	for(i=0;i<ninstructions;i++){
	    SQInstruction &inst = _instructions[i];
	    switch(inst.op){
            CASE_OP(_OP_LINE)
            CASE_OP(_OP_LOAD)
            CASE_OP(_OP_LOADINT)
            CASE_OP(_OP_LOADFLOAT)
            CASE_OP(_OP_DLOAD)
            CASE_OP(_OP_TAILCALL)
            CASE_OP(_OP_CALL)
            CASE_OP(_OP_PREPCALL)
            CASE_OP(_OP_PREPCALLK)
            CASE_OP(_OP_GETK)
            CASE_OP(_OP_MOVE)
            CASE_OP(_OP_NEWSLOT)
            CASE_OP(_OP_DELETE)
            CASE_OP(_OP_SET)
            CASE_OP(_OP_GET)
            CASE_OP(_OP_EQ)
            CASE_OP(_OP_NE)
            CASE_OP(_OP_ADD)
            CASE_OP(_OP_SUB)
            CASE_OP(_OP_MUL)
            CASE_OP(_OP_DIV)
            CASE_OP(_OP_MOD)
            CASE_OP(_OP_BITW)
            CASE_OP(_OP_RETURN)
            CASE_OP(_OP_LOADNULLS)
            CASE_OP(_OP_LOADROOT)
            CASE_OP(_OP_LOADBOOL)
            CASE_OP(_OP_DMOVE)
            CASE_OP(_OP_JMP)
            //CASE_OP(_OP_JNZ)
            CASE_OP(_OP_JCMP)
            CASE_OP(_OP_JZ)
            CASE_OP(_OP_SETOUTER)
            CASE_OP(_OP_GETOUTER)
            CASE_OP(_OP_NEWOBJ)
            CASE_OP(_OP_APPENDARRAY)
            CASE_OP(_OP_COMPARITH)
            CASE_OP(_OP_INC)
            CASE_OP(_OP_INCL)
            CASE_OP(_OP_PINC)
            CASE_OP(_OP_PINCL)
            CASE_OP(_OP_CMP)
            CASE_OP(_OP_EXISTS)
            CASE_OP(_OP_INSTANCEOF)
            CASE_OP(_OP_AND)
            CASE_OP(_OP_OR)
            CASE_OP(_OP_NEG)
            CASE_OP(_OP_NOT)
            CASE_OP(_OP_BWNOT)
            CASE_OP(_OP_CLOSURE)
            CASE_OP(_OP_YIELD)
            CASE_OP(_OP_RESUME)
            CASE_OP(_OP_FOREACH)
            CASE_OP(_OP_POSTFOREACH)
            CASE_OP(_OP_CLONE)
            CASE_OP(_OP_TYPEOF)
            CASE_OP(_OP_PUSHTRAP)
            CASE_OP(_OP_POPTRAP)
            CASE_OP(_OP_THROW)
            CASE_OP(_OP_NEWSLOTA)
            CASE_OP(_OP_GETBASE)
            CASE_OP(_OP_CLOSE)
            default:
                str_op = _SC("???");
	    }
		SafeWriteFmt(v,write,up,"\t\t/*%d*/[\"%s\", %d, %d, %d, %d, %d],\n", i, str_op, inst.op, inst._arg0, inst._arg1, inst._arg2, inst._arg3);
	}
    SafeWriteFmt(v,write,up,"\t],\n");

	SafeWriteFmt(v,write,up,"\tfunctions = [");
	for(i=0;i<nfunctions;i++){
	    SafeWriteFmt(v,write,up,"/*%d*/", i);
	    _CHECK_IO(_funcproto(_functions[i])->SaveAsSource(v,up,write));
	    SafeWriteFmt(v,write,up,",\n");
	}
    SafeWriteFmt(v,write,up,"],\n");

	SafeWriteFmt(v,write,up,"\tstacksize = %d,\n", sizeof(_stacksize));
	SafeWriteFmt(v,write,up,"\tbgenerator = %d,\n", sizeof(_bgenerator));
	SafeWriteFmt(v,write,up,"\tvarparams = %d,\n", sizeof(_varparams));
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

