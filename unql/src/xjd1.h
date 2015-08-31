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
** Interface definitions for XJD1
*/
#ifndef _XJD1_H
#define _XJD1_H

/* Return codes */
#define XJD1_OK       0      /* No error */
#define XJD1_ERROR    1      /* Generic error */
#define XJD1_MISUSE   2      /* Misuse of the interface */
#define XJD1_NOMEM    3      /* Out of heap memory */
#define XJD1_UNKNOWN  4      /* Unknown configuration option */
#define XJD1_SYNTAX   5      /* Syntax error */
#define XJD1_ERROR_OPEN_DB   6      /* Error opening a database */

#define XJD1_ROW      200    /* Another row available */
#define XJD1_DONE     201    /* query complete */


/* Execution context */
typedef struct xjd1_context xjd1_context;

/* A database connection */
typedef struct xjd1 xjd1;

/* A prepared statement */
typedef struct xjd1_stmt xjd1_stmt;

/* Create, setup, and destroy an execution context */
int xjd1_context_new(xjd1_context**);
int xjd1_context_config(xjd1_context*, int, ...);
int xjd1_context_delete(xjd1_context*);

/* Operators for xjd1_context_config() */
#define XJD1_CONTEXT_LOG   1

/* Open and close a database connection */
int xjd1_open(xjd1_context*, const char *zURI, xjd1**);
int xjd1_config(xjd1*, int, ...);
int xjd1_close(xjd1*);

/* sqlite3 database */
typedef struct sqlite3 sqlite3;
int xjd1_open_with_db(xjd1_context*, sqlite3 *db, xjd1**);

/* Operators for xjd1_config() */
#define XJD1_CONFIG_PARSERTRACE    1

/* Report on recent errors */
int xjd1_errcode(xjd1*);
const char *xjd1_errmsg(xjd1*);
const char *xjd1_errcode_name(xjd1*);

/* Create a new prepared statement */
int xjd1_stmt_new(xjd1*, const char*, xjd1_stmt**, int*);
int xjd1_stmt_delete(xjd1_stmt*);
int xjd1_stmt_config(xjd1_stmt*, int, ...);

/* Process a prepared statement */
int xjd1_stmt_step(xjd1_stmt*);
int xjd1_stmt_rewind(xjd1_stmt*);
int xjd1_stmt_value(xjd1_stmt*, const char**);

/* Return true if zStmt is a complete query statement */
int xjd1_complete(const char *zStmt);

/* Show the internal structure of a statement */
char *xjd1_stmt_debug_listing(xjd1_stmt*);


/* Configure or access the malloc/realloc/free used by the module. This
** will change. */
void xjd1_configure_malloc(
  void *(*xMalloc)(int),
  void *(*xRealloc)(void *, int),
  void (*xFree)(void *)
);
void *xjd1_malloc(int N);
void xjd1_free(void *p);
void *xjd1_realloc(void *p, int N);

#endif /* _XJD1_H */
