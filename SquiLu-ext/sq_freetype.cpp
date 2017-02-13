#ifdef SQ_USE_FREETYPE

#include <string.h>
#include <stdio.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "dynamic_library.h"

/*SquiLu

local library_functions = [
    ["FT_Error", "FT_Init_FreeType", "FT_Library *library"],
    ["FT_Error", "FT_New_Face", "FT_Library library, const char*  filepathname, FT_Long face_index, FT_Face *aface"],
    ["FT_Error", "FT_New_Memory_Face", "FT_Library library, const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face *aface"],
    ["FT_Error", "FT_Done_Face", "FT_Face face"],
    ["FT_Error", "FT_Set_Char_Size", "FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution"],
    ["FT_Error", "FT_Load_Glyph", "FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags"],
    ["FT_Error", "FT_Load_Char", "FT_Face face, FT_ULong  char_code, FT_Int32 load_flags"],
    ["FT_Error", "FT_Get_Glyph_Name", "FT_Face face, FT_UInt glyph_index, FT_Pointer buffer, FT_UInt buffer_max"],
    ["const char*", "FT_Get_Postscript_Name", "FT_Face face"],
    ["FT_Error", "FT_Select_Charmap", "FT_Face face, FT_Encoding  encoding"],
    ["FT_Error", "FT_Set_Charmap", "FT_Face face, FT_CharMap  charmap"],
    ["FT_Int", "FT_Get_Charmap_Index", "FT_CharMap  charmap"],
    ["FT_UInt", "FT_Get_Char_Index", "FT_Face face, FT_ULong  charcode"],
    ["FT_ULong", "FT_Get_First_Char", "FT_Face face, FT_UInt  *agindex"],
    ["FT_ULong", "FT_Get_Next_Char", "FT_Face face, FT_ULong   char_code, FT_UInt  *agindex"],
    ["FT_UInt", "FT_Get_Name_Index", "FT_Face face, FT_String*  glyph_name"],

    //next entry should be the last one
    //to make valid the test made on load_library function
    ["FT_Error", "FT_Done_FreeType", "FT_Library library"],
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

static DynamicLibrary libdyn;

//@write_library_functions_declaration();
// generated-code:begin
typedef FT_Error (*FT_Init_FreeType_t)(FT_Library *library);
static FT_Init_FreeType_t dlFT_Init_FreeType = 0;
typedef FT_Error (*FT_New_Face_t)(FT_Library library, const char*  filepathname, FT_Long face_index, FT_Face *aface);
static FT_New_Face_t dlFT_New_Face = 0;
typedef FT_Error (*FT_New_Memory_Face_t)(FT_Library library, const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face *aface);
static FT_New_Memory_Face_t dlFT_New_Memory_Face = 0;
typedef FT_Error (*FT_Done_Face_t)(FT_Face face);
static FT_Done_Face_t dlFT_Done_Face = 0;
typedef FT_Error (*FT_Set_Char_Size_t)(FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution);
static FT_Set_Char_Size_t dlFT_Set_Char_Size = 0;
typedef FT_Error (*FT_Load_Glyph_t)(FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags);
static FT_Load_Glyph_t dlFT_Load_Glyph = 0;
typedef FT_Error (*FT_Load_Char_t)(FT_Face face, FT_ULong  char_code, FT_Int32 load_flags);
static FT_Load_Char_t dlFT_Load_Char = 0;
typedef FT_Error (*FT_Get_Glyph_Name_t)(FT_Face face, FT_UInt glyph_index, FT_Pointer buffer, FT_UInt buffer_max);
static FT_Get_Glyph_Name_t dlFT_Get_Glyph_Name = 0;
typedef const char* (*FT_Get_Postscript_Name_t)(FT_Face face);
static FT_Get_Postscript_Name_t dlFT_Get_Postscript_Name = 0;
typedef FT_Error (*FT_Select_Charmap_t)(FT_Face face, FT_Encoding  encoding);
static FT_Select_Charmap_t dlFT_Select_Charmap = 0;
typedef FT_Error (*FT_Set_Charmap_t)(FT_Face face, FT_CharMap  charmap);
static FT_Set_Charmap_t dlFT_Set_Charmap = 0;
typedef FT_Int (*FT_Get_Charmap_Index_t)(FT_CharMap  charmap);
static FT_Get_Charmap_Index_t dlFT_Get_Charmap_Index = 0;
typedef FT_UInt (*FT_Get_Char_Index_t)(FT_Face face, FT_ULong  charcode);
static FT_Get_Char_Index_t dlFT_Get_Char_Index = 0;
typedef FT_ULong (*FT_Get_First_Char_t)(FT_Face face, FT_UInt  *agindex);
static FT_Get_First_Char_t dlFT_Get_First_Char = 0;
typedef FT_ULong (*FT_Get_Next_Char_t)(FT_Face face, FT_ULong   char_code, FT_UInt  *agindex);
static FT_Get_Next_Char_t dlFT_Get_Next_Char = 0;
typedef FT_UInt (*FT_Get_Name_Index_t)(FT_Face face, FT_String*  glyph_name);
static FT_Get_Name_Index_t dlFT_Get_Name_Index = 0;
typedef FT_Error (*FT_Done_FreeType_t)(FT_Library library);
static FT_Done_FreeType_t dlFT_Done_FreeType = 0;
// generated-code:end

static const char *dynamicLibName = DYNLIB_FOR_OS(libfreetype);

static bool load_library(const char *libname)
{
    if(dlFT_Done_FreeType) return true;
    if(libdyn.open(libname))
    {
        //@write_library_functions_load();
// generated-code:begin
dlFT_Init_FreeType = (FT_Init_FreeType_t) libdyn.dlsym("FT_Init_FreeType");
if(!dlFT_Init_FreeType) return false;
dlFT_New_Face = (FT_New_Face_t) libdyn.dlsym("FT_New_Face");
if(!dlFT_New_Face) return false;
dlFT_New_Memory_Face = (FT_New_Memory_Face_t) libdyn.dlsym("FT_New_Memory_Face");
if(!dlFT_New_Memory_Face) return false;
dlFT_Done_Face = (FT_Done_Face_t) libdyn.dlsym("FT_Done_Face");
if(!dlFT_Done_Face) return false;
dlFT_Set_Char_Size = (FT_Set_Char_Size_t) libdyn.dlsym("FT_Set_Char_Size");
if(!dlFT_Set_Char_Size) return false;
dlFT_Load_Glyph = (FT_Load_Glyph_t) libdyn.dlsym("FT_Load_Glyph");
if(!dlFT_Load_Glyph) return false;
dlFT_Load_Char = (FT_Load_Char_t) libdyn.dlsym("FT_Load_Char");
if(!dlFT_Load_Char) return false;
dlFT_Get_Glyph_Name = (FT_Get_Glyph_Name_t) libdyn.dlsym("FT_Get_Glyph_Name");
if(!dlFT_Get_Glyph_Name) return false;
dlFT_Get_Postscript_Name = (FT_Get_Postscript_Name_t) libdyn.dlsym("FT_Get_Postscript_Name");
if(!dlFT_Get_Postscript_Name) return false;
dlFT_Select_Charmap = (FT_Select_Charmap_t) libdyn.dlsym("FT_Select_Charmap");
if(!dlFT_Select_Charmap) return false;
dlFT_Set_Charmap = (FT_Set_Charmap_t) libdyn.dlsym("FT_Set_Charmap");
if(!dlFT_Set_Charmap) return false;
dlFT_Get_Charmap_Index = (FT_Get_Charmap_Index_t) libdyn.dlsym("FT_Get_Charmap_Index");
if(!dlFT_Get_Charmap_Index) return false;
dlFT_Get_Char_Index = (FT_Get_Char_Index_t) libdyn.dlsym("FT_Get_Char_Index");
if(!dlFT_Get_Char_Index) return false;
dlFT_Get_First_Char = (FT_Get_First_Char_t) libdyn.dlsym("FT_Get_First_Char");
if(!dlFT_Get_First_Char) return false;
dlFT_Get_Next_Char = (FT_Get_Next_Char_t) libdyn.dlsym("FT_Get_Next_Char");
if(!dlFT_Get_Next_Char) return false;
dlFT_Get_Name_Index = (FT_Get_Name_Index_t) libdyn.dlsym("FT_Get_Name_Index");
if(!dlFT_Get_Name_Index) return false;
dlFT_Done_FreeType = (FT_Done_FreeType_t) libdyn.dlsym("FT_Done_FreeType");
if(!dlFT_Done_FreeType) return false;
// generated-code:end
        return true;
    }
    return false;
}

#undef KLASS_VAR
#define KLASS_VAR(Klass, Var) Klass Var

static const SQChar Freetype_Tag[]   = _SC("sqFreetype");
#define GET_freetype_INSTANCE_AT(idx, self) SQ_GET_INSTANCE2(v, idx, self, FT_Library, Freetype_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("sqfreetype object already closed"));
#define GET_freetype_INSTANCE() GET_freetype_INSTANCE_AT(1, self)


static SQRESULT sq_freetype_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	FT_Library self = ((FT_Library)p);
	if(self)
    {
        dlFT_Done_FreeType(self);
    }
	return 1;
}

static SQRESULT sq_freetype_constructor(HSQUIRRELVM v)
{
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libfreetype !"));
    FT_Library library;
	FT_Error error = dlFT_Init_FreeType( &library );
	if(error)
    {
        return sq_throwerror(v,_SC("Failed to initialize FreeType library"));
    }

	sq_setinstanceup(v,1,library);
	sq_setreleasehook(v,1,sq_freetype_releasehook);
	return 0;
}

static SQRESULT sq_freetype_loadlib(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, libname);
    sq_pushbool(v, load_library(libname));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_freetype_##name,nparams,tycheck}
static SQRegFunction sq_freetype_methods[] =
{
	_DECL_FUNC(constructor,1,_SC(".")),
    _DECL_FUNC(loadlib,2,_SC(".s")),
    {0,0}
};
#undef _DECL_FUNC

static const SQChar FreetypeFace_Tag[]   = _SC("sqFreetypeFace");
#define GET_freetype_face_INSTANCE_AT(idx, self) SQ_GET_INSTANCE2(v, idx, self, FT_Face, FreetypeFace_Tag) \
	if(self == NULL) return sq_throwerror(v, _SC("sqFreetypeFace object already closed"));
#define GET_freetype_face_INSTANCE() GET_freetype_face_INSTANCE_AT(1, self)

static SQRESULT sq_freetype_face_releasehook(SQUserPointer p, SQInteger /*size*/, void */*ep*/)
{
	FT_Face self = ((FT_Face)p);
	if(self)
    {
        dlFT_Done_Face(self);
    }
	return 1;
}

static SQRESULT sq_freetype_face_constructor(HSQUIRRELVM v)
{
    if(!load_library(dynamicLibName)) return sq_throwerror(v, _SC("Failed to load libpcre !"));
	SQ_FUNC_VARS(v);
	GET_freetype_INSTANCE_AT(2, library);
	SQ_GET_STRING(v, 3, fname);
	SQ_OPT_INTEGER(v, 4, face_index, 0);
	SQ_OPT_BOOL(v, 5, isMemory, SQFalse);

	FT_Face face;
	FT_Error error;
	if(isMemory) error = dlFT_New_Memory_Face(library, (const FT_Byte*)fname, fname_size, face_index, &face);
	else error = dlFT_New_Face(library, (const char*)fname, face_index, &face);

	if(error)
    {
        return sq_throwerror(v,_SC("Could not create freetype face"));
    }

	sq_setinstanceup(v,1,face);
	sq_setreleasehook(v,1,sq_freetype_face_releasehook);
	return 0;
}

static SQRESULT sq_freetype_face_set_char_size(HSQUIRRELVM v)
{
	SQ_FUNC_VARS(v);
    GET_freetype_face_INSTANCE();
    SQ_GET_INTEGER(v, 2, char_width);
    SQ_GET_INTEGER(v, 3, char_height);
    SQ_OPT_INTEGER(v, 4, horz_resolution, 0);
    SQ_OPT_INTEGER(v, 5, vert_resolution, 0);
    sq_pushinteger(v, dlFT_Set_Char_Size(self, char_width, char_height, horz_resolution, vert_resolution));
	return 1;
}

static SQRESULT sq_freetype_face_load_char_or_glyph(HSQUIRRELVM v, bool onlyGlyph, bool asArray)
{
	SQ_FUNC_VARS(v);
    GET_freetype_face_INSTANCE();
    SQ_GET_INTEGER(v, 2, char_idx);
    SQ_OPT_INTEGER(v, 3, load_flags, FT_LOAD_RENDER);
    FT_GlyphSlot slot = self->glyph;
    FT_Error error;
    if(onlyGlyph) error = dlFT_Load_Char(self, char_idx, load_flags);
    else error = dlFT_Load_Char(self, char_idx, load_flags);
    if(error) sq_pushinteger(v, error);
    else
    {
        if(asArray)
        {
            sq_newarray(v, 8);
            sq_pushstring(v, (const SQChar*)slot->bitmap.buffer,
                           slot->bitmap.width * slot->bitmap.rows * sizeof (unsigned char));
            sq_arrayset(v, -2, 0);
            sq_pushinteger(v, slot->bitmap.width);
            sq_arrayset(v, -2, 1);
            sq_pushinteger(v, slot->bitmap.rows);
            sq_arrayset(v, -2, 2);
            sq_pushinteger(v, slot->metrics.horiBearingX);
            sq_arrayset(v, -2, 3);
            sq_pushinteger(v, slot->metrics.horiBearingY);
            sq_arrayset(v, -2, 4);
            sq_pushinteger(v, slot->metrics.horiAdvance);
            sq_arrayset(v, -2, 5);
            sq_pushinteger(v, slot->metrics.width);
            sq_arrayset(v, -2, 6);
            sq_pushinteger(v, slot->metrics.height);
            sq_arrayset(v, -2, 7);
        }
        else sq_pushstring(v, (const SQChar*)slot->bitmap.buffer,
                       slot->bitmap.width * slot->bitmap.rows * sizeof (unsigned char));
    }
	return 1;
}

static SQRESULT sq_freetype_face_load_char(HSQUIRRELVM v)
{
	return sq_freetype_face_load_char_or_glyph(v, false, false);
}

static SQRESULT sq_freetype_face_load_char2(HSQUIRRELVM v)
{
	return sq_freetype_face_load_char_or_glyph(v, false, true);
}

static SQRESULT sq_freetype_face_load_glyph(HSQUIRRELVM v)
{
	return sq_freetype_face_load_char_or_glyph(v, true, false);
}

static SQRESULT sq_freetype_face_load_glyph2(HSQUIRRELVM v)
{
	return sq_freetype_face_load_char_or_glyph(v, true, true);
}

static SQRESULT sq_freetype_face_char_index(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    SQ_GET_INTEGER(v, 2, glyph_idx);
    sq_pushinteger(v, dlFT_Get_Char_Index(self, glyph_idx));
	return 1;
}

static SQRESULT sq_freetype_face_first_char(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    FT_UInt agindex;
    FT_ULong charcode = dlFT_Get_First_Char(self, &agindex);
    sq_newarray(v, 2);
    sq_pushinteger(v, charcode);
    sq_arrayset(v, -2, 0);
    sq_pushinteger(v, agindex);
    sq_arrayset(v, -2, 1);
	return 1;
}

static SQRESULT sq_freetype_face_next_char(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    SQ_GET_INTEGER(v, 2, char_idx);
    FT_UInt agindex;
    FT_ULong charcode = dlFT_Get_Next_Char(self, char_idx, &agindex);
    sq_newarray(v, 2);
    sq_pushinteger(v, charcode);
    sq_arrayset(v, -2, 0);
    sq_pushinteger(v, agindex);
    sq_arrayset(v, -2, 1);
	return 1;
}

static SQRESULT sq_freetype_face_name_index(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    SQ_GET_STRING(v, 2, char_name);
    FT_UInt gindex = dlFT_Get_Name_Index(self, (FT_String*)char_name);
    sq_pushinteger(v, gindex);
	return 1;
}

static SQRESULT sq_freetype_face_glyph_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    if(FT_HAS_GLYPH_NAMES(self))
    {
        SQ_GET_INTEGER(v, 2, glyph_idx);
        SQInteger buf_size = 254;
        SQChar *buf = sq_getscratchpad(v, buf_size);
        FT_Error error = dlFT_Get_Glyph_Name(self, glyph_idx, buf, buf_size);
        if(error) sq_pushinteger(v, error);
        else sq_pushstring(v, buf, buf_size);
    }
    else sq_pushnull(v);
	return 1;
}

static SQRESULT sq_freetype_face_postscript_name(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    const char *pname = dlFT_Get_Postscript_Name(self);
    if(pname) sq_pushstring(v, pname, -1);
    else sq_pushnull(v);
	return 1;
}

static SQRESULT sq_freetype_face_get_num_glyphs(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    sq_pushinteger(v, self->num_glyphs);
	return 1;
}

static SQRESULT sq_freetype_face_metrics(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP(v);
    GET_freetype_face_INSTANCE();
    sq_newtableex(v, 3);
    sq_pushliteral(v, _SC("ascender"));
    sq_pushinteger(v, self->ascender);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("descender"));
    sq_pushinteger(v, self->descender);
    sq_rawset(v, -3);
    sq_pushliteral(v, _SC("height"));
    sq_pushinteger(v, self->height);
    sq_rawset(v, -3);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_freetype_face_##name,nparams,tycheck}
static SQRegFunction sq_freetype_face_methods[] =
{
	_DECL_FUNC(constructor,-3,_SC(".xsnb")),
    _DECL_FUNC(set_char_size,-3,_SC(".iiii")),
    _DECL_FUNC(get_num_glyphs,1,_SC("x")),
    _DECL_FUNC(load_char,-2,_SC("xi")),
    _DECL_FUNC(load_char2,-2,_SC("xi")),
    _DECL_FUNC(load_glyph,-2,_SC("xi")),
    _DECL_FUNC(load_glyph2,-2,_SC("xi")),
    _DECL_FUNC(first_char,1,_SC("x")),
    _DECL_FUNC(next_char,2,_SC("xi")),
    _DECL_FUNC(char_index,2,_SC("xi")),
    _DECL_FUNC(name_index,2,_SC("xs")),
    _DECL_FUNC(glyph_name,2,_SC("xi")),
    _DECL_FUNC(postscript_name,1,_SC("x")),
    _DECL_FUNC(metrics,1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

typedef struct {
  const SQChar *Str;
  SQInteger Val;
} KeyIntType, * KeyIntPtrType;

static KeyIntType sqfreetype_constants[] = {
    #define MK_CONST(c) {_SC(#c), c}
    #define MK_CONST2(c) {_SC(#c), (SQInteger)c}
    //MK_CONST(SSL_SESSION_ID_SIZE),

	MK_CONST(FT_LOAD_RENDER),
	MK_CONST(FT_LOAD_MONOCHROME),
	MK_CONST(FT_LOAD_TARGET_LIGHT),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_freetype(HSQUIRRELVM v)
{
    sq_pushstring(v, Freetype_Tag,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)Freetype_Tag);
    sq_insert_reg_funcs(v, sq_freetype_methods);

    /*
	//add constants
	KeyIntPtrType KeyIntPtr;
	for (KeyIntPtr = sqfreetype_constants; KeyIntPtr->Str; KeyIntPtr++) {
		sq_pushstring(v, KeyIntPtr->Str, -1);    //first the key
		sq_pushinteger(v, KeyIntPtr->Val);       //then the value
		sq_newslot(v, -3, SQFalse);              //store then
	}
	*/

    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v, FreetypeFace_Tag,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)FreetypeFace_Tag);
    sq_insert_reg_funcs(v, sq_freetype_face_methods);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // SQ_USE_FREETYPE
