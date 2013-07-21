/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
 /*SquiLu
 local k_consts = [
		"account_id", "accounts", "active", "address",  "batch_order_line_id", "buy_description",
		"buy_discount", "buy_notes", "buy_other_costs", "buy_price",
		"buy_quantity_min", "buys", "cash", "cdate", "city", "code", "company",
		"contact", "country", "date", "days", "delete", "description",
		"discount_amt", "discount_over_sales", "discount_pct", "email", "entities", "entity_address",
		"entity_city", "entity_country", "entity_id", "entity_name",
		"entity_order_number", "entity_phone", "entity_sales_tax_exempt",
		"entity_state", "entity_tax_number", "entity_use_sales_tax2",
		"entity_zip", "fax", "first_total", "gps_coordinates", "group_id", "group_set",
		"id", "image_id", "image", "images", "inactive", "insert", "irpf_pct_retention",
		"is_active", "line_subtotal", "line_total", "lines_count", 
		"markup_to_discount", "mdate", "measure_unit_id", "mime_type",
		"months", "name", "notes", "only_prices_older", "order_by_creation",
		"order_by_modification", "order_date", "order_id", "order_number",
		"order_type_id", "order_types", "order_valid_till_date", "orders", "orders_lines",
		"parent_id",
		"payment_type_id", "payment_types", "periode_count", "periode_type", 
		"phone", "price", "price_decimals",
		"price_formula", "product_id", "products",  "quantity", "query_limit",
		"reference", "reference_code", "sales", "sales_tax_exempt",
		"sales_tax_id", "sales_tax1_amt", "sales_tax1_pct", "sales_tax2_amt", "sales_tax2_pct", 
		"sell_description", "sell_markup", "sell_notes",
		"sell_price", "sell_price2", "sell_quantity_min", "sell_without_stock",
		"series", "show_on_buys", "show_on_sales", "show_on_web",
		"show_price_on_web", "state", "stock_max", "stock_min", "subtotal_amt",
		"supplier_code", "tags", "tax_exempt", "tax_number", "thumbnail", "total", "total_amt", "trigger",
		"unit_weight",
		"units_by_package", "update", "user_code", "use_sales_tax2",
		"warranty_id", "web", "weeks", "weight", "weight_total", "with_accounts",
		"with_headers", "with_images", "years", "xref_order_line_id", "zip",		
	];
function mk_consts(){
	k_consts.sort();
	foreach(k in k_consts) putsnl(format("const C_%s = \"%s\";", k, k));
}
 SquiLu*/ 
 
//start dummy nested scope to allow reload without complain about constants already defined 
//also it hides from outer scope anything declared as local
{
 
 //@mk_consts();
// generated-code:begin
const C_account_id = "account_id";
const C_accounts = "accounts";
const C_active = "active";
const C_address = "address";
const C_batch_order_line_id = "batch_order_line_id";
const C_buy_description = "buy_description";
const C_buy_discount = "buy_discount";
const C_buy_notes = "buy_notes";
const C_buy_other_costs = "buy_other_costs";
const C_buy_price = "buy_price";
const C_buy_quantity_min = "buy_quantity_min";
const C_buys = "buys";
const C_cash = "cash";
const C_cdate = "cdate";
const C_city = "city";
const C_code = "code";
const C_company = "company";
const C_contact = "contact";
const C_country = "country";
const C_date = "date";
const C_days = "days";
const C_delete = "delete";
const C_description = "description";
const C_discount_amt = "discount_amt";
const C_discount_over_sales = "discount_over_sales";
const C_discount_pct = "discount_pct";
const C_email = "email";
const C_entities = "entities";
const C_entity_address = "entity_address";
const C_entity_city = "entity_city";
const C_entity_country = "entity_country";
const C_entity_id = "entity_id";
const C_entity_name = "entity_name";
const C_entity_order_number = "entity_order_number";
const C_entity_phone = "entity_phone";
const C_entity_sales_tax_exempt = "entity_sales_tax_exempt";
const C_entity_state = "entity_state";
const C_entity_tax_number = "entity_tax_number";
const C_entity_use_sales_tax2 = "entity_use_sales_tax2";
const C_entity_zip = "entity_zip";
const C_fax = "fax";
const C_first_total = "first_total";
const C_gps_coordinates = "gps_coordinates";
const C_group_id = "group_id";
const C_group_set = "group_set";
const C_id = "id";
const C_image = "image";
const C_image_id = "image_id";
const C_images = "images";
const C_inactive = "inactive";
const C_insert = "insert";
const C_irpf_pct_retention = "irpf_pct_retention";
const C_is_active = "is_active";
const C_line_subtotal = "line_subtotal";
const C_line_total = "line_total";
const C_lines_count = "lines_count";
const C_markup_to_discount = "markup_to_discount";
const C_mdate = "mdate";
const C_measure_unit_id = "measure_unit_id";
const C_mime_type = "mime_type";
const C_months = "months";
const C_name = "name";
const C_notes = "notes";
const C_only_prices_older = "only_prices_older";
const C_order_by_creation = "order_by_creation";
const C_order_by_modification = "order_by_modification";
const C_order_date = "order_date";
const C_order_id = "order_id";
const C_order_number = "order_number";
const C_order_type_id = "order_type_id";
const C_order_types = "order_types";
const C_order_valid_till_date = "order_valid_till_date";
const C_orders = "orders";
const C_orders_lines = "orders_lines";
const C_parent_id = "parent_id";
const C_payment_type_id = "payment_type_id";
const C_payment_types = "payment_types";
const C_periode_count = "periode_count";
const C_periode_type = "periode_type";
const C_phone = "phone";
const C_price = "price";
const C_price_decimals = "price_decimals";
const C_price_formula = "price_formula";
const C_product_id = "product_id";
const C_products = "products";
const C_quantity = "quantity";
const C_query_limit = "query_limit";
const C_reference = "reference";
const C_reference_code = "reference_code";
const C_sales = "sales";
const C_sales_tax1_amt = "sales_tax1_amt";
const C_sales_tax1_pct = "sales_tax1_pct";
const C_sales_tax2_amt = "sales_tax2_amt";
const C_sales_tax2_pct = "sales_tax2_pct";
const C_sales_tax_exempt = "sales_tax_exempt";
const C_sales_tax_id = "sales_tax_id";
const C_sell_description = "sell_description";
const C_sell_markup = "sell_markup";
const C_sell_notes = "sell_notes";
const C_sell_price = "sell_price";
const C_sell_price2 = "sell_price2";
const C_sell_quantity_min = "sell_quantity_min";
const C_sell_without_stock = "sell_without_stock";
const C_series = "series";
const C_show_on_buys = "show_on_buys";
const C_show_on_sales = "show_on_sales";
const C_show_on_web = "show_on_web";
const C_show_price_on_web = "show_price_on_web";
const C_state = "state";
const C_stock_max = "stock_max";
const C_stock_min = "stock_min";
const C_subtotal_amt = "subtotal_amt";
const C_supplier_code = "supplier_code";
const C_tags = "tags";
const C_tax_exempt = "tax_exempt";
const C_tax_number = "tax_number";
const C_thumbnail = "thumbnail";
const C_total = "total";
const C_total_amt = "total_amt";
const C_trigger = "trigger";
const C_unit_weight = "unit_weight";
const C_units_by_package = "units_by_package";
const C_update = "update";
const C_use_sales_tax2 = "use_sales_tax2";
const C_user_code = "user_code";
const C_warranty_id = "warranty_id";
const C_web = "web";
const C_weeks = "weeks";
const C_weight = "weight";
const C_weight_total = "weight_total";
const C_with_accounts = "with_accounts";
const C_with_headers = "with_headers";
const C_with_images = "with_images";
const C_xref_order_line_id = "xref_order_line_id";
const C_years = "years";
const C_zip = "zip";
// generated-code:end 

local globals = getroottable();
if(!globals.get("APP_CODE_FOLDER", false)) ::APP_CODE_FOLDER <- ".";

local constants = getconsttable();

local function getOurbizDBFileName(){
	if(globals.get("jniLog", false)) return APP_CODE_FOLDER + "/ourbiz.db";
	if(globals.get("WIN32", false)) return APP_CODE_FOLDER + "/../../ourbiz-uk/ourbiz.db";
	return "/home/mingo/dev/FrontAccountLua/ourbiz.db";
}

local ourbizDB = null;
local function getOurbizDB(){
	if(!ourbizDB) ourbizDB = SQLite3(getOurbizDBFileName());
	return ourbizDB;
	//return checkCachedDB(APP_CODE_FOLDER + "/ourbiz.db");
}

if(globals.get("AT_DEV_DBG", false) || !globals.get("PdfSqlTable", false)) {
	dofile(APP_CODE_FOLDER + "/pdf-table.nut");
}

local function escape_sql_like_search_str(str){
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

local function mkEmptyWhenZero(tbl, key){
	if (type(key) == "array"){
		foreach( v in key ) {
			if (tbl.get(v, false) == "0") tbl[v] = "";
		}
	}
	else if (tbl[key] == 0) tbl[key] = "";
}

local function mytofloat(val){
	try{
		return val.tofloat();
	}
	catch(e){
		return 0.0;
	}
}

local function add2sle(out_result, str){
	str = str.tostring();
	out_result.write(get_sle_size(str.len()), str);
}

local function WriteSLEArrayFieldNames(str_buf, stmt, col_count)
{
	for(local i=0; i<col_count; ++i)
	{
		add2sle(str_buf, stmt.col_name(i));
	}
}

local function WriteSLEArrayFieldValues(str_buf, stmt, col_count)
{
	for(local i=0; i < col_count; ++i)
	{
		add2sle(str_buf, stmt.asString(i));
	}
}

local function WriteMap2SLEArray(str_buf, map, doAppend=false)
{
	if(!doAppend) str_buf.clear();
	str_buf.write("[[");
	foreach(k,v in map) add2sle(str_buf, k);
	str_buf.writen(SLE_SLEEND, 'c');
	str_buf.write("][");
	foreach(k,v in map) add2sle(str_buf, v);
	str_buf.writen(SLE_SLEEND, 'c');
	str_buf.write("]]");
}

local function mkSLEArray(str_buf, header, body, doAppend=false)
{
	if(!doAppend) str_buf.clear();
	str_buf.write("[[");
	foreach(k,v in header) add2sle(str_buf, k);
	str_buf.writen(SLE_SLEEND, 'c');
	str_buf.write("][");
	foreach(k,v in body) add2sle(str_buf, v);
	str_buf.writen(SLE_SLEEND, 'c');
	str_buf.write("]]");
}

constants.rawdelete("TimePeriode");

//enum do not does autonumber with initialization
enum TimePeriode {is_years = 1, is_months = 2, is_weeks = 3, is_days = 4};

local function getStatisticsPeriodeType(speriode){
	if(speriode == C_years) return TimePeriode.is_years;
	else if(speriode == C_weeks) return TimePeriode.is_weeks;
	else if(speriode == C_days) return TimePeriode.is_days;
	return TimePeriode.is_months;
}

local function get_sql_bar_chart_statistics_periodes (periode_count, periode_type){

	local speriode2;
	local periodeMultiplier = 1;
	local speriode;

	switch(periode_type){
		case TimePeriode.is_years:{
			speriode2 = "%Y";
			speriode = C_years;
		}
		break;
		case TimePeriode.is_weeks:{
			periodeMultiplier = 7;
			speriode2 = "%Y-%W";
			speriode = C_days;
		}
		break;
		case TimePeriode.is_days:{
			speriode2 = "%Y-%m-%d";
			speriode = C_days;
		}
		break;
		default:{
			speriode2 = "%Y-%m";
			speriode = C_months;
		}
	}

	periode_count = periode_count * periodeMultiplier;
	local result_periode = format("strftime('%s', order_date)", speriode2);
	local result_periode_count = format("date('now', '-%d %s')", periode_count, speriode);
	return [result_periode, result_periode_count];
}

local function getOptionsFromMap(map){
	local opt = {};
	opt.search_str <- map.get("search_str", null);
	opt.select_fields <- map.get("select_fields", null);
	opt.search_on <- map.get("search_on", "1"); //to make easy on html interface
	local CHECK_BOOL = function(field){
		local value = map.get(field, null);
		if (value == "1") opt[field] <- "1";
		else opt[field] <- null;
	}
	local math_floor = math.floor;
	local CHECK_INT_DFLT = function(field, dflt) {
		local value = map.get(field, dflt).tointeger();
		opt[field] <- math_floor(value);
	}
	local CHECK_INT = function(field) {CHECK_INT_DFLT(field, 0) };

	CHECK_BOOL(C_active);
	CHECK_BOOL(C_buys);
	CHECK_BOOL(C_cdate);
	CHECK_BOOL(C_group_set);
	CHECK_BOOL(C_inactive);
	CHECK_BOOL(C_mdate);
	CHECK_BOOL(C_only_prices_older);
	CHECK_BOOL(C_order_by_creation);
	CHECK_BOOL(C_order_by_modification);
	CHECK_BOOL(C_sales);
	CHECK_INT(C_entity_id);
	CHECK_INT(C_group_id);
	CHECK_INT(C_image_id);
	CHECK_INT(C_account_id);
	CHECK_INT(C_order_id);
	CHECK_INT(C_payment_type_id);
	CHECK_INT(C_product_id);
	CHECK_INT_DFLT(C_query_limit, 50);
	CHECK_BOOL(C_contact);
	CHECK_BOOL(C_date);
	CHECK_BOOL(C_description);
	CHECK_BOOL(C_entities);
	CHECK_BOOL(C_id);
	CHECK_BOOL(C_name);
	CHECK_BOOL(C_notes);
	CHECK_BOOL(C_phone);
	CHECK_BOOL(C_products);
	CHECK_BOOL(C_reference);
	CHECK_BOOL(C_total);
	CHECK_BOOL(C_with_images);
	CHECK_BOOL(C_with_headers);
	CHECK_BOOL(C_with_accounts);
	CHECK_BOOL(C_code);
	return opt;
}

local function checkQueryStringSAB(qs_tbl, so=null){
	local qs_sab = qs_tbl.get("sab", null);
	if (qs_sab){
		if (qs_sab == "S") {
			if(so) so.sales <- true;
			return qs_sab;
		}
		else if (qs_sab == "B") {
			so.buys <- true;
			return qs_sab;			
		}
		else if (qs_sab == "A") return qs_sab;
	}
	return false;
}

local function get_search_options(req){
	local search_opt = getOptionsFromMap(req);

	local sab = req.get("sab", null);
	if (sab && sab.len() > 0){
		local c = sab.toupper();
		if (c == "S") search_opt.sales <- true;
		else if (c == "B") search_opt.buys <- true;
	}
	return search_opt;
}


local DB_Manager = class {
	table_name = null;
	editable_fields = null;
	has_mdate = null;
	has_version = null;

	constructor(ptable_name, peditable_fields = null){
		if(!peditable_fields) peditable_fields =  [C_is_active, C_code, C_description];
		table_name = ptable_name;
		editable_fields = peditable_fields;
		has_mdate = true;
		has_version = true;
	}

	function db_action(db, data){
		local action = data.__action__;
		if (action == C_insert) return db_insert(db,data);
		else if (action == C_update) return db_update(db,data);
		else if (action == C_delete) return db_delete(db,data);
	}

	function db_insert(db, data){
		local mf = blob();
		mf.write("insert into ", table_name,  "(")
		local isFirst = true;
		foreach( k,v in editable_fields) {
			if (data.get(v, false)){
				if (isFirst) isFirst = false;
				else mf.write(",");
				mf.write(v);
			}
		}
		mf.write(") values(");
		isFirst = true;
		foreach( k,v in editable_fields) {
			if (data.get(v, false)){
				if (isFirst) isFirst = false;
				else mf.write(",");
				mf.write("?");
			}
		}
		mf.write(")");

		local stmt = db.prepare(mf.tostring());
		local x = 0;
		foreach( k,v in editable_fields) {
			if (data.get(v, false)){
				stmt.bind_empty_null(++x, data[v]);
			}
		}
		local result = stmt.step();
		stmt.finalize()
		if (result == SQLite3Stmt.DONE) return db.last_insert_rowid();
		throw db.errmsg();
	}

	function db_update(db, data){
		local mf = blob();
		mf.write("update ", table_name,  " set ");
		local isFirst = true;
		foreach( k,v in editable_fields) {
			if (data.get(v, false)){
				if (isFirst) isFirst = false;
				else mf.write(",");
				mf.write(v, "=?");
			}
		}
		if (has_mdate) mf.write(", mdate=CURRENT_TIMESTAMP ");
		if (has_version) mf.write(", _version_=_version_+1 ");

		mf.write(" where id=? ");
		if (has_version) mf.write(" and _version_=?");
		local stmt = db.prepare(mf.tostring());
		local x = 0;
		foreach( k,v in editable_fields) {
			if (data.get(v, false)) {
				stmt.bind_empty_null(++x, data[v]);
			}
		}
		stmt.bind(++x, data.__id__);
		if (has_version) stmt.bind(++x, data.__version__);

		local result = stmt.step();
		stmt.finalize();
		if (result == SQLite3Stmt.SQLITE_DONE) return db.changes();
		throw db.errmsg();
	}

	function db_delete(db, data){
		local mf = blob();
		mf.write("delete from ", table_name, " where id=? ");
		if (has_version) mf.write(" and _version_=?");

		local stmt = db.prepare(mf.tostring());
		stmt.bind(1, data.__id__);
		if (has_version) stmt.bind(2, data.__version__);

		local result = stmt.step();
		stmt.finalize();
		if (result == SQLite3Stmt.DONE) return db.changes();
		throw db.errmsg();
	}

	function sql_get_one(tbl_qs) {
		return format("select * from %s where id=?", table_name);
	}

	function sql_list(qs_tbl=null, post_tbl=null) {
		return format("select id, code, description, is_active from %s order by description", table_name);
	}

	function sql_short_list(req) {
		return format("select id, description from %s where is_active=1 order by 2", table_name);
	}

	function get_bar_chart_statistics_sql_core(aId, sab, str1, str2){
		local mf = blob();

		mf.write("SELECT ", str1, " as qm, sum(subtotal - total_discount + total_sales_tax1 + total_sales_tax2) as q, '",
			sab, [==[' as sab
FROM orders as o
WHERE entity_id = ]==], aId);
		if (sab == "S" || sab == "B"){
			mf.write(" and o.order_type_id in(select * from order_types_");
			if (sab == "S") mf.write("sell");
			else mf.write("buy");
			mf.write("_payment) ");
		}
		mf.write(" and o.order_date >= ", str2, " group by qm ");
		//debug_print(tostring(mf), "\n");
		return mf.tostring();
	}
	
	function setSleResultChanes (buf, changes)
	{
		mkSLEArray(buf, ["changes"], [changes]);
	}

	function setSleResultId (buf, id)
	{
		mkSLEArray(buf, ["id"], [id]);
	}
	
	function save_image (db, tbl_qs, tbl, sle_buf)                                                                              {
		local thumbnail = tbl_qs.get("thumbnail", null);
		if(thumbnail)
		{
			local image = tbl_qs.get("image", null);
			local mime_type = tbl_qs.get("mime_type", null);

			local id = tbl_qs.get("__id__", 0);
			local isNewRecord = id == 0;
			if(isNewRecord) id = db.last_row_id(); //new record get last_insert_rowid
			if(id)
			{
				local stmt = db.prepare(format("select image_id from %s where id=%d", tbl, id));
				local image_id, changes;
				if(stmt.next_row()) image_id = stmt.col(0);
				else image_id = 0;

				if(image_id)
				{
					stmt.prepare("update images set mime_type= ?, thumbnail = ?, image = ? where id=%d", image_id);
					stmt.bind(1, mime_type);
					stmt.bind_blob(2, thumbnail, thumbnail.len());
					stmt.bind_blob(3, image, image.len());
					changes = stmt.exec_dml();
					setSleResultChanes(sle_buf, changes);
				}
				else
				{
					db.exec_dml("begin;");
					try {
						stmt.prepare(format([==[
insert into images(name, mime_type, thumbnail, image) values('%c-%d', ?, ?, ?)]==], tbl[0], id));
						stmt.bind(1, mime_type);
						stmt.bind_blob(2, thumbnail, thumbnail.len());
						stmt.bind_blob(3, image, image.len());
						changes = stmt.exec_dml();
						if(changes){
							image_id = db.last_row_id();
							stmt.prepare(format("update %s set image_id=%d where id=%d",
								tbl, image_id, id));
							changes = stmt.exec_dml();
						}
						db.exec_dml("commit;");
						if(!isNewRecord){
							setSleResultChanes(sle_buf, changes);
						}
					}
					catch(e){
						db.exec_dml("rollback;");
						throw(e);
					}
				}
			}
		}
	}    
}

//sql queries

local function get_one_record_sql (tbl, id, fields)
{
	return format("select %s from %s where id=%d", fields, tbl, id);
}

local function get_sales_tax_rates_short_list_sql(){
	return "select id, description from sales_tax_rates where is_active=1 order by 2";
}

local function get_measure_units_short_list_sql(){
	return [==[
select id, description
from measure_units
where is_active=1
order by description
]==];
}

local function get_warranty_types_short_list_sql(){
	return [==[
select id, description from warranty_types
where is_active=1
order by description
]==];
}

local function order_totals_get_one(id)
{
	return format([==[
SELECT order_id,
	count(*) AS lines_count,
	sum(weight) AS weight_total,
	sum(subtotal_amt) AS subtotal_amt,
	sum(discount_amt) AS discount_amt,
	sum(sales_tax1_amt) AS sales_tax1_amt,
	sum(sales_tax2_amt) AS sales_tax2_amt,
	sum(total) AS total_amt
FROM order_line_calc_view
WHERE order_id =%d]==], id);
}

local function orders_lines_get_one(id)
{
	return format([==[
SELECT ol.*, p.weight as unit_weight,
	pbs.quantity as xref_order_line_quantity
FROM orders_lines as ol JOIN products as p
	ON ol.product_id = p.id
	LEFT JOIN products_inventory_pending as pbs
	ON ol.id = pbs.order_line_id
WHERE ol.id=%d]==], id);
}

local function products_order_pending_get_one(product_id, entity_id)
{
	return format([==[
SELECT pp.order_line_id as xref_order_line_id,
	pp.quantity as xref_order_line_quantity
FROM products_order_pending as pp, orders_lines as ol, orders as o
WHERE pp.product_id = %d
	AND ol.id = pp.order_line_id
	AND o.id = ol.order_id
	AND o.entity_id = %d
LIMIT 1]==], product_id, entity_id);
}

local function products_inventory_pending_get_one(product_id)
{
	return format([==[
SELECT order_line_id as xref_order_line_id,
	quantity as xref_order_line_quantity
FROM products_inventory_pending
WHERE product_id = %d
LIMIT 1]==], product_id);
}

local function orders_get_one(id)
{
	return format([==[
SELECT o.*, ott.* FROM orders AS o LEFT JOIN ( %s) AS ott ON o.id = ott.order_id WHERE o.id =%d ]==],
		order_totals_get_one(id), id);
}

local function entity_for_order_get_one(entity_id)
{
	return format([==[
select
	address as entity_address,
	city as entity_city,
	id as entity_id,
	name as entity_name,
	phone as entity_phone,
	sales_tax_exempt as entity_sales_tax_exempt,
	state as entity_state,
	country as entity_country,
	tax_number as entity_tax_number,
	use_sales_tax2 as entity_use_sales_tax2,
	zip as entity_zip
from entities
where id=%d]==], entity_id);
}

local function product_for_order_get_one(product_id)
{
	return format([==[
SELECT
p.sell_description as description,
sell_price as price,
price_decimals,
p.id as product_id,
sell_quantity_min as quantity,
st.rate1 as  sales_tax1_pct,
st.rate2 as sales_tax2_pct,
p.weight, p.weight as unit_weight
FROM products as p LEFT JOIN sales_tax_rates as st
ON p.sales_tax_id = st.id
WHERE p.id=%d
LIMIT 1]==], product_id);
}

local function orders_sales_history(ptype, plimit)
{
	// Select history type
	if(ptype < 1) return "select '' as 'Select one|-1'; ";

	local sql = [==[
select t.product_id as 'id|ID|8|R',
	sell_description as 'sell_description|Product|-1',
	t.quantity as 'quantity|Qty.|10|R|N',
	t.total as 'total|Value|12|R|M',
	bround((t.total/gtotal)*100, 3)  as 'pct_total|% Total|10|R|P'
from (
	select product_id, bround(sum(quantity)) as quantity , bround(sum(price*quantity), 2) as total
	from orders_lines
	where order_id in(select id from orders where order_type_id in(select * from order_types_sell_payment))
	group by product_id
) as t, products, (
	select sum(price*quantity)  as gtotal from orders_lines
	where order_id in(select id from orders where order_type_id in(select * from order_types_sell_payment))
)
where t.product_id = id
order by]==];

	switch(ptype)
	{
		case 1:
			sql += " 4 desc";
		break;
		case 2:
			sql += " 2";
		break;
	}
	return sql;
}

local function orders_lines_sql_for_order(order_id)
{
	return format([==[
select
	id,
	product_id,
	description,
	quantity,
	price,
	quantity * price as 'first_total'
from orders_lines
where order_id = %d
order by id]==], order_id)
}

local function entity_past_products_get_sql(entity_id)
{
	return format([==[
SELECT
	product_id as 'id|ID|6|R',
	sell_description as 'sell_description|Product|-1',
	sum(quantity) as 'quantity|Quantity|8|R',
	sum(quantity*price) as 'amount|Amount|8|R|M',
	code as 'order_type_code|Type|4|C'
FROM orders_lines as ol, orders as o, order_types as ot, products as p
WHERE o.entity_id = %d
	AND ot.id = o.order_type_id
	AND ol.order_id = o.id
	AND p.id = ol.product_id
GROUP BY product_id, code
ORDER BY 3 desc]==], entity_id);
}

local function last_product_order_lines_get_sql (product_id)
{
	return format([==[
SELECT ol.product_id as 'id|ID|8|R',
	ol.quantity as 'quantity|Quantity|9|R|D',
	ol.description as 'description|Product|-1',
	ot.code as 'order_type_code|Type|5',
	o.order_date as 'order_date|Date|9'
FROM orders_lines as ol, orders as o, order_types as ot
	where ol.product_id = %d
	AND o.id = ol.order_id
	AND ot.id = o.order_type_id
order by 1 desc limit 20]==], product_id);
}

local function product_appear_together_get_sql(product_id)
{
	return format([==[
SELECT
pg.product_id as 'id|ID|8|R',
pg.quantity as 'quantity|Quantity|9|R|D',
pg.num as 'times|Times|8|R',
sell_description as 'description|Product|-1'
FROM (
SELECT count(*) as num, sum(quantity) as quantity, product_id
FROM orders_lines WHERE order_id in (
SELECT order_id FROM orders_lines
    WHERE product_id = %d
) GROUP BY product_id
ORDER BY 1 desc
LIMIT 20
) as pg, products p
WHERE pg.product_id=p.id
ORDER BY 3 desc]==], product_id);
}

local function orders_numbering_get_sql(order_id, version)
{
	return format([==[
UPDATE orders SET order_number = ifnull(
	(SELECT max(order_number)+1
	FROM orders
	WHERE order_type_id = (SELECT order_type_id from orders where id= %d)
	AND series = (SELECT series from orders where id= %d))
	, strftime("%%Y0001")),
	_version_=_version_+1
WHERE id = %d
	AND _version_= %d
	AND order_number IS NULL]==], order_id, order_id, order_id, version);
}

local function discount_by_quantity_get_one(product_id, quantity)
{
	return format([==[
SELECT discount_pct, price
FROM (
	SELECT quantity, discount_pct, price
	FROM product_prices
	WHERE product_id = %d
	UNION ALL
	SELECT sell_quantity_min as quantity, 0 as discount_pct, sell_price as price
	FROM products
	WHERE id = %d
	ORDER BY quantity
)
WHERE quantity <= %f
ORDER BY quantity DESC LIMIT 1;]==], product_id, product_id, quantity);
}

local function order_lines_onhand_get_sql(order_id)
{
	return format([==[
SELECT
	product_id as 'id|ID|8|R',
	description as 'description|Product|-1',
	quantity as 'quantity|Quantity|9|R|D',
	case when p.kit > 0 then
			p.kit_onhand
		else
			p.quantity_onhand
		end as 'quantity_onhand|Onhand|9|R|D',
	case when quantity > (case when p.kit > 0 then
			p.kit_onhand
		else
			p.quantity_onhand
		end)
	then 'X' else '' end as 'x|X|3|C'
FROM orders_lines ol, products_onhand p
WHERE order_id = %d
AND ol.product_id = p.id
ORDER BY ol.id]==], order_id);
}

local function orders_sum_search_sql(query_limit)
{
	return format([==[
SELECT
	id as 'id|ID|6|R',
	cdate as 'cdate|DATE|9',
	amount_total as 'amount_total|Total|9|R|M',
	description as 'description|Description|-1'
FROM orders_sum_groups
ORDER BY id DESC
LIMIT %d]==], query_limit);
}

local function product_availability_get_sql(product_id, order_id, order_type_map){
	local entity_id = order_type_map.entity_id.tointeger();
	return format([==[
SELECT pp.order_line_id as 'id|ID|6|R',
	ot.code as 'type|Type|4',
	pp.quantity as 'quantity|Quantity|9|R|N',
	o.order_date as 'order_date|DATE|9',
	o.entity_name as 'entity_name|Entiy|-1'
FROM products_quote_pending as pp, orders_lines as ol, orders as o, order_types as ot
WHERE pp.product_id = %d
	AND ol.id = pp.order_line_id
	AND o.id = ol.order_id
	AND o.entity_id = %d
	AND ot.id = o.order_type_id

UNION ALL

SELECT pp.order_line_id as 'id|ID|6|R',
	ot.code as 'type|Type|4',
	pp.quantity as 'quantity|Quantity|9|R|N',
	o.order_date as 'order_date|DATE|9',
	o.entity_name as 'entity_name|Entiy|-1'
FROM products_order_pending as pp, orders_lines as ol, orders as o, order_types as ot
WHERE pp.product_id = %d
	AND ol.id = pp.order_line_id
	AND o.id = ol.order_id
	AND o.entity_id = %d
	AND ot.id = o.order_type_id

UNION ALL

SELECT pp.order_line_id as 'id|ID|6|R',
	ot.code as 'type|Type|4',
	pp.quantity as 'quantity|Quantity|9|R|N',
	o.order_date as 'order_date|DATE|9',
	o.entity_name as 'entity_name|Entiy|-1'
FROM products_inventory_pending as pp, orders_lines as ol, orders as o, order_types as ot
WHERE pp.product_id = %d
	AND ol.id = pp.order_line_id
	AND o.id = ol.order_id
	AND ot.id = o.order_type_id
]==], product_id, entity_id, product_id, entity_id, product_id);
}

local function product_prices_list_sql (product_id){
	product_id = product_id.tointeger();
	return format([==[
select id,quantity ,
	markup_pct ,
	discount_pct ,
	price, mdate, cdate
from product_prices
where product_id = %d
UNION ALL
select 0 , sell_quantity_min, sell_markup, 0,sell_price , price_date , cdate
from products
where id = %d
order by quantity
]==], product_id, product_id);
}

local function products_list_sql(){
	return [==[
select
    p.id  as 'id|ID|6|R',
    case when p.user_code isnull then ' ' else p.user_code end as 'reference|Reference|9',
    p.sell_description as 'sell_description|Description|-1',
    cast(case when p.kit isnull then 0 else kit end as text) as 'kit|Kit|4|C',
    round((p.sell_price * (1 + (st.rate1/100.0))),p.price_decimals) as 'price_taxed|Price+V.A.T.|12|R|M',
    round(case when p.kit > 0 then p.kit_onhand else p.quantity_onhand end, 4) as 'quantity_onhand|Onhand|10|R|N'
from products_onhand as p left join sales_tax_rates as st
on p.sales_tax_id = st.id order by 3
]==];
}

local function product_sales_history_sql(ptype, plimit, product_id)
{
	// Select history type
	if(ptype < 1) return "select '' as 'Select one|-1'; ";

	//view_doc_sales = true
	//print(product_id, ptype, plimit)
	
	local sql1 = format([==[
select ol.order_id as 'order_id|ID|8|R', o.order_date as 'order_date|Date|10',
	o.entity_name as 'entity_name|Entity|-1',ol.quantity as 'quantity|Qty.|9|R',
	(ol.quantity * ol.price ) as 'total|Total|9|R|M',ot.code as 'order_type_code|Type|7|C'
from orders as o, orders_lines as ol, order_types as ot
where ol.product_id = %d
	and o.id = ol.order_id
	and ot.id = o.order_type_id
order by ]==], product_id);

	local sql2 = format([==[
select ol.id as 'id|ID|6|R',o.entity_id as 'entity_id|EID|0',
	o.entity_name as 'entity_name|Entity|-1', o.order_date as 'order_date|Date|8',
	ol.quantity as 'quantity|Qty.|6|R', ol.price * ol.quantity as 'total|Total|9|R|M',
	ol.order_id as 'order_id|IID|0',	ot.code as 'order_type_code|Type|5|C'
from orders_lines as ol, orders as o, order_types as ot
where ol.product_id = %d
	and ol.order_id = o.id
	and ot.id = o.order_type_id
order by ]==], product_id);

	local sql3 = format([==[
select ol.order_id as 'order_id|ID|6|R', o.entity_name as 'entity_name|Entity|-1',
	o.order_date as 'order_date|Date|10',
	ol.quantity as 'quantity|Qty.|9|R',
	ol.price as 'price|Price|9|R|M',
	ol.discount_pct as 'discount_pct|Disc.|9|R|M',
	ot.code as 'order_type_code|Type|7|C'
from orders_lines as ol, orders as o, order_types as ot
where ol.product_id = %d
	and ol.order_id = o.id
	and ot.id = o.order_type_id
order by ]==], product_id);

	local result = blob(0, 8192);

	switch(ptype)
	{
		case 1:
			result.write(sql1, " 2 desc, 1 desc ");
		break;
		case 2:
			result.write(sql1, " 5 desc ");
		break;
		case 3:
			result.write(sql1, " 3, 2 ");
		break;
		case 4:
			result.write(sql2, " 6 desc ");
		break;
		case 5:
			result.write(sql2, " 5 desc ");
		break;
		case 6:
		case 7:
		{
			//view_doc_sales = false
			result = [==[
select 
	ph.cdate as 'cdate|Date|10',
	p.id as 'id|ID|8|R',
	p.sell_description as 'sell_description|Description|-1',
	ph.sell_price as 'sell_price|Price|9|R|M',
	ph.buy_price as 'buy_price|Buy|9|R|M',
	ph.buy_other_costs as 'buy_other_costs|Deliv.|9|R|M',
	ph.sell_markup as 'sell_markup|Markup|9|R|M'
from product_prices_history as ph, products as p
where ph.product_id = p.id ]==];

			if(ptype == 6) result = format("%s and product_id = %d", result, product_id);
			result += " order by 1 desc ";
		}
		break;
		case 8:
		{
			//view_doc_sales = false
			result = [==[
select
	cdate as 'cdate|Date|10',
	description as 'description|Description|-1',
	new_on_hand as 'new_on_hand|New Onhand|9|R',
	old_on_hand as 'old_on_hand|Old Onhand|9|R'
from products_inventory_history_view
where 1=1 ]==];
			if(ptype == 7) result = format("%s and product_id = %d ", result, product_id);
			result += " order by cdate desc "; 
		}
		break;
	}
	//print(result)
	if(plimit) result = format("%s limit %d", result.tostring(), plimit);
	return result.tostring();
}

local function product_prices_short_list_sql(product_id){
	return format("select id, quantity, price from product_prices where product_id=%d", product_id);
}

local function products_kit_list_sql (kit_id, partOf=false){
	kit_id = kit_id.tointeger();
	local str1, str2;
	if (partOf){
		str1 = format("pk.product_id=%d", kit_id);
		str2 = "and pk.kit_id = p.id order by p.sell_description";
	}
	else
	{
		str1 = format("pk.kit_id=%d", kit_id);
		str2 = "and pk.product_id = p.id order by p.sell_description";
	}
	return format([==[
select pk.id,
	p.id,
	p.sell_description,
	pk.quantity,
	round((p.sell_price*pk.quantity),4) as amount,
	p.quantity_onhand
from product_kits as pk, products as p
where %s %s
]==], str1, str2);
}

local function products_kit_details_get_one (kit_id){
	kit_id = kit_id.tointeger();
	return format([==[
select count(*) as nproducts, sum(p.sell_price * pk.quantity) as amt_total
from product_kits as pk left join products as p on pk.product_id = p.id
where pk.kit_id = %d]==], kit_id);
}

local function products_kit_edit_get_one(id)
{
	return format([==[
select pk.*, p.sell_description, p.sell_price
from product_kits as pk left join products as p
	on pk.product_id = p.id
where pk.id=%d]==], id);
}

local function products_kit_product_get_one(product_id)
{
	return format([==[
select id as product_id, sell_description, 1 as quantity, sell_price
from products 
where id=%d]==], product_id);
}

local function entity_sales_history_sql(history_type, query_limit, entity_id){
	// Select history type
	if (history_type < 1) {return "select '' as 'Select one|-1';" };

	local str_entity_id = entity_id.tostring();
	local mf = blob();
	if (history_type < 3){
		mf.write([==[
SELECT o.id as 'id|ID|6|R', o.order_date as 'order_date|Date|10',
ot.code as 'order_type_code|Type|7|C', pt.code as 'payment_type_code|P.T.|4|C',
discount_amt as 'discount_amt|Disc.|9|R|ZM', total as 'total|Total|9|R|M'
FROM orders as o
LEFT JOIN order_types as ot
ON o.order_type_id = ot.id
LEFT JOIN payment_types as pt
ON o.payment_type_id = pt.id
, (
	SELECT order_id, sum(discount_amt) as  discount_amt, sum(total) as total
	FROM order_line_calc_view
	WHERE order_id in(SELECT id FROM orders WHERE entity_id =]==], str_entity_id, [==[)
	GROUP BY order_id
)  as ott
WHERE entity_id =]==], str_entity_id, " and o.id = ott.order_id");
	}
	else
	{
		local strTmp = format(" products_by_entity_view where entity_id =%d ", entity_id);
		local strTmp2 = " (price * quantity) ";
		local strTmp3 = " code ";

		mf.write([==[
select product_id as 'product_id|ID|8|R', description as "description|Product|-1",
sum(quantity) as 'quantity|Qty.|9|R|N', sum(]==], strTmp2, [==[) as "amount|Amount|9|R|M",
]==], strTmp3, [==[ as "order_type_code|Type|6|C"
from ]==], strTmp, [==[
group by product_id, description, ]==], strTmp3);
	}

	switch(history_type){
		case 1: mf.write(" order by order_date desc, o.id desc "); break;
		case 2: mf.write(" order by total desc "); break;
		case 3: mf.write(" order by description "); break;
		case 4: mf.write(" order by 4 desc "); break;
		case 5: mf.write(" order by 3 desc "); break;
	}

	mf.write(" limit ", query_limit.tointeger());
	//debug_print(tostring(mf), "\n");
	return mf.tostring();
}

local function product_get_bar_chart_statistics_sql_core(product_id, sab, speriode, speriode_count){
	return format([==[
select %s as qm, sum(quantity) as q, '%s' as sab 
from orders_lines as ol, orders as o
where ol.product_id = %d
	and o.id = ol.order_id
	and o.order_type_id in(select * from order_types_%s_payment)
	and o.order_date
group by qm]==], speriode, sab, product_id,  (sab == "S" ? "sell" : "buy"), speriode_count);
}

local function product_bar_chart_statistics_sql(product_id, sab, periode_count, periode_type)
{
	local mf = blob();
	local rc = get_sql_bar_chart_statistics_periodes(periode_count, periode_type);
	local speriode = rc[0], speriode_count = rc[1];

	if (sab == "S" || sab == "B"){
		mf.write(product_get_bar_chart_statistics_sql_core(product_id, sab, speriode, speriode_count));
	}
	else
	{
		mf.write("select qm, q, sab from (")
		mf.write(product_get_bar_chart_statistics_sql_core(product_id, "S", speriode, speriode_count))
		mf.write(" UNION ALL ")
		mf.write(product_get_bar_chart_statistics_sql_core(product_id, "B", speriode, speriode_count))
		mf.write(") as t ")
	}

	mf.write(" order by qm, sab")
	//debug_print(mf.tostring(), "\n")
	return mf.tostring();
}

local function entities_list_sql()
{
    return [==[
select id as 'id|ID|6|R', name as 'name|Name|-1',
	contact as 'contact|Contact|12',
	phone as 'phone|Phone|12' from entities order by name;]==];
}

local function enitity_get_bar_chart_statistics_sql_core(entity_d, sab, speriode, speriode_count){
	return format([==[
SELECT %s as qm,
	sum(subtotal - total_discount + total_sales_tax1 + total_sales_tax2) as q, '%s' as sab
FROM orders
WHERE entity_id = %d
AND order_type_id IN( SELECT * FROM order_types_%s_payment)
AND order_date >= %s
GROUP BY qm
]==], speriode, sab, entity_d,  (sab == "S" ? "sell" : "buy"), speriode_count);
}

local function entity_bar_chart_statistics_sql(entity_d, sab, periode_count, periode_type){
	local mf = blob();
	local rc = get_sql_bar_chart_statistics_periodes(periode_count, periode_type);
	local speriode = rc[0], speriode_count = rc[1];

	if (sab == "S" || sab == "B"){
		mf.write(enitity_get_bar_chart_statistics_sql_core(entity_d, sab, speriode, speriode_count));
	}
	else
	{
		mf.write("select qm, q, sab from (")
		mf.write(enitity_get_bar_chart_statistics_sql_core(entity_d, "S", speriode, speriode_count))
		mf.write(" UNION ALL ")
		mf.write(enitity_get_bar_chart_statistics_sql_core(entity_d, "B", speriode, speriode_count))
		mf.write(") as t ")
	}

	mf.write(" order by qm, sab")
	//debug_print(mf.tostring(), "\n")
	return mf.tostring();
}

local function entities_sql_search_list(qs_tbl : table, post_tbl : table){
	local so = get_search_options(post_tbl);
	checkQueryStringSAB(qs_tbl, so);
	local mf = blob();

	mf.write(" select e.id, e.name, e.contact, e.phone, e.is_active, e.image_id ");
	if (so.with_images) mf.write(", i.thumbnail, i.mime_type ");
	mf.write(" from entities as e ");
	if (so.with_images) mf.write(" left join images as i on e.image_id = i.id ");
	mf.write(" where ");

	if  ((!so.group_id) &&  (!so.product_id) && (! (so.search_str && (so.search_str.len() > 0)))){
		//special case where we get the latest entities that made any transaction
		mf.write(" e.id in(select distinct entity_id from orders as o ");
		if (so.sales || so.buys){
			mf.write(", order_types as ot where o.order_type_id = ot.id and ot.group_order =");
		}
		if (so.sales) mf.write("'S'");
		else if (so.buys) mf.write("'B'");

		mf.write(" order by o.id desc");
		if (so.query_limit != 0) mf.write(" limit ", so.query_limit);
		mf.write(") order by 2");
		//print(out_sql);
		return mf.tostring();
	}

	if (so.mdate ) mf.write(" e.mdate is not null order by mdate desc ");
	else if (so.cdate) mf.write(" 1=1 order by e.id desc ");

	if (so.sales && so.buys) mf.write(" 1=1 ");
	else if (so.sales || so.buys){
		if (so.sales) mf.write(" show_on_sales = 1 ");
		else if (so.buys) mf.write(" show_on_buys = 1 ");
	}
	else mf.write(" 1=1 ");

	if (so.group_id && so.group_id != 0){
		//mf.write(" and id in( select entity_id from entity_groups_link where group_id="
		mf.write(" and e.group_id=", so.group_id);
	}

	if (so.active) mf.write(" and e.is_active=1 ");

	local search_str = escape_sql_like_search_str(so.search_str);
	if (so.product_id && so.product_id != 0){
		mf.write(" and e.id in(select entity_id from orders_lines as ol, orders as o2 ");

		if (so.sales || so.buys){
			mf.write(" join order_types as ot2 on o2.order_type_id = ot2.id and ot2.group_order = ");
			if (so.sales) mf.write(" 'S' ");
			else if (so.buys) mf.write(" 'B' ");
		}

		mf.write("where ol.product_id = ",  so.product_id, " and ol.order_id = o2.id order by ol.id desc ");
		if (so.query_limit != 0) mf.write(" limit ", so.query_limit);
		mf.write(")");
	}
	else if (search_str && search_str.len() > 0){
		if (so.id && so.id != 0) mf.write(" and e.id = ", so.search_str.tointeger());
		else
		{
			if (so.products){
				mf.write(" and e.id in ( select entity_id from orders where id in (",
					" select order_id from orders_lines where ",
					" description like "", search_str, "" order by id desc )) ");
			}
			else
			{
				mf.write(" and ");
				if (so.contact) mf.write(" e.contact ");
				else if (so.notes) mf.write(" e.notes ");
				else if (so.phone) mf.write(" e.phone ");
				else mf.write(" e.name ");
				mf.write(" like '", search_str, "' ");
			}
		}
	}

	mf.write(" order by e.name ");

	if (so.query_limit && so.query_limit != 0) mf.write(" limit ", so.query_limit);
	//debug_print(tostring(mf), "\n")
	return  mf.tostring();
}

local function orders_sql_search_list(qs_tbl, post_tbl){
	//foreach(k,v in post_tbl) debug_print("\n", k, ":", v)
	local so = get_search_options(post_tbl);
	checkQueryStringSAB(qs_tbl, so);
	local mf = blob();

	mf.write("SELECT o.id, o.order_date, ot.code, o.series, o.order_number,",
		//" o.entity_name, get_order_total(o.id) as total_amt, pt.code "
		" o.entity_name, ",
		" (subtotal + total_sales_tax1 + total_sales_tax2) as total_amt, ",
		" pt.code ",
		" FROM ((orders AS o LEFT JOIN payment_types as pt ON o.payment_type_id = pt.id) ",
		" JOIN  order_types as ot ON o.order_type_id = ot.id) WHERE 1=1 ");

	if (so.sales || so.buys) {
		if (so.sales) mf.write(" and ot.group_order = 'S' ");
		else if (so.buys) mf.write(" and ot.group_order = 'B' ");
	}

	if (so.payment_type_id && so.payment_type_id != 0)
		mf.write(" and o.payment_type_id = " , so.payment_type_id);

	if (so.group_id && so.group_id != 0)
		mf.write(" and o.order_type_id = " , so.group_id , " ");

	local search_str = escape_sql_like_search_str(so.search_str);
	if (so.entity_id && so.entity_id != 0) mf.write(" and o.entity_id =" , so.entity_id);
	else
	{
		if (so.product_id && so.product_id != 0){
			mf.write(" and o.id in( select order_id from orders_lines as ol ");

			if (so.sales || so.buys){
				mf.write(", (orders as o join order_types as ot on o.order_type_id = ot.id and ot.group_order = ");
				if (so.sales) mf.write(" 'S' )");
				else if (so.buys) mf.write(" 'B' )");
			}

			mf.write("where ol.product_id =", so.product_id);
			if (so.sales || so.buys) mf.write(" and ol.order_id = o.id ");
			mf.write(" order by ol.id desc ");
			if (so.query_limit != 0) mf.write(" limit " , so.query_limit);
			mf.write(")")
		}
		else if (search_str && search_str.len() > 0) {
			if (so.products){
				mf.write(" and o.id in ( select order_id from orders_lines where ",
					" description like "" , search_str , "" order by id desc limit " , so.query_limit , ")");
			}
			else mf.write(" and ");

			if (so.notes) mf.write(" o.notes ");
			else if (so.date) mf.write(" o.order_date ");
			//lo_search_str = escapeSqlLikeSearchStr(dbDate(search_str))
			//table.insert(sql, " and order_date = (("")
			else if (so.total) mf.write(" o.total_amt ");
			else  mf.write(" o.entity_name ");

			mf.write(" like '" , search_str , "' ");
		}
	}

	if (so.order_by_modification) mf.write(" and o.mdate is not null order by o.mdate desc ");
	else if (so.order_by_creation) mf.write(" order by o.id desc ");
	else mf.write(" order by o.order_date desc, o.id desc ");

	if (so.query_limit && so.query_limit != 0) mf.write(" limit " , so.query_limit);
	//debug_print(tostring(mf), "\n")
	return  mf.tostring();
}

local function products_sql_search_list(qs_tbl, post_tbl){
	local so = get_search_options(post_tbl);
	checkQueryStringSAB(qs_tbl, so);
	local mf = blob();

	mf.write([==[
select p.id,
p.reference_code,
p.sell_description,
ifnull(p.kit,  0)  as kit,
	round((p.sell_price * (1 + (st.rate1/100.0))),p.price_decimals) as price_taxed,
	round(p.sell_price, p.price_decimals) as price,
round(case when p.kit > 0 then p.kit_onhand else p.quantity_onhand end, 4) as quantity_onhand,
p.is_active, price_date,
p.image_id
]==])
	if (so.with_images) mf.write(", thumbnail, mime_type ");
	mf.write(" from (products_onhand as p left join sales_tax_rates as st on p.sales_tax_id = st.id) ");
	if (so.with_images) mf.write(" left join images as i on p.image_id = i.id ");
	mf.write(" where ");

	if ( (!so.group_id) && (!so.entity_id) && (! (so.search_str && (so.search_str.len() > 0)))){
		//special case where we get the latest entities that made any transaction
		mf.write(" p.id in(select distinct product_id from orders_lines where order_id in (select o.id from orders as o ");

		if (so.sales || so.buys){
			mf.write(", order_types as ot where o.order_type_id = ot.id and ot.group_order =");
			if (so.sales) mf.write("'S'");
			else if (so.buys) mf.write("'B'");
		}

		mf.write(" order by o.id desc");
		if (so.query_limit && so.query_limit != 0) mf.write(" limit " , so.query_limit);
		mf.write(")) order by 3");
		return mf.tostring();
	}

	if (so.sales && so.buys) mf.write(" 1=1 ");
	else if (so.sales || so.buys){
		if (so.sales) mf.write(" show_on_sales = 1 ");
		else if (so.buys) mf.write(" show_on_buys = 1 ");
	}
	else mf.write(" 1=1 ");

	if (so.group_id && so.group_id != 0)
		mf.write(" and p.group_id=" , so.group_id , " ");

	if (so.active) mf.write(" and p.is_active = 1 ");

	if (so.only_prices_older){
		//local adate = parse_date(optNum)
		//mf.write(" and p.price_update_date < '" ,
		//string.format("%d-%.02d-%.02d", adate.year, adate.month, adate.day) + "' ");
	}

	local search_str = escape_sql_like_search_str(so.search_str);

	if (so.entity_id && so.entity_id != 0){
		mf.write([==[
and p.id in(
select product_id from orders_lines where order_id in (
select id from orders where entity_id = ]==] , so.entity_id, " order by id desc)) ");
	}
	else if (search_str && search_str.len() > 0){
		if (so.id && so.id != 0) mf.write(" and p.id = " , so.search_str.tointeger());
		else
		{
			if (so.entities){
				mf.write(" and p.id in(",
					"select product_id from orders_lines where order_id in (",
					"select id from orders where entity_id in (",
					"select id from entities where name like "" , search_str, "" ",
					"or company like "" , search_str, "" order by id desc ))) ");
			}
			else mf.write(" and ");
			if (so.notes){
				if (so.buys) mf.write(" buy_notes ");
				else mf.write(" sell_notes ");
			}
			else if (so.reference) mf.write(" reference_code ");
			else mf.write(" p.sell_description ");

			mf.write(" like '" , search_str , "' ");
		}
	}

	if (so.order_by_modification) mf.write(" p.mdate is not null order by p.mdate desc ");
	else if (so.order_by_creation) mf.write(" 1=1 order by id desc ");
	else mf.write(" order by 3 ");

	if (so.query_limit && so.query_limit != 0) mf.write(" limit " , so.query_limit);
	//debug_print("\n", mf.tostring(), "\n")
	return  mf.tostring();
}

local db_ourbiz_tables = {};

//
// entities
//

local DB_Entities = class extends DB_Manager {
	constructor(){
		base.constructor(C_entities, [
		C_address, C_city, C_company, C_contact,
		C_country, C_email, C_fax, C_gps_coordinates, C_group_id,
		C_image_id, C_is_active, C_name, C_notes, C_parent_id, C_phone,
		C_sales_tax_exempt, C_show_on_buys, C_show_on_sales, C_state,
		C_tags, C_tax_number, C_use_sales_tax2, C_user_code, C_web, C_zip,
		C_irpf_pct_retention]);
	}

	function sql_list(qs_tbl, post_tbl){
		local entity_id;
		if (qs_tbl.get("search", false)) return entities_sql_search_list(qs_tbl, post_tbl);
		else if (qs_tbl.get("past_products", false)) return entity_past_products_get_sql(qs_tbl.past_products.tointeger());
		else if ( (entity_id = qs_tbl.get("history", 0)) ){
			local htype = qs_tbl.get("htype", 0);
			local query_limit = qs_tbl.get(C_query_limit, 50).tointeger();
			return entity_sales_history_sql(htype, query_limit, entity_id.tointeger());
		}
		else if ( (entity_id = qs_tbl.get("statistics", 0)) ){
			local periode_count = qs_tbl.get(C_periode_count, 12).tointeger();
			local periode_type = getStatisticsPeriodeType(qs_tbl.get(C_periode_type, C_months));
			local sab = qs_tbl.get("sab", "S");
			return entity_bar_chart_statistics_sql(entity_id.tointeger(), sab, periode_count, periode_type);
		}
		else if (qs_tbl.get("print_list", false)){
			return entities_list_sql();
		}
		else if (qs_tbl.get("pdf", false)){
			qs_tbl._doc_pdf_ <- get_pdf_list(qs_tbl);
			return true;
		}
	}
	
	function get_pdf_list(tbl_qs){
		local clipped = tbl_qs.get("clipped", false);
		local pdf = new PdfSqlTable();
		pdf.page_title = "Entities List";
		pdf.water_mark = "T H I S   I S   A   D E M O";

		local sql = [==[
select id as 'id|ID|6|R', name as 'name|Name|-1',
	contact as 'contact|Contact|12',
	phone as 'phone|Phone|12'
from entities
order by name
]==]
		local db = getOurbizDB();
		local mf = blob(0, 8192);
		pdf.GetPDF(db, sql, clipped, mf);
		return mf;
	}

	function sql_get_one(tbl_qs){
		local id = tbl_qs.get(table_name, 0).tointeger();
		if(tbl_qs.get("for_order", false))
		{
			return entity_for_order_get_one(id);
		}
		else if(tbl_qs.get("with_thumbnail", false))
		{
			return [==[
select e.*, i.thumbnail, i.mime_type
from entities e left join images i
on e.image_id = i.id where e.id=?]==];
		}
		else if(tbl_qs.get("pdf", false))
		{
			return get_pdf_list(tbl_qs);
		}
		else
		{
			return base.sql_get_one(tbl_qs);
		}
	}
	
	function db_action(db, data){
		local result = base.db_action(db, data);
		//save_image(db, data, _table_name);
		return result;
	}
}
db_ourbiz_tables.entities <- new DB_Entities();


//
// orders
//

if(globals.get("AT_DEV_DBG", false) || !globals.get("PDF_Order", false)) {
	dofile(APP_CODE_FOLDER + "/pdf-order.nut");
}

local My_PDF_Order = class extends PDF_Order {
	_tr_map = null;
	_lang = null;
	_tr_stmt = null;
	my_logo_image = null;

	constructor(db, lang){
		base.constructor();
		_lang = lang;
		_tr_map = {};
		my_logo_image = false;
		_tr_stmt = db.prepare([==[
select svalue from translations_values as tv, translations_keys as tk, languages as l
where l.lang=? and tk.skey=? and tv.lang_id=l.id and tv.key_id=tk.id]==]);
	}

	function do_init(){
		local map = {};
		base.do_init();
		/*
		map[CFG_ORDER_COMPANY_NAME] <- "OurBiz";
		map[CFG_ORDER_COMPANY_INFO] <- "Our buziness on our pocket.";
		map[CFG_ORDER_BOTTOM_INFO] <- "Thanks for choose our products and services.";
		configGetValues(*_tr_stmt.get_db(), map);
		page_title = map[CFG_ORDER_COMPANY_NAME];
		page_title2 = map[CFG_ORDER_COMPANY_INFO];
		page_footer = map[CFG_ORDER_BOTTOM_INFO];
		*/
		page_title = "OurBiz";
		page_title2 =  "Our buziness on our pocket.";
		page_footer = "Thanks for choose our products and services.";
	}

	function _tr(str)
	{
/*
		local tr_str = _tr_map.get(str, null);
		if(tr_str) return tr_str;

		_tr_stmt.reset();
		_tr_stmt.bind(1, _lang);
		_tr_stmt.bind(2, str);
		if(_tr_stmt.next_row()) tr_str = _tr_stmt.col(0);
		_tr_map[str] = tr_str || str;
*/
		return str;
	}

	function LogoImage(px, py, pwidth){
		local db = _tr_stmt.get_db();
		local stmt = db.prepare("select image, mime_type from images where name = 'order_logo'");
		if(stmt.next_row())
		{
			my_logo_image = stmt.col(0);
			local mime_type = stmt.col(1);
			local mtype = "unknown";
			if(mime_type == "image/jpeg") mtype = "jpg";
			else if(mime_type == "image/png") mtype = "png";
			Image("order_logo", my_logo_image, my_logo_image.size(), px, py, pwidth, 0, mtype);
		}
	}
}

local CalcOrderLine = class
{
	id = null;
	order_id = null;
	price_decimals = null;
	product_id = null;
	quantity = null;
	weight = null;
	unit_weight = null;
	price = null;
	first_total = null;
	discount_pct = null;
	discount_amt = null;
	line_subtotal = null;
	sales_tax1_pct = null;
	sales_tax1_amt = null;
	sales_tax2_pct = null;
	sales_tax2_amt = null;
	line_total = null;
	use_sales_tax2 = null;
	tax_exempt = null;

	constructor(){
		reset();
	}

	//reset is not the same as clear
	//reset puts instance variables as when created
	//and clear preserves tax flags
	function reset(){
		use_sales_tax2 = tax_exempt = false;
		clear_preserving_tax_flags();
	}

	function calc_map(map){
		local fields = [
			C_unit_weight, C_quantity, C_weight, C_price,
			C_first_total, C_discount_pct, C_discount_amt,
			C_line_subtotal, C_sales_tax1_pct, C_sales_tax1_amt,
			C_sales_tax2_pct, C_sales_tax2_amt, C_line_total
			];
		use_sales_tax2 = map.get(C_use_sales_tax2, false) == "1";
		tax_exempt = map.get(C_tax_exempt, false) == "1";
		price_decimals = map.get(C_price_decimals, 2).tointeger();

		foreach(field in fields){
			this[field] = mytofloat(map.get(field, 0.0));
		}

		calc();

		foreach(field in fields){
			map[field] <- this[field];
		}
	}

	function clear_preserving_tax_flags(withFullTax=false)
	{
		unit_weight = quantity = weight = price =
			first_total = discount_pct =
			discount_amt = line_subtotal =
			sales_tax1_pct = sales_tax1_amt = sales_tax2_pct =
			sales_tax2_amt = line_total = 0.0;
		id = order_id = product_id = 0;
		price_decimals = 2;
		if(withFullTax){
			use_sales_tax2 = true;
			tax_exempt=false;
		}
	}

	function clear_preserving_tax_flags2(withFullTax=false)
	{
		unit_weight = quantity = weight = price =
			first_total = discount_pct =
			discount_amt = line_subtotal =
			sales_tax1_pct = sales_tax1_amt = sales_tax2_pct =
			sales_tax2_amt = line_total = Decimal(0);
		id = order_id = product_id = 0;
		price_decimals = 2;
		if(withFullTax){
			use_sales_tax2 = true;
			tax_exempt=false;
		}
	}

	function dumpSLSArrayCalcNames(out_result)
	{
		//this list must match the dumpSLSArrayCalc order
		add2sle(out_result, C_first_total);
		add2sle(out_result, C_discount_amt);
		add2sle(out_result, C_line_subtotal);
		add2sle(out_result, C_sales_tax1_amt);
		add2sle(out_result, C_sales_tax2_amt);
		add2sle(out_result, C_line_total);
	}

	function dumpSLSArrayCalc(out_result)
	{
		//this must match the dumpSLSArrayCalcNames order
		add2sle(out_result, first_total);
		add2sle(out_result, discount_amt);
		add2sle(out_result, line_subtotal);
		add2sle(out_result, sales_tax1_amt);
		add2sle(out_result, sales_tax2_amt);
		add2sle(out_result, line_total);
	}

	function calc()
	{
		local num100 = 100;
		first_total = quantity * price;
		weight = unit_weight * quantity;
		local round = math.broundf;

		discount_amt = first_total * (discount_pct/num100);
		line_subtotal = round(first_total - discount_amt, price_decimals);

		if(!tax_exempt)
		{
			sales_tax1_amt = round(line_subtotal * (sales_tax1_pct/num100), price_decimals);
			if(use_sales_tax2)
			{
				sales_tax2_amt = round(line_subtotal * (sales_tax2_pct/num100), price_decimals);
			}
			else
			{
				sales_tax2_pct = 0.0;
			}
		}
		else
		{
			sales_tax2_pct = sales_tax1_pct = 0.0;
		}
		line_total = round(line_subtotal + sales_tax1_amt + sales_tax2_amt, price_decimals);
	}

	function calc2()
	{
		local num100 = Decimal(100);
		first_total = quantity * price;
		weight = unit_weight * quantity;
		local round = math.broundf;
		local saved_prec = Decimal.gctx.prec();
		Decimal.gctx.prec(price_decimals);

		discount_amt = first_total * (discount_pct/num100);
		line_subtotal = first_total - discount_amt;

		if(!tax_exempt)
		{
			sales_tax1_amt = line_subtotal * (sales_tax1_pct/num100);
			if(use_sales_tax2)
			{
				sales_tax2_amt = line_subtotal * (sales_tax2_pct/num100);
			}
			else
			{
				sales_tax2_pct = 0.0;
			}
		}
		else
		{
			sales_tax2_pct = sales_tax1_pct = 0.0;
		}
		line_total = line_subtotal + sales_tax1_amt + sales_tax2_amt;
		Decimal.gctx.prec(saved_prec);
	}
}

local  CalcOrderTotals = class
{
	_stmt_calc_order = null;
	_stmt_get_tax_usage = null;

	subtotal_amt = null;
	discount_amt = null;
	sales_tax1_amt = null;
	sales_tax2_amt = null;
	total_amt = null;
	weight_total = null;
	lines_count = null;

	constructor()
	{
		clear();
		_stmt_calc_order = _stmt_get_tax_usage = false;
	}
	function clear()
	{
		subtotal_amt = discount_amt = sales_tax1_amt =
			sales_tax2_amt = total_amt = weight_total = 0.0;
		lines_count = 0;
	}
	function clear2()
	{
		subtotal_amt = discount_amt = sales_tax1_amt =
			sales_tax2_amt = total_amt = weight_total = Decimal(0);
		lines_count = 0;
	}

	function dumpSLEFieldNames(out_result)
	{
		add2sle(out_result, C_lines_count);
		add2sle(out_result, C_weight_total);
		add2sle(out_result, C_subtotal_amt);
		add2sle(out_result, C_discount_amt);
		add2sle(out_result, C_sales_tax1_amt);
		add2sle(out_result, C_sales_tax2_amt);
		add2sle(out_result, C_total_amt);
	}

	function dumpSLEFieldValues(out_result)
	{
		add2sle(out_result, lines_count);
		add2sle(out_result, weight_total);
		add2sle(out_result, subtotal_amt);
		add2sle(out_result, discount_amt);
		add2sle(out_result, sales_tax1_amt);
		add2sle(out_result, sales_tax2_amt);
		add2sle(out_result, total_amt);
	}

	function dumpSLEArray(out_result)
	{
		out_result.write("[[");
		dumpSLEFieldNames(out_result);
		out_result.writen(SLE_SLEEND, 'c');
		out_result.write("][");
		dumpSLEFieldValues(out_result);
		out_result.writen(SLE_SLEEND, 'c');
		out_result.write("]");
	}

	function create_stmt_calc_order(db){
		if(!_stmt_get_tax_usage)
		{
			_stmt_get_tax_usage = db.prepare([==[
select entity_sales_tax_exempt, entity_use_sales_tax2
from orders where id=?]==]);
			_stmt_calc_order = db.prepare([==[
select quantity, weight, price, price_decimals, discount_pct,
sales_tax1_pct, sales_tax2_pct, product_id, description
from orders_lines where order_id=?]==]);
		}
	}

	function calc_one_line(calc_line){
		calc_line.clear_preserving_tax_flags();
		calc_line.quantity = _stmt_calc_order.col(0);
		calc_line.weight = _stmt_calc_order.col(1);
		calc_line.price = _stmt_calc_order.col(2);
		calc_line.price_decimals = _stmt_calc_order.col(3);
		calc_line.discount_pct = _stmt_calc_order.col(4);
		calc_line.sales_tax1_pct = _stmt_calc_order.col(5);
		calc_line.sales_tax2_pct = _stmt_calc_order.col(6);

		//collect weight before calc, after calc it will have an incorrect value
		//because we are not suppling unit_weight
		weight_total += calc_line.weight;

		calc_line.calc();

		++lines_count;
		subtotal_amt += calc_line.line_subtotal;
		discount_amt += calc_line.discount_amt;
		sales_tax1_amt += calc_line.sales_tax1_amt;
		sales_tax2_amt += calc_line.sales_tax2_amt;
		total_amt += calc_line.line_total;
	}

	function number_format_trim(num){
		local snum = math.number_format(num);
		return snum;
	}

	function calc_order_totals(db, order_id, calc_line, vec_lines=null)
	{
		clear(); //reset totals
		create_stmt_calc_order(db);
		local number_format = math.number_format;

		_stmt_get_tax_usage.reset();
		_stmt_get_tax_usage.bind(1, order_id);
		if(!_stmt_get_tax_usage.next_row()) return;

		calc_line.tax_exempt = _stmt_get_tax_usage.col(0) == 1;
		calc_line.use_sales_tax2 = _stmt_get_tax_usage.col(1) == 1;
		_stmt_get_tax_usage.reset();

		_stmt_calc_order.reset();
		_stmt_calc_order.bind(1, order_id);
		while(_stmt_calc_order.next_row())
		{
			calc_one_line(calc_line);
			if(vec_lines)
			{
				local vec_str = [];
				vec_str.push(_stmt_calc_order.col(7));
				vec_str.push(_stmt_calc_order.col(8));
				vec_str.push(number_format_trim(calc_line.quantity));
				vec_str.push(number_format(calc_line.price, calc_line.price_decimals));
				vec_str.push(number_format_trim(calc_line.discount_pct));
				vec_str.push(number_format_trim(calc_line.sales_tax1_pct));
				vec_str.push(number_format_trim(calc_line.sales_tax2_pct));
				vec_str.push(number_format(calc_line.first_total, calc_line.price_decimals));
				vec_lines->push(vec_str);
			}
		}
		//sqlite3_reset(_stmt_calc_order);
	}

	function getPdfOrder(db, id, calc_line, lang, logoImg=null)
	{
		local str;
		local  pdf_order = new My_PDF_Order(db, lang);
		pdf_order.do_init();
		if(logoImg) pdf_order.logoFileName = logoImg;
		pdf_order.water_mark = "T H I S   I S   A   D E M O";
		local stmt = db.prepare("select * from orders where id=?");
		stmt.bind(1, id);
		if(stmt.next_row())
		{
			local order_type_id = stmt.col(C_order_type_id);
			pdf_order.labelNumber = db.exec_get_one(
				format("select description_to_print from order_types where id=%d",
					order_type_id));
			local order_number = stmt.col(C_order_number);
			pdf_order.strNumber = stmt.col(C_series);
			pdf_order.strNumber += "/";
			pdf_order.strNumber += order_number;
			pdf_order.strDate = stmt.col(C_order_date);
			pdf_order.strEntity.push(stmt.col(C_entity_name));
			pdf_order.strEntity.push(stmt.col(C_entity_address));
			pdf_order.strEntity.push(stmt.col(C_entity_phone));
			pdf_order.strEntity.push(stmt.col(C_entity_tax_number));
		}
		calc_order_totals(db, id, calc_line, pdf_order.strLines);
		pdf_order.strTotals.push(math.number_format(subtotal_amt));
		pdf_order.strTotals.push(math.number_format(sales_tax1_amt));
		pdf_order.strTotals.push(math.number_format(sales_tax2_amt));
		pdf_order.strTotals.push(math.number_format(total_amt));

		pdf_order.orderTable(31);
		//pdf_order.SetCompression(false);
		return pdf_order.Output("doc-order.pdf", 'S');
	}
}

local MyCalcOrderTotals = class
{
	order_totals = null;
	calc_line = null;
	db = null;

	constructor(adb)
	{
		db = adb;
		calc_line = new CalcOrderLine();
		order_totals = new CalcOrderTotals();
	}

	function calc_total(id){
		order_totals.calc_order_totals(db, id, calc_line);
		return order_totals.total_amt;
	}
	
	function getPdfOrder(id, lang){
		return order_totals.getPdfOrder(db, id, calc_line, lang);
	}
}

class DBTableUpdateOrderLine extends DB_Manager
{
	order_id : integer;

	constructor(){
		base.constructor(C_orders_lines, [
			C_order_id, C_xref_order_line_id, C_batch_order_line_id,
			C_product_id, C_quantity, C_weight, C_price, C_price_decimals, 
			C_sales_tax1_pct, C_sales_tax2_pct, C_discount_pct,
			C_description, C_notes, C_cdate]);
		has_version = false;
		has_mdate = false;
		order_id = 0;
	}
	
	function getWhereClause(){
		return " where id=? and order_id=? ";
	}

	function bindWhereClause(stmt, lastParam)
	{
		stmt.bind(++lastParam, edit_id);
		stmt.bind(++lastParam, order_id);
		return lastParam;
	}
};

local DB_Orders = class extends DB_Manager {
	_calc_line = null;
	_order_totals = null;
	_stmt_update_version = null;
	_db_line = null;
	
	constructor(){
		base.constructor(C_orders, [
			C_order_type_id, C_series, C_order_number, C_entity_order_number,
			C_entity_id, C_entity_name, C_entity_address,
			C_entity_phone, C_entity_zip, C_entity_city,
			C_entity_state, C_entity_country, C_entity_tax_number,
			C_entity_use_sales_tax2, C_entity_sales_tax_exempt,
			C_order_date, C_payment_type_id, C_notes,
			C_cash, C_tags, C_order_valid_till_date,
			C_irpf_pct_retention,
		]);
		_calc_line = new CalcOrderLine();
		_order_totals = CalcOrderTotals();
		
		_db_line = DBTableUpdateOrderLine();
	}

	function get_bar_chart_statistics_sql_core(mf, sab, speriode, speriode_count, strPU1, strPU2){
		mf.write("SELECT ", speriode, " as qm, sum(subtotal + total_sales_tax1 + total_sales_tax2) as q, '",
			sab, "' as sab ", strPU1, " FROM orders WHERE order_type_id IN(SELECT * FROM order_types_",
			(sab == "S" ? "sell" : "buy"), "_payment) AND order_date >= ", speriode_count, " GROUP BY qm ",
			strPU2);
	}

	function sql_bar_chart_statistics(sab, periode_count=12, periode_type=TimePeriode.is_months, paidUnpaid=false)
	{
		local strPU1, strPU2;
		local mf = blob();
		if(paidUnpaid)
		{
			strPU1 = ", payment_type_id = 1 as pt ";
			strPU2 = ", pt ";
		}
		else strPU1 = strPU2 = "";

		local rc = get_sql_bar_chart_statistics_periodes(periode_count, periode_type);
		local speriode = rc[0],  speriode_count = rc[1];

		if(sab == "S" || sab == "B")
			get_bar_chart_statistics_sql_core(mf, sab, speriode, speriode_count, strPU1, strPU2);
		else {
			mf.write("select qm, q, sab ");
			if(paidUnpaid) mf.write(strPU2);
			mf.write(" from (");
			get_bar_chart_statistics_sql_core(mf, "S", speriode, speriode_count, strPU1, strPU2);
			mf.write(" UNION ALL ");
			get_bar_chart_statistics_sql_core(mf, "B", speriode, speriode_count, strPU1, strPU2);
			mf.write(") as t ");
		}

		mf.write(" order by qm, sab");
		if(paidUnpaid) mf.write(strPU2);

		//printf("%d : %d\n%s\n", periode, periode_count, out_sql.cstr());
		return mf.tostring();
	}

	function sql_list(qs_tbl, post_tbl){
		local order_id
		if (qs_tbl.get("search", false)) return orders_sql_search_list(qs_tbl, post_tbl);
		else if( (order_id = qs_tbl.get("lines", 0)) )
		{
			return orders_lines_sql_for_order(order_id.tointeger());
		}
		else if(  (order_id = qs_tbl.get("sum", 0)) )
		{
			local query_limit = qs_tbl.get("query_limit", 50);
			return orders_sum_search_sql(query_limit);
		}
		else if( (order_id = qs_tbl.get("lines_onhand", 0)) )
		{
			return order_lines_onhand_get_sql(order_id.tointeger());
		}		
		else if (qs_tbl.get("history", false)){
			local htype = qs_tbl.get("htype", 0).tointeger();
			local query_limit = qs_tbl.get("query_limit", 50).tointeger();
			return orders_sales_history_sql(htype, query_limit, qs_tbl.history);
		}
		else if (qs_tbl.get("statistics", false)){
			local periode_count = qs_tbl.get(C_periode_count, 12).tointeger();
			local periode_type = getStatisticsPeriodeType(qs_tbl.get("periode_type", "months"));
			local sab = qs_tbl.get("sab", "S");
			return sql_bar_chart_statistics(sab, periode_count, periode_type);
		}
		else if (qs_tbl.get("print_list", false)){
		}
	}

	function calc_order_line_from_map(tbl_qs, rec_map)
	{
		_calc_line.reset();
		local product_id = tbl_qs.get(C_product_id, 0).tointeger();
		local order_id = tbl_qs.get("__id__", 0).tointeger();
		if(product_id  && order_id)
		{
			local db = getOurbizDB();
			local stmt;
			local str =  tbl_qs.get(C_trigger, false);
			//printf("%s\n", str.c_str());
			if(str == C_quantity)
			{
				local discount, quantity;
				stmt = db.prepare(format([==[
select group_order from order_types as ot, orders as o
where o.id=%d
and ot.id = o.order_type_id]==], order_id));
				if(stmt.next_row()){
					str = stmt.col(0);
					if(str != "B"){
						local quanity = rec_map.get(C_quantity, 0);
						if(quantity > 0)
						{
							out_buf.clear();
							stmt.prepare(discount_by_quantity_get_one(product_id, quantity));
							if(stmt.next_row())
							{
								discount = stmt.col(0);
								//special case when discount is negative
								//meaning an increase we don't show
								//it as discount but use it's calculated price
								if(discount <= 0.0)
								{
									discount = 0.0;
									rec_map.discount_pct <- "0";
									rec_map.price <- stmt.asString(1);
								}
								else
								{
									rec_map.discount_pct <- stmt.asString(0);
								}
							}
							else
							{
								rec_map.discount_pct <- "0";
							}
						}
					}
				}
				else throw("Could not get order type !");
			}
			//_calc_line.price_over_weight = ?;

			stmt = db.prepare(format([==[
select  o.entity_sales_tax_exempt, o.entity_use_sales_tax2,
	p.weight, p.sales_tax_id
from orders as o, products as p 
where o.id = %d and p.id = %d]==], order_id, product_id));
			local sales_tax_id = 0;
			if(stmt.next_row())
			{
				_calc_line.tax_exempt = stmt.col(0);
				_calc_line.use_sales_tax2 = stmt.col(1);
				//stmt.get(2, _calc_line.unit_weight);
				rec_map.unit_weight <- stmt.col(2);
				sales_tax_id = stmt.col(3);
			}
			else throw("Could not get sales tax info for product!");

			if(!_calc_line.tax_exempt)
			{
				stmt.prepare(format("select  rate1, rate2 from sales_tax_rates where id = %d", sales_tax_id));
				if(stmt.next_row())
				{
					rec_map.sales_tax1_pct <- stmt.col(0);
					if(_calc_line.use_sales_tax2)
						rec_map.sales_tax2_pct <- stmt.col(1);
				}
				else throw("Could not get sales tax info!");
			}
		}
		_calc_line.calc_map(rec_map);
	}

	function calc_order_line(tbl_qs, out_buf, rec_map)
	{
		calc_order_line_from_map(tbl_qs, rec_map);
		out_buf.clear();
		WriteMap2SLEArray(out_buf, rec_map);
	}
	
	function sql_get_one(tbl_qs) {
		local id = tbl_qs.get(table_name, 0).tointeger();
		if(tbl_qs.get("line", false))
		{
			orders_lines_get_one(id);
		}
		else if(tbl_qs.get("line_calculated", false))
		{
			local db = getOurbizDB();
			local stmt = db.prepare(orders_lines_get_one(id));
			if(!stmt.next_row()) return;
			local rec_map = stmt.asTable();
			rec_map.__id__ <- rec_map.order_id;
			local buf = blob(0, 8192);
			calc_order_line(tbl_qs, buf, rec_map);
			return buf;
		}
		else if (tbl_qs.get("with_lines", false)){
			local db = getOurbizDB();
			local buf = blob(0, 8192);

			//orders_get_one(buf, id);
			local stmt = db.prepare("select * from orders where id=?");
			stmt.bind(1, id);
			if(!stmt.next_row()) return;

			_order_totals.calc_order_totals(db, id, _calc_line);

			local col_count = stmt.col_count();
			buf.clear();
			buf.write("[[");
			WriteSLEArrayFieldNames(buf, stmt, col_count);
			_order_totals.dumpSLEFieldNames(buf);
			buf.writen(SLE_SLEEND, 'c');
			buf.write("][");
			WriteSLEArrayFieldValues(buf, stmt, col_count);
			_order_totals.dumpSLEFieldValues(buf);
			buf.writen(SLE_SLEEND, 'c');
			buf.write("]]");

			stmt.prepare(orders_lines_sql_for_order(id));
			buf.write(stmt.asSleArray());
			return buf;
		}
		else if (tbl_qs.get("pdf", false)){
			local db = getOurbizDB();
			local calc_order = new MyCalcOrderTotals(db);
			tbl_qs._doc_pdf_ <- calc_order.getPdfOrder(id, "en");
			return true;
		}
		else return base.sql_get_one(tbl_qs);
	}

	function db_action(db, data){
		//dbg_dump_map(p.post_map);
		local action = data.get("__action__", false);

		if(action == "calc_line" || action == "calc_order_line")
		{
			local buf = blob(0, 8192);
			calc_order_line(data, buf, data);
			return buf;
		}
		else if(action == "order_numbering")
		{
			local order_id = data.get("__id__", 0).tointeger();
			local version = data.get("__version__", 0).tointeger();
			db.exec_dml("begin;");
			try {
				if(db.exec_dml(orders_numbering_get_sql(order_id, version)) < 1)
				{
					throw("Could not number the order !");
				}
				local order_number = db.exec_get_one(format("select order_number from orders where id=%d", order_id));
				db.exec_dml("commit;");
				
				local buf = blob(0,1024);
				mkSLEArray(buf, ["order_number"], [order_number]);
				return buf;
			}
			catch(e){
				db.exec_dml("rollback;");
				throw(e);
			}
		}
		else if(action == "copy_order")
		{
			//copy_order(p, out_sql);
		}
		else
		{
			do_db_action2(db, data);
		}
	}

	function do_db_action2(db, data){
/*
		local action = data.get("__action__", false);
		local order_id = data.get("__id__", 0).tointeger();
		local version = data.get("__version__", 0).tointeger();
		
		local line_id, entity_id, order_type_id, changes = 0;
		map_str_t order_type_map;
		order_values_st order_values;
		bool order_type_changed = false;

		if(!_stmt_update_version)
		{
			_stmt_update_version = db.prepare([==[
update orders set _version_=_version_+?, mdate=CURRENT_TIMESTAMP,
	subtotal=?, total_discount=?, total_sales_tax1=?, total_sales_tax2=?
where id=? and _version_=?]==]);
		}

		db.exec_dml("begin;");
		try {
			_db_line.dbAction = DBTableUpdate::e_none;
			if(action == "insert_line") _db_line.dbAction = DBTableUpdate::e_insert;
			else if(action == "update_line") _db_line.dbAction = DBTableUpdate::e_update;
			else if(action == "delete_line") _db_line.dbAction = DBTableUpdate::e_delete;

			bool isInsert, isDelete, isUpdate;
			isInsert = isDelete = isUpdate = false;
			if(action == C_insert) isInsert = true;
			else if(action == C_update) isUpdate = true;
			else if(action == C_delete) isDelete = true;

			if(_db_line.dbAction != DBTableUpdate::e_none){
			getVarFromMap("__id__", p.post_map, order_id);
			if(!order_id) throw DBException("Order ID required for line actions!");

			_db_line._order_id = order_id;
			_db_line._db = &p.db;

			getVarFromMap(C_id, p.post_map, line_id);

			std::string str;
			if(getVarFromMap("calc_on_fld", p.post_map, str))
			{
			p.post_map[C_trigger] = str;
			calc_order_line_from_map(p, out_sql, p.post_map);
			out_sql.clear();
			}

			bool hasOld = false;

			switch(_db_line.dbAction)
			{
			case DBTableUpdate::e_delete:
			case DBTableUpdate::e_update:
			hasOld = true;
			}

			if(hasOld) {
			if(!line_id)  throw DBException("Line ID required for line actions!");
			p.post_map["__id__"] = asString(line_id);
			}

			get_order_type(p.db, order_id, order_type_map);

			getVarFromMap(C_entity_id, order_type_map, entity_id);
			order_values.setFromMap(order_type_map);
			apply_order_type_on_order_entity(p.db, entity_id, order_values, true, order_type_map);

			if(hasOld){
			apply_order_type_on_order_line(p.db, line_id, true, order_type_map);
			}
			out_sql.clear();
			db_action_exec(_db_line, p, get_line_fields_for_dbaction(), out_sql);
			changes = p.db.changes();

			switch(_db_line.dbAction)
			{
			case DBTableUpdate::e_update:
			case DBTableUpdate::e_insert:
			apply_order_type_on_order_line(p.db, _db_line.edit_id, false, order_type_map);
			}
			}
			else
			{
			order_id = 0;
			getVarFromMap("__id__", p.post_map, order_id);
			getVarFromMap("order_type_id", p.post_map, order_type_id);

			if(isUpdate || isDelete){//check change on order type
			int old_order_type_id;
			int old_entity_id;

			get_order_type(p.db, order_id, order_type_map);

			getVarFromMap("id", order_type_map, old_order_type_id);
			//when order_type is not changed it's not sent
			//so we adjust it here
			if(!order_type_id) order_type_id = old_order_type_id;

			getVarFromMap("entity_id", order_type_map, old_entity_id);
			order_values.setFromMap(order_type_map);
			apply_order_type_on_order_entity(p.db, old_entity_id, order_values, true, order_type_map);

			order_type_changed = !isDelete && old_order_type_id && (order_type_id != old_order_type_id);
			if(order_type_changed || isDelete){
			apply_order_type_on_order(p.db, order_id, true, order_type_map);
			}
			if(isDelete){
			p.db.exec_dml_fmt("delete from orders_lines where order_id=%d", order_id);
			}
			}

			dbHandler::db_action(p, out_sql);

			if(isInsert){
			sle2map(out_sql, order_type_map);
			getVarFromMap("id", order_type_map, order_id);
			}
			else if(isUpdate) version++; //update will increase version on call dbHandler::db_action
			}

			if(order_type_changed){
			get_order_type(p.db, order_id, order_type_map);
			apply_order_type_on_order(p.db, order_id, false, order_type_map);
			}

			if(!isDelete && !isInsert){
			_order_totals.calc_order_totals(p.db.get_db(), order_id, _calc_line);
			SQLiteResetStatement rstmt(_stmt_update_version);
			int i=1;
			//because when update order header already incerased version
			_stmt_update_version->bind(i++, isUpdate ? 0 : 1);
			_stmt_update_version->bind(i++, _order_totals.subtotal_amt);
			_stmt_update_version->bind(i++, _order_totals.discount_amt);
			_stmt_update_version->bind(i++, _order_totals.sales_tax1_amt);
			_stmt_update_version->bind(i++, _order_totals.sales_tax2_amt);
			_stmt_update_version->bind(i++, order_id);
			_stmt_update_version->bind(i++, version);

			if(_stmt_update_version->exec_dml() != 1)
			{
			throw DBException("Could not update order !");
			}
			order_values.subtotal = _order_totals.subtotal_amt;
			order_values.discount = _order_totals.discount_amt;
			order_values.tax1 = _order_totals.sales_tax1_amt;
			order_values.tax2 = _order_totals.sales_tax2_amt;
			}
			else order_values.setFromMap(order_type_map);

			getVarFromMap("entity_id", order_type_map, entity_id);
			if(!isDelete){
			apply_order_type_on_order_entity(p.db, entity_id, order_values,
						 false, order_type_map);
			}

			p.db.commit_transaction();

			if(_db_line.dbAction != DBTableUpdate::e_none){
			//send the totals with the new id and changes
			out_sql.clear();
			out_sql << "[[";
			_order_totals.dumpSLEFieldNames(out_sql);
			add2sle(out_sql, "id");
			add2sle(out_sql, "changes");
			out_sql.append((char)SLEEND);
			out_sql << "][";
			_order_totals.dumpSLEFieldValues(out_sql);
			add2sle(out_sql, _db_line.edit_id);
			add2sle(out_sql, changes);
			out_sql.append((char)SLEEND);
			out_sql << "]]";
			}
		}
		catch(e){
			db.exec_dml("rollback;");
			throw(e);
		}
*/
	}
}

db_ourbiz_tables.orders <-  new DB_Orders();

//
// products
//

local Product_Calc_price = class
{
	static kalc_fields = [C_buy_price, C_buy_discount, C_buy_other_costs, 
					C_sell_markup, C_markup_to_discount, C_sell_price,
					C_sell_price2, C_discount_over_sales, C_price_decimals];
	static num100 = 100.0;
	static num1 = 1.0;

	constructor()
	{
		clear();
	}

	function clear()
	{
		buy_price = buy_discount = buy_other_costs = sell_markup =
			markup_to_discount = sell_price = sell_price2 = discount_over_sales = 0.0;
		price_decimals = 2;
	}

	function _final_buy_price()
	{
		return (buy_price * (num1-(buy_discount/num100))) + buy_other_costs;
	}
	function _apply_markup(price)
	{
		return price * (num1+(sell_markup/num100));
	}
	function _mk_buy_price(from_price)
	{
		return (from_price - buy_other_costs) / (num1-(buy_discount/num100));
	}

	//save sales prices to allow recalculation again
	function calc_over_sales(based_on)
	{
		local sell_price_saved = sell_price;
		local result = calc(based_on);
		sell_price = sell_price_saved;
		return bresult;
	}

	function calc_over_sales_by_map(map)
	{
		//dbg_dump_map(map);
		local trigger = map.get(C_trigger, null);
		local result_fields = [C_sell_markup, C_discount_over_sales, C_sell_price2];
		foreach(k in result_fields) this[k] = map.get(k, 0.0);
		buy_price = map.get(C_buy_price, 0.0);
		buy_discount = map.get(C_buy_discount, 0.0);
		buy_other_costs = map.get(C_buy_other_costs, 0.0);
		sell_price = map.get(C_sell_price, 0.0);
		price_decimals = map.get(C_price_decimals, 2).tointeger();
		
		local result = calc_over_sales(trigger);
		foreach(k in result_fields) map[k] = this[k];

		return result;
	}

	function calc_by_map(map)
	{
		//dbg_dump_map(map);
		local trigger = map.get(C_trigger, C_sell_markup);
		local result_fields = [C_buy_price, C_buy_discount, C_buy_other_costs, C_sell_price, 
						C_sell_price2, C_sell_markup, C_markup_to_discount];
		price_decimals = map.get(C_price_decimals, 2).tointeger();
		
		foreach(k in result_fields) {
			//debug_print("\n", k, ":", map.get(k, 0.0));
			this[k] = mytofloat(map.get(k, 0.0));
		}
		local result = calc(trigger);
		foreach(k in result_fields) {
			//debug_print("\n", k, ":", this[k], ":", map.get(k, 0));
			map[k] <- this[k];
		}
		
		return result;
	}

	function calc(based_on)
	{
		local tmp;
		local result = true;

		if(based_on == C_markup_to_discount)
		{
			//%Markup = (1 - (100 /(100 - (%GM))))*-100
			tmp = num100 - markup_to_discount;
			if(tmp > 0) sell_markup = (num1-(num100/tmp))*-num100;
			else result = false;
		}
		else if(based_on == C_sell_markup)
		{
			//Gross Margin (%GM) = (1- (100 /(100 + %Markup)))*100
			tmp = num100 + sell_markup;
			if(tmp > 0) markup_to_discount = (num1-(num100/tmp))*num100;
			else result = false;
		}
		else if(based_on == C_discount_over_sales)
		{
			tmp = num1-(discount_over_sales/num100);
			if(tmp > 0)
			{
				based_on = C_sell_price2;
				sell_price2 = sell_price * tmp;
			}
			else result = false;
		}

		if(!result) return result;

		if(based_on == C_sell_price2)
		{
			//with price change on tab Prices
			//we will adjust Markup,Gross Profit
			//or buy price if buy price is 0
			if(buy_price == 0)
			{
				if((sell_markup == 0) && (markup_to_discount == 0))
				{
					buy_price = _mk_buy_price(sell_price2);
				}
				else
				{
					buy_price = _mk_buy_price(sell_price2 / (num1+(sell_markup/num100)) );
				}
			}
			else
			{
				tmp = _final_buy_price();
				sell_markup = (((sell_price2 - tmp) / tmp)*num100);
				markup_to_discount = ((num1-(tmp/ sell_price2))*num100);
			}
		}
		else if(based_on == C_sell_price)
		{
			//with price change on tab Main
			//we only adjust buy price
			sell_price2 = sell_price;
			buy_price = _mk_buy_price(sell_price2 / (num1+(sell_markup/num100)) );
		}
		else sell_price2 = _apply_markup(_final_buy_price());

		if(based_on != C_discount_over_sales){
			discount_over_sales = (num1-(sell_price2 / sell_price)) * num100;
		}
		sell_price = sell_price2 = math.broundf(sell_price2, price_decimals);

		return result;
	}
}
//insert class fields from static array to easy iterate over then later
foreach(k in Product_Calc_price.kalc_fields) Product_Calc_price[k] <- null;

local DB_Products = class extends DB_Manager {
	_calc_prices = null;
	
	constructor(){
		base.constructor(C_products, [
			C_buy_description,C_buy_discount,C_buy_notes,
			C_buy_other_costs,C_buy_price, C_buy_quantity_min,C_group_id,
			C_image_id,C_is_active,C_measure_unit_id,C_price_decimals,
			C_price_formula,C_reference_code,C_sales_tax_id,C_sell_description,
			C_sell_markup,C_sell_notes,C_sell_price,C_sell_quantity_min,C_show_on_buys,
			C_show_on_sales,C_show_on_web,C_show_price_on_web,C_stock_max,C_stock_min,
			C_supplier_code,C_tags,C_units_by_package,C_warranty_id,C_weight,
			C_sell_without_stock,
		]);

		_calc_prices = Product_Calc_price();
	}

	function db_action(db, data){
		mkEmptyWhenZero(data, [C_warranty_id,  C_group_id,  C_measure_unit_id,  C_image_id,  C_sales_tax_id]);
		return base.db_action(db, data);
	}
	
	function getPdfList(qs_tbl){
		local clipped = qs_tbl.get("clipped", false);
		local pdf = new PdfSqlTable();
		pdf.page_title = "Products List";
		pdf.water_mark = "T H I S   I S   A   D E M O";

		local sql =products_list_sql();
		local db = getOurbizDB();
		local mf = blob();
		pdf.GetPDF(db, sql, clipped, mf);
		qs_tbl._doc_pdf_ <- mf;
		return true;
	}

	function sql_list(qs_tbl, post_tbl){
		local product_id;
		if (qs_tbl.get("search", false)) return products_sql_search_list(qs_tbl, post_tbl);
		else if ( (product_id = qs_tbl.get("prices_by_quantity", 0)) ) return product_prices_list_sql(product_id.tointeger());
		else if ( (product_id = qs_tbl.get("past_products", 0)) ) return entity_past_products_get_sql(product_id.tointeger());
		else if ( (product_id = qs_tbl.get("last_order_lines", 0)) ) return last_product_order_lines_get_sql(product_id.tointeger());
		else if ( (product_id = qs_tbl.get("appear_together", 0)) ) return product_appear_together_get_sql(product_id.tointeger());
		else if ( (product_id = qs_tbl.get("history", 0)) ) {
			local htype = qs_tbl.get("htype", 0).tointeger();
			local query_limit = qs_tbl.get(C_query_limit, 50).tointeger();
			return product_sales_history_sql(htype, query_limit, product_id.tointeger());
		}
		else if ( (product_id = qs_tbl.get("statistics", 0)) ){
			local periode_count = qs_tbl.get(C_periode_count, 12).tointeger();
			local periode_type = getStatisticsPeriodeType(qs_tbl.periode_type);
			local sab = qs_tbl.get("sab", "S");
			return product_bar_chart_statistics_sql(product_id.tointeger(), sab, periode_count, periode_type);
		}
		else if (qs_tbl.get("print_list", false)){
			return products_list_sql();
		}
		else if (qs_tbl.get("pdf", false)){
			return getPdfList(qs_tbl);
		}
	}
	
	function getOneSqlWithThumbnail(){
			return [==[
select p.*, i.thumbnail, i.mime_type
from products p left join images i
	on p.image_id = i.id
where p.id=?]==];	
	}

	function sql_get_one(tbl_qs) {
		local id = tbl_qs.get(table_name, 0).tointeger();
		
		if(tbl_qs.get("discount_by_quantity", false))
		{
			local quantity = tbl_qs.get(C_quantity, 0);
			if(!quantity) return;
			return discount_by_quantity_get_one(id, quantity);
		}
		else if(tbl_qs.get("price_for_calc", false))
		{
			return get_one_record_sql( _table_name, id,
                                "buy_price, buy_discount, buy_other_costs, sell_price, price_decimals");
		}
		else if (tbl_qs.get("product_for_edit", false)){
			local db = getOurbizDB();
			local buf = blob(0, 8192);
			local sql;
			
			if(tbl_qs.get("with_thumbnail", false)){
				sql = getOneSqlWithThumbnail();
			} else {
				sql = base.sql_get_one(tbl_qs)
			}

			local stmt = db.prepare(sql);
			stmt.bind(1, id);
			if(!stmt.next_row()) return;
			local rec_map = stmt.asTable();
			_calc_prices.clear();
			_calc_prices.calc_by_map(rec_map);
			WriteMap2SLEArray(buf, rec_map, true);

			stmt.prepare(product_prices_list_sql(id));
			buf.write(stmt.asSleArray());

			stmt.prepare(products_kit_list_sql(id));
			buf.write(stmt.asSleArray());

			stmt.prepare(products_kit_details_get_one(id));
			buf.write(stmt.asSleArray());

			return buf;
		}
		else if(tbl_qs.get("product_aux_data", false))
		{
			local db = getOurbizDB();
			local buf = blob(0, 8192);
			
			local stmt = db.prepare(get_sales_tax_rates_short_list_sql());
			buf.write(stmt.asSleArray());

			stmt.prepare(get_measure_units_short_list_sql());
			buf.write(stmt.asSleArray());

			stmt.prepare(get_warranty_types_short_list_sql());
			buf.write(stmt.asSleArray());

			return buf;
		}
		else if(tbl_qs.get("pdf", false))
		{
			return getPdfList(qs_tbl);
		}
		else if(tbl_qs.get("with_thumbnail", false)){
			return getOneSqlWithThumbnail().replace("?", id.tostring());
		}
		else return base.sql_get_one(tbl_qs);
	}

	function db_action(db, data)
	{
		local str = data.get("__action__", false);
		if(str == "calc_price_by_quantity")
		{
			local id = data.get("__id__", 0);
			if(!id) return;

			_calc_prices.clear();
			local stmt = db.prepare(format([==[
select buy_price, buy_discount, buy_other_costs, sell_price, price_decimals 
from products where id=%d]==], id));
			if(stmt.next_row())
			{
				local count = stmt.col_count();
				for(local i=0; i < count; ++i)
				{
					data[stmt.col_name(i)] = stmt.asString(i);
				}
				_calc_prices.calc_over_sales(data);
				local buf = blob(0, 8192);
				WriteMap2SLEArray(buf, data);
				return buf;
			}
		}
		else if(str == "calc_product_price")
		{
			_calc_prices.clear();
			_calc_prices.calc_by_map(data);
			local buf = blob(0, 8192);
			WriteMap2SLEArray(buf, data);
			return buf;
		}
		else
		{
			local result = base.db_action(db, data);
			//save_image(db, data, _table_name);
			return result;
		}
	};

	function validate_db_fields(dbfileds)
	{
		foreach(key, val in dbfields)
		{
			if(key == C_measure_unit_id && val == "0")
			{
				dbfileds[key] = "";
			}
			else if(key == C_warranty_id && kv.val == "0")
			{
				dbfileds[key] = "";
			}
		}
	}
}

db_ourbiz_tables.products <- new DB_Products();

//
// images
//

local DB_Images = class extends DB_Manager {
	constructor(){
		base.constructor(C_images, [C_mime_type, C_name, C_description, C_group_set, C_image, C_thumbnail]);
	}

	function sql_list(qs_tbl, post_tbl){
		local so = get_search_options(post_tbl);
		local mf = blob();
		mf.write("select id, name, mime_type, group_set ");
		if (so.with_images) mf.write(", thumbnail ");
		mf.write(" from images where 1=1 ");
		if ((so.search_str && so.search_str.len() > 0) || so.image_id != 0){
			local search_str = escape_sql_like_search_str(so.search_str);
			if (so.image_id) mf.write(" and id=", so.image_id);
			else if (so.id) mf.write(" and id=", so.search_str.tointeger());
			else if (so.group_set) mf.write(" and group_set=", so.search_str.tointeger());
			else if (so.description) mf.write(" and description like '", search_str, "' ");
			else  mf.write(" and name like '", search_str, "' ");
		}
		mf.write(" order by name limit ", so.query_limit);
		//debug_print(tostring(mf), "\n")
		return  mf.tostring();
	}

	function sql_get_one(req){
		local mf = blob();
		mf.write("select id, _version_, mime_type, name, description, group_set, cdate, mdate, length(image) as img_size ");
		if (req.get("image_and_size", false)) mf.write(", image ");
		mf.write("from ", table_name ," where id=?");
		return  mf.tostring();
	}
}

db_ourbiz_tables.images <- new  DB_Images();

local DB_order_types = class extends DB_Manager {
	constructor(){
		base.constructor(C_order_types, [C_is_active, C_code, C_description, "description_to_print",
			"group_order", "is_expense", "is_incoming", C_notes, "numbering",
			"series", "show_prices", "show_sales_tax", "subgroup_order",
			"with_credit", "with_inventory", "with_payment", "with_ordered",
			"with_sales_tax", "with_sales_tax_included", "with_quote"]);
	}

	function sql_list(qs_tbl, post_tbl){
		if (qs_tbl.get("short_list", false)){
			local sql = "select id, description from order_types where is_active = 1 ";
			local group_order = qs_tbl.get("group_order", false);
			if (group_order && (group_order == "S" || group_order == "B")){
				sql = format("%s and group_order='%s' ", sql, group_order);
			}
			sql = format("%s order by group_order,subgroup_order", sql);
			return sql;
		}
		else
		{
			return [==[
SELECT
	id,
	code,
	description,
	with_payment,
	with_inventory,
	group_order,
	subgroup_order,
	is_active
FROM order_types
ORDER BY group_order,subgroup_order
]==];
		}
	}
}


db_ourbiz_tables.order_types <- new DB_order_types();


db_ourbiz_tables.measure_units <- new DB_Manager("measure_units");

db_ourbiz_tables.payment_types <- new DB_Manager("payment_types", ["is_active", "code", "description", "payment_terms", "notes"]);

local function check_payment_terms(txt){
	local init = 0;
	local pct = 0.0;
	txt.gmatch("(%d+)%s+(%d+%.?%d*)%D+(%d+)%s+(%S+)", function(n, v, p, pt){
		// n ->decimal number expected
		// v -> pct number expected
		pct += mytofloat(v);
		// p -> integer number expected
		// pt -> string expected
		if (!(pt == "d" || pt == "w" || pt == "m" || pt == "dm")){
			throw "Invalid paymment terms periode !";
		}
	});
	//check for error
	if (pct != 100.0) throw "Total percentage not equal to 100% !";
}

local DB_sales_tax_rates = class extends DB_Manager {
	constructor(){
		base.constructor("sales_tax_rates", ["is_active", "rate1", "rate2", "description"]);
	}

	function sql_list(qs_tbl=null, post_tbl=null) {
		return "select id,rate1,rate2,description,is_active from sales_tax_rates order by rate1";
	}
}

db_ourbiz_tables.sales_tax_rates <- new DB_sales_tax_rates();

db_ourbiz_tables.warranty_types <- new DB_Manager("warranty_types")

local DB_groups_tree = class extends DB_Manager
{
	constructor(){
		base.constructor("groups_tree", ["description", "parent_id", "notes"]);
	}

	function dump_group_tree_childs(parent, out_result, parent_map, data_map){
		for(int_t iter=0, count = parent_map.len(); iter < count; ++iter){
			if (parent_map[iter][0] == parent){
				for (local found=iter, parent_count = parent_map.len(); found < parent_count; ++found){
					local map = parent_map[found];
					local id = map[1];
					local myparent = map[0];
					//debug_print(myparent, "\t", parent, "\n");
					if (myparent != parent) break;
					out_result.write("[");
					add2sle(out_result, id.tostring());
					add2sle(out_result, myparent.tostring());
					add2sle(out_result, data_map[id]);
					out_result.writen(SLE_SLEEND, 'c');
					out_result.write("]");
					dump_group_tree_childs(id, out_result, parent_map, data_map);
				}
				break;
			}
		}
	}

	function group_dump_data(db, out_result, tbl){
		out_result.clear();
		local parent_map = [];
		local  group_map = {};
		local id, parent_id;
		local stmt = db.prepare(format("select id, parent_id, description from %s order by 3", tbl));
		while (stmt.next_row()) {
			id = stmt.col(0);
			parent_id = stmt.col(1);
			if (type(parent_id) != "integer") parent_id = 0;
			//debug_print("\n", parent_id, "\t", id);
			parent_map.push([parent_id, id]);
			group_map[id] <- stmt.col(2);
		}
		parent_map.sort(@(a,b) a[0] <=> b[0] );
		out_result.write("[[");
		add2sle(out_result, "id");
		add2sle(out_result, "parent_id");
		add2sle(out_result, "description");
		out_result.writen(SLE_SLEEND, 'c');
		out_result.write("]");
		dump_group_tree_childs(0, out_result, parent_map, group_map);
		out_result.write("]");
	}

	function get_list(db, mFile)
	{
		group_dump_data(db, mFile, table_name);
	};
};

local DB_product_groups = class extends DB_groups_tree
{
	constructor(){
		base.constructor();
		table_name = "product_groups";
	}
};
db_ourbiz_tables.product_groups <- new DB_product_groups();

local DB_entity_groups = class  extends DB_groups_tree
{
	constructor(){
		base.constructor();
		table_name = "entity_groups";
	}
};
db_ourbiz_tables.entity_groups <- new DB_entity_groups();


local function ourbizDbMobile(request){
	local data = {};
	data.page_name = "list_products";
	local query_string = request.info.query_string;

	local db = getOurbizDB();
	local sql = "select id, reference_code, sell_description, sell_price, image_id from products ";
	local search_str;
	if (query_string) search_str = request.get_var(query_string, "search_str");
	if (search_str){
		sql = format("%s where sell_description like ? ", sql);
	}
	sql += format("%s limit 50", sql);
	local stmt = db.prepare(sql);
	//debug_print(sql, "\n")
	if (search_str){
		stmt.bind(1, escape_sql_like_search_str(search_str));
	}
	local result = getDbListFromStmt(stmt);
	data.rows <- result[0];
	local mFile = gmFile;
	mFile.clear();
	data.mix_write <- function(str){ if(str) mFile.write(str); };
	fillTemplate("mobile-list.tpl", data, AT_DEV_DBG);
	request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\n\r\n", mFile.len()));
	request.write_blob(mFile);
	return true;
}

local function ourbizDbGetList(request){
	local query_string = request.info.query_string
	if (query_string){
		local db = getOurbizDB();
		local qs_tbl = parse_qs_to_table(query_string);
		local list = qs_tbl.get("list", null);
		local sab = qs_tbl.get("sab", "S");
		local isPost = request.info.request_method == "POST";
		local sql, post_tbl, data;
		if (isPost) post_tbl = get_post_fields(request, 1024);
		else post_tbl = {};

		if (!post_tbl.get("query_limit", false)) post_tbl.query_limit <- 50;
		gmFile.clear();

		if (list == "entity_groups") db_ourbiz_tables.entity_groups.get_list(db, gmFile);
		else if (list == "product_groups") db_ourbiz_tables.product_groups.get_list(db, gmFile);
		else if (list == "config") sql = "select id, key,value from config";
		else if (db_ourbiz_tables.get(list, false)){
			sql = db_ourbiz_tables[list].sql_list(qs_tbl, post_tbl);
			local doc_pdf = qs_tbl.get("_doc_pdf_", false);
			if (doc_pdf){
				request.print(format([==[
HTTP/1.1 200 OK
Content-type: application/pdf
Content-Disposition: attachment; filename=%s-list.pdf
Content-Length: %d

]==], list, doc_pdf.len()));
				if(doc_pdf instanceof blob) request.write_blob(doc_pdf);
				else request.print(doc_pdf);
				return true;
			}
		}

		if (sql){
			local stmt = db.prepare(sql);
			//debug_print(sql, "\n", db.errmsg(), "\n")
			data = stmt.asSleArray();
			stmt.finalize();
		}
		else if (gmFile.len() > 0){
			data = gmFile.tostring();
		}

		if (data){
			//using string.format with binary data gives wrong results
			gmFile.clear()
			gmFile.write("HTTP/1.1 200 OK\r\nContent-type: text/plain; charset=x-user-defined\r\nContent-Length: ", data.len(), "\r\n\r\n", data);
			request.write_blob(gmFile);
			return true;
		}
	}
	return false;
}

local function ourbizDbGetOne(request){
	local query_string = request.info.query_string;
	if (query_string){
		local db = getOurbizDB();
		local tbl_qs = {}, tbl_keys = [];
		parse_qs_to_table_k(query_string, tbl_qs, tbl_keys);
		if (tbl_keys.len() == 0) return false;
		local tbl = tbl_keys[0];
		local rec_id = tbl_qs[tbl];
		local sql, data;

		gmFile.clear()

		if (tbl == "config") sql = "select * from config where id=?";
		else if (db_ourbiz_tables.get(tbl, false)) sql = db_ourbiz_tables[tbl].sql_get_one(tbl_qs);
		
		local doc_pdf = tbl_qs.get("_doc_pdf_", false);
		if (doc_pdf){
			request.print(format([==[
HTTP/1.1 200 OK
Content-type: application/pdf
Content-Disposition: attachment; filename=%s-list.pdf
Content-Length: %d

]==], tbl, doc_pdf.len()));
			if(doc_pdf instanceof blob) request.write_blob(doc_pdf);
			else request.print(doc_pdf);
			return true;
		}

		if (sql){
			if(sql instanceof blob) data = sql.tostring();
			else
			{
				local stmt = db.prepare(sql);
				stmt.bind(1, rec_id);
				data = stmt.asSleArray();
				//debug_print(rec_id, "\t", sql, "\t", data.len(), "\t", data, "\n")
				stmt.finalize();
			}
		}
		else if (gmFile.len() > 0) data = gmFile.tostring();

		if (data){
			//using string.format with binary data gives wrong results
			gmFile.clear();
			gmFile.write("HTTP/1.1 200 OK\r\nContent-type: text/plain; charset=x-user-defined\r\nContent-Length: ", data.len(), "\r\n\r\n", data);
			request.write_blob(gmFile);
			return true;
		}
	}
	return false;
}

local function ourbizDbGetBin(request){
	local query_string = request.info.query_string;
	if (query_string){
		local db = getOurbizDB();
		local image = request.get_var(query_string, "image");
		local thumbnail = request.get_var(query_string, "thumbnail");
		local stmt;
		gmFile.clear();

		if (image){
			gmFile.write("select ");
			if (thumbnail) gmFile.write("thumbnail");
			else gmFile.write("image");

			gmFile.write(", mime_type from images where id=",  image.tointeger());
			stmt = db.prepare(gmFile.tostring());
		}

		if (stmt){
			local result = stmt.step() == stmt.SQLITE_ROW;
			if (result){
				local data = stmt.col(0);
				local mime = stmt.col(1);
				//using string.format with binary data gives wrong results
				gmFile.clear();
				gmFile.write("HTTP/1.1 200 OK\r\nContent-type: ", mime, "\r\nContent-Length: ", data.len(), "\r\n\r\n", data);
				request.write_blob(gmFile);
			}
			stmt.finalize();
			return result;
		}
	}
	return false;
}

local function ourbizDbAction(request) {
	local isPost = request.info.request_method == "POST";
	if (isPost){
		local data = get_post_fields(request, 10*1024);
		local db = getOurbizDB();
		local tbl = data.get("__table__", null);
		local action = data.get("__action__", null);
		local result;
		//debug_print(tbl, "\n", action, "\n", data.__id__, "\n")
		gmFile.clear();
		local db_manager = db_ourbiz_tables.get(tbl, null);
		if (db_manager){
			result = db_manager.db_action(db, data);
		}
		if (result != null){
			if(result instanceof blob) data = result.tostring();
			else
			{
				gmFile.clear();
				gmFile.write("[[");
				if (action == C_insert) add2sle(gmFile, "id");
				else add2sle(gmFile, "changes");

				gmFile.writen(SLE_SLEEND, 'c');
				gmFile.write("][");
				add2sle(gmFile, result.tostring());
				gmFile.writen(SLE_SLEEND, 'c');
				gmFile.write("]]");
				data = gmFile.tostring();
			}
		}
		else data = null;

		//debug_print(data, "\n")
		if (data){
			//using string.format with binary data gives wrong results
			gmFile.clear();
			gmFile.write("HTTP/1.1 200 OK\r\nContent-type: text/plain; charset=x-user-defined\r\nContent-Length: ", data.len(), "\r\n\r\n", data);
			//debug_print(tostring(gmFile), "\n")
			request.write_blob(gmFile);
			return true;
		}
	}
	return false;
}

add_uri_hanlders({
	["/OURBIZ"] =function(request){
		request.print("HTTP/1.1 200 OK\r\nServer: OurBiz\r\nContent-Length: 0\r\n\r\n");
		return true;
	},
	["/GET-AUTH-REQ"] =function(request){
		request.send_authorization_request("r.dadbiz.es");
		return true;
	},
	["/DB/GetList"] = ourbizDbGetList,
	["/DB/GetOne"] = ourbizDbGetOne,
	["/DB/Action"] = ourbizDbAction,
	["/DB/GetBin"] = ourbizDbGetBin,
	["/DB/m"] = ourbizDbMobile,
});

} //end dummy nested scope



