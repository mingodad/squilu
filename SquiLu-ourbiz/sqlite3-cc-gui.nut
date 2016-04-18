class Sqlite3cc_Window extends Fl_Double_Window {
  
  // Declaration of class members
  menuBar : Fl_Menu_Bar;
  menu_file_new : Submenu;
  menu_file_open : Submenu;
  menu_file_reopen : Submenu;
  menu_file_attach : Submenu;
  menu_file_close : Submenu;
  menu_file_open_csv : Submenu;
  menu_file_execute : Submenu;
  menu_file_exit : Submenu;
  menu_settings_encoding : Submenu;
  menu_settings_preferences : Submenu;
  menu_help_about : Submenu;
  menu_squilu_edit : Submenu;
  tile : Fl_Tile;
  group_db_tables : Fl_Group;
  grid_tables : Fl_Data_Table;
  iTablesFilter : Fl_Input;
  chkSytemTables : Fl_Check_Button;
  group_sql : Fl_Group;
  group_buttons : Fl_Group;
  iMaxRows : Fl_Input;
  option_query : Fl_Choice;
  menu_sql_select : MenuItem;
  menu_sql_insert : MenuItem;
  menu_sql_update : MenuItem;
  menu_sql_delete : MenuItem;
  menu_sql_schema_update : MenuItem;
  menu_sql_schema_update_norefs : MenuItem;
  menu_sql_sqlite_master_update : MenuItem;
  menu_sql_drop_table : MenuItem;
  menu_sql_dump_table : MenuItem;
  menu_sql_create_index : MenuItem;
  menu_sql_create_trigger : MenuItem;
  menu_sql_references : MenuItem;
  menu_sql_search_all_tables : MenuItem;
  menu_sql_macros : MenuItem;
  menu_sql_macros_base : MenuItem;
  menu_sql_macros_sql : MenuItem;
  menu_sql_macros_insert : MenuItem;
  menu_sql_macros_update : MenuItem;
  menu_sql_update_metadata : MenuItem;
  btnCreateQuery : Fl_Button;
  btnExecute : Fl_Button;
  btnLoad : Fl_Button;
  btnSave : Fl_Button;
  edit_queries : Fl_Text_Editor_Buffered;
  group_log : Fl_Group;
  tabsLog : Fl_Tabs;
  groupMessages : Fl_Group;
  output_messages : Fl_Output;
  groupHistory : Fl_Group;
  browser_history : Fl_Browser;
  group_bottom : Fl_Group;
  tabView : Fl_Tabs;
  groupData : Fl_Group;
  grid_data : Fl_Data_Table;
  groupRecord : Fl_Group;
  edit_record : Fl_Text_Editor_Buffered;
  viewFields : Fl_Group;
  grid_fields : Fl_Data_Table;
  viewIndexes : Fl_Group;
  gridIndexes : Fl_Data_Table;
  viewTriggers : Fl_Group;
  gridTriggers : Fl_Data_Table;
  groupSchema : Fl_Group;
  edit_schema : Fl_Text_Editor_Buffered;
  groupReferences : Fl_Group;
  edit_references : Fl_Text_Editor_Buffered;
  tabDefaults : Fl_Group;
  iDefaultHash : Fl_Input;
  iDefaultRubyStyle : Fl_Check_Button;
  iDefaultWithNL : Fl_Check_Button;
  iDefaultFieldPrefix : Fl_Input;
  iDefaultFieldSufix : Fl_Input;
  iDefaultStrip : Fl_Check_Button;
  iDefaultEscapeSQL : Fl_Check_Button;
  iDefaultFieldConvert : Fl_Check_Button;
  
  constructor(px=218, py=131, pw=780, ph=520, pl=_tr("SQLiteCC")){
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
        menu_file_reopen = o.add(_tr("File/Reopen DB"), 0);
        menu_file_attach = o.add(_tr("File/Attach DB"), 0);
        menu_file_close = o.add(_tr("File/Close DB"), 0, null, null, 128);
        menu_file_open_csv = o.add(_tr("File/Open CSV"), 0, null, null, 128);
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
          local o = Fl_Group(5, 25, 160, 280);
          group_db_tables = o;
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Data_Table(5, 25, 160, 255);
              grid_tables = o;
              o.textsize(16);
              o.labelsize(16);
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(5, 280, 160, 25);
              {
                {
                  local o = Fl_Input(5, 280, 140, 25, _tr("Filter:"));
                  iTablesFilter = o;
                  o.labeltype(FL_NO_LABEL);
                  o.when(1);
                  Fl_Group.current().resizable(o);
                }
                {
                  local o = Fl_Check_Button(147, 281, 18, 24, _tr("System tables"));
                  chkSytemTables = o;
                  o.labeltype(FL_NO_LABEL);
                  o.down_box(FL_DOWN_BOX);
                }
              }
              o.end();
            }
          }
          o.end();
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
                      o->value("50");
                    }
                    {
                      local o = Fl_Choice(370, 30, 120, 25, _tr("Query"));
                      option_query = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o.down_box(FL_BORDER_BOX);
                      {
                        menu_sql_select = o.add(_tr("select"), 0);
                        menu_sql_insert = o.add(_tr("insert"), 0);
                        menu_sql_update = o.add(_tr("update"), 0);
                        menu_sql_delete = o.add(_tr("delete"), 0);
                        menu_sql_schema_update = o.add(_tr("schema update"), 0);
                        menu_sql_schema_update_norefs = o.add(_tr("schema update norefs"), 0);
                        menu_sql_sqlite_master_update = o.add(_tr("sqlite_master update"), 0);
                        menu_sql_drop_table = o.add(_tr("drop table"), 0);
                        menu_sql_dump_table = o.add(_tr("dump table"), 0);
                        menu_sql_create_index = o.add(_tr("create index"), 0);
                        menu_sql_create_trigger = o.add(_tr("create trigger"), 0);
                        menu_sql_references = o.add(_tr("references"), 0);
                        menu_sql_search_all_tables = o.add(_tr("search all tables"), 0);
                        menu_sql_macros = o.add(_tr("sql macros"), 0);
                        menu_sql_macros_base = o.add(_tr("sql macros base"), 0);
                        menu_sql_macros_sql = o.add(_tr("sql macros sql"), 0);
                        menu_sql_macros_insert = o.add(_tr("sql macros insert"), 0);
                        menu_sql_macros_update = o.add(_tr("sql macros update"), 0);
                        menu_sql_update_metadata = o.add(_tr("sql update metadata"), 0);
                      }
                    }
                    {
                      local o = Fl_Button(500, 30, 25, 25, _tr("@->"));
                      btnCreateQuery = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(535, 30, 75, 25, _tr("Execute"));
                      btnExecute = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(620, 30, 70, 25, _tr("Load"));
                      btnLoad = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(700, 30, 70, 25, _tr("Save"));
                      btnSave = o;
                      o.labelsize(16);
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
                      o.textsize(16);
                      o.labelsize(16);
                      o.end();
                    }
                  }
                  o.end();
                  Fl_Group.current().resizable(o);
                }
                {
                  local o = Fl_Group(0, 333, 780, 187, _tr("Record"));
                  groupRecord = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(5, 333, 770, 185);
                      edit_record = o;
                      o.textsize(16);
                      o.labelsize(16);
                    }
                  }
                  o.end();
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
                  local o = Fl_Group(0, 333, 780, 187, _tr("References"));
                  groupReferences = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(5, 333, 770, 185);
                      edit_references = o;
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
    end();
  }
}

class SquiLuEditWindow extends Fl_Double_Window {
  
  // Declaration of class members
  code_editor : Fl_Text_Editor_Buffered;
  words_to_search : Fl_Input;
  btnSearch : Fl_Button;
  btnSavehelp : Fl_Button;
  btnRun : Fl_Button;
  output_editor : Fl_Text_Editor_Buffered;
  
  constructor(px=286, py=113, pw=565, ph=540, pl=_tr("SquiLu Edit")){
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
    end();
  }
}

class CreateTableWindow extends Fl_Double_Window {
  
  // Declaration of class members
  choice_database : Fl_Choice;
  table_name : Fl_Input;
  grid_fields : Fl_Data_Table;
  btnAddColumn : Fl_Button;
  btnDeleteColumn : Fl_Button;
  btnEditColumn : Fl_Button;
  btnUpColumn : Fl_Button;
  btnDownColumn : Fl_Button;
  table_constraints : Fl_Input;
  btnCheck : Fl_Button;
  btnOk : Fl_Button;
  btnCancel : Fl_Button;
  output_editor : Fl_Text_Editor_Buffered;
  
  constructor(px=648, py=270, pw=565, ph=455, pl=_tr("Create Table")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Choice(105, 5, 170, 25, _tr("Database:"));
      choice_database = o;
      o.down_box(FL_BORDER_BOX);
    }
    {
      local o = Fl_Input(330, 5, 230, 25, _tr("Table:"));
      table_name = o;
    }
    {
      local o = Fl_Data_Table(5, 35, 425, 195);
      grid_fields = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Button(435, 37, 125, 33, _tr("Add Column"));
      btnAddColumn = o;
      o.labelsize(16);
    }
    {
      local o = Fl_Button(435, 77, 125, 33, _tr("Delete Column"));
      btnDeleteColumn = o;
      o.labelsize(16);
    }
    {
      local o = Fl_Button(435, 117, 125, 33, _tr("Edit Column"));
      btnEditColumn = o;
      o.labelsize(16);
    }
    {
      local o = Fl_Button(435, 157, 125, 33, _tr("Up"));
      btnUpColumn = o;
      o.labelsize(16);
    }
    {
      local o = Fl_Button(435, 197, 125, 33, _tr("Down"));
      btnDownColumn = o;
      o.labelsize(16);
    }
    {
      local o = Fl_Multiline_Input(5, 255, 555, 75, _tr("Table Constraints"));
      table_constraints = o;
      o.align(5);
    }
    {
      local o = Fl_Group(5, 336, 554, 39);
      o.box(FL_ENGRAVED_BOX);
      o.labeltype(FL_NO_LABEL);
      {
        {
          local o = Fl_Button(10, 340, 115, 30, _tr("Check"));
          btnCheck = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(310, 340, 115, 30, _tr("OK"));
          btnOk = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(435, 340, 115, 30, _tr("Cancel"));
          btnCancel = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Text_Editor_Buffered(5, 382, 555, 71);
      output_editor = o;
      o.textsize(18);
      o.labelsize(18);
    }
    end();
  }
}

class CreateTableFieldWindow extends Fl_Double_Window {
  
  // Declaration of class members
  field_name : Fl_Input;
  field_type : Fl_Choice;
  group_constraints : Fl_Group;
  field_primary_key : Fl_Check_Button;
  field_unique : Fl_Check_Button;
  field_not_null : Fl_Check_Button;
  field_default : Fl_Check_Button;
  field_default_value : Fl_Input;
  field_check : Fl_Input;
  field_collate : Fl_Choice;
  btnOk : Fl_Button;
  btnCancel : Fl_Button;
  
  constructor(px=665, py=216, pw=330, ph=375, pl=_tr("Create/Edit Table Field")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Input(90, 10, 230, 25, _tr("Name:"));
      field_name = o;
    }
    {
      local o = Fl_Choice(90, 40, 230, 25, _tr("Data type:"));
      field_type = o;
      o.down_box(FL_BORDER_BOX);
    }
    {
      local o = Fl_Group(5, 90, 320, 240, _tr("Constraints"));
      group_constraints = o;
      o.box(FL_ENGRAVED_BOX);
      o.align(5);
      {
        {
          local o = Fl_Check_Button(10, 95, 110, 25, _tr("Primary Key"));
          field_primary_key = o;
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Check_Button(10, 120, 110, 25, _tr("Unique"));
          field_unique = o;
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Check_Button(10, 145, 110, 25, _tr("Not Null"));
          field_not_null = o;
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Check_Button(10, 170, 110, 25, _tr("Default"));
          field_default = o;
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Input(85, 170, 230, 25);
          field_default_value = o;
          o.labeltype(FL_NO_LABEL);
        }
        {
          local o = Fl_Input(85, 200, 230, 25, _tr("Check:"));
          field_check = o;
        }
        {
          local o = Fl_Choice(85, 230, 230, 25, _tr("Collate:"));
          field_collate = o;
          o.down_box(FL_BORDER_BOX);
        }
      }
      o.end();
    }
    {
      local o = Fl_Group(5, 336, 320, 39);
      o.box(FL_ENGRAVED_BOX);
      o.labeltype(FL_NO_LABEL);
      {
        {
          local o = Fl_Button(10, 340, 115, 30, _tr("OK"));
          btnOk = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(205, 340, 115, 30, _tr("Cancel"));
          btnCancel = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    end();
  }
}

