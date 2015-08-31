/*
** Copyright (c) 2011 D. Richard Hipp
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the Simplified BSD License (also
** known as the "2-Clause License" or "FreeBSD License".)
**
** This program is distributed in the hope that it will be useful,
** but without any warranty; without even the implied warranty of
** merchantability or fitness for a particular purpose.
**
** Author contact information:
**   drh@hwaci.com
**   http://www.hwaci.com/drh/
**
*************************************************************************
** This file contains C code that implements a simple command-line
** wrapper shell around xjd1.
*/
#include "xjd1Int.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef _WIN32
#include <unistd.h>
#endif

/*
** Remove n elements from argv beginning with the i-th element.
*/
void remove_from_argv(char **argv, int *pArgc, int i, int n){
  int j;
  int argc = *pArgc;
  for(j=i+n; j<argc; i++, j++){
    argv[i] = argv[j];
  }
  *pArgc = i;
}


/*
** Look for a command-line option.  If present, return a pointer.
** Return NULL if missing.
**
** hasArg==0 means the option is a flag.  It is either present or not.
** hasArg==1 means the option has an argument.  Return a pointer to the
** argument.
*/
const char *find_option(
  char **argv,
  int *pArgc,
  const char *zLong,
  const char *zShort,
  int hasArg
){
  int i;
  int nLong;
  const char *zReturn = 0;
  int argc = *pArgc;
  assert( hasArg==0 || hasArg==1 );
  nLong = strlen(zLong);
  for(i=1; i<argc; i++){
    char *z;
    if (i+hasArg >= argc) break;
    z = argv[i];
    if( z[0]!='-' ) continue;
    z++;
    if( z[0]=='-' ){
      if( z[1]==0 ){
        remove_from_argv(argv, &argc, i, 1);
        break;
      }
      z++;
    }
    if( strncmp(z,zLong,nLong)==0 ){
      if( hasArg && z[nLong]=='=' ){
        zReturn = &z[nLong+1];
        remove_from_argv(argv, &argc, i, 1);
        break;
      }else if( z[nLong]==0 ){
        zReturn = argv[i+hasArg];
        remove_from_argv(argv, &argc, i, 1+hasArg);
        break;
      }
    }else if( zShort && strcmp(z,zShort)==0 ){
      zReturn = argv[i+hasArg];
      remove_from_argv(argv, &argc, i, 1+hasArg);
      break;
    }
  }
  *pArgc = argc;
  return zReturn;
}

/*
** State information
*/
typedef struct Shell Shell;
struct Shell {
  xjd1 *pDb;           /* Open database connection */
  const char *zFile;   /* Current filename */
  int nLine;           /* Current line number */
  FILE *pIn;           /* Input file */
  int isTTY;           /* True if pIn is a TTY */
  int shellFlags;      /* Flag settings */
  int testErrcode;     /* Test case error code */
  String testOut;      /* Output from a test case */
  char zModule[50];    /* Mame of current test module */
  char zTestCase[50];  /* Name of current testcase */
  String inBuf;        /* Buffered input text */
  int nCase;           /* Number of --testcase commands seen */
  int nTest;           /* Number of tests performed */
  int nErr;            /* Number of test errors */
};

/*
** Flag names
*/
#define SHELL_PARSER_TRACE     0x00001
#define SHELL_CMD_TRACE        0x00002
#define SHELL_ECHO             0x00004
#define SHELL_TEST_MODE        0x00008
static const struct {
  const char *zName;
  int iValue;
} FlagNames[] = {
  {  "parser-trace",   SHELL_PARSER_TRACE },
  {  "cmd-trace",      SHELL_CMD_TRACE    },
  {  "echo",           SHELL_ECHO         },
  {  "test-mode",      SHELL_TEST_MODE    },
};

/*
** Convert a flag name into a mask.
*/
static int flagMask(const char *zName){
  int i;
  for(i=0; i<sizeof(FlagNames)/sizeof(FlagNames[0]); i++){
    if( strcmp(zName, FlagNames[i].zName)==0 ){
      return FlagNames[i].iValue;
    }
  }
  return 0;
}

/*
** True for space characters.
*/
static int shellIsSpace(char c){
  return c==' ' || c=='\t' || c=='\r' || c=='\n';
}

/*
** Set flags
*/
static int shellSet(Shell *p, int argc, char **argv){
  int i;
  if( argc>=2 ){
    p->shellFlags |= flagMask(argv[1]);
  }else{
    for(i=0; i<sizeof(FlagNames)/sizeof(FlagNames[0]); i++){
      printf("%-20s %s\n", 
        FlagNames[i].zName,
        (p->shellFlags & FlagNames[i].iValue)!=0 ? "on" : "off");
    }
  }
  return 0;
}
static int shellClear(Shell *p, int argc, char **argv){
  if( argc>=2 ){
    p->shellFlags &= ~flagMask(argv[1]);
  }
  return 0;
}

/*
** Command:  .quit
*/
static int shellQuit(Shell *p, int argc, char **argv){
  return 1;
}

/*
** Command:  .testcase NAME
*/
static int shellTestcase(Shell *p, int argc, char **argv){
  if( argc>=2 ){
    int n = strlen(argv[1]);
    if( n>=sizeof(p->zTestCase) ) n = sizeof(p->zTestCase)-1;
    memcpy(p->zTestCase, argv[1], n);
    p->zTestCase[n] = 0;
    xjd1StringTruncate(&p->testOut);
    p->testErrcode = XJD1_OK;
    p->shellFlags |= SHELL_TEST_MODE;
  }
  return 0;
}

/*
** Return non-zero if string z matches glob pattern zGlob and zero if the
** pattern does not match.
**
** Globbing rules:
**
**      '*'       Matches any sequence of zero or more characters.
**
**      '?'       Matches exactly one character.
**
**     [...]      Matches one character from the enclosed list of
**                characters.
**
**     [^...]     Matches one character not in the enclosed list.
**
**      '#'       Matches any sequence of one or more digits with an
**                optional + or - sign in front
*/
static int strnotglob(const char *zGlob, const char *z){
  int c, c2;
  int invert;
  int seen;

  while( (c = (*(zGlob++)))!=0 ){
    if( c=='*' ){
      while( (c=(*(zGlob++))) == '*' || c=='?' ){
        if( c=='?' && (*(z++))==0 ) return 0;
      }
      if( c==0 ){
        return 1;
      }else if( c=='[' ){
        while( *z && strnotglob(zGlob-1,z)==0 ){
          z++;
        }
        return (*z)!=0;
      }
      while( (c2 = (*(z++)))!=0 ){
        while( c2!=c ){
          c2 = *(z++);
          if( c2==0 ) return 0;
        }
        if( strnotglob(zGlob,z) ) return 1;
      }
      return 0;
    }else if( c=='?' ){
      if( (*(z++))==0 ) return 0;
    }else if( c=='[' ){
      int prior_c = 0;
      seen = 0;
      invert = 0;
      c = *(z++);
      if( c==0 ) return 0;
      c2 = *(zGlob++);
      if( c2=='^' ){
        invert = 1;
        c2 = *(zGlob++);
      }
      if( c2==']' ){
        if( c==']' ) seen = 1;
        c2 = *(zGlob++);
      }
      while( c2 && c2!=']' ){
        if( c2=='-' && zGlob[0]!=']' && zGlob[0]!=0 && prior_c>0 ){
          c2 = *(zGlob++);
          if( c>=prior_c && c<=c2 ) seen = 1;
          prior_c = 0;
        }else{
          if( c==c2 ){
            seen = 1;
          }
          prior_c = c2;
        }
        c2 = *(zGlob++);
      }
      if( c2==0 || (seen ^ invert)==0 ) return 0;
    }else if( c=='#' ){
      if( (z[0]=='-' || z[0]=='+') && (z[1]>='0' && z[1]<='9') ) z++;
      if( z[0]<'0' || z[0]>'9' ) return 0;
      z++;
      while( z[0]>='0' && z[0]<='9' ){ z++; }
    }else{
      if( c!=(*(z++)) ) return 0;
    }
  }
  return *z==0;
}
static int strglob(const char *zGlob, const char *z){
  return !strnotglob(zGlob,z);
}

/*
** Verify the output from a test.
*/
static int shellResultCheck(
  Shell *p,
  int argc,
  char **argv,
  int(*xCmp)(const char*,const char*)
){
  const char *zAns = argc>=2 ? argv[1] : "";
  const char *zRes = xjd1StringText(&p->testOut);
  if( zRes==0 ) zRes = "";
  p->nTest++;
  if( xCmp(zAns, zRes) ){
    p->nErr++;
    fprintf(stderr, " Test failed: %s.%s\n Expected: [%s]\n      Got: [%s]\n",
      p->zFile, p->zTestCase, argv[1], zRes);
  }
  return 0;
}

/* Command:  .result PATTERN */
static int shellResult(Shell *p, int argc, char **argv){
  shellResultCheck(p,argc,argv,strcmp);
  return 0;
}
/* Command:  .glob PATTERN */
static int shellGlob(Shell *p, int argc, char **argv){
  shellResultCheck(p,argc,argv,strglob);
  return 0;
}
/* Command:  .notglob PATTERN */
static int shellNotGlob(Shell *p, int argc, char **argv){
  shellResultCheck(p,argc,argv,strnotglob);
  return 0;
}
/* Command:  .json PATTERN */
static int shellJson(Shell *p, int argc, char **argv){
  char *aArg[2];
  String tidy;

  assert( argc==1 || argc==2 );

  xjd1StringInit(&tidy, 0, 0);
  if( argc==2 ){
    int rc = xjd1JsonTidy(&tidy, argv[1]);
    assert( rc==XJD1_OK );
  }

  aArg[0] = argv[0];
  aArg[1] = tidy.zBuf;
  shellResultCheck(p, argc, aArg, strcmp);
  xjd1StringClear(&tidy);

  return 0;
}

/*
** Command:  .open FILENAME
*/
static int shellOpenDB(Shell *p, int argc, char **argv){
  if( argc>=2 ){
    int rc;
    if( p->pDb ) xjd1_close(p->pDb);
    rc = xjd1_open(0, argv[1], &p->pDb);
    if( rc!=XJD1_OK ){
      fprintf(stderr, "%s:%d: cannot open \"%s\"\n",
              p->zFile, p->nLine, argv[1]);
      p->nErr++;
      p->pDb = 0;
    }
  }    
  return 0;
}
/*
** Command:  .new FILENAME
*/
static int shellNewDB(Shell *p, int argc, char **argv){
  if( argc>=2 ){
    if( p->pDb ) xjd1_close(p->pDb);
    p->pDb = 0;
    unlink(argv[1]);
    shellOpenDB(p, argc, argv);
  }
  return 0;
}

/* Forward declaration */
static void processOneFile(Shell*, const char*);

/*
** Command:  .read FILENAME
** Read and process text from a file.
*/
static int shellRead(Shell *p, int argc, char **argv){
  if( argc>=2 ){
    int nErr = p->nErr;
    String newFile;
    char *zNew;
    xjd1StringInit(&newFile, 0, 0);
    if( argv[1][0]=='/' || strcmp(p->zFile,"-")==0 ){
      xjd1StringAppend(&newFile, argv[1], -1);
    }else{
      int i, j;
      for(i=j=0; p->zFile[i]; i++){ if( p->zFile[i]=='/' ) j = i; }
      xjd1StringAppendF(&newFile, "%.*s/%s", j, p->zFile, argv[1]);
    }
    zNew = xjd1StringText(&newFile);
    printf("BEGIN %s\n", zNew);
    processOneFile(p, zNew);
    printf("END %s - %d new errors\n", zNew, p->nErr - nErr);
    xjd1StringClear(&newFile);
  }
  return 0;
}

/*
** Command:  .breakpoint
** A place to seet a breakpoint
*/
static int shellBreakpoint(Shell *p, int argc, char **argv){
  /* no-op */
  return 0;
}

/*
** Command:  .puts TEXT
*/
static int shellPuts(Shell *p, int argc, char **argv){
  if( argc>=2 ) printf("%s\n", argv[1]);
  return 0;
}

static void checkForTestError(Shell *p){
  if( (p->shellFlags & SHELL_TEST_MODE) && p->testErrcode ){
    fprintf(stderr, "%s:%d: ERROR: %s\n", p->zFile, p->nLine, p->testOut.zBuf);
    xjd1StringTruncate(&p->testOut);
    p->testErrcode = XJD1_OK;
    p->nErr++;
  }
}

/*
** Process a command intended for this shell - not for the database.
**
** Return 1 to abort or 0 to continue.
*/
static int processMetaCommand(Shell *p){ char *z;
  int n;
  char *azArg[2];
  int nArg;
  int i;
  int rc;
  static const struct shcmd {
    const char *zCmd;                   /* Name of the command */
    int (*xCmd)(Shell*,int,char**);     /* Procedure to run the command */
    const char *zHelp;                  /* Help string */
  } cmds[] = {
    { "quit",       shellQuit,        ".quit"               },
    { "testcase",   shellTestcase,    ".testcase NAME"      },
    { "json",       shellJson,        ".json TEXT"          },
    { "result",     shellResult,      ".result TEXT"        },
    { "glob",       shellGlob,        ".glob PATTERN"       },
    { "notglob",    shellNotGlob,     ".notglob PATTERN"    },
    { "puts",       shellPuts,        ".puts TEXT"          },
    { "read",       shellRead,        ".read FILENAME"      },
    { "open",       shellOpenDB,      ".open DATABASE"      },
    { "new",        shellNewDB,       ".new DATABASE"       },
    { "set",        shellSet,         ".set FLAG"           },
    { "clear",      shellClear,       ".clear FLAG"         },
    { "breakpoint", shellBreakpoint,  ".breakpoint"         },
  };

  /* Remove trailing whitespace from the command */
  z = xjd1StringText(&p->inBuf);
  if( p->shellFlags & SHELL_ECHO ){
    fprintf(stdout, "%s", z);
  }
  n = xjd1StringLen(&p->inBuf);
  while( n>0 && shellIsSpace(z[n-1]) ){ n--; }
  z[n] = 0;

  /* If the last character of the command is a backslash, the command 
  ** arguments continue onto the next line. Return early without truncating
  ** the input buffer in this case. */
  if( z[n-1]=='\\' ){
    p->inBuf.nUsed = n;
    z[n-1] = ' ';
    return 0;
  }

  /* Find the command name text and its argument (if any) */
  z++;
  azArg[0] = z;
  for(i=0; z[i] && !shellIsSpace(z[i]); i++){}
  if( z[i] ){
    z[i] = 0;
    z += i+1;
    while( shellIsSpace(z[0]) ) z++;
    azArg[1] = z;
    nArg = 2;
  }else{
    azArg[1] = 0;
    nArg = 1;
  }

  if( strcmp(azArg[0], "error") ){
    checkForTestError(p);
  }else{
    p->testErrcode = XJD1_OK;
    azArg[0] = "result";
  }

  /* Find the command */
  for(i=0; i<sizeof(cmds)/sizeof(cmds[0]); i++){
    if( strcmp(azArg[0], cmds[i].zCmd)==0 ){
      rc = cmds[i].xCmd(p, nArg, azArg);
      break;
    }
  }
  if( i>=sizeof(cmds)/sizeof(cmds[0]) ){
    p->nErr++;
    fprintf(stderr, "%s:%d: unknown command \".%s\"\n", 
            p->zFile, p->nLine, azArg[0]);
    return 1;
  }
  xjd1StringTruncate(&p->inBuf);
  return rc;
}

/*
** Append text to the end of the testOut string.  If there is already
** text in the testOut buffer, prepent a space first.
*/
static void appendTestOut(Shell *p, const char *z, int n){
  if( xjd1StringLen(&p->testOut)>0 ){
    xjd1StringAppend(&p->testOut, " ", 1);
  }
  xjd1StringAppend(&p->testOut, z, n);
}

/*
** Run a single statment.
*/
static void processOneStatement(Shell *p, const char *zCmd){
  xjd1_stmt *pStmt;
  int N, rc;
  int once = 0;
  if( p->shellFlags & SHELL_ECHO ){
    fprintf(stdout, "%s\n", zCmd);
  }
  xjd1_config(p->pDb, XJD1_CONFIG_PARSERTRACE, 
              (p->shellFlags & SHELL_PARSER_TRACE)!=0);
  rc = xjd1_stmt_new(p->pDb, zCmd, &pStmt, &N);
  if( rc==XJD1_OK ){
    if( p->shellFlags & SHELL_CMD_TRACE ){
      char *zTrace = xjd1_stmt_debug_listing(pStmt);
      if( zTrace ) printf("%s", zTrace);
      free(zTrace);
    }
    do{
      rc = xjd1_stmt_step(pStmt);
      if( rc==XJD1_ROW ){
        const char *zValue;
        xjd1_stmt_value(pStmt, &zValue);
        if( (p->shellFlags & SHELL_TEST_MODE)==0 ){
          if( once==0 && (p->shellFlags & SHELL_ECHO)!=0 ){
            printf("--------- query results ---------\n");
            once = 1;
          }
          printf("%s\n", zValue);
        }else{
          appendTestOut(p, zValue, -1);
        }
      }
    }while( rc==XJD1_ROW );
    xjd1_stmt_delete(pStmt);
  }else{
    if( p->shellFlags & SHELL_TEST_MODE ){
      appendTestOut(p, xjd1_errcode_name(p->pDb), -1);
      appendTestOut(p, xjd1_errmsg(p->pDb), -1);
      p->testErrcode = rc;
    }else{
      fprintf(stderr, "%s:%d: ERROR: %s\n",
              p->zFile, p->nLine, xjd1_errmsg(p->pDb));
      p->nErr++;
    }
  }
  if( once ) printf("---------------------------------\n");
}

/*
** Process one or more database commands
*/
static void processScript(Shell *p){
  char *z, c;
  int i;

  checkForTestError(p);

  if( p->pDb==0 ){
    if( p->shellFlags & SHELL_ECHO ) printf("%s", xjd1StringText(&p->inBuf));
    xjd1StringTruncate(&p->inBuf);
    return;
  }
  while( xjd1StringLen(&p->inBuf) ){
    z = xjd1StringText(&p->inBuf);
    for(i=0; shellIsSpace(z[i]); i++){}
    if( z[i]=='-' && z[i+1]=='-' ){
      for(i+=2; z[i] && z[i]!='\n'; i++){}
      if( p->shellFlags & SHELL_ECHO ){
        printf("%.*s\n", i, z);
      }
      while( shellIsSpace(z[i]) ) i++;
      xjd1StringRemovePrefix(&p->inBuf, i);
      break;
    }
    if( z[i]==0 ){
      xjd1StringTruncate(&p->inBuf);
      break;
    }
    for(; z[i]; i++){
      if( z[i]!=';' ) continue;
      c = z[i+1];
      z[i+1] = 0;
      if( !xjd1_complete(z) ){
        z[i+1] = c;
        continue;
      }
      processOneStatement(p, z);
      z[i+1] = c;
      while( shellIsSpace(z[i+1]) ){ i++; }
      xjd1StringRemovePrefix(&p->inBuf, i+1);
      i = 0;
      break;
    }
    if( i>0 ) break;
  }
}

/*
** Process a single file of input by reading the text of the file and
** sending statements into the XJD1 database connection.
*/
static void processOneFile(
  Shell *p,               /* Current state of the shell */
  const char *zFile       /* Name of file to process.  "-" for stdin. */
){
  Shell savedState;
  char *zIn;
  char zLine[1000];

  savedState = *p;
  if( strcmp(zFile, "-")==0 ){
    p->pIn = stdin;
    p->isTTY = 1;
  }else{
    p->pIn = fopen(zFile, "r");
    if( p->pIn==0 ){
      fprintf(stderr, "%s:%d: cannot open \"%s\"\n", p->zFile, p->nLine, zFile);
      p->nErr++;
      return;
    }
    p->isTTY = 0;
  }
  p->zFile = zFile;
  p->nLine = 0;
  while( !feof(p->pIn) ){
    if( p->isTTY ){
      printf("%s", xjd1StringLen(&p->inBuf)>0 ? " ...> " : "xjd1> ");
    }
    if( fgets(zLine, sizeof(zLine), p->pIn)==0 ) break;
    p->nLine++;
    if( zLine[0]=='.' && xjd1StringLen(&p->inBuf) ){
      fprintf(stderr, "%s:%d: missing ';'\n", p->zFile, p->nLine);
      xjd1StringTruncate(&p->inBuf);
    }
    xjd1StringAppend(&p->inBuf, zLine, -1);
    zIn = xjd1StringText(&p->inBuf);
    if( zIn[0]=='.' ){
      if( processMetaCommand(p) ) break;
    }else{
      processScript(p);
    }
  }
  if( p->isTTY==0 ){
    fclose(p->pIn);
  }
  p->zFile = savedState.zFile;
  p->nLine = savedState.nLine;
  p->pIn = savedState.pIn;
  p->isTTY = savedState.isTTY;
}

int main(int argc, char **argv){
  int i;
  Shell s;

  memset(&s, 0, sizeof(s));
  if( argc>1 ){
    for(i=1; i<argc; i++){
      processOneFile(&s, argv[i]);
    }
  }else{
    processOneFile(&s, "-");
  }
  if( s.nTest ){
    printf("%d errors from %d tests\n", s.nErr, s.nTest);
  }
  if( s.pDb ) xjd1_close(s.pDb);
  xjd1StringClear(&s.inBuf);
  xjd1StringClear(&s.testOut);
  return 0;
}
