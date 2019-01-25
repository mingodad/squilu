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

static char buff[2048];

#ifdef WIN32
#include <windows.h>
static HANDLE hPipeRead,hWriteSubProcess;

static SQRESULT sq_spawn_open(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, cmd);
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
	SECURITY_DESCRIPTOR sd;
	STARTUPINFO si = {
		sizeof(STARTUPINFO), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	HANDLE hRead2,hPipeWrite;
	BOOL running;
	PROCESS_INFORMATION pi;
	HANDLE hProcess = GetCurrentProcess();
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;

	// Create pipe for output redirection
	// read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
	CreatePipe(&hPipeRead, &hPipeWrite, &sa, 0);

	// Create pipe for input redirection. In this code, you do not
	// redirect the output of the child process, but you need a handle
	// to set the hStdInput field in the STARTUP_INFO struct. For safety,
	// you should not set the handles to an invalid handle.

	hRead2 = NULL;
	// read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
	CreatePipe(&hRead2, &hWriteSubProcess, &sa, 0);

	SetHandleInformation(hPipeRead, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(hWriteSubProcess, HANDLE_FLAG_INHERIT, 0);

	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hRead2;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;

	running = CreateProcess(
			  NULL,
			  (char*)cmd,
			  NULL, NULL,
			  TRUE, CREATE_NEW_PROCESS_GROUP,
			  NULL,
			  NULL, // start directory
			  &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(hRead2);
	CloseHandle(hPipeWrite);

	if (running) {
        lua_pushnumber(L,(int)hPipeRead);
	} else {
        lua_pushnil(L);
	}
	return 1;
}

static SQRESULT sq_spawn_reads(HSQUIRRELVM v)
{
    DWORD bytesRead;
    int res = ReadFile(hPipeRead,buff,sizeof(buff), &bytesRead, NULL);
    buff[bytesRead] = '\0';
    if (res == 0) {
        sq_pushnull(v);
    } else {
        sq_pushstring(L,buff, bytesRead);
    }
    return 1;
}

static SQRESULT sq_spawn_writes(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, str);
    DWORD bytesWrote;
    WriteFile(hWriteSubProcess,str,str_size,&bytesWrote, NULL);
    sq_pushinteger(v, bytesWrote);
    return 1;
}
#else

#ifdef __cplusplus
extern "C" {
#endif
#include <pty.h>
#include <unistd.h>
#include <termios.h>
#ifdef __cplusplus
}
#endif


static int spawn_fd;

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

static SQRESULT sq_spawn_open(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, cmd);
    const char *args[30];
    int pid, i = 0;
    struct termios tm;
    char* argline = strdup(cmd);
    char* arg = quote_strtok(argline,'"');
    if (arg == NULL) return 0;
    while (arg != NULL) {
        args[i++] = arg;
        //fprintf(stderr,"%d %s\n",i,arg);
        arg = quote_strtok(NULL,'"');
    }
    args[i] = NULL;
    memset(&tm,0,sizeof(tm));
    cfmakeraw(&tm);
    errno = 0;
    pid = forkpty(&spawn_fd,NULL,&tm,NULL);
    if (pid == 0) { // child
        execvp(args[0], (char*const*)args);
        // if we get here, it's an error!
        perror("'unable to spawn process");
    } else {
        sq_pushinteger(v,pid);
        //sq_pushstring(L,strerror(errno));
        return 1;
    }
    return 0;
}

static SQRESULT sq_spawn_reads(HSQUIRRELVM v)
{
    int sz = read(spawn_fd,buff,sizeof(buff));
    buff[sz] = '\0';
    if (errno != 0) {
        sq_pushinteger(v, errno);
        //sq_pushstring(v,strerror(errno));
    } else {
        sq_pushstring(v,buff, sz);
    }
    return 1;
}

static SQRESULT sq_spawn_writes(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, str);
    ssize_t n = write(spawn_fd,str,str_size);
    sq_pushinteger(v, n);
    return 1;
}
#endif

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name), sq_spawn_##name,nparams,tycheck}
static SQRegFunction sq_spawn_methods[] =
{
    _DECL_FUNC(open,2,_SC(".s")),
    _DECL_FUNC(reads,1,_SC(".")),
    _DECL_FUNC(writes,2,_SC(".s")),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_sq_spawnx(HSQUIRRELVM v)
{
    sq_pushliteral(v,_SC("sqspawnx"));
    sq_newtable(v);
    sq_insert_reg_funcs(v, sq_spawn_methods);

    sq_newslot(v,-3,SQTrue);
    return 1;
}
#ifdef __cplusplus
}
#endif

#endif // SQ_USE_SPAWNX
