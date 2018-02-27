/*	see copyright notice in squirrel.h */
#ifndef _SQOBJECT_H_
#define _SQOBJECT_H_

#include "squtils.h"

#ifdef _SQ64
#define UINT_MINUS_ONE (0xFFFFFFFFFFFFFFFF)
#else
#define UINT_MINUS_ONE (0xFFFFFFFF)
#endif

#define SQ_CLOSURESTREAM_HEAD (('S'<<24)|('Q'<<16)|('I'<<8)|('R'))
#define SQ_CLOSURESTREAM_PART (('P'<<24)|('A'<<16)|('R'<<8)|('T'))
#define SQ_CLOSURESTREAM_TAIL (('T'<<24)|('A'<<16)|('I'<<8)|('L'))

struct SQSharedState;

enum SQMetaMethod{
	MT_ADD=0,
	MT_SUB=1,
	MT_MUL=2,
	MT_DIV=3,
	MT_UNM=4,
	MT_MODULO=5,
	MT_SET=6,
	MT_GET=7,
	MT_TYPEOF=8,
	MT_NEXTI=9,
	MT_CMP=10,
	MT_CALL=11,
	MT_CLONED=12,
	MT_NEWSLOT=13,
	MT_DELSLOT=14,
	MT_TOSTRING=15,
	MT_NEWMEMBER=16,
	MT_INHERITED=17,
	MT_LAST = 18
};

#define MM_ADD		_SC("_add")
#define MM_SUB		_SC("_sub")
#define MM_MUL		_SC("_mul")
#define MM_DIV		_SC("_div")
#define MM_UNM		_SC("_unm")
#define MM_MODULO	_SC("_modulo")
#define MM_SET		_SC("_set")
#define MM_GET		_SC("_get")
#define MM_TYPEOF	_SC("_typeof")
#define MM_NEXTI	_SC("_nexti")
#define MM_CMP		_SC("_cmp")
#define MM_CALL		_SC("_call")
#define MM_CLONED	_SC("_cloned")
#define MM_NEWSLOT	_SC("_newslot")
#define MM_DELSLOT	_SC("_delslot")
#define MM_TOSTRING	_SC("_tostring")
#define MM_NEWMEMBER _SC("_newmember")
#define MM_INHERITED _SC("_inherited")


#define _CONSTRUCT_VECTOR(type,size,ptr) { \
	for(SQInteger n = 0; n < ((SQInteger)size); n++) { \
			new (&ptr[n]) type(); \
		} \
}

#define _DESTRUCT_VECTOR(type,size,ptr) { \
	for(SQInteger nl = 0; nl < ((SQInteger)size); nl++) { \
			ptr[nl].~type(); \
	} \
}

#define _COPY_VECTOR(dest,src,size) { \
	for(SQInteger _n_ = 0; _n_ < ((SQInteger)size); _n_++) { \
		dest[_n_] = src[_n_]; \
	} \
}

#define _NULL_SQOBJECT_VECTOR(vec,size) { \
	for(SQInteger _n_ = 0; _n_ < ((SQInteger)size); _n_++) { \
		vec[_n_].Null(); \
	} \
}

#define MINPOWER2 4

struct SQRefCounted
{
	SQUnsignedInteger _uiRef;
	struct SQWeakRef *_weakref;
	SQRefCounted(): _uiRef(0), _weakref(NULL) {}
	virtual ~SQRefCounted();
	SQWeakRef *GetWeakRef(SQObjectType type);
	ABSTRACT_METHOD(virtual void Release(), {});

};

struct SQWeakRef : SQRefCounted
{
	void Release();
	SQObject _obj;
};

#define _realval(o) (sq_type((o)) != OT_WEAKREF?(SQObject)o:_weakref(o)->_obj)

struct SQObjectPtr;

#define __AddRefRefCounted(unval) { unval.pRefCounted->_uiRef++; }

#define __AddRef(type,unval) if(ISREFCOUNTED(type))	__AddRefRefCounted(unval)

#define __ReleaseRefCounted(unval) if((--unval.pRefCounted->_uiRef)==0)	\
		{	\
			unval.pRefCounted->Release();	\
		}

#define __Release(type,unval) if(ISREFCOUNTED(type)) __ReleaseRefCounted(unval)

#define __ObjRelease(obj) { \
	if((obj)) {	\
		(obj)->_uiRef--; \
		if((obj)->_uiRef == 0) \
			(obj)->Release(); \
		(obj) = NULL;	\
	} \
}

#define __ObjAddRef(obj) { \
	(obj)->_uiRef++; \
}

#define is_delegable(t) (sq_type(t)&SQOBJECT_DELEGABLE)
#define raw_type(obj) _RAW_TYPE((obj)._type)

#define _integer(obj) ((obj)._unVal.nInteger)
#define _float(obj) ((obj)._unVal.fFloat)
#define _string(obj) ((obj)._unVal.pString)
#define _table(obj) ((obj)._unVal.pTable)
#define _array(obj) ((obj)._unVal.pArray)
#define _closure(obj) ((obj)._unVal.pClosure)
#define _generator(obj) ((obj)._unVal.pGenerator)
#define _nativeclosure(obj) ((obj)._unVal.pNativeClosure)
#define _userdata(obj) ((obj)._unVal.pUserData)
#define _userpointer(obj) ((obj)._unVal.pUserPointer)
#define _thread(obj) ((obj)._unVal.pThread)
#define _funcproto(obj) ((obj)._unVal.pFunctionProto)
#define _class(obj) ((obj)._unVal.pClass)
#define _instance(obj) ((obj)._unVal.pInstance)
#define _delegable(obj) ((SQDelegable *)(obj)._unVal.pDelegable)
#define _weakref(obj) ((obj)._unVal.pWeakRef)
#define _outer(obj) ((obj)._unVal.pOuter)
#define _refcounted(obj) ((obj)._unVal.pRefCounted)
#define _rawval(obj) ((obj)._unVal.raw)

#define _stringval(obj) (obj)._unVal.pString->_val
#define _userdataval(obj) ((SQUserPointer)sq_aligning((obj)._unVal.pUserData + 1))

#define tofloat(num) ((sq_type(num)==OT_INTEGER)?(SQFloat)_integer(num):_float(num))
#define tointeger(num) ((sq_type(num)==OT_FLOAT)?(SQInteger)_float(num):_integer(num))
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#if defined(SQUSEDOUBLE) && !defined(_SQ64) || !defined(SQUSEDOUBLE) && defined(_SQ64)
#define SQ_REFOBJECT_INIT()	SQ_OBJECT_RAWINIT()
#else
#define SQ_REFOBJECT_INIT()
#endif

#define _REF_TYPE_DECL(type,_class,sym) \
	SQObjectPtr(_class * x) \
	{ \
		SQ_OBJECT_RAWINIT() \
		_type=type; \
		_unVal.p##sym = x; \
		assert(_unVal.pTable); \
		_unVal.pRefCounted->_uiRef++; \
	} \
	inline SQObjectPtr& operator=(_class *x) \
	{  \
		SQObjectValue old_unVal; \
		bool isRefCounted = ISREFCOUNTED(_type);\
		if(isRefCounted) old_unVal = _unVal;\
		_type = type; \
		SQ_REFOBJECT_INIT() \
		_unVal.p##sym = x; \
		_unVal.pRefCounted->_uiRef++; \
		if(isRefCounted) __ReleaseRefCounted(old_unVal); \
		return *this; \
	} \
	inline _class *to##sym()\
	{ \
        assert(_type == type); \
        return _unVal.p##sym;\
	}

#define _SCALAR_TYPE_DECL(type,_class,prefix,sym) \
	SQObjectPtr(_class x) \
	{ \
		SQ_OBJECT_RAWINIT() \
		_type=type; \
		_unVal.prefix##sym = x; \
	} \
	inline SQObjectPtr& operator=(_class x) \
	{  \
		if(_type != type){\
			__Release(_type,_unVal); \
			_type = type; \
			SQ_OBJECT_RAWINIT() \
		}\
		_unVal.prefix##sym = x; \
		return *this; \
	} \
	inline _class to##sym()\
	{ \
        assert(_type == type); \
        return _unVal.prefix##sym;\
	}
struct SQObjectPtr : public SQObject
{
	SQObjectPtr()
	{
		SQ_OBJECT_RAWINIT()
		_type=OT_NULL;
		_unVal.pUserPointer=NULL;
	}
	#define _constructWith(o){_type = o._type; _unVal = o._unVal; __AddRef(_type,_unVal);}
	SQObjectPtr(const SQObjectPtr &o)
	{
	    _constructWith(o);
	}
	SQObjectPtr(const SQObject &o)
	{
	    _constructWith(o);
	}
	#undef _constructWith
	_REF_TYPE_DECL(OT_TABLE,SQTable,Table)
	_REF_TYPE_DECL(OT_CLASS,SQClass,Class)
	_REF_TYPE_DECL(OT_INSTANCE,SQInstance,Instance)
	_REF_TYPE_DECL(OT_ARRAY,SQArray,Array)
	_REF_TYPE_DECL(OT_CLOSURE,SQClosure,Closure)
	_REF_TYPE_DECL(OT_NATIVECLOSURE,SQNativeClosure,NativeClosure)
	_REF_TYPE_DECL(OT_OUTER,SQOuter,Outer)
	_REF_TYPE_DECL(OT_GENERATOR,SQGenerator,Generator)
	_REF_TYPE_DECL(OT_STRING,SQString,String)
	_REF_TYPE_DECL(OT_USERDATA,SQUserData,UserData)
	_REF_TYPE_DECL(OT_WEAKREF,SQWeakRef,WeakRef)
	_REF_TYPE_DECL(OT_THREAD,SQVM,Thread)
	_REF_TYPE_DECL(OT_FUNCPROTO,SQFunctionProto,FunctionProto)

	_SCALAR_TYPE_DECL(OT_INTEGER,SQInteger,n, Integer)
	_SCALAR_TYPE_DECL(OT_FLOAT,SQFloat,f, Float)
	_SCALAR_TYPE_DECL(OT_USERPOINTER,SQUserPointer,p, UserPointer)

	SQObjectPtr(bool bBool)
	{
		SQ_OBJECT_RAWINIT()
		_type = OT_BOOL;
		_unVal.nInteger = bBool?1:0;
	}
	inline SQObjectPtr& operator=(bool b)
	{
		if(_type != OT_BOOL){
			__Release(_type,_unVal);
			SQ_OBJECT_RAWINIT()
			_type = OT_BOOL;
		}
		_unVal.nInteger = b?1:0;
		return *this;
	}

	~SQObjectPtr()
	{
		__Release(_type,_unVal);
	}

	inline SQObjectPtr& _assignThis(const SQObject& obj)
	{
		//saving temporarily the old value for cases
		//where we are assigning a inner value to the old value
		//local tbl = {a=2, b=4}; tbl = tbl.a;
		SQObjectValue old_unVal;
		bool isRefCounted = ISREFCOUNTED(_type);
		if(isRefCounted) old_unVal = _unVal;
		_unVal = obj._unVal;
		_type = obj._type;
		__AddRef(_type,_unVal);
		if(isRefCounted) __ReleaseRefCounted(old_unVal);
		return *this;
	}
	inline SQObjectPtr& operator=(const SQObjectPtr& obj)
	{
	    return _assignThis(obj);
	}
	inline SQObjectPtr& operator=(const SQObject& obj)
	{
	    return _assignThis(obj);
	}

	inline bool isNull()
	{
		return _type == OT_NULL;
	}
	inline void Null()
	{
		if(!isNull()){
			__Release(_type,_unVal);
			_type = OT_NULL;
			_unVal.raw = (SQRawObjectVal)NULL;
		}
	}
	SQObjectPtr operator[](SQInteger nidx);
	SQObjectPtr operator[](const SQChar *key);
	private:
		SQObjectPtr(const SQChar *){} //safety
};


inline void _Swap(SQObject &a,SQObject &b)
{
	SQObjectType tOldType = a._type;
	SQObjectValue unOldVal = a._unVal;
	a._type = b._type;
	a._unVal = b._unVal;
	b._type = tOldType;
	b._unVal = unOldVal;
}

/////////////////////////////////////////////////////////////////////////////////////
#ifndef NO_GARBAGE_COLLECTOR
#define MARK_FLAG 0x80000000
struct SQCollectable : public SQRefCounted {
	SQCollectable *_next;
	SQCollectable *_prev;
	SQSharedState *_sharedstate;
	ABSTRACT_METHOD(virtual SQObjectType GetType(), {return OT_NULL;});
	ABSTRACT_METHOD(virtual void Release(), {});
	ABSTRACT_METHOD(virtual void Mark(SQCollectable **chain), {});
	void UnMark();
	ABSTRACT_METHOD(virtual void Finalize(), {});
	static void AddToChain(SQCollectable **chain,SQCollectable *c);
	static void RemoveFromChain(SQCollectable **chain,SQCollectable *c);
};


#define ADD_TO_CHAIN(chain,obj) AddToChain(chain,obj)
#define REMOVE_FROM_CHAIN(chain,obj) {if(!(_uiRef&MARK_FLAG))RemoveFromChain(chain,obj);}
#define CHAINABLE_OBJ SQCollectable
#define INIT_CHAIN() {_next=NULL;_prev=NULL;_sharedstate=ss;}
#else

#define ADD_TO_CHAIN(chain,obj) ((void)0)
#define REMOVE_FROM_CHAIN(chain,obj) ((void)0)
#define CHAINABLE_OBJ SQRefCounted
#define INIT_CHAIN() ((void)0)
#endif

struct SQDelegable : public CHAINABLE_OBJ {
	bool SetDelegate(SQTable *m);
	virtual bool GetMetaMethod(SQVM *v,SQMetaMethod mm,SQObjectPtr &res);
	SQTable *_delegate;
};

SQUnsignedInteger SQTranslateIndex(const SQObjectPtr &idx);
typedef sqvector<SQObjectPtr> SQObjectPtrVec;
typedef sqvector<SQInteger> SQIntVec;
const SQChar *GetTypeName(const SQObjectPtr &obj1);
const SQChar *IdType2Name(SQObjectType type);

const SQChar *SQGetOpName(int op_code);
const SQChar *SQGetArithOpName(int it);
const SQChar *SQGetNewObjTypeName(int it);
const SQChar *SQGetArrayAppendTypeName(int it);
const SQChar *SQGetCmpOpName(int it);
const SQChar *SQGetBitwiseOpName(int it);
const SQChar *SQGetVarTypeName(int it);

#endif //_SQOBJECT_H_
