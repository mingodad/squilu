#ifdef WITH_DNS_SD

#include "squirrel.h"
#include <dns_sd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"

#include "dynamic_library.h"

/*SquiLu
local dns_sd_functions = [
    ["void", "TXTRecordCreate", @"TXTRecordRef *txtRecord, uint16_t bufferLen,
                        void *buffer"],
    ["int", "TXTRecordSetValue", @"TXTRecordRef *txtRecord,
                    const char *key, uint8_t valueSize, const void *value"],
    ["void", "TXTRecordDeallocate", "TXTRecordRef *txtRecord"],
    ["uint16_t", "TXTRecordGetLength", "const TXTRecordRef *txtRecord"],
    ["const void *", "TXTRecordGetBytesPtr", "const TXTRecordRef *txtRecord"],

    //next entry should be the last one
    //to make valid the test made on load_libpq function
    ["DNSServiceErrorType", "DNSServiceRegister", @"DNSServiceRef *sdRef,
                    DNSServiceFlags flags, uint32_t interfaceIndex,
                    const char *name, const char *regtype,
                    const char *domain, const char *host,
                    uint16_t port, uint16_t txtLen,
                    const void *txtRecord, DNSServiceRegisterReply callBack,
                    void *context "],
];

function write_dns_sd_functions_declaration(){
    foreach(k,v in dns_sd_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl" + v[1] + " = 0;");
    }
}

function write_dns_sd_functions_load(){
    foreach(k,v in dns_sd_functions){
        putsnl("dl" + v[1] + " = (" + v[1] + "_t) libdns_sd.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary libdns_sd;

//@write_dns_sd_functions_declaration();
// generated-code:begin
typedef void (*TXTRecordCreate_t)(TXTRecordRef *txtRecord, uint16_t bufferLen,
                        void *buffer);
static TXTRecordCreate_t dlTXTRecordCreate = 0;
typedef int (*TXTRecordSetValue_t)(TXTRecordRef *txtRecord,
                    const char *key, uint8_t valueSize, const void *value);
static TXTRecordSetValue_t dlTXTRecordSetValue = 0;
typedef void (*TXTRecordDeallocate_t)(TXTRecordRef *txtRecord);
static TXTRecordDeallocate_t dlTXTRecordDeallocate = 0;
typedef uint16_t (*TXTRecordGetLength_t)(const TXTRecordRef *txtRecord);
static TXTRecordGetLength_t dlTXTRecordGetLength = 0;
typedef const void * (*TXTRecordGetBytesPtr_t)(const TXTRecordRef *txtRecord);
static TXTRecordGetBytesPtr_t dlTXTRecordGetBytesPtr = 0;
typedef DNSServiceErrorType (*DNSServiceRegister_t)(DNSServiceRef *sdRef,
                    DNSServiceFlags flags, uint32_t interfaceIndex,
                    const char *name, const char *regtype,
                    const char *domain, const char *host,
                    uint16_t port, uint16_t txtLen,
                    const void *txtRecord, DNSServiceRegisterReply callBack,
                    void *context );
static DNSServiceRegister_t dlDNSServiceRegister = 0;
// generated-code:end

#ifdef WIN32
#define LIDNS_SD_NAME "libdns_sd.dll"
#else
#define LIDNS_SD_NAME "libdns_sd.so"
#endif

static bool load_libdns_sd()
{
    if(dlDNSServiceRegister) return true;
    if(libdns_sd.open(LIDNS_SD_NAME))
    {
        //@write_dns_sd_functions_load();
// generated-code:begin
dlTXTRecordCreate = (TXTRecordCreate_t) libdns_sd.dlsym("TXTRecordCreate");
if(!dlTXTRecordCreate) return false;
dlTXTRecordSetValue = (TXTRecordSetValue_t) libdns_sd.dlsym("TXTRecordSetValue");
if(!dlTXTRecordSetValue) return false;
dlTXTRecordDeallocate = (TXTRecordDeallocate_t) libdns_sd.dlsym("TXTRecordDeallocate");
if(!dlTXTRecordDeallocate) return false;
dlTXTRecordGetLength = (TXTRecordGetLength_t) libdns_sd.dlsym("TXTRecordGetLength");
if(!dlTXTRecordGetLength) return false;
dlTXTRecordGetBytesPtr = (TXTRecordGetBytesPtr_t) libdns_sd.dlsym("TXTRecordGetBytesPtr");
if(!dlTXTRecordGetBytesPtr) return false;
dlDNSServiceRegister = (DNSServiceRegister_t) libdns_sd.dlsym("DNSServiceRegister");
if(!dlDNSServiceRegister) return false;
// generated-code:end
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

static const SQChar *DNS_SD_TAG = _SC("DNS_SD");

static SQRESULT sq_dns_sd_ServiceRegister(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
    TXTRecordRef txtRecord;
	DNSServiceRef sdref;
	DNSServiceErrorType err;
	int ret, result = 0;

    SQ_GET_STRING(v, 2, deviceName);
    SQ_GET_STRING(v, 3, serviceType);

	SQInteger rec_count = sq_getsize(v, 4);

	dlTXTRecordCreate(&txtRecord, 0, NULL);

	for(int i=0; i < rec_count; ++i){
        sq_pushinteger(v, i);
        if(SQ_SUCCEEDED(sq_get(v, 4))){
            if((sq_gettype(v, 5) == OT_ARRAY) && (sq_getsize(v, 5) == 2)) {
                sq_pushinteger(v, 0);
                sq_get(v, 5);
                SQ_GET_STRING(v, 6, key);
                sq_pushinteger(v, 1);
                sq_get(v, 5);
                SQ_GET_STRING(v, 7, value);
                sq_pop(v, 2); //remove key,value

                ret = dlTXTRecordSetValue(&txtRecord, key, value_size, value);
                if(ret != kDNSServiceErr_NoError) {
                    sq_throwerror(v, _SC("Array with key, value expected"));
                    result = SQ_ERROR;
                }
            } else {
                sq_throwerror(v, _SC("Array with key, value expected"));
                result = SQ_ERROR;
            }
            sq_pop(v, 1); //remove value
            if(result == SQ_ERROR) {
                break;
            }
        }
    }

    if(result == 0) {
        err = dlDNSServiceRegister(&sdref,0,0,deviceName,serviceType,NULL,NULL,htons(0),
                                dlTXTRecordGetLength(&txtRecord),dlTXTRecordGetBytesPtr(&txtRecord),NULL,NULL);
        if(err != kDNSServiceErr_NoError) {
            sq_throwerror(v, _SC("DNS_SD error registering service %d"),err);
            result = SQ_ERROR;
        }
    }
    dlTXTRecordDeallocate(&txtRecord);

	return result;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_dns_sd_##name,nparams,tycheck}
static SQRegFunction sq_dns_sd_methods[] =
{
	_DECL_FUNC(ServiceRegister,  4, _SC(".ssa")),
	{0,0}
};
#undef _DECL_FUNC


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_DNS_SD(HSQUIRRELVM v)
{
    if(load_libdns_sd())
    {
        sq_pushstring(v,_SC("dns_sd"),-1);
        sq_newtable(v);
        sq_insert_reg_funcs(v, sq_dns_sd_methods);
        sq_newslot(v,-3,SQTrue);
    }
    else
    {
        return sq_throwerror(v, _SC("Failed to load libdns_sd !"));
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif //WITH_DNS_SD
