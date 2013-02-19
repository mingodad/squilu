class MainWindow extends MyBaseWindow {
	tabs=null;
	tabSales=null;
	btnOrdersSales=null;
	btnPaymentsSales=null;
	btnEntitiesSales=null;
	btnProductsSales=null;
	tabBuys=null;
	btnOrdersBuys=null;
	btnPaymentsBuys=null;
	btnProductsBuys=null;
	btnEntitiesBuys=null;
	tabAll=null;
	btnOrders=null;
	btnPayments=null;
	btnEntities=null;
	btnProducts=null;
	tabGL=null;
	btnGLGroups=null;
	btnGLChart=null;
	btnGLTransactions=null;
	tabConfig=null;
	config_scroll=null;
	config_pack=null;
	btnOrdersSum=null;
	btnSalesTaxRates=null;
	btnOrderTypes=null;
	btnPaymentTypes=null;
	btnMeasureUnits=null;
	btnWarrantyTypes=null;
	btnImages=null;
	btnProductGroups=null;
	btnEntityGroups=null;
	btnConfig=null;
	btnOpenDB=null;
	btnTranslations=null;
	btnAppUsers=null;
	btnAskForPrinter=null;
	btnWebServer=null;

constructor(px, py, pw, ph, pl) {
	base.constructor(px, py, pw, ph, pl);
	begin();
	this->box(FL_FLAT_BOX);
	this->color(FL_BACKGROUND_COLOR);
	this->selection_color(FL_BACKGROUND_COLOR);
	this->labeltype(FL_NO_LABEL);
	this->labelfont(0);
	this->labelsize(16);
	this->labelcolor(FL_FOREGROUND_COLOR);
	this->align(FL_ALIGN_TOP);
	this->when(FL_WHEN_RELEASE);
	reset_all_child_windows_ptr();
	{ tabs = new Fl_Tabs(5, 5, 320, 310);
	  tabs->selection_color(4);
	  tabs->labelsize(16);
	  tabs->labelcolor(FL_BACKGROUND2_COLOR);
	  { tabSales = new Fl_Group(5, 35, 320, 280, _tr("Sales"));
	    tabSales->color(127);
	    tabSales->labelsize(16);
	    { btnOrdersSales = new Fl_Button(30, 50, 275, 55, _tr("Sales"));
	      btnOrdersSales->labeltype(FL_SHADOW_LABEL);
	      btnOrdersSales->labelfont(3);
	      btnOrdersSales->labelsize(34);
	      btnOrdersSales->labelcolor(FL_INACTIVE_COLOR);
	      btnOrdersSales->callback(cb_btnOrdersSales);
	      btnOrdersSales->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	    } // Fl_Button* btnOrdersSales
	    { btnPaymentsSales = new Fl_Button(30, 116, 275, 55, _tr("Payments"));
	      btnPaymentsSales->labeltype(FL_SHADOW_LABEL);
	      btnPaymentsSales->labelfont(3);
	      btnPaymentsSales->labelsize(34);
	      btnPaymentsSales->labelcolor(FL_INACTIVE_COLOR);
	      btnPaymentsSales->callback(cb_btnPaymentsSales);
	    } // Fl_Button* btnPaymentsSales
	    { btnEntitiesSales = new Fl_Button(30, 183, 275, 55, _tr("Customers"));
	      btnEntitiesSales->labeltype(FL_SHADOW_LABEL);
	      btnEntitiesSales->labelfont(3);
	      btnEntitiesSales->labelsize(34);
	      btnEntitiesSales->labelcolor(FL_INACTIVE_COLOR);
	      btnEntitiesSales->callback(cb_btnEntitiesSales);
	    } // Fl_Button* btnEntitiesSales
	    { btnProductsSales = new Fl_Button(30, 250, 275, 55, _tr("Products"));
	      btnProductsSales->labeltype(FL_SHADOW_LABEL);
	      btnProductsSales->labelfont(3);
	      btnProductsSales->labelsize(34);
	      btnProductsSales->labelcolor(FL_INACTIVE_COLOR);
	      btnProductsSales->callback(cb_btnProductsSales);
	    } // Fl_Button* btnProductsSales
	    tabSales->end();
	  } // Fl_Group* tabSales
	  { tabBuys = new Fl_Group(5, 35, 320, 280, _tr("Buys"));
	    tabBuys->color(174);
	    tabBuys->labelsize(16);
	    tabBuys->hide();
	    { btnOrdersBuys = new Fl_Button(30, 50, 275, 55, _tr("Buys"));
	      btnOrdersBuys->labeltype(FL_SHADOW_LABEL);
	      btnOrdersBuys->labelfont(3);
	      btnOrdersBuys->labelsize(34);
	      btnOrdersBuys->labelcolor(FL_INACTIVE_COLOR);
	      btnOrdersBuys->callback(cb_btnOrdersBuys);
	      btnOrdersBuys->align((FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
	    } // Fl_Button* btnOrdersBuys
	    { btnPaymentsBuys = new Fl_Button(30, 115, 275, 55, _tr("Payments"));
	      btnPaymentsBuys->labeltype(FL_SHADOW_LABEL);
	      btnPaymentsBuys->labelfont(3);
	      btnPaymentsBuys->labelsize(34);
	      btnPaymentsBuys->labelcolor(FL_INACTIVE_COLOR);
	      btnPaymentsBuys->callback(cb_btnPaymentsBuys);
	    } // Fl_Button* btnPaymentsBuys
	    { btnProductsBuys = new Fl_Button(30, 250, 275, 55, _tr("Products"));
	      btnProductsBuys->labeltype(FL_SHADOW_LABEL);
	      btnProductsBuys->labelfont(3);
	      btnProductsBuys->labelsize(34);
	      btnProductsBuys->labelcolor(FL_INACTIVE_COLOR);
	      btnProductsBuys->callback(cb_btnProductsBuys);
	    } // Fl_Button* btnProductsBuys
	    { btnEntitiesBuys = new Fl_Button(30, 183, 275, 55, _tr("Suppliers"));
	      btnEntitiesBuys->labeltype(FL_SHADOW_LABEL);
	      btnEntitiesBuys->labelfont(3);
	      btnEntitiesBuys->labelsize(34);
	      btnEntitiesBuys->labelcolor(FL_INACTIVE_COLOR);
	      btnEntitiesBuys->callback(cb_btnEntitiesBuys);
	    } // Fl_Button* btnEntitiesBuys
	    tabBuys->end();
	  } // Fl_Group* tabBuys
	  { tabAll = new Fl_Group(5, 35, 320, 280, _tr("All"));
	    tabAll->color(239);
	    tabAll->labelsize(16);
	    tabAll->hide();
	    { btnOrders = new Fl_Button(30, 50, 275, 55, _tr("Orders"));
	      btnOrders->labeltype(FL_SHADOW_LABEL);
	      btnOrders->labelfont(3);
	      btnOrders->labelsize(34);
	      btnOrders->labelcolor(FL_INACTIVE_COLOR);
	      btnOrders->callback(cb_btnOrders);
	      btnOrders->align((FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
	    } // Fl_Button* btnOrders
	    { btnPayments = new Fl_Button(30, 116, 275, 55, _tr("Payments"));
	      btnPayments->labeltype(FL_SHADOW_LABEL);
	      btnPayments->labelfont(3);
	      btnPayments->labelsize(34);
	      btnPayments->labelcolor(FL_INACTIVE_COLOR);
	      btnPayments->callback(cb_btnPayments);
	    } // Fl_Button* btnPayments
	    { btnEntities = new Fl_Button(30, 183, 275, 55, _tr("Entities"));
	      btnEntities->labeltype(FL_SHADOW_LABEL);
	      btnEntities->labelfont(3);
	      btnEntities->labelsize(34);
	      btnEntities->labelcolor(FL_INACTIVE_COLOR);
	      btnEntities->callback(cb_btnEntities);
	    } // Fl_Button* btnEntities
	    { btnProducts = new Fl_Button(30, 250, 275, 55, _tr("Products"));
	      btnProducts->labeltype(FL_SHADOW_LABEL);
	      btnProducts->labelfont(3);
	      btnProducts->labelsize(34);
	      btnProducts->labelcolor(FL_INACTIVE_COLOR);
	      btnProducts->callback(cb_btnProducts);
	    } // Fl_Button* btnProducts
	    tabAll->end();
	    Fl_Group.current()->resizable(tabAll);
	  } // Fl_Group* tabAll
	  { tabGL = new Fl_Group(5, 35, 320, 280, _tr("GL"));
	    tabGL->color(132);
	    tabGL->labelsize(16);
	    tabGL->hide();
	    { btnGLGroups = new Fl_Button(30, 50, 275, 55, _tr("Groups"));
	      btnGLGroups->labeltype(FL_SHADOW_LABEL);
	      btnGLGroups->labelfont(3);
	      btnGLGroups->labelsize(34);
	      btnGLGroups->labelcolor(FL_INACTIVE_COLOR);
	      btnGLGroups->callback(cb_btnGLGroups);
	      btnGLGroups->align((FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
	    } // Fl_Button* btnGLGroups
	    { btnGLChart = new Fl_Button(30, 115, 275, 55, _tr("Chart"));
	      btnGLChart->labeltype(FL_SHADOW_LABEL);
	      btnGLChart->labelfont(3);
	      btnGLChart->labelsize(34);
	      btnGLChart->labelcolor(FL_INACTIVE_COLOR);
	      btnGLChart->callback(cb_btnGLChart);
	    } // Fl_Button* btnGLChart
	    { btnGLTransactions = new Fl_Button(30, 180, 275, 55, _tr("Transactions"));
	      btnGLTransactions->labeltype(FL_SHADOW_LABEL);
	      btnGLTransactions->labelfont(3);
	      btnGLTransactions->labelsize(34);
	      btnGLTransactions->labelcolor(FL_INACTIVE_COLOR);
	      btnGLTransactions->callback(cb_btnGLTransactions);
	    } // Fl_Button* btnGLTransactions
	    tabGL->end();
	  } // Fl_Group* tabGL
	  { tabConfig = new Fl_Group(5, 35, 320, 280, _tr("Config"));
	    tabConfig->labelsize(16);
	    tabConfig->hide();
	    { config_scroll = new Fl_Scroll(10, 45, 310, 265);
	      config_scroll->labelsize(16);
	      { local o = config_pack = new Fl_Pack(30, 45, 270, 260);
		config_pack->labelsize(16);
		{ btnOrdersSum = new Fl_Button(65, 55, 64, 30, _tr("Orders Sum"));
		  btnOrdersSum->labelsize(16);
		  btnOrdersSum->callback(cb_btnOrdersSum);
		} // Fl_Button* btnOrdersSum
		{ btnSalesTaxRates = new Fl_Button(55, 45, 64, 30, _tr("Sales Tax Rates"));
		  btnSalesTaxRates->labelsize(16);
		  btnSalesTaxRates->callback(cb_btnSalesTaxRates);
		} // Fl_Button* btnSalesTaxRates
		{ btnOrderTypes = new Fl_Button(65, 80, 64, 30, _tr("Order Types"));
		  btnOrderTypes->labelsize(16);
		  btnOrderTypes->callback(cb_btnOrderTypes);
		} // Fl_Button* btnOrderTypes
		{ btnPaymentTypes = new Fl_Button(105, 148, 64, 30, _tr("Payment Types"));
		  btnPaymentTypes->labelsize(16);
		  btnPaymentTypes->callback(cb_btnPaymentTypes);
		} // Fl_Button* btnPaymentTypes
		{ btnMeasureUnits = new Fl_Button(115, 158, 64, 30, _tr("Measure Units"));
		  btnMeasureUnits->labelsize(16);
		  btnMeasureUnits->callback(cb_btnMeasureUnits);
		} // Fl_Button* btnMeasureUnits
		{ btnWarrantyTypes = new Fl_Button(125, 168, 64, 30, _tr("Warranty Types"));
		  btnWarrantyTypes->labelsize(16);
		  btnWarrantyTypes->callback(cb_btnWarrantyTypes);
		} // Fl_Button* btnWarrantyTypes
		{ btnImages = new Fl_Button(75, 118, 64, 30, _tr("Images"));
		  btnImages->labelsize(16);
		  btnImages->callback(cb_btnImages);
		} // Fl_Button* btnImages
		{ btnProductGroups = new Fl_Button(85, 128, 64, 30, _tr("Product Groups"));
		  btnProductGroups->labelsize(16);
		  btnProductGroups->callback(cb_btnProductGroups);
		} // Fl_Button* btnProductGroups
		{ btnEntityGroups = new Fl_Button(95, 138, 64, 30, _tr("Entity Groups"));
		  btnEntityGroups->labelsize(16);
		  btnEntityGroups->callback(cb_btnEntityGroups);
		} // Fl_Button* btnEntityGroups
		{ btnConfig = new Fl_Button(105, 148, 64, 30, _tr("App Config"));
		  btnConfig->labelsize(16);
		  btnConfig->callback(cb_btnConfig);
		} // Fl_Button* btnConfig
		{ btnOpenDB = new Fl_Button(115, 158, 64, 30, _tr("Open Database"));
		  btnOpenDB->tooltip(_tr("Open a database file"));
		  btnOpenDB->labelsize(16);
		  btnOpenDB->callback(cb_btnOpenDB);
		} // Fl_Button* btnOpenDB
		{ btnTranslations = new Fl_Button(115, 158, 64, 30, _tr("Translations"));
		  btnTranslations->labelsize(16);
		  btnTranslations->callback(cb_btnTranslations);
		} // Fl_Button* btnTranslations
		{ btnAppUsers = new Fl_Button(75, 65, 64, 30, _tr("App Users"));
		  btnAppUsers->labelsize(16);
		  btnAppUsers->callback(cb_btnAppUsers);
		} // Fl_Button* btnAppUsers
		{ btnAskForPrinter = new Fl_Check_Button(55, 70, 64, 15, _tr("Ask for Printer"));
		  btnAskForPrinter->down_box(FL_DOWN_BOX);
		} // Fl_Check_Button* btnAskForPrinter
		{ btnWebServer = new Fl_Check_Button(65, 80, 64, 15, _tr("Web Server"));
		  btnWebServer->down_box(FL_DOWN_BOX);
		} // Fl_Check_Button* btnWebServer
		o->spacing(5);
		config_pack->end();
	      } // Fl_Pack* config_pack
	      config_scroll->end();
	    } // Fl_Scroll* config_scroll
	    tabConfig->end();
	  } // Fl_Group* tabConfig
	  tabs->end();
	} // Fl_Tabs* tabs
	end();
}

function cb_btnEntitiesSales(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Entities Sales List/Search", EntitiesListSearch);
}
function cb_btnPaymentsSales(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Payments Sales List/Search", PaymentsListSearch);
}
function cb_btnOrdersSales(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Orders Sales List/Search", OrdersListSearch);
}
function cb_btnProductsSales(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Products Sales List/Search", ProductsListSearch);
}
function cb_btnOrdersBuys(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Orders Buys List/Search", OrdersListSearch);
}
function cb_btnPaymentsBuys(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Payments Buys List/Search", PaymentsListSearch);
}
function cb_btnProductsBuys(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Products Buys List/Search", ProductsListSearch);
}
function cb_btnEntitiesBuys(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Entities Buys List/Search", EntitiesListSearch);
}
function cb_btnOrders(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Orders List/Search", OrdersListSearch);
}
function cb_btnPayments(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Payments List/Search", PaymentsListSearch);
}
function cb_btnEntities(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Entities List/Search", EntitiesListSearch);
}
function cb_btnProducts(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Products List/Search", ProductsListSearch);
}
function cb_btnGLGroups(){print(__LINE__);}
function cb_btnGLChart(){print(__LINE__);}
function cb_btnGLTransactions(){print(__LINE__);}
function cb_btnOrdersSum(){print(__LINE__);}
function cb_btnSalesTaxRates(sender, udata){
	local pr = sender.parent_root();
	local win = pr.showChildWindow("Sales Tax Rates List/Edit", OurSalesTax);
}
function cb_btnOrderTypes(){print(__LINE__);}
function cb_btnPaymentTypes(){print(__LINE__);}
function cb_btnMeasureUnits(){print(__LINE__);}
function cb_btnWarrantyTypes(){print(__LINE__);}
function cb_btnImages(){print(__LINE__);}
function cb_btnProductGroups(){print(__LINE__);}
function cb_btnEntityGroups(){print(__LINE__);}
function cb_btnConfig(){print(__LINE__);}
function cb_btnOpenDB(){print(__LINE__);}
function cb_btnTranslations(){print(__LINE__);}
function cb_btnAppUsers(){print(__LINE__);}
function reset_all_child_windows_ptr(){print(__LINE__);}
}
