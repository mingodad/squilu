
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
    if(len > str_size || len < 0) return sq_throwerror(v, _SC("invalid parameter value (%d)"), len);
    SQBlob b(0, 8192);
    markdown_str(mkd_string(str, len, 0), &b ,mywrite, 0);
    sq_pushstring(v, (SQChar*)b.GetBuf(), b.Len());
    return 1;
}


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_markdown(HSQUIRRELVM v)
{
    sq_insertfunc(v, _SC("markdown2html"), sq_markdown2html, -2, _SC(".si"), SQTrue);
    return 0;
}

#ifdef __cplusplus
}
#endif
