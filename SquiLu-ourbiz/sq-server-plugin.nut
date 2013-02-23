local globals = getroottable();
if(!globals.get("APP_CODE_FOLDER", false)) ::APP_CODE_FOLDER <- ".";
WIN32 <- os.getenv("WINDIR") != null;

//local AT_DEV_DBG=true;

//local APP_CODE_FOLDER = sqfs.currentdir();
//local EDIT_MD5_PASSWORD = md5("edit_user:r.dadbiz.es:okdoedit");
//local VIEW_MD5_PASSWORD = md5("view_user:r.dadbiz.es:okdoview");

function checkCompaniesUkDBFile(){
	if (globals.get("jniLog", false)) return APP_CODE_FOLDER + "/companies-uk-RG.db";
	if (globals.get("WIN32", false)) return APP_CODE_FOLDER + "/../../companies-uk/companies-uk-RG.db";
	return "/media/USBHD320/bo/uk/companies-uk-RG.db";
}

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

function getCiaUkSearchList(search_str, search_post_code, search_sic_code,
			search_origin_post_code, search_around_post_code , sic_street, page, limit){
	local offset = page * limit;
	local post_code_radius = strHasContent(search_around_post_code) ? search_around_post_code.tointeger() : 0;
	local stmt, stmt_count, bind_str;
	local hasSicSearch = false;
	local bind_names;
	local base_sql = "select c.id, c.number, c.name, round((julianday('now') - julianday(incorporation_date)) / 354, 1) as age, c.post_code from companies  c "
	//debug_print(search_str or "nil", "\t",search_post_code or "nil", "\t", search_sic_code or "nil", "\t", page, "\n")
	//print(search_str, search_post_code, search_sic_code, search_origin_post_code, search_around_post_code , page, limit)
	//print("AtLine:", __LINE__)
	if (strHasContent(search_str) && search_str.find_lua("%d+") == 0){
		bind_str1 = search_str;
		limit = 0;
		stmt = getCachedStmt("getCiaUkSearchList1", @() format(" %s where c.number = ?", base_sql));
	}
	else if (post_code_radius > 0 && strHasContent(search_origin_post_code)){
		local sic_codes_sql = "";
		local sic_code = null;
		local cached_stmt_name = "";
		local radius_sql_base = [==[
%s, post_codes pc, post_codes ref
where  ref.post_code = :post_code
and pc.easting > ref.easting - :radius and pc.easting < ref.easting + :radius
and pc.northing > ref.northing - :radius and pc.northing < ref.northing +:radius
and is_inside_circle( ref.easting, ref.northing, :radius, pc.easting, pc.northing)
and c.post_code = pc.post_code
%s
limit :limit offset :offset
]==];
		if (strHasContent(search_sic_code)){
			if (search_sic_code.find_lua("%d+") == 0){
				sic_codes_sql = "and c.id in(select company_id from companies_sic_codes where sic_code like :sic_code)";
				sic_code = search_sic_code + "%";
				cached_stmt_name = "getCiaUkSearchList2";
			}
			else
			{
				sic_code = search_sic_code;
				sic_codes_sql = "and c.id in(select company_id from companies_sic_codes csc,  sic_codes_fts sc where sic_codes_fts  match :sic_code and csc.sic_code = sc.docid)";
				cached_stmt_name = "getCiaUkSearchList3";
			}
		}

		//debug_print("\n", string.format(radius_sql_base, base_sql, sic_codes_sql) )
		stmt = getCachedStmt(cached_stmt_name, @() format(radius_sql_base, base_sql, sic_codes_sql));
		limit = 50;
		offset = page * limit;
		bind_names = {post_code = search_origin_post_code, radius = post_code_radius, limit=limit, offset=offset};
		if (sic_code) bind_names.sic_code <- sic_code;
	}
	else if (strHasContent(search_str) && strHasContent(search_post_code)){
		stmt = getCachedStmt("getCiaUkSearchList4", @() format([==[
%s, companies_fts cf
where companies_fts match ?
and c.id = cf.docid
limit ? offset ?
]==], base_sql));
		search_post_code = search_post_code.gsub("%s+", "");
		bind_str = format("name:%s post_code:%s", search_str, search_post_code);
	}
	else if (strHasContent(search_post_code)){
		if (strHasContent(search_sic_code)){
			hasSicSearch = true;
			local sic_code_sql;
			if (search_sic_code.find_lua("%d+") == 0){
				stmt = getCachedStmt("getCiaUkSearchList5", @() format([==[
%s, companies_sic_codes csc
where csc.sic_code like :sic_code
and c.id = csc.company_id
and c.post_code like :post_code
limit :limit offset :offset
]==], base_sql));
				bind_names = {post_code = glob2Sql(search_post_code), sic_code = glob2Sql(search_sic_code), limit=limit, offset=offset};
			}
			else
			{
				stmt = getCachedStmt("getCiaUkSearchList6", @() format([==[
%s , companies_sic_codes csc , sic_codes_fts sc
where sic_codes_fts match :sic_code
and csc.sic_code = sc.docid
and c.id = csc.company_id
and c.post_code like :post_code
limit :limit offset :offset
]==], base_sql));
				bind_names = {post_code = glob2Sql(search_post_code), sic_code = search_sic_code, limit=limit, offset=offset};
			}
		}
		else
		{
			stmt = getCachedStmt("getCiaUkSearchList7", @() format([==[
%s, companies_fts cf
where companies_fts match ?
and c.id = cf.docid
limit ? offset ?
]==], base_sql));
		}
		search_post_code = search_post_code.gsub("%s+", "_");
		bind_str = format("post_code:%s*", search_post_code);
	}
	else if ( (!strHasContent(search_str)) && (!strHasContent(search_post_code)) && strHasContent(search_sic_code)) {
		if (sic_street && sic_street == "street"){
			search_sic_code = "%" + search_sic_code + "%";
			stmt = getCachedStmt("getCiaUkSearchList8", "select distinct id, address, post_code from companies where address like ? order by 2 limit ? offset ?");
		}
		else if (search_sic_code.find_lua("%d+") == 0){
			search_sic_code = search_sic_code + "%";
			stmt = getCachedStmt("getCiaUkSearchList9", "select * from sic_codes where id like ? limit ? offset ?");
		}
		else
		{
			stmt = getCachedStmt("getCiaUkSearchList10", "select sc.* from sic_codes sc, sic_codes_fts scf where sic_codes_fts match ?  and sc.id = scf.docid limit ? offset ?");
		}
		bind_str = search_sic_code;
	}
	else
	{
		bind_str = search_str;
		stmt = getCachedStmt("getCiaUkSearchList11", @() format([==[
%s, companies_fts cf
where cf.name match ?
and c.id = cf.docid
limit ? offset ?
]==], base_sql));
	}

	local xp = 1;
	stmt.reset();
//debug_print("\n", bind_str, ":", limit, ":", offset);
	if (bind_names) stmt.bind_names(bind_names);
	else
	{
		stmt.bind(xp++, bind_str);
		if (hasSicSearch) stmt.bind(xp++, search_sic_code);

		if (limit > 0) {
			stmt.bind(xp++, limit);
			stmt.bind(xp++, offset);
		}
	}
	local result = getDbListFromStmt(stmt, 30);
	result.push(limit);
	return  result;
}

function getOneDbNamedValuesFromStmt(stmt){
	local result;
	if (stmt.step() == stmt.SQLITE_ROW) result = stmt.asTable();
	stmt.reset();
	return result || {};
}

function getCiaUkById(id){
	local stmt = getCachedStmt("getCiaUkById", "select * from company_view where id = ?");
	stmt.reset();
	stmt.bind(1, id);
	return getOneDbNamedValuesFromStmt(stmt);
}

function getCiaUkByIdSicCodes(id){
	local stmt = getCachedStmt("getCiaUkByIdSicCodes", "select sc.* from companies_sic_codes csc join sic_codes sc on csc.sic_code = sc.id where company_id = ?");	stmt.reset();
	stmt.bind(1, id);
	return getDbListFromStmt(stmt);
}

function getCiaUkByIdOldNames(id){
	local stmt = getCachedStmt("getCiaUkByIdOldNames", "select condate, prev_name from companies_old_names where company_id = ?");
	stmt.reset();
	stmt.bind(1, id);
	return getDbListFromStmt(stmt);
}

function getDistances(search_origin_post_code, rows){
	if (rows.len() > 0){
		local stmt = getCachedStmt("getDistances", [==[
select
	round(distance(ref.easting, ref.northing, pc.easting, pc.northing)),
	ifnull(round(bearing(ref.easting, ref.northing, pc.easting, pc.northing)), 0)
from companies c left join post_codes pc on c.post_code = pc.post_code,
	post_codes ref
where c.id = ?
and ref.post_code = ?
]==]);
		foreach( k,v in rows) {
			stmt.reset();
			stmt.bind(1, v[0]);
			stmt.bind(2, search_origin_post_code);
			if (stmt.step() == stmt.SQLITE_ROW){
				v.push(stmt.col(0));
				v.push(stmt.col(1));
			}
		}
		//stmt.reset();
		stmt.finalize();
		rows.sort(@(a,b) (a[6] || 0) <=> (b[6] || 0));
	}
}

function getEastinNorthingForPostCode(post_code){
	local stmt = getCachedStmt("getEastinNorthingForPostCode", [=[
select easting, northing
from post_codes
where post_code = ?
]=]);
	stmt.bind(1, post_code);
	local easting, northing;
	if (stmt.step() == stmt.SQLITE_ROW){
		easting = stmt.col(0);
		northing = stmt.col(1);
	}
	stmt.finalize();
	return [easting, northing];
}

function toDeg(lat){
    return lat * 180.0 / math.PI;
}

 function toRad(x) {
	return x * math.PI / 180.0;
 }

//
// Convert Ordnance Survey grid reference easting/northing coordinate to (OSGB36) latitude/longitude
//
// @param {OsGridRef} easting/northing to be converted to latitude/longitude
// @return {LatLon} latitude/longitude (in OSGB36) of supplied grid reference
//
function osGridToLatLong(easting, northing){
  local E = easting.tofloat();
  local N = northing.tofloat();

  local sin = math.sin;
  local cos = math.cos
  local PI = math.PI;
  local a = 6377563.396;
  local b = 6356256.910;              // Airy 1830 major & minor semi-axes
  local F0 = 0.9996012717;                             // NatGrid scale factor on central meridian
  local lat0 = 49.0*PI/180.0;
  local lon0 = -2.0*PI/180.0;  // NatGrid true origin
  local N0 = -100000.0;
  local E0 = 400000.0;                     // northing & easting of true origin, metres
  local e2 = 1.0 - (b*b)/(a*a);                          // eccentricity squared
  local n = (a-b)/(a+b);
  local n2 = n*n;
  local n3 = n*n*n;

  local lat=lat0;
  local M=0.0;
  do {
    lat = (N-N0-M)/(a*F0) + lat;

    local Ma = (1.0 + n + (5.0/4.0)*n2 + (5.0/4.0)*n3) * (lat-lat0);
    local Mb = (3.0*n + 3.0*n*n + (21.0/8.0)*n3) * sin(lat-lat0) * cos(lat+lat0);
    local Mc = ((15.0/8.0)*n2 + (15.0/8.0)*n3) * sin(2*(lat-lat0)) * cos(2*(lat+lat0));
    local Md = (35.0/24.0)*n3 * sin(3.0*(lat-lat0)) * cos(3*(lat+lat0));
    M = b * F0 * (Ma - Mb + Mc - Md);                // meridional arc

  } while (N-N0-M >= 0.00001);  // ie until < 0.01mm

  local cosLat = cos(lat);
  local sinLat = sin(lat);
  local nu = a*F0/math.sqrt(1.0-e2*sinLat*sinLat);              // transverse radius of curvature
  local rho = a*F0*(1.0-e2)/math.pow(1.0-e2*sinLat*sinLat, 1.5);  // meridional radius of curvature
  local eta2 = nu/rho-1.0;

  local tanLat = math.tan(lat);
  local tan2lat = tanLat*tanLat;
  local tan4lat = tan2lat*tan2lat;
  local tan6lat = tan4lat*tan2lat;
  local secLat = 1.0/cosLat;
  local nu3 = nu*nu*nu;
  local nu5 = nu3*nu*nu;
  local nu7 = nu5*nu*nu;
  local VII = tanLat/(2.0*rho*nu);
  local VIII = tanLat/(24.0*rho*nu3)*(5+3.0*tan2lat+eta2-9.0*tan2lat*eta2);
  local IX = tanLat/(720.0*rho*nu5)*(61+90.0*tan2lat+45.0*tan4lat);
  local X = secLat/nu;
  local XI = secLat/(6.0*nu3)*(nu/rho+2.0*tan2lat);
  local XII = secLat/(120.0*nu5)*(5+28.0*tan2lat+24.0*tan4lat);
  local XIIA = secLat/(5040.0*nu7)*(61+662.0*tan2lat+1320.0*tan4lat+720.0*tan6lat);

  local dE = (E-E0);
  local dE2 = dE*dE;
  local dE3 = dE2*dE;
  local dE4 = dE2*dE2;
  local dE5 = dE3*dE2;
  local dE6 = dE4*dE2;
  local dE7 = dE5*dE2;

  lat = lat - VII*dE2 + VIII*dE4 - IX*dE6;
  local lon = lon0 + X*dE - XI*dE3 + XII*dE5 - XIIA*dE7;

  return [toDeg(lat), toDeg(lon)];
  //return lat, lon
}

function osGridToLatLongAdjusted(easting, northing){
	local lat_long = osGridToLatLong(easting, northing)
	//adjust lat, lon
	lat_long[0] += 0.0024;
	lat_long[1] -=  0.00145;
	return lat_long;
}

function getLatitudeLongitudeForPostCode(post_code){
	local easting_northing = getEastinNorthingForPostCode(post_code);
	return osGridToLatLongAdjusted(easting_northing[0], easting_northing[1]);
}

function downloadChunked(host, file, extra_header=null){
	local sock = socket.tcp();
	sock.settimeout(1000);
	sock.connect(host, 80);

	local count = 0;    // counts number of bytes read
	local req;
	if (extra_header) req = extra_header;
	else req = format("GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", file, host);

	//print("REQUEST:", req)
	sock.send(req);
	local s;
	local data = [];
	while (true){
		local rc = sock.receive("*a");
		if(rc[1] == socket.IO_CLOSED) break;
		else if(rc[1] != socket.IO_DONE) throw (format("socket io error (%d)", rc[1]));
		s = rc[0];
		if (s.len() == 0){
			break;
		}
		data.push(s);
	}
	sock.close();
	//print(file, count)
	return data.concat("\n");
}

function getExtraCompanyDataOnNet(cnum){
	local mainHost = "wck2.companieshouse.gov.uk";

	//get session
	local page;
	try {
		page = downloadChunked(mainHost, "/");
	} catch(e){
	}
	if (!page) return "";

	local cookie1, cookie2;
	local function getCookie(){
		page.gmatch("Set%-Cookie: (chcookie=[^;]+)", function(m){ cookie1=m; return false});
		page.gmatch("Set%-Cookie: (ch_session=[^;]+)", function(m){ cookie2 = m; return false});
		return cookie1 + ", " + cookie2;
	}
	local location;
	page.gmatch("Location: http://wck2.companieshouse.gov.uk(.-)wcframe", function(m) {location=m; return false;});

	//print("LOCATION:", getCookie(), location, page)

	//http://wck2.companieshouse.gov.uk/466fc35f66bf9ef61decbe1581a24080/companysearch

	local postBody = format("cname=&cnumb=%s&stype=A&live=on&cosearch.x=30&cosearch.y=9&cosearch=1", cnum);

	local extra_header = format([==[
POST %scompanysearch HTTP/1.1
Host: %s
Cookie: %s
Content-Type:application/x-www-form-urlencoded
Content-Length:%d
Origin:http://wck2.companieshouse.gov.uk
Referer:http://wck2.companieshouse.gov.uk//wcframe?name=accessCompanyInfo
User-Agent:Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.97 Safari/537.11

%s
]==], location, mainHost, getCookie(), postBody.len(), postBody);

	try {
		page = downloadChunked(mainHost, "/", extra_header);
	} catch(e){
		if(e != "closed") throw e;
	}

	extra_header = format([==[
GET %swcprodorder?ft=1 HTTP/1.1
Host: %s
Cookie: %s
Host:wck2.companieshouse.gov.uk
Referer:http://wck2.companieshouse.gov.uk//wcframe?name=accessCompanyInfo
User-Agent:Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.97 Safari/537.11

]==], location, mainHost, getCookie());

	//print(extra_header)

	page = null;
	try {
		page = downloadChunked(mainHost, "/", extra_header);
	} catch(e){
		if(e != "closed") throw e;
	}
	if (!page) return "";

	//print(err, page)

	local tdata = [];
	local data;
	page.gmatch([==[<!%-%- HTML For any but last document %-%->(.-)<table width="90%%" border="0" cellspacing="0" cellpadding="0" summary="Table order button">]==],
		function(m){data = m; return false;});
	if (!data) return "";

	data.gmatch([==[<!%-%- FILE_END::.-tmpl %-%->.-<td align="left" class="text">(.-)</td>.-<td align="left"><span class="text">(.-)</span>.-<td class="text">(.-)</td>]==],
		function(vtype, vdate, vdesc){ tdata.push(format("%s|%s|%s", vtype, vdate, vdesc)); return true;});

	data = tdata.concat("\n");
	data = data.gsub("<[^>]->", "");
	data = data.replace("&nbsp;", "");
	data = data.replace("&amp;", "&");
	data = data.replace("&#39;", "'");
	return data;
}

function getExtraCompanyData(cid, cnum){
	local data;
	local stmt = ::db.prepare("select data from company_extra_data where id = ?");
	stmt.reset();
	stmt.bind(1, cid);

	if (stmt.step() == stmt.SQLITE_ROW){
		data = stmt.col(0);
	}
	stmt.reset();
	if(data) return data;

	try {
		data = getExtraCompanyDataOnNet(cnum);
		local stmt2 = ::db.prepare("insert into company_extra_data(id, data) values(?,?)");
		stmt2.reset();
		stmt2.bind(1, cid);
		stmt2.bind(2, data);
		stmt2.step();
		//debug_print("company_extra_data", checkCompaniesUkDB():errmsg())
		stmt2.reset();
		return data;
	} catch(e){
		return "";
	}
}

if(AT_DEV_DBG || !globals.get("ourbizDB", false)) {
	dofile(APP_CODE_FOLDER + "/ourbiz.nut");
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
	["/search"] = function(request){
		table_t data = {};
		data.page_name <- "search_results"
		data.limit <- 25;
		local query_string = request.info.query_string;
		bool_t isPost = request.info.request_method == "POST";
		local filed_names = ["search_str", "search_post_code", "search_origin_post_code",
			"search_around_post_code", "search_sic_code", "sic_street", "page"];
		if (isPost) {
			local post_fields =  get_post_fields(request);
			foreach(k in filed_names) data[k] <- post_fields.get(k, null);
		}
		else if (query_string) {
			foreach(k in filed_names) data[k] <-request.get_var(query_string, k);
		}
		else foreach(k in filed_names) data[k] <- null;
		if(!data.get("page", null))  data.page <- 0;
		else data.page = data.page.tointeger();

		local errcode;
		if (strHasContent(data.search_str) || strHasContent(data.search_post_code) || strHasContent(data.search_sic_code)) {
			data.sicSearchResults <- strHasContent(data.search_sic_code) && !(strHasContent(data.search_str) || strHasContent(data.search_post_code))
			local result = getCiaUkSearchList(data.search_str, data.search_post_code, data.search_sic_code,
				data.search_origin_post_code, data.search_around_post_code , data.sic_street, data.page, data.limit);

			if (result[1] /*errcode*/ == SQLite3.SQLITE_INTERRUPT) {
				data.queryWasInterrupted <- true;
			}
			data.rows <- result[0];
			data.limit <- result[2];
			if (!data.sicSearchResults && data.rows.len() == 1) {
					request.print(format("HTTP/1.1 302 Found\r\nLocation: /view?id=%d\r\n\r\n", data.rows[0][0]));
					return true;
			}
			if (strHasContent(data.search_origin_post_code)) {
				getDistances(data.search_origin_post_code, data.rows);
			}
		}

		local mFile = gmFile;
		mFile.clear();
		data.mix_write <- function(str) {if(str) mFile.write(str);}
		fillTemplate("index.tpl", data, AT_DEV_DBG);
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\n\r\n", mFile.len()));
		request.write_blob(gmFile);
		return true;
	},
	["/view"] = function(request){
		local data = {};
		data.page_name <- "view_company";
		local query_string = request.info.query_string;
		if (query_string) data.search_id <- request.get_var(query_string, "id");

		//debug_print(data.search_id, "\n")
		if (strHasContent(data.search_id)){
			data.company <- getCiaUkById(data.search_id);
			local result = getCiaUkByIdSicCodes(data.search_id);
			data.company_sic_codes <- result[0];
			result = getCiaUkByIdOldNames(data.search_id);
			data.company_old_names <- result[0];
			data.extra_data <- getExtraCompanyData(data.company.id, data.company.number);
			if (data.company.post_code) {
				local lat_long = getLatitudeLongitudeForPostCode(data.company.post_code);
				data.latitude <- lat_long[0];
				data.longitude <- lat_long[1];
			}
		}
		local filed_names = ["search_str", "search_post_code", "search_origin_post_code",
			"search_around_post_code", "search_sic_code", "sic_street", "page"];
		foreach(k in filed_names) data[k] <- null;

		//debug_tprint(data.company)

		local mFile = gmFile;
		mFile.clear();
		data.mix_write <- function(str) {mFile.write(str || "");}
		fillTemplate("index.tpl", data, AT_DEV_DBG);
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\n\r\n", mFile.len()));
		request.write_blob(mFile);
		return true;
	},
	["/OURBIZ"] =function(request){
		request.print("HTTP/1.1 200 OK\r\nServer: OurBiz\r\nContent-Length: 0\r\n\r\n");
		return true;
	},
	["/DB/GetList"] = ourbizDbGetList,
	["/DB/GetOne"] = ourbizDbGetOne,
	["/DB/Action"] = ourbizDbAction,
	["/DB/GetBin"] = ourbizDbGetBin,
	["/DB/m"] = ourbizDbMobile,
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
