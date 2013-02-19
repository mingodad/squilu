//
// "$Id: Fl_Pack.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Packing widget for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

// Based on code by Curtis Edwards
// Group that compresses all it's children together and resizes to surround
// them on each redraw (only if box() is zero)
// Bugs: ?

#include <FL/Fl.H>
#include <FL/Fl_Pack.H>
#include <FL/fl_draw.H>

/**
  Creates a new Fl_Pack widget using the given position, size,
  and label string. The default boxtype is FL_NO_BOX.
  <P>The destructor <I>also deletes all the children</I>. This allows a
  whole tree to be deleted at once, without having to keep a pointer to
  all the children in the user code. A kludge has been done so the
  Fl_Pack and all of it's children can be automatic (local)
  variables, but you must declare the Fl_Pack<I>first</I>, so
  that it is destroyed last.
*/
Fl_Pack::Fl_Pack(int X, int Y, int W, int H,const char *l)
: Fl_Group(X, Y, W, H, l) {
  resizable(0);
  spacing_ = 0;
  with_label_ = 0;
  // type(VERTICAL); // already set like this
}

void Fl_Pack::draw() {
  int tx = x()+Fl::box_dx(box());
  int ty = y()+Fl::box_dy(box());
  int tw = w()-Fl::box_dw(box());
  int th = h()-Fl::box_dh(box());
  int rw, rh;
  int ox, oy, ow, oh;
  int current_position = horizontal() ? tx : ty;
  int maximum_position = current_position;
  uchar d = damage();
  Fl_Widget *wresizable = this->resizable();
  Fl_Widget*const* a = array();
  if (horizontal()) {
    rw = -spacing_;
    rh = th;

    for (int i = children(); i--;) {
	  Fl_Widget *o = child(i);
      if (o->visible()) {
        if (o != wresizable) {
			if(with_label_) o->get_full_size(ox, oy, ow, oh);
			else ow = o->w();
        	rw += ow;
        }
        rw += spacing_;
      }
    }
  } else {
    rw = tw;
    rh = -spacing_;

    for (int i = children(); i--;) {
      Fl_Widget *o = child(i);
      if (o->visible()) {
        if (o != wresizable) {
      	  if(with_label_) o->get_full_size(ox, oy, ow, oh);
      	  else oh = o->h();
		  rh += oh;
        }
        rh += spacing_;
      }
    }
  }
  if(wresizable) {
  	if (horizontal()) wresizable->size(tw-rw, wresizable->h());
  	else wresizable->size(wresizable->x(), th-rh);
  }
  for (int i = children(); i--;) {
    Fl_Widget* o = *a++;
    if (o->visible()) {
      int X,Y,W,H;
      if(with_label_) o->get_full_size(ox, oy, ow, oh);
      else {
      	ox = o->x();
      	oy = o->y();
      	ow = o->w();
      	oh = o->h();
	  }
      if (horizontal()) {
        X = current_position;
        W = ow;
        Y = ty;
        H = th;
      } else {
        X = tx;
        W = tw;
        Y = current_position;
        H = oh;
      }
      // Last child, if resizable, takes all remaining room
      if(i == 0 && o == wresizable) {
        if(horizontal())
          W = tw - rw;
        else
          H = th - rh;
      }
      if (spacing_ && current_position>maximum_position && box() &&
        (X != ox || Y != oy || d&FL_DAMAGE_ALL)) {
        fl_color(color());
        if (horizontal())
          fl_rectf(maximum_position, ty, spacing_, th);
        else
          fl_rectf(tx, maximum_position, tw, spacing_);
      }
      if (X != ox || Y != oy || W != ow || H != oh) {
      	if(with_label_) {
			int orx, ory, orw, orh, wdiff, hdiff;
			Fl_Align lbl_align = o->align();
			wdiff = ow - o->w();
			hdiff = oh - o->h();
			orx = X;
			ory = Y;
			orw = W;
			orh = H;
			if (lbl_align & FL_ALIGN_BOTTOM) {
			  orh -= hdiff;
			} else if (lbl_align & FL_ALIGN_TOP) {
			  orh -= hdiff;
			  ory += hdiff;
			} else if (lbl_align & FL_ALIGN_LEFT) {
			  orx += wdiff;
			  orw -= wdiff;
			  orh -= hdiff; // just in case label font size is bigger than widget
			} else if (lbl_align & FL_ALIGN_RIGHT) {
			  orw -= wdiff;
			  orh -= hdiff; // just in case label font size is bigger than widget
			}
			//o->resize(X+wdiff,Y+hdiff, W-wdiff - woffset, H-hdiff - hoffset);
			o->resize(orx, ory, orw, orh);
      	} else o->resize(X,Y, W, H);
        o->clear_damage(FL_DAMAGE_ALL);
      }
      if (d&FL_DAMAGE_ALL) {
        draw_child(*o);
        draw_outside_label(*o);
      } else update_child(*o);
      // child's draw() can change it's size, so use new size:
      if(with_label_) o->get_full_size(ox, oy, ow, oh);
      else {
      	ox = o->x();
      	oy = o->y();
      	ow = o->w();
      	oh = o->h();
	  }
      current_position += (horizontal() ? ow : oh);
      if (current_position > maximum_position)
        maximum_position = current_position;
      current_position += spacing_;
    }
  }

  if (horizontal()) {
    if (maximum_position < tx+tw && box()) {
      fl_color(color());
      fl_rectf(maximum_position, ty, tx+tw-maximum_position, th);
    }
    tw = maximum_position-tx;
  } else {
    if (maximum_position < ty+th && box()) {
      fl_color(color());
      fl_rectf(tx, maximum_position, tw, ty+th-maximum_position);
    }
    th = maximum_position-ty;
  }

  tw += Fl::box_dw(box()); if (tw <= 0) tw = 1;
  th += Fl::box_dh(box()); if (th <= 0) th = 1;
  if (tw != w() || th != h()) {
    Fl_Widget::resize(x(),y(),tw,th);
    d = FL_DAMAGE_ALL;
  }
  if (d&FL_DAMAGE_ALL) {
    draw_box();
    draw_label();
  }
}

//
// End of "$Id: Fl_Pack.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
