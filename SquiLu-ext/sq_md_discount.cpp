
#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include "sqstdblobimpl.h"
extern "C" {
#include "markdown.h"
}

static void mywrite(char const *s,int len,void *blob)
{
	SQBlob *b= (SQBlob*)blob;
	b->Write(s,len);
}

static SQRESULT sq_markdown2html(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, str);
    SQ_OPT_INTEGER(v, 3, len, str_size);
    SQ_OPT_INTEGER(v, 4, flags, 0);
    if(len > str_size || len < 0) return sq_throwerror(v, _SC("invalid parameter value (%d)"), len);
    SQBlob b(0, 8192);
    markdown_str(mkd_string(str, len, 0), &b ,mywrite, flags);
    sq_pushstring(v, (SQChar*)b.GetBuf(), b.Len());
    return 1;
}


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_markdown(HSQUIRRELVM v)
{
    sq_insertfunc(v, _SC("markdown2html"), sq_markdown2html, -2, _SC(".sii"), SQTrue);
    sq_pushconsttable(v);
#define INT_CONST(num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);
    INT_CONST(MKD_NOLINKS);
    INT_CONST(MKD_NOIMAGE);
    INT_CONST(MKD_NOPANTS);
    INT_CONST(MKD_NOHTML);
    INT_CONST(MKD_STRICT);
    INT_CONST(MKD_TAGTEXT);
    INT_CONST(MKD_NO_EXT);
    INT_CONST(MKD_CDATA);
    INT_CONST(MKD_NOSUPERSCRIPT);
    INT_CONST(MKD_NORELAXED);
    INT_CONST(MKD_NOTABLES);
    INT_CONST(MKD_NOSTRIKETHROUGH);
    INT_CONST(MKD_TOC);
    INT_CONST(MKD_1_COMPAT);
    INT_CONST(MKD_AUTOLINK);
    INT_CONST(MKD_SAFELINK);
    INT_CONST(MKD_NOHEADER);
    INT_CONST(MKD_TABSTOP);
    INT_CONST(MKD_NODIVQUOTE);
    INT_CONST(MKD_NOALPHALIST);
    INT_CONST(MKD_NODLIST);
    INT_CONST(MKD_EXTRA_FOOTNOTE);
    INT_CONST(MKD_NOSTYLE);
    INT_CONST(MKD_NODLDISCOUNT);
    INT_CONST(MKD_DLEXTRA);
    INT_CONST(MKD_FENCEDCODE);
    INT_CONST(MKD_IDANCHOR);
    INT_CONST(MKD_GITHUBTAGS);
    INT_CONST(MKD_URLENCODEDANCHOR);

    sq_poptop(v); //remove const table
    return 0;
}

#ifdef __cplusplus
}
#endif
