#include "tinyxml2.h"
#include "squirrel.h"
#include <string.h>
SQ_OPT_STRING_STRLEN();

#define XML_TAG(tag) SQXML_##tag##_TYPE_TAG
#define CREATE_TAG(tag) static const SQChar* XML_TAG(tag) = #tag

static const SQChar sq_tinyxml2_TAG[] = _SC("TinyXml2");
CREATE_TAG(XMLNode);
CREATE_TAG(XMLComment);
CREATE_TAG(XMLText);
CREATE_TAG(XMLDeclaration);
CREATE_TAG(XMLUnknown);
CREATE_TAG(XMLAttribute);
CREATE_TAG(XMLElement);
CREATE_TAG(XMLDocument);
CREATE_TAG(XMLPrinter);

#define GET_TINYXML2_INSTANCE(v, idx, klass, var) SQ_GET_INSTANCE_VAR(v, idx, tinyxml2::klass, var, XML_TAG(klass))
#define GET_XMLNode_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, idx, XMLNode, node)
#define GET_XMLNode_INSTANCE2(v, idx, var) GET_TINYXML2_INSTANCE(v, idx, XMLNode, var)
#define GET_XMLComment_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, idx, Comment, comment)
#define GET_XMLText_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, idx, XMLText, text)
#define GET_XMLDeclaration_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, idx, XMLDeclaration, declaration)
#define GET_XMLUnknown_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, idx, XMLUnknown, unknown)
#define GET_XMLAttribute_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, idx, XMLAttribute, attribute)
#define GET_XMLElement_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, idx, XMLElement, element)
#define GET_XMLDocument_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, 1, XMLDocument, doc)
#define GET_XMLPrinter_INSTANCE(v, idx) GET_TINYXML2_INSTANCE(v, 1, XMLPrinter, printer)

static SQRESULT sq_push_xml_class(HSQUIRRELVM v, const SQChar *klass_tag, void *self){
    if(!self) {
        sq_pushnull(v);
        return 1;
    }
    sq_pushstring(v, sq_tinyxml2_TAG, -1);
    if(sq_getonroottable(v) == SQ_OK){
        sq_pushstring(v, klass_tag, -1);
        if(sq_rawget(v, -2) == SQ_OK) {
            if(sq_createinstance(v, -1) == SQ_OK){
                sq_setinstanceup(v, -1, self);
                return 1;
            }
        }
    }
    return SQ_ERROR;
}

static SQRESULT sq_XMLNode_GetDocument(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLDocument *doc = node->GetDocument();
    return sq_push_xml_class(v, XML_TAG(XMLDocument), doc);
}

static SQRESULT sq_XMLNode_ToElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLElement *ptr = node->ToElement();
    return sq_push_xml_class(v, XML_TAG(XMLElement), ptr);
}

static SQRESULT sq_XMLNode_ToText(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLText *ptr = node->ToText();
    return sq_push_xml_class(v, XML_TAG(XMLText), ptr);
}

static SQRESULT sq_XMLNode_ToComment(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLComment *ptr = node->ToComment();
    return sq_push_xml_class(v, XML_TAG(XMLComment), ptr);
}

static SQRESULT sq_XMLNode_ToDocument(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLDocument *ptr = node->ToDocument();
    return sq_push_xml_class(v, XML_TAG(XMLDocument), ptr);
}

static SQRESULT sq_XMLNode_ToDeclaration(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLDeclaration *ptr = node->ToDeclaration();
    return sq_push_xml_class(v, XML_TAG(XMLDeclaration), ptr);
}

static SQRESULT sq_XMLNode_ToUnknown(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLUnknown *ptr = node->ToUnknown();
    return sq_push_xml_class(v, XML_TAG(XMLUnknown), ptr);
}

static SQRESULT sq_XMLNode_Parent(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLNode *ptr = node->Parent();
    return sq_push_xml_class(v, XML_TAG(XMLNode), ptr);
}

static SQRESULT sq_XMLNode_FirstChild(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLNode *ptr = node->FirstChild();
    return sq_push_xml_class(v, XML_TAG(XMLNode), ptr);
}

static SQRESULT sq_XMLNode_LastChild(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLNode *ptr = node->LastChild();
    return sq_push_xml_class(v, XML_TAG(XMLNode), ptr);
}

static SQRESULT sq_XMLNode_PreviousSibling(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLNode *ptr = node->PreviousSibling();
    return sq_push_xml_class(v, XML_TAG(XMLNode), ptr);
}

static SQRESULT sq_XMLNode_NextSibling(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLNode *ptr = node->NextSibling();
    return sq_push_xml_class(v, XML_TAG(XMLNode), ptr);
}

static SQRESULT sq_XMLNode_FirstChildElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLNode_INSTANCE(v, 1);
    SQ_OPT_STRING(v, 2, name, NULL);
    tinyxml2::XMLElement *ptr = node->FirstChildElement(name);
    return sq_push_xml_class(v, XML_TAG(XMLElement), ptr);
}

static SQRESULT sq_XMLNode_LastChildElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLElement *ptr = node->LastChildElement();
    return sq_push_xml_class(v, XML_TAG(XMLElement), ptr);
}

static SQRESULT sq_XMLNode_PreviousSiblingElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLElement *ptr = node->PreviousSiblingElement();
    return sq_push_xml_class(v, XML_TAG(XMLElement), ptr);
}

static SQRESULT sq_XMLNode_NextSiblingElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLElement *ptr = node->NextSiblingElement();
    return sq_push_xml_class(v, XML_TAG(XMLElement), ptr);
}

static SQRESULT sq_XMLNode_InsertEndChild(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    GET_XMLNode_INSTANCE2(v, 2, addThis);
    node->InsertEndChild(addThis);
    return 1;
}

static SQRESULT sq_XMLNode_LinkEndChild(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    GET_XMLNode_INSTANCE2(v, 2, addThis);
    node->LinkEndChild(addThis);
    return 1;
}

static SQRESULT sq_XMLNode_InsertFirstChild(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    GET_XMLNode_INSTANCE2(v, 2, addThis);
    node->InsertFirstChild(addThis);
    return 1;
}

static SQRESULT sq_XMLNode_InsertAfterChild(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    GET_XMLNode_INSTANCE2(v, 2, afterThis);
    GET_XMLNode_INSTANCE2(v, 3, addThis);
    node->InsertAfterChild(afterThis, addThis);
    return 1;
}

static SQRESULT sq_XMLNode_DeleteChildren(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    node->DeleteChildren();
    return 0;
}

static SQRESULT sq_XMLNode_DeleteChild(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    GET_XMLNode_INSTANCE2(v, 2, child);
    node->DeleteChild(child);
    return 0;
}

static SQRESULT sq_XMLNode_ShallowClone(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLNode_INSTANCE(v, 1);
    tinyxml2::XMLNode *clone;
    if(_top_ > 1){
        GET_XMLDocument_INSTANCE(v, 2);
        clone = node->ShallowClone(doc);
    }
    else clone = node->ShallowClone(0);

    return sq_push_xml_class(v, XML_TAG(XMLNode), clone);
}

static SQRESULT sq_XMLNode_ShallowEqual(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    GET_XMLNode_INSTANCE2(v, 2, compare);
    sq_pushbool(v, node->ShallowEqual(compare));
    return 1;
}

static SQRESULT sq_XMLNode_Value(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLNode_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_STRING(v, 2, value);
        node->SetValue(value);
        return 0;
    }
    else
    {
        const char *value = node->Value();
        if(value) sq_pushstring(v, value, -1);
        else sq_pushnull(v);
    }
	return 1;
}

static SQRESULT sq_XMLNode_NoChildren(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLNode_INSTANCE(v, 1);
    sq_pushbool(v, node->NoChildren());
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_XMLNode_##name,nparams,tycheck}
static SQRegFunction XMLNode_methods[] =
{
    _DECL_FUNC(GetDocument,1,_SC("x")),
    _DECL_FUNC(ToElement,1,_SC("x")),
    _DECL_FUNC(ToText,1,_SC("x")),
    _DECL_FUNC(ToComment,1,_SC("x")),
    _DECL_FUNC(ToDocument,1,_SC("x")),
    _DECL_FUNC(ToDeclaration,1,_SC("x")),
    _DECL_FUNC(ToUnknown,1,_SC("x")),
    _DECL_FUNC(Parent,1,_SC("x")),
    _DECL_FUNC(FirstChild,1,_SC("x")),
    _DECL_FUNC(LastChild,1,_SC("x")),
    _DECL_FUNC(PreviousSibling,1,_SC("x")),
    _DECL_FUNC(NextSibling,1,_SC("x")),
    _DECL_FUNC(FirstChildElement,-1,_SC("xs")),
    _DECL_FUNC(LastChildElement,1,_SC("x")),
    _DECL_FUNC(PreviousSiblingElement,1,_SC("x")),
    _DECL_FUNC(NextSiblingElement,1,_SC("x")),
    _DECL_FUNC(InsertEndChild,2,_SC("xx")),
    _DECL_FUNC(LinkEndChild,2,_SC("xx")),
    _DECL_FUNC(InsertFirstChild,2,_SC("xx")),
    _DECL_FUNC(InsertAfterChild,3,_SC("xxx")),
    _DECL_FUNC(DeleteChildren,1,_SC("x")),
    _DECL_FUNC(DeleteChild,2,_SC("xx")),
    _DECL_FUNC(ShallowClone,-1,_SC("xx")),
    _DECL_FUNC(ShallowEqual,2,_SC("xx")),
    _DECL_FUNC(Value,-1,_SC("xs")),
    _DECL_FUNC(NoChildren,1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_XMLText_CData(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLText_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_BOOL(v, 2, bvalue);
        text->SetCData(bvalue);
        return 0;
    }
    else sq_pushbool(v, text->CData());
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_XMLText_##name,nparams,tycheck}
static SQRegFunction XMLText_methods[] =
{
    _DECL_FUNC(CData,-1,_SC("xb")),
    {0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_XMLAttribute_Name(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLAttribute_INSTANCE(v, 1);
    sq_pushstring(v, attribute->Name(), -1);
	return 1;
}

static SQRESULT sq_XMLAttribute_Value(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLAttribute_INSTANCE(v, 1);
    sq_pushstring(v, attribute->Value(), -1);
	return 1;
}

static SQRESULT sq_XMLAttribute_Next(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLAttribute_INSTANCE(v, 1);
    const tinyxml2::XMLAttribute *next = attribute->Next();
    if(next) return sq_push_xml_class(v, XML_TAG(XMLAttribute), (void*)next);
    sq_pushnull(v);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_XMLAttribute_##name,nparams,tycheck}
static SQRegFunction XMLAttribute_methods[] =
{
    _DECL_FUNC(Name,1,_SC("x")),
    _DECL_FUNC(Value,1,_SC("x")),
    _DECL_FUNC(Next,1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_XMLElement_Name(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLElement_INSTANCE(v, 1);
    if(_top_ > 1){
        SQ_GET_STRING(v, 2, name);
        element->SetName(name);
        return 0;
    }
    else sq_pushstring(v, element->Name(), -1);
	return 1;
}

static SQRESULT sq_XMLElement_Attribute(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLElement_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    SQ_OPT_STRING(v, 3, value, NULL);
    const char* res = element->Attribute(name, value);
    if(res) sq_pushstring(v, res, -1);
    else sq_pushnull(v);
	return 1;
}

static SQRESULT sq_XMLElement_SetAttribute(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLElement_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    SQ_GET_STRING(v, 3, value);
    element->SetAttribute(name, value);
	return 0;
}

static SQRESULT sq_XMLElement_DeleteAttribute(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLElement_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    element->DeleteAttribute(name);
	return 0;
}

static SQRESULT sq_XMLElement_FirstAttribute(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLElement_INSTANCE(v, 1);
    element->FirstAttribute();
	return 0;
}

static SQRESULT sq_XMLElement_FindAttribute(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLElement_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
	const tinyxml2::XMLAttribute *attr = ((const tinyxml2::XMLElement*)element)->FindAttribute(name);
    if(attr) return sq_push_xml_class(v, XML_TAG(XMLAttribute), (void*)attr);
    sq_pushnull(v);
	return 1;
}

static SQRESULT sq_XMLElement_GetText(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLElement_INSTANCE(v, 1);
    const char *txt = element->GetText();
    if(txt) sq_pushstring(v, txt, -1);
    else sq_pushnull(v);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_XMLElement_##name,nparams,tycheck}
static SQRegFunction XMLElement_methods[] =
{
    _DECL_FUNC(Name,-1,_SC("xs")),
    _DECL_FUNC(Attribute,-1,_SC("xss")),
    _DECL_FUNC(SetAttribute,3,_SC("xss")),
    _DECL_FUNC(DeleteAttribute,2,_SC("xs")),
    _DECL_FUNC(FirstAttribute,1,_SC("x")),
    _DECL_FUNC(FindAttribute,2,_SC("xs")),
    _DECL_FUNC(GetText,1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC


static SQRESULT sq_XMLDocument_release_hook(SQUserPointer p, SQInteger size, void */*ep*/) {
    tinyxml2::XMLDocument *doc = (tinyxml2::XMLDocument *)p;
    if(doc) delete doc;
    return 0;
}

/*
** Creates a new XMLDocument.
*/
static SQRESULT sq_XMLDocument_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_OPT_BOOL(v, 2, processEntities, true);
    SQ_OPT_INTEGER(v, 3, Whitespace, tinyxml2::PRESERVE_WHITESPACE);
    if(Whitespace == tinyxml2::PRESERVE_WHITESPACE || Whitespace == tinyxml2::COLLAPSE_WHITESPACE)
    {
        tinyxml2::XMLDocument *ptr = new tinyxml2::XMLDocument(processEntities, (tinyxml2::Whitespace)Whitespace);
        sq_setinstanceup(v, 1, ptr);
        sq_setreleasehook(v, 1, sq_XMLDocument_release_hook);
        return 1;
    }
    return sq_throwerror(v, _SC("invalid value for Whitespace"));
}

static SQRESULT sq_XMLDocument_Parse(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, xml);
    SQ_OPT_INTEGER(v, 3, nBytes, -1);
    sq_pushinteger(v, doc->Parse(xml, (size_t)nBytes));
	return 1;
}

static SQRESULT sq_XMLDocument_LoadFile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, filename);
    sq_pushinteger(v, doc->LoadFile(filename));
	return 1;
}

static SQRESULT sq_XMLDocument_SaveFile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, filename);
    SQ_OPT_BOOL(v, 3, compact, SQFalse);
    sq_pushinteger(v, doc->SaveFile(filename, compact));
	return 1;
}

static SQRESULT sq_XMLDocument_HasBOM(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    sq_pushbool(v, doc->HasBOM());
	return 1;
}

static SQRESULT sq_XMLDocument_SetBOM(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_BOOL(v, 2, useBOM);
    doc->SetBOM(useBOM);
	return 0;
}

static SQRESULT sq_XMLDocument_RootElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    tinyxml2::XMLElement *ptr = doc->RootElement();
    return sq_push_xml_class(v, XML_TAG(XMLElement), ptr);
}

static SQRESULT sq_XMLDocument_NewElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    tinyxml2::XMLElement *ptr = doc->NewElement(name);
    return sq_push_xml_class(v, XML_TAG(XMLElement), ptr);
}

static SQRESULT sq_XMLDocument_NewComment(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    tinyxml2::XMLComment *ptr = doc->NewComment(name);
    return sq_push_xml_class(v, XML_TAG(XMLComment), ptr);
}

static SQRESULT sq_XMLDocument_NewText(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    tinyxml2::XMLText *ptr = doc->NewText(name);
    return sq_push_xml_class(v, XML_TAG(XMLText), ptr);
}

static SQRESULT sq_XMLDocument_NewDeclaration(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_OPT_STRING(v, 2, name, NULL);
    tinyxml2::XMLDeclaration *ptr = doc->NewDeclaration(name);
    return sq_push_xml_class(v, XML_TAG(XMLDeclaration), ptr);
}

static SQRESULT sq_XMLDocument_NewUnknown(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    tinyxml2::XMLUnknown *ptr = doc->NewUnknown(name);
    return sq_push_xml_class(v, XML_TAG(XMLUnknown), ptr);
}

static SQRESULT sq_XMLDocument_DeleteNode(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    GET_XMLNode_INSTANCE(v, 2);
    doc->DeleteNode(node);
	return 0;
}

static SQRESULT sq_XMLDocument_Error(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    sq_pushbool(v, doc->Error());
	return 1;
}

static SQRESULT sq_XMLDocument_ErrorID(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    sq_pushinteger(v, doc->ErrorID());
	return 1;
}

static SQRESULT sq_XMLDocument_GetErrorStr1(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    sq_pushstring(v, doc->GetErrorStr1(), -1);
	return 1;
}

static SQRESULT sq_XMLDocument_GetErrorStr2(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    sq_pushstring(v, doc->GetErrorStr2(), -1);
	return 1;
}

static SQRESULT sq_XMLDocument_ProcessEntities(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    sq_pushbool(v, doc->ProcessEntities());
	return 1;
}

static SQRESULT sq_XMLDocument_WhitespaceMode(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    sq_pushinteger(v, doc->WhitespaceMode());
	return 1;
}

static SQRESULT sq_XMLDocument_tostring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLDocument_INSTANCE(v, 1);
    tinyxml2::XMLPrinter printer;
    doc->Print( &printer );
    sq_pushstring(v, printer.CStr(), printer.CStrSize());
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_XMLDocument_##name,nparams,tycheck}
static SQRegFunction XMLDocument_methods[] =
{
    _DECL_FUNC(constructor,-1,_SC("xbi")),
    _DECL_FUNC(Parse,-2,_SC("xsi")),
    _DECL_FUNC(LoadFile,2,_SC("xs")),
    _DECL_FUNC(SaveFile,-2,_SC("xsb")),
    _DECL_FUNC(HasBOM,1,_SC("x")),
    _DECL_FUNC(SetBOM,2,_SC("xb")),
    _DECL_FUNC(RootElement,1,_SC("x")),
    _DECL_FUNC(NewElement,2,_SC("xs")),
    _DECL_FUNC(NewComment,2,_SC("xs")),
    _DECL_FUNC(NewText,2,_SC("xs")),
    _DECL_FUNC(NewDeclaration,-1,_SC("xs")),
    _DECL_FUNC(NewUnknown,2,_SC("xs")),
    _DECL_FUNC(DeleteNode,2,_SC("xx")),
    _DECL_FUNC(Error,1,_SC("x")),
    _DECL_FUNC(ErrorID,1,_SC("x")),
    _DECL_FUNC(GetErrorStr1,1,_SC("x")),
    _DECL_FUNC(GetErrorStr2,1,_SC("x")),
    _DECL_FUNC(ProcessEntities,1,_SC("x")),
    _DECL_FUNC(WhitespaceMode,1,_SC("x")),
    _DECL_FUNC(tostring,1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_XMLPrinter_release_hook(SQUserPointer p, SQInteger size, void */*ep*/) {
    tinyxml2::XMLPrinter *printer = (tinyxml2::XMLPrinter *)p;
    if(printer) delete printer;
    return 0;
}

/*
** Creates a new XMLDocument.
*/
static SQRESULT sq_XMLPrinter_constructor (HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_OPT_BOOL(v, 2, compact, false);
    tinyxml2::XMLPrinter *ptr = new tinyxml2::XMLPrinter(0, compact);
    sq_setinstanceup(v, 1, ptr);
    sq_setreleasehook(v, 1, sq_XMLPrinter_release_hook);
    return 1;
}

static SQRESULT sq_XMLPrinter_PushHeader(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_GET_BOOL(v, 2, writeBOM);
    SQ_GET_BOOL(v, 3, writeDeclaration);
    printer->PushHeader(writeBOM, writeDeclaration);
	return 0;
}

static SQRESULT sq_XMLPrinter_OpenElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    printer->OpenElement(name);
	return 0;
}

static SQRESULT sq_XMLPrinter_PushAttribute(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, name);
    SQ_GET_STRING(v, 3, value);
    printer->PushAttribute(name, value);
	return 0;
}

static SQRESULT sq_XMLPrinter_CloseElement(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    printer->CloseElement();
	return 0;
}

static SQRESULT sq_XMLPrinter_PushText(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, text);
    SQ_OPT_BOOL(v, 3, cdata, false);
    printer->PushText(text, cdata);
	return 0;
}

static SQRESULT sq_XMLPrinter_PushComment(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, comment);
    printer->PushComment(comment);
	return 0;
}

static SQRESULT sq_XMLPrinter_PushDeclaration(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, value);
    printer->PushDeclaration(value);
	return 0;
}

static SQRESULT sq_XMLPrinter_PushUnknown(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, value);
    printer->PushUnknown(value);
	return 0;
}

static SQRESULT sq_XMLPrinter_tostring(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    GET_XMLPrinter_INSTANCE(v, 1);
    sq_pushstring(v, printer->CStr(), printer->CStrSize());
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),sq_XMLPrinter_##name,nparams,tycheck}
static SQRegFunction XMLPrinter_methods[] =
{
    _DECL_FUNC(constructor,-1,_SC("xb")),
    _DECL_FUNC(PushHeader,-1,_SC("xbb")),
    _DECL_FUNC(OpenElement,2,_SC("xs")),
    _DECL_FUNC(PushAttribute,3,_SC("xss")),
    _DECL_FUNC(CloseElement,1,_SC("x")),
    _DECL_FUNC(PushText,-2,_SC("xsb")),
    _DECL_FUNC(PushComment,2,_SC("xs")),
    _DECL_FUNC(PushDeclaration,2,_SC("xs")),
    _DECL_FUNC(PushUnknown,2,_SC("xs")),
    _DECL_FUNC(tostring,1,_SC("x")),
    {0,0}
};
#undef _DECL_FUNC

static SQRESULT xml_pushnewclass(HSQUIRRELVM v, const SQChar *klass, const SQChar *parent,
                                 void *tag, SQRegFunction *obj_funcs, SQBool isStatic){
	sq_pushstring(v, klass,-1);
	if(parent){
        sq_pushstring(v, parent,-1);
        if (SQ_FAILED(sq_get(v, -3)))
            return sq_throwerror(v, _SC("Missing base class."));
	}
	sq_newclass(v, parent ? SQTrue : SQFalse);
	sq_settypetag(v,-1,tag);
	if(obj_funcs) sq_insert_reg_funcs(v, obj_funcs);
	sq_newslot(v,-3, isStatic);
	return SQ_OK;
}
#define PUSH_XML_CLASS0(klass)\
    if(xml_pushnewclass(v, #klass, 0, (void*)XML_TAG(klass), klass##_methods, SQFalse) < 0) return SQ_ERROR;

#define PUSH_XML_CLASS(klass, parent)\
    if(xml_pushnewclass(v, #klass, #parent, (void*)XML_TAG(klass), klass##_methods, SQFalse) < 0) return SQ_ERROR;

#define PUSH_XML_CLASS2(klass, parent)\
    if(xml_pushnewclass(v, #klass, #parent, (void*)XML_TAG(klass), 0, SQFalse) < 0) return SQ_ERROR;

#ifdef __cplusplus
extern "C" {
#endif

#define CONST_INT(num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v, tinyxml2::num);sq_newslot(v,-3,SQTrue);

SQRESULT sqext_register_tinyxml2(HSQUIRRELVM v)
{
	sq_pushstring(v, sq_tinyxml2_TAG, -1);
	sq_newtable(v);

    CONST_INT(XML_NO_ERROR);
    CONST_INT(XML_SUCCESS);
    CONST_INT(XML_NO_ATTRIBUTE);
    CONST_INT(XML_WRONG_ATTRIBUTE_TYPE);
    CONST_INT(XML_ERROR_FILE_NOT_FOUND);
    CONST_INT(XML_ERROR_FILE_COULD_NOT_BE_OPENED);
    CONST_INT(XML_ERROR_FILE_READ_ERROR);
    CONST_INT(XML_ERROR_ELEMENT_MISMATCH);
    CONST_INT(XML_ERROR_PARSING_ELEMENT);
    CONST_INT(XML_ERROR_PARSING_ATTRIBUTE);
    CONST_INT(XML_ERROR_IDENTIFYING_TAG);
    CONST_INT(XML_ERROR_PARSING_TEXT);
    CONST_INT(XML_ERROR_PARSING_CDATA);
    CONST_INT(XML_ERROR_PARSING_COMMENT);
    CONST_INT(XML_ERROR_PARSING_DECLARATION);
    CONST_INT(XML_ERROR_PARSING_UNKNOWN);
    CONST_INT(XML_ERROR_EMPTY_DOCUMENT);
    CONST_INT(XML_ERROR_MISMATCHED_ELEMENT);
    CONST_INT(XML_ERROR_PARSING);
    CONST_INT(XML_CAN_NOT_CONVERT_TEXT);
    CONST_INT(XML_NO_TEXT_NODE);

    CONST_INT(PRESERVE_WHITESPACE);
    CONST_INT(COLLAPSE_WHITESPACE);

    sq_pushstring(v,XML_TAG(XMLNode),-1);
	sq_newclass(v,SQFalse);
	sq_settypetag(v,-1,(void*)XML_TAG(XMLNode));
    sq_insert_reg_funcs(v, XMLNode_methods);
    sq_newslot(v,-3,SQTrue);

    PUSH_XML_CLASS2(XMLComment, XMLNode);
    PUSH_XML_CLASS(XMLText, XMLNode);
    PUSH_XML_CLASS2(XMLDeclaration, XMLNode);
    PUSH_XML_CLASS2(XMLUnknown, XMLNode);
    PUSH_XML_CLASS0(XMLAttribute);
    PUSH_XML_CLASS(XMLElement, XMLNode);
    PUSH_XML_CLASS(XMLDocument, XMLNode);
    PUSH_XML_CLASS0(XMLPrinter);

    sq_newslot(v,-3,SQTrue);
    return 0;
}

#ifdef __cplusplus
}
#endif
