#!/home/mingo/bin/squilu
/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
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
		return _cols_info[idx].colname;
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

function create_stmt_bind(db, sql, bind_values=null)
{
	local result = false;
	local stmt = db.prepare(sql);
	if(bind_values)
	{
		foreach(k,v in bind_values)
		{
			stmt.bind(k+1, v);
		}
	}
	return stmt;
}

function exec_get_all(db, sql, bind_values=null)
{
	local stmt = create_stmt_bind(db, sql, bind_values);
	local result = stmt.asArrayOfTables();
	stmt.finalize();
	return result;
}

function exec_get_one(db, sql, bind_values=null)
{
	local result = null;
	local stmt = create_stmt_bind(db, sql, bind_values);
	if(stmt.next_row())
	{
		result = stmt.col(0);
	}
	stmt.finalize();
	return result;
}

function exec_dml(db, sql, bind_values=null)
{
	local stmt = create_stmt_bind(db, sql, bind_values);
	local result = stmt.step() == stmt.SQLITE_DONE;
	stmt.finalize();
	return result;
}

local function sanitizeDBName(dbname)
{
	return dbname.gsub("([^_%-a-zA-Z0-9])","");
}

local function escapeRE(str)
{
	return str.gsub("(%-)", "%%%1");
}

function getReferencesOnDBSchema(db, name)
{
	local reference_name = escapeRE(sanitizeDBName(name));
	local prefix_suffix_re = "[%s%(%),%.<>!=%-%+%*/\"']";
	local reference_name_re = prefix_suffix_re + "(" + reference_name + ")" + prefix_suffix_re;
	local reference_re = "()" + reference_name + "()";
	//print(reference_re);

	local checkValidRefenceName = function(whole_str, start_idx, end_idx)
	{
		//!!!this assume that the name searched is not at the begning or end of whole_str
		local context = whole_str.slice( (start_idx ? start_idx-1 : start_idx) , 
								(end_idx < whole_str.len() ? end_idx+1 : end_idx) );
		if(context.match(reference_name_re))
		{
			if( (context[0] == '\'') && (context[context.len()-1] != '\'') ) return false;
			else if( (context[0] == '"') && (context[context.len()-1] != '"') ) return false;
			return true;
		}
		return false;
	}

	local result = blob(0, 8192);
	local stmt = db.prepare("SELECT type, name, sql FROM sqlite_master;");
	while(stmt.next_row())
	{
		local sql = stmt.col(2);
		if((::type(sql) == "string"))
		{
			sql = sql.tolower();
			sql.gmatch(reference_re, function(start_idx, end_idx){
					//print(start_idx, end_idx, sql.slice(start_idx, end_idx));
					if(checkValidRefenceName(sql, start_idx, end_idx))
					{
						//print("idx", start_idx, end_idx, sql.slice(start_idx, end_idx));
						//print(sql);
						result.write("-------------\n");
						result.write(stmt.col(0), "\t", stmt.col(1), "\n");
						result.write(stmt.col(2), "\n");
						return false; //one match is enough
					}
					return true;
				});
		}
	}
	stmt.finalize();

	return result.tostring();
}

local function searchOnAllTables(db, search_str, search_limit)
{
	local embedded_limit = search_str.match("^(%d+):");
	if(embedded_limit)
	{
		search_limit = embedded_limit.tointeger();
		search_str = search_str.match("^%d+:(.+)");
	}
	local the_search_str;
	if( search_str.match("^_re_:") )
	{
		the_search_str = search_str.match("^_re_:(.+)");
	}
	else the_search_str = escapeRE(search_str);
	the_search_str = the_search_str.tolower();
	local search_count = 0;
	local result = blob(0, 8192);
	local stmt = db.prepare("SELECT name FROM sqlite_master WHERE type='table';");
	while(stmt.next_row())
	{
		local tbl_name = stmt.col(0);
		local tbl_stmt = db.prepare("SELECT * FROM \"" + tbl_name + "\"");
		local col_count = tbl_stmt.col_count();
		local text_cols = [];
		for(local i=0; i < col_count; ++i)
		{
			local dtype = tbl_stmt.col_declared_type(i).tolower();
			if( (dtype.indexOf("varchar") >= 0) || (dtype.indexOf("text") >= 0) ) text_cols.push(i);
		}
		if(text_cols.len())
		{
			local text_cols_len = text_cols.len();
			local tbl_done = false;
			while(tbl_stmt.next_row() && !tbl_done)
			{
				for(local i=0; i < text_cols_len; ++i)
				{
					local col_idx = text_cols[i];
					local str = tbl_stmt.col(col_idx);
					if((::type(str) == "string"))
					{
						if(str.tolower().match(the_search_str))
						{
							result.write(tbl_name, ":", tbl_stmt.col_name(col_idx), "\n");
							++search_count;
							tbl_done = true;
							break;
						}
					}
				}
			}
		}
		tbl_stmt.finalize();
		
		if(search_count >= search_limit) break;
	}
	stmt.finalize();

	return result.tostring();
}

local function multiply3(ctx,a,b,c){
	//print(ctx.user_data());
	ctx.result_double(a*b*c);
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
  
	constructor(){
		base.constructor();
		menuBar.callback(menuBar_cb);
		btnExecute.callback(btnExecute_cb);
		btnCreateQuery.callback(btnCreateQuery_cb);
		tabView.callback(tabView_cb);
		grid_tables._call_this = this.weakref();
		grid_data._call_this = this.weakref();
		local last_db_file_name = Fl.preferences_get("last_db_file_name", "");
		openDB(last_db_file_name);
		iTablesFilter.callback(doDataSearch_cb);
	}
	
	function doDataSearch_cb(sender : Fl_Widget, udata : any)
	{
		this = sender->window();
		local filtered_data = doFilterMyData(iTablesFilter->value(), grid_tables->_data);
		grid_tables->set_data(filtered_data);
	}
	
	function doFilterMyData(value, prev_filtered_data)
	{
		local the_data = prev_filtered_data;
		local value_len = value.len();
		//print(__LINE__, _my_tables_data.len(), the_data.len(), value.len(), value);
		if(value_len == 0) return _my_tables_data;
		
		if(value_len == 1 || the_data.len() == 0)
		{
			//we start from scratch
			the_data = _my_tables_data;
		}
		local filtered_data = [];
		foreach(rec in the_data)
		{
			foreach(field in rec)
			{
				if(field)
				{
					local fs = field.tostring();
					if(fs.len() && fs.tostring().indexOf(value) >= 0)
					{
						filtered_data.push(rec);
						break;
					}
				}
			}
		}
		return filtered_data;
	}
	
	function getSchemaVersion()
	{
		return db.exec_get_one("PRAGMA schema_version");
	}
	
	function checkSchemaVersion()
	{
		if(_the_schema_version != getSchemaVersion())
		{
			local tables_idx = grid_tables->row();
			get_tables();
			local v = iTablesFilter->value();
			if(v.len())
			{
				doDataSearch_cb(iTablesFilter, null);
			}
			grid_tables->row(tables_idx < grid_tables->rows() ? tables_idx : grid_tables->rows()-1);
		}
	}

	function getSchemaSqlFor(tbl)
	{
		local str = format("SELECT sql FROM sqlite_master WHERE (type='table' OR type='view') AND tbl_name='%s'", tbl);
		return str;
	}
	function getSchemaFor(tbl)
	{
		local str = db.exec_get_one(getSchemaSqlFor(tbl));
		return str;
	}
	
	function getIndexesSqlFor(tbl)
	{
		local str = format("SELECT name, sql FROM sqlite_master WHERE type='index' AND tbl_name='%s' ORDER BY name;", tbl);
		return str;
	}
	
	function getTriggersSqlFor(tbl)
	{
		local str = format("SELECT name, sql FROM sqlite_master WHERE type='trigger' AND tbl_name='%s' ORDER BY name;", tbl);
		return str;
	}
	
	function getFieldsFor(tbl)
	{
		local stmt = db.prepare("select * from \"" + tbl + "\"");
		local fields = stmt.colsAsArray();
		stmt.finalize();
		return fields;
	}

	function getFieldsAsCSVFor(tbl)
	{
		local fields = getFieldsFor(tbl);
		local str_fields =  "\"" + fields.concat("\", \"") + "\"";
		return str_fields;
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
			refreshTabView(tbl); 
		}
	}
	
	function row_selected(sender, ev){
		if(sender == grid_tables){
			if(ev == Fl_Data_Table_Events.e_update){
				local tbl = grid_tables.get_value(grid_tables.row(), 1);
				local sql = "select * from \"" + tbl + "\"";
				local limit = iMaxRows.value();
				if(limit && limit.len()) sql += " limit " + limit;
				local start_time = os.clock();
				get_records_by_sql(grid_data, sql, true);
				showExecutionTime(start_time);
				tabView->value(groupData);
			}
		}
		else if(sender == grid_data){
			if(ev == Fl_Data_Table_Events.e_update){
				local row = grid_data.get_row();
				local record = blob(0, 8192);
				foreach(idx, val in row)
					record.write((idx ? "\n" : ""), "----", idx, ":", 
						grid_data.get_col_name(idx),"\n", val);
				edit_record.value(record.tostring());
				tabView->value(groupRecord);
			}
		}
	}
  
	function btnExecute_cb(sender, udata){
		this = sender->window();
		local sql = edit_queries->value();
		if(sql && sql.len()) {
			local start_time;
			local action = option_query.text();
			local foreign_keys_saved = null;
			switch(action)
			{
				case "references":
					local references = getReferencesOnDBSchema(db, sql);
					edit_references->value(references);
					tabView->value(groupReferences);
				break;

				case "search all tables":
					local references = searchOnAllTables(db, sql, iMaxRows->value().tointeger());
					edit_references->value(references);
					tabView->value(groupReferences);
				break;

				case "schema update":
				case "schema update norefs":
					foreign_keys_saved = db.exec_get_one("PRAGMA foreign_keys");
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
							case "create index":
							case "create trigger":
							case "schema update":
							case "schema update norefs":
							case "sqlite_master update":
							case "drop table":
								checkSchemaVersion();
							break;
						}						
					}
					catch(e)
					{
						if(!db.IsAutoCommitOn() || (action == "schema update")) db.exec_dml("ROLLBACK;");
						if(foreign_keys_saved != null) db.exec_dml("PRAGMA foreign_keys=" + foreign_keys_saved);
						fl_alert(e);
					}
				break;
				
				case "dump table":
				break;

				default:
					try
					{
						start_time = os.clock();
						get_records_by_sql(grid_data, sql, true);
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
	}
	
	function getIndexesAndTriggersFor(tbl, result)
	{
		local stmt = db.prepare(getIndexesSqlFor(tbl));
		while(stmt.next_row())
		{
			local str = stmt.col(1);
			if(::type(str) == "string") result.write("\n\n", str, ";");
		}
		stmt.finalize();

		stmt = db.prepare(getTriggersSqlFor(tbl));
		while(stmt.next_row())
		{
			local str = stmt.col(1);
			if(::type(str) == "string") result.write("\n\n", str, ";");
		}
		stmt.finalize();
	}
  
	function btnCreateQuery_cb(sender, udata){
		this = sender->window();
		if(db){
			local tbl = grid_tables.get_value(grid_tables.row(), 1);
			local sql = option_query.text();
			local fields = getFieldsFor(tbl);
			local fields_csv = "\"" + fields.concat("\", \"") + "\"";
			
			local genSchemaUpgrade = function(with_references)
			{
				local new_suffix = "___new";
				local old_suffix = "";
				local result = blob(0, 8192);
				result.write("PRAGMA foreign_keys=OFF;\n\nBEGIN;\n\n");
				local str_schema = getSchemaFor(tbl);
				str_schema = str_schema.gsub("(" + escapeRE(tbl) + ")", "%1" + new_suffix, 1);
				str_schema = str_schema.gsub("\n%s+", "\n\t");
				result.write(str_schema, ";");
				fields_csv = fields_csv.gsub(", ", ",\n\t");
				result.write(format("\n\nINSERT INTO %s%s(\n\t%s\n\t)\nSELECT\n\t%s\nFROM \"%s%s\";", tbl, new_suffix, fields_csv, fields_csv, tbl, old_suffix));
				result.write("\n\nDROP TABLE \"", tbl, old_suffix, "\";");
				result.write("\n\nALTER TABLE \"", tbl, new_suffix, "\" RENAME TO \"", tbl, old_suffix, "\";");
				
				getIndexesAndTriggersFor(tbl, result);
				
				if(with_references)
				{
					result.write("\n\nDROP VIEW \"view_name\";\n\n");
					result.write(getReferencesOnDBSchema(db, tbl));
					result.write("\n\nCREATE VIEW  \"view_name\"  AS \"db_table_name\";");
				}
				result.write("\n\nPRAGMA foreign_key_check;\n\nCOMMIT;\n\nPRAGMA foreign_keys=ON;");
				return result.tostring();
			};
			
			if(sql == "select")
			{
				local alias_letter = 'a';
				local myjoins = "";
				local fields_last_idx = fields.len()-1;

				local stmt = db.prepare(format("PRAGMA foreign_key_list(\"%s\")", tbl));
				local last_fk_id = -1;
				while(stmt.next_row())
				{
					local fk_id = stmt.col(0);
					local ftable = stmt.col(2);
					local ffrom = stmt.col(3);
					local fto = stmt.col(4);
					local field_idx = fields.find(ffrom);
					local is_new_join = last_fk_id != fk_id;
					if(is_new_join) ++alias_letter;
					if(field_idx)
					{
						fields[field_idx] += format("\"%s --%c.\"%s", (fields_last_idx == field_idx ? "" : ","), alias_letter, fto);
					}
					if(is_new_join)
					{
						myjoins += format("\n--LEFT JOIN \"%s\" AS %c ON a.\"%s\" = %c.\"%s\"", ftable, alias_letter, ffrom, alias_letter, fto);
					}
					else
					{
						myjoins += format(" AND a.\"%s\" = %c.\"%s\"", ffrom, alias_letter, fto);
					}
					last_fk_id = fk_id;
				}
				stmt.finalize();

				fields_csv = "a.\"" + fields.concat("\",\n\ta.\"") + "\"";
				sql = format("--CREATE VIEW \"%s_list_view\" AS\nSELECT\n\t%s\nFROM \"%s\" AS a\nLIMIT %d", 
					tbl, fields_csv, tbl, iMaxRows->value().tointeger());
				sql += myjoins;
			}
			else if(sql == "insert") sql = format("INSERT INTO \"%s\"(%s)\nVALUES(%s)", tbl, fields_csv, fields_csv);
			else if(sql == "update") {
				fields_csv = "\"" + fields.concat("\"=?, \"") + "\"";
				sql = format("UPDATE \"%s\" SET %s=?\nWHERE \"id\"=?", tbl, fields_csv);
			}
			else if(sql == "delete") sql = format("DELETE FROM \"%s\" WHERE \"id\"=?", tbl);
			else if(sql == "create index") sql = format("CREATE INDEX \"%s_idx\" ON \"%s\"(\"field\" COLLATE NOCASE)", tbl, tbl);
			else if(sql == "create trigger") sql = format("CREATE TRIGGER \"%s_trigger\"\nBEFORE/AFTER/INSTEAD OF INSERT, UPDATE, DELETE OF col_name ON \"%s\"\nFOR EACH ROW WHEN expr\nBEGIN\nEND;", tbl, tbl);
			else if(sql == "drop table")
			{
				local table_type = db.exec_get_one("SELECT \"type\" FROM sqlite_master WHERE name='" + tbl + "'");
				sql = format("DROP %s \"%s\"", table_type, tbl);
			}
			else if(sql == "dump table")
			{
				local result = blob(0, 8192);
				local str_schema = getSchemaFor(tbl);
				str_schema = str_schema.gsub("\n%s+", "\n\t");
				result.write("BEGIN;\n\n", str_schema, ";");
				getIndexesAndTriggersFor(tbl, result);
				fields_csv = fields_csv.gsub(", ", ",\n\t");
				result.write(format("\n\nINSERT INTO \"%s\" (\n\t%s\n\t) VALUES", tbl, fields_csv));

				local result_size = result.len();
				local stmt = db.prepare(format("SELECT * FROM \"%s\"", tbl));
				local col_count = stmt.col_count();
				
				local SQLITE_INTEGER = stmt.SQLITE_INTEGER;
				local SQLITE_FLOAT = stmt.SQLITE_FLOAT;
				local SQLITE_NULL = stmt.SQLITE_NULL;
				local SQLITE_TEXT = stmt.SQLITE_TEXT;
				local SQLITE_BLOB = stmt.SQLITE_BLOB;
				
				while(stmt.next_row())
				{
					result.write("\n(");
					for(local i=0; i < col_count; ++i)
					{
						local value = stmt.col(i);
						if(i) result.write(",");
						
						local ctype = stmt.col_type(i);

						if(ctype == SQLITE_INTEGER  || ctype == SQLITE_FLOAT)
							result.write(value.tostring());
							
						else if(ctype == SQLITE_NULL) result.write("NULL");
						else if(ctype == SQLITE_TEXT) result.write("'", value.gsub("'", "''") ,"'");
						else if(ctype == SQLITE_BLOB) result.write(format("%q", value));
						else result.write("??");
					}
					result.write("),");
				}
				stmt.finalize();
				
				if(result_size < result.len())
				{
					result.resize(result.len()-1); //delete last comma
					result.write(";");
				}
				
				result.write("\n\nCOMMIT;");
				sql = result.tostring();
			}
			else if(sql == "references") sql = tbl;
			else if(sql == "schema update") {
				sql = genSchemaUpgrade(true);
			}
			else if(sql == "schema update norefs") {
				sql = genSchemaUpgrade(false);
			}
			else if(sql == "sqlite_master update") {
				local result = blob(0, 8192);
				local schema_version = getSchemaVersion();
				result.write("BEGIN;\n--PRAGMA schema_version; --> ", schema_version.tostring(), "\n\n");
				result.write("PRAGMA writable_schema=ON;\n\n");
				local str_schema = getSchemaFor(tbl);
				str_schema = str_schema.gsub("\n%s+", "\n\t");
				str_schema = str_schema.gsub("'", "''");
				
				result.write("UPDATE sqlite_master\nSET sql='", str_schema, "'\nWHERE type='table' AND name='", tbl, "';\n\n"); 
				
				local stmt = db.prepare(getIndexesSqlFor(tbl));
				while(stmt.next_row())
				{
					local str = stmt.col(1);
					if(::type(str) == "string") result.write("\n\n", str, ";");
				}
				stmt.finalize();

				stmt = db.prepare(getTriggersSqlFor(tbl));
				while(stmt.next_row())
				{
					local str = stmt.col(1);
					if(::type(str) == "string") result.write("\n\n", str, ";");
				}
				stmt.finalize();
				
				result.write("\n\nDROP VIEW \"view_name\";\n\n");
				result.write(getReferencesOnDBSchema(db, tbl));
				result.write("\n\nCREATE VIEW  \"view_name\"  AS \"db_table_name\";");
				result.write("\n\nPRAGMA schema_version=", (schema_version.tointeger() + 1).tostring(), 
					";\n\nPRAGMA writable_schema=OFF;\n\nPRAGMA integrit_check;\n\nCOMMIT;");
				sql = result.tostring();
			}
			else return;
			edit_queries->value(sql);
		}
	}
	
	function refreshTabView(tbl)
	{
		local tab = tabView->value();
		local sql = false;
		local the_grid = false;
		if(tab == viewFields)
		{
			sql = format("PRAGMA table_info(\"%s\");", tbl);
			the_grid = grid_fields;
		}
		if(tab == viewIndexes)
		{
			sql = getIndexesSqlFor(tbl);
			the_grid = gridIndexes;
		}
		else if(tab == viewTriggers)
		{
			sql = getTriggersSqlFor(tbl);
			the_grid = gridTriggers;
		}
		else if(tab == groupSchema)
		{
			local str = getSchemaFor(tbl);
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
			refreshTabView(tbl);
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

	function get_records_by_sql(grid, sql , named=false){
		local cursor_wait = fl_cursor_wait();
		local stmt = db.prepare(sql);
		local rec_list = stmt.asArrayOfArrays(SQLite3Stmt.WITH_COL_NAMES | 
			SQLite3Stmt.AS_STRING_ALWAYS | SQLite3Stmt.NULL_AS_EMPTY_STR);
		grid.set_new_data(rec_list);
	}

	function get_tables(){
		local sql = "SELECT rowid AS 'rowid|ID|0', name AS 'name|Name|-1', type as 'type|Type|4' FROM sqlite_master WHERE (type='table' OR type='view') ORDER BY name";
		get_records_by_sql(grid_tables, sql, true);
		_the_schema_version = getSchemaVersion();
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
			//db.exec_dml("PRAGMA mmap_size=268435456;");
			//db.trace(function(udata, sql){print(udata, ":", sql);}, "SQL");
			//db.create_function("multiply3",3, multiply3); //SQLITE_DETERMINISTIC
			get_tables();
			Fl.preferences_set("last_db_file_name", the_db_file_name);
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
				if (dbname && !attached_databases.get(dbname, false)){
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
