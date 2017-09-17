
#include "squirrel.h"
#include <string.h>
#include <stdio.h>
#include "sqstdblobimpl.h"
extern "C" {
#include "render_html.h"
}

static void mywrite(const MD_CHAR *s,MD_SIZE len,void *blob)
{
	SQBlob *b= (SQBlob*)blob;
	b->Write(s,len);
}

static SQRESULT sq_markdown2html(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, str);
    SQ_OPT_INTEGER(v, 3, len, str_size);
    SQ_OPT_INTEGER(v, 4, parser_flags, MD_DIALECT_GITHUB | MD_FLAG_PERMISSIVEATXHEADERS);
    SQ_OPT_INTEGER(v, 5, renderer_flags, 0);
    if(len > str_size || len < 0) return sq_throwerror(v, _SC("invalid parameter value (%d)"), len);
    /* Input size is good estimation of output size. Add some more reserve to
     * deal with the HTML header/footer and tags. */
    SQBlob b(0, len + (len/8 + 64));
    int ret = md_render_html((const MD_CHAR*)str, len, mywrite,
                (void*) &b, parser_flags, renderer_flags);
    if(ret != 0) return sq_throwerror(v, _SC("markdown2html parsing failed"));
    sq_pushstring(v, (SQChar*)b.GetBuf(), b.Len());
    return 1;
}


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_markdown(HSQUIRRELVM v)
{
    sq_insertfunc(v, _SC("markdown2html"), sq_markdown2html, -2, _SC(".siii"), SQTrue);
    sq_pushconsttable(v);
#define INT_CONST(num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);
    INT_CONST(MD_FLAG_COLLAPSEWHITESPACE);
    INT_CONST(MD_FLAG_PERMISSIVEATXHEADERS);
    INT_CONST(MD_FLAG_PERMISSIVEURLAUTOLINKS);
    INT_CONST(MD_FLAG_PERMISSIVEEMAILAUTOLINKS);
    INT_CONST(MD_FLAG_NOINDENTEDCODEBLOCKS);
    INT_CONST(MD_FLAG_NOHTMLBLOCKS);
    INT_CONST(MD_FLAG_NOHTMLSPANS);
    INT_CONST(MD_FLAG_TABLES);
    INT_CONST(MD_FLAG_STRIKETHROUGH);
    INT_CONST(MD_FLAG_PERMISSIVEWWWAUTOLINKS);
    INT_CONST(MD_FLAG_PERMISSIVEAUTOLINKS);
    INT_CONST(MD_FLAG_NOHTML);
    INT_CONST(MD_DIALECT_COMMONMARK);
    INT_CONST(MD_DIALECT_GITHUB);

    INT_CONST(MD_RENDER_FLAG_DEBUG);
    INT_CONST(MD_RENDER_FLAG_VERBATIM_ENTITIES);

    sq_poptop(v); //remove const table
    return 0;
}

#ifdef __cplusplus
}
#endif
