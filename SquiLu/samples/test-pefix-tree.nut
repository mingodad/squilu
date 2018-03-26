auto word_list = [==[
account_id
accounts
active
address
batch_order_line_id
buy_description
buy_discount
buy_notes
buy_other_costs
buy_price
buy_quantity_min
buys
cash
_cdate_
city
code
company
contact
country
date
days
delete
description
discount_amt
discount_over_sales
discount_pct
email
entities
entity_address
entity_city
entity_country
entity_id
entity_name
entity_order_number
entity_phone
entity_sales_tax_exempt
entity_state
entity_tax_number
entity_use_sales_tax2
entity_zip
fax
first_total
gps_coordinates
group_id
group_set
id
image
image_id
images
inactive
insert
irpf_pct_retention
is_active
line_subtotal
line_total
lines_count
markup_to_discount
_mdate_
measure_unit_id
mime_type
months
name
notes
only_prices_older
order_by_creation
order_by_modification
order_date
order_id
order_number
order_type_id
order_types
order_valid_till_date
orders
orders_lines
parent_id
payment_type_id
payment_types
periode_count
periode_type
phone
price
price_decimals
price_formula
product_id
products
quantity
query_limit
reference
reference_code
sales
sales_tax1_amt
sales_tax1_pct
sales_tax2_amt
sales_tax2_pct
sales_tax_exempt
sales_tax_id
sell_description
sell_markup
sell_notes
sell_price
sell_price2
sell_quantity_min
sell_without_stock
series
show_on_buys
show_on_sales
show_on_web
show_price_on_web
state
stock_max
stock_min
subtotal_amt
supplier_code
tags
tax_exempt
tax_number
thumbnail
total
total_amt
trigger
unit_weight
units_by_package
update
use_sales_tax2
user_code
warranty_id
web
weeks
weight
weight_total
with_accounts
with_headers
with_images
xref_order_line_id
years
zip
]==];

/*
word_list.gmatch(
	"\"([^\"]+)\"",
	function(m)
	{
		print(m);
		return true;
	}
);
*/

word_list = word_list.split('\n');
word_list.sort();
//print(word_list.join("\n"));

auto prefix_level = array(word_list.len());

auto min(a,b) {return a < b ? a : b;}
auto prev_word;

foreach(idx, word in word_list)
{
	if(idx > 0)
	{
		auto i=0, len = min(prev_word.len(), word.len());
		for(; i < len && (prev_word[i] == word[i]) ; ++i);
		prefix_level[idx] = i;
	}
	else
	{
		prefix_level[idx] = 0;
	}
	prev_word = word;
}

foreach(idx, word in word_list)
{
	//if(prefix_level[idx] == 0)
		print(prefix_level[idx], idx, word);
}

/*
#include "/home/mingo/dev/SquiLu/SquiLu/samples/Trie.nut"

var the_trie = new Trie({});

foreach(word in word_list) the_trie.add(word);

print(the_trie.contains("with_images"));
*/

/*
#include "/home/mingo/dev/SquiLu/SquiLu/samples/perfect-hash.nut"

var mywords = {};
foreach(idx, word in word_list)
{
	mywords[word] <- idx+1;
	//mywords[word] <- word;
}

var ph = perfect_hash_create(mywords);

foreach(k,v in mywords)
{
	print(k,v, perfect_hash_lookup(ph[0], ph[1], k));
}

foreach(k,v in ph[0])
{
	print(k,v);
}

foreach(k,v in ph[1])
{
	print(k,v);
}
*/
