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
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input_.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Tabs.H>
#include <FL/Flv_List.H>
#include <FL/Flv_Table.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_JPEG_Image.H>

#include <squirrel.h>

#include "sqfltk.h"

struct MyFltkUData {
    HSQOBJECT self;
    HSQOBJECT udata;
    HSQUIRRELVM v;
};

#define FLTK_TAG(tag) SQFLTK_##tag##_TYPE_TAG

#define CREATE_TAG(tag) \
static const char* FLTK_TAG(tag) = #tag


CREATE_TAG(Fl);
CREATE_TAG(Fl_Widget);
CREATE_TAG(Fl_Box);
CREATE_TAG(Fl_Button);
CREATE_TAG(Fl_Light_Button);
CREATE_TAG(Fl_Check_Button);
CREATE_TAG(Fl_Radio_Button);
CREATE_TAG(Fl_Menu_);
CREATE_TAG(Fl_Menu_Item);
CREATE_TAG(Fl_Choice);
CREATE_TAG(Fl_Input_);
CREATE_TAG(Fl_Input);
CREATE_TAG(Fl_Float_Input);
CREATE_TAG(Fl_Int_Input);
CREATE_TAG(Fl_Output);
CREATE_TAG(Fl_Secret_Input);
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

static void do_register_object_and_instance(HSQUIRRELVM sqvm, int instance_idx, void *cptr){
    sq_pushregistrytable(sqvm);
    sq_pushuserpointer(sqvm, cptr);
    HSQOBJECT obj; sq_resetobject(&obj);
    sq_getstackobj(sqvm, instance_idx, &obj);
    sq_pushobject(sqvm, obj);
    sq_rawset(sqvm, -3);
    sq_pop(sqvm, -1);
}

#define SETUP_FL_KLASS(v, Klass) \
	Klass *self = NULL; \
	{ if(SQ_FAILED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(void*)FLTK_TAG(Klass)))) \
		return SQ_ERROR; }

#define SETUP_FL_WIDGET(v) SETUP_FL_KLASS(v, Fl_Widget)

#define NOTHING(a)
#define AS_IS(a) a

#define FLTK_CONSTRUCTOR_RELEASE(Klass, RELEASE_HOOK, _release_hook)\
static SQInteger _##Klass##_constructor(HSQUIRRELVM sqvm)\
{\
	const SQChar *label;\
	SQInteger argc, x,y,w,h;\
	Klass *cptr;\
	argc = sq_gettop(sqvm);\
	if(argc > 5){\
	    sq_getinteger(sqvm, 2, &x);\
	    sq_getinteger(sqvm, 3, &y);\
	    sq_getinteger(sqvm, 4, &w);\
	    sq_getinteger(sqvm, 5, &h);\
        if(argc == 6) sq_getstring(sqvm, 6, &label);\
        else label = NULL;\
	    cptr = new Klass(x,y,w,h,label);\
	    do_register_object_and_instance(sqvm, 1, cptr);\
	} else\
        return sq_throwerror(sqvm, _SC("Wrong number of parameters."));\
\
    sq_setinstanceup(sqvm, 1, cptr);\
    RELEASE_HOOK(sq_setreleasehook(sqvm,1,_release_hook));\
	return 1;\
}

#define FLTK_CONSTRUCTOR(Klass) FLTK_CONSTRUCTOR_RELEASE(Klass, NOTHING, a)

#define FL_WIDGET_GETSET_STR(funcNAME) \
static SQInteger _flwidget_##funcNAME(HSQUIRRELVM sqvm) \
{\
    SETUP_FL_WIDGET(sqvm);\
    SQInteger argc = sq_gettop(sqvm);\
    const SQChar *funcNAME = NULL;\
    if(argc > 0){\
        sq_getstring(sqvm, 2, &funcNAME);\
        self->funcNAME(funcNAME);\
        return 0;\
    }\
    funcNAME = self->funcNAME();\
    if(funcNAME) sq_pushstring(sqvm, funcNAME, -1);\
    else sq_pushnull(sqvm);\
	return 1;\
}

#define FL_WIDGET_GETSET_INT0(funcNAME, typeNAME) \
static SQInteger _flwidget_##funcNAME(HSQUIRRELVM sqvm) \
{\
    SETUP_FL_WIDGET(sqvm);\
    SQInteger argc = sq_gettop(sqvm);\
    SQInteger funcNAME;\
    if(argc > 0){\
        sq_getinteger(sqvm, 2, &funcNAME);\
        self->funcNAME((typeNAME)funcNAME);\
        return 0;\
    }\
    sq_pushinteger(sqvm, self->funcNAME());\
	return 1;\
}

#define FL_WIDGET_GETSET_INT(funcNAME) FL_WIDGET_GETSET_INT0(funcNAME, int)

#define FL_WIDGET_VOID_CALL(funcNAME) \
static SQInteger _flwidget_##funcNAME(HSQUIRRELVM sqvm) \
{\
    SETUP_FL_WIDGET(sqvm);\
    self->funcNAME();\
	return 0;\
}

#define FL_WIDGET_VOID_CALL_2INT(funcNAME) \
static SQInteger _flwidget_##funcNAME(HSQUIRRELVM sqvm) \
{\
    SETUP_FL_WIDGET(sqvm);\
    SQInteger i1, i2;\
    sq_getinteger(sqvm, 2, &i1);\
    sq_getinteger(sqvm, 3, &i2);\
    self->funcNAME(i1,i2);\
	return 0;\
}

#define FL_WIDGET_VOID_CALL_4INT(funcNAME) \
static SQInteger _flwidget_##funcNAME(HSQUIRRELVM sqvm) \
{\
    SETUP_FL_WIDGET(sqvm);\
    SQInteger i1, i2, i3, i4;\
    sq_getinteger(sqvm, 2, &i1);\
    sq_getinteger(sqvm, 3, &i2);\
    sq_getinteger(sqvm, 4, &i3);\
    sq_getinteger(sqvm, 5, &i4);\
    self->funcNAME(i1,i2,i3,i4);\
	return 0;\
}

#define FL_WIDGET_INT_CALL(funcNAME) \
static SQInteger _flwidget_##funcNAME(HSQUIRRELVM sqvm) \
{\
    SETUP_FL_WIDGET(sqvm);\
    sq_pushinteger(sqvm, self->funcNAME());\
	return 1;\
}

static void sq_call_fl_virtual(void *self, const SQChar *func_name){
    HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
    //sq_reservestack(sqvm, 20);
    SQInteger top = sq_gettop(sqvm);
    sq_pushregistrytable(sqvm);
    sq_pushuserpointer(sqvm, self);
    if(sq_rawget(sqvm, -2) == SQ_OK){
        sq_pushstring(sqvm, func_name, -1);
        sq_get(sqvm, -2);
        SQObjectType toptype = sq_gettype(sqvm, -1);
        if(toptype == OT_CLOSURE || toptype == OT_NATIVECLOSURE){
            sq_push(sqvm, -2); //this
            sq_call(sqvm, 1, SQFalse, SQTrue);
        }
    }
    sq_settop(sqvm, top);
}


typedef struct {
    HSQOBJECT callback;
    HSQOBJECT callee;
    HSQOBJECT udata;
} st_mycb;

//static st_mycb static_cb;

static void free_st_mycb(HSQUIRRELVM sqvm, st_mycb *cb){
    sq_release(sqvm, &cb->callback);
    sq_release(sqvm, &cb->udata);
    sq_free((void*)cb, sizeof(st_mycb));
}

static void At_Widget_Destroy(Fl_Widget *widget){
printf("%d %p At_Widget_Destroy\n", __LINE__, widget);
    Fl_Callback_p cb = widget->callback();
    if(!cb || cb == Fl_Widget::default_callback) {
        //no need to check
        return;
    }
    HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(sqvm);
printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(sqvm));
    sq_pushregistrytable(sqvm);
printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(sqvm));
    sq_pushuserpointer(sqvm, widget);
printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(sqvm));
    int rc = sq_deleteslot(sqvm, -2, SQTrue);
printf("%d %p %d At_Widget_Destroy\n", __LINE__, widget, sq_gettop(sqvm));
printf("%d %p %d %s At_Widget_Destroy\n", __LINE__, widget, rc, sq_getlasterror_str(sqvm));
    //if(sq_deleteslot(sqvm, -2, SQTrue) == SQ_OK){
    if(rc == SQ_OK){
printf("%d %p At_Widget_Destroy\n", __LINE__, widget);
        if(sq_gettype(sqvm, -1) != OT_NULL){
            //////////////////////////////nota all will have user pointer !!!!!!!!
            SQUserPointer ptr;
            sq_getuserpointer(sqvm, -1, &ptr);
            free_st_mycb(sqvm, (st_mycb*)ptr);
        }
    }
    sq_settop(sqvm, savedTop);
}

static void fltk_calback_hook(Fl_Widget *sender, void* udata){
    printf("Sender %p, data %p\n", sender, udata);
    HSQUIRRELVM sqvm = (HSQUIRRELVM) Fl::user_data;
    SQInteger savedTop = sq_gettop(sqvm);
    st_mycb *mycb = (st_mycb *)udata;
    sq_pushobject(sqvm, mycb->callback);
    sq_pushroottable(sqvm); //’this’ (function environment object)
    sq_pushobject(sqvm, mycb->callee);
    sq_pushobject(sqvm, mycb->udata);
    printf("%d\n", sq_call(sqvm, 3, SQFalse, SQTrue));
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
        sq_resetobject(&mycb->callee);
        sq_resetobject(&mycb->udata);
        sq_getstackobj(sqvm, 2, &mycb->callback);//fetches an handle of the function you passed
        if(sq_gettop(sqvm) > 2) { //we have user data too
            sq_getstackobj(sqvm, 3, &mycb->udata);
            sq_addref(sqvm,&mycb->udata);
        }
        if(sq_type(mycb->callback) == OT_CLOSURE || sq_type(mycb->callback) == OT_NATIVECLOSURE){
            sq_addref(sqvm,&mycb->callback);
            sq_getcallee(sqvm);
            sq_getstackobj(sqvm, -1, &mycb->callee);
            self->callback(&fltk_calback_hook, mycb);
            //registry for free when widget is destroyed
            sq_pushregistrytable(sqvm);
            sq_pushuserpointer(sqvm, self);
            sq_pushuserpointer(sqvm, mycb);
            sq_rawset(sqvm, -2);
            printf("Button %p\n", self);
        }
    }
	return 0;
}

FL_WIDGET_GETSET_INT(align);
FL_WIDGET_GETSET_INT(argument);
//FL_WIDGET_GETSET_INT(box);
FL_WIDGET_GETSET_INT(color);
FL_WIDGET_GETSET_INT(selection_color);
FL_WIDGET_GETSET_STR(label);
FL_WIDGET_GETSET_INT(labelcolor);
FL_WIDGET_GETSET_INT(labelfont);
FL_WIDGET_GETSET_INT(labelsize);
//FL_WIDGET_GETSET_INT(labeltype);
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
	_DECL_FL_WIDGET_FUNC(callback,-1,_SC("xc.")),
	_DECL_FL_WIDGET_FUNC(color,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(selection_color,1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(label,-1,_SC("xs")),
	_DECL_FL_WIDGET_FUNC(labelcolor,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(labelfont,-1,_SC("xi")),
	_DECL_FL_WIDGET_FUNC(labelsize,-1,_SC("xi")),
	//_DECL_FL_WIDGET_FUNC(labeltype,1,_SC("xi")),
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
	{0,0}
};

class MyFl_Box : public Fl_Box {
    public:
    MyFl_Box(int X, int Y, int W, int H, const char *L=0):Fl_Box(X,Y,W,H,L){}
    void draw(){
        sq_call_fl_virtual(this, "draw");
    }
};

FLTK_CONSTRUCTOR(MyFl_Box);

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
#define _DECL_FL_BUTTON_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Button_##name,nparams,pmask}
static SQRegFunction fl_button_obj_funcs[]={
	_DECL_FL_BUTTON_FUNC(constructor,-5,_SC("xiiiis")),
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

/*
FLTK_CONSTRUCTOR(Fl_Menu_Item);
#define _DECL_FL_MENU_ITEM_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Menu_Item_##name,nparams,pmask}
static SQRegFunction fl_menu_item_obj_funcs[]={
	_DECL_FL_MENU_ITEM_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};
*/

FLTK_CONSTRUCTOR(Fl_Menu_);
#define _DECL_FL_MENU__FUNC(name,nparams,pmask) {_SC(#name),_Fl_Menu__##name,nparams,pmask}
static SQRegFunction fl_menu__obj_funcs[]={
	_DECL_FL_MENU__FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Choice);
#define _DECL_FL_CHOICE_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Choice_##name,nparams,pmask}
static SQRegFunction fl_choice_obj_funcs[]={
	_DECL_FL_CHOICE_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};


FLTK_CONSTRUCTOR(Fl_Input_);
#define _DECL_FL_INPUT__FUNC(name,nparams,pmask) {_SC(#name),_Fl_Input__##name,nparams,pmask}
static SQRegFunction fl_input__obj_funcs[]={
	_DECL_FL_INPUT__FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Input);
#define _DECL_FL_INPUT_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Input_##name,nparams,pmask}
static SQRegFunction fl_input_obj_funcs[]={
	_DECL_FL_INPUT_FUNC(constructor,-5,_SC("xiiiis")),
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

static SQInteger _Fl_Group_end(HSQUIRRELVM sqvm)
{
    SETUP_FL_GROUP(sqvm);
    self->end();
	return 0;
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

#define _DECL_FL_GROUP_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Group_##name,nparams,pmask}
static SQRegFunction fl_group_obj_funcs[]={
	_DECL_FL_GROUP_FUNC(constructor,-5,_SC("xiiiis")),
	_DECL_FL_GROUP_FUNC(end,1,_SC("x")),
	_DECL_FL_GROUP_FUNC(resizable,-1,_SC("xx")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Pack);
#define _DECL_FL_PACK_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Pack_##name,nparams,pmask}
static SQRegFunction fl_pack_obj_funcs[]={
	_DECL_FL_PACK_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Flv_List);
#define _DECL_FLV_LIST_FUNC(name,nparams,pmask) {_SC(#name),_Flv_List_##name,nparams,pmask}
static SQRegFunction flv_list_obj_funcs[]={
	_DECL_FLV_LIST_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Flv_Table);
#define _DECL_FLV_TABLE_FUNC(name,nparams,pmask) {_SC(#name),_Flv_Table_##name,nparams,pmask}
static SQRegFunction flv_table_obj_funcs[]={
	_DECL_FLV_TABLE_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Tabs);
#define _DECL_FL_TABS_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Tabs_##name,nparams,pmask}
static SQRegFunction fl_tabs_obj_funcs[]={
	_DECL_FL_TABS_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Display);
#define _DECL_FL_TEXT_DISPLAY_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Text_Display_##name,nparams,pmask}
static SQRegFunction fl_text_display_obj_funcs[]={
	_DECL_FL_TEXT_DISPLAY_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Editor);
#define _DECL_FL_TEXT_EDITOR_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Text_Editor_##name,nparams,pmask}
static SQRegFunction fl_text_editor_obj_funcs[]={
	_DECL_FL_TEXT_EDITOR_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

FLTK_CONSTRUCTOR(Fl_Text_Editor_Buffered);
#define _DECL_FL_TEXT_EDITOR_BUFFERED_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Text_Editor_Buffered_##name,nparams,pmask}
static SQRegFunction fl_text_editor_buffered_obj_funcs[]={
	_DECL_FL_TEXT_EDITOR_BUFFERED_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};


static SQInteger _fl_window_releasehook(SQUserPointer p, SQInteger size)
{
	Fl_Window *self = ((Fl_Window *)p);
	delete self;
	//printf("Releasing %p\n", self);
	return 1;
}

FLTK_CONSTRUCTOR_RELEASE(Fl_Window, AS_IS, _fl_window_releasehook);

#define _DECL_FL_WINDOW_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Window_##name,nparams,pmask}
static SQRegFunction fl_window_obj_funcs[]={
	_DECL_FL_WINDOW_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

static SQInteger _fl_double_window_releasehook(SQUserPointer p, SQInteger size)
{
	Fl_Double_Window *self = ((Fl_Double_Window *)p);
	delete self;
	//printf("Releasing %p\n", self);
	return 1;
}

FLTK_CONSTRUCTOR_RELEASE(Fl_Double_Window, AS_IS, _fl_double_window_releasehook);

#define _DECL_FL_DOUBLE_WINDOW_FUNC(name,nparams,pmask) {_SC(#name),_Fl_Double_Window_##name,nparams,pmask}
static SQRegFunction fl_double_window_obj_funcs[]={
	_DECL_FL_DOUBLE_WINDOW_FUNC(constructor,-5,_SC("xiiiis")),
	{0,0}
};

static void delayedDeleteWindow(void *w){
    printf("delayed %p\n", w);
    Fl_Widget *win = ((Fl_Widget*)w);
    win->hide();
    delete win;
}

static void delWindow(Fl_Widget *sender){
    Fl_Widget *win = sender->parent_root();
    printf("Parent root %p\n", win);
    Fl::add_timeout(0.1, &delayedDeleteWindow, win);
}

static Fl_Double_Window *newMyWin(){

    Fl_Double_Window *win = new Fl_Double_Window(200,200, "My FLTK Window");
    printf("New Window %p\n", win);

	Fl_Button *btnNew = new Fl_Button(50,50, 80,25, "New Win");
	btnNew->callback((Fl_Callback_p)&newMyWin);
	Fl_Button *btnClose = new Fl_Button(50,100, 80,25, "Close Win");
	btnClose->callback(&delWindow);

    win->end();
	win->resizable(win);
	win->show();

	return win;
}

static SQInteger _fl_show(HSQUIRRELVM sqvm)
{
    Fl_Double_Window *win = newMyWin();
	sq_pushinteger(sqvm, Fl::run());
	//delete win;
	return 1;
}

static SQInteger _fl_run(HSQUIRRELVM sqvm)
{
	sq_pushinteger(sqvm, Fl::run());
	return 1;
}


#define _DECL_FL_FUNC(name,nparams,pmask) {_SC(#name),_fl_##name,nparams,pmask}
static SQRegFunction fl_obj_funcs[]={
	_DECL_FL_FUNC(show,-1,_SC("y")),
	_DECL_FL_FUNC(run,1,_SC("y")),
	{0,0}
};
#undef _DECL_FL_FUNC

static void insertFuncs(HSQUIRRELVM sqvm, SQRegFunction *obj_funcs){
	SQInteger i = 0;
	while(obj_funcs[i].name != 0) {
		SQRegFunction &f = obj_funcs[i];
		sq_pushstring(sqvm,f.name,-1);
		sq_newclosure(sqvm,f.f,0);
		sq_setparamscheck(sqvm,f.nparamscheck,f.typemask);
		sq_setnativeclosurename(sqvm,-1,f.name);
		sq_newslot(sqvm,-3,SQFalse);
		i++;
	}
}

#define PUSH_FL_CLASS(klass, parent, method_funcs)\
	sq_pushstring(sqvm,_SC(#klass),-1);\
	sq_pushstring(sqvm,_SC(#parent),-1);\
	if (SQ_FAILED(sq_get(sqvm, -3)))\
        return sq_throwerror(sqvm, _SC("Missing base class."));\
	sq_newclass(sqvm,SQTrue);\
	sq_settypetag(sqvm,-1,(void*)FLTK_TAG(klass));\
	insertFuncs(sqvm, method_funcs);\
	sq_newslot(sqvm,-3,SQFalse);

SQRESULT sqstd_register_fltklib(HSQUIRRELVM sqvm)
{
    Fl::user_data = sqvm;
	sq_pushstring(sqvm,_SC("fltk"),-1);
	sq_newtable(sqvm);

    //Fl class
	sq_pushstring(sqvm,_SC("Fl"),-1);
	sq_newclass(sqvm,SQFalse);
	sq_settypetag(sqvm,-1,(void*)FLTK_TAG(Fl));
	insertFuncs(sqvm, fl_obj_funcs);
	sq_newslot(sqvm,-3,SQFalse);

	//Fl_Widget class
	sq_pushstring(sqvm,_SC("Fl_Widget"),-1);
	sq_newclass(sqvm,SQFalse);
	sq_settypetag(sqvm,-1,(void*)FLTK_TAG(Fl_Widget));
	insertFuncs(sqvm, fl_widget_obj_funcs);
	sq_newslot(sqvm,-3,SQFalse);

	//Fl_Box class
	PUSH_FL_CLASS(Fl_Box, Fl_Widget, fl_box_obj_funcs);

	//Fl_Button class
	PUSH_FL_CLASS(Fl_Button, Fl_Widget, fl_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Light_Button, Fl_Button, fl_light_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Check_Button, Fl_Light_Button, fl_check_button_obj_funcs);
	PUSH_FL_CLASS(Fl_Radio_Button, Fl_Button, fl_radio_button_obj_funcs);

	PUSH_FL_CLASS(Fl_Menu_, Fl_Widget, fl_menu__obj_funcs);
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
	PUSH_FL_CLASS(Fl_Text_Display, Fl_Group, fl_text_display_obj_funcs);
	PUSH_FL_CLASS(Fl_Text_Editor, Fl_Text_Display, fl_text_editor_obj_funcs);
	PUSH_FL_CLASS(Fl_Text_Editor_Buffered, Fl_Text_Editor, fl_text_editor_buffered_obj_funcs);

	PUSH_FL_CLASS(Flv_List, Fl_Group, flv_list_obj_funcs);
	PUSH_FL_CLASS(Flv_Table, Flv_List, flv_table_obj_funcs);

	//Fl_Window class
	PUSH_FL_CLASS(Fl_Window, Fl_Group, fl_window_obj_funcs);

	//Fl_Double_Window class
	PUSH_FL_CLASS(Fl_Double_Window, Fl_Window, fl_double_window_obj_funcs);

	sq_newslot(sqvm,-3,SQFalse); //add fltk table to the root table
    sq_poptop(sqvm); //removes fltk table

#define INT_CONST(key) \
    sq_pushstring(sqvm, _SC(#key), -1);\
    sq_pushinteger(sqvm, key);\
    sq_newslot(sqvm, -3, SQFalse);

    sq_pushconsttable(sqvm);
    INT_CONST(FL_RED);
    INT_CONST(FL_YELLOW);
    INT_CONST(FL_GREEN);
    sq_poptop(sqvm);

    //check to see if we need to release resources
    Fl::do_at_widget_destroy_ = At_Widget_Destroy;

	return SQ_OK;
}

#endif //WITH_FLTK
