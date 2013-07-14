Jaml.register('ProductEditWindow', function(args) {
	form({action: "/DB/Action", method: "post", onsubmit:"return false;",
			id: args.form_id, style: "width:100%;height:100%;"}, 
		table({style: "width:100%;height:100%;", cls: "editForm"},
			tr(
				td({style: "width:12px;"},
					table({style: "width:100%;"},
						tr(
							td({style: "width:1%;"},input({type: "text", name: "p_id", size: 8})),
							td(input({type: "text", name: "p_sell_description", style: "width:99%;"})),
							td({style: "width:1%;"},input({type: "text", name: "p_mdate", size: 9}))
						)
					)
				)
			),
			tr({id: args.main_tabs_id, "class": "tabs"},
				td(
					button({name:"tab_main" + args.id}, _tr('Main')),
					button({name:"tab_prices" + args.id}, _tr('Prices')),
					button({name:"tab_kit" + args.id}, _tr('Kit')),
					button({name:"tab_statistics" + args.id}, _tr('Statistics')),
					button({name:"tab_history" + args.id}, _tr('History')),
					button({name:"tab_reports" + args.id}, _tr('Reports')),
					button({name:"tab_totals" + args.id}, _tr('Totals'))
				)
			),
			tr({id:"tab_main" + args.id, 'class':"tabContent"},
				td(
					table({style: "width:100%;"},
						tbody({cls:"firstRight"},
							tr({cls:"fcr"},
								td(_tr("Reference")),
								td(input({type: "text", name: "p_reference_code", cls:"size9"})),
								td(_tr("Ref. Supp.")),
								td(input({type: "text", name: "p_supplier_code", cls:"size9"})),
								td(_tr("Barcode")),
								td(input({type: "text", name: "p_bar_code", cls:"size9"}))
							),
							tr({cls:"fcr"},
								td(_tr("Description")),
								td({colspan:5}, input({type: "text", name: "p_sell_description", cls: "fw"}))
							),
							tr({cls:"fcr"},
								td(_tr("Des. Supp.")),
								td({colspan:5}, input({type: "text", name: "p_buy_description", cls: "fw"}))
							),
							tr({cls:"fcr"},
								td(_tr("Sales Price")),
								td(input({type: "text", name: "p_sell_price", cls:"size9"})),
								td(_tr("Measure Unit")),
								td(select({type: "text", name: "p_measure_unit_id", cls:"size9"})),
								td({colspan:2, rowspan:6}, img({id: 'prod_img_' +args.id}))
							),
							tr({cls:"fcr"},
								td(_tr("Sales Tax")),
								td(select({type: "text", name: "p_sales_tax_id", cls:"size9"})),
								td(_tr("Warranty")),
								td(select({type: "text", name: "p_warranty_id", cls:"size9"}))
							),
							tr({cls:"fcr"},
								td(_tr("Onhand")),
								td(input({type: "text", name: "p_quantity_onhand", cls:"size9"})),
								td(_tr("Weight")),
								td(input({type: "text", name: "p_weight", cls:"size9"}))
							),
							tr({cls:"fcr"},
								td(_tr("Reserved")),
								td(input({type: "text", name: "p_quantity_reserved", cls:"size9"})),
								td(_tr("Sell Min. Qty.")),
								td(input({type: "text", name: "p_sell_quantity_min", cls:"size9"}))
							),
							tr(
								td({colspan:2}, input({type: "checkbox", name: "p_show_on_sales"}),_tr("Show on Sales")),
								td( input({type: "checkbox", name: "p_show_on_web"}),_tr("WEB"))
							),
							tr(
								td({colspan:2}, input({type: "checkbox", name: "p_show_on_buys"}),_tr("Show on Buys")),
								td(input({type: "checkbox", name: "p_show_price_on_web"}),_tr("WEB Price"))
							)
						),
						tbody(
							tr({id:args.notes_tabs_id, 'class':"tabs"},
								td({colspan:4},
									button({name:"tab_notes" + args.id}, _tr('Sales Notes')),
									button({name:"tab_notes_supplier" + args.id}, _tr('Buy Notes')),
									button({name:"tab_tags" + args.id}, _tr('Tags')),
									button({name:"tab_groups" + args.id}, _tr('Groups'))
								),
								td({colspan:2},
									Jaml.render('ActionSelect', args),
									input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")})
								)
							),
							tr({id:"tab_notes" + args.id, 'class':"tabContent"},
								td({colspan:6},textarea({rows: 7, name: "p_sell_notes", cls: "fw"}))
							),
							tr({id:"tab_notes_supplier" + args.id, 'class':"tabContent"},
								td({colspan:6},textarea({rows: 7, name: "p_buy_notes", cls: "fw"}))
							),
							tr({id:"tab_tags" + args.id, 'class':"tabContent"},
								td({colspan:6},textarea({rows: 7, name: "p_tags", cls: "fw"}))
							),
							tr({id:"tab_groups" + args.id, 'class':"tabContent"},
								td({colspan:6},textarea({rows: 7, name: "p_notes", cls: "fw"}))
							)
						)
					)
				)
			),
			tr({id:"tab_prices" + args.id, 'class':"tabContent"},
				td({align:"center"},
					table(
						tr(
							td(_tr("Buy $")),
							td(_tr("Buy Disc. %")),
							td(_tr("Other Costs $")),
							td(_tr("Markup %")),
							td(_tr("Margin %")),
							td(_tr("Sales $"))
						),
						tr(
							td(input({type: "text", name: "p_buy_price", cls:"size9"})),
							td(input({type: "text", name: "p_buy_discount", cls:"size9"})),
							td(input({type: "text", name: "p_buy_other_costs", cls:"size9"})),
							td(input({type: "text", name: "p_sell_markup", cls:"size9"})),
							td(input({type: "text", name: "p_sell_margin", cls:"size9"})),
							td(input({type: "text", name: "p_sell_price2", cls:"size9"}))
						),
						tr(
							td({rowspan:3, colspan:4},
								_tr("Price Formula"),
								br(),
								textarea({rows: 3, name: "p_price_formula", cls: "fw"})
							),
							td(_tr("Stock Min.")),
							td(input({type: "text", name: "p_stock_min", cls:"size9"}))
						),
						tr(
							td(_tr("Stock Max.")),
							td(input({type: "text", name: "p_stock_max", cls:"size9"}))
						),
						tr(
							td(_tr("Buy Min. Qty")),
							td(input({type: "text", name: "p_buy_quantity_min", cls:"size9"}))
						)
					)
				)
			),
			tr({id:"tab_kit" + args.id, 'class':"tabContent"},
				td(
					_tr("Empty")
				)
			),
			tr({id:"tab_statistics" + args.id, 'class':"tabContent"},
				td(
					Jaml.render('StatisticsTabContent', args)
				)
			),
			tr({id:"tab_history" + args.id, 'class':"tabContent"},
				td(
					Jaml.render('HistoryTabContent', args)
				)
			),
			tr({id:"tab_reports" + args.id, 'class':"tabContent"},
				td(
					button({id:"rptProductsList" + args.id}, _tr('Products List'))
				)
			),
			tr({id:"tab_totals" + args.id, 'class':"tabTotals"},
				td({style:"height:100%;"},
					table(
						tr(
							td(_tr("Quantity quoted sales")),
							td(input({type: "text", name: "p_quantity_quoted_sales", cls: "vright"})),
							td(_tr("Quantity quoted buys")),
							td(input({type: "text", name: "p_quantity_quoted_buys", cls: "vright"}))
						),
						tr(
							td(_tr("Quantity ordered sales")),
							td(input({type: "text", name: "p_quantity_ordered_sales", cls: "vright"})),
							td(_tr("Quantity ordered buys")),
							td(input({type: "text", name: "p_quantity_ordered_buys", cls: "vright"}))
						),
						tr(
							td(_tr("Quantity delivered sales")),
							td(input({type: "text", name: "p_quantity_delivered", cls: "vright"})),
							td(_tr("Quantity received buys")),
							td(input({type: "text", name: "p_quantity_received", cls: "vright"}))
						),
						tr(
							td(_tr("Quantity invoiced sales")),
							td(input({type: "text", name: "p_quantity_invoiced_sales", cls: "vright"})),
							td(_tr("Quantity invoiced buys")),
							td(input({type: "text", name: "p_quantity_invoiced_buys", cls: "vright"}))
						),
						tr(
							td(_tr("Quantity lost")),
							td({colspan:3}, input({type: "text", name: "p_quantity_lost", cls: "vright"}))
						)
					)
				)
			)
		)
	);
});

function ProductEditWindowRefresh(ajax) {
}

function showProductEditWindow(all_sales_buys, caller, id){
	var win = newProductEditWindow(all_sales_buys);
	win.ud.caller = caller;
	win.ud.edit_id = id;
	var ajax = win.ud.ajaxLoad;
	if(ajax){
		ajax.post("/DB/GetOne", "products=" + id, 'GET', null);
	}
}	

function ProductEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(ProductEditWindowRefresh, this.form, "products")
}

function onProductEditCalcPrice(){
	//alert(this.value)
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxCalcPrice) {
		var form_prefix = "p_";
		var values = dad.getFormDataWithPrefix(this.form, form_prefix, true);
		values.push("__table__=products");
		values.push("__id__=" + win.ud.edit_id);
		values.push("__action__=calc_product_price");
		values.push("trigger=" + this.name.substring(form_prefix.length));
		win.ud.ajaxCalcPrice.post('/DB/Action', values.join('&'), "POST");
	}	
}

Jaml.register('ProductsHistoryOptions', function(args) {
	option({value:""}, "----");
	option(_tr("Sales by Date"));
	option(_tr("Sales by Amount"));
	option(_tr("Entities that Bought"));
	option(_tr("Entities that Bought by Value"));
	option(_tr("Entities that Bought by Quantity"));
	option(_tr("Price History"));
	option(_tr("Price History All"));
});

function newProductEditWindow(all_sales_buys){
	var title = dad._getSABTitle(all_sales_buys, 'Product Edit');
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var Action_id = 'Action' + newId;
		var btnAction_id = 'btnAction' + newId;
		var table_history_id = 'table_history' + newId;
		var table_history_header_id = 'table_history_header' + newId;
		var main_tabs_id = 'main_tabs' + newId;
		var notes_tabs_id = 'notes_tabs' + newId;
		var data = {
			id: newId,
			main_tabs_id: main_tabs_id,
			notes_tabs_id: notes_tabs_id,
			form_id: 'form' + newId,
			Action_id: Action_id,
			btnAction_id: btnAction_id,
			table_history_id: table_history_id,
			table_history_header_id: table_history_header_id,
			history_options: Jaml.render('ProductsHistoryOptions', data)
		}
		var win = dad.newWindow(newId,220,20, 800, 500, _tr(title), Jaml.render('ProductEditWindow', data));
		//dad.setContentOverflow(newId);
		dad.initTab($id(main_tabs_id));
		dad.initTab($id(notes_tabs_id));
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		var choiceHistory = $id('history_type_id' + newId);
		choiceHistory.ajaxLoadHistory = dad.newAjaxDataTableAjax(win, 
				{id:newId, table:table_history_id, table_header:table_history_header_id});
		choiceHistory.onchange = function(){
			if(!this.selectedIndex) return;
			this.ajaxLoadHistory._withHeaders = true;
			this.ajaxLoadHistory.post('/DB/GetList', 'list=products&history=' + 
					win.ud.edit_id + '&htype=' + this.selectedIndex +
					'&query_limit=' + this.form.history_query_limit.value, "GET");
		}

		dad.setupBarchart(newId, function(){
			return 'list=products&statistics=' + win.ud.edit_id + '&sab=' + win.ud.sab;;
		});

		var myform = $id(data.form_id);
		myform.my_field_prefix = "p_";
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		myform.afterFill = function(){
			var img_id = this._dbrecord.image_id;
			var img = $id('prod_img_' + newId);
			dad.getImageForImg(img, img_id);
		}

		var calc_fields_list = ["p_buy_price","p_buy_discount", "p_buy_other_costs", 
			"p_sell_markup", "p_sell_margin", "p_sell_price",  "p_sell_price2"];
		for(var i in calc_fields_list){
			myform[calc_fields_list[i]].onchange = onProductEditCalcPrice;
		}
		
		var btnRptProductsList = $id("rptProductsList" + newId);
		btnRptProductsList.onclick = function(){
			var url = '/DB/GetList?list=products&pdf=1';
			window.open(url, "printPDF");
		}

		var ajaxAuxTables = new dad.Ajax(function(select){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText);
				dad.fillSelectByRecords(select, records, true, "");
			}
		}, myform.p_measure_unit_id);
		if(ajaxAuxTables) ajaxAuxTables.post('/DB/GetList', 'list=measure_units&short_list=1', "GET");

		ajaxAuxTables = new dad.Ajax(function(select){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText);
				dad.fillSelectByRecords(select, records, true);
			}
		}, myform.p_sales_tax_id);
		if(ajaxAuxTables) ajaxAuxTables.post('/DB/GetList', 'list=sales_tax_rates&short_list=1', "GET");

		ajaxAuxTables = new dad.Ajax(function(select){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText); 
				dad.fillSelectByRecords(select, records, true, "");
			}
		}, myform.p_warranty_id);
		if(ajaxAuxTables) ajaxAuxTables.post('/DB/GetList', 'list=warranty_types&short_list=1', "GET");

		win.ud.ajaxCalcPrice = new dad.Ajax(function(){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var record = dad.parseSLEData2Object(this.responseText); 
				var form = $id(data.form_id);
				dad.formFillByRecord(form, record, "p_");
			} else {
				alert("An error has occured making the request");
			}
		}, null, false);
		
		var btn = $id(btnAction_id);
		btn.onclick = ProductEditWindowOnSubmit;
	}
	dad.bringToFront(win);
	return win;
}

Jaml.register('ProductsListSearchOn', function(args) {
	input({type: "radio", name: "search_on", id: "by_description" + args.id, value: "description", checked:1 }),
	label({'for': "by_description" + args.id},  _tr("Description")),
	input({type: "radio", name: "search_on", id: "by_notes" + args.id, value: "notes"}),
	label({'for': "by_notes" + args.id},  _tr("Notes")),
	input({type: "radio", name: "search_on", id: "by_reference" + args.id, value: "reference"}),
	label({'for': "by_reference" + args.id},  _tr("Reference")),
	input({type: "radio", name: "search_on", id: "by_entity" + args.id, value: "entity"}),
	label({'for': "by_entity" + args.id},  _tr("Entity")),
	input({type: "radio", name: "search_on", id: "by_id" + args.id, value: "id"}),
	label({'for': "by_id" + args.id},  _tr("ID"))
});

function newProductsListSearchWindow(all_sales_buys){
	var colHeaders = ["id|ID|6|R",
                "reference_code|Reference|9",
                "sell_description|Description|-1",
                "kit|Kit|3|C",
                "price_taxed|Price+V.A.T.|9|R|M",
                "price|Price|0|R|M",
                "quantity_onhand|Onhand|6|R|N",
                "is_active|Active|4|C|B",
                "price_date|Price Date|0",
                "image_id|image_id|0"];
	var title = dad._getSABTitle(all_sales_buys, 'Products List/Search');
	var win = newListSearchWindow(all_sales_buys, title, colHeaders, showProductEditWindow,
		"products", 'ProductsListSearchOn', null, all_sales_buys);
	var myId = win.ud.win_id;
	var ltbl =  $id('table' +myId);
	ltbl.row_over_cb =  function(evt){
		var img_id = this.cells[10].innerHTML;
		var img = $id('ls_img_' + myId);
		dad.getImageForImg(img, img_id, true);
	};
	return win;
}
