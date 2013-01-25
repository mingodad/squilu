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
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Bar.H>
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
CREATE_TAG(Fl_Menu_);
CREATE_TAG(Fl_Menu_Bar);
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

static const SQChar FLTK_key[] = _SC("FLTK");

static SQRESULT fltk_register_object_and_instance(HSQUIRRELVM v, int instance_idx, void *cptr){
    sq_pushstring(v, FLTK_key, -1);
    if(sq_getonregistrytable(v) < 0){
        sq_newtable(v);
        sq_pushstring(v, FLTK_key, -1);
        sq_push(v, -2);
        sq_setonregistrytable(v);
    }
    sq_pushuserpointer(v, cptr);
    //sq_weakref(sqvm, instance_idx);
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

#define SETUP_KLASS(v, Klass, Klass_TAG) \
	Klass *self = NULL; \
	{ if(SQ_FAILED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(void*)Klass_TAG))) \
		return SQ_ERROR; }

#define SETUP_FL_KLASS(v, Klass) SETUP_KLASS(v, Klass, FLTK_TAG(Klass))

#define SETUP_FL_WIDGET(v) SETUP_FL_KLASS(v, Fl_Widget)

#define NOTHING(a)
#define AS_IS(a) a

#define FLTK_CONSTRUCTOR_RELEASE_WINDOW(Klass, RELEASE_HOOK, _release_hook)\
static SQInteger _##Klass##_constructor(HSQUIRRELVM sqvm)\
{\
	const SQChar *label;\
	SQInteger argc, x,y,w,h;\
	Klass *cptr;\
	argc = sq_gettop(sqvm);\
	if(argc > 4){\
	    sq_getinteger(sqvm, 2, &x);\
	    sq_getinteger(sqvm, 3, &y);\
	    sq_getinteger(sqvm, 4, &w);\
	    sq_getinteger(sqvm, 5, &h);\
        if(argc == 6) sq_getstring(sqvm, 6, &label);\
        else label = NULL;\
	    cptr = new Klass(x,y,w,h,label);\
	} else if(argc > 2){\
	    sq_getinteger(sqvm, 2, &w);\
	    sq_getinteger(sqvm, 3, &h);\
        if(argc == 4) sq_getstring(sqvm, 4, &label);\
        else label = NULL;\
	    cptr = new Klass(w,h,label);\
	} else \
        return sq_throwerror(sqvm, _SC("Wrong number of parameters."));\
    fltk_register_object_and_instance(sqvm, 1, cptr);\
    sq_setinstanceup(sqvm, 1, cptr);\
    RELEASE_HOOK(sq_setreleasehook(sqvm,1,_release_hook));\
	return 1;\
}

#define FLTK_CONSTRUCTOR_RELEASE(Klass, RELEASE_HOOK, _release_hook)\
static SQInteger _##Klass##_constructor(HSQUIRRELVM sqvm)\
{\
	const SQChar *label;\
	SQInteger argc, x,y,w,h;\
	Klass *cptr;\
	argc = sq_gettop(sqvm);\
	if(argc > 4){\
	    sq_getinteger(sqvm, 2, &x);\
	    sq_getinteger(sqvm, 3, &y);\
	    sq_getinteger(sqvm, 4, &w);\
	    sq_getinteger(sqvm, 5, &h);\
        if(argc == 6) sq_getstring(sqvm, 6, &label);\
        else label = NULL;\
	    cptr = new Klass(x,y,w,h,label);\
	    fltk_register_object_and_instance(sqvm, 1, cptr);\
	} else \
        return sq_throwerror(sqvm, _SC("Wrong number of parameters."));\
    sq_setinstanceup(sqvm, 1, cptr);\
    RELEASE_HOOK(sq_setreleasehook(sqvm,1,_release_hook));\
	return 1;\
}

#define FLTK_CONSTRUCTOR(Klass) FLTK_CONSTRUCTOR_RELEASE(Klass, NOTHING, a)

#define FUNC_GETSET_STR(prefix, getSelf, selfPtr, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf;\
    SQInteger argc = sq_gettop(sqvm);\
    const SQChar *str = NULL;\
    if(argc > 1){\
        sq_getstring(sqvm, 2, &str);\
        selfPtr funcNAME(str);\
        return 0;\
    }\
    str = selfPtr funcNAME();\
    if(str) sq_pushstring(sqvm, str, -1);\
    else sq_pushnull(sqvm);\
	return 1;\
}

#define FUNC_GETSET_INT(prefix, getSelf, selfPtr, funcNAME, typeCast) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf;\
    SQInteger argc = sq_gettop(sqvm);\
    SQInteger iparm;\
    if(argc > 1){\
        sq_getinteger(sqvm, 2, &iparm);\
        selfPtr funcNAME((typeCast)iparm);\
        return 0;\
    }\
    sq_pushinteger(sqvm, selfPtr funcNAME());\
	return 1;\
}

#define FUNC_GETSET_INT_ONE_CALL(prefix, getSelf, selfPtr, funcNAME, typeCast) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf;\
    SQInteger argc = sq_gettop(sqvm);\
    SQInteger iparm;\
    if(argc > 1){\
        sq_getinteger(sqvm, 2, &iparm);\
        sq_pushinteger(sqvm, selfPtr funcNAME((typeCast)iparm));\
        return 1;\
    }\
    return sq_throwerror(sqvm, _SC("one integer parameter expected"));\
}

#define FUNC_GETSET_INT_BOOL(prefix, getSelf, selfPtr, funcNAME, typeCast) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf;\
    SQInteger argc = sq_gettop(sqvm);\
    SQInteger iparm;\
    SQBool bparm;\
    if(argc > 1){\
        sq_getinteger(sqvm, 2, &iparm);\
        if(argc > 2) {\
            sq_getbool(sqvm, 3, &bparm);\
            selfPtr funcNAME((typeCast)iparm, bparm == SQTrue);\
            return 0;\
        }\
        sq_pushbool(sqvm, selfPtr funcNAME((typeCast)iparm));\
        return 1;\
    }\
	return sq_throwerror(sqvm, _SC("invalid number of parameters"));\
}

#define FUNC_VOID_CALL(prefix, getSelf, selfPtr, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf(sqvm);\
    selfPtr funcNAME();\
	return 0;\
}

#define FUNC_VOID_CALL_2INT(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf(sqvm);\
    SQInteger i1, i2;\
    sq_getinteger(sqvm, 2, &i1);\
    sq_getinteger(sqvm, 3, &i2);\
    self->funcNAME(i1,i2);\
	return 0;\
}

#define FUNC_VOID_CALL_4INT(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf(sqvm);\
    SQInteger i1, i2, i3, i4;\
    sq_getinteger(sqvm, 2, &i1);\
    sq_getinteger(sqvm, 3, &i2);\
    sq_getinteger(sqvm, 4, &i3);\
    sq_getinteger(sqvm, 5, &i4);\
    self->funcNAME(i1,i2,i3,i4);\
	return 0;\
}

#define FUNC_INT_CALL(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf(sqvm);\
    sq_pushinteger(sqvm, self->funcNAME());\
	return 1;\
}

#define FUNC_STR_CALL(prefix, getSelf, funcNAME) \
static SQInteger prefix##funcNAME(HSQUIRRELVM sqvm) \
{\
    getSelf(sqvm);\
    sq_pushstring(sqvm, self->funcNAME(), -1);\
	return 1;\
}

#define FL_WIDGET_GETSET_STR(funcNAME) FUNC_GETSET_STR(_flwidget_, SETUP_FL_WIDGET(sqvm), self->, funcNAME)

#define FL_WIDGET_GETSET_INT0(funcNAME, typeNAME) FUNC_GETSET_INT(_flwidget_, SETUP_FL_WIDGET(sqvm), self->, funcNAME, typeNAME)
#define FL_WIDGET_GETSET_INT(funcNAME) FL_WIDGET_GETSET_INT0(funcNAME, int)
#define FL_WIDGET_GETSET_INT_CAST(funcNAME, castType) FL_WIDGET_GETSET_INT0(funcNAME, castType)

#define FL_WIDGET_VOID_CALL(funcNAME) FUNC_VOID_CALL(_flwidget_, SETUP_FL_WIDGET, self->, funcNAME)

#define FL_WIDGET_VOID_CALL_2INT(funcNAME) FUNC_VOID_CALL_2INT(_flwidget_, SETUP_FL_WIDGET, funcNAME)

#define FL_WIDGET_VOID_CALL_4INT(funcNAME) FUNC_VOID_CALL_4INT(_flwidget_, SETUP_FL_WIDGET, funcNAME)

#define FL_WIDGET_INT_CALL(funcNAME) FUNC_INT_CALL(_flwidget_, SETUP_FL_WIDGET, funcNAME)
#define FL_WIDGET_STR_CALL(funcNAME) FUNC_STR_CALL(_flwidget_, SETUP_FL_WIDGET, funcNAME)

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

GETINSTANCE_FOR_FL_KLASS(Fl_Widget);
GETINSTANCE_FOR_FL_KLASS(Fl_Group);

static SQRESULT sq_get_fl_virtual(HSQUIRRELVM sqvm, void *self, const SQChar *func_name){
    //sq_reservestack(sqvm, 20);
    if(fltk_get_registered_instance(sqvm, self) == SQ_OK){
        sq_pushstring(sqvm, func_name, -1);
        sq_get(sqvm, -2);
        SQObjectType toptype = sq_gettype(sqvm, -1);
        if(toptype == OT_CLOSURE || toptype == OT_NATIVECLOSURE){
            return SQ_OK;
        }
    }
    return SQ_ERROR;
}

static void sq_call_fl_virtual(void *self, const SQChar *func_name){
    HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
    //sq_reservestack(sqvm, 20);
    SQInteger top = sq_gettop(sqvm);
    if(sq_get_fl_virtual(sqvm, self, func_name) == SQ_OK){
        sq_push(sqvm, -2); //this
        sq_call(sqvm, 1, SQFalse, SQTrue);
    }
    sq_settop(sqvm, top);
}

typedef struct {
    HSQOBJECT callback;
    HSQOBJECT sender;
    HSQOBJECT udata;
} st_mycb;

//static st_mycb static_cb;

static void free_st_mycb(HSQUIRRELVM sqvm, st_mycb *cb){
    sq_release(sqvm, &cb->callback);
    sq_release(sqvm, &cb->udata);
    sq_free((void*)cb, sizeof(st_mycb));
}

static void At_Widget_Destroy(Fl_Widget *widget){
//printf("%d %p At_Widget_Destroy\n", __LINE__, widget);
    Fl_Callback_p cb = widget->callback();
    if(!cb || cb == Fl_Widget::default_callback) {
        //no need to check
        return;
    }
    HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(sqvm);
//printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(sqvm));
    int rc = fltk_deregister_instance(sqvm, widget);
//printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(sqvm));
//printf("%d %p %d %s At_Widget_Destroy\n", __LINE__, widget, rc, sq_getlasterror_str(sqvm));
    //if(sq_deleteslot(sqvm, -2, SQTrue) == SQ_OK){
    if(rc == SQ_OK){
//printf("%d %p At_Widget_Destroy\n", __LINE__, widget);
        if(sq_gettype(sqvm, -1) != OT_NULL){
            //////////////////////////////nota all will have user pointer !!!!!!!!
            SQUserPointer ptr;
            sq_getuserpointer(sqvm, -1, &ptr);
            //free_st_mycb(sqvm, (st_mycb*)ptr);
        }
    }
    sq_settop(sqvm, savedTop);
}

static void fltk_calback_hook(Fl_Widget *sender, void* udata){
//printf("Sender %p, data %p\n", sender, udata);
    HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(sqvm);
    st_mycb *mycb = (st_mycb *)udata;
    sq_pushobject(sqvm, mycb->callback);
    sq_pushroottable(sqvm); //’this’ (function environment object)
    if(fltk_get_registered_instance(sqvm, sender) != SQ_OK) sq_pushnull(sqvm);
    sq_pushobject(sqvm, mycb->udata);
    sq_call(sqvm, 3, SQFalse, SQTrue);
//printf("%d\n", sq_call(sqvm, 3, SQFalse, SQTrue));
    sq_settop(sqvm, savedTop);
}

static SQInteger _flwidget_callback(HSQUIRRELVM sqvm)
{
//printf("%d %s\n", __LINE__, __FILE__);
    SETUP_FL_WIDGET(sqvm);
    SQInteger argc = sq_gettop(sqvm);
    Fl_Callback_p cb = self->callback();
    if(cb == Fl_Widget::default_callback){
        cb = 0;
    }
    if(argc == 1){
        if(cb) sq_pushuserpointer(sqvm, (void*)cb);
        else sq_pushnull(sqvm);
        return 1;
    }
    if(argc >= 2){
        int ptypecb = sq_gettype(sqvm, 2);
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
        sq_getstackobj(sqvm, 2, &mycb->callback);//fetches an handle of the function you passed
        if(sq_gettop(sqvm) > 2) { //we have user data too
            sq_getstackobj(sqvm, 3, &mycb->udata);
            sq_addref(sqvm,&mycb->udata);
        }
        if(sq_type(mycb->callback) == OT_CLOSURE || sq_type(mycb->callback) == OT_NATIVECLOSURE){
            sq_addref(sqvm,&mycb->callback);
            //sq_getstackobj(sqvm, 1, &mycb->sender);
            self->callback(&fltk_calback_hook, mycb);
            //registry for free when widget is destroyed
            sq_pushregistrytable(sqvm);
            sq_pushuserpointer(sqvm, self);
            sq_pushuserpointer(sqvm, mycb);
            sq_rawset(sqvm, -2);
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
FL_WIDGET_INT_CALL(x);
FL_WIDGET_INT_CALL(y);
FL_WIDGET_INT_CALL(w);
FL_WIDGET_INT_CALL(h);
FL_WIDGET_VOID_CALL_2INT(position);
FL_WIDGET_VOID_CALL_2INT(size);
FL_WIDGET_VOID_CALL_4INT(resize);

//FL_WIDGET_STR_CALL(classId);

static SQInteger _flwidget_classId(HSQUIRRELVM sqvm)
{
    SETUP_FL_WIDGET(sqvm);
    if(!self) return sq_throwerror(sqvm, _SC("attempt to call method with null object"));
    const char *cn = self->classId();
    sq_pushstring(sqvm, cn, -1);
	return 1;
}

static SQInteger _flwidget_parent(HSQUIRRELVM sqvm)
{
    SETUP_FL_WIDGET(sqvm);
    if(!self) return sq_throwerror(sqvm, _SC("attempt to call method with null object"));
    if(getInstance_for_Fl_Widget(sqvm, self->parent()) != SQ_OK) sq_pushnull(sqvm);
	return 1;
}

static SQInteger _flwidget_handle(HSQUIRRELVM sqvm)
{
    SETUP_FL_WIDGET(sqvm);
    SQInteger event;
    sq_getinteger(sqvm, 2, &event);
    sq_pushinteger(sqvm, self->handle(event));
	return 1;
}

FL_WIDGET_VOID_CALL(redraw);
FL_WIDGET_VOID_CALL(redraw_label);
FL_WIDGET_VOID_CALL(hide);
FL_WIDGET_VOID_CALL(show);

#define _DECL_FL_WIDGET_FUNC(name,nparams,pmask) {_SC(#name),_flwidget_##name,nparams,pmask}
static SQRegFunction fl_widget_obj_funcs[]={
	_DECL_FL_WIDGET_FUNC(align,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(argument,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(box,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(callback,-1,_SC("xc.")),
	_DECL_FL_WIDGET_FUNC(color,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(selection_color,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(label,-1,_SC("xs")),
	_DECL_FL_WIDGET_FUNC(labelcolor,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(labelfont,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(labelsize,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(labeltype,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(textfont,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(textsize,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(tooltip,-1,_SC("xs")),
	_DECL_FL_WIDGET_FUNC(type,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(when,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(handle,2,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(redraw,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(redraw_label,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(x,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(y,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(w,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(h,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(position,3,_SC("xii")),
	_DECL_FL_WIDGET_FUNC(size,3,_SC("xii")),
	_DECL_FL_WIDGET_FUNC(resize,5,_SC("xiiii")),
	_DECL_FL_WIDGET_FUNC(show,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(hide,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(classId,1,_SC("x")),
	_DECL_FL_WIDGET_FUNC(parent,1,_SC("x")),
	{0,0}
};

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

static SQInteger _MyFl_Box_draw(HSQUIRRELVM sqvm)
{
    SETUP_FL_WIDGET(sqvm);
    ((MyFl_Box*)self)->Fl_Box::draw();
	return 0;
}

#define _DECL_FL_BOX_FUNC(name,nparams,pmask) {_SC(#name),_MyFl_Box_##name,nparams,pmask}
static SQRegFunction fl_box_obj_funcs[]={
	_DECL_FL_BOX_FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_BOX_FUNC(draw,1,_SC("x")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Button);
#define SETUP_FL_BUTTON(v) SETUP_FL_KLASS(v, Fl_Button)
#define FL_BUTTON_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_Fl_Button_, SETUP_FL_BUTTON(sqvm), self->, funcNAME, typeNAME)
FL_BUTTON_GETSET_INT_CAST(down_box, Fl_Boxtype);
FL_BUTTON_GETSET_INT_CAST(value, int);

#define _DECL_FL_BUTTON_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Button_##name,nparams,pmask}
static SQRegFunction fl_button_obj_funcs[]={
	_DECL_FL_BUTTON_FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_BUTTON_FUNC(down_box,-1,_SC("xi")),
	_DECL_FL_BUTTON_FUNC(value,-1,_SC("xi")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Light_Button);
#define _DECL_FL_LIGHT_BUTTON_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Light_Button_##name,nparams,pmask}
static SQRegFunction fl_light_button_obj_funcs[]={
	_DECL_FL_LIGHT_BUTTON_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Check_Button);
#define _DECL_FL_CHECK_BUTTON_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Check_Button_##name,nparams,pmask}
static SQRegFunction fl_check_button_obj_funcs[]={
	_DECL_FL_CHECK_BUTTON_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Radio_Button);
#define _DECL_FL_RADIO_BUTTON_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Radio_Button_##name,nparams,pmask}
static SQRegFunction fl_radio_button_obj_funcs[]={
	_DECL_FL_RADIO_BUTTON_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Return_Button);
#define _DECL_FL_RETURN_BUTTON_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Return_Button_##name,nparams,pmask}
static SQRegFunction fl_return_button_obj_funcs[]={
	_DECL_FL_RETURN_BUTTON_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

/*
FLTK_CONSTRUCTOR(Fl_Menu_Item);
#define _DECL_FL_MENU_ITEM_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Menu_Item_##name,nparams,pmask}
static SQRegFunction fl_menu_item_obj_funcs[]={
	_DECL_FL_MENU_ITEM_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};
*/

FLTK_CONSTRUCTOR(Fl_Menu_);
#define SETUP_FL_MENU_(v) SETUP_FL_KLASS(v, Fl_Menu_)

static SQInteger _Fl_Menu__copy(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
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

#define _DECL_FL_MENU__FUNC(name,nparams,pmask) {_SC(#name),_Fl_Menu__##name,nparams,pmask}
static SQRegFunction fl_menu__obj_funcs[]={
	_DECL_FL_MENU__FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_MENU__FUNC(copy,-2,_SC("xa.")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Menu_Bar);
#define _DECL_FL_MENU_BAR_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Menu_Bar_##name,nparams,pmask}
static SQRegFunction fl_menu_bar_obj_funcs[]={
	_DECL_FL_MENU_BAR_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Choice);
#define _DECL_FL_CHOICE_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Choice_##name,nparams,pmask}
static SQRegFunction fl_choice_obj_funcs[]={
	_DECL_FL_CHOICE_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};


FLTK_CONSTRUCTOR(Fl_Input_);
#define SETUP_FL_INPUT_(v) SETUP_FL_KLASS(v, Fl_Input_)
FUNC_GETSET_STR(_Fl_Input__, SETUP_FL_INPUT_(sqvm), self->, value);
#define _DECL_FL_INPUT__FUNC(name,nparams,pmask) {_SC(#name),_Fl_Input__##name,nparams,pmask}
static SQRegFunction fl_input__obj_funcs[]={
	_DECL_FL_INPUT__FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_INPUT__FUNC(value,-1,_SC("xs")),
	{0,0}
};

MY_FL_CLASS(Fl_Input,
    int handle(int event){
        HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
        SQInteger top = sq_gettop(sqvm);
        if(sq_get_fl_virtual(sqvm, this, "handle") == SQ_OK){
            sq_push(sqvm, -2); //this
            sq_pushinteger(sqvm, event);
            if(sq_call(sqvm, 2, SQTrue, SQTrue) == SQ_OK){
                if(sq_gettype(sqvm, -1) == OT_INTEGER){
                    sq_getinteger(sqvm, -1, &event);
                    sq_settop(sqvm, top);
                    return event;
                }
            }
            exit(-1);
        }
        sq_settop(sqvm, top);
        return Fl_Input::handle(event);
    }
)

static SQInteger _MyFl_Input_handle(HSQUIRRELVM sqvm)
{
    SQ_FUNC_VARS_NO_TOP(sqvm);
    SETUP_FL_WIDGET(sqvm);
    SQ_GET_INTEGER(sqvm, 2, event);
    sq_pushinteger(sqvm, ((MyFl_Input*)self)->Fl_Input::handle(event));
    return 1;
}

#define SETUP_FL_INPUT(v) SETUP_FL_KLASS(v, Fl_Input)
FUNC_GETSET_STR(_MyFl_Input_, , Fl_Input::, default_number_format);
FUNC_GETSET_STR(_MyFl_Input_, SETUP_FL_INPUT(sqvm), self->, number_format);
#define FL_INPUT_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_MyFl_Input_, SETUP_FL_INPUT(sqvm), self->, funcNAME, typeNAME)
FL_INPUT_GETSET_INT_CAST(use_numeric_format, int);
FL_INPUT_GETSET_INT_CAST(right_to_left, int);
FL_INPUT_GETSET_INT_CAST(decimal_places, int);

#define _DECL_FL_INPUT_FUNC(name,nparams,pmask,isStatic) {_SC(#name),_MyFl_Input_##name,nparams,pmask,isStatic}
static SQRegFunction fl_input_obj_funcs[]={
	_DECL_FL_INPUT_FUNC(constructor,-5,_SC("xiiiis"),SQFalse),
	_DECL_FL_INPUT_FUNC(handle,2,_SC("xi"),SQFalse),
	_DECL_FL_INPUT_FUNC(default_number_format,-1,_SC("ys"),SQTrue),
	_DECL_FL_INPUT_FUNC(number_format,-1,_SC("xs"),SQFalse),
	_DECL_FL_INPUT_FUNC(use_numeric_format,-1,_SC("xi"),SQFalse),
	_DECL_FL_INPUT_FUNC(right_to_left,-1,_SC("xi"),SQFalse),
	_DECL_FL_INPUT_FUNC(decimal_places,-1,_SC("xi"),SQFalse),
	{0,0}
};


FLTK_CONSTRUCTOR(Fl_Float_Input);
#define _DECL_FL_FLOAT_INPUT_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Float_Input_##name,nparams,pmask}
static SQRegFunction fl_float_input_obj_funcs[]={
	_DECL_FL_FLOAT_INPUT_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Int_Input);
#define _DECL_FL_INT_INPUT_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Int_Input_##name,nparams,pmask}
static SQRegFunction fl_int_input_obj_funcs[]={
	_DECL_FL_INT_INPUT_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Output);
#define _DECL_FL_INT_OUTPUT_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Output_##name,nparams,pmask}
static SQRegFunction fl_output_obj_funcs[]={
	_DECL_FL_INT_OUTPUT_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Secret_Input);
#define _DECL_FL_SECRET_INPUT_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Secret_Input_##name,nparams,pmask}
static SQRegFunction fl_secret_input_obj_funcs[]={
	_DECL_FL_SECRET_INPUT_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

#define SETUP_FL_GROUP(v) SETUP_FL_KLASS(v, Fl_Group)

FLTK_CONSTRUCTOR(Fl_Group);

enum _Fl_Group_Funcs {add, begin, current, end, resizable};
static SQRESULT _Fl_Group_dispatcher(HSQUIRRELVM sqvm, _Fl_Group_Funcs func_idx){
    SETUP_FL_GROUP(sqvm);
    switch(func_idx){
        case add:{
            SQUserPointer rsz;
            sq_getinstanceup(sqvm, -1, &rsz, 0);
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

static SQInteger _Fl_Group_begin(HSQUIRRELVM sqvm)
{
    SETUP_FL_GROUP(sqvm);
    self->begin();
	return 0;
}

static SQInteger _Fl_Group_end(HSQUIRRELVM sqvm)
{
    SETUP_FL_GROUP(sqvm);
    self->end();
	return 0;
}

static SQInteger _Fl_Group_current(HSQUIRRELVM sqvm)
{
    Fl_Group *grp = Fl_Group::current();
    if(!grp || getInstance_for_Fl_Group(sqvm, grp) != SQ_OK) sq_pushnull(sqvm);
	return 1;
}

static SQInteger _Fl_Group_resizable(HSQUIRRELVM sqvm)
{
    SETUP_FL_GROUP(sqvm);
    if(sq_gettop(sqvm)==1){
        sq_pushuserpointer(sqvm, self->resizable());
        return 1;
    }
    SQUserPointer rsz;
    sq_getinstanceup(sqvm, -1, &rsz, 0);
    self->resizable((Fl_Widget*)rsz);
	return 0;
}

static SQInteger _Fl_Group_add(HSQUIRRELVM sqvm)
{
    SETUP_FL_GROUP(sqvm);
    SQUserPointer rsz;
    sq_getinstanceup(sqvm, -1, &rsz, 0);
    self->add((Fl_Widget*)rsz);
	return 0;
}

#define _DECL_FL_GROUP_FUNC(name,nparams,pmask, isStatic) {_SC(#name),_Fl_Group_##name,nparams,pmask, isStatic}
static SQRegFunction fl_group_obj_funcs[]={
	_DECL_FL_GROUP_FUNC(constructor,-5,_SC("xiiiis"), SQFalse),
	_DECL_FL_GROUP_FUNC(begin,1,_SC("x"), SQFalse),
	_DECL_FL_GROUP_FUNC(end,1,_SC("x"), SQFalse),
	_DECL_FL_GROUP_FUNC(resizable,-1,_SC("xx"), SQFalse),
	_DECL_FL_GROUP_FUNC(add,2,_SC("xx"), SQFalse),
	_DECL_FL_GROUP_FUNC(current,1,_SC("y"), SQTrue),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Pack);
#define SETUP_FL_PACK(v) SETUP_FL_KLASS(v, Fl_Pack)

#define FL_PACK_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_Fl_Pack_, SETUP_FL_PACK(sqvm), self->, funcNAME, typeNAME)
FL_PACK_GETSET_INT_CAST(spacing, int);
FL_PACK_GETSET_INT_CAST(with_label, int);

#define _DECL_FL_PACK_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Pack_##name,nparams,pmask}
static SQRegFunction fl_pack_obj_funcs[]={
	_DECL_FL_PACK_FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_PACK_FUNC(spacing,-1,_SC("xi")),
	_DECL_FL_PACK_FUNC(with_label,-1,_SC("xi")),
	{0,0}
};

FLTK_CONSTRUCTOR(Flv_List);
#define SETUP_FLV_LIST(v) SETUP_FL_KLASS(v, Flv_List)

#define FLV_LIST_GETSET_INT_CAST(funcNAME, typeNAME) FUNC_GETSET_INT(_Flv_List_, SETUP_FLV_LIST(sqvm), self->, funcNAME, typeNAME)
#define FLV_LIST_GETSET_INT_ONE_CALL_CAST(funcNAME, typeNAME) FUNC_GETSET_INT_ONE_CALL(_Flv_List_, SETUP_FLV_LIST(sqvm), self->, funcNAME, typeNAME)
FLV_LIST_GETSET_INT_CAST(has_scrollbar, Flv_ShowScrollbar);
FLV_LIST_GETSET_INT_ONE_CALL_CAST(feature, int);
FLV_LIST_GETSET_INT_ONE_CALL_CAST(feature_add, int);
FLV_LIST_GETSET_INT_ONE_CALL_CAST(feature_remove, int);

#define _DECL_FLV_LIST_FUNC(name,nparams,pmask) {_SC(#name),_Flv_List_##name,nparams,pmask}
static SQRegFunction flv_list_obj_funcs[]={
	_DECL_FLV_LIST_FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FLV_LIST_FUNC(has_scrollbar,-1,_SC("xi")),
	_DECL_FLV_LIST_FUNC(feature,-1,_SC("xi")),
	_DECL_FLV_LIST_FUNC(feature_add,-1,_SC("xi")),
	_DECL_FLV_LIST_FUNC(feature_remove,-1,_SC("xi")),
	{0,0}
};

FLTK_CONSTRUCTOR(Flv_Table);
#define _DECL_FLV_TABLE_FUNC(name,nparams,pmask) {_SC(#name),_Flv_Table_##name,nparams,pmask}
static SQRegFunction flv_table_obj_funcs[]={
	_DECL_FLV_TABLE_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Scroll);
#define _DECL_FL_SCROLL_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Scroll_##name,nparams,pmask}
static SQRegFunction fl_scroll_obj_funcs[]={
	_DECL_FL_SCROLL_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Tabs);
#define _DECL_FL_TABS_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Tabs_##name,nparams,pmask}
static SQRegFunction fl_tabs_obj_funcs[]={
	_DECL_FL_TABS_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

static SQChar Fl_Text_Buffer_TAG[] = _SC("Fl_Text_Buffer");

static SQInteger _Fl_Text_Buffer_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_Text_Buffer *self = ((Fl_Text_Buffer *)p);
	delete self;
	return 1;
}

static SQInteger _Fl_Text_Buffer_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, requestedSize, 0);
    SQ_OPT_INTEGER(v, 3, preferredGapSize, 1024);
	Fl_Text_Buffer *cptr = new Fl_Text_Buffer(requestedSize, preferredGapSize);
    //do_register_object_and_instance(sqvm, 1, cptr);
    sq_setinstanceup(v, 1, cptr);
    sq_setreleasehook(v,1, _Fl_Text_Buffer_releasehook);
	return 1;
}

#define SETUP_FL_TEXT_BUFFER(v) SETUP_KLASS(v, Fl_Text_Buffer, Fl_Text_Buffer_TAG)
FUNC_GETSET_STR(_Fl_Text_Buffer_, SETUP_FL_TEXT_BUFFER(sqvm), self->, text);
FUNC_INT_CALL(_Fl_Text_Buffer_, SETUP_FL_TEXT_BUFFER(sqvm), length);

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

#define _DECL_FL_TEXT_BUFFER_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Text_Buffer_##name,nparams,pmask}
static SQRegFunction fl_text_buffer_obj_funcs[]={
	_DECL_FL_TEXT_BUFFER_FUNC(constructor,-1,_SC("xii")),
	_DECL_FL_TEXT_BUFFER_FUNC(text,-1,_SC("xs")),
	_DECL_FL_TEXT_BUFFER_FUNC(length,-1,_SC("xii")),
	_DECL_FL_TEXT_BUFFER_FUNC(loadfile,-2,_SC("xsi")),
	_DECL_FL_TEXT_BUFFER_FUNC(input_file_was_transcoded,1,_SC("x")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Display);
#define SETUP_FL_TEXT_DISPLAY(v) SETUP_FL_KLASS(v, Fl_Text_Display)

static SQInteger _Fl_Text_Display_buffer(HSQUIRRELVM sqvm)
{
    SETUP_FL_TEXT_DISPLAY(sqvm);
    SQUserPointer rsz;
    sq_getinstanceup(sqvm, -1, &rsz, 0);
    self->buffer((Fl_Text_Buffer*)rsz);
	return 0;
}

static SQInteger _Fl_Text_Display_highlight_data(HSQUIRRELVM sqvm)
{
    SETUP_FL_TEXT_DISPLAY(sqvm);
    SQUserPointer rsz;
	return 0;
}

#define _DECL_FL_TEXT_DISPLAY_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Text_Display_##name,nparams,pmask}
static SQRegFunction fl_text_display_obj_funcs[]={
	_DECL_FL_TEXT_DISPLAY_FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_TEXT_DISPLAY_FUNC(buffer,-1,_SC("xx")),
	_DECL_FL_TEXT_DISPLAY_FUNC(highlight_data, 7,_SC("xxaiic.")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Editor);
#define _DECL_FL_TEXT_EDITOR_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Text_Editor_##name,nparams,pmask}
static SQRegFunction fl_text_editor_obj_funcs[]={
	_DECL_FL_TEXT_EDITOR_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Editor_Buffered);
#define SETUP_FL_TEXT_EDITOR_BUFFERED(v) SETUP_FL_KLASS(v, Fl_Text_Editor_Buffered)

static SQInteger _Fl_Text_Editor_Buffered_value(HSQUIRRELVM sqvm)
{
    SQ_FUNC_VARS(sqvm);
    const SQChar *txt;
    SETUP_FL_TEXT_EDITOR_BUFFERED(sqvm);
    if(_top_ > 1){
        sq_getstring(sqvm, 2, &txt);
        self->buffer()->text(txt);
        return 0;
    }
    txt = self->buffer()->text();
    sq_pushstring(sqvm, txt, -1);
    return 1;
}

#define _DECL_FL_TEXT_EDITOR_BUFFERED_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Text_Editor_Buffered_##name,nparams,pmask}
static SQRegFunction fl_text_editor_buffered_obj_funcs[]={
	_DECL_FL_TEXT_EDITOR_BUFFERED_FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_TEXT_EDITOR_BUFFERED_FUNC(value,-1,_SC("xs")),
	{0,0}
};

class MyFl_Window : public Fl_Window {
    public:
    MyFl_Window(int X, int Y, int W, int H, const char *L=0):Fl_Window(X,Y,W,H,L){}
    MyFl_Window(int W, int H, const char *L=0):Fl_Window(W,H,L){}
    void hide(){sq_call_fl_virtual(this, "hide");}
};

static SQInteger _MyFl_Window_hide(HSQUIRRELVM sqvm)
{
    SETUP_FL_WIDGET(sqvm);
    ((MyFl_Window*)self)->Fl_Window::hide();
	return 0;
}

extern int sq_main_argc;
extern char **sq_main_argv;

static SQInteger _MyFl_Window_show_main(HSQUIRRELVM sqvm)
{
    SETUP_FL_WIDGET(sqvm);
    ((MyFl_Window*)self)->Fl_Window::show(sq_main_argc, sq_main_argv);
	return 0;
}


static SQInteger _fl_window_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	MyFl_Window *self = ((MyFl_Window *)p);
	Fl::delete_widget(self);
//printf("Releasing %p\n", self);
	return 1;
}

static SQInteger _MyFl_Window_set_non_modal(HSQUIRRELVM sqvm)
{
    SETUP_FL_WIDGET(sqvm);
    ((MyFl_Window*)self)->set_non_modal();
	return 0;
}

FLTK_CONSTRUCTOR_RELEASE_WINDOW(MyFl_Window, AS_IS, _fl_window_releasehook);

#define _DECL_FL_WINDOW_FUNC(name,nparams,pmask) {_SC(#name),_MyFl_Window_##name,nparams,pmask}
static SQRegFunction fl_window_obj_funcs[]={
	_DECL_FL_WINDOW_FUNC(constructor,-3,_SC("xii")),
	_DECL_FL_WINDOW_FUNC(show_main,1,_SC("x")),
	_DECL_FL_WINDOW_FUNC(hide,1,_SC("x")),
	_DECL_FL_WINDOW_FUNC(set_non_modal,1,_SC("x")),
	{0,0}
};

static SQInteger _fl_double_window_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_Double_Window *self = ((Fl_Double_Window *)p);
	Fl::delete_widget(self);
//printf("Releasing %p\n", self);
	return 1;
}

FLTK_CONSTRUCTOR_RELEASE_WINDOW(Fl_Double_Window, AS_IS, _fl_double_window_releasehook);

#define _DECL_FL_DOUBLE_WINDOW_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Double_Window_##name,nparams,pmask}
static SQRegFunction fl_double_window_obj_funcs[]={
	_DECL_FL_DOUBLE_WINDOW_FUNC(constructor,-3,_SC("xii")),
	{0,0}
};

static SQChar Fl_File_Chooser_TAG[] = _SC("Fl_File_Chooser");

static SQInteger _Fl_File_Chooser_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_File_Chooser *self = ((Fl_File_Chooser *)p);
	delete self;
	return 1;
}

static SQInteger _Fl_File_Chooser_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, pathname);
    SQ_GET_STRING(v, 3, pattern);
    SQ_GET_INTEGER(v, 4, type);
    SQ_GET_STRING(v, 5, title);

	Fl_File_Chooser *cptr = new Fl_File_Chooser(pathname, pattern, type, title);
    //do_register_object_and_instance(sqvm, 1, cptr);
    sq_setinstanceup(v, 1, cptr);
    sq_setreleasehook(v,1, _Fl_File_Chooser_releasehook);
	return 1;
}

#define _DECL_FL_FILE_CHOOSER_FUNC(name,nparams,pmask) {_SC(#name),_Fl_File_Chooser_##name,nparams,pmask}
static SQRegFunction fl_file_chooser_obj_funcs[]={
	_DECL_FL_FILE_CHOOSER_FUNC(constructor,5,_SC("xssis")),
	{0,0}
};

static SQChar Fl_Native_File_Chooser_TAG[] = _SC("Fl_Native_File_Chooser");

static SQInteger _Fl_Native_File_Chooser_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	Fl_Native_File_Chooser *self = ((Fl_Native_File_Chooser *)p);
	delete self;
	return 1;
}

static SQInteger _Fl_Native_File_Chooser_constructor(HSQUIRRELVM v)
{
    SQ_FUNC_VARS(v);
    SQ_OPT_INTEGER(v, 2, type, Fl_Native_File_Chooser::BROWSE_FILE);

	Fl_Native_File_Chooser *cptr = new Fl_Native_File_Chooser(type);
    //do_register_object_and_instance(sqvm, 1, cptr);
    sq_setinstanceup(v, 1, cptr);
    sq_setreleasehook(v,1, _Fl_Native_File_Chooser_releasehook);
	return 1;
}

#define _DECL_FL_NATIVE_FILE_CHOOSER_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Native_File_Chooser_##name,nparams,pmask}
static SQRegFunction fl_native_file_chooser_obj_funcs[]={
	_DECL_FL_NATIVE_FILE_CHOOSER_FUNC(constructor,-1,_SC("xi")),
/*
	_DECL_FL_NATIVE_FILE_CHOOSER_FUNC(title,-1,_SC("xs")),
	_DECL_FL_NATIVE_FILE_CHOOSER_FUNC(type,-1,_SC("xi")),
	_DECL_FL_NATIVE_FILE_CHOOSER_FUNC(show,1,_SC("x")),
	_DECL_FL_NATIVE_FILE_CHOOSER_FUNC(filename,1,_SC("x")),
	_DECL_FL_NATIVE_FILE_CHOOSER_FUNC(directory,1,_SC("x")),
*/
	{0,0}
};


#define FL_FUNC_NO_ARG_NO_RET(fn) static SQInteger _fl_##fn(HSQUIRRELVM sqvm){Fl::fn();return 0;}
FL_FUNC_NO_ARG_NO_RET(check);
FL_FUNC_NO_ARG_NO_RET(do_widget_deletion);

#define FL_FUNC_NO_ARG_RET_INT(fn) static SQInteger _fl_##fn(HSQUIRRELVM sqvm){sq_pushinteger(sqvm, Fl::fn());return 1;}
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

#define FL_FUNC_GETSET_INT(fn) FUNC_GETSET_INT(_fl_, , Fl::, fn, int)

FL_FUNC_GETSET_INT(event_key);
FL_FUNC_GETSET_INT(event_state);
FL_FUNC_GETSET_INT(event_is_click);
FL_FUNC_GETSET_INT(event_clicks);
FL_FUNC_GETSET_INT(scrollbar_size);
FL_FUNC_GETSET_INT(visible_focus);

#define FL_FUNC_GETSET_INT_ONE_CALL(fn) FUNC_GETSET_INT_ONE_CALL(_fl_, , Fl::, fn, int)
FL_FUNC_GETSET_INT_ONE_CALL(visual);

#define FL_FUNC_GETSET_INT_BOOL(fn) FUNC_GETSET_INT_BOOL(_fl_, , Fl::, fn, Fl::Fl_Option)
FL_FUNC_GETSET_INT_BOOL(option);

#define FL_FUNC_GETSET_STR(fn) FUNC_GETSET_STR(_fl_, , Fl::, fn)
FL_FUNC_GETSET_STR(scheme);

static SQInteger _fl_delete_widget(HSQUIRRELVM v)
{
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_INSTANCE_VAR(v, 2, Fl_Widget, widget, FLTK_TAG(Fl_Widget));
	Fl::delete_widget(widget);
	return 0;
}

static void fltk_cb_hook(void* udata, bool freeAfter){
    HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(sqvm);
    st_mycb *mycb = (st_mycb *)udata;
    sq_pushobject(sqvm, mycb->callback);
    sq_pushroottable(sqvm); //’this’ (function environment object)
    sq_pushobject(sqvm, mycb->udata);
    sq_call(sqvm, 2, SQFalse, SQTrue);

    if(freeAfter){
        //cleanup
        sq_release(sqvm, &mycb->callback);
        sq_release(sqvm, &mycb->udata);
        sq_free(mycb, sizeof(st_mycb));
    }

    //restore stack
    sq_settop(sqvm, savedTop);
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
    st_mycb *mycb;
    fltk_add_cb(v, 3, &mycb);
    if(mycb)  Fl::add_timeout(delay, &fltk_add_timeout_hook, mycb);
    return 0;
}

static SQInteger _fl_add_idle(HSQUIRRELVM v)
{
    st_mycb *mycb;
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

#define _DECL_FL_FUNC(name,nparams,pmask) {_SC(#name),_fl_##name,nparams,pmask}
static SQRegFunction fl_obj_funcs[]={
	_DECL_FL_FUNC(check,1,_SC("y")),
	_DECL_FL_FUNC(run,1,_SC("y")),
	_DECL_FL_FUNC(event,1,_SC("y")),
	_DECL_FL_FUNC(event_alt,1,_SC("y")),
	_DECL_FL_FUNC(event_button,1,_SC("y")),
	_DECL_FL_FUNC(event_button1,1,_SC("y")),
	_DECL_FL_FUNC(event_button2,1,_SC("y")),
	_DECL_FL_FUNC(event_button3,1,_SC("y")),
	_DECL_FL_FUNC(event_buttons,1,_SC("y")),
	_DECL_FL_FUNC(event_command,1,_SC("y")),
	_DECL_FL_FUNC(event_ctrl,1,_SC("y")),
	_DECL_FL_FUNC(event_dx,1,_SC("y")),
	_DECL_FL_FUNC(event_dy,1,_SC("y")),
	_DECL_FL_FUNC(event_length,1,_SC("y")),
	_DECL_FL_FUNC(event_shift,1,_SC("y")),
	_DECL_FL_FUNC(event_x,1,_SC("y")),
	_DECL_FL_FUNC(event_x_root,1,_SC("y")),
	_DECL_FL_FUNC(event_y,1,_SC("y")),
	_DECL_FL_FUNC(event_y_root,1,_SC("y")),
	_DECL_FL_FUNC(x,1,_SC("y")),
	_DECL_FL_FUNC(y,1,_SC("y")),
	_DECL_FL_FUNC(h,1,_SC("y")),
	_DECL_FL_FUNC(w,1,_SC("y")),
	_DECL_FL_FUNC(ready,1,_SC("y")),
	_DECL_FL_FUNC(screen_count,1,_SC("y")),

	_DECL_FL_FUNC(set_fonts,-1,_SC("ys")),
	_DECL_FL_FUNC(set_font,3,_SC("yi s|i")),
	_DECL_FL_FUNC(get_font,2,_SC("yi")),
	_DECL_FL_FUNC(get_font_name, 2,_SC("yi")),
	_DECL_FL_FUNC(scheme,-1,_SC("ys")),
	_DECL_FL_FUNC(visual,2,_SC("yi")),
	_DECL_FL_FUNC(option,-2,_SC("yib")),

	_DECL_FL_FUNC(event_key,-1,_SC("yi")),
	_DECL_FL_FUNC(event_state,-1,_SC("yi")),
	_DECL_FL_FUNC(event_is_click,-1,_SC("yi")),
	_DECL_FL_FUNC(event_clicks,-1,_SC("yi")),
	_DECL_FL_FUNC(scrollbar_size,-1,_SC("yi")),
	_DECL_FL_FUNC(visible_focus,-1,_SC("yi")),

	_DECL_FL_FUNC(do_widget_deletion,1,_SC("y")),
	_DECL_FL_FUNC(delete_widget,2,_SC("yx")),
	_DECL_FL_FUNC(add_timeout,-3,_SC("ync.")),
	_DECL_FL_FUNC(add_idle,-2,_SC("yc.")),
	_DECL_FL_FUNC(add_focus_changing_handler,2,_SC("yc")),
	{0,0}
};
#undef _DECL_FL_FUNC

#define INT_CONST(s)   { #s, s },

static const struct {
    const char* name;
    int value;
} fltk_constants[] = {
	INT_CONST(FLVF_PERSIST_SELECT)
	INT_CONST(FLVF_ROW_SELECT)
	INT_CONST(FLVS_NONE)
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
    /* terminator */
    { NULL, 0 }
};

#undef INT_CONST

#define PUSH_FL_CLASS(klass, parent, method_funcs) \
    sq_pushnewclass(sqvm, _SC(#klass), _SC(#parent), (void*)FLTK_TAG(klass), method_funcs, SQFalse)

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_fltklib(HSQUIRRELVM sqvm)
{
    Fl::user_data = sqvm;
	//sq_pushstring(sqvm,_SC("fltk"),-1);
	//sq_newtable(sqvm);

#define INT_CONST_PREFIX_VALUE(prefix, key, value) \
    sq_pushstring(sqvm, _SC(#key), -1);\
    sq_pushinteger(sqvm, prefix value);\
    sq_newslot(sqvm, -3, SQTrue);

#define INT_CONST_PREFIX(prefix, key) INT_CONST_PREFIX_VALUE(prefix, key, key)
#define INT_CONST(key) INT_CONST_PREFIX(, key)

    //Fl class
    sq_pushnewclass(sqvm, _SC("Fl"), NULL, (void*)FLTK_TAG(Fl), fl_obj_funcs, SQTrue);

    INT_CONST_PREFIX(Fl::, OPTION_ARROW_FOCUS);
    INT_CONST_PREFIX(Fl::, OPTION_VISIBLE_FOCUS);
    INT_CONST_PREFIX(Fl::, OPTION_DND_TEXT);
    INT_CONST_PREFIX(Fl::, OPTION_SHOW_TOOLTIPS);

	sq_poptop(sqvm); //remove Fl class

	//Fl_Widget class
	sq_pushnewclass(sqvm, _SC("Fl_Widget"), NULL, (void*)FLTK_TAG(Fl_Widget), fl_widget_obj_funcs, SQTrue);
	sq_pushuserpointer(sqvm, (void*)FLTK_TAG(Fl_Widget)); //push a key to be used on registrytable
	sq_push(sqvm, -2);
	sq_setonregistrytable(sqvm); //save a reference on registry to easy access, this will pop the Fl_Widget class
	sq_poptop(sqvm);

	//Fl_Box class
	PUSH_FL_CLASS(Fl_Box, Fl_Widget, fl_box_obj_funcs);

	//Fl_Button class
	PUSH_FL_CLASS(Fl_Button, Fl_Widget, fl_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Light_Button, Fl_Button, fl_light_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Check_Button, Fl_Light_Button, fl_check_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Radio_Button, Fl_Button, fl_radio_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Return_Button, Fl_Button, fl_radio_button_obj_funcs);

	PUSH_FL_CLASS(Fl_Menu_, Fl_Widget, fl_menu__obj_funcs);
	PUSH_FL_CLASS(Fl_Menu_Bar, Fl_Menu_, fl_menu_bar_obj_funcs);
	PUSH_FL_CLASS(Fl_Choice, Fl_Menu_, fl_choice_obj_funcs);

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

	//Fl_Window class
	PUSH_FL_CLASS(Fl_Window, Fl_Group, fl_window_obj_funcs);

	//Fl_Double_Window class
	PUSH_FL_CLASS(Fl_Double_Window, Fl_Window, fl_double_window_obj_funcs);

    sq_pushnewclass(sqvm, Fl_Text_Buffer_TAG, NULL, (void*)Fl_Text_Buffer_TAG, fl_text_buffer_obj_funcs, SQFalse);

    sq_pushconsttable(sqvm);
    INT_CONST_PREFIX_VALUE(, FL_SHADOW_LABEL, FL_SHADOW_LABEL);
    /* add constants to global table */
    int i=0;
    while (fltk_constants[i].name) {
        sq_pushstring(sqvm, fltk_constants[i].name, -1);
        sq_pushinteger(sqvm, fltk_constants[i].value);
        sq_rawset(sqvm, -3);
        ++i;
    }
    sq_poptop(sqvm);

    //check to see if we need to release resources
    Fl::do_at_widget_destroy_ = At_Widget_Destroy;

	return SQ_OK;
}
#ifdef __cplusplus
}
#endif

#endif //WITH_FLTK
