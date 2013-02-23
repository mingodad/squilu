function _tr(str) {return str;}

class Fl_Data_Table extends Flv_Data_Table {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Multiline_Output extends Fl_Output {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		type(4);
	}
}

class Sqlite3cc_Window extends Fl_Double_Window {
  
  // Declaration of class members
  menuBar = null;
  menu_file_new = null;
  menu_file_open = null;
  menu_file_attach = null;
  menu_file_close = null;
  menu_file_execute = null;
  menu_file_exit = null;
  menu_settings_encoding = null;
  menu_settings_preferences = null;
  menu_help_about = null;
  menu_squilu_edit = null;
  tile = null;
  grid_tables = null;
  group_sql = null;
  group_buttons = null;
  iMaxRows = null;
  option_query = null;
  btnCreateQuery = null;
  btnExecute = null;
  btnLoad = null;
  btnSave = null;
  edit_queries = null;
  group_log = null;
  tabsLog = null;
  groupMessages = null;
  output_messages = null;
  groupHistory = null;
  browser_history = null;
  group_bottom = null;
  tabView = null;
  groupData = null;
  grid_data = null;
  viewFields = null;
  grid_fields = null;
  viewIndexes = null;
  gridIndexes = null;
  viewTriggers = null;
  gridTriggers = null;
  groupSchema = null;
  edit_schema = null;
  tabDefaults = null;
  iDefaultHash = null;
  iDefaultRubyStyle = null;
  iDefaultWithNL = null;
  iDefaultFieldPrefix = null;
  iDefaultFieldSufix = null;
  iDefaultStrip = null;
  iDefaultEscapeSQL = null;
  iDefaultFieldConvert = null;
  
  constructor(px=103, py=78, pw=780, ph=520, pl=_tr("SQLiteCC")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Menu_Bar(0, 0, 780, 25);
      menuBar = o;
      o.textsize(16);
      o.labelsize(16);
      {
        //menu_file File
        menu_file_new = o.add(_tr("File/New DB"), 262254);
        menu_file_open = o.add(_tr("File/Open DB"), 0);
        menu_file_attach = o.add(_tr("File/Attach DB"), 0);
        menu_file_close = o.add(_tr("File/Close DB"), 0, null, null, 128);
        menu_file_execute = o.add(_tr("File/Execute"), 0, null, null, 128);
        menu_file_exit = o.add(_tr("File/Exit"), 0);
        //menu_settings Settings
        //menu_settings_encoding Settings/Encoding
        menu_settings_encoding = o.add(_tr("Settings/Encoding/Unicode UTF-8"), 0, null, null, 12);
        menu_settings_encoding = o.add(_tr("Settings/Encoding/Unicode UTF-16"), 0, null, null, 8);
        menu_settings_encoding = o.add(_tr("Settings/Encoding/ISO Latin2"), 0, null, null, 8);
        menu_settings_encoding = o.add(_tr("Settings/Encoding/Windows 1251"), 0, null, null, 8);
        menu_settings_preferences = o.add(_tr("Settings/Preferences"), 0);
        //menu_help Help
        menu_help_about = o.add(_tr("Help/About"), 0);
        menu_squilu_edit = o.add(_tr("Help/SquiLu Edit"), 0);
      }
    }
    {
      local o = Fl_Tile(0, 25, 780, 495);
      tile = o;
      {
        {
          local o = Fl_Data_Table(5, 25, 160, 280);
          grid_tables = o;
        }
        {
          local o = Fl_Group(165, 25, 615, 280);
          {
            {
              local o = Fl_Group(165, 25, 615, 170);
              group_sql = o;
              o.box(FL_DOWN_BOX);
              o.labelsize(16);
              o.labeltype(FL_NO_LABEL);
              {
                {
                  local o = Fl_Group(165, 25, 615, 35);
                  group_buttons = o;
                  o.box(FL_DOWN_FRAME);
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Int_Input(265, 30, 49, 25, _tr("Max Rows"));
                      iMaxRows = o;
                      o.textsize(16);
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Choice(370, 30, 120, 25, _tr("Query"));
                      option_query = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o.down_box(FL_BORDER_BOX);
                    }
                    {
                      local o = Fl_Button(500, 30, 25, 25, _tr("@->"));
                      btnCreateQuery = o;
                      o.labelsize(16);
                      o.callback(function(sender, udata){
                        dispatch_func(create_sql_query, self)
                      });
                    }
                    {
                      local o = Fl_Button(535, 30, 75, 25, _tr("Execute"));
                      btnExecute = o;
                      o.labelsize(16);
                      o.callback(function(sender, udata){
                        dispatch_func(execute_sql, self)
                      });
                    }
                    {
                      local o = Fl_Button(620, 30, 70, 25, _tr("Load"));
                      btnLoad = o;
                      o.labelsize(16);
                      o.callback(function(sender, udata){
                        dispatch_func(load_sql_from_file, self)
                      });
                    }
                    {
                      local o = Fl_Button(700, 30, 70, 25, _tr("Save"));
                      btnSave = o;
                      o.labelsize(16);
                      o.callback(function(sender, udata){
                        dispatch_func(save_sql_to_file, self)
                      });
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Text_Editor_Buffered(165, 60, 610, 135);
                  edit_queries = o;
                  o.textsize(16);
                  o.box(FL_NO_BOX);
                  o.labelsize(15);
                  Fl_Group.current().resizable(o);
                }
              }
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(165, 195, 615, 110);
              group_log = o;
              o.box(FL_DOWN_BOX);
              {
                {
                  local o = Fl_Tabs(165, 195, 610, 110);
                  tabsLog = o;
                  o.align(2);
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Group(165, 220, 610, 85, _tr("Messages"));
                      groupMessages = o;
                      o.labelsize(16);
                      {
                        {
                          local o = Fl_Multiline_Output(165, 220, 610, 85);
                          output_messages = o;
                          o.textsize(16);
                          o.labelsize(16);
                        }
                      }
                      o.end();
                      Fl_Group.current().resizable(o);
                    }
                    {
                      local o = Fl_Group(165, 220, 610, 85, _tr("History"));
                      groupHistory = o;
                      o.labelsize(16);
                      {
                        {
                          local o = Fl_Hold_Browser(165, 220, 610, 85);
                          browser_history = o;
                          o.textsize(16);
                          o.labelsize(16);
                          o.callback(function(sender, udata){
                            dispatch_func(history_to_edit_query, self)
                          });
                          o.end();
                        }
                      }
                      o.end();
                    }
                  }
                  o.end();
                }
              }
              o.end();
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(0, 305, 780, 215);
          group_bottom = o;
          o.box(FL_DOWN_BOX);
          {
            {
              local o = Fl_Tabs(0, 305, 780, 215);
              tabView = o;
              o.labelsize(16);
              {
                {
                  local o = Fl_Group(0, 333, 780, 187, _tr("Data"));
                  groupData = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Data_Table(0, 333, 780, 187);
                      grid_data = o;
                    }
                  }
                  o.end();
                  Fl_Group.current().resizable(o);
                }
                {
                  local o = Fl_Group(0, 333, 780, 187, _tr("Fields"));
                  viewFields = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Data_Table(0, 333, 780, 187);
                      grid_fields = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o.end();
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(0, 333, 780, 187, _tr("Indexes"));
                  viewIndexes = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Data_Table(0, 333, 780, 187);
                      gridIndexes = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o.end();
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(0, 333, 780, 187, _tr("Triggers"));
                  viewTriggers = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Data_Table(0, 333, 780, 187);
                      gridTriggers = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o.end();
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(0, 333, 780, 187, _tr("Schema"));
                  groupSchema = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(5, 333, 770, 185);
                      edit_schema = o;
                      o.textsize(16);
                      o.labelsize(16);
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(0, 333, 780, 187, _tr("Defaults"));
                  tabDefaults = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Input(145, 375, 135, 24, _tr("Hash for queries"));
                      iDefaultHash = o;
                      o.textsize(16);
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Check_Button(145, 414, 135, 25, _tr("Ruby Style"));
                      iDefaultRubyStyle = o;
                      o.labelsize(16);
                      o.down_box(FL_DOWN_BOX);
                    }
                    {
                      local o = Fl_Check_Button(145, 439, 135, 25, _tr("Use new lines"));
                      iDefaultWithNL = o;
                      o.labelsize(16);
                      o.down_box(FL_DOWN_BOX);
                    }
                    {
                      local o = Fl_Input(410, 375, 135, 25, _tr("Field prefix"));
                      iDefaultFieldPrefix = o;
                      o.textsize(15);
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Input(410, 405, 135, 25, _tr("Field sufix"));
                      iDefaultFieldSufix = o;
                      o.textsize(16);
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Check_Button(410, 435, 135, 25, _tr("strip"));
                      iDefaultStrip = o;
                      o.labelsize(16);
                      o.down_box(FL_DOWN_BOX);
                    }
                    {
                      local o = Fl_Check_Button(410, 459, 135, 25, _tr("escape sql"));
                      iDefaultEscapeSQL = o;
                      o.labelsize(16);
                      o.down_box(FL_DOWN_BOX);
                    }
                    {
                      local o = Fl_Check_Button(410, 485, 135, 25, _tr("convert (to_i)"));
                      iDefaultFieldConvert = o;
                      o.labelsize(16);
                      o.down_box(FL_DOWN_BOX);
                    }
                  }
                  o.end();
                }
              }
              o.end();
              Fl_Group.current().resizable(o);
            }
          }
          o.end();
        }
      }
      o.end();
    }
  }
}

class SquiLuEditWindow extends Fl_Double_Window {
  
  // Declaration of class members
  code_editor = null;
  words_to_search = null;
  btnSearch = null;
  btnSavehelp = null;
  btnRun = null;
  output_editor = null;
  
  constructor(px=286, py=113, pw=569, ph=540, pl=_tr("Lua Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Text_Editor_Buffered(10, 10, 550, 308);
      code_editor = o;
      o.textsize(18);
      o.labelsize(18);
    }
    {
      local o = Fl_Group(10, 350, 549, 35);
      {
        {
          local o = Fl_Input(10, 350, 150, 33, _tr("Words to search"));
          words_to_search = o;
          o.textsize(18);
          o.align(5);
          o.labelsize(18);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Button(170, 350, 115, 33, _tr("Search"));
          btnSearch = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(444, 350, 115, 33, _tr("Save"));
          btnSavehelp = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(310, 350, 115, 33, _tr("Run"));
          btnRun = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Text_Editor_Buffered(10, 397, 550, 133);
      output_editor = o;
      o.textsize(18);
      o.labelsize(18);
      Fl_Group.current().resizable(o);
    }
  }
}

function menuBar_cb(sender, udata){
	this = sender->window();
	fl_alert(format("%d = %s", sender.value(), sender.text()));
	//print(sender, udata, sender.value(), menu_file_exit);
}

local win = new Sqlite3cc_Window();
win.menuBar.callback(menuBar_cb);
//local win = new SalesTaxRatesEditWindow();
win->resizable(win);
win->show_main();

Fl.run();