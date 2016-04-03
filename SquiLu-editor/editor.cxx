//
// "$Id: editor.cxx 9718 2012-11-13 13:03:20Z manolo $"
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

#include <stdarg.h>
#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdaux.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#define MAX_PATH 1024
#endif

#ifdef SQUNICODE
#define scfprintf fwprintf
#define scfopen	_wfopen
#define scvprintf vfwprintf
#else
#define scfprintf fprintf
#define scfopen	fopen
#define scvprintf vfprintf
#endif

static HSQUIRRELVM v;

static bool _debug_wait = false;
static bool _stop_debug = false;
static int _debug_breakpoint_line = -1;
static int _debug_pos = 0;
static void sq_debug_hook(HSQUIRRELVM v, SQInteger type, const SQChar *sourcename, SQInteger line, const SQChar *funcname);

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

int                changed = 0;
char               filename[FL_PATH_MAX] = "";
char               title[FL_PATH_MAX];
Fl_Text_Buffer     *textbuf = 0;


// Syntax highlighting stuff...
#define TS 14 // default editor textsize
Fl_Text_Buffer     *stylebuf = 0;
Fl_Text_Display::Style_Table_Entry
                   styletable[] = {	// Style table
		     { FL_BLACK,      FL_COURIER,           TS }, // A - Plain
		     { FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS }, // B - Line comments
		     { FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS }, // C - Block comments
		     { FL_BLUE,       FL_COURIER,           TS }, // D - Strings
		     { FL_DARK_RED,   FL_COURIER,           TS }, // E - Directives
		     { FL_DARK_RED,   FL_COURIER_BOLD,      TS }, // F - Types
		     { FL_BLUE,       FL_COURIER_BOLD,      TS }, // G - Keywords
		   };
const char         *code_keywords[] = {	// List of known C/C++ keywords...
		     "and",
		     "and_eq",
		     "array",
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
		     "foreach",
		     "function",
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
		     "table",
		     "template",
		     "this",
		     "throw",
		     "true",
		     "try",
		     "while",
		     "xor",
		     "xor_eq"
		   };
const char         *code_types[] = {	// List of known C/C++ types...
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
		     "local",
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
		   };


//
// 'compare_keywords()' - Compare two keywords...
//

extern "C" {
  int
  compare_keywords(const void *a,
                   const void *b) {
    return (strcmp(*((const char **)a), *((const char **)b)));
  }
}

//
// 'style_parse()' - Parse text and produce style data.
//

void
style_parse(const char *text,
            char       *style,
	    int        length) {
  char	     current;
  int	     col;
  int	     last;
  char	     buf[255],
             *bufptr;
  const char *temp;

  // Style letters:
  //
  // A - Plain
  // B - Line comments
  // C - Block comments
  // D - Strings
  // E - Directives
  // F - Types
  // G - Keywords

  for (current = *style, col = 0, last = 0; length > 0; length --, text ++) {
    if (current == 'B' || current == 'F' || current == 'G') current = 'A';
    if (current == 'A') {
      // Check for directives, comments, strings, and keywords...
      if (col == 0 && *text == '#') {
        // Set style to directive
        current = 'E';
      } else if (strncmp(text, "//", 2) == 0) {
        current = 'B';
	for (; length > 0 && *text != '\n'; length --, text ++) *style++ = 'B';

        if (length == 0) break;
      } else if (strncmp(text, "/*", 2) == 0) {
        current = 'C';
      } else if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted quote...
	*style++ = current;
	*style++ = current;
	text ++;
	length --;
	col += 2;
	continue;
      } else if (*text == '\"') {
        current = 'D';
      } else if (!last && (islower((*text)&255) || *text == '_')) {
        // Might be a keyword...
	for (temp = text, bufptr = buf;
	     (islower((*temp)&255) || *temp == '_') && bufptr < (buf + sizeof(buf) - 1);
	     *bufptr++ = *temp++);

        if (!islower((*temp)&255) && *temp != '_') {
	  *bufptr = '\0';

          bufptr = buf;

	  if (bsearch(&bufptr, code_types,
	              sizeof(code_types) / sizeof(code_types[0]),
		      sizeof(code_types[0]), compare_keywords)) {
	    while (text < temp) {
	      *style++ = 'F';
	      text ++;
	      length --;
	      col ++;
	    }

	    text --;
	    length ++;
	    last = 1;
	    continue;
	  } else if (bsearch(&bufptr, code_keywords,
	                     sizeof(code_keywords) / sizeof(code_keywords[0]),
		             sizeof(code_keywords[0]), compare_keywords)) {
	    while (text < temp) {
	      *style++ = 'G';
	      text ++;
	      length --;
	      col ++;
	    }

	    text --;
	    length ++;
	    last = 1;
	    continue;
	  }
	}
      }
    } else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
      // Close a C comment...
      *style++ = current;
      *style++ = current;
      text ++;
      length --;
      current = 'A';
      col += 2;
      continue;
    } else if (current == 'D') {
      // Continuing in string...
      if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted end quote...
	*style++ = current;
	*style++ = current;
	text ++;
	length --;
	col += 2;
	continue;
      } else if (*text == '\"') {
        // End quote...
	*style++ = current;
	col ++;
	current = 'A';
	continue;
      }
    }

    // Copy style info...
    if (current == 'A' && (*text == '{' || *text == '}')) *style++ = 'G';
    else *style++ = current;
    col ++;

    last = isalnum((*text)&255) || *text == '_' || *text == '.';

    if (*text == '\n') {
      // Reset column and possibly reset the style
      col = 0;
      if (current == 'B' || current == 'E') current = 'A';
    }
  }
}


//
// 'style_init()' - Initialize the style buffer...
//

void
style_init(void) {
  char *style = new char[textbuf->length() + 1];
  char *text = textbuf->text();

  memset(style, 'A', textbuf->length());
  style[textbuf->length()] = '\0';

  if (!stylebuf) stylebuf = new Fl_Text_Buffer(textbuf->length());

  style_parse(text, style, textbuf->length());

  stylebuf->text(style);
  delete[] style;
  free(text);
}


//
// 'style_unfinished_cb()' - Update unfinished styles.
//

void
style_unfinished_cb(int, void*) {
}


//
// 'style_update()' - Update the style buffer...
//

void
style_update(int        pos,		// I - Position of update
             int        nInserted,	// I - Number of inserted chars
	     int        nDeleted,	// I - Number of deleted chars
             int        /*nRestyled*/,	// I - Number of restyled chars
	     const char * /*deletedText*/,// I - Text that was deleted
             void       *cbArg) {	// I - Callback data
  int	start,				// Start of text
	end;				// End of text
  char	last,				// Last style on line
	*style,				// Style data
	*text;				// Text data


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
    delete[] style;
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
  ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);

  if (start==end || last != style[end - start - 1]) {
//    printf("Recalculate the rest of the buffer style\n");
    // Either the user deleted some text, or the last character
    // on the line changed styles, so reparse the
    // remainder of the buffer...
    free(text);
    free(style);

    end   = textbuf->length();
    text  = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);

    style_parse(text, style, end - start);

    stylebuf->replace(start, end, style);
    ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);
  }

  free(text);
  free(style);
}


// Editor window functions and class...
void save_cb();
void saveas_cb();
void find2_cb(Fl_Widget*, void*);
void replall_cb(Fl_Widget*, void*);
void replace2_cb(Fl_Widget*, void*);
void replcan_cb(Fl_Widget*, void*);

class EditorWindow : public Fl_Double_Window {
  public:
    EditorWindow(int w, int h, const char* t);
    ~EditorWindow();

    Fl_Window          *replace_dlg;
    Fl_Input           *replace_find;
    Fl_Input           *replace_with;
    Fl_Button          *replace_all;
    Fl_Return_Button   *replace_next;
    Fl_Button          *replace_cancel;

    Fl_Text_Editor     *editor;
    char               search[256];
};

EditorWindow::EditorWindow(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
  replace_dlg = new Fl_Window(300, 105, "Replace");
    replace_find = new Fl_Input(80, 10, 210, 25, "Find:");
    replace_find->align(FL_ALIGN_LEFT);

    replace_with = new Fl_Input(80, 40, 210, 25, "Replace:");
    replace_with->align(FL_ALIGN_LEFT);

    replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    replace_all->callback((Fl_Callback *)replall_cb, this);

    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_next->callback((Fl_Callback *)replace2_cb, this);

    replace_cancel = new Fl_Button(230, 70, 60, 25, "Cancel");
    replace_cancel->callback((Fl_Callback *)replcan_cb, this);
  replace_dlg->end();
  replace_dlg->set_non_modal();
  editor = 0;
  *search = (char)0;
}

EditorWindow::~EditorWindow() {
  delete replace_dlg;
}

int check_save(void) {
  if (!changed) return 1;

  int r = fl_choice("The current file has not been saved.\n"
                    "Would you like to save it now?",
                    "Cancel", "Save", "Don't Save");

  if (r == 1) {
    save_cb(); // Save the file...
    return !changed;
  }

  return (r == 2) ? 1 : 0;
}

int loading = 0;
void load_file(const char *newfile, int ipos) {
  loading = 1;
  int insert = (ipos != -1);
  changed = insert;
  if (!insert) strcpy(filename, "");
  int r;
  if (!insert) r = textbuf->loadfile(newfile);
  else r = textbuf->insertfile(newfile, ipos);
  changed = changed || textbuf->input_file_was_transcoded;
  if (r)
    fl_alert("Error reading from file \'%s\':\n%s.", newfile, strerror(errno));
  else
    if (!insert) strcpy(filename, newfile);
  loading = 0;
  textbuf->call_modify_callbacks();
}

void save_file(const char *newfile) {
  if (textbuf->savefile(newfile))
    fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
  else
    strcpy(filename, newfile);
  changed = 0;
  textbuf->call_modify_callbacks();
}

void copy_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  Fl_Text_Editor::kf_copy(0, e->editor);
}

void cut_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  Fl_Text_Editor::kf_cut(0, e->editor);
}

void delete_cb(Fl_Widget*, void*) {
  textbuf->remove_selection();
}

void find_cb(Fl_Widget* w, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  const char *val;

  val = fl_input("Search String:", e->search);
  if (val != NULL) {
    // User entered a string - go find it!
    strcpy(e->search, val);
    find2_cb(w, v);
  }
}

void find2_cb(Fl_Widget* w, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  if (e->search[0] == '\0') {
    // Search string is blank; get a new one...
    find_cb(w, v);
    return;
  }

  int pos = e->editor->insert_position();
  int found = textbuf->search_forward(pos, e->search, &pos);
  if (found) {
    // Found a match; select and update the position...
    textbuf->select(pos, pos+strlen(e->search));
    e->editor->insert_position(pos+strlen(e->search));
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", e->search);
}

void set_title(Fl_Window* w) {
  if (filename[0] == '\0') strcpy(title, "Untitled");
  else {
    char *slash;
    slash = strrchr(filename, '/');
#ifdef WIN32
    if (slash == NULL) slash = strrchr(filename, '\\');
#endif
    if (slash != NULL) strcpy(title, slash + 1);
    else strcpy(title, filename);
  }

  if (changed) strcat(title, " (modified)");

  w->label(title);
}

void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
  if ((nInserted || nDeleted) && !loading) changed = 1;
  EditorWindow *w = (EditorWindow *)v;
  set_title(w);
  if (loading) w->editor->show_insert_position();
}

void new_cb(Fl_Widget*, void*) {
  if (!check_save()) return;

  filename[0] = '\0';
  textbuf->select(0, textbuf->length());
  textbuf->remove_selection();
  changed = 0;
  textbuf->call_modify_callbacks();
}

void open_cb(Fl_Widget*, void*) {
  if (!check_save()) return;
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Open file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  if ( fnfc.show() ) return;
  load_file(fnfc.filename(), -1);

}

void insert_cb(Fl_Widget*, void *v) {
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Insert file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  if ( fnfc.show() ) return;
  EditorWindow *w = (EditorWindow *)v;
  load_file(fnfc.filename(), w->editor->insert_position());
}

void paste_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  Fl_Text_Editor::kf_paste(0, e->editor);
}

int num_windows = 0;

void close_cb(Fl_Widget*, void* v) {
  EditorWindow* w = (EditorWindow*)v;

  if (num_windows == 1) {
    if (!check_save())
      return;
  }

  w->hide();
  w->editor->buffer(0);
  textbuf->remove_modify_callback(style_update, w->editor);
  textbuf->remove_modify_callback(changed_cb, w);
  Fl::delete_widget(w);

  num_windows--;
  if (!num_windows) exit(0);
}

void quit_cb(Fl_Widget*, void*) {
  if (changed && !check_save())
    return;

  exit(0);
}

void replace_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  e->replace_dlg->show();
}

void replace2_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  const char *find = e->replace_find->value();
  const char *replace = e->replace_with->value();

  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  int pos = e->editor->insert_position();
  int found = textbuf->search_forward(pos, find, &pos);

  if (found) {
    // Found a match; update the position and replace text...
    textbuf->select(pos, pos+strlen(find));
    textbuf->remove_selection();
    textbuf->insert(pos, replace);
    textbuf->select(pos, pos+strlen(replace));
    e->editor->insert_position(pos+strlen(replace));
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", find);
}

void replall_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  const char *find = e->replace_find->value();
  const char *replace = e->replace_with->value();

  find = e->replace_find->value();
  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  e->editor->insert_position(0);
  int times = 0;

  // Loop through the whole string
  for (int found = 1; found;) {
    int pos = e->editor->insert_position();
    found = textbuf->search_forward(pos, find, &pos);

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

void replcan_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  e->replace_dlg->hide();
}

void save_cb() {
  if (filename[0] == '\0') {
    // No filename - get one!
    saveas_cb();
    return;
  }
  else save_file(filename);
}

void saveas_cb() {
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Save File As?");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  if ( fnfc.show() ) return;
  save_file(fnfc.filename());
}

void chdir_to(){
  if (filename[0]) {
    char buf[MAX_PATH];
    int len = snprintf(buf, sizeof(buf), "%s", filename);
    for(int i=len; i >=0; --i){
        if(buf[i] == '/' || buf[i] == '\\'){
            buf[i] = '\0';
            break;
        }
    }
    chdir(buf);
  }
}

void run_cb(Fl_Widget*, void* ew) {
  if (filename[0]) {
      chdir_to();
      sq_setnativedebughook(v, NULL);
      sq_enabledebuginfo(v, 0);
      sqstd_dofile(v, filename, SQFalse, SQTrue, SQTrue);
  }
}

#define DBG_EDITOR_WINDOW_KEY "DebugEditorWindow"

void breakpoint_cb(Fl_Widget*, void* ew)
{
    EditorWindow* w = (EditorWindow*)ew;
    _debug_breakpoint_line = w->editor->count_lines(0, w->editor->insert_position(), true);
    printf("Breakpoint at line %d\n", _debug_breakpoint_line);
}

void debug_cb(Fl_Widget*, void* ew) {
  if(_debug_wait){
      _debug_wait = false;
      return;
  }
  if (filename[0]) {
      chdir_to();
      _debug_wait = true;
      _stop_debug = false;
      _debug_pos = 0;
      sq_pushregistrytable(v);
      sq_pushstring(v, DBG_EDITOR_WINDOW_KEY, -1);
      sq_pushuserpointer(v, ew);
      sq_rawset(v, -3);
      sq_poptop(v);
      sq_setnativedebughook(v, sq_debug_hook);
      sq_enabledebuginfo(v, 1);
      sqstd_dofile(v, filename, SQFalse, SQTrue, SQTrue);
  }
}

void stop_debug_cb(Fl_Widget*, void* ew) {
    _stop_debug = true;
}

Fl_Window* new_view();

void view_cb(Fl_Widget*, void*) {
  Fl_Window* w = new_view();
  w->show();
}

Fl_Menu_Item menuitems[] = {
  { "&File",              0, 0, 0, FL_SUBMENU },
    { "&New File",        0, (Fl_Callback *)new_cb },
    { "&Open File...",    FL_COMMAND + 'o', (Fl_Callback *)open_cb },
    { "&Insert File...",  FL_COMMAND + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
    { "&Save File",       FL_COMMAND + 's', (Fl_Callback *)save_cb },
    { "Save File &As...", FL_COMMAND + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
    { "New &View",        FL_ALT
#ifdef __APPLE__
      + FL_COMMAND
#endif
      + 'v', (Fl_Callback *)view_cb, 0 },
    { "&Close View",      FL_COMMAND + 'w', (Fl_Callback *)close_cb, 0, FL_MENU_DIVIDER },
    { "E&xit",            FL_COMMAND + 'q', (Fl_Callback *)quit_cb, 0 },
    { 0 },

  { "&Edit", 0, 0, 0, FL_SUBMENU },
    { "Cu&t",             FL_COMMAND + 'x', (Fl_Callback *)cut_cb },
    { "&Copy",            FL_COMMAND + 'c', (Fl_Callback *)copy_cb },
    { "&Paste",           FL_COMMAND + 'v', (Fl_Callback *)paste_cb },
    { "&Delete",          0, (Fl_Callback *)delete_cb },
    { 0 },

  { "&Search", 0, 0, 0, FL_SUBMENU },
    { "&Find...",         FL_COMMAND + 'f', (Fl_Callback *)find_cb },
    { "F&ind Again",      FL_COMMAND + 'g', find2_cb },
    { "&Replace...",      FL_COMMAND + 'r', replace_cb },
    { "Re&place Again",   FL_COMMAND + 't', replace2_cb },
    { 0 },

  { "Run F5", FL_F+5, (Fl_Callback *)run_cb },
  { "Debug F8", FL_F+8, debug_cb },
  { "&Breakpoint", FL_COMMAND + FL_SHIFT + 'b', breakpoint_cb },
  { "Stop Debug", 0, stop_debug_cb },
  { 0 }
};

Fl_Window* new_view() {
  EditorWindow* w = new EditorWindow(660, 400, title);
    w->begin();
    Fl_Menu_Bar* m = new Fl_Menu_Bar(0, 0, 660, 30);
    m->copy(menuitems, w);
    w->editor = new Fl_Text_Editor(0, 30, 660, 370);
    w->editor->textfont(FL_COURIER);
    w->editor->textsize(TS);
  //w->editor->wrap_mode(Fl_Text_Editor::WRAP_AT_BOUNDS, 250);
    w->editor->buffer(textbuf);
    w->editor->highlight_data(stylebuf, styletable,
                              sizeof(styletable) / sizeof(styletable[0]),
			      'A', style_unfinished_cb, 0);
  textbuf->text();
  style_init();
  w->end();
  w->resizable(w->editor);
  w->callback((Fl_Callback *)close_cb, w);

  textbuf->add_modify_callback(style_update, w->editor);
  textbuf->add_modify_callback(changed_cb, w);
  textbuf->call_modify_callbacks();
  num_windows++;
  return w;
}

void cb(const char *fname) {
  load_file(fname, -1);
}

void printfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stdout, s, vl);
	va_end(vl);
}

void errorfunc(HSQUIRRELVM v,const SQChar *s,...)
{
	va_list vl;
	va_start(vl, s);
	scvprintf(stderr, s, vl);
	va_end(vl);
}

extern "C" {
    SQRESULT sqext_register_sqfs(HSQUIRRELVM v);
    SQRESULT sqext_register_sq_zmq3(HSQUIRRELVM v);
    SQRESULT sqext_register_sq_socket(HSQUIRRELVM v);
    SQRESULT sqext_register_sq_zlib(HSQUIRRELVM v);
    SQRESULT sqext_register_mongoose(HSQUIRRELVM v);
    SQRESULT sqrat_register_importlib(HSQUIRRELVM v);
    SQRESULT sqext_register_sq_slave_vm(HSQUIRRELVM v);
    SQRESULT sqext_register_axtls (HSQUIRRELVM v);
    SQRESULT sqext_register_base64(HSQUIRRELVM v);
    SQRESULT sqext_register_Sq_Fpdf(HSQUIRRELVM v);
    SQRESULT sqext_register_SQLite3(HSQUIRRELVM v);
    SQRESULT sqext_register_mix (HSQUIRRELVM sqvm);
    SQRESULT sqext_register_rs232(HSQUIRRELVM v);
    SQRESULT sqext_register_tinyxml2(HSQUIRRELVM v);
    SQRESULT sqext_register_decimal(HSQUIRRELVM v);
    SQRESULT sqext_register_markdown(HSQUIRRELVM v);
    SQRESULT sqext_register_PostgreSQL(HSQUIRRELVM v);
    SQRESULT sqext_register_MySQL(HSQUIRRELVM v);
    SQRESULT sqext_register_Java(HSQUIRRELVM v);
    SQRESULT sqext_register_ThreadObjects(HSQUIRRELVM v);
    SQRESULT sqext_register_csv_parser (HSQUIRRELVM v);
    SQRESULT sqext_register_fltklib(HSQUIRRELVM v);
}


int sq_main_argc = 0;
char** sq_main_argv = 0;

static void sq_debug_hook(HSQUIRRELVM v, SQInteger ev_type, const SQChar *sourcename, SQInteger line, const SQChar *funcname){
    if(_stop_debug) return;
    if(_debug_breakpoint_line > 0)
    {
        if(ev_type == 'l' && line == _debug_breakpoint_line)
        {
            _debug_breakpoint_line = -1; //only once right now
        }
        else return;
    }
    const SQChar *fname = funcname ? funcname : "unknown";
    printf("evt %d %c\n", ev_type, ev_type);
    switch(ev_type){
        case 'l': {//called every line(that contains some code)
            printf("LINE line [%d] func [%s] l\n", line, fname);
            EditorWindow *w=0;
            SQInteger top = sq_gettop(v);
            sq_pushstring(v, DBG_EDITOR_WINDOW_KEY, -1);
            if(sq_getonregistrytable(v) == SQ_OK){
                sq_getuserpointer(v, -1, (SQUserPointer*)&w);
            }
            sq_settop(v, top);
            int pos = textbuf->skip_lines(0, line-1);
            textbuf->select(pos, textbuf->line_end(pos));
            if(w){
                w->editor->insert_position(pos);
                w->editor->show_insert_position();
            }
            sqstd_printcallstack(v);
        }
        break;
        case 'c': //called when a function has been called
            printf("LINE line [%d] func [%s] c\n", line, fname);
        break;
        case 'r': //called when a function returns
            printf("LINE line [%d] func [%s] r\n", line, fname);
        break;
    }
    while(_debug_wait && !_stop_debug){
        Fl::wait(0.01);
    }
    _debug_wait = true;
}

int main(int argc, char **argv) {

    sq_main_argc = argc;
    sq_main_argv = argv;

	v=sq_open(1024);
	sq_setprintfunc(v,printfunc,errorfunc);

	sq_pushroottable(v);

	sqstd_register_bloblib(v);
	sqstd_register_iolib(v);
	sqstd_register_systemlib(v);
	sqstd_register_mathlib(v);
	sqstd_register_stringlib(v);

#ifdef WITH_DAD_EXTRAS
	sqext_register_base64(v);
	sqext_register_Sq_Fpdf(v);
	sqext_register_SQLite3(v);
	sqext_register_PostgreSQL(v);
	sqext_register_MySQL(v);
	//sqext_register_dad_utils(v);
	sqext_register_mix(v);
	sqext_register_sqfs(v);
	sqext_register_sq_socket(v);
	sqext_register_axtls(v);
	sqext_register_sq_zlib(v);
	sqext_register_mongoose(v);
	sqrat_register_importlib(v);
	sqext_register_tinyxml2(v);
	sqext_register_decimal(v);
	sqext_register_markdown(v);
	//sqext_register_csv_parser(v);

	sqext_register_sq_slave_vm(v);
	//sqext_register_ThreadObjects(v);

	//sqext_register_sq_zmq3(v);
	//sqext_register_Java(v);

	sqext_register_rs232(v);
#ifdef WITH_FLTK
	sqext_register_fltklib(v);
#endif
#endif
	//aux library
	//sets error handlers
	sqstd_seterrorhandlers(v);

  textbuf = new Fl_Text_Buffer;
//textbuf->transcoding_warning_action = NULL;
  style_init();
  fl_open_callback(cb);

  Fl_Window* window = new_view();

  window->show(1, argv);
#ifndef __APPLE__
  if (argc > 1) load_file(argv[1], -1);
#endif

  int rc = Fl::run();
  sq_close(v);
  return rc;
}

//
// End of "$Id: editor.cxx 9718 2012-11-13 13:03:20Z manolo $".
//
