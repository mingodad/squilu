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
	
//class BaseReportA4 extends Fl_Group {
class ListSearch extends MyBaseWindow {
	topGroup=null;
	group_filter=null;
	query_limit=null;
	btnNotes=null;
	middleGroup=null;
	grid=null;
	bottomGroup=null;
	pack_search_options=null;
	search_str=null;
	btnSearch=null;
	btnSelect=null;
	btnUpdate=null;
	btnInsert=null;
	btnThumbImage=null;
	pack_search_options2=null;

constructor() {
	base.constructor(10, 50, 800, 560, _tr("List Search"));
begin();
this->box(FL_FLAT_BOX);
this->color(FL_BACKGROUND_COLOR);
this->selection_color(FL_BACKGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(16);
this->labelcolor(FL_FOREGROUND_COLOR);
this->align((FL_ALIGN_TOP));
this->when(FL_WHEN_RELEASE);
{ topGroup = new Fl_Group(0, 0, 800, 35);
  topGroup->labelsize(16);
  { group_filter = new Flu_Combo_Box(5, 5, 675, 25);
    group_filter->resize(5, 5, 675, 25);
    group_filter->box(FL_DOWN_BOX);
    group_filter->color(FL_BACKGROUND2_COLOR);
    group_filter->selection_color(FL_SELECTION_COLOR);
    group_filter->labeltype(FL_NO_LABEL);
    group_filter->labelfont(0);
    group_filter->labelsize(16);
    group_filter->labelcolor(FL_FOREGROUND_COLOR);
    group_filter->textsize(16);
    group_filter->align((FL_ALIGN_LEFT));
    group_filter->when(FL_WHEN_RELEASE);
  } // Flu_Combo_Box* group_filter
  { local o = query_limit = new Fl_Int_Input(690, 6, 60, 25);
    query_limit->type(2);
    query_limit->labeltype(FL_NO_LABEL);
    query_limit->labelsize(16);
    query_limit->textsize(16);
    o->value("50");
  } // Fl_Int_Input* query_limit
  { btnNotes = new Fl_Button(760, 6, 35, 25, _tr("?"));
    btnNotes->labelsize(16);
  } // Fl_Button* btnNotes
  topGroup->end();
} // Fl_Group* topGroup
{ middleGroup = new Fl_Group(0, 41, 800, 409);
  middleGroup->labelsize(16);
  { grid = new Fl_Data_Table(5, 41, 790, 389);
    grid->resize(5, 41, 790, 389);
    grid->type(3);
    grid->box(FL_NO_BOX);
    grid->color(FL_BACKGROUND2_COLOR);
    grid->selection_color(FL_SELECTION_COLOR);
    grid->labeltype(FL_NO_LABEL);
    grid->labelfont(0);
    grid->labelsize(16);
    grid->labelcolor(FL_FOREGROUND_COLOR);
    grid->textsize(16);
    grid->align((FL_ALIGN_BOTTOM));
    grid->when(FL_WHEN_RELEASE_ALWAYS);
    Fl_Group.current()->resizable(grid);
  } // Fl_Data_Table* grid
  middleGroup->end();
  Fl_Group.current()->resizable(middleGroup);
} // Fl_Group* middleGroup
{ bottomGroup = new Fl_Group(4, 440, 790, 115);
  bottomGroup->box(FL_ENGRAVED_BOX);
  bottomGroup->color(246);
  bottomGroup->labelsize(16);
  { local o = pack_search_options = new Fl_Pack(14, 450, 320, 25);
    pack_search_options->type(1);
    pack_search_options->labelsize(16);
    o->spacing(10);
    pack_search_options->end();
  } // Fl_Pack* pack_search_options
  { search_str = new Fl_Input(35, 485, 299, 25, _tr("@search"));
    search_str->resize(35, 485, 299, 25);
    search_str->label(_tr("@search"));
    search_str->labelsize(16);
    search_str->textsize(16);
    search_str->callback(cb_search_str);
    search_str->when(FL_WHEN_ENTER_KEY);
  } // Fl_Input* search_str
  { btnSearch = new Fl_Button(345, 485, 100, 25, _tr("Search"));
    btnSearch->labelsize(16);
    btnSearch->callback(cb_btnSearch);
  } // Fl_Button* btnSearch
  { btnSelect = new Fl_Button(14, 520, 100, 25, _tr("Select"));
    btnSelect->labelsize(16);
    btnSelect->callback(cb_btnSelect);
  } // Fl_Button* btnSelect
  { btnUpdate = new Fl_Button(124, 520, 100, 25, _tr("Update"));
    btnUpdate->labelsize(16);
    btnUpdate->callback(cb_btnUpdate);
  } // Fl_Button* btnUpdate
  { btnInsert = new Fl_Button(234, 520, 100, 25, _tr("Insert"));
    btnInsert->labelsize(16);
    btnInsert->callback(cb_btnInsert);
  } // Fl_Button* btnInsert
  { btnThumbImage = new Fl_Image_Box(663, 445, 125, 105);
    btnThumbImage->box(FL_NO_BOX);
    btnThumbImage->color(FL_BACKGROUND_COLOR);
    btnThumbImage->selection_color(FL_BACKGROUND_COLOR);
    btnThumbImage->labeltype(FL_NORMAL_LABEL);
    btnThumbImage->labelfont(0);
    btnThumbImage->labelsize(14);
    btnThumbImage->labelcolor(FL_FOREGROUND_COLOR);
    btnThumbImage->align((FL_ALIGN_CLIP|FL_ALIGN_INSIDE));
    btnThumbImage->when(FL_WHEN_RELEASE);
  } // Fl_Image_Box* btnThumbImage
  { local o = new Fl_Box(650, 445, 5, 105);
    o->labeltype(FL_NO_LABEL);
    o->labelsize(16);
    o->align((FL_ALIGN_CLIP|FL_ALIGN_INSIDE));
    Fl_Group.current()->resizable(o);
  } // Fl_Box* o
  { local o = pack_search_options2 = new Fl_Pack(345, 520, 300, 25);
    pack_search_options2->type(1);
    pack_search_options2->labelsize(16);
    o->spacing(10);
    pack_search_options2->end();
  } // Fl_Pack* pack_search_options2
  bottomGroup->end();
} // Fl_Group* bottomGroup
group_filter->textfont(group_filter->labelfont());
group_filter->textsize(group_filter->labelsize());
end();
}

function cb_search_str(){}
function cb_btnSearch(){}
function cb_btnSelect(){}
function cb_btnUpdate(){}
function cb_btnInsert(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Order Edit", EditOrderWindow);
}
}
