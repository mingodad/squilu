/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
//start dummy nested scope to allow reload without complain about constants already defined 
//also it hides from outer scope anything declared as local
{

local globals = getroottable();

local function sqliteTrace(udata, sql)
{
	if(sql == "-- TRIGGER ") return;
	debug_print(udata, ":", sql, "\n");
}

local function getCompaniesUkDBFileName(){
	if (globals.rawget("jniLog", false)) return APP_CODE_FOLDER + "/companies-uk-RG.db";
	if (globals.rawget("WIN32", false)) return APP_CODE_FOLDER + "/../../companies-uk/companies-uk-RG.db";
	//return APP_CODE_FOLDER + "/../../companies-uk/companies-uk-RG.db";
	//return APP_CODE_FOLDER + "/../../companies-uk/companies-uk-RG-2014-07.db";
	//return APP_CODE_FOLDER + "/../../companies-uk/companies-uk-2013-10.db";
	return APP_CODE_FOLDER + "/../../companies-uk/companies-uk-2014-07.db";
	//return "file:companies_uk_db?mode=memory&cache=shared";
}

local companiesUkDB = null;
local function getCompaniesUkDB(){
	if(!companiesUkDB) {
		companiesUkDB = SQLite3(getCompaniesUkDBFileName());
		if(AT_DEV_DBG) companiesUkDB.trace(sqliteTrace, "SQL", false);
		//companiesUkDB.exec_dml("PRAGMA cache_size = 4000;");
	}
	return companiesUkDB;
}

local __stmtCache = {};
local function getCachedStmt(stmt_key, sql_or_func){
	local stmt = __stmtCache.get(stmt_key, false);
	if (!stmt){
		//local db =checkCompaniesUkDB()
		local sql;
		if (type(sql_or_func) == "function") sql = sql_or_func();
		else sql = sql_or_func;
		//debug_print("\nsql: ", sql);
		stmt = getCompaniesUkDB().prepare(sql);
		__stmtCache.stmt_key <- stmt;
	}
	return stmt;
}


local function getCiaUkSearchList(search_str, search_post_code, search_sic_code,
			search_origin_post_code, search_around_post_code , sic_street, page, limit, search_extra_info){
	local offset = page * limit;
	local post_code_radius = strHasContent(search_around_post_code) ? search_around_post_code.tointeger() : 0;
	local stmt, stmt_count, bind_str;
	local hasSicSearch = false;
	local bind_names;
	local base_sql = "select c.id, c.number, c.name, round((julianday('now') - julianday(incorporation_date)) / 354, 1) as age, c.post_code from companies  c "
	//debug_print(search_str || "nil", "\t",search_post_code || "nil", "\t", search_sic_code || "nil", "\t", page, "\n")
	//print(search_str, search_post_code, search_sic_code, search_origin_post_code, search_around_post_code , page, limit)
	//print("AtLine:", __LINE__)

	if (strHasContent(search_str) && search_str.find_lua("%d+") == 0){
		bind_str = search_str;
		limit = 0;
		stmt = getCachedStmt("getCiaUkSearchList1", @() format(" %s where c.number = ?", base_sql));
	}
	/*
	else if (strHasContent(search_extra_info) ){
	}
	*/
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
//debug_print("\n", stmt.get_sql(), " == ", limit, ":", offset);
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

local function getOneDbNamedValuesFromStmt(stmt){
	local result;
	if (stmt.step() == stmt.SQLITE_ROW) result = stmt.asTable();
	stmt.reset();
	return result || {};
}

local function getCiaUkById(id){
	local stmt = getCachedStmt("getCiaUkById", "select * from company_view where id = ?");
	stmt.reset();
	stmt.bind(1, id);
	return getOneDbNamedValuesFromStmt(stmt);
}

local function getCiaUkByIdSicCodes(id){
	local stmt = getCachedStmt("getCiaUkByIdSicCodes", "select sc.* from companies_sic_codes csc join sic_codes sc on csc.sic_code = sc.id where company_id = ?");	stmt.reset();
	stmt.bind(1, id);
	return getDbListFromStmt(stmt);
}

local function getCiaUkByIdOldNames(id){
	local stmt = getCachedStmt("getCiaUkByIdOldNames", "select condate, prev_name from companies_old_names where company_id = ?");
	stmt.reset();
	stmt.bind(1, id);
	return getDbListFromStmt(stmt);
}

local function getDistances(search_origin_post_code, rows){
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
		//rows.sort(@(a,b) (a[6] || 0) <=> (b[6] || 0));
		rows.sort(@(a,b) (a[5] || 0) <=> (b[5] || 0));
/*
		foreach( k,v in rows) {
			foreach(zk, zv in v) debug_print(zk.tostring(), ":", zv);
			debug_print("\n");
		}
*/
	}
}

local function getEastinNorthingForPostCode(post_code){
	local stmt = getCachedStmt("getEastinNorthingForPostCode", [=[
select easting, northing
from post_codes
where post_code = ?
]=]);
	stmt.bind(1, post_code);
	local easting=0, northing=0;
	if (stmt.step() == stmt.SQLITE_ROW){
		easting = stmt.col(0);
		northing = stmt.col(1);
	}
	stmt.finalize();
	return [easting, northing];
}

local function toDeg(lat){
    return lat * 180.0 / math.PI;
}

 local function toRad(x) {
	return x * math.PI / 180.0;
 }

//
// Convert Ordnance Survey grid reference easting/northing coordinate to (OSGB36) latitude/longitude
//
// @param {OsGridRef} easting/northing to be converted to latitude/longitude
// @return {LatLon} latitude/longitude (in OSGB36) of supplied grid reference
//
local function osGridToLatLong(easting, northing){
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
  
  local ma_p1 = (1.0 + n + (5.0/4.0)*n2 + (5.0/4.0)*n3);
  local mb_p1 = (3.0*n + 3.0*n2 + (21.0/8.0)*n3);
  local mc_p1 = ((15.0/8.0)*n2 + (15.0/8.0)*n3);
  local md_p1 = (35.0/24.0)*n3;
  local n_less_n0 = N-N0;
  local bf0 = b * F0;
  local af0 = a * F0;
  
  do {
    lat = (n_less_n0-M)/af0 + lat;  
    local lat_less_lat0 =  lat-lat0;
    local lat_plus_lat0 =  lat+lat0;
    local Ma =  ma_p1 * lat_less_lat0;
    local Mb =  mb_p1 * sin(lat_less_lat0) * cos(lat_plus_lat0);
    local Mc =  mc_p1 * sin(2*lat_less_lat0) * cos(2*lat_plus_lat0);
    local Md =  md_p1 * sin(3.0*lat_less_lat0) * cos(3*lat_plus_lat0);
    M = bf0 * (Ma - Mb + Mc - Md);                // meridional arc

  } while (n_less_n0-M >= 0.00001);  // ie until < 0.01mm

  local cosLat = cos(lat);
  local sinLat = sin(lat);
  local e2_sinLat2 = 1.0-e2*sinLat*sinLat;
  local nu = a*F0/math.sqrt(e2_sinLat2);              // transverse radius of curvature
  local rho = a*F0*(1.0-e2)/math.pow(e2_sinLat2, 1.5);  // meridional radius of curvature
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

local function osGridToLatLongAdjusted(easting, northing){
	local lat_long = osGridToLatLong(easting, northing)
	//adjust lat, lon
	lat_long[0] += 0.0024;
	lat_long[1] -=  0.00145;
	return lat_long;
}

local function getLatitudeLongitudeForPostCode(post_code){
	local easting_northing = getEastinNorthingForPostCode(post_code);
	return osGridToLatLongAdjusted(easting_northing[0], easting_northing[1]);
}

local function downloadChunked(host, file, extra_header=null){
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

local function downloadChunked2(host, file, extra_header=null){
	local sock = socket.tcp();
	sock.settimeout(1000);
	sock.connect(host, 80);

	local count = 0;    // counts number of bytes read
	local req;
	if (extra_header) req = extra_header;
	else req = format("GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", file, host);

	//print("REQUEST:", req)
	sock.send(req);
	local rc, s;
	local data = [];

	// load header
	while (true){
		rc = sock.receive("*l");
		s = rc[0];
		//print("s", s, rc[1]);
		//if err == "closed" then break end
		if (s.len() == 0) break;
		//if (rc[1] == socket.IO_CLOSED) break;
	}
	
	// load data
	while (true){
		rc = sock.receive("*l");
		s = rc[0];
		//print("s", s, rc[1]);
		if (s.len() == 0) break;
		local chunk_size = ("0x" + rc[0]).tointeger(16);
		//print(chunk_size);
		
		rc = sock.receive(chunk_size);
		if(rc[1] == socket.IO_CLOSED) break;
		else if(rc[1] != socket.IO_DONE) throw (format("socket io error (%d)", rc[1]));
		s = rc[0];
		if (s.len() == 0){
			break;
		}
		data.push(s);
		rc = sock.receive("*l"); //eat the new line after chunk
	}
	sock.close();
	//print(file, count)
	return data.concat("");
}

local function getExtraCompanyDataOnNet0(cnum){
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

local function getExtraCompanyDataOnNet(cnum){
	local mainHost = "wck2.companieshouse.gov.uk";
	local page = null;
	try {
		page = downloadChunked2(mainHost, "/api/filinghistory?q=" + cnum);
	} catch(e){
		if(e != "closed") throw e;
	}
	if (!page) return "";
	
	local data = json2var(page);
	local tdata = [];
	local results = data.rawget("results", []);
	for(var i=0, len=results.len(); i < len; ++i)
	{
		local rec = results[i];
		local line = rec.rawget("type", "") + "|" + rec.rawget("date", "") + "|" + rec.rawget("description", "");
		line = line.gsub("\n", "\\n");
		tdata.push( line );
	}
	return tdata.concat("\n");
}

local function getExtraCompanyData(cid, cnum){
	local data;
	local db = getCompaniesUkDB();
	local stmt = db.prepare("select data from company_extra_data where id = ?");
	stmt.reset();
	stmt.bind(1, cid);

	if (stmt.step() == stmt.SQLITE_ROW){
		data = stmt.col(0);
	}
	stmt.reset();
	if(data) return data;

	try {
		data = getExtraCompanyDataOnNet(cnum);
		local stmt2 = db.prepare("insert into company_extra_data(id, data) values(?,?)");
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

local my_uri_handlers = {
	["/search"] = function(request){
		table_t data = {};
		data.page_name <- "search_results"
		data.limit <- 25;
		local query_string = request.info.query_string;
		bool_t isPost = request.info.request_method == "POST";
		local filed_names = ["search_str", "search_extra_info", "search_post_code", "search_origin_post_code",
			"search_around_post_code", "search_sic_code", "sic_street", "page"];
		if (isPost) {
			local post_fields =  get_post_fields(request);
			foreach(k in filed_names) data[k] <- post_fields.get(k, null);
		}
		else if (query_string) {
			foreach(k in filed_names) data[k] <-request.get_var(query_string, k);
		}
		else foreach(k in filed_names) data[k] <- null;
		if(!data.rawget("page", null))  data.page <- 0;
		else data.page = data.page.tointeger();

		local errcode;
		if (strHasContent(data.search_str) || strHasContent(data.search_extra_info) || strHasContent(data.search_post_code) 
				|| strHasContent(data.search_sic_code)) {
			data.sicSearchResults <- strHasContent(data.search_sic_code) && !(strHasContent(data.search_str) || strHasContent(data.search_post_code))
			local result = getCiaUkSearchList(data.search_str, data.search_post_code, data.search_sic_code,
				data.search_origin_post_code, data.search_around_post_code , data.sic_street, data.page, data.limit, data.search_extra_info);

			if (result[1] /*errcode*/ == SQLite3.SQLITE_INTERRUPT) {
				data.queryWasInterrupted <- true;
			}
			data.rows <- result[0];
			data.limit <- result[2];
			if (!data.sicSearchResults && data.rows.len() == 1) {
					request.print(format("HTTP/1.1 302 Found\r\nLocation: /view?id=%d\r\nConnection: Keep-Alive\r\n\r\n", data.rows[0][0]));
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
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: Keep-Alive\r\nContent-Length: %d\r\n\r\n", mFile.len()));
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
			"search_around_post_code", "search_sic_code", "sic_street", "page", "search_extra_info"];
		foreach(k in filed_names) data[k] <- null;

		//debug_tprint(data.company)

		local mFile = gmFile;
		mFile.clear();
		data.mix_write <- function(str) {mFile.write(str || "");}
		fillTemplate("index.tpl", data, AT_DEV_DBG);
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: Keep-Alive\r\nContent-Length: %d\r\n\r\n", mFile.len()));
		request.write_blob(mFile);
		return true;
	},
	["/api/v2/companysearch"] = function(request){
		local query_string = request.info.query_string;
		if(!query_string) return false;
		
		local data = {};
		data.limit <- 25;
		local filed_names = ["search_str", "search_extra_info", "search_post_code", "search_origin_post_code",
			"search_around_post_code", "search_sic_code", "sic_street", "page"];
		foreach(k in filed_names) data[k] <-request.get_var(query_string, k);

		data.page <- 0;
		data.search_str <- request.get_var(query_string, "q");

		local result = getCiaUkSearchList(data.search_str, data.search_post_code, data.search_sic_code,
			data.search_origin_post_code, data.search_around_post_code , data.sic_street, data.page, data.limit, data.search_extra_info);

		if (result[1] /*errcode*/ == SQLite3.SQLITE_INTERRUPT) {
			data.queryWasInterrupted <- true;
		}
		

		result = var2json(result[0]);
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nConnection: Keep-Alive\r\nContent-Length: %d\r\n\r\n", result.len()));
		request.print(result);
		return true;
	},
	["/hello"] = function(request){
		local response = "<html><body>Hello World !</body></html>"
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: Keep-Alive\r\nContent-Length: %d\r\n\r\n%s", response.len(), response));
		return true;
	},
}

add_uri_hanlders(my_uri_handlers);

/*
local function my_uri_filter(request)
{
	local request_uri = request.info.uri;
	if(request_uri.startswith("/api/"))
	{
	}
}
add_uri_filters(my_uri_filter);
*/

 ::MyCompaniesUkLoaded <- true;

} //end dummy nested scope
 