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

// Include the Fl_Help_View headers first to avoid messing the definitions with the hack below:

#  include <stdio.h>
#  include <FL/Fl.H>
#  include <FL/Fl_Group.H>
#  include <FL/Fl_Scrollbar.H>
#  include <FL/fl_draw.H>
#  include <FL/Fl_Shared_Image.H>
#  include <string.h>
#  include <stdlib.h>

// Hack to access private members
// Warning! It might not work with some compilers

# define enum friend class  Fl_Html_Label_Helper; enum
#include <FL/Fl_Help_View.H>
#undef enum


static char extra_tag[] = "<br>";
static class Fl_Html_Label_Helper: private Fl_Help_View{


  // size_ does not give very good value, we have to use blocks to calculate it
  int height() const {
    int s = size_;
    s -=textsize();
    return s;
  }

  int set_value(const char * v){
    if(value_) return 0;
    int len = 0;
    // we first call formaton new data and later free old ones to avoid reloading the same images
    if(v){
      len = strlen(v);
      value_ = new char[strlen(v) + sizeof(extra_tag) + 1];
      strcpy((char *)value_, v);
      strcpy((char *)value_ + len, extra_tag);
      format();
    }else
      value_ = 0;

    set_changed();
    return 1;

  }

public:

  void measure(const Fl_Label& l, int& w, int&h){
    textfont(l.font);
    textcolor(Fl_Color(l.color));
    textsize(l.size);

    int st = set_value(l.value);

    int iw = 0;//w; // minimum size
    int th = 0;
    if(l.image){
      if(l.image->w()>iw)
        iw = l.image->w();
      th = l.image->h();
    }
    Fl_Widget::w(iw + Fl::scrollbar_size()); // set initial width to zero (excluding scrollbar) for formatting but allow for image width
    Fl_Widget::h(10000);
    format();
    w = hsize_;
    h = th + height();
    if(st){
      delete[] value_;
      value_ = 0;
    }
  }


  void draw(const Fl_Label& l, int X, int Y, int W, int H, int align, bool do_align = 1){
    int tw = 0;
    int th;
    set_value(l.value);
    measure(l, tw, th);

    if(align & FL_ALIGN_BOTTOM)
      Y += H - th;
    else if(!(align & FL_ALIGN_TOP))
      Y += (H - th)/2;
    if(l.image){ // drawing image
      int ix = X;
      int iy = Y;
      if (align & FL_ALIGN_LEFT){
        if(align & FL_ALIGN_RIGHT) // centering
          ix += (W - l.image->w()) /2;
      }else if(align & FL_ALIGN_RIGHT)
        ix += (W - l.image->w());
      else
         ix += (W - l.image->w()) /2;
      if(align & FL_ALIGN_TEXT_OVER_IMAGE)
        iy += hsize_;
      else
        Y += l.image->h();
      l.image->draw(ix, iy);

    }
    if(do_align){
      if(align & FL_ALIGN_RIGHT){
        if(align & FL_ALIGN_LEFT) // center
          X += (W - hsize_)/2;
        else
          X += (W - hsize_);
      }else if(!(align & FL_ALIGN_LEFT)) // center
          X += (W - hsize_)/2;
    }
      

    // resize the widget INCLUDING scrollbart for proper drawing and avoiding clipping
    int ss = Fl::scrollbar_size();
    Fl_Widget::w(hsize_ + ss);
    Fl_Widget::h(size() + ss);
    Fl_Widget::x(X);
    Fl_Widget::y(Y);

    // hide scrollbars
    scrollbar_.hide();
    hscrollbar_.hide();
    fl_font(l.font, l.size);
    fl_color(l.color);
    Fl_Help_View::draw(); // here we go
    delete[] value_;
    value_ = 0;


  }
  Fl_Html_Label_Helper():Fl_Help_View(0, 0, Fl::scrollbar_size(), Fl::scrollbar_size()){
     Fl_Group * g = Fl_Group::current();
     if(g) g->remove(*this); // avoids inclusion in current group
     // Ugly: HFl_Help_View overrised FL_NO_BOX with FL_DOWN_BOX. We have to use some other boxtype, here 255
     Fl::set_boxtype(Fl_Boxtype(255), FL_NO_BOX);
     box(Fl_Boxtype(255));
  }
  ~Fl_Html_Label_Helper(){
        value_ = 0; // avoids destruction of the string
  }

} html_helper;

// drawing & measurement functions:

void fl_draw_html(const Fl_Label *label, int x, int y, int w, int h, Fl_Align align) {
  html_helper.draw(*label, x, y, w, h, align);
}

void fl_measure_html(const Fl_Label *label, int &w, int &h){
  html_helper.measure(*label, w, h);
}

Fl_Labeltype fl_define_html_label(int which){
  Fl::set_labeltype(Fl_Labeltype(which), &fl_draw_html, &fl_measure_html);
  return Fl_Labeltype(which);
}

void fl_draw_html(const char * html_text, int x, int y, int w, int h, Fl_Align align, Fl_Image* img){
  Fl_Label l;
  l.value =  html_text;
  l.font = fl_font();
  l.size = fl_size();
  l.color = fl_color();
  l.image = img;
  html_helper.draw(l, x, y, w, h, align);
}

void fl_draw_html(const char * html_text, int x, int y){
  Fl_Label l;
  l.value =  html_text;
  l.font = fl_font();
  l.size = fl_size();
  l.color = fl_color();
  l.image = 0;
  html_helper.draw(l, x, y, 0, 0, 0, 0);
}

void fl_measure_html(const char * html_text, int &w, int &h){
  Fl_Label l;
  l.value =  html_text;
  l.font = fl_font();
  l.size = fl_size();
  l.image = 0;
  w = 0;
  html_helper.measure(l, w, h);
}
class Fl_Named_Image:public Fl_Shared_Image{
public:
  Fl_Named_Image(const char * name, Fl_Image *im):Fl_Shared_Image(name, im){add();}
};

Fl_Shared_Image * fl_name_image(Fl_Image * im, const char * name){
  Fl_Shared_Image * i = Fl_Shared_Image::find(name);
  if(i) return i;
  return new Fl_Named_Image(name, im);
}



