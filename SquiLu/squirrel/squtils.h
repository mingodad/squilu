/*	see copyright notice in squirrel.h */
#ifndef _SQUTILS_H_
#define _SQUTILS_H_

#include <new>

#ifndef SQ_EXCLUDE_DEFAULT_MEMFUNCTIONS
void *sq_vm_malloc(SQUnsignedInteger size);
void *sq_vm_realloc(void *p,SQUnsignedInteger oldsize,SQUnsignedInteger size);
void sq_vm_free(void *p,SQUnsignedInteger size);
#else
#include <stdlib.h>
#define sq_vm_malloc(x) malloc(x)
#define sq_vm_realloc(a, b, c) realloc(a, c)
#define sq_vm_free(x, y) free(x)
#endif

#define sq_new(__ptr,__type) {__ptr=(__type *)sq_vm_malloc(sizeof(__type));new (__ptr) __type;}
#define sq_delete(__ptr,__type) {__ptr->~__type();sq_vm_free(__ptr,sizeof(__type));}
#define SQ_MALLOC(__size) sq_vm_malloc((__size));
#define SQ_FREE(__ptr,__size) sq_vm_free((__ptr),(__size));
#define SQ_REALLOC(__ptr,__oldsize,__size) sq_vm_realloc((__ptr),(__oldsize),(__size));

#define sq_aligning(v) (((size_t)(v) + (SQ_ALIGNMENT-1)) & (~(SQ_ALIGNMENT-1)))

//sqvector mini vector class, supports objects by value
template<typename T> class sqvector
{
public:
	sqvector():_vals(NULL),_size(0),_allocated(0)
	{
	}
	sqvector(const sqvector<T>& v)
	{
		copy(v);
	}
	void copy(const sqvector<T>& v)
	{
		if(_size) {
			resize(0); //destroys all previous stuff
		}
		//resize(v._size);
		if(v._size > _allocated) {
			_realloc(v._size);
		}
		for(SQSizeType i = 0; i < v._size; i++) {
			new ((void *)&_vals[i]) T(v._vals[i]);
		}
		_size = v._size;
	}
	~sqvector()
	{
		if(_allocated) {
			for(SQSizeType i = 0; i < _size; i++)
				_vals[i].~T();
			SQ_FREE(_vals, (_allocated * sizeof(T)));
		}
	}
	void reserve(SQSizeType newsize) { _realloc(newsize); }
	void resize(SQSizeType newsize, const T& fill = T())
	{
		if(newsize > _allocated)
			_realloc(newsize);
		if(newsize > _size) {
			while(_size < newsize) {
				new ((void *)&_vals[_size]) T(fill);
				_size++;
			}
		}
		else{
			for(SQUnsignedInteger i = newsize; i < _size; i++) {
				_vals[i].~T();
			}
			_size = newsize;
		}
	}
	void shrinktofit() { if(_size > 4) { _realloc(_size); } }
	T& top() const { return _vals[_size - 1]; }
	inline SQSizeType size() const { return _size; }
	inline SQSizeType sizeOf() const { return sizeof(T); }
	bool empty() const { return (_size <= 0); }
	inline T &push_back(const T& val = T())
	{
		if(_allocated <= _size)
			_realloc(_size * 2);
		return *(new ((void *)&_vals[_size++]) T(val));
	}
	inline void pop_back()
	{
		_size--; _vals[_size].~T();
	}
	bool insert(SQSizeType idx, const T& val)
	{
	    if(idx > _size) return false;
		resize(_size + 1);
		for(SQSizeType i = _size - 1; i > idx; i--) {
			_vals[i] = _vals[i - 1];
		}
    	_vals[idx] = val;
    	return true;
	}
	bool remove(SQSizeType idx)
	{
	    if(idx < _size){
            _vals[idx].~T();
            if(idx < (_size - 1)) {
                memmove(&_vals[idx], &_vals[idx+1], sizeof(T) * (_size - idx - 1));
            }
            _size--;
            return true;
	    }
		return false;
	}
	void removeFromBegining(SQSizeType count)
	{
	    if(count <= _size){
	        for(SQSizeType i=0; i < count; ++i) _vals[i].~T();
            if(count < (_size - 1)) {
                memmove(&_vals[0], &_vals[count], sizeof(T) * (_size - count));
            }
            _size -= count;
	    }
	}
	SQSizeType capacity() { return _allocated; }
	inline T &back() const { return _vals[_size - 1]; }
	inline T get(SQSizeType pos) const{ return _vals[pos]; }
	inline void set(SQSizeType pos, T val) const{ _vals[pos] = val; }
	inline T& operator[](SQSizeType pos) const{ return _vals[pos]; }
	T* _vals;
private:
	void _realloc(SQSizeType newsize)
	{
		newsize = (newsize > 0)?newsize:4;
		_vals = (T*)SQ_REALLOC(_vals, _allocated * sizeof(T), newsize * sizeof(T));
		_allocated = newsize;
	}
	SQSizeType _size;
	SQSizeType _allocated;
};

class SQCharBuf : public sqvector<char>
{
public:
    void append(const char *p, SQSizeType count)
    {
        SQSizeType old_size = size();
        resize(old_size + count);
        memcpy(_vals+old_size, p, count);
    }
    const char *data(){return _vals;}
};

#endif //_SQUTILS_H_
