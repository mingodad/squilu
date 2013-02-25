/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */

local WIN32 = os.getenv("WINDIR") != null
socket.open();

dofile("ourbiz-client.nut");
dofile("db-updater.nut");
local appServer = AppServer.getAppServer();
appServer.credentials("mingote", "tr14pink");
appServer.connect("localhost", 8855); //8888);//8855);

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

class Fl_Multiline_Input extends Fl_Input {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		type(4);
	}
}

class Fl_Multiline_Output extends Fl_Output {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		type(4);
	}
}

class MyFl_Text_Editor extends Fl_Text_Editor_Buffered
{
	constructor(px, py, pw, ph, pl = null){
		base.constructor(px, py, pw, ph, pl);
	}
/*
	function handle(event){
		if(event == FL_KEYBOARD){
			local pos, line;
			switch(Fl.event_key()){
				case FL_Up:
					pos = insert_position();
					if ( pos == 0 ){
						if(position_to_line( pos, &line ) && (line == 0)){
						return Fl_Group::handle(event);
					}
				}
				break;
			}
		}
		return base.handle(event);
	}
*/
};

class MyTree_Browser extends Flu_Tree_Browser {

	constructor(px, py, pw, ph, pl = null){
		base.constructor(px, py, pw, ph, pl);
	}

	function handle(event){
		if(event == FL_PUSH){
		    if( Fl.event_clicks() > 0 )
		    {
			Fl.event_clicks(0);
			local node = get_hilighted();
			if(node){
			    node->do_callback(FLU_DOUBLE_CLICK);
			    return 1;
			}
		    }
		}

		local result = base.handle(event);

		if(event == FL_KEYBOARD)
		{
		    switch(Fl.event_key())
		    {
		    case FL_Key_Space:
		    case FL_Enter:
		    case FL_KP_Enter:
			local node = get_hilighted();
			if(node) node->do_callback(FLU_DOUBLE_CLICK);
		    break;
		    }
		}

		return result;
	}
};

class Fl_Box_ClearLabel extends Fl_Box {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Image_Box extends Fl_Box {
	image_id = null;
	image_type = null;
	thumbIMG = null;

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
	function handle(event)
	{
		if(event == FL_KEYBOARD)
		{

		    switch(Fl.event_key())
		    {
		    case FL_Enter:
		    case FL_KP_Enter:
			do_callback();
			return 0;
		    }
		}
		return base.handle(event);
	}
}

class Fl_Choice_Int extends Fl_Choice {
	my_values = null;

	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		my_values = [];
	}
	function my_add (akey, avalue){
		my_values.push(akey);
		add(avalue);
	}
	function my_add_tr (akey,  avalue){
		my_values.push(akey);
		add(_tr(avalue));
	}
	function my_get_value (pos=-1){
		if(pos == -1) pos = value();
		if(pos == -1) return -1;
		return my_values[pos];
	}
	function my_get_value_str (pos=-1){
		if(pos == -1) pos = value();
		if(pos == -1) return "";
		return my_values[pos].tostring();
	}
	function my_set_value (akey){
		my_clear();
		for(local i = 0, len = my_values.size(); i < len; ++i )
		{
		    if(my_values[i] == akey)
		    {
			value(i);
			break;
		    }
		}
	}
	function my_clear (){ value(-1);}
}

class Fl_Choice_Str extends Fl_Choice_Int {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
	}
}

enum Fl_Data_Table_Events {e_none, e_event, e_select, e_insert, e_update, e_delete};

enum DbAction_Enum {
		e_none, e_insert, e_update, e_delete,
		e_export, e_import, e_refresh, e_copy, e_last
		};

class Fl_Choice_dbAction extends Fl_Choice {
	_options = null;
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		tooltip(_tr("Select an operation to perform on this record"));
		_options = [];
		add_tr("Insert", DbAction_Enum.e_insert);
		add_tr("Update", DbAction_Enum.e_update);
		add_tr("Delete", DbAction_Enum.e_delete);
	}

	function action(act=null){
		if(!act) return _options[value()][1];
		local x = 0;
		foreach(opt in _options){
			if(opt[1] == act){
				value(x);
				break;
			}
			++x;
		}
	}
	function add_tr(label, act){
		local opt = [_tr(label), act];
		_options.push(opt);
		return add(opt[0], 0, 0, opt[1], 0);
	}

	function insert_export_import(){
		add_tr("Export", DbAction_Enum.e_export);
		add_tr("Import", DbAction_Enum.e_import);
	}
	function insert_refresh(){add_tr("Refresh", DbAction_Enum.e_refresh);}
	function insert_copy(){add_tr("Copy", DbAction_Enum.e_copy);}
};

local app_help_window = null;

class Base_Window extends Fl_Window {
	childWindows=null;
	_db_map = null;

	constructor(px, py, pw, ph, pl=null) {
		if(px < 0) base.constructor(pw, ph, pl);
		else base.constructor(px, py, pw, ph, pl);
		childWindows = {};
		_db_map = {};
	}
	
	function show_help_window(sender=null, udata=null){
		if(sender) this = sender->window();
		if(!app_help_window){
			app_help_window = OurHelpWindow();
		}
		app_help_window.show();
		//Fl_Group *grp = wdg->as_group();
		app_help_window->search_help_file(label(), true);
	}

	function handle(event){
		if(event == FL_KEYBOARD && Fl.event_command() == 0){
			local key = Fl.event_key();
			switch(key){
				//case FL_Menu:  menu_bar_navigate();break;
				case FL_F+1: show_help_window(); break;
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

	function get_input_fields(dbname){ return _db_map.get(dbname, null);}

	function setup_grid(grid, pcall_this){
		grid->callback_when(FLVEcb_ROW_CHANGED | FLVEcb_CLICKED | FLVEcb_ROW_HEADER_CLICKED);
		grid->callback(grid_cb);
		grid->_call_this = pcall_this;
	}	
	function grid_cb(sender, udata){}

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
	
	function insert_group_tree_childs (tree, node, parent, pos, size, data)
	{
	    local new_node;
	    assert(node);
	    while(pos < size)
	    {
		local rec = data[pos];
		local myparent = rec[1].tointeger();
		if(myparent != parent) break;
		local id = rec[0].tointeger();

		local str = rec[2].gsub("/", "\\/");
		new_node = tree->add(node, str);
		if(new_node) new_node->user_data(id);
		else throw ("Try to insert an invalid Tree Node : " + str);

		++pos;
		pos = insert_group_tree_childs(tree, new_node, id, pos, size, data);
	    }
	    return pos;
	}
	
	function setup_tree_browser_for_selection (tree) {
		local node = tree->get_root();
		if(node) node->always_open(true);
		tree->show_root(true);
		tree->auto_branches(true);
		tree->show_branches(true);
		tree->show_leaves(true);
		tree->all_branches_always_open(true);
		tree->branch_text(tree->labelcolor(), tree->labelfont(), tree->labelsize());
		tree->leaf_text(tree->labelcolor(), tree->labelfont(), tree->labelsize());
		//tree->selection_mode(FLU_MULTI_SELECT);
		tree->selection_mode(FLU_SINGLE_SELECT);
		tree->selection_drag_mode(FLU_DRAG_TO_SELECT);
		tree->shaded_entry_colors( FL_WHITE, FL_GRAY );
		tree->selection_follows_hilight( true );
	}
	
	function treeLoadChilds (tree, node, parent, _table_name)
	{
		local data = [];
		tree->clear();
		appServer.get_list_data(data, _table_name , 0, 0);
		local pos = 0;
		if(!node) node = tree->get_root();
		insert_group_tree_childs(tree, node, parent, pos, data.size(), data);
	}
	function delete_on_window_close_cb (sender, udata){
		if(sender){
			if(!sender.as_window()) throw(_tr("Only windows can use this callback !"));
			Fl.delete_widget(sender);
		}
	}
}

class Widget_Fill_By_Map {
	_map = null;
	_setChanged = null;

	constructor(map, setChanged=false){
		_setChanged = setChanged;
		_map = map;
	};

	function set_widget_value_by_map(inputs){
		foreach(fld_name, wdg in inputs){
			set_widget_value(wdg, fld_name);
		}
	}

	function setValue(fld_name, value){
		_map[fld_name] <- value ? value : "";
	}

	function getValue(fld_name){
		return _map.get(fld_name, "");
	}
	
	function getValueInteger(fld_name){
		local value = _map.get(fld_name, "");
		return value.len() ? value.tointeger() : 0;
	}

	function set_widget_value(wdg, fld_name, asBlank=false, dflt=null){
		if(wdg instanceof Fl_Text_Editor){
			wdg->buffer()->text((asBlank) ? "" : getValue(fld_name));
		}
		else if(wdg instanceof Fl_Check_Button){
			wdg->value((asBlank) ? (dflt ? 1 : 0) : getValueInteger(fld_name));
		}
		else if(wdg instanceof Fl_Choice_Str){
			 wdg->my_set_value((asBlank) ? "" : getValue(fld_name));
		}
		else if(wdg instanceof Fl_Choice_Int){
			 wdg->my_set_value((asBlank) ? (dflt ? dflt : -1) : getValueInteger(fld_name));
		}
		else if(wdg instanceof Flu_Combo_Box){
			wdg->select_by_data((asBlank) ? (dflt ? dflt : 0) : getValueInteger(fld_name));
		}
		else if(wdg instanceof Flu_Tree_Browser){
			local value = (asBlank) ? (dflt ? dflt : 0) : getValueInteger(fld_name);
			wdg->unselect_all();
			local item = wdg->find_by_user_data(value);
			if(item){
				wdg->set_hilighted(item);
				item->select(true);
				//clear changed because Flu_Tree_Browser will set it when select
				wdg->clear_changed_all();
			}
		}
		else if(wdg instanceof Fl_Input_){
			wdg->value((asBlank) ? (dflt ? dflt : "") : getValue(fld_name));
		}
		if(_setChanged) wdg->set_changed2();
	}
}

class DBUpdateByWidget extends DBTableUpdate
{
	constructor(){
		base.constructor();
	}

	function get_widget_value(inputs){
		foreach(fld_name, wdg in inputs){
			//printf("%s : %s : %s\n", fld_name, wdg->classId(), Fl_Check_Button.className());
			if(wdg instanceof Fl_Text_Editor){
				if((dbAction == e_insert) || wdg->changed2())
				{
					_field_changes.push([fld_name, wdg->buffer()->text()]);
				}
			}
			else if(wdg instanceof Fl_Check_Button){
				if((dbAction == e_insert) || wdg->changed2())
				{
					_field_changes.push([fld_name, wdg->value() ? "1" : "0"]);
				}
			}
			else if(wdg instanceof Fl_Choice_Str){
				if((dbAction == e_insert) || wdg->changed2())
				{
					_field_changes.push([fld_name, wdg->my_get_value()]);
				}
			}
			else if(wdg instanceof Fl_Choice_Int){
				if((dbAction == e_insert) || wdg->changed2())
				{
					_field_changes.push([fld_name, wdg->my_get_value_str()]);
				}
			}
			else if(wdg instanceof Flu_Combo_Box){
				if((dbAction == e_insert) || wdg->input.changed2())
				{
					_field_changes.push([fld_name, wdg->get_data_at().tostring()]);
				}
			}
			else if(wdg instanceof Flu_Tree_Browser){
				if((dbAction == e_insert) || wdg->changed2())
				{
					local tmp = "";
					local node = wdg->get_hilighted();
					if(node) {
						local value = node->user_data();
						if(value) tmp  = value.tostring();
					}
					_field_changes.push([fld_name, tmp]);
				}
			}
			else if(wdg instanceof Fl_Input_){
				if((dbAction == e_insert) || wdg->changed2())
				{
					_field_changes.push([fld_name, wdg->value()]);
				}
			}
		}
	}

	function validate_regex(wdg,  re, emptyTrue=false)
	{
		local value = wdg->value();
		if(emptyTrue && (!value || !value.len())) return true;
		if(!re || !value) return false;
		return value.find_lua(re);
	}
}

class Edit_Base_Window extends Base_Window {
	_record = null;
	_dbUpdater = null;
	_choiceDbAction = null;
	_btnDbAction = null;
	_formGroup = null;
	
	constructor(px, py, pw, ph, pl){
		base.constructor(px, py, pw, ph, pl);
		_record = {};
	}
	
	function setDbActionControls(choice, btn){
		_choiceDbAction = choice;
		_choiceDbAction.callback(on_Change_dbAction);
		_btnDbAction = btn;
		_btnDbAction.callback(on_btnDbAction);
	}
	function fill_choice_by_data (choice, data)
	{
		for(local i=0, max_count = data.size(); i<max_count; ++i)
		{
			local rec = data[i];
			choice->my_add(rec[0].tointeger(), rec[1]);
			//choice->add(rec[1]);
		}
	}
	
	function on_Change_dbAction(sender=null, udata=null)
	{
		if(sender) this = sender->window();
		local colors = [255, 95, 79, 90, 238, 238, 238, 238];
		local idx = _choiceDbAction->action();
		_btnDbAction->label(_choiceDbAction->text());
		_btnDbAction->color(colors[idx]);
	}
	function on_btnDbAction(sender, udata)
	{
		this = sender->window();
		
		local cursor_wait = fl_cursor_wait();
		try {
			switch(_choiceDbAction->action())
			{
			case DbAction_Enum.e_insert:
			    do_insert();
			    break;
			case DbAction_Enum.e_update:
			    do_update();
			    break;
			case DbAction_Enum.e_delete:
			    do_delete();
			    break;
			case DbAction_Enum.e_export:
			    do_export();
			    break;
			case DbAction_Enum.e_import:
			    do_import();
			    break;
			case DbAction_Enum.e_refresh:
			    do_refresh();
			    break;
			case DbAction_Enum.e_copy:
			    do_copy();
			    break;
			}
		}
		catch(e){ fl_alert(e);}
	}
	function get_record_by_id(id){
		appServer.get_record(_record, dbUpdater()->table_name, 0, dbUpdater()->edit_id);
	}
	function do_edit(aid){
		local dbu = dbUpdater();
		dbu->edit_id = aid.tointeger();
		dbu->version = 0;
		dbAction->action(aid ? DbAction_Enum.e_update : DbAction_Enum.e_insert);
		on_Change_dbAction();
		delayed_method_call(this, this.do_edit_delayed, aid);
	}
	function do_edit_delayed(udata)
	{
		local cursor_wait = fl_cursor_wait();
		get_record_by_id(udata);
		fill_edit_form(dbUpdater()->edit_id == 0);
	}
	function fill_edit_form(asBlank=false){
		if(asBlank) _record.clear();
		local  wfq = Widget_Fill_By_Map(_record);
		local input_fld_map = get_input_fields(dbUpdater()->get_fields_map_name());
		wfq.set_widget_value_by_map(input_fld_map);
		dbUpdater()->version = wfq.getValueInteger("_version_");
	}
	function get_widget_changed(uw){
		local input_fld_map = get_input_fields(dbUpdater()->get_fields_map_name());
		uw.get_widget_value(input_fld_map);
	}
	function do_insert(){
		//fl_alert("do_insert");
		local dbu = dbUpdater();
		dbu->dbAction = dbu.e_insert;
		dbu->clear();
		get_widget_changed(dbu);
		dbu->insert_table();
		if(_formGroup) clear_changed_on_group(_formGroup);
		dbAction->action(DbAction_Enum.e_update);
		on_Change_dbAction();
	}
	function do_update(){
		//fl_alert("do_update");
		local dbu = dbUpdater();
		dbu->dbAction = dbu.e_update;
		dbu->clear();
		get_widget_changed(dbu);
		dbu->update_table();
		if(_formGroup) clear_changed_on_group(_formGroup);
	}
	function do_delete()	{
		//fl_alert("do_delete");
		local dbu = dbUpdater();
		dbu->dbAction = dbu.e_delete;
		dbu->clear();
		dbu->delete_record();
		dbAction->action(DbAction_Enum.e_insert);
		on_Change_dbAction();
	}
	function do_export(){/*fl_alert("do_delete");*/}
	function do_import(){/*fl_alert("do_delete");*/}
	function do_refresh(){/*fl_alert("do_delete");*/}
	function do_copy(){/*fl_alert("do_delete");*/}
	
	function dbUpdater(dbu=null){
		if(dbu){
			_dbUpdater = dbu;
			return;
		}
		if(!_dbUpdater) _dbUpdater = new DBUpdateByWidget();
		return _dbUpdater;
	}
	
	function do_edit_by_grid(pid, sender){
		do_edit(pid);
	}
	function row_selected(sender, ev){
		this = sender->window();
		switch(ev){
			case Fl_Data_Table_Events.e_insert:
				do_edit_by_grid(0, sender);
			break;
			case Fl_Data_Table_Events.e_update:
				do_edit_by_grid(sender->get_row_id(), sender);
			break;
			case Fl_Data_Table_Events.e_delete:
				do_edit_by_grid(sender->get_row_id(), sender);
			break;
		}
	}	
}

class Fl_Data_Table extends Flv_Data_Table {
	_cols_info = null;
	_data = null;
	_call_this = null;
	_forPrint = null;

	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		_cols_info = [];
		_data = [];
		_forPrint = false;
		//has_scrollbar(FLVS_BOTH);
	}

	function for_print(bval){
		_forPrint = bval; 
		if(_forPrint) box(FL_NO_BOX);
	}

	function resize(ax, ay, aw, ah){
		base.resize(ax, ay, aw, ah);
		calc_cols();
	}

	function clear_data_rows(){
		row(0);
		rows(0);
		_data.clear();
	}
	function recalc_data(){
		rows(_data.size());
		redraw();
	}

	function set_data(data_array){
		_data = data_array;
		recalc_data();
	}

	function set_new_data(data_array){
		clear_data_rows();
		_data = data_array;
		local mycols = _data[0];
		_data.remove(0);
		set_cols(mycols);
		recalc_data();
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
			case 'S': return value.tostring();
			break;
			case '@': return value.replace("@", "@@"); //escape any @ 	{
			break;
		}
		return value;
	}

	function get_style(style, Row, Col){
		base.get_style(style, Row, Col);
		if(Row >= 0 && Col >= 0)
		{
		    local rs = row_style().get(Row);
		    local cs = col_style().get(Col);
		    local have_bg = 0;
		    if(cs.background_defined()) have_bg = cs.background();
		    else if(rs.background_defined()) have_bg = rs.background();

		    if((Row % 2) == 0)
		    {
			if(have_bg)
			    //s:background(fltk.fl_color_average(207, have_bg, 0.5))
			    style.background(have_bg);
			else
			    style.background(207);
		    }
		    else
		    {
			if(have_bg)
			    //s:background(have_bg)
			    style.background(fl_color_average(FL_WHITE, have_bg, 0.5));
			else
			    style.background(FL_WHITE);
		    }
		}
		else if(_forPrint)
		{
		    style.frame(FL_NO_BOX);
		}
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
	function row_selected(ev){ if(_call_this) _call_this.row_selected(this, ev);}
	function set_cols(mycols, size_absolute=false){
		_cols_info.clear();
		for(local i=0, max_cols=mycols.size(); i < max_cols; ++i){
		    local col_info = {};
		    parse_field_header(mycols[i], col_info);
		    _cols_info.push(col_info);
		}
		cols(_cols_info.size());
		calc_cols(size_absolute);
	}
	function parse_field_header(str, col_info){
		local ci = str.split('|');
		local ci_size = ci.size();
		local curr_ci = 0;
		col_info.colname <- (ci_size > curr_ci++) ? ci[curr_ci-1] : "?";
		col_info.header <- (ci_size > curr_ci++) ? ci[curr_ci-1] : "?";
		col_info.width <- (ci_size > curr_ci++) ? ci[curr_ci-1].tofloat() : 10;
		col_info.align <- (ci_size > curr_ci++) ? ci[curr_ci-1][0] : 'L';
		col_info.format <- (ci_size > curr_ci++) ? ci[curr_ci-1][0] : '\0';
		col_info.color <- (ci_size > curr_ci++) ? ci[curr_ci-1].tointeger() : 0;
		col_info.bgcolor <- (ci_size > curr_ci++) ? ci[curr_ci-1].tointeger() : 0;
		if(ci_size == 1) {
			col_info.colname = str;
			col_info.header = str;
		}
	}
	function calc_cols(size_absolute=false){
		calc_cols_width(size_absolute);
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
	function calc_cols_width(size_absolute=false){
		if(size_absolute){
			for(local k=0, len = cols(); k < len; ++k)
			{
			    local cs = col_style().get(k);
			    cs.width(_cols_info[k].width);
			}
			return;
		}
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
		return _data[arow][0].tointeger();
	}
}

class Fl_Data_Table_Calendar extends Fl_Data_Table {

	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
	}
	function get_style(style, R, C )
	{
		base.get_style(style, R, C);
		if (R >= 0)
		{
		    local zebra = false;

		    if ((R % 2) == 0)
		    {
			if ((C % 2) == 0) zebra = true;
		    }
		    else if((C % 2) != 0) zebra = true;

		    if (zebra) style.background(207);
		    else style.background(FL_WHITE);

		    if ((R == 0 && _data[0][C] > 7) ||
			    (R > 3 && _data[R][C] < 20))
			style.foreground(fl_color_average(FL_BLACK, FL_WHITE, 0.5));
		    else style.foreground(FL_BLACK);
		}
	}
}

dofile("search-options.nut");
dofile("utils-fltk.nut");
dofile("ourbiz-gui.nut");
dofile("help-view.nut");
dofile("fl-bar-chart.nut");
dofile("calendar-utils.nut");

class MyBarChart extends BarChartGroup {
	
	constructor(){
		base.constructor();
	}

	function periode_type ()
	{
		return btn_periode_is_years->value() ? "years" :
		       btn_periode_is_weeks->value() ? "weeks" :
		       btn_periode_is_days->value() ? "days" :
		       "months";
	}
	
	function periodes (){
		local val = periodes_to_show->value();
		return val.len() ? val.tointeger() : 0;
	}

	function show_bar_chart(data, byTwo, bySAB)
	{
		local cursor_wait = fl_cursor_wait();
		bar_chart->bar_begin_update();
		bar_chart->bar_clear();
		//bar_chart.label(_tr("Sales Statistics"));
		local cl;
		for(local i=0, max_count=data.size(); i<max_count;++i)
		{
		    local rec = data[i];
		    if(byTwo){
			cl = rec[2] == "1" ? FL_BLUE : FL_RED;
		    }
		    else if(bySAB){
			//printf("%s : %s : %s\n", rec[0].c_str(), rec[1].c_str(), rec[2].c_str());
			cl = rec[2] == "S" ? FL_BLUE : FL_GREEN;
		    } else {
			cl = FL_BLUE;
		    }
		    bar_chart->bar_add(rec[0], rec[1].tofloat(), cl);
		}
		bar_chart->bar_end_update();
	}
}

class MyListSearchWindow extends ListSearchWindow {
	_popup = null;
	_search_options = null;
	_sab = null;
	_callee_cb = null;

	//the parameter is here only to remember derived classes that it can be called
	//to create a new instance that do not start with default search results
	//mainly when it is called asyncronously (in that case a undesirable mix of both data will happen)
	constructor(doInitialSearch){
		base.constructor();
		_search_options = OurBizSearchOptions();
		setup_grid(grid, this);
		btnSelect->deactivate();
		btnNotes.callback(show_help_window);
		btnUpdate.callback(cb_btnUpdate);
		btnInsert.callback(cb_btnInsert);
		btnSelect.callback(cb_btnSelect);
		btnSearch.callback(cb_btnSearch);
		search_str.callback(cb_search_str);
	}

	function cb_btnUpdate(sender, udata){
		this = sender->window();
		row_selected(sender, Fl_Data_Table_Events.e_update);
	}
	function cb_btnInsert(sender, udata){
		this = sender->window();
		row_selected(sender, Fl_Data_Table_Events.e_insert);
	}
	function cb_btnSelect(sender, udata){
		this = sender->window();
		row_selected(sender, Fl_Data_Table_Events.e_select);
	}
	function get_edit_window(){return null;}
	
	function show_edit_window(edit_id){
		local win = get_edit_window();
		win.show();
		win.do_edit(edit_id);
	}
	
	function search_for_me(search_for_str, callee_func){
		_callee_cb = callee_func;
		btnSelect->activate();
		search_str->value(search_for_str);
		show();
		fill_grid();
	}

	function row_selected(sender, ev){
		switch(ev){
			case Fl_Data_Table_Events.e_select:
				if(_callee_cb) {
					_callee_cb(grid->get_row_id());
					_callee_cb = null;
					btnSelect->deactivate();
					hide();
				}
			break;
			case Fl_Data_Table_Events.e_insert:
				show_edit_window(0);
			break;
			case Fl_Data_Table_Events.e_update:
				show_edit_window(grid->get_row_id());
			break;
			case Fl_Data_Table_Events.e_delete:
				show_edit_window(grid->get_row_id());
			break;
		}
	}

	function get_search_data(data, search_otions){}
	function get_search_options(){ return null;}
	function fill_grid(search_otions = null){
		local cursor_wait = fl_cursor_wait();
		if(!search_otions) search_otions = get_search_options();
		grid->clear_data_rows();
		get_search_data(grid->_data, search_otions);
		grid->recalc_data();
	}
	function cb_search_str(sender, udata){
		this = sender->window();
		cb_btnSearch(sender, udata);
	}
	function cb_btnSearch(sender, udata){
		this = sender.window();
		fill_grid();
		delayed_focus(grid);
	}

	function create_search_by0(name, Klass, pack){
		local name_tr = _tr(name);
		local sb = Klass(0,0,25,20, name_tr);
		local label_font = pack->labelfont();
		local label_size = pack->labelsize();
		sb->labelsize(label_size);
		local saved_font = fl_font();
		local saved_font_size = fl_size();
		fl_font(label_font, label_size);
		sb->w(fl_width(name_tr) + fl_width("W"));
		fl_font(saved_font, saved_font_size);
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
		setup_tree_browser_for_selection(ctree->tree());
		//tree.label("----");

		ctree.callback(on_filter);
		group_filter = ctree;
	}
}

class OurTreeGroups extends GroupsListEditWindow {
	static GroupDbUpdater = class extends DBUpdateByWidget {
		constructor(){
			base.constructor();
		}
		function get_fields_map_name(){
			return "group";
		}
	};
	_table_name = null;
	
	constructor(){
		base.constructor();
		setup_tree_browser_for_selection(tree);
		tree.callback(on_tree_cb);
		btnWrapNotes.callback(on_wrap_notes_cb);
		btnWrapNotes->do_callback();
		setDbActionControls(dbAction, btnDbAction);
		dbAction->action(DbAction_Enum.e_insert);
		on_Change_dbAction();
	}
	
	function dbUpdater(dbu=null){
		if(dbu){
			_dbUpdater = dbu;
			return;
		}
		if(!_dbUpdater) _dbUpdater = new GroupDbUpdater();
		return _dbUpdater;
	}		

	function on_tree_cb(sender, udata)
	{
		this = sender->window();
		local tree = sender;
		local reason = tree->callback_reason();
		local node = tree->callback_node();
		switch(reason)
		{
		//case FLU_HILIGHTED:
		//case FLU_UNHILIGHTED:
		//case FLU_SELECTED:
		//case FLU_UNSELECTED:
		//case FLU_OPENED:
		//case FLU_CLOSED:
		case FLU_DOUBLE_CLICK:
		//case FLU_WIDGET_CALLBACK:
		//case FLU_MOVED_NODE:
		//case FLU_NEW_NODE:
		    //Flu_Tree_Browser::Node *node = tree->get_hilighted();
		    if(node) do_edit(node->user_data().tointeger());
		break;
		}
	}

	function do_edit(id)
	{
		if(!_table_name) return;
		local cursor_wait = fl_cursor_wait();
		base.do_edit(id);
		appServer.get_record(_record, dbUpdater()->table_name, 0, id);
		fill_edit_form();
		delayed_focus(db_group_description);
	}

	function on_btnDbAction()
	{
		base.on_btnDbAction();
		treeLoadChilds(tree, 0, 0, _table_name);

		fill_edit_form(true);
		dbAction->action(DbAction_Enum.e_insert);
		on_Change_dbAction();
		delayed_focus(db_group_description);
	}
	
	function on_wrap_notes_cb(sender, udata)
	{
		this = sender->window();
		db_group_notes->wrap_mode(btnWrapNotes->value(), 0);
	}
}

class OurProductGroups extends OurTreeGroups
{
	constructor(){
		base.constructor();
		label(_tr("Product Groups List/Edit"));
		_table_name = "product_groups";
		dbUpdater()->table_name = _table_name;
		treeLoadChilds(tree, 0, 0, _table_name);
		//tree->label( _tr("Product Groups") );
	}
};

class OurEntityGroups extends OurTreeGroups
{
	constructor(){
		base.constructor();        
		label(_tr("Entity Groups List/Edit"));
		_table_name = "entity_groups";
		dbUpdater()->table_name = _table_name;
		treeLoadChilds(tree, 0, 0, _table_name);
		//tree->label( _tr("Entity Groups") );
	}
};


class OurImages extends ImagesListEditWindow {
	_search_options = null;
	
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "images";
		local cols_info = [
			"id|ID|6",
			"name|Name|-1",
			"mime_type|Type|12|C",
			"group_set|Group|6",
		];
		grid->set_cols(cols_info);
		setup_grid(grid, this);
		setDbActionControls(dbAction, btnDbAction);
		_search_options = OurBizSearchOptions();
		fill_grid();
	}
	function fill_grid(){
		_search_options.query_limit = query_limit->value();
		_search_options.search_str = db_images_name->value();
		local cursor_wait = fl_cursor_wait();
		grid->clear_data_rows();
		appServer.images_get_list(grid->_data, _search_options);
		grid->recalc_data();
	}
	
	function do_edit(aid){
		base.do_edit(aid);
		if(aid) button_show_db_image(btnImage, aid.tointeger(), 0, false, false);
		else {
			btnImage->hide();
			//btnImage->image(&jpegNophoto);
			btnImage->image(null);
			btnImage->show();
		}
	}
}

class OurAppConfig extends AppConfigEditWindow {
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "config";
		local cols_info = [
			"id|ID|0",
			"key|Key|26",
			"value|Value|-1|L|@",
		];
		grid->set_cols(cols_info);
		setup_grid(grid, this);
		setDbActionControls(dbAction, btnDbAction);
		fill_grid();
	}
	function fill_grid(){
		local cursor_wait = fl_cursor_wait();
		grid->clear_data_rows();
		appServer.config_get_list(grid->_data);
		grid->recalc_data();
	}
}

class OurSalesTax extends SalesTaxRatesEditWindow {
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "sales_tax_rates";
		local cols_info = [
			"id|ID|0",
			"rate1|V.A.T. %|8|R",
			"rate2|R.E. %|8|R",
			"description|Description|-1",
			"is_active|Active|5|C|B",
		];
		grid->set_cols(cols_info);
		setup_grid(grid, this);
		setDbActionControls(dbAction, btnDbAction);
		fill_grid();
	}
	function fill_grid(){
		local cursor_wait = fl_cursor_wait();
		grid->clear_data_rows();
		appServer.sales_tax_rates_get_list(grid->_data);
		grid->recalc_data();
	}
}

class OurWarrantyTypes extends WarrantyTypesEditWindow {
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "warranty_types";
		local cols_info = [
			"id|ID|0",
			"code|Code|8",
			"description|Description|-1",
			"is_active|Active|5|C|B",
		];
		grid->set_cols(cols_info);
		setup_grid(grid, this);
		setDbActionControls(dbAction, btnDbAction);
		fill_grid();
	}
	function fill_grid(){
		local cursor_wait = fl_cursor_wait();
		grid->clear_data_rows();
		appServer.warranty_types_get_list(grid->_data);
		grid->recalc_data();
	}
}

class OurMeasureUnits extends MeasureUnitsEditWindow {
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "measure_units";
		local cols_info = [
			"id|ID|0",
			"code|Code|8",
			"description|Description|-1",
			"is_active|Active|5|C|B",
		];
		grid->set_cols(cols_info);
		setup_grid(grid, this);
		setDbActionControls(dbAction, btnDbAction);
		fill_grid();
	}
	function fill_grid(){
		local cursor_wait = fl_cursor_wait();
		grid->clear_data_rows();
		appServer.measure_units_get_list(grid->_data);
		grid->recalc_data();
	}
}

class OurPaymentTypes extends PaymentTypesEditWindow {
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "payment_types";
		local cols_info = [
			"id|ID|0",
			"code|Code|8",
			"description|Description|-1",
			"is_active|Active|5|C|B",
		];
		grid->set_cols(cols_info);
		setup_grid(grid, this);
		setDbActionControls(dbAction, btnDbAction);
		payment_periodes->tooltip(_tr("Select a periode to use when generating the payment terms"));
		payment_periodes->add(_tr("m months"));
		payment_periodes->add(_tr("w weeks"));
		payment_periodes->add(_tr("d days"));
		payment_periodes->add(_tr("dm day of month"));
		payment_periodes->value(0);
		fill_grid();
		btnGenerateTerms.callback(on_generate_terms_cb);
	}
	function fill_grid(){
		local cursor_wait = fl_cursor_wait();
		grid->clear_data_rows();
		appServer.payment_types_get_list(grid->_data);
		grid->recalc_data();
	}

	function check_payments(){
		local pattern = "(%d+)%s+(%d+%.?%d*)%D+(%d+)%s+(%S+)";
		local txt = db_payment_types_payment_terms->buffer()->text();
	}

	function on_generate_terms_cb(sender, udata){
		this = sender->window();
		local nterms = npayments->value();
		nterms = nterms.len() ? nterms.tointeger() : 1;
		local pterms = payment_periodes->value();
		local multiplier = 1;
		local buf = db_payment_types_payment_terms->buffer();
		buf->text("");
		local pct = math.roundf(100.0 / nterms, 2);
		local first_pct = (100.0 - (pct*(nterms-1)));
		local periode;

		switch(pterms){
			case 1: //weeks
				periode = "w";
			break;
			case 2: //days
				periode = "d";
				multiplier = 30;
			break;
			case 3: //day of month
				periode = "dm";
			break;
			default: //months
				periode = "m";
		}

		for(local i=1; i<=nterms; ++i){
			local str = format("%d\t%0.2f\t%d\t%s\n", i, i == 1 ? first_pct : pct, i*multiplier, periode);
			buf->append(str);
		}
		db_payment_types_payment_terms->redraw();
		db_payment_types_payment_terms->set_changed();
	}
}

class OurOrderTypes extends OrderTypesEditWindow {
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "order_types";
		local cols_info = [
			"id|ID|0|R",
			"code|Code|6|R",
			"description|Description|-1",
			"with_payment|Pay|6|C",
			"with_inventory|Inv.|6|C",
			"group_order|Grp. 1|6",
			"subgroup_order|Grp. 2|6",
			"is_active|Active|6|C|B",
		];
		grid->set_cols(cols_info);
		setup_grid(grid, this);
		setDbActionControls(dbAction, btnDbAction);
		fill_grid();
	}
	function fill_grid(){
		local cursor_wait = fl_cursor_wait();
		grid->clear_data_rows();
		appServer.order_types_get_list(grid->_data);
		grid->recalc_data();
	}
	
	function get_widget_changed(uw)
	{
		//if(!uw.validate_regex(db_order_types_with_credit, "[+-]", true))
		//    throw TWidgetValidateException(db_order_types_with_credit,
		//				   _tr("Valid options are +- !"));
		base.get_widget_changed(uw);
	}
}

function print_entities_list_contact_report(sender=null, udata=null)
{
	local mywin = sender ? sender.window() : null;
	local progress = mywin ? mywin.print_progress : null;
	if(progress) progress.maximum(10);
	
	local cursor_wait = fl_cursor_wait();
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
	    
            //to allow user do something meanwhile
            Fl_Display_Device.display_device()->set_current();
	    if(progress) {
		progress.label(_tr("Processing page ") + i);
		progress.value(i % 10);
	    }
            Fl.check();
            printer.set_current();
	}
	printer.end_job();
	if(progress) {
		progress.label(format(_tr("Processed %d pages"), npages));
		progress.value(progress.maximum());
	}
}

class MyEditEntityWindow extends EditEntityWindow {
	_sab = null;
	_ourBarChart = null;
	_ourHistory = null;
	static _history_options = [
		"--- Select one",
		"Sales by date",
		"Sales by amount",
		"Products bought",
		"Products bought by value",
		"Products bought by quantity",
	];
	
	constructor(){
		base.constructor();
		dbUpdater()->table_name = "entities";
		_sab = "A";
		setDbActionControls(dbAction, btnDbAction);
		btnEntitesListContactReport.callback(print_entities_list_contact_report);
		
		_ourBarChart = new MyBarChart();
		replace_widget_for(tabChartStatistics, _ourBarChart);
		_ourBarChart->btnShowChart.callback(on_show_chart_cb);
		
		_ourHistory = new HistoryGroup();
		replace_widget_for(tabHistory, _ourHistory);
		local choice =  _ourHistory->history_choice;
		foreach(opt in _history_options)
		    choice->add(_tr(opt));
		choice->value(0);
		choice.callback(on_history_cb);
	}
	function do_edit_delayed(udata)
	{
		base.do_edit_delayed(udata);
		delayed_focus(db_entities_name);
	}
	function on_show_chart_cb(sender, udata){
		this = sender->window();
		local cursor_wait = fl_cursor_wait();
		local mydata = [];
		appServer.entities_get_bar_chart_statistics_list(mydata,
			dbUpdater()->edit_id, _sab, _ourBarChart->periodes(),
			_ourBarChart->periode_type());
		_ourBarChart->show_bar_chart(mydata, false, (_sab == "A"));
	}
	function on_history_cb(sender, udata){
		this = sender->window();
		local cursor_wait = fl_cursor_wait();
		local data = [];
		local tbl = _ourHistory->grid_history;
		local query_limit = _ourHistory->history_query_limit->value();
		query_limit = query_limit.len() ? query_limit.tointeger() : 0;
		appServer.entities_get_sales_history_list(data,
					_ourHistory->history_choice->value(),
					query_limit, dbUpdater()->edit_id);
		tbl->set_new_data(data);
	}
}

class EntitiesListSearch extends MyListSearchWindow {
	_search_by_name = null;
	_search_by_notes = null;
	_search_by_phone = null;
	_search_by_product = null;
	_search_by_id = null;
	_search_by_active = null;
	_sales_orders_list = null;

	constructor(doInitialSearch=true) {
		base.constructor(doInitialSearch);
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
		if(doInitialSearch) delayed_method_call(this, this.fill_grid, null);
	}
	function fill_search_options(){
		pack_search_options.add(Fl_Radio_Button());
	}
	function get_search_options(){
		_search_options.search_str = search_str.value();
		_search_options.name = _search_by_name.value() == 1;
		_search_options.notes = _search_by_notes.value() == 1;
		_search_options.phone = _search_by_phone.value() == 1;
		_search_options.product_id = _search_by_product.value() == 1;
		_search_options.id = _search_by_id.value() == 1;
		_search_options.active = _search_by_active.value() == 1;
		_search_options.group_id = group_filter->get_data_at();
		return _search_options;
	}
	function get_search_data(data, so){
		appServer.entities_get_list(grid->_data, so);
	}

	function get_edit_window(){return getChildWindow("Entity Edit", MyEditEntityWindow);}
	function mk_popup()
	{
		_popup = create_popup_menu_for(grid, grid->global_style().height());
		_popup.callback(on_popupmenu_cb);

		_popup->add(_tr("Orders"));
		_popup->add(_tr("Products"));
	}

	function on_popupmenu_cb(sender, udata){
		this = sender.window();
		local row = grid->row();
		if(row < 0) return;
		switch(_popup->value()){
			case 0:{
				_sales_orders_list = OrdersListSearch(false);
				//_sales_orders_list.callback(delete_on_window_close_cb);
				local so = OurBizSearchOptions();
				so.entity_id = grid->get_row_id(row);
				so.sales = _search_options.sales;
				so.buys = _search_options.buys;
				_sales_orders_list->fill_grid(so);
				_sales_orders_list->show();
			}
			break;
			case 1:{
				local products_list = ProductsListSearch(false);
				//products_list.callback(delete_on_window_close_cb);
				local so = OurBizSearchOptions();
				so.entity_id = grid->get_row_id(row);
				so.sales = _search_options.sales;
				so.buys = _search_options.buys;
				products_list->fill_grid(so);
				products_list->show();
			}
			break;
		}
	}
}

function print_products_list(sender=null, udata=null)
{
	local mywin = sender ? sender.window() : null;
	local progress = mywin ? mywin.print_progress : null;
	if(progress) progress.maximum(10);

	local cursor_wait = fl_cursor_wait();
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
	    if(progress) {
		progress.label(_tr("Processing page ") + i);
		progress.value(i % 10);
	    }
            Fl.check();
            printer.set_current();
	}
	printer.end_job();
	if(progress) {
		progress.label(format(_tr("Processed %d pages"), npages));
		progress.value(progress.maximum());
	}
}

class MyEditProductWindow extends EditProductWindow {
	_sab = null;
	_ourBarChart = null;
	_ourHistory = null;
	_ourProductKit = null;
	_ourProductPrices = null;
	static _history_options = [
		"--- Select one",
		"Sales by date",
		"Sales by amount",
		"Entities that bought",
		"Entities that bought by value",
		"Entities that bought by quantity",
		"Price history",
		"Price history fora all products",
	];

	constructor(){
		base.constructor();
		dbUpdater()->table_name = "products";
		_sab = "A";
		setDbActionControls(dbAction, btnDbAction);

		_ourBarChart = new MyBarChart();
		replace_widget_for(tabChartStatistics, _ourBarChart);
		_ourBarChart->btnShowChart.callback(on_show_chart_cb);
		
		_ourHistory = new HistoryGroup();
		replace_widget_for(tabHistory, _ourHistory);
		local choice =  _ourHistory->history_choice;
		foreach(opt in _history_options)
		    choice->add(_tr(opt));
		choice->value(0);
		choice.callback(on_history_cb);
		
		_ourProductKit = new ProductKitGroup();
		replace_widget_for(tabKit, _ourProductKit);
		//_ourProductKit->btnShowChart.callback(on_show_chart_cb);

		_ourProductPrices = new ProductPricesGroup();
		replace_widget_for(productPrices, _ourProductPrices);
		
		tabsMoreData.callback(tabsMoreData_cb);

		load_aux_data();
	}
	function do_edit_delayed(udata)
	{
		base.do_edit_delayed(udata);
		delayed_focus(db_products_sell_description);
		local image_id = _record.get("image_id", false);
		if(image_id) button_show_db_image(btnImage, image_id.tointeger(), null, false, false);
		else {
			btnImage->hide();
			//btnImage->image(&jpegNophoto);
			btnImage->image(null);
			btnImage->show();
		}
	}
	function load_aux_data(){
		local tree = db_products_group_id;
		setup_tree_browser_for_selection(tree);
		treeLoadChilds(tree, 0, 0, "product_groups");

		local sales_tax_data = [], measure_units_data = [], warranty_data = [];

		appServer.get_product_aux_data(
				    sales_tax_data,
				    measure_units_data,
				    warranty_data);

		fill_choice_by_data(db_products_sales_tax_id, sales_tax_data);

		fill_choice_by_data(db_products_measure_unit_id, measure_units_data);

		fill_choice_by_data(db_products_warranty_id, warranty_data);
	}
	
	function tabsMoreData_cb(sender, udata){
		this = sender->window();
		if(tabsMoreData->value() == tabGroups){
			//db_products_group_id.redraw();
		}
	}
	function on_show_chart_cb(sender, udata){
		this = sender->window();
		local cursor_wait = fl_cursor_wait();
		local mydata = [];
		appServer.products_get_bar_chart_statistics_list(mydata,
			dbUpdater()->edit_id, _sab, _ourBarChart->periodes(),
			_ourBarChart->periode_type());
		_ourBarChart->show_bar_chart(mydata, false, (_sab == "A"));
	}
	function on_history_cb(sender, udata){
		this = sender->window();
		local cursor_wait = fl_cursor_wait();
		local data = [];
		local tbl = _ourHistory->grid_history;
		local query_limit = _ourHistory->history_query_limit->value();
		query_limit = query_limit.len() ? query_limit.tointeger() : 0;
		appServer.products_get_sales_history_list(data,
					_ourHistory->history_choice->value(),
					query_limit, dbUpdater()->edit_id);
		tbl->set_new_data(data);
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
	_fetching_image = null; //flag to prevent stack overflow due to multiple background calls

	constructor(doInitialSearch=true) {
		_last_image_id = 0;
		base.constructor(doInitialSearch);
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
		setFilterComboTree();
		_search_by_description = create_search_by("Description");
		_search_by_notes = create_search_by("Notes");
		_search_by_reference = create_search_by("Reference");
		_search_by_entities = create_search_by("Entities");
		_search_by_id = create_search_by("ID");
		_search_by_active = create_search_by2("Active");
		_search_by_description->setonly();
		_search_by_active->value(1);
		if(doInitialSearch) delayed_method_call(this, this.fill_grid, null);
	}

	function fill_group_filter(udata)
	{
		local tree = group_filter->tree();
		treeLoadChilds(tree, 0, 0, "product_groups");
		tree->label("----");
	}

	function get_search_options(){
		_search_options.search_str = search_str.value();
		_search_options.description = _search_by_description.value() == 1;
		_search_options.notes = _search_by_notes.value() == 1;
		_search_options.reference = _search_by_reference.value() == 1;
		_search_options.entities = _search_by_entities.value() == 1;
		_search_options.id = _search_by_id.value() == 1;
		_search_options.active = _search_by_active.value() == 1;
		_search_options.group_id = group_filter->get_data_at();
		return _search_options;
	}

	function get_search_data(data, so){
		appServer.products_get_list(grid->_data, so);
	}

	function get_edit_window(){return getChildWindow("Product Edit", MyEditProductWindow);}

	function grid_cb(sender, udata){
		//print("on_row_changed", sender, row);
		if(sender->why_event() == FLVE_ROW_CHANGED){
			this = sender.window();
			if(shown() && !_fetching_image){ 
				try {
					//flag to prevent stack overflow due to multiple background calls
					_fetching_image = true;
					local img_id = sender->get_data_value(sender->row(), sender->cols()-1);
					if(img_id){
						img_id = img_id.tointeger();
						if(img_id != _last_image_id){
							button_show_db_image(btnThumbImage, img_id, _image_window, true, false);
							_last_image_id = img_id;
						}
					}
					_fetching_image = false;
				}
				catch(e){
					_fetching_image = false;
					throw(e);
				}
			}
		}
	}
}

class MyDeliveryCalcWindow extends DeliveryCalcWindow {
	_delivery_data = null;

	constructor(){
		base.constructor();
		_delivery_data = dofile("delivery-info.json.nut");
		foreach(idx, v in _delivery_data.provincias){
			gui_destination_zone.add_item(idx, v[2]);
		}
		gui_destination_zone.callback(cb_gui_destination_zone);
	}
	function cb_gui_destination_zone(sender, udata){
		this = sender->window();
		local zone_id = gui_destination_zone.get_data_at();
		local zone = _delivery_data.provincias[zone_id];
		gui_destination_city.list().clear();
		for(local i = 3, len=zone.len(); i < len; ++i){
			gui_destination_city.add_item(zone[1], zone[i]);
		}
	}
}

class MyCalendarWindow extends CalendarWindow {
	_date = null;
	_date_source = null;
	_last_row = null;
	_last_col = null;
	static _week_days_abbr = ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"];

	constructor(){
		base.constructor();
		grid->has_scrollbar( FLVS_AUTOMATIC );
		local gs = grid.global_style();
		gs->font_size(grid->labelsize());
		gs->height(28);
		gs->resizable(false);
		grid->feature_remove(FLVF_ROW_SELECT);
		local col_size = grid->w() / _week_days_abbr.len();
		local cols_info = array(_week_days_abbr.len());
		foreach(i, v in _week_days_abbr){
			cols_info[i] = format("%s|%s|%d|R|S", v, v, col_size);
		}
		grid->set_cols(cols_info, true);
		makemonth();
	}
	function makemonth(){
		local tm = CalendarBase.makemonth(2013, 1);
		grid->set_data(tm);
	}
	function cb_gui_destination_zone(sender, udata){
		this = sender->window();
	}
}

class MyEditOrderWindow extends EditOrderWindow {
	_sab = null;
	_line_edit_id = null;
	_ourBarChart = null;
	_ourHistory = null;
	_search_entities_window = null;
	_search_products_window = null;
	_ourPrintPreview = null;
	static _history_options = [
		"--- Select one",
		"Sales by 80\\/20",
		"Sales by 80\\/20 alphabetical",
	];
	
	constructor(){
		base.constructor();
		local cols_info = [
			"id|ID|0",
			"product_id|Code|6|R",
			"description|Description|-1",
			"quantity|Quantity|8|R|N",
			"price|Price|8|R|M",
			"first_total|1st Total|8|R|M",
		];
		grid_lines->set_cols(cols_info);
		
		dbUpdater()->table_name = "orders";
		_sab = "A";
		setDbActionControls(dbAction, btnDbAction);

		_ourBarChart = new MyBarChart();
		replace_widget_for(tabChartStatistics, _ourBarChart);
		_ourBarChart->btnShowChart.callback(on_show_chart_cb);
		
		_ourHistory = new HistoryGroup();
		replace_widget_for(tabHistory, _ourHistory);
		local choice =  _ourHistory->history_choice;
		foreach(opt in _history_options)
		    choice->add(_tr(opt));
		choice->value(0);
		choice.callback(on_history_cb);

		_ourPrintPreview = new PrintPreviewGroup();
		replace_widget_for(tabPrintPreview, _ourPrintPreview);

		btnCalcDelivery.callback(cb_btnCalcDelivery);
		btnShowCalendar.callback(cb_btnShowCalendar);
		btnSearchEntity.callback(cb_btnSearchEntity);
		btnSearchProduct.callback(cb_btnSearchProduct);
	}
	function on_first_time_show(){
		local data = [];
		appServer.order_types_list_short(data, 0);
		fill_combolist_by_data(db_orders_order_type_id, data);
		appServer.payment_types_get_short_list(data);
		fill_choice_by_data(db_orders_payment_type_id, data);
	}
	
	function get_record_by_id(id){
		local lines = grid_lines->_data;
		grid_lines->clear_data_rows();
		appServer.get_record_and_array(_record, lines, dbUpdater()->table_name, 0, 
			dbUpdater()->edit_id, "&with_lines=1");
	}
	
	function do_edit_delayed(udata){
		base.do_edit_delayed(udata);
		local lines = grid_lines->_data;
		_line_edit_id = 0;
		if(lines.size()) lines.remove(0); //remove headers from data vector
		redraw_lines(false);
		delayed_focus(db_orders_entity_id);
	}

	function fill_edit_form(asBlank=false){
		base.fill_edit_form(asBlank);
		fill_edit_lines_form(true, false); //clear lines entries
		fill_edit_form_totals(_record, asBlank);
		fill_edit_form_entity(_record, asBlank);

		if(asBlank)
		{
		    db_orders_series->value("A");
		    db_orders_order_date->value(cDate().AsSQLString());
		    db_orders_payment_type_id->my_set_value(1);
		    db_orders_order_type_id->select_by_data(1);
		    db_orders_entity_id->value("1");
		    db_orders_entity_id->set_changed();
		}
		else
		{
		/*
		    LOWFQ(order_date);
		    LOWFQ(payment_type_id);
		    LOWFQ(series);
		    LOWFQ(order_type_id);
		*/
		}
	}

	function redraw_lines(select_last){
		grid_lines->recalc_data();
		if(select_last) grid_lines->row(grid_lines->rows()-1);
		else grid_lines->top_row(0);
		grid_lines->redraw();
	}

	function get_record(id){
		appServer.get_record(_record, _main_table, 0, id, "&with_lines=1");
	}
	
	function set_decimal_places(dp){
		db_orders_lines_price->decimal_places(dp);
		db_orders_lines_first_total->decimal_places(dp);
		db_orders_lines_discount_amt->decimal_places(dp);
		db_orders_lines_line_subtotal->decimal_places(dp);
		db_orders_lines_sales_tax1_amt->decimal_places(dp);
		db_orders_lines_line_total->decimal_places(dp);
	}
	
	function fill_edit_form_entity(myrec, asBlank=false, setChanged=false){
	}
	
	function fill_edit_form_totals(myrec, asBlank=false){
	}
	
	function fill_edit_lines_form(asBlank=false, doFocus=true, doCalc=true){
		if(doFocus) {
		    linesTab->value(group_lines);
		    delayed_focus(db_orders_lines_quantity);
		}
	}
	
	function cb_btnCalcDelivery(sender, udata){
		this = sender->window();
		local dc = getChildWindow("Delivery Calc", MyDeliveryCalcWindow);
		dc.show();
	}
	function cb_btnShowCalendar(sender, udata){
		this = sender->window();
		local dc = getChildWindow("Calendar", MyCalendarWindow);
		dc.show();
	}
	function getEntitiesListSearchWindow(){
		if(!_search_entities_window){
			_search_entities_window = new EntitiesListSearch();
		}
		return _search_entities_window;
	}
	function validate_enity_id(entity_id){
		db_orders_entity_id->value(entity_id.tostring());
	}
	function cb_btnSearchEntity(sender, udata){
		this = sender->window();
		if(sender == db_orders_entity_name && db_orders_entity_id->value())
		{
		    //if we have a valid entity id we accept direct changes to entity name
		    linesTab->value(group_lines);
		    delayed_focus(db_orders_lines_description);
		    return;
		}
		local swin = getEntitiesListSearchWindow();
		local cb = function(entity_id) {
				show();
				validate_enity_id(entity_id);
			}
		cb.setenv(this);
		swin->search_for_me(db_orders_entity_name->value(), cb);		
	}
	
	function getProductsListSearchWindow(){
		if(!_search_products_window){
			_search_products_window = new ProductsListSearch();
		}
		return _search_products_window;
	}
	function validate_product_id(product_id){
		db_orders_lines_product_id->value(product_id.tostring());
	}
	function cb_btnSearchProduct(sender, udata){
		this = sender->window();
		if(sender == db_orders_lines_description)
		{
		    if(db_orders_lines_product_id->value())
		    {
			//if we have a valid product id we accept direct changes to product description
			return;
		    }
		    local description = db_orders_lines_description->value();
		    if(!description.len())
		    {
			//empty description do nothing
			return;
		    }
		}
		local swin = getProductsListSearchWindow();
		local cb = function(product_id) {
				show();
				validate_product_id(product_id);
			}
		cb.setenv(this);
		swin->search_for_me(db_orders_lines_description->value(), cb);		
	}
		
	function on_show_chart_cb(sender, udata){
		this = sender->window();
		local paidUpaid = _ourBarChart->chkOpt->value();
		local cursor_wait = fl_cursor_wait();
		local mydata = [];
		appServer.orders_get_bar_chart_statistics_list(mydata, _sab, 
			_ourBarChart->periodes(), _ourBarChart->periode_type() , paidUpaid);
		_ourBarChart->show_bar_chart(mydata, paidUpaid, (_sab == "A"));	
	}
	function on_history_cb(sender, udata){
		this = sender->window();
		local cursor_wait = fl_cursor_wait();
		local data = [];
		local tbl = _ourHistory->grid_history;
		local query_limit = _ourHistory->history_query_limit->value();
		query_limit = query_limit.len() ? query_limit.tointeger() : 0;
		appServer.orders_get_sales_history_list(data,
					_ourHistory->history_choice->value(), query_limit);
		tbl->set_new_data(data);
	}
}

class OrdersListSearch extends MyListSearchWindow {
	_search_by_entities = null;
	_search_by_notes = null;
	_search_by_products = null;
	_search_by_date = null;
	_search_by_total = null;
	_search_wp = null;

	constructor(doInitialSearch=true) {
		base.constructor(doInitialSearch);
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
		if(doInitialSearch) delayed_method_call(this, this.fill_grid, null);
	}

	function get_data_group_filter(data)
	{
		appServer.order_types_list_short(data, 0);
	}

	function get_search_options(){
		_search_options.search_str = search_str.value();
		_search_options.entities = _search_by_entities.value() == 1;
		_search_options.notes = _search_by_notes.value() == 1;
		_search_options.products = _search_by_products.value() == 1;
		_search_options.date = _search_by_date.value() == 1;
		_search_options.total = _search_by_total.value() == 1;
		_search_options.group_id = group_filter->get_data_at();
		return _search_options;
	}

	function get_search_data(data, so){
		appServer.orders_get_list(grid->_data, so);
	}

	function get_edit_window(){return getChildWindow("Order Edit", MyEditOrderWindow);}

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
		this = sender.window();
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

class PaymentsListSearch extends ListSearchWindow {

	constructor() {
		base.constructor();
		label(_tr("Payments List Search"));
	}
}

class MyMainWindow extends MainWindow {
	constructor() {
		base.constructor();
		label(_tr("OurBiz"));
		btnOrdersSales->callback(cb_btnOrdersSales);
		btnPaymentsSales->callback(cb_btnPaymentsSales);
		btnEntitiesSales->callback(cb_btnEntitiesSales);
		btnProductsSales->callback(cb_btnProductsSales);
		
		btnOrdersBuys->callback(cb_btnOrdersBuys);
		btnPaymentsBuys->callback(cb_btnPaymentsBuys);
		btnProductsBuys->callback(cb_btnProductsBuys);
		btnEntitiesBuys->callback(cb_btnEntitiesBuys);
		
		btnOrders->callback(cb_btnOrders);
		btnPayments->callback(cb_btnPayments);
		btnEntities->callback(cb_btnEntities);
		btnProducts->callback(cb_btnProducts);
		
		btnGLGroups->callback(cb_btnGLGroups);
		btnGLChart->callback(cb_btnGLChart);
		btnGLTransactions->callback(cb_btnGLTransactions);
		
		btnOrdersSum->callback(cb_btnOrdersSum);
		btnSalesTaxRates->callback(cb_btnSalesTaxRates);
		btnOrderTypes->callback(cb_btnOrderTypes);
		btnPaymentTypes->callback(cb_btnPaymentTypes);
		btnMeasureUnits->callback(cb_btnMeasureUnits);
		btnWarrantyTypes->callback(cb_btnWarrantyTypes);
		btnImages->callback(cb_btnImages);
		btnProductGroups->callback(cb_btnProductGroups);
		btnEntityGroups->callback(cb_btnEntityGroups);
		btnConfig->callback(cb_btnConfig);
		btnOpenDB->callback(cb_btnOpenDB);
		btnTranslations->callback(cb_btnTranslations);
		btnAppUsers->callback(cb_btnAppUsers);
	}
	function cb_btnEntitiesSales(sender, udata){
		this = sender.window();
		local win = showChildWindow("Entities Sales List/Search", EntitiesListSearch);
	}
	function cb_btnPaymentsSales(sender, udata){
		this = sender.window();
		local win = showChildWindow("Payments Sales List/Search", PaymentsListSearch);
	}
	function cb_btnOrdersSales(sender, udata){
		this = sender.window();
		local win = showChildWindow("Orders Sales List/Search", OrdersListSearch);
	}
	function cb_btnProductsSales(sender, udata){
		this = sender.window();
		local win = showChildWindow("Products Sales List/Search", ProductsListSearch);
	}
	function cb_btnOrdersBuys(sender, udata){
		this = sender.window();
		local win = showChildWindow("Orders Buys List/Search", OrdersListSearch);
	}
	function cb_btnPaymentsBuys(sender, udata){
		this = sender.window();
		local win = showChildWindow("Payments Buys List/Search", PaymentsListSearch);
	}
	function cb_btnProductsBuys(sender, udata){
		this = sender.window();
		local win = showChildWindow("Products Buys List/Search", ProductsListSearch);
	}
	function cb_btnEntitiesBuys(sender, udata){
		this = sender.window();
		local win = showChildWindow("Entities Buys List/Search", EntitiesListSearch);
	}
	function cb_btnOrders(sender, udata){
		this = sender.window();
		local win = showChildWindow("Orders List/Search", OrdersListSearch);
	}
	function cb_btnPayments(sender, udata){
		this = sender.window();
		local win = showChildWindow("Payments List/Search", PaymentsListSearch);
	}
	function cb_btnEntities(sender, udata){
		this = sender.window();
		local win = showChildWindow("Entities List/Search", EntitiesListSearch);
	}
	function cb_btnProducts(sender, udata){
		this = sender.window();
		local win = showChildWindow("Products List/Search", ProductsListSearch);
	}
	function cb_btnGLGroups(sender, udata){print(__LINE__);}
	function cb_btnGLChart(sender, udata){print(__LINE__);}
	function cb_btnGLTransactions(sender, udata){print(__LINE__);}
	function cb_btnOrdersSum(sender, udata){print(__LINE__);}
	function cb_btnSalesTaxRates(sender, udata){
		this = sender.window();
		local win = showChildWindow("Sales Tax Rates List/Edit", OurSalesTax);
	}
	function cb_btnOrderTypes(sender, udata){
		this = sender.window();
		local win = showChildWindow("Order Types List/Edit", OurOrderTypes);
	}
	function cb_btnPaymentTypes(sender, udata){
		this = sender.window();
		local win = showChildWindow("Payment Types List/Edit", OurPaymentTypes);
	}
	function cb_btnMeasureUnits(sender, udata){
		this = sender.window();
		local win = showChildWindow("Measure Units List/Edit", OurMeasureUnits);
	}
	function cb_btnWarrantyTypes(sender, udata){
		this = sender.window();
		local win = showChildWindow("Warranty Types List/Edit", OurWarrantyTypes);
	}
	function cb_btnImages(sender, udata){
		this = sender.window();
		local win = showChildWindow("Images List/Edit", OurImages);
	}
	function cb_btnProductGroups(sender, udata){
		this = sender.window();
		local win = showChildWindow("Product Groups", OurProductGroups);
	}
	function cb_btnEntityGroups(sender, udata){
		this = sender.window();
		local win = showChildWindow("Entity Groups", OurEntityGroups);
	}
	function cb_btnConfig(sender, udata){
		this = sender.window();
		local win = showChildWindow("App Config", OurAppConfig);
	}
	function cb_btnOpenDB(sender, udata){print(__LINE__);}
	function cb_btnTranslations(sender, udata){print(__LINE__);}
	function cb_btnAppUsers(sender, udata){print(__LINE__);}
	function reset_all_child_windows_ptr(sender, udata){print(__LINE__);}
}

local win = new MyMainWindow();
win->resizable(win);
win->show_main();

Fl.run();
socket.close();
