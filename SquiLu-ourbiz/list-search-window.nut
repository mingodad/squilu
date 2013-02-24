class ListSearch extends MyBaseWindow {

  // Declaration of class members
  topGroup = null;
  group_filter = null;
  query_limit = null;
  btnNotes = null;
  middleGroup = null;
  grid = null;
  bottomGroup = null;
  pack_search_options = null;
  search_str = null;
  btnSearch = null;
  btnSelect = null;
  btnUpdate = null;
  btnInsert = null;
  btnThumbImage = null;
  pack_search_options2 = null;
  
  constructor(px=132, py=118, pw=800, ph=560, pl=_tr("List Search")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Group(0, 0, 800, 35);
      topGroup = o;
      o.labelsize(16);
      {
        {
          local o = Flu_Combo_List(5, 5, 675, 25);
          group_filter = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
        }
        {
          local o = Fl_Int_Input(690, 6, 60, 25);
          query_limit = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          o->value("50");
        }
        {
          local o = Fl_Button(760, 6, 35, 25, _tr("?"));
          btnNotes = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Group(0, 41, 800, 409);
      middleGroup = o;
      o.labelsize(16);
      {
        {
          local o = Fl_Data_Table(5, 41, 790, 389);
          grid = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          o.end();
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(4, 440, 790, 115);
      bottomGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.color(246);
      {
        {
          local o = Fl_Pack(14, 450, 320, 25);
          pack_search_options = o;
          o.type(1);
          o.labelsize(16);
          o->spacing(10);
          o.end();
        }
        {
          local o = Fl_Input(35, 485, 299, 25, _tr("@search"));
          search_str = o;
          o.textsize(16);
          o.when(8);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(345, 485, 100, 25, _tr("Search"));
          btnSearch = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(14, 520, 100, 25, _tr("Select"));
          btnSelect = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(124, 520, 100, 25, _tr("Update"));
          btnUpdate = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(234, 520, 100, 25, _tr("Insert"));
          btnInsert = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Image_Box(663, 445, 125, 105);
          btnThumbImage = o;
          o.box(FL_NO_BOX);
          o.align(80);
        }
        {
          local o = Fl_Box(650, 445, 5, 105);
          o.align(80);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Pack(345, 520, 300, 25);
          pack_search_options2 = o;
          o.type(1);
          o.labelsize(16);
          o->spacing(10);
          o.end();
        }
      }
      o.end();
    }
  }
}
