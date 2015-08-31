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
** This file contains C code that implements the xjd1_complete() API.
*/
#include "xjd1Int.h"

/*
** Return true if the statement in zStmt is complete.
**
** A complete statement ends with a semicolon where the semicolon is
** not part of a larger token such as a string or a JSON structure.
**
** Trailing whitespace is ignored.  Comment text is considered whitespace
** for the purposes of the previous sentence.
**
** An empty string is considered a complete statement.
*/
int xjd1_complete(const char *zStmt){
  int isComplete = 1;
  if( zStmt==0 ) return 1;
  while( zStmt[0] ){
    switch( zStmt[0] ){
      case ';': {
        isComplete = 1;
        zStmt++;
        break;
      }
      case ' ':
      case '\r':
      case '\t':
      case '\n':
      case '\f': {  /* White space is ignored */
        zStmt++;
        break;
      }
      case '/': {   /* C-style comments */
        if( zStmt[1]!='*' ){
          zStmt++;
          break;
        }
        zStmt += 2;
        while( zStmt[0] && (zStmt[0]!='*' || zStmt[1]!='/') ){ zStmt++; }
        if( zStmt[0]==0 ) return 0;
        zStmt++;
        break;
      }
      case '-': {   /* SQL-style comments from "--" to end of line */
        if( zStmt[1]!='-' ){
          zStmt++;
          break;
        }
        while( zStmt[0] && *zStmt!='\n' ){ zStmt++; }
        if( zStmt[0]==0 ) return isComplete;
        break;
      }
      case '"': {   /* C-style string literals */
        zStmt++;
        while( zStmt[0] && zStmt[0]!='"' ){
          if( zStmt[0]=='\\' ) zStmt++;
          zStmt++;
        }
        if( zStmt[0]==0 ) return 0;
        zStmt++;
        isComplete = 0;
        break;
      }
      default: {    /* Anything else */
        zStmt++;
        isComplete = 0;
        break;
      }
    }
  }
  return isComplete;
}
