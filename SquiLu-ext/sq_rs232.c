/*
 * Copyright (c) 2011 Petr Stetiar <ynezz@true.cz>, Gaben Ltd.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Ported to Squirrel/SquiLu by Domingo Alvarez Duarte
 */

#include "squirrel.h"
#include <string.h>

#include <string.h>
#include <stdlib.h>

SQ_OPT_STRING_STRLEN();

#include "librs232/rs232.h"
#include "librs232/rs232.c"

#ifdef WIN32
#include "librs232/rs232_windows.h"
#include "librs232/rs232_windows.c"
#else
#include "librs232/rs232_posix.h"
#include "librs232/rs232_posix.c"
#endif

#define MODULE_TIMESTAMP __DATE__ " " __TIME__
#define MODULE_VERSION "1.0.3"
#define MODULE_BUILD "$Id: luars232.c 15 2011-02-23 09:02:20Z sp $"
#define MODULE_COPYRIGHT "Copyright (c) 2011 Petr Stetiar <ynezz@true.cz>, Gaben Ltd."

static const SQChar SQRS232_TAG[] = _SC("SqRs232");

static SQRESULT get_rs232_instance(HSQUIRRELVM v, SQInteger idx, struct rs232_port_t **self){
    SQRESULT _rc_;
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)self,(void*)SQRS232_TAG)) < 0) return _rc_;
	if(!*self) return sq_throwerror(v, _SC("rs232 is closed"));
	return _rc_;
}

#define GET_sq_rs232_INSTANCE(v, idx) struct rs232_port_t *self=NULL; \
	if((_rc_ = get_rs232_instance(v,idx,&self)) < 0) return _rc_;

static SQRESULT sq_rs232_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v) {
    struct rs232_port_t *self = (struct rs232_port_t *)p;
    if(self) rs232_end(self);
    return 0;
}

/*
** Creates a new SqRs232.
*/
static SQRESULT sq_rs232_constructor (HSQUIRRELVM v) {
	sq_setinstanceup(v, 1, 0);
	sq_setreleasehook(v, 1, sq_rs232_release_hook);
  return 1;
}

static SQRESULT sq_rs232_open(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    struct rs232_port_t *self=NULL;
    if((_rc_ = sq_getinstanceup(v,1,(SQUserPointer*)&self,(void*)SQRS232_TAG)) < 0) return _rc_;
	SQ_GET_STRING(v, 2, device);
	if(self != NULL) sq_throwerror(v, _SC("rs232 already opened"));
	self = rs232_init();
	if(self == NULL) sq_throwerror(v, rs232_strerror(RS232_ERR_CONFIG));
	rs232_set_device(self, (char*)device);
	int ret = rs232_open(self);
    if (ret > RS232_ERR_NOERROR) {
		free(self->pt);
		free(self);
		return sq_throwerror(v, rs232_strerror(ret));
	}
    sq_setinstanceup(v, 1, self);
	return 0;
}

static SQRESULT sq_rs232_close(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
	GET_sq_rs232_INSTANCE(v, 1);
	_rc_ = sq_rs232_release_hook(self, 0, v);
	sq_setinstanceup(v, 1, 0); //next calls will fail with "rs232 port already closed"
	sq_pushinteger(v, _rc_);
	return 1;
}

static SQRESULT sq_rs232_read(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
	GET_sq_rs232_INSTANCE(v, 1);
	SQ_GET_INTEGER(v, 2, len);
	unsigned int bytes_read = 0;
	SQChar *data = NULL;

	switch (_top_) {
	case 2:{
            data = sq_getscratchpad(v, len);
            _rc_ = rs232_read(self, (unsigned char *)data, len, &bytes_read);
        }
		break;
	case 3:
	case 4:{
            SQ_GET_INTEGER(v, 3, timeout);
            SQ_OPT_BOOL(v, 4, forced, SQFalse);
            data = sq_getscratchpad(v, len);
            if(forced) _rc_ = rs232_read_timeout_forced(self, (unsigned char *)data, len, &bytes_read, timeout);
            else _rc_ = rs232_read_timeout(self, (unsigned char *)data, len, &bytes_read, timeout);
        }
		break;
	default:
		return sq_throwerror(v, _SC("wrong number of parameters %d"), _top_-1);
	}

	DBG("rc=%d hex='%s' bytes_read=%d\n",
	    _rc_, rs232_hex_dump(data, bytes_read), bytes_read);

    if(bytes_read > 0) sq_pushstring(v, data, bytes_read);
    else sq_pushinteger(v, _rc_);
	return 1;
}

static SQRESULT sq_rs232_write(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
	GET_sq_rs232_INSTANCE(v, 1);
    SQ_GET_STRING(v, 2, data);
	unsigned int wlen = 0;

	switch (_top_) {
	case 2:{
            _rc_ = rs232_write(self, (unsigned char*) data, (unsigned int) data_size, &wlen);
        }
		break;
	case 3:{
            SQ_GET_INTEGER(v, 3, timeout);
            _rc_ = rs232_write_timeout(self, (unsigned char*) data, (unsigned int) data_size, &wlen, timeout);
        }
		break;
	default:
		return sq_throwerror(v, _SC("wrong number of parameters %d"), _top_-1);
	}

    if(_rc_ > RS232_ERR_NOERROR) return sq_throwerror(v, rs232_strerror(_rc_));
    sq_pushinteger(v, wlen);
	return 1;
}

static SQRESULT sq_rs232_in_qeue(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
	GET_sq_rs232_INSTANCE(v, 1);
	unsigned int in_bytes;
	int rc = rs232_in_qeue(self, &in_bytes);
    if(rc > RS232_ERR_NOERROR) return sq_throwerror(v, rs232_strerror(rc));
	sq_pushinteger(v, in_bytes);
	return 1;
}

static SQRESULT sq_rs232_in_qeue_clear(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
	GET_sq_rs232_INSTANCE(v, 1);
	rs232_in_qeue_clear(self);
	return 0;
}

static SQRESULT sq_rs232_flush(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
	GET_sq_rs232_INSTANCE(v, 1);
	sq_pushinteger(v, rs232_flush(self));
	return 1;
}

static SQRESULT sq_rs232_device(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
	GET_sq_rs232_INSTANCE(v, 1);
	const char *ret = rs232_get_device(self);
	if (ret == NULL) sq_pushnull(v);
	else sq_pushstring(v, ret, -1);
	return 1;
}

static SQRESULT sq_rs232_fd(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
	GET_sq_rs232_INSTANCE(v, 1);
	sq_pushinteger(v, rs232_fd(self));
	return 1;
}

static SQRESULT sq_rs232_strerror(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, error_code);
    sq_pushstring(v, rs232_strerror(error_code), -1);
	return 1;
}

static SQRESULT sq_rs232__tostring(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    GET_sq_rs232_INSTANCE(v, 1);
    sq_pushstring(v, rs232_to_string(self), -1);
	return 1;
}

#define FN_GET_SET_PORT(type, sufix) \
	static SQRESULT sq_rs232_##type##sufix(HSQUIRRELVM v) \
	{ \
        SQ_FUNC_VARS(v);\
        GET_sq_rs232_INSTANCE(v, 1);\
        if(_top_ > 1) {\
            SQ_GET_INTEGER(v, 2, iparam);\
            sq_pushinteger(v, rs232_set_##type(self, (unsigned int)iparam));\
        }\
        else sq_pushinteger(v, rs232_get_##type(self));\
        return 1;\
	}

#define FN_GET_PORT_STRING(type, sufix) \
	static SQRESULT sq_rs232_##type##sufix##_tostring(HSQUIRRELVM v) \
	{ \
        SQ_FUNC_VARS(v);\
        GET_sq_rs232_INSTANCE(v, 1);\
        SQ_OPT_INTEGER(v, 2, iparam, -1);\
		const char *ret = rs232_str##type(iparam == -1 ? rs232_get_##type(self) : (unsigned int) iparam); \
		if (ret == NULL) sq_pushnull(v); \
		else sq_pushstring(v, ret, -1); \
		return 1; \
	}

#define FN_GET_SET_STR_PORT(type, sufix) FN_GET_SET_PORT(type, sufix); FN_GET_PORT_STRING(type, sufix)

FN_GET_SET_STR_PORT(baud, _rate);
FN_GET_SET_STR_PORT(data, _bits);
FN_GET_SET_STR_PORT(stop, _bits);
FN_GET_SET_STR_PORT(parity,);
FN_GET_SET_STR_PORT(flow, _control);
FN_GET_SET_STR_PORT(dtr,);
FN_GET_SET_STR_PORT(rts,);


#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name), sq_rs232_##name,nparams,tycheck}
static SQRegFunction rs232_methods[] =
{
    _DECL_FUNC(constructor,1,_SC("x")),
    _DECL_FUNC(open,2,_SC("xs")),
    _DECL_FUNC(close,1,_SC("x")),
    _DECL_FUNC(read,-2,_SC("xiib")),
    _DECL_FUNC(write,-2,_SC("xsi")),
    _DECL_FUNC(flush,1,_SC("x")),
    _DECL_FUNC(in_qeue,1,_SC("x")),
    _DECL_FUNC(in_qeue_clear,1,_SC("x")),
    _DECL_FUNC(device,1,_SC("x")),
    _DECL_FUNC(fd,1,_SC("x")),
    _DECL_FUNC(strerror,2,_SC(".i")),
    _DECL_FUNC(_tostring,1,_SC("x")),
	/* baud */
    _DECL_FUNC(baud_rate,-1,_SC("xi")),
    _DECL_FUNC(baud_rate_tostring,-1,_SC(".i")),
	/* data */
    _DECL_FUNC(data_bits,-1,_SC("xi")),
    _DECL_FUNC(data_bits_tostring,-1,_SC(".i")),
	/* stop */
    _DECL_FUNC(stop_bits,-1,_SC("xi")),
    _DECL_FUNC(stop_bits_tostring,-1,_SC(".i")),
	/* parity */
    _DECL_FUNC(parity,-1,_SC("xi")),
    _DECL_FUNC(parity_tostring,-1,_SC(".i")),
	/* flow */
    _DECL_FUNC(flow_control,-1,_SC("xi")),
    _DECL_FUNC(flow_control_tostring,-1,_SC(".i")),
	/* dtr */
    _DECL_FUNC(dtr,-1,_SC("xi")),
    _DECL_FUNC(dtr_tostring,-1,_SC(".i")),
	/* rts */
    _DECL_FUNC(rts,-1,_SC("xi")),
    _DECL_FUNC(rts_tostring,-1,_SC(".i")),
    {0,0}
};

#define INT_CONST(v,num) 	sq_pushstring(v,_SC(#num),-1);sq_pushinteger(v,num);sq_newslot(v,-3,SQTrue);

SQRESULT sqext_register_rs232(HSQUIRRELVM v)
{
    sq_pushstring(v,SQRS232_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)SQRS232_TAG);
    sq_insert_reg_funcs(v, rs232_methods);

	INT_CONST(v, RS232_BAUD_300 );
	INT_CONST(v, RS232_BAUD_2400 );
	INT_CONST(v, RS232_BAUD_4800 );
	INT_CONST(v, RS232_BAUD_9600 );
	INT_CONST(v, RS232_BAUD_19200 );
	INT_CONST(v, RS232_BAUD_38400 );
	INT_CONST(v, RS232_BAUD_57600 );
	INT_CONST(v, RS232_BAUD_115200 );
	INT_CONST(v, RS232_BAUD_460800 );
	/* databits */
	INT_CONST(v, RS232_DATA_5 );
	INT_CONST(v, RS232_DATA_6 );
	INT_CONST(v, RS232_DATA_7 );
	INT_CONST(v, RS232_DATA_8 );
	/* stop bits */
	INT_CONST(v, RS232_STOP_1 );
	INT_CONST(v, RS232_STOP_2 );
	/* parity */
	INT_CONST(v, RS232_PARITY_NONE );
	INT_CONST(v, RS232_PARITY_ODD );
	INT_CONST(v, RS232_PARITY_EVEN );
	/* flow */
	INT_CONST(v, RS232_FLOW_OFF );
	INT_CONST(v, RS232_FLOW_HW );
	INT_CONST(v, RS232_FLOW_XON_XOFF );
	/* DTR and RTS */
	INT_CONST(v, RS232_DTR_ON );
	INT_CONST(v, RS232_DTR_OFF );
	INT_CONST(v, RS232_RTS_ON );
	INT_CONST(v, RS232_RTS_OFF );
	/* errors */
	INT_CONST(v, RS232_ERR_NOERROR );
	INT_CONST(v, RS232_ERR_UNKNOWN );
	INT_CONST(v, RS232_ERR_OPEN );
	INT_CONST(v, RS232_ERR_CLOSE );
	INT_CONST(v, RS232_ERR_FLUSH );
	INT_CONST(v, RS232_ERR_CONFIG );
	INT_CONST(v, RS232_ERR_READ );
	INT_CONST(v, RS232_ERR_WRITE );
	INT_CONST(v, RS232_ERR_SELECT );
	INT_CONST(v, RS232_ERR_TIMEOUT );
	INT_CONST(v, RS232_ERR_IOCTL );
	INT_CONST(v, RS232_ERR_PORT_CLOSED );

    sq_newslot(v,-3,SQTrue);
    return 1;
}

