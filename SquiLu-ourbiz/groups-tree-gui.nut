class GroupsListEditWindow extends EditWindow {
  
// Declaration of class members
  tree = null;
  bottomGroup = null;
  db_group_description = null;
  db_group_parent_id = null;
  dbAction = null;
  btnDbAction = null;
  db_group_id = null;
  btnWrapNotes = null;
  db_group_notes = null;
  
  constructor(px=328, py=91, pw=800, ph=560, pl=_tr("Groups List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = MyTree_Browser(10, 10, 780, 360);
      tree = o;
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      //o->showroot(0);
      //o->select_focus(1);
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(0, 375, 800, 185);
      bottomGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.color(246);
      {
        {
          local o = Fl_Input(98, 408, 576, 28, _tr("Description"));
          db_group_description = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("group", "description", db_group_description);
        }
        {
          local o = Fl_Int_Input(684, 408, 106, 28, _tr("Parent"));
          db_group_parent_id = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("group", "parent_id", db_group_parent_id);
        }
        {
          local o = Fl_Choice_dbAction(684, 484, 106, 28);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(686, 522, 104, 28, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Output(10, 408, 78, 28, _tr("ID"));
          db_group_id = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("group", "id", db_group_id);
        }
        {
          local o = Fl_Check_Button(95, 440, 115, 25, _tr("Wrap lines"));
          btnWrapNotes = o;
          o.labelsize(16);
          o.value(1);
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = MyFl_Text_Editor(10, 462, 660, 88, _tr("Notes"));
          db_group_notes = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("group", "notes", db_group_notes);
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
    }
  }
}
