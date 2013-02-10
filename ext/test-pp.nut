local pgsql_functions = [
    ["ConnStatusType", "PQstatus", "const PGconn *conn"],
    ["void", "PQfinish", "PGconn *conn"],
    ["PGresult *", "PQprepare", @"PGconn *conn,
                        const char *stmtName,
                        const char *query,
                        int nParams,
                        const Oid *paramTypes"],
    ["PGresult *", "PQdescribePrepared", "PGconn *conn, const char *stmtName"],
    ["int", "PQnparams", "const PGresult *res"],
    ["Oid", "PQparamtype", "const PGresult *res, int param_number"],
    ["PGresult *", "PQexecPrepared", @"PGconn *conn,
                         const char *stmtName,
                         int nParams,
                         const char * const *paramValues,
                         const int *paramLengths,
                         const int *paramFormats,
                         int resultFormat"],
    ["PGresult *", "PQexec", "PGconn *conn, const char *command"],
    ["const char*", "PQgetvalue", @"const PGresult *res,
                         int row_number,
                         int column_number"],
    ["int", "PQntuples", "const PGresult *res"],
    ["char *", "PQcmdTuples", "const PGresult *res"],
    ["int", "PQnfields", "const PGresult *res"],
    ["int", "PQgetisnull", @"const PGresult *res,
                        int row_number,
                        int column_number"],
    ["void", "PQclear", "const PGresult *res"],
    ["int", "PQfnumber", "const PGresult *res, const char *column_name"],
    ["char *", "PQfname", "const PGresult *res, int column_number"],
    ["ExecStatusType", "PQresultStatus", "const PGresult *res"],
    ["char *", "PQerrorMessage", "const PGconn *conn"],
    ["void", "PQreset", "PGconn *conn"],
    ["PGresult *", "PQgetResult", "PGconn *conn"],
    ["int", "PQsetnonblocking", "PGconn *conn, int arg"],
    ["Oid", "PQftype", "const PGresult *res, int column_number"],
    ["int", "PQserverVersion", "const PGconn *conn"],

    // Large-object access routines
    ["int", "lo_open", "PGconn *conn, Oid lobjId, int mode"],
    ["int", "lo_close", "PGconn *conn, int fd"],
    ["int", "lo_read", "PGconn *conn, int fd, char *buf, size_t len"],
    ["int", "lo_write", "PGconn *conn, int fd, const char *buf, size_t len"],
    ["int", "lo_lseek", "PGconn *conn, int fd, int offset, int whence"],
    ["Oid",	"lo_creat", "PGconn *conn, int mode"],
    ["Oid",	"lo_create", "PGconn *conn, Oid lobjId"],
    ["int", "lo_tell", "PGconn *conn, int fd"],
    ["int", "lo_truncate", "PGconn *conn, int fd, size_t len"],
    ["int", "lo_unlink", "PGconn *conn, Oid lobjId"],
    ["Oid",	"lo_import", "PGconn *conn, const char *filename"],
    ["Oid",	"lo_import_with_oid", "PGconn *conn, const char *filename, Oid lobjId"],
    ["int", "lo_export", "PGconn *conn, Oid lobjId, const char *filename"],

    //next entry should be the last one
    //to make valid the test made on load_libpq function
    ["PGconn *", "PQconnectdb", "const char *conninfo"],
];

function write_pgsql_functions_declaration(){
    foreach(k,v in pgsql_functions) {
        putsnl("typedef " + v[1] + " (*" + v[2] + "_t)(" + v[3] + ");");
        putsnl("static " + v[2] + "_t dl" + v[2] + " = 0;");
    }
}

function write_pgsql_functions_load(){
    foreach(k,v in pgsql_functions){
        putsnl("dl" + v[2] + " = (" + v[2] + "_t) libpq.dlsym(\"" + v[2] + "\");");
        putsnl("if(!dl" + v[2] + ") return false;");
    }
}
