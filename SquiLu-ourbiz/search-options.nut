/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
class OurBizSearchOptions
{
    search_str = null;
    select_fields = null;
    group_id = null;
    product_id = null;
    entity_id = null;
    order_id = null;
    payment_type_id = null;
    query_limit = null;
    image_id = null;
    account_id = null;
    active = null;
    cdate = null;
    contact = null;
    id = null;
    inactive = null;
    mdate = null;
    date = null;
    name = null;
    description = null;
    notes = null;
    phone = null;
    products = null;
    entities = null;
    buys = null;
    sales = null;
    only_prices_older = null;
    order_by_creation = null;
    order_by_modification = null;
    reference = null;
    total = null;
    group_set = null;
    with_images = null;
    with_headers = null;
    with_accounts = null;
    code = null;
    headers = null;
    accounts = null;

    constructor()
    {
        reset();
    }
    function reset()
    {
        search_str = "";
        select_fields = "";
        active =
            cdate =
                buys =
                    contact =
                        id =
                            inactive =
                                mdate = date = total = group_set = false;
        name = notes = phone =
                           products = entities = description = sales = false;
        only_prices_older = order_by_creation = order_by_modification =
            reference = with_images = false;
        with_headers = with_accounts = code = false;
        group_id = 0;
        product_id = entity_id = order_id = payment_type_id = image_id = account_id = 0;
        query_limit = 50;
    }

    function getPostOptions()
    {
        local ar = ["search_str="];
        if(search_str.size()){
		ar.push(url_encode(search_str));
        }
        if(select_fields.size()){
		ar.push("&select_fields=");
		ar.push(url_encode(select_fields));
        }
	foreach(k,v in OurBizSearchOptions){
		local val = this[k];
		local ktype = type(val);
		if(ktype == "integer") {
			if(val) ar.push(format("&%s=%d", k, val));
		}
		else if(ktype == "bool"){
			if(val) ar.push(format("&%s=1", k));
		}
	}
        return ar.concat();
    }

    function getOptionsFromMap(map)
    {
        search_str = table_rawget(map, "search_str", "");
        select_fields = table_rawget(map, "select_fields", "");
        search_on = table_rawget(map, "search_on", "1");

	foreach(k,v in this){
		local ktype = type(v);
		if(ktype == "integer") {
			local value = table_get(map, k, null);
			if(value != null) this[k] = value.tointeger();
		}
		else if(ktype == "bool"){
			this[k] = table_get(map, k, null) != null;
		}
	}
    }
};
