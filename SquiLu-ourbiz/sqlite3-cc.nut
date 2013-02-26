function _tr(str) {return str;}

class Fl_Multiline_Output extends Fl_Output {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		type(4);
	}
}

enum Fl_Data_Table_Events {e_none, e_event, e_select, e_insert, e_update, e_delete};

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
		has_scrollbar(FLVS_BOTH);
	}

	function for_print(bval){_forPrint = bval; }

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

dofile("sqlite3-cc-gui.nut", false, false);

class Sqlite3CC extends Sqlite3cc_Window {
  
	// Declaration of class members
	db_file_name = null;
	label_title = null;
	db = null;
	attached_databases = null;
	squilu_edit_window = null;
  
	constructor(){
		base.constructor();
		menuBar.callback(menuBar_cb);
		btnExecute.callback(btnExecute_cb);
		btnCreateQuery.callback(btnCreateQuery_cb);
		grid_tables._call_this = this;
	}
	
	function row_selected(sender, ev){
		if(sender == grid_tables, ev){
			if(ev == Fl_Data_Table_Events.e_update){
				local tbl = grid_tables.get_value(grid_tables.row(), 1);
				local sql = "select * from " + tbl;
				local limit = iMaxRows.value();
				if(limit && limit.len()) sql += " limit " + limit;
				get_records_by_sql(grid_data, sql, true);
			}
		}
	}
  
	function btnExecute_cb(sender, udata){
		this = sender->window();
		local sql = edit_queries->value();
		if(sql && sql.len()) get_records_by_sql(grid_data, sql, true);
	}
  
	function btnCreateQuery_cb(sender, udata){
		this = sender->window();
		if(db){
			local tbl = grid_tables.get_value(grid_tables.row(), 1);
			local sql = option_query.text();
			local stmt = db.prepare("select * from " + tbl);
			local fields = stmt.colsAsArray();
			local str_fields =  fields.concat(", ");
			stmt.finalize();
			if(sql == "select") sql = format("select %s\nfrom %s", str_fields, tbl);
			else if(sql == "insert") sql = format("insert into %s(%s)\nvalues(%s)", tbl, str_fields, str_fields);
			else if(sql == "update") {
				str_fields = fields.concat("=?, ");
				sql = format("update %s set %s=?\nwhere id=?", tbl, str_fields);
			}
			else if(sql == "delete") sql = format("delete from %s where id=?", tbl);
			else return;
			edit_queries->value(sql);
		}
	}
  
	function set_label_dbf(dbf){
		db_file_name = dbf;
		label_title = "SqliteCC";
		if (dbf) label_title += " - " + dbf;
		label(label_title);
	}

	function get_records_by_sql(grid, sql , named=false){
		local cursor_wait = fl_cursor_wait();
		local stmt = db.prepare(sql);
		local rec_list = stmt.asArrayOfArrays(SQLite3Stmt.WITH_COL_NAMES | 
			SQLite3Stmt.AS_STRING_ALWAYS | SQLite3Stmt.NULL_AS_EMPTY_STR);
		grid.set_new_data(rec_list);
	}

	function get_tables(){
		local sql = "select rowid as 'rowid|ID|0', name as 'name|Name|-1', type as 'type|Type|4' from sqlite_master where (type='table' OR type='view') order by name";
		get_records_by_sql(grid_tables, sql, true);
	}

	function menuBar_cb(sender, udata){
		this = sender->window();
		//fl_alert(format("%d = %s", sender.value(), sender.text()));
		//print(sender, udata, sender.value(), menu_file_exit);
		local choice = sender.value();
		if(choice == menu_file_new) {
			local dbf = fl_file_chooser(_tr("Select a database"), "*.db", null);
			if (dbf) {
				//print(dbf);
				if (db) db.close();
				attached_databases = {};
				set_label_dbf(dbf);
				db = SQLite3(dbf);
			}
		}
		else if(choice == menu_file_open) {
			local dbf = fl_file_chooser(_tr("Select a database"), "*.db", null);
			if (dbf){
				//print(dbf);
				if (db) db.close();
				attached_databases = {};
				set_label_dbf(dbf);
				// SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_SUBLATIN_NA_LIKE
				db = SQLite3(dbf);
				get_tables();
			}
		}
		else if(choice == menu_file_attach) {
			local dbf = fl_file_chooser(_tr("Select a database"), "*.db", null);
			if (dbf){
				//print(dbf)
				local dbname = fl_input(_tr("Attach database with name ?"), "adb");
				if (dbname && !attached_databases.get(dbname, false)){
					attached_databases[dbname] <- true;
					local sql = format("attach database '%s' as %s;", dbf, dbname);
					db.exec_dml(sql);
					get_tables();
				}
			}
		}
		else if(choice == menu_file_close) {
			if (db){
				db.close();
				db = null;
				set_label_dbf(null);
				grid_tables.clear_data_rows();
				attached_databases = {};
			}
		}
		else if(choice == menu_file_execute) {
		}
		else if(choice == menu_file_exit) os.exit();
		else if(choice == menu_help_about) {
		}
		else if(choice == menu_squilu_edit) {
			if(!squilu_edit_window){
				squilu_edit_window = new SquiLuEditWindow();
			}
			squilu_edit_window.show();
		}
	}
}

Fl.scheme("gtk+");
Fl.visual(FL_RGB);
//allow arrow keys navigation
Fl.option(Fl.OPTION_ARROW_FOCUS, true);

local win = new Sqlite3CC();
//local win = new SalesTaxRatesEditWindow();
win->resizable(win);
win->show_main();

Fl.run();