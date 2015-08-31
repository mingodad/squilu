Jaml.register('WarrantyTypesListEdit', function(args) {
	form({action: "/DB/Action", method: "post", id: args.form_id, style: "width:100%;height:100%;"},
		table({style: "width:100%;height:100%;"},
			args.data_table,
			tr(
				td({'class': "editBox", style: "width:100%;"},
					fieldset(legend(_tr("Edit Form")),
						table(
							tr(
								td(_tr("ID")), td(_tr("CDate")), td(_tr("MDate")), td("&nbsp;")
							),
							tr(
								td(input({type: "input", name: "wt_id", size: 12, readonly:"1"})), 
								td(input({type: "input", name: "wt_cdate", size: 12, readonly:"1"})), 
								td(input({type: "input", name: "wt_mdate", size: 12, readonly:"1"})),
								td(input({type: "checkbox", name: "wt_is_active"}, _tr("Active")))
							),
							tr(
								td(_tr("Code")), 
								td({colspan:3}, _tr("Description"))
							),
							tr(
								td(input({type: "input", name: "wt_code", size: 12})), 
								td({colspan:3},input({type: "input", name: "wt_description", style: "width:98%;"})) 
							),
							tr(
								td({colspan:3}, "&nbsp"), 
								td({style: "width:5%;padding-left:1em;"},
									Jaml.render('ActionSelect', args),
									br(),
									input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")})
									)
							)
						)
					)
				)
			)
		)
	);
});

function WarrantyTypesListEditWindowOnTableRowClick(){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoad){
			win.ud.ajaxLoad.post('/DB/GetOne', 'warranty_types=' + this.cells[1].innerHTML , "GET");
			var form = dad.getFirstParentWithTagName(this, "FORM");
			if(form) form.wt_code.focus();
	}
}

function WarrantyTypesListEditWindowRefresh(ajax) {
	ajax.post('/DB/GetList', 'list=warranty_types', "GET");
}

function WarrantyTypesListEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(MeasureUnitsListEditWindowRefresh, this.form, "warranty_types")
}
	
function newWarrantyTypesListEditWindow(){
	var title = "Warranty Types List Edit"
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
		//data.table_height = "50%";
		data.data_table = Jaml.render('2TRDataTable', data);

		var win = dad.newWindow(newId,220,20, 550, 450, _tr(title), Jaml.render('WarrantyTypesListEdit', data));
		var myform = $id(data.form_id);
		win.ud.form = myform;
		win.ud.WindowRefresh = WarrantyTypesListEditWindowRefresh;
		myform.my_field_prefix = "wt_";
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		var mytable = $id(table_id);
		mytable.my_record_header = ["id|ID|0",
                "code|Code|8",
                "description|Description|-1",
                "is_active|Active|5|C|B"];
		mytable.row_click_cb = WarrantyTypesListEditWindowOnTableRowClick;
		win.ud.ajaxTable = new dad.newAjaxDataTableAjax(win, newId);

		var btn = $id(btnAction_id);
		btn.onclick = WarrantyTypesListEditWindowOnSubmit;
		
		WarrantyTypesListEditWindowRefresh(win.ud.ajaxTable);
	}
	dad.bringToFront(win)
}
