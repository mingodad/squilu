function _tr(str){ return str;}

class Fl_Data_Table extends Flv_Table {
	_forPrint = null;
	
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
		_forPrint=false;
	}
}

class Fl_Box_ClearLabel extends Fl_Box {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Flu_Combo_List extends Fl_Box {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Image_Box extends Fl_Box {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}


class Fl_Choice_Int extends Fl_Button {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Float_Input_Fmt extends Fl_Float_Input {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class My_Fl_Float_Input extends Fl_Float_Input {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}


class My_Fl_Return_Button extends Fl_Button {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Choice_Str extends Fl_Button {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Flu_Combo_Box extends Fl_Button {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}


function add_input_field_to_map(tbl, fldname, fld){
}
	
class ListSearchWindow extends Fl_Window {
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
  
  constructor(){
    base.constructor(132, 118, 800, 560);
    // Create member functions and widgets
    {
      local o = Fl_Group(0, 0, 800, 35);
      topGroup = o;
      o.labelsize(16);
      {
        {
          local o = Flu_Combo_Tree(5, 5, 675, 25);
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
          local o = Fl_Button(760, 6, 35, 25, _tr ("?"));
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
          local o = Fl_Input(35, 485, 299, 25, _tr ("@search"));
          search_str = o;
          o.textsize(16);
          o.when(8);
          o.labelsize(16);
          o.callback(function(sender, udata){
            on_search();
          });
        }
        {
          local o = Fl_Button(345, 485, 100, 25, _tr ("Search"));
          btnSearch = o;
          o.labelsize(16);
          o.callback(function(sender, udata){
            on_search();
          });
        }
        {
          local o = Fl_Button(14, 520, 100, 25, _tr ("Select"));
          btnSelect = o;
          o.labelsize(16);
          o.callback(function(sender, udata){
            on_select();
          });
        }
        {
          local o = Fl_Button(124, 520, 100, 25, _tr ("Update"));
          btnUpdate = o;
          o.labelsize(16);
          o.callback(function(sender, udata){
            on_update();
          });
        }
        {
          local o = Fl_Button(234, 520, 100, 25, _tr ("Insert"));
          btnInsert = o;
          o.labelsize(16);
          o.callback(function(sender, udata){
            on_insert();
          });
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
    group_filter->textfont(group_filter->labelfont());
    group_filter->textsize(group_filter->labelsize());

  };
  
}

//local win = new MainWindow(10, 50, 330, 320, "OURBIZ");
local win = new ListSearchWindow();
win->resizable(win);
win->show_main();

local tree = win->group_filter->tree();
tree->show_root(true);
tree->auto_branches(true);
tree->show_branches(true);
tree->all_branches_always_open(true);
local node = tree->get_root();
print(node)
local node2 = tree->add(node, "DAD");
print(node2)
local node3 = tree->add(node2, "TAIS");
print(node3)
local node22 = tree->add(node, "LIINUX");
print(node22)

math.number_format_set_dec_point(",");
math.number_format_set_thousand_sep(".");
Fl_Input.default_number_format("\0x02,.");
//Fl:scheme("plastic");
Fl.scheme("gtk+");
//use partial match to find verdana font
Fl.visual(FL_RGB);
//allow arrow keys navigation
Fl.option(Fl.OPTION_ARROW_FOCUS, true);

Fl.run();
