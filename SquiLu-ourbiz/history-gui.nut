class HistoryGroup extends Fl_Group {
  
  // Declaration of class members
  history_choice = null;
  history_query_limit = null;
  btnViewDocHsitory = null;
  grid_history = null;
  
  constructor(){
    base.constructor(13, 33, 760, 465, _tr("History"));
    local _x = 13, _y = 33;
    // Create member functions and widgets
    {
      local o = Fl_Pack(_x + 0, _y + 0, 760, 25);
      o.type(1);
      o.labelsize(16);
      o->spacing(10);
      {
        {
          local o = Fl_Choice(_x + 0, _y + 0, 564, 25);
          history_choice = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Int_Input(_x + 574, _y + 0, 80, 25);
          history_query_limit = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          o.tooltip(_tr("Write then number of periodes you want to see"));
          o->value("50");
        }
        {
          local o = Fl_Button(_x + 664, _y + 0, 96, 25, _tr("View Doc."));
          btnViewDocHsitory = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Data_Table(_x + 0, _y + 33, 760, 432);
      grid_history = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
  }
}
