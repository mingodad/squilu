//
// "$Id: Fl_Input.cxx 12208 2017-03-17 17:32:00Z AlbrechtS $"
//
// Input widget for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2016 by Bill Spitzak and others.
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

// This is the "user interface", it decodes user actions into what to
// do to the text.  See also Fl_Input_.cxx, where the text is actually
// manipulated (and some ui, in particular the mouse, is done...).
// In theory you can replace this code with another subclass to change
// the keybindings.

#include <stdio.h>
#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_System_Driver.H>
#include <FL/Fl_Screen_Driver.H>
#include <FL/Fl_Input.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include "flstring.h"

#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Secret_Input.H>

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

char *do_number_format(char *resbuf, int resbuf_len, double d, int dec, char dec_point, char thousand_sep)
{
	char tmpbuf[64];
	char *s, *t;  /* source, target */
	char *dp;
	int integral;
	int tmplen, reslen=0;
	int count=0;
	int is_negative=0;
	int trim_right_zeros=0;

	if (d < 0) {
		is_negative = 1;
		d = -d;
	}

	if (dec < 0) {
		trim_right_zeros = 1;
		dec = -dec;
	}

	tmplen = snprintf(tmpbuf, sizeof(tmpbuf), "%.*f", dec, d);

	if (!isdigit((int)tmpbuf[0])) {
		return NULL;
	}

	/* find decimal point, if expected */
	if (dec) {
		dp = strpbrk(tmpbuf, ".,");
	} else {
		dp = NULL;
	}

	/* calculate the length of the return buffer */
	if (dp) {
		integral = dp - tmpbuf;
	} else {
		/* no decimal point was found */
		integral = tmplen;
	}

	/* allow for thousand separators */
	if (thousand_sep) {
		integral += (integral-1) / 3;
	}

	reslen = integral;

	if (dec) {
		reslen += dec;

		if (dec_point) {
			reslen++;
		}
	}

	/* add a byte for minus sign */
	if (is_negative) {
		reslen++;
	}
	if(resbuf_len < reslen+1) return NULL;

	s = tmpbuf+tmplen-1;
	t = resbuf+reslen;
	*t-- = '\0';

	/* copy the decimal places.
	 * Take care, as the sprintf implementation may return less places than
	 * we requested due to internal buffer limitations */
	if (dec) {
		int declen = dp ? s - dp : 0;
		int topad = dec > declen ? dec - declen : 0;

		/* pad with '0's */
		while (topad--) {
			*t-- = '0';
		}

		if (dp) {
			s -= declen + 1; /* +1 to skip the point */
			t -= declen;

			/* now copy the chars after the point */
			memcpy(t + 1, dp + 1, declen);
		}

		/* add decimal point */
		if (dec_point) {
			*t-- = dec_point;
		}
	}

	/* copy the numbers before the decimal point, adding thousand
	 * separator every three digits */
	while(s >= tmpbuf) {
		*t-- = *s--;
		if (thousand_sep && (++count%3)==0 && s>=tmpbuf) {
			*t-- = thousand_sep;
		}
	}

	/* and a minus sign, if needed */
	if (is_negative) {
		*t-- = '-';
	}

	/* trim right zeros */
	if (dec_point && trim_right_zeros) {
		for(t=resbuf+reslen-1; *t != dec_point; t--){
			if (*t == '0') *t = '\0';
			else break;
		}
		if (*t == dec_point) *t = '\0';
	}

	return resbuf;
}

/*[0] = decimal_places, [1] = decimal_separator, [3] = thousands_separator*/
char Fl_Input::default_number_format_[4] = "\2.,";

void Fl_Input::draw() {
  if (input_type() == FL_HIDDEN_INPUT) return;
  char num_buffer[64];
  char saved_value[64];
  int using_number_format = 0;
  Fl_Boxtype b = box();
  int bborder = Fl::box_dx(b);
  int xo = x()+bborder;
  int yo = y()+bborder;
  int wo = w()-bborder*2;
  int ho = h()-bborder*2;

  if ((flags() & NUMERIC_RIGHT_TO_LEFT) && (Fl::focus() != this)) {
	  using_number_format = (flags() & USE_NUMERIC_FORMAT);
	  if (using_number_format) {
		double d = atof(value());
		int dec;
		char dec_point;
		char thousand_sep;
		if(number_format_) {
			dec = number_format_[0];
			dec_point = number_format_[1] == -1 ? Fl_Input::default_number_format_[1] : number_format_[1];
			thousand_sep = number_format_[2] == -1 ? Fl_Input::default_number_format_[2] : number_format_[2];
		} else {
			dec = Fl_Input::default_number_format_[0];
			dec_point = Fl_Input::default_number_format_[1];
			thousand_sep = Fl_Input::default_number_format_[2];
		}
		if(input_type() == FL_INT_INPUT) dec = 0;
		if(do_number_format(num_buffer, sizeof(num_buffer), d, dec, dec_point, thousand_sep)){
			strncpy(saved_value, value(), sizeof(saved_value));
			int has_changed = changed();
			value(num_buffer);
			if(has_changed) set_changed();
		} else using_number_format = 0;
	  }

	// Simplest to just redraw the whole box every time...
	damage(FL_DAMAGE_ALL);

	int wt, ht;

	// How long is the string to display?
	wt = 0; ht = 0;
	fl_font( textfont(), textsize() );
	fl_measure(value(), wt, ht);

	// Make the text window be at the right hand end
	wt = wt + 5;
	xo = xo + wo - wt;
	wo = wt;
  }
  if (damage() & FL_DAMAGE_ALL) draw_box(b, color());
  // Update the text window
  Fl_Input_::drawtext(xo, yo, wo, ho);
  if (using_number_format) {
      int has_changed = changed();
      value(saved_value);
      if(has_changed) set_changed();
  }
}

// kludge so shift causes selection to extend:
int Fl_Input::shift_position(int p) {
  return position(p, Fl::event_state(FL_SHIFT) ? mark() : p);
}

int Fl_Input::shift_up_down_position(int p) {
  return up_down_position(p, Fl::event_state(FL_SHIFT));
}

// Old text from FLTK 1.1 for reference:
// If you define NORMAL_INPUT_MOVE as zero you will get the peculiar fltk
// behavior where moving off the end of an input field will move the
// cursor into the next field:
// define it as 1 to prevent cursor movement from going to next field:
//
// Note: this has been replaced by Fl::option(Fl::OPTION_ARROW_FOCUS)
// in FLTK 1.3.  This option has "inverted" values:
//   1 = Arrow keys move focus (previously 0)
//   0 = Arrow keys don't move focus (previously 1)
// Hence we define ...
//
#define NORMAL_INPUT_MOVE (Fl::option(Fl::OPTION_ARROW_FOCUS) ? 0 : 1)

#define ctrl(x) ((x)^0x40)

// List of characters that are legal in a floating point input field.
// This text string is created at run-time to take the current locale
// into account (for example, continental Europe uses a comma instead
// of a decimal point). For back compatibility reasons, we always 
// allow the decimal point.
#ifdef HAVE_LOCALECONV
static const char *standard_fp_chars = ".eE+-"; 
static const char *legal_fp_chars = 0L;
#else
static const char *legal_fp_chars = ".eE+-"; 
#endif

// Move cursor up specified #lines
//    If OPTION_ARROW_FOCUS is disabled, return 1 to prevent focus navigation.
//
int Fl_Input::kf_lines_up(int repeat_num) {
  int i = position();
  if (!line_start(i)) {
    //UNNEEDED if (input_type()==FL_MULTILINE_INPUT && !Fl::option(Fl::OPTION_ARROW_FOCUS)) return 1;
    return NORMAL_INPUT_MOVE;
  }
  while(repeat_num--) {
    i = line_start(i);
    if (!i) break;
    i--;
  }
  shift_up_down_position(line_start(i));
  return 1;
}

// Move cursor down specified #lines
//    If OPTION_ARROW_FOCUS is disabled, return 1 to prevent focus navigation.
//
int Fl_Input::kf_lines_down(int repeat_num) {
  int i = position();
  if (line_end(i) >= size()) {
    //UNNEEDED if (input_type()==FL_MULTILINE_INPUT && !Fl::option(Fl::OPTION_ARROW_FOCUS)) return 1;
    return NORMAL_INPUT_MOVE;
  }
  while (repeat_num--) {  
    i = line_end(i);
    if (i >= size()) break;
    i++;
  }
  shift_up_down_position(i);
  return 1;
}

// Move up a page
int Fl_Input::kf_page_up() {
  return kf_lines_up(linesPerPage());
}

// Move down a page
int Fl_Input::kf_page_down() {
  return kf_lines_down(linesPerPage());
}

// Toggle insert mode
int Fl_Input::kf_insert_toggle() {
  if (readonly()) { fl_beep(); return 1; }
  return 1;				// \todo: needs insert mode
}

// Delete word right
int Fl_Input::kf_delete_word_right() {
  if (readonly()) { fl_beep(); return 1; }
  if (mark() != position()) return cut();
  cut(position(), word_end(position()));
  return 1;
}

// Delete word left
int Fl_Input::kf_delete_word_left() {
  if (readonly()) { fl_beep(); return 1; }
  if (mark() != position()) return cut();
  cut(word_start(position()), position());
  return 1;
}

// Delete to start of line
int Fl_Input::kf_delete_sol() {
  if (readonly()) { fl_beep(); return 1; }
  if (mark() != position()) return cut();
  cut(line_start(position()), position());
  return 1;
}

// Delete to end of line
int Fl_Input::kf_delete_eol() {
  if (readonly()) { fl_beep(); return 1; }
  if (mark() != position()) return cut();
  cut(position(), line_end(position()));
  return 1;
}

int Fl_Input::kf_delete_char_right() {
  if (readonly()) { fl_beep(); return 1; }
  if (mark() != position()) return cut();
  else return cut(1);
}

int Fl_Input::kf_delete_char_left() {
  if (readonly()) { fl_beep(); return 1; }
  if (mark() != position()) cut();
  else cut(-1);
  return 1;
}

// Move cursor to start of line
int Fl_Input::kf_move_sol() {
  return shift_position(line_start(position())) + NORMAL_INPUT_MOVE;
}

// Move cursor to end of line
int Fl_Input::kf_move_eol() {
  return shift_position(line_end(position())) + NORMAL_INPUT_MOVE;
}

// Clear to end of line
int Fl_Input::kf_clear_eol() {
  if (readonly()) { fl_beep(); return 1; }
  if (position()>=size()) return 0;
  int i = line_end(position());
  if (i == position() && i < size()) i++;
  cut(position(), i);
  return copy_cuts();
}

// Move cursor one character to the left
//    If OPTION_ARROW_FOCUS is disabled, return 1 to prevent focus navigation.
//
int Fl_Input::kf_move_char_left() {
  int i = shift_position(position()-1) + NORMAL_INPUT_MOVE;
  return Fl::option(Fl::OPTION_ARROW_FOCUS) ? i : 1;
}

// Move cursor one character to the right
//    If OPTION_ARROW_FOCUS is disabled, return 1 to prevent focus navigation.
//
int Fl_Input::kf_move_char_right() {
  int i = shift_position(position()+1) + NORMAL_INPUT_MOVE;
  return Fl::option(Fl::OPTION_ARROW_FOCUS) ? i : 1;
}

// Move cursor word-left
int Fl_Input::kf_move_word_left() {
  shift_position(word_start(position()));
  return 1; 
}

// Move cursor word-right
int Fl_Input::kf_move_word_right() {
  shift_position(word_end(position()));
  return 1;
}

// Move cursor up one line and to the start of line (paragraph up)
int Fl_Input::kf_move_up_and_sol() {
  if (line_start(position())==position() && position()>0)
    return shift_position(line_start(position()-1)) + NORMAL_INPUT_MOVE;
  else
    return shift_position(line_start(position())) + NORMAL_INPUT_MOVE;
}

// Move cursor down one line and to the end of line (paragraph down)
int Fl_Input::kf_move_down_and_eol() {
  if (line_end(position())==position() && position()<size())
    return shift_position(line_end(position()+1)) + NORMAL_INPUT_MOVE;
  else
    return shift_position(line_end(position())) + NORMAL_INPUT_MOVE;
}

// Move to top of document
int Fl_Input::kf_top() {
  shift_position(0);
  return 1;
}

// Move to bottom of document
int Fl_Input::kf_bottom() {
  shift_position(size());
  return 1; 
}

// Select all text in the widget
int Fl_Input::kf_select_all() {
  position(0,size());
  return 1;
}

// Undo.
int Fl_Input::kf_undo() {
  if (readonly()) { fl_beep(); return 1; }
  return undo();
}

// Redo. (currently unimplemented.. toggles undo() instead)
int Fl_Input::kf_redo() {
  if (readonly()) { fl_beep(); return 1; }
  return kf_undo();			// currently we don't support multilevel undo
}

// Do a copy operation
int Fl_Input::kf_copy() {
  return copy(1);
}

// Do a paste operation
int Fl_Input::kf_paste() {
  if (readonly()) { fl_beep(); return 1; }
  Fl::paste(*this, 1);
  return 1;
}

// Do a cut with copy
int Fl_Input::kf_copy_cut() {
  if (readonly()) { fl_beep(); return 1; }
  copy(1);
  return cut();
}

// Handle a keystroke.
//     Returns 1 if handled by us, 0 if not.
//
int Fl_Input::handle_key() {
  
  char ascii = Fl::event_text()[0];
  
  int del;
  if (Fl::compose(del)) {
    
    // Insert characters into numeric fields after checking for legality:
    if (input_type() == FL_FLOAT_INPUT || input_type() == FL_INT_INPUT) {
      Fl::compose_reset(); // ignore any composed characters...
      
      // initialize the list of legal characters inside a floating point number
#if defined(HAVE_LOCALECONV)
      if (!legal_fp_chars) {
        size_t len = strlen(standard_fp_chars);
        struct lconv *lc = localeconv();
        if (lc) {
          if (lc->decimal_point) len += strlen(lc->decimal_point);
          if (lc->mon_decimal_point) len += strlen(lc->mon_decimal_point);
          if (lc->positive_sign) len += strlen(lc->positive_sign);
          if (lc->negative_sign) len += strlen(lc->negative_sign);
        }
        // the following line is not a true memory leak because the array is only
        // allocated once if required, and automatically freed when the program quits
        char *chars = (char*)malloc(len+1);
	legal_fp_chars = chars;
        strcpy(chars, standard_fp_chars);
        if (lc) {
          if (lc->decimal_point) strcat(chars, lc->decimal_point);
          if (lc->mon_decimal_point) strcat(chars, lc->mon_decimal_point);
          if (lc->positive_sign) strcat(chars, lc->positive_sign);
          if (lc->negative_sign) strcat(chars, lc->negative_sign);
        }
      }
#endif // HAVE_LOCALECONV
      
      // find the insert position
      int ip = position()<mark() ? position() : mark();
      // This is complex to allow "0xff12" hex to be typed:
      if (   (!ip && (ascii == '+' || ascii == '-')) 
          || (ascii >= '0' && ascii <= '9') 
          || (ip==1 && index(0)=='0' && (ascii=='x' || ascii == 'X')) 
          || (ip>1 && index(0)=='0' && (index(1)=='x'||index(1)=='X') 
              && ((ascii>='A'&& ascii<='F') || (ascii>='a'&& ascii<='f'))) 
          || (input_type()==FL_FLOAT_INPUT && ascii && strchr(legal_fp_chars, ascii))) 
      {
	if (readonly()) fl_beep();
	else replace(position(), mark(), &ascii, 1);
      }
      return 1;
    }
    
    if (del || Fl::event_length()) {
      if (readonly()) fl_beep();
      else replace(position(), del ? position()-del : mark(),
	           Fl::event_text(), Fl::event_length());
    }
    if (Fl::screen_driver()->has_marked_text() && Fl::compose_state) {
      this->mark( this->position() - Fl::compose_state );
    }
    return 1;
  }
  
  unsigned int mods = Fl::event_state() & (FL_META|FL_CTRL|FL_ALT);
  unsigned int shift = Fl::event_state() & FL_SHIFT;
  unsigned int multiline = (input_type() == FL_MULTILINE_INPUT) ? 1 : 0;
  //
  // The following lists apps that support these keypresses.
  // Prefixes: '!' indicates NOT supported, '?' indicates un-verified.
  //
  //    HIG=Human Interface Guide, 
  //    TE=TextEdit.app, SA=Safari.app, WOX=MS Word/OSX -- OSX 10.4.x
  //    NP=Notepad, WP=WordPad, WOW=MS Word/Windows     -- WinXP
  //    GE=gedit, KE=kedit                              -- Ubuntu8.04
  //    OF=old FLTK behavior (<=1.1.10)
  //
  // Example: (NP,WP,!WO) means supported in notepad + wordpad, but NOT word.
  //

  // handle keypresses that can have a platform-dependent processing
  int retval = Fl::screen_driver()->input_widget_handle_key(Fl::event_key(), mods, shift, this);
  if (retval >= 0) return retval;
  
  switch (Fl::event_key()) {

    case FL_Insert:
      // Note: Mac has no "Insert" key; it's the "Help" key.
      //       This keypress is apparently not possible on macs.
      //
      if (mods==0 && shift) return kf_paste();			// Shift-Insert   (WP,NP,WOW,GE,KE,OF)
      if (mods==0)          return kf_insert_toggle();		// Insert         (Standard)
      if (mods==FL_CTRL)    return kf_copy();			// Ctrl-Insert    (WP,NP,WOW,GE,KE,OF)
      return 0;							// ignore other combos, pass to parent

    case FL_Enter:
    case FL_KP_Enter:
      if (when() & FL_WHEN_ENTER_KEY) {
        position(size(), 0);
        maybe_do_callback();
        return 1;
      } else if (multiline && !readonly()) {
        return replace(position(), mark(), "\n", 1);
      } return 0;			// reserved for shortcuts

    case FL_Tab:
      // Handle special case for multiline input with 'old tab behavior';
      // tab handled as a normal insertable character.
      //
      if (mods==0 && !shift 		// Tab?
	   && !tab_nav()		// old tab behavior enabled?
	   && multiline) {		// multiline input?
        break;				// insert tab character
      }
      if (mods==0) return 0;					// Tab, Shift-Tab? nav focus      (Standard/OSX-HIG)
      return 0;							// ignore other combos, pass to parent

    case 'a':
      if (mods==FL_COMMAND) return kf_select_all();		// Ctrl-A, Mac:Meta-A             (Standard/OSX-HIG)
      break;							// handle other combos elsewhere
    case 'c':
      if (mods==FL_COMMAND) return kf_copy();			// Ctrl-C, Mac:Meta-C             (Standard/OSX-HIG)
      break;							// handle other combos elsewhere
    case 'v':
      if (mods==FL_COMMAND) return kf_paste();			// Ctrl-V, Mac:Meta-V             (Standard/OSX-HIG)
      break;							// handle other combos elsewhere
    case 'x':
      if (mods==FL_COMMAND) return kf_copy_cut();		// Ctrl-X, Mac:Meta-X             (Standard/OSX-HIG)
      break;
    case 'z':
      if (mods==FL_COMMAND && !shift) return kf_undo();		// Ctrl-Z, Mac:Meta-Z             (Standard/OSX-HIG)
      if (mods==FL_COMMAND && shift)  return kf_redo();		// Shift-Ctrl-Z, Mac:Shift-Meta-Z (Standard/OSX-HIG)
      break;							// handle other combos elsewhere
  }
  
  switch (ascii) {
    case ctrl('H'):
      return kf_delete_char_left();				// Ctrl-H                           (!WP,!NP,!WOW,!WOX,TE,SA,GE,KE,OF)
    case ctrl('I'): 						// Ctrl-I (literal Tab)             (!WP,NP,!WOW,!GE,KE,OF)
    case ctrl('J'):						// Ctrl-J (literal Line Feed/Enter) (Standard)
    case ctrl('L'):						// Ctrl-L (literal Form Feed)       (Standard)
    case ctrl('M'):						// Ctrl-M (literal Cr)              (Standard)
      if (readonly()) { fl_beep(); return 1; }
      // insert a few selected control characters literally:
      if (input_type() != FL_FLOAT_INPUT && input_type() != FL_INT_INPUT)
        return replace(position(), mark(), &ascii, 1);
      break;
  }
  
  return 0;		// ignored
}

int Fl_Input::handle(int event) {
  static int dnd_save_position, dnd_save_mark, drag_start = -1, newpos;
  static Fl_Widget *dnd_save_focus = NULL;
  switch (event) {
    case FL_UNFOCUS:
      if (Fl::screen_driver()->has_marked_text() && Fl::compose_state) {
	this->mark( this->position() );
	Fl::reset_marked_text();
      }
      break;
    case FL_FOCUS:
      if (input_type() == FL_FLOAT_INPUT || input_type() == FL_INT_INPUT)
        damage(FL_DAMAGE_ALL);
      if(select_all_on_focus()){
        kf_select_all();
        break;
      }
      switch (Fl::event_key()) {
        case FL_Right:
          position(0);
          break;
        case FL_Left:
          position(size());
          break;
        case FL_Down:
          up_down_position(0);
          break;
        case FL_Up:
          up_down_position(line_start(size()));
          break;
        case FL_Tab:
          position(size(),0);
          break;
        default:
          position(position(),mark());// turns off the saved up/down arrow position
          break;
      }
      break;
      
    case FL_KEYBOARD:
      // Handle special case for multiline input with 'old tab behavior'
      // where tab is entered as a character: make sure user attempt to 'tab over'
      // widget doesn't destroy the field, replacing it with a tab character.
      //
      if (Fl::event_key() == FL_Tab 			// Tab key?
          && !Fl::event_state(FL_SHIFT)			// no shift?
          && !tab_nav()					// with tab navigation disabled?
	  && input_type() == FL_MULTILINE_INPUT		// with a multiline input?
	  && size() > 0                                 // non-empty field?
          && ((mark()==0 && position()==size()) || (position()==0 && mark()==size()))) {// while entire field selected?
        // Set cursor to the end of the selection...
        if (mark() > position())
          position(mark());
        else
          position(position());
        return (1);
      } else {
        if (active_r() && window() && this == Fl::belowmouse()) 
          window()->cursor(FL_CURSOR_NONE);
        return handle_key();
      }
      //NOTREACHED
      
    case FL_PUSH:
      if (Fl::dnd_text_ops()) {
        int oldpos = position(), oldmark = mark();
        Fl_Boxtype b = box();
        Fl_Input_::handle_mouse(x()+Fl::box_dx(b), y()+Fl::box_dy(b),
                                w()-Fl::box_dw(b), h()-Fl::box_dh(b), 0);
        newpos = position(); 
        position( oldpos, oldmark );
        if (Fl::focus()==this && !Fl::event_state(FL_SHIFT) && input_type()!=FL_SECRET_INPUT &&
           ( (newpos >= mark() && newpos < position()) || 
             (newpos >= position() && newpos < mark()) ) ) {
          // user clicked in the selection, may be trying to drag
          drag_start = newpos;
          return 1;
        }
        drag_start = -1;
      }
      
      if (Fl::focus() != this) {
        Fl::focus(this);
        handle(FL_FOCUS);
      }
      break;
      
    case FL_DRAG:
      if (Fl::dnd_text_ops()) {
        if (drag_start >= 0) {
          if (Fl::event_is_click()) return 1; // debounce the mouse
                                              // save the position because sometimes we don't get DND_ENTER:
          dnd_save_position = position();
          dnd_save_mark = mark();
	  dnd_save_focus = this;
          // drag the data:
          copy(0);
          Fl::screen_driver()->dnd(1);
          return 1;
        }
      }
      break;
      
    case FL_RELEASE:
      if (Fl::event_button() == 2) {
        Fl::event_is_click(0); // stop double click from picking a word
        Fl::paste(*this, 0);
      } else if (!Fl::event_is_click()) {
        // copy drag-selected text to the clipboard.
        copy(0);
      } else if (Fl::event_is_click() && drag_start >= 0) {
        // user clicked in the field and wants to reset the cursor position...
        position(drag_start, drag_start);
        drag_start = -1;
      } else if (Fl::event_clicks()) {
        // user double or triple clicked to select word or whole text
        copy(0);
      }
      
      // For output widgets, do the callback so the app knows the user
      // did something with the mouse...
      if (readonly()) do_callback();
      
      return 1;
      
    case FL_DND_ENTER:
      Fl::belowmouse(this); // send the leave events first
      if (dnd_save_focus != this) {
	dnd_save_position = position();
	dnd_save_mark = mark();
	dnd_save_focus = Fl::focus();
        Fl::focus(this);
        handle(FL_FOCUS);
      }
      // fall through:
    case FL_DND_DRAG: 
      //int p = mouse_position(X, Y, W, H);
#ifdef DND_OUT_XXXX
      if (Fl::focus()==this && (p>=dnd_save_position && p<=dnd_save_mark ||
                                p>=dnd_save_mark && p<=dnd_save_position)) {
        position(dnd_save_position, dnd_save_mark);
        return 0;
      }
#endif
      {
        Fl_Boxtype b = box();
        Fl_Input_::handle_mouse(x()+Fl::box_dx(b), y()+Fl::box_dy(b),
                                w()-Fl::box_dw(b), h()-Fl::box_dh(b), 0);
      }
      return 1;
      
    case FL_DND_LEAVE:
      position(dnd_save_position, dnd_save_mark);
#ifdef DND_OUT_XXXX
      if (!focused())
#endif
        if (dnd_save_focus && dnd_save_focus != this) {
          Fl::focus(dnd_save_focus);
          handle(FL_UNFOCUS);
        }
      Fl::first_window()->cursor(FL_CURSOR_MOVE);
      dnd_save_focus = NULL;
      return 1;
      
    case FL_DND_RELEASE:
      if (dnd_save_focus == this) { // if the dragged text comes from the same widget
	// remove the selected text
	int old_position = position();
	if (dnd_save_mark > dnd_save_position) {
	  int tmp = dnd_save_mark;
	  dnd_save_mark = dnd_save_position;
	  dnd_save_position = tmp;
	  }
	replace(dnd_save_mark, dnd_save_position, NULL, 0);
	if (old_position > dnd_save_position) position(old_position - (dnd_save_position - dnd_save_mark));
	else position(old_position);
	}
      else if(dnd_save_focus) {
	dnd_save_focus->handle(FL_UNFOCUS);
	}
      dnd_save_focus = NULL;
      take_focus();
      return 1;
      
      /* TODO: this will scroll the area, but stop if the cursor would become invisible.
       That clipping happens in drawtext(). Do we change the clipping or should 
       we move the cursor (ouch)?
       case FL_MOUSEWHEEL:
       if (Fl::e_dy > 0) {
       yscroll( yscroll() - Fl::e_dy*15 );
       } else if (Fl::e_dy < 0) {
       yscroll( yscroll() - Fl::e_dy*15 );
       }
       return 1;
       */
  }
  Fl_Boxtype b = box();
  return Fl_Input_::handletext(event,
                               x()+Fl::box_dx(b), y()+Fl::box_dy(b),
                               w()-Fl::box_dw(b), h()-Fl::box_dh(b));
}

/**
 Creates a new Fl_Input widget using the given position, size,
 and label string. The default boxtype is FL_DOWN_BOX.
 */
Fl_Input::Fl_Input(int X, int Y, int W, int H, const char *l)
: Fl_Input_(X, Y, W, H, l) {
	number_format_ = NULL;
}

Fl_Input::~Fl_Input(){
	if(number_format_)
		free((void*)number_format_);
}

void Fl_Input::default_number_format(const char *format){
	memcpy(Fl_Input::default_number_format_, format, sizeof(Fl_Input::default_number_format_));
}

const char *Fl_Input::default_number_format(void){
	return Fl_Input::default_number_format_;
}

void Fl_Input::number_format(const char *format){
	if(!number_format_)
		number_format_ = (char *)malloc(sizeof(Fl_Input::default_number_format_));
	memcpy(number_format_, format, sizeof(Fl_Input::default_number_format_));
}

int Fl_Input::decimal_places(){
	if(number_format_) return number_format_[0];
	return Fl_Input::default_number_format_[0];
}

void Fl_Input::decimal_places(int count){
	if(!number_format_){
		number_format_ = (char *)malloc(sizeof(Fl_Input::default_number_format_));
		number_format_[1] = -1;
		number_format_[2] = -1;
	}
	number_format_[0] = count;
}


Fl_Float_Input::Fl_Float_Input(int X,int Y,int W,int H,const char *l)
: Fl_Input(X,Y,W,H,l) 
{
  type(FL_FLOAT_INPUT);
  clear_flag(MAC_USE_ACCENTS_MENU);
  right_to_left(1);
}


Fl_Int_Input::Fl_Int_Input(int X,int Y,int W,int H,const char *l)
: Fl_Input(X,Y,W,H,l) {
  type(FL_INT_INPUT);
  clear_flag(MAC_USE_ACCENTS_MENU);
  right_to_left(1);
}


Fl_Multiline_Input::Fl_Multiline_Input(int X,int Y,int W,int H,const char *l)
: Fl_Input(X,Y,W,H,l) {
  type(FL_MULTILINE_INPUT);
}


Fl_Output::Fl_Output(int X,int Y,int W,int H, const char *l)
: Fl_Input(X, Y, W, H, l) {
  type(FL_NORMAL_OUTPUT);
}


Fl_Multiline_Output::Fl_Multiline_Output(int X,int Y,int W,int H,const char *l)
: Fl_Output(X,Y,W,H,l) {
  type(FL_MULTILINE_OUTPUT);
}


Fl_Secret_Input::Fl_Secret_Input(int X,int Y,int W,int H,const char *l)
: Fl_Input(X,Y,W,H,l) {
  type(FL_SECRET_INPUT);
  clear_flag(MAC_USE_ACCENTS_MENU);
}

int Fl_Secret_Input::handle(int event) {
  int retval = Fl_Input::handle(event);
  if (event == FL_KEYBOARD && Fl::screen_driver()->has_marked_text() && Fl::compose_state) {
    this->mark( this->position() ); // don't underline marked text
  }
  return retval;
}

//
// End of "$Id: Fl_Input.cxx 12208 2017-03-17 17:32:00Z AlbrechtS $".
//
