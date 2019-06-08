#ifdef __cplusplus
extern "C" {
#endif

//#ifdef USE_IPC

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  /* for malloc */
#include <assert.h>  /* for a few sanity tests */
#include <sqstdio.h>
#include <sqstdfile.h>

//#include "FileLock.h"
/* check for POSIX */
#if defined( unix ) || defined( __unix ) || defined( __unix__ ) || \
    (defined( __APPLE__ ) && defined( __MACH__ )) || \
    HAVE_UNISTD_H
#  include <unistd.h>
#  if defined( _POSIX_VERSION ) && _POSIX_VERSION >= 200112L
#    define HAVE_FLOCK
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

typedef off_t ipc_flock_off_t;
#  endif
#endif


/* check for Windows */
#if !defined( HAVE_FLOCK ) && \
    defined( _WIN32 ) && !defined( __CYGWIN__ )
#  define HAVE_FLOCK
#include <stddef.h>
#include <ctype.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>


typedef ULONGLONG ipc_flock_off_t;
#endif

static int ipc_err( char const* file, int line, char const* func,
                       int code ) {
  if( code != 0 ) {
    if( func != NULL )
      fprintf( stderr, "[%s:%d] error return (%d) in function '%s'\n",
               file, line, code, func );
    else
      fprintf( stderr, "[%s:%d]: error return (%d)\n",
               file, line, code );
    fflush( stderr );
  }
  return code;
}

#ifndef NDEBUG
#  if (defined( __STDC_VERSION__ ) && __STDC_VERSION__  >= 199901L) || \
      defined( __GNUC__ ) || defined( __clang__ )
#    define IPC_ERR( code ) (ipc_err( __FILE__, __LINE__, __func__, (int)(code) ))
#  elif defined( _MSC_VER ) && _MSC_VER >= 1100L
#    define IPC_ERR( code ) (ipc_err( __FILE__, __LINE__, __FUNCTION__, (int)(code) ))
#  else
#    define IPC_ERR( code ) (ipc_err( __FILE__, __LINE__, NULL, (int)(code) ))
#  endif
#else
#  define IPC_ERR( code ) ((int)(code))
#endif

#define IPC_EINTR( _rv, _call ) \
  do { \
    _rv = _call; \
  } while( _rv < 0 && errno == EINTR )

#if 0
//memmap
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


typedef off_t ipc_mmap_off_t;

#define MEMFILE_R  1
#define MEMFILE_W  2
#define MEMFILE_RW (MEMFILE_R|MEMFILE_W)

typedef struct {
  void* addr;
  size_t len;
} ipc_mmap_handle;

static void* ipc_mmap_addr( ipc_mmap_handle* h ) {
  return h->addr;
}

static size_t ipc_mmap_size( ipc_mmap_handle* h ) {
  return h->len;
}


static void ipc_mmap_error( char* buf, size_t len, int code ) {
  if( len > 0 && strerror_r( code, buf, len ) != (int)0 ) {
    strncpy( buf, "unknown error", len-1 );
    buf[ len-1 ] = '\0';
  }
}


static size_t ipc_mmap_pagesize( void ) {
  long result = sysconf( _SC_PAGESIZE );
  if( result < 1 )
    result = 4096;
  return (size_t)result;
}


static int ipc_mmap_open( ipc_mmap_handle* h, char const* name,
                          int mode, off_t offset, size_t size ) {
  int fd, oflags = 0, mmflags = 0;
  if( (mode & MEMFILE_RW) == MEMFILE_RW ) {
    oflags = O_RDWR;
    mmflags = PROT_READ | PROT_WRITE;
  } else if( mode & MEMFILE_R ) {
    oflags = O_RDONLY;
    mmflags = PROT_READ;
  } else if( mode & MEMFILE_W ) {
    oflags = O_RDWR;
    mmflags = PROT_WRITE;
  }
#ifdef O_CLOEXEC
  oflags |= O_CLOEXEC;
#endif
  fd = open( name, oflags );
  if( fd < 0 )
    return IPC_ERR( errno );
  h->len = size;
  if( size == 0 ) { /* figure out its size */
    struct stat buf;
    if( fstat( fd, &buf ) < 0 ) {
      int saved_errno = errno;
      close( fd );
      return IPC_ERR( saved_errno );
    }
    if( buf.st_size < offset ) {
      close( fd );
      return IPC_ERR( EINVAL );
    }
    if( buf.st_size - offset > (size_t)-1 )
      h->len = (size_t)-1;
    else
      h->len = buf.st_size - offset;
  }
  /* create mmap */
  h->addr = mmap( NULL, h->len, mmflags, MAP_SHARED, fd, offset );
  if( h->addr == MAP_FAILED ) {
    int saved_errno = errno;
    close( fd );
    return IPC_ERR( saved_errno );
  }
  close( fd ); /* we don't need it anymore! */
  return 0;
}


static int ipc_mmap_close( ipc_mmap_handle* h ) {
  int rv = munmap( h->addr, h->len );
  if( rv < 0 )
    return IPC_ERR( errno );
  return 0;
}


#if defined( _POSIX_SYNCHRONIZED_IO ) && _POSIX_SYNCHRONIZED_IO > 0
#  define IPC_MMAP_HAVE_FLUSH
static int ipc_mmap_flush( ipc_mmap_handle* h, size_t pos ) {
  int rv = msync( h->addr, pos, MS_ASYNC|MS_INVALIDATE );
  if( rv < 0 )
    return IPC_ERR( errno );
  return 0;
}
#endif

//#ifdef HAVE_MMAP

#define NAME "ipc.mmap"

typedef struct {
  ipc_mmap_handle h; /* platform specific data */
  /* extra management info: */
  char is_valid;
} sq_mmap_handle;


static SQRESULT mmap_pusherror( HSQUIRRELVM v, int code ) {
  char buf[ IPC_MAXERRMSG ];
  ipc_mmap_error( buf, sizeof( buf ), code );
  return sq_throwerror(v, _SC("%s"), buf);
}


static SQRESULT sq_mmap_close(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
	GET_file_INSTANCE();
    int rv = 0;
    if(!self->is_valid) return sq_throwerror(v, _SC("attempt to use invalid mmap object"));
    rv = ipc_mmap_close( &self->h );
    if( rv != 0 ) return mmap_pusherror(v, rv);
    self->is_valid = 0;
    sq_pushbool(v, SQTrue);
    return 1;
}


static SQRESULT sq_mmap_gc( HSQUIRRELVM v ) {
  l_mmap_handle* h = lua_touserdata( L, 1 );
  if( h->is_valid )
    ipc_mmap_close( &h->h );
  return 0;
}


#ifdef IPC_MMAP_HAVE_FLUSH
static SQRESULT sq_mmap_flush( HSQUIRRELVM v ) {
	SQ_FUNC_VARS(v);
	GET_file_INSTANCE();
	SQ_GET_INTEGER(v, 2, pos);
    int rv = 0;
    if( !self->is_valid ) return sq_throwerror(v, _SC("attempt to use invalid mmap object"));
    rv = ipc_mmap_flush( &self->h, pos );
    if( rv != 0 ) return mmap_pusherror(v, rv);
    sq_pushbool(v, SQTrue);
    return 1;
}
#endif

static SQRESULT getMmapMode(HSQUIRRELVM v, const char *mode)
{
    if(mode)
    {
        switch(mode[0])
        {
        case 'r':
            if(mode[1] == 'w') return MEMFILE_RW;
            return MEMFILE_R;
        break;

        case 'w':
            return MEMFILE_W;
            break;
        }
    }
    return sq_throwerror(v, _SC("invalid parameter mode '%s'"), mode);;
}


static SQRESULT sq_mmap_open(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
	GET_file_INSTANCE();

	SQ_GET_STRING(v, 2, name);
	SQ_GET_STRING(v, 3, mode);
	SQ_OPT_INTEGER(v, 4, offset, 0);
	SQ_OPT_INTEGER(v, 5, nbytes, 0);

    int mmode = getMmapMode(v, mode);
    if(mmode < 0) return mmode;
    ipc_mmap_off_t offset = offset;
    size_t size = nbytes;
    l_mmap_handle* h = lua_newuserdata( L, sizeof( *h ) );
    int rv = 0;
    h->is_valid = 0;
    luaL_getmetatable( L, NAME );
    lua_setmetatable( L, -2 );
    rv = ipc_mmap_open( &h->h, name, mode, offset, size );
    if( rv != 0 ) return mmap_pusherror( L, rv );
    h->is_valid = 1;
    sq_pushcfunction(v, sq_mmap_close );
#ifdef IPC_MMAP_HAVE_FLUSH
    sq_pushcfunction(v, l_mmap_flush );
    memfile_new( L, ipc_mmap_addr( &h->h ), ipc_mmap_size( &h->h ),
               mode, -3, -2, -1 );
#else
    memfile_new( L, ipc_mmap_addr( &h->h ), ipc_mmap_size( &h->h ),
               mode, -2, -1, 0 );
#endif
  return 1;
}

//#endif // HAVE_MMAP
#endif // 0

//file lock
static void ipc_flock_error( char* buf, size_t len, int code ) {
#ifdef _WIN32
    if( len > 0 ) {
        if( 0 == FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 code,
                                 0,
                                 buf,
                                 len,
                                 NULL ) ) {
              strncpy( buf, "unknown error", len-1 );
              buf[ len-1 ] = '\0';
        } else { /* Windows puts an extra newline in there! */
              size_t n = strlen( buf );
              while( n > 0 && isspace( (unsigned char)buf[ --n ] ) )
                buf[ n ] = '\0';
        }
    }
#else
    if( len > 0 && strerror_r( code, buf, len ) != (int)0 ) {
        strncpy( buf, "unknown error", len-1 );
        buf[ len-1 ] = '\0';
    }
#endif // _WIN32
}


static int ipc_flock_lock( FILE* f, int is_wlock, int* could_lock,
                           ipc_flock_off_t start,
                           ipc_flock_off_t len ) {
#ifdef _WIN32
    HANDLE fh = (HANDLE)_get_osfhandle( _fileno( f ) );
    DWORD flags = is_wlock ? LOCKFILE_EXCLUSIVE_LOCK : 0;
    DWORD lenlo = (DWORD)len, lenhi = (DWORD)(len >> 32);
    OVERLAPPED ov;
    if( fh == (HANDLE)INVALID_HANDLE_VALUE ) return IPC_ERR( ERROR_INVALID_HANDLE );
    if( could_lock != NULL ) flags |= LOCKFILE_FAIL_IMMEDIATELY;
    ov.Offset = (DWORD)start;
    ov.OffsetHigh = (DWORD)(start >> 32);
    ov.hEvent = NULL;
    if( len == 0 ) lenhi = lenlo = (DWORD)-1;
    if( !LockFileEx( fh, flags, 0, lenlo, lenhi, &ov ) ) {
        int code = GetLastError();
        if( could_lock != NULL && (code == ERROR_LOCK_VIOLATION || code == ERROR_IO_PENDING) )
        {
            *could_lock = 0;
            return 0;
        }
        return IPC_ERR( code );
    }
#else
    int rv = 0;
    int fd = fileno( f );
    int op = could_lock != NULL ? F_SETLK : F_SETLKW;
    struct flock fl;
    fl.l_type = is_wlock ? F_WRLCK : F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = len;
    IPC_EINTR( rv, fcntl( fd, op, &fl ) );
    if( rv < 0 ) {
        if( could_lock != NULL && (errno == EACCES || errno == EAGAIN) ) {
            *could_lock = 0;
            return 0;
        }
        return IPC_ERR( errno );
    }
#endif
    if( could_lock != NULL ) *could_lock = 1;
    return 0;
}


static int ipc_flock_unlock( FILE* f, ipc_flock_off_t start,
                             ipc_flock_off_t len ) {
#ifdef _WIN32
    HANDLE fh = (HANDLE)_get_osfhandle( _fileno( f ) );
    DWORD lenlo = (DWORD)len, lenhi = (DWORD)(len >> 32);
    DWORD offlo = (DWORD)start, offhi = (DWORD)(start >> 32);
    if( fh == (HANDLE)INVALID_HANDLE_VALUE ) return IPC_ERR( ERROR_INVALID_HANDLE );
    if( len == 0 ) lenhi = lenlo = (DWORD)-1;
    if( !UnlockFile( fh, offlo, offhi, lenlo, lenhi ) ) return IPC_ERR( GetLastError() );
#else
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = len;
    if( fcntl( fileno( f ), F_SETLK, &fl ) < 0 ) return IPC_ERR( errno );
#endif // _WIN32
    return 0;
}

/* maximum expected length of error messages */
#define IPC_MAXERRMSG 200

static SQRESULT flock_pusherror(HSQUIRRELVM v, int code) {
  char buf[ IPC_MAXERRMSG ];
  ipc_flock_error( buf, sizeof( buf ), code );
  return sq_throwerror(v, _SC("%s"), buf);
}

static void invalidate_input_buffer( FILE* f ) {
  /* Linux (and apparently many other implementations) discard
   * unread characters from the input buffer if fflush is called on
   * an input file, but this is not guaranteed by ISO C. */
  fflush( f );
  /* This should also invalidate the input buffer unless the
   * implementation checks for that specific case. */
  fseek( f, 0, SEEK_CUR );
  /* If both methods don't work, we are out of luck. But using
   * low-level file locking with buffered IO is a bad idea
   * anyway! */
}

SQ_OPT_STRING_STRLEN();

#define GET_file_INSTANCE() SQ_GET_INSTANCE(v, 1, SQFile, SQSTD_FILE_TYPE_TAG) \
	if(self == NULL) return sq_throwerror(v, _SC("file object already closed"));


static SQRESULT getFlockMode(HSQUIRRELVM v, const char *mode)
{
    if(mode)
    {
        switch(mode[0])
        {
        case 'r':
            if(mode[1] == 'w') return 1;
            return 0;
        break;

        case 'w':
            return 1;
            break;
        }
    }
    return sq_throwerror(v, _SC("invalid parameter mode '%s'"), mode);;
}

static SQRESULT sq_FileLock_lock0(HSQUIRRELVM v, int isTry){
	SQ_FUNC_VARS(v);
	GET_file_INSTANCE();

	SQ_GET_STRING(v, 2, mode);
	SQ_OPT_INTEGER(v, 3, offset, 0);
	SQ_OPT_INTEGER(v, 4, nbytes, 0);

    FILE *fp = (FILE*)self->GetHandle();
    int is_wlock = getFlockMode(v, mode);
    if(is_wlock < 0) return is_wlock;
    ipc_flock_off_t start = offset;
    ipc_flock_off_t len = nbytes;
    int could_lock = isTry ? 0 : 1;
    int rv = ipc_flock_lock(fp, (isTry ? is_wlock : 0), &could_lock, start, len);
    if( rv != 0 ) return flock_pusherror(v, rv);
    /* try to flush input buffer */
    if(could_lock) invalidate_input_buffer(fp);
    sq_pushbool(v, could_lock);
    return 1;
}

static SQRESULT sq_FileLock_lock(HSQUIRRELVM v){
    return sq_FileLock_lock0(v, 0);
}

static SQRESULT sq_FileLock_trylock(HSQUIRRELVM v){
    return sq_FileLock_lock0(v, 1);
}

static SQRESULT sq_FileLock_unlock(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_file_INSTANCE();

	SQ_OPT_INTEGER(v, 2, offset, 0);
	SQ_OPT_INTEGER(v, 3, nbytes, 0);

    FILE *fp = (FILE*)self->GetHandle();
    ipc_flock_off_t start = offset;
    ipc_flock_off_t len = nbytes;
    int rv = 0;
    fflush(fp); /* flush output buffer */
    rv = ipc_flock_unlock(fp, start, len);
    if( rv != 0 ) return flock_pusherror(v, rv);
    sq_pushbool(v, SQTrue);
    return 1;
}

#define _DECL_FILELOCK_FUNC(name,nparams,pmask) {_SC(#name),sq_FileLock_##name,nparams,pmask}
static SQRegFunction FileLock_obj_funcs[]={
	_DECL_FILELOCK_FUNC(lock, -2, _SC("xsii")),
	_DECL_FILELOCK_FUNC(trylock, -2, _SC("xsii")),
	_DECL_FILELOCK_FUNC(unlock, -1, _SC("x")),
	{0,0}
};
#undef _DECL_FILELOCK_FUNC

/* This defines a function that opens up your library. */
SQRESULT sqext_register_ipc (HSQUIRRELVM v) {
	//add a namespace FileLock
    sq_pushstring(v, SQSTD_FILE_CLASS_TYPE_TAG, -1);
    if(sq_getonregistrytable(v) == SQ_OK){
        sq_insert_reg_funcs(v, FileLock_obj_funcs);
        sq_poptop(v);
        return SQ_OK;
    }
    return sq_throwerror(v, _SC("file class not found"));
}

#ifdef __cplusplus
}

//#endif //USE_IPC

#endif

