#ifdef WITH_LIBCLANG

#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"

#include "dynamic_library.h"

#include <clang-c/Index.h>
#include <clang-c/Platform.h>

/*SquiLu
local dynamic_functions = [
    ["CXIndex", "clang_createIndex",
            @"int excludeDeclarationsFromPCH, int displayDiagnostics"],
    ["void", "clang_disposeIndex", "CXIndex index"],
    ["CXTranslationUnit", "clang_parseTranslationUnit",
            @"CXIndex CIdx,
            const char *source_filename,
            const char *const *command_line_args,
            int num_command_line_args,
            struct CXUnsavedFile *unsaved_files,
            unsigned num_unsaved_files,
            unsigned options"],
    ["CXCursor", "clang_getTranslationUnitCursor", "CXTranslationUnit"],
    ["void", "clang_disposeTranslationUnit", "CXTranslationUnit"],
    ["unsigned", "clang_visitChildren",
            @"CXCursor parent,
            CXCursorVisitor visitor,
            CXClientData client_data"],
    ["enum CXCursorKind", "clang_getCursorKind", "CXCursor"],
    ["CXType", "clang_getCursorType", "CXCursor"],
    ["CXString", "clang_getCursorSpelling", "CXCursor"],
    ["CXString", "clang_getTypeSpelling", "CXType CT"],
    ["CXString", "clang_getCursorKindSpelling", "enum CXCursorKind"],
    ["CXCursor", "clang_getCursorReferenced", "CXCursor"],
    ["const char *", "clang_getCString", "CXString string"],
    ["void", "clang_disposeString", "CXString str"],
    ["CXSourceLocation", "clang_getCursorLocation", "CXCursor"],
    ["void", "clang_getPresumedLocation",
            @"CXSourceLocation location,
            CXString *filename,
            unsigned *line,
            unsigned *column"],
    ["enum CXLinkageKind", "clang_getCursorLinkage", "CXCursor"],
    ["CXType", "clang_getResultType", "CXType"],
];

function write_dynamic_functions_declaration(){
    foreach(k,v in dynamic_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_dynamic_functions_load(){
    foreach(k,v in dynamic_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) dynamicLib.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary dynamicLib;

//@write_dynamic_functions_declaration();
// generated-code:begin
typedef CXIndex (*clang_createIndex_t)(int excludeDeclarationsFromPCH, int displayDiagnostics);
static clang_createIndex_t dlclang_createIndex = 0;
typedef void (*clang_disposeIndex_t)(CXIndex index);
static clang_disposeIndex_t dlclang_disposeIndex = 0;
typedef CXTranslationUnit (*clang_parseTranslationUnit_t)(CXIndex CIdx,
            const char *source_filename,
            const char *const *command_line_args,
            int num_command_line_args,
            struct CXUnsavedFile *unsaved_files,
            unsigned num_unsaved_files,
            unsigned options);
static clang_parseTranslationUnit_t dlclang_parseTranslationUnit = 0;
typedef CXCursor (*clang_getTranslationUnitCursor_t)(CXTranslationUnit);
static clang_getTranslationUnitCursor_t dlclang_getTranslationUnitCursor = 0;
typedef void (*clang_disposeTranslationUnit_t)(CXTranslationUnit);
static clang_disposeTranslationUnit_t dlclang_disposeTranslationUnit = 0;
typedef unsigned (*clang_visitChildren_t)(CXCursor parent,
            CXCursorVisitor visitor,
            CXClientData client_data);
static clang_visitChildren_t dlclang_visitChildren = 0;
typedef enum CXCursorKind (*clang_getCursorKind_t)(CXCursor);
static clang_getCursorKind_t dlclang_getCursorKind = 0;
typedef CXType (*clang_getCursorType_t)(CXCursor);
static clang_getCursorType_t dlclang_getCursorType = 0;
typedef CXString (*clang_getCursorSpelling_t)(CXCursor);
static clang_getCursorSpelling_t dlclang_getCursorSpelling = 0;
typedef CXString (*clang_getTypeSpelling_t)(CXType CT);
static clang_getTypeSpelling_t dlclang_getTypeSpelling = 0;
typedef CXString (*clang_getCursorKindSpelling_t)(enum CXCursorKind);
static clang_getCursorKindSpelling_t dlclang_getCursorKindSpelling = 0;
typedef CXCursor (*clang_getCursorReferenced_t)(CXCursor);
static clang_getCursorReferenced_t dlclang_getCursorReferenced = 0;
typedef const char * (*clang_getCString_t)(CXString string);
static clang_getCString_t dlclang_getCString = 0;
typedef void (*clang_disposeString_t)(CXString str);
static clang_disposeString_t dlclang_disposeString = 0;
typedef CXSourceLocation (*clang_getCursorLocation_t)(CXCursor);
static clang_getCursorLocation_t dlclang_getCursorLocation = 0;
typedef void (*clang_getPresumedLocation_t)(CXSourceLocation location,
            CXString *filename,
            unsigned *line,
            unsigned *column);
static clang_getPresumedLocation_t dlclang_getPresumedLocation = 0;
typedef enum CXLinkageKind (*clang_getCursorLinkage_t)(CXCursor);
static clang_getCursorLinkage_t dlclang_getCursorLinkage = 0;
typedef CXType (*clang_getResultType_t)(CXType);
static clang_getResultType_t dlclang_getResultType = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libclang);

static bool load_dynamicLib(const char *libname)
{
    if(dlclang_createIndex) return true;
    if(dynamicLib.open(libname))
    {
        //@write_dynamic_functions_load();
// generated-code:begin
dlclang_createIndex = (clang_createIndex_t) dynamicLib.dlsym("clang_createIndex");
if(!dlclang_createIndex) return false;
dlclang_disposeIndex = (clang_disposeIndex_t) dynamicLib.dlsym("clang_disposeIndex");
if(!dlclang_disposeIndex) return false;
dlclang_parseTranslationUnit = (clang_parseTranslationUnit_t) dynamicLib.dlsym("clang_parseTranslationUnit");
if(!dlclang_parseTranslationUnit) return false;
dlclang_getTranslationUnitCursor = (clang_getTranslationUnitCursor_t) dynamicLib.dlsym("clang_getTranslationUnitCursor");
if(!dlclang_getTranslationUnitCursor) return false;
dlclang_disposeTranslationUnit = (clang_disposeTranslationUnit_t) dynamicLib.dlsym("clang_disposeTranslationUnit");
if(!dlclang_disposeTranslationUnit) return false;
dlclang_visitChildren = (clang_visitChildren_t) dynamicLib.dlsym("clang_visitChildren");
if(!dlclang_visitChildren) return false;
dlclang_getCursorKind = (clang_getCursorKind_t) dynamicLib.dlsym("clang_getCursorKind");
if(!dlclang_getCursorKind) return false;
dlclang_getCursorType = (clang_getCursorType_t) dynamicLib.dlsym("clang_getCursorType");
if(!dlclang_getCursorType) return false;
dlclang_getCursorSpelling = (clang_getCursorSpelling_t) dynamicLib.dlsym("clang_getCursorSpelling");
if(!dlclang_getCursorSpelling) return false;
dlclang_getTypeSpelling = (clang_getTypeSpelling_t) dynamicLib.dlsym("clang_getTypeSpelling");
if(!dlclang_getTypeSpelling) return false;
dlclang_getCursorKindSpelling = (clang_getCursorKindSpelling_t) dynamicLib.dlsym("clang_getCursorKindSpelling");
if(!dlclang_getCursorKindSpelling) return false;
dlclang_getCursorReferenced = (clang_getCursorReferenced_t) dynamicLib.dlsym("clang_getCursorReferenced");
if(!dlclang_getCursorReferenced) return false;
dlclang_getCString = (clang_getCString_t) dynamicLib.dlsym("clang_getCString");
if(!dlclang_getCString) return false;
dlclang_disposeString = (clang_disposeString_t) dynamicLib.dlsym("clang_disposeString");
if(!dlclang_disposeString) return false;
dlclang_getCursorLocation = (clang_getCursorLocation_t) dynamicLib.dlsym("clang_getCursorLocation");
if(!dlclang_getCursorLocation) return false;
dlclang_getPresumedLocation = (clang_getPresumedLocation_t) dynamicLib.dlsym("clang_getPresumedLocation");
if(!dlclang_getPresumedLocation) return false;
dlclang_getCursorLinkage = (clang_getCursorLinkage_t) dynamicLib.dlsym("clang_getCursorLinkage");
if(!dlclang_getCursorLinkage) return false;
dlclang_getResultType = (clang_getResultType_t) dynamicLib.dlsym("clang_getResultType");
if(!dlclang_getResultType) return false;
// generated-code:end

        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

static const SQChar *LibClang_TAG = _SC("LibClang");

struct MyLibClang {
    CXIndex index;
    int depth;
	const char *file_name;
	char *function_name;
	unsigned int prev_line, prev_column;
	unsigned int line, column;
	HSQUIRRELVM v;
	HSQOBJECT visitor_cb;
	HSQOBJECT visitor_udata;
};

static SQRESULT get_libclang_instance(HSQUIRRELVM v, SQInteger idx, MyLibClang **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)LibClang_TAG,SQFalse)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("libclang is closed"));
	return _rc_;
}

#define GET_libclang_INSTANCE_AT(idx) \
	MyLibClang *self=NULL; \
	if((_rc_ = get_libclang_instance(v,idx,&self)) < 0) return _rc_;

#define GET_libclang_INSTANCE() GET_libclang_INSTANCE_AT(1)

static void release_visitor_cb(MyLibClang *self)
{
    sq_release(self->v, &self->visitor_cb);
    sq_resetobject(&self->visitor_cb);
}

static SQRESULT sq_libclang_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	MyLibClang *self = ((MyLibClang *)p);
	if (self)
    {
        if(self->index)
        {
            dlclang_disposeIndex(self->index);
        }
        release_visitor_cb(self);

        sq_free(self, sizeof(MyLibClang));
    }
	return 0;
}

static SQRESULT sq_libclang_constructor(HSQUIRRELVM v)
{
    //SQ_FUNC_VARS_NO_TOP(v);
    if(!load_dynamicLib(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libclang !"));

    MyLibClang *self = (MyLibClang *)sq_malloc(sizeof(MyLibClang));
    memset(self, 0, sizeof(MyLibClang));
    self->v = v;
    self->index = dlclang_createIndex(0, 0);

    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, sq_libclang_releasehook);

    return 1;
}

static SQRESULT sq_libclang_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_libclang_INSTANCE();
	sq_libclang_releasehook(self, sizeof(MyLibClang), v);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "libclang is closed"
	return 0;
}

static void call_visitor_cb(MyLibClang *self, const SQChar *sig, ...)
{
    HSQUIRRELVM v = self->v;
    int top = sq_gettop(v);

    sq_pushobject(v, self->visitor_cb);
    //sq_pushroottable(v);
    //sq_pushobject(v, self->busy_udata);
    va_list vl;
    va_start(vl, sig);
    /*SQRESULT rc =*/ sq_call_va_vl(v, SQFalse, -1, NULL, 0, sig, vl);
    va_end(vl);

    sq_settop(v, top);
}

static enum CXChildVisitResult
functionDeclVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
	MyLibClang *cvu = (MyLibClang*) client_data;
	enum CXCursorKind kind = dlclang_getCursorKind(cursor);
	CXType type = dlclang_getCursorType(cursor);
	CXString type_spelling;
	//CXString type_kind_spelling;
	CXString name;

	if (kind == CXCursor_ParmDecl){
		name = dlclang_getCursorSpelling(cursor);
		type_spelling = dlclang_getTypeSpelling(type);
		//db_add_funcparam(cvu->db, cvu->function_name, dlclang_getCString(name), dlclang_getCString(type_spelling), type.kind);
		call_visitor_cb(cvu, "ssiisssi",
                        "FuncParam",
                        cvu->file_name,
                        cvu->line,
                        cvu->column,
                        cvu->function_name,
                        dlclang_getCString(name),
                        dlclang_getCString(type_spelling),
                        type.kind);
		dlclang_disposeString(name);
		dlclang_disposeString(type_spelling);
	}

	return CXChildVisit_Continue;
}

static enum CXChildVisitResult
cursorVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
	MyLibClang *cvu = (MyLibClang*) client_data;
	enum CXCursorKind kind = dlclang_getCursorKind(cursor);
	CXString name = dlclang_getCursorSpelling(cursor);
	enum CXChildVisitResult ret = CXChildVisit_Recurse;

	CXString filename;
	unsigned int line, column;

	CXSourceLocation location = dlclang_getCursorLocation(cursor);
	dlclang_getPresumedLocation(location, &filename, &line, &column);
    cvu->file_name = dlclang_getCString(filename);
    cvu->line = line;
    cvu->column = column;
	//sqlite_int64 file_id = db_add_funcfile(cvu->db, dlclang_getCString(filename));
	//call_visitor_cb(cvu, "ss", "FuncFile", dlclang_getCString(filename));
	if (kind == CXCursor_FunctionDecl) {
		enum CXLinkageKind lk = dlclang_getCursorLinkage(cursor);
		CXType type = dlclang_getCursorType(cursor);
		CXString type_spelling = dlclang_getTypeSpelling(type);
		CXType rtype = dlclang_getResultType(type);
		CXString rtype_spelling = dlclang_getTypeSpelling(rtype);
		free(cvu->function_name);
		cvu->function_name = strdup(dlclang_getCString(name));
		cvu->prev_line = line;
		cvu->prev_column = column;
		//db_add_funcdecl(cvu->db, cvu->function_name, dlclang_getCString(type_spelling),
        //          dlclang_getCString(rtype_spelling), lk, file_id, line, column);
        call_visitor_cb(cvu, "ssiisssi",
                        "FuncDecl",
                        cvu->file_name,
                        cvu->line,
                        cvu->column,
                        cvu->function_name,
                        dlclang_getCString(type_spelling),
                        dlclang_getCString(rtype_spelling),
                        lk);
		dlclang_visitChildren(cursor, *functionDeclVisitor, cvu);
		dlclang_disposeString(type_spelling);
		dlclang_disposeString(rtype_spelling);
	} else if (kind == CXCursor_CallExpr) {
		//CXType type = clang_getCursorType(cursor);
		//CXString type_spelling = clang_getTypeSpelling(type);
		//printf("%s\n", type_spelling);
		//db_add_funccall(cvu->db, cvu->function_name, dlclang_getCString(name), file_id, line, column);

		CXCursor to_func_file_cursor = dlclang_getCursorReferenced(cursor);
        CXString to_func_filename;
        unsigned int to_func_line, to_func_column;

        CXSourceLocation to_func_location = dlclang_getCursorLocation(to_func_file_cursor);
        dlclang_getPresumedLocation(to_func_location, &to_func_filename, &to_func_line, &to_func_column);

		call_visitor_cb(cvu, "ssiisiisiis",
                        "CallExpr",
                        cvu->file_name,
                        line,
                        column,
                        cvu->function_name, //from function
                        cvu->prev_line,
                        cvu->prev_column,
                        dlclang_getCString(to_func_filename),
                        to_func_line,
                        to_func_column,
                        dlclang_getCString(name)); //to function

		dlclang_disposeString(to_func_filename);
		ret = CXChildVisit_Continue;
	} else if (kind == CXCursor_VarDecl) {
	}

	dlclang_disposeString(name);
	dlclang_disposeString(filename);
	return ret;
}

static SQRESULT sq_libclang_parseTranslationUnit(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_libclang_INSTANCE();

    if(sq_gettype(v, 2) != OT_CLOSURE)
            return sq_throwerror(v, _SC("invalid fisrt parameter expected closure"));

	SQ_GET_STRING(v, 3, fname);

    release_visitor_cb(self);
	sq_getstackobj(v, 2, &self->visitor_cb);
    sq_addref(v, &self->visitor_cb);

	const char *cl_argsDefault[] = {"-I."};
	const char **cl_args = cl_argsDefault;
	int cl_argNum = 1;
	int rc = 0;
	const int cl_arg_start = 4;
	bool has_extra_params = _top_ >= cl_arg_start;

	if(has_extra_params)
    {
        //create cl_args with extra parameters
        cl_argNum = _top_ - (cl_arg_start -1);
        cl_args = (const char **)sq_malloc(sizeof(char*) * cl_argNum);
        for(int i=cl_arg_start; i <= _top_; ++i)
        {
            const SQChar *p;
            if(sq_gettype(v, i) == OT_STRING)
            {
                rc = sq_getstring(v, i, &p);
            }
            else
            {
                rc = sq_throwerror(v, _SC("not a string parameter at %d"), i);
                goto cleanup;
            }
            cl_args[i-cl_arg_start] = p;
        }
    }

	CXTranslationUnit TU;
	CXCursor rootCursor;

    TU = dlclang_parseTranslationUnit(self->index, fname,
                    cl_args, cl_argNum, 0, 0, CXTranslationUnit_Incomplete);

    if (TU == NULL) {
        rc = sq_throwerror(v, _SC("clang_parseTranslationUnit for %s failed\n"), fname);
        goto cleanup;
    }

    rootCursor = dlclang_getTranslationUnitCursor(TU);
    dlclang_visitChildren(rootCursor, cursorVisitor, self);
    dlclang_disposeTranslationUnit(TU);

cleanup:
	if(has_extra_params)
    {
        sq_free(cl_args, sizeof(char*) * cl_argNum);
    }
	return rc;
}

static SQRESULT sq_libclang_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
	SQ_GET_STRING(v, 2, libname);
	sq_pushbool(v, load_dynamicLib(libname));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_libclang_##name,nparams,tycheck}
static SQRegFunction sq_libclang_methods[] =
{
	_DECL_FUNC(constructor, 1, _SC("x")),
	_DECL_FUNC(close, 1, _SC("x")),
	_DECL_FUNC(parseTranslationUnit, -3, _SC("xcs")),
	_DECL_FUNC(loadlib,2,_SC(".s")),
	{0,0}
};
#undef _DECL_FUNC


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_libclang(HSQUIRRELVM v)
{
    sq_pushstring(v,LibClang_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)LibClang_TAG);
    sq_insert_reg_funcs(v, sq_libclang_methods);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // WITH_LIBCLANG
