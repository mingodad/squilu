// Fl_Time.cxx
// Source file for the time widget class
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

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <FLU/Fl_Time.H>

Fl_Time::Fl_Time(int x, int y, int w, int h, char *l) : Fl_Group(x, y, w, h, l) {
  int button_width = (int)(w/7);

  input_time = new Fl_Input(x, y, w - button_width * 4, h, 0);
  input_time->callback(input_changed_cb, this);
  input_time->when(FL_WHEN_CHANGED);
  input_time->textsize(10);

  button_decrease_hour = new Fl_Repeat_Button(x + w - 4 * button_width, y, button_width, h, "H-");
  button_decrease_hour->callback(button_cb, this);
  button_decrease_hour->labelsize(10);

  button_increase_hour = new Fl_Repeat_Button(x + w - 3 * button_width, y, button_width, h, "H+");
  button_increase_hour->callback(button_cb, this);
  button_increase_hour->labelsize(10);

  button_decrease_minute = new Fl_Repeat_Button(x + w - 2 * button_width, y, button_width, h, "M-");
  button_decrease_minute->callback(button_cb, this);
  button_decrease_minute->labelsize(10);

  button_increase_minute = new Fl_Repeat_Button(x + w - button_width, y, button_width, h, "M+");
  button_increase_minute->callback(button_cb, this);
  button_increase_minute->labelsize(10);

  end();

  type(FL_TIME_12HOUR);
  current_time();
}

void Fl_Time::current_time() {
  struct tm* display_time_tm;

  gettimeofday(&current_tv, 0);
  display_tv.tv_sec = current_tv.tv_sec;
  display_tv.tv_usec = current_tv.tv_usec;
  display_time_tm = localtime(&current_tv.tv_sec);

  if(type() == FL_TIME_24HOUR) strftime(time_string, 19, "%2H:%2M", display_time_tm);
  else strftime(time_string, 19, "%2I:%2M %p", display_time_tm);

  input_time->value(time_string);
}

void Fl_Time::refresh() {
  long different;
  struct tm *display_time_tm;

  if (valid()) {
    different = - display_tv.tv_sec + current_tv.tv_sec;
    gettimeofday(&current_tv, 0);

    display_tv.tv_sec = current_tv.tv_sec - different;
    display_time_tm = localtime(&display_tv.tv_sec);

    if(type() == FL_TIME_24HOUR) strftime(time_string, 19, "%2H:%2M", display_time_tm);
    else strftime(time_string, 19, "%2I:%2M %p", display_time_tm);

    input_time->value(time_string);
  }
}

int Fl_Time::hour() {
  struct tm *display_time_tm;

  display_time_tm = localtime(&display_tv.tv_sec);
  return display_time_tm->tm_hour;
}

int Fl_Time::minute() {
  struct tm *display_time_tm;

  display_time_tm = localtime(&display_tv.tv_sec);
  return display_time_tm->tm_min;
}

void Fl_Time::redisplay() {
  struct tm *display_time_tm;

  display_time_tm = localtime(&display_tv.tv_sec);

  if(type() == FL_TIME_24HOUR) strftime(time_string, 19, "%2H:%2M", display_time_tm);
  else strftime(time_string, 19, "%2I:%2M %p", display_time_tm);

  input_time->value(time_string);
}

void Fl_Time::hour(int value) {
  struct tm *display_time_tm;

  display_time_tm = localtime(&display_tv.tv_sec);
  display_time_tm->tm_hour = value;
  display_tv.tv_sec = mktime(display_time_tm);
}

void Fl_Time::minute(int value) {
  struct tm *display_time_tm;

  display_time_tm = localtime(&display_tv.tv_sec);
  if(value < 0) {
    display_time_tm->tm_min = 59;
  } else if(value >= 0 && value <= 59) {
    display_time_tm->tm_min = value;
  } else if (value > 59) {
    display_time_tm->tm_min = 0;
  }
  display_time_tm->tm_sec = 0;
  display_tv.tv_sec = mktime(display_time_tm);
}

void Fl_Time::value(int h, int m) {
  hour(h);
  minute(m);
}

bool Fl_Time::valid() {
  int h, m;
  char a[5];

  if(type() == FL_TIME_12HOUR) {
    if (sscanf(input_time->value(), "%d:%d %s",&h, &m, a) == 3) {
      if (h >= 1 && h <= 12 && m >= 0 && m <= 59 && (strcasecmp(a, "am") == 0 || strcasecmp(a, "pm") == 0)) {
	last_valid = true;
	return true;
      }
    }
  } else {
    if (sscanf(input_time->value(), "%d:%d",&h, &m) == 2) {
      if (h >= 0 && h <= 23 && m >= 0 && m <= 59) {
	last_valid = true;
	return true;
      }
    }
  }
  last_valid = false;
  return false;
}

void Fl_Time::input_changed_cb(Fl_Widget* widget, void* data) {
  Fl_Time* t = (Fl_Time*) data;
  int h, m;
  char a[5];

  if (t->valid()) {
    if(t->type() == FL_TIME_12HOUR) {
      sscanf(t->input_time->value(), "%d:%d %2s",&h, &m, a);
      if(strcasecmp(a, "am") == 0) {
	if(h < 12) {
	  t->hour(h);
	} else {
	  t->hour(0);
	}
      } else {
	if(h < 12) {
	  t->hour(h + 12);
	} else {
	  t->hour(12);
	}
      }
    } else {
      sscanf(t->input_time->value(), "%d:%d",&h, &m);
      t->hour(h);
    }
    t->minute(m);
  }
  t->do_callback();
}

void Fl_Time::button_cb(Fl_Widget* widget, void* data) {
  Fl_Time* t = (Fl_Time*) data;

  if(widget == t->button_decrease_hour) {
    t->hour(t->hour()-1);
  }
  if (widget == t->button_decrease_minute) {
    t->minute(t->minute()-1);
  }
  if (widget == t->button_increase_minute) {
    t->minute(t->minute()+1);
  }
  if (widget == t->button_increase_hour) {
    t->hour(t->hour()+1);
  }
  t->redisplay();
  t->do_callback();
}

void Fl_Time::textsize(int size) {
  input_time->textsize(size);
}

void Fl_Time::labelsize(int size) {
  button_decrease_hour->labelsize(size);
  button_decrease_minute->labelsize(size);
  button_increase_minute->labelsize(size);
  button_increase_hour->labelsize(size);
  Fl_Group::labelsize(size);
}

void Fl_Time::textfont(Fl_Font font) {
  input_time->textfont(font);
}

void Fl_Time::labelfont(Fl_Font font) {
  button_decrease_hour->labelfont(font);
  button_decrease_minute->labelfont(font);
  button_increase_minute->labelfont(font);
  button_increase_hour->labelfont(font);
  Fl_Group::labelfont(font);
}

void Fl_Time::textcolor(Fl_Color color) {
  input_time->textcolor(color);
}

void Fl_Time::labelcolor(Fl_Color color) {
  button_decrease_hour->labelcolor(color);
  button_decrease_minute->labelcolor(color);
  button_increase_minute->labelcolor(color);
  button_increase_hour->labelcolor(color);
  Fl_Group::labelcolor(color);
}

int Fl_Time::textsize() {
  return input_time->textsize();
}

int Fl_Time::labelsize() {
  return button_decrease_hour->labelsize();
}

Fl_Font Fl_Time::labelfont() {
  return button_decrease_hour->labelfont();
}

Fl_Font Fl_Time::textfont() {
  return input_time->textfont();
}

Fl_Color Fl_Time::labelcolor() {
  return button_decrease_hour->labelcolor();
}

Fl_Color Fl_Time::textcolor() {
  return input_time->textcolor();
}
