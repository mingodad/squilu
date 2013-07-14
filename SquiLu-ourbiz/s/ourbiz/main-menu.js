function newMainOrdersListSearchWindow(otype){
	//dad.loadScript('orders.js');
	if(window.newOrdersListSearchWindow) newOrdersListSearchWindow(otype);
}

function newMainPaymentsListSearchWindow(otype){
	//dad.loadScript('products.js');
	if(window.newPaymentsListSearchWindow) newPaymentsListSearchWindow(otype);
}

function newMainProductsListSearchWindow(otype){
	//dad.loadScript('products.js');
	if(window.newProductsListSearchWindow) newProductsListSearchWindow(otype);
}

function newMainEntitiesListSearchWindow(otype){
	//dad.loadScript('entities.js');
	if(window.newEntitiesListSearchWindow) newEntitiesListSearchWindow(otype);
}

Jaml.register('MainMenu', function(args) {
	form({id: args.form_id, onsubmit:"return false;"},
		table({'class':"content mainmenu"},
			tr({id: args.tabs_id, "class": "tabs"},
				td(
					button({name:"sales" + args.id}, _tr('Sales')),
					button({name:"buys" + args.id}, _tr('Buys')),
					button({name:"all" + args.id}, _tr('All')),
					button({name:"gl" + args.id}, _tr('GL')),
					button({name:"config" + args.id}, _tr('Config'))
				)
			),
			tr({id:"sales" + args.id, 'class':"tabContent"},
				td(
					button({onclick:"newMainOrdersListSearchWindow('S')"}, _tr('Sales')),
					button({onclick:"newMainPaymentsListSearchWindow('S')"}, _tr('Payments')),
					button({onclick:"newMainProductsListSearchWindow('S')"}, _tr('Products')),
					button({onclick:"newMainEntitiesListSearchWindow('S')"}, _tr('Entities'))
				)
			),
			tr({id:"buys" + args.id, 'class':"tabContent"},
				td(
					button({onclick:"newMainOrdersListSearchWindow('B')"}, _tr('Buys')),
					button({onclick:"newMainPaymentsListSearchWindow('B')"}, _tr('Payments')),
					button({onclick:"newMainProductsListSearchWindow('B')"}, _tr('Products')),
					button({onclick:"newMainEntitiesListSearchWindow('B')"}, _tr('Entities'))
				)
			),
			tr({id:"all" + args.id, 'class':"tabContent"},
				td(
					button({onclick:"newMainOrdersListSearchWindow()"}, _tr('Orders')),
					button({onclick:"newMainPaymentsListSearchWindow()"}, _tr('Payments')),
					button({onclick:"newMainProductsListSearchWindow()"}, _tr('Products')),
					button({onclick:"newMainEntitiesListSearchWindow()"}, _tr('Entities'))
				)
			),
			tr({id:"gl" + args.id, 'class':"tabContent"},
				td(
					button({onclick:"newGLGroupsListEditWindow()"}, _tr('Groups')),
					button({onclick:"newGLChartListSearchWindow()"}, _tr('Accounts')),
					button({onclick:"newGLTransationsListSearchWindow()"}, _tr('Transactions'))
				)
			),
			tr({id:"config" + args.id, 'class':"tabContent"},
				td(
					button({onclick:"newAjaxDataTableWindow()", cls:"cfg"}, _tr('Ajax Data Table')),
					button({onclick:"newSalesTaxListEditWindow()", cls:"cfg"}, _tr('Sales taxes')),
					button({onclick:"newOrderTypesListEditWindow()", cls:"cfg"}, _tr('Order Types')),
					button({onclick:"newPaymentTypesListEditWindow()", cls:"cfg"}, _tr('Payment Types')),
					button({onclick:"newMeasureUnitsListEditWindow()", cls:"cfg"}, _tr('Measure Units')),
					button({onclick:"newWarrantyTypesListEditWindow()", cls:"cfg"}, _tr('Warranty Types')),
					button({onclick:"newSalesTaxListEditWindow()", cls:"cfg"}, _tr('Images')),
					button({onclick:"newProductGroupsListEditWindow()", cls:"cfg"}, _tr('Product Groups')),
					button({onclick:"newEntityGroupsListEditWindow()", cls:"cfg"}, _tr('Entity Groups')),
					button({onclick:"newSalesTaxListEditWindow()", cls:"cfg"}, _tr('App Config')),
					button({onclick:"newSalesTaxListEditWindow()", cls:"cfg"}, _tr('Open Database'))
				)
			)
		)
	)
});

function newMainMenuWindow(){
	var title = _tr("Main Menu");
	var win = dad._windows[title];
	if(!win){
		var newId = dad.newSeqId();
		var tabs_id = 'tabs' + newId;
		var data = {
			id: newId,
			form_id: 'form' + newId,
			tabs_id: tabs_id
		}
		win = dad.newWindow(newId,10,10, 0, 0, title, Jaml.render('MainMenu', data), true, true);
		win.className = win.className + ' mainWindow';
		if(dad.isIE && (dad.isIE < 9)) win.firstChild.style.width = '';
		var tabsObj = $id(tabs_id);
		dad.initTab(tabsObj)
		//dad.setupKeyboardNavigation(tabsObj, ["A", "LI"]);
		var myform = $id(data.form_id);
		dad.setupEditForm(myform);
	}
	dad.bringToFront(win);

	dad.addEvent(document,'contextmenu',function(e){
		var evt = dad.checkEvent(event);
		if(evt.srcElement == document){ //only with keyboard menu key
			//We don't want the browsers Context menu appearing - so disable it.
			dad.cancelEvent(evt, true);
			dad.bringToFront(win);
		}
	});	
}

var _transalations_ = {
	'Sales' : 'Ventas',
	'Buys' : 'Compras',
	'Payments' : 'Pagos',
	'Products' : 'Productos',
	'Entity' : 'Entidad',
	'Entities' : 'Entidades',
	'List Search' : 'Listado búsqueda',
	'Main Menu' : 'Menu Principal'
}

window.onload = function(){
	dad.onLoadInit();
	newMainMenuWindow();
}
