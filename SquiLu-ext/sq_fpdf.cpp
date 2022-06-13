#include "squirrel.h"
#include "sqstdblobimpl.h"
#include "fpdf.h"
#include <string.h>
#include <stdio.h>

class Sq_Fpdf : public FPDF {
    protected:
        HSQUIRRELVM sqvm;
        HSQOBJECT self_weakref;
    public:
    Sq_Fpdf(HSQUIRRELVM v, HSQOBJECT &self):FPDF(){
        sqvm = v;
        self_weakref = self;
    }

    void callVirtual(const SQChar *func_name){
        //sq_reservestack(sqvm, 20);
        SQInteger top = sq_gettop(sqvm);
        sq_pushobject(sqvm, self_weakref);
        sq_pushstring(sqvm, func_name, -1);
        sq_get(sqvm, -2);
        if(sq_gettype(sqvm, -1) == OT_CLOSURE){
            sq_push(sqvm, -2); //this
            sq_call(sqvm, 1, SQFalse, SQTrue);
        }
        sq_settop(sqvm, top);
    }

    void Header (){
        callVirtual(_SC("Header"));
    };
    void Footer () {
        callVirtual(_SC("Footer"));
    };
    bool AcceptPageBreak (){
        callVirtual(_SC("AcceptPageBreak"));
        return FPDF::AcceptPageBreak();
    };
};

static SQRESULT sq_glue_releasehook(SQUserPointer p, SQInteger size, void */*ep*/)
{
	Sq_Fpdf *self = ((Sq_Fpdf *)p);
	delete self;
	return 1;
}

static SQRESULT sq_glue_constructor(HSQUIRRELVM v)
{
    //SQ_FUNC_VARS_NO_TOP(v);
    HSQOBJECT self;
    sq_resetobject(&self);
    sq_getstackobj(v, 1, &self);

	Sq_Fpdf *cptr = new Sq_Fpdf(v, self);
    sq_setinstanceup(v, 1, cptr);
    sq_setreleasehook(v,1, sq_glue_releasehook);
	return 1;
}

SQ_OPT_STRING_STRLEN();

static const SQChar *SQ_FPDF_TAG = "Sq_Fpdf";

#define GET_SQ_FPDF() SQ_GET_INSTANCE(v, 1, Sq_Fpdf, SQ_FPDF_TAG)

/* bool AcceptPageBreak(  ) */
static SQRESULT sq_glue_AcceptPageBreak(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushbool(v, self->FPDF::AcceptPageBreak());
	return 1;
}

/* void AddFont( char const * afamily , char const * astyle  = 0 , char const * afile  = 0 ) */
static SQRESULT sq_glue_AddFont(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, afamily);
	SQ_OPT_STRING(v, 3, astyle, 0);
	SQ_OPT_STRING(v, 4, afile, 0);
	self->AddFont(afamily, astyle, afile);
	return 0;
}

/* int AddLink(  ) */
static SQRESULT sq_glue_AddLink(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushinteger(v, self->AddLink());
	return 1;
}

/* void AddPage( FPDF::e_orientation orientation  = FPDF::e_orientation_none , FPDF::st_pagesize * psize  = 0 ) */
static SQRESULT sq_glue_AddPage(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_OPT_INTEGER(v, 2, orientation, FPDF::e_orientation_none);
	SQ_OPT_STRING(v, 3, psize, 0);
	self->AddPage((FPDF::e_orientation)orientation, 0);
	return 0;
}

/* void AliasNbPages( char const * alias  = "{nb}" ) */
static SQRESULT sq_glue_AliasNbPages(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_OPT_STRING(v, 2, alias, "{nb}");
	self->AliasNbPages(alias);
	return 0;
}

/* int CalcLines( pdf_float_t w , char const * txt ) */
static SQRESULT sq_glue_CalcLines(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_GET_STRING(v, 3, txt);
	sq_pushinteger(v, self->CalcLines(w, txt));
	return 1;
}

/* void Cell( pdf_float_t w , pdf_float_t h  = 0.0f , char const * txt  = 0 , char const * border  = 0 , int ln  = 0 , char align  = ' ' , bool fill  = false , int link  = 0 ) */
static SQRESULT sq_glue_Cell(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_OPT_FLOAT(v, 3, h, 0.0f);
	SQ_OPT_STRING(v, 4, txt, 0);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, ln, 0);
	SQ_OPT_INTEGER(v, 7, align, ' ');
	SQ_OPT_BOOL(v, 8, fill, false);
	SQ_OPT_INTEGER(v, 9, link, 0);
	self->Cell(w, h, txt, border, ln, align, fill ? 1 : 0, link);
	return 0;
}

/* void CellFit( pdf_float_t w , pdf_float_t h  = 0 , char const * txt  = 0 , char const * border  = 0 , int ln  = 0 , char align  = ' ' , bool fill  = false , int link  = 0 , bool scale  = false , bool force  = true ) */
static SQRESULT sq_glue_CellFit(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_OPT_FLOAT(v, 3, h, 0);
	SQ_OPT_STRING(v, 4, txt, 0);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, ln, 0);
	SQ_OPT_INTEGER(v, 7, align, ' ');
	SQ_OPT_BOOL(v, 8, fill, false);
	SQ_OPT_INTEGER(v, 9, link, 0);
	SQ_OPT_INTEGER(v, 10, scale, false);
	SQ_OPT_INTEGER(v, 11, force, true);
	self->CellFit(w, h, txt, border, ln, align, fill ? 1 : 0, link, scale, force);
	return 0;
}

/* void CellFitScale( pdf_float_t w , pdf_float_t h  = 0 , char const * txt  = 0 , char const * border  = 0 , int ln  = 0 , char align  = ' ' , bool fill  = false , int link  = 0 ) */
static SQRESULT sq_glue_CellFitScale(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_OPT_FLOAT(v, 3, h, 0);
	SQ_OPT_STRING(v, 4, txt, 0);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, ln, 0);
	SQ_OPT_INTEGER(v, 7, align, ' ');
	SQ_OPT_BOOL(v, 8, fill, false);
	SQ_OPT_INTEGER(v, 9, link, 0);
	self->CellFitScale(w, h, txt, border, ln, align, fill ? 1 : 0, link);
	return 0;
}

/* void CellFitScaleForce( pdf_float_t w , pdf_float_t h  = 0 , char const * txt  = 0 , char const * border  = 0 , int ln  = 0 , char align  = ' ' , bool fill  = false , int link  = 0 ) */
static SQRESULT sq_glue_CellFitScaleForce(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_OPT_FLOAT(v, 3, h, 0);
	SQ_OPT_STRING(v, 4, txt, 0);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, ln, 0);
	SQ_OPT_INTEGER(v, 7, align, ' ');
	SQ_OPT_BOOL(v, 8, fill, false);
	SQ_OPT_INTEGER(v, 9, link, 0);
	self->CellFitScaleForce(w, h, txt, border, ln, align, fill ? 1 : 0, link);
	return 0;
}

/* void CellFitSpace( pdf_float_t w , pdf_float_t h  = 0 , char const * txt  = 0 , char const * border  = 0 , int ln  = 0 , char align  = ' ' , bool fill  = false , int link  = 0 ) */
static SQRESULT sq_glue_CellFitSpace(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_OPT_FLOAT(v, 3, h, 0);
	SQ_OPT_STRING(v, 4, txt, 0);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, ln, 0);
	SQ_OPT_INTEGER(v, 7, align, ' ');
	SQ_OPT_BOOL(v, 8, fill, false);
	SQ_OPT_INTEGER(v, 9, link, 0);
	self->CellFitSpace(w, h, txt, border, ln, align, fill ? 1 : 0, link);
	return 0;
}

/* void CellFitSpaceForce( pdf_float_t w , pdf_float_t h  = 0 , char const * txt  = 0 , char const * border  = 0 , int ln  = 0 , char align  = ' ' , bool fill  = false , int link  = 0 ) */
static SQRESULT sq_glue_CellFitSpaceForce(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_OPT_FLOAT(v, 3, h, 0);
	SQ_OPT_STRING(v, 4, txt, 0);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, ln, 0);
	SQ_OPT_INTEGER(v, 7, align, ' ');
	SQ_OPT_BOOL(v, 8, fill, false);
	SQ_OPT_INTEGER(v, 9, link, 0);
	self->CellFitSpaceForce(w, h, txt, border, ln, align, fill ? 1 : 0, link);
	return 0;
}

/* void CheckPageBreak( pdf_float_t height ) */
static SQRESULT sq_glue_CheckPageBreak(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, height);
	self->CheckPageBreak(height);
	return 0;
}

/* void Circle( pdf_float_t x , pdf_float_t y , pdf_float_t r , char const * style  = "D" ) */
static SQRESULT sq_glue_Circle(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_FLOAT(v, 4, r);
	SQ_OPT_STRING(v, 5, style, "D");
	self->Circle(x, y, r, style);
	return 0;
}

/* void ClippedCell( pdf_float_t w , pdf_float_t h  = 0 , char const * txt  = 0 , char const * border  = 0 , int ln  = 0 , char align  = ' ' , bool fill  = false , int link  = 0 ) */
static SQRESULT sq_glue_ClippedCell(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_OPT_FLOAT(v, 3, h, 0);
	SQ_OPT_STRING(v, 4, txt, 0);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, ln, 0);
	SQ_OPT_INTEGER(v, 7, align, ' ');
	SQ_OPT_BOOL(v, 8, fill, false);
	SQ_OPT_INTEGER(v, 9, link, 0);
	self->ClippedCell(w, h, txt, border, ln, align, fill ? 1 : 0, link);
	return 0;
}

/* void ClippingRect( pdf_float_t x , pdf_float_t y , pdf_float_t w , pdf_float_t h , bool outline  = false ) */
static SQRESULT sq_glue_ClippingRect(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_FLOAT(v, 4, w);
	SQ_GET_FLOAT(v, 5, h);
	SQ_OPT_INTEGER(v, 6, outline, false);
	self->ClippingRect(x, y, w, h, outline);
	return 0;
}

/* void ClippingText( pdf_float_t x , pdf_float_t y , char const * txt , bool outline  = false ) */
static SQRESULT sq_glue_ClippingText(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_STRING(v, 4, txt);
	SQ_OPT_INTEGER(v, 5, outline, false);
	self->ClippingText(x, y, txt, outline);
	return 0;
}

/* void Close(  ) */
static SQRESULT sq_glue_Close(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	self->Close();
	return 0;
}

/* void Ellipse( pdf_float_t x , pdf_float_t y , pdf_float_t rx , pdf_float_t ry , char const * style  = "D" ) */
static SQRESULT sq_glue_Ellipse(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_FLOAT(v, 4, rx);
	SQ_GET_FLOAT(v, 5, ry);
	SQ_OPT_STRING(v, 6, style, "D");
	self->Ellipse(x, y, rx, ry, style);
	return 0;
}

/* void Error( char const * msg ) */
static SQRESULT sq_glue_Error(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, msg);
	self->Error(msg);
	return 0;
}

/* void Footer(  ) */
static SQRESULT sq_glue_Footer(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	self->FPDF::Footer();
	return 0;
}

/* const & GetAliasNbPages(  ) */
static SQRESULT sq_glue_GetAliasNbPages(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	std::string str = self->GetAliasNbPages();
	sq_pushstring(v, str.c_str(), str.size());
	return 1;
}

static void push_pdf_color(HSQUIRRELVM v, FPDF::pdf_color_t &color){
	if(sq_gettop(v) >= 2 && sq_gettype(v, 2) == OT_TABLE){
	    sq_settop(v, 2);
	}
	else sq_newtable(v);

	sq_pushliteral(v, _SC("r"));
	sq_pushinteger(v, color.r);
	sq_rawset(v, -3);

	sq_pushliteral(v, _SC("g"));
	sq_pushinteger(v, color.g);
	sq_rawset(v, -3);

	sq_pushliteral(v, _SC("b"));
	sq_pushinteger(v, color.b);
	sq_rawset(v, -3);

	sq_pushliteral(v, _SC("t"));
	sq_pushinteger(v, color.t);
	sq_rawset(v, -3);
}

/* void GetDrawColor( FPDF::pdf_color_t & color ) */
static SQRESULT sq_glue_GetDrawColor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	FPDF::pdf_color_t color;
	self->GetDrawColor(color);
    push_pdf_color(v, color);
	return 1;
}

/* void GetFillColor( FPDF::pdf_color_t & color ) */
static SQRESULT sq_glue_GetFillColor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	FPDF::pdf_color_t color;
	self->GetFillColor(color);
    push_pdf_color(v, color);
	return 1;
}

/* void GetFontSettings( font_settings_st & fs ) */
static SQRESULT sq_glue_GetFontSettings(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	FPDF::font_settings_st fs;
	self->GetFontSettings(fs);
	if(_top_ >= 2 && sq_gettype(v, 2) == OT_TABLE){
	    sq_settop(v, 2);
	}
	else sq_newtable(v);

	sq_pushliteral(v, _SC("family"));
	sq_pushstring(v, fs.family.c_str(), fs.family.size());
	sq_rawset(v, -3);

	sq_pushliteral(v, _SC("style"));
	sq_pushstring(v, fs.style.c_str(), fs.style.size());
	sq_rawset(v, -3);

	sq_pushliteral(v, _SC("size"));
	sq_pushfloat(v, fs.size);
	sq_rawset(v, -3);

	return 1;
}

/* pdf_float_t GetFontSize(  ) */
static SQRESULT sq_glue_GetFontSize(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushfloat(v, self->GetFontSize());
	return 1;
}

/* pdf_float_t GetH(  ) */
static SQRESULT sq_glue_GetH(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushfloat(v, self->GetH());
	return 1;
}

/* pdf_float_t GetLeftMargin(  ) */
static SQRESULT sq_glue_GetLeftMargin(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushfloat(v, self->GetLeftMargin());
	return 1;
}

/* pdf_float_t GetRightMargin(  ) */
static SQRESULT sq_glue_GetRightMargin(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushfloat(v, self->GetRightMargin());
	return 1;
}

/* pdf_float_t GetStringWidth( char const * s ) */
static SQRESULT sq_glue_GetStringWidth(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, s);
	sq_pushfloat(v, self->GetStringWidth(s));
	return 1;
}

/* void GetTextColor( FPDF::pdf_color_t & color ) */
static SQRESULT sq_glue_GetTextColor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	FPDF::pdf_color_t color;
	self->GetTextColor(color);
    push_pdf_color(v, color);
	return 1;
}

/* pdf_float_t GetW(  ) */
static SQRESULT sq_glue_GetW(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushfloat(v, self->GetW());
	return 1;
}

/* pdf_float_t GetX(  ) */
static SQRESULT sq_glue_GetX(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushfloat(v, self->GetX());
	return 1;
}

/* pdf_float_t GetY(  ) */
static SQRESULT sq_glue_GetY(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushfloat(v, self->GetY());
	return 1;
}

/* void Header(  ) */
static SQRESULT sq_glue_Header(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	self->FPDF::Header();
	return 0;
}

/* void Image( char const * file , pdf_float_t x  = -1 , pdf_float_t y  = -1 , pdf_float_t w  = 0.0 , pdf_float_t h  = 0.0 , char const * atype  = 0 , int link  = 0 ) */
static SQRESULT sq_glue_Image(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, file);
	SQ_OPT_FLOAT(v, 3, x, -1);
	SQ_OPT_FLOAT(v, 4, y, -1);
	SQ_OPT_FLOAT(v, 5, w, 0.0);
	SQ_OPT_FLOAT(v, 6, h, 0.0);
	SQ_OPT_STRING(v, 7, atype, 0);
	SQ_OPT_INTEGER(v, 8, link, 0);
	self->Image(file, x, y, w, h, atype, link);
	return 0;
}

/* void IncludeJS( char const * script ) */
static SQRESULT sq_glue_IncludeJS(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, script);
	self->IncludeJS(script);
	return 0;
}

/* void Line( pdf_float_t x1 , pdf_float_t y1 , pdf_float_t x2 , pdf_float_t y2 ) */
static SQRESULT sq_glue_Line(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x1);
	SQ_GET_FLOAT(v, 3, y1);
	SQ_GET_FLOAT(v, 4, x2);
	SQ_GET_FLOAT(v, 5, y2);
	self->Line(x1, y1, x2, y2);
	return 0;
}

/* void Link( pdf_float_t x , pdf_float_t y , pdf_float_t w , pdf_float_t h , int link ) */
static SQRESULT sq_glue_Link(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_FLOAT(v, 4, w);
	SQ_GET_FLOAT(v, 5, h);
	SQ_GET_INTEGER(v, 6, link);
	self->Link(x, y, w, h, link);
	return 0;
}

/* void Ln( pdf_float_t h  = 0.0 ) */
static SQRESULT sq_glue_Ln(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_OPT_FLOAT(v, 2, h, 0.0);
	self->Ln(h);
	return 0;
}

/* void MultiCell( pdf_float_t w , pdf_float_t h , char const * txt , char const * border  = 0 , char align  = 'J' , bool fill  = false ) */
static SQRESULT sq_glue_MultiCell(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_GET_FLOAT(v, 3, h);
	SQ_GET_STRING(v, 4, txt);
	SQ_OPT_STRING(v, 5, border, 0);
	SQ_OPT_INTEGER(v, 6, align, 'J');
	SQ_OPT_BOOL(v, 7, fill, false);
	self->MultiCell(w, h, txt, border, align, fill ? 1 : 0);
	return 0;
}

/* void MultiCellBlt( pdf_float_t w , pdf_float_t h , char const * blt , char const * txt , char const * border  = 0 , char align  = 'J' , bool fill  = false ) */
static SQRESULT sq_glue_MultiCellBlt(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, w);
	SQ_GET_FLOAT(v, 3, h);
	SQ_GET_STRING(v, 4, blt);
	SQ_GET_STRING(v, 5, txt);
	SQ_OPT_STRING(v, 6, border, 0);
	SQ_OPT_INTEGER(v, 7, align, 'J');
	SQ_OPT_BOOL(v, 8, fill, false);
	self->MultiCellBlt(w, h, blt, txt, border, align, fill ? 1 : 0);
	return 0;
}

/* void Open(  ) */
static SQRESULT sq_glue_Open(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	self->Open();
	return 0;
}

/* string Output( char const * name  = 0 , char dest  = ' ' ) */
static SQRESULT sq_glue_Output(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQBlob *blob = NULL;
	if(_top_ > 1) {
        SQObjectType ptype = sq_gettype(v, 2);
        if(ptype == OT_INSTANCE){
            if(SQ_FAILED(sq_getinstanceup(v,2,(SQUserPointer*)&blob,(SQUserPointer)SQBlob::SQBlob_TAG,SQFalse)))
                return sq_throwerror(v,_SC("invalid type tag"));
            if(!blob || !blob->IsValid())
                return sq_throwerror(v,_SC("the blob is invalid"));
            std::string str = self->Output(0, 'S');
            blob->SetLen(str.size());
            memcpy(blob->GetBuf(), str.c_str(), str.size());
            return 0;
        }
	}
	SQ_OPT_STRING(v, 2, name, 0);
	SQ_OPT_INTEGER(v, 3, dest, ' ');

	std::string str = self->Output(name, dest);
	sq_pushstring(v, str.c_str(), str.size());
	return 1;
}

/* int PageNo(  ) */
static SQRESULT sq_glue_PageNo(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushinteger(v, self->PageNo());
	return 1;
}

/* void Rect( pdf_float_t x , pdf_float_t y , pdf_float_t w , pdf_float_t h , char const * style  = 0 ) */
static SQRESULT sq_glue_Rect(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_FLOAT(v, 4, w);
	SQ_GET_FLOAT(v, 5, h);
	SQ_OPT_STRING(v, 6, style, 0);
	self->Rect(x, y, w, h, style);
	return 0;
}

/* void Rotate( pdf_float_t angle , pdf_float_t x  = -1 , pdf_float_t y  = -1 ) */
static SQRESULT sq_glue_Rotate(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, angle);
	SQ_OPT_FLOAT(v, 3, x, -1);
	SQ_OPT_FLOAT(v, 4, y, -1);
	self->Rotate(angle, x, y);
	return 0;
}

/* void RotatedText( pdf_float_t x , pdf_float_t y , char const * txt , pdf_float_t angle ) */
static SQRESULT sq_glue_RotatedText(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_STRING(v, 4, txt);
	SQ_GET_FLOAT(v, 5, angle);
	self->RotatedText(x, y, txt, angle);
	return 0;
}

/* void RoundedRect( pdf_float_t x , pdf_float_t y , pdf_float_t w , pdf_float_t h , pdf_float_t r , char const * style  = "" ) */
static SQRESULT sq_glue_RoundedRect(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_FLOAT(v, 4, w);
	SQ_GET_FLOAT(v, 5, h);
	SQ_GET_FLOAT(v, 6, r);
	SQ_OPT_STRING(v, 7, style, "");
	self->RoundedRect(x, y, w, h, r, style);
	return 0;
}

/* void SetAlpha( pdf_float_t alpha , char const * bm  = 0 ) */
static SQRESULT sq_glue_SetAlpha(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, alpha);
	SQ_OPT_STRING(v, 3, bm, 0);
	self->SetAlpha(alpha, bm);
	return 0;
}

/* void SetAuthor( char const * author ) */
static SQRESULT sq_glue_SetAuthor(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, author);
	self->SetAuthor(author);
	return 0;
}

/* void SetAutoPageBreak( bool b , pdf_float_t margin  = 0.0f ) */
static SQRESULT sq_glue_SetAutoPageBreak(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_INTEGER(v, 2, b);
	SQ_OPT_FLOAT(v, 3, margin, 0.0f);
	self->SetAutoPageBreak(b, margin);
	return 0;
}

/* void SetCompression( bool compress ) */
static SQRESULT sq_glue_SetCompression(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_BOOL(v, 2, compress);
	self->SetCompression(compress);
	return 0;
}

/* void SetCreator( char const * creator ) */
static SQRESULT sq_glue_SetCreator(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, creator);
	self->SetCreator(creator);
	return 0;
}

/* void SetDash( pdf_float_t black  = -1 , pdf_float_t white  = -1 ) */
static SQRESULT sq_glue_SetDash(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_OPT_FLOAT(v, 2, black, -1);
	SQ_OPT_FLOAT(v, 3, white, -1);
	self->SetDash(black, white);
	return 0;
}

/* void SetDisplayMode( FPDF::e_zoom_mode zoom , FPDF::e_layout_mode layout  = FPDF::e_layout_default ) */
static SQRESULT sq_glue_SetDisplayMode(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_INTEGER(v, 2, zoom);
	SQ_OPT_INTEGER(v, 3, layout, FPDF::e_layout_default);
	//self->SetDisplayMode(zoom, layout);
	return 0;
}

/* void SetDoubleSided( pdf_float_t inner  = 7 , pdf_float_t outer  = 13 ) */
static SQRESULT sq_glue_SetDoubleSided(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_OPT_FLOAT(v, 2, inner, 7);
	SQ_OPT_FLOAT(v, 3, outer, 13);
	self->SetDoubleSided(inner, outer);
	return 0;
}

static SQRESULT get_pdfcolor_from_table(HSQUIRRELVM v, FPDF::pdf_color_t &color){
	if(sq_gettop(v) == 2){
	    if(sq_gettype(v, 2) == OT_TABLE){
	        SQInteger icolor;
	        sq_pushliteral(v, _SC("r"));
	        if(sq_rawget(v, 2) != SQ_OK) return SQ_ERROR;
            sq_getinteger(v, -1, &icolor);
            sq_poptop(v);
            color.r = icolor;

	        sq_pushliteral(v, _SC("g"));
	        if(sq_rawget(v, 2) != SQ_OK) return SQ_ERROR;
            sq_getinteger(v, -1, &icolor);
            sq_poptop(v);
            color.g = icolor;

	        sq_pushliteral(v, _SC("b"));
	        if(sq_rawget(v, 2) != SQ_OK) return SQ_ERROR;
            sq_getinteger(v, -1, &icolor);
            sq_poptop(v);
            color.b = icolor;

	        sq_pushliteral(v, _SC("t"));
	        if(sq_rawget(v, 2) != SQ_OK) return SQ_ERROR;
            sq_getinteger(v, -1, &icolor);
            sq_poptop(v);
            color.t = icolor;

	        return SQ_OK;
	    }
	}
	return SQ_ERROR;
}

/* void SetDrawColor( unsigned char r , unsigned char g , unsigned char b ) */
static SQRESULT sq_glue_SetDrawColor(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	if(_top_ == 2){
	    FPDF::pdf_color_t color;
	    if(get_pdfcolor_from_table(v, color) == SQ_OK){
	        self->SetDrawColor(color);
	    }
	    else if(sq_gettype(v, 2) == OT_INTEGER){
	        SQInteger icolor;
	        sq_getinteger(v, 2, &icolor);
	        self->SetDrawColor(icolor);
	    }
	    else return SQ_ERROR;
	}
	else
	{
        SQ_GET_INTEGER(v, 2, r);
        SQ_GET_INTEGER(v, 3, g);
        SQ_GET_INTEGER(v, 4, b);
        self->SetDrawColor(r, g, b);
	}
	return 0;
}

/* void SetFillColor( unsigned char r , unsigned char g , unsigned char b ) */
static SQRESULT sq_glue_SetFillColor(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	if(_top_ == 2){
	    FPDF::pdf_color_t color;
	    if(get_pdfcolor_from_table(v, color) == SQ_OK){
	        self->SetFillColor(color);
	    }
	    else if(sq_gettype(v, 2) == OT_INTEGER){
	        SQInteger icolor;
	        sq_getinteger(v, 2, &icolor);
	        self->SetFillColor(icolor);
	    }
	    else return SQ_ERROR;
	}
	else
	{
        SQ_GET_INTEGER(v, 2, r);
        SQ_GET_INTEGER(v, 3, g);
        SQ_GET_INTEGER(v, 4, b);
        self->SetFillColor(r, g, b);
	}
	return 0;
}

/* void SetFont( char const * afamily  = 0 , char const * astyle  = 0 , pdf_float_t size  = 0 ) */
static SQRESULT sq_glue_SetFont(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_OPT_STRING(v, 2, afamily, 0);
	SQ_OPT_STRING(v, 3, astyle, 0);
	SQ_OPT_FLOAT(v, 4, size, 0);
	self->SetFont(afamily, astyle, size);
	return 0;
}

/* void SetFontSettings( font_settings_st & fs ) */
static SQRESULT sq_glue_SetFontSettings(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	if(_top_ == 2){
	    if(sq_gettype(v, 2) == OT_TABLE){
	        const SQChar *str;
	        SQFloat num;
	        FPDF::font_settings_st fs;

	        sq_pushliteral(v, _SC("family"));
	        if(sq_rawget(v, 2) != SQ_OK) return SQ_ERROR;
            sq_getstring(v, -1, &str);
            sq_poptop(v);
            fs.family = str;

	        sq_pushliteral(v, _SC("style"));
	        if(sq_rawget(v, 2) != SQ_OK) return SQ_ERROR;
            sq_getstring(v, -1, &str);
            sq_poptop(v);
            fs.style = str;

	        sq_pushliteral(v, _SC("size"));
	        if(sq_rawget(v, 2) != SQ_OK) return SQ_ERROR;
            sq_getfloat(v, -1, &num);
            sq_poptop(v);
            fs.size = num;
            self->SetFontSettings(fs);
	        return 0;
	    }
	}
	return SQ_ERROR;
}

/* void SetFontSize( pdf_float_t size ) */
static SQRESULT sq_glue_SetFontSize(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, size);
	self->SetFontSize(size);
	return 0;
}

/* void SetKeywords( char const * keywords ) */
static SQRESULT sq_glue_SetKeywords(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, keywords);
	self->SetKeywords(keywords);
	return 0;
}

/* void SetLeftMargin( pdf_float_t margin ) */
static SQRESULT sq_glue_SetLeftMargin(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, margin);
	self->SetLeftMargin(margin);
	return 0;
}

/* void SetLineWidth( pdf_float_t width ) */
static SQRESULT sq_glue_SetLineWidth(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, width);
	self->SetLineWidth(width);
	return 0;
}

/* void SetLink( int link , pdf_float_t y  = 0 , int page  = -1 ) */
static SQRESULT sq_glue_SetLink(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_INTEGER(v, 2, link);
	SQ_OPT_FLOAT(v, 3, y, 0);
	SQ_OPT_INTEGER(v, 4, page, -1);
	self->SetLink(link, y, page);
	return 0;
}

/* void SetMargins( pdf_float_t left , pdf_float_t top , pdf_float_t right  = 0.0f ) */
static SQRESULT sq_glue_SetMargins(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, left);
	SQ_GET_FLOAT(v, 3, top);
	SQ_OPT_FLOAT(v, 4, right, 0.0f);
	self->SetMargins(left, top, right);
	return 0;
}

/* void SetRightMargin( pdf_float_t margin ) */
static SQRESULT sq_glue_SetRightMargin(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, margin);
	self->SetRightMargin(margin);
	return 0;
}

/* void SetSubject( char const * subject ) */
static SQRESULT sq_glue_SetSubject(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, subject);
	self->SetSubject(subject);
	return 0;
}

/* void SetTextColor( unsigned char r , unsigned char g , unsigned char b ) */
static SQRESULT sq_glue_SetTextColor(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	if(_top_ == 2){
	    FPDF::pdf_color_t color;
	    if(get_pdfcolor_from_table(v, color) == SQ_OK){
	        self->SetTextColor(color);
	    }
	    else if(sq_gettype(v, 2) == OT_INTEGER){
	        SQInteger icolor;
	        sq_getinteger(v, 2, &icolor);
	        self->SetTextColor(icolor);
	    }
	    else return SQ_ERROR;
	}
	else
	{
        SQ_GET_INTEGER(v, 2, r);
        SQ_GET_INTEGER(v, 3, g);
        SQ_GET_INTEGER(v, 4, b);
        self->SetTextColor(r, g, b);
	}
	return 0;
}

/* void SetTitle( char const * title ) */
static SQRESULT sq_glue_SetTitle(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_STRING(v, 2, title);
	self->SetTitle(title);
	return 0;
}

/* void SetTopMargin( pdf_float_t margin ) */
static SQRESULT sq_glue_SetTopMargin(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, margin);
	self->SetTopMargin(margin);
	return 0;
}

/* void SetX( pdf_float_t x ) */
static SQRESULT sq_glue_SetX(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	self->SetX(x);
	return 0;
}

/* void SetXY( pdf_float_t x , pdf_float_t y ) */
static SQRESULT sq_glue_SetXY(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	self->SetXY(x, y);
	return 0;
}

/* void SetY( pdf_float_t y ) */
static SQRESULT sq_glue_SetY(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, y);
	self->SetY(y);
	return 0;
}

/* void Text( pdf_float_t x , pdf_float_t y , char const * txt ) */
static SQRESULT sq_glue_Text(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_STRING(v, 4, txt);
	self->Text(x, y, txt);
	return 0;
}

/* void TextShadow( pdf_float_t x , pdf_float_t y , char const * txt , pdf_float_t displacement  = .3 ) */
static SQRESULT sq_glue_TextShadow(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, x);
	SQ_GET_FLOAT(v, 3, y);
	SQ_GET_STRING(v, 4, txt);
	SQ_OPT_FLOAT(v, 5, displacement, .3);
	self->TextShadow(x, y, txt, displacement);
	return 0;
}

/* void UnsetClipping(  ) */
static SQRESULT sq_glue_UnsetClipping(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	self->UnsetClipping();
	return 0;
}

/* void Write( pdf_float_t h , char const * txt , int link  = 0 ) */
static SQRESULT sq_glue_Write(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_GET_FLOAT(v, 2, h);
	SQ_GET_STRING(v, 3, txt);
	SQ_OPT_INTEGER(v, 4, link, 0);
	self->Write(h, txt, link);
	return 0;
}

/* int getCustomZoom(  ) */
static SQRESULT sq_glue_getCustomZoom(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	sq_pushinteger(v, self->getCustomZoom());
	return 1;
}

/* void reset( FPDF::e_orientation orientation  = FPDF::e_orientation_portrait , FPDF::e_units unit  = FPDF::e_mm , FPDF::e_page_sizes psize  = FPDF::e_A4 ) */
static SQRESULT sq_glue_reset(HSQUIRRELVM v){
	SQ_FUNC_VARS(v);
	GET_SQ_FPDF();
	SQ_OPT_INTEGER(v, 2, orientation, FPDF::e_orientation_portrait);
	SQ_OPT_INTEGER(v, 3, unit, FPDF::e_mm);
	SQ_OPT_INTEGER(v, 4, psize, FPDF::e_A4);
	//self->reset(orientation, unit, psize);
	return 0;
}

/* void setCustomZoom( int zoom ) */
static SQRESULT sq_glue_setCustomZoom(HSQUIRRELVM v){
	SQ_FUNC_VARS_NO_TOP(v);
	GET_SQ_FPDF();
	SQ_GET_INTEGER(v, 2, zoom);
	self->setCustomZoom(zoom);
	return 0;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  sq_glue_##name,nparams,tycheck}
static SQRegFunction sq_glue_my_methods[] =
{
	_DECL_FUNC(constructor,  1, _SC("x")),
	_DECL_FUNC(AcceptPageBreak,  1, _SC("x")),
	_DECL_FUNC(AddFont,  -2, _SC("xsss")),
	_DECL_FUNC(AddLink,  1, _SC("x")),
	_DECL_FUNC(AddPage,  -1, _SC("xis")),
	_DECL_FUNC(AliasNbPages,  -1, _SC("xs")),
	_DECL_FUNC(CalcLines,  3, _SC("xns")),
	_DECL_FUNC(Cell,  -2, _SC("xnnssiibi")),
	_DECL_FUNC(CellFit,  -2, _SC("xnnssiibiii")),
	_DECL_FUNC(CellFitScale,  -2, _SC("xnnssiibi")),
	_DECL_FUNC(CellFitScaleForce,  -2, _SC("xnnssiibi")),
	_DECL_FUNC(CellFitSpace,  -2, _SC("xnnssiibi")),
	_DECL_FUNC(CellFitSpaceForce,  -2, _SC("xnnssiibi")),
	_DECL_FUNC(CheckPageBreak,  2, _SC("xn")),
	_DECL_FUNC(Circle,  -4, _SC("xnnns")),
	_DECL_FUNC(ClippedCell,  -2, _SC("xnnssiibi")),
	_DECL_FUNC(ClippingRect,  -5, _SC("xnnnni")),
	_DECL_FUNC(ClippingText,  -4, _SC("xnnsi")),
	_DECL_FUNC(Close,  1, _SC("x")),
	_DECL_FUNC(Ellipse,  -5, _SC("xnnnns")),
	_DECL_FUNC(Error,  2, _SC("xs")),
	_DECL_FUNC(Footer,  1, _SC("x")),
	_DECL_FUNC(GetAliasNbPages,  1, _SC("x")),
	_DECL_FUNC(GetDrawColor,  -1, _SC("xt")),
	_DECL_FUNC(GetFillColor,  -1, _SC("xt")),
	_DECL_FUNC(GetFontSettings,  -1, _SC("xt")),
	_DECL_FUNC(GetFontSize,  1, _SC("x")),
	_DECL_FUNC(GetH,  1, _SC("x")),
	_DECL_FUNC(GetLeftMargin,  1, _SC("x")),
	_DECL_FUNC(GetRightMargin,  1, _SC("x")),
	_DECL_FUNC(GetStringWidth,  2, _SC("xs")),
	_DECL_FUNC(GetTextColor,  -1, _SC("xt")),
	_DECL_FUNC(GetW,  1, _SC("x")),
	_DECL_FUNC(GetX,  1, _SC("x")),
	_DECL_FUNC(GetY,  1, _SC("x")),
	_DECL_FUNC(Header,  1, _SC("x")),
	_DECL_FUNC(Image,  -2, _SC("xsnnnnsi")),
	_DECL_FUNC(IncludeJS,  2, _SC("xs")),
	_DECL_FUNC(Line,  5, _SC("xnnnn")),
	_DECL_FUNC(Link,  6, _SC("xnnnni")),
	_DECL_FUNC(Ln,  -1, _SC("xn")),
	_DECL_FUNC(MultiCell,  -4, _SC("xnnssib")),
	_DECL_FUNC(MultiCellBlt,  -5, _SC("xnnsssbi")),
	_DECL_FUNC(Open,  1, _SC("x")),
	_DECL_FUNC(Output,  -1, _SC("x s|x i")),
	_DECL_FUNC(PageNo,  1, _SC("x")),
	_DECL_FUNC(Rect,  -5, _SC("xnnnns")),
	_DECL_FUNC(Rotate,  -2, _SC("xnnn")),
	_DECL_FUNC(RotatedText,  5, _SC("xnnsn")),
	_DECL_FUNC(RoundedRect,  -6, _SC("xnnnnns")),
	_DECL_FUNC(SetAlpha,  -2, _SC("xns")),
	_DECL_FUNC(SetAuthor,  2, _SC("xs")),
	_DECL_FUNC(SetAutoPageBreak,  -2, _SC("xin")),
	_DECL_FUNC(SetCompression,  2, _SC("xb")),
	_DECL_FUNC(SetCreator,  2, _SC("xs")),
	_DECL_FUNC(SetDash,  -1, _SC("xnn")),
	_DECL_FUNC(SetDisplayMode,  -2, _SC("xii")),
	_DECL_FUNC(SetDoubleSided,  -1, _SC("xnn")),
	_DECL_FUNC(SetDrawColor,  -2, _SC("xiii")),
	_DECL_FUNC(SetFillColor,  -2, _SC("x t|i ii")),
	_DECL_FUNC(SetFont,  -1, _SC("xssn")),
	_DECL_FUNC(SetFontSettings,  2, _SC("xt")),
	_DECL_FUNC(SetFontSize,  2, _SC("xn")),
	_DECL_FUNC(SetKeywords,  2, _SC("xs")),
	_DECL_FUNC(SetLeftMargin,  2, _SC("xn")),
	_DECL_FUNC(SetLineWidth,  2, _SC("xn")),
	_DECL_FUNC(SetLink,  -2, _SC("xini")),
	_DECL_FUNC(SetMargins,  -3, _SC("xnnn")),
	_DECL_FUNC(SetRightMargin,  2, _SC("xn")),
	_DECL_FUNC(SetSubject,  2, _SC("xs")),
	_DECL_FUNC(SetTextColor,  -2, _SC("xiii")),
	_DECL_FUNC(SetTitle,  2, _SC("xs")),
	_DECL_FUNC(SetTopMargin,  2, _SC("xn")),
	_DECL_FUNC(SetX,  2, _SC("xn")),
	_DECL_FUNC(SetXY,  3, _SC("xnn")),
	_DECL_FUNC(SetY,  2, _SC("xn")),
	_DECL_FUNC(Text,  4, _SC("xnns")),
	_DECL_FUNC(TextShadow,  -4, _SC("xnnsn")),
	_DECL_FUNC(UnsetClipping,  1, _SC("x")),
	_DECL_FUNC(Write,  -3, _SC("xnsi")),
	_DECL_FUNC(getCustomZoom,  1, _SC("x")),
	_DECL_FUNC(reset,  -1, _SC("xiii")),
	_DECL_FUNC(setCustomZoom,  2, _SC("xi")),
	{0,0}
};


#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_Sq_Fpdf(HSQUIRRELVM v)
{
    sq_pushstring(v,SQ_FPDF_TAG,-1);
    sq_newclass(v,SQFalse);
    sq_settypetag(v,-1,(void*)SQ_FPDF_TAG);
    sq_insert_reg_funcs(v, sq_glue_my_methods);
    sq_newslot(v,-3,SQTrue);
    return 1;
}

#ifdef __cplusplus
}
#endif
