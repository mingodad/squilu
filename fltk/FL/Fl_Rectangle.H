// "$Id: Rectangle.h 5454 2006-09-19 02:38:02Z spitzak $"
//
// Copyright 1998-2006 by Bill Spitzak and others.
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
// Please report all bugs and problems to "fltk-bugs@fltk.org".

#ifndef fltk_Rectangle_h
#define fltk_Rectangle_h

#include "Enumerations.H"

// rectangle macros that help keeping rectangle predicates as strict as possible
// even when not using rectangles in some situations (as when only using w h scalars)
// so that there is only one strict defintion for common predicates,
// if one change the following, it will be repercuted in all the core lib
#define FLTK_RECT_EMPTY(w,h)        (w <= 0 || h <= 0)
// we should always use the same evaluation for center_x, center_y  in all corelib code:
//#define FLTK_CENTER_X(coord, length)  (coord + (length>>1))
//#define FLTK_CENTER_Y(coord, length)  (coord + (length>>1))

//namespace fltk {

typedef int coord_t;

class FL_EXPORT Fl_Rectangle {
 protected:
  coord_t x_, y_, w_, h_;

 public:

  /*! Left edge */
  coord_t x() const {return x_;}
  /*! Top edge */
  coord_t y() const {return y_;}
  /*! Distance between left and right edges */
  coord_t w() const {return w_;}
  /*! Distance between top and bottom edges */
  coord_t h() const {return h_;}
  /*! Return x()+w(), the right edge of the rectangle. */
  coord_t r() const {return x_+w_;}
  /*! Return y()+h(), the bottom edge of the rectangle. */
  coord_t b() const {return y_+h_;}
  /*! Move the rectangle so the left edge is at \a v. */
  void x(coord_t v) {x_ = v;}
  /*! Move the rectangle so the top edge is at \a v. */
  void y(coord_t v) {y_ = v;}
  /*! Change w() by moving the right edge. x() does not change. */
  void w(coord_t v) {w_ = v;}
  /*! Change h() by moving the bottom edge. y() does not change. */
  void h(coord_t v) {h_ = v;}
  /*! Change x() without changing r(), by changing the width. */
  void set_x(coord_t v) {w_ -= v-x_; x_ = v;}
  /*! Change y() without changing b(), by changing the height. */
  void set_y(coord_t v) {h_ -= v-y_; y_ = v;}
  /*! Change r() without changing x(), by changine the width. */
  void set_r(coord_t v) {w_ = v-x_;}
  /*! Change b() without changing y(), by changine the height. */
  void set_b(coord_t v) {h_ = v-y_;}
  /*! Set x(), y(), w(), and h() all at once. */
  void set(coord_t x, coord_t y, coord_t w, coord_t h) {x_=x; y_=y; w_=w; h_=h;}
  /*! Sets  x, y, w, h so that's it's centered or aligned (if flags!=0) inside the source r */
  void set (const Fl_Rectangle& r, coord_t w, coord_t h, coord_t flags = 0);
  /*! Add \a d to x() without changing r() (it reduces w() by \a d). */
  void move_x(coord_t d) {x_ += d; w_ -= d;}
  /*! Add \a d to y() without changing b() (it reduces h() by \a d). */
  void move_y(coord_t d) {y_ += d; h_ -= d;}
  /*! Add \a d to r() and w(). */
  void move_r(coord_t d) {w_ += d;}
  /*! Add \a d to b() and h(). */
  void move_b(coord_t d) {h_ += d;}
  /*! Move all edges in by \a d. See also Symbol::inset() */
  void inset(coord_t d) {x_ += d; y_ += d; w_ -= 2*d; h_ -= 2*d;}
  /*! Move entire rectangle by given distance in x and y. */
  void move(coord_t dx, coord_t dy) {x_ += dx; y_ += dy;}
  /*! True if w() or h() are less or equal to zero. */
  bool empty() const {return FLTK_RECT_EMPTY(w_, h_);}
  /*! Same as !empty(), true if w() and h() are both greater than zero. */
  bool not_empty() const {return  !FLTK_RECT_EMPTY(w_, h_);}
  /*! Integer center position. Rounded to the left if w() is odd. */
  coord_t center_x() const {return x_+(w_/2);}
  /*! Integer center position. Rounded to lower y if h() is odd. */
  coord_t center_y() const {return y_+(h_/2);}
  /*! Where to put baseline to center current font nicely */
  coord_t baseline_y() const;

  Fl_Rectangle() {x_ = y_ = w_ = h_ = 0;}

  /*! Constructor that sets x(), y(), w(), and h(). */
  Fl_Rectangle(coord_t x, coord_t y, coord_t w, coord_t h) : x_(x), y_(y), w_(w), h_(h) {}

  /*! Constructor that sets x() and y() to zero, and sets w() and h(). */
  Fl_Rectangle(coord_t w, coord_t h) : x_(0), y_(0), w_(w), h_(h) {}

  /*! Copy constructor. */
  Fl_Rectangle(const Fl_Rectangle& r) : x_(r.x_),y_(r.y_),w_(r.w_),h_(r.h_) {}

  /*! Constructor that calls set(). */
  Fl_Rectangle(const Fl_Rectangle& r, coord_t w, coord_t h, coord_t flags = 0) {set(r,w,h,flags);}

  /*! True if rectangle contains the pixel who's upper-left corner is at x,y */
  bool contains(coord_t x, coord_t y) const {return x>=x_ && y>=y_ && x<x_+w_ && y<y_+h_;}

  void merge(const Fl_Rectangle& r);
  void intersect(const Fl_Rectangle& r);

};

//}

#endif
