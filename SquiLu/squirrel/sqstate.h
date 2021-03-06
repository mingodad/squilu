/*	see copyright notice in squirrel.h */
#ifndef _SQSTATE_H_
#define _SQSTATE_H_

#include "squtils.h"
#include "sqobject.h"
struct SQString;
struct SQTable;
//max number of character for a printed number
#define NUMBER_MAX_CHAR 50

struct SQStringTable
{
	SQStringTable(SQSharedState*ss);
	~SQStringTable();
	SQString *Add(const SQChar *,SQInteger len);
	SQString *Add(SQString *strBuf);
	SQString *NewStrBuf(SQInteger len);
	void DeleteStrBuf(SQString *);
	void Remove(SQString *);
private:
    SQString *Contains(const SQChar *news, SQInteger &len, SQHash &newhash, SQHash &h);
	SQString *Add(SQString *strBuf, SQHash newhash, SQHash h);
	void Resize(SQInteger size);
	void AllocNodes(SQInteger size);
	SQString **_strings;
	SQUnsignedInteger _numofslots;
	SQUnsignedInteger _slotused;
	SQSharedState *_sharedstate;
};

struct RefTable {
	struct RefNode {
		SQObjectPtr obj;
		SQUnsignedInteger refs;
		struct RefNode *next;
	};
	RefTable();
	~RefTable();
	void AddRef(SQObject &obj);
	SQBool Release(SQObject &obj);
	SQUnsignedInteger GetRefCount(SQObject &obj);
#ifndef NO_GARBAGE_COLLECTOR
	void Mark(SQCollectable **chain);
#endif
	void Finalize();
private:
	RefNode *Get(SQObject &obj,SQHash &mainpos,RefNode **prev,bool add);
	RefNode *Add(SQHash mainpos,SQObject &obj);
	void Resize(SQUnsignedInteger size);
	void AllocNodes(SQUnsignedInteger size);
	SQUnsignedInteger _numofslots;
	SQUnsignedInteger _slotused;
	RefNode *_nodes;
	RefNode *_freelist;
	RefNode **_buckets;
};

#define ADD_STRING(ss,str,len) ss->_stringtable->Add(str,len)
#define REMOVE_STRING(ss,bstr) ss->_stringtable->Remove(bstr)

struct SQObjectPtr;

#ifdef SQ_WITH_DELAYED_RELEASE_HOOKS
struct SQDelayedReleseHook {
    SQRELEASEHOOK hook;
    SQUserPointer ptr;
    SQInteger size;
};
#endif // SQ_WITH_DELAYED_RELEASE_HOOKS

struct SQSharedState
{
	SQSharedState();
	~SQSharedState();
	void Init();
public:
	SQChar* GetScratchPad(SQInteger size);
	SQChar* GetScratchStr(SQInteger size);
	SQString* AddScratchStr();
	SQBool DelScratchStr(SQChar *s);
	SQInteger GetMetaMethodIdxByName(const SQObjectPtr &name);
#ifndef NO_GARBAGE_COLLECTOR
	SQInteger CollectGarbage(SQVM *vm);
	void RunMark(SQVM *vm,SQCollectable **tchain);
	SQInteger ResurrectUnreachable(SQVM *vm);
	static void MarkObject(SQObjectPtr &o,SQCollectable **chain);
#endif
	SQObjectPtrVec *_metamethods;
	SQObjectPtr _metamethodsmap;
	SQObjectPtrVec *_systemstrings;
	SQObjectPtrVec *_types;
	SQObjectPtr _defined_names;
	SQStringTable *_stringtable;
	RefTable _refs_table;
	SQObjectPtr _registry;
	SQObjectPtr _consts;
	SQObjectPtr _constructoridx;
	SQObjectPtr _destructoridx;
	SQObjectPtr _define_squilu;
	SQObjectPtr _define_squilu_int_sz;
	SQObjectPtr _define_squilu_float_sz;
	SQObjectPtr _define_squilu_ptr_sz;
#ifndef NO_GARBAGE_COLLECTOR
	SQCollectable *_gc_chain;
#endif
#ifdef SQ_WITH_DELAYED_RELEASE_HOOKS
    void AddDelayedReleaseHook(SQRELEASEHOOK hook, SQUserPointer ptr, SQInteger size);
    void CallDelayedReleaseHooks(SQVM *vm, int count=0);
#endif // SQ_WITH_DELAYED_RELEASE_HOOKS
	SQObjectPtr _root_vm;
	SQObjectPtr _table_default_delegate_squirrel;
	static SQRegFunction _table_default_delegate_squirrel_funcz[];
	SQObjectPtr _table_default_delegate;
	static SQRegFunction _table_default_delegate_funcz[];
	SQObjectPtr _array_default_delegate;
	static SQRegFunction _array_default_delegate_funcz[];
	SQObjectPtr _string_default_delegate;
	static SQRegFunction _string_default_delegate_funcz[];
	SQObjectPtr _number_default_delegate;
	static SQRegFunction _number_default_delegate_funcz[];
	SQObjectPtr _generator_default_delegate;
	static SQRegFunction _generator_default_delegate_funcz[];
	SQObjectPtr _closure_default_delegate;
	static SQRegFunction _closure_default_delegate_funcz[];
	SQObjectPtr _thread_default_delegate;
	static SQRegFunction _thread_default_delegate_funcz[];
	SQObjectPtr _class_default_delegate;
	static SQRegFunction _class_default_delegate_funcz[];
	SQObjectPtr _instance_default_delegate;
	static SQRegFunction _instance_default_delegate_funcz[];
	SQObjectPtr _weakref_default_delegate;
	static SQRegFunction _weakref_default_delegate_funcz[];

	SQCOMPILERERROR _compilererrorhandler;
	SQPRINTFUNCTION _printfunc;
	SQPRINTFUNCTION _errorfunc;
	SQUserPointer _foreignptr;
	SQRELEASEHOOK _releasehook;
private:
	SQChar *_scratchpad;
	SQString *_scratchstr;
	SQInteger _scratchpadsize;
#ifdef SQ_WITH_DELAYED_RELEASE_HOOKS
	bool _already_in_CallDelayedReleaseHooks;
	sqvector<SQDelayedReleseHook> _delayed_release_hook;
#endif // SQ_WITH_DELAYED_RELEASE_HOOKS
public: //moved here for alignment purposes
	bool _debuginfo;
	bool _notifyallexceptions;
};

#define _sp(s) (_sharedstate->GetScratchPad(s))
#define _spval (_sharedstate->GetScratchPad(-1))

#define _table_ddel		_table(_sharedstate->_table_default_delegate)
#define _array_ddel		_table(_sharedstate->_array_default_delegate)
#define _string_ddel	_table(_sharedstate->_string_default_delegate)
#define _number_ddel	_table(_sharedstate->_number_default_delegate)
#define _generator_ddel	_table(_sharedstate->_generator_default_delegate)
#define _closure_ddel	_table(_sharedstate->_closure_default_delegate)
#define _thread_ddel	_table(_sharedstate->_thread_default_delegate)
#define _class_ddel		_table(_sharedstate->_class_default_delegate)
#define _instance_ddel	_table(_sharedstate->_instance_default_delegate)
#define _weakref_ddel	_table(_sharedstate->_weakref_default_delegate)

#ifdef SQUNICODE //rsl REAL STRING LEN
#define rsl(l) ((l)<<1)
#else
#define rsl(l) (l)
#endif

//extern SQObjectPtr _null_;

bool CompileTypemask(SQIntVec &res,const SQChar *typemask);


#endif //_SQSTATE_H_
