#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "squirrel.h"
#include "sqstdblobimpl.h"
SQ_OPT_STRING_STRLEN();

/* Lua System: Internet Server Application: FastCGI */

#define FCGI_VERSION		1

#define FCGI_HEADER_LEN		8
#define FCGI_END_REQUEST_LEN	8

#define FCGI_RESPONDER		1

#define FCGI_KEEP_CONN		1

#define FCGI_BEGIN_REQUEST	1
#define FCGI_ABORT_REQUEST	2
#define FCGI_END_REQUEST	3
#define FCGI_PARAMS		4
#define FCGI_STDIN		5
#define FCGI_STDOUT		6
#define FCGI_STDERR		7
#define FCGI_DATA		8
#define FCGI_GET_VALUES		9
#define FCGI_GET_VALUES_RESULT	10
#define FCGI_UNKNOWN_TYPE	11

#define FCGI_REQUEST_COMPLETE	0
#define FCGI_CANT_MPX_CONN	1
#define FCGI_OVERLOADED		2
#define FCGI_UNKNOWN_ROLE	3

#define FCGI_NULL_REQUEST_ID	~0U

#define FCGI_MAX_CONTENT_LEN	0xFFFF


static const char *
fcgi_decode_begin_request (HSQUIRRELVM v, const unsigned char *cp,
                           const size_t size)
{
    int role, keep_conn;

    if (size != 8) return "bad content";

    role = (*cp++ << 8);
    role |= *cp++;
    keep_conn = *cp & FCGI_KEEP_CONN;

    if (role != FCGI_RESPONDER) return "bad role";

    sq_pushbool(v, keep_conn);
    sq_setfield(v, -2, "keep_conn");

    return NULL;
}

static const char *
fcgi_decode_params (HSQUIRRELVM v, const unsigned char *cp, const size_t size)
{
    const unsigned char *endp = cp + size;

    while (cp < endp)
    {
        unsigned int name_len, value_len;

        name_len = *cp++;
        if (name_len & 0x80)
        {
            if (cp + 3 >= endp) return "bad content";

            name_len ^= 0x80;
            name_len = (name_len << 8) | *cp++;
            name_len = (name_len << 8) | *cp++;
            name_len = (name_len << 8) | *cp++;
        }
        if (cp >= endp) return "bad content";

        value_len = *cp++;
        if (value_len & 0x80)
        {
            if (cp + 3 >= endp) return "bad content";

            value_len ^= 0x80;
            value_len = (value_len << 8) | *cp++;
            value_len = (value_len << 8) | *cp++;
            value_len = (value_len << 8) | *cp++;
        }
        if (cp + name_len + value_len > endp)
            return "bad content";

        sq_pushstring(v, (const char *) cp, name_len);
        cp += name_len;
        sq_pushstring(v, (const char *) cp, value_len);
        cp += value_len;
        sq_rawset(v, -3);
    }

    return NULL;
}

static const char *
fcgi_decode_stdin (HSQUIRRELVM v, const unsigned char *cp, const size_t size)
{
    int concat = 1;

    sq_getfield(v, -1, "stdin");
    if (sq_isnil(v, -1))
    {
        sq_pop(v, 1);
        concat = 0;
    }

    sq_pushstring(v, (const char *) cp, size);
    if (concat) sq_concat(v, 2);
    sq_setfield(L, -2, "stdin");

    return NULL;
}

/*
 * Arguments: ..., channel (table)
 */
static int
fcgi_decode_record (HSQUIRRELVM v, const unsigned char *cp, const size_t size,
                    unsigned int *ready_request_id)
{
    unsigned int version, type, request_id, content_len, padding_len, len;
    int is_ready = 0;
    const char *err = NULL;

    version = *cp++;
    type = *cp++;
    request_id = (*cp++ << 8);
    request_id |= *cp++;
    content_len = (*cp++ << 8);
    content_len |= *cp++;
    padding_len = *cp++;
    cp++;  /* reserved */

    len = FCGI_HEADER_LEN + content_len + padding_len;
    if (len > size) return 0;

    sq_rawgeti(v, -1, request_id);
    if (!sq_istable(v, -1))
    {
        sq_pop(v, 1);
        sq_newtable(v);
        sq_pushvalue(v, -1);
        sq_rawseti(v, -3, request_id);

        sq_pushinteger(v, request_id);
        sq_setfield(v, -2, "id");

        sq_getfield(v, -2, "request_meta");  /* metatable from channel */
        sq_setmetatable(v, -2);

        sq_pushvalue(v, -2);  /* channel */
        sq_setfield(v, -2, "channel");
    }

    if (version != FCGI_VERSION)
        err = "bad version";
    else
    {
        switch (type)
        {
        case FCGI_BEGIN_REQUEST:
            err = fcgi_decode_begin_request(v, cp, content_len);
            break;
        case FCGI_ABORT_REQUEST:
            break;
        case FCGI_PARAMS:
            if (content_len)
                err = fcgi_decode_params(v, cp, content_len);
            break;
        case FCGI_STDIN:
            if (content_len)
                err = fcgi_decode_stdin(v, cp, content_len);
            else
                is_ready = 1;
            break;
        default:
            err = "bad type";
        }
    }

    if (err)
    {
        sq_pushstring(v, err, -1);
        sq_setfield(v, -2, "error");

        /*
        lua_pushinteger(L, version);
        lua_setfield(L, -2, "version");

        lua_pushinteger(L, type);
        lua_setfield(L, -2, "type");

        lua_pushlstring(L, cp, content_len);
        lua_setfield(L, -2, "content");
        */

        is_ready = 1;
    }
    if (is_ready)
    {
        if (*ready_request_id != FCGI_NULL_REQUEST_ID)
            sq_pushinteger(v, *ready_request_id);
        else
            sq_pushnil(v);
        sq_setfield(v, -2, "next_ready");

        *ready_request_id = request_id;
    }
    sq_pop(v, 1);
    return len;
}

/*
 * Arguments: {string | membuf_udata}, channel (table)
 * Returns: [request_id (number)]
 */
static SQRESULT sq_fastcgi_decode (HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    struct sys_buffer sb;
    const char *cp;
    unsigned int ready_request_id = FCGI_NULL_REQUEST_ID;

    if (!sys_buffer_read_init(v, 1, &sb))
        return 0;
    sq_checktype(v, 2, LUA_TTABLE);

    cp = sb.ptr.r;
    while (sb.size >= FCGI_HEADER_LEN)
    {
        const int len = fcgi_decode_record(v,
                                           (const unsigned char *) cp, sb.size, &ready_request_id);

        if (!len) break;
        cp += len;
        sb.size -= len;
    }
    sys_buffer_read_next(&sb, cp - sb.ptr.r);

    if (ready_request_id != FCGI_NULL_REQUEST_ID)
    {
        sq_pushinteger(v, ready_request_id);
        return 1;
    }
    return 0;
}


/*
 * Arguments: membuf_udata, request_id (number), prev_offset (number), string
 * Returns: offset (number)
 */
static SQRESULT sq_fastcgi_encode (HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQBlob *blob = NULL;
    SQObjectType ptype = sq_gettype(v, 2);
    if(ptype == OT_INSTANCE){
            if(SQ_FAILED(sq_getinstanceup(v,2,(SQUserPointer*)&blob,(SQUserPointer)SQBlob::SQBlob_TAG)))
                return sq_throwerror(v,_SC("invalid type tag"));
            if(!blob || !blob->IsValid())
                return sq_throwerror(v,_SC("the blob is invalid"));
    }
    SQ_GET_INTEGER(v, 3, request_id);
    SQ_GET_INTEGER(v, 4, prev_offset);
    SQ_GET_STRING(v, 5, s);
    unsigned char *cp;
    unsigned char *lenp;
    SQInteger prev_len = 0, prev_padding_len = 0;
    SQInteger data_len, padding_len;

    if (s_size > FCGI_MAX_CONTENT_LEN) {
        return sq_throwerror(v,_SC("too long"));
    }

    sys_buffer_write_init(v, 1, &sb, NULL, 0);

    if (prev_offset)
    {
        cp = (unsigned char *) sb.ptr.w - prev_offset;
        if (cp[-1] == (unsigned char) request_id
                && cp[-2] == (unsigned char) (request_id >> 8))
        {
            prev_len = (cp[0] << 8) | cp[1];
            prev_padding_len = cp[2];
        }
        else
        {
            prev_offset = 0;
        }
    }

    if (!len)
    {
        data_len = 2 * FCGI_HEADER_LEN + FCGI_END_REQUEST_LEN;
        prev_offset = 0;
    }
    else
    {
        data_len = len;
        if (!prev_offset || (prev_len + len) > FCGI_MAX_CONTENT_LEN)
        {
            data_len += FCGI_HEADER_LEN;
            prev_offset = 0;
        }
        else
        {
            data_len -= prev_padding_len;
        }
    }

    if (data_len <= 0)
    {
        padding_len = prev_padding_len - len;  /* fits to previous padding */
        data_len = 0;
    }
    else
    {
        padding_len = 8 - data_len % 8;
        if (padding_len == 8) padding_len = 0;

        data_len += padding_len;
        if (!sys_buffer_write_next(v, &sb, NULL, data_len))
            return 0;
    }

    /* buffer may be re-allocated */
    cp = (unsigned char *) sb.ptr.w;

    if (prev_offset)
    {
        lenp = cp - prev_offset;
        cp -= prev_padding_len;
        memcpy(cp, s, len);  /* concat to previous record */
        cp += len;
        len += prev_len;
    }
    else
    {
        *cp++ = FCGI_VERSION;
        *cp++ = FCGI_STDOUT;
        *cp++ = (unsigned char) (request_id >> 8);
        *cp++ = (unsigned char) request_id;
        lenp = cp++;  /* content_len */
        cp++;
        cp++;  /* padding_len */
        *cp++ = 0;  /* reserved */

        if (len)
        {
            memcpy(cp, s, len);
            cp += len;
        }
        else
        {
            *cp++ = FCGI_VERSION;
            *cp++ = FCGI_END_REQUEST;
            *cp++ = (unsigned char) (request_id >> 8);
            *cp++ = (unsigned char) request_id;
            *cp++ = 0;  /* content_len */
            *cp++ = FCGI_END_REQUEST_LEN;
            *cp++ = 0;  /* padding_len */
            *cp++ = 0;  /* reserved */

            memset(cp, 0, FCGI_END_REQUEST_LEN);
        }
    }

    lenp[0] = (unsigned char) (len >> 8);
    lenp[1] = (unsigned char) len;
    lenp[2] = (unsigned char) padding_len;

    if (padding_len)
    {
        memset(cp, 0, padding_len);
        cp += padding_len;
    }

    sys_buffer_write_done(v, &sb, NULL, data_len);

    sq_pushinteger(v, cp - lenp);
    return 1;
}


#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_fastcgi_##name,nparams,tycheck}
static SQRegFunction sq_fastcgi_methods[] =
{
    _DECL_FUNC(encode,  -1, _SC("x")),
    _DECL_FUNC(decode,  1, _SC("x")),
    {0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_fastcgi(HSQUIRRELVM v)
    {
        //add a namespace sqfastcgi
        sq_pushstring(v, sq_fastcgi_TAG,-1);
        sq_newclass(v, SQFalse);
        sq_insert_reg_funcs(v, sq_fastcgi_methods);
        sq_newslot(v,-3,SQTrue); //add sqfastcgi table to the root table

        return SQ_OK;
    }

#ifdef __cplusplus
}
#endif
