Jaml.register('PaymentsEditWindow', function(args) {
	form({id: args.form_id, style: "width:100%;height:100%;", onsubmit:"return false;"}, 
		table({style: "width:100%;height:100%", cls: "editForm"},
			tr({style:"height:2%;"},
				td(_tr("Payment #")),
				td(input({type: "text", name: "pmt_id", size: 10})),
				td(_tr("Amount")),
				td(input({type: "text", name: "pmt_amount", size: 10})),
				td(_tr("Due Date")),
				td(input({type: "text", name: "pmt_due_date", size: 10})),
				td("&nbsp;")
			),
			tr({style:"height:2%;"},
				td({colspan:5},_tr("Entity")),
				td(_tr("Created")),
				td("&nbsp;")
			),
			tr({style:"height:2%;"},
				td({colspan:5}, input({type: "text", name: "pmt_entity_name", style:"width:98%;"})),
				td(input({type: "text", name: "pmt_cdate", size: 10})),
				td("&nbsp;")
			),
			tr({style:"height:2%;"},
				td({colspan:5},_tr("Description")),
				td(_tr("Cleared")),
				td("&nbsp;")
			),
			tr({style:"height:2%;"},
				td({colspan:5}, 
					input({type: "text", name: "pmt_description", style:"width:98%;"})
				),
				td({style:"width:2%;"},
					input({type: "text", name: "pmt_cleared_date", size: 10})
				),
				td({style:"width:2%;"},
					input({type: "button", name: "select_due_date", value: "&lt;&gt;", 
						onclick:"newCalendarChooserWindow()"})
				)
			),
			tr(
				td({colspan:7, style: "width:100%;height:100%;"},
					table({style: "width:100%;height:100%;"},
						args.data_table,
						tr(
							td(_tr("Description"))
						),
						tr(
							td(input({type: "text", name: "pl_description", style:"width:98%;"}))
						),
						tr(
							td(
								table(
									tr(
										td(_tr("Amount")), td(_tr("Date")), td("&nbsp;"), td("&nbsp;")
									),
									tr(
										td(input({type: "text", name: "pl_amount", size: 10})), 
										td(input({type: "text", name: "pl_payment_date", size: 10})), 
										td(input({type: "button", name: "select_payment_date", value: "&lt;&gt;", 
												onclick:"newCalendarChooserWindow()"})
										), 
										td(
											input({id: args.btnSaveLine_id, type: "button", name: "btnSaveLine", value: _tr("Save Line")})
										)
									)
								)
							)
						)
					)
				)
			),
			tr(
				td({style:"text-align:right;", colspan:7},
					input({id: args.btnAction_id, type: "button", name: "btnSave", value: _tr("Save")}),
					input({id: args.btnAction_id, type: "button", name: "btnPrint", value: _tr("Print")})
				)
			)
		)
	);
});

function showPaymentsEditWindow(all_sales_buys, caller, id){
	var win = newPaymentsEditWindow(all_sales_buys);
	win.ud.caller = caller;
	win.ud.sab = all_sales_buys;
	var ajax = win.ud.ajaxLoad;
	if(ajax){
		ajax.post("/DB/GetOne", "payments=" + id + "&with_lines=1", 'GET', null);
	}
/*	
	var ajaxLines = win.ud.ajaxLines;
	if(ajaxLines){
		ajaxLines.post('/DB/GetList', 'list=orders&lines=' + id, "GET");
	}
*/	
}

function FillPaymentLineEdit(form, row){
	form.product_id.value = row[1].innerHTML;
	form.description.value = row[2].innerHTML;
	form.quantity.value = row[3].innerHTML;
	form.price.value = row[4].innerHTML;
}

function PaymentLinesOnTableRowDoubleClick(ev){
	var table = dad.getTableForTableChild(this);
	if(table){
		var win = dad.getWindowForChild(table);
		if(win && win.ud.form) 		FillTransactionLineEdit(win.ud.form, this.cells);
	}
	dad.cancelEvent(dad.checkEvent(ev));
	//return true;
}

function PaymentLinesOnTableRowClick(ev){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoadLine) {
		var row = this.cells;
		win.ud.ajaxLoadLine.post('/DB/GetOne', 'payments=' + row[1].innerHTML + '&line_calculated=1', "GET");
	}
	dad.cancelEvent(dad.checkEvent(ev));
	//return true;
}

function onPaymentsEditValidateEntity(win, id, selection){
	win.ud.ajaxLoadEntity.post('/DB/GetOne', 'entities=' + id + '&for_order=1', "GET");
}

function onPaymentsEditSearchEntity(sender){
	var form = sender.form;
	var search_str = form.o_entity_name.value;
	var win = dad.getWindowForChild(sender);
	var entity_list_search = win.ud.entity_list_search;
	if(!entity_list_search || !entity_list_search.ud.sab){
		win.ud.entity_list_search = entity_list_search = newEntitiesListSearchWindow(win.ud.sab);
	}
	entity_list_search.searchForMe(search_str, win, onGLTransationsEditValidateEntity);
	return false;
}

function newPaymentsEditWindow(all_sales_buys){
	var title = dad._getSABTitle(all_sales_buys, 'Payment Edit');
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var Action_id = 'Action' + newId;
		var btnAction_id = 'btnAction' + newId;
		var table_id = 'table' + newId;
		var table_header_id = 'table_header' + newId;
		var data = {
			id: newId,
			form_id: 'form' + newId,
			Action_id: Action_id,
			btnAction_id: btnAction_id,
			table_id: table_id,
			table_header_id: table_header_id
		}
		data.table_height = (dad.isOpera || dad.isIE) ? "8em" : "100%";
		data.data_table = Jaml.render('2TRDataTable', data);

		var win = dad.newWindow(newId,220,20, 800, 500, _tr(title), Jaml.render('PaymentsEditWindow', data));
		//dad.setContentOverflow(newId);

		var data_table = $(table_id);
		data_table.className = data_table.className + " payments_lines";
		var myform = $(data.form_id);
		
		win.ud.form = myform;
		myform.my_field_prefix = "pmt_";
		dad.setupEditForm(myform);
		
		
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

		var mytable = $(table_id);
		mytable.my_record_header = [
                "id|ID|0",
                "notes|Description|-1",
                "payment_date|Date|9",
                "amount|Amount|9|R"
			];
		mytable.row_click_cb = PaymentLinesOnTableRowClick;
		mytable._onDelete_cb = function(sender){
			var tr = dad.getFirstParentWithTagName( sender, "TR" );
			dad.formFocusNext(sender);
			tr.parentNode.removeChild(tr);
		}
		
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);
		win.ud.ajaxLines = dad.newAjaxDataTableAjax(win, newId);
		win.ud.ajaxLoadLine = new dad.Ajax(function(){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var record = dad.parseSLEData2Object(this.responseText); 
				var form = $(data.form_id);
				dad.formFillByRecord(form, record, "pl_");
				//if(!record.__table__) form.ol_quantity.focus();
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
	}
	dad.bringToFront(win);
	return win;
}

Jaml.register('PaymentsListSearchOn', function(args) {
	input({type: "radio", name: "search_on", id: "by_entities" + args.id, value: "entities", checked:1}),
	label({'for': "by_entities" + args.id},  _tr("Entities")),
	input({type: "radio", name: "search_on", id: "by_notes" + args.id, value: "notes"}),
	label({'for': "by_notes" + args.id},  _tr("Notes")),
	input({type: "radio", name: "search_on", id: "by_date" + args.id, value: "date"}),
	label({'for': "by_date" + args.id},  _tr("Date"))
});

function newPaymentsListSearchWindow(all_sales_buys){
	var colHeaders = ["id|ID|6",
                "entity_name|Entity|-1",
				"amount|Amount|12|R",
                "due_date|Date|9"];
	var title = dad._getSABTitle(all_sales_buys, 'Payments List/Search');
	var win = newListSearchWindow(all_sales_buys, title, colHeaders, showPaymentsEditWindow,
		"payments", 'PaymentsListSearchOn', null, all_sales_buys);
	var data_table = $("table" + win.ud.win_id);
	data_table.className = data_table.className + " payments_list";

	var ajaxOrderType = new dad.Ajax(function(id){
		if(this.status == 200){
			//retrieve result as an JavaScript object
			var records = dad.parseSLEData(this.responseText); 
			var select = $('group_filter' + id);
			dad.fillSelectByRecords(select, records, true);
		}
	}, win.ud.win_id, false);
	if(ajaxOrderType){
		ajaxOrderType.post('/DB/GetList', 'list=order_types&short_list=1&group_order=' + 
			all_sales_buys, "GET");
	}
		
	return win;
}
