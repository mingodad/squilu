Jaml.register('2TRDataTable', function(args) {
		tr(
			td({colspan:3, style: "width:100%; height:10px;"}, //no height for firefox
				div({'class': "divTable", style: "width:100%;overflow:auto;"},
						table({id: args.table_header_id, 'class': "wrappedTable"},
							thead( tr( 
								th( input({type: "checkbox", onkeydown:"dad.formInputTextOnKeydown"}) )
								) )
						)
				)
			)
		),
		tr(
		//(args.table_height || "100%") 65% for firefox
			td({colspan:3, style: "width:100%;height:" + (args.table_height || "100%") + "; vertical-align:top;"},
				div({'class': "divTable", style: "width:100%;height:100%;overflow:auto;"},
					div({style:"height: 0px;"},
						table({id: args.table_id, 'class': "wrappedTable"},
							tbody(),
							tfoot( tr( th( "&nbsp;" ) ) )
						)
					)
				)
			)
		)
});


Jaml.register('HistoryTabContent', function(args) {
	table({style: "width:100%;height:100%;"},
		tr(
			td({style: "width:80%;height:10px;"},
				select({id: 'history_type_id' + args.id, style: "width:100%;"}, args.history_options)
			),
			td({style: "width:5%;"},input({type: "text", name: "history_query_limit", size:4, value: "50"})),
			td({style: "width:15%;"},button(_tr("View Doc.")))
		),
		tr(
			td({colspan:3, style: "width:100%;height:10px;"},
				div({'class': "divTable", style: "width:100%;"},
						table({id: args.table_history_header_id, 'class': "wrappedTable"},
							thead( tr(  th( input({type: "checkbox", onkeydown:"dad.formInputTextOnKeydown"}) ) ) )
						)
				)
			)
		),
		tr(
			td({colspan:3, style: "width:100%;"},
				div({'class': "divTable", style: "width:100%;height:22em;overflow:auto;"},
					div({style:"height: 0px;"},
						table({id: args.table_history_id, 'class': "wrappedTable"},
							tbody(),
							tfoot( tr( th( "&nbsp;" ) ) )
						)
					)
				)
			)
		)
	)
});

Jaml.register('StatisticsTabContent', function(args) {
	var newIdBy2 = dad.newIdBy2calls;
	table({style: "width:100%;height:100%;"},
		tr(
			td({style: "height:1%;"},
				button({id: "btnShowChart" + args.id}, _tr("Show")),
				input({type: "text", name: "statistics_periodes", size:4, value:"12"}),
				input({type: "radio", name: "statistics_by", id: newIdBy2(), value: "years" }),
				label({'for': newIdBy2()},  _tr("Years")),
				input({type: "radio", name: "statistics_by", id: newIdBy2(), value: "months", checked:"1"}),
				label({'for': newIdBy2()},  _tr("Months")),
				input({type: "radio", name: "statistics_by", id: newIdBy2(), value: "weeks"}),
				label({'for': newIdBy2()},  _tr("Weeks")),
				input({type: "radio", name: "statistics_by", id: newIdBy2(), value: "days"}),
				label({'for': newIdBy2()},  _tr("Days"))
			)
		),
		tr(
			td({style: "width:100%;height:90%;"},
				div({'class': "divChart", style: "width:100%;height:100%"},
					canvas({id:"canvas" + args.id, width:760, height:380},
						"This text is displayed if your browser does not support HTML5 Canvas."
					)
				)
			)
		)
	)
});

function BarChartFillByAjax(id){
	if(this.status == 200){
		var records = dad.parseSLEData(this.responseText);
		var row_count = records.length;
		
		var btnShowChart = $("btnShowChart" + id);
		var chart = btnShowChart._chart;

		//chart.set_bar_num_format_decimals(0);
		chart.bar_begin_update();
		chart.bar_clear();
		for(var i=1; i<row_count; i++){ //start at 1 to skip headers
			var rec = records[i];
			var cl = rec[2] == "S" ? "#11a" : "#1a1";
			chart.bar_add(rec[0], parseFloat(rec[1]), cl);
		}
		chart.bar_end_update();
	} else {
		alert("An error has occured making the request");
	}
}

dad.setupBarchart = function(bid, ajaxParamsFunc){
	var btnShowChart = $("btnShowChart" + bid);
	var canvasChart = $("canvas" + bid);
	btnShowChart._chart = new dad.barChart();
	btnShowChart._chart.init(canvasChart);
	btnShowChart.ajaxData = new dad.Ajax(BarChartFillByAjax, bid, false);
	btnShowChart.ajaxParamsFunc = ajaxParamsFunc;
	btnShowChart.onclick = function(){
		var ajax = btnShowChart.ajaxData;
		var params = this.ajaxParamsFunc();
		var form = this.form;
		var statistics_by = 'months';
		for (var i=form.statistics_by.length-1; i >= 0; --i){ 
		  if (form.statistics_by[i].checked){
				statistics_by = form.statistics_by[i].value;
				break; 
			}
		} 
		params += "&periode_count=" + form.statistics_periodes.value;
		params += "&periode_type=" + statistics_by;
		params += "&query_limit=" + form.history_query_limit.value;
		ajax.post('/DB/GetList', params, "GET");
		return false;
	}
}
