#ifdef WITH_FLTK

#define LUAFLTK

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input_.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Tabs.H>
#include <FL/Flv_List.H>
#include <FL/Flv_Table.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_JPEG_Image.H>

#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <FLU/Flu_Combo_Box.H>
#include <FLU/Flu_Combo_List.H>
#include <FLU/Flu_Combo_Tree.H>
#include <FLU/Flu_Tree_Browser.H>

#include <squirrel.h>

#include "sqfltk.h"

struct MyFltkUData {
    HSQOBJECT self;
    HSQOBJECT udata;
    HSQUIRRELVM v;
};

#define FLTK_TAG(tag) SQFLTK_##tag##_TYPE_TAG
#define CREATE_TAG(tag) static const SQChar FLTK_TAG(tag)[] = _SC(#tag);

CREATE_TAG(Fl);
CREATE_TAG(Fl_Widget);
CREATE_TAG(Fl_Button);
CREATE_TAG(Fl_Light_Button);
CREATE_TAG(Fl_Check_Button);
CREATE_TAG(Fl_Radio_Button);
CREATE_TAG(Fl_Return_Button);
CREATE_TAG(Fl_Round_Button);
CREATE_TAG(Fl_Menu_);
CREATE_TAG(Fl_Menu_Bar);
CREATE_TAG(Fl_Menu_Button);
CREATE_TAG(Fl_Menu_Item);
CREATE_TAG(Fl_Choice);
CREATE_TAG(Fl_Input_);
CREATE_TAG(Fl_Float_Input);
CREATE_TAG(Fl_Int_Input);
CREATE_TAG(Fl_Output);
CREATE_TAG(Fl_Secret_Input);
CREATE_TAG(Fl_Scroll);
CREATE_TAG(Fl_Group);
CREATE_TAG(Fl_Pack);
CREATE_TAG(Fl_Tabs);
CREATE_TAG(Flv_Style);
CREATE_TAG(Flv_Style_List);
CREATE_TAG(Flv_List);
CREATE_TAG(Flv_Table);
CREATE_TAG(Fl_Text_Display);
CREATE_TAG(Fl_Text_Editor);
CREATE_TAG(Fl_Text_Editor_Buffered);
CREATE_TAG(Fl_Window);
CREATE_TAG(Fl_Double_Window);

CREATE_TAG(Fl_Image);
CREATE_TAG(Fl_PNG_Image);
CREATE_TAG(Fl_JPEG_Image);

CREATE_TAG(Fl_File_Chooser);
CREATE_TAG(Fl_Native_File_Chooser);
CREATE_TAG(Fl_Text_Buffer);

CREATE_TAG(Flu_Combo_Box);
CREATE_TAG(Flu_Combo_List);
CREATE_TAG(Flu_Combo_Tree);
CREATE_TAG(Flu_Tree_Browser);


static const SQChar FLTK_key[] = _SC("FLTK");
static const SQChar FLTK_constructor_Mask[] = _SC("xiiii s|o");

static SQRESULT fltk_register_object_and_instance(HSQUIRRELVM v, int instance_idx, void *cptr){
    sq_pushstring(v, FLTK_key, -1);
    if(sq_getonregistrytable(v) < 0){
        sq_newtable(v);
        sq_pushstring(v, FLTK_key, -1);
        sq_push(v, -2);
        sq_setonregistrytable(v);
    }
    sq_pushuserpointer(v, cptr);
    //sq_weakref(v, instance_idx);
    sq_push(v, instance_idx);
    sq_rawset(v, -3);
    sq_pop(v, -1);
    return SQ_OK;
}

static SQRESULT fltk_deregister_instance(HSQUIRRELVM v, void *cptr){
    sq_pushstring(v, FLTK_key, -1);
    if(sq_getonregistrytable(v) >= 0){
	    sq_pushuserpointer(v, cptr);
	    sq_deleteslot(v, -2, SQFalse);
	    sq_pop(v, -1);
	    return SQ_OK;
    }
    return SQ_ERROR;
}

static SQRESULT fltk_get_registered_instance(HSQUIRRELVM v, void *cptr){
    sq_pushstring(v, FLTK_key, -1);
    if(sq_getonregistrytable(v) >= 0){
	    sq_pushuserpointer(v, cptr);
	    if(sq_rawget(v, -2) >= 0){
		    sq_remove(v, -2);
		    return SQ_OK;
	    }
	    sq_poptop(v);
    }
    return SQ_ERROR;
}

static SQInteger getInstance_for_Fl_Klass(HSQUIRRELVM v, const SQChar *klass, SQUserPointer *ptr){
    if(!ptr) return SQ_ERROR;
    if(SQ_SUCCEEDED(fltk_get_registered_instance(v, ptr))) return SQ_OK;
    sq_pushstring(v, klass, -1);
    if(SQ_SUCCEEDED(sq_getonroottable(v))
        && SQ_SUCCEEDED(sq_createinstance(v, -1))
        && SQ_SUCCEEDED(sq_setinstanceup(v, -1, ptr))){
        sq_replace(v, -2); //remove Fl_widget class from stack
        return SQ_OK;
    }
    return SQ_ERROR;
}

#define SETUP_KLASS(v, idx, Var, Klass, Klass_TAG) \
	Klass *Var = NULL; \
	{ if(SQ_FAILED(sq_getinstanceup(v,idx,(SQUserPointer*)&Var,(void*)Klass_TAG))) \
		return SQ_ERROR; }

#define SETUP_FL_KLASS_AT(v, idx, Var, Klass) SETUP_KLASS(v, idx, Var, Klass, FLTK_TAG(Klass))
#define SETUP_FL_KLASS(v, Klass) SETUP_FL_KLASS_AT(v, 1, self, Klass)

#define SETUP_FL_WIDGET(v) SETUP_FL_KLASS(v, Fl_Widget)
#define SETUP_FL_WIDGET_AT(v, idx, var) SETUP_FL_KLASS_AT(v, idx, var, Fl_Widget)

#define NOTHING(a)
#define AS_IS(a) a

#define FLTK_CONSTRUCTOR_RELEASE_WINDOW(Klass, RELEASE_HOOK, _release_hook)\
static SQInteger _##Klass##_constructor(HSQUIRRELVM v)\
{\
	const SQChar *label = NULL;\
	SQInteger argc, x,y,w,h;\
	Klass *cptr;\
	argc = sq_gettop(v);\
	if(argc > 4){\
	    sq_getinteger(v, 2, &x);\
	    sq_getinteger(v, 3, &y);\
	    sq_getinteger(v, 4, &w);\
	    sq_getinteger(v, 5, &h);\
        if(argc > 5 && sq_gettype(v, 6) == OT_STRING) sq_getstring(v, 6, &label);\
	    cptr = new Klass(x,y,w,h,label);\
	} else if(argc > 2){\
	    sq_getinteger(v, 2, &w);\
	    sq_getinteger(v, 3, &h);\
        if(argc > 3 && sq_gettype(v, 4) == OT_STRING) sq_getstring(v, 4, &label);\
	    cptr = new Klass(w,h,label);\
	} else \
        return sq_throwerror(v, _SC("Wrong number of parameters."));\
    fltk_register_object_and_instance(v, 1, cptr);\
    sq_setinstanceup(v, 1, cptr);\
    RELEASE_HOOK(sq_setreleasehook(v,1,_release_hook));\
	return 1;\
}

#define FLTK_CONSTRUCTOR_RELEASE(Klass, RELEASE_HOOK, _release_hook)\
static SQInteger _##Klass##_constructor(HSQUIRRELVM v)\
{\
	const SQChar *label = NULL;\
	SQInteger argc, x,y,w,h;\
	Klass *cptr;\
	argc = sq_gettop(v);\
	if(argc > 4){\
	    sq_getinteger(v, 2, &x);\
	    sq_getinteger(v, 3, &y);\
	    sq_getinteger(v, 4, &w);\
	    sq_getinteger(v, 5, &h);\
        if(argc > 5 && sq_gettype(v, 6) == OT_STRING) sq_getstring(v, 6, &label);\
	    cptr = new Klass(x,y,w,h,label);\
	    fltk_register_object_and_instance(v, 1, cptr);\
	} else \
        return sq_throwerror(v, _SC("Wrong number of parameters."));\
    sq_setinstanceup(v, 1, cptr);\
    RELEASE_HOOK(sq_setreleasehook(v,1,_release_hook));\
	return 1;\
}

#define FLTK_CONSTRUCTOR(Klass) FLTK_CONSTRUCTOR_RELEASE(Klass, NOTHING, a)

#define FUNC_GETSET_STR(prefix, getSelf, selfPtr, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger argc = sq_gettop(v);\
    const SQChar *str = NULL;\
    if(argc > 1){\
        if(sq_gettype(v, 2) == OT_STRING) sq_getstring(v, 2, &str);\
        selfPtr funcNAME(str);\
        return 0;\
    }\
    str = selfPtr funcNAME();\
    if(str) sq_pushstring(v, str, -1);\
    else sq_pushnull(v);\
	return 1;\
}

#define FUNC_GETSET_INT(prefix, getSelf, selfPtr, funcNAME, typeCast) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger argc = sq_gettop(v);\
    SQInteger iparm;\
    if(argc > 1){\
        sq_getinteger(v, 2, &iparm);\
        selfPtr funcNAME((typeCast)iparm);\
        return 0;\
    }\
    sq_pushinteger(v, selfPtr funcNAME());\
	return 1;\
}

#define FUNC_GETSET_INT_ONE_CALL(prefix, getSelf, selfPtr, funcNAME, typeCast) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger argc = sq_gettop(v);\
    SQInteger iparm;\
    if(argc > 1){\
        sq_getinteger(v, 2, &iparm);\
        sq_pushinteger(v, selfPtr funcNAME((typeCast)iparm));\
        return 1;\
    }\
    return sq_throwerror(v, _SC("one integer parameter expected"));\
}

#define FUNC_DUMMY(prefix, funcNAME)\
static SQInteger prefix##funcNAME(HSQUIRRELVM v) {return 0;}

#define FUNC_GETSET_INT_BOOL(prefix, getSelf, selfPtr, funcNAME, typeCast) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger argc = sq_gettop(v);\
    SQInteger iparm;\
    SQBool bparm;\
    if(argc > 1){\
        sq_getinteger(v, 2, &iparm);\
        if(argc > 2) {\
            sq_getbool(v, 3, &bparm);\
            selfPtr funcNAME((typeCast)iparm, bparm == SQTrue);\
            return 0;\
        }\
        sq_pushbool(v, selfPtr funcNAME((typeCast)iparm));\
        return 1;\
    }\
	return sq_throwerror(v, _SC("invalid number of parameters"));\
}

#define FUNC_VOID_CALL(prefix, getSelf, selfPtr, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    selfPtr funcNAME();\
	return 0;\
}

#define FUNC_VOID_CALL_2INT(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger i1, i2;\
    sq_getinteger(v, 2, &i1);\
    sq_getinteger(v, 3, &i2);\
    self->funcNAME(i1,i2);\
	return 0;\
}

#define FUNC_VOID_CALL_4INT(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger i1, i2, i3, i4;\
    sq_getinteger(v, 2, &i1);\
    sq_getinteger(v, 3, &i2);\
    sq_getinteger(v, 4, &i3);\
    sq_getinteger(v, 5, &i4);\
    self->funcNAME(i1,i2,i3,i4);\
	return 0;\
}

#define FUNC_INT_CALL(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    sq_pushinteger(v, self->funcNAME());\
	return 1;\
}

#define FUNC_SET_INT(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger iparam; sq_getinteger(v, 2, &iparam);\
    self->funcNAME(iparam));\
	return 0;\
}

#define FUNC_STR_CALL(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    sq_pushstring(v, self->funcNAME(), -1);\
	return 1;\
}

#define FUNC_GET_BOOL(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    sq_pushbool(v, self->funcNAME());\
	return 1;\
}

#define FUNC_GETSET_BOOL(prefix, getSelf, selfPtr, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM v) \
{\
    getSelf(v);\
    SQInteger argc = sq_gettop(v);\
    SQBool bparm;\
    if(argc > 1){\
        sq_getbool(v, 2, &bparm);\
        selfPtr funcNAME(bparm);\
        return 0;\
    }\
    sq_pushbool(v, selfPtr funcNAME());\
	return 1;\
}

#define FL_WIDGET_GETSET_STR(funcNAME) FUNC_GETSET_STR(_Fl_Widget_, SETUP_FL_WIDGET, self->, funcNAME)

#define FL_WIDGET_GETSET_INT0(funcNAME, typeNAME) FUNC_GETSET_INT(_Fl_Widget_, SETUP_FL_WIDGET, self->, funcNAME, typeNAME)
#define FL_WIDGET_GETSET_INT(funcNAME) FL_WIDGET_GETSET_INT0(funcNAME, int)
#define FL_WIDGET_GETSET_INT_CAST(funcNAME, castType) FL_WIDGET_GETSET_INT0(funcNAME, castType)

#define FL_WIDGET_VOID_CALL(funcNAME) FUNC_VOID_CALL(_Fl_Widget_, SETUP_FL_WIDGET, self->, funcNAME)

#define FL_WIDGET_VOID_CALL_2INT(funcNAME) FUNC_VOID_CALL_2INT(_Fl_Widget_, SETUP_FL_WIDGET, funcNAME)

#define FL_WIDGET_VOID_CALL_4INT(funcNAME) FUNC_VOID_CALL_4INT(_Fl_Widget_, SETUP_FL_WIDGET, funcNAME)

#define FL_WIDGET_INT_CALL(funcNAME) FUNC_INT_CALL(_Fl_Widget_, SETUP_FL_WIDGET, funcNAME)
#define FL_WIDGET_STR_CALL(funcNAME) FUNC_STR_CALL(_Fl_Widget_, SETUP_FL_WIDGET, funcNAME)

#define GETINSTANCE_FOR_FL_KLASS(Klass)\
static SQInteger getInstance_for_##Klass(HSQUIRRELVM v, Klass *widget){\
    if(!widget) return SQ_ERROR;\
    if(fltk_get_registered_instance(v, widget) == SQ_OK) return SQ_OK;\
    int rc;\
    sq_pushuserpointer(v, (void*)FLTK_TAG(Klass));\
    rc = sq_getonregistrytable(v);\
    rc = sq_createinstance(v, -1);\
    rc = sq_setinstanceup(v, -1, widget);\
    sq_replace(v, -2);\
    return SQ_OK;\
}

#define CHEAP_RTTI_FOR(Klass) \
static SQRESULT _##Klass##_className(HSQUIRRELVM v){sq_pushstring(v, Klass::className(), -1); return 1;};\
static SQRESULT _##Klass##_cheap_rtti_info(HSQUIRRELVM v){sq_pushuserpointer(v, (SQUserPointer)Klass::cheap_rtti_info()); return 1;};

#define CHEAP_RTTI_REG_FUN_FOR(Klass) \
{_SC("className"),_##Klass##_className, 1,_SC("y"), SQTrue},\
{_SC("cheap_rtti_info"),_##Klass##_cheap_rtti_info, 1,_SC("y"), SQTrue},

GETINSTANCE_FOR_FL_KLASS(Fl_Widget);
GETINSTANCE_FOR_FL_KLASS(Fl_Group);

static SQChar Fl_Image_TAG[] = _SC("Fl_Image");
#define SETUP_FL_IMAGE_AT(v, idx, Var) SETUP_KLASS(v, idx, Var, Fl_Image, Fl_Image_TAG)
#define SETUP_FL_IMAGE(v) SETUP_FL_IMAGE_AT(v, 1, self)


static SQRESULT fltk_pushinstance(HSQUIRRELVM v, const SQChar *klass, SQUserPointer self){
	sq_pushstring(v, klass,-1);
	if(sq_getonroottable(v) == SQ_OK){
	    if(sq_createinstance(v, -1) == SQ_OK){
	        sq_setinstanceup(v, -1, self);
	        return 1;
	    }
	}
    return SQ_ERROR;
}


static SQRESULT sq_get_fl_virtual(HSQUIRRELVM v, void *self, const SQChar *func_name){
    //sq_reservestack(v, 20);
    if(fltk_get_registered_instance(v, self) == SQ_OK){
        sq_pushstring(v, func_name, -1);
        sq_get(v, -2);
        SQObjectType toptype = sq_gettype(v, -1);
        if(toptype == OT_CLOSURE || toptype == OT_NATIVECLOSURE){
            return SQ_OK;
        }
    }
    return SQ_ERROR;
}

static void sq_call_fl_virtual(void *self, const SQChar *func_name){
    //printf("%d sq_call_fl_virtual %p %s\n", __LINE__, self, func_name);
    HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
    //sq_reservestack(v, 20);
    SQInteger top = sq_gettop(v);
    if(sq_get_fl_virtual(v, self, func_name) == SQ_OK){
        sq_push(v, -2); //this
        sq_call(v, 1, SQFalse, SQTrue);
    }
    sq_settop(v, top);
}

typedef struct {
    HSQOBJECT callback;
    HSQOBJECT sender;
    HSQOBJECT udata;
} st_mycb;

//static st_mycb static_cb;

static void free_st_mycb(HSQUIRRELVM v, st_mycb *cb){
    sq_release(v, &cb->callback);
    sq_release(v, &cb->udata);
    sq_free((void*)cb, sizeof(st_mycb));
}

static void At_Widget_Destroy(Fl_Widget *widget){
//printf("%d %p At_Widget_Destroy\n", __LINE__, widget);
    Fl_Callback_p cb = widget->callback();
    if(!cb || cb == Fl_Widget::default_callback) {
        //no need to check
        return;
    }
    HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(v);
//printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(v));
    int rc = fltk_deregister_instance(v, widget);
//printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(v));
//printf("%d %p %d %s At_Widget_Destroy\n", __LINE__, widget, rc, sq_getlasterror_str(v));
    //if(sq_deleteslot(v, -2, SQTrue) == SQ_OK){
    if(rc == SQ_OK){
//printf("%d %p At_Widget_Destroy\n", __LINE__, widget);
        if(sq_gettype(v, -1) != OT_NULL){
            //////////////////////////////nota all will have user pointer !!!!!!!!
            SQUserPointer ptr;
            sq_getuserpointer(v, -1, &ptr);
            //free_st_mycb(v, (st_mycb*)ptr);
        }
    }
    sq_settop(v, savedTop);
}

static void fltk_calback_hook(Fl_Widget *sender, void* udata){
//printf("Sender %p, data %p\n", sender, udata);
    HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(v);
    st_mycb *mycb = (st_mycb *)udata;
    sq_pushobject(v, mycb->callback);
    sq_pushroottable(v); //’this’ (function environment object)
    if(fltk_get_registered_instance(v, sender) != SQ_OK) sq_pushnull(v);
    sq_pushobject(v, mycb->udata);
    sq_call(v, 3, SQFalse, SQTrue);
//printf("%d\n", sq_call(v, 3, SQFalse, SQTrue));
    sq_settop(v, savedTop);
}

static SQInteger _Fl_Widget_callback(HSQUIRRELVM v)
{
//printf("%d %s\n", __LINE__, __FILE__);
    SETUP_FL_WIDGET(v);
    SQInteger argc = sq_gettop(v);
    Fl_Callback_p cb = self->callback();
    if(cb == Fl_Widget::default_callback){
        cb = 0;
    }
    if(argc == 1){
        if(cb) sq_pushuserpointer(v, (void*)cb);
        else sq_pushnull(v);
        return 1;
    }
    if(argc >= 2){
        int ptypecb = sq_gettype(v, 2);
        if(cb || ptypecb == OT_NULL) { //not allways the cb was set by us
            At_Widget_Destroy(self);
            if(ptypecb == OT_NULL) {
                self->callback((Fl_Callback*)NULL, (void*)NULL);
                return 0;
            }
        }
        st_mycb *mycb = (st_mycb *)sq_malloc(sizeof(st_mycb));
        sq_resetobject(&mycb->callback); //initializes the object handle (sets to NULL)
        sq_resetobject(&mycb->sender);
        sq_resetobject(&mycb->udata);
        sq_getstackobj(v, 2, &mycb->callback);//fetches an handle of the function you passed
        if(sq_gettop(v) > 2) { //we have user data too
            sq_getstackobj(v, 3, &mycb->udata);
            sq_addref(v,&mycb->udata);
        }
        if(sq_type(mycb->callback) == OT_CLOSURE || sq_type(mycb->callback) == OT_NATIVECLOSURE){
            sq_addref(v,&mycb->callback);
            //sq_getstackobj(v, 1, &mycb->sender);
            self->callback(&fltk_calback_hook, mycb);
            //registry for free when widget is destroyed
            sq_pushregistrytable(v);
            sq_pushuserpointer(v, self);
            sq_pushuserpointer(v, mycb);
            sq_rawset(v, -2);
//printf("Button %p\n", self);
        }
    }
	return 0;
}

FL_WIDGET_GETSET_INT(align);
FL_WIDGET_GETSET_INT(argument);
FL_WIDGET_GETSET_INT_CAST(box, Fl_Boxtype);
FL_WIDGET_GETSET_INT(color);
FL_WIDGET_GETSET_INT(selection_color);
FL_WIDGET_GETSET_STR(label);
FL_WIDGET_GETSET_INT(labelcolor);
FL_WIDGET_GETSET_INT(labelfont);
FL_WIDGET_GETSET_INT(labelsize);
FL_WIDGET_GETSET_INT_CAST(labeltype, Fl_Labeltype);
FL_WIDGET_GETSET_INT(textfont);
FL_WIDGET_GETSET_INT(textsize);
FL_WIDGET_GETSET_STR(tooltip);
FL_WIDGET_GETSET_INT(when);
FL_WIDGET_GETSET_INT(type);
FL_WIDGET_GETSET_INT(x);
FL_WIDGET_GETSET_INT(y);
FL_WIDGET_GETSET_INT(w);
FL_WIDGET_GETSET_INT(h);
FL_WIDGET_VOID_CALL_2INT(position);
FL_WIDGET_VOID_CALL_2INT(size);
FL_WIDGET_VOID_CALL_4INT(resize);
//FUNC_SET_INT(_Fl_Widget_, SETUP_FL_WIDGET, clear_flag);

//FL_WIDGET_STR_CALL(classId);

static SQInteger _Fl_Widget_classId(HSQUIRRELVM v)
{
    SETUP_FL_WIDGET(v);
    if(!self) return sq_throwerror(v, _SC("attempt to call method with null object"));
    const char *cn = self->classId();
    sq_pushstring(v, cn, -1);
	return 1;
}

static SQInteger _Fl_Widget_classRTTI(HSQUIRRELVM v)
{
    SETUP_FL_WIDGET(v);
    if(!self) return sq_throwerror(v, _SC("attempt to call method with null object"));
    sq_pushuserpointer(v, (SQUserPointer)self->classRTTI());
	return 1;
}

static SQInteger _Fl_Widget_inherits_from(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_WIDGET(v);
    if(!self) return sq_throwerror(v, _SC("attempt to call method with null object"));
    SQ_GET_USERPOINTER(v, 2, ptr);
    sq_pushbool(v, self->inherits_from((const st_cheap_rtti *)ptr));
	return 1;
}

static SQInteger _Fl_Widget_parent(HSQUIRRELVM v)
{
    SETUP_FL_WIDGET(v);
    if(!self) return sq_throwerror(v, _SC("attempt to call method with null object"));
    if(getInstance_for_Fl_Widget(v, self->parent()) != SQ_OK) sq_pushnull(v);
	return 1;
}

static SQInteger _Fl_Widget_parent_root(HSQUIRRELVM v)
{
    SETUP_FL_WIDGET(v);
    if(!self) return sq_throwerror(v, _SC("attempt to call method with null object"));
    if(getInstance_for_Fl_Widget(v, self->parent_root()) != SQ_OK) sq_pushnull(v);
	return 1;
}

static SQInteger _Fl_Widget_handle(HSQUIRRELVM v)
{
    SETUP_FL_WIDGET(v);
    SQInteger event;
    sq_getinteger(v, 2, &event);
    sq_pushinteger(v, self->handle(event));
	return 1;
}

FL_WIDGET_VOID_CALL(redraw);
FL_WIDGET_VOID_CALL(redraw_label);
FL_WIDGET_VOID_CALL(hide);
FL_WIDGET_VOID_CALL(show);

static SQInteger _Fl_Widget_image(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_WIDGET(v);
    if(_top_ > 1){
        if(sq_gettype(v, 2) == OT_NULL) self->image(0);
        else
        {
            SETUP_FL_IMAGE_AT(v, 2, img);
            self->image(img);
        }
        return 0;
    }
    Fl_Image *img = self->image();
    if(img) return fltk_pushinstance(v, Fl_Image_TAG, self->image());
    sq_pushnull(v);
    return 1;
}

CHEAP_RTTI_FOR(Fl_Widget);

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Widget_##name,nparams,pmask,isStatic}
static SQRegFunction fl_widget_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Widget)
	_DECL_FUNC(align,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(argument,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(box,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(callback,-1,_SC("xc."), SQFalse),
	_DECL_FUNC(color,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(selection_color,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(label,-1,_SC("x s|o"), SQFalse),
	_DECL_FUNC(labelcolor,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(labelfont,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(labelsize,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(labeltype,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(textfont,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(textsize,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(tooltip,-1,_SC("xs"), SQFalse),
	_DECL_FUNC(type,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(when,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(handle,2,_SC("xi"), SQFalse),
	_DECL_FUNC(redraw,1,_SC("x"), SQFalse),
	_DECL_FUNC(redraw_label,1,_SC("x"), SQFalse),
	_DECL_FUNC(x,-1,_SC("xn"), SQFalse),
	_DECL_FUNC(y,-1,_SC("xn"), SQFalse),
	_DECL_FUNC(w,-1,_SC("xn"), SQFalse),
	_DECL_FUNC(h,-1,_SC("xn"), SQFalse),
	_DECL_FUNC(position,3,_SC("xii"), SQFalse),
	_DECL_FUNC(size,3,_SC("xii"), SQFalse),
	_DECL_FUNC(resize,5,_SC("xiiii"), SQFalse),
	_DECL_FUNC(show,1,_SC("x"), SQFalse),
	_DECL_FUNC(hide,1,_SC("x"), SQFalse),
	_DECL_FUNC(classId,1,_SC("x"), SQFalse),
	_DECL_FUNC(inherits_from,2,_SC("xp"), SQFalse),
	_DECL_FUNC(classRTTI,1,_SC("x"), SQFalse),
	_DECL_FUNC(parent,1,_SC("x"), SQFalse),
	_DECL_FUNC(parent_root,1,_SC("x"), SQFalse),
	_DECL_FUNC(image,-1,_SC("x x|o"), SQFalse),
//	_DECL_FUNC(clear_flag,2,_SC("xi"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

#define MY_FL_CLASS(klass, myfuncs)\
class My##klass : public klass {\
    public:\
    My##klass(int X, int Y, int W, int H, const char *L=0):klass(X,Y,W,H,L){}\
    myfuncs\
};\
CREATE_TAG(klass);\
FLTK_CONSTRUCTOR(My##klass);


MY_FL_CLASS(Fl_Box,
    void draw(){
        sq_call_fl_virtual(this, "draw");
    }
)

static SQInteger _MyFl_Box_draw(HSQUIRRELVM v)
{
    SETUP_FL_WIDGET(v);
    ((MyFl_Box*)self)->Fl_Box::draw();
	return 0;
}

CHEAP_RTTI_FOR(Fl_Box);

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_MyFl_Box_##name,nparams,pmask,isStatic}
static SQRegFunction fl_box_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Box)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(draw,1,_SC("x"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Button);
#define SETUP_FL_BUTTON(v) SETUP_FL_KLASS(v, Fl_Button)
#define FL_BUTTON_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_Fl_Button_, SETUP_FL_BUTTON, self->, funcNAME, typeNAME)
FL_BUTTON_GETSET_INT_CAST(down_box, Fl_Boxtype);
FL_BUTTON_GETSET_INT_CAST(value, int);

static SQRESULT _Fl_Button_setonly(HSQUIRRELVM v){
    SETUP_FL_BUTTON(v);
    self->setonly();
    return 0;
}

CHEAP_RTTI_FOR(Fl_Button);

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Button_##name,nparams,pmask,isStatic}
static SQRegFunction fl_button_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Button)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(down_box,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(value,-1,_SC("xi"), SQFalse),
	_DECL_FUNC(setonly,1,_SC("x"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Light_Button);
CHEAP_RTTI_FOR(Fl_Light_Button);

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Light_Button_##name,nparams,pmask,isStatic}
static SQRegFunction fl_light_button_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Light_Button)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Check_Button);
CHEAP_RTTI_FOR(Fl_Check_Button);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Check_Button_##name,nparams,pmask,isStatic}
static SQRegFunction fl_check_button_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Check_Button)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Radio_Button);
CHEAP_RTTI_FOR(Fl_Radio_Button);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Radio_Button_##name,nparams,pmask,isStatic}
static SQRegFunction fl_radio_button_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Radio_Button)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Return_Button);
CHEAP_RTTI_FOR(Fl_Return_Button);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Return_Button_##name,nparams,pmask,isStatic}
static SQRegFunction fl_return_button_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Return_Button)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Round_Button);
CHEAP_RTTI_FOR(Fl_Round_Button);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Round_Button_##name,nparams,pmask,isStatic}
static SQRegFunction fl_round_button_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Round_Button)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

/*
FLTK_CONSTRUCTOR(Fl_Menu_Item);
#define _DECL_FL_MENU_ITEM_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Menu_Item_##name,nparams,pmask,isStatic}
static SQRegFunction fl_menu_item_obj_funcs[]={
	_DECL_FL_MENU_ITEM_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
*/

FLTK_CONSTRUCTOR(Fl_Menu_);
#define SETUP_FL_MENU_(v) SETUP_FL_KLASS(v, Fl_Menu_)

static SQInteger _Fl_Menu__copy(HSQUIRRELVM v)
{
    SETUP_FL_MENU_(v);
    SQInteger menu_size = sq_getsize(v, 2);
    sq_reservestack(v, 20);
    SQInteger work_top = sq_gettop(v);
    SQInteger nested = 0;
    for(int i=0; i < menu_size; ++i){
        sq_pushinteger(v, i);
        sq_rawget(v, 2);
        SQInteger item_size = sq_getsize(v, -1);
        if(item_size > 0) {
            const SQChar *str;
            SQInteger flags = 0;
            sq_pushinteger(v, 0);
            sq_rawget(v, -2);
            if(sq_gettype(v, -1) == OT_STRING){
                sq_getstring(v, -1, &str);
                if(item_size > 4){
                    sq_pushinteger(v, 4);
                    sq_rawget(v, -3);
                    if(sq_gettype(v, -1) == OT_INTEGER){
                        sq_getinteger(v, -1, &flags);
                    }
                }
                //if(flags & FL_SUBMENU)
                self->add(str, 0,0,0,flags);
            }
        }
        sq_settop(v, work_top);
    }
    //self->copy(&menu_items);
    return 0;
}

static SQInteger _Fl_Menu__add(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_MENU_(v);
    SQ_GET_STRING(v, 2, label);
    if(_top_ > 2) {
        //SQObjectType ptype3 = sq_gettype(v, 3);
        SQ_GET_INTEGER(v, 5, udata);
        self->add(label, 0,0, (void*)udata, 0);
    }
    else self->add(label);
    return 0;
}

static SQInteger _Fl_Menu__value(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_MENU_(v);
    sq_pushinteger(v, self->value());
    return 1;
}

FUNC_GETSET_INT(_Fl_Menu__, SETUP_FL_MENU_, self->, down_box, Fl_Boxtype);

CHEAP_RTTI_FOR(Fl_Menu_);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Menu__##name,nparams,pmask,isStatic}
static SQRegFunction fl_menu__obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Menu_)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(copy,-2,_SC("xa."),SQFalse),
	_DECL_FUNC(add,-2,_SC("xs s|i|o c|n .i"),SQFalse),
	_DECL_FUNC(value,1,_SC("x"),SQFalse),
	_DECL_FUNC(down_box,-1,_SC("xi"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Menu_Bar);
CHEAP_RTTI_FOR(Fl_Menu_Bar);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Menu_Bar_##name,nparams,pmask,isStatic}
static SQRegFunction fl_menu_bar_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Menu_Bar)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Menu_Button);
#define SETUP_FL_MENU_BUTTON(v) SETUP_FL_KLASS(v, Fl_Menu_Button)

static SQInteger _Fl_Menu_Button_popup(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_MENU_BUTTON(v);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, x);
        SQ_GET_INTEGER(v, 3, y);
        self->popup(x, y);
    }
    else self->popup();
	return 0;
}

CHEAP_RTTI_FOR(Fl_Menu_Button);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Menu_Button_##name,nparams,pmask,isStatic}
static SQRegFunction fl_menu_button_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Menu_Button)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(popup,-1,_SC("xnn"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Choice);
CHEAP_RTTI_FOR(Fl_Choice);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Choice_##name,nparams,pmask,isStatic}
static SQRegFunction fl_choice_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Choice)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Input_);
#define SETUP_FL_INPUT_(v) SETUP_FL_KLASS(v, Fl_Input_)
FUNC_GETSET_STR(_Fl_Input__, SETUP_FL_INPUT_, self->, value);
CHEAP_RTTI_FOR(Fl_Input_);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Input__##name,nparams,pmask,isStatic}
static SQRegFunction fl_input__obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Input_)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(value,-1,_SC("xs"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

MY_FL_CLASS(Fl_Input,
    int handle(int event){
        HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
        SQInteger top = sq_gettop(v);
        if(sq_get_fl_virtual(v, this, "handle") == SQ_OK){
            sq_push(v, -2); //this
            sq_pushinteger(v, event);
            if(sq_call(v, 2, SQTrue, SQTrue) == SQ_OK){
                if(sq_gettype(v, -1) == OT_INTEGER){
                    sq_getinteger(v, -1, &event);
                    sq_settop(v, top);
                    return event;
                }
            }
            exit(-1);
        }
        sq_settop(v, top);
        return Fl_Input::handle(event);
    }
)

#define SETUP_FL_INPUT(v) SETUP_FL_KLASS(v, Fl_Input)

static SQInteger _MyFl_Input_handle(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_INPUT(v);
    SQ_GET_INTEGER(v, 2, event);
    sq_pushinteger(v, ((MyFl_Input*)self)->Fl_Input::handle(event));
    return 1;
}

FUNC_GETSET_STR(_MyFl_Input_, NOTHING, Fl_Input::, default_number_format);
FUNC_GETSET_STR(_MyFl_Input_, SETUP_FL_INPUT, self->, number_format);
#define FL_INPUT_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_MyFl_Input_, SETUP_FL_INPUT, self->, funcNAME, typeNAME)
FL_INPUT_GETSET_INT_CAST(use_numeric_format, int);
FL_INPUT_GETSET_INT_CAST(right_to_left, int);
FL_INPUT_GETSET_INT_CAST(decimal_places, int);

CHEAP_RTTI_FOR(Fl_Input);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_MyFl_Input_##name,nparams,pmask,isStatic}
static SQRegFunction fl_input_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Input)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask,SQFalse),
	_DECL_FUNC(handle,2,_SC("xi"),SQFalse),
	_DECL_FUNC(default_number_format,-1,_SC("ys"),SQTrue),
	_DECL_FUNC(number_format,-1,_SC("xs"),SQFalse),
	_DECL_FUNC(use_numeric_format,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(right_to_left,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(decimal_places,-1,_SC("xi"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC


FLTK_CONSTRUCTOR(Fl_Float_Input);
CHEAP_RTTI_FOR(Fl_Float_Input);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Float_Input_##name,nparams,pmask,isStatic}
static SQRegFunction fl_float_input_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Float_Input)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Int_Input);
CHEAP_RTTI_FOR(Fl_Int_Input);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Int_Input_##name,nparams,pmask,isStatic}
static SQRegFunction fl_int_input_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Int_Input)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Output);
CHEAP_RTTI_FOR(Fl_Output);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Output_##name,nparams,pmask,isStatic}
static SQRegFunction fl_output_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Output)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Secret_Input);
CHEAP_RTTI_FOR(Fl_Secret_Input);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Secret_Input_##name,nparams,pmask,isStatic}
static SQRegFunction fl_secret_input_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Secret_Input)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

#define SETUP_FL_GROUP(v) SETUP_FL_KLASS(v, Fl_Group)

FLTK_CONSTRUCTOR(Fl_Group);

/*
enum _Fl_Group_Funcs {add, begin, current, end, resizable};
static SQRESULT _Fl_Group_dispatcher(HSQUIRRELVM v, _Fl_Group_Funcs func_idx){
    SETUP_FL_GROUP(v);
    switch(func_idx){
        case add:{
            SQUserPointer rsz;
            sq_getinstanceup(v, -1, &rsz, 0);
            self->add((Fl_Widget*)rsz);
            return 0;
        }
        break;
        case begin:
        case current:
        case end:
        case resizable:
        break;
    }
    return 0;
}
*/

static SQInteger _Fl_Group_begin(HSQUIRRELVM v)
{
    SETUP_FL_GROUP(v);
    self->begin();
	return 0;
}

static SQInteger _Fl_Group_end(HSQUIRRELVM v)
{
    SETUP_FL_GROUP(v);
    self->end();
	return 0;
}

static SQInteger _Fl_Group_current(HSQUIRRELVM v)
{
    Fl_Group *grp = Fl_Group::current();
    if(!grp || getInstance_for_Fl_Group(v, grp) != SQ_OK) sq_pushnull(v);
	return 1;
}

static SQInteger _Fl_Group_resizable(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_GROUP(v);
    if(_top_ > 1){
        SETUP_FL_WIDGET_AT(v, 2, wdg);
        self->resizable(wdg);
        return 0;
    }
    return fltk_pushinstance(v, FLTK_TAG(Fl_Widget), self->resizable());
}

static SQInteger _Fl_Group_add(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_GROUP(v);
    SETUP_FL_WIDGET_AT(v, 2, wdg);
    self->add(wdg);
	return 0;
}

static SQInteger _Fl_Group_insert(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_GROUP(v);
    SETUP_FL_WIDGET_AT(v, 2, wdg);
    switch(sq_gettype(v, 3)){
        case OT_INTEGER:{
            SQ_GET_INTEGER(v, 3, idx);
            self->insert((Fl_Widget&)*wdg, idx);
        }
        break;
        case OT_INSTANCE:{
            SETUP_FL_WIDGET_AT(v, 3, wdg2)
            self->insert((Fl_Widget&)*wdg, wdg2);
        }
        break;
        default:
            return sq_throwerror(v, _SC("invalid parameter (%s)"), sq_gettypename(v, 3));
    }
	return 0;
}

static SQInteger _Fl_Group_remove(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_GROUP(v);
    switch(sq_gettype(v, 2)){
        case OT_INTEGER:{
            SQ_GET_INTEGER(v, 2, idx);
            self->remove(idx);
        }
        break;
        case OT_INSTANCE:{
            SETUP_FL_WIDGET_AT(v, 2, wdg);
            self->remove(wdg);
        }
        break;
        default:
            return sq_throwerror(v, _SC("invalid parameter (%s)"), sq_gettypename(v, 2));
    }
	return 0;
}

CHEAP_RTTI_FOR(Fl_Group);
#define _DECL_FUNC(name,nparams,pmask, isStatic) {_SC(#name),_Fl_Group_##name,nparams,pmask, isStatic}
static SQRegFunction fl_group_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Group)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(begin,1,_SC("x"), SQFalse),
	_DECL_FUNC(end,1,_SC("x"), SQFalse),
	_DECL_FUNC(resizable,-1,_SC("xx"), SQFalse),
	_DECL_FUNC(add,2,_SC("xx"), SQFalse),
	_DECL_FUNC(insert,3,_SC("xx x|i"), SQTrue),
	_DECL_FUNC(remove,2,_SC("x x|i"), SQTrue),
	_DECL_FUNC(current,1,_SC("y"), SQTrue),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Pack);
#define SETUP_FL_PACK(v) SETUP_FL_KLASS(v, Fl_Pack)

#define FL_PACK_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_Fl_Pack_, SETUP_FL_PACK, self->, funcNAME, typeNAME)
FL_PACK_GETSET_INT_CAST(spacing, int);
FL_PACK_GETSET_INT_CAST(with_label, int);

CHEAP_RTTI_FOR(Fl_Pack);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Pack_##name,nparams,pmask,isStatic}
static SQRegFunction fl_pack_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Pack)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(spacing,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(with_label,-1,_SC("xi"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

#define SETUP_FLU_COMBO_BOX(v) SETUP_FL_KLASS(v, Flu_Combo_Box)

FUNC_VOID_CALL(_Flu_Combo_Box_, SETUP_FLU_COMBO_BOX, self->, clear_items);
FUNC_GET_BOOL(_Flu_Combo_Box_, SETUP_FLU_COMBO_BOX, hasItems);

static SQRESULT _Flu_Combo_Box_add_item(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLU_COMBO_BOX(v);
    SQ_GET_INTEGER(v, 2, adata);
    SQ_GET_STRING(v, 3, avalue);
    self->add_item((void*)adata, avalue);
    return 0;
}

static SQRESULT _Flu_Combo_Box_get_data_at(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    SETUP_FLU_COMBO_BOX(v);
    SQ_OPT_INTEGER(v, 2, idx, -1);
    sq_pushinteger(v, (SQInteger)self->get_data_at(idx));
    return 1;
}

static SQRESULT _Flu_Combo_Box_select_by_data(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    SETUP_FLU_COMBO_BOX(v);
    SQ_GET_INTEGER(v, 2, idx);
    self->select_by_data((void*)idx);
    return 0;
}

CHEAP_RTTI_FOR(Flu_Combo_Box);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flu_Combo_Box_##name,nparams,pmask,isStatic}
static SQRegFunction flu_combo_box_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Flu_Combo_Box)
	_DECL_FUNC(add_item,3,_SC("x.s"),SQFalse),
	_DECL_FUNC(get_data_at,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(select_by_data,2,_SC("x."),SQFalse),
	_DECL_FUNC(clear_items,1,_SC("x"),SQFalse),
	_DECL_FUNC(hasItems,1,_SC("x"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Flu_Combo_List);
#define SETUP_FLU_COMBO_LIST(v) SETUP_FL_KLASS(v, Flu_Combo_List)
CHEAP_RTTI_FOR(Flu_Combo_List);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flu_Combo_List_##name,nparams,pmask,isStatic}
static SQRegFunction flu_combo_list_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Flu_Combo_List)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Flu_Combo_Tree);
#define SETUP_FLU_COMBO_TREE(v) SETUP_FL_KLASS(v, Flu_Combo_Tree)

static SQRESULT _Flu_Combo_Tree_tree(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLU_COMBO_TREE(v);
    return fltk_pushinstance(v, FLTK_TAG(Flu_Tree_Browser), self);
}
CHEAP_RTTI_FOR(Flu_Combo_Tree);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flu_Combo_Tree_##name,nparams,pmask,isStatic}
static SQRegFunction flu_combo_tree_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Flu_Combo_Tree)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(tree,1,_SC("x"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Flu_Tree_Browser);
#define SETUP_FLU_COMBO_TREE_BROWSER(v) SETUP_FL_KLASS(v, Flu_Tree_Browser)

FUNC_GETSET_BOOL(_Flu_Tree_Browser_, SETUP_FLU_COMBO_TREE_BROWSER, self->, auto_branches);
FUNC_GETSET_BOOL(_Flu_Tree_Browser_, SETUP_FLU_COMBO_TREE_BROWSER, self->, show_branches);
FUNC_GETSET_BOOL(_Flu_Tree_Browser_, SETUP_FLU_COMBO_TREE_BROWSER, self->, all_branches_always_open);

static SQRESULT _Flu_Tree_Browser_shaded_entry_colors(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLU_COMBO_TREE_BROWSER(v);
    SQ_GET_INTEGER(v, 2, color1);
    SQ_GET_INTEGER(v, 3, color2);
    self->shaded_entry_colors(color1, color2);
    return 0;
}

static SQRESULT _Flu_Tree_Browser_branch_text(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLU_COMBO_TREE_BROWSER(v);
    SQ_GET_INTEGER(v, 2, color);
    SQ_GET_INTEGER(v, 3, font);
    SQ_GET_INTEGER(v, 3, size);
    self->branch_text(color, font, size);
    return 0;
}

static SQRESULT _Flu_Tree_Browser_leaf_text(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLU_COMBO_TREE_BROWSER(v);
    SQ_GET_INTEGER(v, 2, color);
    SQ_GET_INTEGER(v, 3, font);
    SQ_GET_INTEGER(v, 3, size);
    self->leaf_text(color, font, size);
    return 0;
}
CHEAP_RTTI_FOR(Flu_Tree_Browser);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flu_Tree_Browser_##name,nparams,pmask,isStatic}
static SQRegFunction flu_combo_tree_browser_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Flu_Tree_Browser)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(auto_branches,-1,_SC("xb"),SQFalse),
	_DECL_FUNC(show_branches,-1,_SC("xb"),SQFalse),
	_DECL_FUNC(all_branches_always_open,-1,_SC("xb"),SQFalse),
	_DECL_FUNC(branch_text,4,_SC("xiii"),SQFalse),
	_DECL_FUNC(leaf_text,4,_SC("xiii"),SQFalse),
	_DECL_FUNC(shaded_entry_colors,3,_SC("xii"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

#define SETUP_FLV_STYLE(v) SETUP_FL_KLASS(v, Flv_Style)
#define FLV_STYLE_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_Flv_Style_, SETUP_FLV_STYLE, self->, funcNAME, typeNAME)
#define FLV_STYLE_GETSET_INT(funcNAME) FLV_STYLE_GETSET_INT_CAST(funcNAME, int)


//no constructor for Flv_Style
/*
static SQInteger Flv_Style_release_hook(SQUserPointer p, SQInteger size, HSQUIRRELVM v){
    Flv_Style *self = (Flv_Style*)p;
    if(self) delete self;
    return 0;
}

static SQInteger Flv_Style_constructor(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_STYLE(v);
    self = new Flv_Style();
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v, 1, Flv_Style_release_hook);
    return 1;
}
*/

/*
#define RET_BOOL_IF(n, nc, nm) if(name[n] == nc) {sq_pushbool(v, self->nm()); return 1;}
#define GET_SET_BOOL_IF(n, nc, nm) if(name[n] == nc) {if(_top_ > 1) {SQ_GET_BOOL(v, 2, bparm); self->nm(bparm); return 0;} \
    else {sq_pushbool(v, self->nm()); return 1;}}
#define GET_SET_INT_IF(n, nc, nm) if(name[n] == nc) {if(_top_ > 1) {SQ_GET_INTEGER(v, 2, iparm); self->nm(iparm); return 0;} \
    else {sq_pushinteger(v, self->nm()); return 1;}}

static SQInteger Flv_Style_class(HSQUIRRELVM v){
    SQ_FUNC_VARS(v);
    Flv_Style *self;
    if(sq_getinstanceup(v, 1, (void**)&self, (void*)FLTK_TAG(Flv_Style)) < 0) return SQ_ERROR;
    if(sq_getclosurename(v, 0) == SQ_OK) {
        SQ_GET_STRING(v, -1, name);
        sq_poptop(v);
printf("%d %s %p %d\n", __LINE__, name, self, _top_);
        SQChar c = name[0];
        if(c == 'a'){
            //add
            //align
            //align_defined
            RET_BOOL_IF(6, 'd', align_defined);
            //all_clear
            //all_defined
            RET_BOOL_IF(4, 'd', all_defined);
        }
        else if(c == 'b'){
            //background
            //background_defined
            RET_BOOL_IF(11, 'd', background_defined);
            //border
            GET_SET_INT_IF(1, 'o', border);
            //border_color
            //border_color_defined
            RET_BOOL_IF(7, 'c', border_color_defined);
            //border_defined
            RET_BOOL_IF(7, 'd', border_defined);
            //border_spacing
            GET_SET_INT_IF(7, 's', border_spacing);
            //border_spacing_defined
            RET_BOOL_IF(15, 'd', border_spacing_defined);
            //bottom_border
        }
        else if(c == 'c'){
            //cell_style
            //clear_align
            //clear_all
            //clear_background
            //clear_border
            //clear_border_color
            //clear_border_spacing
            //clear_editor
            //clear_font
            //clear_font_size
            //clear_foreground
            //clear_frame
            //clear_height
            //clear_locked
            //clear_resizable
            //clear_x_margin
            //clear_y_margin
            //clear_width
        }
        else if(c == 'e'){
            //editor
            //editor_defined
            RET_BOOL_IF(7, 'd', editor_defined);
        }
        else if(c == 'f'){
            //font
            //font_defined
            RET_BOOL_IF(5, 'd', font_defined);
            //font_size
            GET_SET_INT_IF(5, 's', font_size);
            //font_size_defined
            RET_BOOL_IF(10, 'd', font_size_defined);
            //foreground
            //foreground_defined
            RET_BOOL_IF(11, 'd', foreground_defined);
            //frame
            //frame_defined
            RET_BOOL_IF(6, 'd', frame_defined);
        }
        else if(c == 'h'){
            //height
            GET_SET_INT_IF(2, 'i', height);
            //height_defined
            RET_BOOL_IF(7, 'd', height_defined);
        }
        else if(c == 'i'){
            //inner_bottom_border
            //inner_left_border
            //inner_right_border
            //inner_top_border
        }
        else if(c == 'l'){
            //left_border
            //locked
            GET_SET_BOOL_IF(2, 'c', locked);
            //locked_defined
            RET_BOOL_IF(7, 'd', locked_defined);
        }
        else if(c == 'r'){
            //resizable
            GET_SET_BOOL_IF(2, 's', resizable);
            //resizable_defined
            RET_BOOL_IF(10, 'd', resizable_defined);
            //right_border
        }
        else if(c == 't'){
            //top_border
        }
        else if(c == 'x'){
            //x_margin_defined
            RET_BOOL_IF(9, 'd', x_margin_defined);
            //x_margin
            GET_SET_INT_IF(2, 'm', x_margin);
        }
        else if(c == 'y'){
            //y_margin_defined
            RET_BOOL_IF(9, 'd', y_margin_defined);
            //y_margin
            GET_SET_INT_IF(2, 'm', y_margin);
        }
        else if(c == 'w'){
            //width_defined
            RET_BOOL_IF(6, 'd', width_defined);
            //width
            GET_SET_INT_IF(2, 'd', width);
        }
    }
    return SQ_ERROR;
}
*/

FLV_STYLE_GETSET_INT(border);
FLV_STYLE_GETSET_INT(border_spacing);
FLV_STYLE_GETSET_INT(font_size);
FLV_STYLE_GETSET_INT(height);
FLV_STYLE_GETSET_INT(x_margin);
FLV_STYLE_GETSET_INT(y_margin);
FLV_STYLE_GETSET_INT(width);
FLV_STYLE_GETSET_INT_CAST(align, Fl_Align);
FLV_STYLE_GETSET_INT_CAST(border_color, Fl_Color);
FLV_STYLE_GETSET_INT_CAST(foreground, Fl_Color);
FLV_STYLE_GETSET_INT_CAST(background, Fl_Color);

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flv_Style_##name,nparams,pmask,isStatic}
static SQRegFunction flv_style_obj_funcs[]={
    //no constructor for Flv_Style
	//{_SC("constructor"), Flv_Style_constructor, -1,_SC("xi")},
	_DECL_FUNC(border,-1,_SC("xn"),SQFalse),
	_DECL_FUNC(border_spacing,-1,_SC("xn"),SQFalse),
	_DECL_FUNC(font_size,-1,_SC("xn"),SQFalse),
	_DECL_FUNC(height,-1,_SC("xn"),SQFalse),
	_DECL_FUNC(x_margin,-1,_SC("xn"),SQFalse),
	_DECL_FUNC(y_margin,-1,_SC("xn"),SQFalse),
	_DECL_FUNC(width,-1,_SC("xn"),SQFalse),
	_DECL_FUNC(align,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(border_color,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(foreground,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(background,-1,_SC("xi"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

#define SETUP_FLV_STYLE_LIST(v) SETUP_FL_KLASS(v, Flv_Style_List)

static SQRESULT _Flv_Style_List_get(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_STYLE_LIST(v);
    SQ_GET_INTEGER(v, 2, idx);
    fltk_pushinstance(v, FLTK_TAG(Flv_Style), &self->get(idx));
    return 1;
}

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flv_Style_List_##name,nparams,pmask,isStatic}
static SQRegFunction flv_style_list_obj_funcs[]={
	_DECL_FUNC(get,-1,_SC("xi"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Flv_List);
#define SETUP_FLV_LIST(v) SETUP_FL_KLASS(v, Flv_List)

#define FLV_LIST_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_Flv_List_, SETUP_FLV_LIST, self->, funcNAME, typeNAME)
#define FLV_LIST_GETSET_INT(funcNAME) FLV_LIST_GETSET_INT_CAST(funcNAME, int)
#define FLV_LIST_GETSET_INT_ONE_CALL_CAST(funcNAME, typeNAME) FUNC_GETSET_INT_ONE_CALL(_Flv_List_, SETUP_FLV_LIST, self->, funcNAME, typeNAME)
FLV_LIST_GETSET_INT_CAST(has_scrollbar, Flv_ShowScrollbar);
FLV_LIST_GETSET_INT(scrollbar_width);
FLV_LIST_GETSET_INT_ONE_CALL_CAST(feature, int);
FLV_LIST_GETSET_INT_ONE_CALL_CAST(feature_add, int);
FLV_LIST_GETSET_INT_ONE_CALL_CAST(feature_remove, int);
FLV_LIST_GETSET_INT(rows);
FLV_LIST_GETSET_INT(row);
FLV_LIST_GETSET_INT(why_event);
FLV_LIST_GETSET_INT(callback_when);

static SQRESULT _Flv_List_global_style(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_LIST(v);
    fltk_pushinstance(v, FLTK_TAG(Flv_Style), &self->global_style);
    return 1;
}

static SQRESULT _Flv_List_row_style(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_LIST(v);
    fltk_pushinstance(v, FLTK_TAG(Flv_Style_List), &self->row_style);
    return 1;
}

static SQRESULT _Flv_List_add_callback_when(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_LIST(v);
    SQ_GET_INTEGER(v, 2, when);
    self->add_callback_when(when);
    return 1;
}

static SQRESULT _Flv_List_clear_callback_when(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_LIST(v);
    SQ_GET_INTEGER(v, 2, when);
    self->clear_callback_when(when);
    return 1;
}
CHEAP_RTTI_FOR(Flv_List);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flv_List_##name,nparams,pmask,isStatic}
static SQRegFunction flv_list_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Flv_List)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(has_scrollbar,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(scrollbar_width,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(feature,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(feature_add,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(feature_remove,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(rows,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(row,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(global_style,1,_SC("x"),SQFalse),
	_DECL_FUNC(row_style,1,_SC("x"),SQFalse),
	_DECL_FUNC(why_event,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(callback_when,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(add_callback_when,2,_SC("xi"),SQFalse),
	_DECL_FUNC(clear_callback_when,2,_SC("xi"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Flv_Table);
#define SETUP_FLV_TABLE(v) SETUP_FL_KLASS(v, Flv_Table)
#define FLV_TABLE_GETSET_INT0(funcNAME, typeNAME) FUNC_GETSET_INT(_Flv_Table_, SETUP_FLV_TABLE, self->, funcNAME, typeNAME)
#define FLV_TABLE_GETSET_INT(funcNAME) FLV_TABLE_GETSET_INT0(funcNAME, int)

FLV_TABLE_GETSET_INT(cols);
FLV_TABLE_GETSET_INT(col);

static SQRESULT _Flv_Table_col_style(HSQUIRRELVM v){
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_TABLE(v);
    fltk_pushinstance(v, FLTK_TAG(Flv_Style_List), &self->col_style);
    return 1;
}
CHEAP_RTTI_FOR(Flv_Table);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flv_Table_##name,nparams,pmask,isStatic}
static SQRegFunction flv_table_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Flv_Table)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(cols,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(col,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(col_style,1,_SC("x"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC


CREATE_TAG(Flv_Data_Table);
#define SETUP_FLV_DATA_TABLE(v) SETUP_FL_KLASS(v, Flv_Data_Table)

#define FL_Key_Space 32
#define FL_Key_Plus 61
#define FL_Key_Minus 45
#define FL_KP_Plus 65451
#define FL_KP_Minus 65453
#define FL_KP_Times 65450
#define FL_KP_Division 65455
#define FL_KP_Dot 65454
#define FL_Key_0 '0'
#define FL_Key_9 '9'
#define FL_KP_0 FL_KP + '0'
#define FL_KP_1 FL_KP + '1'
#define FL_KP_2 FL_KP + '2'
#define FL_KP_3 FL_KP + '3'
#define FL_KP_4 FL_KP + '4'
#define FL_KP_5 FL_KP + '5'
#define FL_KP_6 FL_KP + '6'
#define FL_KP_7 FL_KP + '7'
#define FL_KP_8 FL_KP + '8'
#define FL_KP_9 FL_KP + '9'

class Flv_Data_Table : public Flv_Table
{
public:
    int selection_count, _draw_offset;
    bool _forPrint;

    Flv_Data_Table(int X, int Y, int W=340, int H=202, const char *L=0):
        Flv_Table(X, Y, W, H, L)
    {
        _draw_offset = 5;
        selection_count = 0;
        _forPrint = false;

        callback_when( FLVEcb_ROW_CHANGED | FLVEcb_CLICKED);
        selection_color(FL_BLUE);
        feature(FLVF_ROW_HEADER | FLVF_DIVIDERS | //flvw.FLVF_MULTI_SELECT,
                FLVF_ROW_SELECT | FLVF_PERSIST_SELECT);
        has_scrollbar( FLVS_VERTICAL_ALWAYS );

        Flv_Style &gs = global_style;
        gs.resizable(true);
        gs.width(80);
        gs.height(22);

        dead_space_color(FL_WHITE);

        row_style.get(-1).align(FL_ALIGN_CLIP);
        col_style.get(-1).align(FL_ALIGN_CLIP);
    }
    void resize(int x, int y, int w, int h)
    {
        bool done = false;
        HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
        SQInteger top = sq_gettop(v);
        if(sq_get_fl_virtual(v, this, "resize") == SQ_OK){
            sq_push(v, -2); //this
            sq_pushinteger(v, x);
            sq_pushinteger(v, y);
            sq_pushinteger(v, w);
            sq_pushinteger(v, h);
            if(sq_call(v, 5, SQFalse, SQTrue) == SQ_OK){
                    done = true;
            }
        }
        sq_settop(v, top);
        if(!done) Flv_Table::resize(x, y, w, h);
    }

    void textsize(Fl_Fontsize s)
    {
        global_style.font_size(s);
        global_style.height(s + 8);
        //calc_cols();
    }

    Fl_Fontsize textsize() const
    {
        return global_style.font_size();
    }

    void get_style(Flv_Style &s, int R, int C )
    {
        Flv_Table::get_style(s, R, C);
        if(R >= 0 && C >= 0)
        {
            Flv_Style &rs = row_style.get(R);
            Flv_Style &cs = col_style.get(C);
            Fl_Color have_bg = 0;
            if(cs.background_defined()) have_bg = cs.background();
            else if(rs.background_defined()) have_bg = rs.background();
            if((R % 2) == 0)
            {
                if(have_bg)
                    //s:background(fltk.fl_color_average(207, have_bg, 0.5))
                    s.background(have_bg);
                else
                    s.background(207);
            }
            else
            {
                if(have_bg)
                    //s:background(have_bg)
                    s.background(fl_color_average(FL_WHITE, have_bg, 0.5));
                else
                    s.background(FL_WHITE);
            }
        }
        else if(_forPrint)
        {
            s.frame(FL_NO_BOX);
        }
    }

    virtual void draw_cell( int Offset, int &X, int &Y, int &W, int &H, int R, int C ){
        Flv_Style s;
        get_style(s, R, C);
        Flv_Table::draw_cell(Offset, X, Y, W, H, R, C);

        HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
        SQInteger top = sq_gettop(v);
        if(sq_get_fl_virtual(v, this, "get_value") == SQ_OK){
            sq_push(v, -2); //this
            sq_pushinteger(v, R);
            sq_pushinteger(v, C);
            if(sq_call(v, 3, SQTrue, SQTrue) == SQ_OK){
                if(sq_gettype(v, -1) == OT_STRING){
                    const SQChar *str;
                    sq_getstring(v, -1, &str);
                    fl_draw(str, X-Offset+_draw_offset, Y, W-_draw_offset*3, H, s.align());
                }
            }
        }
        sq_settop(v, top);
    }

    int handle(int event){
        switch(event)
        {
        case FL_KEYBOARD:
        {
            int key = Fl::event_key();
            //printf("%d\n", key);
            switch(key)
            {
            case FL_KP+'3':
                Fl::e_keysym = FL_Page_Down; //PageDown
                break;
            case FL_KP+'2':
                Fl::e_keysym = FL_Down; //ArrowDown
                break;
            case FL_KP+'8':
                Fl::e_keysym = FL_Up; //ArrowUp
                break;
            case FL_KP+'9':
                Fl::e_keysym = FL_Page_Up; //PageUp
                break;
            case FL_KP+'0':
                mark_row(false, true); //PageUp
                break;
                //case FL_Menu: grid->show_popup_menu(); //PageUp
                //break;
            case FL_KP_Plus:
            case FL_KP_Minus:
            case FL_Key_Plus:
            case FL_Key_Minus:
                //printf("Key = %d\n", key);
                if(Fl::event_ctrl())
                {
                    switch(key)
                    {
                    case FL_KP_Plus:
                    case FL_Key_Plus:
                        textsize(textsize()+1);
                        redraw();
                        break;
                    case FL_KP_Minus:
                    case FL_Key_Minus:
                    {
                        int tsize = textsize();
                        if(tsize > 9)
                        {
                            textsize(tsize-1);
                            redraw();
                        }
                    }
                    break;
                    }
                }
                break;
                //default:
                //handled = handle_key(key);
            }
        }
        break;
        case FL_PUSH:
        {
            switch(Fl::event_button())
            {
            case FL_LEFT_MOUSE:
                take_focus();
                break;
            }
        }
        break;
        case FL_RELEASE:
        {
            mark_row(true);
        }
        break;
        }

        HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
        SQInteger top = sq_gettop(v);
        if(sq_get_fl_virtual(v, this, "handle") == SQ_OK){
            sq_push(v, -2); //this
            sq_pushinteger(v, event);
            if(sq_call(v, 2, SQTrue, SQTrue) == SQ_OK){
                if(sq_gettype(v, -1) == OT_INTEGER){
                    sq_getinteger(v, -1, &event);
                    sq_settop(v, top);
                    return event;
                }
            }
            //exit(-1);
        }
        sq_settop(v, top);
        return Flv_Table::handle(event);
    }

    virtual Fl_Color get_multi_select_color()
    {
        return fl_color_average(FL_RED, FL_WHITE, 0.5);;
    }

    void clear_selection()
    {
        if(selection_count == 0) return;
        Fl_Color scolor = get_multi_select_color();
        for(int i=0; i < rows(); ++i)
        {
            Flv_Style  *rs = row_style.find(i);
            if(!rs) continue;
            if(rs->background_defined() && rs->background() == scolor)
            {
                rs->clear_background();
            }
        }
        selection_count = 0;
    }

    void mark_row(bool withCtrl, bool withAdvance=false){
        if(!withCtrl || Fl::event_ctrl())
        {
            //this->on_Mark_Row();
            Flv_Style  &rs = row_style.get(row());
            Fl_Color scolor = get_multi_select_color();
            if(rs.background_defined() && rs.background() == scolor)
            {
                --selection_count;
                rs.clear_background();
            }
            else
            {
                ++selection_count;
                rs.background(scolor);
            }
            //return 1;
        }
        if(withAdvance){
            int next_row = row()+1;
            if(rows() > next_row) row(next_row);
        }
    }
};

static SQInteger _Flv_Data_Table_handle(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_DATA_TABLE(v);
    SQ_GET_INTEGER(v, 2, event);
    sq_pushinteger(v, ((Flv_Data_Table*)self)->Flv_Table::handle(event));
    return 1;
}

static SQInteger _Flv_Data_Table_resize(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FLV_DATA_TABLE(v);
    SQ_GET_INTEGER(v, 2, x);
    SQ_GET_INTEGER(v, 3, y);
    SQ_GET_INTEGER(v, 4, w);
    SQ_GET_INTEGER(v, 5, h);
    ((Flv_Data_Table*)self)->Flv_Table::resize(x, y, w, h);
    return 0;
}

static SQInteger _Flv_Data_Table_draw_offset(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FLV_DATA_TABLE(v);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, offset);
        ((Flv_Data_Table*)self)->_draw_offset = offset;
        return 0;
    }
    sq_pushinteger(v, ((Flv_Data_Table*)self)->_draw_offset);
    return 1;
}

FLTK_CONSTRUCTOR(Flv_Data_Table);

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Flv_Data_Table_##name,nparams,pmask,isStatic}
static SQRegFunction flv_data_table_obj_funcs[]={
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(handle,2,_SC("xi"),SQFalse),
	_DECL_FUNC(resize,5,_SC("xnnnn"),SQFalse),
	_DECL_FUNC(draw_offset, -1,_SC("xn"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Scroll);
CHEAP_RTTI_FOR(Fl_Scroll);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Scroll_##name,nparams,pmask,isStatic}
static SQRegFunction fl_scroll_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Scroll)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Tabs);
CHEAP_RTTI_FOR(Fl_Tabs);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Tabs_##name,nparams,pmask,isStatic}
static SQRegFunction fl_tabs_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Tabs)
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};
#undef _DECL_FUNC


static SQInteger _Fl_Image_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_Image *self = ((Fl_Image *)p);
	delete self;
	return 0;
}

static SQInteger _Fl_Image_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_IMAGE(v);
    SQ_GET_INTEGER(v, 2, iw);
    SQ_GET_INTEGER(v, 3, ih);
    SQ_GET_INTEGER(v, 4, id);
	self = new Fl_Image(iw, ih, id);
    //do_register_object_and_instance(v, 1, cptr);
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, _Fl_Image_releasehook);
	return 1;
}

static SQInteger _Fl_Image_copy(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_IMAGE(v);
    Fl_Image *img;
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, iw);
        SQ_GET_INTEGER(v, 3, ih);
        img = self->copy(iw, ih);
    }
    else img = self->copy();
	return fltk_pushinstance(v, Fl_Image_TAG, img);
}

#define FUNC_FL_IMAGE_INT_CALL(fn) FUNC_INT_CALL(_Fl_Image_, SETUP_FL_IMAGE, fn)
FUNC_FL_IMAGE_INT_CALL(w);
FUNC_FL_IMAGE_INT_CALL(h);
FUNC_FL_IMAGE_INT_CALL(d);
FUNC_FL_IMAGE_INT_CALL(ld);
FUNC_FL_IMAGE_INT_CALL(count);

static SQInteger _Fl_Image_data(HSQUIRRELVM v)
{
    SETUP_FL_IMAGE(v);
    sq_pushuserpointer(v, (SQUserPointer)self->data());
	return 1;
}

static SQInteger _Fl_Image_uncache(HSQUIRRELVM v)
{
    SETUP_FL_IMAGE(v);
    self->uncache();
	return 0;
}
CHEAP_RTTI_FOR(Fl_Image);
#define _DECL_FUNC(name,nparams,pmask, isStatic) {_SC(#name),_Fl_Image_##name,nparams,pmask, isStatic}
static SQRegFunction fl_image_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Image)
	_DECL_FUNC(constructor,4,_SC("xiii"), SQFalse),
	_DECL_FUNC(copy,-1,_SC("xii"), SQFalse),
	_DECL_FUNC(w,1,_SC("x"), SQFalse),
	_DECL_FUNC(h,1,_SC("x"), SQFalse),
	_DECL_FUNC(d,1,_SC("x"), SQFalse),
	_DECL_FUNC(ld,1,_SC("x"), SQFalse),
	_DECL_FUNC(count,1,_SC("x"), SQFalse),
	_DECL_FUNC(data,1,_SC("x"), SQFalse),
	_DECL_FUNC(uncache,1,_SC("x"), SQFalse),
	{0,0}
};
#undef _DECL_FUNC

static SQChar Fl_RGB_Image_TAG[] = _SC("Fl_RGB_Image");
#define SETUP_FL_RGB_IMAGE(v) SETUP_KLASS(v, 1, self, Fl_RGB_Image, Fl_RGB_Image_TAG)

static SQInteger _Fl_RGB_Image_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_RGB_IMAGE(v);
    SQ_GET_STRING(v, 2, img);
    SQ_GET_INTEGER(v, 3, iw);
    SQ_GET_INTEGER(v, 4, ih);
    SQ_OPT_INTEGER(v, 5, id, 3);
    SQ_OPT_INTEGER(v, 6, ild, 0);
	self = new Fl_RGB_Image((const uchar*)img, iw, ih, id, ild);
    //do_register_object_and_instance(v, 1, cptr);
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, _Fl_Image_releasehook);
	return 1;
}

static SQInteger _Fl_RGB_Image_max_size(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, size);
        Fl_RGB_Image::max_size(size);
        return 0;
    }
    sq_pushinteger(v, Fl_RGB_Image::max_size());
	return 1;
}
CHEAP_RTTI_FOR(Fl_RGB_Image);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_RGB_Image_##name,nparams,pmask,isStatic}
static SQRegFunction fl_rgb_image_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_RGB_Image)
	_DECL_FUNC(constructor,-4,_SC("xsiiii"), SQFalse),
	_DECL_FUNC(max_size,-1,_SC("yi"), SQTrue),
	{0,0}
};
#undef _DECL_FUNC

static SQChar Fl_JPEG_Image_TAG[] = _SC("Fl_JPEG_Image");
#define SETUP_FL_JPEG_IMAGE(v) SETUP_KLASS(v, 1, self, Fl_JPEG_Image, Fl_JPEG_Image_TAG)

static SQInteger _Fl_JPEG_Image_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_JPEG_IMAGE(v);
    SQ_GET_STRING(v, 2, img_name);
    if(_top_ > 2){
        SQ_GET_STRING(v, 3, img);
        self = new Fl_JPEG_Image(img_name, (const uchar*)img);
    }
    else self = new Fl_JPEG_Image(img_name);
    //do_register_object_and_instance(v, 1, cptr);
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, _Fl_Image_releasehook);
	return 1;
}

static SQInteger _Fl_JPEG_Image_encode(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_IMAGE_AT(v, 2, img);
    SQ_GET_INTEGER(v, 3, quality);
    unsigned char *outbuffer;
    int outlen;
    if(Fl_JPEG_Image::encode(img, quality, &outbuffer, outlen))
        sq_throwerror(v, _SC("error trying to encode image"));
    sq_pushstring(v, (const SQChar*)outbuffer, outlen);
    free(outbuffer);
	return 1;
}
CHEAP_RTTI_FOR(Fl_JPEG_Image);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_JPEG_Image_##name,nparams,pmask,isStatic}
static SQRegFunction fl_jpeg_image_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_JPEG_Image)
	_DECL_FUNC(constructor,-2,_SC("xss"), SQFalse),
	_DECL_FUNC(encode,3,_SC("yxi"), SQTrue),
	{0,0}
};
#undef _DECL_FUNC

static SQChar Fl_PNG_Image_TAG[] = _SC("Fl_PNG_Image");
#define SETUP_FL_PNG_IMAGE(v) SETUP_KLASS(v, 1, self, Fl_PNG_Image, Fl_PNG_Image_TAG)

static SQInteger _Fl_PNG_Image_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_PNG_IMAGE(v);
    SQ_GET_STRING(v, 2, img_name);
    if(_top_ > 2){
        SQ_GET_STRING(v, 3, img);
        SQ_GET_INTEGER(v, 4, size);
        self = new Fl_PNG_Image(img_name, (const uchar*)img, size);
    }
    else self = new Fl_PNG_Image(img_name);
    //do_register_object_and_instance(v, 1, cptr);
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, _Fl_Image_releasehook);
	return 1;
}

static SQInteger _Fl_PNG_Image_encode(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_IMAGE_AT(v, 2, img);
    unsigned char *outbuffer;
    int outlen;
    if(Fl_PNG_Image::encode(img, &outbuffer, outlen))
        sq_throwerror(v, _SC("error trying to encode image"));
    sq_pushstring(v, (const SQChar*)outbuffer, outlen);
    free(outbuffer);
	return 1;
}

CHEAP_RTTI_FOR(Fl_PNG_Image);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_PNG_Image_##name,nparams,pmask,isStatic}
static SQRegFunction fl_png_image_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_PNG_Image)
	_DECL_FUNC(constructor,-2,_SC("xssi"),SQFalse),
	_DECL_FUNC(encode,2,_SC("yx"), SQTrue),
	{0,0}
};
#undef _DECL_FUNC

static SQChar Fl_Text_Buffer_TAG[] = _SC("Fl_Text_Buffer");

static SQInteger _Fl_Text_Buffer_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_Text_Buffer *self = ((Fl_Text_Buffer *)p);
	delete self;
	return 0;
}

static SQInteger _Fl_Text_Buffer_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, requestedSize, 0);
    SQ_OPT_INTEGER(v, 3, preferredGapSize, 1024);
	Fl_Text_Buffer *cptr = new Fl_Text_Buffer(requestedSize, preferredGapSize);
    //do_register_object_and_instance(v, 1, cptr);
    sq_setinstanceup(v, 1, cptr);
    sq_setreleasehook(v,1, _Fl_Text_Buffer_releasehook);
	return 1;
}

#define SETUP_FL_TEXT_BUFFER(v) SETUP_KLASS(v, 1, self, Fl_Text_Buffer, Fl_Text_Buffer_TAG)
FUNC_GETSET_STR(_Fl_Text_Buffer_, SETUP_FL_TEXT_BUFFER, self->, text);
FUNC_INT_CALL(_Fl_Text_Buffer_, SETUP_FL_TEXT_BUFFER, length);

static SQInteger _Fl_Text_Buffer_loadfile(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SETUP_FL_TEXT_BUFFER(v);
    SQ_GET_STRING(v, 2, file_name);
    SQ_OPT_INTEGER(v, 3, buflen, 128*1024);
    sq_pushinteger(v, self->loadfile(file_name, buflen));
	return 1;
}

static SQInteger _Fl_Text_Buffer_input_file_was_transcoded(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SETUP_FL_TEXT_BUFFER(v);
    sq_pushinteger(v, self->input_file_was_transcoded);
	return 1;
}

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Text_Buffer_##name,nparams,pmask,isStatic}
static SQRegFunction fl_text_buffer_obj_funcs[]={
	_DECL_FUNC(constructor,-1,_SC("xii"),SQFalse),
	_DECL_FUNC(text,-1,_SC("xs"),SQFalse),
	_DECL_FUNC(length,-1,_SC("xii"),SQFalse),
	_DECL_FUNC(loadfile,-2,_SC("xsi"),SQFalse),
	_DECL_FUNC(input_file_was_transcoded,1,_SC("x"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

FLTK_CONSTRUCTOR(Fl_Text_Display);
#define SETUP_FL_TEXT_DISPLAY(v) SETUP_FL_KLASS(v, Fl_Text_Display)

static SQInteger _Fl_Text_Display_buffer(HSQUIRRELVM v)
{
    SETUP_FL_TEXT_DISPLAY(v);
    SQUserPointer rsz;
    sq_getinstanceup(v, -1, &rsz, 0);
    self->buffer((Fl_Text_Buffer*)rsz);
	return 0;
}

static SQInteger _Fl_Text_Display_highlight_data(HSQUIRRELVM v)
{
    SETUP_FL_TEXT_DISPLAY(v);
    SQUserPointer rsz;
	return 0;
}
CHEAP_RTTI_FOR(Fl_Text_Display);
#define _DECL_FL_TEXT_DISPLAY_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Text_Display_##name,nparams,pmask,isStatic}
static SQRegFunction fl_text_display_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Text_Display)
	_DECL_FL_TEXT_DISPLAY_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FL_TEXT_DISPLAY_FUNC(buffer,-1,_SC("xx"),SQFalse),
	_DECL_FL_TEXT_DISPLAY_FUNC(highlight_data, 7,_SC("xxaiic."),SQFalse),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Editor);
CHEAP_RTTI_FOR(Fl_Text_Editor);
#define _DECL_FL_TEXT_EDITOR_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Text_Editor_##name,nparams,pmask,isStatic}
static SQRegFunction fl_text_editor_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Text_Editor)
	_DECL_FL_TEXT_EDITOR_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Editor_Buffered);
#define SETUP_FL_TEXT_EDITOR_BUFFERED(v) SETUP_FL_KLASS(v, Fl_Text_Editor_Buffered)

static SQInteger _Fl_Text_Editor_Buffered_value(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    const SQChar *txt;
    SETUP_FL_TEXT_EDITOR_BUFFERED(v);
    if(_top_ > 1){
        sq_getstring(v, 2, &txt);
        self->buffer()->text(txt);
        return 0;
    }
    txt = self->buffer()->text();
    sq_pushstring(v, txt, -1);
    return 1;
}

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Text_Editor_Buffered_##name,nparams,pmask,isStatic}
static SQRegFunction fl_text_editor_buffered_obj_funcs[]={
	_DECL_FUNC(constructor,-5,FLTK_constructor_Mask, SQFalse),
	_DECL_FUNC(value,-1,_SC("xs"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

//CREATE_TAG(MyFl_Window);
//#define SETUP_FL_WINDOW(v) SETUP_FL_KLASS(v, Fl_Window)
#define SETUP_FL_WINDOW(v) \
	MyFl_Window *self = NULL; \
	{ if(SQ_FAILED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(void*)FLTK_TAG(Fl_Window)))) \
		return SQ_ERROR; }


static Fl_Preferences *_appPreferences = 0;

class MyFl_Window : public Fl_Double_Window {
    typedef Fl_Double_Window SUPERCLASS;
    typedef MyFl_Window THISCLASS;

    void _init(){
        isFirstTimeShow=true;
    }
    public:
    bool isFirstTimeShow;

    MyFl_Window(int X, int Y, int W, int H, const char *L=0):Fl_Double_Window(X,Y,W,H,L){
        _init();
    }
    MyFl_Window(int W, int H, const char *L=0):Fl_Double_Window(W,H,L){
        _init();
    }
    ~MyFl_Window()
    {
        if(_appPreferences && (x() >= 0) && (y() >= 0))  {
            Fl_Preferences winPref(_appPreferences, label());
            winPref.set("win_x", x());
            winPref.set("win_y", y());
            winPref.set("win_w", w());
            winPref.set("win_h", h());
        }
        //if(_toClearOnDestroy) *_toClearOnDestroy = 0;
        //if(_safe_ptr) delete _safe_ptr;
    }

    void hide(){sq_call_fl_virtual(this, "hide");}

    void show(){
        if(isFirstTimeShow){
            on_first_time_show();
            if(_appPreferences) {
                Fl_Preferences winPref(_appPreferences, label());
                int prefx, prefy, prefw, prefh;
                winPref.get("win_x", prefx, x());
                winPref.get("win_y", prefy, y());
                winPref.get("win_w", prefw, w());
                winPref.get("win_h", prefh, h());
                int X, Y, W, H;
                Fl::screen_xywh(X, Y, W, H);
                //try prevent window be invizible
                if( (prefx <= X+W-100) && (prefy <= Y+H-100) && (prefw > 100) && (prefh > 100)){
                    resize(prefx, prefy, prefw, prefh);
                }
            }

            isFirstTimeShow = false;
        }
        SUPERCLASS::show();
    }
    void on_first_time_show(){
        sq_call_fl_virtual(this, "on_first_time_show");
    }
};

static SQInteger _MyFl_Window_hide(HSQUIRRELVM v)
{
    SETUP_FL_WINDOW(v);
    self->Fl_Double_Window::hide();
	return 0;
}

extern int sq_main_argc;
extern char **sq_main_argv;

static SQInteger _MyFl_Window_show_main(HSQUIRRELVM v)
{
    SETUP_FL_WINDOW(v);
    int i;
	Fl::args(sq_main_argc, sq_main_argv, i);
    self->Fl_Double_Window::show(sq_main_argc, sq_main_argv);
	return 0;
}

static SQInteger _MyFl_Window_on_first_time_show(HSQUIRRELVM v)
{
	return 0;
}

static SQInteger _fl_window_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	MyFl_Window *self = ((MyFl_Window *)p);
	Fl::delete_widget(self);
//printf("Releasing %p\n", self);
	return 0;
}

static SQInteger _MyFl_Window_set_non_modal(HSQUIRRELVM v)
{
    SETUP_FL_WINDOW(v);
    self->set_non_modal();
	return 0;
}

static SQInteger _MyFl_Window_shown(HSQUIRRELVM v)
{
    SETUP_FL_WINDOW(v);
    sq_pushbool(v, self->shown());
	return 1;
}

FLTK_CONSTRUCTOR_RELEASE_WINDOW(MyFl_Window, AS_IS, _fl_window_releasehook);
CHEAP_RTTI_FOR(Fl_Window);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_MyFl_Window_##name,nparams,pmask,isStatic}
static SQRegFunction fl_window_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Window)
	_DECL_FUNC(constructor,-3,_SC("xii"),SQFalse),
	_DECL_FUNC(show_main,1,_SC("x"),SQFalse),
	_DECL_FUNC(shown,1,_SC("x"),SQFalse),
	_DECL_FUNC(hide,1,_SC("x"),SQFalse),
	_DECL_FUNC(set_non_modal,1,_SC("x"),SQFalse),
	_DECL_FUNC(on_first_time_show,1,_SC("x"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

static SQInteger _fl_double_window_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_Double_Window *self = ((Fl_Double_Window *)p);
	Fl::delete_widget(self);
//printf("Releasing %p\n", self);
	return 0;
}

FLTK_CONSTRUCTOR_RELEASE_WINDOW(Fl_Double_Window, AS_IS, _fl_double_window_releasehook);
CHEAP_RTTI_FOR(Fl_Double_Window);
#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Double_Window_##name,nparams,pmask,isStatic}
static SQRegFunction fl_double_window_obj_funcs[]={
    CHEAP_RTTI_REG_FUN_FOR(Fl_Double_Window)
	_DECL_FUNC(constructor,-3,_SC("xii"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

static SQChar Fl_File_Chooser_TAG[] = _SC("Fl_File_Chooser");

static SQInteger _Fl_File_Chooser_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_File_Chooser *self = ((Fl_File_Chooser *)p);
	delete self;
	return 0;
}

static SQInteger _Fl_File_Chooser_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, pathname);
    SQ_GET_STRING(v, 3, pattern);
    SQ_GET_INTEGER(v, 4, type);
    SQ_GET_STRING(v, 5, title);

	Fl_File_Chooser *cptr = new Fl_File_Chooser(pathname, pattern, type, title);
    //do_register_object_and_instance(v, 1, cptr);
    sq_setinstanceup(v, 1, cptr);
    sq_setreleasehook(v,1, _Fl_File_Chooser_releasehook);
	return 1;
}

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_File_Chooser_##name,nparams,pmask,isStatic}
static SQRegFunction fl_file_chooser_obj_funcs[]={
	_DECL_FUNC(constructor,5,_SC("xssis"),SQFalse),
	{0,0}
};
#undef _DECL_FUNC

static SQChar Fl_Native_File_Chooser_TAG[] = _SC("Fl_Native_File_Chooser");

static SQInteger _Fl_Native_File_Chooser_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_Native_File_Chooser *self = ((Fl_Native_File_Chooser *)p);
	delete self;
	return 0;
}

static SQInteger _Fl_Native_File_Chooser_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, type, Fl_Native_File_Chooser::BROWSE_FILE);

	Fl_Native_File_Chooser *cptr = new Fl_Native_File_Chooser(type);
    //do_register_object_and_instance(v, 1, cptr);
    sq_setinstanceup(v, 1, cptr);
    sq_setreleasehook(v,1, _Fl_Native_File_Chooser_releasehook);
	return 1;
}

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_Fl_Native_File_Chooser_##name,nparams,pmask,isStatic}
static SQRegFunction fl_native_file_chooser_obj_funcs[]={
	_DECL_FUNC(constructor,-1,_SC("xi"),SQFalse),
/*
	_DECL_FUNC(title,-1,_SC("xs"),SQFalse),
	_DECL_FUNC(type,-1,_SC("xi"),SQFalse),
	_DECL_FUNC(show,1,_SC("x"),SQFalse),
	_DECL_FUNC(filename,1,_SC("x"),SQFalse),
	_DECL_FUNC(directory,1,_SC("x"),SQFalse),
*/
	{0,0}
};
#undef _DECL_FUNC


#define FL_FUNC_NO_ARG_NO_RET(fn) static SQInteger _fl_##fn(HSQUIRRELVM v){Fl::fn();return 0;}
FL_FUNC_NO_ARG_NO_RET(check);
FL_FUNC_NO_ARG_NO_RET(do_widget_deletion);

#define FL_FUNC_NO_ARG_RET_INT(fn) static SQInteger _fl_##fn(HSQUIRRELVM v){sq_pushinteger(v, Fl::fn());return 1;}
FL_FUNC_NO_ARG_RET_INT(run);
FL_FUNC_NO_ARG_RET_INT(event);
FL_FUNC_NO_ARG_RET_INT(event_alt);
FL_FUNC_NO_ARG_RET_INT(event_button);
FL_FUNC_NO_ARG_RET_INT(event_button1);
FL_FUNC_NO_ARG_RET_INT(event_button2);
FL_FUNC_NO_ARG_RET_INT(event_button3);
FL_FUNC_NO_ARG_RET_INT(event_buttons);
FL_FUNC_NO_ARG_RET_INT(event_command);
FL_FUNC_NO_ARG_RET_INT(event_ctrl);
FL_FUNC_NO_ARG_RET_INT(event_dx);
FL_FUNC_NO_ARG_RET_INT(event_dy);
FL_FUNC_NO_ARG_RET_INT(event_length);
FL_FUNC_NO_ARG_RET_INT(event_shift);
FL_FUNC_NO_ARG_RET_INT(event_x);
FL_FUNC_NO_ARG_RET_INT(event_x_root);
FL_FUNC_NO_ARG_RET_INT(event_y);
FL_FUNC_NO_ARG_RET_INT(event_y_root);
FL_FUNC_NO_ARG_RET_INT(x);
FL_FUNC_NO_ARG_RET_INT(y);
FL_FUNC_NO_ARG_RET_INT(h);
FL_FUNC_NO_ARG_RET_INT(w);
FL_FUNC_NO_ARG_RET_INT(ready);
FL_FUNC_NO_ARG_RET_INT(screen_count);

#define FL_FUNC_GETSET_INT(fn) FUNC_GETSET_INT(_fl_, NOTHING, Fl::, fn, int)

FL_FUNC_GETSET_INT(event_key);
FL_FUNC_GETSET_INT(event_state);
FL_FUNC_GETSET_INT(event_is_click);
FL_FUNC_GETSET_INT(event_clicks);
FL_FUNC_GETSET_INT(scrollbar_size);
FL_FUNC_GETSET_INT(visible_focus);

#define FL_FUNC_GETSET_INT_ONE_CALL(fn) FUNC_GETSET_INT_ONE_CALL(_fl_, NOTHING, Fl::, fn, int)
FL_FUNC_GETSET_INT_ONE_CALL(visual);

#define FL_FUNC_GETSET_INT_BOOL(fn) FUNC_GETSET_INT_BOOL(_fl_, NOTHING, Fl::, fn, Fl::Fl_Option)
FL_FUNC_GETSET_INT_BOOL(option);

#define FL_FUNC_GETSET_STR(fn) FUNC_GETSET_STR(_fl_, NOTHING, Fl::, fn)
FL_FUNC_GETSET_STR(scheme);

static SQInteger _fl_delete_widget(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INSTANCE_VAR(v, 2, Fl_Widget, widget, FLTK_TAG(Fl_Widget));
	Fl::delete_widget(widget);
	return 0;
}

static void fltk_cb_hook(void* udata, bool freeAfter){
    HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(v);
    st_mycb *mycb = (st_mycb *)udata;
    sq_pushobject(v, mycb->callback);
    sq_pushroottable(v); //’this’ (function environment object)
    sq_pushobject(v, mycb->udata);
    sq_call(v, 2, SQFalse, SQTrue);

    if(freeAfter){
        //cleanup
        sq_release(v, &mycb->callback);
        sq_release(v, &mycb->udata);
        sq_free(mycb, sizeof(st_mycb));
    }

    //restore stack
    sq_settop(v, savedTop);
}

static SQInteger fltk_add_cb(HSQUIRRELVM v, int idx, st_mycb **cb)
{
    int ptype = sq_gettype(v, idx);
    if(ptype == OT_CLOSURE || ptype == OT_NATIVECLOSURE){
        st_mycb *mycb = (st_mycb *)sq_malloc(sizeof(st_mycb));
        memset(mycb, 0, sizeof(mycb));
        sq_getstackobj(v, idx, &mycb->callback);//fetches an handle of the function you passed
        sq_addref(v, &mycb->callback);
        if(sq_gettop(v) > idx) { //we have user data too
            sq_getstackobj(v, idx+1, &mycb->udata);
            sq_addref(v,&mycb->udata);
        }
        *cb = mycb;
    }
    return 0;
}

static void fltk_add_timeout_hook(void* udata){
    fltk_cb_hook(udata, true);
}

static void fltk_add_idle_hook(void* udata){
    fltk_cb_hook(udata, false);
}

static SQInteger _fl_add_timeout(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_FLOAT(v, 2, delay);
    st_mycb *mycb = 0;
    fltk_add_cb(v, 3, &mycb);
    if(mycb)  Fl::add_timeout(delay, &fltk_add_timeout_hook, mycb);
    return 0;
}

static SQInteger _fl_add_idle(HSQUIRRELVM v)
{
    st_mycb *mycb = 0;
    fltk_add_cb(v, 2, &mycb);
    if(mycb)  Fl::add_idle(&fltk_add_idle_hook, mycb);
    return 0;
}


static int flfk_focus_changing_handler_hook(Fl_Widget *to, Fl_Widget *from){
    HSQUIRRELVM v = (HSQUIRRELVM) Fl::user_data;
    SQInteger result;
    SQInteger savedTop = sq_gettop(v);
    sq_pushuserpointer(v, (void*)flfk_focus_changing_handler_hook);
    sq_getonregistrytable(v);
    if(sq_gettype(v, -1) == OT_CLOSURE){
        sq_pushroottable(v);
        if(getInstance_for_Fl_Widget(v, to) != SQ_OK) sq_pushnull(v);
        if(getInstance_for_Fl_Widget(v, from) != SQ_OK) sq_pushnull(v);
        if(sq_call(v, 3, SQTrue, SQTrue) == SQ_OK &&
            sq_getinteger(v, -1, &result) == SQ_OK){
            return result;
        };
    }
    //restore stack
    sq_settop(v, savedTop);
    return 0;
}

static SQInteger _fl_add_focus_changing_handler(HSQUIRRELVM v)
{
    sq_pushregistrytable(v);
    sq_pushuserpointer(v, (void*)flfk_focus_changing_handler_hook);
    sq_push(v, 2);
    sq_rawset(v, -3);
    Fl::add_focus_changing_handler(flfk_focus_changing_handler_hook);
    return 0;
}

static SQInteger _fl_set_fonts(HSQUIRRELVM v)
{
    const SQChar *filter = 0;
    if(sq_gettop(v) > 1) sq_getstring(v, 2, &filter);
    sq_pushinteger(v, Fl::set_fonts(filter));
    return 1;
}

static SQInteger _fl_set_font(HSQUIRRELVM v)
{
    const SQChar *fname = 0;
    SQInteger font, new_font;
    sq_getinteger(v, 2, &font);
    SQObjectType ptype = sq_gettype(v, 3);
    if(ptype == OT_INTEGER) {
        sq_getinteger(v, 3, &new_font);
        Fl::set_font((Fl_Font)font, (Fl_Font)new_font);
    }
    else if(ptype == OT_STRING) {
        sq_getstring(v, 3, &fname);
        Fl::set_font((Fl_Font)font, fname);
    }
    else return sq_throwerror(v, _SC("invalid parameter number 2"));
    return 0;
}

static SQInteger _fl_get_font(HSQUIRRELVM v)
{
    SQInteger font;
    sq_getinteger(v, 2, &font);
    sq_pushstring(v, Fl::get_font((Fl_Font)font), -1);
    return 1;
}

static SQInteger _fl_get_font_name(HSQUIRRELVM v)
{
    SQInteger font, attributes = 0;
    sq_getinteger(v, 2, &font);
    const char *fname = Fl::get_font_name((Fl_Font)font, &attributes);
    sq_newtable(v);

    sq_pushliteral(v, _SC("name"));
    sq_pushstring(v, fname, -1);
    sq_newslot(v, -3, SQFalse);

    sq_pushliteral(v, _SC("attributes"));
    sq_pushinteger(v, attributes);
    sq_newslot(v, -3, SQFalse);

    return 1;
}

static SQInteger _fl_fl_font(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INTEGER(v, 2, font);
    SQ_GET_INTEGER(v, 3, font_size);
    fl_font(font, font_size);
    return 0;
}

static SQInteger _fl_fl_height(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    if(_top_ > 1){
        SQ_GET_INTEGER(v, 2, font);
        SQ_GET_INTEGER(v, 2, font_size);
        sq_pushinteger(v, fl_height(font, font_size));
    }
    else sq_pushinteger(v, fl_height());
    return 1;
}

static SQInteger _fl_fl_width(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_STRING(v, 2, str);
    SQ_OPT_INTEGER(v, 3, size, str_size);
    sq_pushfloat(v, fl_width(str, size));
    return 1;
}

static SQInteger _fl_fl_descent(HSQUIRRELVM v)
{
    sq_pushinteger(v, fl_descent());
    return 1;
}

//void fl_cursor(Fl_Cursor, Fl_Color fg=FL_BLACK, Fl_Color bg=FL_WHITE);
static SQInteger _fl_fl_cursor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_INTEGER(v, 2, cursor);
    SQ_OPT_INTEGER(v, 3, fg, FL_BLACK);
    SQ_OPT_INTEGER(v, 4, bg, FL_WHITE);
    fl_cursor((Fl_Cursor)cursor, fg, bg);
    return 0;
}

static SQInteger _fl_fl_preferences(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_GET_INTEGER(v, 2, ptype);
    SQ_GET_STRING(v, 3, vendor);
    SQ_GET_STRING(v, 4, application);
    delete _appPreferences;
    _appPreferences = new Fl_Preferences( (Fl_Preferences::Root)ptype, vendor, application);
    return 0;
}

#define _DECL_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_fl_##name,nparams,pmask,isStatic}
static SQRegFunction fl_obj_funcs[]={
	_DECL_FUNC(check,1,_SC("y"),SQFalse),
	_DECL_FUNC(run,1,_SC("y"),SQFalse),
	_DECL_FUNC(event,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_alt,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_button,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_button1,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_button2,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_button3,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_buttons,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_command,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_ctrl,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_dx,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_dy,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_length,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_shift,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_x,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_x_root,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_y,1,_SC("y"),SQFalse),
	_DECL_FUNC(event_y_root,1,_SC("y"),SQFalse),
	_DECL_FUNC(x,1,_SC("y"),SQFalse),
	_DECL_FUNC(y,1,_SC("y"),SQFalse),
	_DECL_FUNC(h,1,_SC("y"),SQFalse),
	_DECL_FUNC(w,1,_SC("y"),SQFalse),
	_DECL_FUNC(ready,1,_SC("y"),SQFalse),
	_DECL_FUNC(screen_count,1,_SC("y"),SQFalse),

	_DECL_FUNC(set_fonts,-1,_SC("ys"),SQFalse),
	_DECL_FUNC(set_font,3,_SC("yi s|i"),SQFalse),
	_DECL_FUNC(get_font,2,_SC("yi"),SQFalse),
	_DECL_FUNC(get_font_name, 2,_SC("yi"),SQFalse),
	_DECL_FUNC(scheme,-1,_SC("ys"),SQFalse),
	_DECL_FUNC(visual,2,_SC("yi"),SQFalse),
	_DECL_FUNC(option,-2,_SC("yib"),SQFalse),

	_DECL_FUNC(event_key,-1,_SC("yi"),SQFalse),
	_DECL_FUNC(event_state,-1,_SC("yi"),SQFalse),
	_DECL_FUNC(event_is_click,-1,_SC("yi"),SQFalse),
	_DECL_FUNC(event_clicks,-1,_SC("yi"),SQFalse),
	_DECL_FUNC(scrollbar_size,-1,_SC("yi"),SQFalse),
	_DECL_FUNC(visible_focus,-1,_SC("yi"),SQFalse),

	_DECL_FUNC(do_widget_deletion,1,_SC("y"),SQFalse),
	_DECL_FUNC(delete_widget,2,_SC("yx"),SQFalse),
	_DECL_FUNC(add_timeout,-3,_SC("ync."),SQFalse),
	_DECL_FUNC(add_idle,-2,_SC("yc."),SQFalse),
	_DECL_FUNC(add_focus_changing_handler,2,_SC("yc"),SQFalse),

	//globals made static on Fl
	_DECL_FUNC(fl_cursor, -2,_SC("yiii"),SQFalse),
	_DECL_FUNC(fl_font,3,_SC("yii"),SQFalse),
	_DECL_FUNC(fl_width,-2,_SC("ysi"),SQFalse),
	_DECL_FUNC(fl_height,-1,_SC("yii"),SQFalse),
	_DECL_FUNC(fl_descent,1,_SC("y"),SQFalse),
	_DECL_FUNC(fl_preferences,4,_SC("yiss"),SQFalse),

	{0,0}
};
#undef _DECL_FUNC

#define INT_CONST(s)   { #s, s },

static const struct {
    const char* name;
    int value;
} fltk_constants[] = {
	INT_CONST(FLVE_ROW_CHANGED)
	INT_CONST(FLVE_ROWS_CHANGED)
	INT_CONST(FLVE_SELECTION_CHANGED)
	INT_CONST(FLVE_COL_CHANGED)
	INT_CONST(FLVE_COLS_CHANGED)
	INT_CONST(FLVE_FEATURE_CHANGED)
	INT_CONST(FLVE_CLICKED)
	INT_CONST(FLVE_ENTER_PRESSED)
	INT_CONST(FLVE_ROW_HEADER_CLICKED)
	INT_CONST(FLVE_ROW_FOOTER_CLICKED)
	INT_CONST(FLVE_COL_HEADER_CLICKED)
	INT_CONST(FLVE_COL_FOOTER_CLICKED)
	INT_CONST(FLVE_LABEL_CLICKED)
	INT_CONST(FLVE_TITLE_CLICKED)
	INT_CONST(FLVE_ALL_CLICKED)

//	Call back when flags
	INT_CONST(FLVEcb_ROW_CHANGED)
	INT_CONST(FLVEcb_ROWS_CHANGED)
	INT_CONST(FLVEcb_SELECTION_CHANGED)
	INT_CONST(FLVEcb_COL_CHANGED)
	INT_CONST(FLVEcb_COLS_CHANGED)
	INT_CONST(FLVEcb_FEATURE_CHANGED)
	INT_CONST(FLVEcb_CLICKED)
	INT_CONST(FLVEcb_ENTER_PRESSED)
	INT_CONST(FLVEcb_ROW_HEADER_CLICKED)
	INT_CONST(FLVEcb_ROW_FOOTER_CLICKED)
	INT_CONST(FLVEcb_COL_HEADER_CLICKED)
	INT_CONST(FLVEcb_COL_FOOTER_CLICKED)
	INT_CONST(FLVEcb_LABEL_CLICKED)
	INT_CONST(FLVEcb_TITLE_CLICKED)
	INT_CONST(FLVEcb_ALL_CLICKED)

//	Feature list
	INT_CONST(FLVF_NONE)
	INT_CONST(FLVF_ROW_HEADER)
	INT_CONST(FLVF_ROW_FOOTER)
	INT_CONST(FLVF_ROW_DIVIDER)
	INT_CONST(FLVF_COL_HEADER)
	INT_CONST(FLVF_COL_FOOTER)
	INT_CONST(FLVF_COL_DIVIDER)
	INT_CONST(FLVF_MULTI_SELECT)
	INT_CONST(FLVF_ROW_SELECT)
	INT_CONST(FLVF_PERSIST_SELECT)
	INT_CONST(FLVF_FULL_RESIZE)
	INT_CONST(FLVF_DIVIDERS)
	INT_CONST(FLVF_HEADERS)
	INT_CONST(FLVF_FOOTERS)
	INT_CONST(FLVF_ROW_ENDS)
	INT_CONST(FLVF_COL_ENDS)
	INT_CONST(FLVF_ALL_ROW)
	INT_CONST(FLVF_ALL_COL)

//	Scroll bar visibility
	INT_CONST(FLVS_NONE)
	INT_CONST(FLVS_HORIZONTAL)
	INT_CONST(FLVS_VERTICAL)
	INT_CONST(FLVS_BOTH)

	INT_CONST(FLVS_AUTOMATIC)
	INT_CONST(FLVS_ALWAYS)

	INT_CONST(FLVS_HORIZONTAL_ALWAYS)
	INT_CONST(FLVS_VERTICAL_ALWAYS)
//	INT_CONST(FLVS_BOTH_ALWAYS)
	INT_CONST(FLVS_BOTH_ALWAYS)

//	Edit when constants
	INT_CONST(FLV_EDIT_ALWAYS)
	INT_CONST(FLV_EDIT_AUTOMATIC)
	INT_CONST(FLV_EDIT_MANUAL)

	INT_CONST(FL_ALIGN_BOTTOM)
	INT_CONST(FL_ALIGN_CENTER)
	INT_CONST(FL_ALIGN_CLIP)
	INT_CONST(FL_ALIGN_INSIDE)
	INT_CONST(FL_ALIGN_LEFT)
	INT_CONST(FL_ALIGN_RIGHT)
	INT_CONST(FL_ALIGN_TOP)
	INT_CONST(FL_ALIGN_TOP_LEFT)
	INT_CONST(FL_ALIGN_TOP_RIGHT)
	INT_CONST(FL_ALIGN_WRAP)
	INT_CONST(FL_ALT)
	INT_CONST(FL_BACKGROUND2_COLOR)
	INT_CONST(FL_BACKGROUND_COLOR)
	INT_CONST(FL_BLACK)
	INT_CONST(FL_BLUE)
	INT_CONST(FL_BOLD)
	INT_CONST(FL_BORDER_BOX)
	INT_CONST(FL_BORDER_FRAME)
	INT_CONST(FL_COMMAND)
	INT_CONST(FL_COURIER)
	INT_CONST(FL_COURIER_BOLD)
	INT_CONST(FL_COURIER_BOLD_ITALIC)
	INT_CONST(FL_COURIER_ITALIC)
	INT_CONST(FL_CURSOR_DEFAULT)
	INT_CONST(FL_CURSOR_WAIT)
	INT_CONST(FL_CYAN)
	INT_CONST(FL_DARK1)
	INT_CONST(FL_DARK2)
	INT_CONST(FL_DARK3)
	INT_CONST(FL_DARK_BLUE)
	INT_CONST(FL_DARK_CYAN)
	INT_CONST(FL_DARK_GREEN)
	INT_CONST(FL_DARK_MAGENTA)
	INT_CONST(FL_DARK_RED)
	INT_CONST(FL_DARK_YELLOW)
	INT_CONST(FL_DOUBLE)
	INT_CONST(FL_DOWN_BOX)
	INT_CONST(FL_DOWN_FRAME)
	INT_CONST(FL_ENGRAVED_BOX)
	INT_CONST(FL_ENGRAVED_FRAME)
	INT_CONST(FL_FLAT_BOX)
	INT_CONST(FL_FOREGROUND_COLOR)
	INT_CONST(FL_GRAY)
	INT_CONST(FL_GREEN)
	INT_CONST(FL_HELVETICA)
	INT_CONST(FL_HELVETICA_BOLD)
	INT_CONST(FL_HELVETICA_BOLD_ITALIC)
	INT_CONST(FL_HELVETICA_ITALIC)
	INT_CONST(FL_INACTIVE_COLOR)
	INT_CONST(FL_INDEX)
	INT_CONST(FL_ITALIC)
	INT_CONST(FL_MAGENTA)
	INT_CONST(FL_MENU_DIVIDER)
	INT_CONST(FL_NORMAL_LABEL)
	INT_CONST(FL_NO_BOX)
	INT_CONST(FL_NO_LABEL)
	INT_CONST(FL_RADIO_BUTTON)
	INT_CONST(FL_RED)
	INT_CONST(FL_RGB)
	INT_CONST(FL_RGB8)
	INT_CONST(FL_SELECTION_COLOR)
	INT_CONST(FL_SHADOW_LABEL)
	INT_CONST(FL_SHIFT)
	INT_CONST(FL_SUBMENU)
	INT_CONST(FL_TIMES)
	INT_CONST(FL_TIMES_BOLD)
	INT_CONST(FL_TIMES_BOLD_ITALIC)
	INT_CONST(FL_TIMES_ITALIC)
	INT_CONST(FL_UP_BOX)
	INT_CONST(FL_WHEN_CHANGED)
	INT_CONST(FL_WHEN_ENTER_KEY)
	INT_CONST(FL_WHEN_ENTER_KEY_ALWAYS)
	INT_CONST(FL_WHEN_ENTER_KEY_CHANGED)
	INT_CONST(FL_WHEN_NEVER)
	INT_CONST(FL_WHEN_NOT_CHANGED)
	INT_CONST(FL_WHEN_RELEASE)
	INT_CONST(FL_WHEN_RELEASE_ALWAYS)
	INT_CONST(FL_WHITE)
	INT_CONST(FL_YELLOW)

    /*keyboard*/
    INT_CONST(FL_Key_Space)
    INT_CONST(FL_Key_Plus)
    INT_CONST(FL_Key_Minus)
    INT_CONST(FL_KP_Plus)
    INT_CONST(FL_KP_Minus)
    INT_CONST(FL_KP_Times)
    INT_CONST(FL_KP_Division)
    INT_CONST(FL_KP_Dot)
    INT_CONST(FL_Key_0)
    INT_CONST(FL_Key_9)
    INT_CONST(FL_KP_0)
    INT_CONST(FL_KP_1)
    INT_CONST(FL_KP_2)
    INT_CONST(FL_KP_3)
    INT_CONST(FL_KP_4)
    INT_CONST(FL_KP_5)
    INT_CONST(FL_KP_6)
    INT_CONST(FL_KP_7)
    INT_CONST(FL_KP_8)
    INT_CONST(FL_KP_9)

    INT_CONST(FL_Button)
    INT_CONST(FL_BackSpace)
    INT_CONST(FL_Tab)
    INT_CONST(FL_Iso_Key)
    INT_CONST(FL_Enter)
    INT_CONST(FL_Pause)
    INT_CONST(FL_Scroll_Lock)
    INT_CONST(FL_Escape)
    INT_CONST(FL_Kana)
    INT_CONST(FL_Eisu)
    INT_CONST(FL_Yen)
    INT_CONST(FL_JIS_Underscore)
    INT_CONST(FL_Home)
    INT_CONST(FL_Left)
    INT_CONST(FL_Up)
    INT_CONST(FL_Right)
    INT_CONST(FL_Down)
    INT_CONST(FL_Page_Up)
    INT_CONST(FL_Page_Down)
    INT_CONST(FL_End)
    INT_CONST(FL_Print)
    INT_CONST(FL_Insert)
    INT_CONST(FL_Menu)
    INT_CONST(FL_Help)
    INT_CONST(FL_Num_Lock)
    INT_CONST(FL_KP)
    INT_CONST(FL_KP_Enter)
    INT_CONST(FL_KP_Last)
    INT_CONST(FL_F)
    INT_CONST(FL_F_Last)
    INT_CONST(FL_Shift_L)
    INT_CONST(FL_Shift_R)
    INT_CONST(FL_Control_L)
    INT_CONST(FL_Control_R)
    INT_CONST(FL_Caps_Lock)
    INT_CONST(FL_Meta_L)
    INT_CONST(FL_Meta_R)
    INT_CONST(FL_Alt_L)
    INT_CONST(FL_Alt_R)
    INT_CONST(FL_Delete)

    INT_CONST(FL_LEFT_MOUSE)
    INT_CONST(FL_MIDDLE_MOUSE)
    INT_CONST(FL_RIGHT_MOUSE)
    INT_CONST(FL_SHIFT)
    INT_CONST(FL_CAPS_LOCK)
    INT_CONST(FL_CTRL)
    INT_CONST(FL_ALT)
    INT_CONST(FL_NUM_LOCK)
    INT_CONST(FL_META)
    INT_CONST(FL_SCROLL_LOCK)
    INT_CONST(FL_BUTTON1)
    INT_CONST(FL_BUTTON2)
    INT_CONST(FL_BUTTON3)
    INT_CONST(FL_BUTTONS)
    INT_CONST(FL_KEY_MASK)

    /*EVENTS*/
    INT_CONST(FL_NO_EVENT)
    INT_CONST(FL_PUSH)
    INT_CONST(FL_RELEASE)
    INT_CONST(FL_ENTER)
    INT_CONST(FL_LEAVE)
    INT_CONST(FL_DRAG)
    INT_CONST(FL_FOCUS)
    INT_CONST(FL_UNFOCUS)
    INT_CONST(FL_KEYDOWN)
    INT_CONST(FL_KEYBOARD)
    INT_CONST(FL_KEYUP)
    INT_CONST(FL_CLOSE)
    INT_CONST(FL_MOVE)
    INT_CONST(FL_SHORTCUT)
    INT_CONST(FL_DEACTIVATE)
    INT_CONST(FL_ACTIVATE)
    INT_CONST(FL_HIDE)
    INT_CONST(FL_SHOW)
    INT_CONST(FL_PASTE)
    INT_CONST(FL_SELECTIONCLEAR)
    INT_CONST(FL_MOUSEWHEEL)
    INT_CONST(FL_DND_ENTER)
    INT_CONST(FL_DND_DRAG)
    INT_CONST(FL_DND_LEAVE)
    INT_CONST(FL_DND_RELEASE)
    INT_CONST(FL_SCREEN_CONFIGURATION_CHANGED)
    INT_CONST(FL_FULLSCREEN)
    /* terminator */
    { NULL, 0 }
};

#undef INT_CONST

#define PUSH_FL_CLASS(klass, parent, method_funcs) \
    sq_pushnewclass(v, _SC(#klass), _SC(#parent), (void*)FLTK_TAG(klass), method_funcs, SQFalse)

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_fltklib(HSQUIRRELVM v)
{
    Fl::user_data = v;
	//sq_pushstring(v,_SC("fltk"),-1);
	//sq_newtable(v);

#define INT_CONST_PREFIX_VALUE(prefix, key, value) \
    sq_pushstring(v, _SC(#key), -1);\
    sq_pushinteger(v, prefix value);\
    sq_newslot(v, -3, SQTrue);

#define INT_CONST_PREFIX(prefix, key) INT_CONST_PREFIX_VALUE(prefix, key, key)
#define INT_CONST(key) INT_CONST_PREFIX(, key)

    //Fl class
    sq_pushnewclass(v, _SC("Fl"), NULL, (void*)FLTK_TAG(Fl), fl_obj_funcs, SQTrue);

    INT_CONST_PREFIX(Fl::, OPTION_ARROW_FOCUS);
    INT_CONST_PREFIX(Fl::, OPTION_VISIBLE_FOCUS);
    INT_CONST_PREFIX(Fl::, OPTION_DND_TEXT);
    INT_CONST_PREFIX(Fl::, OPTION_SHOW_TOOLTIPS);

    INT_CONST_PREFIX_VALUE(Fl_Preferences::,FL_PREFERENCES_USER, USER);
    INT_CONST_PREFIX_VALUE(Fl_Preferences::,FL_PREFERENCES_SYSTEM, SYSTEM);

	sq_poptop(v); //remove Fl class

	//Fl_Widget class
	sq_pushnewclass(v, FLTK_TAG(Fl_Widget), NULL, (void*)FLTK_TAG(Fl_Widget), fl_widget_obj_funcs, SQTrue);
	sq_pushuserpointer(v, (void*)FLTK_TAG(Fl_Widget)); //push a key to be used on registrytable
	sq_push(v, -2);
	sq_setonregistrytable(v); //save a reference on registry to easy access, this will pop the Fl_Widget class
	sq_poptop(v);

	sq_pushnewclass(v, FLTK_TAG(Flv_Style), NULL, (void*)FLTK_TAG(Flv_Style), flv_style_obj_funcs, SQFalse);
	sq_pushnewclass(v, FLTK_TAG(Flv_Style_List), NULL, (void*)FLTK_TAG(Flv_Style_List), flv_style_list_obj_funcs, SQFalse);

	//Fl_Box class
	PUSH_FL_CLASS(Fl_Box, Fl_Widget, fl_box_obj_funcs);

	//Fl_Button class
	PUSH_FL_CLASS(Fl_Button, Fl_Widget, fl_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Light_Button, Fl_Button, fl_light_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Check_Button, Fl_Light_Button, fl_check_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Radio_Button, Fl_Button, fl_radio_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Return_Button, Fl_Button, fl_return_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Round_Button, Fl_Light_Button, fl_round_button_obj_funcs);

	PUSH_FL_CLASS(Fl_Menu_, Fl_Widget, fl_menu__obj_funcs);
	PUSH_FL_CLASS(Fl_Menu_Bar, Fl_Menu_, fl_menu_bar_obj_funcs);
	PUSH_FL_CLASS(Fl_Choice, Fl_Menu_, fl_choice_obj_funcs);
	sq_pushnewclass(v, FLTK_TAG(Fl_Menu_Button), FLTK_TAG(Fl_Menu_), (void*)FLTK_TAG(Fl_Menu_Button),
                 fl_menu_button_obj_funcs, SQTrue);
#define POPUP_INT_CONST(key) INT_CONST_PREFIX(Fl_Menu_Button::, key)

	POPUP_INT_CONST(POPUP1);
	POPUP_INT_CONST(POPUP2);
	POPUP_INT_CONST(POPUP12);
	POPUP_INT_CONST(POPUP3);
	POPUP_INT_CONST(POPUP13);
	POPUP_INT_CONST(POPUP23);
	POPUP_INT_CONST(POPUP123);
	sq_poptop(v); //remove Fl_Menu_Button

	//Fl_Input_ class
	PUSH_FL_CLASS(Fl_Input_, Fl_Widget, fl_input__obj_funcs);
	PUSH_FL_CLASS(Fl_Input, Fl_Input_, fl_input_obj_funcs);
	PUSH_FL_CLASS(Fl_Float_Input, Fl_Input, fl_float_input_obj_funcs);
	PUSH_FL_CLASS(Fl_Int_Input, Fl_Input, fl_int_input_obj_funcs);
	PUSH_FL_CLASS(Fl_Output, Fl_Input, fl_output_obj_funcs);
	PUSH_FL_CLASS(Fl_Secret_Input, Fl_Input, fl_secret_input_obj_funcs);

	//Fl_Group class
	PUSH_FL_CLASS(Fl_Group, Fl_Widget, fl_group_obj_funcs);
	PUSH_FL_CLASS(Fl_Pack, Fl_Group, fl_pack_obj_funcs);
	PUSH_FL_CLASS(Fl_Tabs, Fl_Group, fl_tabs_obj_funcs);
	PUSH_FL_CLASS(Fl_Scroll, Fl_Group, fl_scroll_obj_funcs);
	PUSH_FL_CLASS(Fl_Text_Display, Fl_Group, fl_text_display_obj_funcs);
	PUSH_FL_CLASS(Fl_Text_Editor, Fl_Text_Display, fl_text_editor_obj_funcs);
	PUSH_FL_CLASS(Fl_Text_Editor_Buffered, Fl_Text_Editor, fl_text_editor_buffered_obj_funcs);

	PUSH_FL_CLASS(Flv_List, Fl_Group, flv_list_obj_funcs);
	PUSH_FL_CLASS(Flv_Table, Flv_List, flv_table_obj_funcs);
	PUSH_FL_CLASS(Flv_Data_Table, Flv_Table, flv_data_table_obj_funcs);

	PUSH_FL_CLASS(Flu_Combo_Box, Fl_Group, flu_combo_box_obj_funcs);
	PUSH_FL_CLASS(Flu_Combo_List, Flu_Combo_Box, flu_combo_list_obj_funcs);
	PUSH_FL_CLASS(Flu_Combo_Tree, Flu_Combo_Box, flu_combo_tree_obj_funcs);
	PUSH_FL_CLASS(Flu_Tree_Browser, Fl_Group, flu_combo_tree_browser_obj_funcs);

	//Fl_Window class
	PUSH_FL_CLASS(Fl_Window, Fl_Group, fl_window_obj_funcs);

	//Fl_Double_Window class
	PUSH_FL_CLASS(Fl_Double_Window, Fl_Window, fl_double_window_obj_funcs);

    sq_pushnewclass(v, Fl_Text_Buffer_TAG, NULL, (void*)Fl_Text_Buffer_TAG, fl_text_buffer_obj_funcs, SQFalse);

    sq_pushnewclass(v, Fl_Image_TAG, NULL, (void*)Fl_Image_TAG, fl_image_obj_funcs, SQFalse);
    sq_pushnewclass(v, Fl_RGB_Image_TAG, Fl_Image_TAG, (void*)Fl_RGB_Image_TAG, fl_rgb_image_obj_funcs, SQFalse);
    sq_pushnewclass(v, Fl_JPEG_Image_TAG, Fl_RGB_Image_TAG, (void*)Fl_JPEG_Image_TAG, fl_jpeg_image_obj_funcs, SQFalse);
    sq_pushnewclass(v, Fl_PNG_Image_TAG, Fl_RGB_Image_TAG, (void*)Fl_PNG_Image_TAG, fl_png_image_obj_funcs, SQFalse);

    sq_pushconsttable(v);
    INT_CONST_PREFIX_VALUE(, FL_SHADOW_LABEL, FL_SHADOW_LABEL);
    /* add constants to global table */
    int i=0;
    while (fltk_constants[i].name) {
        sq_pushstring(v, fltk_constants[i].name, -1);
        sq_pushinteger(v, fltk_constants[i].value);
        sq_rawset(v, -3);
        ++i;
    }
    sq_poptop(v);

    //check to see if we need to release resources
    Fl::do_at_widget_destroy_ = At_Widget_Destroy;

	return SQ_OK;
}
#ifdef __cplusplus
}
#endif

#endif //WITH_FLTK
