Jaml.register('EntityEditWindow', function(args) {
	form({action: "/DB/Action", method: "post", onsubmit:"return false;",
			id: args.form_id, style: "width:100%;height:100%;"}, 
		table({style: "width:100%;height:100%;", cls: "editForm"},
			tr(
				td({style: "height:12px;"},
					table({style: "width:100%;"},
						tr(
							td({style: "width:1%;"},input({type: "text", name: "e_id", size: 8})),
							td(input({type: "text", name: "e_name", style: "width:99%;"})),
							td({style: "width:1%;"},input({type: "text", name: "e_mdate", size: 9}))
						)
					)
				)
			),
			tr({id: args.main_tabs_id, 'class':"tabs"},
				td(
					button({name:"tab_main" + args.id}, _tr('Main')),
					button({name:"tab_totals" + args.id}, _tr('Totals')),
					button({name:"tab_statistics" + args.id}, _tr('Statistics')),
					button({name:"tab_history" + args.id}, _tr('History')),
					button({name:"tab_reports" + args.id}, _tr('Reports'))
				)
			),
			tr({id:"tab_main" + args.id, 'class':"tabContent"},
				td(
					table({style: "width:100%;"},
						tbody({cls:"firstRight"},
							tr({cls:"fcr"},
								td(_tr("Name")),
								td({colspan:4},input({type: "text", name: "e_name", cls: "fw"}))
							),
							tr({cls:"fcr"},
								td(_tr("Company")),
								td({colspan:4},input({type: "text", name: "e_company", cls: "fw"}))
							),
							tr({cls:"fcr"},
								td(_tr("Adderss")),
								td({colspan:4},input({type: "text", name: "e_address", cls: "fw"}))
							),
							tr({cls:"fcr"},
								td(_tr("ZIP")),
								td(input({type: "text", name: "e_zip"})),
								td( _tr("City")),
								td({colspan:2},
									input({type: "text", name: "e_city", cls:"fw"}))
							),
							tr({cls:"fcr"},
								td(_tr("State")),
								td({colspan:2}, input({type: "text", name: "e_state"})),
								td(_tr("Country")),
								td(input({type: "text", name: "e_country", style: "width:98%;"}))
							),
							tr({cls:"fcr"},
								td(_tr("Tax Nº")),
								td(input({type: "text", name: "e_tax_number"})),
								td({colspan:2},
									input({type: "checkbox", name: "e_show_on_sales"}),_tr("Sales"),
									input({type: "checkbox", name: "e_show_on_buys"}),_tr("Buys")
								),
								td(input({type: "checkbox", name: "e_is_active"}),_tr("Active"))
							),
							tr({cls:"fcr"},
								td(_tr("Phone")),
								td(input({type: "text", name: "e_phone"})),
								td(_tr("Fax")),
								td(input({type: "text", name: "e_fax"})),
								td({style: "white-space: nowrap;", colspan:2},
									input({type: "checkbox", name: "e_use_sales_tax2"}),_tr("Use Sales Tax2"))
							),
							tr({cls:"fcr"},
								td(_tr("Email")),
								td({colspan:3},input({type: "text", name: "e_email", cls: "fw"})),
								td(input({type: "checkbox", name: "e_sales_tax_exempt"}),_tr("Sales Tax Exempt"))
							),
							tr({cls:"fcr"},
								td(_tr("Web")),
								td({colspan:2},input({type: "text", name: "e_web", cls: "fw"})),
								td({colspan:2},_tr("GPS"), input({type: "text", name: "e_gps_coordinates"}))
							),
							tr({cls:"fcr"},
								td(_tr("Contact")),
								td({colspan:4},input({type: "text", name: "e_contact", cls: "fw"}))
							)
						),
						tbody(
							tr({id:args.notes_tabs_id, 'class':"tabs"},
								td({colspan:4},
									button({name:"tab_notes" + args.id}, _tr('Notes')),
									button({name:"tab_tags" + args.id}, _tr('Tags')),
									button({name:"tab_groups" + args.id}, _tr('Groups'))
								),
								td(
									Jaml.render('ActionSelect', args),
									input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")})
								)
							),
							tr({id:"tab_notes" + args.id, 'class':"tabContent"},
								td({colspan:5},textarea({rows: 5, name: "e_notes", cls: "fw"}))
							),
							tr({id:"tab_tags" + args.id, 'class':"tabContent"},
								td({colspan:5},textarea({rows: 5, name: "e_tags", cls: "fw"}))
							),
							tr({id:"tab_groups" + args.id, 'class':"tabContent"},
								td({colspan:5},textarea({rows: 5, name: "e_notes", cls: "fw"}))
							)
						)
					)
				)
			),
			tr({id:"tab_totals" + args.id, 'class':"tabTotals"},
				td({style:"height:100%;"},
					table(
						tr(
							td(_tr("Credit")),
							td({colspan:3}, input({type: "text", name: "e_credit_allowed", cls: "vright"}))
						),
						tr(
							td(_tr("Balance")),
							td({colspan:3}, input({type: "text", name: "e_balance", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Quoted")),
							td(input({type: "text", name: "e_sales_quoted", cls: "vright"})),
							td(_tr("Buys Quoted")),
							td(input({type: "text", name: "e_buys_quoted", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Ordered")),
							td(input({type: "text", name: "e_sales_ordered", cls: "vright"})),
							td(_tr("Buys Ordered")),
							td(input({type: "text", name: "e_buys_ordered", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Delivered")),
							td(input({type: "text", name: "e_sales_delivered", cls: "vright"})),
							td(_tr("Buys Delivered")),
							td(input({type: "text", name: "e_buys_received", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Invoiced")),
							td(input({type: "text", name: "e_sales_invoiced", cls: "vright"})),
							td(_tr("Buys Invoiced")),
							td(input({type: "text", name: "e_buys_invoiced", cls: "vright"}))
						),
						tr(
							td(_tr("Sales Paid")),
							td(input({type: "text", name: "e_sales_paid", cls: "vright"})),
							td(_tr("Buys Paid")),
							td(input({type: "text", name: "e_buys_paid", cls: "vright"}))
						)
					)
				)
			),
			tr({id:"tab_statistics" + args.id, 'class':"tabContent"},
				td({style:"height:100%;"},
					Jaml.render('StatisticsTabContent', args)
				)
			),
			tr({id:"tab_history" + args.id, 'class':"tabContent"},
				td({style:"height:100%;"},
					Jaml.render('HistoryTabContent', args)
				)
			),
			tr({id:"tab_reports" + args.id, 'class':"tabContent"},
				td({style:"height:100%;"},
					button({id:"rptEntitiesList" + args.id}, _tr('Entities List'))
				)
			)
		)
	);
});

function EntityEditWindowRefresh(ajax) {
}

function showEntityEditWindow(all_sales_buys, caller, id){
	var win = newEntityEditWindow(all_sales_buys);
	win.ud.caller = caller;
	win.ud.edit_id = id;
	var ajax = win.ud.ajaxLoad;
	if(ajax){
		ajax.post("/DB/GetOne", "entities=" + id, 'GET', null);
	}
}

function EntityEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(EntityEditWindowRefresh, this.form, "entities")
}

function EntityEditWindowOnFillForm(form_field, value) {
	if(form_field.name == "entity_type"){
		var options = form_field.options;
		for(var i=0; i < options.length; i++){
			if(value == options[i].value) {
				form_field.selectedIndex = i;
				break;
			}
		}
		return true;
	}
	return false;
}

Jaml.register('EntitiesHistoryOptions', function(args) {
	option({value:""}, "----");
	option(_tr("Sales by Date"));
	option(_tr("Sales by Amount"));
	option(_tr("Products Bought"));
	option(_tr("Products Bought by Value"));
	option(_tr("Products Bought by Quantity"));
});

function newEntityEditWindow(all_sales_buys){
	var title = dad._getSABTitle(all_sales_buys, 'Entity Edit');

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
			history_options: Jaml.render('EntitiesHistoryOptions', data)
		}
		var win = dad.newWindow(newId,220,20, 800, 500, _tr(title), Jaml.render('EntityEditWindow', data));
		//dad.setContentOverflow(newId);
		dad.initTab($(main_tabs_id));
		dad.initTab($(notes_tabs_id));
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		var choiceHistory = $('history_type_id' + newId);
		choiceHistory.ajaxLoadHistory = dad.newAjaxDataTableAjax(win, 
				{id:newId, table:table_history_id, table_header:table_history_header_id});
		choiceHistory.onchange = function(){
			if(!this.selectedIndex) return;
			this.ajaxLoadHistory._withHeaders = true;
			this.ajaxLoadHistory.post('/DB/GetList', 'list=entities&history=' + 
					win.ud.edit_id + '&htype=' + this.selectedIndex +
					'&query_limit=' + this.form.history_query_limit.value, "GET");
		}

		dad.setupBarchart(newId, function(){
			return 'list=entities&statistics=' + win.ud.edit_id + '&sab=' + win.ud.sab;
		});
		
		var myform = $(data.form_id);
		myform.my_field_prefix = "e_";
		myform.onFillForm = EntityEditWindowOnFillForm;
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		
		var btnRptEntitiesList = $("rptEntitiesList" + newId);
		btnRptEntitiesList.onclick = function(){
			var url = '/DB/GetList?list=entities&pdf=1';
			window.open(url, "printPDF");
		}
		var btn = $(btnAction_id);
		btn.onclick = EntityEditWindowOnSubmit;
	}
	
	dad.bringToFront(win)
	return win;
}

Jaml.register('EntitiesListSearchOn', function(args) {
	input({type: "radio", name: "search_on", id: "by_name" + args.id, value: "name", checked:1 }),
	label({'for': "by_name" + args.id},  _tr("Name")),
	input({type: "radio", name: "search_on", id: "by_notes" + args.id, value: "notes"}),
	label({'for': "by_notes" + args.id},  _tr("Notes")),
	input({type: "radio", name: "search_on", id: "by_products" + args.id, value: "products"}),
	label({'for': "by_products" + args.id},  _tr("Products")),
	input({type: "radio", name: "search_on", id: "by_id" + args.id, value: "id"}),
	label({'for': "by_id" + args.id},  _tr("ID"))
});

function newEntitiesListSearchWindow(all_sales_buys){
	var colHeaders = ["id|ID|6|R",
                "name|Name|-1",
                "contact|Contact|12",
                "phone|Phone|12",
                "is_active|Active|0"];
	var title = dad._getSABTitle(all_sales_buys, 'Entities List/Search');
	return newListSearchWindow(all_sales_buys, title, colHeaders, showEntityEditWindow,
		"entities", 'EntitiesListSearchOn', null, all_sales_buys);
}
