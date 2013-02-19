Jaml.register('OrderEditWindow', function(args) {
	dad.rightClickMenu = "rightClickMenu" + args.newId;
	div({id: dad.rightClickMenu, style:"display:none"},
		table(
			tr(th("Menu")),
			tr(td("Copy")),
			tr(td("Paste"))
		)
	),
	form({id: args.form_id, style: "width:100%;height:100%;", onsubmit:"return false;"}, 
		table({style: "width:100%;height:100%", cls: "editForm"},
			tr({cls:"trvam"},
				td({colspan:8},
					select({name: 'o_order_type_id', style: "width:100%;", id: args.select_ot_id})
				),
				td({colspan:2, width:"2%"}, input({type: "text", name: "o_mdate"}))
			),
			tr({cls:"trvam minw"},
				td(_tr("Series")),
				td( input({type: "text", name: "o_series", size: 3})),
				td(_tr("Number")),
				td(input({type: "text", name: "o_order_number", size: 9})),
				td(_tr("Entity")),
				td(input({type: "text", name: "o_entity_id", size: 6})),
				td({colspan:3, style:"width:90%;"},
					input({type: "text", name: "o_entity_name", style:"width:100%;"})),
				td(input({type: "button", name: "btnSearchEntity", value: "&lt;&gt;", 
						onclick:"onOrderEditSearchEntity(this)"}))
			),
			tr({id: args.tabs_id, "class": "tabs"},
				td({colspan:10},
					button({name:"tab_main" + args.id}, _tr('Main')),
					button({name:"tab_delivery" + args.id}, _tr('Delivery')),
					button({name:"tab_statistics" + args.id}, _tr('Statistics')),
					button({name:"tab_history" + args.id}, _tr('History')),
					button({name:"tab_preview" + args.id}, _tr('Preview')),
					button({name:"tab_totals" + args.id}, _tr('Totals'))
				)
			),
			tr({id:"tab_main" + args.id, 'class':"tabContent"},
				td({colspan:10, style:"height:100%;"},
					table({style: "width:100%;height:100%;"},
						tr({cls:"trvam"},
							td({colspan:3, style: "width:100%;height:12px;"},
								_tr("Lines"), input({type: "text", name: "o_lines_count", size: 4}),
								_tr("Weight"), input({type: "text", name: "o_weight_total", size: 9}),
								_tr("Payment Type"), select({name: "o_payment_type_id"}),
								_tr("Date"), input({type: "text", name: "o_order_date", size: 10}),
								input({type: "button", name: "select_order_date", value: "&lt;&gt;", 
									onclick: "newCalendarChooserWindow()"})
							)
						),
						args.data_table,
						tr(
							td({colspan:2,'class': "editBox", style: "width:100%;height:50px;vertical-align:top;"},
								fieldset({'class': "editBox"},
									table({cls: "fw"},
										tr(
											td({cls: "fw"},
												table({cls: "fw"},
													tr(
														td(_tr("ID")),
														td(_tr("Description")),
														td("&nbsp;")
													),
													tr(
														td({style: "width: 5%;"}, input({type: "text", name: "ol_product_id", size: 6, onfocus:"this.select()"})),
														td(input({type: "text", name: "ol_description", cls: "fw", onfocus:"this.select()"})),
														td({style: "width: 5%;"},input({type: "button", name: "btnSearchProduct", 
															value: "&lt;&gt;", onclick:"onOrderEditSearchProduct(this)"}))
													)
												)
											)
										),
										tr(
											td({cls: "fw"},
												table({cls: "fw"},
													tr(
														td(_tr("Qty.")),
														td(_tr("Weight")),
														td(_tr("Price")),
														td(_tr("1º Total")),
														td(_tr("Disc. %")),
														td(_tr("Disc. $"))
													),
													tr(
														td(input({type: "text", name: "ol_quantity", size: 6, onfocus:"this.select()"})),
														td(input({type: "text", name: "ol_weight", size: 6, onfocus:"this.select()"})),
														td(input({type: "text", name: "ol_price", size: 6, onfocus:"this.select()"})),
														td(input({type: "text", name: "ol_first_total", size: 6})),
														td(input({type: "text", name: "ol_discount_pct", size: 6, onfocus:"this.select()"})),
														td(input({type: "text", name: "ol_discount_amt", size: 6}))
													)
												)
											)
										),
										tr(
											td({cls: "fw"},
												table({cls: "fw"},
													tr(
														td(_tr("Subtotal")),
														td(_tr("Tax II %")),
														td(_tr("Tax I %")),
														td(_tr("Tax I $")),
														td("&nbsp;"),
														td(_tr("Total $"))
													),
													tr(
														td(input({type: "text", name: "ol_line_subtotal", size: 6})),
														td(input({type: "text", name: "ol_sales_tax2_pct", size: 6})),
														td(input({type: "text", name: "ol_sales_tax1_pct", size: 6})),
														td(input({type: "text", name: "ol_sales_tax1_amt", size: 6})),
														td(input({type: "button", name: "btnSaveLine", value: _tr("Save")})),
														td(input({type: "text", name: "ol_line_total", size: 6}))
													)
												)
											)
										)
									)
								),
								Jaml.render('ActionSelect', args), 
								input({id: args.btnAction_id, type: "button", name: "btnAction", value:_tr("Action")}),
								input({type: "button", value:_tr("Numbering")})
							),
							td({'class': "editBox", style: "width:50px;height:1%;vertical-align:top;"},
								fieldset({'class': "editBox"},
									table(
										tr(td(_tr("Subtotal")), td(input({type: "text", name: "o_subtotal_amt", size: 9, 'class': "vright"}))),
										tr(td(_tr("Sales Tax I")), td(input({type: "text", name: "o_sales_tax1_amt", size: 9, 'class': "vright"}))),
										tr(td(_tr("Sales Tax II")), td(input({type: "text", name: "o_sales_tax2_amt", size: 9, 'class': "vright"}))),
										tr(td(_tr("Total")), td(input({type: "text", name: "o_total_amt", size: 9, 'class': "vright"}))),
										tr(td(_tr("Cash")), td("&nbsp;")),
										tr(td(_tr("Return")), td("&nbsp;")),
										tr(
											td("&nbsp;"), 
											td(input({type: "button", id: "btnPrint" + args.id, value:_tr("Print")}))
										)
									)
								)
							)
						)
					)
				)
			),
			tr({id:"tab_delivery" + args.id, 'class':"tabContent"},
				td({colspan:10, style:"height:100%;"},
					table({style: "width:100%;"},
						tr(
							td(_tr("Adderss")),
							td(input({type: "text", name: "o_entity_address", cls: "fw"}))
						),
						tr(
							td(_tr("ZIP")),
							td(
								input({type: "text", name: "o_entity_zip_code"}),
								_tr("City"), input({type: "text", name: "o_entity_city"})
							)
						),
						tr(
							td(_tr("State")),
							td(
								input({type: "text", name: "o_state"}),
								_tr("Country"), input({type: "text", name: "o_country"})
							)
						),
						tr(
							td(_tr("Phone")),
							td(
								input({type: "text", name: "o_phone"}),
								_tr("Tax Number"), input({type: "text", name: "o_tax_number"})
							)
						),
						tr(
							td(_tr("Valid till")),
							td(
								input({type: "text", name: "o_order_valid_till_date"}),
								_tr("Their Number"), input({type: "text", name: "o_entity_order_number"})
							)
						),
						tr(
							td(
								input({type: "checkbox", name: "o_entity_tax_exempt"}), _tr("Tax exempt")
							),
							td(
								input({type: "checkbox", name: "o_entity_use_sales_tax2"}), _tr("Use sales tax2")
							)
						),
						tr(
							td({colspan:2}, 
								_tr("Notes"),
								textarea({rows: 5, name: "o_order_notes", cls: "fw"})
							)
						)
					)
				)
			),
			tr({id:"tab_statistics" + args.id, 'class':"tabContent"},
				td({colspan:10, style:"height:100%;"},
					Jaml.render('StatisticsTabContent', args)
				)
			),
			tr({id:"tab_history" + args.id, 'class':"tabContent"},
				td({colspan:10, style:"height:100%;"},
					Jaml.render('HistoryTabContent', args)
				)
			),
			tr({id:"tab_preview" + args.id, 'class':"tabContent"},
				td({colspan:10, style:"height:100%;"}
					//_tr("Empty")
				)
			),
			tr({id:"tab_totals" + args.id, 'class':"tabTotals"},
				td({colspan:10, style:"height:100%;"},
					table(
						tr(
							td(_tr("Balance")),
							td(input({type: "text", name: "e_balance", cls: "vright"})),
							td("&nbsp;"),
							td(input({type: "button", name: "btnTotalsOverviewRefresh", value: "Refresh"}))
						),
						tr(
							td(_tr("Sales Quoted")),
							td(input({type: "text", name: "t_sales_quoted", cls: "vright"})),
							td(_tr("Buys Quoted")),
							td(input({type: "text", name: "t_buys_quoted", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Ordered")),
							td(input({type: "text", name: "t_sales_ordered", cls: "vright"})),
							td(_tr("Buys Ordered")),
							td(input({type: "text", name: "t_buys_ordered", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Delivered")),
							td(input({type: "text", name: "t_sales_delivered", cls: "vright"})),
							td(_tr("Buys Delivered")),
							td(input({type: "text", name: "t_buys_received", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Invoiced")),
							td(input({type: "text", name: "t_sales_invoiced", cls: "vright"})),
							td(_tr("Buys Invoiced")),
							td(input({type: "text", name: "t_buys_invoiced", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Paid")),
							td(input({type: "text", name: "t_sales_paid", cls: "vright"})),
							td(_tr("Buys Paid")),
							td(input({type: "text", name: "t_buys_paid", cls: "vright"}))
						)
					)					
				)
			)
		)
	);
});

function showOrderEditWindow(all_sales_buys, caller, id){
	var win = newOrderEditWindow(all_sales_buys);
	win.ud.caller = caller;
	win.ud.sab = all_sales_buys;
	var ajax = win.ud.ajaxLoad;
	if(ajax){
		ajax.post("/DB/GetOne", "orders=" + id + "&with_lines=1", 'GET', null);
	}
/*	
	var ajaxLines = win.ud.ajaxLines;
	if(ajaxLines){
		ajaxLines.post('/DB/GetList', 'list=orders&lines=' + id, "GET");
	}
*/	
}

function FillOrderLineEdit(form, row){
	form.product_id.value = row[1].innerHTML;
	form.description.value = row[2].innerHTML;
	form.quantity.value = row[3].innerHTML;
	form.price.value = row[4].innerHTML;
}

function EntityEditWindowOnFillForm(form_field, value) {
	switch(form_field.name){
		case "o_order_type_id":{
			var options = form_field.options;
			for(var i=0; i < options.length; i++){
				if(value == options[i].value) {
					form_field.selectedIndex = i;
					break;
				}
			}
			return true;
		}
		break;
		case "o_weight_total":
			form_field.value = dad.formatNumeric(value);
		break;
		case "o_subtotal_amt":
		case "o_sales_tax1_amt":
		case "o_sales_tax2_amt":
		case "o_total_amt":
		{
			form_field.value = dad.formatCurrency(value);
			return true;
		}
		break;
	}
	return false;
}

function OrderLinesOnTableRowDoubleClick(ev){
	var table = dad.getTableForTableChild(this);
	if(table){
		var win = dad.getWindowForChild(table);
		if(win && win.ud.form) 		FillOrderLineEdit(win.ud.form, this.cells);
	}
	dad.cancelEvent(dad.checkEvent(ev));
	//return true;
}

function OrderLinesOnTableRowClick(ev){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoadLine) {
		var row = this.cells;
		win.ud.ajaxLoadLine.post('/DB/GetOne', 'orders=' + row[1].innerHTML + '&line_calculated=1', "GET");
	}
	dad.cancelEvent(dad.checkEvent(ev));
	//return true;
}

function onOrderEditValidateEntity(win, id, selection){
	win.ud.ajaxLoadEntity.post('/DB/GetOne', 'entities=' + id + '&for_order=1', "GET");
}

function onOrderEditSearchEntity(sender){
	var form = sender.form;
	var search_str = form.o_entity_name.value;
	var win = dad.getWindowForChild(sender);
	var entity_list_search = win.ud.entity_list_search;
	if(!entity_list_search || !entity_list_search.ud.sab){
		win.ud.entity_list_search = entity_list_search = newEntitiesListSearchWindow(win.ud.sab);
	}
	entity_list_search.searchForMe(search_str, win, onOrderEditValidateEntity);
	return false;
}

function onOrderEditValidateProduct(win, id, selection){
	win.ud.ajaxLoadLine.post('/DB/GetOne', 'products=' + id + '&for_order=1', "GET");
	if(selection){
		for(var i in selection){
			alert(selection[i])
		}
	}
}

function onOrderEditSearchProduct(sender){
	var form = sender.form;
	var search_str = form.ol_description.value;
	var win = dad.getWindowForChild(sender);
	var product_list_search = win.ud.product_list_search;
	if(!product_list_search || !product_list_search.ud.sab){
		win.ud.product_list_search = product_list_search = newProductsListSearchWindow(win.ud.sab);
	}
	product_list_search.searchForMe(search_str, win, onOrderEditValidateProduct);
	return false;
}

function onOrderEditCalcLine(){
	//alert(this.value)
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoadLine) {
		var form_prefix = "ol_";
		var values = dad.getFormDataWithPrefix(this.form, form_prefix);
		values.push("__table__=orders");
		values.push("__id__=" + win.ud.form._dbrecord.id);
		values.push("__action__=calc_order_line");
		values.push("trigger=" + this.name.substring(form_prefix.length));
		win.ud.ajaxLoadLine.post('/DB/Action', values.join('&'), "POST");
	}	
}

function onOrderLineProductChange(){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoadLine) {
		win.ud.ajaxLoadLine.post('/DB/GetOne?products=' + 
			win.ud.form.ol_product_id.value + "&for_order=1", "GET");
	}		
}

function newOrderEditWindow(all_sales_buys){
	var title = dad._getSABTitle(all_sales_buys, 'Order Edit');
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var Action_id = 'Action' + newId;
		var btnAction_id = 'btnAction' + newId;
		var table_id = 'table' + newId;
		var table_header_id = 'table_header' + newId;
		var select_ot_id = 'select' + newId;
		var tabs_id = 'tabs' + newId;
		var data = {
			id: newId,
			tabs_id: tabs_id,
			form_id: 'form' + newId,
			select_ot_id: select_ot_id,
			Action_id: Action_id,
			btnAction_id: btnAction_id,
			table_id: table_id,
			table_header_id: table_header_id
		}
		data.table_height = (dad.isOpera || dad.isIE) ? "8em" : "100%";
		data.data_table = Jaml.render('2TRDataTable', data);

		var win = dad.newWindow(newId,220,20, 800, 500, _tr(title), Jaml.render('OrderEditWindow', data));
		//dad.setContentOverflow(newId);
		dad.initTab($(tabs_id));

		var data_table = $(table_id);
		data_table.className = data_table.className + " orders_lines";
		var myform = $(data.form_id);
		
		win.ud.form = myform;
		myform.my_field_prefix = "o_";
		myform.onFillForm = EntityEditWindowOnFillForm;
		dad.setupEditForm(myform);
		
		myform.ol_product_id.onchange = onOrderLineProductChange;
		myform.ol_description.onchange = onOrderLineProductChange;

		var calc_fields_list = ["ol_quantity","ol_price", "ol_discount_pct"];
		for(var i in calc_fields_list){
			myform[calc_fields_list[i]].onchange = onOrderEditCalcLine;
		}
		
		myform.processResponse = function(data){
			var result_array = [];
			sle = dad.StrLenEncoded();
			var processed = sle.sle2vecOfvec(data, data.length, result_array);
			var lines = [];
			sle.sle2vecOfvec(data, data.length, lines, processed);
			if(lines.length > 0) lines.shift();
			dad.fillTableById(lines, newId);
			return result_array;
		}

		dad.setupBarchart(newId, function(){
			return 'list=orders&statistics=1&sab=' + win.ud.sab;
		});

		var mytable = $(table_id);
		mytable.my_record_header = ['id|ID|0', 'product_id|Code|6|R',
			'description|Description|-1', 'quantity|Quantity|8|R|N',
			'price|Price|8|R|M', 'first_total|1st Total|8|R|M'];
		mytable.row_click_cb = OrderLinesOnTableRowClick;
		mytable._onDelete_cb = function(sender){
			var tr = dad.getFirstParentWithTagName( sender, "TR" );
			dad.formFocusNext(sender);
			tr.parentNode.removeChild(tr);
		}
		
		var btnPrint = $("btnPrint" + newId);
		btnPrint.onclick = function(){
			var url = '/DB/GetOne?orders=' + this.form._dbrecord.id + '&pdf=1';
			window.open(url, "printPDF");
		}

		var ajaxOrderType = new dad.Ajax(function(){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText); 
				var select = $(select_ot_id);
				dad.fillSelectByRecords(select, records, false);
			}
		});
		if(ajaxOrderType){
			ajaxOrderType.post('/DB/GetList', 'list=order_types&short_list=1&group_order=' + 
				all_sales_buys, "GET");
		}

		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);
		win.ud.ajaxLines = dad.newAjaxDataTableAjax(win, newId);
		win.ud.ajaxLoadLine = new dad.Ajax(function(){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var record = dad.parseSLEData2Object(this.responseText); 
				var form = $(data.form_id);
				dad.formFillByRecord(form, record, "ol_");
				if(!record.__table__) form.ol_quantity.focus();
			} else {
				alert("An error has occured making the request");
			}
		}, null, false);
		
		win.ud.ajaxLoadEntity = new dad.Ajax(function(id){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var record = dad.parseSLEData2Object(this.responseText);
				var form = $(data.form_id);
				dad.fillFormWithExistingFields(form, record);
				form.ol_description.focus();
			} else {
				alert("An error has occured making the request");
			}
		}, null, false);

		var ajaxAuxTables = new dad.Ajax(function(select){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText); 
				dad.fillSelectByRecords(select, records, true);
			}
		}, myform.o_payment_type_id);
		if(ajaxAuxTables) ajaxAuxTables.post('/DB/GetList', 'list=payment_types&short_list=1', "GET");

		var btn = myform.btnTotalsOverviewRefresh;
		btn.ajaxLoadTotals = new dad.Ajax(function(){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var record = dad.parseSLEData2Object(this.responseText); 
				var form = $(data.form_id);
				dad.formFillByRecord(form, record, "t_");
			} else {
				alert("An error has occured making the totals request");
			}
		}, null, false);
		btn.onclick = function(){
			if(this.ajaxLoadTotals) 
				this.ajaxLoadTotals.post('/DB/GetOne?orders=1&totals_overview=1', "GET");
		}		
	}
	dad.bringToFront(win);
	return win;
}

Jaml.register('OrdersListSearchOn', function(args) {
	input({type: "radio", name: "search_on", id: "by_entity" + args.id, value: "entity", checked:1 }),
	label({'for': "by_entity" + args.id},  _tr("Entity")),
	input({type: "radio", name: "search_on", id: "by_notes" + args.id, value: "notes"}),
	label({'for': "by_notes" + args.id},  _tr("Notes")),
	input({type: "radio", name: "search_on", id: "by_products" + args.id, value: "products"}),
	label({'for': "by_products" + args.id},  _tr("Products")),
	input({type: "radio", name: "search_on", id: "by_date" + args.id, value: "date"}),
	label({'for': "by_date" + args.id},  _tr("Date")),
	input({type: "radio", name: "search_on", id: "by_total" + args.id, value: "total"}),
	label({'for': "by_total" + args.id},  _tr("Total"))
});

function newOrdersListSearchWindow(all_sales_buys){
	var colHeaders = ["id|ID|6|R",
                "order_date|Date|8",
                "order_type_code|Type|4",
                "series|Series|5",
                "order_number|Number|7",
                "entity_name|Entity|-1",
                "total_amt|Total|8|R|M",
                "payment_type_code|P.T.|4|C"];
	var title = dad._getSABTitle(all_sales_buys, 'Orders List/Search');
	var win = newListSearchWindow(all_sales_buys, title, colHeaders, showOrderEditWindow,
		"orders", 'OrdersListSearchOn', null, all_sales_buys);
	var data_table = $("table" + win.ud.win_id);
	data_table.className = data_table.className + " orders_list";
	
	var group_filter = $('group_filter' + win.ud.win_id);
	if(group_filter.options.length == 0){
		var ajaxOrderType = new dad.Ajax(function(id){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText); 
				dad.fillSelectByRecords(group_filter, records, true);
			}
		}, win.ud.win_id, false);
		if(ajaxOrderType){
			ajaxOrderType.post('/DB/GetList', 'list=order_types&short_list=1&group_order=' + 
				all_sales_buys, "GET");
		}
	}
		
	return win;
}
