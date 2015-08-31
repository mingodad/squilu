Jaml.register('GLGroupsListEdit', function(args) {
	form({action: "/DB/Action", method: "post", id: args.form_id, style: "width:100%;height:100%;"},
		table({style: "width:100%;height:100%;"},
			args.data_table,
			tr(
				td({'class': "editBox", style: "width:100%;"},
					fieldset(legend(_tr("Edit Form")),
						table(
							tr(
								td(_tr("ID")), td(_tr("Code")), td(_tr("Op. Debit(+/-)")), td(_tr("Op. Credit(+/-)"))
							),
							tr(
								td(input({type: "input", name: "glg_id", size: 12, readonly:"1"})), 
								td(input({type: "input", name: "glg_code", size: 12})), 
								td(input({type: "input", name: "glg_debit_op", size: 2})),
								td(input({type: "input", name: "glg_credit_op", size: 2}))
							),
							tr(
								td({colspan:3}, _tr("Description")), 
								td(Jaml.render('ActionSelect', args))
							),
							tr(
								td({colspan:3},input({type: "input", style: "width:98%;", name: "glg_description"})), 
								td(input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")}))
							)
						)
					)
				)
			)
		)
	);
});

function GLGroupsListEditWindowOnTableRowClick(){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoad){
			win.ud.ajaxLoad.post('/DB/GetOne', 'gl_groups=' + this.cells[1].innerHTML , "GET");
			var form = dad.getFirstParentWithTagName(this, "FORM");
			if(form) form.st_rate1.focus();
	}
}

function GLGroupsListEditWindowRefresh(ajax) {
	ajax.post('/DB/GetList', 'list=gl_groups', "GET");
}

function GLGroupsListEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(GLGroupsListEditWindowRefresh, this.form, "gl_groups")
}

function newGLGroupsListEditWindow(){
	var title = "GL Groups List Edit"
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

		var win = dad.newWindow(newId,220,20, 460, 420, _tr(title), Jaml.render('GLGroupsListEdit', data));
		var myform = $id(data.form_id);
		win.ud.form = myform;
		win.ud.WindowRefresh = GLGroupsListEditWindowRefresh;
		myform.my_field_prefix = "glg_";
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		var mytable = $id(table_id);
		mytable.my_record_header = ["id|ID|0", "code|Code|8",
                "description|Description|-1", "debit_op|Dbt|8|C", "credit_op|Cdt|8|C"];
		mytable.row_click_cb = GLGroupsListEditWindowOnTableRowClick;
		win.ud.ajaxTable = new dad.newAjaxDataTableAjax(win, newId);

		var btn = $id(btnAction_id);
		btn.onclick = GLGroupsListEditWindowOnSubmit;

		GLGroupsListEditWindowRefresh(win.ud.ajaxTable);
	}
	dad.bringToFront(win)
}
