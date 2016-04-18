#!/home/mingo/bin/squilu
/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
dofile("sqlite-utils.nut");
 
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
		col(0);
		cols(0);
		row(0);
		rows(0);
		_data.clear();
	}
	function recalc_data(){
		col(0);
		row(0);
		rows(0);
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
				local clicks = Fl.event_clicks();
				if(clicks > 0){
					row_selected(Fl_Data_Table_Events.e_update);
					done = true;
				}
				else row_changed();
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
	function row_changed(){ if(_call_this) _call_this.row_changed(this);}
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
	function get_col_name(idx){
		return _cols_info.len() ? _cols_info[idx].colname : idx.tostring();
	}
	function get_row(arow=null){
		if(arow == null) arow = row();
		return _data.len() ? _data[arow] : null;
	}
	function get_row_id(arow=null){
		if(arow == null) arow = row();
		return _data.len() ? _data[arow][0].tointeger() : 0;
	}
	function get_row_field_by_name(field_name, arow=null){
		if(arow == null) arow = row();
		foreach(idx, ci in _cols_info)
		{
			if(ci.colname == field_name)
			{
				return _data.len() ? _data[arow][idx] : null;
			}
		}
		return null;
	}
	function select_row_by_id(the_id){
		for(local i=0, len=_data.len(); i < len; ++i)
		{
			if(_data[i][0] == the_id)
			{
				row(i);
				return i;
			}
		}
		return -1;
	}
	
	function delete_row_id(arow=null)
	{
		if(arow == null) arow = row();
		_data.remove(arow);
		recalc_data();
	}
}

dofile("sqlite3-cc-gui.nut", false, false);

local function multiply3(ctx,a,b,c){
	//print(ctx.user_data());
	ctx.result_double(a*b*c);
}

local function sqlite3_progress_handler(window)
{
	//print("sqlite3_progress_handler");
	Fl.check();
	return window.ref()._stop_processing_query ? 1 : 0;
}

class Sqlite3CC extends Sqlite3cc_Window {
  
	// Declaration of class members
	db_file_name = null;
	_the_schema_version = null;
	_my_tables_data = null;
	label_title = null;
	db = null;
	attached_databases = null;
	squilu_edit_window = null;
	_busy_working = null;
	_stop_processing_query = null;
	//_history_db_name = null;
	//_history_db = null;
	_hide_system_tables = false;
  
	constructor(){
		base.constructor();
		menuBar.callback(menuBar_cb);
		btnExecute.callback(btnExecute_cb);
		btnCreateQuery.callback(btnCreateQuery_cb);
		tabView.callback(tabView_cb);
		grid_tables._call_this = this.weakref();
		grid_data._call_this = this.weakref();
		grid_fields._call_this = this.weakref();
		gridIndexes._call_this = this.weakref();
		gridTriggers._call_this = this.weakref();
		local last_db_file_name = Fl.preferences_get("last_db_file_name", "");
		openDB(last_db_file_name);
		iTablesFilter.callback(doDataSearch_cb);
		chkSytemTables.callback(showHideSystemTables_cb);
	}
	
	function setBusyWorking(turnOn)
	{
		if(turnOn && _busy_working)
		{
			_stop_processing_query = true;
			return false;
		}
		_busy_working = turnOn;
		_stop_processing_query = false;
		return true;
	}

	function doDataSearch_cb(sender : Fl_Widget, udata : any)
	{
		this = sender->window();
		local filtered_data = doFilterMyData(iTablesFilter->value(), grid_tables->_data);
		grid_tables->set_data(filtered_data);
	}
	
	function filterSystemTables()
	{
		local the_data = _my_tables_data;
		local filtered_data = [];
		foreach(rec in the_data)
		{
			local tbl_name = rec[1];
			if(_hide_system_tables && tbl_name.len() && tbl_name.startswith("__")) continue;
			filtered_data.push(rec);
		}
		return filtered_data
	}
	
	function showHideSystemTables()
	{
		_hide_system_tables = chkSytemTables->value();
		local filtered_data = filterSystemTables();
		grid_tables->set_data(filtered_data);
	}
	
	function showHideSystemTables_cb(sender : Fl_Widget, udata : any)
	{
		this = sender->window();
		showHideSystemTables();
	}
	
	function doFilterMyData(value, prev_filtered_data)
	{
		local the_data = prev_filtered_data;
		local value_len = value.len();
		//print(__LINE__, _my_tables_data.len(), the_data.len(), value.len(), value);
		if(value_len == 0) return _hide_system_tables ? filterSystemTables() : _my_tables_data;
		
		if(value_len == 1 || the_data.len() == 0)
		{
			//we start from scratch
			return _hide_system_tables ? filterSystemTables() : _my_tables_data;
		}
		local filtered_data = [];
		foreach(rec in the_data)
		{
			if(_hide_system_tables)
			{
				local tbl_name = rec[1];
				if(tbl_name.len() && tbl_name.startswith("__")) continue;
			}
			foreach(field in rec)
			{
				if(field)
				{
					local fs = field.tostring();
					if(fs.len() && fs.indexOf(value) >= 0)
					{
						filtered_data.push(rec);
						break;
					}
				}
			}
		}
		return filtered_data;
	}
	
	function getGridTableAttachedDB()
	{
		return (grid_tables.cols() > 3) ? (grid_tables.get_value(grid_tables.row(), 3) + ".") : "";
	}
	
	function checkSchemaVersion()
	{
		if(_the_schema_version != SQLiteUtils.getSchemaVersion(db, getGridTableAttachedDB()))
		{
			local tables_idx = grid_tables->row();
			get_tables(false);
			local v = iTablesFilter->value();
			if(v.len())
			{
				doDataSearch_cb(iTablesFilter, null);
			}
			else showHideSystemTables();
			grid_tables->row(tables_idx < grid_tables->rows() ? tables_idx : grid_tables->rows()-1);
		}
	}
	
	function showExecutionTime(start_time)
	{
		local end_time = os.clock();
		output_messages->value("Execution time = " + (end_time - start_time) + " seconds");
		tabsLog->value(groupMessages);
	}

	function row_changed(sender)
	{
		if(sender == grid_tables){
			local tbl = grid_tables.get_value(grid_tables.row(), 1);
			refreshTabView(tbl, getGridTableAttachedDB()); 
		}
	}
	
	function showRecord(the_grid)
	{
		local row = the_grid.get_row();
		local record = blob(0, 8000);
		foreach(idx, val in row)
			record.write((idx ? "\n" : ""), "----", idx, ":", 
				grid_data.get_col_name(idx),"\n", val);
		edit_record.value(record.tostring());
		tabView->value(groupRecord);
	}
	
	function row_selected(sender, ev){
		if(sender == grid_tables){
			if(ev == Fl_Data_Table_Events.e_update){
				local tbl = grid_tables.get_value(grid_tables.row(), 1);
				local sql = format("select * from %s\"%s\"", getGridTableAttachedDB(), tbl);
				local limit = iMaxRows.value();
				if(limit && limit.len()) sql += " limit " + limit;
				local start_time = os.clock();
				get_records_by_sql(grid_data, sql, true);
				showExecutionTime(start_time);
				tabView->value(groupData);
			}
		}
		else if(
			(sender == grid_fields) || 
			(sender == grid_data) || 
			(sender == gridIndexes) || 
			(sender == gridTriggers)
			){
				if(ev == Fl_Data_Table_Events.e_update){
					showRecord(sender);
				}
		}
	}
  
	function btnExecute_cb(sender, udata){
		this = sender->window();
		if(!setBusyWorking(true)) return;
		local sql;
		if(edit_queries->buffer()->selected()) sql = edit_queries->buffer()->selection_text();
		else sql = edit_queries->value();
		
		local action = option_query.text();
		if( (sql && sql.len()) || (action == "sql update metadata")) {
			local start_time;
			local foreign_keys_saved = null;
			local cursor_wait = fl_cursor_wait();
			switch(action)
			{
				case "references":
					local references = SQLiteUtils.getReferencesOnDBSchema(db, sql, getGridTableAttachedDB());
					edit_references->value(references);
					tabView->value(groupReferences);
				break;

				case "search all tables":
					local references = SQLiteUtils.searchOnAllTables(db, sql, iMaxRows->value().tointeger(), getGridTableAttachedDB());
					edit_references->value(references);
					tabView->value(groupReferences);
				break;

				case "schema update":
				case "schema update norefs":
					foreign_keys_saved = db.exec_get_one(format("PRAGMA %sforeign_keys", getGridTableAttachedDB()));
				case "insert":
				case "update":
				case "delete":
				case "create index":
				case "create trigger":
				case "sqlite_master update":
				case "drop table":
					try
					{
						start_time = os.clock();
						sql = edit_queries->value();

						db.exec_dml(sql);

						showExecutionTime(start_time);

						switch(action)
						{
							case "sqlite_master update":
								_the_schema_version = 0;
							case "create index":
							case "create trigger":
							case "schema update":
							case "schema update norefs":
							case "drop table":
								checkSchemaVersion();
							break;
						}						
					}
					catch(e)
					{
						if(!db.IsAutoCommitOn() || action.startswith("schema update")) db.exec_dml("ROLLBACK;");
						if(foreign_keys_saved != null) db.exec_dml(format("PRAGMA %sforeign_keys=" + foreign_keys_saved, getGridTableAttachedDB()));
						fl_alert(e);
					}
				break;
				
				case "dump table":
				break;

				case "sql update metadata":
					start_time = os.clock();
					SQLiteUtils.doUpdateMetadata(db);
					showExecutionTime(start_time);
					checkSchemaVersion();
				break;

				case "sql macros base":
					local sql_pp = new SqlPreprocessor();
					local new_sql = sql_pp.getPreprocessorQuery(db, sql, "base");
					if(new_sql) edit_queries->value(new_sql);
				break;
				case "sql macros sql":
					local sql_pp = new SqlPreprocessor();
					local new_sql = sql_pp.getPreprocessorQuery(db, sql, "sql");
					if(new_sql) edit_queries->value(new_sql);
				break;
				case "sql macros insert":
					local sql_pp = new SqlPreprocessor();
					sql_pp.insertPreprocessorQuery(db, sql);
				break;
				case "sql macros update":
					local sql_pp = new SqlPreprocessor();
					sql_pp.updatePreprocessorQuery(db, sql);
				break;
				case "sql macros":
					local sql_pp = new SqlPreprocessor();
					local new_sql = sql_pp.getPreprocessorQuery(db, sql);
					if(new_sql)
					{
						sql = sql_pp.preprocessSqlQueryParams(new_sql, {});
						edit_queries->value(sql);
					}
					else break;

				default:
					try
					{
						start_time = os.clock();
						get_records_by_sql(grid_data, sql, true, false);
						showExecutionTime(start_time);
						tabView->value(groupData);
						checkSchemaVersion();
					}
					catch(e)
					{
						if(!db.IsAutoCommitOn()) db.exec_dml("ROLLBACK;");
						fl_alert(e);
					}
			}
		}
		setBusyWorking(false);
	}
  
	function btnCreateQuery_cb(sender, udata){
		this = sender->window();
		if(db){
			local tbl = grid_tables.get_value(grid_tables.row(), 1);
			local sql = SQLiteUtils.createQuery(db, tbl, option_query.text(), iMaxRows.value().tointeger(), getGridTableAttachedDB());
			if(sql) edit_queries->value(sql);
		}
	}
	
	function refreshTabView(tbl, attached_db)
	{
		local tab = tabView->value();
		local sql = false;
		local the_grid = false;
		if(tab == viewFields)
		{
			sql = SQLiteUtils.getTableInfoSqlFor(tbl, attached_db);
			the_grid = grid_fields;
		}
		if(tab == viewIndexes)
		{
			sql = SQLiteUtils.getIndexesSqlFor(tbl, attached_db);
			the_grid = gridIndexes;
		}
		else if(tab == viewTriggers)
		{
			sql = SQLiteUtils.getTriggersSqlFor(tbl, attached_db);
			the_grid = gridTriggers;
		}
		else if(tab == groupSchema)
		{
			local str = SQLiteUtils.getSchemaFor(db, tbl, attached_db);
			edit_schema->value(str);
		}

		if(sql && the_grid)
		{
			get_records_by_sql(the_grid, sql, true);
		}
	}
  
	function tabView_cb(sender, udata){
		this = sender->window();
		//print(tab->label());
		local tbl = grid_tables.get_value(grid_tables.row(), 1);
		if(tbl)
		{
			refreshTabView(tbl, getGridTableAttachedDB());
		}
	}
  
	function set_label_dbf(dbf){
		db_file_name = dbf;
		label_title = "SqliteCC";
		if (dbf) label_title += " - " + dbf;
		label(label_title);
	}
	
	function queryAsArrayOfArrays(sql)
	{
		local result = [];
		local qry = db.exec_query(sql);
		local col_count = qry.col_count();
		local header = [];
		for(local i=0; i < col_count; ++i){
			header.append(qry.col_name(i));
		}
		result.append(header);
		while(qry.next_row()){
			result.append(qry.row_as_array());
		}
		qry.close();
		return result;
	}

	function get_records_by_sql(grid, sql , named=false, withBusyWorking=true){
		if(withBusyWorking && !setBusyWorking(true)) return;
		local cursor_wait = fl_cursor_wait();
		local stmt = db.prepare(sql);
		local rec_list = stmt.asArrayOfArrays(SQLite3Stmt.WITH_COL_NAMES | 
			SQLite3Stmt.AS_STRING_ALWAYS | SQLite3Stmt.NULL_AS_EMPTY_STR);
		grid.set_new_data(rec_list);
		if(withBusyWorking) setBusyWorking(false);
	}

	function get_tables(withBusyWorking=true){
		local with_attached = table_len(attached_databases) > 0;
		local sql_template = "SELECT rowid AS 'rowid|ID|0', name AS 'name|Name|-1', type as 'type|Type|4'";
		if(with_attached) sql_template += ", '%s' AS 'db|DB|4'";
		sql_template += " FROM %s.sqlite_master WHERE (type='table' OR type='view')";
		local main_prefix = "main";
		local sql = format(sql_template, main_prefix, main_prefix);
		foreach(k,v in attached_databases)
		{
			sql += "\nUNION ALL\n" + format(sql_template, k, k);
		}
		sql += "\nORDER BY " + (with_attached ? "4,2" : "2");
		get_records_by_sql(grid_tables, sql, true, withBusyWorking);
		_the_schema_version = SQLiteUtils.getSchemaVersion(db, getGridTableAttachedDB());
		_my_tables_data = grid_tables->_data;
	}
	
	function openDB(the_db_file_name)
	{
		if (the_db_file_name) {
			//print(dbf);
			if (db) db.close();
			attached_databases = {};
			set_label_dbf(the_db_file_name);
			// SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_SUBLATIN_NA_LIKE
			db = SQLite3(the_db_file_name);
			db.progress_handler(1000, sqlite3_progress_handler, this.weakref());
			//db.exec_dml("PRAGMA mmap_size=268435456;");
			//db.trace(function(udata, sql){print(udata, ":", sql);}, "SQL");
			//db.create_function("multiply3",3, multiply3); //SQLITE_DETERMINISTIC
			get_tables();
			Fl.preferences_set("last_db_file_name", the_db_file_name);
			//_history_db_name = Fl.preferences_getUserdataPath() + "/sqlite3_cc_history.db";
			//_history_db = SQLite3(_history_db_name);
		}
	}

	function menuBar_cb(sender, udata){
		this = sender->window();
		//fl_alert(format("%d = %s", sender.value(), sender.text()));
		//print(sender, udata, sender.value(), menu_file_exit);
		local choice = sender.value();
		local path = db_file_name ? db_file_name.gsub("/[^/]+$", "") : null;
		if(choice == menu_file_new) {
			local dbf = fl_file_chooser(_tr("Select a database"), "*.db", path);
			openDB(dbf);
		}
		else if(choice == menu_file_open) {
			local dbf = fl_file_chooser(_tr("Select a database"), "*.db", path);
			openDB(dbf);
		}
		else if(choice == menu_file_reopen) {
			if(db) openDB(db_file_name);
		}
		else if(choice == menu_file_attach) {
			local dbf = fl_file_chooser(_tr("Select a database"), "*.db", path);
			if (dbf){
				//print(dbf)
				local dbname = fl_input(_tr("Attach database with name ?"), "adb");
				if (dbname && !table_get(attached_databases, dbname, false)){
					attached_databases[dbname] <- true;
					local sql = format("ATTACH DATABASE '%s' as %s;", dbf, dbname);
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
		else if(choice == menu_file_open_csv) {
			local csvf = fl_file_chooser(_tr("Select a CSV file"), "*.csv", path);
			local sep = fl_input("CSV Separator", ",");
			local hasHeaders = fl_ask("The CSV first line is header ?");
			if(sep)
			{
				local cursor_wait = fl_cursor_wait();
				local max_rows = iMaxRows.value().tointeger();
				local fd = file(csvf, "r");
				local line;
				local rec_list = [];
				local isFirstLine = true;
				while( (line = fd.read_line()) )
				{
					local record = line.split_csv(sep[0]);
					if(isFirstLine)
					{
						if(!hasHeaders)
						{
							local col_count = record.len();
							local header = array(col_count);
							for(local i=0; i < col_count; ++i) header[i] = "col" + i;
							rec_list.append(header);
						}
						isFirstLine = false;
					}
					rec_list.append(record);
				}
				fd.close();
				grid_data.set_new_data(rec_list);
				tabView->value(groupData);
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

fl_preferences(Fl.FL_PREFERENCES_USER, "dadbiz", "sqlite3-cc");
Fl.scheme("gtk+");
Fl.visual(FL_RGB);
//allow arrow keys navigation
Fl.option(Fl.OPTION_ARROW_FOCUS, true);

local win = new Sqlite3CC();
//local win = new SalesTaxRatesEditWindow();
win->resizable(win);
win->show_main();

Fl.run();

Fl.preferences_flush();
