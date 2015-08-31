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
** Code to evaluate a pragma.
*/
#include "xjd1Int.h"

/*
** Evaluate a pragma.
**
** Unknown pragmas are silently ignored.
*/
int xjd1PragmaStep(xjd1_stmt *pStmt){
  Command *pCmd = pStmt->pCmd;
  int rc = XJD1_OK;
  assert( pCmd!=0 );
  assert( pCmd->eCmdType==TK_PRAGMA );
  return rc;
}
