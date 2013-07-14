Jaml.register('GLTransationsEditWindow', function(args) {
	form({id: args.form_id, style: "width:100%;height:100%;", onsubmit:"return false;"}, 
		table({style: "width:100%;height:100%", cls: "editForm"},
			tr({style:"height:2%;"},
				td(_tr("Description")),
				td({colspan:2},_tr("Date"))
			),
			tr({style:"height:2%;"},
				td({style:"width:98%;"}, input({type: "text", name: "glt_description", style:"width:100%;"})),
				td(input({type: "text", name: "glt_transaction_date", size: 10})),
				td(input({type: "button", name: "select_transaction_date", value: "&lt;&gt;", 
						onclick:"newCalendarChooserWindow()"}))
			),
			tr(
				td({colspan:3, style: "width:100%;height:100%;"},
					table({style: "width:100%;height:100%;"},
						args.data_table,
						tr(
							td(
								table(
									tr(
										td(_tr("Code")), td(_tr("Grp.")), td({colspan:2},_tr("Account"))
									),
									tr(
										td(_tr("Code")), td(_tr("Grp.")), td({colspan:2},_tr("Account"))
									)
								)
							)
						),
						tr(
							td(
								table(
									tr(
										td(_tr("Document")), td(_tr("Debit")), td(_tr("Credit"))
									),
									tr(
										td(_tr("Document")), td(_tr("Debit")), td(_tr("Credit"))
									)
								)
							)
						),
						tr(
							td(
								table(
									tr(
										td({colspan:2},_tr("Due Date")), td({colspan:3},_tr("Checked Date"))
									),
									tr(
										td({colspan:2},_tr("Due Date")), td({colspan:3},_tr("Checked Date"))
									)
								)
							)
						)
					)
				)
			),
			tr(
				td({colspan:2},
					Jaml.render('ActionSelect', args),
					input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")})
				),
				td({style:"text-align:right;"},
					_tr("Transaction #"), input({type: "input", name: "glt_id", size: 12, readonly:"1"})
				)
			)
		)
	);
});

function showGLTransationsEditWindow(all_sales_buys, caller, id){
	var win = newGLTransationsEditWindow(all_sales_buys);
	win.ud.caller = caller;
	win.ud.sab = all_sales_buys;
	var ajax = win.ud.ajaxLoad;
	if(ajax){
		ajax.post("/DB/GetOne", "gl_transactions=" + id + "&with_lines=1", 'GET', null);
	}
/*	
	var ajaxLines = win.ud.ajaxLines;
	if(ajaxLines){
		ajaxLines.post('/DB/GetList', 'list=orders&lines=' + id, "GET");
	}
*/	
}

function FillTransactionLineEdit(form, row){
	form.product_id.value = row[1].innerHTML;
	form.description.value = row[2].innerHTML;
	form.quantity.value = row[3].innerHTML;
	form.price.value = row[4].innerHTML;
}

function TransactionLinesOnTableRowDoubleClick(ev){
	var table = dad.getTableForTableChild(this);
	if(table){
		var win = dad.getWindowForChild(table);
		if(win && win.ud.form) 		FillTransactionLineEdit(win.ud.form, this.cells);
	}
	dad.cancelEvent(dad.checkEvent(ev));
	//return true;
}

function TransactionLinesOnTableRowClick(ev){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoadLine) {
		var row = this.cells;
		win.ud.ajaxLoadLine.post('/DB/GetOne', 'gl_transactions=' + row[1].innerHTML + '&line_calculated=1', "GET");
	}
	dad.cancelEvent(dad.checkEvent(ev));
	//return true;
}

function onGLTransationsEditValidateEntity(win, id, selection){
	win.ud.ajaxLoadEntity.post('/DB/GetOne', 'entities=' + id + '&for_order=1', "GET");
}

function onGLTransationsEditSearchEntity(sender){
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

function newGLTransationsEditWindow(all_sales_buys){
	var title = dad._getSABTitle(all_sales_buys, 'GL Transaction Edit');
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

		var win = dad.newWindow(newId,220,20, 800, 500, _tr(title), Jaml.render('GLTransationsEditWindow', data));
		//dad.setContentOverflow(newId);

		var data_table = $id(table_id);
		data_table.className = data_table.className + " gl_transactions_lines";
		var myform = $id(data.form_id);
		
		win.ud.form = myform;
		myform.my_field_prefix = "glt_";
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

		var mytable = $id(table_id);
		mytable.my_record_header = ["id|ID|0",
                "code|Code|8",
                "description|Account|-1",
                "account_op|Op.|5",
                "debit|Debit|9|R|ZM",
                "credit|Credit|9|R|ZM"];
		mytable.row_click_cb = TransactionLinesOnTableRowClick;
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
				var form = $id(data.form_id);
				dad.formFillByRecord(form, record, "tl_");
				//if(!record.__table__) form.ol_quantity.focus();
			} else {
				alert("An error has occured making the request");
			}
		}, null, false);
		
		win.ud.ajaxLoadEntity = new dad.Ajax(function(id){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var record = dad.parseSLEData2Object(this.responseText);
				var form = $id(data.form_id);
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

Jaml.register('GLTransationsListSearchOn', function(args) {
	input({type: "radio", name: "search_on", id: "by_description" + args.id, value: "description"}),
	label({'for': "by_notes" + args.id},  _tr("Notes")),
	input({type: "radio", name: "search_on", id: "by_date" + args.id, value: "date"}),
	label({'for': "by_date" + args.id},  _tr("Date"))
});

function newGLTransationsListSearchWindow(all_sales_buys){
	var colHeaders = ["id|ID|6",
                "transaction_date|Date|9",
                "descriptio|Description|-1",
                "amount|Amount|12"];
	var title = dad._getSABTitle(all_sales_buys, 'GL Transactions List/Search');
	var win = newListSearchWindow(all_sales_buys, title, colHeaders, showGLTransationsEditWindow,
		"gl_transactions", 'GLTransationsListSearchOn', null, all_sales_buys);
	var data_table = $id("table" + win.ud.win_id);
	data_table.className = data_table.className + " gl_transactions_list";

	var ajaxOrderType = new dad.Ajax(function(id){
		if(this.status == 200){
			//retrieve result as an JavaScript object
			var records = dad.parseSLEData(this.responseText); 
			var select = $id('group_filter' + id);
			dad.fillSelectByRecords(select, records, true);
		}
	}, win.ud.win_id, false);
	if(ajaxOrderType){
		ajaxOrderType.post('/DB/GetList', 'list=order_types&short_list=1&group_order=' + 
			all_sales_buys, "GET");
	}
		
	return win;
}
