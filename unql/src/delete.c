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
** Code to evaluate a DELETE command.
*/
#include "xjd1Int.h"

/*
** Evaluate a pragma.
**
** Unknown pragmas are silently ignored.
*/
int xjd1DeleteStep(xjd1_stmt *pStmt){
  Command *pCmd = pStmt->pCmd;
  int rc = XJD1_OK;
  int inAutocommit;
  sqlite3 *db;
  sqlite3_stmt *pQuery = 0;
  sqlite3_stmt *pIns = 0;
  char *zSql;

  assert( pCmd!=0 );
  assert( pCmd->eCmdType==TK_DELETE );
  db = pStmt->pConn->db;
  inAutocommit = sqlite3_get_autocommit(db);
  if( pCmd->u.del.pWhere==0 ){
    zSql = sqlite3_mprintf("DELETE FROM \"%w\"", pCmd->u.del.zName);
    sqlite3_exec(db, zSql, 0, 0, 0);
    sqlite3_free(zSql);
    return XJD1_OK;
  }
  zSql = sqlite3_mprintf("%sCREATE TEMP TABLE _t1(x INTEGER PRIMARY KEY)",
            inAutocommit ? "BEGIN;" : "");
  sqlite3_exec(db, zSql, 0, 0, 0);
  sqlite3_free(zSql);
  zSql = sqlite3_mprintf("SELECT rowid, x FROM \"%w\"", pCmd->u.del.zName);
  sqlite3_prepare_v2(db, zSql, -1, &pQuery, 0);
  sqlite3_prepare_v2(db, "INSERT INTO _t1(x) VALUES(?1)", -1, &pIns, 0);
  if( pQuery ){
    while( SQLITE_ROW==sqlite3_step(pQuery) ){
      const char *zJson = (const char*)sqlite3_column_text(pQuery, 1);
      pStmt->pDoc = xjd1JsonParse(zJson, -1);
      if( xjd1ExprTrue(pCmd->u.del.pWhere) ){
        sqlite3_bind_int64(pIns, 1, sqlite3_column_int64(pQuery, 0));
        sqlite3_step(pIns);
        sqlite3_reset(pIns);
      }
      xjd1JsonFree(pStmt->pDoc);
      pStmt->pDoc = 0;
    }
  }
  sqlite3_finalize(pQuery);
  sqlite3_finalize(pIns);
  sqlite3_free(zSql);
  zSql = sqlite3_mprintf(
            "DELETE FROM \"%w\" WHERE rowid IN _t1;"
            "DROP TABLE _t1;%s", pCmd->u.del.zName,
            inAutocommit ? "COMMIT;" : "");
  sqlite3_exec(db, zSql, 0, 0, 0);
  sqlite3_free(zSql);
  return rc;
}
