/* BurrTools
 *
 * BurrTools is the legal property of its developers, whose
 * names are listed in the COPYRIGHT file, which is included
 * within the source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef __FL_LAYOUTER_H__
#define __FL_LAYOUTER_H__

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>

class Fl_Box;
class Fl_Button;
class Fl_Check_Button;
class Fl_Round_Button;
class Fl_Radio_Button;
class Fl_Slider;
class Fl_Choice;
class Fl_Input;
class Fl_Output;
class Fl_Int_Input;
class Fl_Float_Input;
class Fl_Value_Output;
class Fl_Value_Input;
class Fl_Value_Slider;
class Fl_Roller;
class Fl_Menu_Bar;
class Fl_Progress;
class Fl_Scroll;
class Fl_Tabs;
class Fl_Widget;
class Fl_Int_Vector;

// this class is used to calculate the minimal size of a widget
class layoutMeasurer_c
{
  public:

    /* the following function is for the layouter to find out how much space
     * the widget requires at least
     */
    virtual void getMinSize(const Fl_Widget * w, int *width, int *height) const = 0;
};

// a layout measurer class, where minsize returns a constant value
class layoutMeasurerConst_c : public layoutMeasurer_c
{
  private:
    int minw, minh;

  public:
    layoutMeasurerConst_c(int mw, int mh) : minw(mw), minh(mh) {}

    virtual void getMinSize(const Fl_Widget * /*w*/, int *width, int *height) const
    {
      *width = minw;
      *height = minh;
    }
};

// a layout measurer class, that measures the size of the label and adds an constant offset
class layoutMeasurerPad_c : public layoutMeasurer_c
{
  private:
    int padw, padh;

  public:
    layoutMeasurerPad_c(int pw, int ph) : padw(pw), padh(ph) {}

    virtual void getMinSize(const Fl_Widget * w, int *width, int *height) const;
};

// a layout measurer class, where minsize returns a value proportional to font size
class layoutMeasurerFont_c : public layoutMeasurer_c
{
  private:
    int minw, minh;

  public:
    layoutMeasurerFont_c(int mw, int mh) : minw(mw), minh(mh) {}

    virtual void getMinSize(const Fl_Widget * w, int *width, int *height) const
    {
      *width = minw;
	  if(w->textsize() < minh) {
		*height = w->textsize() + 6;
		((Fl_Widget *) w)->size(w->w(), minh);
	  }
      else *height = minh;
    }
};

// this class is the group that actually layouts its subwidgets according to the layouting information
// provided with each widget
class layouter_c : public Fl_Group {

  private:

    bool minsizeValid;
    int mw, mh;

  public:

  void calcLayout(int task, Fl_Int_Vector *widths, Fl_Int_Vector *heights,
                  Fl_Int_Vector *widgetsW, Fl_Int_Vector *widgetsH,
                  int targetW = 0, int tagetH = 0) const;

  virtual void resize(int x, int y, int w, int h);

  void getMinSize(int *width, int *height) const;

  layouter_c(int x, int y, int w, int h) : Fl_Group(x, y, w, h), minsizeValid(false) {}

  void remove(Fl_Widget &w);
  void remove(Fl_Widget *w);
  void add(Fl_Widget &w);
  void add(Fl_Widget *w);
};

// the measurer class that should be attached to the layouter
class layoutMeasurerLayouter_c : public layoutMeasurer_c
{
  public:
    layoutMeasurerLayouter_c(void) {}

    virtual void getMinSize(const Fl_Widget * w, int *width, int *height) const
    {
      ((layouter_c*)w)->getMinSize(width, height);
    }
};

// a tabs class, that uses layoutable subwidgets
class LFl_Tabs : public Fl_Tabs {

public:

  LFl_Tabs(int x, int y, int w, int h);
  void getMinSize(int *width, int *height) const;
  void resize(int x, int y, int w, int h);
};


/* window class that alread contains a layouter, and all further added widgets
 * will automatically go into this layouter...
 */
class LFl_Double_Window : public Fl_Double_Window {

  layouter_c * lay;
  bool res;
  bool placed;

  public:

  LFl_Double_Window(bool resizab) : Fl_Double_Window(10, 10), res(resizab), placed(false) {
    lay = new layouter_c(0, 0, 10, 10);
    resizable(lay);
  }

  void show(void);

  void resize(int x, int y, int w, int h) {
    placed = true;
    Fl_Double_Window::resize(x, y, w, h);
  }

  void begin() {
    lay->begin();
  }
};


// functions to access and change layouting information for a layoutable
// widget. If the widget is not suitable for layouting these function do nothing

// how is a widget supposed to behave, when the available space
// is bigger than required
typedef enum {
  STRETCH_MINUS, STRETCH_PLUS, STRETCH_MIDDLE, STRETCH, STRETCH_SQUARE
} stretch_t;

class Fl_Layout_Utils {
	public:
	/* Some functions that return a widget, that are suitable for layouting.
	 * Those are mainly tool functions to make it easier to construct the widgets
	 * the gevne values for x, y, w, h are no longer pixel positions but grid positions
	 */

	Fl_Box * getNewBox(const char *txt, int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Box * getNewBox(int x = 0, int y = 0, int w = 1, int h = 1);
	layouter_c * getNewFrame(int x = 0, int y = 0, int w = 1, int h = 1);
	layouter_c * getNewLayouter(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Button * getNewButton(const char * text, int x = 0, int y = 0, int w = 1, int h = 1, int padX=4, int padY=4);
	Fl_Check_Button * getNewCheckButton(const char * text, int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Round_Button * getNewRoundButton(const char *text, int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Round_Button * getNewRadioButton(const char *text, int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Slider * getNewSlider(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Choice * getNewChoice(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Input * getNewInput(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Output * getNewOutput(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Int_Input * getNewIntInput(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Float_Input * getNewFloatInput(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Value_Output * getNewValueOutput(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Value_Input * getNewValueInput(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Value_Slider * getNewValueSlider(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Roller * getNewRoller(int x = 0, int y = 0, int w = 1, int h = 1);
	Fl_Menu_Bar * getNewMenuBar(int x, int y, int w, int h);
	Fl_Progress * getNewProgress(int x, int y, int w, int h);
	Fl_Scroll * getNewScroll(int x, int y, int w, int h);

	// functions to access layouting information attached to a widget
	// the get functions, when applied to a widget without layouting information
	// will simply return the default value
	// when setter functions are called on a widget without layouting information
	// nothing is done, this is because the layouting information is right now specialized on
	// the widget until Fltk includes a get minimal size functions into the widget class

	int getMinWidth(const Fl_Widget * w);
	int getMinHeight(const Fl_Widget * w);
	unsigned char getPitch(const Fl_Widget * w);
	unsigned char getWeightX(const Fl_Widget * w);
	unsigned char getWeightY(const Fl_Widget * w);
	stretch_t getStretchX(const Fl_Widget * w);
	stretch_t getStretchY(const Fl_Widget * w);
	void getGridValues(const Fl_Widget * wi, unsigned int *x, unsigned int *y, unsigned int *w, unsigned int *h);
	void setGridValues(const Fl_Widget * wi, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
	void setLayoutParameter(const Fl_Widget * w, stretch_t stretchX, stretch_t stretchY, unsigned char pitch, unsigned char weightX, unsigned char weightY);
	void pitch(const Fl_Widget * w, unsigned char pitch);
	void stretchLeft(const Fl_Widget * w);
	void stretchRight(const Fl_Widget * w);
	void stretchTop(const Fl_Widget * w);
	void stretchBottom(const Fl_Widget * w);
	void stretchSquare(const Fl_Widget * w);
	void stretchHCenter(const Fl_Widget * w);
	void stretchVCenter(const Fl_Widget * w);
	void stretchCenter(const Fl_Widget * w);
	void stretch(const Fl_Widget * w);
	void weight(const Fl_Widget * w, unsigned char x, unsigned char y);
	void setMinimumSize(const Fl_Widget * w, unsigned int minWidth, unsigned int minHeight);
	void setMeasureClass(const Fl_Widget * w, layoutMeasurer_c * m);

	void getMinSize(const Fl_Widget * wi, int * w, int * h);

	bool hasLayoutInfo(const Fl_Widget * w);
	void removeLayoutInfo(const Fl_Widget * w);
};

#endif
