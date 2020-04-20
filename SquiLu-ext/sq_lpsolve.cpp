#if defined(SQ_USE_LPSOLVE) || defined(SQ_USE_LPSOLVE_STATIC)

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

#include "lp_lib.h"

////////
#include "dynamic_library.h"

/*SquiLu

local library_functions = [
    ["void", "lp_solve_version", "int *majorversion, int *minorversion, int *release, int *build"],
    ["lprec*", "make_lp", "int rows, int columns"],
    ["MYBOOL", "resize_lp", "lprec *lp, int rows, int columns"],
    ["lprec*", "read_LP", "char *filename, int verbose, char *lp_name"],
    ["lprec*", "read_MPS", "char *filename, int options"],
    ["lprec*", "read_freeMPS", "char *filename, int options"],
    ["void", "delete_lp", "lprec *plp"],

    //next entry should be the last one
    //to make valid the test made on load_library function
    ["void", "free_lp", "lprec **plp"],
];

function write_library_functions_declaration(){
    foreach(k,v in library_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_library_functions_load(){
    foreach(k,v in library_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libdyn.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}

function write_library_functions_static_defines(){
    foreach(k,v in library_functions){
        putsnl("#define dl" + v[1] + " " + v[1]);
    }
}
SquiLu*/

#ifdef SQ_USE_LPSOLVE_STATIC

#define load_library(x) true

//@write_library_functions_static_defines()
// generated-code:begin
#define dllp_solve_version lp_solve_version
#define dlmake_lp make_lp
#define dlresize_lp resize_lp
#define dlread_LP read_LP
#define dlread_MPS read_MPS
#define dlread_freeMPS read_freeMPS
#define dldelete_lp delete_lp
#define dlfree_lp free_lp
// generated-code:end

#else

static DynamicLibrary libdyn;

//@write_library_functions_declaration();
// generated-code:begin
typedef void (*lp_solve_version_t)(int *majorversion, int *minorversion, int *release, int *build);
static lp_solve_version_t dllp_solve_version = 0;
typedef lprec* (*make_lp_t)(int rows, int columns);
static make_lp_t dlmake_lp = 0;
typedef MYBOOL (*resize_lp_t)(lprec *lp, int rows, int columns);
static resize_lp_t dlresize_lp = 0;
typedef lprec* (*read_LP_t)(char *filename, int verbose, char *lp_name);
static read_LP_t dlread_LP = 0;
typedef lprec* (*read_MPS_t)(char *filename, int options);
static read_MPS_t dlread_MPS = 0;
typedef lprec* (*read_freeMPS_t)(char *filename, int options);
static read_freeMPS_t dlread_freeMPS = 0;
typedef void (*delete_lp_t)(lprec *plp);
static delete_lp_t dldelete_lp = 0;
typedef void (*free_lp_t)(lprec **plp);
static free_lp_t dlfree_lp = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(liblpsolve55);

static bool load_library(const char *libname)
{
    if(dlfree_lp) return true;
    if(libdyn.open(libname))
    {
        //@write_library_functions_load();
// generated-code:begin
dllp_solve_version = (lp_solve_version_t) libdyn.dlsym("lp_solve_version");
if(!dllp_solve_version) return false;
dlmake_lp = (make_lp_t) libdyn.dlsym("make_lp");
if(!dlmake_lp) return false;
dlresize_lp = (resize_lp_t) libdyn.dlsym("resize_lp");
if(!dlresize_lp) return false;
dlread_LP = (read_LP_t) libdyn.dlsym("read_LP");
if(!dlread_LP) return false;
dlread_MPS = (read_MPS_t) libdyn.dlsym("read_MPS");
if(!dlread_MPS) return false;
dlread_freeMPS = (read_freeMPS_t) libdyn.dlsym("read_freeMPS");
if(!dlread_freeMPS) return false;
dldelete_lp = (delete_lp_t) libdyn.dlsym("delete_lp");
if(!dldelete_lp) return false;
dlfree_lp = (free_lp_t) libdyn.dlsym("free_lp");
if(!dlfree_lp) return false;
// generated-code:end

        return true;
    }
    return false;
}

#endif // SQ_USE_LPSOLVE_STATIC

SQ_OPT_STRING_STRLEN();

static const SQChar LPSOLVE_Tag[]   = _SC("SQLPSolve");
#define GET_lpsolve_INSTANCE() SQ_GET_INSTANCE(v, 1, lprec, LPSOLVE_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("sqlpsolve object already closed"));

static SQRESULT sq_lpsolve_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	lprec *self = ((lprec *)p);
	if(self)
    {
        dldelete_lp(self);
    }
	return 1;
}

static SQRESULT sq_lpsolve_constructor(HSQUIRRELVM v)
{
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load liblpsolve !"));
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_INTEGER(v, 2, constraints);
	SQ_GET_INTEGER(v, 3, variables);

	lprec *self = dlmake_lp(constraints, variables);
	self->set_verbose(self, IMPORTANT);

	sq_setinstanceup(v,1,self);
	sq_setreleasehook(v,1,sq_lpsolve_releasehook);
	return 0;
}

static SQRESULT sq_lpsolve_version(HSQUIRRELVM v)
{
    int majorversion, minorversion, release, build;
    dllp_solve_version(&majorversion, &minorversion, &release, &build);
	sq_pushfstring(v,_SC("lp_solve %d.%d.%d.%d"), majorversion, minorversion, release, build);
	return 1;
}

static SQRESULT sq_lpsolve_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, libname);
    sq_pushbool(v, load_library(libname));
	return 1;
}

static SQRESULT sq_lpsolve_set_verbose(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, mode);
	//dlset_verbose(self, mode);
	self->set_verbose(self, mode);
	return 0;
}

static SQRESULT sq_lpsolve_set_obj_fn(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    const SQInteger ary_idx = 2;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        sq_pushbool(v, self->set_obj_fn(self, (REAL*)ary));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_add_constraint(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    const SQInteger ary_idx = 2;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        SQ_GET_INTEGER(v, 3, constr_type);
        SQ_GET_FLOAT(v, 4, rh);
        //for(SQInteger i= 0, len =sq_getsize(v, ary_idx); i < len; ++i) printf("add_constraint %d %f\n", (int) i, ((REAL*)ary)[i]);
        sq_pushbool(v, self->add_constraint(self, (REAL*)ary, constr_type, rh));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_add_constraintex(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, asize);
    const SQInteger ary_row_idx = 3;
    const SQInteger ary_col_idx = 4;
    if(sq_gettype(v, ary_row_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_row_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array for rows expected"));
        if(sq_arraygettype(v, ary_col_idx, &atype) != SQ_OK && atype == eat_SQInt32Array) return sq_throwerror(v, _SC("SQInt32Array for cols expected"));
        void *ary_row, *ary_col;
        sq_arraygetrawdata(v, ary_row_idx, &ary_row);
        sq_arraygetrawdata(v, ary_col_idx, &ary_col);
        SQ_GET_INTEGER(v, 5, constr_type);
        SQ_GET_FLOAT(v, 6, rh);
        //for(SQInteger i= 0, len =sq_getsize(v, ary_idx); i < len; ++i) printf("add_constraint %d %f\n", (int) i, ((REAL*)ary)[i]);
        sq_pushbool(v, self->add_constraintex(self, asize, (REAL*)ary_row, (int*)ary_col, constr_type, rh));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_set_lowbo(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, colnr);
	SQ_GET_FLOAT(v, 3, val);
	sq_pushbool(v, self->set_lowbo(self, colnr, val));
	return 1;
}

static SQRESULT sq_lpsolve_set_upbo(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, rownr);
	SQ_GET_FLOAT(v, 3, val);
	sq_pushbool(v, self->set_upbo(self, rownr, val));
	return 1;
}

static SQRESULT sq_lpsolve_set_bounds(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, colno)
    SQ_GET_FLOAT(v, 3, lb);
    SQ_GET_FLOAT(v, 4, ub);
    sq_pushbool(v, self->set_bounds(self, colno, lb, ub));
	return 1;
}

static SQRESULT sq_lpsolve_set_col_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, colnr);
	SQ_GET_STRING(v, 3, name);
	sq_pushbool(v, self->set_col_name(self, colnr, (SQChar*)name));
	return 1;
}

static SQRESULT sq_lpsolve_get_col_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, colnr);
	sq_pushstring(v, self->get_col_name(self, colnr), -1);
	return 1;
}

static SQRESULT sq_lpsolve_set_row_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, rownr);
	SQ_GET_STRING(v, 3, name);
	sq_pushbool(v, self->set_row_name(self, rownr, (SQChar*)name));
	return 1;
}

static SQRESULT sq_lpsolve_get_row_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, rownr);
	sq_pushstring(v, self->get_row_name(self, rownr), -1);
	return 1;
}

static SQRESULT sq_lpsolve_set_lp_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_STRING(v, 2, name);
	sq_pushbool(v, self->set_lp_name(self, (SQChar*)name));
	return 1;
}

static SQRESULT sq_lpsolve_get_lp_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	sq_pushstring(v, self->get_lp_name(self), -1);
	return 1;
}

static SQRESULT sq_lpsolve_write_lp(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_STRING(v, 2, filename);
	sq_pushbool(v, self->write_lp(self, (SQChar*)filename));
	return 1;
}

static SQRESULT sq_lpsolve_write_mps(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_STRING(v, 2, filename);
	sq_pushbool(v, self->write_mps(self, (SQChar*)filename));
	return 1;
}

static SQRESULT sq_lpsolve_write_freemps(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_STRING(v, 2, filename);
	sq_pushbool(v, self->write_freemps(self, (SQChar*)filename));
	return 1;
}

static SQRESULT sq_lpsolve_write_params(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_STRING(v, 2, filename);
	SQ_GET_STRING(v, 3, options);
	sq_pushbool(v, self->write_params(self, (SQChar*)filename, (SQChar*)options));
	return 1;
}

static SQRESULT sq_lpsolve_read_problem(HSQUIRRELVM v, lprec *self)
{
	if(self)
    {
        sq_pushstring(v, LPSOLVE_Tag, -1);
        if(sq_getonroottable(v) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, self);
                sq_setreleasehook(v,-1,sq_lpsolve_releasehook);
            }
        }
    }
    else sq_pushnull(v);
	return 1;
}

static SQRESULT sq_lpsolve_read_LP(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, filename);
	SQ_GET_BOOL(v, 3, verbose);
	SQ_GET_STRING(v, 4, lp_name);

	lprec *self = dlread_LP((SQChar*)filename, verbose, (SQChar*)lp_name);
	return sq_lpsolve_read_problem(v, self);
}

static SQRESULT sq_lpsolve_read_MPS(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, filename);
	SQ_GET_INTEGER(v, 3, options);

	lprec *self = dlread_MPS((SQChar*)filename, options);
	return sq_lpsolve_read_problem(v, self);
}

static SQRESULT sq_lpsolve_read_freeMPS(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, filename);
	SQ_GET_INTEGER(v, 3, options);

	lprec *self = dlread_freeMPS((SQChar*)filename, options);
	return sq_lpsolve_read_problem(v, self);
}

static SQRESULT sq_lpsolve_read_params(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_STRING(v, 2, filename);
	SQ_GET_STRING(v, 3, options);
    sq_pushbool(v, self->read_params(self, (SQChar*)filename, (SQChar*)options));
    return 1;
}

static SQRESULT sq_lpsolve_get_mat(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, rownr);
	SQ_GET_INTEGER(v, 3, colnr);
	sq_pushfloat(v, self->get_mat(self, rownr, colnr));
	return 1;
}

static SQRESULT sq_lpsolve_set_mat(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, rownr);
	SQ_GET_INTEGER(v, 3, colnr);
	SQ_GET_FLOAT(v, 4, value);
	sq_pushbool(v, self->set_mat(self, rownr, colnr, value));
	return 1;
}

static SQRESULT sq_lpsolve_solve(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	sq_pushinteger(v, self->solve(self));
	return 1;
}

static SQRESULT sq_lpsolve_get_objective(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	sq_pushfloat(v, self->get_objective(self));
	return 1;
}

static SQRESULT sq_lpsolve_get_variables(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    const SQInteger ary_idx = 2;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        int  ncols = self->get_Ncolumns(self);
        if(sq_getsize(v, ary_idx) != ncols) return sq_throwerror(v, _SC("SQFloat64Array of size %d expected"), ncols);
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        sq_pushbool(v, self->get_variables(self, (REAL*)ary));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_get_constraints(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    const SQInteger ary_idx = 2;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        int  nrows = self->get_Nrows(self);
        if(sq_getsize(v, ary_idx) != nrows) return sq_throwerror(v, _SC("SQFloat64Array of size %d expected"), nrows);
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        sq_pushbool(v, self->get_constraints(self, (REAL*)ary));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_get_dual_solution(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    const SQInteger ary_idx = 2;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        int  nrows = self->get_Nrows(self);
        if(sq_getsize(v, ary_idx) != nrows) return sq_throwerror(v, _SC("SQFloat64Array of size %d expected"), nrows);
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        sq_pushbool(v, self->get_dual_solution(self, (REAL*)ary));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_get_sensitivity_rhs(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    const SQInteger ary_idx_duals = 2;
    const SQInteger ary_idx_dualsfrom = 3;
    const SQInteger ary_idx_dualstill = 4;
    SQInteger atype;
    if(sq_arraygettype(v, ary_idx_duals, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
    if(sq_arraygettype(v, ary_idx_dualsfrom, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
    if(sq_arraygettype(v, ary_idx_dualstill, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
    int  nrows = self->get_Nrows(self);
    if(sq_getsize(v, ary_idx_duals) != nrows || sq_getsize(v, ary_idx_dualsfrom) != nrows || sq_getsize(v, ary_idx_dualstill) != nrows)
        return sq_throwerror(v, _SC("SQFloat64Array of size %d expected"), nrows);
    void *ary_duals, *ary_dualsfrom, *ary_dualstill;
    sq_arraygetrawdata(v, ary_idx_duals, &ary_duals);
    sq_arraygetrawdata(v, ary_idx_dualsfrom, &ary_dualsfrom);
    sq_arraygetrawdata(v, ary_idx_dualstill, &ary_dualstill);
    sq_pushbool(v, self->get_sensitivity_rhs(self, (REAL*)ary_duals, (REAL*)ary_dualsfrom, (REAL*)ary_dualstill));
	return 1;
}

static SQRESULT sq_lpsolve_get_nonzeros(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	sq_pushinteger(v, self->get_nonzeros(self));
	return 1;
}

static SQRESULT sq_lpsolve_get_Nrows(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	sq_pushinteger(v, self->get_Nrows(self));
	return 1;
}

static SQRESULT sq_lpsolve_get_Ncolumns(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	sq_pushinteger(v, self->get_Ncolumns(self));
	return 1;
}

static SQRESULT sq_lpsolve_get_timeout(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	sq_pushinteger(v, self->get_timeout(self));
	return 1;
}

static SQRESULT sq_lpsolve_set_timeout(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	SQ_GET_INTEGER(v, 2, sectimeout);
	self->set_timeout(self, sectimeout);
	return 0;
}

static SQRESULT sq_lpsolve_print_lp(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	self->print_lp(self);
	return 0;
}

static SQRESULT sq_lpsolve_print_objective(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	self->print_objective(self);
	return 0;
}

static SQRESULT sq_lpsolve_print_solution(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, ncols);
	self->print_solution(self, ncols);
	return 0;
}

static SQRESULT sq_lpsolve_print_constraints(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, ncols);
	self->print_constraints(self, ncols);
	return 0;
}

static SQRESULT sq_lpsolve_print_duals(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	self->print_duals(self);
	return 0;
}

static SQRESULT sq_lpsolve_set_maxim(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	self->set_maxim(self);
	return 0;
}

static SQRESULT sq_lpsolve_set_rh(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, col);
    SQ_GET_FLOAT(v, 3, row);
	sq_pushbool(v, self->set_rh(self, col, row));
	return 1;
}

static SQRESULT sq_lpsolve_set_rh_range(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, col);
    SQ_GET_FLOAT(v, 3, row);
	sq_pushbool(v, self->set_rh_range(self, col, row));
	return 1;
}

static SQRESULT sq_lpsolve_set_int(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, col);
    SQ_GET_BOOL(v, 3, bval);
	sq_pushbool(v, self->set_int(self, col, bval));
	return 1;
}

static SQRESULT sq_lpsolve_set_trace(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_BOOL(v, 2, bval);
	self->set_trace(self, bval);
	return 0;
}

static SQRESULT sq_lpsolve_set_debug(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_BOOL(v, 2, bval);
	self->set_debug(self, bval);
	return 0;
}

static SQRESULT sq_lpsolve_del_constraint(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, cnum);
	sq_pushbool(v, self->del_constraint(self, cnum));
	return 1;
}

static SQRESULT sq_lpsolve_add_column(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    const SQInteger ary_idx = 2;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        //for(SQInteger i= 0, len =sq_getsize(v, ary_idx); i < len; ++i) printf("add_constraint %d %f\n", (int) i, ((REAL*)ary)[i]);
        sq_pushbool(v, self->add_column(self, (REAL*)ary));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_add_columnex(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, asize);
    const SQInteger ary_row_idx = 3;
    const SQInteger ary_col_idx = 4;
    if(sq_gettype(v, ary_row_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_row_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array for rows expected"));
        if(sq_arraygettype(v, ary_col_idx, &atype) != SQ_OK && atype == eat_SQInt32Array) return sq_throwerror(v, _SC("SQInt32Array for cols expected"));
        void *ary_row, *ary_col;
        sq_arraygetrawdata(v, ary_row_idx, &ary_row);
        sq_arraygetrawdata(v, ary_col_idx, &ary_col);
        //for(SQInteger i= 0, len =sq_getsize(v, ary_idx); i < len; ++i) printf("add_constraint %d %f\n", (int) i, ((REAL*)ary)[i]);
        sq_pushbool(v, self->add_columnex(self, asize, (REAL*)ary_row, (int*)ary_col));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_set_column(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, colno);
    const SQInteger ary_idx = 3;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        //for(SQInteger i= 0, len =sq_getsize(v, ary_idx); i < len; ++i) printf("add_constraint %d %f\n", (int) i, ((REAL*)ary)[i]);
        sq_pushbool(v, self->set_column(self, colno, (REAL*)ary));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_set_columnex(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, colno);
    SQ_GET_INTEGER(v, 3, asize);
    const SQInteger ary_row_idx = 4;
    const SQInteger ary_col_idx = 5;
    if(sq_gettype(v, ary_row_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_row_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array for rows expected"));
        if(sq_arraygettype(v, ary_col_idx, &atype) != SQ_OK && atype == eat_SQInt32Array) return sq_throwerror(v, _SC("SQInt32Array for cols expected"));
        void *ary_row, *ary_col;
        sq_arraygetrawdata(v, ary_row_idx, &ary_row);
        sq_arraygetrawdata(v, ary_col_idx, &ary_col);
        //for(SQInteger i= 0, len =sq_getsize(v, ary_idx); i < len; ++i) printf("add_constraint %d %f\n", (int) i, ((REAL*)ary)[i]);
        sq_pushbool(v, self->set_columnex(self, colno, asize, (REAL*)ary_row, (int*)ary_col));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_del_column(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, colno);
    sq_pushbool(v, self->del_column(self, colno));
	return 1;
}

static SQRESULT sq_lpsolve_set_scaling(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, scal_type);
	self->set_scaling(self, scal_type);
	return 0;
}

static SQRESULT sq_lpsolve_unscale(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	self->unscale(self);
	return 0;
}

static SQRESULT sq_lpsolve_default_basis(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
	self->default_basis(self);
	return 0;
}

static SQRESULT sq_lpsolve_is_presolve(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, flags);
	sq_pushbool(v, self->is_presolve(self, flags));
	return 1;
}

static SQRESULT sq_lpsolve_set_presolve(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, flags);
    SQ_GET_INTEGER(v, 3, maxloops);
	self->set_presolve(self, flags, maxloops);
	return 0;
}

static SQRESULT sq_lpsolve_set_constr_type(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, rowno);
    SQ_GET_INTEGER(v, 3, constraint_type);
	sq_pushbool(v, self->set_constr_type(self, rowno, constraint_type));
	return 1;
}

static SQRESULT sq_lpsolve_set_row(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, rowno);
    const SQInteger ary_idx = 3;
    if(sq_gettype(v, ary_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array expected"));
        void *ary;
        sq_arraygetrawdata(v, ary_idx, &ary);
        sq_pushbool(v, self->set_row(self, rowno, (REAL*)ary));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_lpsolve_set_rowex(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_lpsolve_INSTANCE();
    SQ_GET_INTEGER(v, 2, rowno);
    SQ_GET_INTEGER(v, 3, asize);
    const SQInteger ary_row_idx = 4;
    const SQInteger ary_col_idx = 5;
    if(sq_gettype(v, ary_row_idx) == OT_ARRAY)
    {
        SQInteger atype;
        if(sq_arraygettype(v, ary_row_idx, &atype) != SQ_OK && atype == eat_SQFloat64Array) return sq_throwerror(v, _SC("SQFloat64Array for rows expected"));
        if(sq_arraygettype(v, ary_col_idx, &atype) != SQ_OK && atype == eat_SQInt32Array) return sq_throwerror(v, _SC("SQInt32Array for cols expected"));
        void *ary_row, *ary_col;
        sq_arraygetrawdata(v, ary_row_idx, &ary_row);
        sq_arraygetrawdata(v, ary_col_idx, &ary_col);
        sq_pushbool(v, self->set_rowex(self, rowno, asize, (REAL*)ary_row, (int*)ary_col));
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_lpsolve_##name,nparams,tycheck}
static SQRegFunction sq_lpsolve_methods[] =
{
	_DECL_FUNC(constructor,3,_SC(".ii")),
    _DECL_FUNC(version,1,_SC(".")),
    _DECL_FUNC(loadlib,2,_SC(".s")),
    _DECL_FUNC(set_verbose,2,_SC("xi")),
    _DECL_FUNC(set_obj_fn,2,_SC("xa")),
    _DECL_FUNC(add_constraint,4,_SC("xain")),
    _DECL_FUNC(add_constraintex,6,_SC("xiaain")),
    _DECL_FUNC(del_constraint,2,_SC(".i")),
    _DECL_FUNC(set_constr_type,3,_SC(".ii")),
    _DECL_FUNC(set_lowbo,3,_SC("xin")),
    _DECL_FUNC(set_upbo,3,_SC("xin")),
    _DECL_FUNC(set_bounds,4,_SC("xinn")),
    _DECL_FUNC(set_lp_name,2,_SC("xs")),
    _DECL_FUNC(get_lp_name,1,_SC("x")),
    _DECL_FUNC(set_col_name,3,_SC("xis")),
    _DECL_FUNC(get_col_name,2,_SC("xi")),
    _DECL_FUNC(set_row_name,3,_SC("xis")),
    _DECL_FUNC(get_row_name,2,_SC("xi")),
    _DECL_FUNC(write_lp,2,_SC("xs")),
    _DECL_FUNC(write_mps,2,_SC("xs")),
    _DECL_FUNC(write_freemps,2,_SC("xs")),
    _DECL_FUNC(write_params,3,_SC("xss")),
    _DECL_FUNC(read_LP,4,_SC(".sbs")),
    _DECL_FUNC(read_MPS,3,_SC(".si")),
    _DECL_FUNC(read_freeMPS,3,_SC(".si")),
    _DECL_FUNC(read_params,3,_SC("xss")),
    _DECL_FUNC(get_mat,3,_SC("xii")),
    _DECL_FUNC(set_mat,4,_SC("xiin")),
    _DECL_FUNC(solve,1,_SC("x")),
    _DECL_FUNC(get_objective,1,_SC("x")),
    _DECL_FUNC(get_variables,2,_SC("xa")),
    _DECL_FUNC(get_constraints,2,_SC("xa")),
    _DECL_FUNC(get_dual_solution,2,_SC("xa")),
    _DECL_FUNC(get_sensitivity_rhs,4,_SC("xaaa")),
    _DECL_FUNC(get_nonzeros,1,_SC("x")),
    _DECL_FUNC(get_Nrows,1,_SC("x")),
    _DECL_FUNC(get_Ncolumns,1,_SC("x")),
    _DECL_FUNC(set_timeout,2,_SC("xi")),
    _DECL_FUNC(get_timeout,1,_SC("x")),
    _DECL_FUNC(print_lp,1,_SC(".")),
    _DECL_FUNC(print_objective,1,_SC(".")),
    _DECL_FUNC(print_solution,2,_SC(".i")),
    _DECL_FUNC(print_constraints,2,_SC(".i")),
    _DECL_FUNC(print_duals,1,_SC(".")),
    _DECL_FUNC(set_maxim,1,_SC(".")),
    _DECL_FUNC(set_rh,3,_SC(".in")),
    _DECL_FUNC(set_rh_range,3,_SC(".in")),
    _DECL_FUNC(set_int,3,_SC(".ib")),
    _DECL_FUNC(set_debug,2,_SC(".b")),
    _DECL_FUNC(set_trace,2,_SC(".b")),
    _DECL_FUNC(add_column,2,_SC(".a")),
    _DECL_FUNC(add_columnex,4,_SC(".iaa")),
    _DECL_FUNC(set_column,3,_SC(".ia")),
    _DECL_FUNC(set_columnex,5,_SC(".iiaa")),
    _DECL_FUNC(del_column,2,_SC(".i")),
    _DECL_FUNC(set_scaling,2,_SC(".i")),
    _DECL_FUNC(unscale,1,_SC(".")),
    _DECL_FUNC(default_basis,1,_SC(".")),
    _DECL_FUNC(is_presolve,2,_SC(".i")),
    _DECL_FUNC(set_presolve,3,_SC(".ii")),
    _DECL_FUNC(set_row,3,_SC(".ia")),
    _DECL_FUNC(set_rowex,5,_SC(".iiaa")),
    {0,0}
};
#undef _DECL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType lpsolve_constants[] = {
    #define MK_CONST(c) {_SC(#c), c}
    MK_CONST(NEUTRAL),
    MK_CONST(CRITICAL),
    MK_CONST(SEVERE),
    MK_CONST(IMPORTANT),
    MK_CONST(NORMAL),
    MK_CONST(DETAILED),
    MK_CONST(FULL),

    MK_CONST(SCALE_NONE),
    MK_CONST(SCALE_EXTREME),
    MK_CONST(SCALE_RANGE),
    MK_CONST(SCALE_MEAN),
    MK_CONST(SCALE_GEOMETRIC),
    MK_CONST(SCALE_FUTURE1),
    MK_CONST(SCALE_FUTURE2),
    MK_CONST(SCALE_CURTISREID),

    MK_CONST(SCALE_LINEAR),
    MK_CONST(SCALE_QUADRATIC),
    MK_CONST(SCALE_LOGARITHMIC),
    MK_CONST(SCALE_USERWEIGHT),
    MK_CONST(SCALE_MAXTYPE),

    MK_CONST(SCALE_POWER2),
    MK_CONST(SCALE_EQUILIBRATE),
    MK_CONST(SCALE_INTEGERS),
    MK_CONST(SCALE_DYNUPDATE),
    MK_CONST(SCALE_ROWSONLY),
    MK_CONST(SCALE_COLSONLY),

    MK_CONST(SCALEMODEL_EQUILIBRATED),
    MK_CONST(SCALEMODEL_GEOMETRIC),
    MK_CONST(SCALEMODEL_ARITHMETIC),
    MK_CONST(SCALEMODEL_DYNAMIC),
    MK_CONST(SCALEMODEL_CURTISREID),

    /* Presolve defines */
    MK_CONST(PRESOLVE_NONE),
    MK_CONST(PRESOLVE_ROWS),
    MK_CONST(PRESOLVE_COLS),
    MK_CONST(PRESOLVE_LINDEP),
    MK_CONST(PRESOLVE_AGGREGATE),
    MK_CONST(PRESOLVE_SPARSER),
    MK_CONST(PRESOLVE_SOS),
    MK_CONST(PRESOLVE_REDUCEMIP),
    MK_CONST(PRESOLVE_KNAPSACK),
    MK_CONST(PRESOLVE_ELIMEQ2),
    MK_CONST(PRESOLVE_IMPLIEDFREE),
    MK_CONST(PRESOLVE_REDUCEGCD),
    MK_CONST(PRESOLVE_PROBEFIX),
    MK_CONST(PRESOLVE_PROBEREDUCE),
    MK_CONST(PRESOLVE_ROWDOMINATE),
    MK_CONST(PRESOLVE_COLDOMINATE),
    MK_CONST(PRESOLVE_MERGEROWS),
    MK_CONST(PRESOLVE_IMPLIEDSLK),
    MK_CONST(PRESOLVE_COLFIXDUAL),
    MK_CONST(PRESOLVE_BOUNDS),
    MK_CONST(PRESOLVE_LASTMASKMODE),
    MK_CONST(PRESOLVE_DUALS),
    MK_CONST(PRESOLVE_SENSDUALS),

    MK_CONST(FR),
    MK_CONST(LE),
    MK_CONST(GE),
    MK_CONST(EQ),
    MK_CONST(OF),
    {0,0}
};


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_lpsolve(HSQUIRRELVM v)
{
    sq_pushstring(v,LPSOLVE_Tag,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(SQUserPointer)LPSOLVE_Tag);
    sq_insert_reg_funcs(v, sq_lpsolve_methods);

    //add constants
    KeyIntPtrType KeyIntPtr;
    for (KeyIntPtr = lpsolve_constants; KeyIntPtr->Str; KeyIntPtr++) {
        sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
        sq_pushinteger(v, KeyIntPtr->Val);       //then the value
        sq_newslot(v, -3, SQTrue);              //store then
    }

    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif //SQ_USE_LPSOLVE



