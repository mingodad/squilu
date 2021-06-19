/*	see copyright notice in squirrel.h */
#ifndef _SQSTRING_H_
#define _SQSTRING_H_

inline SQHash _hashstr (const SQChar *s, size_t l)
{
		SQHash h = (SQHash)l;  /* seed */
		size_t step = (l>>5)+1;  /* if string is too long, don't hash all its chars */
		for (; l>=step; l-=step)
			h ^= ((h<<5)+(h>>2)+(SQUChar)(s[l-1]));
		return h;
}

#define SQSTRING_PAD_VAL (sizeof(void*))
#define SQSTRING_CALCULATED_SIZE(sl) \
    ((sl >= SQSTRING_PAD_VAL) ? (sizeof(SQString)+(sl-SQSTRING_PAD_VAL+1)) \
     : (sizeof(SQString)))
struct SQString : public SQRefCounted
{
	SQString(){}
	~SQString(){}
public:
	static SQString *Create(SQSharedState *ss, const SQChar *, SQInteger len = -1 );
	static SQString *Create(SQSharedState *ss, SQString * );
	SQInteger Next(const SQObjectPtr &refpos, SQObjectPtr &outkey, SQObjectPtr &outval);
	void Release();
	SQSharedState *_sharedstate;
	SQString *_next; //chain for the string table
	SQInteger _len;
	SQHash _hash;
	SQChar _val[SQSTRING_PAD_VAL];
	//<FIXME> Padding not accounted
};

struct SQStringUtf8 : public SQString {
    SQInteger Next(const SQObjectPtr &refpos, SQObjectPtr &outkey, SQObjectPtr &outval);
};


#endif //_SQSTRING_H_
