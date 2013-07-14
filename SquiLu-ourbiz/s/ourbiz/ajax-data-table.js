Jaml.register('AjaxDataTable', function(args) {
	form({id: args.form_id, style: "width:100%;height:100%;", onsubmit:"return false;"},
		table({style: "width:100%;height:100%;"},
			tr(
				td({style:"height:12px; white-space:nowrap;"}, 
					input({id: 'url_' + args.id, style: "width:90%;", value:"/DB/GetList?list=orders&search=1&sab=S"}),
					button({id: 'btnGo' + args.id}, "Go")
				)
			),
			args.data_table
		)
	)
});


function newAjaxDataTableWindow(title){
	if(!title) title = 'Ajax Data Table';
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var table_id = 'table' + newId;
		var table_header_id = 'table_header' + newId;
		var data = {
			id: newId,
			form_id: 'form_ajax' + newId,
			'table_id': table_id,
			'table_header_id': table_header_id
		}
		data.table_height = "100%";
		data.data_table = Jaml.render('2TRDataTable', data);
		var win = dad.newWindow(newId, 160,20, 600, 400, title, Jaml.render('AjaxDataTable', data));

		var myform = $id(data.form_id);
		dad.setupEditForm(myform);

		var btn = $id('btnGo' + newId);
		btn._url_input = $id('url_' + newId);
		
		btn.ajax = dad.newAjaxDataTableAjax(win, newId);
		btn.ajax._withHeaders = true;
		btn.onclick = function(){
			var go_value = btn._url_input.value;
			var url, qs;
			var splitPos = go_value.indexOf("?");
			if(splitPos >= 0){
				url = go_value.substr(0, splitPos);
				qs = go_value.substr(splitPos+1);
			} else {
				url = go_value;
				qs = '';
			}
			this.ajax.post(url,  qs, 'GET');
		};
	}
	dad.bringToFront(win)
}
