function _tr(str){ return str;}

class Fl_Data_Table extends Flv_Table {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}
	
class SalesTaxRatesEditWindow extends Fl_Window {
	grid=null;
	editGroup=null;
	db_sales_tax_rates_id=null;
	db_sales_tax_rates_cdate=null;
	db_sales_tax_rates_mdate=null;
	db_sales_tax_rates_rate1=null;
	db_sales_tax_rates_rate2=null;
	db_sales_tax_rates_is_active=null;
	db_sales_tax_rates_description=null;
	dbAction=null;
	btnDbAction= null;
	
constructor() {
	base.constructor(10, 50, 500, 385, _tr("Sales Tax Rates List / Edit"));
	dbAction = new Fl_Button(0, 0, 100, 25, _tr("Action"));
	btnDbAction = new Fl_Button(0, 0, 100, 25, _tr("Action"));
	begin();
	this->box(FL_FLAT_BOX);
	this->color(FL_BACKGROUND_COLOR);
	this->selection_color(FL_BACKGROUND_COLOR);
	this->labeltype(FL_NO_LABEL);
	this->labelfont(0);
	this->labelsize(16);
	this->labelcolor(FL_FOREGROUND_COLOR);
	this->align((FL_ALIGN_TOP));
	this->when(FL_WHEN_RELEASE);
	{ grid = new Fl_Data_Table(5, 5, 490, 200);
	  grid->tooltip(_tr("List of available sales tax"));
	  grid->type(3);
	  grid->box(FL_NO_BOX);
	  grid->color(FL_BACKGROUND2_COLOR);
	  grid->selection_color(FL_SELECTION_COLOR);
	  grid->labeltype(FL_NO_LABEL);
	  grid->labelfont(0);
	  grid->labelsize(16);
	  grid->labelcolor(FL_FOREGROUND_COLOR);
	  grid->textsize(16);
	  grid->align((FL_ALIGN_BOTTOM));
	  grid->when(FL_WHEN_RELEASE_ALWAYS);
	  Fl_Group.current()->resizable(grid);
	} // Fl_Data_Table* grid
	{ editGroup = new Fl_Group(5, 215, 490, 165);
	  editGroup->box(FL_ENGRAVED_BOX);
	  editGroup->color(246);
	  editGroup->labelsize(16);
	  editGroup->align((FL_ALIGN_RIGHT));
	  { db_sales_tax_rates_id = new Fl_Output(15, 240, 82, 26, _tr("ID"));
	    db_sales_tax_rates_id->tooltip(_tr("The unique identifier"));
	    db_sales_tax_rates_id->labelsize(16);
	    db_sales_tax_rates_id->textsize(16);
	    db_sales_tax_rates_id->align((FL_ALIGN_TOP_LEFT));
	    add_input_field_to_map("sales_tax_rates", "id", db_sales_tax_rates_id);
	  } // Fl_Output* db_sales_tax_rates_id
	  { db_sales_tax_rates_cdate = new Fl_Output(115, 240, 120, 25, _tr("CDate"));
	    db_sales_tax_rates_cdate->tooltip(_tr("The creation date"));
	    db_sales_tax_rates_cdate->labelsize(16);
	    db_sales_tax_rates_cdate->textsize(16);
	    db_sales_tax_rates_cdate->align((FL_ALIGN_TOP_LEFT));
	    add_input_field_to_map("sales_tax_rates", "cdate", db_sales_tax_rates_cdate);
	  } // Fl_Output* db_sales_tax_rates_cdate
	  { db_sales_tax_rates_mdate = new Fl_Output(255, 240, 120, 25, _tr("MDate"));
	    db_sales_tax_rates_mdate->tooltip(_tr("The modification date"));
	    db_sales_tax_rates_mdate->labelsize(16);
	    db_sales_tax_rates_mdate->textsize(16);
	    db_sales_tax_rates_mdate->align((FL_ALIGN_TOP_LEFT));
	    add_input_field_to_map("sales_tax_rates", "mdate", db_sales_tax_rates_mdate);
	  } // Fl_Output* db_sales_tax_rates_mdate
	  { db_sales_tax_rates_rate1 = new Fl_Float_Input(15, 295, 82, 25, _tr("V.A.T. %"));
	    db_sales_tax_rates_rate1->tooltip(_tr("Main sales tax percentage"));
	    db_sales_tax_rates_rate1->type(1);
	    db_sales_tax_rates_rate1->labelsize(16);
	    db_sales_tax_rates_rate1->textfont(1);
	    db_sales_tax_rates_rate1->textsize(16);
	    db_sales_tax_rates_rate1->align((FL_ALIGN_TOP_LEFT));
	    add_input_field_to_map("sales_tax_rates", "rate1", db_sales_tax_rates_rate1);
	  } // Fl_Float_Input* db_sales_tax_rates_rate1
	  { db_sales_tax_rates_rate2 = new Fl_Float_Input(115, 295, 82, 25, _tr("R.E. %"));
	    db_sales_tax_rates_rate2->tooltip(_tr("Secondary sales tax percentage"));
	    db_sales_tax_rates_rate2->type(1);
	    db_sales_tax_rates_rate2->labelsize(16);
	    db_sales_tax_rates_rate2->textfont(1);
	    db_sales_tax_rates_rate2->textsize(16);
	    db_sales_tax_rates_rate2->align((FL_ALIGN_TOP_LEFT));
	    add_input_field_to_map("sales_tax_rates", "rate2", db_sales_tax_rates_rate2);
	  } // Fl_Float_Input* db_sales_tax_rates_rate2
	  { db_sales_tax_rates_is_active = new Fl_Check_Button(255, 294, 100, 23, _tr("Active"));
	    db_sales_tax_rates_is_active->tooltip(_tr("We can deactivate instead of delete it"));
	    db_sales_tax_rates_is_active->down_box(FL_DOWN_BOX);
	    db_sales_tax_rates_is_active->labelsize(16);
	    add_input_field_to_map("sales_tax_rates", "is_active", db_sales_tax_rates_is_active);
	  } // Fl_Check_Button* db_sales_tax_rates_is_active
	  { db_sales_tax_rates_description = new Fl_Input(15, 345, 350, 25, _tr("Description"));
	    db_sales_tax_rates_description->tooltip(_tr("A friendly description to identify this tax"));
	    db_sales_tax_rates_description->labelsize(16);
	    db_sales_tax_rates_description->textfont(1);
	    db_sales_tax_rates_description->textsize(16);
	    db_sales_tax_rates_description->align((FL_ALIGN_TOP_LEFT));
	    add_input_field_to_map("sales_tax_rates", "description", db_sales_tax_rates_description);
	  } // Fl_Input* db_sales_tax_rates_description
	  { 
	    Fl_Group.current()->add(dbAction);
	    dbAction->resize(380, 310, 105, 25);
	    dbAction->down_box(FL_BORDER_BOX);
	    dbAction->labeltype(FL_NO_LABEL);
	    dbAction->labelsize(16);
	    dbAction->textsize(16);
	  } // Fl_Choice* dbAction
	  { 
	    Fl_Group.current()->add(btnDbAction);
	    btnDbAction->resize(380, 345, 105, 25);
	    btnDbAction->label(_tr("Action"));
	    btnDbAction->labelsize(16);
	  } // Fl_Button* btnDbAction
	  editGroup->end();
	} // Fl_Group* editGroup
	end();
}

function add_input_field_to_map(tbl, fldname, fld){}
}


class MainWindow extends Fl_Window {
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
function cb_btnEntitiesSales(){print(__LINE__);}
function cb_btnPaymentsSales(){print(__LINE__);}
function cb_btnOrdersSales(sender, udata){print(__LINE__, sender, udata);}
function cb_btnProductsSales(){print(__LINE__);}
function cb_btnOrdersBuys(){print(__LINE__);}
function cb_btnPaymentsBuys(){print(__LINE__);}
function cb_btnProductsBuys(){print(__LINE__);}
function cb_btnEntitiesBuys(){print(__LINE__);}
function cb_btnOrders(){print(__LINE__);}
function cb_btnPayments(){print(__LINE__);}
function cb_btnEntities(){print(__LINE__);}
function cb_btnProducts(){print(__LINE__);}
function cb_btnGLGroups(){print(__LINE__);}
function cb_btnGLChart(){print(__LINE__);}
function cb_btnGLTransactions(){print(__LINE__);}
function cb_btnOrdersSum(){print(__LINE__);}
function cb_btnSalesTaxRates(sender, udata){
	local win = new SalesTaxRatesEditWindow();
	win.show();
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
local win = new MainWindow(10, 50, 330, 320, "OURBIZ");
//local win = new SalesTaxRatesEditWindow();
win->resizable(win);
win->show_main();

math.number_format_set_dec_point(",");
math.number_format_set_thousand_sep(".");
Fl_Input.default_number_format("\0x02,.");
//Fl:scheme("plastic");
Fl.scheme("gtk+");
//use partial match to find verdana font
Fl.visual(FL_RGB);
//allow arrow keys navigation
Fl.option(Fl.OPTION_ARROW_FOCUS, true);

Fl.run();
