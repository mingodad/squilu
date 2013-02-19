// $Id: Flu_Progress.cpp,v 1.14 2005/02/14 02:16:47 jbryan Exp $

/***************************************************************
 *                FLU - FLTK Utility Widgets
 *  Copyright (C) 2002 Ohio Supercomputer Center, Ohio State University
 *
 * This file and its content is protected by a software license.
 * You should have received a copy of this license with this file.
 * If not, please contact the Ohio Supercomputer Center immediately:
 * Attn: Jason Bryan Re: FLU 1224 Kinnear Rd, Columbus, Ohio 43212
 *
 ***************************************************************/



#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <stdlib.h>
#include <FL/math.h>

#include "FLU/Flu_Progress.H"

class _FlWindowFlush : public Fl_Window
{
public:
  _FlWindowFlush() : Fl_Window(0,0,0,0) {}
  inline void flush() { Fl_Window::flush(); }
};

class _FlDoubleWindowFlush : public Fl_Double_Window
{
public:
  _FlDoubleWindowFlush() : Fl_Double_Window(0,0,0,0) {}
  inline void flush() { Fl_Double_Window::flush(); }
};

Flu_Progress :: Flu_Progress( int X, int Y, int W, int H, const char* l )
  : Fl_Valuator( X, Y, W, H, l )
{
  box( FL_THIN_DOWN_BOX );
  align( FL_ALIGN_LEFT );
  color( FL_WHITE );
  selection_color( FL_BLUE );
  precision( 2 );
  range( 0, 1 );
  Fl_Valuator::value( 0 );
}

Flu_Progress :: ~Flu_Progress()
{
}

int Flu_Progress :: value( float v )
{
  int lastPercent = int(100.0 * (value() - minimum()) / (maximum()-minimum()) );
  int thisPercent = int(100.0 * (v - minimum()) / (maximum()-minimum()) );
  int ret = Fl_Valuator::value( v );
  if( thisPercent != lastPercent && window() && window()->shown() )
    Fl::check();
  return ret;
}

void Flu_Progress :: draw()
{
  int dx = Fl::box_dx( box() ), dy = Fl::box_dy( box() ),
    dw = Fl::box_dw( box() ), dh = Fl::box_dh( box() );
  float percent = (value() - minimum()) / (maximum()-minimum());
  int val1 = int( float(w()-dw)*percent ), val2 = w()-dw - val1;

  char dummy[8];
  const char *buf;
  if( label() && (align() | FL_ALIGN_INSIDE) )
    buf = label();
  else
    {
      sprintf( dummy, "%d%%", int(percent*100.0) );
      buf = dummy;
    }

  fl_font( FL_HELVETICA_BOLD, h()/2+2 );
  int fW = 0, fH;
  fl_measure( buf, fW, fH );

  fl_draw_box( box(), x(), y(), w(), h(), color() );
  fl_color( active_r() ? selection_color() : fl_inactive(selection_color()) );
  fl_rectf( x()+dx, y()+dy, val1, h()-dh );
  fl_push_clip( x()+dx+val1, y()+dy, val2, h()-dh );
  fl_draw( buf, x()+w()/2-(fW>>1), y()+h()/2-(fH>>1), fW, fH, FL_ALIGN_CENTER );
  fl_pop_clip();

  fl_color( color() );
  fl_push_clip( x()+dx, y()+dy, val1, h()-dh );
  fl_draw( buf, x()+w()/2-(fW>>1), y()+h()/2-(fH>>1), fW, fH, FL_ALIGN_CENTER );
  fl_pop_clip();

  // draw the label
  if( label() && !(align() | FL_ALIGN_INSIDE) )
  draw_label();
}
