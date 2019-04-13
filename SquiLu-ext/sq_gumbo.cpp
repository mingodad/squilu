/// @module gumbo
// Lua bindings for the [Gumbo][] HTML5 parsing library.
// [Gumbo]: https://github.com/google/gumbo-parser
// @copyright 2013 Craig Barnes
// @license ISC
// Ported to Squilu by Domingo Alvarez Duarte

#include "squirrel.h"
#include <string.h>
#include "sqstdblobimpl.h"
SQ_OPT_STRING_STRLEN();
#include "gumbo.h"
#include <stdio.h>
#include <errno.h>

#define MYNAME		_SC("gumbo")

#define assert(cond) if (!(cond)) goto error
static SQRESULT build_node(HSQUIRRELVM v, GumboNode* node);

static void reg_string(HSQUIRRELVM v, const char *name, const char *val) {
  sq_pushstring(v, name, -1);
  if(val) sq_pushstring(v, val, -1);
  else sq_pushnull(v);
  sq_rawset(v, -3);
}

static void reg_boolean(HSQUIRRELVM v, const char *name, SQBool val) {
  sq_pushstring(v, name, -1);
  sq_pushbool(v, val);
  sq_rawset(v, -3);
}

static inline SQRESULT add_children(HSQUIRRELVM v, GumboVector *children) {
    unsigned int tl = 0;
    for (unsigned int i = 0, cl = children->length; i < cl; i++) {
        switch(build_node(v, (GumboNode*)children->data[i])){
            case SQTrue:
                sq_arrayset(v, -2, tl++);
                break;
            case SQFalse:
                break;
            case SQ_ERROR:
                return SQ_ERROR;
        }
    }
    if(tl < children->length) {
        sq_arrayresize(v, -1, tl);
    }
    return SQ_OK;
}

static SQRESULT build_document(HSQUIRRELVM v, GumboDocument *document) {
    sq_newtableex(v, 6);
    reg_string(v, _SC("name"), document->name);
    reg_string(v, _SC("public_identifier"), document->public_identifier);
    reg_string(v, _SC("system_identifier"), document->system_identifier);
    reg_boolean(v, _SC("has_doctype"), document->has_doctype ? SQTrue : SQFalse);
    sq_pushliteral(v, _SC("children"));
    sq_newarray(v, document->children.length);
    if(add_children(v, &document->children) == SQ_ERROR) return SQ_ERROR;
    sq_rawset(v, -3);
    return 1;
}

static SQRESULT build_element(HSQUIRRELVM v, GumboElement *element) {
    unsigned int nattrs = element->attributes.length;
    sq_newtableex(v, nattrs ? 3 : 2);

    // Add tag name
    sq_pushliteral(v, _SC("tag"));
    if (element->tag == GUMBO_TAG_UNKNOWN) {
        GumboStringPiece original_tag = element->original_tag;
        gumbo_tag_from_original_text(&original_tag);
        sq_pushstring(v, original_tag.data, original_tag.length);
    } else {
        sq_pushstring(v, gumbo_normalized_tagname(element->tag), -1);
    }
    sq_rawset(v, -3);

    // Add attributes
    if (nattrs) {
        sq_pushliteral(v, _SC("attr"));
        sq_newtableex(v, nattrs);
        for (unsigned int i = 0; i < nattrs; ++i) {
            GumboAttribute *attribute = (GumboAttribute *)element->attributes.data[i];
            reg_string(v, attribute->name, attribute->value);
        }
        sq_rawset(v, -3);
    }

    sq_pushliteral(v, _SC("children"));
    sq_newarray(v, element->children.length);
    if(add_children(v, &element->children) == SQ_ERROR) return SQ_ERROR;
    sq_rawset(v, -3);

    sq_pushliteral(v, _SC("start_pos"));
    sq_pushinteger(v, element->start_pos.offset);
    sq_rawset(v, -3);

    sq_pushliteral(v, _SC("end_pos"));
    sq_pushinteger(v, element->end_pos.offset);
    sq_rawset(v, -3);

    return SQ_OK;
}

static SQRESULT build_node(HSQUIRRELVM v, GumboNode* node) {
    switch (node->type) {
    case GUMBO_NODE_DOCUMENT:
        build_document(v, &node->v.document);
        return SQTrue;

    case GUMBO_NODE_ELEMENT:
        build_element(v, &node->v.element);
        return SQTrue;

    case GUMBO_NODE_COMMENT:
        sq_newtableex(v, 1);
        reg_string(v, _SC("comment"), node->v.text.text);
        return SQTrue;

    case GUMBO_NODE_TEXT:
    case GUMBO_NODE_CDATA:
        sq_pushstring(v, node->v.text.text, -1);
        return SQTrue;

    case GUMBO_NODE_WHITESPACE:
        return SQFalse;

    default:
        return sq_throwerror(v, _SC("Invalid node type"));
    }
}

static inline SQRESULT parse(HSQUIRRELVM v, const SQChar *input, SQInteger len) {
    GumboOutput *output;
    output = gumbo_parse_with_options(&kGumboDefaultOptions, input, len);
    SQRESULT result = build_node(v, output->document);
    if(result == SQ_ERROR) {
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        return SQ_ERROR;
    }
    sq_pushliteral(v, _SC("children"));
    sq_rawget(v, -2);
    sq_pushliteral(v, _SC("root"));
    sq_arrayget(v, -2, output->root->index_within_parent);
    sq_rawset(v, -4); //set root on main table
    sq_poptop(v); //remove children array from stack
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return result;
}

/// Parse a string of HTML
// @function parse
// @param document String containing HTML
// @return Abstract syntax tree table
// @see README.md
static SQRESULT gumbo_parse(HSQUIRRELVM v)		/** parse(s) */
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, input);
    return parse(v, input, input_size);
}

/// Read and parse a HTML file
// @function parse_file
// @param filename Path to HTML file
// @return Abstract syntax tree table
// @throw exception (if opening or reading file fails)
static SQRESULT gumbo_parse_file(HSQUIRRELVM v)		/** parse_file(s) */
{
    SQRESULT result;
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, filename);
    FILE *file = NULL;
    char *input = NULL;
    long len;

    assert(file = fopen(filename, "rb"));
    assert(fseek(file, 0, SEEK_END) != -1);
    assert((len = ftell(file)) != -1);
    rewind(file);
    assert(input = (char*)sq_malloc(len + 1));
    assert(fread(input, 1, len, file) == (unsigned long)len);
    fclose(file);
    input[len] = '\0';
    result = parse(v, input, len);
    sq_free(input, len+1);
    return result;

  error: // Return nil and an error message if an assertion fails
    if (file) fclose(file);
    if (input) sq_free(input, len+1);
    return sq_throwerror(v, _SC("%s"), strerror(errno));
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),gumbo_##name,nparams,tycheck}
static SQRegFunction gumbo_methods[] =
{
    _DECL_FUNC(parse,2,_SC(".s")),
    _DECL_FUNC(parse_file,2,_SC(".s")),
    {0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_gumbo(HSQUIRRELVM v)
{
    sq_pushstring(v,_SC("gumbo"),-1);
    sq_newtable(v);
    sq_insert_reg_funcs(v, gumbo_methods);
    sq_newslot(v,-3,SQTrue);
    return 0;
}

#ifdef __cplusplus
}
#endif
