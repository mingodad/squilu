class PrintPreviewGroup extends Fl_Group {
  
  // Declaration of class members
  btnRefresh = null;
  btnPrinter = null;
  btnPrint = null;
  chkOnlyData = null;
  chkA4 = null;
  iNumCopies = null;
  brwPages = null;
  PreviewScroll = null;
  
  constructor(){
    base.constructor(181, 165, 770, 440, _tr("Print Preview"));
    local _x = 181, _y = 165;
    // Create member functions and widgets
    {
      local o = Fl_Group(_x + 0, _y + 0, 80, 440);
      {
        {
          local o = Fl_Button(_x + 0, _y + 5, 80, 25, _tr("Refresh"));
          btnRefresh = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(_x + 0, _y + 35, 80, 25, _tr("Printer"));
          btnPrinter = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(_x + 0, _y + 65, 80, 25, _tr("Print"));
          btnPrint = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Check_Button(_x + 30, _y + 118, 20, 25, _tr("Only Data"));
          chkOnlyData = o;
          o.align(1);
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Check_Button(_x + 30, _y + 165, 20, 25, _tr("A4"));
          chkA4 = o;
          o.align(1);
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Int_Input(_x + 10, _y + 211, 56, 24, _tr("Copies"));
          iNumCopies = o;
          o.textsize(16);
          o.align(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Hold_Browser(_x + 5, _y + 260, 70, 180, _tr("Pages"));
          brwPages = o;
          o.textsize(16);
          o.align(1);
          o.labelsize(16);
          o.end();
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
    }
    {
      local o = OrderPageGroup(_x + 90, _y + 0, 680, 440);
      PreviewScroll = o;
      o.box(FL_ENGRAVED_FRAME);
      o.color(7);
      o.end();
      Fl_Group.current().resizable(o);
    }
  }
}
