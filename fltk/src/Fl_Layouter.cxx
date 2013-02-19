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
#include <FL/Fl_Layouter.H>
#include <FL/Fl_WidgetAssociation.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Roller.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Progress.H>

static Fl_Layout_Utils lay_util;

// this stucture is attatched to a widget so that it
// can be layouted, it contains all information for the
// proper widget placement
class layoutable_c {

public:

  /* these are the grid positions of the widgets for the layouter */
  unsigned int gridX, gridY, gridW, gridH;

  /* what to do with the widget, if the space is bigger than the widget
  * requested: _MINUS put it on the upper or left edge, _PLUS put it on
  * the lower or right edge, _MIDDLE put it in the middle, STRETCH stretch
  * it to the available size
  * stretch _SQUARE takes the smaller size in the two dimensions and applies
  * this size to both (width and height)
  */
  stretch_t stretchX, stretchY;

  /* surround the widget by pitch pixel */
  unsigned char pitch;

  /* if space is left the available pixels will be distributed weighted by
   * these values
   */
  unsigned char weightX, weightY;

  /* if you want to enforce a size for the widget independent of the size
   * the widget itself requests, use these 2 values
   */
  int minWidth, minHeight;

  layoutMeasurer_c * measure;

  layoutable_c(int gx = 0, int gy = 0, int gw = 1, int gh = 1) : gridX(gx), gridY(gy), gridW(gw), gridH(gh), stretchX(STRETCH), stretchY(STRETCH), pitch(0), weightX(0), weightY(0), minWidth(0), minHeight(0), measure(0) {}

};


class LayoutableAssociationType : public Fl_Association_Type
{
  virtual void destroy(void * data) const
  {
    layoutable_c * d = (layoutable_c*)(data);

    if (d->measure) delete d->measure;

    delete d;
  }
};

static LayoutableAssociationType layoutableAssociationType;

// functions to access layouting information attached to a widget
// the get functions, when applied to a widget without layouting information
// will simply return the default value

int Fl_Layout_Utils::getMinWidth(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (l)
  {
    return l->minWidth;
  }
  else
  {
    return 0;
  }
}
int Fl_Layout_Utils::getMinHeight(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (l)
  {
    return l->minHeight;
  }
  else
  {
    return 0;
  }
}
unsigned char Fl_Layout_Utils::getPitch(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (l)
  {
    return l->pitch;
  }
  else
  {
    return 0;
  }
}
unsigned char Fl_Layout_Utils::getWeightX(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (l)
  {
    return l->weightX;
  }
  else
  {
    return 0;
  }
}
unsigned char Fl_Layout_Utils::getWeightY(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (l)
  {
    return l->weightY;
  }
  else
  {
    return 0;
  }
}
stretch_t Fl_Layout_Utils::getStretchX(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (l)
  {
    return l->stretchX;
  }
  else
  {
    return STRETCH;
  }
}
stretch_t Fl_Layout_Utils::getStretchY(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (l)
  {
    return l->stretchY;
  }
  else
  {
    return STRETCH;
  }
}

void Fl_Layout_Utils::getGridValues(const Fl_Widget * wi, unsigned int *x, unsigned int *y, unsigned int *w, unsigned int *h)
{
  layoutable_c * l = (layoutable_c*)wi->get_assoc(layoutableAssociationType);

  if (l)
  {
    *x = l->gridX;
    *y = l->gridY;
    *w = l->gridW;
    *h = l->gridH;
  }
  else
  {
    *x = 0;
    *y = 0;
    *w = 0;
    *h = 0;
  }
}

static layoutable_c * getLayoutable(const Fl_Widget * w)
{
  layoutable_c * l = (layoutable_c*)w->get_assoc(layoutableAssociationType);

  if (!l)
  {
    l = new layoutable_c();
    w->set_assoc(layoutableAssociationType, l);
  }

  return l;
}

void Fl_Layout_Utils::setMeasureClass(const Fl_Widget * w, layoutMeasurer_c * m)
{
  layoutable_c * l = getLayoutable(w);

  if (l->measure)
    delete l->measure;

  l->measure = m;
}

void Fl_Layout_Utils::setGridValues(const Fl_Widget * wi, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
  layoutable_c * l = getLayoutable(wi);

  l->gridX = x;
  l->gridY = y;
  l->gridW = w;
  l->gridH = h;
}

void Fl_Layout_Utils::setLayoutParameter(const Fl_Widget * w, stretch_t stretchX, stretch_t stretchY, unsigned char pitch, unsigned char weightX, unsigned char weightY)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchX = stretchX;
  l->stretchY = stretchY;
  l->pitch = pitch;
  l->weightX = weightX;
  l->weightY = weightY;
}

void Fl_Layout_Utils::pitch(const Fl_Widget * w, unsigned char pitch)
{
  layoutable_c * l = getLayoutable(w);

  l->pitch = pitch;
}
void Fl_Layout_Utils::stretchLeft(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchX = STRETCH_MINUS;
}
void Fl_Layout_Utils::stretchRight(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchX = STRETCH_PLUS;
}
void Fl_Layout_Utils::stretchTop(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchY = STRETCH_MINUS;
}
void Fl_Layout_Utils::stretchBottom(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchY = STRETCH_PLUS;
}
void Fl_Layout_Utils::stretchSquare(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchX = l->stretchY = STRETCH_SQUARE;
}
void Fl_Layout_Utils::stretchHCenter(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchX = STRETCH_MIDDLE;
}
void Fl_Layout_Utils::stretchVCenter(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchY = STRETCH_MIDDLE;
}
void Fl_Layout_Utils::stretchCenter(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchX = l->stretchY = STRETCH_MIDDLE;
}
void Fl_Layout_Utils::stretch(const Fl_Widget * w)
{
  layoutable_c * l = getLayoutable(w);

  l->stretchX = l->stretchY = STRETCH;
}
void Fl_Layout_Utils::weight(const Fl_Widget * w, unsigned char x, unsigned char y)
{
  layoutable_c * l = getLayoutable(w);

  l->weightX = x;
  l->weightY = y;
}

void Fl_Layout_Utils::setMinimumSize(const Fl_Widget * w, unsigned int minWidth, unsigned int minHeight)
{
  layoutable_c * l = getLayoutable(w);

  l->minWidth = minWidth;
  l->minHeight = minHeight;
}

void Fl_Layout_Utils::getMinSize(const Fl_Widget * wi, int * w, int * h)
{
  layoutable_c * l = (layoutable_c*)wi->get_assoc(layoutableAssociationType);

  if (l)
  {
    if (l->measure)
    {
      l->measure->getMinSize(wi, w, h);
      return;
    }
  }

  *w = *h = 0;
}

bool Fl_Layout_Utils::hasLayoutInfo(const Fl_Widget * w)
{
  return w->get_assoc(layoutableAssociationType) != 0;
}

void Fl_Layout_Utils::removeLayoutInfo(const Fl_Widget * w)
{
  w->remove_assoc(layoutableAssociationType);
}

void layoutMeasurerPad_c::getMinSize(const Fl_Widget * w, int *width, int *height) const
{
  *width = 0;
  ((Fl_Widget *)w)->measure_label(*width, *height);
  *width += padw;
  *height += padh;
}


class layoutMeasurerSlider_c : public layoutMeasurer_c
{
  private:

    int length, cross;

  public:
    layoutMeasurerSlider_c(int l, int c) : length(l), cross(c) {}

    virtual void getMinSize(const Fl_Widget * w, int *width, int *height) const
    {
      switch (((Fl_Slider*)w)->type()) {
        case FL_VERTICAL:
        case FL_VERT_FILL_SLIDER:
        case FL_VERT_NICE_SLIDER:
          *width = cross;
          *height = length;
          break;
        case FL_HORIZONTAL:
        case FL_HOR_FILL_SLIDER:
        case FL_HOR_NICE_SLIDER:
          *width = length;
          *height = cross;
          break;
      }
    }
};

Fl_Box * getNewBox(const char *txt, int x, int y, int w, int h)
{
  Fl_Box * ww = new Fl_Box(0, 0, 0, 0, txt);
  lay_util.setGridValues(ww, x, y, w, h);
  lay_util.setMeasureClass(ww, new layoutMeasurerPad_c(0, 0));
  return ww;
}

Fl_Box * getNewBox(int x, int y, int w, int h)
{
  Fl_Box * ww = new Fl_Box(0, 0, 0, 0);
  lay_util.setGridValues(ww, x, y, w, h);
  lay_util.setMeasureClass(ww, new layoutMeasurerPad_c(0, 0));
  return ww;
}

layouter_c * Fl_Layout_Utils::getNewFrame(int x, int y, int w, int h)
{
  layouter_c * ww = getNewLayouter(x, y, w, h);
  ww->box(FL_ENGRAVED_BOX);
  pitch(ww, 1);

  layouter_c * l2 = getNewLayouter(0, 0, 1, 1);
  pitch(l2, 3);

  return ww;
}

layouter_c * Fl_Layout_Utils::getNewLayouter(int x, int y, int w, int h)
{
  layouter_c * ww = new layouter_c(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerLayouter_c());
  return ww;
}

Fl_Button * Fl_Layout_Utils::getNewButton(const char * txt, int x, int y, int w, int h, int padX, int padY)
{
  Fl_Button * ww = new Fl_Button(0, 0, 0, 0, txt);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerPad_c(padX, padY));
  return ww;
}

Fl_Check_Button * Fl_Layout_Utils::getNewCheckButton(const char * txt, int x, int y, int w, int h)
{
  Fl_Check_Button * ww = new Fl_Check_Button(0, 0, 0, 0, txt);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerPad_c(20, 4));
  return ww;
}

Fl_Round_Button * Fl_Layout_Utils::getNewRoundButton(const char *txt, int x, int y, int w, int h)
{
  Fl_Round_Button * ww = new Fl_Round_Button(0, 0, 0, 0, txt);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerPad_c(18, 4));
  return ww;
}

Fl_Round_Button * Fl_Layout_Utils::getNewRadioButton(const char *txt, int x, int y, int w, int h)
{
  Fl_Round_Button * ww = new Fl_Round_Button(0, 0, 0, 0, txt);
  ww->type(FL_RADIO_BUTTON);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerPad_c(18, 4));
  return ww;
}

Fl_Slider * Fl_Layout_Utils::getNewSlider(int x, int y, int w, int h)
{
  Fl_Slider * ww = new Fl_Slider(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerSlider_c(30, 15));
  return ww;
}

Fl_Choice * Fl_Layout_Utils::getNewChoice(int x, int y, int w, int h)
{
  Fl_Choice * ww = new Fl_Choice(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerFont_c(30, 20));
  stretchVCenter(ww);
  return ww;
}

Fl_Input * Fl_Layout_Utils::getNewInput(int x, int y, int w, int h)
{
  Fl_Input * ww = new Fl_Input(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerFont_c(30, 20));
  stretchVCenter(ww);
  return ww;
}

Fl_Output * Fl_Layout_Utils::getNewOutput(int x, int y, int w, int h)
{
  Fl_Output * ww = new Fl_Output(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerFont_c(30, 20));
  stretchVCenter(ww);
  return ww;
}

Fl_Int_Input * Fl_Layout_Utils::getNewIntInput(int x, int y, int w, int h)
{
  Fl_Int_Input * ww = new Fl_Int_Input(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerFont_c(30, 20));
  stretchVCenter(ww);
  return ww;
}

Fl_Float_Input * Fl_Layout_Utils::getNewFloatInput(int x, int y, int w, int h)
{
  Fl_Float_Input * ww = new Fl_Float_Input(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerFont_c(30, 20));
  stretchVCenter(ww);
  return ww;
}

Fl_Value_Output * Fl_Layout_Utils::getNewValueOutput(int x, int y, int w, int h)
{
  Fl_Value_Output * ww = new Fl_Value_Output(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerFont_c(30, 20));
  return ww;
}

Fl_Value_Input * Fl_Layout_Utils::getNewValueInput(int x, int y, int w, int h)
{
  Fl_Value_Input * ww = new Fl_Value_Input(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerFont_c(30, 20));
  return ww;
}

Fl_Value_Slider * Fl_Layout_Utils::getNewValueSlider(int x, int y, int w, int h)
{
  Fl_Value_Slider * ww = new Fl_Value_Slider(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerSlider_c(30, 20));
  return ww;
}

Fl_Roller * Fl_Layout_Utils::getNewRoller(int x, int y, int w, int h)
{
  Fl_Roller * ww = new Fl_Roller(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerConst_c(30, 20));
  return ww;
}

Fl_Menu_Bar * Fl_Layout_Utils::getNewMenuBar(int x, int y, int w, int h)
{
  Fl_Menu_Bar * ww = new Fl_Menu_Bar(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerConst_c(30, 25));
  return ww;
}

Fl_Progress * Fl_Layout_Utils::getNewProgress(int x, int y, int w, int h)
{
  Fl_Progress * ww = new Fl_Progress(0, 0, 0, 0);
  setGridValues(ww, x, y, w, h);
  setMeasureClass(ww, new layoutMeasurerConst_c(20, 20));
  return ww;
}

// 2 subclasses for the scoller and the tabs with some special functionality
class LFl_Scroll : public Fl_Scroll {

  private:

    layouter_c * lay;

  public:

    LFl_Scroll(void) : Fl_Scroll(0, 0, 100, 100) {

      lay = new layouter_c(0, 0, 100, 100);
      resizable(lay);
    }

    void getMinSize(int *width, int *height) const {

      lay->getMinSize(width, height);

      switch(type()) {
        case Fl_Scroll::HORIZONTAL_ALWAYS: // - Horizontal scrollbar always on, vertical always off.
        case Fl_Scroll::HORIZONTAL:// - Only a horizontal scrollbar.
          *width = 30;
          *height += hscrollbar.h();
          break;
        case Fl_Scroll::VERTICAL_ALWAYS: // - Vertical scrollbar always on, horizontal always off.
        case Fl_Scroll::VERTICAL:// - Only a vertical scrollbar.
          *height = 30;
          *width += scrollbar.w();
          break;
        case Fl_Scroll::BOTH_ALWAYS: // - Both always on.
        case Fl_Scroll::BOTH: // - The default is both scrollbars.
          *width = *height = 30;
          break;
      }
    }

    void resize(int x, int y, int w, int h) {

      int lw, lh;

      lay->getMinSize(&lw, &lh);

      lay->resize(lay->x(), lay->y(), lw, lh);

      Fl_Scroll::resize(x, y, w, h);
    }

};

class layoutMeasurerScroll_c : public layoutMeasurer_c
{

  public:
    layoutMeasurerScroll_c(void) {}

    virtual void getMinSize(const Fl_Widget * w, int *width, int *height) const
    {
      ((LFl_Scroll*)w)->getMinSize(width, height);
    }
};

class layoutMeasurerTabs_c : public layoutMeasurer_c
{

  public:
    layoutMeasurerTabs_c() {}

    virtual void getMinSize(const Fl_Widget * w, int *width, int *height) const
    {
      ((LFl_Tabs*)w)->getMinSize(width, height);
    }
};

LFl_Tabs::LFl_Tabs(int x, int y, int w, int h) : Fl_Tabs(0, 0, 100, 100)
{
  lay_util.setGridValues(this, x, y, w, h);
  lay_util.setMeasureClass(this, new layoutMeasurerTabs_c());
}

void LFl_Tabs::getMinSize(int *width, int *height) const {

  Fl_Widget *const * _widgets = array();
  *height = *width = 10;

  for (int i = 0; i < children(); i++) {

    int w, h;
    lay_util.getMinSize(_widgets[i], &w, &h);

    w += 2 * lay_util.getPitch(_widgets[i]);
    h += 2 * lay_util.getPitch(_widgets[i]);

    if (w > *width) *width = w;
    if (h > *height) *height = h;
  }

  *height += 20;
}

void LFl_Tabs::resize(int x, int y, int w, int h) {

  Fl_Tabs::resize(x, y, w, h);

  Fl_Widget *const * _widgets = array();

  for (int i = 0; i < children(); i++) {

    unsigned int p = lay_util.getPitch(_widgets[i]);

    _widgets[i]->resize(x+p, y+20+p, w-2*p, h-20-2*p);
  }
}


Fl_Scroll * getNewScroll(int x, int y, int w, int h)
{
  Fl_Scroll * ww = new LFl_Scroll();

  lay_util.setGridValues(ww, x, y, w, h);
  lay_util.setMeasureClass(ww, new layoutMeasurerScroll_c());

  return ww;
}


/* task      what we want to do with the endresult
 * widths    the widths of the different columns of the endgrid
 * heights   the heights of the rows in the calculated grid
 * widgetsW  the minimum widths calculated for each widget
 * widgetsH  the minimum heights for each widget
 * targetW and targetH only used when task = 1, it should be bigger than min size
 *      or strange thing may happen
 */
void layouter_c::calcLayout(int task, Fl_Int_Vector *widths, Fl_Int_Vector *heights,
                            Fl_Int_Vector *widgetsW, Fl_Int_Vector *widgetsH,
                            int targetW, int targetH) const {

  /* right this routine is stolen from the gridbaglayouter of the gcc java awt class set */

  /* task = 0: minsize; task = 1: SIZE */

  /* these 2 contain the maximum column and rownumber */
  unsigned int max_x = 0;
  unsigned int max_y = 0;

  /* as the caller is not required to provide the last two vectors
   * we'll have local instances and set the pointer to them if
   * we need that
   */
  Fl_Int_Vector ww, wh;

  if (!widgetsW) widgetsW = &ww;
  if (!widgetsH) widgetsH = &wh;

  /* resize the vectors to the correct size */
  widgetsW->size(children());
  widgetsH->size(children());

  Fl_Widget *const * _widgets = array();

  // first we figure out how many rows/columns
  for (int i = 0; i < children(); i++) {

    layoutable_c * widget = (layoutable_c*)_widgets[i]->get_assoc(layoutableAssociationType);
    if (!widget) continue;
    if (!widget->measure) continue;

    unsigned int gX = widget->gridX;
    unsigned int gY = widget->gridY;
    unsigned int gW = widget->gridW;
    unsigned int gH = widget->gridH;

    int w, h;

    widget->measure->getMinSize(_widgets[i], &w, &h);

    /* check the min sizes of the current widget */

    if (w < widget->minWidth) w = widget->minWidth;
    if (h < widget->minHeight) h = widget->minHeight;

    (*widgetsW)[i] = w;
    (*widgetsH)[i] = h;

    max_x = (max_x > gX+gW) ? (max_x) : (gX+gW);
    max_y = (max_y > gY+gH) ? (max_y) : (gY+gH);
  }

  Fl_Int_Vector weightsW(max_x), weightsH(max_y);
  widths->size(max_x);
  heights->size(max_y);

  /* initialize the vectors */
  for (unsigned int ii = 0; ii < max_x; ii++) {
    (*widths)[ii] = 0;
    weightsW[ii] = 0;
  }

  for (unsigned int ii = 0; ii < max_y; ii++) {
    (*heights)[ii] = 0;
    weightsH[ii] = 0;
  }

  /* separate the process into two steps. First put in the
   * widgets that have a fixed column and then place the other
   * ones. The wider ones can now have a more optimal
   * stretching of the column
   */
  for (int i = 0; i < children(); i++) {

    layoutable_c * widget = (layoutable_c*)_widgets[i]->get_assoc(layoutableAssociationType);
    if (!widget) continue;
    if (!widget->measure) continue;

    unsigned int gX = widget->gridX;
    unsigned int gW = widget->gridW;

    if (gW > 1) continue;

    int width = (*widgetsW)[i];

    width += 2 * widget->pitch;

    if (width > (*widths)[gX])
      (*widths)[gX] = width;

    if (widget->weightX > weightsW[gX])
      weightsW[gX] = widget->weightX;
  }

  for (int i = 0; i < children(); i++) {

    layoutable_c * widget = (layoutable_c*)_widgets[i]->get_assoc(layoutableAssociationType);
    if (!widget) continue;
    if (!widget->measure) continue;

    unsigned int gX = widget->gridX;
    unsigned int gW = widget->gridW;

    if (gW < 2) continue;

    /* find the column with the biggest weight */
    unsigned int bestW = gX;
    for (unsigned int w = gX + 1; w < gX+gW; w++)
      if (weightsW[w] >= weightsW[bestW])
        bestW = w;

    int width = (*widgetsW)[i];

    width += 2 * widget->pitch;

    for (unsigned int w = gX; w < gX+gW; w++)
      if (w != bestW)
        width -= (*widths)[w];

    if (width < 0) width = 0;

    if (width > (*widths)[bestW])
      (*widths)[bestW] = width;

    if (widget->weightX > weightsW[bestW])
      weightsW[bestW] = widget->weightX;
  }

  /* calculate rows, see columns */
  for (int i = 0; i < children(); i++) {

    layoutable_c * widget = (layoutable_c*)_widgets[i]->get_assoc(layoutableAssociationType);
    if (!widget) continue;
    if (!widget->measure) continue;

    unsigned int gY = widget->gridY;
    unsigned int gH = widget->gridH;

    if (gH > 1) continue;

    int height = (*widgetsH)[i];

    height += 2 * widget->pitch;

    if (height > (*heights)[gY])
      (*heights)[gY] = height;

    if (widget->weightY > weightsH[gY])
      weightsH[gY] = widget->weightY;
  }

  for (int i = 0; i < children(); i++)
  {
    layoutable_c * widget = (layoutable_c*)_widgets[i]->get_assoc(layoutableAssociationType);
    if (!widget) continue;
    if (!widget->measure) continue;

    unsigned int gY = widget->gridY;
    unsigned int gH = widget->gridH;

    if (gH < 2) continue;

    /* find the column with the biggest weight */
    unsigned int bestH = gY;
    for (unsigned int h = gY + 1; h < gY+gH; h++)
      if (weightsH[h] >= weightsH[bestH])
        bestH = h;

    int height = (*widgetsH)[i];

    height += 2 * widget->pitch;

    for (unsigned int h = gY; h < gY+gH; h++)
      if (h != bestH)
        height -= (*heights)[h];

    if (height < 0) height = 0;

    if (height > (*heights)[bestH])
      (*heights)[bestH] = height;

    if (widget->weightY > weightsH[bestH])
      weightsH[bestH] = widget->weightY;
  }

  if (task == 1) {
    /* stretch to final size if we do an actual layout we need to do this
     * here because the weight vectors are not exported
     */

    /* sum up the weight values */
    int sumWeightX = 0, sumWeightY = 0, sumW = 0, sumH = 0;

    for (unsigned int x = 0; x < max_x; x++) {
      sumWeightX += weightsW[x];
      sumW += (*widths)[x];
    }
    for (unsigned int y = 0; y < max_y; y++) {
      sumWeightY += weightsH[y];
      sumH += (*heights)[y];
    }

    /* now add the missing difference to the rows and columns */
    int dW = targetW - sumW;
    int dH = targetH - sumH;

    if (dW > 0)
      for (unsigned int x = 0; x < max_x; x++) {

        int add = (sumWeightX) ? (dW * weightsW[x] / sumWeightX) : (dW / (max_x-x));

        (*widths)[x] += add;
        dW -= add;
        sumWeightX -= weightsW[x];

        if (dW <= 0)
          break;
      }

    if (dH > 0)
      for (unsigned int y = 0; y < max_y; y++) {

        int add = (sumWeightY) ? (dH * weightsH[y] / sumWeightY) : (dH / (max_y-y));

        (*heights)[y] += add;
        dH -= add;
        sumWeightY -= weightsH[y];

        if (dH <= 0)
          break;
      }
  }
}

void layouter_c::resize(int xt, int yt, int w, int h) {

  /* first step calculate the widths of the rows and columns */
  Fl_Int_Vector widths;
  Fl_Int_Vector heights;

  Fl_Int_Vector widgetW;
  Fl_Int_Vector widgetH;

  calcLayout(1, &widths, &heights, &widgetW, &widgetH, w, h);

  /* check, if we need to make our widget bigger to accommodate all subwidgets */
  int wi = 0, hi = 0;
  for (unsigned int i = 0; i < widths.size(); i++)
    wi += widths[i];
  for (unsigned int i = 0; i < heights.size(); i++)
    hi += heights[i];

  if (wi > w) w = wi;
  if (hi > h) h = hi;

  Fl_Widget::resize(xt, yt, w, h);

  Fl_Widget *const * _widgets = array();

  /* and now layout widgets according to the plan */
  for (int i = 0; i < children(); i++) {

    int xp, yp, w, h;
    xp = yp = h = w = 0;

    layoutable_c * widget = (layoutable_c*)_widgets[i]->get_assoc(layoutableAssociationType);
    if (!widget) continue;
    if (!widget->measure) continue;

    unsigned int gX = widget->gridX;
    unsigned int gY = widget->gridY;
    unsigned int gW = widget->gridW;
    unsigned int gH = widget->gridH;

    for (unsigned int j = 0; j < gX; j++) xp += widths[j];
    for (unsigned int j = gX; j < gX+gW; j++) w += widths[j];

    for (unsigned int j = 0; j < gY; j++) yp += heights[j];
    for (unsigned int j = gY; j < gY+gH; j++) h += heights[j];

    /* pitch handling */
    int p = widget->pitch;
    xp += p;
    yp += p;
    w -= 2*p;
    h -= 2*p;

    unsigned int minHW = (widgetW[i] < widgetH[i]) ? (widgetW[i]) : (widgetH[i]);

    /* stretch handling */
    switch (widget->stretchX) {
    case STRETCH_MINUS:
      w = widgetW[i];
      break;
    case STRETCH_PLUS:
      xp += w - widgetW[i];
      w = widgetW[i];
      break;
    case STRETCH_MIDDLE:
      xp += (w - widgetW[i]) / 2;
      w = widgetW[i];
      break;
    case STRETCH_SQUARE:
      xp += (w - minHW) / 2;
      w = minHW;
      break;
    case STRETCH:
      break;
    }

    switch (widget->stretchY) {
    case STRETCH_MINUS:
      h = widgetH[i];
      break;
    case STRETCH_PLUS:
      yp += h - widgetH[i];
      h = widgetH[i];
      break;
    case STRETCH_MIDDLE:
      yp += (h - widgetH[i]) / 2;
      h = widgetH[i];
      break;
    case STRETCH_SQUARE:
      yp += (h - minHW) / 2;
      h = minHW;
      break;
    case STRETCH:
      break;
    }

    /* finally place the widget */
    _widgets[i]->resize(xp+x(), yp+y(), w, h);
  }
}

void layouter_c::getMinSize(int *width, int *height) const {

  if (!minsizeValid) {

    Fl_Int_Vector widths;
    Fl_Int_Vector heights;

    ((layouter_c*)this)->mw = ((layouter_c*)this)->mh = 0;

    if (children()) {

      calcLayout(0, &widths, &heights, 0, 0);

      /* accumulate the rows and columns */
      for (unsigned int i = 0; i < widths.size(); i++) ((layouter_c*)this)->mw += widths[i];
      for (unsigned int i = 0; i < heights.size(); i++) ((layouter_c*)this)->mh += heights[i];
    }

    ((layouter_c*)this)->minsizeValid = true;
  }

  *width = mw;
  *height = mh;
}

void layouter_c::remove(Fl_Widget &w) {
  minsizeValid = false;
  Fl_Group::remove(w);
}
void layouter_c::remove(Fl_Widget *w) {
  minsizeValid = false;
  Fl_Group::remove(w);
}
void layouter_c::add(Fl_Widget &w) {
  minsizeValid = false;
  Fl_Group::add(w);
}

void layouter_c::add(Fl_Widget *w) {
  minsizeValid = false;
  Fl_Group::add(w);
}





void LFl_Double_Window::show(void) {

  if (!shown()) {

    int wy, hy;
    lay->getMinSize(&wy, &hy);

    if (res)
      size_range(wy, hy, 0, 0);
    else
      size_range(wy, hy, wy, hy);

    if (!placed || wy > w() || hy > h()) {
      size(wy, hy);

      // center the window
      {
        int sw, sh, sx, sy;
        Fl::screen_xywh(sx, sy, sw, sh);

        position((sw-wy)/2+sx, (sh-hy)/2+sy);
      }
    }
  }

  Fl_Double_Window::show();
}


