//#ifdef WITH_LIBCLANG

#include "squirrel.h"
#include <clang-c/Index.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"

#include "dynamic_library.h"

/*SquiLu
local libclang_functions = [
    ["CXCodeCompleteResults *", "clang_codeCompleteAt", @"CXTranslationUnit TU,
                                            const char *complete_filename,
                                            unsigned complete_line,
                                            unsigned complete_column,
                                            struct CXUnsavedFile *unsaved_files,
                                            unsigned num_unsaved_files,
                                            unsigned options"],
    ["CXDiagnostic", "clang_codeCompleteGetDiagnostic", @"CXCodeCompleteResults *Results,
                                             unsigned Index"],
    ["unsigned", "clang_codeCompleteGetNumDiagnostics", "CXCodeCompleteResults *Results"],
    ["CXIndex", "clang_createIndex", @"int excludeDeclarationsFromPCH,
                                         int displayDiagnostics"],
    ["CXTranslationUnit", "clang_createTranslationUnit", @"CXIndex CIdx,
                                         const char *source_filename,
                                         int num_clang_command_line_args,
                                   const char * const *clang_command_line_args,
                                         unsigned num_unsaved_files,
                                         struct CXUnsavedFile *unsaved_files"],
    ["CXCursor", "clang_Cursor_getArgument", "CXCursor C, unsigned i"],
    ["int", "clang_Cursor_getNumArguments", "CXCursor C"],
    ["int", "clang_Cursor_isNull", "CXCursor cursor"],
    ["unsigned", "clang_CXXMethod_isStatic", "CXCursor C"],
    ["unsigned", "clang_CXXMethod_isVirtual", "CXCursor C"],
    ["unsigned", "clang_defaultCodeCompleteOptions", "void"],
    ["unsigned", "clang_defaultDiagnosticDisplayOptions", "void"],
    ["void", "clang_disposeCodeCompleteResults", "CXCodeCompleteResults *Results"],
    ["void", "clang_disposeDiagnostic", "CXDiagnostic Diagnostic"],
    ["void", "clang_disposeIndex", "CXIndex index"],
    ["void", "clang_disposeString", "CXString *str"],
    ["unsigned", "clang_equalCursors", "CXCursor, CXCursor"],
    ["unsigned", "clang_equalTypes", "CXType A, CXType B"],
    ["CXString", "clang_formatDiagnostic", @"CXDiagnostic Diagnostic,
                                               unsigned Options"],
    ["CXType", "clang_getCanonicalType", "CXType T"],
    ["CXString", "clang_getCompletionAnnotation", @"CXCompletionString completion_string,
                              unsigned annotation_number"],
    ["enum CXCompletionChunkKind", "clang_getCompletionChunkKind", @"CXCompletionString completion_string,
                             unsigned chunk_number"],
    ["CXString", "clang_getCompletionChunkText", @"CXCompletionString completion_string,
                             unsigned chunk_number"],
    ["unsigned", "clang_getCompletionNumAnnotations", "CXCompletionString completion_string"],
    ["unsigned", "clang_getCompletionPriority", "CXCompletionString completion_string"],
    ["void", "clang_getCString", "PGconn *conn, Oid lobjId"],
    ["CXCursor", "clang_getCursorDefinition", "CXCursor"],
    ["void", "clang_getCursorDisplayName", "PGconn *conn, int fd"],
    ["CXSourceRange", "clang_getCursorExtent", "CXCursor"],
    ["CXString", "clang_getCursorKind", "enum CXCursorKind Kind"],
    ["CXCursor", "clang_getCursorReferenced", "CXCursor"],
    ["CXType", "clang_getCursorResultType", "CXCursor C"],
    ["CXCursor", "clang_getCursorSemanticParent", "CXCursor"],
    ["CXString", "clang_getCursorSpelling", "CXCursor"],
    ["CXType", "clang_getCursorType", "CXCursor"],
    ["void", "clang_getCursorUSR", "const char *conninfo"],
    ["enum CX_CXXAccessSpecifier", "clang_getCXXAccessSpecifier", "CXCursor"],
    ["CXString", "clang_getDiagnosticCategoryText", "CXDiagnostic"],
    ["CXDiagnostic", "clang_getDiagnostic", @"CXTranslationUnit Unit,
                                                unsigned Index"],
    ["CXString", "clang_getFileName", "CXFile SFile"],
    ["time_t", "clang_getFileTime", "CXFile SFile"],
    ["unsigned", "clang_getNumCompletionChunks", "CXCompletionString completion_string"],
    ["unsigned", "clang_getNumDiagnostics", "CXTranslationUnit Unit"],
    ["CXType", "clang_getPointeeType", "CXType"],
    ["CXSourceLocation", "clang_getRangeEnd", "CXSourceRange range"],
    ["CXSourceLocation", "clang_getRangeStart", "CXSourceRange range"],
    ["void", "clang_getSpellingLocation", @"CXSourceLocation location,
                                              CXFile *file,
                                              unsigned *line,
                                              unsigned *column,
                                              unsigned *offset"],
    ["CXCursor", "clang_getTranslationUnitCursor", "CXTranslationUnit"],
    ["CXCursor", "clang_getTypeDeclaration", "CXType"],
    ["CXString", "clang_getTypeKindSpelling", "enum CXTypeKind K"],
    ["unsigned", "clang_isPODType", "CXType"],
    ["CXTranslationUnit", "clang_parseTranslationUnit", @"CXIndex CIdx,
                           const char *source_filename,
                           const char *const *command_line_args,
                           int num_command_line_args,
                           struct CXUnsavedFile *unsaved_files,
                           unsigned num_unsaved_files,
                           unsigned options"],
    ["unsigned", "clang_visitChildren", @"CXCursor parent,
                                            CXCursorVisitor visitor,
                                            CXClientData client_data"],

    //next entry should be the last one
    //to make valid the test made on load_libclang function
    ["void", "clang_getCursorExtent", "const char *conninfo"],
];

function write_libclang_functions_declaration(){
    foreach(k,v in libclang_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_libclang_functions_load(){
    foreach(k,v in libclang_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libclang.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary libclang;

//@write_libclang_functions_declaration();
// generated-code:begin
typedef CXCodeCompleteResults * (*clang_codeCompleteAt_t)(CXTranslationUnit TU,
                                            const char *complete_filename,
                                            unsigned complete_line,
                                            unsigned complete_column,
                                            struct CXUnsavedFile *unsaved_files,
                                            unsigned num_unsaved_files,
                                            unsigned options);
static clang_codeCompleteAt_t dlclang_codeCompleteAt = 0;
typedef CXDiagnostic (*clang_codeCompleteGetDiagnostic_t)(CXCodeCompleteResults *Results,
                                             unsigned Index);
static clang_codeCompleteGetDiagnostic_t dlclang_codeCompleteGetDiagnostic = 0;
typedef unsigned (*clang_codeCompleteGetNumDiagnostics_t)(CXCodeCompleteResults *Results);
static clang_codeCompleteGetNumDiagnostics_t dlclang_codeCompleteGetNumDiagnostics = 0;
typedef CXIndex (*clang_createIndex_t)(int excludeDeclarationsFromPCH,
                                         int displayDiagnostics);
static clang_createIndex_t dlclang_createIndex = 0;
typedef CXTranslationUnit (*clang_createTranslationUnit_t)(CXIndex CIdx,
                                         const char *source_filename,
                                         int num_clang_command_line_args,
                                   const char * const *clang_command_line_args,
                                         unsigned num_unsaved_files,
                                         struct CXUnsavedFile *unsaved_files);
static clang_createTranslationUnit_t dlclang_createTranslationUnit = 0;
typedef CXCursor (*clang_Cursor_getArgument_t)(CXCursor C, unsigned i);
static clang_Cursor_getArgument_t dlclang_Cursor_getArgument = 0;
typedef int (*clang_Cursor_getNumArguments_t)(CXCursor C);
static clang_Cursor_getNumArguments_t dlclang_Cursor_getNumArguments = 0;
typedef int (*clang_Cursor_isNull_t)(CXCursor cursor);
static clang_Cursor_isNull_t dlclang_Cursor_isNull = 0;
typedef unsigned (*clang_CXXMethod_isStatic_t)(CXCursor C);
static clang_CXXMethod_isStatic_t dlclang_CXXMethod_isStatic = 0;
typedef unsigned (*clang_CXXMethod_isVirtual_t)(CXCursor C);
static clang_CXXMethod_isVirtual_t dlclang_CXXMethod_isVirtual = 0;
typedef unsigned (*clang_defaultCodeCompleteOptions_t)(void);
static clang_defaultCodeCompleteOptions_t dlclang_defaultCodeCompleteOptions = 0;
typedef unsigned (*clang_defaultDiagnosticDisplayOptions_t)(void);
static clang_defaultDiagnosticDisplayOptions_t dlclang_defaultDiagnosticDisplayOptions = 0;
typedef void (*clang_disposeCodeCompleteResults_t)(CXCodeCompleteResults *Results);
static clang_disposeCodeCompleteResults_t dlclang_disposeCodeCompleteResults = 0;
typedef void (*clang_disposeDiagnostic_t)(CXDiagnostic Diagnostic);
static clang_disposeDiagnostic_t dlclang_disposeDiagnostic = 0;
typedef void (*clang_disposeIndex_t)(CXIndex index);
static clang_disposeIndex_t dlclang_disposeIndex = 0;
typedef void (*clang_disposeString_t)(CXString *str);
static clang_disposeString_t dlclang_disposeString = 0;
typedef unsigned (*clang_equalCursors_t)(CXCursor, CXCursor);
static clang_equalCursors_t dlclang_equalCursors = 0;
typedef unsigned (*clang_equalTypes_t)(CXType A, CXType B);
static clang_equalTypes_t dlclang_equalTypes = 0;
typedef CXString (*clang_formatDiagnostic_t)(CXDiagnostic Diagnostic,
                                               unsigned Options);
static clang_formatDiagnostic_t dlclang_formatDiagnostic = 0;
typedef CXType (*clang_getCanonicalType_t)(CXType T);
static clang_getCanonicalType_t dlclang_getCanonicalType = 0;
typedef CXString (*clang_getCompletionAnnotation_t)(CXCompletionString completion_string,
                              unsigned annotation_number);
static clang_getCompletionAnnotation_t dlclang_getCompletionAnnotation = 0;
typedef enum CXCompletionChunkKind (*clang_getCompletionChunkKind_t)(CXCompletionString completion_string,
                             unsigned chunk_number);
static clang_getCompletionChunkKind_t dlclang_getCompletionChunkKind = 0;
typedef CXString (*clang_getCompletionChunkText_t)(CXCompletionString completion_string,
                             unsigned chunk_number);
static clang_getCompletionChunkText_t dlclang_getCompletionChunkText = 0;
typedef unsigned (*clang_getCompletionNumAnnotations_t)(CXCompletionString completion_string);
static clang_getCompletionNumAnnotations_t dlclang_getCompletionNumAnnotations = 0;
typedef unsigned (*clang_getCompletionPriority_t)(CXCompletionString completion_string);
static clang_getCompletionPriority_t dlclang_getCompletionPriority = 0;
typedef void (*clang_getCString_t)(PGconn *conn, Oid lobjId);
static clang_getCString_t dlclang_getCString = 0;
typedef CXCursor (*clang_getCursorDefinition_t)(CXCursor);
static clang_getCursorDefinition_t dlclang_getCursorDefinition = 0;
typedef void (*clang_getCursorDisplayName_t)(PGconn *conn, int fd);
static clang_getCursorDisplayName_t dlclang_getCursorDisplayName = 0;
typedef CXSourceRange (*clang_getCursorExtent_t)(CXCursor);
static clang_getCursorExtent_t dlclang_getCursorExtent = 0;
typedef CXString (*clang_getCursorKind_t)(enum CXCursorKind Kind);
static clang_getCursorKind_t dlclang_getCursorKind = 0;
typedef CXCursor (*clang_getCursorReferenced_t)(CXCursor);
static clang_getCursorReferenced_t dlclang_getCursorReferenced = 0;
typedef CXType (*clang_getCursorResultType_t)(CXCursor C);
static clang_getCursorResultType_t dlclang_getCursorResultType = 0;
typedef CXCursor (*clang_getCursorSemanticParent_t)(CXCursor);
static clang_getCursorSemanticParent_t dlclang_getCursorSemanticParent = 0;
typedef CXString (*clang_getCursorSpelling_t)(CXCursor);
static clang_getCursorSpelling_t dlclang_getCursorSpelling = 0;
typedef CXType (*clang_getCursorType_t)(CXCursor);
static clang_getCursorType_t dlclang_getCursorType = 0;
typedef void (*clang_getCursorUSR_t)(const char *conninfo);
static clang_getCursorUSR_t dlclang_getCursorUSR = 0;
typedef enum CX_CXXAccessSpecifier (*clang_getCXXAccessSpecifier_t)(CXCursor);
static clang_getCXXAccessSpecifier_t dlclang_getCXXAccessSpecifier = 0;
typedef CXString (*clang_getDiagnosticCategoryText_t)(CXDiagnostic);
static clang_getDiagnosticCategoryText_t dlclang_getDiagnosticCategoryText = 0;
typedef CXDiagnostic (*clang_getDiagnostic_t)(CXTranslationUnit Unit,
                                                unsigned Index);
static clang_getDiagnostic_t dlclang_getDiagnostic = 0;
typedef CXString (*clang_getFileName_t)(CXFile SFile);
static clang_getFileName_t dlclang_getFileName = 0;
typedef time_t (*clang_getFileTime_t)(CXFile SFile);
static clang_getFileTime_t dlclang_getFileTime = 0;
typedef unsigned (*clang_getNumCompletionChunks_t)(CXCompletionString completion_string);
static clang_getNumCompletionChunks_t dlclang_getNumCompletionChunks = 0;
typedef unsigned (*clang_getNumDiagnostics_t)(CXTranslationUnit Unit);
static clang_getNumDiagnostics_t dlclang_getNumDiagnostics = 0;
typedef CXType (*clang_getPointeeType_t)(CXType);
static clang_getPointeeType_t dlclang_getPointeeType = 0;
typedef CXSourceLocation (*clang_getRangeEnd_t)(CXSourceRange range);
static clang_getRangeEnd_t dlclang_getRangeEnd = 0;
typedef CXSourceLocation (*clang_getRangeStart_t)(CXSourceRange range);
static clang_getRangeStart_t dlclang_getRangeStart = 0;
typedef void (*clang_getSpellingLocation_t)(CXSourceLocation location,
                                              CXFile *file,
                                              unsigned *line,
                                              unsigned *column,
                                              unsigned *offset);
static clang_getSpellingLocation_t dlclang_getSpellingLocation = 0;
typedef CXCursor (*clang_getTranslationUnitCursor_t)(CXTranslationUnit);
static clang_getTranslationUnitCursor_t dlclang_getTranslationUnitCursor = 0;
typedef CXCursor (*clang_getTypeDeclaration_t)(CXType);
static clang_getTypeDeclaration_t dlclang_getTypeDeclaration = 0;
typedef CXString (*clang_getTypeKindSpelling_t)(enum CXTypeKind K);
static clang_getTypeKindSpelling_t dlclang_getTypeKindSpelling = 0;
typedef unsigned (*clang_isPODType_t)(CXType);
static clang_isPODType_t dlclang_isPODType = 0;
typedef CXTranslationUnit (*clang_parseTranslationUnit_t)(CXIndex CIdx,
                           const char *source_filename,
                           const char *const *command_line_args,
                           int num_command_line_args,
                           struct CXUnsavedFile *unsaved_files,
                           unsigned num_unsaved_files,
                           unsigned options);
static clang_parseTranslationUnit_t dlclang_parseTranslationUnit = 0;
typedef unsigned (*clang_visitChildren_t)(CXCursor parent,
                                            CXCursorVisitor visitor,
                                            CXClientData client_data);
static clang_visitChildren_t dlclang_visitChildren = 0;
typedef void (*clang_getCursorExtent_t)(const char *conninfo);
static clang_getCursorExtent_t dlclang_getCursorExtent = 0;
// generated-code:end
#ifdef WIN32
#define libclang_NAME "libclang.dll"
#else
#define libclang_NAME "libclang.so"
#endif

static bool load_libclang()
{
    if(dlPQconnectdb) return true;
    if(libclang.open(libclang_NAME))
    {
        //@write_libclang_functions_load();
// generated-code:begin
dlclang_codeCompleteAt = (clang_codeCompleteAt_t) libclang.dlsym("clang_codeCompleteAt");
if(!dlclang_codeCompleteAt) return false;
dlclang_codeCompleteGetDiagnostic = (clang_codeCompleteGetDiagnostic_t) libclang.dlsym("clang_codeCompleteGetDiagnostic");
if(!dlclang_codeCompleteGetDiagnostic) return false;
dlclang_codeCompleteGetNumDiagnostics = (clang_codeCompleteGetNumDiagnostics_t) libclang.dlsym("clang_codeCompleteGetNumDiagnostics");
if(!dlclang_codeCompleteGetNumDiagnostics) return false;
dlclang_createIndex = (clang_createIndex_t) libclang.dlsym("clang_createIndex");
if(!dlclang_createIndex) return false;
dlclang_createTranslationUnit = (clang_createTranslationUnit_t) libclang.dlsym("clang_createTranslationUnit");
if(!dlclang_createTranslationUnit) return false;
dlclang_Cursor_getArgument = (clang_Cursor_getArgument_t) libclang.dlsym("clang_Cursor_getArgument");
if(!dlclang_Cursor_getArgument) return false;
dlclang_Cursor_getNumArguments = (clang_Cursor_getNumArguments_t) libclang.dlsym("clang_Cursor_getNumArguments");
if(!dlclang_Cursor_getNumArguments) return false;
dlclang_Cursor_isNull = (clang_Cursor_isNull_t) libclang.dlsym("clang_Cursor_isNull");
if(!dlclang_Cursor_isNull) return false;
dlclang_CXXMethod_isStatic = (clang_CXXMethod_isStatic_t) libclang.dlsym("clang_CXXMethod_isStatic");
if(!dlclang_CXXMethod_isStatic) return false;
dlclang_CXXMethod_isVirtual = (clang_CXXMethod_isVirtual_t) libclang.dlsym("clang_CXXMethod_isVirtual");
if(!dlclang_CXXMethod_isVirtual) return false;
dlclang_defaultCodeCompleteOptions = (clang_defaultCodeCompleteOptions_t) libclang.dlsym("clang_defaultCodeCompleteOptions");
if(!dlclang_defaultCodeCompleteOptions) return false;
dlclang_defaultDiagnosticDisplayOptions = (clang_defaultDiagnosticDisplayOptions_t) libclang.dlsym("clang_defaultDiagnosticDisplayOptions");
if(!dlclang_defaultDiagnosticDisplayOptions) return false;
dlclang_disposeCodeCompleteResults = (clang_disposeCodeCompleteResults_t) libclang.dlsym("clang_disposeCodeCompleteResults");
if(!dlclang_disposeCodeCompleteResults) return false;
dlclang_disposeDiagnostic = (clang_disposeDiagnostic_t) libclang.dlsym("clang_disposeDiagnostic");
if(!dlclang_disposeDiagnostic) return false;
dlclang_disposeIndex = (clang_disposeIndex_t) libclang.dlsym("clang_disposeIndex");
if(!dlclang_disposeIndex) return false;
dlclang_disposeString = (clang_disposeString_t) libclang.dlsym("clang_disposeString");
if(!dlclang_disposeString) return false;
dlclang_equalCursors = (clang_equalCursors_t) libclang.dlsym("clang_equalCursors");
if(!dlclang_equalCursors) return false;
dlclang_equalTypes = (clang_equalTypes_t) libclang.dlsym("clang_equalTypes");
if(!dlclang_equalTypes) return false;
dlclang_formatDiagnostic = (clang_formatDiagnostic_t) libclang.dlsym("clang_formatDiagnostic");
if(!dlclang_formatDiagnostic) return false;
dlclang_getCanonicalType = (clang_getCanonicalType_t) libclang.dlsym("clang_getCanonicalType");
if(!dlclang_getCanonicalType) return false;
dlclang_getCompletionAnnotation = (clang_getCompletionAnnotation_t) libclang.dlsym("clang_getCompletionAnnotation");
if(!dlclang_getCompletionAnnotation) return false;
dlclang_getCompletionChunkKind = (clang_getCompletionChunkKind_t) libclang.dlsym("clang_getCompletionChunkKind");
if(!dlclang_getCompletionChunkKind) return false;
dlclang_getCompletionChunkText = (clang_getCompletionChunkText_t) libclang.dlsym("clang_getCompletionChunkText");
if(!dlclang_getCompletionChunkText) return false;
dlclang_getCompletionNumAnnotations = (clang_getCompletionNumAnnotations_t) libclang.dlsym("clang_getCompletionNumAnnotations");
if(!dlclang_getCompletionNumAnnotations) return false;
dlclang_getCompletionPriority = (clang_getCompletionPriority_t) libclang.dlsym("clang_getCompletionPriority");
if(!dlclang_getCompletionPriority) return false;
dlclang_getCString = (clang_getCString_t) libclang.dlsym("clang_getCString");
if(!dlclang_getCString) return false;
dlclang_getCursorDefinition = (clang_getCursorDefinition_t) libclang.dlsym("clang_getCursorDefinition");
if(!dlclang_getCursorDefinition) return false;
dlclang_getCursorDisplayName = (clang_getCursorDisplayName_t) libclang.dlsym("clang_getCursorDisplayName");
if(!dlclang_getCursorDisplayName) return false;
dlclang_getCursorExtent = (clang_getCursorExtent_t) libclang.dlsym("clang_getCursorExtent");
if(!dlclang_getCursorExtent) return false;
dlclang_getCursorKind = (clang_getCursorKind_t) libclang.dlsym("clang_getCursorKind");
if(!dlclang_getCursorKind) return false;
dlclang_getCursorReferenced = (clang_getCursorReferenced_t) libclang.dlsym("clang_getCursorReferenced");
if(!dlclang_getCursorReferenced) return false;
dlclang_getCursorResultType = (clang_getCursorResultType_t) libclang.dlsym("clang_getCursorResultType");
if(!dlclang_getCursorResultType) return false;
dlclang_getCursorSemanticParent = (clang_getCursorSemanticParent_t) libclang.dlsym("clang_getCursorSemanticParent");
if(!dlclang_getCursorSemanticParent) return false;
dlclang_getCursorSpelling = (clang_getCursorSpelling_t) libclang.dlsym("clang_getCursorSpelling");
if(!dlclang_getCursorSpelling) return false;
dlclang_getCursorType = (clang_getCursorType_t) libclang.dlsym("clang_getCursorType");
if(!dlclang_getCursorType) return false;
dlclang_getCursorUSR = (clang_getCursorUSR_t) libclang.dlsym("clang_getCursorUSR");
if(!dlclang_getCursorUSR) return false;
dlclang_getCXXAccessSpecifier = (clang_getCXXAccessSpecifier_t) libclang.dlsym("clang_getCXXAccessSpecifier");
if(!dlclang_getCXXAccessSpecifier) return false;
dlclang_getDiagnosticCategoryText = (clang_getDiagnosticCategoryText_t) libclang.dlsym("clang_getDiagnosticCategoryText");
if(!dlclang_getDiagnosticCategoryText) return false;
dlclang_getDiagnostic = (clang_getDiagnostic_t) libclang.dlsym("clang_getDiagnostic");
if(!dlclang_getDiagnostic) return false;
dlclang_getFileName = (clang_getFileName_t) libclang.dlsym("clang_getFileName");
if(!dlclang_getFileName) return false;
dlclang_getFileTime = (clang_getFileTime_t) libclang.dlsym("clang_getFileTime");
if(!dlclang_getFileTime) return false;
dlclang_getNumCompletionChunks = (clang_getNumCompletionChunks_t) libclang.dlsym("clang_getNumCompletionChunks");
if(!dlclang_getNumCompletionChunks) return false;
dlclang_getNumDiagnostics = (clang_getNumDiagnostics_t) libclang.dlsym("clang_getNumDiagnostics");
if(!dlclang_getNumDiagnostics) return false;
dlclang_getPointeeType = (clang_getPointeeType_t) libclang.dlsym("clang_getPointeeType");
if(!dlclang_getPointeeType) return false;
dlclang_getRangeEnd = (clang_getRangeEnd_t) libclang.dlsym("clang_getRangeEnd");
if(!dlclang_getRangeEnd) return false;
dlclang_getRangeStart = (clang_getRangeStart_t) libclang.dlsym("clang_getRangeStart");
if(!dlclang_getRangeStart) return false;
dlclang_getSpellingLocation = (clang_getSpellingLocation_t) libclang.dlsym("clang_getSpellingLocation");
if(!dlclang_getSpellingLocation) return false;
dlclang_getTranslationUnitCursor = (clang_getTranslationUnitCursor_t) libclang.dlsym("clang_getTranslationUnitCursor");
if(!dlclang_getTranslationUnitCursor) return false;
dlclang_getTypeDeclaration = (clang_getTypeDeclaration_t) libclang.dlsym("clang_getTypeDeclaration");
if(!dlclang_getTypeDeclaration) return false;
dlclang_getTypeKindSpelling = (clang_getTypeKindSpelling_t) libclang.dlsym("clang_getTypeKindSpelling");
if(!dlclang_getTypeKindSpelling) return false;
dlclang_isPODType = (clang_isPODType_t) libclang.dlsym("clang_isPODType");
if(!dlclang_isPODType) return false;
dlclang_parseTranslationUnit = (clang_parseTranslationUnit_t) libclang.dlsym("clang_parseTranslationUnit");
if(!dlclang_parseTranslationUnit) return false;
dlclang_visitChildren = (clang_visitChildren_t) libclang.dlsym("clang_visitChildren");
if(!dlclang_visitChildren) return false;
dlclang_getCursorExtent = (clang_getCursorExtent_t) libclang.dlsym("clang_getCursorExtent");
if(!dlclang_getCursorExtent) return false;
// generated-code:end

        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

static const SQChar *PostgreSQL_TAG = _SC("PostgreSQL");

static SQRESULT get_pgsql_instance(HSQUIRRELVM v, SQInteger idx, PGconn **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)PostgreSQL_TAG)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("database is closed"));
	return _rc_;
}

#define GET_pgsql_INSTANCE_AT(idx) \
	PGconn *self=NULL; \
	if((_rc_ = get_pgsql_instance(v,idx,&self)) < 0) return _rc_;

#define GET_pgsql_INSTANCE() GET_pgsql_INSTANCE_AT(1)

static const SQChar *PostgreSQL_Result_TAG = _SC("PostgreSQL_Result");
static const SQChar *_curr_row_key = _SC("_curr_row");

static SQRESULT get_pgsql_result_instance(HSQUIRRELVM v, SQInteger idx, PGresult **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)PostgreSQL_Result_TAG)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("PGresult is closed"));
	return _rc_;
}

#define GET_pgsql_result_INSTANCE_AT(idx) \
	PGresult *self=NULL; \
	if((_rc_ = get_pgsql_result_instance(v,idx,&self)) < 0) return _rc_;

#define GET_pgsql_result_INSTANCE() GET_pgsql_result_INSTANCE_AT(1)

static SQRESULT sq_pgsql_result_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	PGresult *self = ((PGresult *)p);
	if (self) dlPQclear(self);
	return 0;
}

static SQRESULT sq_pgsql_result_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	dlPQclear(self);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "Pgresult is closed"
	return 0;
}

static SQRESULT sq_pgsql_result_col_count(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	sq_pushinteger(v, dlPQnfields(self));
	return 1;
}

static SQRESULT sq_pgsql_result_row_count(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	sq_pushinteger(v, dlPQntuples(self));
	return 1;
}

static SQRESULT sq_pgsql_result_col_name(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	SQ_GET_INTEGER(v, 2, col);
	sq_pushstring(v, dlPQfname(self, col), -1);
	return 1;
}

static SQRESULT sq_pgsql_result_col_index(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	SQ_GET_STRING(v, 2, name);
	sq_pushinteger(v, dlPQfnumber(self, name));
	return 1;
}

static SQRESULT sq_pgsql_result_eof(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
    sq_pushstring(v, _curr_row_key, -1);
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        sq_pushbool(v, curr_row < dlPQntuples(self));
    }
	else sq_pushbool(v, SQTrue);
	return 1;
}

static SQRESULT sq_pgsql_result_next_row(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
    sq_pushstring(v, _curr_row_key, -1);
    sq_push(v, -1); //make a copy
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        if(++curr_row < dlPQntuples(self)){
            sq_poptop(v);
            sq_pushinteger(v, curr_row);
            sq_set(v, 1);
            sq_pushbool(v, SQTrue);
            return 1;
        }
    }
	sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_pgsql_result_col_value(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_result_INSTANCE();
	SQObjectType ptype = sq_gettype(v, 2);
	int col = -1;
	if(ptype == OT_STRING){
	    SQ_GET_STRING(v, 2, col_name);
	    col = dlPQfnumber(self, col_name);
	}
	else
	{
	    SQ_GET_INTEGER(v, 2, idx);
	    col = idx;
	}
	if(col < 0) return sq_throwerror(v, _SC("invalid col index/name"));

    sq_pushstring(v, _curr_row_key, -1);
    if(sq_get(v, 1) == SQ_OK){
        SQ_GET_INTEGER(v, -1, curr_row);
        if(curr_row < dlPQntuples(self)){
            sq_pushstring(v, dlPQgetvalue(self, curr_row, col), -1);
            return 1;
        }
    }
	return SQ_ERROR;
}

static SQRESULT sq_pgsql_result_row_as_array(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_pgsql_result_INSTANCE();
	SQ_OPT_INTEGER(v, 2, row, -1);
    if(row < 0){
        sq_pushstring(v, _curr_row_key, -1);
        if(sq_get(v, 1) == SQ_OK){
            sq_getinteger(v, -1, &row);
        }
    }
    int row_count = dlPQntuples(self);
    if(row < 0 || row >= row_count) return sq_throwerror(v, _SC("invalid row (%d)"), row);

    int col_count = dlPQnfields(self);
    sq_newarray(v, col_count);
    for(int i=0; i < col_count; ++i){
        sq_pushinteger(v, i);
        sq_pushstring(v, dlPQgetvalue(self, row, i), -1);
        sq_rawset(v, -3);
    }
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_pgsql_result_##name,nparams,tycheck}
static SQRegFunction sq_pgsql_result_methods[] =
{
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(eof,  1, _SC("x")),
	_DECL_FUNC(next_row,  1, _SC("x")),
	_DECL_FUNC(col_count,  1, _SC("x")),
	_DECL_FUNC(row_count,  1, _SC("x")),
	_DECL_FUNC(col_name,  2, _SC("xi")),
	_DECL_FUNC(col_index,  2, _SC("xs")),
	_DECL_FUNC(col_value,  2, _SC("x i|s")),
	_DECL_FUNC(row_as_array,  -1, _SC("xi")),
	{0,0}
};
#undef _DECL_FUNC

struct PgSqlStatement {
    PGconn *db;
    PGresult *result;
    char name[64];
};

static const SQChar *PostgreSQL_Statement_TAG = _SC("PostgreSQL_Statement");

static SQRESULT get_pgsql_statement_instance(HSQUIRRELVM v, SQInteger idx, PgSqlStatement **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)PostgreSQL_Statement_TAG)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("PGstatement is closed"));
	return _rc_;
}

#define GET_pgsql_statement_INSTANCE_AT(idx) \
	PgSqlStatement *self=NULL; \
	if((_rc_ = get_pgsql_statement_instance(v,idx,&self)) < 0) return _rc_;

#define GET_pgsql_statement_INSTANCE() GET_pgsql_statement_INSTANCE_AT(1)

static SQRESULT sq_pgsql_statement_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	PgSqlStatement *self = ((PgSqlStatement *)p);
	if (self && self->result){
        char sql[128];
        snprintf(sql, sizeof(sql), "DEALLOCATE '%s'", self->name);
        PGresult *qres = dlPQexec(self->db, sql);
        bool is_ok = dlPQresultStatus(qres) != PGRES_BAD_RESPONSE;
        dlPQclear(qres);
        if(is_ok) dlPQclear(self->result);
        sq_free(self, sizeof(PgSqlStatement));
	}
	return 0;
}

static SQRESULT sq_pgsql_statement_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_statement_INSTANCE();
	sq_pgsql_statement_releasehook(self, 0, v);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "Pgstatement is closed"
	return 0;
}


#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_pgsql_statement_##name,nparams,tycheck}
static SQRegFunction sq_pgsql_statement_methods[] =
{
	_DECL_FUNC(close,  1, _SC("x")),
	{0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_pgsql_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	PGconn *self = ((PGconn *)p);
	if (self) dlPQfinish(self);
	return 0;
}

static SQRESULT sq_pgsql_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, szConnInfo);
    PGconn *self=0;

    if(load_libclang())
    {
        self = dlPQconnectdb(szConnInfo);
        if (dlPQstatus(self) == CONNECTION_BAD) return sq_throwerror(v, _SC("Failed to connect ot database !"));
    }
    else return sq_throwerror(v, _SC("Failed to load libclang !"));

    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, sq_pgsql_releasehook);

    //save a weakref to allow statement return it's db
    sq_pushuserpointer(v, self);
    sq_weakref(v, 1);
    sq_setonregistrytable(v);

    return 1;
}

static SQRESULT sq_pgsql_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	dlPQfinish(self);
    sq_setinstanceup(v, 1, 0); //next calls will fail with "database is closed"
	return 0;
}

static SQRESULT sq_pgsql_exec_dml(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);
     int result = 0;

    PGresult *qres = dlPQexec(self, szSQL);

    bool is_ok = dlPQresultStatus(qres) == PGRES_COMMAND_OK;

    if (is_ok) result = atoi(dlPQcmdTuples(qres));
    dlPQclear(qres);

    if (!is_ok) return sq_throwerror(v, dlPQerrorMessage(self));

    sq_pushinteger(v, result);
	return 1;
}

static SQRESULT sq_pgsql_exec_scalar(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);
     int result = 0;

    PGresult *qres = dlPQexec(self, szSQL);

    bool is_ok = (dlPQresultStatus(qres) == PGRES_TUPLES_OK) &&
                 (dlPQntuples(qres) == 1) && (dlPQnfields(qres) > 0);

    if (is_ok) result = atoi(dlPQgetvalue(qres, 0, 0));
    dlPQclear(qres);

    if (!is_ok) return sq_throwerror(v, dlPQerrorMessage(self));

    sq_pushinteger(v, result);
	return 1;
}

static SQRESULT sq_pgsql_exec_query(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);

    PGresult *qres = dlPQexec(self, szSQL);

    if(dlPQresultStatus(qres) == PGRES_TUPLES_OK){
        sq_pushroottable(v);
        sq_pushstring(v, PostgreSQL_Result_TAG, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, qres);
                sq_setreleasehook(v, -1, sq_pgsql_result_releasehook);
                sq_pushstring(v, _curr_row_key, -1);
                sq_pushinteger(v, -1);
                sq_set(v, -3);
                return 1;
            }
        }
    }
    return sq_throwerror(v, dlPQerrorMessage(self));
}

static SQRESULT sq_pgsql_prepare(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_STRING(v, 2, szSQL);

    PgSqlStatement *stmt = (PgSqlStatement*)sq_malloc(sizeof(PgSqlStatement));
    stmt->db = self;
    snprintf(stmt->name, sizeof(stmt->name), "sq_pg_preared_stmt_%p", stmt);

    stmt->result = dlPQprepare(self, stmt->name, szSQL, 0, NULL);

    if(dlPQresultStatus(stmt->result) == PGRES_COMMAND_OK){
        sq_pushroottable(v);
        sq_pushstring(v, PostgreSQL_Statement_TAG, -1);
        if(sq_get(v, -2) == SQ_OK){
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, stmt);
                sq_setreleasehook(v, -1, sq_pgsql_statement_releasehook);
                return 1;
            }
        }
    }
    sq_free(stmt, sizeof(PgSqlStatement));
    return sq_throwerror(v, dlPQerrorMessage(self));
}

static SQRESULT sq_pgsql_error_message(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	sq_pushstring(v, dlPQerrorMessage(self), -1);
	return 1;
}

static SQRESULT sq_pgsql_version(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	sq_pushinteger(v, dlPQserverVersion(self));
	return 1;
}

static int
 inv_read =  0x40000,
 inv_write = 0x20000,
 invalidoid = 0,
 inv_seek_set = 0,
 inv_seek_curr = 1,
 inv_seek_end = 2;

static SQRESULT sq_pgsql_get_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	SQ_GET_INTEGER(v, 2, oid);
    //begin_recursive_transaction();
    char *result = 0;
	int ofd = dllo_open(self, oid, inv_read);
	if(ofd >= 0){
		int blobSize = dllo_lseek(self, ofd, 0, inv_seek_end);
		dllo_lseek(self, ofd, 0, inv_seek_set);
	    SQBlob blob(blobSize);
		result = (char*)blob.GetBuf();
		int numRead = 0;
		while(blobSize > 0){
		 	int i = dllo_read(self, ofd, result+numRead, blobSize);
			numRead += i;
			blobSize -= i;
		}
		dllo_close(self, oid);
		sq_pushstring(v, (const SQChar*)blob.GetBuf(), blob.Len());
	}
	//commit_recursive_transaction();
	if(!result) sq_pushnull(v);
	return 1;
}

static SQRESULT sq_pgsql_insert_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    int result = 0;
    SQ_GET_STRING(v, 2, blob);
    SQ_GET_BOOL(v, 3, isFileName);
	if(isFileName){
		result = dllo_import(self, blob);
	} else {
		result = dllo_creat(self, inv_write);
		if(result){
			int ofd = dllo_open(self, result, inv_write);
			if (ofd >= 0){
				int i = blob_size;
				const char *blopPtr = (const char *)blob;
				int numWriten = 0;
				while(i > 0){
				 	int i2 = dllo_write(self, ofd, blopPtr+numWriten, i);
					numWriten += i2;
					i -= i2;
				}
				dllo_close(self, ofd);
			}
			else return sq_throwerror(v, _SC("Failed to insert blob !"));
		}
	}
	if(!result) sq_pushnull(v);
	return 1;
}

static SQRESULT sq_pgsql_update_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
    SQ_GET_INTEGER(v, 2, oid);
    SQ_GET_STRING(v, 3, blob);
    SQ_GET_BOOL(v, 4, isFileName);

    int result_oid = 0;
    int result_error = SQ_OK;
	int loid = dllo_creat(self, inv_write);
	int ofd = dllo_open(self, loid, inv_write);
	if(ofd >= 0){
		dllo_unlink(self, oid);
		result_oid = loid;
		if(isFileName)
		{
			char buf[2048];
			FILE *fp = fopen(blob, "rb");
			if(!fp) {
			    sq_throwerror(v, _SC("Failed to update blob from file !"));
			    result_error = SQ_ERROR;
			}
			else
			{
                char *charPtr = buf;
                int numRead;
                do{
                    numRead = fread(buf, 1, sizeof(buf), fp);
                    int numWriten = dllo_write(self, ofd, charPtr, numRead);
                    if (numWriten != numRead) {
                        sq_throwerror(v, _SC("Failed to update blob from file !"));
                        result_error = SQ_ERROR;
                        break;
                    }
                } while (numRead == 0);
                fclose(fp);
			}
		}else{
			int i = blob_size;
            const char *blopPtr = (const char *)blob;
			int numWriten = 0;
			while(i > 0){
			 	int i2 = dllo_write(self, ofd, blopPtr+numWriten, i);
				numWriten += i2;
				i -= i2;
			}
		}
		dllo_close(self, ofd);
	}
	if(result_error == SQ_ERROR) return result_error;
	sq_pushinteger(v, result_oid);
	return 1;
}

static SQRESULT sq_pgsql_delete_blob_field(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_pgsql_INSTANCE();
	SQ_GET_INTEGER(v, 2, oid);
	sq_pushinteger(v, dllo_unlink(self, oid));
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_pgsql_##name,nparams,tycheck}
static SQRegFunction sq_pgsql_methods[] =
{
	_DECL_FUNC(constructor,  2, _SC("xs")),
	_DECL_FUNC(close,  1, _SC("x")),
	_DECL_FUNC(exec_dml,  2, _SC("xs")),
	_DECL_FUNC(exec_scalar,  2, _SC("xs")),
	_DECL_FUNC(exec_query,  2, _SC("xs")),
	_DECL_FUNC(prepare,  2, _SC("xs")),
	_DECL_FUNC(error_message,  1, _SC("x")),
	_DECL_FUNC(version,  1, _SC("x")),
	_DECL_FUNC(get_blob_field,  2, _SC("xi")),
	_DECL_FUNC(insert_blob_field,  3, _SC("xsb")),
	_DECL_FUNC(update_blob_field,  3, _SC("xisb")),
	_DECL_FUNC(delete_blob_field,  2, _SC("xi")),
	{0,0}
};
#undef _DECL_FUNC


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_PostgreSQL(HSQUIRRELVM v)
{
    sq_pushstring(v,PostgreSQL_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PostgreSQL_TAG);
    sq_insert_reg_funcs(v, sq_pgsql_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,PostgreSQL_Statement_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PostgreSQL_Statement_TAG);
    sq_insert_reg_funcs(v, sq_pgsql_statement_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,PostgreSQL_Result_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)PostgreSQL_Result_TAG);
    sq_insert_reg_funcs(v, sq_pgsql_result_methods);
    sq_pushstring(v, _curr_row_key, -1);
    sq_pushnull(v);
    sq_newslot(v, -3, SQFalse);
    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // WITH_LIBCLANG
