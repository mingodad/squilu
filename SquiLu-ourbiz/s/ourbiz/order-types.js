Jaml.register('OrderTypesListEdit', function(args) {
	var newIdBy2 = dad.newIdBy2calls;
	form({action: "/DB/Action", method: "post", id: args.form_id, style: "width:100%;height:100%;"},
		table({style: "width:100%;height:100%;"},
			args.data_table,
			tr(
				td({'class': "editBox", style: "width:100%;"},
					fieldset(legend(_tr("Edit Form")),
						table(
							tr(
								td(_tr("ID")), td(_tr("Code")), td(_tr("Print desc.")),
								td(_tr("Series")), td(_tr("Number")), td(_tr("Grp. 1")),
								td(_tr("Grp. 2"))
							),
							tr(
								td(input({type: "input", name: "ot_id", size: 6, readonly:"readonly"})), 
								td(input({type: "input", name: "ot_code", size: 5})), 
								td(input({type: "input", name: "ot_description_to_print", size: 20})),
								td(input({type: "input", name: "ot_series", size: 5})),
								td(input({type: "input", name: "ot_numbering", size: 12})),
								td(input({type: "input", name: "ot_group_order", size: 6})),
								td(input({type: "input", name: "ot_subgroup_order", size: 6}))
							)
						),
						table({style: "width:100%;"},
							tr(
								td({colspan: 3},
									input({type: "checkbox", name: "ot_is_active", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Active")),
									input({type: "checkbox", name: "ot_with_sales_tax1", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Sales Tax")),
									input({type: "checkbox", name: "ot_with_sales_tax_included", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Sales Tax Included")),
									input({type: "checkbox", name: "ot_show_sales_tax", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Show Sales Tax")),
									input({type: "checkbox", name: "ot_show_prices", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Show Prices"))
								)
							),
							tr(
								td(_tr("Description")), 
								td(
									input({type: "input", name: "ot_with_quote", size: 1, cls:"bold"}),
									_tr("With Quote")
								), 
								td(
									input({type: "checkbox", name: "ot_is_expense", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Is Expense"))
								)
							),
							tr(
								td(input({type: "input", style: "width:98%;", name: "ot_description"})), 
								td(
									input({type: "input", name: "ot_with_order", size: 1, cls:"bold"}),
									_tr("With Order")
								), 
								td(
									input({type: "checkbox", name: "ot_is_income", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Is Income"))
								)
							),
							tr(
								td(_tr("Notes")), 
								td(
									input({type: "input", name: "ot_with_inventory", size: 1, cls:"bold"}),
									_tr("With Inventory")
								), 
								td(Jaml.render('ActionSelect', args))
							),
							tr(
								td({rowspan:3, style: "width:60%;"},textarea({rows: 5, name: "ott_notes", cls: "fw"})),
								td(
									input({type: "input", name: "ot_with_payment", size: 1, cls:"bold"}),
									_tr("With Payment")
								), 
								td(input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")}))
							),
							tr(
								td(
									input({type: "input", name: "ot_with_credit", size: 1, cls:"bold"}),
									_tr("With Credit")
								), 
								td("&nbsp;")
							),
							tr(
								td(
									input({type: "input", name: "ot_with_lost", size: 1, cls:"bold"}),
									_tr("With Lost")
								), 
								td("&nbsp;")
							)
						)
					)
				)
			)
		)
	);
});

function OrderTypesListEditWindowOnTableRowClick(){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoad){
			win.ud.ajaxLoad.post('/DB/GetOne', 'order_types=' + this.cells[1].innerHTML , "GET");
			var form = dad.getFirstParentWithTagName(this, "FORM");
			if(form) form.ot_code.focus();
	}
}

function OrderTypesListEditWindowRefresh(ajax) {
	ajax.post('/DB/GetList', 'list=order_types', "GET");
}

function OrderTypesListEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(OrderTypesListEditWindowRefresh, this.form, "order_types")
}
	
function newOrderTypesListEditWindow(){
	var title = "Order Types List Edit"
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var Action_id = 'Action' + newId;
		var btnAction_id = 'btnAction' + newId;
		var table_id = 'table' + newId;
		var table_header_id = 'table_header' + newId;
		var form_id = 'form' + newId;
		var data = {
			id: newId,
			form_id: form_id,
			Action_id: Action_id,
			btnAction_id: btnAction_id,
			table_id: table_id,
			table_header_id: table_header_id
		}
		data.table_height = "50%";
		data.data_table = Jaml.render('2TRDataTable', data);

		var win = dad.newWindow(newId,220,20, 800, 560, _tr(title), Jaml.render('OrderTypesListEdit', data));
		var myform = $id(data.form_id);
		win.ud.form = myform;
		win.ud.WindowRefresh = OrderTypesListEditWindowRefresh;
		myform.my_field_prefix = "ot_";
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		var mytable = $id(table_id);
		mytable.my_record_header = ["id|ID|0|R",
                "code|Code|6|R",
                "description|Description|-1",
                "with_payment|Pay|6|C",
                "with_inventory|Inv.|6|C",
                "group_order|Grp. 1|6|C",
                "subgroup_order|Grp. 2|6|C",
                "is_active|Active|6|C|B"];
		mytable.row_click_cb = OrderTypesListEditWindowOnTableRowClick;
		win.ud.ajaxTable = new dad.newAjaxDataTableAjax(win, newId);

		var btn = $id(btnAction_id);
		btn.onclick = OrderTypesListEditWindowOnSubmit;
		
		OrderTypesListEditWindowRefresh(win.ud.ajaxTable);
	}
	dad.bringToFront(win)
}
