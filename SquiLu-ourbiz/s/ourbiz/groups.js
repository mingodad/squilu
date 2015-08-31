Jaml.register('GroupsListEdit', function(args) {
	var newIdBy2 = dad.newIdBy2calls;
	form({action: "/DB/Action", method: "post", id: args.form_id, style: "width:100%;height:100%;"},
		table({style: "width:100%;height:100%;"},
			tr({style: "height:100%;"},td(div({id:args.div_tree_id, 'class': "divTree", style: "width:100%;height:100%;overflow:auto;min-height:8em;"}))),
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
									input({type: "input", name: "ot_with_payment", size: 1, cls:"bold"}),
									_tr("With Payment")
								), 
								td(
									input({type: "checkbox", name: "ot_is_expense", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Is Expense"))
								)
							),
							tr(
								td(input({type: "input", style: "width:98%;", name: "ot_description"})), 
								td(
									input({type: "input", name: "ot_with_inventory", size: 1, cls:"bold"}),
									_tr("With Inventory")
								), 
								td(
									input({type: "checkbox", name: "ot_is_income", id: newIdBy2()}),
									label({'for': newIdBy2()},  _tr("Is Income"))
								)
							),
							tr(
								td(_tr("Notes")), 
								td(
									input({type: "input", name: "ot_with_reserve", size: 1, cls:"bold"}),
									_tr("With Reserve")
								), 
								td(Jaml.render('ActionSelect', args))
							),
							tr(
								td({style: "width:60%;"}, input({type: "input", style: "width:98%;", name: "ot_notes"})), 
								td(
									input({type: "input", name: "ot_with_credit", size: 1, cls:"bold"}),
									_tr("With Credit")
								), 
								td(input({id: args.btnAction_id, type: "button", name: "btnAction", value: _tr("Action")}))
							)
						)
					)
				)
			)
		)
	);
});

function GroupsListEditWindowOnTableRowClick(){
	var win = dad.getWindowForChild(this);
	if(win && win.ud.ajaxLoad){
			win.ud.ajaxLoad.post('/DB/GetOne', 'product_groups=' + this.cells[1].innerHTML , "GET");
			var form = dad.getFirstParentWithTagName(this, "FORM");
			if(form) form.ot_code.focus();
	}
}

function GroupsListEditWindowRefresh(ajax) {
	ajax.post('/DB/GetList', 'list=product_groups', "GET");
}

function GroupsListEditWindowOnSubmit(btn) {
	return dad.formOnSubmit(GroupsListEditWindowRefresh, this.form, "product_groups")
}

function doEditGroupID(id){
	alert(id);
}

function fillGroupsTree(ctx){
	var html = [];
	html.push("<ul>");
	var len = ctx.records.length;
	var start_idx = ctx.idx;
	var curr_parent = ctx.parent;
	while(ctx.idx<len){
		var rec = ctx.records[ctx.idx];
		var myparent = rec[1];
        if(myparent != curr_parent) break;
		html.push("<li onclick='doEditGroupID(" + rec[0] + ")'>");
		html.push(rec[2]);
		html.push("</li>");
		++ctx.idx;
		ctx.parent = rec[0];
		var result = fillGroupsTree(ctx);
		if(result) html.push(result);
	}
	html.push("</ul>");
	if(start_idx == ctx.idx) return false;
	return html.join('');
}

function fillGroupsTreeById(records, id){
	var div_tree = $id('div_tree' +id);
	var tree_ctx = {records:records, parent:0, idx:0, div_tree:div_tree};
	var result = fillGroupsTree(tree_ctx);
	if(result) div_tree.innerHTML = result;
}
	
function newGroupsListEditWindow(){
	var title = "Groups List Edit"
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var Action_id = 'Action' + newId;
		var btnAction_id = 'btnAction' + newId;
		var div_tree_id = 'div_tree' + newId;
		var form_id = 'form' + newId;
		var data = {
			id: newId,
			form_id: form_id,
			Action_id: Action_id,
			btnAction_id: btnAction_id,
			div_tree_id: div_tree_id
		}

		var win = dad.newWindow(newId,220,20, 800, 560, _tr(title), Jaml.render('GroupsListEdit', data));
		var myform = $id(data.form_id);
		win.ud.form = myform;
		win.ud.WindowRefresh = GroupsListEditWindowRefresh;
		myform.my_field_prefix = "ot_";
		dad.setupEditForm(myform);
		myform.ajaxSubmit = new dad.Ajax(dad.listEditWindowOnSubmitRespose, newId, false);
		win.ud.ajaxLoad = new dad.Ajax(dad.formAjaxLoadResponse, newId, false);

		win.ud.ajaxTable = new dad.Ajax(function(id){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseJSON(this.responseText);
				if(!records || !dad.isArray(records) || records.length < 1){
					alert("An error has occured making the request");
					return;
				}
				fillGroupsTreeById(records, id);
			} else {
				alert("An error has occured making the request");
			}
		}, newId, false);

		var btn = $id(btnAction_id);
		btn.onclick = GroupsListEditWindowOnSubmit;
		
		GroupsListEditWindowRefresh(win.ud.ajaxTable);
	}
	dad.bringToFront(win)
}

function newProductGroupsListEditWindow(){
	newGroupsListEditWindow();
}

function newEntityGroupsListEditWindow(){
	newGroupsListEditWindow();
}
