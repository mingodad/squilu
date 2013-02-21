local globals = getroottable();

if(!globals.get("HTTPConn", false)) dofile("ourbiz-client.nut");

enum edbAction {e_none, e_insert, e_update, e_delete};

class DBTableUpdateBase {
	table_name = null;
	_field_changes = null;
	edit_id = null;
	version = null;
	_dbChanges = null;
	dbAction = null;
	hasVersion = null;
	hasMdate = null;
	useMultiPart = null;
	useSLE = null;
	static boundary = "----OurbizZ1X2c3LkUw";
	static e_none = edbAction.e_none;
	static e_insert = edbAction.e_insert;
	static e_update = edbAction.e_update;
	static e_delete = edbAction.e_delete;

	constructor() {
		table_name = "no_table_name";
		edit_id = 0;
		_dbChanges = 0;
		dbAction = edbAction.e_none;
		hasVersion = hasMdate = true;
		useMultiPart = false;
		useSLE = true;
		_field_changes = [];
	}

	function get_fields_map_name(){return table_name;}

	function add_http_param(httpBody, field, value){
		if(useSLE){
		    _field_changes.push([field, value.tostring()]);
		}
		else if(useMultiPart){
		    httpBody.write(boundary);
		    httpBody.write("\r\nContent-Disposition: form-data; name=\"");
		    httpBody.write(field, "\"\r\n\r\n", value, "\r\n");
		}
		else
		{
		    if(httpBody.size() > 0) httpBody.write("&");
		    httpBody .write(field, "=", url_encode(value));
		}
	}

	function get_http_params(httpBody){
		local key, value;
		key = "__table__";
		value = table_name;
		add_http_param(httpBody, key, value);

		key = "__action__";
		switch(dbAction)
		{
		case edbAction.e_insert:
		    value = "insert";
		    break;
		case edbAction.e_update:
		    value = "update";
		    break;
		case edbAction.e_delete:
		    value = "delete";
		    break;
		}
		add_http_param(httpBody, key, value);

		if(dbAction != edbAction.e_insert)
		{
		    key = "__id__";
		    value = edit_id.tostring();
		    add_http_param(httpBody, key, value);
		}
		if(hasVersion)
		{
		    key = "__version__";
		    value = version.tostring();
		    add_http_param(httpBody, key, value);
		}
	}

	function afterDbActionHttpConn(request_result){}
    
	function doDbActionHttpConn(){
		local my_result = blob(0, 8192);
		local httpRequest = HTTPConn(my_result);
		local appServer = AppServer.getAppServer();
		appServer.httpcon_setup(httpRequest);

		local httpBody = blob(0, 8192);
		local url = "/DB/Action";
		get_http_params(httpBody);

		local body_str = null;
		local nchanges = _field_changes.size();
		if(useSLE){
			local httpBodyMemBuf = blob(0, 8192);
			httpBodyMemBuf.write("[[");
			for(local i=0; i<nchanges; ++i)
			{
				//printf("%s : %s\n", _field_changes[i].key,  _field_changes[i].val);
				add2sle(httpBodyMemBuf, _field_changes[i][0]);
			}
			httpBodyMemBuf.writen(SLE_SLEEND, 'c');
			httpBodyMemBuf.write("][");
			for(local i=0; i<nchanges; ++i)
			{
				add2sle(httpBodyMemBuf, _field_changes[i][1]);
			}
			httpBodyMemBuf.writen(SLE_SLEEND, 'c');
			httpBodyMemBuf.write("]]");

			body_str = httpBodyMemBuf.tostring();
		}
		else
		{
			for(local i=0; i<nchanges; ++i)
			{
				add_http_param(httpBody, _field_changes[i][0], _field_changes[i][1]);
			}
			if(useMultiPart){
				httpBody.write(boundary, "--\r\n");
			}

			body_str = httpBody.tostring();
		}

		httpRequest.send_my_request("POST", url, body_str, body_str.len(),
					 useMultiPart ? boundary + 2 : 0, //+2 to discount "--"
					 useSLE);
		//local check_idle = globals.get("Fl", false);
		while( httpRequest.outstanding() )
		{
			httpRequest.pump();
			//if(check_idle) Fl.check();
		}

		if(httpRequest.my_status != 200){throw(my_result);}
		switch(dbAction)
		{
		case edbAction.e_insert:
		{
			version = 0;
			local rec = {};
			appServer.asle2map(my_result, rec);
			local new_id = rec.get("id", 0).tointeger();
			if(new_id == 0) throw("Could not update this record !");
			edit_id = new_id;
		}
		break;
		case edbAction.e_update:
		case edbAction.e_delete:
		{
			local rec = {};
			appServer.sle2map(my_result, rec);
			local changes = rec.get("changes", 0).tointeger();
			if(changes == 0) {
				if(dbAction == edbAction.e_update) throw("Could not update this record !");
				else throw("Could not delete this record !");
			}
			if(dbAction == edbAction.e_update) ++version;
			else edit_id = version = 0;
		}
		break;
		}
		afterDbActionHttpConn(my_result);
	}

	function clear(){ _field_changes.clear();}

	function value_extra(fld_name, fld_value)
	{
		_field_changes.push([fld_name, fld_value.tostring()]);
	}

	function value_for_insert(fld_name, fld_value)
	{
		if(dbAction == edbAction.e_insert) value_extra(fld_name, fld_value);
	}

	function getWhereClause(){return " where id=? ";}

	function bindWhereClause(stmt, lastParam)
	{
		stmt.bind(++lastParam, edit_id);
		return lastParam;
	}

	function get_db(){return null;}

	function bind_field(stmt, pos, field, value){
		local val = value.tostring();
		//printf("%s\n", val);
		if(val) stmt.bind(pos, val);
		else stmt.bind_null(pos);
	}

	function insert_table(){
		local nchanges = _field_changes.size();
		if(nchanges)
		{
		    _dbChanges = 0;
		    local db = get_db();
		    if(!db) return;
		    local sql = "insert into ";
		    local sql2 = ") values(";
		    sql += table_name + "(";
		    for(local i=0; i<nchanges; ++i)
		    {
			sql += _field_changes[i][0] + "," + "?,";
		    }
		    //erase last comma
		    sql = sql.slice(0, -1);
		    sql2 = sql2.slice(0, -1);

		    sql2 += ")";
		    sql += sql2;

		    //printf("%s\n", sql.c_str());

		    local stmt = db.preapre(sql);
		    for(local i=0; i<nchanges; ++i)
		    {
			local kv = _field_changes[i];
			bind_field(stmt, i+1, kv[0], kv[1]);
		    }
		    _dbChanges = stmt.exec_dml();
		    if(_dbChanges)
		    {
			edit_id = db->last_row_id();
			version = 0;
			clear();
		    }
		    else
		    {
			throw("Could not insert the record !");
		    }
		}
	}

	function update_table()
	{
		if(!edit_id) return;

		local nchanges = _field_changes.size();
		if(nchanges)
		{
		    _dbChanges = 0;
		    local db = get_db();
		    if(!db) return;

		    local sql = "update "  + table_name + " set ";
		    for(local i=0; i<nchanges; ++i)
		    {
			sql += _field_changes[i][0] + "=?,";
		    }
		    //erase last comma
		    sql = sql.slice(0, -1);

		    if(hasMdate) sql += " ,mdate=CURRENT_TIMESTAMP ";
		    if(hasVersion) sql += " ,_version_ = _version_ + 1 ";
		    sql += getWhereClause();
		    if(hasVersion) sql += " and _version_=? ";
		    //printf("%s\n", sql.c_str());

		    local stmt = db.prepare(sql);
		    for(local i=0; i<nchanges; ++i)
		    {
			local kv = _field_changes[i];
			bind_field(stmt, i+1, kv[0], kv[1]);
		    }
		    local lastParam = bindWhereClause(stmt, nchanges);
		    if(hasVersion) stmt.bind(lastParam+1, version);
		    //printf("%d\n", version);
		    _dbChanges = stmt.exec_dml();
		    if(_dbChanges)
		    {
			version++;
			clear();
		    }
		    else
		    {
			throw("Could not update the record !");
		    }
		}
	}

	function delete_record()
	{
		if(!edit_id) return;

		local db = get_db();
		if(!db) return;

		_dbChanges = 0;
		local sql = "delete from " + table_name + getWhereClause();
		if(hasVersion) sql += " and _version_=? ";
		local stmt = db.prepare(sql);
		local lastParam = bindWhereClause(stmt, 0);
		if(hasVersion) stmt.bind(lastParam+1, version);
		_dbChanges = stmt.exec_dml();
		if(_dbChanges)
		{
		    version = 0;
		    edit_id = 0;
		    clear();
		}
		else
		{
		    throw("Could not delete the record !");
		}
	}

	function execAction()
	{
		switch(dbAction)
		{
		case edbAction.e_insert:
		    insert_table();
		    break;
		case edbAction.e_update:
		    update_table();
		    break;
		case edbAction.e_delete:
		    delete_record();
		    break;
		}
	}
};

class DBTableUpdateByDB extends DBTableUpdateBase {
	_db = null;

	constructor(){
		base.constructor();
	}
	function get_db(){return _db;}
};

class DBTableUpdate extends DBTableUpdateBase {

	constructor(){
		base.constructor();
	}
	function get_db(){
		local appServer = AppServer.getAppServer();
		if(appServer.get_conn_type() == AppServer.e_conn_http)
		{
			doDbActionHttpConn();
			return 0;
		}
		return appServer.get_db();
	}
};
