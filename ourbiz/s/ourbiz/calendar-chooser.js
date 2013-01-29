Jaml.register('CalendarChooserWindow', function(args) {
	var callOpFunc = "CalendarChooserSetDate('" + args.id + "','";
	var callOp = function(op){return callOpFunc + op + "')";}
	var cbtn = function() {return input({type:"button"});}
	form({id: args.form_id, style: "width:100%;height:100%;", onsubmit:"return false;"},
		table({style: "width:100%;", cls:"calTbl"},
			tr(
				td({id: "cc_header" + args.id, cls: "fw hcenter", style: "height: 10%;"}, "date")
			),
			tr(
				td({cls: "fw cc_month", style: "height: 20%;"},
					button({onclick: callOp('m1')}, "1"),
					button({onclick: callOp('m2')}, "2"),
					button({onclick: callOp('m3')}, "3"),
					button({onclick: callOp('m4')}, "4"),
					button({onclick: callOp('m5')}, "5"),
					button({onclick: callOp('m6')}, "6"),
					button({onclick: callOp('m7')}, "7"),
					button({onclick: callOp('m8')}, "8"),
					button({onclick: callOp('m9')}, "9"),
					button({onclick: callOp('m10')}, "10"),
					button({onclick: callOp('m11')}, "11"),
					button({onclick: callOp('m12')}, "12")
				)
			),
			tr(
				td(
					table({style: "width:100%;height:60%", cls: "cc_days", id: "cc_days_month" + args.id},
						tr(
							th(_tr("Mo")),th(_tr("Tu")),th(_tr("We")),th(_tr("Th")),th(_tr("Fr")),th(_tr("Sa")),th(_tr("Su"))
						),
						tr(
							td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn())
						),
						tr(
							td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn())
						),
						tr(
							td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn())
						),
						tr(
							td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn())
						),
						tr(
							td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn())
						),
						tr(
							td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn()),td(cbtn())
						)
					)
				)
			),
			tr(
				td({cls: "fw cc_nav", style: "height: 10%;"},
					button({onclick: callOp('-y')}, "&lt;&lt;"),
					button({onclick: callOp('-m')}, "&lt;"),
					button({onclick: callOp('0')}, "&lt;&gt;"),
					button({onclick: callOp('0')}, "@"),
					button({onclick: callOp('+m')}, "&gt;"),
					button({onclick: callOp('+y')}, "&gt;&gt;")
				)
			)
		)
	);
});

function CalendarChooserMakemonth(y,m){
	var DAY_SECONDS = 86400;
	var DAY_MILISECONDS = DAY_SECONDS * 1000;
	var monthArray = [];
	var adate = new Date();
	adate.setDate(1);
	adate.setMonth(m);
	adate.setFullYear(y);
	// get back to the nearest monday
	adate.setTime(adate.getTime()-(DAY_MILISECONDS * (adate.getDay()-1)));
	var day = adate.getDate();
	if(day < 7 && day != 1){
		//go back one more week
		adate.setTime(adate.getTime() - (DAY_MILISECONDS * 7));
	}
	do{
		var weekArray = [];
		monthArray.push(weekArray);
		do { // insert the week days
			weekArray.push(adate.getDate());
			adate.setTime(adate.getTime() + DAY_MILISECONDS);
		} while( adate.getDay() != 1 ) // monday
	} while( adate.getMonth() == m );
	return monthArray;
}

function CalendarChooserSetDate(id, op){
	//alert(table_month_id + ':' + op)
	var table_month = document.getElementById("cc_days_month" + id);
	var date = table_month._date ? table_month._date : 0;
	switch(op){
		case '-y': date.setFullYear(date.getFullYear()-1); break;
		case '+y': date.setFullYear(date.getFullYear()+1); break;
		case '-m': date.setMonth(date.getMonth()-1); break;
		case '+m': date.setMonth(date.getMonth()+1); break;
		case 'm1': date.setMonth(0); break;
		case 'm2': date.setMonth(1); break;
		case 'm3': date.setMonth(2); break;
		case 'm4': date.setMonth(3); break;
		case 'm5': date.setMonth(4); break;
		case 'm6': date.setMonth(5); break;
		case 'm7': date.setMonth(6); break;
		case 'm8': date.setMonth(7); break;
		case 'm9': date.setMonth(8); break;
		case 'm10': date.setMonth(9); break;
		case 'm11': date.setMonth(10); break;
		case 'm12': date.setMonth(11); break;
		default:
			table_month._date = new Date();
	}

	var cc_header = document.getElementById("cc_header" + id);
	cc_header.innerHTML = table_month._date.toLocaleString();
	
	var monthArray = CalendarChooserMakemonth(table_month._date.getFullYear(), table_month._date.getMonth());
	for(var i = 0; i < monthArray.length; i++){
		var week = monthArray[i];
		var row = table_month.rows[i+1];
		var cells = row.cells;
		for(var j=0; j < week.length; j++){
			dad.getFirstChild(cells[j]).value = week[j];
		}
	}
}		

function CalendarChooserWindowOnTableCellClick(){
}

function CalendarChooserFocusUpDown(elm, up){
	var table = dad.getFirstParentWithTagName(elm, "TABLE");
	var getFirstChild = dad.getFirstChild;
	var rows = table.tBodies[0].rows;
	for(var r=rows.length-1; r >= 0; --r){
		var cells = rows[r].cells;
		for(var c=cells.length-1; c >= 0; --c){
			if(getFirstChild(cells[c]) == elm){
				if(up){
					if(r > 1){ //second row and bellow
						getFirstChild(rows[r-1].cells[c]).focus();
						return true;
					}
				} else if(r < rows.length-1) {
					getFirstChild(rows[r+1].cells[c]).focus();
					return true;
				}
			}
		}
	}
	return false;
}

function CalendarChooserInputOnKeydown(event){
	var evt = dad.checkEvent(event);
	//alert(dad.getEventSource(evt).name);

	switch(dad.getEventKey(evt)){
		case 40: //arrow down
			if(CalendarChooserFocusUpDown(this, false)) return false;
		break;
		case 38: //arrow up
			if(CalendarChooserFocusUpDown(this, true)) return false;
		break;
		case 37: //arrow left
			dad.formFocusNextFromEvent(evt, true);
			return false;
		break;
		case 39: //arrow right
			dad.formFocusNextFromEvent(evt);
			return false;
		break;
		case 13: //enter
		case 32: //space bar
		break;
	}
}	

function newCalendarChooserWindow(){
	var title = "Calendar Chooser"
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var Action_id = 'Action' + newId;
		var btnAction_id = 'btnAction' + newId;
		var table_id = 'table' + newId;
		var form_id = 'form' + newId;
		var data = {
			id: newId,
			form_id: form_id,
			Action_id: Action_id,
			btnAction_id: btnAction_id,
			table_id: table_id
		}
		var win = dad.newWindow(newId, 220,20, 300, 0, _tr(title), Jaml.render('CalendarChooserWindow', data));
		CalendarChooserSetDate(newId, 0);
		
		var myform = document.getElementById(data.form_id);
		for ( var i = 0, cnl = myform.length; i < cnl; i++ ) {
			var elm = myform[i];
			if ( dad.isFocusableTag(elm.tagName) ){
				elm.onkeydown = CalendarChooserInputOnKeydown;
			}
		}
	}
	dad.bringToFront(win)
}
