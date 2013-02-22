class HelpWindow extends MyBaseWindow {
  
  // Declaration of class members
  tabs = null;
  tabView = null;
  view_html = null;
  tabEdit = null;
  help_text = null;
  words_to_search = null;
  btnSearchHelp = null;
  btnSavehelp = null;
  btnBackward = null;
  btnTop = null;
  btnForward = null;
  btnFontSmaller = null;
  btnFontBigger = null;
  
  constructor(){
    base.constructor(76, 128, 600, 400, _tr("Help"));
    // Create member functions and widgets
    {
      local o = Fl_Tabs(5, 5, 590, 355);
      tabs = o;
      o.labelsize(16);
      {
        {
          local o = Fl_Group(5, 30, 590, 330, _tr ("View"));
          tabView = o;
          {
            {
              local o = Fl_Help_View(5, 30, 590, 330);
              view_html = o;
              o.labelsize(16);
              o.end();
            }
          }
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(5, 30, 590, 310, _tr ("Edit"));
          tabEdit = o;
          {
            {
              local o = Fl_Text_Editor_Buffered(5, 30, 590, 310);
              help_text = o;
              o.textsize(18);
              o.labelsize(18);
              Fl_Group.current().resizable(o);
            }
          }
          o.end();
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 370, 590, 25);
      {
        {
          local o = Fl_Input(34, 370, 225, 25, _tr ("@search"));
          words_to_search = o;
          o.textsize(18);
          o.labelsize(18);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Button(265, 370, 80, 25, _tr ("Search"));
          btnSearchHelp = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(350, 370, 80, 25, _tr ("Save"));
          btnSavehelp = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(435, 370, 25, 25, _tr ("@<-"));
          btnBackward = o;
          o.labelcolor(63);
        }
        {
          local o = Fl_Button(470, 370, 25, 25, _tr ("@8->"));
          btnTop = o;
          o.labelcolor(63);
        }
        {
          local o = Fl_Button(503, 370, 25, 25, _tr ("@->"));
          btnForward = o;
          o.labelcolor(63);
        }
        {
          local o = Fl_Button(536, 370, 25, 25, _tr ("F"));
          btnFontSmaller = o;
          o.labelsize(10);
          o.labelfont(1);
        }
        {
          local o = Fl_Button(569, 370, 25, 25, _tr ("F"));
          btnFontBigger = o;
          o.labelfont(1);
        }
      }
      o.end();
    }
  }
}

