class Fl_Image_Box extends Fl_Box {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}


class Fl_Choice_Int extends Fl_Button {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Float_Input_Fmt extends Fl_Float_Input {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class My_Fl_Float_Input extends Fl_Float_Input {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}


class My_Fl_Return_Button extends Fl_Button {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Fl_Choice_Str extends Fl_Button {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}
	
//class EditProductWindow extends Fl_Group {
class EditProductWindow extends EditWindow {
	db_products_id=null;
	db_products_description_ro=null;
	db_products_mdate=null;
	db_products_cdate=null;
	tabs=null;
	tabMain=null;
	db_products_reference_code=null;
	db_products_supplier_code=null;
	db_products_bar_code=null;
	db_products_sell_description=null;
	db_products_buy_description=null;
	db_products_sell_price=null;
	db_products_measure_unit_id=null;
	db_products_sales_tax_id=null;
	db_products_warranty_id=null;
	db_products_sell_quantity_min=null;
	db_products_units_by_package=null;
	db_products_weight=null;
	db_products_sell_without_stock=null;
	db_products_is_active=null;
	db_products_show_on_sales=null;
	db_products_show_on_buys=null;
	db_products_show_on_web=null;
	db_products_show_price_on_web=null;
	tabNotes=null;
	db_products_sell_notes=null;
	tabNotesSupplier=null;
	db_products_buy_notes=null;
	tabTags=null;
	db_products_tags=null;
	tabGroups=null;
	btnImage=null;
	db_products_quantity_onhand=null;
	tabPrices=null;
	db_products_buy_price=null;
	db_products_buy_discount=null;
	db_products_buy_other_costs=null;
	db_products_sell_markup=null;
	markup_to_discount=null;
	db_products_sell_price_ro=null;
	db_products_price_decimals=null;
	btnSaveProrduct=null;
	db_products_stock_min=null;
	db_products_stock_max=null;
	db_products_buy_quantity_min=null;
	db_products_price_formula=null;
	db_products_price_date=null;
	productPrices=null;
	tabKit=null;
	tabChartStatistics=null;
	tabHistory=null;
	tabReports=null;
	grpExportImport=null;
	btnProductsExport=null;
	btnProductsImport=null;
	chkWithoutPrices=null;
	chkWebOnly=null;
	boxExportImportCount=null;
	btnProductGroupsExport=null;
	btnProductGroupsImport=null;
	btnUpdateWeb=null;
	grpPrintPriceList=null;
	chkPriceListWithVAT=null;
	chkPriceListSellOnWeb=null;
	btnPrintPriceList=null;
	chkPriceListSellSales=null;
	chkPriceListSellPDF=null;
	tabTotals=null;
	db_products_quantity_quoted_sales=null;
	db_products_quantity_ordered_sales=null;
	db_products_quantity_delivered=null;
	db_products_quantity_invoiced_sales=null;
	db_products_quantity_lost=null;
	db_products_quantity_quoted_buys=null;
	db_products_quantity_ordered_buys=null;
	db_products_quantity_received=null;
	db_products_quantity_invoiced_buys=null;
	dbAction=null;
	btnDbAction=null;
	db_products_group_id=null;
	
constructor() {
	base.constructor(10, 50, 800, 560, _tr("Edit Products"));
begin();
this->box(FL_FLAT_BOX);
this->color(FL_BACKGROUND_COLOR);
this->selection_color(FL_BACKGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(14);
this->labelcolor(FL_FOREGROUND_COLOR);
this->align((FL_ALIGN_TOP));
this->when(FL_WHEN_RELEASE);
{ db_products_id =new Fl_Output(5, 5, 96, 25);
  db_products_id->labeltype(FL_NO_LABEL);
  db_products_id->labelsize(16);
  db_products_id->textsize(16);
  add_input_field_to_map("products", "id", db_products_id);
} // Fl_Output* db_products_id
{ db_products_description_ro =new Fl_Output(110, 5, 545, 25);
  db_products_description_ro->labeltype(FL_NO_LABEL);
  db_products_description_ro->labelsize(16);
  db_products_description_ro->textsize(16);
} // Fl_Output* db_products_description_ro
{ db_products_mdate =new Fl_Output(666, 5, 130, 25);
  db_products_mdate->labeltype(FL_NO_LABEL);
  db_products_mdate->labelsize(16);
  db_products_mdate->textsize(16);
  add_input_field_to_map("products", "mdate", db_products_mdate);
} // Fl_Output* db_products_mdate
{ db_products_cdate =new Fl_Output(665, 35, 130, 25);
  db_products_cdate->labeltype(FL_NO_LABEL);
  db_products_cdate->labelsize(16);
  db_products_cdate->textsize(16);
  add_input_field_to_map("products", "cdate", db_products_cdate);
} // Fl_Output* db_products_cdate
{ tabs =new Fl_Tabs(5, 35, 790, 520);
  tabs->selection_color(4);
  tabs->labelsize(16);
  tabs->labelcolor(FL_BACKGROUND2_COLOR);
  { tabMain =new Fl_Group(5, 60, 790, 495, _tr("Main"));
    tabMain->color(246);
    tabMain->labelsize(16);
    { db_products_reference_code =new Fl_Input(115, 70, 165, 25, _tr("Reference"));
      db_products_reference_code->labelsize(16);
      db_products_reference_code->textsize(16);
      add_input_field_to_map("products", "reference_code", db_products_reference_code);
    } // Fl_Input* db_products_reference_code
    { db_products_supplier_code =new Fl_Input(380, 70, 165, 25, _tr("Ref. Supp."));
      db_products_supplier_code->labelsize(16);
      db_products_supplier_code->textsize(16);
      add_input_field_to_map("products", "supplier_code", db_products_supplier_code);
    } // Fl_Input* db_products_supplier_code
    { db_products_bar_code =new Fl_Input(620, 70, 165, 25, _tr("Barcode"));
      db_products_bar_code->labelsize(16);
      db_products_bar_code->textsize(16);
      add_input_field_to_map("products", "bar_code", db_products_bar_code);
    } // Fl_Input* db_products_bar_code
    { db_products_sell_description =new Fl_Input(115, 100, 670, 25, _tr("Description"));
      db_products_sell_description->labelsize(16);
      db_products_sell_description->textsize(16);
      add_input_field_to_map("products", "sell_description", db_products_sell_description);
    } // Fl_Input* db_products_sell_description
    { db_products_buy_description =new Fl_Input(115, 130, 670, 25, _tr("Desc. Supp."));
      db_products_buy_description->labelsize(16);
      db_products_buy_description->textsize(16);
      add_input_field_to_map("products", "buy_description", db_products_buy_description);
    } // Fl_Input* db_products_buy_description
    { db_products_sell_price =new Fl_Float_Input_Fmt(115, 160, 110, 25, _tr("Sales Price"));
      db_products_sell_price->type(1);
      db_products_sell_price->box(FL_DOWN_BOX);
      db_products_sell_price->color(FL_BACKGROUND2_COLOR);
      db_products_sell_price->selection_color(FL_SELECTION_COLOR);
      db_products_sell_price->labeltype(FL_NORMAL_LABEL);
      db_products_sell_price->labelfont(0);
      db_products_sell_price->labelsize(16);
      db_products_sell_price->labelcolor(FL_FOREGROUND_COLOR);
      db_products_sell_price->textsize(16);
      db_products_sell_price->align((FL_ALIGN_LEFT));
      db_products_sell_price->when(FL_WHEN_RELEASE);
      add_input_field_to_map("products", "sell_price", db_products_sell_price);
    } // Fl_Float_Input_Fmt* db_products_sell_price
    { db_products_measure_unit_id =new Fl_Choice_Int(370, 160, 110, 25, _tr("Measure Unit"));
      db_products_measure_unit_id->box(FL_FLAT_BOX);
      db_products_measure_unit_id->down_box(FL_BORDER_BOX);
      db_products_measure_unit_id->color(FL_BACKGROUND_COLOR);
      db_products_measure_unit_id->selection_color(FL_SELECTION_COLOR);
      db_products_measure_unit_id->labeltype(FL_NORMAL_LABEL);
      db_products_measure_unit_id->labelfont(0);
      db_products_measure_unit_id->labelsize(16);
      db_products_measure_unit_id->labelcolor(FL_FOREGROUND_COLOR);
      db_products_measure_unit_id->textsize(16);
      db_products_measure_unit_id->align((FL_ALIGN_LEFT));
      db_products_measure_unit_id->when(FL_WHEN_RELEASE);
      add_input_field_to_map("products", "measure_unit_id", db_products_measure_unit_id);
    } // Fl_Choice_Int* db_products_measure_unit_id
    { db_products_sales_tax_id =new Fl_Choice_Int(115, 190, 110, 25, _tr("V.A.T."));
      db_products_sales_tax_id->box(FL_FLAT_BOX);
      db_products_sales_tax_id->down_box(FL_BORDER_BOX);
      db_products_sales_tax_id->color(FL_BACKGROUND_COLOR);
      db_products_sales_tax_id->selection_color(FL_SELECTION_COLOR);
      db_products_sales_tax_id->labeltype(FL_NORMAL_LABEL);
      db_products_sales_tax_id->labelfont(0);
      db_products_sales_tax_id->labelsize(16);
      db_products_sales_tax_id->labelcolor(FL_FOREGROUND_COLOR);
      db_products_sales_tax_id->textsize(16);
      db_products_sales_tax_id->align((FL_ALIGN_LEFT));
      db_products_sales_tax_id->when(FL_WHEN_RELEASE);
      add_input_field_to_map("products", "sales_tax_id", db_products_sales_tax_id);
    } // Fl_Choice_Int* db_products_sales_tax_id
    { db_products_warranty_id =new Fl_Choice_Int(370, 190, 110, 25, _tr("Warranty"));
      db_products_warranty_id->box(FL_FLAT_BOX);
      db_products_warranty_id->down_box(FL_BORDER_BOX);
      db_products_warranty_id->color(FL_BACKGROUND_COLOR);
      db_products_warranty_id->selection_color(FL_SELECTION_COLOR);
      db_products_warranty_id->labeltype(FL_NORMAL_LABEL);
      db_products_warranty_id->labelfont(0);
      db_products_warranty_id->labelsize(16);
      db_products_warranty_id->labelcolor(FL_FOREGROUND_COLOR);
      db_products_warranty_id->textsize(16);
      db_products_warranty_id->align((FL_ALIGN_LEFT));
      db_products_warranty_id->when(FL_WHEN_RELEASE);
      add_input_field_to_map("products", "warranty_id", db_products_warranty_id);
    } // Fl_Choice_Int* db_products_warranty_id
    { local o = db_products_sell_quantity_min =new Fl_Float_Input(370, 250, 110, 25, _tr("Sell Min. Qty."));
      db_products_sell_quantity_min->type(1);
      db_products_sell_quantity_min->labelsize(16);
      db_products_sell_quantity_min->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "sell_quantity_min", db_products_sell_quantity_min);
    } // Fl_Float_Input* db_products_sell_quantity_min
    { local o = db_products_units_by_package =new Fl_Float_Input(370, 280, 110, 25, _tr("Units by Pack."));
      db_products_units_by_package->type(1);
      db_products_units_by_package->labelsize(16);
      db_products_units_by_package->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "units_by_package", db_products_units_by_package);
    } // Fl_Float_Input* db_products_units_by_package
    { local o = db_products_weight =new Fl_Float_Input(370, 220, 110, 25, _tr("Weight"));
      db_products_weight->type(1);
      db_products_weight->labelsize(16);
      db_products_weight->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "weight", db_products_weight);
    } // Fl_Float_Input* db_products_weight
    { db_products_sell_without_stock =new Fl_Check_Button(10, 250, 215, 25, _tr("Sell without stock"));
      db_products_sell_without_stock->down_box(FL_DOWN_BOX);
      db_products_sell_without_stock->labelsize(16);
      add_input_field_to_map("products", "sell_without_stock", db_products_sell_without_stock);
    } // Fl_Check_Button* db_products_sell_without_stock
    { db_products_is_active =new Fl_Check_Button(10, 280, 110, 25, _tr("Active"));
      db_products_is_active->down_box(FL_DOWN_BOX);
      db_products_is_active->labelsize(16);
      add_input_field_to_map("products", "is_active", db_products_is_active);
    } // Fl_Check_Button* db_products_is_active
    { db_products_show_on_sales =new Fl_Check_Button(10, 310, 105, 25, _tr("Sales"));
      db_products_show_on_sales->down_box(FL_DOWN_BOX);
      db_products_show_on_sales->labelsize(16);
      add_input_field_to_map("products", "show_on_sales", db_products_show_on_sales);
    } // Fl_Check_Button* db_products_show_on_sales
    { db_products_show_on_buys =new Fl_Check_Button(10, 340, 95, 25, _tr("Buys"));
      db_products_show_on_buys->down_box(FL_DOWN_BOX);
      db_products_show_on_buys->labelsize(16);
      add_input_field_to_map("products", "show_on_buys", db_products_show_on_buys);
    } // Fl_Check_Button* db_products_show_on_buys
    { db_products_show_on_web =new Fl_Check_Button(115, 310, 120, 25, _tr("Web"));
      db_products_show_on_web->down_box(FL_DOWN_BOX);
      db_products_show_on_web->labelsize(16);
      add_input_field_to_map("products", "show_on_web", db_products_show_on_web);
    } // Fl_Check_Button* db_products_show_on_web
    { db_products_show_price_on_web =new Fl_Check_Button(115, 340, 100, 25, _tr("Web price"));
      db_products_show_price_on_web->down_box(FL_DOWN_BOX);
      db_products_show_price_on_web->labelsize(16);
      add_input_field_to_map("products", "show_price_on_web", db_products_show_price_on_web);
    } // Fl_Check_Button* db_products_show_price_on_web
    { local o =new Fl_Group(675, 402, 110, 143);
      { local o =new Fl_Box(675, 402, 110, 68, _tr("Spacer"));
        o->labeltype(FL_NO_LABEL);
        Fl_Group.current()->resizable(o);
      } // Fl_Box* o
      { dbAction = new Fl_Button(675, 520, 110, 25);
        dbAction->label(_tr("Action"));
        dbAction->down_box(FL_BORDER_BOX);
        dbAction->labelsize(16);
        dbAction->textsize(16);
        dbAction->align((FL_ALIGN_TOP));
      } // Fl_Choice* dbAction
      { btnDbAction = new My_Fl_Return_Button(675, 470, 110, 25, _tr("Action"));
        btnDbAction->labelsize(16);
      } // Fl_Button* btnDbAction
      o->end();
    } // Fl_Group* o
    { local o =new Fl_Tabs(10, 370, 650, 181);
      o->selection_color(4);
      o->labelsize(16);
      o->labelcolor(FL_BACKGROUND2_COLOR);
      { tabNotes =new Fl_Group(10, 400, 650, 150, _tr("Notes"));
        tabNotes->labelsize(16);
        { db_products_sell_notes =new Fl_Text_Editor_Buffered(10, 400, 650, 150);
          db_products_sell_notes->type(4);
          db_products_sell_notes->box(FL_DOWN_BOX);
          db_products_sell_notes->color(FL_BACKGROUND2_COLOR);
          db_products_sell_notes->selection_color(FL_SELECTION_COLOR);
          db_products_sell_notes->labeltype(FL_NO_LABEL);
          db_products_sell_notes->labelfont(0);
          db_products_sell_notes->labelsize(16);
          db_products_sell_notes->labelcolor(FL_FOREGROUND_COLOR);
          db_products_sell_notes->textsize(16);
          db_products_sell_notes->align((FL_ALIGN_TOP_LEFT));
          db_products_sell_notes->when(FL_WHEN_RELEASE);
          Fl_Group.current()->resizable(db_products_sell_notes);
          add_input_field_to_map("products", "sell_notes", db_products_sell_notes);
        } // Fl_Text_Editor_Buffered* db_products_sell_notes
        tabNotes->end();
      } // Fl_Group* tabNotes
      { tabNotesSupplier =new Fl_Group(10, 400, 650, 150, _tr("Notes Supplier"));
        tabNotesSupplier->labelsize(16);
        tabNotesSupplier->hide();
        { db_products_buy_notes =new Fl_Text_Editor_Buffered(10, 400, 650, 150);
          db_products_buy_notes->type(4);
          db_products_buy_notes->box(FL_DOWN_BOX);
          db_products_buy_notes->color(FL_BACKGROUND2_COLOR);
          db_products_buy_notes->selection_color(FL_SELECTION_COLOR);
          db_products_buy_notes->labeltype(FL_NO_LABEL);
          db_products_buy_notes->labelfont(0);
          db_products_buy_notes->labelsize(16);
          db_products_buy_notes->labelcolor(FL_FOREGROUND_COLOR);
          db_products_buy_notes->textsize(16);
          db_products_buy_notes->align((FL_ALIGN_TOP_LEFT));
          db_products_buy_notes->when(FL_WHEN_RELEASE);
          Fl_Group.current()->resizable(db_products_buy_notes);
          add_input_field_to_map("products", "buy_notes", db_products_buy_notes);
        } // Fl_Text_Editor_Buffered* db_products_buy_notes
        tabNotesSupplier->end();
      } // Fl_Group* tabNotesSupplier
      { tabTags =new Fl_Group(10, 400, 650, 150, _tr("Tags"));
        tabTags->labelsize(16);
        tabTags->hide();
        { db_products_tags =new Fl_Text_Editor_Buffered(10, 400, 650, 150);
          db_products_tags->type(4);
          db_products_tags->box(FL_DOWN_BOX);
          db_products_tags->color(FL_BACKGROUND2_COLOR);
          db_products_tags->selection_color(FL_SELECTION_COLOR);
          db_products_tags->labeltype(FL_NO_LABEL);
          db_products_tags->labelfont(0);
          db_products_tags->labelsize(16);
          db_products_tags->labelcolor(FL_FOREGROUND_COLOR);
          db_products_tags->textsize(16);
          db_products_tags->align((FL_ALIGN_TOP_LEFT));
          db_products_tags->when(FL_WHEN_RELEASE);
          Fl_Group.current()->resizable(db_products_tags);
          add_input_field_to_map("products", "tags", db_products_tags);
        } // Fl_Text_Editor_Buffered* db_products_tags
        tabTags->end();
      } // Fl_Group* tabTags
      { tabGroups =new Fl_Group(10, 395, 650, 156, _tr("Groups"));
        tabGroups->hide();
        { db_products_group_id = new Flu_Tree_Browser(10, 400, 650, 150);
          db_products_group_id->box(FL_DOWN_BOX);
          db_products_group_id->color(FL_BACKGROUND2_COLOR);
          db_products_group_id->selection_color(FL_SELECTION_COLOR);
          db_products_group_id->labeltype(FL_NORMAL_LABEL);
          db_products_group_id->labelfont(0);
          db_products_group_id->labelsize(16);
          db_products_group_id->labelcolor(FL_FOREGROUND_COLOR);
          db_products_group_id->align((FL_ALIGN_TOP));
          db_products_group_id->when(FL_WHEN_CHANGED);
          add_input_field_to_map("products", "group_id", db_products_group_id);
        } // Flu_Tree_Browser* db_products_group_id
        tabGroups->end();
      } // Fl_Group* tabGroups
      o->end();
    } // Fl_Tabs* o
    { btnImage =new Fl_Image_Box(485, 158, 300, 232);
      btnImage->box(FL_ENGRAVED_BOX);
      btnImage->color(FL_BACKGROUND2_COLOR);
      btnImage->selection_color(FL_BACKGROUND_COLOR);
      btnImage->labeltype(FL_NORMAL_LABEL);
      btnImage->labelfont(0);
      btnImage->labelsize(14);
      btnImage->labelcolor(FL_FOREGROUND_COLOR);
      btnImage->align((FL_ALIGN_CLIP|FL_ALIGN_INSIDE));
      btnImage->when(FL_WHEN_RELEASE);
    } // Fl_Image_Box* btnImage
    { local o =new Fl_Check_Button(370, 375, 102, 25, _tr("Wrap lines"));
      o->down_box(FL_DOWN_BOX);
      o->value(1);
      o->labelsize(16);
      o->callback(cb_Wrap);
    } // Fl_Check_Button* o
    { local o = db_products_quantity_onhand =new Fl_Float_Input(115, 220, 110, 25, _tr("Onhand"));
      db_products_quantity_onhand->type(1);
      db_products_quantity_onhand->labelsize(16);
      db_products_quantity_onhand->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_onhand", db_products_quantity_onhand);
    } // Fl_Float_Input* db_products_quantity_onhand
    tabMain->end();
    Fl_Group.current()->resizable(tabMain);
  } // Fl_Group* tabMain
  { tabPrices =new Fl_Group(5, 60, 790, 495, _tr("Prices"));
    tabPrices->color(246);
    tabPrices->labelsize(16);
    tabPrices->hide();
    { local o =new Fl_Group(15, 88, 770, 201, _tr("Sales Price"));
      o->box(FL_UP_BOX);
      o->color(166);
      o->labelsize(16);
      { db_products_buy_price =new Fl_Float_Input_Fmt(25, 115, 115, 25, _tr("Buy $"));
        db_products_buy_price->type(1);
        db_products_buy_price->box(FL_DOWN_BOX);
        db_products_buy_price->color(FL_BACKGROUND2_COLOR);
        db_products_buy_price->selection_color(FL_SELECTION_COLOR);
        db_products_buy_price->labeltype(FL_NORMAL_LABEL);
        db_products_buy_price->labelfont(0);
        db_products_buy_price->labelsize(16);
        db_products_buy_price->labelcolor(FL_FOREGROUND_COLOR);
        db_products_buy_price->textsize(16);
        db_products_buy_price->align((FL_ALIGN_TOP_LEFT));
        db_products_buy_price->when(FL_WHEN_RELEASE);
        add_input_field_to_map("products", "buy_price", db_products_buy_price);
      } // Fl_Float_Input_Fmt* db_products_buy_price
      { local o = db_products_buy_discount =new Fl_Float_Input_Fmt(151, 115, 115, 25, _tr("Buy Disc. %"));
        db_products_buy_discount->type(1);
        db_products_buy_discount->box(FL_DOWN_BOX);
        db_products_buy_discount->color(FL_BACKGROUND2_COLOR);
        db_products_buy_discount->selection_color(FL_SELECTION_COLOR);
        db_products_buy_discount->labeltype(FL_NORMAL_LABEL);
        db_products_buy_discount->labelfont(0);
        db_products_buy_discount->labelsize(16);
        db_products_buy_discount->labelcolor(FL_FOREGROUND_COLOR);
        db_products_buy_discount->textsize(16);
        db_products_buy_discount->align((FL_ALIGN_TOP_LEFT));
        db_products_buy_discount->when(FL_WHEN_RELEASE);
        add_input_field_to_map("products", "buy_discount", db_products_buy_discount);
        o->decimal_places(-6);
      } // Fl_Float_Input_Fmt* db_products_buy_discount
      { db_products_buy_other_costs =new Fl_Float_Input_Fmt(278, 115, 115, 25, _tr("Other Costs $"));
        db_products_buy_other_costs->type(1);
        db_products_buy_other_costs->box(FL_DOWN_BOX);
        db_products_buy_other_costs->color(FL_BACKGROUND2_COLOR);
        db_products_buy_other_costs->selection_color(FL_SELECTION_COLOR);
        db_products_buy_other_costs->labeltype(FL_NORMAL_LABEL);
        db_products_buy_other_costs->labelfont(0);
        db_products_buy_other_costs->labelsize(16);
        db_products_buy_other_costs->labelcolor(FL_FOREGROUND_COLOR);
        db_products_buy_other_costs->textsize(16);
        db_products_buy_other_costs->align((FL_ALIGN_TOP_LEFT));
        db_products_buy_other_costs->when(FL_WHEN_RELEASE);
        add_input_field_to_map("products", "buy_other_costs", db_products_buy_other_costs);
      } // Fl_Float_Input_Fmt* db_products_buy_other_costs
      { local o = db_products_sell_markup =new Fl_Float_Input_Fmt(404, 115, 115, 25, _tr("Markup %"));
        db_products_sell_markup->type(1);
        db_products_sell_markup->box(FL_DOWN_BOX);
        db_products_sell_markup->color(FL_BACKGROUND2_COLOR);
        db_products_sell_markup->selection_color(FL_SELECTION_COLOR);
        db_products_sell_markup->labeltype(FL_NORMAL_LABEL);
        db_products_sell_markup->labelfont(0);
        db_products_sell_markup->labelsize(16);
        db_products_sell_markup->labelcolor(FL_FOREGROUND_COLOR);
        db_products_sell_markup->textsize(16);
        db_products_sell_markup->align((FL_ALIGN_TOP_LEFT));
        db_products_sell_markup->when(FL_WHEN_RELEASE);
        add_input_field_to_map("products", "sell_markup", db_products_sell_markup);
        //o->decimal_places(-6);
      } // Fl_Float_Input_Fmt* db_products_sell_markup
      { local o = markup_to_discount =new Fl_Float_Input_Fmt(531, 115, 115, 25, _tr("Margin %"));
        markup_to_discount->type(1);
        markup_to_discount->box(FL_DOWN_BOX);
        markup_to_discount->color(FL_BACKGROUND2_COLOR);
        markup_to_discount->selection_color(FL_SELECTION_COLOR);
        markup_to_discount->labeltype(FL_NORMAL_LABEL);
        markup_to_discount->labelfont(0);
        markup_to_discount->labelsize(16);
        markup_to_discount->labelcolor(FL_FOREGROUND_COLOR);
        markup_to_discount->textsize(16);
        markup_to_discount->align((FL_ALIGN_TOP_LEFT));
        markup_to_discount->when(FL_WHEN_RELEASE);
        //o->decimal_places(-6);
      } // Fl_Float_Input_Fmt* markup_to_discount
      { db_products_sell_price_ro =new Fl_Float_Input_Fmt(658, 115, 115, 25, _tr("Sales $"));
        db_products_sell_price_ro->type(1);
        db_products_sell_price_ro->box(FL_DOWN_BOX);
        db_products_sell_price_ro->color(FL_BACKGROUND2_COLOR);
        db_products_sell_price_ro->selection_color(FL_SELECTION_COLOR);
        db_products_sell_price_ro->labeltype(FL_NORMAL_LABEL);
        db_products_sell_price_ro->labelfont(0);
        db_products_sell_price_ro->labelsize(16);
        db_products_sell_price_ro->labelcolor(FL_FOREGROUND_COLOR);
        db_products_sell_price_ro->textsize(16);
        db_products_sell_price_ro->align((FL_ALIGN_TOP_LEFT));
        db_products_sell_price_ro->when(FL_WHEN_RELEASE);
      } // Fl_Float_Input_Fmt* db_products_sell_price_ro
      { local o = db_products_price_decimals =new Fl_Int_Input(530, 150, 30, 25, _tr("Decimals for calculation"));
        db_products_price_decimals->type(2);
        db_products_price_decimals->labelfont(1);
        db_products_price_decimals->labelsize(16);
        db_products_price_decimals->textfont(1);
        db_products_price_decimals->textsize(16);
        add_input_field_to_map("products", "price_decimals", db_products_price_decimals);
        //o->decimal_places(-6);
      } // Fl_Int_Input* db_products_price_decimals
      { btnSaveProrduct =new Fl_Button(662, 150, 111, 25, _tr("Save"));
        btnSaveProrduct->labelsize(16);
      } // Fl_Button* btnSaveProrduct
      { local o = db_products_stock_min =new Fl_Float_Input(665, 195, 110, 25, _tr("Stock Min."));
        db_products_stock_min->type(1);
        db_products_stock_min->labelsize(16);
        db_products_stock_min->textsize(16);
        add_input_field_to_map("products", "stock_min", db_products_stock_min);
        o->decimal_places(-6);
      } // Fl_Float_Input* db_products_stock_min
      { local o = db_products_stock_max =new Fl_Float_Input(665, 225, 110, 25, _tr("Stock Max."));
        db_products_stock_max->type(1);
        db_products_stock_max->labelsize(16);
        db_products_stock_max->textsize(16);
        add_input_field_to_map("products", "stock_max", db_products_stock_max);
        o->decimal_places(-6);
      } // Fl_Float_Input* db_products_stock_max
      { local o = db_products_buy_quantity_min =new Fl_Float_Input(665, 255, 110, 25, _tr("Buy Min. Qty."));
        db_products_buy_quantity_min->type(1);
        db_products_buy_quantity_min->labelsize(16);
        db_products_buy_quantity_min->textsize(16);
        add_input_field_to_map("products", "buy_quantity_min", db_products_buy_quantity_min);
        o->decimal_places(-6);
      } // Fl_Float_Input* db_products_buy_quantity_min
      { db_products_price_formula =new Fl_Input(25, 197, 495, 83, _tr("Price formula"));
        db_products_price_formula->type(4);
        db_products_price_formula->labelsize(16);
        db_products_price_formula->textsize(16);
        db_products_price_formula->align((FL_ALIGN_TOP_LEFT));
        add_input_field_to_map("products", "price_formula", db_products_price_formula);
      } // Fl_Input* db_products_price_formula
      { db_products_price_date =new Fl_Output(25, 150, 130, 25, _tr("Last Update"));
        db_products_price_date->labelsize(16);
        db_products_price_date->textsize(16);
        db_products_price_date->align((FL_ALIGN_RIGHT));
      } // Fl_Output* db_products_price_date
      o->end();
    } // Fl_Group* o
    { local o =new Fl_Box(15, 290, 770, 40, _tr("Spacer"));
      o->labeltype(FL_NO_LABEL);
      Fl_Group.current()->resizable(o);
    } // Fl_Box* o
    { productPrices =new Fl_Group(15, 332, 770, 212);
      productPrices->box(FL_UP_BOX);
      productPrices->color(166);
      productPrices->labeltype(FL_NO_LABEL);
      productPrices->labelsize(16);
      productPrices->end();
    } // Fl_Group* productPrices
    tabPrices->end();
  } // Fl_Group* tabPrices
  { tabKit =new Fl_Group(5, 59, 790, 495, _tr("Kit"));
    tabKit->color(246);
    tabKit->labelsize(16);
    tabKit->hide();
    tabKit->end();
  } // Fl_Group* tabKit
  { tabChartStatistics =new Fl_Group(5, 60, 790, 495, _tr("Statistics"));
    tabChartStatistics->color(246);
    tabChartStatistics->labelsize(16);
    tabChartStatistics->hide();
    tabChartStatistics->end();
  } // Fl_Group* tabChartStatistics
  { tabHistory =new Fl_Group(5, 60, 790, 495, _tr("History"));
    tabHistory->color(246);
    tabHistory->labelsize(16);
    tabHistory->hide();
    tabHistory->end();
  } // Fl_Group* tabHistory
  { tabReports =new Fl_Group(5, 60, 790, 495, _tr("Reports"));
    tabReports->box(FL_UP_BOX);
    tabReports->color(246);
    tabReports->labelsize(16);
    tabReports->hide();
    { grpExportImport =new Fl_Group(15, 465, 770, 80, _tr("Export / Import"));
      grpExportImport->box(FL_ENGRAVED_FRAME);
      grpExportImport->labelsize(16);
      { btnProductsExport =new Fl_Button(25, 475, 175, 25, _tr("Products Export"));
        btnProductsExport->labelsize(16);
      } // Fl_Button* btnProductsExport
      { btnProductsImport =new Fl_Button(25, 510, 175, 25, _tr("Products Import"));
        btnProductsImport->labelsize(16);
      } // Fl_Button* btnProductsImport
      { chkWithoutPrices =new Fl_Check_Button(225, 480, 150, 25, _tr("Withouth Prices"));
        chkWithoutPrices->down_box(FL_DOWN_BOX);
        chkWithoutPrices->labelsize(16);
      } // Fl_Check_Button* chkWithoutPrices
      { chkWebOnly =new Fl_Check_Button(225, 510, 150, 25, _tr("WEB Only"));
        chkWebOnly->down_box(FL_DOWN_BOX);
        chkWebOnly->labelsize(16);
      } // Fl_Check_Button* chkWebOnly
      { boxExportImportCount =new Fl_Box(405, 499, 42, 25, _tr("0"));
        boxExportImportCount->labelsize(16);
      } // Fl_Box* boxExportImportCount
      { btnProductGroupsExport =new Fl_Button(555, 475, 220, 25, _tr("Product Groups Export"));
        btnProductGroupsExport->labelsize(16);
      } // Fl_Button* btnProductGroupsExport
      { btnProductGroupsImport =new Fl_Button(555, 510, 220, 25, _tr("Product Groups import"));
        btnProductGroupsImport->labelsize(16);
      } // Fl_Button* btnProductGroupsImport
      grpExportImport->end();
    } // Fl_Group* grpExportImport
    { btnUpdateWeb =new Fl_Button(610, 70, 175, 25, _tr("Update WEB"));
      btnUpdateWeb->labelsize(16);
    } // Fl_Button* btnUpdateWeb
    { grpPrintPriceList =new Fl_Group(10, 85, 195, 165, _tr("Price List"));
      grpPrintPriceList->box(FL_ENGRAVED_FRAME);
      grpPrintPriceList->labelsize(16);
      { chkPriceListWithVAT =new Fl_Check_Button(20, 95, 175, 25, _tr("With V.A.T."));
        chkPriceListWithVAT->down_box(FL_DOWN_BOX);
        chkPriceListWithVAT->labelsize(16);
      } // Fl_Check_Button* chkPriceListWithVAT
      { chkPriceListSellOnWeb =new Fl_Check_Button(20, 125, 170, 25, _tr("Sell on WEB"));
        chkPriceListSellOnWeb->down_box(FL_DOWN_BOX);
        chkPriceListSellOnWeb->labelsize(16);
      } // Fl_Check_Button* chkPriceListSellOnWeb
      { btnPrintPriceList =new Fl_Button(20, 215, 175, 25, _tr("Print"));
        btnPrintPriceList->labelsize(16);
      } // Fl_Button* btnPrintPriceList
      { chkPriceListSellSales =new Fl_Check_Button(20, 155, 175, 25, _tr("Sales"));
        chkPriceListSellSales->down_box(FL_DOWN_BOX);
        chkPriceListSellSales->labelsize(16);
      } // Fl_Check_Button* chkPriceListSellSales
      { chkPriceListSellPDF =new Fl_Check_Button(20, 185, 175, 25, _tr("PDF"));
        chkPriceListSellPDF->down_box(FL_DOWN_BOX);
        chkPriceListSellPDF->labelsize(16);
      } // Fl_Check_Button* chkPriceListSellPDF
      grpPrintPriceList->end();
    } // Fl_Group* grpPrintPriceList
    tabReports->end();
  } // Fl_Group* tabReports
  { tabTotals =new Fl_Group(5, 60, 790, 495, _tr("Totals"));
    tabTotals->color(246);
    tabTotals->labelsize(16);
    tabTotals->hide();
    { local o = db_products_quantity_quoted_sales =new Fl_Float_Input(220, 80, 110, 25, _tr("Quantity quoted sales"));
      db_products_quantity_quoted_sales->type(1);
      db_products_quantity_quoted_sales->labelsize(16);
      db_products_quantity_quoted_sales->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_quoted_sales", db_products_quantity_quoted_sales);
    } // Fl_Float_Input* db_products_quantity_quoted_sales
    { local o = db_products_quantity_ordered_sales =new Fl_Float_Input(220, 115, 110, 25, _tr("Quantity ordered sales"));
      db_products_quantity_ordered_sales->type(1);
      db_products_quantity_ordered_sales->labelsize(16);
      db_products_quantity_ordered_sales->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_ordered_sales", db_products_quantity_ordered_sales);
    } // Fl_Float_Input* db_products_quantity_ordered_sales
    { local o = db_products_quantity_delivered =new Fl_Float_Input(220, 150, 110, 25, _tr("Quantity delivered"));
      db_products_quantity_delivered->type(1);
      db_products_quantity_delivered->labelsize(16);
      db_products_quantity_delivered->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_delivered", db_products_quantity_delivered);
    } // Fl_Float_Input* db_products_quantity_delivered
    { local o = db_products_quantity_invoiced_sales =new Fl_Float_Input(220, 185, 110, 25, _tr("Quantity invoiced sales"));
      db_products_quantity_invoiced_sales->type(1);
      db_products_quantity_invoiced_sales->labelsize(16);
      db_products_quantity_invoiced_sales->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_invoiced_sales", db_products_quantity_invoiced_sales);
    } // Fl_Float_Input* db_products_quantity_invoiced_sales
    { local o = db_products_quantity_lost =new Fl_Float_Input(220, 220, 110, 25, _tr("Quantity lost"));
      db_products_quantity_lost->type(1);
      db_products_quantity_lost->labelsize(16);
      db_products_quantity_lost->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_lost", db_products_quantity_lost);
    } // Fl_Float_Input* db_products_quantity_lost
    { local o = db_products_quantity_quoted_buys =new Fl_Float_Input(665, 85, 110, 25, _tr("Quantity quoted buys"));
      db_products_quantity_quoted_buys->type(1);
      db_products_quantity_quoted_buys->labelsize(16);
      db_products_quantity_quoted_buys->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_quoted_buys", db_products_quantity_quoted_buys);
    } // Fl_Float_Input* db_products_quantity_quoted_buys
    { local o = db_products_quantity_ordered_buys =new Fl_Float_Input(665, 120, 110, 25, _tr("Quantity ordered buys"));
      db_products_quantity_ordered_buys->type(1);
      db_products_quantity_ordered_buys->labelsize(16);
      db_products_quantity_ordered_buys->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_ordered_buys", db_products_quantity_ordered_buys);
    } // Fl_Float_Input* db_products_quantity_ordered_buys
    { local o = db_products_quantity_received =new Fl_Float_Input(665, 155, 110, 25, _tr("Quantity received"));
      db_products_quantity_received->type(1);
      db_products_quantity_received->labelsize(16);
      db_products_quantity_received->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_received", db_products_quantity_received);
    } // Fl_Float_Input* db_products_quantity_received
    { local o = db_products_quantity_invoiced_buys =new Fl_Float_Input(665, 190, 110, 25, _tr("Quantity invoiced buys"));
      db_products_quantity_invoiced_buys->type(1);
      db_products_quantity_invoiced_buys->labelsize(16);
      db_products_quantity_invoiced_buys->textsize(16);
      o->decimal_places(-6);
      add_input_field_to_map("products", "quantity_invoiced_buys", db_products_quantity_invoiced_buys);
    } // Fl_Float_Input* db_products_quantity_invoiced_buys
    tabTotals->end();
  } // Fl_Group* tabTotals
  tabs->end();
  Fl_Group.current()->resizable(tabs);
} // Fl_Tabs* tabs
end();
}

function cb_Wrap(){}
function cb_btnSearch(){}
function cb_btnSelect(){}
function cb_btnUpdate(){}
function cb_btnInsert(){}
}
