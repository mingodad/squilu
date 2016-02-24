#ifndef SQ_SQLITE3_H
#define SQ_SQLITE3_H

#include "squirrel.h"
#include "sqlite3.h"
#include <string.h>
#include <stdio.h>
#include "sqstdblobimpl.h"

typedef struct sq_sqlite3_sdb sq_sqlite3_sdb;
typedef struct sq_sqlite3_sdb_func sq_sqlite3_sdb_func;

/* to use as C user data so i know what function sqlite is calling */
struct sq_sqlite3_sdb_func
{
    /* references to associated lua values */
    HSQOBJECT fn_step;
    HSQOBJECT fn_finalize;
    HSQOBJECT udata;

    sq_sqlite3_sdb *sdb;
    sq_sqlite3_sdb_func *next;
};

/* information about database */
struct sq_sqlite3_sdb
{
    /* associated squirrel vm */
    HSQUIRRELVM v;
    /* sqlite database handle */
    sqlite3 *db;

    /* sql functions stack usage */
    sq_sqlite3_sdb_func *func;         /* top SQL function being called */

    /* references */
    HSQOBJECT busy_cb;        /* busy callback */
    HSQOBJECT busy_udata;

    HSQOBJECT progress_cb;    /* progress handler */
    HSQOBJECT progress_udata;

    HSQOBJECT trace_cb;       /* trace callback */
    HSQOBJECT trace_udata;

    HSQOBJECT update_hook_cb;       /* update hook callback */
    HSQOBJECT update_hook_udata;

    HSQOBJECT authorizer_hook_cb;       /* update hook callback */
    HSQOBJECT authorizer_hook_udata;

    HSQOBJECT null_value;
};

extern const SQChar *SQLite3_TAG;

#endif // SQ_SQLITE3_H
