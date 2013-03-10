/* see copyright notice in squirrel.h */
#include <squirrel.h>
#include <string.h>
#include "sqstdblobimpl.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32_WCE
#include <signal.h>
#endif
#include <sqstdsystem.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef SQUNICODE
#include <wchar.h>
#define scgetenv _wgetenv
#define scsystem _wsystem
#define scasctime _wasctime
#define scstrftime _wstrftime
#define scremove _wremove
#define screname _wrename
#else
#define scgetenv getenv
#define scsystem system
#define scasctime asctime
#define scstrftime strftime
#define scremove remove
#define screname rename
#endif

SQ_OPT_STRING_STRLEN();

#ifndef _WIN32_WCE
static SQRESULT _system_getenv(HSQUIRRELVM v)
{
	const SQChar *s;
	if(SQ_SUCCEEDED(sq_getstring(v,2,&s))){
        sq_pushstring(v,scgetenv(s),-1);
		return 1;
	}
	return 0;
}

static SQRESULT _system_system(HSQUIRRELVM v)
{
	const SQChar *s;
	if(SQ_SUCCEEDED(sq_getstring(v,2,&s))){
		sq_pushinteger(v,scsystem(s));
		return 1;
	}
	return sq_throwerror(v,_SC("wrong param"));
}


static SQRESULT _system_clock(HSQUIRRELVM v)
{
	sq_pushfloat(v,((SQFloat)clock())/(SQFloat)CLOCKS_PER_SEC);
	return 1;
}
#endif

/*
static SQRESULT _system_time(HSQUIRRELVM v)
{
	time_t t;
	time(&t);
	sq_pushinteger(v,*((SQInteger *)&t));
	return 1;
}
*/

static SQRESULT get_int_field (HSQUIRRELVM v, const SQChar *key, int dflt) {
    SQInteger res;
    SQObjectType stype;
    sq_pushstring(v, key, -1);
    if(sq_get(v, -2) == SQ_OK){
        stype = sq_gettype(v, -1);
        if (stype & SQOBJECT_NUMERIC){
            sq_getinteger(v, -1, &res);
            sq_poptop(v);
            return res;
        }
        sq_poptop(v);
  }
  if(dflt < 0) return sq_throwerror(v, _SC("field %s is missing"), key);
  return dflt;
}

static SQRESULT _system_time(HSQUIRRELVM v) {
  time_t t;
  if (sq_gettop(v) == 1)  /* called without args? */
    t = time(NULL);  /* get current time */
  else {
    struct tm ts;
    if(sq_gettype(v, 2) != OT_TABLE)
        return sq_throwerror(v, _SC("table expected as parameter but got (%s)"), sq_gettypename(v, 2));
    sq_settop(v, 2);  /* make sure table is at the top */
    SQInteger rc = get_int_field(v, _SC("sec"), 0);
    if(rc == SQ_ERROR) return rc;
    else ts.tm_sec = rc;

    rc = get_int_field(v, _SC("min"), 0);
    if(rc == SQ_ERROR) return rc;
    else ts.tm_min = rc;

    rc = get_int_field(v, _SC("hour"), 12);
    if(rc == SQ_ERROR) return rc;
    else ts.tm_hour = rc;

    rc = get_int_field(v, _SC("day"), -1);
    if(rc == SQ_ERROR) return rc;
    else ts.tm_mday = rc;

    rc = get_int_field(v, _SC("month"), -1);
    if(rc == SQ_ERROR) return rc;
    else ts.tm_mon = rc;

    rc = get_int_field(v, _SC("year"), -1);
    if(rc == SQ_ERROR) return rc;
    else ts.tm_year = rc - 1900;

    SQBool isdst = SQTrue;
    sq_pushstring(v, _SC("isdst"), -1);
    if(sq_get(v, 2) == SQ_OK){
        if(sq_getbool(v, -1, &isdst) == SQ_OK){
            ts.tm_isdst = isdst;
        }
        sq_poptop(v);
    }
    ts.tm_isdst = isdst;
    t = mktime(&ts);
  }
  if (t == (time_t)(-1))
    sq_pushnull(v);
  else sq_pushinteger(v,(SQInteger)t);
  return 1;
}


static SQRESULT _system_difftime (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    SQ_GET_FLOAT(v, 2, t1);
    SQ_OPT_FLOAT(v, 3, t2, 0);
    sq_pushfloat(v, difftime( (time_t)t1, (time_t)t2));
  return 1;
}

#ifndef _WIN32_WCE
static SQRESULT _system_remove(HSQUIRRELVM v)
{
	const SQChar *s;
	sq_getstring(v,2,&s);
	if(scremove(s)==-1)
		return sq_throwerror(v,_SC("remove() failed"));
	return 0;
}

static SQRESULT _system_rename(HSQUIRRELVM v)
{
	const SQChar *oldn,*newn;
	sq_getstring(v,2,&oldn);
	sq_getstring(v,3,&newn);
	if(screname(oldn,newn)==-1)
		return sq_throwerror(v,_SC("rename() failed"));
	return 0;
}

static void _set_integer_slot(HSQUIRRELVM v,const SQChar *name,SQInteger val)
{
	sq_pushstring(v,name,-1);
	sq_pushinteger(v,val);
	sq_rawset(v,-3);
}

static SQRESULT _system_date(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_STRING(v, 2, arg_format, _SC("%c"));
    SQ_OPT_FLOAT(v, 3, arg_time, time(NULL));
	time_t t = (time_t)arg_time;

    struct tm *stm;
    if (*arg_format == _SC('!')) {  /* UTC? */
        stm = gmtime(&t);
        arg_format++;  /* skip `!' */
    }
    else
        stm = localtime(&t);
    if (stm == NULL)  /* invalid date? */
        sq_pushnull(v);
    else if (scstrcmp(arg_format, _SC("*t")) == 0) {
        sq_newtableex(v, 9); /* 9 = number of fields */
        _set_integer_slot(v, _SC("sec"), stm->tm_sec);
        _set_integer_slot(v, _SC("min"), stm->tm_min);
        _set_integer_slot(v, _SC("hour"), stm->tm_hour);
        _set_integer_slot(v, _SC("day"), stm->tm_mday);
        _set_integer_slot(v, _SC("month"), stm->tm_mon);
        _set_integer_slot(v, _SC("year"), stm->tm_year+1900);
        _set_integer_slot(v, _SC("wday"), stm->tm_wday);
        _set_integer_slot(v, _SC("yday"), stm->tm_yday);
        sq_pushliteral(v, _SC("isdst"));
        sq_pushbool(v, stm->tm_isdst);
        sq_rawset(v, -3);
    }
    else {
        SQChar cc[3];
        SQBlob b(0, BLOB_BUFSIZE);
        cc[0] = _SC('%'); cc[2] = _SC('\0');
        for (; *arg_format; arg_format++) {
            if (*arg_format != _SC('%') || *(arg_format + 1) == _SC('\0'))  /* no conversion specifier? */
                b.WriteChar(*arg_format);
            else {
                size_t reslen;
                SQChar buff[200];  /* should be big enough for any conversion result */
                cc[1] = *(++arg_format);
                reslen = scstrftime(buff, sizeof(buff)/sizeof(SQChar), cc, stm);
                b.Write(buff, reslen);
            }
        }
        sq_pushstring(v, (const SQChar*)b.GetBuf(), b.Len());
    }
    return 1;
}
#endif

static SQRESULT _system_exit (HSQUIRRELVM v) {
  SQRESULT status = 0;
  SQ_FUNC_VARS(v);
  if(_top_ > 1){
      SQObjectType ptype = sq_gettype(v, 1);
      if (ptype == OT_BOOL){
        SQ_GET_BOOL(v,2, b);
        status = b ? EXIT_SUCCESS : EXIT_FAILURE;
      }
      else
        sq_getinteger(v, 2, &status);
      SQ_OPT_BOOL(v, 3, bclose, false);
      if (bclose) sq_close(v);
  }
  exit(status);
}

#if defined(SC_USE_MKSTEMP)
#include <unistd.h>
#define SQ_TMPNAMBUFSIZE	32
#define sq_tmpnam(b,e)	{ \
	strcpy(b, "/tmp/lua_XXXXXX"); \
	e = mkstemp(b); \
	if (e != -1) close(e); \
	e = (e == -1); }

#else
#define SQ_TMPNAMBUFSIZE	L_tmpnam
#define sq_tmpnam(b,e)		{ e = (tmpnam(b) == NULL); }
#endif

#ifndef _WIN32_WCE
static SQRESULT _system_tmpname (HSQUIRRELVM v) {
  SQChar buff[SQ_TMPNAMBUFSIZE];
  int err;
  sq_tmpnam(buff, err);
  if (err)
    return sq_throwerror(v, _SC("unable to generate a unique filename"));
  sq_pushstring(v, buff, -1);
  return 1;
}

#include <locale.h>
static SQRESULT _system_setlocale (HSQUIRRELVM v) {
  static const int cat[] = {LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY,
                      LC_NUMERIC, LC_TIME};
  static const SQChar *const catnames[] = {_SC("all"), _SC("collate"), _SC("ctype"),
		_SC("monetary"), _SC("numeric"), _SC("time"), NULL};
  SQ_FUNC_VARS(v);
  SQ_OPT_STRING(v, 2, l, NULL);
  SQ_OPT_STRING(v, 3, name, _SC("all"));
  for (int i=0; catnames[i]; i++)
    if (scstrcmp(catnames[i], name) == 0){
        sq_pushstring(v, setlocale(cat[i], l), -1);
        return 1;
    }
  return sq_throwerror(v, _SC("invalid option %s for param %d"), name, 3);
}
#endif

/*-------------------------------------------------------------------------*\
* Sleep for n seconds.
\*-------------------------------------------------------------------------*/
static SQRESULT  _system_sleep(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FLOAT(v, 2, n);
#ifdef _WIN32
    Sleep((int)(n*1000));
#else
    struct timespec t, r;
    t.tv_sec = (int) n;
    n -= t.tv_sec;
    t.tv_nsec = (int) (n * 1000000000);
    if (t.tv_nsec >= 1000000000) t.tv_nsec = 999999999;
    while (nanosleep(&t, &r) != 0) {
        t.tv_sec = r.tv_sec;
        t.tv_nsec = r.tv_nsec;
    }
#endif
    return 0;
}

#ifndef _WIN32_WCE
#include <sys/timeb.h>

int GetMilliCount()
{
  // Something like GetTickCount but portable
  // It rolls over every ~ 12.1 days (0x100000/24/60/60)
  // Use GetMilliSpan to correct for rollover
  timeb tb;
  ftime( &tb );
  int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
  return nCount;
}

int GetMilliSpan( int nTimeStart )
{
  int nSpan = GetMilliCount() - nTimeStart;
  if ( nSpan < 0 )
    nSpan += 0x100000 * 1000;
  return nSpan;
}

static SQRESULT _system_getmillicount (HSQUIRRELVM v) {
    sq_pushinteger(v, GetMilliCount());
    return 1;
}

static SQRESULT _system_getmillispan (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, nTimeStart);
    sq_pushinteger(v, GetMilliSpan(nTimeStart));
    return 1;
}
#endif

#if 0
struct sq_signal_list
{
  char *name; /* name of the signal */
  int sig; /* the signal */
};

static const struct sq_signal_list sq_signals_list[] = {
  /* ANSI C signals */
#ifdef SIGABRT
  {"SIGABRT", SIGABRT},
#endif
#ifdef SIGFPE
  {"SIGFPE", SIGFPE},
#endif
#ifdef SIGILL
  {"SIGILL", SIGILL},
#endif
#ifdef SIGINT
  {"SIGINT", SIGINT},
#endif
#ifdef SIGSEGV
  {"SIGSEGV", SIGSEGV},
#endif
#ifdef SIGTERM
  {"SIGTERM", SIGTERM},
#endif
  /* posix signals */
#ifdef SIGHUP
  {"SIGHUP", SIGHUP},
#endif
#ifdef SIGQUIT
  {"SIGQUIT", SIGQUIT},
#endif
#ifdef SIGTRAP
  {"SIGTRAP", SIGTRAP},
#endif
#ifdef SIGKILL
  {"SIGKILL", SIGKILL},
#endif
#ifdef SIGUSR1
  {"SIGUSR1", SIGUSR1},
#endif
#ifdef SIGUSR2
  {"SIGUSR2", SIGUSR2},
#endif
#ifdef SIGPIPE
  {"SIGPIPE", SIGPIPE},
#endif
#ifdef SIGALRM
  {"SIGALRM", SIGALRM},
#endif
#ifdef SIGCHLD
  {"SIGCHLD", SIGCHLD},
#endif
#ifdef SIGCONT
  {"SIGCONT", SIGCONT},
#endif
#ifdef SIGSTOP
  {"SIGSTOP", SIGSTOP},
#endif
#ifdef SIGTTIN
  {"SIGTTIN", SIGTTIN},
#endif
#ifdef SIGTTOU
  {"SIGTTOU", SIGTTOU},
#endif
  /* some BSD signals */
#ifdef SIGIOT
  {"SIGIOT", SIGIOT},
#endif
#ifdef SIGBUS
  {"SIGBUS", SIGBUS},
#endif
#ifdef SIGCLD
  {"SIGCLD", SIGCLD},
#endif
#ifdef SIGURG
  {"SIGURG", SIGURG},
#endif
#ifdef SIGXCPU
  {"SIGXCPU", SIGXCPU},
#endif
#ifdef SIGXFSZ
  {"SIGXFSZ", SIGXFSZ},
#endif
#ifdef SIGVTALRM
  {"SIGVTALRM", SIGVTALRM},
#endif
#ifdef SIGPROF
  {"SIGPROF", SIGPROF},
#endif
#ifdef SIGWINCH
  {"SIGWINCH", SIGWINCH},
#endif
#ifdef SIGPOLL
  {"SIGPOLL", SIGPOLL},
#endif
#ifdef SIGIO
  {"SIGIO", SIGIO},
#endif
  /* add odd signals */
#ifdef SIGSTKFLT
  {"SIGSTKFLT", SIGSTKFLT}, /* stack fault */
#endif
#ifdef SIGSYS
  {"SIGSYS", SIGSYS},
#endif
  {NULL, 0}
};

static HSQUIRRELVM vsig = NULL;

static void sq_sig_handle(int sig)
{
    if(vsig){
        sq_pushliteral(vsig, _SC("sq_sig_handler"));
        if(sq_getonregistrytable(vsig) == SQ_OK){
            SQObjectType ptype = sq_gettype(v, -1);
            if(ptype == OT_CLOSURE || ptype == OT_NATIVECLOSURE){
                sq_pushroottable(v);
                sq_pushinteger(v, sig);
                sq_call(v, 2, SQFalse, SQFalse);
            }
        }
    }
}

/*
 * _system_raise == raise(signal)
 *
 * signal = signal number or string
*/

static int _system_raise(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    switch(sq_gettype(v, 2)){
        case OT_INTEGER:{
            SQ_GET_INTEGER(v, 2, iparam);
            sq_pushinteger(v, raise(iparam));
        }
        break;
        case OT_STRING:{
            int sig = 0;
            SQ_GET_STRING(v, 2, signame);
            for(int i=0, len = sizeof(sq_signals_list)/sizeof(sq_signal_list); i < len; ++i){
                if(strcmp(sq_signals_list[i].name, signame) == 0){
                    sig = sq_signals_list[i].sig;
                    break;
                }
            }
            if(sig) sq_pushinteger(v, raise(sig));
            else return sq_throwerror(v, _SC("invalid signal (%s)"), signame);
        }
        break;
        default:
            return sq_throwerror(v, _SC("invalid paramter (%s)"), sq_gettypename(v, 2));
    }
    return 1;
}
#endif

#define _DECL_FUNC(name,nparams,pmask) {_SC(#name),_system_##name,nparams,pmask}
static SQRegFunction systemlib_funcs[]={
#ifndef _WIN32_WCE
	_DECL_FUNC(getenv,2,_SC(".s")),
	_DECL_FUNC(system,2,_SC(".s")),
	_DECL_FUNC(clock,0,NULL),
	_DECL_FUNC(remove,2,_SC(".s")),
	_DECL_FUNC(rename,3,_SC(".ss")),
	_DECL_FUNC(date,-1,_SC(".sn")),
	_DECL_FUNC(tmpname,1,_SC(".")),
	_DECL_FUNC(setlocale,-1,_SC(".ss")),
	_DECL_FUNC(getmillicount,1,_SC(".")),
	_DECL_FUNC(getmillispan,2,_SC(".i")),
#endif
	_DECL_FUNC(time,-1,_SC(".t")),
	_DECL_FUNC(difftime,-2,_SC(".nn")),
	_DECL_FUNC(exit, -1,_SC(". b|i b")),
	_DECL_FUNC(sleep, 2,_SC(".n")),
	{0,0}
};
#undef _DECL_FUNC

SQInteger sqstd_register_systemlib(HSQUIRRELVM v)
{
    sq_pushstring(v,_SC("os"),-1);
    sq_newtable(v);

	SQInteger i=0;
	while(systemlib_funcs[i].name!=0)
	{
		sq_pushstring(v,systemlib_funcs[i].name,-1);
		sq_newclosure(v,systemlib_funcs[i].f,0);
		sq_setparamscheck(v,systemlib_funcs[i].nparamscheck,systemlib_funcs[i].typemask);
		sq_setnativeclosurename(v,-1,systemlib_funcs[i].name);
		sq_newslot(v,-3,SQFalse);
		i++;
	}

	sq_newslot(v,-3,SQTrue); //insert os
	return 1;
}
