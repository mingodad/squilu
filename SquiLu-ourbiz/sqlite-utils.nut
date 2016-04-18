class SQLiteUtils {
	
	static __db_cache = {};

	static function getDbFor(dbname)
	{
		local db = table_rawget(__db_cache, dbname, false);
		if(!db)
		{
			db =  SQLite3(dbname);
			//db.exec_dml("PRAGMA synchronous = 0;");
			//db.exec_dml("PRAGMA journal_mode = WAL");
			table_rawset(__db_cache, dbname, db);
		}
		return db;
	}

	static __query_cache = {};

	static function getCachedQuery(qfn, db, sql)
	{
		return qfn(db, sql);
		/*
		There is a problem here we need to cache by db also
		local result = table_rawget(__query_cache, sql, false);
		if(!result)
		{
			result = qfn(db, sql);
			__query_cache[sql] <- result;
		}
		return result;
		*/
	}

	static function escapeRE(str)
	{
		return str.gsub("(%-)", "%%%1");
	}

	static function escape_sql_like_search_str(str){
		if (str && (str.len() > 0)){
			str = str.gsub("%%", "%%%%");
			if (str.find(" ") == 0)
				str = str.gsub("^%s*(.+)%s*$","%1%%");
			else
				str = str.gsub("^%s*(.+)%s*$","%%%1%%");
		}
		//print("escape_sql_like_search_str +" + str + "+")
		return str;
	}
	
	static function sanitizeDBName(dbname)
	{
		return dbname.gsub("([^_%-a-zA-Z0-9])","");
	}
	
	static function create_stmt_bind(db, sql, bind_values=null)
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

	static function exec_get_all(db, sql, bind_values=null)
	{
		local stmt = create_stmt_bind(db, sql, bind_values);
		local result = stmt.asArrayOfTables();
		stmt.finalize();
		return result;
	}

	static function exec_get_one(db, sql, bind_values=null)
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

	static function exec_dml(db, sql, bind_values=null)
	{
		local stmt = create_stmt_bind(db, sql, bind_values);
		//debug_print("exec_dml: ", sql, "\n");
		local result = stmt.step() == stmt.SQLITE_DONE;
		stmt.finalize();
		return result;
	}
	
	static function getReadOnlyFields(db, attached_db="")
	{
		local read_only_fields = {};
		local ro = exec_get_all(db, format("SELECT * FROM %s__fields_metadata_sys_names_view", attached_db));
		if(ro && ro.len())
		{
			foreach(k,v in ro[0])
			{
				table_rawset(read_only_fields, v, true);
			}
		}
		return read_only_fields;
	}

	static function getAcceptFieldsFields(db, tbl_name, attached_db="")
	{
		local af = {};
		local fld_name_key = "field_id_name";
		local all_af = exec_get_all(db, format("SELECT \"%s\" FROM %s__table_metadata_accept_fields_view WHERE table_id_name=?", fld_name_key, attached_db), [tbl_name]);
		if(all_af && all_af.len())
		{
			foreach(k,v in all_af)
			{
				table_rawset(af, table_rawget(v, fld_name_key), true);
			}
		}
		return af;
	}
	
	static function getTableListFields(db, tbl_name, attached_db="")
	{
		local fld_name_key = "field_id_name";
		local fields = exec_get_one(db, format("SELECT group_concat(\"%s\", ',') AS fields FROM %s__table_metadata_fields_view0 WHERE table_id_name=? AND hide_on_list <> 1", fld_name_key, attached_db), [tbl_name]);
		return fields;
	}
	
	static function getEditLinkFields(db, table_name, attached_db="")
	{
		local rows = exec_get_all(db, format("SELECT * FROM %s__table_metadata_edit_links_view WHERE table_id_name=?", attached_db), [table_name]);
		local result = {};
		foreach(k,row in rows)
		{
			result[row.field_id_name] <- row;
		}
		return result;
	}

	static function getTableListInstead(db, the_table_name, attached_db="")
	{
		local list_instead = exec_get_one(db, 
			format("SELECT list_table_id_name FROM %s__tables_metadata_view WHERE name=?", attached_db), 
			[the_table_name]);
		if(::type(list_instead) == "string")
		{
			return list_instead;
		}
		return the_table_name;
	}
	
	static function getTableEditInstead(db, the_table_name, attached_db="")
	{
		local edit_instead = exec_get_one(db, 
			format("SELECT edit_table_id_name FROM %s__tables_metadata_view WHERE name=?", attached_db), 
			[the_table_name]);
		if(::type(edit_instead) == "string")
		{
			return edit_instead;
		}
		return the_table_name;
	}
	
	static function getListLimitForTableName(db, the_table_name, attached_db="")
	{
		the_table_name = getTableEditInstead(db, the_table_name, attached_db);
		local limit = exec_get_one(db, format("SELECT default_list_limit FROM %s__tables_metadata WHERE name =?", attached_db), [the_table_name]);
		return limit ? limit : 0;
	}
	
	static function getIsReadOnlyForTableName(db, the_table_name, attached_db="")
	{
		local is_read_only = exec_get_one(db, format("SELECT is_read_only FROM %s__tables_metadata WHERE name=?", attached_db), [the_table_name]);
		return is_read_only != null ? is_read_only : 1; //if not found it's read only
	}

	static function getFilterTables(db, table_name, attached_db="")
	{
		local rows = exec_get_all(db, format("SELECT * FROM %s__table_metadata_filter_tables_list_view WHERE table_id_name=?", attached_db), [table_name]);
		return rows;
	}

	static function getByIdFieldAsBlob(db, table_name, id, field_name, attached_db="")
	{
		local result = exec_get_one(db, format("SELECT \"%s\" FROM %s\"%s\" WHERE \"id\"=?", 
					sanitizeDBName(field_name), attached_db, table_name), [id]);
		return result;
	}

	static function getSchemaSqlFor(tbl, attached_db="")
	{
		local str = format("SELECT sql FROM %ssqlite_master WHERE (type='table' OR type='view') AND tbl_name='%s'", attached_db, tbl);
		return str;
	}
	
	static function getSchemaFor(db, tbl, attached_db="")
	{
		local str = db.exec_get_one(getSchemaSqlFor(tbl, attached_db));
		return str;
	}
	
	static function getIndexesSqlFor(tbl, attached_db="")
	{
		local str = format("SELECT name, sql FROM %ssqlite_master WHERE type='index' AND tbl_name='%s' ORDER BY name;", attached_db, tbl);
		return str;
	}
	
	static function getTriggersSqlFor(tbl, attached_db="")
	{
		local str = format("SELECT name, sql FROM %ssqlite_master WHERE type='trigger' AND tbl_name='%s' ORDER BY name;", attached_db, tbl);
		return str;
	}
	
	static function getTableInfoSqlFor(tbl, attached_db="")
	{
		local str = format("PRAGMA %stable_info(\"%s\");", attached_db, tbl);
		return str;
	}
	
	static function getTableFieldsInfoFor(db, tbl, attached_db="")
	{
		local stmt = db.prepare(getTableInfoSqlFor(tbl, attached_db));
		local fields = stmt.colsAsArray();
		stmt.finalize();
		return fields;
	}

	static function getFieldsFor(db, tbl, attached_db="")
	{
		local stmt = db.prepare(format("select * from %s\"%s\"", attached_db, tbl));
		local fields = stmt.colsAsArray();
		stmt.finalize();
		return fields;
	}
	
	static function getFieldsAsCSVFor(db, tbl, attached_db="")
	{
		local fields = getFieldsFor(db, tbl, attached_db);
		local str_fields =  "\"" + fields.concat("\", \"") + "\"";
		return str_fields;
	}

	static function getIndexesAndTriggersFor(db, tbl, result, attached_db="")
	{
		local stmt = db.prepare(getIndexesSqlFor(tbl, attached_db));
		while(stmt.next_row())
		{
			local str = stmt.col(1);
			if(::type(str) == "string") result.write("\n\n", str, ";");
		}
		stmt.finalize();

		stmt = db.prepare(getTriggersSqlFor(tbl, attached_db));
		while(stmt.next_row())
		{
			local str = stmt.col(1);
			if(::type(str) == "string") result.write("\n\n", str, ";");
		}
		stmt.finalize();
	}

	static function createQuery(db, tbl, query_type, max_rows, attached_db=""){
		if(db){
			local sql;
			local fields = getFieldsFor(db, tbl, attached_db);
			local fields_csv = "\"" + fields.concat("\", \"") + "\"";
			
			local genSchemaUpgrade = function(db, with_references)
			{
				local new_suffix = "___new";
				local old_suffix = "";
				local result = blob(0, 8000);
				result.write("PRAGMA foreign_keys=OFF;\n\nBEGIN;\n\n");
				local str_schema = getSchemaFor(db, tbl, attached_db);
				local tbl_name_suffixed = tbl + new_suffix;
				str_schema = str_schema.gsub("(" + escapeRE(tbl) + ")", tbl_name_suffixed, 1);
				str_schema = str_schema.gsub("(\"?" + escapeRE(tbl_name_suffixed) + "\"?)", attached_db + "%1", 1);
				str_schema = str_schema.gsub("\n%s+", "\n\t");
				result.write(str_schema, ";");
				fields_csv = fields_csv.gsub(", ", ",\n\t");
				result.write(format("\n\nINSERT INTO %s\"%s%s\"(\n\t%s\n\t)\nSELECT\n\t%s\nFROM %s\"%s%s\";", 
					attached_db, tbl, new_suffix, fields_csv, fields_csv, attached_db, tbl, old_suffix));
				result.write("\n\nDROP TABLE ", attached_db,"\"", tbl, old_suffix, "\";");
				result.write("\n\nALTER TABLE ", attached_db, "\"", tbl, new_suffix, "\" RENAME TO \"", tbl, old_suffix, "\";");
				
				getIndexesAndTriggersFor(db, tbl, result, attached_db);
				
				if(with_references)
				{
					result.write("\n\nDROP VIEW \"view_name\";\n\n");
					result.write(getReferencesOnDBSchema(db, tbl, attached_db));
					result.write("\n\nCREATE VIEW  \"view_name\"  AS \"db_table_name\";");
				}
				result.write("\n\nPRAGMA foreign_key_check;\n\nCOMMIT;\n\nPRAGMA foreign_keys=ON;");
				return result.tostring();
			};
			
			if(query_type == "select")
			{
				local alias_letter = 'a';
				local myjoins = "";
				local fields_last_idx = fields.len()-1;

				local stmt = db.prepare(format("PRAGMA %sforeign_key_list(\"%s\")", attached_db, tbl));
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
						myjoins += format("\n--LEFT JOIN %s\"%s\" AS %c ON a.\"%s\" = %c.\"%s\"", attached_db, ftable, alias_letter, ffrom, alias_letter, fto);
					}
					else
					{
						myjoins += format(" AND a.\"%s\" = %c.\"%s\"", ffrom, alias_letter, fto);
					}
					last_fk_id = fk_id;
				}
				stmt.finalize();

				fields_csv = "a.\"" + fields.concat("\",\n\ta.\"") + "\"";
				sql = format("--CREATE VIEW %s\"%s_list_view\" AS\nSELECT\n\t%s\nFROM %s\"%s\" AS a\nLIMIT %d", 
					attached_db, tbl, fields_csv, attached_db, tbl, max_rows.tointeger());
				sql += myjoins;
			}
			else if(query_type == "insert") sql = format("INSERT INTO %s\"%s\"(%s)\nVALUES(%s)", attached_db, tbl, fields_csv, fields_csv);
			else if(query_type == "update") {
				fields_csv = "\"" + fields.concat("\"=?, \"") + "\"";
				sql = format("UPDATE %s\"%s\" SET %s=?\nWHERE \"id\"=?", attached_db, tbl, fields_csv);
			}
			else if(query_type == "delete") sql = format("DELETE FROM %s\"%s\" WHERE \"id\"=?", attached_db, tbl);
			else if(query_type == "create index") sql = format("CREATE INDEX %s\"%s_idx\" ON %s\"%s\"(\"field\" COLLATE NOCASE)", attached_db, tbl, attached_db, tbl);
			else if(query_type == "create trigger") sql = format("CREATE TRIGGER %s\"%s_trigger\"\nBEFORE/AFTER/INSTEAD OF INSERT, UPDATE, DELETE OF col_name ON %s\"%s\"\nFOR EACH ROW WHEN expr\nBEGIN\nEND;", 
							attached_db, tbl, attached_db, tbl);
			else if(query_type == "drop table")
			{
				local table_type = db.exec_get_one(format("SELECT \"type\" FROM %ssqlite_master WHERE name='%s'", attached_db, tbl));
				sql = format("DROP %s %s\"%s\"", table_type, attached_db, tbl);
			}
			else if(query_type == "dump table")
			{
				local result = blob(0, 8000);
				local str_schema = getSchemaFor(db, tbl, attached_db);
				str_schema = str_schema.gsub("\n%s+", "\n\t");
				result.write("BEGIN;\n\n", str_schema, ";");
				getIndexesAndTriggersFor(db, tbl, result, attached_db);
				fields_csv = fields_csv.gsub(", ", ",\n\t");
				result.write(format("\n\nINSERT INTO %s\"%s\" (\n\t%s\n\t) VALUES", attached_db, tbl, fields_csv));

				local result_size = result.len();
				local stmt = db.prepare(format("SELECT * FROM %s\"%s\"", attached_db, tbl));
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
			else if(query_type == "references") sql = tbl;
			else if(query_type == "schema update") {
				sql = genSchemaUpgrade(db, true);
			}
			else if(query_type == "schema update norefs") {
				sql = genSchemaUpgrade(db, false);
			}
			else if(query_type == "sqlite_master update") {
				local result = blob(0, 8000);
				local schema_version = getSchemaVersion(db, attached_db);
				result.write("BEGIN;\n--PRAGMA ", attached_db, "schema_version; --> ", schema_version.tostring(), "\n\n");
				result.write("PRAGMA ", attached_db, "writable_schema=ON;\n\n");
				
				local stmt = db.prepare("select rowid, sql from sqlite_master where tbl_name = '" + tbl + "';");
				while(stmt.next_row())
				{
					local str_schema = stmt.col(1);
					if(::type(str_schema) == "string")
					{
						str_schema = str_schema.gsub("\n%s+", "\n\t");
						str_schema = str_schema.gsub("'", "''");
						result.write("UPDATE ", attached_db, "sqlite_master\nSET sql='", str_schema, "'\nWHERE rowid=", stmt.col(0), ";\n\n"); 
					}
				}				
				stmt.finalize();
				
				result.write("\n\nDROP VIEW \"view_name\";\n\n");
				result.write(getReferencesOnDBSchema(db, tbl, attached_db));
				result.write("\n\nCREATE VIEW  \"view_name\"  AS \"db_table_name\";");
				result.write("\n\nPRAGMA ", attached_db, "schema_version=", (schema_version.tointeger() + 1).tostring(), 
					";\n\nPRAGMA ", attached_db, "writable_schema=OFF;\n\nPRAGMA ", attached_db, "integrit_check;\n\nCOMMIT;");
				sql = result.tostring();
			}
			return sql;
		}
	}
	
	static function executeQuery(db, query_type, sql, attached_db=""){
		local result = {query_type=query_type, result=""};
		if(sql && sql.len()) {
			local start_time;
			local foreign_keys_saved = null;
			switch(query_type)
			{
				case "references":
					local references = getReferencesOnDBSchema(db, sql, attached_db);
					table_rawset(result, "value", references);
					table_rawset(result, "result", "OK");
				break;

				case "search all tables":
					local references = searchOnAllTables(db, sql, iMaxRows->value().tointeger(), attached_db);
					table_rawset(result, "value", references);
					table_rawset(result, "result", "OK");
				break;

				case "schema update":
				case "schema update norefs":
					foreign_keys_saved = db.exec_get_one(format("PRAGMA %sforeign_keys", attached_db));
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

						db.exec_dml(sql);

						table_rawset(result, "time_spent", os.clock() - start_time);
						table_rawset(result, "result", "OK");

						switch(query_type)
						{
							case "create index":
							case "create trigger":
							case "schema update":
							case "schema update norefs":
							case "sqlite_master update":
							case "drop table":
								table_rawset(result, "schema_version", getSchemaVersion(db, attached_db));
							break;
						}						
					}
					catch(e)
					{
						if(!db.IsAutoCommitOn() || query_type.startswith("schema update")) db.exec_dml("ROLLBACK;");
						if(foreign_keys_saved != null) db.exec_dml(format("PRAGMA %sforeign_keys=" + foreign_keys_saved, attached_db));
						table_rawset(result, "result", e);
					}
				break;
				
				case "dump table":
				break;

				default:
			}
		}
		return result;
	}
	
	static function getSchemaVersion(db, attached_db="")
	{
		return db.exec_get_one(format("PRAGMA %sschema_version", attached_db));
	}
	
	static function get_tables(db, attached_db=""){
		local sql = format("select rowid as 'rowid|ID|0', name as 'name|Name|-1', type as 'type|Type|4' from %ssqlite_master where (type='table' OR type='view') order by name", attached_db);
		get_records_by_sql(grid_tables, sql, true);
		//_the_schema_version = getSchemaVersion();
	}
	
	static function get_records_by_sql(db, sql , named=false){
		local stmt = db.prepare(sql);
		local rec_list = stmt.asArrayOfArrays(SQLite3Stmt.WITH_COL_NAMES | 
			SQLite3Stmt.AS_STRING_ALWAYS | SQLite3Stmt.NULL_AS_EMPTY_STR);
		stmt.finalize();
	}

	static function doUpdateTablesMetadataFor(db, all_tables, attached_db="")
	{
		local stmt_insert_table = db.prepare(format("INSERT INTO %s__tables_metadata(name, is_view) VALUES(?,?)", attached_db));
		local stmt_insert_fields = db.prepare(format("INSERT INTO %s__fields_metadata(name, type_id, length) VALUES(?,?,?)", attached_db));
		foreach(idx, tbl in all_tables)
		{
			local is_view = tbl.type == "view" ? 1 : 0;
			stmt_insert_table.bind(1, tbl.name);
			stmt_insert_table.bind(2, is_view);
			stmt_insert_table.step();
			stmt_insert_table.reset();
			
			//if(!is_view)
			{
				local all_fields = exec_get_all(db, format("PRAGMA %stable_info(\"%s\");", attached_db, tbl.name));
				foreach(fld in all_fields)
				{
					stmt_insert_fields.bind(1, fld.name);
					local field_type = fld.type.gsub("%(.+$", "");
					if(!field_type.len()) field_type = "VARCHAR";
					local type_id = exec_get_one(db, format("SELECT id FROM %s__field_types WHERE name=?", attached_db), [field_type]);
					stmt_insert_fields.bind(2, type_id);
					local field_length = fld.type.match("%((%d+)%)");
					stmt_insert_fields.bind(3, field_length);
					stmt_insert_fields.step();
					stmt_insert_fields.reset();
				}
			}
		}
		stmt_insert_table.finalize();
		stmt_insert_fields.finalize();
	}
	
	static function doUpdateTableMetadataFieldsFor(db, all_tables, attached_db="")
	{
		local stmt_insert_fields = db.prepare(format("INSERT INTO %s__table_metadata_fields(table_id, field_id) VALUES(?,?)", attached_db));
		local stmt_insert_accept_fields = db.prepare(format("INSERT INTO %s__table_metadata_accept_fields(table_id, field_id) VALUES(?,?)", attached_db));
		local stmt_insert_edit_links = db.prepare(format("INSERT INTO %s__table_metadata_edit_links(table_id, field_id, link_table_id, link_field_id, show_table_id, show_field_id, show_text_id) VALUES(?,?,?,?,?,?,?)",attached_db));
		foreach(tbl in all_tables)
		{
			local all_fields = exec_get_all(db, format("PRAGMA %stable_info(\"%s\");", attached_db, tbl.name));
			foreach(fld in all_fields)
			{
				stmt_insert_fields.bind(1, tbl.id);
				local field_id = exec_get_one(db, format("SELECT id FROM %s__fields_metadata WHERE name=?", attached_db), [fld.name]);
				stmt_insert_fields.bind(2, field_id); 
				stmt_insert_fields.step();
				stmt_insert_fields.reset();

				local ro = getReadOnlyFields(db, attached_db);
				if(!table_rawin(ro, fld.name))
				{
					stmt_insert_accept_fields.bind(1, tbl.id);
					stmt_insert_accept_fields.bind(2, field_id); 
					stmt_insert_accept_fields.step();
					stmt_insert_accept_fields.reset();
				}

				if(fld.name.endswith("_id"))
				{
					field_id = db.last_row_id();
					local table_name = fld.name.gsub("_id$", "s");
					local link_table_id = exec_get_one(db, format("SELECT id FROM %s__tables_metadata WHERE name=?", attached_db), [table_name]);
					local link_field_id = exec_get_one(db, format("SELECT id FROM %s__table_metadata_fields_view WHERE table_id_name=? AND field_id_name='id'", attached_db), [table_name]);
					//print(table_name, link_table_id, link_field_id);
					if(link_table_id && link_field_id)
					{
						stmt_insert_edit_links.bind(1, tbl.id);
						stmt_insert_edit_links.bind(2,field_id);
						stmt_insert_edit_links.bind(3, link_table_id);
						stmt_insert_edit_links.bind(4, link_field_id);
						stmt_insert_edit_links.bind(5, link_field_id);
						stmt_insert_edit_links.bind(6, link_field_id);
						stmt_insert_edit_links.bind(7, link_field_id);
						stmt_insert_edit_links.step();
						stmt_insert_edit_links.reset();
					}
				}
			}			
		}
		stmt_insert_fields.finalize();
		stmt_insert_accept_fields.finalize();
		stmt_insert_edit_links.finalize();
	}

	static function doCleanTableMetadataFieldsFor(db, all_tables, attached_db="")
	{
		//cleanup references for inexisting tables/views at global level
		local updateTablesMetadata = function()
		{
			local sql_exists = format("SELECT 1 FROM %ssqlite_master sm, %s__tables_metadata tm WHERE tm.id=a.%%s and sm.name=tm.name",
				attached_db, attached_db);
			local sql = format("UPDATE %s__tables_metadata a set list_table_id=NULL WHERE NOT EXISTS(%s)", attached_db, format(sql_exists, "list_table_id"));
			db.exec_dml(sql);
			sql = format("UPDATE %s__tables_metadata a set edit_table_id=NULL WHERE NOT EXISTS(%s)", attached_db, format(sql_exists, "edit_table_id"));
			db.exec_dml(sql);
			sql = format("UPDATE %s__app_menu a set table_view_id=NULL WHERE NOT EXISTS(%s)", attached_db, format(sql_exists, "table_view_id"));
			db.exec_dml(sql);
		}
		updateTablesMetadata();

		//now for each table clean references to inexistent fields/tables/views
		foreach(tbl in all_tables)
		{
			local all_fields = exec_get_all(db, format("PRAGMA %stable_info(\"%s\");", attached_db, tbl.name));
			local existing_fields = [];
			foreach(fld in all_fields)
			{
				existing_fields.push(fld.name);
			}
			//if(!existing_fields.len()) continue;
			existing_fields = "'" + existing_fields.concat("','") + "'";

			local deleteNotIn = function(tbl_to_delete, tbl_id_name, field_names_to_keep, table_id_fld="table_id", field_id_fld="field_id")
			{
				local sql = format("DELETE FROM %s%s WHERE table_id IN (SELECT tm.id FROM %s__tables_metadata tm WHERE tm.name = '%s' AND NOT EXISTS(SELECT 1 FROM %ssqlite_master sm WHERE sm.name=tm.name))",
					attached_db, tbl_to_delete, attached_db, tbl_id_name, attached_db);
				db.exec_dml(sql);
			
				sql = format("DELETE FROM %s\"" + tbl_to_delete + "\" WHERE " + table_id_fld + 
					"=(SELECT id FROM %s__tables_metadata WHERE name='" + 
					tbl_id_name + "') AND " + field_id_fld + " NOT IN(SELECT id FROM %s__fields_metadata WHERE name IN(" + field_names_to_keep + "))",
					attached_db, attached_db, attached_db);
				//debug_print(sql, "\n");
				db.exec_dml(sql);
			}
			//order is important here first any of __table_metadata_list_fields, __table_metadata_accept_fields, __table_metadata_edit_links,
			//then __table_metadata_filter_tables and __table_metadata_fields
			deleteNotIn("__table_metadata_list_fields", tbl.name, existing_fields);
			deleteNotIn("__table_metadata_accept_fields", tbl.name, existing_fields);
			
			//__table_metadata_edit_links references tables/views in multiple fields
			deleteNotIn("__table_metadata_edit_links", tbl.name, existing_fields, "show_table_id", "show_field_id");
			deleteNotIn("__table_metadata_edit_links", tbl.name, existing_fields, "link_table_id", "link_field_id");
			deleteNotIn("__table_metadata_edit_links", tbl.name, existing_fields);
			
			deleteNotIn("__table_metadata_filter_tables", tbl.name, existing_fields, "table_filtered_id", "table_filtered_field_id");
			deleteNotIn("__table_metadata_fields", tbl.name, existing_fields);			
		}

		//finally remove inexistent tables/views
		local sql = format("DELETE FROM %s__tables_metadata a WHERE NOT EXISTS(SELECT 1 FROM %ssqlite_master sm WHERE sm.name=a.name)",
			attached_db, attached_db);
		db.exec_dml(sql);
	}

	static function doUpdateMetadata(db, attached_db="")
	{
		db.exec_dml("BEGIN");

		local all_tables = exec_get_all(db, format("SELECT type, name FROM %ssqlite_master WHERE type IN('view','table')", attached_db));
		doUpdateTablesMetadataFor(db, all_tables);
		
		all_tables = exec_get_all(db, format("SELECT id, name FROM %s__tables_metadata", attached_db)); // WHERE is_view = 0");
		doUpdateTableMetadataFieldsFor(db, all_tables);
		
		doCleanTableMetadataFieldsFor(db, all_tables);
		
		db.exec_dml("COMMIT");
	}

	static function getReferencesOnDBSchema(db, name, attached_db="")
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

		local result = blob(0, 8000);
		local stmt = db.prepare(format("SELECT type, name, sql FROM %ssqlite_master;", attached_db));
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

	static function searchOnAllTables(db, search_str, search_limit, attached_db="")
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
		local result = blob(0, 8000);
		local stmt = db.prepare(format("SELECT name FROM %ssqlite_master WHERE type='table';", attached_db));
		while(stmt.next_row())
		{
			local tbl_name = stmt.col(0);
			local tbl_stmt = db.prepare(format("SELECT * FROM %s\"%s\"", attached_db, tbl_name));
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
	
}

class SqlPreprocessor
{
	stmt_get = null;
	stmt_get_by_name = null;
	stmt_set = null;
	
	function createSqlPreprocessorMainTable(db)
	{
		local sql = [==[
CREATE TABLE IF NOT EXISTS __app_sql (
	id INTEGER PRIMARY KEY NOT NULL,
	_version_ INTEGER NOT NULL DEFAULT 0,
	_cdate_ DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
	_mdate_ DATETIME,
	name VARCHAR NOT NULL COLLATE NOCASE
	CONSTRAINT __app_sql_name_unique UNIQUE,
	base TEXT COLLATE NOCASE NOT NULL,
	sql TEXT COLLATE NOCASE,
	notes TEXT COLLATE NOCASE
);		
]==];
		db.exec_dml(sql);
	}
	
	function doPrepare(db)
	{
		if(!stmt_get)
		{
			stmt_get = db.prepare("SELECT base FROM __app_sql WHERE id=?");
			stmt_get_by_name = db.prepare("SELECT base FROM __app_sql WHERE name=?");
			stmt_set = db.prepare("UPDATE __app_sql SET sql=? WHERE id=?");
		}
	}
	
	function doFinalize(db)
	{
		if(stmt_get)
		{
			stmt_get.finalize();
			stmt_get = null;
			stmt_set.finalize();
			stmt_get_by_name.finalize();
		}
	}
	
	//the macros for params are {$param_name:param_value}
	static function preprocessSqlQueryParams(sql_seed, params)
	{
		local found;
		do {
			found = false;
			sql_seed = sql_seed.gsub("(%b{})", function(m){
				if(m[1] == '$')
				{
					found = true;
					local ary = m.slice(2,-1).split(':');
					local val = table_get(params, ary[0], ary[1]);
					return val;
				}
				return m;
			});
			//print("loop", found);
		} while(found);
		
		return sql_seed;
	}	

	function preprocessSqlQuery(db, sql_seed, id)
	{
		local need_prepare = stmt_get == null;
		if(need_prepare) doPrepare(db);
				
		if(sql_seed)
		{
			local found;
			local self = this;
			do {
				found = false;
				sql_seed = sql_seed.gsub("(%b{})", function(m){
					if(m[1] == ':')
					{
						local vt = m.slice(2,-1);
						self.stmt_get_by_name.bind(1, vt);
						if(self.stmt_get_by_name.next_row())
						{
							//need check to prevent infinite recursion
							found = true;
							local val = self.stmt_get_by_name.col(0);
							self.stmt_get_by_name.reset();
							return val;
						}
					}
					return m;
				});
				//print("loop", found);
			} while(found);
			stmt_set.bind(1, sql_seed);
			stmt_set.bind(2, id);
			stmt_set.step();
			stmt_set.reset();
		}
		if(need_prepare) doFinalize(db);
		return sql_seed;
	}

	//the macros for params are {:query_name_to_include}
	function preprocessSqlQueries(db)
	{
		local stmt = db.prepare("SELECT id FROM __app_sql");
		local all_ids = [];
		while(stmt.next_row())
		{
			all_ids.push(stmt.col(0));
		}
		stmt.finalize();
		
		doPrepare(db);

		try
		{
			db.exec_dml("BEGIN");
			foreach(id in all_ids)
			{
				local found;
				local sql_seed = false;
				stmt_get.bind(1, id);
				if(stmt_get.next_row()) sql_seed = stmt_get.col(0);
				stmt_get.reset();
				
				if(sql_seed)
				{
					preprocessSqlQuery(db, sql_seed, id);
				}
			}
			db.exec_dml("COMMIT");
		}
		catch(e)
		{
			db.exec_dml("ROLLBACK");
		}
		doFinalize(db);
	}
	
	function getPreprocessorQueryName(db, sql)
	{
		createSqlPreprocessorMainTable(db);
		return sql.match("^%-%-name=(%S+)");
	}
	
	function getPreprocessorQuery(db, sql, field="ifnull(sql, base) as sql")
	{
		local name = getPreprocessorQueryName(db, sql);
		if(name)
		{
			local stmt = db.prepare(format("SELECT %s FROM __app_sql WHERE name=?", field));
			stmt.bind(1, name);
			if(stmt.next_row())
			{
				name = stmt.col(0);
			}
			stmt.finalize();
		}
		return name;
	}

	function insertPreprocessorQuery(db, sql)
	{
		local name = getPreprocessorQueryName(db, sql);
		if(name)
		{
			local stmt = db.prepare("insert into __app_sql(name, base) values(?,?)");
			stmt.bind(1, name);
			stmt.bind(2, sql);
			stmt.step();
			print(db.errmsg());
			stmt.finalize();
			preprocessSqlQueries(db);
		}
		return name;
	}

	function updatePreprocessorQuery(db, sql)
	{
		local name = getPreprocessorQueryName(db, sql);
		if(name)
		{
			local stmt = db.prepare("update __app_sql set base = ? where name = ?");
			stmt.bind(1, sql);
			stmt.bind(2, name);
			stmt.step();
			stmt.finalize();
			preprocessSqlQueries(db);
		}
		return name;
	}
}
