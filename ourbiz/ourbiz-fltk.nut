dofile("ourbiz-client.nut");
local appServer = AppServer.getAppServer();
appServer.credentials("mingote", "tr14pink");
appServer.connect("localhost", 8855);

function _tr(str){ return str;}

function create_popup_menu_for (wgt, yOffset=0, options=null){
	local parent = wgt->parent();
	if(!parent){
		parent = wgt->as_group();
		if(!parent) return null;
	}
	local o = new Fl_Menu_Button(wgt->x(), wgt->y()+yOffset, wgt->w(), wgt->h()-yOffset);
	o->labelsize(wgt->labelsize());
	o->textsize(wgt->labelsize());

	if(options) for(local i=0, len= options.size(); i < len; ++i) o->add(opt[i]);
	o->type(Fl_Menu_Button.POPUP3);

	if(parent) parent->add(o);
	else if(wgt->as_group()) wgt->as_group()->add(o);
	return o;
}

function clear_image_box(btn)
{
    if (btn->thumbIMG)
    {
        btn->hide();
        btn->image(0);
        btn->thumbIMG = null;
    }
}

function button_show_image(btn, image_id, mime_type, image, image_size, window)
{
    btn->image_id = image_id;
    btn->image_type = mime_type;
    if (btn->image_id)
    {
        if (btn->thumbIMG)
        {
            btn->thumbIMG = null;
            btn->image(null);
        }
        if(image_size)
        {
            local image_name;
	    local image_header = image.slice(0, 4);
            if(image_header == "\x89PNG") btn->image_type = "png";
            else if(image_header == "\xFF\xD8\xFF\xE0") btn->image_type = "jpg";

            image_name = format("thumbJPG%d", btn->image_id);
            if(btn->image_type == "jpg") btn->thumbIMG = new Fl_JPEG_Image(image_name, image);
            else if(btn->image_type == "png") btn->thumbIMG = new Fl_PNG_Image(image_name, image, image_size);
            else return;

            if( (btn->w() < btn->thumbIMG->w()) || (btn->h() < btn->thumbIMG->h()) )
            {
                local imgb = btn->thumbIMG;
                local new_w = imgb->w();
                local new_h = imgb->h();
                best_fit(new_w, new_h, btn->w(), btn->h());

                if(new_w < imgb->w() || new_h < imgb->h())
                {
                    btn->thumbIMG = imgb->copy(new_w, new_h);
                }
            }
        }

        btn->hide();
        btn->image(btn->thumbIMG);
        btn->show();
        if(window  && window->shown()) window->show_image(btn->image_id);
    }
    else
    {
        clear_image_box(btn);
    }
}

function button_show_db_image(btn, image_id, window=null, asThumb=true, throwNotFound=true)
{
	if(image_id)
	{
		local rec = blob(0, 8192), image_type;
		appServer.images_get_image_record_by_id(rec, image_type, image_id, asThumb, throwNotFound);
		button_show_image(btn, image_id, image_type, rec.tostring(), rec.len(), window);
		return;
	}
	clear_image_box(btn);
}

class Fl_Box_ClearLabel extends Fl_Box {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Choice_Int extends Fl_Choice {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

enum DbAction_Enum {
		e_none, e_insert, e_update, e_delete,
		e_export, e_import, e_refresh, e_copy, e_last
		};
	
class Fl_Choice_dbAction extends Fl_Choice {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		tooltip(_tr("Select an operation to perform on this record"));
		add_tr("Insert", DbAction_Enum.e_insert);
		add_tr("Update", DbAction_Enum.e_update);
		add_tr("Delete", DbAction_Enum.e_delete);
	}

	function action(){return mvalue()->user_data();}
	function action(act){
		local m = menu();
		for(local i=0, count = m->size(); i<count; ++i){
			if(m[i].user_data() == act){
				value(i);
				break;
			}
		}
	}
	function add_tr(label, act){return add(_tr(label), 0, 0, act, 0);}

	function insert_export_import(){
		add_tr("Export", DbAction_Enum.e_export);
		add_tr("Import", DbAction_Enum.e_import);
	}
	function insert_refresh(){add_tr("Refresh", DbAction_Enum.e_refresh);}
	function insert_copy(){add_tr("Copy", DbAction_Enum.e_copy);}
};

local app_help_window = null;

class MyBaseWindow extends Fl_Window {
	childWindows=null;
	_db_map = null;

	constructor(px, py, pw, ph, pl=null) {
		if(px < 0) base.constructor(pw, ph, pl);
		else base.constructor(px, py, pw, ph, pl);
		childWindows = {};
		_db_map = {};
	}

	function handle(event){
		if(event == FL_KEYBOARD && Fl.event_command() == 0){
			local key = Fl.event_key();
			switch(key){
				//case FL_Menu:  menu_bar_navigate();break;
				case FL_F+1:
					if(!app_help_window){
						app_help_window = OurHelpWindow();
					}
					app_help_window.show();
					//Fl_Group *grp = wdg->as_group();
					app_help_window->search_help_file(label(), true);
				break;
			}
		}
		return base.handle(event);
	}

	function add_input_field_to_map(tbl, fldname, fld){
		local tbl_map = _db_map.get(tbl, false);
		if(!tbl_map){
			tbl_map = {};
			_db_map[tbl] <- tbl_map;
		}
		tbl_map[fldname] <- fld;
	}

	function getChildWindow(winName, WindowClass){
		local win = childWindows.get(winName, false);
		if(!win){
			win = new WindowClass();
			win.label(winName);
			childWindows[winName] <- win;
		}
		return win;
	}
	
	function showChildWindow(winName, WindowClass){
		local win = getChildWindow(winName, WindowClass);
		win->show();
		return win;
	}
	
	function fill_parent_size(wdg, xOffset=0, yOffset=0){
		local my_parent = wdg->parent();
		local newWidth =0;
		local oldRight = my_parent->x()+my_parent->w();
		local newRight = wdg->x()+wdg->w();
		if( newRight >  (oldRight - xOffset))
		{
		    newWidth = wdg->w() - (newRight - oldRight + xOffset);
		}
		else if(newRight <  (oldRight - xOffset))
		{
		    newWidth = wdg->w() + (oldRight - xOffset - newRight);
		}

		local newHeight =0;
		local oldBottom = my_parent->y()+my_parent->h();
		local newBottom = wdg->y()+wdg->h();
		if( newBottom >  (oldBottom - xOffset))
		{
		    newHeight = wdg->h() - (newBottom - oldBottom + xOffset);
		}
		else if(newBottom <  (oldBottom - xOffset))
		{
		    newHeight = wdg->h() + (oldBottom - xOffset - newBottom);
		}

		if(newWidth || newHeight)
		{
		    wdg->size(
			newWidth ? newWidth : wdg->w(),
			newHeight ? newHeight : wdg->h()
		    );
		}
	}
	
	function copy_widget_properties(src, dest){
		dest->labeltype(src->labeltype());
		dest->label(src->label());
		dest->labelfont(src->labelfont());
		dest->labelsize(src->labelsize());
		dest->textfont(src->textfont());
		dest->textsize(src->textsize());
		dest->color(src->color());
	}

	function replace_widget_for(wold, wnew){
		local xOffset=5, yOffset=10;
		copy_widget_properties(wold, wnew);
		wold->parent()->insert(wnew, wold);
		wnew->position(wold->x()+xOffset, wold->y()+yOffset);
		wnew->parent()->remove(wold);
		fill_parent_size(wnew, xOffset, yOffset);
		//delete wold;
	}

	function replace_widget_by(wold, wnew)
	{
		copy_widget_properties(wold, wnew);
		wold->parent()->insert(wnew, wold);
		wnew->parent()->remove(wold);
		wnew->resize(wold->x(), wold->y(), wold->w(), wold->h());
		//delete wold;
	}
	
	function fill_combolist_by_data (choice, data)
	{
		for(local i=0, max_count = data.size(); i<max_count; ++i)
		{
			local rec = data[i];
			choice->add_item(rec[0].tointeger(), rec[1]);
		}
	}		
}

class EditWindow extends MyBaseWindow {
	_record = null;
	_id = null;
	_main_table = null;
	
	constructor(px, py, pw, ph, pl){
		base.constructor(px, py, pw, ph, pl);
		_record = {};
	}
	function show_id(id){
		appServer.get_record(_record, _main_table, 0, id);
		_id = id;
		local tbl_map = _db_map.get(_main_table, false);
		if(tbl_map){
			foreach(k, wdg in tbl_map){
				if(_record.rawin(k)){
					local value =  _record[k];
					local classId = wdg->classId();
					if(classId == Fl_Text_Editor.className()){
						//set_widget_value((Fl_Text_Editor*)wdg, fld_name);
					}
					else if(classId == Fl_Check_Button.className()){
						//set_widget_value((Fl_Check_Button*)wdg, fld_name);
						wdg->value(value == "1" ? 1 : 0);
					}
					/*
					else if(classId == Fl_Choice_Str.className()){
						//set_widget_value((Fl_Choice_Str*)wdg, fld_name);
					}
					else if(classId == Fl_Choice_Int.className()){
						//set_widget_value((Fl_Choice_Int*)wdg, fld_name);
					}
					else if(classId == Flu_Combo_Box.className()){
						//set_widget_value((Flu_Combo_Box*)wdg, fld_name);
					}
					else if(classId == Flu_Tree_Browser.className()){
						//set_widget_value((Flu_Tree_Browser*)wdg, fld_name);
					}
					*/
					else if(wdg->inherits_from(Fl_Input_.cheap_rtti_info())){
						//set_widget_value((Fl_Input_*)wdg, fld_name);
						wdg->value(value);
					}
				}
			}
		}
	}
}

enum Fl_Data_Table_Events {e_none, e_event, e_select, e_insert, e_update, e_delete};

class Fl_Data_Table extends Flv_Data_Table {
	_cols_info = null;
	_data = null;
	_call_this = null;

	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		_cols_info = [];
		_data = [];
	}
	function resize(ax, ay, aw, ah){
		base.resize(ax, ay, aw, ah);
		calc_cols();
	}

	function clear_data_rows(){
		_data.clear();
	}
	function recalc_data(){
		rows(_data.size());
		redraw();
	}
	
	function get_data_value(arow, acol){ return _data[arow][acol];}
	
	function get_value(arow, acol){
		if(acol < 0)
		{
		    if(arow < 0) return "#";
		    return format("%d", arrow+1);
		}
		if(arow < 0) return _cols_info[acol].header;

		local ci = _cols_info[acol];

		local value = _data[arow][acol];

		switch(ci.format)
		{
			case 'M': return math.number_format(value.size() ? value.tofloat() : 0, 2);
			break;
			case 'P': return math.number_format(value.size() ? value.tofloat() : 0, 3);
			break;
			case 'N': return math.number_format(value.size() ? value.tofloat() : 0, -6);
			break;
			case 'Z': //Empty when zero
			case 'D': //Zero monetary //ZM':
			{
			    if(value == "0" || value == "") return "";
			    else
			    {
				if(ci.format == 'Z') return math.number_format(value.tofloat(), -6);
				else return math.number_format(value.tofloat(), 2);
			    }
			}
			break;
			case 'B': return (value == "1" ? "@-3+" : "");
			break;
			case '@': return value.replace("@", "@@"); //escape any @ 	{
			break;
		}
		return value;
	}
	
	function handle(event){
		switch(event){
			case FL_RELEASE:{
				local done = false;
				if(Fl.event_clicks() > 0){
					row_selected(Fl_Data_Table_Events.e_update);
					done = true;
				}
				if(done){
					Fl.event_clicks(0);
					return 1;
				}
			}
			break;
			case FL_KEYBOARD:
				local key = Fl.event_key();
				switch(key){
					case FL_KP_Plus:
						if(Fl.event_ctrl()) break;
					case FL_Insert:
						row_selected(Fl_Data_Table_Events.e_insert);
					break;
					//case FL_KP_Minus:
					//    if(Fl::event_ctrl()) break;
					case FL_Delete:
						row_selected(Fl_Data_Table_Events.e_delete);
					break;
					case FL_KP_Enter:
					case FL_Enter:
					case FL_Key_Space:
						if(!Fl.event_ctrl()){
							row_selected(Fl_Data_Table_Events.e_update);
						}
					break;
				}
			break;
		}
		local rc = base.handle(event);
		return rc;
	}
	function row_selected(ev){ if(_call_this) _call_this.row_selected(ev);}
	function set_cols(mycols){
		_cols_info.clear();
		for(local i=0, max_cols=mycols.size(); i < max_cols; ++i){
		    local col_info = {};
		    parse_field_header(mycols[i], col_info);
		    _cols_info.push(col_info);
		}
		cols(_cols_info.size());
		calc_cols();
	}
	function parse_field_header(str, col_info){
		local ci = str.split("|");
		local ci_size = ci.size();
		local curr_ci = 0;
		col_info.colname <- (ci_size > curr_ci++) ? ci[curr_ci-1] : "?";
		col_info.header <- (ci_size > curr_ci++) ? ci[curr_ci-1] : "?";
		col_info.width <- (ci_size > curr_ci++) ? ci[curr_ci-1].tofloat() : 10;
		col_info.align <- (ci_size > curr_ci++) ? ci[curr_ci-1][0] : 'L';
		col_info.format <- (ci_size > curr_ci++) ? ci[curr_ci-1][0] : '\0';
		col_info.color <- (ci_size > curr_ci++) ? ci[curr_ci-1].tointeger() : 0;
		col_info.bgcolor <- (ci_size > curr_ci++) ? ci[curr_ci-1].tointeger() : 0;
	}
	function calc_cols(){
		calc_cols_width();
		for(local k=0, len = cols(); k < len; ++k)
		{
		    local v = _cols_info[k];
		    //v.header = _tr(v.header.c_str());
		    local col_align;
		    switch(v.align)
		    {
		    case 'C':
			col_align = FL_ALIGN_CENTER;
			break;
		    case 'R':
			col_align = FL_ALIGN_RIGHT;
			break;
		    default:
			col_align = FL_ALIGN_LEFT;
		    }
		    local cs = col_style().get(k);
		    cs.align(col_align);
		    if(v.color) cs.foreground(v.color);
		    if(v.bgcolor) cs.background(v.bgcolor);
		}
	}
	function calc_cols_width(){
		local grid_width, total_widths, char_width;
		grid_width = w();
		if(has_scrollbar()) grid_width -= scrollbar_width();
		//char_width = self:labelsize() --fltk.fl_width('w')
		//it seems that if not set the text font on fluid
		//we get segfault here
		fl_font(textfont(), textsize());
		local gs = global_style();
		gs.height(fl_height()+fl_descent());
		char_width = fl_width("w");
		draw_offset(char_width/3);
		//print(grid_width, char_width)

		total_widths = 0;
		for(local k=0, len = cols(); k < len; ++k)
		{
		    local v = _cols_info[k].width;
		    if(v > 0) total_widths += ((v * char_width) + 1);
		}

		for(local k=0, len = cols(); k < len; ++k)
		{
		    local v = _cols_info[k];
		    local col_width = v.width;
		    if( col_width < 0)	col_width = grid_width - total_widths;
		    else col_width *= char_width;
		    local cs = col_style().get(k);
		    cs.width(col_width);
		}
	}
	function get_selection(ar_out, withIds=false){
	}
	function clear_selection(){
	}
	function get_row_id(arow=null){
		if(arow == null) arow = row();
		return _data[arow][0];
	}
}

dofile("search-options.nut");
dofile("utils-fltk.nut");
dofile("help-view-gui.nut");
dofile("help-view.nut");
dofile("base-report-A4.nut");
dofile("invoice-A4.nut");
dofile("edit-product-window.nut");
dofile("edit-entity-window.nut");
dofile("edit-order-window.nut");
dofile("list-search-window.nut");

class MyListSearchWindow extends ListSearch {
	_popup = null;
	_search_options = null;
	_sab = null;

	constructor(){
		base.constructor();
		_search_options = OurBizSearchOptions();
		grid->callback_when(FLVEcb_ROW_CHANGED | FLVEcb_CLICKED | FLVEcb_ROW_HEADER_CLICKED);
		grid->callback(grid_cb);
		grid->_call_this = this;
	}
	
	function grid_cb(sender, udata){}
	function get_edit_window(){return null;}
	
	function row_selected(ev){
		local edit_window = get_edit_window();
		if(edit_window){
			edit_window.show();
			edit_window.show_id(grid->get_row_id());
			switch(ev){
				case Fl_Data_Table_Events.e_select:					
				break;
				case Fl_Data_Table_Events.e_insert:
				break;
				case Fl_Data_Table_Events.e_update:
				break;
				case Fl_Data_Table_Events.e_delete:
				break;
			}
		}
	}

	function get_search_data(data){}
	function fill_grid(){
		//SafeCursorWait cursor_wait;
		fl_cursor(FL_CURSOR_WAIT);
		grid->clear_data_rows();
		get_search_data(grid->_data);
		grid->recalc_data();
		fl_cursor(FL_CURSOR_DEFAULT);
	}
	function cb_btnSearch(sender, udata){
		this = sender.window();
		fill_grid();
	}
	
	function create_search_by0(name, Klass, pack){
		local sb = Klass(0,0,25,20, _tr(name));
		sb->labelsize(pack->labelsize());
		sb->w(fl_width(sb.label()) + fl_width("W"));
		pack->add(sb);
		return sb;
	}
	function create_search_by(name){
		local sb = create_search_by0(name, Fl_Round_Button, pack_search_options);
		sb->type(FL_RADIO_BUTTON);
		return sb;
	}
	function create_search_by2(name){
		local sb = create_search_by0(name, Fl_Check_Button, pack_search_options2);
		return sb;
	}
	function on_first_time_show()
	{
		local filter_data = [];
		get_data_group_filter(filter_data);
		fill_group_filter(filter_data);
		//fill_search_options();
		mk_popup();
		//Fl::add_timeout(0.1, &do_delayed_on_search, this);
	}
	
	function get_data_group_filter(data){}
	function mk_popup(){}
	function fill_group_filter(data)
	{
		group_filter->clear_items();
		group_filter->add_item(0, "---");
		fill_combolist_by_data(group_filter, data);
		group_filter->select_by_data(0);
		group_filter.callback(on_filter);
	}
	function on_filter(sender, udata){
		this = sender.window();
		cb_btnSearch(sender, udata);
	}
	
	function setFilterComboTree(){
		local ctree = new Flu_Combo_Tree(0,0, 25,25);
		replace_widget_by(group_filter, ctree);

		ctree->textfont(ctree->labelfont());
		ctree->textsize(ctree->labelsize());

		local tree = ctree->tree();
		tree.auto_branches(true);
		tree.show_branches(true);
		tree.all_branches_always_open(true);
		tree.branch_text(ctree->labelcolor(), ctree->labelfont(), ctree->labelsize());
		tree.leaf_text(ctree->labelcolor(), ctree->labelfont(), ctree->labelsize());
		tree.shaded_entry_colors( FL_WHITE, FL_GRAY );
		//tree.label("----");

		ctree.callback(on_filter);
		group_filter = ctree;
	}
}


dofile("sales-tax-window.nut");

class OurSalesTax extends SalesTaxRatesEditWindow {
	constructor(){
		base.constructor();
		local cols_info = [
			"id|ID|0",
			"rate1|V.A.T. %|8|R",
			"rate2|R.E. %|8|R",
			"description|Description|-1",
			"is_active|Active|5|C|B",
		];
		grid->set_cols(cols_info);
		fill_grid();
	}
	function fill_grid(){
		//SafeCursorWait cursor_wait;
		grid->clear_data_rows();
		appServer.sales_tax_rates_get_list(grid->_data);
		grid->recalc_data();
	}
}

function print_entities_list_contact_report()
{
	//SafeCursorWait cursor_wait;
	fl_cursor(FL_CURSOR_WAIT);
	local mydata = [];
	appServer.entities_toprint_get_list(mydata);

	local report = new BaseReportA4(0,0,420,594);
	//report.resize(0,0, 546, 772);
	report.resize(0,0, 546, 790);
	//report.rpt_Body->_forPrint = true;
	local lines_per_page = 56;

	local _cols_info = [
		"id|ID|6|R",
		"name|Name|-1",
		"contact|Contact|12",
		"phone|Phone|12",
	    ];
	report.rpt_Body->set_cols(_cols_info);

	local nrows = mydata.size();
	local npages = nrows/lines_per_page;
	if(nrows%lines_per_page) npages++; //check integer division truncation

	local printer = Fl_Pdf_File_Device();
	printer.compress(true);
	local result = printer.start_job("entities-contact-list.pdf", 1);
	local date = os.date("*t");
	local bufSubTitle, bufSubFooter;

	local rptData = report.rpt_Body->_data;
	local iter1, iter2, iter0 = 0;
	
	local function array_assign(dest, src, start, end){
		dest.clear();
		dest.extend(src.slice(start, end));
	}

	for(local i=0; i< npages; ++i)
	{
	    local line_start = i*lines_per_page;
	    local line_end = line_start + lines_per_page;
	    iter1 = iter0 + line_start;
	    iter2 = iter0 + (line_end > nrows ? nrows : line_end);
	    array_assign(rptData, mydata, iter1, iter2);
	    report.rpt_Body->recalc_data();

	    report.rpt_Title->label(_tr("Entities List"));
	    bufSubTitle = format("%s %.2d/%.2d/%d", _tr("Date"), date.day, date.month, date.year);
	    report.rpt_SubTitle->label(bufSubTitle);

	    bufSubFooter = format("%s %d / %d", _tr("Page"), i+1, npages);
	    report.rpt_SubFooter->label(bufSubFooter);
	    printer.start_page();
	    printer.origin(30,25);
	    printer.print_widget(report);
	    printer.end_page();
	}
	printer.end_job();
	fl_cursor(FL_CURSOR_DEFAULT);
}

class MyEditEntityWindow extends EditEntityWindow {
	constructor(){
		base.constructor();
		_main_table = "entities";
	}
}

class EntitiesListSearch extends MyListSearchWindow {
	_search_by_name = null;
	_search_by_notes = null;
	_search_by_phone = null;
	_search_by_product = null;
	_search_by_id = null;
	_search_by_active = null;
	
	constructor() {
		base.constructor();
		label(_tr("Entities List Search"));
		local cols_info = [
			"id|ID|6|R",
			"name|Name|-1",
			"contact|Contact|12",
			"phone|Phone|12",
			"is_active|Active|0",
			"image_id|image_id|0",
		];
		grid->set_cols(cols_info);
		_search_by_name = create_search_by("Name");
		_search_by_notes = create_search_by("Notes");
		_search_by_phone = create_search_by("Phone");
		_search_by_product = create_search_by("Product");
		_search_by_id = create_search_by("ID");
		_search_by_active = create_search_by2("Active");
		_search_by_name->setonly();
		_search_by_active->value(1);
		fill_grid();
	}
	function fill_search_options(){
		pack_search_options.add(Fl_Radio_Button());
	}
	function get_search_data(data){
		_search_options.search_str = search_str.value();
		_search_options.name = _search_by_name.value() == 1;
		_search_options.notes = _search_by_notes.value() == 1;
		_search_options.phone = _search_by_phone.value() == 1;
		_search_options.product_id = _search_by_product.value() == 1;
		_search_options.id = _search_by_id.value() == 1;
		_search_options.active = _search_by_active.value() == 1;
		_search_options.group_id = group_filter->get_data_at();
		appServer.entities_get_list(grid->_data, _search_options);
	}
	
	function get_edit_window(){return getChildWindow("Entity Edit", MyEditEntityWindow);}
	
	function cb_btnInsert(sender, udata){
		this = sender.window();
		local win = showChildWindow("Entity Edit", EditEntitiesWindow);
	}
	function cb_btnUpdate(sender, udata){
		print_entities_list_contact_report();
	}
}

function print_products_list()
{
	//SafeCursorWait cursor_wait;
	fl_cursor(FL_CURSOR_WAIT);
	local mydata = [];
	appServer.products_list_get_list(mydata);

	local report = new BaseReportA4(0,0,420,594);
	//report.resize(0,0, 546, 772);
	report.resize(0,0, 546, 790);
	//report.rpt_Body->_forPrint = true;
	local lines_per_page = 56;

	local _cols_info = [
                "id|ID|6|R",
                "reference|Reference|9",
                "sell_description|Description|-1",
                "kit|Kit|4|C",
                "price_taxed|Price+V.A.T.|12|R|M",
                "quantity_onhand|Onhand|9|R|N",
	    ];
	report.rpt_Body->set_cols(_cols_info);

	local nrows = mydata.size();
	local npages = nrows/lines_per_page;
	if(nrows%lines_per_page) npages++; //check integer division truncation

	local printer = Fl_Pdf_File_Device();
	printer.compress(true);
	local result = printer.start_job("products-list.pdf", 1);
	local date = os.date("*t");
	local bufSubTitle, bufSubFooter;

	local rptData = report.rpt_Body->_data;
	local iter1, iter2, iter0 = 0;
	
	local function array_assign(dest, src, start, end){
		dest.clear();
		dest.extend(src.slice(start, end));
	}

	for(local i=0; i< npages; ++i)
	{
	    local line_start = i*lines_per_page;
	    local line_end = line_start + lines_per_page;
	    iter1 = iter0 + line_start;
	    iter2 = iter0 + (line_end > nrows ? nrows : line_end);
	    array_assign(rptData, mydata, iter1, iter2);
	    report.rpt_Body->recalc_data();

	    report.rpt_Title->label(_tr("Products List"));
	    bufSubTitle = format("%s %.2d/%.2d/%d", _tr("Date"), date.day, date.month, date.year);
	    report.rpt_SubTitle->label(bufSubTitle);

	    bufSubFooter = format("%s %d / %d", _tr("Page"), i+1, npages);
	    report.rpt_SubFooter->label(bufSubFooter);
	    printer.start_page();
	    printer.origin(30,25);
	    printer.print_widget(report);
	    printer.end_page();
	    
            //to allow user do something meanwhile
            Fl_Display_Device.display_device()->set_current();
            Fl.check();
            printer.set_current();	    
	}
	printer.end_job();
	fl_cursor(FL_CURSOR_DEFAULT);
}

class MyEditProductWindow extends EditProductWindow {
	constructor(){
		base.constructor();
		_main_table = "products";
	}
}

class ProductsListSearch extends MyListSearchWindow {
	_search_by_description = null;
	_search_by_notes = null;
	_search_by_reference = null;
	_search_by_entities = null;
	_search_by_id = null;
	_search_by_active = null;
	_last_image_id = null;
	_image_window = null;
	
	constructor() {
		_last_image_id = 0;
		base.constructor();
		_search_options = OurBizSearchOptions();
		label(_tr("Products List Search"));
		local cols_info = [
			"id|ID|6|R",
			"reference_code|Reference|9",
			"sell_description|Description|-1",
			"kit|Kit|3|C",
			"price_taxed|Price+V.A.T.|9|R|M",
			"price|Price|0|R|M",
			"quantity_onhand|Onhand|6|R|N",
			"is_active|Active|5|C|B",
			"price_date|Price Date|0",
			"image_id|image_id|0",
		];
		grid->set_cols(cols_info);
		//setFilterComboTree();
		_search_by_description = create_search_by("Description");
		_search_by_notes = create_search_by("Notes");
		_search_by_reference = create_search_by("Reference");
		_search_by_entities = create_search_by("Entities");
		_search_by_id = create_search_by("ID");
		_search_by_active = create_search_by2("Active");
		_search_by_description->setonly();
		_search_by_active->value(1);
		fill_grid();
	}
	
	function get_search_data(data){
		_search_options.search_str = search_str.value();
		_search_options.description = _search_by_description.value() == 1;
		_search_options.notes = _search_by_notes.value() == 1;
		_search_options.reference = _search_by_reference.value() == 1;
		_search_options.entities = _search_by_entities.value() == 1;
		_search_options.id = _search_by_id.value() == 1;
		_search_options.active = _search_by_active.value() == 1;
		_search_options.group_id = group_filter->get_data_at();
		appServer.products_get_list(grid->_data, _search_options);
	}

	function get_edit_window(){return getChildWindow("Product Edit", MyEditProductWindow);}

	function cb_btnInsert(sender, udata){
		this = sender.windw();
		local win = showChildWindow("Product Edit", EditProductWindow);
	}
	
	function grid_cb(sender, udata){
		//print("on_row_changed", sender, row);
		if(sender->why_event() == FLVE_ROW_CHANGED){
			this = sender.window();
			if(shown()){
				local img_id = sender->get_data_value(sender->row(), sender->cols()-1);
				if(img_id){
					img_id = img_id.tointeger();
					if(img_id != _last_image_id){
						button_show_db_image(btnThumbImage, img_id, _image_window, true, false);
						_last_image_id = img_id;
					}
				}
			}	
		}
	}
	function cb_btnUpdate(sender, udata){
		print_products_list();
	}
}

class MyEditOrderWindow extends EditOrderWindow {
	constructor(){
		base.constructor();
		_main_table = "orders";
	}
}


class OrdersListSearch extends MyListSearchWindow {
	_search_by_entities = null;
	_search_by_notes = null;
	_search_by_products = null;
	_search_by_date = null;
	_search_by_total = null;
	_search_wp = null;

	constructor() {
		base.constructor();
		label(_tr("Orders List Search"));
		local cols_info = [
			"id|ID|6|R",
			"order_date|Date|9",
			"order_type_code|Type|4",
			"series|Series|5",
			"order_number|Number|7",
			"entity_name|Entity|-1",
			"total_amt|Total|8|R|M",
			"payment_type_code|P.T.|4|C",
		];
		grid->set_cols(cols_info);
		_search_by_entities = create_search_by("Entities");
		_search_by_notes = create_search_by("Notes");
		_search_by_products = create_search_by("Products");
		_search_by_date = create_search_by("Date");
		_search_by_total = create_search_by("Total");
		_search_wp = create_search_by2("WP");
		_search_by_entities->setonly();
		fill_grid();
	}
	
	function get_data_group_filter(data)
	{
		appServer.order_types_list_short(data, 0);
	}
	
	function get_search_data(data){
		_search_options.search_str = search_str.value();
		_search_options.entities = _search_by_entities.value() == 1;
		_search_options.notes = _search_by_notes.value() == 1;
		_search_options.products = _search_by_products.value() == 1;
		_search_options.date = _search_by_date.value() == 1;
		_search_options.total = _search_by_total.value() == 1;
		_search_options.group_id = group_filter->get_data_at();
		appServer.orders_get_list(grid->_data, _search_options);
	}

	function get_edit_window(){return getChildWindow("Order Edit", MyEditOrderWindow);}

	function cb_btnInsert(sender, udata){
		this = sender.windw();
		local win = showChildWindow("Order Edit", EditOrderWindow);
	}
	function mk_popup()
	{
		_popup = create_popup_menu_for(grid, grid->global_style().height());
		_popup.callback(on_popupmenu_cb);

		_popup->add(_tr("Copy"));
		_popup->add(_tr("Copy grouping"));
		_popup->add(_tr("Clear selection"));
	}
	
	function on_popupmenu_cb(sender, udata){
		//printf("%p : %d : %s\n", sender, popup->value(),
		//       popup->menu_at(popup->value())->label());
		this = sender.windw();
		local row = grid->row();
		if(row < 0) return;
		switch(_popup->value()){
			case 0:{
				local vi = [];
				grid->get_selection(vi, true);
				if(vi.size() == 0) vi.push(grid->get_row_id(row));
				copy_order(vi, false, _sab);
			}
			break;
			case 1:{
			    local vi = [];
			    grid->get_selection(vi, true);
			    copy_order(vi, true, _sab);
			}
			break;
			case 2:{
			    grid->clear_selection();
			    grid->redraw();
			}
			break;
		}
	}
	
	function copy_order(ar_ids, grouping, sab){
	}
}

class PaymentsListSearch extends ListSearch {

	constructor() {
		base.constructor();
		label(_tr("Payments List Search"));
	}
}

dofile("main-window.nut");

local win = new MainWindow(10, 50, 330, 320, "OURBIZ");
//local win = new SalesTaxRatesEditWindow();
win->resizable(win);
win->show_main();

Fl.run();
