#include "squirrel.h"
#include "jni.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqstdblobimpl.h"

#include "dynamic_library.h"

/*SquiLu
local jni_functions = [
    ["jint", "JNI_GetDefaultJavaVMInitArgs", "void *args"],
    ["jint", "JNI_CreateJavaVM", "JavaVM **pvm, void **penv, void *args"],
];

function write_jni_functions_declaration(){
    foreach(k,v in jni_functions) {
        putsnl("typedef " + v[0] + " (*" + v[1] + "_t)(" + v[2] + ");");
        putsnl("static " + v[1] + "_t dl_" + v[1] + " = 0;");
    }
}

function write_jni_functions_load(){
    foreach(k,v in jni_functions){
        putsnl("dl_" + v[1] + " = (" + v[1] + "_t) libjni.dlsym(\"" + v[1] + "\");");
        putsnl("if(!dl_" + v[1] + ") return false;");
    }
}
SquiLu*/

static DynamicLibrary libjni;

//@write_jni_functions_declaration();
// generated-code:begin
typedef jint (*JNI_GetDefaultJavaVMInitArgs_t)(void *args);
static JNI_GetDefaultJavaVMInitArgs_t dl_JNI_GetDefaultJavaVMInitArgs = 0;
typedef jint (*JNI_CreateJavaVM_t)(JavaVM **pvm, void **penv, void *args);
static JNI_CreateJavaVM_t dl_JNI_CreateJavaVM = 0;
// generated-code:end


static const SQChar *jvm_lib_name =
#ifdef WIN32
 _SC("jvm.dll");
#else
 _SC("/usr/lib/jvm/default-java/jre/lib/i386/client/libjvm.so");
#endif

static bool load_libjni()
{
    if(dl_JNI_CreateJavaVM) return true;
    if(libjni.open(jvm_lib_name))
    {
        //@write_jni_functions_load();
// generated-code:begin
dl_JNI_GetDefaultJavaVMInitArgs = (JNI_GetDefaultJavaVMInitArgs_t) libjni.dlsym("JNI_GetDefaultJavaVMInitArgs");
if(!dl_JNI_GetDefaultJavaVMInitArgs) return false;
dl_JNI_CreateJavaVM = (JNI_CreateJavaVM_t) libjni.dlsym("JNI_CreateJavaVM");
if(!dl_JNI_CreateJavaVM) return false;
// generated-code:end
        return true;
    }
    return false;
}

/*
Java Type 	JNI Type 	machine dependent
C/C++ typedef 	Signature	Call...Method
Get...Field
boolean	jboolean	unsigned char 	Z	Boolean
byte 	jbyte 	signed char 	B	Byte
char 	jchar 	unsigned short	C	Char
short 	jshort 	short 	S	Short
int 	jint 	int 	I	Int
long 	jlong 	long 	J	Long
float 	jfloat 	float 	F	Float
double 	jdouble 	double 	D	Double
void 	void 	V	Void
nonprimitive 	jobject 	*...	L...;	Object

Examples
method definition	signature
int m1 ()	()I
double m2 (long l, char c)	(JC)D
void m3 (String s, int[] a)	(Ljava/lang/String;[I)V
String m4 (boolean b)	(Z)Ljava/lang/String;
Object m4 (BigDecimal b)	(Ljava/math/BigDecimal;)Ljava/lang/Object;
*/

#define JAVAVM_MAXOPTIONS 32
#define JAVAVM_JNIVERSION JNI_VERSION_1_6

static JavaVMOption _jvm_options[JAVAVM_MAXOPTIONS];
static JavaVM *_jvm_vm = 0;
static JavaVMInitArgs _jvm_args;
static JNIEnv *_jvm_env = 0;

static const SQChar *Java_TAG = _SC("Java");

static SQRESULT check_jnienv(HSQUIRRELVM v){
	if(!_jvm_env) return sq_throwerror(v, _SC("java vm is closed"));
	return SQ_OK;
}

#define CHECK_JNIENV() \
	if((_rc_ = check_jnienv(v)) < 0) return _rc_;

static const SQChar *JavaClass_TAG = _SC("_JavaClass_");
static const SQChar *_java_class_methods_key = _SC("_class_methods");

#define GET_java_class_INSTANCE_AT(idx) \
	jclass self=NULL; \
	if((_rc_ = sq_getinstanceup(v,idx,(SQUserPointer*)&self,(void*)JavaClass_TAG)) < 0) return _rc_;

#define GET_java_class_INSTANCE() GET_java_class_INSTANCE_AT(1)

static SQRESULT sq_java_class_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	jclass self = ((jclass)p);
	if (_jvm_env && self) _jvm_env->DeleteGlobalRef(self);
	return 0;
}

static SQRESULT sq_java_class_GetMethodID(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	CHECK_JNIENV();
	GET_java_class_INSTANCE();
	SQ_GET_STRING(v, 2, method_name);
	SQ_GET_STRING(v, 3, method_signature);
	jmethodID mid = _jvm_env->GetMethodID(self, method_name, method_signature);
    if(mid){
        sq_pushstring(v, _java_class_methods_key, -1);
        if(sq_get(v, 1) == SQ_OK){
            if(sq_gettype(v, -1) != OT_TABLE){
                sq_poptop(v);
                sq_newtable(v);
                sq_pushstring(v, _java_class_methods_key, -1);
                sq_push(v, -2);
                sq_set(v, 1);
            }
            sq_push(v, 2);
            sq_pushuserpointer(v, mid);
            sq_set(v, -3);
            sq_pushbool(v, SQTrue);
        }
        else return sq_throwerror(v, _SC("instance field not found (%s)"), _java_class_methods_key);
    }
    else sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_java_class__get(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	CHECK_JNIENV();
	GET_java_class_INSTANCE();
	SQ_GET_STRING(v, 2, method_name);
    sq_pushbool(v, SQFalse);
	return 1;
}

static SQRESULT sq_java_class_currentTimeMillis(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	CHECK_JNIENV();
	GET_java_class_INSTANCE();
	jmethodID mid = _jvm_env->GetStaticMethodID(self, "currentTimeMillis", "()J");
	if(mid){
	    jlong ms = _jvm_env->CallLongMethod(self, mid, NULL);
	    sq_pushinteger(v, ms);
	}
    else sq_pushbool(v, SQFalse);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck,isStatic) {_SC(#name),  sq_java_class_##name,nparams,tycheck,isStatic}
static SQRegFunction sq_java_class_methods[] =
{
	_DECL_FUNC(GetMethodID,  3, _SC("xss"),SQFalse),
	_DECL_FUNC(_get,  2, _SC("xs"),SQFalse),
	_DECL_FUNC(currentTimeMillis,  1, _SC("x"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

static SQRESULT sq_java_open(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    if(_jvm_vm) return sq_throwerror(v, _SC("java vm already opened"));

    if(load_libjni())
    {
        /* Create Java VM */
        int num_options=0;
        _jvm_args.version = JAVAVM_JNIVERSION;
        dl_JNI_GetDefaultJavaVMInitArgs(&_jvm_args);
        _jvm_args.options = _jvm_options;
        _jvm_args.nOptions = num_options;
        _jvm_args.ignoreUnrecognized = JNI_TRUE;
        int res = dl_JNI_CreateJavaVM(&_jvm_vm, (void**) &_jvm_env, &_jvm_args);
        if (res < 0) return sq_throwerror(v, _SC("error creating Java VM: %d"), res);
    }
    else return sq_throwerror(v, _SC("Failed to load %s !"), jvm_lib_name);

    return 0;
}

static SQRESULT sq_java_close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	CHECK_JNIENV();
    _jvm_vm->DestroyJavaVM();
    _jvm_vm = 0;
    _jvm_env = 0;
	return 0;
}

static SQRESULT sq_java_GetVersion(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	CHECK_JNIENV();
	sq_pushinteger(v, _jvm_env->GetVersion());
	return 1;
}

static SQRESULT sq_java_FindClass(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	CHECK_JNIENV();
	SQ_GET_STRING(v, 2, klass);
	jclass cls = _jvm_env->FindClass(klass);
	if(cls){
	    cls = (jclass)_jvm_env->NewGlobalRef(cls);
	    if(!cls) return sq_throwerror(v, _SC("JNI error: NewGlobalRef() failed"));
	    sq_pushstring(v, JavaClass_TAG, -1);
	    if(sq_getonroottable(v) == SQ_OK){
	        sq_createinstance(v, -1);
	        sq_setinstanceup(v, -1, cls);
	        sq_setreleasehook(v, -1, sq_java_class_releasehook);
	        return 1;
	    }
	}
	sq_pushnull(v);
	return 1;
}

#define _DECL_FUNC(name,nparams,tycheck,isStatic) {_SC(#name),  sq_java_##name,nparams,tycheck,isStatic}
static SQRegFunction sq_java_methods[] =
{
	_DECL_FUNC(open,  1, _SC("y"),SQTrue),
	_DECL_FUNC(close,  1, _SC("y"),SQTrue),
	_DECL_FUNC(GetVersion,  1, _SC("y"),SQTrue),
	_DECL_FUNC(FindClass,  2, _SC("ys"),SQTrue),
	{0,0}
};
#undef _DECL_FUNC

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_Java(HSQUIRRELVM v)
{
    sq_pushstring(v,Java_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)Java_TAG);
    sq_insert_reg_funcs(v, sq_java_methods);
    sq_newslot(v,-3,SQTrue);

    sq_pushstring(v,JavaClass_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)JavaClass_TAG);
    sq_insert_reg_funcs(v, sq_java_class_methods);

    sq_pushstring(v, _java_class_methods_key, -1);
    sq_pushnull(v);
    sq_newslot(v, -3, SQFalse);

    sq_newslot(v,-3,SQTrue);

    return 0;
}

#ifdef __cplusplus
}
#endif

