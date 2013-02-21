local globals = getroottable();
local constants = getconsttable();

class AuthDigest {
	_nc = null;
	_cnonce = null;
	_username = null;
	_password = null;
	_authenticate = null;
	_realm = null;
	_qop = null;
	_nonce = null;
	_opaque = null;
	
	constructor(username =null, password = null){
		_nc = 0;
		_cnonce = os.time();
		credentials(username, password);
	}
	function credentials(username, password){
		_username = username;
		_password = password;
	}
	function okToAuthDigest(){
		return _authenticate && _authenticate.len() > 0;
	}
	function auth_digest(method, uri){
		if(!okToAuthDigest()) throw "Trying to auth digest without authentication.";
		local ha1, ha2, ha, ncbuf;
		ncbuf = format("%08x", ++_nc);
		local cnoncebuf = format("%012d", ++_cnonce);
		local _sep = ":";
		ha1 = md5(_username, _sep, _realm, _sep, _password);
		ha2 = md5(method, _sep, uri);
		ha = md5(ha1, _sep, _nonce, _sep, ncbuf, _sep, cnoncebuf, _sep, _qop, _sep, ha2);
		local result = format([==[Digest username="%s", realm="%s", nonce="%s", uri="%s", qop="%s", nc="%s", cnonce="%s", response="%s"]==], 
			_username, _realm, _nonce, uri, _qop, ncbuf, cnoncebuf, ha);
		if(_opaque && _opaque.len()){
			result = format("%s, opaque=\"%s\"", result, _opaque);
		}
		return result;
	}

	function parse_authenticate(szAuthenticate){
		_authenticate = szAuthenticate;
		_realm = null;
		_qop = null;
		_nonce = null;
		_opaque = null;
		local function checkKV(k,v){
			if(v[0] == '"') v = v.slice(1, -1);
			if(k == "realm") _realm = v;
			else if(k == "qop") _qop = v;
			else if(k == "nonce") _nonce = v;
			else if(k == "opaque") _opaque = v;
			return true;
		}
		checkKV.setenv(this);
		szAuthenticate.gmatch("(%w+)%s*=%s*([^%s,]+)", checkKV);
		//printf("%s, %s, %s, %s\n", _realm || "?", _qop || "?", _nonce || "?", _opaque || "?");
	}
}

//local auth = AuthDigest();
//auth.parse_authenticate([==[Digest username="mingote", realm="ourbiz.dadbiz.es", nonce="7f1de29e6da19d22b51c68001e7e0e54", uri="/DB/GetList?list=sales_tax_rates", response="08cfa598e5945d3aada09308017da216", qop=auth, nc=00000002, cnonce="bf5f336c1827bbb2"]==]);

dofile("happyhttp.nut");

class HTTPConnBase extends HappyHttpConnection
{
    time_start = null;
    my_status = null;
    my_content_len = null;
    _www_authenticate = null;

    constructor(host="localhost", port=8008){
	base.constructor(host, port);
        my_status = my_content_len = 0;
        time_start = os.getmillicount();
    }

    function set_params(host, port)
    {
        m_Host = host;
        m_Port = port;
    }

    function response_begin( r )
    {
        my_status = r->getstatus();
        my_content_len = r->getLength();
        if(my_content_len < 0) my_content_len = 0;
        if(my_status == 401)
        {
            _www_authenticate = r->getheader("WWW-Authenticate");
        }
    }
    function response_data( r, data, data_idx, n )
    {
    }
    function response_complete( r )
    {
        printf("%d : %d : %s\n", os.getmillispan(time_start), my_content_len, r->geturl());
    }
};

class HTTPConnAuthBase extends HTTPConnBase
{
    _cnonce = null;
    _nc = null;
    _digest_auth = null;

    constructor()
    {
	base.constructor();
	_cnonce = os.time();
	_nc = 0;
        _digest_auth = null;
    }
    function set_params(host, port, digest_auth)
    {
        base.set_params(host, port);
        _digest_auth = digest_auth;
    }
    function response_begin( r )
    {
        base.response_begin(r);
        if(my_status != 200)
        {
	    //foreach(k,v in get_last_stackinfo()) print(k,v)
            throw (format("HTTP Error %d", my_status));
        }
    }

    function send_my_request(method, uri, body=null, bodysize=null, boundary=null, useSLE=false)
    {
        //printf("Request body %d : %s\n", bodysize, body);
        local str;
        for(local i=0; i<2;++i){
            try
            {
                my_status = 0;
                putrequest(method, uri);
                if(method == "POST"){
                    if(useSLE) str = "SLE";
                    else if(boundary){
                        str = format("multipart/form-data; boundary=%s", boundary);
                    }
                    else str = "x-www-form-urlencoded";
                    putheader("Content-Type", str);
                }
                if(_digest_auth->okToAuthDigest())
                    putheader("Authorization", _digest_auth->auth_digest(method, uri));
                putheader("Content-Length", bodysize || 0);
                endheaders();
                if(body && bodysize) send( body, bodysize );
                while( outstanding() )
                {
                    if(globals.get("Fl", false)) Fl.check();//check_idle();
                    pump();
                }
                break;
            }
            catch(e)
            {
		//foreach(k,v in get_last_stackinfo()) print(k,v)
                if(my_status == 401){
                        _digest_auth->parse_authenticate(_www_authenticate);
                        close();
                }
                else if(my_status == 200)
                        break;
                else throw e;
            }
        }
    }
};

class HTTPConn extends HTTPConnAuthBase
{
    result_out = null;

    constructor(aresult_out)
    {
	assert(aresult_out instanceof stream)
	base.constructor();
	result_out = aresult_out;
    }
    function response_begin( r )
    {
        base.response_begin(r);
        result_out.clear();
        local resp_length = r->getLength();
        if(resp_length > 0) result_out.reserve(resp_length);
    }

    function response_data( r, data, data_idx, n )
    {
        result_out.write_str(data, data_idx, n);
    }
};

class HTTPConnBin extends HTTPConnAuthBase
{
    my_result = null;
    my_content_type = null;

    constructor(vresult)
    {
	base.constructor();
	my_result = vresult;
    }
    function response_begin( r )
    {
        base.response_begin(r);
        my_result.clear();
        local resp_length = r->getLength();
        if(resp_length > 0) my_result.reserve(resp_length);
        my_content_type = r->getheader("Content-type");
    }

    function response_data( r, data, data_idx, n )
    {
        my_result.write_str(data, data_idx, n);
    }
};

enum conn_type_e {e_conn_none, e_conn_http, e_conn_dbfile};

constants.rawdelete("TimePeriode");
enum TimePeriode {is_years = 1, is_months, is_weeks, is_days};

local _the_app_server = null;
class AppServer
{
    _host = null;
    _digest = null;
    _port = null;
    _conn_type = null;
    _db = null;
    static e_conn_none = conn_type_e.e_conn_none;
    static e_conn_http = conn_type_e.e_conn_http;
    static e_conn_dbfile = conn_type_e.e_conn_dbfile;
    
    constructor()
    {
        _conn_type = conn_type_e.e_conn_none;
        _port = 0;
	_digest = AuthDigest();
    }

    static function getAppServer()
    {
	if(!_the_app_server) _the_app_server = AppServer();
        return _the_app_server;
    }

    function credentials(username, password)
    {
        _digest.credentials(username, password);
    }

    function get_username()
    {
        return _digest.get_username();
    }

    function get_user_password()
    {
        return _digest.get_user_password();
    }

    function connect(host, port=8008)
    {
        _host = host;
        _port = port;
        _conn_type = conn_type_e.e_conn_http;
    }

    function get_host()
    {
        return _host;
    }

    function get_port()
    {
        return _port;
    }

    function check_login()
    {
        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local my_result = blob();
            local http = HTTPConn(my_result);
            httpcon_setup(http);
            http.send_my_request("GET", "/OURBIZ");
            if(http.my_status != 200)
            {
                throw(my_result.tostring());
            }
        }
    }

    function get_conn_type()
    {
        return _conn_type;
    }

    function httpcon_setup(conn)
    {
        conn.set_params(_host, _port, _digest);
    }

    function close()
    {
        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local my_result = blob();
            local http = HTTPConn(my_result);
            httpcon_setup(http);
	    try {
		http.send_my_request("GET", "/SQ/logout");
	    }
	    catch(e){
		if(http.my_status != 302) throw(my_result.tostring());
	    }
        }
    }

    function _get_data(method, query_string, httpBody, data_out, throwNotFound=true)
    {
        data_out.clear();

        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local httpRequest = HTTPConn(data_out);
            httpcon_setup(httpRequest);

            httpRequest.send_my_request(method, query_string, httpBody,
                                     httpBody ? httpBody.len() : 0);
            if(httpRequest.my_status != 200)
            {
                if(throwNotFound) throw(data_out.tostring());
                else return;
            }
        }
    }

    function get_binary_data_from_url(get_url, rec, content_type, throwNotFound=true)
    {
        rec.clear();

        if(get_conn_type() == conn_type_e.e_conn_http)
        {
		local  httpRequest = HTTPConnBin(rec);
		httpcon_setup(httpRequest);

		httpRequest.send_my_request("GET", get_url);
		if(httpRequest.my_status != 200)
		{
			if(throwNotFound) throw(httpRequest.my_result.tostring());
			else return;
		}
		content_type = httpRequest.my_content_type;
        }
    }

    function get_binary_data(rec, binary_type, table, aid, extra_url=0, throwNotFound=true)
    {
        rec.clear();
        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local url = format("/DB/GetBin?%s=%d", table, aid);
            if(extra_url) url += extra_url;

            get_binary_data_from_url(url, rec, binary_type, throwNotFound);
        }
    }

    function get_list_data(data, table, query_string, httpBody, withHeaders=false)
    {
        data.clear();

        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local my_result = blob();
            local  httpRequest = HTTPConn(my_result);
            httpcon_setup(httpRequest);

            local url = format("/DB/GetList?list=%s", table);
            if(query_string) url += query_string;

            httpRequest.send_my_request("POST", url, httpBody,
                                     httpBody ? httpBody.len() : 0);
            if(httpRequest.my_status != 200)
            {
		if(my_result.len()) throw(my_result.tostring());
		else throw(format("HTTP error %d", httpRequest.my_status));
            }
            //parse_jsonArray2Vector(data, my_result);
            sle2vecOfvec(my_result, data);
            if(!withHeaders) data.remove(0); //remove headers
        }
    }

    function get_map_list_data(data, table, qs, httpBody)
    {
        data.clear();

        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local my_result = blob();
            local httpRequest = HTTPConn(my_result);
            httpcon_setup(httpRequest);

            local url = format("/DB/GetList?list=%s", table);
            if(qs) url += qs;

            httpRequest.send_my_request("POST", url, httpBody,
                                     httpBody ? httpBody.len() : 0);

            if(httpRequest.my_status != 200)
            {
                throw(my_result.tostring());
            }

            //parse_jsonArray2map(data, my_result.c_str());
            sleArray2map(my_result, data);
        }
    }
    
    function sle2map(sle_str, rec, pos=0){
	local ar = [];
	sle2vecOfvec(sle_str, ar, pos);
	local keys = ar[0];
	local values = ar[1];
	foreach(idx, k in keys) rec[k] <- values[idx]; 
    }

    function do_dbaction(rec, action, table, aid, version, query_string=0)
    {
        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local my_result = blob();
            local httpRequest = HTTPConn(my_result);
            httpcon_setup(httpRequest);

            local url = "/DB/Action";
            if(query_string) url = format("%s?%s", url, query_string);
            local httpBody = blob(0, 8192);

            httpBody.write("[[");

            add2sle(httpBody, "__table__");
            add2sle(httpBody, "__id__");
            add2sle(httpBody, "__version__");
            add2sle(httpBody, "__action__");

	    foreach(k,v in rec)
            {
                add2sle(httpBody, k);
            }
            httpBody.writen(SLE_SLEEND, 'c');
            httpBody.write("][");

            add2sle(httpBody, table);
            add2sle(httpBody, aid.tostring());
            add2sle(httpBody, version.tostring());
            add2sle(httpBody, action);
	    foreach(k,v in rec)
            {
                add2sle(httpBody, v);
            }
            httpBody.writen(SLE_SLEEND, 'c');
            httpBody.write("]]");

            //printf("%d : %s\n", rec.size(), httpBody.c_str());
            httpRequest.send_my_request("POST", url, httpBody.tostring(),
                                    httpBody.len(), 0, true);
            if(httpRequest.my_status != 200)
            {
                throw(my_result.tostring());
            }
            rec.clear();
            //parse_jsonObject2map(rec, my_result.c_str());
            sle2map(my_result, rec);
        }
    }

    function get_str_record(table, qs, aid, extra_url=0)
    {
        if(get_conn_type() == conn_type_e.e_conn_http)
        {
            local my_result = blob();
            local httpRequest = HTTPConn(my_result);
            httpcon_setup(httpRequest);

            local url = format("/DB/GetOne?%s", table);
            if(qs) url += qs;
	    url = format("%s=%s", url, aid.tostring());
            if(extra_url) url += extra_url;

            httpRequest.send_my_request("GET", url);
            if(httpRequest.my_status != 200)
            {
                throw(my_result.tostring());
            }
            return my_result;
        }
        return "";
    }

    function get_record(rec, table, qs, aid, extra_url=0)
    {
        rec.clear();
        //parse_jsonObject2map(rec, get_str_record(table, qs, aid, extra_url).c_str());
        local str_sls = get_str_record(table, qs, aid, extra_url);
        sle2map(str_sls, rec);
    }

    function get_record_and_array(rec, data, table, qs, aid, extra_url=0)
    {
        rec.clear();
        data.clear();
        local jres = get_str_record(table, qs, aid, extra_url);
        //parse_jsonRecordAndArray(rec, data, jres.c_str());
        sle2RecordAndArray(jres, rec, data);
    }

    function config_get_list(data)
    {
        get_list_data(data, "config", 0, 0);
    }

    function config_get_map_list(data)
    {
        get_map_list_data(data, "config", "&map_list=1", 0);
    }

    function entities_get_list(data, so)
    {
        get_list_data_search(data, "entities", "&search=1", so);
    }

    function entities_toprint_get_list(data)
    {
        get_list_data(data, "entities", "&print_list=1", 0);
    }

    function entities_get_bar_chart_statistics_list(data, entity_id, sab, periode_count=12,
            periode=TimePeriode.is_months)
    {
        local qs = format("&statistics=%d&periode_type=%s&periode_count=%d&sab=%c",
                           entity_id, getStatisticsPeriodeType(periode),
                           periode_count, sab);
        get_list_data(data, "entities", qs, 0);
    }

    function entities_get_sales_history_list(data, ptype, plimit, entity_id)
    {
        local myUrl = format("&history=%d&htype=%d&query_limit=%d", entity_id, ptype, plimit);
        get_list_data(data, "entities", myUrl, 0, true);
    }

    function entity_past_products_list(data, entity_id)
    {
        local myUrl = format("&past_products=%d", entity_id);
        get_list_data(data, "entities", myUrl, 0, true);
    }

    function images_get_list(data, so)
    {
        get_list_data_search(data, "images", 0, so);
    }

    function images_get_image_record_by_id(rec, binary_type, id, thumbnail=true, throwNotFound=true)
    {
        local extra_url = thumbnail ? "&thumbnail=1" : null;
        get_binary_data(rec, binary_type, "image", id, extra_url, throwNotFound);
    }

    function images_get_image_record_by_name(rec, binary_type, img_name, thumbnail=true, throwNotFound=true)
    {
        local extra_url = thumbnail ? "&thumbnail=1" : null;
        get_binary_data(rec, binary_type, "image", 0, extra_url, throwNotFound);
    }

    function languages_get_list(data)
    {
        get_list_data(data, "languages", 0, 0);
    }

    function languages_get_short_list(data)
    {
        get_list_data(data, "languages", "&short_list=1", 0);
    }

    function measure_units_get_list(data)
    {
        get_list_data(data, "measure_units", 0, 0);
    }

    function measure_units_get_short_list(data)
    {
        get_list_data(data, "measure_units", "&short_list=1", 0);
    }

    function order_types_get_list(data)
    {
        get_list_data(data, "order_types", 0, 0);
    }

    function order_types_list_short(data, group_order)
    {
        local url = "&short_list=1";
        if(group_order) url += "&group_order=" + group_order;
        get_list_data(data, "order_types", url, 0);
    }

    function orders_lines_get_for_order(data, order_id)
    {
        local myUrl = "&lines=" + order_id;
        get_list_data(data, "orders", myUrl, 0);
    }

    function orders_get_sales_history_list(data, ptype, plimit)
    {
        local myUrl = format("&history=1&htype=%d&query_limit=%d", ptype, plimit);
        get_list_data(data, "orders", myUrl, 0, true);
    }

    function orders_get_bar_chart_statistics_list(data, sab, periode_count=12,
            periode=TimePeriode.is_months, paidUnpaid=false)
    {
        local myUrl = format("&statistics=1&periode_type=%s&periode_count=%d&paid_unpaid=%s&sab=%s", 
		getStatisticsPeriodeType(periode), periode_count, paidUnpaid ? "1" : "0", sab);
        get_list_data(data, "orders", myUrl, 0);
    }

    function get_list_data_search(data, table, qs, so)
    {
        local httpBody = so.getPostOptions();
        local myqs = "";
        if(qs) myqs += qs;
        if(so.sales) myqs += "&sab=sales";
        else if(so.buys) myqs += "&sab=buys";
        get_list_data(data, table, myqs, httpBody);
    }

    function orders_get_list(data, so)
    {
        get_list_data_search(data, "orders", "&search=1", so);
    }

    function order_lines_onhand_list(data, order_id)
    {
        local myUrl = "&lines_onhand=" + order_id;
        get_list_data(data, "orders", myUrl, 0, true);
    }

    function orders_sum_get_list(data, query_limit)
    {
        local so = "";
        get_list_data_search(data, "orders_sum", 0, so);
    }

    function get_product_for_edit(product_id, rec, prices_list, kit_list, kit_details){
        local qs, js;
        qs = format("/DB/GetOne?products=%d&product_for_edit=1", product_id);
        _get_data("GET", qs, 0, js);

/*
        JsonJSMN p;
        JsonJSMN::jsmntok_t tok;
        JsonJSMN::jsmnerr_t rc = p.parse(js.c_str(), tok);
        if(rc != JsonJSMN::JSMN_SUCCESS) throw "Invalid json !";
        if(tok.type == JsonJSMN::JSMN_ARRAY){
            parse_jsonObject2map(p, rec, js.c_str());
            parse_jsonArray2Vector(p, prices_list, js.c_str());
            parse_jsonArray2Vector(p, kit_list, js.c_str());
            parse_jsonObject2map(p, kit_details, js.c_str());
        }
*/
        if(js[0] != '[') throw "Invalid sle encoded !";
        local start = 0;
        local pos = sle2map(js, rec, start);
        pos = sle2vecOfvec(js, prices_list, pos);
        pos = sle2vecOfvec(js, kit_list, pos);
        sle2map(js, kit_details, pos);
        prices_list.remove(0); //remove header
        kit_list.remove(0); //remove header
    }
    function get_product_aux_data(sales_tax_data, measure_units_data, warranty_data){
        local qs, js = blob(0, 8192);
        qs = "/DB/GetOne?products=0&product_aux_data=1";
        _get_data("GET", qs, 0, js);
/*
        JsonJSMN p;
        JsonJSMN::jsmntok_t tok;
        JsonJSMN::jsmnerr_t rc = p.parse(js.c_str(), tok);
        if(rc != JsonJSMN::JSMN_SUCCESS) throw "Invalid json !";
        if(tok.type == JsonJSMN::JSMN_ARRAY){
            parse_jsonArray2Vector(p, sales_tax_data, js.c_str());
            parse_jsonArray2Vector(p, measure_units_data, js.c_str());
            parse_jsonArray2Vector(p, warranty_data, js.c_str());
        }
*/
        if(js[0] != '[') throw "Invalid sle encoded !";
        local start = 0;
        local pos = sle2vecOfvec(js, sales_tax_data, start);
        pos = sle2vecOfvec(js,  measure_units_data, pos);
        pos = sle2vecOfvec(js, warranty_data, pos);
        //now deleting header data
        sales_tax_data.remove(0);
        measure_units_data.remove(0);
        warranty_data.remove(0);
    }
    function products_list_get_list(data)
    {
        get_list_data(data, "products", "&print_list=1", 0);
    }

    function products_get_list(data, so)
    {
        get_list_data_search(data, "products", "&search=1", so);
    }

    function product_prices_list(data, product_id)
    {
        local myUrl = "&prices_by_quantity="  + product_id;
        get_list_data(data, "products", myUrl, 0);
    }

    function products_kit_list(data, kit_id, part_of=false)
    {
        local myUrl = "&kit_id=" + kit_id;
        if(part_of) myUrl += "&part_of=1";
        get_list_data(data, "product_kits", myUrl, 0);
    }

    function products_kit_edit(rec, id)
    {
        get_record(rec, "product_kits", 0, id);
    }

    function products_get_sales_history_list(data, ptype, plimit, product_id)
    {
        local myUrl = format("&history=%d&htype=%d&query_limit=%d", product_id, ptype, plimit);
        get_list_data(data, "products", myUrl, 0, true);
    }

    function products_get_bar_chart_statistics_list(data, product_id, sab, periode_count=12,
            periode=TimePeriode.is_months)
    {
        local myUrl = format("&statistics=%d&periode_type=%s&periode_count=%d&sab=%s", product_id, 
		getStatisticsPeriodeType(periode), periode_count, sab);
        get_list_data(data, "products", myUrl, 0);
    }

    function last_product_order_lines_list(data, product_id)
    {
        local myUrl = "&last_order_lines=" + product_id;
        get_list_data(data, "products", myUrl, 0, true);
    }

    function product_appear_together_list(data, product_id)
    {
        local myUrl = "&appear_together=" + product_id;
        get_list_data(data, "products", myUrl, 0, true);
    }

    function product_availability_list(data, product_id, order_id)
    {
        local myUrl = format("&product_availability=%d&order_id=%d", product_id, order_id);
        get_list_data(data, "orders", myUrl, 0, true);
    }

    function discount_by_quantity_get_record(rec, product_id, quantity)
    {
        local extraUrl = "&quantity=" + quantity;
        get_record(rec, "products", "&discount_by_quantity", product_id, extraUrl);
    }

    function payment_types_get_list(data)
    {
        get_list_data(data, "payment_types", 0, 0);
    }

    function payment_types_get_short_list(data)
    {
        get_list_data(data, "payment_types", "&short_list=1", 0);
    }

    function sales_tax_rates_get_list(data)
    {
        get_list_data(data, "sales_tax_rates", 0, 0);
    }

    function sales_tax_rates_get_short_list(data)
    {
        get_list_data(data, "sales_tax_rates", "&short_list=1", 0);
    }

    function translations_values_get_list(data, lang_id=0)
    {
        local myUrl = "";
        if(lang_id)
        {
            myUrl += "&values=1&lang_id=" + lang_id;
        }
        get_list_data(data, "translations", myUrl, 0);
    }

    function warranty_types_get_list(data)
    {
        get_list_data(data, "warranty_types", 0, 0);
    }

    function warranty_types_get_short_list(data)
    {
        get_list_data(data, "warranty_types", "&short_list=1", 0);
    }

    function gl_groups_get_list(data)
    {
        get_list_data(data, "gl_groups", 0, 0);
    }

    function gl_groups_get_short_list(data)
    {
        get_list_data(data, "gl_groups", "&short_list=1", 0);
    }

    function gl_chart_get_short_list(data)
    {
        get_list_data(data, "gl_chart", "&short_list=1", 0);
    }

    function gl_chart_get_list(data, so)
    {
        get_list_data_search(data, "gl_chart", "&search=1", so);
    }

    function gl_transactions_get_list(data, so)
    {
        get_list_data_search(data, "gl_transactions", "&search=1", so);
    }

    function payments_get_list(data, so)
    {
        get_list_data_search(data, "payments", "&search=1", so);
    }

    function app_users_get_list(data)
    {
        get_list_data(data, "app_users", 0, 0);
    }
};

/*
local data = [];
local appServer = AppServer.getAppServer();
appServer.credentials("mingote", "tr14pink");
appServer.connect("localhost", 8855);

appServer.sales_tax_rates_get_list(data);
foreach(i, row in data){
	foreach(j, fld in row) print(i, j, fld);
}

appServer.order_types_get_list(data);
foreach(i, row in data){
	foreach(j, fld in row) print(i, j, fld);
}

function OurBizClientAtExit(){
	local as = AppServer.getAppServer();
	as.close();
	print("Bye !");
}
setatexithandler(OurBizClientAtExit);

//print(OurBizClientAtExit, getatexithandler());
*/