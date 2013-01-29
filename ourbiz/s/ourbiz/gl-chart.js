Jaml.register('GLChartEditWindow', function(args) {
	form({action: "/DB/Action", method: "post", onsubmit:"return false;",
			id: args.form_id, style: "width:100%;height:100%;"}, 
		table({style: "width:100%;height:100%;", cls: "editForm"},
			tr(
				td(
					table(
						tr(
							td(_tr("ID")),td(_tr("Budget")),td(_tr("Balance"))
						),
						tr(
							td(input({type: "text", name: "glc_id", readonly:"1"})),
							td(input({type: "text", name: "glc_budget"})),
							td(input({type: "text", name: "glc_balance"}))
						),
						tr(
							td(_tr("Code")),td({colspan:2},_tr("Group"))
						),
						tr(
							td(input({type: "text", name: "glc_group_code"})),
							td({colspan:2},select({name: "glc_gl_group_id", cls:"fw"}))
						),
						tr(
							td({colspan:3},_tr("Description"))
						),
						tr(
							td({colspan:3}, input({type: "text", name: "glc_description", cls:"fw"}))
						)
					)
				),
				td(
					br(),
					input({type: "checkbox", name: "glc_is_active"}),_tr("Active"),
					br(),
					input({type: "checkbox", name: "glc_is_header"}),_tr("Header"),
					br(),
					Jaml.render('ActionSelect', args),
					br(),
					input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")})
				)
			),
			tr(
				td({colspan:2},_tr("Notes"))
			),
			tr(
				td({colspan:2},textarea({rows: 5, name: "glc_notes", cls: "fw"}))
			)
		)
	);
});

function GLChartEditWindowRefresh(ajax) {
}

function showGLChartEditWindow(all_sales_buys, caller, id){
	var win = newGLChartEditWindow(all_sales_buys);
	win.ud.caller = caller;
	win.ud.edit_id = id;
	var ajax = win.ud.ajaxLoad;
	if(ajax){
		ajax.post("/DB/GetOne", "gl_chart=" + id, 'GET', null);
	}
}

function GLChartEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(GLChartEditWindowRefresh, this.form, "gl_chart")
}

function GLChartEditWindowOnFillForm(form_field, value) {
	if(form_field.name == "gl_group_id"){
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

function newGLChartEditWindow(all_sales_buys){
	var title = dad._getSABTitle(all_sales_buys, 'GL Chart Edit');

	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var Action_id = 'Action' + newId;
		var btnAction_id = 'btnAction' + newId;
		var data = {
			id: newId,
			form_id: 'form' + newId,
			Action_id: Action_id,
			btnAction_id: btnAction_id
		}
		var win = dad.newWindow(newId,220,20, 670, 315, _tr(title), Jaml.render('GLChartEditWindow', data));
		//dad.setContentOverflow(newId);
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);
		
		var myform = $(data.form_id);
		myform.my_field_prefix = "glc_";
		myform.onFillForm = GLChartEditWindowOnFillForm;
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listGLChartWindowOnSubmitRespose, newId, false);
		
		var btn = $(btnAction_id);
		btn.onclick = GLChartEditWindowOnSubmit;
		
		var ajaxGlGroups = new dad.Ajax(function(select){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText); 
				dad.fillSelectByRecords(select, records, true);
			}
		}, myform.glc_gl_group_id);
		if(ajaxGlGroups) ajaxGlGroups.post('/DB/GetList', 'list=gl_groups&short_list=1', "GET");		
	}
	
	dad.bringToFront(win)
	return win;
}

Jaml.register('GLChartListSearchOn', function(args) {
	input({type: "radio", name: "search_on", id: "by_description" + args.id, value: "description", checked:1 }),
	label({'for': "by_description" + args.id},  _tr("Description")),
	input({type: "radio", name: "search_on", id: "by_notes" + args.id, value: "notes"}),
	label({'for': "by_notes" + args.id},  _tr("Notes")),
	input({type: "radio", name: "search_on", id: "by_code" + args.id, value: "code"}),
	label({'for': "by_code" + args.id},  _tr("Code"))
});

Jaml.register('GLChartListSearchOn2', function(args) {
	input({type: "checkbox", id: "by_active" + args.id, value: "by_active"}),
	label({'for': "by_active" + args.id},  _tr("Active")),
	input({type: "checkbox", id: "by_headers" + args.id, value: "headers"}),
	label({'for': "by_headers" + args.id},  _tr("Headers")),
	input({type: "checkbox", id: "by_accounts" + args.id, value: "accounts"}),
	label({'for': "by_accounts" + args.id},  _tr("Accounts"))
});

function newGLChartListSearchWindow(all_sales_buys){
	var colHeaders = ["id|ID|0|R",
                "group_code|Grp.|3|C",
                "code|Code|8",
                "description|Description|-1",
                "is_header|Hdr.|3|C|B"];
	var title = dad._getSABTitle(all_sales_buys, 'GL Chart List/Search');
	return newListSearchWindow(all_sales_buys, title, colHeaders, showGLChartEditWindow,
		"gl_chart", 'GLChartListSearchOn', 'GLChartListSearchOn2', all_sales_buys);
}
