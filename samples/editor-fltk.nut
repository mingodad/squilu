//
// "$Id: editor.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// A simple text editor program for the Fast Light Tool Kit (FLTK).
//
// This program is described in Chapter 4 of the FLTK Programmer's Guide.
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

//
// Include necessary headers...
//
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef __MWERKS__
# define FL_DLL
#endif

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/filename.H>
*/
local WIN32 = os.getenv("WINDIR") != null
local __APPLE__ = os.getenv("MAC??") != null

local                changed = 0;
local               filename = "";
local               title="";
local		textbuf = null;


// Syntax highlighting stuff...
const TS = 14; // default editor textsize
local stylebuf = null;
local styletable = [	// Style table
		     [ FL_BLACK,      FL_COURIER,           TS ], // A - Plain
		     [ FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS ], // B - Line comments
		     [ FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS ], // C - Block comments
		     [ FL_BLUE,       FL_COURIER,           TS ], // D - Strings
		     [ FL_DARK_RED,   FL_COURIER,           TS ], // E - Directives
		     [ FL_DARK_RED,   FL_COURIER_BOLD,      TS ], // F - Types
		     [ FL_BLUE,       FL_COURIER_BOLD,      TS ], // G - Keywords
		   ];
local code_keywords = [	// List of known C/C++ keywords...
		     "and",
		     "and_eq",
		     "asm",
		     "bitand",
		     "bitor",
		     "break",
		     "case",
		     "catch",
		     "compl",
		     "continue",
		     "default",
		     "delete",
		     "do",
		     "else",
		     "false",
		     "for",
		     "goto",
		     "if",
		     "new",
		     "not",
		     "not_eq",
		     "operator",
		     "or",
		     "or_eq",
		     "return",
		     "switch",
		     "template",
		     "this",
		     "throw",
		     "true",
		     "try",
		     "while",
		     "xor",
		     "xor_eq"
		   ];
local code_types = [	// List of known C/C++ types...
		     "auto",
		     "bool",
		     "char",
		     "class",
		     "const",
		     "const_cast",
		     "double",
		     "dynamic_cast",
		     "enum",
		     "explicit",
		     "extern",
		     "float",
		     "friend",
		     "inline",
		     "int",
		     "long",
		     "mutable",
		     "namespace",
		     "private",
		     "protected",
		     "public",
		     "register",
		     "short",
		     "signed",
		     "sizeof",
		     "static",
		     "static_cast",
		     "struct",
		     "template",
		     "typedef",
		     "typename",
		     "union",
		     "unsigned",
		     "virtual",
		     "void",
		     "volatile"
		   ];


//
// 'compare_keywords()' - Compare two keywords...
//

//extern "C" {
//  int
function compare_keywords(a, b) { return a <=> b; }
//}

//
// 'style_parse()' - Parse text and produce style data.
//

//void
function style_parse(text, style, length) {
  local	     current;
  local	     col;
  local	     last;
  local	     buf, bufptr;
  local temp;
  local istyle = 0;
  local itext = 0;

  // Style letters:
  //
  // A - Plain
  // B - Line comments
  // C - Block comments
  // D - Strings
  // E - Directives
  // F - Types
  // G - Keywords

  //~ for (current = style[istyle], col = 0, last = 0; length > 0; length --, itext ++) {
    //~ if (current == 'B' || current == 'F' || current == 'G') current = 'A';
    //~ if (current == 'A') {
      //~ // Check for directives, comments, strings, and keywords...
      //~ if (col == 0 && text[itext] == '#') {
        //~ // Set style to directive
        //~ current = 'E';
      //~ } else if (strncmp(text, "//", 2) == 0) {
        //~ current = 'B';
	//~ for (; length > 0 && text[itext] != '\n'; length --, itext ++) istyle++ = 'B';

        //~ if (length == 0) break;
      //~ } else if (strncmp(text, "/*", 2) == 0) {
        //~ current = 'C';
      //~ } else if (strncmp(text, "\\\"", 2) == 0) {
        //~ // Quoted quote...
	//~ style[istyle++] = current;
	//~ style[style++] = current;
	//~ itext ++;
	//~ length --;
	//~ col += 2;
	//~ continue;
      //~ } else if (text[itext] == '\"') {
        //~ current = 'D';
      //~ } else if (!last && (islower((text[itext])&255) || text[itext] == '_')) {
        //~ // Might be a keyword...
	//~ for (temp = text[itext], bufptr = buf;
	     //~ (islower((temp)&255) || temp == '_') && bufptr < (buf + sizeof(buf) - 1);
	     //~ *bufptr++ = *temp++);

        //~ if (!islower((*temp)&255) && *temp != '_') {
	  //~ *bufptr = '\0';

          //~ bufptr = buf;

	  //~ if (bsearch(&bufptr, code_types,
	              //~ sizeof(code_types) / sizeof(code_types[0]),
		      //~ sizeof(code_types[0]), compare_keywords)) {
	    //~ while (text < temp) {
	      //~ style[istyle++] = 'F';
	      //~ itext ++;
	      //~ length --;
	      //~ col ++;
	    //~ }

	    //~ itext --;
	    //~ length ++;
	    //~ last = 1;
	    //~ continue;
	  //~ } else if (bsearch(&bufptr, code_keywords,
	                     //~ sizeof(code_keywords) / sizeof(code_keywords[0]),
		             //~ sizeof(code_keywords[0]), compare_keywords)) {
	    //~ while (text < temp) {
	      //~ style[istyle++] = 'G';
	      //~ itext ++;
	      //~ length --;
	      //~ col ++;
	    //~ }

	    //~ itext --;
	    //~ length ++;
	    //~ last = 1;
	    //~ continue;
	  //~ }
	//~ }
      //~ }
    //~ } else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
      //~ // Close a C comment...
      //~ style[istyle++] = current;
      //~ istyle++ = current;
      //~ itext ++;
      //~ length --;
      //~ current = 'A';
      //~ col += 2;
      //~ continue;
    //~ } else if (current == 'D') {
      //~ // Continuing in string...
      //~ if (strncmp(text[itext], "\\\"", 2) == 0) {
        //~ // Quoted end quote...
	//~ style[istyle++] = current;
	//~ style[istyle++] = current;
	//~ itext ++;
	//~ length --;
	//~ col += 2;
	//~ continue;
      //~ } else if (text[itext] == '\"') {
        //~ // End quote...
	//~ style[istyle++] = current;
	//~ col ++;
	//~ current = 'A';
	//~ continue;
      //~ }
    //~ }

    //~ // Copy style info...
    //~ if (current == 'A' && (text[itext] == '{' || text[itext] == '}')) style[style++] = 'G';
    //~ else style[istyle++] = current;
    //~ col ++;

    //~ last = isalnum((text[itext])&255) || text[itext] == '_' || text[itext] == '.';

    //~ if (text[itext] == '\n') {
      //~ // Reset column and possibly reset the style
      //~ col = 0;
      //~ if (current == 'B' || current == 'E') current = 'A';
    //~ }
  //~ }
}


//
// 'style_init()' - Initialize the style buffer...
//

//void
function style_init() {
  local style = new blob(textbuf->length()+1);
  local text = textbuf->text();

  style.memset(0, 'A', textbuf->length());
  style[textbuf->length()] = '\0';

  if (!stylebuf) stylebuf = new Fl_Text_Buffer(textbuf->length());

  style_parse(text, style, textbuf->length());

  stylebuf->text(style.asString());
  //delete[] style;
  //free(text);
}


//
// 'style_unfinished_cb()' - Update unfinished styles.
//

//void
function style_unfinished_cb(pint, pvoid) {
}


//
// 'style_update()' - Update the style buffer...
//

//void
function style_update(pos,		// I - Position of update
             nInserted,	// I - Number of inserted chars
	     nDeleted,	// I - Number of deleted chars
             nRestyled,	// I - Number of restyled chars
	     deletedText,// I - Text that was deleted
             cbArg) {	// I - Callback data
  local	start,				// Start of text
	end;				// End of text
  local	last,				// Last style on line
	style,				// Style data
	text;				// Text data


  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
    stylebuf->unselect();
    return;
  }

  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    style = new char[nInserted + 1];
    memset(style, 'A', nInserted);
    style[nInserted] = '\0';

    stylebuf->replace(pos, pos + nDeleted, style);
    //delete[] style;
  } else {
    // Just delete characters in the style buffer...
    stylebuf->remove(pos, pos + nDeleted);
  }

  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  stylebuf->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the previous line of the changed region to the end of
  // the line of the changed region...  Then we check the last
  // style character and keep updating if we have a multi-line
  // comment character...
  start = textbuf->line_start(pos);
//  if (start > 0) start = textbuf->line_start(start - 1);
  end   = textbuf->line_end(pos + nInserted);
  text  = textbuf->text_range(start, end);
  style = stylebuf->text_range(start, end);
  if (start==end)
    last = 0;
  else
    last  = style[end - start - 1];

//  printf("start = %d, end = %d, text = \"%s\", style = \"%s\", last='%c'...\n",
//         start, end, text, style, last);

  style_parse(text, style, end - start);

//  printf("new style = \"%s\", new last='%c'...\n",
//         style, style[end - start - 1]);

  stylebuf->replace(start, end, style);
  /*((Fl_Text_Editor *)*/cbArg->redisplay_range(start, end);

  if (start==end || last != style[end - start - 1]) {
//    printf("Recalculate the rest of the buffer style\n");
    // Either the user deleted some text, or the last character
    // on the line changed styles, so reparse the
    // remainder of the buffer...
    //free(text);
    //free(style);

    end   = textbuf->length();
    text  = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);

    style_parse(text, style, end - start);

    stylebuf->replace(start, end, style);
    /*((Fl_Text_Editor *)*/cbArg->redisplay_range(start, end);
  }

  //free(text);
  //free(style);
}


// Editor window functions and class...
/*
void save_cb();
void saveas_cb();
void find2_cb(Fl_Widget*, void*);
void replall_cb(Fl_Widget*, void*);
void replace2_cb(Fl_Widget*, void*);
void replcan_cb(Fl_Widget*, void*);
*/
class EditorWindow extends Fl_Double_Window {
  //public:
    replace_dlg = null;
    replace_find = null;
    replace_with = null;
    replace_all = null;
    replace_next = null;
    replace_cancel = null;

    editor = null;
    search = null;

constructor(iw, ih, t) {
  base.constructor(iw, ih, t);
  replace_dlg = new Fl_Window(300, 105, "Replace");
    replace_find = new Fl_Input(80, 10, 210, 25, "Find:");
    replace_find->align(FL_ALIGN_LEFT);

    replace_with = new Fl_Input(80, 40, 210, 25, "Replace:");
    replace_with->align(FL_ALIGN_LEFT);

    replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    replace_all->callback(replall_cb, this);

    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_next->callback(replace2_cb, this);

    replace_cancel = new Fl_Button(230, 70, 60, 25, "Cancel");
    replace_cancel->callback(replcan_cb, this);
  replace_dlg->end();
  replace_dlg->set_non_modal();
  editor = 0;
  search = "";
}

//destructor() {
//  delete replace_dlg;
//}
};

function check_save() {
  if (!changed) return 1;

  local r = fl_choice("The current file has not been saved.\nWould you like to save it now?",
                    "Cancel", "Save", "Don't Save");

  if (r == 1) {
    save_cb(); // Save the file...
    return !changed;
  }

  return (r == 2) ? 1 : 0;
}

local loading = 0;
function load_file(newfile, ipos) {
  loading = 1;
  local insert = (ipos != -1);
  changed = insert;
  if (!insert) filename = "";
  local r;
  if (!insert) r = textbuf->loadfile(newfile);
  else r = textbuf->insertfile(newfile, ipos);
  changed = changed || textbuf->input_file_was_transcoded;
  if (r)
    fl_alert("Error reading from file \'%s\':\n%s.", newfile, strerror(errno));
  else
    if (!insert) filename = newfile;
  loading = 0;
  //textbuf->call_modify_callbacks();
}

function save_file(newfile) {
  if (textbuf->savefile(newfile))
    fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
  else
    filename = newfile;
  changed = 0;
  textbuf->call_modify_callbacks();
}

function copy_cb(sender, e) {
  Fl_Text_Editor.kf_copy(0, e->editor);
}

function cut_cb(sender, e) {
  Fl_Text_Editor.kf_cut(0, e->editor);
}

function delete_cb(sender, v) {
  textbuf->remove_selection();
}

function find_cb(w, e) {
  local val;

  val = fl_input("Search String:", e->search);
  if (val != null) {
    // User entered a string - go find it!
    e->search = val;
    find2_cb(w, v);
  }
}

function find2_cb(w, e) {
  if (e->search.len() == 0) {
    // Search string is blank; get a new one...
    find_cb(w, v);
    return;
  }

  local pos = e->editor->insert_position();
  local found = textbuf->search_forward(pos, e->search/*, &pos*/);
  if (found) {
    // Found a match; select and update the position...
    textbuf->select(pos, pos+strlen(e->search));
    e->editor->insert_position(pos+e->search.len());
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", e->search);
}

function set_title(w) {
  if (filename.len() == 0) title = "Untitled";
  else {
    local slash = filename.rfind("/");
    if( WIN32 && slash == null) slash = filename.find("\\");
    if (slash != null) title = filename.slice(slash + 1);
    else title = filename;
  }

  if (changed) title += " (modified)";

  w->label(title);
}

function changed_cb(iparam, nInserted, nDeleted, iparam2, strparam, w) {
  if ((nInserted || nDeleted) && !loading) changed = 1;
  set_title(w);
  if (loading) w->editor->show_insert_position();
}

function new_cb(sender, udata) {
  if (!check_save()) return;

  filename = "";
  textbuf->select(0, textbuf->length());
  textbuf->remove_selection();
  changed = 0;
  textbuf->call_modify_callbacks();
}

function open_cb(sender, udata) {
  if (!check_save()) return;
  local  fnfc = Fl_Native_File_Chooser();
  fnfc.title("Open file");
  fnfc.type(Fl_Native_File_Chooser.BROWSE_FILE);
  if ( fnfc.show() ) return;
  load_file(fnfc.filename(), -1);

}

function insert_cb(sender, w) {
  local fnfc = Fl_Native_File_Chooser();
  fnfc.title("Insert file");
  fnfc.type(Fl_Native_File_Chooser.BROWSE_FILE);
  if ( fnfc.show() ) return;
  load_file(fnfc.filename(), w->editor->insert_position());
}

function paste_cb(sender, e) {
  Fl_Text_Editor.kf_paste(0, e->editor);
}

local num_windows = 0;

function close_cb(sender, w) {

  if (num_windows == 1) {
    if (!check_save())
      return;
  }

  w->hide();
  w->editor->buffer(0);
  textbuf->remove_modify_callback(style_update, w->editor);
  textbuf->remove_modify_callback(changed_cb, w);
  Fl.delete_widget(w);

  num_windows--;
  if (!num_windows) os.exit(0);
}

function quit_cb(sender, udata) {
  if (changed && !check_save())
    return;

  os.exit(0);
}

function replace_cb(sender, e) {
  e->replace_dlg->show();
}

function replace2_cb(sender, e) {
  local find = e->replace_find->value();
  local replace = e->replace_with->value();

  if (find.len() == 0) {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  local pos = e->editor->insert_position();
  local found = textbuf->search_forward(pos, find /*, &pos*/);

  if (found) {
    // Found a match; update the position and replace text...
    textbuf->select(pos, pos+strlen(find));
    textbuf->remove_selection();
    textbuf->insert(pos, replace);
    textbuf->select(pos, pos+replace.len());
    e->editor->insert_position(pos+replace.len());
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", find);
}

function replall_cb(sender, e) {
  local find = e->replace_find->value();
  local replace = e->replace_with->value();

  find = e->replace_find->value();
  if (find.len() == 0) {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  e->editor->insert_position(0);
  local times = 0;

  // Loop through the whole string
  for (local found = 1; found;) {
    local pos = e->editor->insert_position();
    found = textbuf->search_forward(pos, find/*, &pos*/);

    if (found) {
      // Found a match; update the position and replace text...
      textbuf->select(pos, pos+strlen(find));
      textbuf->remove_selection();
      textbuf->insert(pos, replace);
      e->editor->insert_position(pos+strlen(replace));
      e->editor->show_insert_position();
      times++;
    }
  }

  if (times) fl_message("Replaced %d occurrences.", times);
  else fl_alert("No occurrences of \'%s\' found!", find);
}

function replcan_cb(sender, e) {
  e->replace_dlg->hide();
}

function save_cb() {
  if (filename.len() == 0) {
    // No filename - get one!
    saveas_cb();
    return;
  }
  else save_file(filename);
}

function saveas_cb() {
  local fnfc = Fl_Native_File_Chooser();
  fnfc.title("Save File As?");
  fnfc.type(Fl_Native_File_Chooser.BROWSE_SAVE_FILE);
  if ( fnfc.show() ) return;
  save_file(fnfc.filename());
}

function view_cb(sender, udata) {
  local w = new_view();
  w->show();
}


local menuitems = [
  [ "&File",              0, 0, 0, FL_SUBMENU ],
    [ "&New File",        0, new_cb ],
    [ "&Open File...",    FL_COMMAND + 'o', open_cb ],
    [ "&Insert File...",  FL_COMMAND + 'i', insert_cb, 0, FL_MENU_DIVIDER ],
    [ "&Save File",       FL_COMMAND + 's', save_cb ],
    [ "Save File &As...", FL_COMMAND + FL_SHIFT + 's', saveas_cb, 0, FL_MENU_DIVIDER ],
    [ "New &View",        FL_ALT + (__APPLE__ ? FL_COMMAND : 0) + 'v', view_cb, 0 ],
    [ "&Close View",      FL_COMMAND + 'w', close_cb, 0, FL_MENU_DIVIDER ],
    [ "E&xit",            FL_COMMAND + 'q', quit_cb, 0 ],
    [ 0 ],

  [ "&Edit", 0, 0, 0, FL_SUBMENU ],
    [ "Cu&t",             FL_COMMAND + 'x', cut_cb ],
    [ "&Copy",            FL_COMMAND + 'c', copy_cb ],
    [ "&Paste",           FL_COMMAND + 'v', paste_cb ],
    [ "&Delete",          0, delete_cb ],
    [ 0 ],

  [ "&Search", 0, 0, 0, FL_SUBMENU ],
    [ "&Find...",         FL_COMMAND + 'f', find_cb ],
    [ "F&ind Again",      FL_COMMAND + 'g', find2_cb ],
    [ "&Replace...",      FL_COMMAND + 'r', replace_cb ],
    [ "Re&place Again",   FL_COMMAND + 't', replace2_cb ],
    [ 0 ],

  [ 0 ]
];

function mergerSubMenu(prefix, amenu, curr_pos, last_pos){
	for(; curr_pos < last_pos; ++curr_pos){
		local item = amenu[curr_pos];
		if(!item[0]) break;
		if(prefix){
			item[0] = prefix + "/" + item[0];
		}
		if(item.len() > 4 && item[4] & FL_SUBMENU){
			curr_pos = mergerSubMenu(item[0], amenu, ++curr_pos, last_pos);
		}
	}
	return curr_pos;
}
mergerSubMenu(null, menuitems, 0, menuitems.len());


function new_view() {
  local w = new EditorWindow(660, 400, title);
    w->begin();
    local m = new Fl_Menu_Bar(0, 0, 660, 30);
    m->copy(menuitems, w);
    w->editor = new Fl_Text_Editor(0, 30, 660, 370);
    w->editor->textfont(FL_COURIER);
    w->editor->textsize(TS);
  //w->editor->wrap_mode(Fl_Text_Editor::WRAP_AT_BOUNDS, 250);
    w->editor->buffer(textbuf);
    w->editor->highlight_data(stylebuf, styletable, styletable.len(),
			      'A', style_unfinished_cb, 0);
  textbuf->text();
  style_init();
  w->end();
  w->resizable(w->editor);
 
  //w->callback(close_cb, w);

  //textbuf->add_modify_callback(style_update, w->editor);
  //textbuf->add_modify_callback(changed_cb, w);
  //textbuf->call_modify_callbacks();
  num_windows++;
  return w;
}

function main(argc, argv) {
  textbuf = new Fl_Text_Buffer();
//textbuf->transcoding_warning_action = NULL;
  style_init();

  local window = new_view();

  //window->show(1, argv);
  window->show();

  if (argc > 0) load_file(argv[0], -1);
load_file("editor-fltk.nut", -1);

  return Fl.run();
}

main(vargv.len(), vargv);

//
// End of "$Id: editor.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
