Jaml.register('MeasureUnitsListEdit', function(args) {
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
								td(input({type: "input", name: "mu_id", size: 12, readonly:"1"})), 
								td(input({type: "input", name: "mu_cdate", size: 12, readonly:"1"})), 
								td(input({type: "input", name: "mu_mdate", size: 12, readonly:"1"})),
								td(input({type: "checkbox", name: "mu_is_active"}, _tr("Active")))
							),
							tr(
								td(_tr("Code")), 
								td({colspan:3}, _tr("Description"))
							),
							tr(
								td(input({type: "input", name: "mu_code", size: 12})), 
								td({colspan:3},input({type: "input", name: "mu_description", style: "width:98%;"})) 
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

function MeasureUnitsListEditWindowOnTableRowClick(){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoad){
			win.ud.ajaxLoad.post('/DB/GetOne', 'measure_units=' + this.cells[1].innerHTML , "GET");
			var form = dad.getFirstParentWithTagName(this, "FORM");
			if(form) form.mu_code.focus();
	}
}

function MeasureUnitsListEditWindowRefresh(ajax) {
	ajax.post('/DB/GetList', 'list=measure_units', "GET");
}

function MeasureUnitsListEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(MeasureUnitsListEditWindowRefresh, this.form, "measure_units")
}
	
function newMeasureUnitsListEditWindow(){
	var title = "Measure Units List Edit"
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

		var win = dad.newWindow(newId,220,20, 550, 450, _tr(title), Jaml.render('MeasureUnitsListEdit', data));
		var myform = $(data.form_id);
		win.ud.form = myform;
		win.ud.WindowRefresh = MeasureUnitsListEditWindowRefresh;
		myform.my_field_prefix = "mu_";
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		var mytable = $(table_id);
		mytable.my_record_header = ["id|ID|0",
                "code|Code|8",
                "description|Description|-1",
                "is_active|Active|5|C|B"];
		mytable.row_click_cb = MeasureUnitsListEditWindowOnTableRowClick;
		win.ud.ajaxTable = new dad.newAjaxDataTableAjax(win, newId);

		var btn = $(btnAction_id);
		btn.onclick = MeasureUnitsListEditWindowOnSubmit;
		
		MeasureUnitsListEditWindowRefresh(win.ud.ajaxTable);
	}
	dad.bringToFront(win)
}
