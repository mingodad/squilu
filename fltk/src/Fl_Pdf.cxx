//
// "$Id: Fl_Pdf.cxx 7653 2010-06-24 08:55:04Z manolo $"
//
// PDF device support for the Fast Light Tool Kit (FLTK).
//
// Copyright 2010 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to:
//
//     http://www.fltk.org/str.php
//

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <stdio.h>
#include <FL/Fl_Pdf.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <math.h>

/*
	Strategy used to deal with coordinate system:
	Translate the page coordinated one page height in
	the "y axys" then multiply by -1 every "y coordinate".
*/

static FILE *dbg = NULL;
#define SDBG
//fprintf(dbg, "%d\n", __LINE__);

#define MTX printf("MTX %f, %f, %f, %f, %f, %f\n", \
	m.a, m.b, m.c, m.d, m.x, m.y);

#define MTX0(page) mtx = HPDF_Page_GetTransMatrix (page);\
	printf("MTX2 %d, %f, %f, %f, %f, %f, %f\n", __LINE__, mtx.a, mtx.b, mtx.c, mtx.d, mtx.x, mtx.y);

#define MTX2 MTX0(pdf_page)
#define MTX3 MTX0(pdf->pdf_page)

#define CHECK_TEXT_BUFFER_SIZE(n) \
	if(text_buffer_size < n) \
		text_buffer = (char*)realloc((void*)text_buffer, ((((n+1)/1024) +1)*1024) * sizeof(char));

const char *Fl_Pdf_Graphics_Driver::class_id = "Fl_Pdf_Graphics_Driver";
const char *Fl_Pdf_File_Device::class_id = "Fl_Pdf_File_Device";
/** \brief Label of the PDF file chooser window */
const char *Fl_Pdf_File_Device::file_chooser_title = "Select a .ps file";

#define NEG(x) (-x)

static void pdf_error_handler (HPDF_STATUS   error_no, HPDF_STATUS   detail_no,  void         *user_data)
{
    fprintf (dbg, "ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
    //longjmp(env, 1);
}

/**
 @brief The constructor.
 */
Fl_Pdf_Graphics_Driver::Fl_Pdf_Graphics_Driver(void)
{
SDBG
  close_cmd_ = 0;
  //lang_level_ = 3;
  lang_level_ = 2;
  mask = 0;
  pdf_filename_ = NULL;
  scale_x = scale_y = 1.;
  bg_r = bg_g = bg_b = 255;
  text_buffer = NULL;
  text_buffer_size = 0;
  pdf_page = NULL;
  pdf_font_ = NULL;
  //dbg = fopen("debug.txt", "w");
  //pdf = HPDF_New (&pdf_error_handler /*error_handler*/, NULL);
  pdf = HPDF_New (NULL /*error_handler*/, NULL);
}

/** \brief The destructor. */
Fl_Pdf_Graphics_Driver::~Fl_Pdf_Graphics_Driver() {
SDBG
  if(pdf_filename_) free(pdf_filename_);
  HPDF_Free (pdf);
  if (dbg) fclose(dbg);
  if(text_buffer) free(text_buffer);
}

/**
 @brief The constructor.
 */
Fl_Pdf_File_Device::Fl_Pdf_File_Device(void)
{
SDBG
#ifdef __APPLE__
  gc = fl_gc; // the display context is used by fl_text_extents()
#endif
  Fl_Surface_Device::driver( new Fl_Pdf_Graphics_Driver() );
}

/**
 \brief Returns the PDF driver of this drawing surface.
 */
Fl_Pdf_Graphics_Driver *Fl_Pdf_File_Device::driver()
{
  return (Fl_Pdf_Graphics_Driver*)Fl_Surface_Device::driver();
}


/**
 @brief Begins the session where all graphics requests will go to a local PDF file.
 *
 Opens a file dialog entitled with Fl_Pdf_File_Device::file_chooser_title to select an output PDF file.
 @param pagecount The total number of pages to be created.
 @param format Desired page format.
 @param layout Desired page layout.
 @return 0 iff OK, 1 if user cancelled the file dialog, 2 if fopen failed on user-selected output file.
 */
int Fl_Pdf_File_Device::start_job (int pagecount, enum Fl_Paged_Device::Page_Format format, enum Fl_Paged_Device::Page_Layout layout)
{
SDBG
  Fl_Native_File_Chooser fnfc;
  fnfc.title(Fl_Pdf_File_Device::file_chooser_title);
  fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
  fnfc.filter("PDF\t*.pdf\n");
  // Show native chooser
  if ( fnfc.show() ) return 1;
  Fl_Pdf_Graphics_Driver *pdf = driver();
  pdf->pdf_filename_ = strdup(fnfc.filename());
  pdf->start_PDF(pagecount, format, layout);
  this->set_current();
  return 0;
}

/**
 @brief Begins the session where all graphics requests will go to FILE pointer.
 *
 @param ps_output A writable FILE pointer that will receive PDF output and that will be closed
 when end_job() will be called.
 @param pagecount The total number of pages to be created.
 @param format Desired page format.
 @param layout Desired page layout.
 @return always 0.
 */
int Fl_Pdf_File_Device::start_job (const char *pdf_filename, int pagecount, enum Fl_Paged_Device::Page_Format format, enum Fl_Paged_Device::Page_Layout layout)
{
SDBG
  Fl_Pdf_Graphics_Driver *pdf = driver();
  pdf->pdf_filename_ = strdup((char*)pdf_filename);
  pdf->start_PDF(pagecount, format, layout);
  this->set_current();
  return 0;
}

/**
 @brief The destructor.
 */
Fl_Pdf_File_Device::~Fl_Pdf_File_Device() {
SDBG
  Fl_Pdf_Graphics_Driver *pdf = driver();
  if (pdf) delete pdf;
}

#ifndef FL_DOXYGEN

#if ! (defined(__APPLE__) || defined(WIN32) )
  #include "print_panel.cxx"
#endif

int Fl_Pdf_Graphics_Driver::start_PDF (int pagecount, enum Fl_Paged_Device::Page_Format format, enum Fl_Paged_Device::Page_Layout layout)
//returns 0 iff OK
{
SDBG
  if (format == Fl_Paged_Device::A4) {
    left_margin = 18;
    top_margin = 18;
  }
  else {
    left_margin = 12;
    top_margin = 12;
  }
  page_format_ = (enum Fl_Paged_Device::Page_Format)(format | layout);

  pw_ = Fl_Paged_Device::page_formats[format].width;
  ph_ = Fl_Paged_Device::page_formats[format].height;
  page_policy_ = 1;

  reset();
  nPages=0;
  return 0;
}

void Fl_Pdf_Graphics_Driver::recover(){
SDBG
  //HPDF_Page_GRestore(pdf_page);
}

void Fl_Pdf_Graphics_Driver::reset(){
SDBG
  gap_=1;
  clip_=0;
  color(0,0,0);
  font(FL_HELVETICA, 12);
  //font(font_, size_);
  linewidth_=0;
  linestyle_=FL_SOLID;
  strcpy(linedash_,"");
  Clip *c=clip_;   ////just not to have memory leaks for badly writen code (forgotten clip popping)

  while(c){
    clip_=clip_->prev;
    delete c;
    c=clip_;
  }

}

void Fl_Pdf_Graphics_Driver::page_policy(int p){
SDBG
}

// //////////////////// paging //////////////////////////////////////////



void Fl_Pdf_Graphics_Driver::page(double pw, double ph, int media) {
SDBG
	pdf_page = HPDF_AddPage (pdf);
	//HPDF_Page_SetSize(pdf_page, HPDF_PAGE_SIZE_A5, HPDF_PAGE_LANDSCAPE);
	//HPDF_STATUS
	HPDF_Page_SetWidth(pdf_page, pw);
	pw_ = pw;
	//HPDF_STATUS
	HPDF_Page_SetHeight(pdf_page, ph);
	ph_ = ph;
	HPDF_Page_Concat(pdf_page, 1,0,0,1,0,ph_);
  	//HPDF_STATUS
  	HPDF_Page_SetLineWidth(pdf_page, 0);
  	//font(font_, size_);
};

void Fl_Pdf_Graphics_Driver::page(int format){
SDBG
  if(format &  Fl_Paged_Device::LANDSCAPE){
    ph_=Fl_Paged_Device::page_formats[format & 0xFF].width;
    pw_=Fl_Paged_Device::page_formats[format & 0xFF].height;
  }else{
    pw_=Fl_Paged_Device::page_formats[format & 0xFF].width;
    ph_=Fl_Paged_Device::page_formats[format & 0xFF].height;
  }
  page(pw_,ph_,format & 0xFF00);//,orientation only;
};

void Fl_Pdf_Graphics_Driver::page(){
SDBG
  page(page_format_);//,orientation only;
};

void Fl_Pdf_Graphics_Driver::rect(int x, int y, int w, int h) {
SDBG
	int line_diff = 0; //linewidth_ ? 0 : 1;
	/*HPDF_STATUS*/
	HPDF_Page_Rectangle(pdf_page, x, NEG(y), w-line_diff, NEG(h-line_diff));
	HPDF_Page_Stroke(pdf_page);
}

void Fl_Pdf_Graphics_Driver::rectf(int x, int y, int w, int h) {
SDBG
	int line_diff = 0; //linewidth_ ? 0 : 1;
  	HPDF_Page_Rectangle(pdf_page, x, NEG(y), w-line_diff, NEG(h-line_diff));
	HPDF_Page_Fill(pdf_page);
}

void Fl_Pdf_Graphics_Driver::line(int x1, int y1, int x2, int y2) {
SDBG
	HPDF_Page_MoveTo(pdf_page, x1, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y2));
	HPDF_Page_Stroke(pdf_page);
}

void Fl_Pdf_Graphics_Driver::line(int x0, int y0, int x1, int y1, int x2, int y2) {
SDBG
	HPDF_Page_MoveTo(pdf_page, x0, NEG(y0));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y2));
	HPDF_Page_Stroke(pdf_page);
}

void Fl_Pdf_Graphics_Driver::xyline(int x, int y, int x1, int y2, int x3){
SDBG
	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y2));
	HPDF_Page_LineTo(pdf_page, x3, NEG(y2));
	HPDF_Page_Stroke(pdf_page);
};


void Fl_Pdf_Graphics_Driver::xyline(int x, int y, int x1, int y2){
SDBG
	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y2));
	HPDF_Page_Stroke(pdf_page);
};

void Fl_Pdf_Graphics_Driver::xyline(int x, int y, int x1){
SDBG
	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y));
	HPDF_Page_Stroke(pdf_page);
};

void Fl_Pdf_Graphics_Driver::yxline(int x, int y, int y1, int x2, int y3){
SDBG
	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
	HPDF_Page_LineTo(pdf_page, x, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y3));
	HPDF_Page_Stroke(pdf_page);
};

void Fl_Pdf_Graphics_Driver::yxline(int x, int y, int y1, int x2){
SDBG
	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
	HPDF_Page_LineTo(pdf_page, x, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y1));
	HPDF_Page_Stroke(pdf_page);
};

void Fl_Pdf_Graphics_Driver::yxline(int x, int y, int y1){
SDBG
	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
	HPDF_Page_LineTo(pdf_page, x, NEG(y1));
	HPDF_Page_Stroke(pdf_page);
};

void Fl_Pdf_Graphics_Driver::loop(int x0, int y0, int x1, int y1, int x2, int y2) {
SDBG
	HPDF_Page_MoveTo(pdf_page, x0, NEG(y0));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y2));
	HPDF_Page_ClosePathStroke(pdf_page);
}

void Fl_Pdf_Graphics_Driver::loop(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
SDBG
	HPDF_Page_MoveTo(pdf_page, x0, NEG(y0));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y2));
	HPDF_Page_LineTo(pdf_page, x3, NEG(y3));
	HPDF_Page_ClosePathStroke(pdf_page);
}

void Fl_Pdf_Graphics_Driver::polygon(int x0, int y0, int x1, int y1, int x2, int y2) {
SDBG
	HPDF_Page_MoveTo(pdf_page, x0, NEG(y0));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y2));
	HPDF_Page_ClosePathFillStroke(pdf_page);
}

void Fl_Pdf_Graphics_Driver::polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
SDBG
	HPDF_Page_MoveTo(pdf_page, x0, NEG(y0));
	HPDF_Page_LineTo(pdf_page, x1, NEG(y1));
	HPDF_Page_LineTo(pdf_page, x2, NEG(y2));
	HPDF_Page_LineTo(pdf_page, x3, NEG(y3));
	HPDF_Page_ClosePathFillStroke(pdf_page);
}

void Fl_Pdf_Graphics_Driver::point(int x, int y){
SDBG
  rectf(x,y,1,1);
}

static int dashes_flat[5][7]={
{-1,0,0,0,0,0,0},
{3,1,-1,0,0,0,0},
{1,1,-1,0,0,0,0},
{3,1,1,1,-1,0,0},
{3,1,1,1,1,1,-1}
};


//yeah, hack...
static double dashes_cap[5][7]={
{-1,0,0,0,0,0,0},
{2,2,-1,0,0,0,0},
{0.01,1.99,-1,0,0,0,0},
{2,2,0.01,1.99,-1,0,0},
{2,2,0.01,1.99,0.01,1.99,-1}
};

static HPDF_UINT16 dash_FL_DASH[1] = {3};
static HPDF_UINT16 dash_FL_DOT[2] = {7,3};
static HPDF_UINT16 dash_FL_DASHDOT[4] = {8,7,2,7};
static HPDF_UINT16 dash_FL_DASHDOTDOT[4] = {8,7,2,7};

void Fl_Pdf_Graphics_Driver::line_style(int style, int width, char* dashes){
SDBG
  //line_styled_=1;

  linewidth_=width;
  linestyle_=style;
  //dashes_= dashes;
  if(dashes){
    if(dashes != linedash_)
      strcpy(linedash_,dashes);

  }else
    linedash_[0]=0;
  char width0 = 0;
  if(!width){
    width=1; //for screen drawing compatability
    width0=1;
  }

  //HPDF_STATUS
  HPDF_Page_SetLineWidth(pdf_page, width-1); //-1 because in pdf 0 is the thinest line instead of 1

  if(!style && (!dashes || !(*dashes)) && width0) //system lines
    style = FL_CAP_SQUARE;

  int cap = (style &0xf00) >> 8;
  if(cap) cap--;
  HPDF_LineCap  line_cap;
  if(style & FL_CAP_ROUND) line_cap = HPDF_ROUND_END;
  else if(style & FL_CAP_SQUARE) line_cap = HPDF_PROJECTING_SCUARE_END;
  else  line_cap = HPDF_BUTT_END;

  //HPDF_STATUS
  HPDF_Page_SetLineCap(pdf_page, line_cap);

  int join = (style & 0xf000) >> 12;

  if(join) join--;
  HPDF_LineJoin  line_join;
  if(style & FL_JOIN_ROUND) line_join = HPDF_ROUND_JOIN;
  else if(style & FL_JOIN_BEVEL) line_join = HPDF_BEVEL_JOIN;
  else line_join = HPDF_MITER_JOIN;
  //HPDF_STATUS
  HPDF_Page_SetLineJoin(pdf_page, line_join);


  if(dashes && *dashes){
    while(*dashes){
      dashes++;
    }
  }else{
    int * ds;
    if(style & 0x200){ // round and square caps, dash length need to be adjusted
      double *dt = dashes_cap[style & 0xff];
      while (*dt >= 0){
	dt++;
      }
    }else{

      ds = dashes_flat[style & 0xff];
      while (*ds >= 0){
        ds++;
      }
    }
  }

  if(style & FL_DASH) HPDF_Page_SetDash(pdf_page, dash_FL_DASH, 1, 1);
  else if(style & FL_DOT) HPDF_Page_SetDash(pdf_page, dash_FL_DOT, 2, 2);
  else if(style & FL_DASHDOT) HPDF_Page_SetDash(pdf_page, dash_FL_DASHDOT, 4, 0);
  else if(style & FL_DASHDOTDOT) HPDF_Page_SetDash(pdf_page, dash_FL_DASHDOTDOT, 4, 0);
  else HPDF_Page_SetDash(pdf_page, NULL, 0, 0);
};

static const char *_fontNames[] = {
"Helvetica",
"Helvetica-Bold",
"Helvetica-Oblique",
"Helvetica-BoldOblique",
"Courier",
"Courier-Bold",
"Courier-Oblique",
"Courier-BoldOblique",
"Times-Roman",
"Times-Bold",
"Times-Italic",
"Times-BoldItalic",
"Symbol",
"Courier",
"Courier-Bold",
"ZapfDingbats"
};

void Fl_Pdf_Graphics_Driver::font(int f, int s) {
SDBG
  if (f >= FL_FREE_FONT)
    f = FL_COURIER;
   /* create default-font */
   //fprintf(dbg, "Font %d %s\n", f, _fontNames[f]);
  Fl_Graphics_Driver *driver = Fl_Display_Device::display_device()->driver();
  driver->font(f,s); // Use display fonts for font measurement
  Fl_Graphics_Driver::font(f, s);
  Fl_Font_Descriptor *desc = driver->font_descriptor();
  this->font_descriptor(desc);

  pdf_font_ = HPDF_GetFont (pdf, _fontNames[f], "WinAnsiEncoding");
  HPDF_Page_SetFontAndSize(pdf_page, (HPDF_Font) pdf_font_, s);
  //fprintf(dbg, "Fotn : %s, %d\n", _fontNames[f], s);
};

void Fl_Pdf_Graphics_Driver::color(Fl_Color c) {
SDBG
	//colored_=1;
  	//fl_color_ = c; // Set display color
  	//color(c);
	Fl_Graphics_Driver::color( c );
  	Fl::get_color(c, cr_, cg_, cb_);
    HPDF_REAL fr, fg, fb;
    fr = cr_/255.0;
    fg = cg_/255.0;
    fb = cb_/255.0;
    //HPDF_Page_Low_Level_Write(pdf_page, "%color(Fl_Color c)\n");
	HPDF_Page_SetRGBFill(pdf_page,  fr , fg , fb);
	HPDF_Page_SetRGBStroke(pdf_page,  fr , fg , fb);
}

void Fl_Pdf_Graphics_Driver::color(unsigned char r, unsigned char g, unsigned char b) {
SDBG
  	color(fl_rgb_color(r,g,b));
}

// outputs in PDF a UTF8 string replacing non-Latin1 characters by ?
// and using the same width in points as on display
static char *pdf_decode_utf8(char *text_buffer, const char* str, int n) {
  int len;
  if (!n || !str || !*str) return NULL;
  const char *last = str + n;
  char *buf = text_buffer;
  // compute display width of string
  while (str < last) {
    // Extract each unicode character of string.
    // Until 0xFF, UTF codes coincide with iso-Latin1 (iso-8859-1)
    unsigned utf = fl_utf8decode(str, last, &len);
    str += len;
    if (utf > 0xFF) {
      utf = '?'; // replace non Latin-1 unicodes by ?
      }
    else if (*last & 0x80) {
      sprintf(buf, "\\%o", utf);
      buf += 4;
      continue;
     }

    *buf++ = utf; // output the latin character
  }
  *buf = '\0';
  return text_buffer;
}

void Fl_Pdf_Graphics_Driver::draw(int angle, const char *str, int n, int x, int y)
{
SDBG
  CHECK_TEXT_BUFFER_SIZE(n*3);
  if (pdf_decode_utf8(text_buffer, str, n)) {
	  //HPDF_STATUS
	  HPDF_Page_GSave(pdf_page);
	  HPDF_REAL fradian = angle / 180.0 * 3.141592; /* Calcurate the radian value. */
	  //HPDF_STATUS
	  HPDF_Page_BeginText(pdf_page);
	  HPDF_Page_SetTextMatrix (pdf_page, cos(fradian), sin(fradian), -sin(fradian), cos(fradian),
					x, NEG(y));
	  HPDF_Page_ShowText(pdf_page, text_buffer);
	  //HPDF_STATUS
	  HPDF_Page_EndText(pdf_page);
	  //HPDF_STATUS
	  HPDF_Page_GRestore(pdf_page);
  }
}

// outputs in PDF a UTF8 string replacing non-Latin1 characters by ?
// and using the same width in points as on display
void Fl_Pdf_Graphics_Driver::transformed_draw(const char* str, int n, double x, double y) {
	CHECK_TEXT_BUFFER_SIZE(n*3);
	if (pdf_decode_utf8(text_buffer, str, n)) {
		//HPDF_STATUS
		HPDF_Page_BeginText(pdf_page);
		//HPDF_STATUS
		HPDF_Page_MoveTextPos(pdf_page, x, NEG(y));
		HPDF_Page_ShowText(pdf_page, text_buffer);
		//HPDF_STATUS
		HPDF_Page_EndText(pdf_page);
	}
}

void Fl_Pdf_Graphics_Driver::rtl_draw(const char* str, int n, int x, int y) {
  const char *last = str + n;
  const char *str2 = str;
  unsigned int *unis = new unsigned int[n + 1];
  char *out = new char[n + 1];
  int u = 0, len;
  char *p = out;
  double w = fl_width(str, n);
  while (str2 < last) {
    unis[u++] = fl_utf8decode(str2, last, &len);
    str2 += len;
    }
  while (u > 0) {
    len = fl_utf8encode(unis[--u], p);
    p += len;
    }
  transformed_draw(out, p - out, x - w, y);
  delete [] unis;
  delete [] out;
}

void Fl_Pdf_Graphics_Driver::concat(){
	//HPDF_STATUS
	//HPDF_Page_Concat(pdf_page, fl_matrix->a , fl_matrix->b , fl_matrix->c , fl_matrix->d , fl_matrix->x , fl_matrix->y);
	//HPDF_Page_Concat(pdf_page, fl_matrix->a , NEG(fl_matrix->b) , fl_matrix->c , fl_matrix->d , fl_matrix->x , NEG(fl_matrix->y));
	HPDF_Page_Concat(pdf_page, fl_matrix->a , fl_matrix->b , fl_matrix->c , fl_matrix->d , fl_matrix->x , NEG(fl_matrix->y));
	//scale
    //HPDF_Page_Concat(pdf_page, 1,0,0,-1,0,0);
}

void Fl_Pdf_Graphics_Driver::reconcat(){
	//HPDF_Page_Concat(pdf_page, 1/fl_matrix->a , fl_matrix->b , fl_matrix->c , -fl_matrix->d , -fl_matrix->x , -(fl_matrix->y));
}

/////////////////  transformed (double) drawings ////////////////////////////////


void Fl_Pdf_Graphics_Driver::begin_points(){
SDBG
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  concat();
  gap_=1;
  shape_=POINTS;
};

void Fl_Pdf_Graphics_Driver::begin_line(){
SDBG
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  concat();
  gap_=1;
  shape_=LINE;
};

void Fl_Pdf_Graphics_Driver::begin_loop(){
SDBG
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  concat();
  gap_=1;
  shape_=LOOP;
};

void Fl_Pdf_Graphics_Driver::begin_polygon(){
SDBG
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  concat();
  gap_=1;
  shape_=POLYGON;
};

void Fl_Pdf_Graphics_Driver::vertex(double x, double y){
SDBG
  if(shape_==POINTS){
	//HPDF_STATUS
    //HPDF_Page_MoveTo(pdf_page, x, changeY(pdf_page, y));
    HPDF_Page_MoveTo(pdf_page, x, NEG(y));
    gap_=1;
    return;
  }
  if(gap_){
	//HPDF_STATUS
  	//HPDF_Page_MoveTo(pdf_page, x, changeY(pdf_page, y));
  	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
    gap_=0;
  }else
	//HPDF_STATUS
	//HPDF_Page_LineTo(pdf_page, x, changeY(pdf_page, y));
	HPDF_Page_LineTo(pdf_page, x, NEG(y));
};

void Fl_Pdf_Graphics_Driver::curve(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3){
SDBG
  if(shape_==NONE) return;

  if(gap_){
    HPDF_Page_MoveTo(pdf_page, x, NEG(y));
	gap_=0;
  }
  else
    HPDF_Page_LineTo(pdf_page, x, NEG(y));

  //HPDF_STATUS
  HPDF_Page_CurveTo(pdf_page, x1, NEG(y1), x2, NEG(y2), x3, NEG(y3));
};


void Fl_Pdf_Graphics_Driver::circle(double x, double y, double r){
  if(shape_==NONE){
  	//HPDF_Page_GSave(pdf_page);
    HPDF_Page_Circle(pdf_page, x, NEG(y), r);
   	//HPDF_Page_Arc(pdf_page, rx, ry, r, 0, 360);
   	//HPDF_Page_GRestore(pdf_page);
  }else {
    //arc(rx, ry, r, 0, 360);
    HPDF_Page_Circle(pdf_page, x, NEG(y), r);
    //HPDF_Page_Arc(pdf_page, rx, ry, r, 0, 360);
  }
};

void Fl_Pdf_Graphics_Driver::arc(double x, double y, double r, double start, double a){
SDBG
  if(shape_==NONE) return;
  gap_=0;
  //y = y-0.06;
  HPDF_Page_MoveTo (pdf_page, x, NEG(y));
  a = a -45;
  start = start-45;
  if(start<a){
  	//HPDF_STATUS
  	HPDF_Page_Arc(pdf_page, x, NEG(y), r, start, a);
  } else {
  	//HPDF_STATUS
  	HPDF_Page_Arc(pdf_page, x, NEG(y), r, a, start);
  }
};

void Fl_Pdf_Graphics_Driver::arc(int x, int y, int w, int h, double a1, double a2) {
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  begin_line();

  HPDF_Page_Concat(pdf_page, 1,0,0,1,x + w/2.0 + 0.5, NEG(y)-h/2 + 0.5);
  HPDF_Page_Concat(pdf_page, NEG((w-1)/2.0),0,0,(h-1)/2.0,0,0);
  arc(0,0,1,a2-45,a1-45);
  end_line();

  //HPDF_STATUS
  HPDF_Page_GRestore(pdf_page);
}

void Fl_Pdf_Graphics_Driver::pie(int x, int y, int w, int h, double a1, double a2) {
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  HPDF_Page_Concat(pdf_page, 1,0,0,1,x + w/2.0 + 0.5, NEG(y)-h/2 + 0.5);
  HPDF_Page_Concat(pdf_page, NEG((w-1)/2.0),0,0,(h-1)/2.0,0,0);
  begin_polygon();
  vertex(0,0);
  arc(0,0, 1, a2-45,a1-45);
  end_polygon();
  //HPDF_STATUS
  HPDF_Page_GRestore(pdf_page);
}

void Fl_Pdf_Graphics_Driver::end_points(){
  gap_=1;
  //HPDF_STATUS
  HPDF_Page_Stroke(pdf_page);
  //HPDF_STATUS
  HPDF_Page_GRestore(pdf_page);
  shape_=NONE;
}

void Fl_Pdf_Graphics_Driver::end_line(){
  gap_=1;
  //HPDF_STATUS
  HPDF_Page_Stroke(pdf_page);
  //HPDF_STATUS
  HPDF_Page_GRestore(pdf_page);
  shape_=NONE;
}
void Fl_Pdf_Graphics_Driver::end_loop(){
  gap_=1;
  //HPDF_STATUS
  HPDF_Page_ClosePathStroke(pdf_page);
  //HPDF_STATUS
  HPDF_Page_GRestore(pdf_page);
  shape_=NONE;
}

void Fl_Pdf_Graphics_Driver::end_polygon(){
SDBG
  gap_=1;
  //HPDF_STATUS
  //HPDF_Page_FillStroke
  HPDF_Page_ClosePathFillStroke(pdf_page);
  //HPDF_STATUS
  HPDF_Page_GRestore(pdf_page);
  shape_=NONE;
}

void Fl_Pdf_Graphics_Driver::transformed_vertex(double x, double y){
SDBG
  if(gap_){
  	HPDF_Page_MoveTo(pdf_page, x, NEG(y));
    gap_=0;
  }else
  	HPDF_Page_LineTo(pdf_page, x, NEG(y));
  concat();
};

/////////////////////////////   Clipping /////////////////////////////////////////////

void Fl_Pdf_Graphics_Driver::push_clip(int x, int y, int w, int h) {
SDBG
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  HPDF_REAL line_width = HPDF_Page_GetLineWidth (pdf_page);
  /*HPDF_STATUS*/
  HPDF_Page_SetLineWidth (pdf_page, 0);
  HPDF_Page_Rectangle(pdf_page, x, NEG(y), w, NEG(h));
  HPDF_Page_Clip(pdf_page);
  HPDF_Page_EndPath(pdf_page);
  //HPDF_Page_Clip_No_Border (pdf_page);
  //HPDF_Page_Stroke (pdf_page);
  //HPDF_STATUS
  HPDF_Page_SetLineWidth (pdf_page, line_width);
}

void Fl_Pdf_Graphics_Driver::push_no_clip() {
SDBG
  //HPDF_STATUS
  HPDF_Page_GSave(pdf_page);
  /*HPDF_STATUS*/
  HPDF_Page_Rectangle(pdf_page, 0, 0, HPDF_Page_GetWidth(pdf_page), NEG(HPDF_Page_GetHeight(pdf_page)));
  HPDF_Page_Clip(pdf_page);
  HPDF_Page_EndPath(pdf_page);
  //HPDF_Page_Clip_No_Border (pdf_page);
  //HPDF_Page_Stroke (pdf_page);
}

void Fl_Pdf_Graphics_Driver::pop_clip() {
SDBG
  //if(!clip_)return;
  //HPDF_STATUS
  HPDF_Page_GRestore(pdf_page);
}

int Fl_Pdf_Graphics_Driver::clip_box(int x, int y, int w, int h, int &X, int &Y, int &W, int &H){
SDBG
  if(!clip_){
    X=x;Y=y;W=w;H=h;
    return 1;
  }
  if(clip_->w < 0){
    X=x;Y=y;W=w;H=h;
    return 1;
  }
  int ret=0;
  if (x > (X=clip_->x)) {X=x; ret=1;}
  if (y > (Y=clip_->y)) {Y=y; ret=1;}
  if ((x+w) < (clip_->x+clip_->w)) {
    W=x+w-X;

    ret=1;

  }else
    W = clip_->x + clip_->w - X;
  if(W<0){
    W=0;
    return 1;
  }
  if ((y+h) < (clip_->y+clip_->h)) {
    H=y+h-Y;
    ret=1;
  }else
    H = clip_->y + clip_->h - Y;
  if(H<0){
    W=0;
    H=0;
    return 1;
  }
  return ret;
};

int Fl_Pdf_Graphics_Driver::not_clipped(int x, int y, int w, int h){
SDBG
  if(!clip_) return 1;
  if(clip_->w < 0) return 1;
  int X, Y, W, H;
  clip_box(x, y, w, h, X, Y, W, H);
  if(W) return 1;
  return 0;
};

void Fl_Pdf_Graphics_Driver::draw_scaled_image(const uchar *data, double x, double y, double w, double h, int iw, int ih, int D, int LD){
SDBG
}

void Fl_Pdf_Graphics_Driver::draw_scaled_image_mono(const uchar *data, double x, double y, double w, double h, int iw, int ih, int D, int LD){
SDBG
}

void Fl_Pdf_Graphics_Driver::draw_scaled_image(Fl_Draw_Image_Cb call, void *data, double x, double y, double w, double h, int iw, int ih, int D){
SDBG
}

void Fl_Pdf_Graphics_Driver::draw_scaled_image_mono(Fl_Draw_Image_Cb call, void *data, double x, double y, double w, double h, int iw, int ih, int D){
SDBG
}

void Fl_Pdf_Graphics_Driver::draw(Fl_Pixmap * pxm,int XP, int YP, int WP, int HP, int cx, int cy){
SDBG
}

void Fl_Pdf_Graphics_Driver::draw(Fl_RGB_Image * rgb, int XP, int YP, int WP, int HP, int cx, int cy){
SDBG
	HPDF_Image image;
	HPDF_BYTE* newImg = NULL;
	if(rgb->d() == 4){
		const HPDF_BYTE* pimg = (const HPDF_BYTE*)*rgb->data();
		newImg = new HPDF_BYTE[WP * HP * 3];
		HPDF_BYTE* pnewImg = newImg;
		int ilw = WP*4;
		for(int ih=0; ih < HP; ih++ ){
			int ih2 = ih * ilw;
			for(int iw=0; iw < ilw; iw+=4 ) {
				int ihc = ih2+iw;
				for(int ic=0; ic < 3; ic++)
					*pnewImg++ =  pimg[ihc+ic];
			}
		}
		//HPDF_STATUS
		image = HPDF_LoadRawImageFromMem  (pdf, newImg,
			WP, HP, HPDF_CS_DEVICE_RGB, 8);

#if 0
		int vc, vm, vy, vk;
		const HPDF_BYTE* pimg = (const HPDF_BYTE*)*rgb->data();
		newImg = new HPDF_BYTE[WP * HP * 4];
		HPDF_BYTE* pnewImg = newImg;
		int ilw = WP*4;
		for(int ih=0; ih < HP; ih++ ){
			int ih2 = ih * ilw;
			for(int iw=0; iw < ilw; iw+=4 ) {
				int ihc = ih2+iw;
				vc = 1- (pimg[ihc++]/255);
				vm = 1- (pimg[ihc++]/255);
				vy = 1- (pimg[ihc++]/255);
				ihc++; //alpha
				vk = 1;
				if(vc < vk) vk = vc;
				if(vm < vk) vk = vm;
				if(vy < vk) vk = vy;
				if(vk == 1) vc = vm = vy = 0; //black
				else {
					vc = (vc - vk) / (1 - vk);
					vm = (vm - vk) / (1 - vk);
					vy = (vy - vk) / (1 - vk);
				}
				*pnewImg++ = vc;
				*pnewImg++ = vm;
				*pnewImg++ = vy;
				*pnewImg++ = vk;
			}
		}

		//HPDF_STATUS
		image = HPDF_LoadRawImageFromMem  (pdf, newImg,
			WP, HP, HPDF_CS_DEVICE_CMYK, 8);
#endif
	} else {
		//HPDF_STATUS
		image = HPDF_LoadRawImageFromMem  (pdf, (const HPDF_BYTE*)*rgb->data(),
			WP, HP, rgb->d() == 4 ? HPDF_CS_DEVICE_CMYK : HPDF_CS_DEVICE_RGB, 8);
	}
	if(image) {
		HPDF_Page_DrawImage(pdf_page, image, XP, NEG(YP-HP), WP, HP);
	}
	if(newImg) delete[] newImg;
}

void Fl_Pdf_Graphics_Driver::draw(Fl_Bitmap * bitmap,int XP, int YP, int WP, int HP, int cx, int cy){
SDBG
}

double Fl_Pdf_Graphics_Driver::width(const char* s, int n)
{
    //return HPDF_Font_TextWidth(pdf_font_, (const HPDF_BYTE*)s, n).width;
    return Fl_Display_Device::display_device()->driver()->width(s, n);
}
int Fl_Pdf_Graphics_Driver::descent()
{
    //return HPDF_Font_GetDescent (pdf_font_);
    return Fl_Display_Device::display_device()->driver()->descent();
}
int Fl_Pdf_Graphics_Driver::height()
{
    //return HPDF_Font_GetXHeight(pdf_font_);
    return Fl_Display_Device::display_device()->driver()->height();
}

void Fl_Pdf_File_Device::compress(int abool){
	Fl_Pdf_Graphics_Driver *pdf = driver();
	HPDF_SetCompressionMode (pdf->pdf, abool ? HPDF_COMP_ALL : HPDF_COMP_NONE);
}

HPDF_Doc Fl_Pdf_File_Device::pdf_doc(){
	return driver()->pdf;
}

void Fl_Pdf_File_Device::margins(int *left, int *top, int *right, int *bottom) // to implement
{
SDBG
  Fl_Pdf_Graphics_Driver *pdf = driver();
  if(left) *left = (int)(pdf->left_margin / pdf->scale_x + .5);
  if(right) *right = (int)(pdf->left_margin / pdf->scale_x + .5);
  if(top) *top = (int)(pdf->top_margin / pdf->scale_y + .5);
  if(bottom) *bottom = (int)(pdf->top_margin / pdf->scale_y + .5);
}

int Fl_Pdf_File_Device::printable_rect(int *w, int *h)
//returns 0 iff OK
{
SDBG
  Fl_Pdf_Graphics_Driver *pdf = driver();
  if(w) *w = (int)((pdf->pw_ - 2 * pdf->left_margin) / pdf->scale_x + .5);
  if(h) *h = (int)((pdf->ph_ - 2 * pdf->top_margin) / pdf->scale_y + .5);
  return 0;
}

void Fl_Pdf_File_Device::origin(int x, int y)
{
SDBG
  x_offset = x;
  y_offset = y;
//printf("origin %d, %d\n", x, y);
  translate(x,y);
  //Fl_Pdf_Graphics_Driver *pdf = driver();
  //fprintf(pdf->output, "GR GR GS %d %d TR  %f %f SC %d %d TR %f rotate GS\n",
	//  pdf->left_margin, pdf->top_margin, pdf->scale_x, pdf->scale_y, x, y, pdf->angle);
}

void Fl_Pdf_File_Device::scale (float s_x, float s_y)
{
SDBG
	Fl_Pdf_Graphics_Driver *pdf = driver();
  	HPDF_Page_Concat(pdf->pdf_page, s_x,0,0,s_y,0,0);
}

void Fl_Pdf_File_Device::rotate (float rot_angle)
{
SDBG
	float fradian;
	fradian = rot_angle / 180 * 3.141592;
	Fl_Pdf_Graphics_Driver *pdf = driver();
 	HPDF_Page_Concat (pdf->pdf_page, cos (fradian), sin (fradian), -sin (fradian), cos (fradian), 0, 0);
}

void Fl_Pdf_File_Device::translate(int x, int y)
{
SDBG
  Fl_Pdf_Graphics_Driver *pdf = driver();
  /* save the current graphics states */
  HPDF_Page_GSave(pdf->pdf_page);
  HPDF_Page_Concat(pdf->pdf_page, 1,0,0,1,x,NEG(y));
}

void Fl_Pdf_File_Device::untranslate(void)
{
SDBG
  Fl_Pdf_Graphics_Driver *pdf = driver();
  /* restore the graphics states */
  HPDF_Page_GRestore(pdf->pdf_page);
}

int Fl_Pdf_File_Device::start_page (void)
{
SDBG
	Fl_Pdf_Graphics_Driver *pdf = driver();
	//pdf->pdf_page = HPDF_AddPage (pdf->pdf);
	pdf->page();
  	return 0;
}

int Fl_Pdf_File_Device::end_page (void)
{
SDBG
  return 0;
}

void Fl_Pdf_File_Device::end_job (void)
// finishes PDF & closes file
{
SDBG
  Fl_Pdf_Graphics_Driver *pdf = driver();
  //pdf->reset();
  //HPDF_STATUS
  HPDF_STATUS result = HPDF_SaveToFile (pdf->pdf,    pdf->pdf_filename_);
  //printf("Result %f\n", result);
  //if(ferror(pdf->output)) {
  //  fl_alert ("Error during PDF data output.");
  //  }
  while (pdf->clip_){
    Fl_Pdf_Graphics_Driver::Clip * c= pdf->clip_;
    pdf->clip_= pdf->clip_->prev;
    delete c;
  }
  if (pdf->close_cmd_) (*pdf->close_cmd_)(pdf->output);
  Fl_Display_Device::display_device()->set_current();
}

#endif // FL_DOXYGEN

//
// End of "$Id: Fl_Pdf.cxx 7653 2010-06-24 08:55:04Z manolo $".
//
