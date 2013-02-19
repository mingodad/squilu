Jaml.register('SalesTaxListEdit', function(args) {
	form({action: "/DB/Action", method: "post", id: args.form_id, style: "width:100%;height:100%;"},
		table({style: "width:100%;height:100%;"},
			args.data_table,
			tr(
				td({'class': "editBox", style: "width:100%;"},
					fieldset(legend(_tr("Edit Form")),
						table(
							tr(
								td(_tr("ID")), td(_tr("CDate")), td(_tr("MDate"))
							),
							tr(
								td(input({type: "input", name: "st_id", size: 12, readonly:"1"})), 
								td(input({type: "input", name: "st_cdate", size: 12, readonly:"1"})), 
								td(input({type: "input", name: "st_mdate", size: 12, readonly:"1"}))
							),
							tr(
								td(_tr("Tax I %")), td(_tr("Tax II %")), td("&nbsp;")
							),
							tr(
								td(input({type: "input", name: "st_rate1", size: 12})), 
								td(input({type: "input", name: "st_rate2", size: 12})), 
								td(input({type: "checkbox", name: "st_is_active"}, _tr("Active")))
							),
							tr(
								td({colspan:2}, _tr("Description")), 
								td(Jaml.render('ActionSelect', args))
							),
							tr(
								td({colspan:2},input({type: "input", style: "width:98%;", name: "st_description"})), 
								td(input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")}))
							)
						)
					)
				)
			)
		)
	);
});

function SalesTaxListEditWindowOnTableRowClick(){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoad){
			win.ud.ajaxLoad.post('/DB/GetOne', 'sales_tax_rates=' + this.cells[1].innerHTML , "GET");
			var form = dad.getFirstParentWithTagName(this, "FORM");
			if(form) form.st_rate1.focus();
	}
}

function SalesTaxListEditWindowRefresh(ajax) {
	ajax.post('/DB/GetList', 'list=sales_tax_rates', "GET");
}

function SalesTaxListEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(SalesTaxListEditWindowRefresh, this.form, "sales_tax_rates")
}

function newSalesTaxListEditWindow(){
	var title = "Sales Tax List Edit"
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

		var win = dad.newWindow(newId,220,20, 460, 420, _tr(title), Jaml.render('SalesTaxListEdit', data));
		var myform = $(data.form_id);
		win.ud.form = myform;
		win.ud.WindowRefresh = SalesTaxListEditWindowRefresh;
		myform.my_field_prefix = "st_";
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		var mytable = $(table_id);
		mytable.my_record_header = ["id|ID|0", "rate1|V.A.T. %|8|R", "rate2|R.E. %|8|R",
			"description|Description|-1", "is_active|Active|5|C|B"];
		mytable.row_click_cb = SalesTaxListEditWindowOnTableRowClick;
		win.ud.ajaxTable = new dad.newAjaxDataTableAjax(win, newId);

		var btn = $(btnAction_id);
		btn.onclick = SalesTaxListEditWindowOnSubmit;

		SalesTaxListEditWindowRefresh(win.ud.ajaxTable);
	}
	dad.bringToFront(win)
}
