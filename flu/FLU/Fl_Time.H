// Fl_Time.h
// Header file for the time Widget class
//
// Copyright (C) 2000 Softfield Research Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef __TIME_WIDGET_H
#define __TIME_WIDGET_H

#include "sys/time.h"
#include "unistd.h"
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Input.H>

#define FL_TIME_24HOUR 0
#define FL_TIME_12HOUR 1

/** @package libflek_ui
 * Fl_Time is a widget that allows a user to select a time
 * (hour and minute). 
 * <p><img src="Fl_Time.png">
 */
class Fl_Time : public Fl_Group {

private:
  Fl_Repeat_Button* button_decrease_hour;
  Fl_Repeat_Button* button_decrease_minute;
  Fl_Input*  input_time;
  Fl_Repeat_Button* button_increase_minute;
  Fl_Repeat_Button* button_increase_hour;
  
  struct timeval current_tv;
  struct timeval display_tv;
  char time_string[20];
  bool last_valid;
  
  int look_;
  
  static void input_changed_cb(Fl_Widget* widget, void* data);
  static void button_cb(Fl_Widget* widget, void* data);
  
public:
  /**
   * The constructor for an empty Fl_Time widget.
   */
  Fl_Time(int x, int y, int w, int h, char *l=0); 

  /**
   * Gets the hour.
   *
   * @return The hour associated with this widget.
   */  
  int hour();

  /**
   * Sets the hour.
   *
   * @param hour The hour associated with this widget.
   */  
  void hour(int value);
  
  /**
   * Gets the minute.
   *
   * @return The minute associated with this widget.
   */  
  int minute();

  /**
   * Sets the minute.
   *
   * @param minute The minute associated with this widget.
   */  
  void minute(int value);
  
  // Be sure to run this after using hour and min to change the clock value.
  void redisplay();
  
  /**
   * Sets the minute and hour at the same time.
   *
   * @param minute The minute associated with this widget.
   * @param hour The hour associated with this widget.
   */  
  void value(int h, int m);

  /**
   * Sets the minute and hour to the system minute and hour.
   */
  void current_time();

  /**
   * Refreshes the widget.
   */
  void refresh();

  /**
   * Sets the size of the label text which is used for the M+,
   * M-, Y+, and Y- labels.
   *
   * @param size The size of the label font.
   */  
  void labelsize(int size);

  /**
   * Sets the label font which is used for the M+,
   * M-, Y+, and Y- labels.
   *
   * @param font The label font.
   */  
  void labelfont(Fl_Font font);

  /**
   * Sets the label color which is used for the M+,
   * M-, Y+, and Y- labels.
   *
   * @param font The label color.
   */  
  void labelcolor(Fl_Color color);

  /**
   * Sets the size of the text which is used to display
   * the set time.
   *
   * @param size The size of the text font.
   */  
  void textsize(int size);

  /**
   * Sets the font of the text which is used to display
   * the set time.
   *
   * @param font The font of the text font.
   */  
  void textfont(Fl_Font);

  /**
   * Sets the color of the text which is used to display
   * the set time.
   *
   * @param color The color of the text font.
   */  
  void textcolor(Fl_Color);
  
  /**
   * Gets the size of the label text which is used for the M+,
   * M-, Y+, and Y- labels.
   *
   * @return The size of the label font.
   */  
  int labelsize();

  /**
   * Gets the label font which is used for the M+,
   * M-, Y+, and Y- labels.
   *
   * @return The label font.
   */  
  Fl_Font labelfont();

  /**
   * Gets the label color which is used for the M+,
   * M-, Y+, and Y- labels.
   *
   * @return The label color.
   */  
  Fl_Color labelcolor();
  
  /**
   * Gets the size of the text which is used to display
   * the set time.
   *
   * @return The size of the text font.
   */  
  int textsize();

  /**
   * Gets the font of the text which is used to display
   * the set time.
   *
   * @return The font of the text font.
   */  
  Fl_Font textfont();

  /**
   * Gets the color of the text which is used to display
   * the set time.
   *
   * @return The color of the text font.
   */
  Fl_Color textcolor();

  /**
   * Determines if the entered time is a recognized format.
   *
   * @return True if it is a valid time format, otherwise false.
   */  
  bool valid();	
};

#endif
