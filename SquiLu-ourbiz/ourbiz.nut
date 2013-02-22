local globals = getroottable();
if(!globals.get("APP_CODE_FOLDER", false)) ::APP_CODE_FOLDER <- ".";

local constants = getconsttable();

function getOurbizDBFileName(){
	if(globals.get("jniLog", false)) return APP_CODE_FOLDER + "/ourbiz.db";
	if(globals.get("WIN32", false)) return APP_CODE_FOLDER + "/../../ourbiz-uk/ourbiz.db";
	return "/home/mingo/dev/FrontAccountLua/ourbiz.db";
}

::ourbizDB <- null;
function getOurbizDB(){
	if(!::ourbizDB) ::ourbizDB = SQLite3(getOurbizDBFileName());
	return ::ourbizDB;
	//return checkCachedDB(APP_CODE_FOLDER + "/ourbiz.db");
}

if(globals.get("AT_DEV_DBG", false) || !globals.get("PdfSqlTable", false)) {
	dofile(APP_CODE_FOLDER + "/pdf-table.nut");
}

function escape_sql_like_search_str(str){
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

function mkEmptyWhenZero(tbl, key){
	if (type(key) == "array"){
		foreach( v in key ) {
			if (tbl.get(v, false) == "0") tbl[v] = "";
		}
	}
	else if (tbl[key] == 0) tbl[key] = "";
}

constants.rawdelete("TimePeriode");
enum TimePeriode {is_years = 1, is_months, is_weeks, is_days};

function getStatisticsPeriodeType(speriode){
	if(speriode == "years") return TimePeriode.is_years;
	else if(speriode == "weeks") return TimePeriode.is_weeks;
	else if(speriode == "days") return TimePeriode.is_days;
	return TimePeriode.is_months;
}

function get_sql_bar_chart_statistics_periodes (periode_count, periode_type){
	local speriode2;
	local periodeMultiplier = 1;
	local speriode;

	switch(periode_type){
		case TimePeriode.is_years:{
			speriode2 = "%Y";
			speriode = "years";
		}
		break;
		case TimePeriode.is_weeks:{
			periodeMultiplier = 7;
			speriode2 = "%Y-%W";
			speriode = "days";
		}
		break;
		case TimePeriode.is_days:{
			speriode2 = "%Y-%m-%d";
			speriode = "days";
		}
		break;
		default:{
			speriode2 = "%Y-%m";
			speriode = "months";
		}
	}

	periode_count = periode_count * periodeMultiplier;
	local result_periode = format("strftime('%s', order_date)", speriode2);
	local result_periode_count = format("date('now', '-%d %s')", periode_count, speriode);
	return [result_periode, result_periode_count];
}

function getOptionsFromMap(map){
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

        CHECK_BOOL("active");
        CHECK_BOOL("buys");
        CHECK_BOOL("cdate");
        CHECK_BOOL("group_set");
        CHECK_BOOL("inactive");
        CHECK_BOOL("mdate");
        CHECK_BOOL("only_prices_older");
        CHECK_BOOL("order_by_creation");
        CHECK_BOOL("order_by_modification");
        CHECK_BOOL("sales");
        CHECK_INT("entity_id");
        CHECK_INT("group_id");
        CHECK_INT("image_id");
        CHECK_INT("account_id");
        CHECK_INT("order_id");
        CHECK_INT("payment_type_id");
        CHECK_INT("product_id");
        CHECK_INT_DFLT("query_limit", 50);
        CHECK_BOOL("contact");
        CHECK_BOOL("date");
        CHECK_BOOL("description");
        CHECK_BOOL("entities");
        CHECK_BOOL("id");
        CHECK_BOOL("name");
        CHECK_BOOL("notes");
        CHECK_BOOL("phone");
        CHECK_BOOL("products");
        CHECK_BOOL("reference");
        CHECK_BOOL("total");
        CHECK_BOOL("with_images");
        CHECK_BOOL("with_headers");
        CHECK_BOOL("with_accounts");
        CHECK_BOOL("code");
	return opt;
}

function checkQueryStringSAB(qs_tbl, so){
	local qs_sab = qs_tbl.get("sab", null);
	if (qs_sab){
		if (qs_sab == "S") so.sales <- true;
		else if (qs_sab == "B") so.buys <- true;
	}
}

function get_search_options(req){
	local search_opt = getOptionsFromMap(req);

	local sab = req.get("sab", null);
	if (sab && sab.len() > 0){
		local c = sab.toupper();
		if (c == "S") search_opt.sales <- true;
		else if (c == "B") search_opt.buys <- true;
	}
	return search_opt;
}


class DB_Manager {
	table_name = null;
	editable_fields = null;
	has_mdate = null;
	has_version = null;

	constructor(ptable_name, peditable_fields = null){
		if(!peditable_fields) peditable_fields =  ["is_active", "code", "description"];
		table_name = ptable_name;
		editable_fields = peditable_fields;
		has_mdate = true;
		has_version = true;
	}

	function db_action(db, data){
		local action = data.__action__;
		if (action == "insert") return db_insert(db,data);
		else if (action == "update") return db_update(db,data);
		else if (action == "delete") return db_delete(db,data);
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

	function sql_get_one(req) {
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
}

local db_ourbiz_tables = {};

//
// entities
//

class DB_Entities extends DB_Manager {
	constructor(){
		base.constructor("entities", [
		"address", "city", "company", "contact",
		"country", "email", "fax", "gps_coordinates", "group_id",
		"image_id", "is_active", "name", "notes", "parent_id", "phone",
		"sales_tax_exempt", "show_on_buys", "show_on_sales", "state",
		"tags", "tax_number", "use_sales_tax2", "user_code", "web", "zip",
		"irpf_pct_retention"]);
	}

	function past_products_sql(entity_id){
		return format([==[
SELECT
    product_id as 'id|ID|6|R',
    sell_description as 'sell_description|Product|-1',
    sum(quantity) as 'quantity|Quantity|8|R',
    sum(quantity*price) as 'amount|Amount|8|R|M',
    code as 'order_type_code|Type|4|C'
    FROM orders_lines as ol, orders as o, order_types as ot, products as p
    WHERE o.entity_id = %d
    and ot.id = o.order_type_id
    and ol.order_id = o.id
    and p.id = ol.product_id
    GROUP BY product_id, code
    ORDER BY 3 desc
]==], entity_id);
	}

	static function sales_history_sql(history_type, query_limit, entity_id){
		// Select history type
		history_type = history_type.tointeger();
		if (history_type < 1) {return "select '' as 'Select one|-1';" };

		entity_id = entity_id.tointeger();
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
    WHERE order_id in(SELECT id FROM orders WHERE entity_id =]==], entity_id, [==[)
    GROUP BY order_id
    )  as ott
    WHERE entity_id =]==], entity_id, " and o.id = ott.order_id");
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

	function sql_bar_chart_statistics(aId, sab, periode_count, periode_type){
		local mf = blob();
		local rc = get_sql_bar_chart_statistics_periodes(periode_count, periode_type);
		local speriode = rc[0], speriode_count = rc[1];

		if (sab == "S" || sab == "B"){
			mf.write(get_bar_chart_statistics_sql_core(aId, sab, speriode, speriode_count));
		}
		else
		{
			mf.write("select qm, q, sab from (")
			mf.write(get_bar_chart_statistics_sql_core(aId, "S", speriode, speriode_count))
			mf.write(" UNION ALL ")
			mf.write(get_bar_chart_statistics_sql_core(aId, "B", speriode, speriode_count))
			mf.write(") as t ")
		}

	    mf.write(" order by qm, sab")
	    //debug_print(tostring(mf), "\n")
	    return mf.tostring();
	}

	function sql_search_list(qs_tbl, post_tbl){
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

	function sql_list(qs_tbl, post_tbl){
		if (qs_tbl.get("search", false)) return sql_search_list(qs_tbl, post_tbl);
		else if (qs_tbl.get("past_products", false)) return past_products_sql(qs_tbl.past_products);
		else if (qs_tbl.get("history", false)){
			local htype = qs_tbl.get("htype", 0);
			local query_limit = qs_tbl.get("query_limit", 50).tointeger();
			return sales_history_sql(htype, query_limit, qs_tbl.history);
		}
		else if (qs_tbl.get("statistics", false)){
			local periode_count = qs_tbl.get("periode_count", 12).tointeger();
			local periode_type = getStatisticsPeriodeType(qs_tbl.get("periode_type", "months"));
			local sab = qs_tbl.get("sab", "S");
			return sql_bar_chart_statistics(qs_tbl.get("statistics", 0).tointeger(), sab, periode_type, periode_count);
		}
		else if (qs_tbl.get("print_list", false)){
			return [==[
select id as 'id|ID|6|R', name as 'name|Name|-1', contact as 'contact|Contact|12', phone as 'phone|Phone|12'
from entities order by name
]==];
		}
		else if (qs_tbl.get("pdf", false)){
			local clipped = qs_tbl.get("clipped", false);
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
			local mf = blob();
			pdf.GetPDF(db, sql, clipped, mf);
			qs_tbl._doc_pdf_ <- mf;
			return true;
		}
	}
}

db_ourbiz_tables.entities <- new DB_Entities();


//
// orders
//

if(globals.get("AT_DEV_DBG", false) || !globals.get("PDF_Order", false)) {
	dofile(APP_CODE_FOLDER + "/pdf-order.nut");
}

class My_PDF_Order extends PDF_Order {
	_tr_map = null;
	_lang = null;
	_tr_stmt = null;
	my_logo_image = null;

	constructor(db, lang){
		base.constructor();
		_lang = lang;
		_tr_map = {};
		my_logo_image = false;
		_tr_stmt = db.prepare(" select svalue from translations_values as tv, translations_keys as tk, languages as l "
			" where l.lang=? and tk.skey=? and tv.lang_id=l.id and tv.key_id=tk.id ");
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

class CalcOrderLine
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
			"unit_weight", "quantity", "weight", "price",
			"first_total", "discount_pct", "discount_amt",
			"line_subtotal", "sales_tax1_pct", "sales_tax1_amt",
			"sales_tax2_pct", "sales_tax2_amt", "line_total"
			];
		use_sales_tax2 = map.get("use_sales_tax2", false) == "1";
		tax_exempt = map.get("tax_exempt", false) == "1";
		price_decimals = map.get("price_decimals", 2).tointeger();

		foreach(field in fields){
			this[field] = map.get(field, 0.0).tofloat();
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
		add2sle(out_result, "first_total");
		add2sle(out_result, "discount_amt");
		add2sle(out_result, "line_subtotal");
		add2sle(out_result, "sales_tax1_amt");
		add2sle(out_result, "sales_tax2_amt");
		add2sle(out_result, "line_total");
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

class  CalcOrderTotals
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
		add2sle(out_result, "lines_count");
		add2sle(out_result, "weight_total");
		add2sle(out_result, "subtotal_amt");
		add2sle(out_result, "discount_amt");
		add2sle(out_result, "sales_tax1_amt");
		add2sle(out_result, "sales_tax2_amt");
		add2sle(out_result, "total_amt");
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
		local stmt = db.prepare("select * from orders where id=?", id);
		if(stmt.next_row())
		{
			local order_type_id = stmt.col("order_type_id");
			pdf_order.labelNumber = db.exec_get_one(
				format("select description_to_print from order_types where id=%d",
					order_type_id));
			local order_number = stmt.col("order_number");
			pdf_order.strNumber = stmt.col("series");
			pdf_order.strNumber += "/";
			pdf_order.strNumber += order_number;
			pdf_order.strDate = stmt.get("order_date");
			pdf_order.strEntity.push(stmt.get("entity_name"));
			pdf_order.strEntity.push(stmt.col("entity_address"));
			pdf_order.strEntity.push(stmt.col("entity_phone"));
			pdf_order.strEntity.push(stmt.get("entity_tax_number"));
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

class MyCalcOrderTotals
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
}

class DB_Orders extends DB_Manager {
	constructor(){
		base.constructor("orders", [
		"order_type_id", "series", "order_number", "entity_order_number",
		"entity_id", "entity_name", "entity_address",
		"entity_phone", "entity_zip", "entity_city",
		"entity_state", "entity_country", "entity_tax_number",
		"entity_use_sales_tax2", "entity_sales_tax_exempt",
		"order_date", "payment_type_id", "notes",
		"cash", "tags", "order_valid_till_date",
		"irpf_pct_retention"]);
	}

	function sql_search_list(qs_tbl, post_tbl){
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
		if (qs_tbl.get("search", false)) return sql_search_list(qs_tbl, post_tbl);
		else if (qs_tbl.get("history", false)){
			local htype = qs_tbl.get("htype", 0);
			local query_limit = qs_tbl.get("query_limit", 50).tointeger();
			return DB_Entities.sales_history_sql(htype, query_limit, qs_tbl.history);
		}
		else if (qs_tbl.get("statistics", false)){
			local periode_count = qs_tbl.get("periode_count", 12).tointeger();
			local periode_type = getStatisticsPeriodeType(qs_tbl.get("periode_type", "months"));
			local sab = qs_tbl.get("sab", "S");
			return sql_bar_chart_statistics(sab, periode_count, periode_type);
		}
		else if (qs_tbl.get("print_list", false)){
		}
	}
}

db_ourbiz_tables.orders <-  new DB_Orders();

//
// products
//

class DB_Products extends DB_Manager {
	constructor(){
		base.constructor("products", [
		"buy_description","buy_discount","buy_notes",
		"buy_other_costs","buy_price",	"buy_quantity_min","group_id",
		"image_id","is_active","measure_unit_id","price_decimals",
		"price_formula","reference_code","sales_tax_id","sell_description",
		"sell_markup","sell_notes","sell_price","sell_quantity_min","show_on_buys",
		"show_on_sales","show_on_web","show_price_on_web","stock_max","stock_min",
		"supplier_code","tags","units_by_package","warranty_id","weight",
		"sell_without_stock"]);
	}

	function db_action(db, data){
		mkEmptyWhenZero(data, ["warranty_id",  "group_id",  "measure_unit_id",  "image_id",  "sales_tax_id"]);
		return base.db_action(db, data);
	}

	function sql_search_list(qs_tbl, post_tbl){
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
	select id from orders where entity_id = " , so.entity_id
	order by id desc)) ]==]);
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
		//debug_print(tostring(mf), "\n")
		return  mf.tostring();
	}

	function sql_list(qs_tbl, post_tbl){
		if (qs_tbl.get("search", false)) return sql_search_list(qs_tbl, post_tbl);
		else if (qs_tbl.get("past_products", false)) return DB_Entitles.past_products_sql(qs_tbl.past_products);
		else if (qs_tbl.get("history", false)) {
			local htype = qs_tbl.get("htype", 0);
			local query_limit = qs_tbl.get("query_limit", 50).tointeger();
			return DB_Entities.sales_history_sql(htype, query_limit, qs_tbl.history);
		}
		else if (qs_tbl.get("statistics", false)){
			local periode_count = qs_tbl.get("periode_count", 12).tointeger();
			local periode_type = getStatisticsPeriodeType(qs_tbl.periode_type);
			local sab = qs_tbl.get("sab", "S");
			return DB_Entities.sql_bar_chart_statistics(qs_tbl.statistics.tointeger(), sab, periode_count, periode_type);
		}
		else if (qs_tbl.get("print_list", false)){
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
		else if (qs_tbl.get("pdf", false)){
			local clipped = qs_tbl.get("clipped", false);
			local pdf = new PdfSqlTable();
			pdf.page_title = "Products List";
			pdf.water_mark = "T H I S   I S   A   D E M O";

			local sql = [==[
select
    p.id  as 'id|ID|6|R',
    case when p.user_code isnull then ' ' else p.user_code end as 'reference|Reference|9',
    p.sell_description as 'sell_description|Description|-1',
    cast(case when p.kit isnull then 0 else kit end as text) as 'kit|Kit|4|C',
    round((p.sell_price * (1 + (st.rate1/100.0))),p.price_decimals) as 'price_taxed|Price+V.A.T.|12|R|M',
    round(case when p.kit > 0 then p.kit_onhand else p.quantity_onhand end, 4) as 'quantity_onhand|Onhand|10|R|N'
from products_onhand as p left join sales_tax_rates as st
on p.sales_tax_id = st.id order by 3
]==]
			local db = getOurbizDB();
			local mf = blob();
			pdf.GetPDF(db, sql, clipped, mf);
			qs_tbl._doc_pdf_ <- mf;
			return true;
		}
	}
}

db_ourbiz_tables.products <- new DB_Products();

//
// images
//

class DB_Images extends DB_Manager {
	constructor(){
		base.constructor("images", ["mime_type", "name", "description", "group_set", "image", "thumbnail"]);
	}

	function sql_list(qs_tbl, post_tbl){
		local so = get_search_options(post_tbl);
		local mf = blob();
		mf.write("select id, name, mime_type, group_set ");
		if (so.with_images) mf.write(", thumbnail ");
		mf.write(" from images where 1=1 ");
		if ((so.search_str && so.len().search_str > 0) || so.image_id != 0){
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

class DB_order_types extends DB_Manager {
	constructor(){
		base.constructor("order_types", ["is_active", "code", "description", "description_to_print",
            "group_order", "is_expense", "is_incoming", "notes", "numbering",
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

function check_payment_terms(txt){
	local init = 0;
	local pct = 0.0;
	txt.gmatch("(%d+)%s+(%d+%.?%d*)%D+(%d+)%s+(%S+)", function(n, v, p, pt){
		// n ->decimal number expected
		// v -> pct number expected
		pct += v.tofloat();
		// p -> integer number expected
		// pt -> string expected
		if (!(pt == "d" || pt == "w" || pt == "m" || pt == "dm")){
			throw "Invalid paymment terms periode !";
		}
	});
	//check for error
	if (pct != 100.0) throw "Total percentage not equal to 100% !";
}

class DB_sales_tax_rates extends DB_Manager {
	constructor(){
		base.constructor("sales_tax_rates", ["is_active", "rate1", "rate2", "description"]);
	}

	function sql_list(qs_tbl=null, post_tbl=null) {
		return "select id,rate1,rate2,description,is_active from sales_tax_rates order by rate1";
	}
}

db_ourbiz_tables.sales_tax_rates <- new DB_sales_tax_rates();

db_ourbiz_tables.warranty_types <- new DB_Manager("warranty_types")


function product_prices_list_sql (product_id){
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

function products_kit_list_sql (kit_id, partOf=false){
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

function products_kit_details_get_one (kit_id){
	kit_id = kit_id.tointeger();
	return format([==[
select count(*) as nproducts, sum(p.sell_price * pk.quantity) as amt_total
from product_kits as pk left join products as p on pk.product_id = p.id
where pk.kit_id = %d]==], kit_id);
}

function add2sle(out_result, str){
	str = str.tostring();
	out_result.write(get_sle_size(str.len()), str);
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

function send_http_error_500(request, err_msg){
	if(AT_DEV_DBG) {
		foreach(k,v in get_last_stackinfo()) debug_print("\n", k, ":", v);
		debug_print("\n", err_msg, "\n")
	}
	gmFile.clear();
	gmFile.write("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: ", err_msg.len(), "\r\n\r\n", err_msg);
	//debug_print(tostring(gmFile), "\n")
	request.write_blob(gmFile);
	return true;
}

function ourbizDbMobile(request){
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

function ourbizDbGetList(request){
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

		if (list == "entity_groups") group_dump_data(db, gmFile, "entity_groups");
		else if (list == "product_groups") group_dump_data(db, gmFile, "product_groups");
		else if (list == "config") sql = "select key,value from config";
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
			try {
				local stmt = db.prepare(sql);
				//debug_print(sql, "\n", db.errmsg(), "\n")
				data = stmt.asSleArray();
				stmt.finalize();
			}
			catch(e){
				return send_http_error_500(request, e);
			}
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

function ourbizDbGetOne(request){
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

		if (tbl == "orders"){
			if (request.get_var(query_string, "with_lines")){
				sql = "select * from orders where id=?";
				local stmt = db.prepare(sql);
				stmt.bind(1, rec_id);
				gmFile.write(stmt.asSleArray());
				stmt.finalize();
				sql = "select id, product_id, description, quantity, price, 0 as sub_total from orders_lines where order_id=?";
				stmt = db.prepare(sql);
				stmt.bind(1, rec_id);
				gmFile.write(stmt.asSleArray());
				stmt.finalize();
				sql = null;
			}
			else if (request.get_var(query_string, "line_calculated")){
				sql = "select * from orders_lines where id=?";
			}
		}
		else if (tbl == "products"){
			local product_aux_data = request.get_var(query_string, "product_aux_data");
			local product_for_edit = request.get_var(query_string, "product_for_edit");
			if (product_aux_data){
				local stmt = db.prepare(db_ourbiz_tables.sales_tax_rates.sql_short_list(tbl_qs));
				gmFile.write(stmt.asSleArray());
				stmt.finalize();
				stmt = db.prepare("select id, description from measure_units where is_active=1 order by description");
				gmFile.write(stmt.asSleArray());
				stmt.finalize();
				stmt = db.prepare("select id, description from warranty_types where is_active=1 order by description");
				gmFile.write(stmt.asSleArray());
				stmt.finalize();
			}
			else if (product_for_edit){
				local stmt = db.prepare("select * from products where id=?");
				stmt.bind(1, rec_id);
				gmFile.write(stmt.asSleArray());
				stmt.finalize();

				stmt = db.prepare(product_prices_list_sql(rec_id));
				gmFile.write(stmt.asSleArray());
				stmt.finalize();

				stmt = db.prepare(products_kit_list_sql(rec_id));
				gmFile.write(stmt.asSleArray());
				stmt.finalize();

				stmt = db.prepare(products_kit_details_get_one(rec_id));
				gmFile.write(stmt.asSleArray());
				stmt.finalize();
			}
			else sql = "select * from products where id=?";
		}
		else if (tbl == "config") sql = "select * from config where id=?";
		else if (db_ourbiz_tables.get(tbl, false)) sql = db_ourbiz_tables[tbl].sql_get_one(tbl_qs);

		if (sql){
			local stmt = db.prepare(sql);
			stmt.bind(1, rec_id);
			data = stmt.asSleArray();
			//debug_print(rec_id, "\t", sql, "\t", data.len(), "\t", data, "\n")
			stmt.finalize();
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

function ourbizDbGetBin(request){
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

function ourbizDbAction(request){
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
			try {
				result = db_manager.db_action(db, data);
			} catch(e){
				return send_http_error_500(request, e);
			}
		}
		if (result != null){
			gmFile.clear();
			gmFile.write("[[");
			if (action == "insert") add2sle(gmFile, "id");
			else add2sle(gmFile, "changes");

			gmFile.writen(SLE_SLEEND, 'c');
			gmFile.write("][");
			add2sle(gmFile, result.tostring());
			gmFile.writen(SLE_SLEEND, 'c');
			gmFile.write("]]");
			data = gmFile.tostring();
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


