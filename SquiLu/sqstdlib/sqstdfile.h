/*	see copyright notice in squirrel.h */
#ifndef _SQSTDFILE_H_
#define _SQSTDFILE_H_

//File
struct SQFile : public SQStream {
	SQFile() { _handle = 0; _owns = false;}
	SQFile(SQFILE file, bool owns) { _handle = file; _owns = owns;}
	virtual ~SQFile() { Close(); }
	virtual bool Open(const SQChar *filename ,const SQChar *mode) {
		Close();
		if( (_handle = sqstd_fopen(filename,mode)) ) {
			_owns = true;
			return true;
		}
		return false;
	}
	virtual void Close() {
		if(_handle && _owns) {
			sqstd_fclose(_handle);
			_handle = 0;
			_owns = false;
		}
	}
	SQInteger Read(void *buffer,SQInteger size) {
		return sqstd_fread(buffer,1,size,_handle);
	}
	SQInteger Gets(char *buffer,SQInteger size) {
	    SQChar *res = sqstd_fgets(buffer,size,_handle);
		return res ? scstrlen(res) : 0;
	}
	SQInteger Write(const void *buffer,SQInteger size) {
		return sqstd_fwrite((const SQUserPointer)buffer,1,size,_handle);
	}
	SQInteger Flush() {
		return sqstd_fflush(_handle);
	}
	SQInteger Tell() {
		return sqstd_ftell(_handle);
	}
	SQInteger Len() {
		SQInteger prevpos=Tell();
		Seek(0,SQ_SEEK_END);
		SQInteger size=Tell();
		Seek(prevpos,SQ_SEEK_SET);
		return size;
	}
	SQInteger Seek(SQInteger offset, SQInteger origin)	{
		return sqstd_fseek(_handle,offset,origin);
	}
	bool IsValid() { return _handle?true:false; }
	bool EOS() { return Tell()==Len()?true:false;}
	SQFILE GetHandle() {return _handle;}
protected:
	SQFILE _handle;
	bool _owns;
};

#endif
