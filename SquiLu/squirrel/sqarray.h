/*	see copyright notice in squirrel.h */
#ifndef _SQARRAY_H_
#define _SQARRAY_H_


struct SQArrayBase : public CHAINABLE_OBJ
{
protected:
	SQArrayBase(SQSharedState *ss,SQInteger nsize){
	    INIT_CHAIN();
	    ADD_TO_CHAIN(&_ss(this)->_gc_chain,this);
	}
	~SQArrayBase()
	{
		REMOVE_FROM_CHAIN(&_ss(this)->_gc_chain,this);
	}
public:

#ifndef NO_GARBAGE_COLLECTOR
    ABSTRACT_METHOD(virtual bool isCollectable(), {return true;})
	void Mark(SQCollectable **chain);
	SQObjectType GetType() {return OT_ARRAY;}
#endif
	void Finalize(){Resize(0);}
	bool Get(const SQInteger nidx,SQObjectPtr &val)
	{
		if(((SQUnsignedInteger)nidx)<Size()){
			_getRealObjectPtr(nidx, val);
			return true;
		}
		return false;
	}
	SQObjectPtr operator[](SQInteger nidx) {
		SQObjectPtr val;
		Get(nidx, val);
		return val;
	}
	bool Exists(const SQInteger nidx)
	{
		return (((SQUnsignedInteger)nidx)<Size());
	}
	bool Set(const SQInteger nidx,const SQObjectPtr &val)
	{
		return (((SQUnsignedInteger)nidx)<Size())
			&& _setObjectPtr(nidx, val);
	}
	SQInteger Next(const SQObjectPtr &refpos,SQObjectPtr &outkey,SQObjectPtr &outval)
	{
		SQUnsignedInteger idx=SQTranslateIndex(refpos);
		while(idx<Size()){
			//first found
			outkey=(SQInteger)idx;
			_getRealObjectPtr(idx, outval);
			//return idx for the next iteration
			return ++idx;
		}
		//nothing to iterate anymore
		return -1;
	}
	ABSTRACT_METHOD(virtual SQArrayBase *Clone(bool withData=true), { return NULL;})
	ABSTRACT_METHOD(virtual void *RawData(), { return NULL;})
	ABSTRACT_METHOD(virtual SQInteger GetArrayType(), { return 0;})
	ABSTRACT_METHOD(virtual SQUnsignedInteger SizeOf() const, { return 0;})
	ABSTRACT_METHOD(virtual SQUnsignedInteger Size() const, { return 0;})
	bool Resize(SQUnsignedInteger size)
	{
		SQObjectPtr _null;
		return Resize(size,_null);
	}
	bool Resize(SQUnsignedInteger size,SQObjectPtr &fill) {
	    bool rc = _resizeValues(size,fill);
	    if(rc) ShrinkIfNeeded();
	    return rc;
    }
	bool Minsize(SQUnsignedInteger size) { return (Size() < size) ? Resize(size) : true;}
	bool Minsize(SQUnsignedInteger size,SQObjectPtr &fill) {
	    return (Size() < size) ? _resizeValues(size,fill) : true;
    }
	ABSTRACT_METHOD(virtual void Reserve(SQUnsignedInteger size), {})
	ABSTRACT_METHOD(virtual SQUnsignedInteger Capacity(), {return 0;})
	ABSTRACT_METHOD(virtual bool Append(const SQObject &o), {return false;})
	void Extend(SQArrayBase *a);
	ABSTRACT_METHOD(virtual void Top(SQObjectPtr &out), {})
	ABSTRACT_METHOD(virtual void Pop(), {})
	bool Insert(SQInteger idx,const SQObject &val){
		return _insertValue((SQUnsignedInteger)idx,val);
	}
	void ShrinkIfNeeded() {
		if(Size() <= Capacity()>>2) //shrink the array
			_shrinktofit();
	}
	bool Remove(SQInteger idx){
	    if(!_removeValue(idx)) return false;
		ShrinkIfNeeded();
		return true;
	}
	void Release()
	{
		sq_assert(NULL);
	}
	ABSTRACT_METHOD(virtual void _swap(SQInteger idx1, SQInteger idx2),{})
	ABSTRACT_METHOD(virtual void _copy(SQInteger idx1, SQInteger idx2),{})
	ABSTRACT_METHOD(virtual bool _set(SQInteger idx1, SQObjectPtr &val),{return false;})
	ABSTRACT_METHOD(virtual void _get2(SQInteger idx, SQObject& out),{})
    	ABSTRACT_METHOD(virtual void DummyPinVtable(), {})
private:
    ABSTRACT_METHOD(virtual void _getObjectPtr(const SQInteger nidx,SQObjectPtr &val),{})
    ABSTRACT_METHOD(virtual void _getRealObjectPtr(const SQInteger nidx,SQObjectPtr &val),{})
    ABSTRACT_METHOD(virtual bool _setObjectPtr(const SQInteger nidx, const SQObjectPtr &val),{return false;})
    ABSTRACT_METHOD(virtual bool _resizeValues(SQInteger size,SQObjectPtr &fill),{return false;})
    ABSTRACT_METHOD(virtual bool _insertValue(SQInteger idx,const SQObject &val),{return false;})
    ABSTRACT_METHOD(virtual bool _removeValue(SQInteger idx),{return false;})
    ABSTRACT_METHOD(virtual void _shrinktofit(),{})
};

#define DECLARE_CLASS_ARRAY_MEMBERS(atype)\
	static atype* Create(SQSharedState *ss,SQInteger nInitialSize){\
		atype *newarray=(atype*)SQ_MALLOC(sizeof(atype));\
		new (newarray) atype(ss,nInitialSize);\
		return newarray;\
	}\
	atype *Clone(bool withData=true){\
	    atype *anew=Create(_opt_ss(this),0);\
	    if(withData) anew->_vec_values.copy(_vec_values);\
	    return anew;\
    }\
	void Release(){sq_delete(this,atype);}\
	virtual SQInteger GetArrayType() { return eat_##atype;} \
	void DummyPinVtable();

struct SQArray : public SQArrayBase
{
protected:
    SQObjectPtrVec _vec_values;
    SQArray(SQSharedState *ss,SQInteger nsize):SQArrayBase(ss, nsize){
	    Resize(nsize);
    }

public:
#ifndef NO_GARBAGE_COLLECTOR
    virtual bool isCollectable(){return true;}
#endif
	DECLARE_CLASS_ARRAY_MEMBERS(SQArray)
	SQUnsignedInteger SizeOf() const {return _vec_values.sizeOf();}
	SQUnsignedInteger Size() const {return _vec_values.size();}
	void Reserve(SQUnsignedInteger size) { _vec_values.reserve(size); }
	SQUnsignedInteger Capacity() { return _vec_values.capacity(); }
	bool Append(const SQObject &o){_vec_values.push_back(o); return true;}
	void Top(SQObjectPtr &out){out = _vec_values.top();}
	void Pop(){_vec_values.pop_back(); ShrinkIfNeeded(); }
	void _swap(SQInteger idx1, SQInteger idx2)
	{
	    SQObjectPtr t = _vec_values[idx1];
        _vec_values[idx1] = _vec_values[idx2];
        _vec_values[idx2] = t;
	}
	void _copy(SQInteger idx1, SQInteger idx2)
	{
        _vec_values[idx1] = _vec_values[idx2];
	}
	bool _set(SQInteger idx1, SQObjectPtr &val)
	{
        _vec_values[idx1] = val;
        return true;
	}
	void _get2(SQInteger idx, SQObject& out)
	{
        out = _vec_values[idx];
	}
	virtual void *RawData() { return NULL;}
private:

    virtual void _getObjectPtr(const SQInteger nidx,SQObjectPtr &val){
        val = _vec_values[nidx];
    }
    virtual void _getRealObjectPtr(const SQInteger nidx,SQObjectPtr &val){
        SQObjectPtr &o = _vec_values[nidx];
        val = _realval(o);
    }
    virtual bool _setObjectPtr(const SQInteger nidx, const SQObjectPtr &val){
        _vec_values[nidx] = val;
        return true;
    }
    virtual bool _resizeValues(SQInteger size,SQObjectPtr &fill){
        _vec_values.resize(size,fill);
        return true;
    }
    virtual bool _insertValue(SQInteger idx,const SQObject &val){
		_vec_values.insert(idx,val);
		return true;
    }
    virtual bool _removeValue(SQInteger idx){
        return _vec_values.remove(idx);
    }
    virtual void _shrinktofit(){
        _vec_values.shrinktofit();
    }
};

template <typename T>
struct SQNumericBaseArray : public SQArrayBase
{
protected:
    sqvector<T> _vec_values;
    SQNumericBaseArray(SQSharedState *ss,SQInteger nsize):SQArrayBase(ss, nsize){
	    Resize(nsize);
    }

    virtual bool isNumeric(const SQObject &o, T &tv)
    {
	    return false;
    }
public:
#ifndef NO_GARBAGE_COLLECTOR
    virtual bool isCollectable(){return false;}
#endif
	SQUnsignedInteger SizeOf() const {return _vec_values.sizeOf();}
	SQUnsignedInteger Size() const {return _vec_values.size();}
	void Reserve(SQUnsignedInteger size) { _vec_values.reserve(size); }
	SQUnsignedInteger Capacity() { return _vec_values.capacity(); }
	bool Append(const SQObject &o){
	    T tv;
	    if(!isNumeric(o, tv)) return false;
	    _vec_values.push_back(tv);
	    return true;
	}
	//void Top(SQObjectPtr& out){out = _vec_values.top();}
	void Pop(){_vec_values.pop_back(); ShrinkIfNeeded(); }
	void _swap(SQInteger idx1, SQInteger idx2)
	{
	    T tmp = _vec_values[idx1];
        _vec_values[idx1] = _vec_values[idx2];
        _vec_values[idx2] = tmp;
	}
	void _copy(SQInteger idx1, SQInteger idx2)
	{
        _vec_values[idx1] = _vec_values[idx2];
	}
	bool _set(SQInteger idx1, SQObjectPtr &val)
	{
        T tv;
	    if(!isNumeric(val, tv)) return false;
        _vec_values[idx1] = tv;
        return true;
	}
	virtual void *RawData() { return _vec_values._vals;}
private:

    virtual bool _setObjectPtr(const SQInteger nidx, const SQObjectPtr &val){
	    T tv;
	    if(!isNumeric(val, tv)) return false;
        _vec_values[nidx] = tv;
        return true;
    }
    virtual bool _resizeValues(SQInteger size,SQObjectPtr &fill){
	    T tv;
	    switch(sq_type(fill))
	    {
	        case OT_NULL: tv = 0;break;
	        case OT_FLOAT: tv = _float(fill);break;
	        case OT_INTEGER: tv = _integer(fill);break;
	        default:
                return false;
	    }
        _vec_values.resize(size,tv);
        return true;
    }
    virtual bool _insertValue(SQInteger idx,const SQObject &val){
	    T tv;
	    if(!isNumeric(val, tv)) return false;
		_vec_values.insert(idx,tv);
        return true;
    }
    virtual bool _removeValue(SQInteger idx){
        return _vec_values.remove(idx);
    }
    virtual void _shrinktofit(){
        _vec_values.shrinktofit();
    }
};

template <typename T>
struct SQFloatBaseArray : public SQNumericBaseArray<T>
{
protected:
    SQFloatBaseArray(SQSharedState *ss,SQInteger nsize):
        SQNumericBaseArray<T>(ss, nsize){
    }

    bool isNumeric(const SQObject &o, T &tv)
    {
	    switch(sq_type(o))
	    {
	        case OT_FLOAT: tv = _float(o);break;
	        case OT_INTEGER: tv = _integer(o);break;
	        default:
                return false;
	    }
	    return true;
    }
public:
    void Top(SQObjectPtr& out){out = (SQFloat)SQNumericBaseArray<T>::_vec_values.top();}
    virtual void _getObjectPtr(const SQInteger nidx,SQObjectPtr &val){
        val = (SQFloat)SQNumericBaseArray<T>::_vec_values[nidx];
    }
    virtual void _getRealObjectPtr(const SQInteger nidx,SQObjectPtr &val){
        SQObjectPtr o = (SQFloat)SQNumericBaseArray<T>::_vec_values[nidx];
        val = _realval(o);
    }
	void _get2(SQInteger idx, SQObject& val)
	{
	    val._type = OT_FLOAT;
        _float(val) = SQNumericBaseArray<T>::_vec_values[idx];
	}
};

struct SQFloat64Array : public SQFloatBaseArray<SQFloat64>
{
protected:
    SQFloat64Array(SQSharedState *ss,SQInteger nsize):SQFloatBaseArray(ss, nsize){}

public:
    DECLARE_CLASS_ARRAY_MEMBERS(SQFloat64Array)
};

struct SQFloat32Array : public SQFloatBaseArray<SQFloat32>
{
protected:
    SQFloat32Array(SQSharedState *ss,SQInteger nsize):SQFloatBaseArray(ss, nsize){}

public:
    DECLARE_CLASS_ARRAY_MEMBERS(SQFloat32Array)
};

template <typename T>
struct SQIntegerBaseArray : public SQNumericBaseArray<T>
{
protected:
    SQIntegerBaseArray(SQSharedState *ss,SQInteger nsize):
        SQNumericBaseArray<T>(ss, nsize){
    }

    bool isNumeric(const SQObject &o, T &tv)
    {
	    switch(sq_type(o))
	    {
	        //case OT_FLOAT: tv = _float(o);break;
	        case OT_INTEGER: tv = _integer(o);break;
	        default:
                return false;
	    }
	    return true;
    }
public:
    virtual void _getObjectPtr(const SQInteger nidx,SQObjectPtr &val){
        val = (SQInteger)SQNumericBaseArray<T>::_vec_values[nidx];
    }
	void Top(SQObjectPtr& out){out = (SQInteger)SQNumericBaseArray<T>::_vec_values.top();}
    virtual void _getRealObjectPtr(const SQInteger nidx,SQObjectPtr &val){
        SQObjectPtr o = (SQInteger)SQNumericBaseArray<T>::_vec_values[nidx];
        val = _realval(o);
    }
	void _get2(SQInteger idx, SQObject& val)
	{
	    val._type = OT_INTEGER;
        _integer(val) = SQNumericBaseArray<T>::_vec_values[idx];
	}
};

struct SQInt64Array : public SQIntegerBaseArray<SQInt64>
{
protected:
    SQInt64Array(SQSharedState *ss,SQInteger nsize):SQIntegerBaseArray(ss, nsize){}

public:
    DECLARE_CLASS_ARRAY_MEMBERS(SQInt64Array)
};

struct SQInt32Array : public SQIntegerBaseArray<SQInt32>
{
protected:
    SQInt32Array(SQSharedState *ss,SQInteger nsize):SQIntegerBaseArray(ss, nsize){}

public:
    DECLARE_CLASS_ARRAY_MEMBERS(SQInt32Array)
};

struct SQInt16Array : public SQIntegerBaseArray<SQInt16>
{
protected:
    SQInt16Array(SQSharedState *ss,SQInteger nsize):SQIntegerBaseArray(ss, nsize){}

public:
    DECLARE_CLASS_ARRAY_MEMBERS(SQInt16Array)
};

struct SQInt8Array : public SQIntegerBaseArray<SQInt8>
{
protected:
    SQInt8Array(SQSharedState *ss,SQInteger nsize):SQIntegerBaseArray(ss, nsize){}

public:
    DECLARE_CLASS_ARRAY_MEMBERS(SQInt8Array)
};

#endif //_SQARRAY_H_
