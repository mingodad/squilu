/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
local globals = getroottable();
if(!globals.get("APP_CODE_FOLDER", false)) ::APP_CODE_FOLDER <- ".";
WIN32 <- os.getenv("WINDIR") != null;

//local AT_DEV_DBG=true;

//local APP_CODE_FOLDER = sqfs.currentdir();
//local EDIT_MD5_PASSWORD = md5("edit_user:r.dadbiz.es:okdoedit");
//local VIEW_MD5_PASSWORD = md5("view_user:r.dadbiz.es:okdoview");

if(!globals.get("gmFile", false)) ::gmFile <- blob();
if(!globals.get("__tplCache", false)) ::__tplCache <- {};
if(!globals.get("__stmtCache", false)) ::__stmtCache <- {};
if(!globals.get("db", false)) ::db <- SQLite3(checkCompaniesUkDBFile());
::db.exec_dml("PRAGMA cache_size = 4000;");

function getTemplate(fname, nocache){
	local mixBase = ::__tplCache.get(fname, false);
	if (!mixBase || nocache){
		local rfn = format("%s/%s", APP_CODE_FOLDER, fname);
//debug_print("\n", rfn);
		try {
			mixBase = sqmix.loadfile(rfn);
		} catch(e){
			debug_print("\n", e);
		}
		::__tplCache[fname] <- mixBase;
	}
	return mixBase;
}

function getCachedStmt(stmt_key, sql_or_func){
	local stmt = ::__stmtCache.get(stmt_key, false);
	if (!stmt){
		//local db =checkCompaniesUkDB()
		local sql;
		if (type(sql_or_func) == "function") sql = sql_or_func();
		else sql = sql_or_func;
//debug_print("\n", sql);
		stmt = ::db.prepare(sql);
		::__stmtCache.stmt_key <- stmt;
	}
	return stmt;
}

function unescapeHtml ( str ){
	if (str){
		return str.gsub("(&[^;]-;)", function(m){
				if (m == "&lt;") return "<";
				else if (m == "&gt;") return ">";
				else if (m == "&amp;") return "&";
				return m;
			});
	}
}

function escapeHtml ( str ){
	if (str){
		return str.gsub("([<>&])", function(m){
				if (m == "<") return "&lt;";
				else if (m == ">") return "&gt;";
				else if (m == "&") return "&amp;";
				return m;
			});
	}
}

function fillTemplate(template, data, nocache){
	data.escapeHtml <- escapeHtml;
	local mixFunc =getTemplate(template, nocache);
	mixFunc.call(data);
}

//
// Post
//
function split_filename(path){
  local result;
  path.gmatch("[/\\]?([^/\\]+)$", function(m){
	result = m;
	return false;
  });
  return result;
}

function insert_field (dest, key, value){
  local fld = dest.get(key, null);
  if (!fld) dest[key] <- value;
  else
  {
	if (type (fld) == "array") fld.push(value);
	else  dest[key] <- [fld, value];
  }
}

function multipart_data_get_field_names(headers, name_value){
  local disp_header = headers["content-disposition"] || "";
  local attrs = {};
  disp_header.gmatch(";%s*([^%s=]+)=\"(.-)\"", function(attr, val) {
	attrs[attr] <- val;
	return true;
  });
  name_value[0] = attrs.name;
  name_value[1] = attrs.filename ? split_filename(attrs.filename) : null;
}

function multipart_data_break_headers(header_data){
	local headers = {};
	header_data.gmatch("([^%c%s:]+):%s+([^\n]+)", function(type, val){
		headers[type.tolower()] <- val;
		return true;
	});
	return headers;
}

function multipart_data_read_field_headers(input, state){
	local s, e, pos = state.pos;
	input.lua_find("\r\n\r\n", function(start, end){s=start; e=end; return false;}, pos, true);
	if( s ) {
		state.pos = e+1;
		return multipart_data_break_headers(input.slice(pos, s-1));
	}
	else return nil;
}

function multipart_data_read_field_contents(input, state){
	local boundaryline = "\r\n" + state.boundary;
	local s, e, pos = state.pos;
	input.find_lua(boundaryline, function(start, end){ s=start; e=end; return false;}, pos, true)
	if (s) {
		state.pos = e+1;
		state.size = s-pos;
		return input.slice(pos, s-1);
	}
	else {
		state.size = 0;
		return nil;
	}
}

function multipart_data_file_value(file_contents, file_name, file_size, headers){
  local value = { contents = file_contents, name = file_name, size = file_size };
  foreach( h, v in headers) {
	if (h != "content-disposition") value[h] = v;
  }
  return value;
}

function multipart_data_parse_field(input, state){
	local headers, value;

	headers = multipart_data_read_field_headers(input, state);
	if (headers) {
		local name_value=[];
		multipart_data_get_field_names(headers, name_value);
		if (name_value[1]) { //file_name
			value = multipart_data_read_field_contents(input, state);
			value = multipart_data_file_value(value, name_value[1], state.size, headers);
			name_value[1] = value;
		}
		else name_value[1] = multipart_data_read_field_contents(input, state)
		return name_value;
	}
	return null;
}

function multipart_data_get_boundary(content_type){
	local boundary;
	content_type.gmatch("boundary%=(.-)$", function(m){
		boundary = m;
		return false;
	});
	return "--" + boundary;
}

function parse_multipart_data(input, input_type, tab=null){
	if(!tab) tab = {};
	local state = {};
	state.boundary <- get_boundary(input_type);
	input.find_lua(state.boundary, function(start, end){state.pos = end+1;return false;}, 0, true);
	while(true){
		local name_value = multipart_data_parse_field(input, state);
		if(!name_value) break;
		insert_field(tab, name_value[0], name_value[1]);
	}
	return tab;
}

function parse_qs(qs, tab=null){
	if(!tab) tab = {};
	if (type(qs) == "string") {
		//debug_print(qs)
		qs.gmatch("([^&=]+)=([^&=]*)&?", function(key,val){
			//debug_print(key, "->", val)
			insert_field(tab, url_decode(key), url_decode(val));
			return true;
		});
	}
	else if (qs) throw("Request error: invalid query string");

	return tab;
}

function parse_qs_to_table(qs, tab=null){
	if(!tab) tab = {};
	if (type(qs) == "string") {
		//debug_print(qs)
		qs.gmatch("([^&=]+)=([^&=]*)&?", function(key,val){
			//debug_print(key, "->", val)
			key = url_decode(key);
			tab[key] <- url_decode(val);
			return true;
		});
	}
	else if (qs) throw("Request error: invalid query string");

	return tab;
}

function parse_qs_to_table_k(qs, tab=null, tabk=null){
	if(!tab) tab = {};
	if(!tabk) tabk = [];
	if (type(qs) == "string") {
		qs.gmatch("([^&=]+)=([^&=]*)&?", function(key, val){
		//debug_print(key, "->", val)
			key = url_decode(key);
			tabk.push(key);
			tab[key] <- url_decode(val);
			return true;
		});
	}
	else if (qs) throw("Request error: invalid query string");
	return tab;
}

function parse_post_data(input_type, data, tab = null){
	if(!tab) tab = {};
	local length = data.len();
	if (input_type.find("x-www-form-urlencoded") >= 0) parse_qs(data, tab);
	else if (input_type.find("multipart/form-data") >= 0) parse_multipart_data(data, input_type, tab);
	else if (input_type.find("SLE") >= 0) {
		local vv = [];
		sle2vecOfvec(data, vv);
		if (vv.len() > 0) {
			local names = vv[0];
			local values = vv[1];
			for (local i=0, len = names.len(); i < len; ++i){
				tab[names[i]] <- values[i];
			}
		}
	}
	return tab;
}

function get_post_fields(request, max_len=1024*1000){
	local data_len = (request.get_header("Content-Length") || "0").tointeger();
	local post_fields = {};
	if (data_len > 0 && data_len <= max_len) {
		local content_type = request.get_header("Content-Type") || "x-www-form-urlencoded";
		local data = request.read(data_len);
/*
		local fd = file("post.txt", "wb");
		fd.write(data, data.len());
		fd.close();
		debug_print(conn_get_header("Content-Type"), "\n");
*/
		parse_post_data(content_type, data, post_fields);
	}
	return post_fields;
}

function sanitizePath(path){
	//reorient separators
	path=path.gsub("\\", "/");

	//remove relativeness
	local relatpattern = "%.%.+";

	while (path.find_lua(relatpattern) > 0){
		path=path.gsub(relatpattern, "") //something like    /Repositories/swycartographer/res/msys/ + /mod
	}                              //gets converted to /Repositories/swycartographer/res/msys/mod

	//remove possible doubles
	relatpattern = "//";
	while(path.find(relatpattern) >= 0){
		path=path.gsub(relatpattern, "/");
	}

	//remove trailing slash
	if(path.endswith("/")) path=path.slice(0,-2);
	//remove slash at the begining
	if(path.startswith("/")) path = path.slice(1);
	path = path.gsub("[^A-Za-z0-9_%-%./]", "");
	return path
}

local allowedEditFileExtensions = {
	[".nut"] = true,
	[".tpl"] = true,
	[".html"] = true,
	[".css"] = true,
	[".js"] = true,
}

function isExtensionAllowed(fname){
	local ext;
	fname.gmatch("(%.?[^%.\\/]*)$", @(m) ext=m)
	if( ext ) return allowedEditFileExtensions.get(ext, false);
	return false;
}

function getFilesInPath(path, files=null, prefix=""){
	if(!files) files = [];
	foreach( file in sqfs.dir(path) ){
		if(file != "." && file != ".." ){
			local f = path + "/" + file;
			local pf
			if (prefix.len() > 0) pf = prefix + "/" + file;
			else pf = file;

			local attr = sqfs.attributes (f);

			if(attr.mode == "directory") getFilesInPath (f, files, pf);
			else
			{
				if( isExtensionAllowed(pf) ) files.push(pf);
				//foreach(name, value in attr) print (name, value);
			}
		}
	}
	files.sort();
	return files;
}

function getDbListFromStmt(stmt, maxSeconds=0){
	local error_code = 0;
	local rows = [];
	local db = stmt.get_db();
	if (maxSeconds){
		//local x = 0
		db.progress_handler(25, function(info) {
			//x = x +1
			//debug_print(x, "\n")
			//debug_print(x, ":", os.difftime(os.time(), info[0]), ":", info[1], "\n")
			if (os.difftime(os.time(), info[1]) > info[0]) return 1;
			return 0;
		}, [maxSeconds, os.time()]);
	}
	else db.progress_handler(null);

	try {
		while (stmt.next_row()){
			rows.push(stmt.asArray());
		}
	} catch(e){
		error_code = db.error_code();
	}

	if (maxSeconds) db.progress_handler(null);
	stmt.reset();
	//debug_print("\n", rows.len(), "\n");
	return [rows, error_code];
}

function strHasContent(v){
	if(v && v.len() > 0) return true;
	return false;
}

function send_http_error_500(request, err_msg){
	if(AT_DEV_DBG) {
		foreach(k,v in get_last_stackinfo()) debug_print("\n", k, ":", v);
		debug_print("\n", err_msg, "\n")
	}
	local response = format("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s", 
		err_msg.len(),  err_msg);
	request.write(response, response.len());
	return true;
}

local uri_handlers = {
	["/SQ/testParams"] = function(request){
		request.print("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n")
		request.print("<html><body><h1>Request Info</h1><ul>")
		foreach(k, v in request.info) {
			if ("table" == type(v) ){
				request.print(format("<li><b>%s</b>:</li><ul>", k));
				foreach( k2, v2 in v){
					request.print(format("<li><b>%s</b>: %s</li>", k2, v2));
				}
				request.print("</ul>");
			}
			else request.print(format("<li><b>%s</b>: %s</li>", k, (v == NULL ? "" : v).tostring()));
		}
		request.print("</ul></body></html>");
		return true;
	},
	["/SQ/logout"] = function(request){
		request.close_session();
		request.print(format("HTTP/1.1 302 Found\r\nLocation: http://%s\r\n\r\n", request.info.http_headers.Host))
		return true;
	},
	["/SQ/edit"] = function(request){
		//password protected
		bool_t canEdit = false;
		//print("EDIT_MD5_PASSWORD=", EDIT_MD5_PASSWORD, "\n")
		bool_t isViewOnly = VIEW_MD5_PASSWORD && request.check_password(VIEW_MD5_PASSWORD);
		if (!isViewOnly) canEdit = EDIT_MD5_PASSWORD && request.check_password(EDIT_MD5_PASSWORD);

		if(!(canEdit || isViewOnly) ) {
			request.send_authorization_request("r.dadbiz.es");
			return true;
		}

		table_t data = {
			file_name=null,
			content=null,
		};
		data.files <- getFilesInPath(APP_CODE_FOLDER);
		local query_string = request.info.query_string;
		bool_t isPost = request.info.request_method == "POST";
		if (isPost && canEdit) {
			local post_fields = get_post_fields(request);
			if (post_fields.get("save", false)) {
				local content = post_fields.get("content", null);
				if (content){
					data.file_name <- sanitizePath(post_fields.file_name);
					if (!isExtensionAllowed(data.file_name)) data.file_name = NULL;
					else
					{
						local fd = file(APP_CODE_FOLDER + "/" + data.file_name, "w");
						content = content.gsub("\r", "");
						fd.write(content);
						fd.close();
					}
				}
			}
		}
		else if( query_string ) data.file_name <- request.get_var(query_string, "file");

		//debug_print(data.search_id, "\n")
		if( data.file_name ){
			data.file_name = sanitizePath(data.file_name);
			if(! isExtensionAllowed(data.file_name) ) data.file_name = NULL;
			else
			{
				local fd = file(APP_CODE_FOLDER + "/" + data.file_name, "r");
				if( fd ){
					data.content <- fd.read(fd.len());
					fd.close();
				}
			}
		}

		//debug_tprint(data.company)

		local mFile = gmFile;
		mFile.clear(); //allways reset global vars
		data.mix_write <- function(str){ mFile.write(str || "")}
		fillTemplate("webappedit.tpl", data, AT_DEV_DBG);
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\n\r\n", mFile.len()));
		request.write_blob(mFile);
		return true;
	},
}

function add_uri_hanlders(tbl){
	foreach(k,v in tbl) uri_handlers[k] <- v;
}

if(AT_DEV_DBG || !globals.get("checkCompaniesUkDBFile", false)) {
	dofile(APP_CODE_FOLDER + "/companies-uk.nut");
}

if(AT_DEV_DBG || !globals.get("ourbizDB", false)) {
	dofile(APP_CODE_FOLDER + "/ourbiz.nut");
}

local ourbiz_password = md5("mingote:ourbiz.dadbiz.es:tr14pink");
function handle_request(request){
	//static content served by mongoose directly
	local request_uri = request.info.uri;
	//debug_print(request.get_option("document_root"), request_uri, "\n")
	if(request_uri.startswith("/DB/")){
		if(!request.check_password(ourbiz_password)) {
			request.send_authorization_request("ourbiz.dadbiz.es");
			return true;
		}
	}
	if (request_uri.endswith(".js") || request_uri.endswith(".css") ) return false;
	if (request_uri == "/index.html" || request_uri == "/" ) return uri_handlers["/search"](request);
	if( uri_handlers.get(request_uri, false) ) return uri_handlers[request_uri](request);
	return false;
}
