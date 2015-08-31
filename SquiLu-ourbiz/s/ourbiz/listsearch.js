Jaml.register('ListSearch', function(args) {
	form({id: args.form_id, style: "width:100%;height:100%;", onsubmit:"return false;"},
		table({style: "width:100%;height:100%;"},
			tr(
				td({style:"height:12px; white-space:nowrap;"}, 
					select({id: 'group_filter' + args.id, name:"group_id", style: "width:100%;"})
				)
			),
			args.data_table,
			tr(
				td({'class': "editBox", style: "width:100%;height:50px; white-space:nowrap;"},
					fieldset(legend(_tr("Search")),
						table(
							tr( td({'class': "radioBox", colspan:2}, args.search_on ) ),
							tr(
								td({colspan:2},
									input({type: "text", name: "search_str"}),
									input({id: args.btnSearch_id, type: "button", name: "btnSearch", value: _tr("Search")}),
									input({type: "text", name: "query_limit", size: 4, value: "50"})
								),
								td({rowspan:3}, img({id: 'ls_img_' +args.id}))
							),
							tr(
								td(
									input({type: "button", name: "btnSelect", value: _tr("Select")}),
									input({type: "button", value: _tr("Update"), onclick: "newCalendarChooserWindow()"}),
									input({type: "button", value: _tr("Insert")})
								),
								td(args.search_on2)
							)
						)
					)
				)
			)
		)
	)
});

function ListSearchWindowOnTableRowClick(ev){
	var table = dad.getTableForTableChild(this);
	if(table){
		var win = dad.getWindowForChild(table);
		if(win && win.ud.editWindow) win.ud.editWindow(win.ud.all_sales_buys, win, this.cells[1].innerHTML);
	}
	dad.cancelEvent(dad.checkEvent(ev));
	//return true;
}

function ListSearchWindowOnTableFirstColClick(ev){
	var win = dad.getWindowForChild(this);
	var id = this.innerHTML;
	if(win && win.ud.editWindow) {
		if(win.ud.callerFuncCB){
			var selection = [];
			var clickedSelection = win.ud.mySelection;
			for(var elm in clickedSelection){
				if(clickedSelection[elm]){
					selection.push(elm);
				}
				win.ud.mySelection = {};
			}
			dad.closeTopParent(this);
			win.ud.callerFuncCB(win.ud.caller, id, selection);
			win.ud.callerFuncCB = null;
		} else {
			win.ud.editWindow(win.ud.all_sales_buys, win, id);
		}
		dad.cancelEvent(dad.checkEvent(ev));
	}
	//return true;
}

Jaml.register('ListSearchOn2', function(args) {
	input({type: "checkbox", id: "by_sales" + args.id, value: "by_sales"}),
	label({'for': "by_sales" + args.id},  _tr("Sales")),
	input({type: "checkbox", id: "by_buys" + args.id, value: "by_buys"}),
	label({'for': "by_buys" + args.id},  _tr("Buys"))
});

function newListSearchWindow(all_sales_buys, title, colHeaders, editWindow, dbTable, search_on, search_on2, db_extra_type){
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var btnSearch_id = 'btnSearch' + newId;
		var table_id = 'table' + newId;
		var table_header_id = 'table_header' + newId;
		var obj_id = {id:newId};
		if(!search_on2) search_on2 = 'ListSearchOn2';
		var data = {
			id: newId,
			form_id: 'form_list_search' + newId,
			'btnSearch_id': btnSearch_id,
			'table_id': table_id,
			'table_header_id': table_header_id,
			search_on: Jaml.render(search_on, obj_id),
			search_on2: Jaml.render(search_on2, obj_id)
		}
		data.table_height = (dad.isIE ? ((dad.isIE == 9) ? "20em" : "19em") : "100%");
		//data.table_height = (dad.isIE && (dad.isIE == 9)) ? "20em" : "19em";
		data.data_table = Jaml.render('2TRDataTable', data);

		var win = dad.newWindow(newId,220,20, 800, 500, title, Jaml.render('ListSearch', data));
		win.ud.editWindow = editWindow;
		win.ud.all_sales_buys = all_sales_buys;
		win.ud.win_id = newId;

		var myform = $id(data.form_id);
		win.ud.form = myform;
		dad.setupEditForm(myform);

		myform.search_str.onkeydown = function(evt){
			var evt = dad.checkEvent(evt);
			if (evt.altKey || evt.ctrlKey || evt.shiftKey) return true;
			if(dad.getEventKey(evt) == 13){
				this.form.btnSearch.onclick();
				return false;
			}
			return dad.formInputTextOnKeydown(evt);
		}
		
		win.searchForMe = function(search_str, call_win, cbFunc){
			win.ud.caller = call_win;
			myform.search_str.value = search_str;
			win.ud.callerFuncCB = cbFunc;
			myform.btnSearch.onclick();
		}
		
		var group_filter = $id('group_filter' + newId);
		group_filter.onchange=function(evt){
			var btn = $id(btnSearch_id);
			btn.onclick();
		}

		var btn = $id(btnSearch_id);
		btn.dbTable = dbTable;
		btn.dbExtraType = db_extra_type;
		var mytable = $id(table_id);
		mytable.my_record_header = colHeaders;
		win.ud.mySelection = {};
		mytable.row_dblclick_cb = ListSearchWindowOnTableRowClick;
		mytable.first_row_click_cb = ListSearchWindowOnTableFirstColClick;
		mytable.myPrevFocus = myform['group_filter'+newId];
		mytable.myNextFocus = myform.search_str;
		mytable.row_over_cb =  function(evt){};
			
		mytable.selection_click_cb = function(evt){
			dad.checkEvent(evt);
			var sender = dad.getEventSource(evt);
			var tr = dad.getFirstParentWithTagName(sender, "TR");
			var id = tr.cells[1].innerHTML;
			win.ud.mySelection[id] = sender.checked;
		}
		btn.ajax = dad.newAjaxDataTableAjax(win, newId);
		btn.onclick = function(){
			this.form._dbrecord = {};
			var values = dad.getFormData(this.form, this.dbTable);
			//ajax.post(form.action, values.join('&'), 'POST', null);
			this.ajax.post('/DB/GetList?list=' + this.dbTable + '&search=1&sab=' + this.dbExtraType,  values.join('&'), 'POST');
		};
		btn.onclick();
	}
	dad.bringToFront(win)
	win.ud.form.search_str.focus();
	return win;
}
