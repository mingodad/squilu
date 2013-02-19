// fl_html_label 1.0
//
// Copyright 2009 by Roman Kantor.
// 
// License: LGPL + fltk exceptions, see license for fltk library at http://fltk.org
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// version 2 as published by the Free Software Foundation.
//
// This library is distributed  WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.


#ifndef _fl_html_label_h_
#define _fl_html_label_h_
#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>


// For impatient:
// Following functions are for instant use of FL_HTM_LABEL type 
// without the need to use Fl::set_labeltype() function

FL_EXPORT Fl_Labeltype fl_define_html_label(int position);
#define _FL_HTML_LABEL 15 // default position in the labeltype table
#define FL_HTML_LABEL fl_define_html_label(_FL_HTML_LABEL)


// Drawing and measurement functions for labeltypes.
// These functions should be used for the definition
// of the HTML labeltype using Fl::set_labeltype() function.

FL_EXPORT void fl_draw_html(const Fl_Label *label, int x, int y, int w, int h, Fl_Align align);
FL_EXPORT void fl_measure_html(const Fl_Label *label, int &w, int &h);

// Some usefull functions to directly draw or measure a HTML text string.
// The functions use current fl_font() and fl_size() as the base font
// parameters for formatting and fl_color() as the base text color.

FL_EXPORT void fl_draw_html(const char * html_text, int x, int y);
FL_EXPORT void fl_draw_html(const char * html_text, int x, int y, int w, int h, Fl_Align align, Fl_Image* img =0);
FL_EXPORT void fl_measure_html(const char * html_text, int &w, int &h);

// A trick to use in-memory images within html labels/strings is creating "named" shared image.
// We need to make constructor public and register the name.



Fl_Shared_Image * fl_name_image(Fl_Image * im, const char * name);


#endif // _fl_html_label_h_
