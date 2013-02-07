class CalendarWindow extends MyBaseWindow {
  
  // Declaration of class members
  label_month = null;
  btnMonth1 = null;
  btnMonth2 = null;
  btnMonth3 = null;
  btnMonth4 = null;
  btnMonth5 = null;
  btnMonth6 = null;
  btnMonth7 = null;
  btnMonth8 = null;
  btnMonth9 = null;
  btnMonth10 = null;
  btnMonth11 = null;
  btnMonth12 = null;
  grid = null;
  btnPrevYear = null;
  btnPrevMonth = null;
  btnToday = null;
  btnSelect = null;
  btnNextMonth = null;
  btnNextYear = null;
  
  constructor(){
    base.constructor(732, 184, 350, 305);
    // Create member functions and widgets
    {
      local o = Fl_Box(5, 5, 340, 25, _tr("Calendar"));
      label_month = o;
      o.labelsize(16);
      o.box(FL_ENGRAVED_BOX);
      o.color(7);
    }
    {
      local o = Fl_Group(6, 35, 340, 25);
      o.labelsize(16);
      {
        {
          local o = Fl_Button(6, 35, 25, 25, _tr("1"));
          btnMonth1 = o;
        }
        {
          local o = Fl_Button(34, 35, 25, 25, _tr("2"));
          btnMonth2 = o;
        }
        {
          local o = Fl_Button(63, 35, 25, 25, _tr("3"));
          btnMonth3 = o;
        }
        {
          local o = Fl_Button(91, 35, 25, 25, _tr("4"));
          btnMonth4 = o;
        }
        {
          local o = Fl_Button(120, 35, 25, 25, _tr("5"));
          btnMonth5 = o;
        }
        {
          local o = Fl_Button(148, 35, 25, 25, _tr("6"));
          btnMonth6 = o;
        }
        {
          local o = Fl_Button(177, 35, 25, 25, _tr("7"));
          btnMonth7 = o;
        }
        {
          local o = Fl_Button(205, 35, 25, 25, _tr("8"));
          btnMonth8 = o;
        }
        {
          local o = Fl_Button(234, 35, 25, 25, _tr("9"));
          btnMonth9 = o;
        }
        {
          local o = Fl_Button(262, 35, 25, 25, _tr("10"));
          btnMonth10 = o;
        }
        {
          local o = Fl_Button(291, 35, 25, 25, _tr("11"));
          btnMonth11 = o;
        }
        {
          local o = Fl_Button(320, 35, 25, 25, _tr("12"));
          btnMonth12 = o;
        }
      }
      o.end();
    }
    {
      local o = Fl_Data_Table_Calendar(5, 65, 340, 202);
      grid = o;
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
    }
    {
      local o = Fl_Group(5, 275, 340, 25);
      o.labelsize(16);
      {
        {
          local o = Fl_Repeat_Button(5, 275, 50, 25, _tr("@<<"));
          btnPrevYear = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to previous year"));
        }
        {
          local o = Fl_Repeat_Button(63, 275, 50, 25, _tr("@<"));
          btnPrevMonth = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to previous month"));
        }
        {
          local o = Fl_Button(121, 275, 50, 25, _tr("@<->"));
          btnToday = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to today date"));
        }
        {
          local o = Fl_Button(179, 275, 50, 25, _tr("@circle"));
          btnSelect = o;
          o.labelsize(16);
          o.tooltip(_tr("Select current date"));
        }
        {
          local o = Fl_Repeat_Button(237, 275, 50, 25, _tr("@>"));
          btnNextMonth = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to next month"));
        }
        {
          local o = Fl_Repeat_Button(295, 275, 50, 25, _tr("@>>"));
          btnNextYear = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to next year"));
        }
      }
      o.end();
    }
  }
}

