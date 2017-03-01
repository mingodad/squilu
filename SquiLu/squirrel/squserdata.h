/*	see copyright notice in squirrel.h */
#ifndef _SQUSERDATA_H_
#define _SQUSERDATA_H_

struct SQUserData : SQDelegable
{
	SQUserData(SQSharedState *ss, SQInteger size=0):_size(size),_hook(NULL),_typetag(0) {
	    _delegate = 0; INIT_CHAIN(); ADD_TO_CHAIN(&_ss(this)->_gc_chain, this);
    }
	~SQUserData()
	{
		REMOVE_FROM_CHAIN(&_ss(this)->_gc_chain, this);
		SetDelegate(NULL);
	}
	static SQUserData* Create(SQSharedState *ss, SQInteger size)
	{
		SQUserData* ud = (SQUserData*)SQ_MALLOC(sq_aligning(sizeof(SQUserData))+size);
		new (ud) SQUserData(ss, size);
		return ud;
	}
#ifndef NO_GARBAGE_COLLECTOR
	void Mark(SQCollectable **chain);
	void Finalize(){SetDelegate(NULL);}
	SQObjectType GetType(){ return OT_USERDATA;}
#endif
	void Release() {
		if (_hook) {
#if !defined(NO_GARBAGE_COLLECTOR) && defined(SQ_WITH_DELAYED_RELEASE_HOOKS)
		    _sharedstate->AddDelayedReleaseHook(_hook, (SQUserPointer)sq_aligning(this + 1), _size);
#else
		    _hook((SQUserPointer)sq_aligning(this + 1),_size, 0);
#endif
		}
		SQInteger tsize = _size;
		this->~SQUserData();
		SQ_FREE(this, sq_aligning(sizeof(SQUserData)) + tsize);
	}


	SQInteger _size;
	SQRELEASEHOOK _hook;
	SQUserPointer _typetag;
	//SQChar _val[1];
};

#endif //_SQUSERDATA_H_
