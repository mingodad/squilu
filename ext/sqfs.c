/*
** LuaFileSystem
** Copyright Kepler Project 2003 (http://www.keplerproject.org/luafilesystem)
**
** File system manipulation library.
** This library offers these functions:
**   lfs.attributes (filepath [, attributename])
**   lfs.chdir (path)
**   lfs.currentdir ()
**   lfs.dir (path)
**   lfs.lock (fh, mode)
**   lfs.lock_dir (path)
**   lfs.mkdir (path)
**   lfs.rmdir (path)
**   lfs.setmode (filepath, mode)
**   lfs.symlinkattributes (filepath [, attributename]) -- thanks to Sam Roberts
**   lfs.touch (filepath [, atime [, mtime]])
**   lfs.unlock (fh)
**
** $Id: lfs.c,v 1.61 2009/07/04 02:10:16 mascarenhas Exp $
*
* Ported to Squirrel by Domingo Alvarez Duarte
*/

#ifndef _WIN32
#ifndef _AIX
#define _FILE_OFFSET_BITS 64 /* Linux, Solaris and HP-UX */
#else
#define _LARGE_FILES 1 /* AIX */
#endif
#endif

#define _LARGEFILE64_SOURCE

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#include <io.h>
#include <sys/locking.h>
#ifdef __BORLANDC__
 #include <utime.h>
#else
 #include <sys/utime.h>
#endif
#include <fcntl.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utime.h>
#endif

#include "squirrel.h"
SQ_OPT_STRING_STRLEN();
#include "sqfs.h"

/* Define 'strerror' for systems that do not implement it */
#ifdef NO_STRERROR
#define strerror(_)	"System unable to describe the error"
#endif

/* Define 'getcwd' for systems that do not implement it */
#ifdef NO_GETCWD
#define getcwd(p,s)	NULL
#define getcwd_error	"Function 'getcwd' not provided by system"
#else
#define getcwd_error    strerror(errno)
  #ifdef _WIN32
	 /* MAX_PATH seems to be 260. Seems kind of small. Is there a better one? */
    #define LFS_MAXPATHLEN MAX_PATH
  #else
	/* For MAXPATHLEN: */
    #include <sys/param.h>
    #define LFS_MAXPATHLEN MAXPATHLEN
  #endif
#endif

#define DIR_METATABLE "directory metatable"
typedef struct dir_data {
        int  closed;
#ifdef _WIN32
        long hFile;
        char pattern[MAX_PATH+1];
#else
        DIR *dir;
#endif
} dir_data;

#define LOCK_METATABLE "lock metatable"

#ifdef _WIN32
 #ifdef __BORLANDC__
  #define lfs_setmode(L,file,m)   ((void)L, setmode(_fileno(file), m))
  #define STAT_STRUCT struct stati64
 #else
  #define lfs_setmode(L,file,m)   ((void)L, _setmode(_fileno(file), m))
  #define STAT_STRUCT struct _stati64
 #endif
#define STAT_FUNC _stati64
#define LSTAT_FUNC STAT_FUNC
#else
#define _O_TEXT               0
#define _O_BINARY             0
#define lfs_setmode(L,file,m)   ((void)L, (void)file, (void)m, 0)
#define STAT_STRUCT struct stat
#define STAT_FUNC stat
#define LSTAT_FUNC lstat
#endif

/*
** This function changes the working (current) directory
*/
static int sqfs_chdir (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, path);
	if (chdir(path)) {
	    return sq_throwerror(v, "Unable to change working directory to '%s'\n%s\n",
				path, chdir_error);
	} else {
		sq_pushbool (v, SQTrue);
		return 1;
	}
}

/*
** This function returns the current directory
** If unable to get the current directory, it returns nil
**  and a string describing the error
*/
static int sqfs_currentdir (HSQUIRRELVM v) {
  SQChar *path;
  /* Passing (NULL, 0) is not guaranteed to work. Use a temp buffer and size instead. */
  char buf[LFS_MAXPATHLEN];
  if ((path = getcwd(buf, LFS_MAXPATHLEN)) == NULL) {
    return sq_throwerror(v, getcwd_error);
  }
  else {
    sq_pushstring(v, buf, -1);
    return 1;
  }
}
#if 0
/*
** Check if the given element on the stack is a file and returns it.
*/
static FILE *check_file (HSQUIRRELVM v, int idx, const SQChar *funcname) {
	FILE **fh = (FILE **)luaL_checkudata (L, idx, "FILE*");
	if (fh == NULL) {
		return sq_throwerror(v, "%s: not a file", funcname);
	} else if (*fh == NULL) {
		return sq_throwerror(v, "%s: closed file", funcname);
		return 0;
	} else
		return *fh;
}


/*
**
*/
static int _file_lock (HSQUIRRELVM v, FILE *fh, const char *mode, const long start, long len, const char *funcname) {
	int code;
#ifdef _WIN32
	/* lkmode valid values are:
	   LK_LOCK    Locks the specified bytes. If the bytes cannot be locked, the program immediately tries again after 1 second. If, after 10 attempts, the bytes cannot be locked, the constant returns an error.
	   LK_NBLCK   Locks the specified bytes. If the bytes cannot be locked, the constant returns an error.
	   LK_NBRLCK  Same as _LK_NBLCK.
	   LK_RLCK    Same as _LK_LOCK.
	   LK_UNLCK   Unlocks the specified bytes, which must have been previously locked.

	   Regions should be locked only briefly and should be unlocked before closing a file or exiting the program.

	   http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_crt__locking.asp
	*/
	int lkmode;
	switch (*mode) {
		case 'r': lkmode = LK_NBLCK; break;
		case 'w': lkmode = LK_NBLCK; break;
		case 'u': lkmode = LK_UNLCK; break;
		default : return sq_throwerror (v, "%s: invalid mode", funcname);
	}
	if (!len) {
		fseek (fh, 0L, SEEK_END);
		len = ftell (fh);
	}
	fseek (fh, start, SEEK_SET);
	code = _locking (fileno(fh), lkmode, len);
#else
	struct flock f;
	switch (*mode) {
		case 'w': f.l_type = F_WRLCK; break;
		case 'r': f.l_type = F_RDLCK; break;
		case 'u': f.l_type = F_UNLCK; break;
		default : return sq_throwerror(v, "%s: invalid mode", funcname);
	}
	f.l_whence = SEEK_SET;
	f.l_start = (off_t)start;
	f.l_len = (off_t)len;
	code = fcntl (fileno(fh), F_SETLK, &f);
#endif
	return (code != -1);
}

#ifdef _WIN32
typedef struct sqfs_Lock {
  HANDLE fd;
} lfs_Lock;
static int sqfs_lock_dir(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, path);
  HANDLE fd;
  lfs_Lock *lock;
  char *ln;
  const char *lockfile = "/lockfile.sqfs";
  ln = (char*)sq_malloc(path_size + strlen(lockfile) + 1);
  if(!ln) return sq_throwerror(v, strerror(errno));

  strcpy(ln, path); strcat(ln, lockfile);
  if((fd = CreateFile(ln, GENERIC_WRITE, 0, NULL, CREATE_NEW,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL)) == INVALID_HANDLE_VALUE) {
        int en = GetLastError();
        free(ln);
        if(en == ERROR_FILE_EXISTS || en == ERROR_SHARING_VIOLATION)
                sq_pushliteral(v, "File exists");
        else
                sq_pushstring(v, strerror(en), -1);
        return 1;
  }
  free(ln);
  lock = (lfs_Lock*)lua_newuserdata(L, sizeof(lfs_Lock));
  lock->fd = fd;
  luaL_getmetatable (L, LOCK_METATABLE);
  lua_setmetatable (L, -2);
  return 1;
}
static int sqfs_unlock_dir(HSQUIRRELVM v) {
  lfs_Lock *lock = luaL_checkudata(L, 1, LOCK_METATABLE);
  CloseHandle(lock->fd);
  return 0;
}
#else
typedef struct sqfs_Lock {
  char *ln;
} lfs_Lock;
static int sqfs_lock_dir(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, path);
  lfs_Lock *lock;
  char *ln;
  const char *lockfile = "/lockfile.sqfs";
  lock = (lfs_Lock*)lua_newuserdata(L, sizeof(sqfs_Lock));
  ln = (char*)sq_malloc(path_size + strlen(lockfile) + 1);
  if(!ln) return sq_throwerror(v, strerror(errno));
  strcpy(ln, path); strcat(ln, lockfile);
  if(symlink("lock", ln) == -1) {
    free(ln);
    return sq_throwerror(v, strerror(errno));
  }
  lock->ln = ln;
  luaL_getmetatable (L, LOCK_METATABLE);
  lua_setmetatable (L, -2);
  return 1;
}
static int lfs_unlock_dir(HSQUIRRELVM v) {
  lfs_Lock *lock = luaL_checkudata(L, 1, LOCK_METATABLE);
  if(lock->ln) {
    unlink(lock->ln);
    free(lock->ln);
    lock->ln = NULL;
  }
  return 0;
}
#endif

#ifdef _WIN32
static int lfs_g_setmode (HSQUIRRELVM v, FILE *f, int arg) {
  static const int mode[] = {_O_TEXT, _O_BINARY};
  static const char *const modenames[] = {"text", "binary", NULL};
  int op = luaL_checkoption(L, arg, NULL, modenames);
  int res = lfs_setmode(L, f, mode[op]);
  if (res != -1) {
    int i;
    lua_pushboolean(L, 1);
    for (i = 0; modenames[i] != NULL; i++) {
      if (mode[i] == res) {
        lua_pushstring(L, modenames[i]);
        goto exit;
      }
    }
    lua_pushnil(L);
  exit:
    return 2;
  } else {
    int en = errno;
    lua_pushnil(L);
    lua_pushfstring(L, "%s", strerror(en));
    lua_pushinteger(L, en);
    return 3;
  }
}
#else
static int lfs_g_setmode (HSQUIRRELVM v, FILE *f, int arg) {
  return sq_throwerror(v, "setmode not supported on this platform");
}
#endif

static int sqfs_setmode(HSQUIRRELVM v) {
  return lfs_g_setmode(v, check_file(v, 1, "setmode"), 2);
}

/*
** Locks a file.
** @param #1 File handle.
** @param #2 String with lock mode ('w'rite, 'r'ead).
** @param #3 Number with start position (optional).
** @param #4 Number with length (optional).
*/
static int sqfs_lock (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 3, mode);
    SQ_OPT_INTEGER(v, 4, start, 0);
    SQ_OPT_INTEGER(v, 5, len, 0);
	FILE *fh = check_file (L, 1, SC("lock");
	if (_file_lock (v, fh, mode, start, len, _SC("lock")) {
		sq_pushbool (v, SQTrue);
		return 1;
	} else {
		return sq_throwerror(v, "%s", strerror(errno));
	}
}


/*
** Unlocks a file.
** @param #1 File handle.
** @param #2 Number with start position (optional).
** @param #3 Number with length (optional).
*/
static int sqfs_unlock (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 3, start, 0);
    SQ_OPT_INTEGER(v, 4, len, 0);
	FILE *fh = check_file (L, 1, "unlock");
	if (_file_lock (v, fh, "u", start, len, "unlock")) {
		sq_pushbool (v, SQTrue);
		return 1;
	} else {
		return sq_throwerror (v, "%s", strerror(errno));
	}
}
#endif

/*
** Creates a link.
** @param #1 Object to link to.
** @param #2 Name of link.
** @param #3 True if link is symbolic (optional).
*/
static int sqfs_link(HSQUIRRELVM v)
{
#ifndef _WIN32
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, oldpath);
    SQ_GET_STRING(v, 3, newpath);
    SQ_OPT_BOOL(v, 4, bsym, SQFalse);
    sq_pushinteger(v, (bsym ? symlink : link)(oldpath, newpath));
    return 1;
#else
    return sq_throwerror(v, "make_link is not supported on Windows");
#endif
}

static int sqfs_mkdir (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, path);
	int fail;
#ifdef _WIN32
	int oldmask = umask (0);
	fail = _mkdir (path);
#else
	mode_t oldmask = umask( (mode_t)0 );
	fail =  mkdir (path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
	                     S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH );
#endif
	if (fail) {
        return sq_throwerror (v, "%s", strerror(errno));
	}
	umask (oldmask);
	sq_pushbool (v, SQTrue);
	return 1;
}

/*
** Removes a directory.
** @param #1 Directory path.
*/
static int sqfs_rmdir (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, path);
	int fail;

	fail = rmdir (path);

	if (fail) {
		return sq_throwerror (v, "%s", strerror(errno));
	}
	sq_pushbool (v, SQTrue);
	return 1;
}

static const SQChar SQFS_DIR_TAG[] = _SC("sqfs_dir_tag");
#define GET_dir_INSTANCE() SQ_GET_INSTANCE(v, 1, dir_data, SQFS_DIR_TAG)

static void _dir_close_dir(dir_data *dir)
{
#ifdef _WIN32
    if (!dir->closed && dir->hFile) {
            _findclose (dir->hFile);
    }
#else
    if (!dir->closed && dir->dir) {
            closedir (dir->dir);
    }
#endif
    dir->closed = 1;
}

static SQRESULT _dir_releasehook(SQUserPointer p, SQInteger size)
{
	dir_data *dir = ((dir_data *)p);
	_dir_close_dir(dir);
	sq_free(dir, sizeof(dir_data));
	return 0;
}

static SQRESULT _dir_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, path);
	dir_data *dir = (dir_data*)sq_malloc(sizeof(dir_data));
	dir->closed = 0;
#ifdef _WIN32
    dir->hFile = 0L;
    if (strlen(path) > MAX_PATH-2)
      return sq_throwerror(v, "path too long: %s", path);
    else
      scsnprintf (dir->pattern, sizeof(dir->pattern), "%s/*", path);
#else
    dir->dir = opendir (path);
    if (dir->dir == NULL)
      return sq_throwerror(v, "cannot open %s: %s", path, strerror (errno));
#endif
    sq_setinstanceup(v, 1, dir); //replace self for this instance with this new sqlite3_stmt
    sq_setreleasehook(v,1, _dir_releasehook);
	return 1;
}

static SQRESULT _dir_close(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_dir_INSTANCE();
    _dir_close_dir(self);
    return 0;
}

static SQRESULT _dir__get(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_dir_INSTANCE();
    sq_pushuserpointer(v, self);
    if(sq_getonregistrytable(v) != SQ_OK) sq_pushnull(v);
    //do not worry about registry table it will be removed anyway when restoring the stack
    return 1;
}

static SQRESULT _dir__nexti(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_dir_INSTANCE();
	SQInteger idx;
	if(sq_gettype(v,2) == OT_NULL) idx = 0;
	else
	{
        if(!SQ_SUCCEEDED(sq_getinteger(v, 2, &idx)))
            return sq_throwerror(v,_SC("internal error (_nexti) wrong argument type"));
        ++idx;
	}
#ifdef _WIN32
    struct _finddata_t c_file;
#else
    struct dirent *entry;
#endif
    const char *fname;
    if(self->closed) return sq_throwerror(v, "closed directory");
#ifdef _WIN32
    if (self->hFile == 0L) { /* first entry */
        if ((self->hFile = _findfirst (self->pattern, &c_file)) == -1L) {
                self->closed = 1;
                return sq_throwerror(v, strerror (errno));
        }
    } else { /* next entry */
        if (_findnext (self->hFile, &c_file) == -1L) {
                /* no more entries => close directory */
                _findclose (self->hFile);
                self->closed = 1;
                sq_pushnull(v);
                return 1;
        }
    }
    fname = c_file.name;
#else
    if ((entry = readdir (self->dir)) == NULL) {
        /* no more entries => close directory */
        closedir (self->dir);
        self->closed = 1;
        sq_pushnull(v);
        return 1;
    }
    fname = entry->d_name;
#endif
    sq_pushuserpointer(v, self);
    sq_pushstring(v, fname, -1);
    sq_setonregistrytable(v);

	sq_pushinteger(v, idx);
	return 1;
}

#define _DECL_DIR_FUNC(name,nparams,typecheck) {_SC(#name),_dir_##name,nparams,typecheck}
static SQRegFunction _dir_methods[] = {
	_DECL_DIR_FUNC(constructor,-1,_SC("xs")),
	_DECL_DIR_FUNC(close,1,_SC("x")),
	_DECL_DIR_FUNC(_nexti,2,_SC("x.")),
	_DECL_DIR_FUNC(_get,2,_SC("xn")),
	{0,0,0,0}
};


#ifdef _WIN32
 #ifndef S_ISDIR
   #define S_ISDIR(mode)  (mode&_S_IFDIR)
 #endif
 #ifndef S_ISREG
   #define S_ISREG(mode)  (mode&_S_IFREG)
 #endif
 #ifndef S_ISLNK
   #define S_ISLNK(mode)  (0)
 #endif
 #ifndef S_ISSOCK
   #define S_ISSOCK(mode)  (0)
 #endif
 #ifndef S_ISFIFO
   #define S_ISFIFO(mode)  (0)
 #endif
 #ifndef S_ISCHR
   #define S_ISCHR(mode)  (mode&_S_IFCHR)
 #endif
 #ifndef S_ISBLK
   #define S_ISBLK(mode)  (0)
 #endif
#endif
/*
** Convert the inode protection mode to a string.
*/
#ifdef _WIN32
static const SQChar *mode2string (unsigned short mode) {
#else
static const SQChar *mode2string (mode_t mode) {
#endif
  if ( S_ISREG(mode) )
    return _SC("file");
  else if ( S_ISDIR(mode) )
    return _SC("directory");
  else if ( S_ISLNK(mode) )
	return _SC("link");
  else if ( S_ISSOCK(mode) )
    return _SC("socket");
  else if ( S_ISFIFO(mode) )
	return _SC("named pipe");
  else if ( S_ISCHR(mode) )
	return _SC("char device");
  else if ( S_ISBLK(mode) )
	return _SC("block device");
  else
	return _SC("other");
}


/*
** Set access time and modification values for file
*/
static int sqfs_touch (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, file);
	struct utimbuf utb, *buf;

	if (_top_ == 2) /* set to current date/time */
		buf = NULL;
	else {
	    SQ_OPT_INTEGER(v, 3, actime, 0);
	    SQ_OPT_INTEGER(v, 4, modtime, actime);
		utb.actime = (time_t)actime;
		utb.modtime = (time_t)modtime;
		buf = &utb;
	}
	if (utime (file, buf)) {
		return sq_throwerror(v, "%s", strerror (errno));
	}
	sq_pushbool (v, SQTrue);
	return 1;
}


/* inode protection mode */
static void push_st_mode (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushstring (v, mode2string (info->st_mode), -1);
}
/* device inode resides on */
static void push_st_dev (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_dev);
}
/* inode's number */
static void push_st_ino (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_ino);
}
/* number of hard links to the file */
static void push_st_nlink (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_nlink);
}
/* user-id of owner */
static void push_st_uid (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_uid);
}
/* group-id of owner */
static void push_st_gid (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_gid);
}
/* device type, for special file inode */
static void push_st_rdev (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_rdev);
}
/* time of last access */
static void push_st_atime (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, info->st_atime);
}
/* time of last data modification */
static void push_st_mtime (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, info->st_mtime);
}
/* time of last file status change */
static void push_st_ctime (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, info->st_ctime);
}
/* file size, in bytes */
static void push_st_size (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_size);
}
#ifndef _WIN32
/* blocks allocated for file */
static void push_st_blocks (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_blocks);
}
/* optimal file system I/O blocksize */
static void push_st_blksize (HSQUIRRELVM v, STAT_STRUCT *info) {
	sq_pushinteger (v, (SQInteger)info->st_blksize);
}
#endif
static void push_invalid (HSQUIRRELVM v, STAT_STRUCT *info) {
  sq_throwerror(v, "invalid attribute name");
#ifndef _WIN32
  info->st_blksize = 0; /* never reached */
#endif
}

typedef void (*_push_function) (HSQUIRRELVM v, STAT_STRUCT *info);

struct _stat_members {
	const char *name;
	_push_function push;
};

struct _stat_members members[] = {
	{ "mode",         push_st_mode },
	{ "dev",          push_st_dev },
	{ "ino",          push_st_ino },
	{ "nlink",        push_st_nlink },
	{ "uid",          push_st_uid },
	{ "gid",          push_st_gid },
	{ "rdev",         push_st_rdev },
	{ "access",       push_st_atime },
	{ "modification", push_st_mtime },
	{ "change",       push_st_ctime },
	{ "size",         push_st_size },
#ifndef _WIN32
	{ "blocks",       push_st_blocks },
	{ "blksize",      push_st_blksize },
#endif
	{ NULL, push_invalid }
};

/*
** Get file or symbolic link information
*/
static int _file_info_ (HSQUIRRELVM v, int (*st)(const SQChar*, STAT_STRUCT*)) {
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, file);
	int i;
	STAT_STRUCT info;

	if (st(file, &info)) {
		return sq_throwerror(v, "cannot obtain information from file `%s'", file);
	}
	if(_top_ > 2){
	    int ptype = sq_gettype(v, 3);
        if (ptype == OT_STRING) {
            int iv;
            SQ_GET_STRING(v, 3, member);
            if (strcmp (member, "mode") == 0) iv = 0;
#ifndef _WIN32
            else if (strcmp (member, "blocks")  == 0) iv = 11;
            else if (strcmp (member, "blksize") == 0) iv = 12;
#endif
            else /* look for member */
                for (iv = 1; members[iv].name; iv++)
                    if (*members[iv].name == *member)
                        break;
            /* push member value and return */
            members[iv].push (v, &info);
            return 1;
        } else if (ptype != OT_TABLE)
            /* creates a table if none is given */
            sq_newtable (v);
	}
	else sq_newtable (v);
	/* stores all members in table on top of the stack */
	for (i = 0; members[i].name; i++) {
		sq_pushstring (v, members[i].name, -1);
		members[i].push (v, &info);
		sq_rawset (v, -3);
	}
	return 1;
}


/*
** Get file information using stat.
*/
static int sqfs_attributes (HSQUIRRELVM v) {
	return _file_info_ (v, STAT_FUNC);
}


/*
** Get symbolic link information using lstat.
*/
#ifndef _WIN32
static int sqfs_symlinkattributes (HSQUIRRELVM v) {
	return _file_info_ (v, LSTAT_FUNC);
}
#else
static int sqfs_symlinkattributes (HSQUIRRELVM v) {
  sq_pushliteral(v, "symlinkattributes not supported on this platform");
  return 1;
}
#endif


/*
** Assumes the table is on top of the stack.
*/
static void set_info (HSQUIRRELVM v) {
	sq_pushliteral (v, "_COPYRIGHT");
	sq_pushliteral (v, "Copyright (C) Copyright (C) 2003-2012 Kepler Project, Domingo Alvarez Duarte");
	sq_rawset (v, -3);
	sq_pushliteral (v, "_DESCRIPTION");
	sq_pushliteral (v, "LuaFileSystem is a Lua library developed to complement the set of"
                 " functions related to file systems offered by the standard Lua distribution"
                 ", adapted to Squirrel by Domingo Alvarez Duarte");
	sq_rawset (v, -3);
	sq_pushliteral (v, "_VERSION");
	sq_pushliteral (v, "LuaFileSystem 1.61");
	sq_rawset (v, -3);
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sqfs_##name,nparams,tycheck}
static SQRegFunction sqfs_methods[] =
{
	_DECL_FUNC(attributes,  -2, _SC(".ss")),
	_DECL_FUNC(chdir,  2, _SC(".s")),
	_DECL_FUNC(currentdir,  1, _SC(".")),
/*
	_DECL_FUNC(lock,  -3, _SC("xsii")),
*/
	_DECL_FUNC(link,  -3, _SC(".ssb")),
	_DECL_FUNC(mkdir,  2, _SC(".s")),
	_DECL_FUNC(rmdir,  2, _SC(".s")),
	_DECL_FUNC(symlinkattributes,  -2, _SC(".ss")),

//	_DECL_FUNC(setmode,  3, _SC(".ss")),
	_DECL_FUNC(touch,  -2, _SC(".sii")),
/*
	_DECL_FUNC(unlock,  -2, _SC(".sii")),
	_DECL_FUNC(lock_dir,  -2, _SC(".si")),
*/
	{0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqstd_register_sqfs(HSQUIRRELVM v)
    {
        sq_pushstring(v,_SC("sqfs"),-1);
        sq_newtable(v);
        set_info(v);
        sq_insert_reg_funcs(v, sqfs_methods);

        sq_pushliteral(v, _SC("dir"));
        sq_newclass(v, SQFalse);
        sq_settypetag(v,-1,(void*)SQFS_DIR_TAG);
        sq_insert_reg_funcs(v, _dir_methods);
        sq_newslot(v,-3,SQTrue);

        sq_newslot(v,-3,SQTrue);

        return 1;
    }

#ifdef __cplusplus
}
#endif
