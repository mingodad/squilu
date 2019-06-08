/* Copyright (c) 2010 Joshua Phillips
 * Ported on 2016 to SquiLu by Domingo Alvarez Duarte
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifdef OS_POSIX
#define _POSIX_SOURCE
#endif

#if !defined(OS_WINDOWS) && !defined(OS_POSIX)
#error None of these are defined: OS_WINDOWS, OS_POSIX
#else

#include "squirrel.h"
#include "sqstdblobimpl.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
SQ_OPT_STRING_STRLEN();

#if defined(OS_POSIX)
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
typedef int filedes_t;

/* return 1 if the named directory exists and is a directory */
static int direxists(const char *fname)
{
    struct stat statbuf;
    if (stat(fname, &statbuf)){
        return 0;
    }
    return !!S_ISDIR(statbuf.st_mode);
}

#elif defined(OS_WINDOWS)
#include "windows.h"

/* Some SDKs don't define this */
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD) -1)
#endif

typedef HANDLE filedes_t;

/* return 1 if the named directory exists and is a directory */
static int direxists(const char *fname)
{
    DWORD result;
    result = GetFileAttributes(fname);
    if (result == INVALID_FILE_ATTRIBUTES) return 0;
    return !!(result & FILE_ATTRIBUTE_DIRECTORY);
}

#endif /* defined(OS_WINDOWS) */

/* Some systems don't define these, but we use them as indices for our arrays.
   I probably oughtn't, in case a system doesn't use 0, 1 and 2 for these. */
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif

/* This is the proc object, which is stored as Lua userdata */
struct proc {
#if defined(OS_POSIX)
    pid_t pid;
#elif defined(OS_WINDOWS)
    DWORD pid;
    HANDLE hProcess;
#endif
    int exitcode;
    unsigned char done; /* set to 1 when child has finished and closed */
};

/* Lua registry key for proc metatable */
#define SP_PROC_META "subprocess_proc*"

/* Environment keys */
/* This is an integer index into the environment of C functions in this module.
   At this index is stored a table of [pid]=proc items. The items in this table
   will all have their `done` fields set to false. This table is at present only
   used for the `subprocess.wait` function.
   On POSIX, it is used to get the proc object corresponding to a pid. On
   Windows, it is used to assemble a HANDLE array for WaitForMultipleObjects. */
#define SP_LIST "subprocess_pid_list"

/* Check to see if object at the given index is a proc object.
   Return pointer to proc object, or NULL if it isn't. */
static struct proc *toproc(HSQUIRRELVM v, int index)
{
    int eq;
    if (lua_type(L, index) != LUA_TUSERDATA) return NULL;
    lua_getmetatable(L, index);
    luaL_getmetatable(L, SP_PROC_META);
    eq = lua_equal(L, -2, -1);
    lua_pop(L, 2);
    if (!eq) return NULL;
    return lua_touserdata(L, index);
}

/* Same but raise an error instead of returning NULL */
#define checkproc(L, index) ((struct proc *) luaL_checkudata((L), (index), SP_PROC_META))

/* Create and return a new proc object */
static struct proc *newproc(HSQUIRRELVM v)
{
    struct proc *proc = sq_newuserdata(v, sizeof *proc);
    proc->done = 1;
    proc->pid = 0;
    luaL_getmetatable(L, SP_PROC_META);
    lua_setmetatable(L, -2);
    lua_newtable(L);
    lua_setfenv(L, -2);
    return proc;
}

/* Mark a process (at index) as done */
static void doneproc(HSQUIRRELVM v, int index)
{
    struct proc *proc = toproc(L, index);
    if (!proc){
        fputs("subprocess.c: doneproc: not a proc\n", stderr);
    } else {
        proc->done = 1;
        /* remove proc from SP_LIST */
        sq_reservestack(v, 4);
        sq_pushvalue(v, index);    /* stack: proc */
        luaL_getmetatable(L, SP_LIST);
        /* stack: proc list */
        if (lua_isnil(L, -1)){
            fputs("subprocess.c: XXX: SP_LIST IS NIL\n", stderr);
        } else {
            lua_pushinteger(L, proc->pid);      /* stack: proc list pid */
            lua_pushvalue(L, -1);               /* stack: proc list pid pid */
            lua_gettable(L, -3);                /* stack: proc list pid proc2 */
            if (!lua_equal(L, -4, -1)){
                /* lookup by pid didn't work */
                fputs("subprocess.c: doneproc: XXX: pid lookup in SP_LIST failed\n", stderr);
                lua_pop(L, 2);                  /* stack: proc list */
            } else {
                lua_pop(L, 1);                  /* stack: proc list pid */
                lua_pushnil(L);                 /* stack: proc list pid nil */
                lua_settable(L, -3);            /* stack: proc list */
            }
            /* stack: proc list */
        }
        sq_pop(v, 2);
    }
}

/* Remove old SP_LIST entries by polling them.
   Calling this every now and again can avoid leaking proc objects
   that are not waited for. */
static SQRESULT sq_subprocess_prune(HSQUIRRELVM v)
{
    int top = lua_gettop(L);
    sq_reservestack(v, 5);
    luaL_getmetatable(L, SP_LIST);
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        return 0;
    }
    lua_pushnil(L);
    while (lua_next(L, -2)){
        lua_getfield(L, -1, "poll");
        lua_pushvalue(L, -2);
        lua_call(L, 1, 0);
        lua_pop(L, 1);
    }
    lua_settop(L, top);
    return 0;
}

/* Special constants for popen arguments. */
static char PIPE, STDOUT;

/* Names of standard file handles. */
static const char *fd_names[3] = {"stdin", "stdout", "stderr"};

/* Information about what to do for a standard file handle.
   This is constructed from popen arguments. */
struct fdinfo {
    enum {
        FDMODE_INHERIT = 0,  /* fd is inherited from parent */
        FDMODE_FILENAME,     /* open named file */
        FDMODE_FILEDES,      /* use a file descriptor */
        FDMODE_FILEOBJ,      /* use FILE* */
        FDMODE_PIPE,         /* create and use pipe */
        FDMODE_STDOUT        /* redirect to stdout (only for stderr) */
    } mode;
    union {
        const char *filename;
        filedes_t filedes;
        FILE *fileobj;
    } info;
};

/* Close multiple file descriptors */
static void closefds(filedes_t *fds, int n)
{
    int i;
    for (i=0; i<n; ++i){
#if defined(OS_POSIX)
        if (fds[i] != -1)
            close(fds[i]);
#elif defined(OS_WINDOWS)
        if (fds[i] != INVALID_HANDLE_VALUE)
            CloseHandle(fds[i]);
#endif
    }
}

/* Close multiple C files */
static void closefiles(FILE **files, int n)
{
    int i;
    for (i=0; i<n; ++i)
        if (files[i] != NULL)
            fclose(files[i]);
}

/* Free multiple strings */
static void freestrings(char **strs, int n)
{
    int i;
    for (i=0; i<n; ++i)
        if (strs[i] != NULL)
            sq_free(strs[i], -1);
}

#ifdef OS_WINDOWS
/* Copy a Windows error into a buffer */
static void copy_w32error(char errmsg_out[], size_t errmsg_len, DWORD error)
{
    if (FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0,
        (void *) errmsg_out, errmsg_len, NULL) == 0)
    {
        strncpy(errmsg_out, "failed to get error message", errmsg_len + 1);
    }
}

/* Push a Windows error onto a Lua stack */
static void push_w32error(HSQUIRRELVM v, DWORD error)
{
    LPTSTR buf;
    if (FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, error, 0, (void *) &buf, 1, NULL) == 0)
    {
        lua_pushliteral(L, "failed to get error message");
    } else {
        lua_pushstring(L, buf);
        LocalFree(buf);
    }
}

/* n is 0, 1 or 2
   return handle for standard input/output/error */
static HANDLE getstdhandle(int n)
{
    DWORD n2;
    switch (n){
        case 0: n2 = STD_INPUT_HANDLE; break;
        case 1: n2 = STD_OUTPUT_HANDLE; break;
        case 2: n2 = STD_ERROR_HANDLE; break;
        default: return INVALID_HANDLE_VALUE;
    }
    return GetStdHandle(n2);
}

struct str {
    char *data;
    size_t len;
    size_t size; /* size allocated */
};

static void str_init(struct str *s)
{
    s->data = NULL;
    s->len = 0;
    s->size = 0;
}

/* Append n chars from s2 */
static int str_appendlstr(struct str *s, char *s2, size_t n)
{
    void *newp;
    if (s->size < s->len + n){
        if (s->size < 16) s->size = 16;
        while (s->size < s->len + n)
            s->size = (s->size * 3) / 2;
        newp = realloc(s->data, s->size + 1);
        if (newp == NULL){
            free(s->data);
            return 0;
        }
        s->data = newp;
    }
    memcpy(s->data + s->len, s2, n);
    s->len += n;
    s->data[s->len] = '\0';
    return 1;
}

static int str_appendc(struct str *s, char ch)
{
    return str_appendlstr(s, &ch, 1);
}

/* Compiles command line for CreateProcess. Returns malloc'd string. */
static char *compile_cmdline(const char *const *args)
{
    /*  "      --> \"
        \"     --> \\\"
        \<NUL> --> \\    */
    struct str str;
    const char *arg;
    str_init(&str);
    while (*args != NULL){
        arg = *args++;
        if (!str_appendc(&str, '"')) return NULL;
        while (arg[0]){
            if (arg[0] == '"'){
                if (!str_appendlstr(&str, "\\\"", 2)) return NULL;
            } else if (arg[0] == '\\'){
                if (arg[1] == '"' || arg[1] == '\0'){
                    if (!str_appendlstr(&str, "\\\\", 2)) return NULL;
                } else {
                    if (!str_appendc(&str, '\\')) return NULL;
                }
            } else {
                if (!str_appendc(&str, arg[0])) return NULL;
            }
            arg++;
        }
        if (!str_appendlstr(&str, "\" ", 2)) return NULL;
    }
    str.data[str.len - 1] = '\0';
    return str.data;
}
#endif

/* Function for opening subprocesses. Returns 0 on success and -1 on failure.
   On failure, errmsg_out shall contain a '\0'-terminated error message. */
static int dopopen(const char *const *args,  /* program arguments with NULL sentinel */
                   const char *executable,   /* actual executable */
                   struct fdinfo fdinfo[3],  /* info for stdin/stdout/stderr */
                   int close_fds,            /* 1 to close all fds */
                   int binary,               /* 1 to use binary files */
                   const char *cwd,          /* working directory for program */
                   struct proc *proc,        /* populated on success! */
                   FILE *pipe_ends_out[3],   /* pipe ends are put here */
                   char errmsg_out[],        /* written to on failure */
                   size_t errmsg_len         /* length of errmsg_out (EXCLUDING sentinel) */
                  )
#if defined(OS_POSIX)
{
    int fds[3];
    int i;
    struct fdinfo *fdi;
    int piperw[2];
    int errpipe[2]; /* pipe for returning error status */
    int flags;
    int en; /* saved errno */
    int count;
    pid_t pid;

    errmsg_out[errmsg_len] = '\0';

    for (i=0; i<3; ++i)
        pipe_ends_out[i] = NULL;

    /* Manage stdin/stdout/stderr */
    for (i=0; i<3; ++i){
        fdi = &fdinfo[i];
        switch (fdi->mode){
            case FDMODE_INHERIT:
inherit:
                fds[i] = dup(i);
                if (fds[i] == -1){
fd_failure:
                    strncpy(errmsg_out, strerror(errno), errmsg_len + 1);
                    closefds(fds, i);
                    closefiles(pipe_ends_out, i);
                    return -1;
                }
                break;
            case FDMODE_FILENAME:
                if (i == STDIN_FILENO){
                    if ((fds[i] = open(fdi->info.filename, O_RDONLY)) == -1) goto fd_failure;
                } else {
                    if ((fds[i] = creat(fdi->info.filename, 0666)) == -1) goto fd_failure;
                }
                break;
            case FDMODE_FILEDES:
                if ((fds[i] = dup(fdi->info.filedes)) == -1) goto fd_failure;
                break;
            case FDMODE_FILEOBJ:
                if ((fds[i] = dup(fileno(fdi->info.fileobj))) == -1) goto fd_failure;
                break;
            case FDMODE_PIPE:
                if (pipe(piperw) == -1) goto fd_failure;
                if (i == STDIN_FILENO){
                    fds[i] = piperw[0]; /* give read end to process */
                    if ((pipe_ends_out[i] = fdopen(piperw[1], "w")) == NULL) goto fd_failure;
                } else {
                    fds[i] = piperw[1]; /* give write end to process */
                    if ((pipe_ends_out[i] = fdopen(piperw[0], "r")) == NULL) goto fd_failure;
                }
                break;
            case FDMODE_STDOUT:
                if (i == STDERR_FILENO){
                    if ((fds[STDERR_FILENO] = dup(fds[STDOUT_FILENO])) == -1) goto fd_failure;
                } else goto inherit;
                break;
        }
    }

    /* Find executable name */
    if (!executable){
        /* use first arg */
        executable = args[0];
    }
    assert(executable != NULL);

    /* Create a pipe for returning error status */
    if (pipe(errpipe) == -1){
        strncpy(errmsg_out, strerror(errno), errmsg_len + 1);
        closefds(fds, 3);
        closefiles(pipe_ends_out, 3);
        return -1;
    }
    /* Make write end close on exec */
    flags = fcntl(errpipe[1], F_GETFD);
    if (flags == -1){
pipe_failure:
        strncpy(errmsg_out, strerror(errno), errmsg_len + 1);
        closefds(errpipe, 2);
        closefds(fds, 3);
        closefiles(pipe_ends_out, 3);
        return -1;
    }
    if (fcntl(errpipe[1], F_SETFD, flags | FD_CLOEXEC) == -1) goto pipe_failure;

    /* Do the fork/exec (TODO: use vfork somehow?) */
    pid = fork();
    if (pid == -1) goto pipe_failure;
    else if (pid == 0){
        /* child */
        close(errpipe[0]);

        /* dup file descriptors */
        for (i=0; i<3; ++i){
            if (dup2(fds[i], i) == -1) goto child_failure;
        }

        /* close other fds */
        if (close_fds){
            for (i=3; i<sysconf(_SC_OPEN_MAX); ++i){
                if (i != errpipe[1])
                    close(i);
            }
        }

        /* change directory */
        if (cwd && chdir(cwd)) goto child_failure;

        /* exec! Farewell, subprocess.c! */
        execvp(executable, (char *const*) args); /* XXX: const cast */

        /* Oh dear, we're still here. */
child_failure:
        en = errno;
        write(errpipe[1], &en, sizeof en);
        _exit(1);
    }

    /* parent */
    /* close unneeded fds */
    closefds(fds, 3);
    close(errpipe[1]);

    /* read errno from child */
    while ((count = read(errpipe[0], &en, sizeof en)) == -1)
        if (errno != EAGAIN && errno != EINTR) break;
    if (count > 0){
        /* exec failed */
        close(errpipe[0]);
        strncpy(errmsg_out, strerror(en), errmsg_len + 1);
        return -1;
    }
    close(errpipe[0]);

    /* Child is now running */
    proc->done = 0;
    proc->pid = pid;
    return 0;
}
#elif defined(OS_WINDOWS)
{
    HANDLE hfiles[3], piper, pipew, hfile;
    int i, fd;
    struct fdinfo *fdi;
    SECURITY_ATTRIBUTES secattr;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char *cmdline;

    errmsg_out[errmsg_len] = '\0';

    /* Create a SECURITY_ATTRIBUTES for inheritable handles */
    secattr.nLength = sizeof secattr;
    secattr.lpSecurityDescriptor = NULL;
    secattr.bInheritHandle = TRUE;

    for (i=0; i<3; ++i)
        pipe_ends_out[i] = NULL;

    /* Manage stdin/stdout/stderr */
    for (i=0; i<3; ++i){
        fdi = &fdinfo[i];
        switch (fdi->mode){
            case FDMODE_INHERIT:
inherit:
                /* XXX: duplicated file handles share the
                   same object (and thus file cursor, etc.).
                   CreateFile might be a better idea. */
                hfile = getstdhandle(i);
                if (hfile == INVALID_HANDLE_VALUE){
fd_failure:
                    copy_w32error(errmsg_out, errmsg_len, GetLastError());
                    closefds(hfiles, i);
                    closefiles(pipe_ends_out, i);
                    return -1;
                }
dup_hfile:
                if (DuplicateHandle(GetCurrentProcess(), hfile,
                    GetCurrentProcess(), &hfiles[i], 0, TRUE,
                    DUPLICATE_SAME_ACCESS) == 0)
                {
                    goto fd_failure;
                }
                break;
            case FDMODE_FILENAME:
                if (i == STDIN_FILENO){
                    hfiles[i] = CreateFile(
                        fdi->info.filename,
                        GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        &secattr,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
                } else {
                    hfiles[i] = CreateFile(
                        fdi->info.filename,
                        GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        &secattr,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
                }
                if (hfiles[i] == INVALID_HANDLE_VALUE){
                    goto fd_failure;
                }
                break;
            case FDMODE_FILEDES:
                if (DuplicateHandle(GetCurrentProcess(), fdi->info.filedes,
                    GetCurrentProcess(), &hfiles[i], 0, TRUE,
                    DUPLICATE_SAME_ACCESS) == 0)
                {
                    goto fd_failure;
                }
                break;
            case FDMODE_FILEOBJ:
                fd = _fileno(fdi->info.fileobj);
                if (fd == -1){
get_osf_failure:
                    strncpy(errmsg_out, strerror(errno), errmsg_len + 1);
failure:
                    closefds(hfiles, i);
                    closefiles(pipe_ends_out, i);
                    return -1;
                }
                hfile = (HANDLE) _get_osfhandle(fd);
                if (hfile == INVALID_HANDLE_VALUE) goto get_osf_failure;
                goto dup_hfile;
            case FDMODE_PIPE:
                if (CreatePipe(&piper, &pipew, &secattr, 0) == 0)
                    goto fd_failure;
                if (i == STDIN_FILENO){
                    hfiles[i] = piper;
                    fd = _open_osfhandle((long) pipew, binary ? 0 : _O_TEXT);
                    if (fd == -1){
                        strncpy(errmsg_out, "_open_osfhandle failed", errmsg_len + 1);
                        goto failure;
                    }
                    pipe_ends_out[i] = _fdopen(fd, "w");
                    if (pipe_ends_out[i] == 0){
                        strncpy(errmsg_out, "_fdopen failed", errmsg_len + 1);
                        goto failure;
                    }
                } else {
                    hfiles[i] = pipew;
                    fd = _open_osfhandle((long) piper, _O_RDONLY | (binary ? 0 : _O_TEXT));
                    if (fd == -1){
                        strncpy(errmsg_out, "_open_osfhandle failed", errmsg_len + 1);
                        goto failure;
                    }
                    pipe_ends_out[i] = _fdopen(fd, "r");
                    if (pipe_ends_out[i] == 0){
                        strncpy(errmsg_out, "_fdopen failed", errmsg_len + 1);
                        goto failure;
                    }
                }
                break;
            case FDMODE_STDOUT:
                if (i == STDERR_FILENO){
                    hfile = hfiles[STDOUT_FILENO];
                    goto dup_hfile;
                } else goto inherit;
        }
    }

    /* Find executable name */
    if (executable == NULL){
        /* use first arg */
        /*executable = args[0];*/
    }

    /* Compile command line into string. Yuck. */
    cmdline = compile_cmdline(args);
    if (!cmdline){
        strncpy(errmsg_out, "memory full", errmsg_len + 1);
        closefds(hfiles, 3);
        closefiles(pipe_ends_out, 3);
        return -1;
    }


    si.cb = sizeof si;
    si.lpReserved = NULL;
    si.lpDesktop = NULL;
    si.lpTitle = NULL;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;
    si.hStdInput = hfiles[0];
    si.hStdOutput = hfiles[1];
    si.hStdError = hfiles[2];

    if (CreateProcess(
        executable, /* lpApplicationName */
        cmdline,    /* lpCommandLine */
        NULL,       /* lpProcessAttributes */
        NULL,       /* lpThreadAttributes */
        TRUE,       /* bInheritHandles */
        0,          /* dwCreationFlags */
        NULL,       /* lpEnvironment */
        cwd,        /* lpCurrentDirectory */
        &si,        /* lpStartupInfo */
        &pi)        /* lpProcessInformation */
    == 0){
        copy_w32error(errmsg_out, errmsg_len, GetLastError());
        free(cmdline);
        closefds(hfiles, 3);
        closefiles(pipe_ends_out, 3);
        return -1;
    }
    CloseHandle(pi.hThread); /* Don't want this handle */
    free(cmdline);
    closefds(hfiles, 3); /* XXX: is this correct? */
    proc->done = 0;
    proc->pid = pi.dwProcessId;
    proc->hProcess = pi.hProcess;
    return 0;
}
#endif

/* popen {arg0, arg1, arg2, ..., [executable=...]} */
static SQRESULT sq_subprocess_popen(HSQUIRRELVM v)
{
    struct proc *proc = NULL;

    /* List of arguments (malloc'd NULL-terminated array of C strings.
       The C strings are owned by Lua) */
    int nargs = 0;
    const char **args = NULL;
    /* Command to run (owned by Lua) */
    const char *executable = NULL;
    /* Directory to run it in (owned by Lua) */
    const char *cwd = NULL;
    /* File options */
    struct fdinfo fdinfo[3];
    /* Close fds? */
    int close_fds = 0;
    /* Use binary mode for files? */
    int binary = 0;

    FILE *pipe_ends[3] = {NULL, NULL, NULL};
    int i, result;
    FILE *f;
    const char *s;

    char errmsg_buf[256];

    prune(v);

    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1);

    proc = newproc(v);

    /* Stack: kwargs proc <strings etc....>
       Lua strings are left on the stack while they are needed,
       and Lua can garbage-collect them later. */

    /* get arguments */
    nargs = lua_objlen(L, 1);
    if (nargs == 0) return luaL_error(L, "no arguments specified");
    args = lua_newuserdata(L, (nargs + 1) * sizeof *args); /*alloc((nargs + 1) * sizeof *args);*/
    if (!args) return luaL_error(L, "memory full");
    for (i=0; i<=nargs; ++i) args[i] = NULL;
    luaL_checkstack(L, nargs, "cannot grow stack");
    for (i=1; i<=nargs; ++i){
        lua_rawgeti(L, 1, i);
        s = lua_tostring(L, -1);
        if (!s){
            /*freestrings(args, nargs);
            free(args);*/
            return luaL_error(L, "popen argument %d not a string", (int) i);

        }
        args[i-1] = s; /*strdup(s);*/
        /*if (args[i-1] == NULL){
strings_failure:
            freestrings(args, nargs);
            free(args);
            return luaL_error(L, "memory full");
        } */
        /*lua_pop(L, 1);*/
    }

    luaL_checkstack(L, 12, "cannot grow stack");

    /* get executable string */
    lua_getfield(L, 1, "executable");
    s = lua_tostring(L, -1);
    if (s){
        executable = s; /*strdup(s);*/
        /*if (executable == NULL) goto strings_failure;*/
    } else lua_pop(L, 1);
    /*lua_pop(L, 1); */ /* to match lua_getfield */

    /* get directory name */
    lua_getfield(L, 1, "cwd");
    cwd = lua_tostring(L, -1);
    if (cwd == NULL) lua_pop(L, 1);
    else {
    /*if (lua_isstring(L, -1)){
        cwd = lua_tostring(L, -1);*/ /*strdup(lua_tostring(L, -1));
        if (!cwd){
            free(executable);
            freestrings(args, nargs);
            free(args);
            return luaL_error(L, "memory full");
        }                            */
        /* make sure the cwd exists */
        if (!direxists(cwd)){
            /*free(executable);
            freestrings(args, nargs);*/
            /*free(args);*/
            return luaL_error(L, "directory `%s' does not exist", cwd);
        }
    }
    /*lua_pop(L, 1);*/

    /* close_fds */
    lua_getfield(L, 1, "close_fds");
    close_fds = lua_toboolean(L, -1);
    lua_pop(L, 1);

    /* binary */
    lua_getfield(L, 1, "binary");
    binary = lua_toboolean(L, -1);
    lua_pop(L, 1);

    /* handle stdin/stdout/stderr */
    for (i=0; i<3; ++i){
        lua_getfield(L, 1, fd_names[i]);
        if (lua_isnil(L, -1)){
            fdinfo[i].mode = FDMODE_INHERIT;
            lua_pop(L, 1);
        } else if (lua_touserdata(L, -1) == &PIPE){
            fdinfo[i].mode = FDMODE_PIPE;
            lua_pop(L, 1);
        } else if (lua_touserdata(L, -1) == &STDOUT){
            if (i == STDERR_FILENO /*&& fdinfo[STDOUT_FILENO].mode == FDMODE_PIPE*/){
                fdinfo[i].mode = FDMODE_STDOUT;
            } else {
                lua_pushliteral(L, "STDOUT must be used only for stderr"/* when stdout is set to PIPE"*/);
files_failure:
                /*for (j=0; j<i; ++j){
                    if (fdinfo[j].mode == FDMODE_FILENAME)
                        free(fdinfo[j].info.filename);
                }
                free(executable);
                freestrings(args, nargs);
                free(args);*/
                return lua_error(L);
            }
            lua_pop(L, 1);
        } else if (lua_isstring(L, -1)){
            /* open a file */
            fdinfo[i].mode = FDMODE_FILENAME;
            /*if ((fdinfo[i].info.filename = strdup(lua_tostring(L, -1))) == NULL){
                lua_pushliteral(L, "out of memory");
                goto files_failure;
            } */
            fdinfo[i].info.filename = lua_tostring(L, -1);
            /* do not pop */
        } else if (lua_isnumber(L, -1)){
            /* use this fd */
            fdinfo[i].mode = FDMODE_FILEDES;
            fdinfo[i].info.filedes = (filedes_t) lua_tointeger(L, -1);
            lua_pop(L, 1);
        } else {
            f = liolib_copy_tofile(L, -1);
            if (f){
                fdinfo[i].mode = FDMODE_FILEOBJ;
                fdinfo[i].info.fileobj = f;
            } else {
                /* huh? */
                lua_pushfstring(L, "unexpected value for %s", fd_names[i]);
                goto files_failure;
            }
            lua_pop(L, 1);
        }
    }

    result = dopopen(args, executable, fdinfo, close_fds, binary, cwd, proc, pipe_ends, errmsg_buf, 255);
    /*for (i=0; i<3; ++i)
        if (fdinfo[i].mode == FDMODE_FILENAME)
            free(fdinfo[i].info.filename);
    free(executable);
    freestrings(args, nargs);
    free(args);*/
    if (result == -1){
        /* failed */
        return luaL_error(L, "popen failed: %s", errmsg_buf);
    }

    /* Put pipe objects in proc userdata's environment */
    lua_getfenv(L, 2);
    for (i=0; i<3; ++i){
        if (pipe_ends[i]){
            *liolib_copy_newfile(L) = pipe_ends[i];
            lua_setfield(L, -2, fd_names[i]);
        }
    }
    lua_pop(L, 1);

    /* Put proc object in SP_LIST table */
    luaL_getmetatable(L, SP_LIST);
    if (lua_isnil(L, -1)){
        fputs("subprocess.c: XXX: SP_LIST IS NIL\n", stderr);
    } else {
        lua_pushinteger(L, proc->pid); /* stack: list pid */
        lua_pushvalue(L, 2);           /* stack: list pid proc */
        lua_settable(L, -3);           /* stack: list */
    }
    lua_pop(L, 1);

    /* Return the proc */
    lua_settop(L, 2);
    return 1;
}

/* __gc */
static int proc_gc(HSQUIRRELVM v)
{
    struct proc *proc = checkproc(v, 1);
    if (!proc->done){
#if defined(OS_POSIX)
        /* Try to wait for process to avoid leaving zombie.
           If the process hasn't finished yet, we'll end up leaving a zombie. */
        int stat;
        waitpid(proc->pid, &stat, WNOHANG);
#elif defined(OS_WINDOWS)
        CloseHandle(proc->hProcess);
#endif
        doneproc(v, 1);
    }
    return 0;
}

/* __index */
static int proc_index(HSQUIRRELVM v)
{
    struct proc *proc;
    const char *s;
    lua_settop(L, 2);
    proc = checkproc(v, 1);
    /* first check environment table */
    lua_getfenv(L, 1);
    lua_pushvalue(L, 2);
    lua_gettable(L, 3);
    if (!lua_isnil(L, 4)) return 1;
    lua_pop(L, 2);
    /* next check metatable */
    lua_getmetatable(L, 1);
    lua_pushvalue(L, 2);
    lua_gettable(L, 3);
    if (!lua_isnil(L, 4)) return 1;
    lua_pop(L, 2);
    /* lastly, fixed fields */
    s = lua_tostring(L, 2);
    if (!strcmp(s, "pid")){
        lua_pushinteger(L, proc->pid);
        return 1;
    } else if (!strcmp(s, "exitcode") && proc->done){
        lua_pushinteger(L, proc->exitcode);
        return 1;
    } else {
        return 0;
    }
}

/* Push string representation of process on stack */
static int proc_tostring(HSQUIRRELVM v)
{
    struct proc *proc = checkproc(L, 1);
    if (proc->done)
        lua_pushliteral(L, "(finished process)");
    else
        lua_pushfstring(L, "process (%d)", (int) proc->pid);
    return 1;
}

#if defined(OS_POSIX)
/* Get exitcode from wait's 'stat' value */
static int getexitcode(int stat)
{
    if (WIFEXITED(stat))
        return WEXITSTATUS(stat);
    else if (WIFSIGNALED(stat))
        return -WTERMSIG(stat);
    else if (WIFSTOPPED(stat))
        return -WSTOPSIG(stat);
    else {
        fputs("child disappeared into black hole\n", stderr);
        return -1;
    }
}
#endif

/* Wait for, or poll, a process */
static int do_waitpid(HSQUIRRELVM v, struct proc *proc, int wait)
#if defined(OS_POSIX)
{
    int stat, options;

    if (proc->done){
        lua_pushinteger(L, proc->exitcode);
        return 1;
    }

    if (wait) options = 0;
    else options = WNOHANG;
    switch (waitpid(proc->pid, &stat, options)){
        case -1:
            return luaL_error(L, strerror(errno));
        case 0:
            /* child still running */
            lua_pushnil(L);
            return 1;
        default:
            proc->exitcode = getexitcode(stat);
            doneproc(L, 1);
            lua_pushinteger(L, proc->exitcode);
            return 1;
    }
}
#elif defined(OS_WINDOWS)
{
    DWORD dwMilliseconds, retval, exitcode;

    if (proc->done){
        lua_pushinteger(L, proc->exitcode);
        return 1;
    }
    if (wait) dwMilliseconds = INFINITE;
    else dwMilliseconds = 0;
    retval = WaitForSingleObject(proc->hProcess, dwMilliseconds);
    switch (retval){
        case WAIT_FAILED:
failure:
            push_w32error(L, GetLastError());
            return lua_error(L);
        case WAIT_OBJECT_0:
            /* child finished */
            if (GetExitCodeProcess(proc->hProcess, &exitcode) == 0){
                goto failure;
            }
            CloseHandle(proc->hProcess);
            proc->exitcode = exitcode;
            doneproc(L, 1);
            lua_pushinteger(L, proc->exitcode);
            return 1;
        case WAIT_TIMEOUT:
        default:
            /* child still running */
            lua_pushnil(L);
            return 1;
    }
}
#endif

static int SQRESULT sq_process_poll(HSQUIRRELVM v)
{
    return do_waitpid(L, checkproc(L, 1), 0);
}

static int SQRESULT sq_process_wait(HSQUIRRELVM v)
{
    return do_waitpid(L, checkproc(L, 1), 1);
}

#if defined(OS_POSIX)
static SQRESULT sq_process_send_signal(HSQUIRRELVM v)
{
    struct proc *proc = checkproc(L, 1);
    int sig = luaL_checkinteger(L, 2);
    if (!proc->done){
        if (kill(proc->pid, sig)){
            return luaL_error(L, "kill: %s", strerror(errno));
        }
        proc->exitcode = -sig;
        doneproc(L, 1);
    }
    return 0;
}

static SQRESULT sq_process_terminate(HSQUIRRELVM v)
{
    lua_settop(L, 1);
    lua_pushinteger(L, SIGTERM);
    return proc_send_signal(L);
}

static SQRESULT sq_process_kill(HSQUIRRELVM v)
{
    lua_settop(L, 1);
    lua_pushinteger(L, SIGKILL);
    return proc_send_signal(L);
}
#elif defined(OS_WINDOWS)
static SQRESULT sq_process_terminate(HSQUIRRELVM v)
{
    struct proc *proc = checkproc(L, 1);
    if (!proc->done){
        if (TerminateProcess(proc->hProcess, -9) == 0){
            push_w32error(L, GetLastError());
            return lua_error(L);
        }
        CloseHandle(proc->hProcess);
        proc->exitcode = -9;
        doneproc(L, 1);
    }
    return 0;
}
#endif

static const luaL_Reg proc_meta[] = {
    {"__tostring", proc_tostring},
    {"__gc", proc_gc},
    {"__index", proc_index},
    {"poll", proc_poll},
    {"wait", proc_wait},
#if defined(OS_POSIX)
    {"send_signal", proc_send_signal},
    {"terminate", proc_terminate},
    {"kill", proc_kill},
#elif defined(OS_WINDOWS)
    {"terminate", proc_terminate},
    {"kill", proc_terminate},
#endif
    {NULL, NULL}
};

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_process_##name,nparams,tycheck}
static SQRegFunction sq_process_methods[] =
{
    _DECL_FUNC(constructor,  -2, _SC("xsi")),
    _DECL_FUNC(poll,  1, _SC("x")),
    _DECL_FUNC(wait,  1, _SC("x")),
#if defined(OS_POSIX)
    _DECL_FUNC(send_signal,  1, _SC("x")),
    _DECL_FUNC(terminate,  1, _SC("x")),
    _DECL_FUNC(kill,  1, _SC("x")),
#elif defined(OS_WINDOWS)
    _DECL_FUNC(terminate,  1, _SC("x")),
    _DECL_FUNC(kill,  1, _SC("x")),
#endif

    {0,0}
};
#undef _DECL_FUNC

/* convenience functions */
static SQRESULT sq_subprocess_call(HSQUIRRELVM v)
{
    int r = superpopen(L);
    if (r != 1){
        return r;
    }
    lua_replace(L, 1);
    lua_settop(L, 1);
    return proc_wait(L);
}

static SQRESULT sq_subprocess_call_capture(HSQUIRRELVM v)
{
    int r;
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getfield(L, 1, "stdout");
    lua_pushlightuserdata(L, &PIPE);
    lua_setfield(L, 1, "stdout");
    r = superpopen(L);
    if (r != 1) return r;
    /* stack: args oldstdout sp */
    /* restore old stdout value in table */
    lua_pushvalue(L, 2);
    lua_setfield(L, 1, "stdout");
    lua_replace(L, 1);
    lua_settop(L, 1);
    /* stack: sp */
    lua_getfield(L, 1, "stdout");
    lua_getfield(L, 2, "read");
    lua_pushvalue(L, 2);
    lua_pushliteral(L, "*a");
    lua_call(L, 2, 2);
    /* stack: sp stdout a b */
    /* close stdout, rather than relying on GC */
    lua_getfield(L, 2, "close");
    lua_pushvalue(L, 2);
    lua_call(L, 1, 0);
    /* wait for child (to avoid leaving a zombie) */
    lua_getfield(L, 1, "wait");
    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);
    /* return exitcode, content */
    lua_pushvalue(L, 3);
    return 2;
}

/* Miscellaneous */

static SQRESULT sq_subprocess_wait(HSQUIRRELVM v)
{
    struct proc *proc;
#if defined(OS_POSIX)
    int stat;
    pid_t pid;
    int exitcode;
#elif defined(OS_WINDOWS)
    HANDLE *handles = NULL, hProcess;
    int i, nprocs;
    DWORD retval;
    DWORD exitcode;
#endif

    luaL_getmetatable(L, SP_LIST);
    if (lua_isnil(L, -1))
        return luaL_error(L, "SP_LIST is nil");
#if defined(OS_POSIX)
    pid = wait(&stat);
    if (pid == -1){
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    exitcode = getexitcode(stat);
    /* find proc object corresponding to pid */
    lua_pushinteger(L, pid);
    lua_pushvalue(L, -1);    /* stack: list pid pid */
    lua_gettable(L, -3);     /* stack: list pid proc */
    if (lua_isnil(L, -1)){
        fprintf(stderr, "subprocess.c: XXX: cannot find proc object for pid %d\n", (int) pid);
    }
    lua_replace(L, -3);     /* stack: proc pid */
    lua_pop(L, 1);          /* stack: proc */
    /* update proc object */
    proc = toproc(L, -1);
    if (!proc){
        fputs("subprocess.c: XXX: proc list entry is wrong type\n", stderr);
    } else {
        proc->exitcode = exitcode;
        doneproc(L, -1);
    }
    lua_pushinteger(L, exitcode);
    lua_pushinteger(L, pid);
    /* stack: proc exitcode pid */
    return 3;
#elif defined(OS_WINDOWS)
    /* count number of procs there are */
    nprocs = sq_getsize(v, -1);
    /* stack: list */
    if (nprocs > 0){
        handles = malloc(nprocs * sizeof *handles);
        if (!handles)
            return luaL_error(L, "memory full");
        i = 0;
        lua_pushnil(L);
        while (lua_next(L, -2)){
            proc = toproc(L, -1);
            if (proc && !proc->done && i < nprocs){
                handles[i++] = proc->hProcess;
            } else if (proc && !proc->done){
                fputs("subprocess.c: XXX: handles array allocated too small\n", stderr);
            } else if (!proc){
                fputs("foreign object in SP_LIST\n", stderr);
            }
            lua_pop(L, 1);
        }
    } else i = 0;
    if (i > 0){
        if (i > MAXIMUM_WAIT_OBJECTS){
            free(handles);
            return luaL_error(L, "too many wait objects: %d", i);
        }
        retval = WaitForMultipleObjects(i, handles, FALSE, INFINITE);
        if (retval >= WAIT_OBJECT_0 && retval < (DWORD)(WAIT_OBJECT_0 + i)){
            hProcess = handles[retval - WAIT_OBJECT_0];
            free(handles);
            /* find this process again in the table */
            lua_pushnil(L);
            while (lua_next(L, -2)){
                proc = toproc(L, -1);
                if (proc && !proc->done && proc->hProcess == hProcess){
                    /* it's this one */
                    if (GetExitCodeProcess(proc->hProcess, &exitcode) == 0){
                        {
                            char buf[256];
                            copy_w32error(buf, 255, GetLastError());
                            fprintf(stderr, "GetExitCodeProcess failed: %s\n", buf);
                        }
                        proc->exitcode = -1; /*  :-\  */
                    } else {
                        proc->exitcode = exitcode;
                    }
                    CloseHandle(proc->hProcess);
                    doneproc(L, -1);
                    lua_pushinteger(L, exitcode);
                    lua_pushinteger(L, proc->pid); /* stack: list key proc exitcode pid */
                    return 3;
                }
                lua_pop(L, 1);
            }
            fputs("proc has mysteriously disappeared from table!\n", stderr);
            return 0;
        } else if (retval == WAIT_FAILED){
            free(handles);
            push_w32error(L, GetLastError());
            return lua_error(L);
        } else {
            free(handles);
            return luaL_error(L, "WaitForMultipleObjects failed unexpectedly");
        }
    } else {
        free(handles);
        lua_pushnil(L);
        lua_pushliteral(L, "no processes to wait for");
        return 2;
    }
#endif
}

static const luaL_Reg subprocess[] = {
    /* {"pipe", superpipe}, */
    {"popen", superpopen},
    {"call", call},
    {"call_capture", call_capture},
    {"wait", superwait},
    {"prune", prune},
    {NULL, NULL}
};

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_subprocess_##name,nparams,tycheck}
static SQRegFunction sq_subprocess_methods[] =
{
    _DECL_FUNC(constructor,  -2, _SC("xsi")),
    _DECL_FUNC(popen,  1, _SC("x")),
    _DECL_FUNC(call,  1, _SC("x")),
    _DECL_FUNC(call_capture,  1, _SC("x")),
    _DECL_FUNC(prune,  1, _SC("x")),
    _DECL_FUNC(wait,  1, _SC("x")),

    {0,0}
};
#undef _DECL_FUNC

SQRESULT sq_register_subprocess(HSQUIRRELVM v)
{
    /* create environment table for C functions */
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_REGISTRYINDEX, SP_LIST);
    lua_pop(L, 1);

#if LUA_VERSION_NUM >= 502
    lua_createtable(L, 0, sizeof subprocess / sizeof *subprocess - 1);
    luaL_setfuncs(L, subprocess, 0);
#else
    luaL_register(L, "subprocess", subprocess);
#endif

    /* export PIPE and STDOUT constants */
    lua_pushlightuserdata(L, &PIPE);
    lua_setfield(L, -2, "PIPE");
    lua_pushlightuserdata(L, &STDOUT);
    lua_setfield(L, -2, "STDOUT");

    /* create metatable for proc objects */
    luaL_newmetatable(L, SP_PROC_META);
#if LUA_VERSION_NUM >= 502
    luaL_setfuncs(L, proc_meta, 0);
#else
    luaL_register(L, NULL, proc_meta);
#endif
    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "__metatable");
    lua_pop(L, 1);

    return 1;
}

#endif
