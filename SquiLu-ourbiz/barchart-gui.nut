class BarChartGroup extends Fl_Group {
  
  // Declaration of class members
  pack = null;
  btnShowChart = null;
  periodes_to_show = null;
  btn_periode_is_years = null;
  btn_periode_is_months = null;
  btn_periode_is_weeks = null;
  btn_periode_is_days = null;
  chkOpt = null;
  bar_chart = null;
  
  constructor(){
    base.constructor(288, 260, 760, 460, _tr("BarChartGroup"));
    local _x = 288, _y = 260;
    // Create member functions and widgets
    {
      local o = Fl_Pack(_x + 0, _y + 0, 760, 25);
      pack = o;
      o.type(1);
      o.labelsize(16);
      o->spacing(10);
      {
        {
          local o = Fl_Button(_x + 0, _y + 0, 96, 25, _tr("Show"));
          btnShowChart = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Int_Input(_x + 106, _y + 0, 80, 25);
          periodes_to_show = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          o.tooltip(_tr("Write then number of periodes you want to see"));
          o->value("12");
        }
        {
          local o = Fl_Round_Button(_x + 196, _y + 0, 110, 25, _tr("Years"));
          btn_periode_is_years = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
        }
        {
          local o = Fl_Round_Button(_x + 316, _y + 0, 110, 25, _tr("Months"));
          btn_periode_is_months = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
          o->setonly();
        }
        {
          local o = Fl_Round_Button(_x + 436, _y + 0, 110, 25, _tr("Weeks"));
          btn_periode_is_weeks = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
        }
        {
          local o = Fl_Round_Button(_x + 556, _y + 0, 110, 25, _tr("Days"));
          btn_periode_is_days = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
        }
        {
          local o = Fl_Check_Button(_x + 660, _y + 5, 64, 15);
          chkOpt = o;
          o.down_box(FL_DOWN_BOX);
        }
      }
      o.end();
    }
    {
      local o = Fl_Bar_Chart(_x + 0, _y + 30, 760, 430);
      bar_chart = o;
      o.align(5);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.color(7);
      Fl_Group.current().resizable(o);
    }
  }
}
