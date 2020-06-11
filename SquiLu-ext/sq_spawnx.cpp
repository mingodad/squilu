//adapted from http://mysite.mweb.co.za/residents/sdonovan/lua/lua-gdb.zip
#ifdef SQ_USE_SPAWNX

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

#ifdef WIN32
    #include <windows.h>
#else
    #ifdef __cplusplus
    extern "C" {
    #endif
#ifdef __MACH__
    #include <util.h>
	int openpty(int *, int *, char *, struct termios *, struct winsize *);
	pid_t forkpty(int *, char *, struct termios *, struct winsize *);
#else
    #include <pty.h>
#endif
    #include <unistd.h>
    #include <termios.h>
    #ifdef __cplusplus
    }
    #endif
#endif

struct SQ_SpawnX {
#ifdef WIN32
    HANDLE hPipeRead,hWriteSubProcess;
    PROCESS_INFORMATION pi;
#else
    int spawn_fd;
    int pid;
    struct termios tm;
#endif // WIN32
};

#ifdef WIN32

static SQRESULT sq_spawnx_open(HSQUIRRELVM v, SQ_SpawnX *self, const char *cmd)
{
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
	SECURITY_DESCRIPTOR sd;
	STARTUPINFO si = {
		sizeof(STARTUPINFO), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	HANDLE hRead2,hPipeWrite;
	BOOL running;
	HANDLE hProcess = GetCurrentProcess();
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;

	// Create pipe for output redirection
	// read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
	CreatePipe(&self->hPipeRead, &hPipeWrite, &sa, 0);

	// Create pipe for input redirection. In this code, you do not
	// redirect the output of the child process, but you need a handle
	// to set the hStdInput field in the STARTUP_INFO struct. For safety,
	// you should not set the handles to an invalid handle.

	hRead2 = NULL;
	// read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
	CreatePipe(&hRead2, &self->hWriteSubProcess, &sa, 0);

	SetHandleInformation(self->hPipeRead, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(self->hWriteSubProcess, HANDLE_FLAG_INHERIT, 0);

	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hRead2;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;

	running = CreateProcess(
			  NULL,
			  (LPSTR)cmd,
			  NULL, NULL,
			  TRUE, CREATE_NEW_PROCESS_GROUP,
			  NULL,
			  NULL, // start directory
			  &si, &self->pi);

	CloseHandle(self->pi.hThread);
	CloseHandle(hRead2);
	CloseHandle(hPipeWrite);

	if (running) return SQ_OK;
	return sq_throwerror(v, _SC("unable to spawn process"));
}

#else

static char *quote_strtok(char *str, char str_delim)
{
// a specialized version of strtok() which treats quoted strings specially
// (used for handling command-line parms)
    static char *tok;
    if(str != NULL) tok = str;

    while (*tok && isspace(*tok)) tok++;
    if (*tok == '\0') return NULL;

    if (*tok == str_delim) {
       tok++;            // skip "
       str = tok;
       while (*tok && *tok != str_delim) tok++;
    } else {
       str = tok;
       while (*tok && ! isspace(*tok)) tok++;
    }
    if (*tok) *tok++ = '\0';
    return str;
}

static SQRESULT sq_spawnx_open(HSQUIRRELVM v, SQ_SpawnX *self, const char *cmd)
{
    const char *args[30];
    int i = 0;
    char* argline = strdup(cmd);
    char* arg = quote_strtok(argline,'"');
    if (arg == NULL) return 0;
    while (arg != NULL) {
        args[i++] = arg;
        //fprintf(stderr,"%d %s\n",i,arg);
        arg = quote_strtok(NULL,'"');
    }
    args[i] = NULL;
    memset(&self->tm,0,sizeof(self->tm));
    cfmakeraw(&self->tm);
    errno = 0;
    self->pid = forkpty(&self->spawn_fd,NULL,&self->tm,NULL);
    if (self->pid == 0) { // child
        execvp(args[0], (char*const*)args);
        // if we get here, it's an error!
        perror("unable to spawn process");
    } else {
        return SQ_OK;
    }
    return sq_throwerror(v, _SC("unable to spawn process"));
}
#endif

static const SQChar sq_spawnx_TAG[] = _SC("SQ_SpawnX");

static SQRESULT get_spawnx_instance(HSQUIRRELVM v, SQInteger idx, SQ_SpawnX **spawnx_st)
{
    if(sq_getinstanceup(v, idx, (SQUserPointer*)spawnx_st, (void*)sq_spawnx_TAG) != SQ_OK) return SQ_ERROR;
    if(!*spawnx_st) return sq_throwerror(v, _SC("%s"), _SC("spawnx already closed"));
    return SQ_OK;
}

#define GET_sq_spawnx_INSTANCE(v, idx) \
    SQ_SpawnX *self=NULL; \
    if(get_spawnx_instance(v, idx, &self) != SQ_OK) return SQ_ERROR;


static SQRESULT sq_spawnx_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	SQ_SpawnX *self = ((SQ_SpawnX *)p);
	if(self)
	{
		sq_free(self, sizeof(SQ_SpawnX));
	}
	return 1;
}

static SQRESULT sq_spawnx_constructor(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, cmd);

	SQ_SpawnX *self = (SQ_SpawnX*)sq_malloc(sizeof(SQ_SpawnX));

	_rc_ = sq_spawnx_open(v, self, cmd);

	sq_setinstanceup(v,1,self);
	sq_setreleasehook(v,1,sq_spawnx_releasehook);
	return 0;
}

static SQRESULT sq_spawnx_read(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_sq_spawnx_INSTANCE(v, 1);
#define DEFAULT_BUFFSIZE 2048
    SQ_OPT_INTEGER(v, 2, read_sz, DEFAULT_BUFFSIZE);
    //int isScratchString = (read_sz == DEFAULT_BUFFSIZE);
    //SQChar *buf =  isScratchString ? sq_getscratchstr(v, read_sz) : sq_getscratchpad(v, read_sz);
    SQChar *buf = sq_getscratchpad(v, read_sz);
#ifdef WIN32
    DWORD bytesRead;
    int res = ReadFile(self->hPipeRead,buf,read_sz, &bytesRead, NULL);
    if (res == 0)  {
	    //if(isScratchString) sq_delscratchstr(v, buf);
	    return sq_throwerror(v, _SC("error reading GetLastError() = %d"), GetLastError());
#else
    int bytesRead = read(self->spawn_fd, buf, read_sz);
    if(bytesRead < 0) {
	    //if(isScratchString) sq_delscratchstr(v, buf);
	    return sq_throwerror(v, _SC("error reading %d : %s"), errno, strerror(errno));
#endif
    } else {
	/*if(isScratchString) sq_pushscratchstr(v);
        else*/ sq_pushstring(v,buf, bytesRead);
    }
    return 1;
}

static SQRESULT sq_spawnx_write(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_sq_spawnx_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, str);
#ifdef WIN32
    DWORD bytesWrote;
    WriteFile(self->hWriteSubProcess,str,str_size,&bytesWrote, NULL);
    sq_pushinteger(v, bytesWrote);
#else
    ssize_t n = write(self->spawn_fd,str,str_size);
    //fdatasync(self->spawn_fd);
    sq_pushinteger(v, n);
#endif
    return 1;
}

static SQRESULT sq_spawnx_pid(HSQUIRRELVM v)
{
    GET_sq_spawnx_INSTANCE(v, 1);
#ifdef WIN32
    sq_pushinteger(v, (intptr_t)(self->hPipeRead));
#else
    sq_pushinteger(v, self->pid);
#endif
    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name), sq_spawnx_##name,nparams,tycheck}
static SQRegFunction sq_spawnx_methods[] =
{
    _DECL_FUNC(constructor,2,_SC(".s")),
    _DECL_FUNC(read,-1,_SC("xi")),
    _DECL_FUNC(write,2,_SC("xs")),
    _DECL_FUNC(pid,1,_SC("x")),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_sq_spawnx(HSQUIRRELVM v)
{
    sq_pushstring(v, sq_spawnx_TAG, -1);
    sq_newclass(v, SQFalse);
    sq_settypetag(v,-1,(void*)sq_spawnx_TAG);
    sq_insert_reg_funcs(v, sq_spawnx_methods);
    sq_newslot(v,-3,SQTrue);
    return 1;
}
#ifdef __cplusplus
}
#endif

#endif // SQ_USE_SPAWNX
