class Fl_Box_ClearLabel extends Fl_Box {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

class Flu_Combo_List extends Fl_Box {
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



function add_input_field_to_map(tbl, fldname, fld){
}
	
//class EditOrderWindow extends Fl_Group {
class EditOrderWindow extends MyBaseWindow {
	delivery_calc_window = 0;
	entities_list_window = 0;
	products_list_window = 0;
	calendar_window = 0;
	db_orders_mdate=null;
	pack_line2=null;
	db_orders_series=null;
	db_orders_order_number=null;
	db_orders_entity_id=null;
	db_orders_entity_name=null;
	btnSearchEntity=null;
	db_orders_entity_order_number=null;
	tabs=null;
	tabMain=null;
	pack_line3=null;
	out_lines=null;
	db_orders_weight_total=null;
	btnCalcDelivery=null;
	db_orders_payment_type_id=null;
	btnSortLines=null;
	db_orders_order_date=null;
	btnShowCalendar=null;
	group_totals=null;
	db_orders_discount_amt=null;
	db_orders_subtotal_amt=null;
	db_orders_sales_tax1_amt=null;
	db_orders_sales_tax2_amt=null;
	db_orders_irpf_amt=null;
	db_orders_total_amt=null;
	db_orders_cash=null;
	cash_return=null;
	linesTab=null;
	group_lines=null;
	db_orders_lines_batch_order_line_quantity=null;
	db_orders_lines_batch_order_line_id=null;
	btnSearchBatchOrderLineId=null;
	db_orders_lines_xref_order_line_quantity=null;
	db_orders_lines_xref_order_line_id=null;
	btnSearchXrefOrderLineId=null;
	db_orders_lines_product_id=null;
	db_orders_lines_description=null;
	btnSearchProduct=null;
	db_orders_lines_price_decimals=null;
	db_orders_lines_first_total=null;
	db_orders_lines_discount_amt=null;
	db_orders_lines_line_subtotal=null;
	db_orders_lines_sales_tax2_pct=null;
	db_orders_lines_sales_tax1_pct=null;
	db_orders_lines_sales_tax1_amt=null;
	db_orders_lines_line_total=null;
	tabLineNotes=null;
	db_orders_lines_notes=null;
	tabDues=null;
	grid_dues=null;
	due_id=null;
	due_date=null;
	btnShowCalendarDueDate=null;
	due_pct=null;
	due_amount=null;
	tabPayments=null;
	grid_payments=null;
	payment_id=null;
	payment_date=null;
	btnShowCalendarPaymentDate=null;
	payemnt_amount=null;
	tabDelivery=null;
	db_orders_entity_address=null;
	db_orders_entity_zip=null;
	db_orders_entity_city=null;
	db_orders_entity_state=null;
	db_orders_entity_country=null;
	db_orders_entity_phone=null;
	db_orders_entity_tax_number=null;
	db_orders_order_valid_till_date=null;
	btnShowCalendarValidTill=null;
	db_orders_entity_use_sales_tax2=null;
	db_orders_entity_sales_tax_exempt=null;
	db_orders_notes=null;
	db_orders_irpf_pct_retention=null;
	tabOptions=null;
	opt_ask_for_printer=null;
	opt_print_only_data=null;
	opt_print_to_pdf=null;
	opt_print_page_format=null;
	btnOptPrint=null;
	opt_email_program=null;
	opt_email_program_command=null;
	btnOptSendEmail=null;
	opt_print_order_company_info=null;
	opt_print_order_bottom_info=null;
	tabChartStatistics=null;
	tabHistory=null;
	tabPrintPreview=null;
	tabTotals=null;
	totals_balance=null;
	db_totals_overview_sales_quoted=null;
	db_totals_overview_sales_ordered=null;
	db_totals_overview_sales_delivered=null;
	db_totals_overview_sales_invoiced=null;
	db_totals_overview_sales_discount=null;
	db_totals_overview_sales_tax1=null;
	db_totals_overview_sales_tax2=null;
	db_totals_overview_sales_paid=null;
	db_totals_overview_buys_quoted=null;
	db_totals_overview_buys_ordered=null;
	db_totals_overview_buys_received=null;
	db_totals_overview_buys_invoiced=null;
	db_totals_overview_buys_discount=null;
	db_totals_overview_buys_tax1=null;
	db_totals_overview_buys_tax2=null;
	db_totals_overview_buys_paid=null;
	db_orders_order_type_id=null;
	grid_lines=null;
	btnPrint=null;
	db_orders_lines_quantity=null;
	db_orders_lines_price=null;
	db_orders_lines_weight=null;
	db_orders_lines_discount_pct=null;
	btnSaveLine=null;
	btnDueSave=null;
	btnPaymentSave=null;
	btnNumberOrder=null;
	dbAction=null;
	btnDbAction=null;
	btnRefreshTotals=null;
	
constructor() {
	base.constructor(10, 50, 800, 560, _tr("Edit Order"));
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
delivery_calc_window = 0;
entities_list_window = 0;
products_list_window = 0;
calendar_window = 0;
{ local o = new Fl_Group(5, 1, 792, 62);
  { db_orders_mdate = new Fl_Output(670, 5, 125, 25);
    db_orders_mdate->labeltype(FL_NO_LABEL);
    db_orders_mdate->labelsize(16);
    db_orders_mdate->textsize(16);
    add_input_field_to_map("orders", "mdate", db_orders_mdate);
  } // Fl_Output* db_orders_mdate
  { db_orders_order_type_id = new Flu_Combo_List(5, 5, 655, 25);
    db_orders_order_type_id->tooltip(_tr("Selection of the order type"));
    db_orders_order_type_id->box(FL_DOWN_BOX);
    db_orders_order_type_id->color(FL_BACKGROUND_COLOR);
    db_orders_order_type_id->selection_color(FL_BACKGROUND_COLOR);
    db_orders_order_type_id->labeltype(FL_NO_LABEL);
    db_orders_order_type_id->labelfont(1);
    db_orders_order_type_id->labelsize(16);
    db_orders_order_type_id->labelcolor(FL_FOREGROUND_COLOR);
    db_orders_order_type_id->align((FL_ALIGN_CENTER));
    db_orders_order_type_id->when(FL_WHEN_RELEASE);
    add_input_field_to_map("orders", "order_type_id", db_orders_order_type_id);
  } // Flu_Combo_List* db_orders_order_type_id
  { local o = pack_line2 = new Fl_Pack(5, 35, 790, 25);
    pack_line2->type(1);
    pack_line2->labelsize(16);
    { db_orders_series = new Fl_Input(55, 35, 45, 25, _tr("Series"));
      db_orders_series->labelsize(16);
      db_orders_series->textsize(16);
      add_input_field_to_map("orders", "series", db_orders_series);
    } // Fl_Input* db_orders_series
    { local o = db_orders_order_number = new Fl_Int_Input(172, 35, 95, 25, _tr("Number"));
      db_orders_order_number->type(2);
      db_orders_order_number->labelsize(16);
      db_orders_order_number->textsize(16);
      add_input_field_to_map("orders", "order_number", db_orders_order_number);
      o->use_numeric_format(0);
    } // Fl_Int_Input* db_orders_order_number
    { db_orders_entity_id = new Fl_Int_Input(319, 35, 95, 25, _tr("Entity"));
      db_orders_entity_id->type(2);
      db_orders_entity_id->labelsize(16);
      db_orders_entity_id->textsize(16);
      add_input_field_to_map("orders", "entity_id", db_orders_entity_id);
    } // Fl_Int_Input* db_orders_entity_id
    { db_orders_entity_name = new Fl_Input(420, 35, 335, 25);
      db_orders_entity_name->tooltip(_tr("Type partal description to search on availlable entities"));
      db_orders_entity_name->labeltype(FL_NO_LABEL);
      db_orders_entity_name->labelsize(16);
      db_orders_entity_name->textfont(1);
      db_orders_entity_name->textsize(16);
      Fl_Group.current()->resizable(db_orders_entity_name);
      add_input_field_to_map("orders", "entity_name", db_orders_entity_name);
    } // Fl_Input* db_orders_entity_name
    { btnSearchEntity = new Fl_Button(765, 35, 30, 25, _tr("@<->"));
      btnSearchEntity->tooltip(_tr("Show entities search window"));
      btnSearchEntity->labelsize(18);
      btnSearchEntity->labelcolor(22);
    } // Fl_Button* btnSearchEntity
    o->spacing(5);
    o->with_label(1);
    pack_line2->end();
  } // Fl_Pack* pack_line2
  o->end();
} // Fl_Group* o
{ db_orders_entity_order_number = new Fl_Input(585, 65, 210, 25, _tr("Their Nº"));
  db_orders_entity_order_number->labelsize(16);
  db_orders_entity_order_number->textsize(16);
  add_input_field_to_map("orders", "entity_order_number", db_orders_entity_order_number);
} // Fl_Input* db_orders_entity_order_number
{ tabs = new Fl_Tabs(5, 68, 790, 487);
  tabs->selection_color(4);
  tabs->labelsize(16);
  tabs->labelcolor(FL_BACKGROUND2_COLOR);
  { tabMain = new Fl_Group(5, 95, 790, 460, _tr("Main"));
    tabMain->color(246);
    tabMain->labelfont(2);
    tabMain->labelsize(16);
    { local o = pack_line3 = new Fl_Pack(11, 105, 780, 25);
      pack_line3->type(1);
      pack_line3->labelsize(16);
      { local o = out_lines = new Fl_Output(57, 105, 69, 25, _tr("Lines"));
        out_lines->labelsize(16);
        out_lines->textsize(16);
        o->right_to_left(1);
      } // Fl_Output* out_lines
      { local o = db_orders_weight_total = new Fl_Output(189, 105, 110, 25, _tr("Weight"));
        db_orders_weight_total->labelsize(16);
        db_orders_weight_total->textsize(16);
        o->right_to_left(1);
        o->decimal_places(-2);
        o->use_numeric_format(1);
      } // Fl_Output* db_orders_weight_total
      { btnCalcDelivery = new Fl_Button(309, 105, 30, 25, _tr("@<->"));
        btnCalcDelivery->tooltip(_tr("Show delivery calc window"));
        btnCalcDelivery->labelsize(18);
        btnCalcDelivery->labelcolor(22);
      } // Fl_Button* btnCalcDelivery
      { db_orders_payment_type_id = new Fl_Choice_Int(345, 105, 235, 25);
        db_orders_payment_type_id->box(FL_FLAT_BOX);
        db_orders_payment_type_id->down_box(FL_BORDER_BOX);
        db_orders_payment_type_id->color(FL_BACKGROUND_COLOR);
        db_orders_payment_type_id->selection_color(FL_SELECTION_COLOR);
        db_orders_payment_type_id->labeltype(FL_NO_LABEL);
        db_orders_payment_type_id->labelfont(0);
        db_orders_payment_type_id->labelsize(16);
        db_orders_payment_type_id->labelcolor(FL_FOREGROUND_COLOR);
        db_orders_payment_type_id->textfont(1);
        db_orders_payment_type_id->textsize(16);
        db_orders_payment_type_id->align((FL_ALIGN_LEFT));
        db_orders_payment_type_id->when(FL_WHEN_RELEASE);
        Fl_Group.current()->resizable(db_orders_payment_type_id);
        add_input_field_to_map("orders", "payment_type_id", db_orders_payment_type_id);
      } // Fl_Choice_Int* db_orders_payment_type_id
      { btnSortLines = new Fl_Button(590, 105, 30, 25, _tr("@2"));
        btnSortLines->tooltip(_tr("Sort lines by description"));
        btnSortLines->labelsize(18);
        btnSortLines->labelcolor(22);
      } // Fl_Button* btnSortLines
      { db_orders_order_date = new Fl_Input(629, 105, 120, 25);
        db_orders_order_date->labeltype(FL_NO_LABEL);
        db_orders_order_date->labelsize(16);
        db_orders_order_date->textfont(1);
        db_orders_order_date->textsize(16);
        add_input_field_to_map("orders", "order_date", db_orders_order_date);
      } // Fl_Input* db_orders_order_date
      { btnShowCalendar = new Fl_Button(760, 105, 30, 25, _tr("@<->"));
        btnShowCalendar->tooltip(_tr("Show calendar"));
        btnShowCalendar->labelsize(18);
        btnShowCalendar->labelcolor(22);
      } // Fl_Button* btnShowCalendar
      o->spacing(5);
      o->with_label(1);
      pack_line3->end();
    } // Fl_Pack* pack_line3
    { grid_lines = new Fl_Data_Table(10, 135, 780, 150);
      grid_lines->resize(10, 135, 780, 150);
      grid_lines->box(FL_NO_BOX);
      grid_lines->color(FL_BACKGROUND2_COLOR);
      grid_lines->selection_color(FL_SELECTION_COLOR);
      grid_lines->labeltype(FL_NORMAL_LABEL);
      grid_lines->labelfont(0);
      grid_lines->labelsize(16);
      grid_lines->labelcolor(FL_FOREGROUND_COLOR);
      grid_lines->textsize(16);
      grid_lines->align((FL_ALIGN_BOTTOM));
      grid_lines->when(FL_WHEN_RELEASE_ALWAYS);
      Fl_Group.current()->resizable(grid_lines);
    } // Fl_Data_Table* grid_lines
    { local o = new Fl_Group(10, 290, 781, 260);
      { group_totals = new Fl_Group(600, 291, 190, 259);
        group_totals->box(FL_UP_BOX);
        group_totals->color(174);
        { local o = db_orders_discount_amt = new Fl_Output(685, 297, 100, 25, _tr("Discount"));
          db_orders_discount_amt->labelsize(16);
          db_orders_discount_amt->textsize(16);
          o->right_to_left(1);
          o->use_numeric_format(1);
          add_input_field_to_map("orders", "discount_amt", db_orders_discount_amt);
        } // Fl_Output* db_orders_discount_amt
        { local o = db_orders_subtotal_amt = new Fl_Output(684, 326, 100, 25, _tr("Subtotal"));
          db_orders_subtotal_amt->labelsize(16);
          db_orders_subtotal_amt->textsize(16);
          o->right_to_left(1);
          o->use_numeric_format(1);
          add_input_field_to_map("orders", "subtotal_amt", db_orders_subtotal_amt);
        } // Fl_Output* db_orders_subtotal_amt
        { local o = db_orders_sales_tax1_amt = new Fl_Output(685, 356, 100, 25, _tr("V.A.T."));
          db_orders_sales_tax1_amt->labelsize(16);
          db_orders_sales_tax1_amt->textsize(16);
          o->right_to_left(1);
          o->use_numeric_format(1);
          add_input_field_to_map("orders", "sales_tax1_amt", db_orders_sales_tax1_amt);
        } // Fl_Output* db_orders_sales_tax1_amt
        { local o = db_orders_sales_tax2_amt = new Fl_Output(684, 386, 100, 25, _tr("R.E."));
          db_orders_sales_tax2_amt->labelsize(16);
          db_orders_sales_tax2_amt->textsize(16);
          o->right_to_left(1);
          o->use_numeric_format(1);
          add_input_field_to_map("orders", "sales_tax2_amt", db_orders_sales_tax2_amt);
        } // Fl_Output* db_orders_sales_tax2_amt
        { local o = db_orders_irpf_amt = new Fl_Output(685, 416, 100, 25, _tr("I.R.P.F."));
          db_orders_irpf_amt->labelsize(16);
          db_orders_irpf_amt->textsize(16);
          o->right_to_left(1);
          o->use_numeric_format(1);
          add_input_field_to_map("orders", "irpf_amt", db_orders_irpf_amt);
        } // Fl_Output* db_orders_irpf_amt
        { local o = db_orders_total_amt = new Fl_Output(610, 453, 174, 33, _tr("Total"));
          db_orders_total_amt->color(119);
          db_orders_total_amt->labelfont(1);
          db_orders_total_amt->textfont(1);
          db_orders_total_amt->textsize(30);
          db_orders_total_amt->align((FL_ALIGN_TOP_LEFT));
          o->right_to_left(1);
          o->use_numeric_format(1);
          add_input_field_to_map("orders", "total_amt", db_orders_total_amt);
        } // Fl_Output* db_orders_total_amt
        { db_orders_cash = new Fl_Float_Input_Fmt(685, 490, 100, 25, _tr("Cash"));
          db_orders_cash->type(1);
          db_orders_cash->box(FL_DOWN_BOX);
          db_orders_cash->color(FL_BACKGROUND2_COLOR);
          db_orders_cash->selection_color(FL_SELECTION_COLOR);
          db_orders_cash->labeltype(FL_NORMAL_LABEL);
          db_orders_cash->labelfont(1);
          db_orders_cash->labelsize(16);
          db_orders_cash->labelcolor(FL_FOREGROUND_COLOR);
          db_orders_cash->textfont(1);
          db_orders_cash->textsize(16);
          db_orders_cash->align((FL_ALIGN_LEFT));
          db_orders_cash->when(FL_WHEN_RELEASE);
          add_input_field_to_map("orders", "cash", db_orders_cash);
        } // Fl_Float_Input_Fmt* db_orders_cash
        { local o = cash_return = new Fl_Output(685, 520, 100, 25, _tr("Return"));
          cash_return->labelfont(1);
          cash_return->labelsize(16);
          cash_return->textfont(1);
          cash_return->textsize(16);
          o->right_to_left(1);
          o->use_numeric_format(1);
        } // Fl_Output* cash_return
        group_totals->end();
      } // Fl_Group* group_totals
      { btnPrint = new My_Fl_Return_Button(485, 525, 100, 25, _tr("Print"));
        btnPrint->box(FL_UP_BOX);
        btnPrint->color(FL_BACKGROUND_COLOR);
        btnPrint->selection_color(FL_BACKGROUND_COLOR);
        btnPrint->labeltype(FL_NORMAL_LABEL);
        btnPrint->labelfont(0);
        btnPrint->labelsize(16);
        btnPrint->labelcolor(FL_FOREGROUND_COLOR);
        btnPrint->align((FL_ALIGN_CENTER));
        btnPrint->when(FL_WHEN_RELEASE);
      } // My_Fl_Return_Button* btnPrint
      { linesTab = new Fl_Tabs(10, 290, 585, 230);
        linesTab->selection_color(4);
        linesTab->labelfont(2);
        linesTab->labelsize(16);
        linesTab->labelcolor(FL_BACKGROUND2_COLOR);
        { group_lines = new Fl_Group(10, 321, 585, 199, _tr("Line Edit"));
          group_lines->box(FL_UP_BOX);
          group_lines->color(166);
          group_lines->labelfont(2);
          group_lines->labelsize(16);
          { local o = db_orders_lines_batch_order_line_quantity = new Fl_Output(265, 330, 110, 25, _tr("Batch Qty."));
            db_orders_lines_batch_order_line_quantity->labelsize(16);
            db_orders_lines_batch_order_line_quantity->textsize(16);
            o->right_to_left(1);
            add_input_field_to_map("orders_lines", "batch_order_line_quantity", db_orders_lines_batch_order_line_quantity);
          } // Fl_Output* db_orders_lines_batch_order_line_quantity
          { db_orders_lines_batch_order_line_id = new Fl_Int_Input(435, 329, 110, 25, _tr("Batch #"));
            db_orders_lines_batch_order_line_id->type(2);
            db_orders_lines_batch_order_line_id->labelsize(16);
            db_orders_lines_batch_order_line_id->textsize(16);
            add_input_field_to_map("orders_lines", "batch_order_line_id", db_orders_lines_batch_order_line_id);
          } // Fl_Int_Input* db_orders_lines_batch_order_line_id
          { btnSearchBatchOrderLineId = new Fl_Button(555, 329, 30, 25, _tr("@<->"));
            btnSearchBatchOrderLineId->tooltip(_tr("Show products search window"));
            btnSearchBatchOrderLineId->labelsize(18);
            btnSearchBatchOrderLineId->labelcolor(22);
          } // Fl_Button* btnSearchBatchOrderLineId
          { local o = db_orders_lines_xref_order_line_quantity = new Fl_Output(265, 360, 110, 25, _tr("XRef. Qty."));
            db_orders_lines_xref_order_line_quantity->labelsize(16);
            db_orders_lines_xref_order_line_quantity->textsize(16);
            o->right_to_left(1);
            add_input_field_to_map("orders_lines", "xref_order_line_quantity", db_orders_lines_xref_order_line_quantity);
          } // Fl_Output* db_orders_lines_xref_order_line_quantity
          { db_orders_lines_xref_order_line_id = new Fl_Int_Input(435, 359, 110, 25, _tr("XRef #"));
            db_orders_lines_xref_order_line_id->type(2);
            db_orders_lines_xref_order_line_id->labelsize(16);
            db_orders_lines_xref_order_line_id->textsize(16);
            add_input_field_to_map("orders_lines", "xref_order_line_id", db_orders_lines_xref_order_line_id);
          } // Fl_Int_Input* db_orders_lines_xref_order_line_id
          { btnSearchXrefOrderLineId = new Fl_Button(555, 359, 30, 25, _tr("@<->"));
            btnSearchXrefOrderLineId->tooltip(_tr("Show products search window"));
            btnSearchXrefOrderLineId->labelsize(18);
            btnSearchXrefOrderLineId->labelcolor(22);
          } // Fl_Button* btnSearchXrefOrderLineId
          { db_orders_lines_product_id = new Fl_Int_Input(21, 387, 70, 25, _tr("Code"));
            db_orders_lines_product_id->type(2);
            db_orders_lines_product_id->labelsize(16);
            db_orders_lines_product_id->textsize(16);
            db_orders_lines_product_id->align((FL_ALIGN_TOP_LEFT));
            add_input_field_to_map("orders_lines", "product_id", db_orders_lines_product_id);
          } // Fl_Int_Input* db_orders_lines_product_id
          { db_orders_lines_description = new Fl_Input(101, 387, 444, 25, _tr("Description"));
            db_orders_lines_description->tooltip(_tr("Type partal description to search on availlable products"));
            db_orders_lines_description->labelsize(16);
            db_orders_lines_description->textfont(1);
            db_orders_lines_description->textsize(16);
            db_orders_lines_description->align((FL_ALIGN_TOP_LEFT));
            add_input_field_to_map("orders_lines", "description", db_orders_lines_description);
          } // Fl_Input* db_orders_lines_description
          { btnSearchProduct = new Fl_Button(555, 387, 30, 25, _tr("@<->"));
            btnSearchProduct->tooltip(_tr("Show products search window"));
            btnSearchProduct->labelsize(18);
            btnSearchProduct->labelcolor(22);
          } // Fl_Button* btnSearchProduct
          { local o = db_orders_lines_quantity = new My_Fl_Float_Input(21, 435, 84, 25, _tr("Quantity"));
            db_orders_lines_quantity->type(1);
            db_orders_lines_quantity->box(FL_DOWN_BOX);
            db_orders_lines_quantity->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_quantity->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_quantity->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_quantity->labelfont(1);
            db_orders_lines_quantity->labelsize(16);
            db_orders_lines_quantity->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_quantity->textfont(1);
            db_orders_lines_quantity->textsize(16);
            db_orders_lines_quantity->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_quantity->when(FL_WHEN_RELEASE);
            o->decimal_places(-6);
            add_input_field_to_map("orders_lines", "quantity", db_orders_lines_quantity);
          } // My_Fl_Float_Input* db_orders_lines_quantity
          { db_orders_lines_price = new My_Fl_Float_Input(115, 435, 108, 25, _tr("Price"));
            db_orders_lines_price->type(1);
            db_orders_lines_price->box(FL_DOWN_BOX);
            db_orders_lines_price->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_price->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_price->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_price->labelfont(1);
            db_orders_lines_price->labelsize(16);
            db_orders_lines_price->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_price->textfont(1);
            db_orders_lines_price->textsize(16);
            db_orders_lines_price->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_price->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "price", db_orders_lines_price);
          } // My_Fl_Float_Input* db_orders_lines_price
          { db_orders_lines_price_decimals = new Fl_Int_Input(230, 435, 20, 25, _tr("(.)"));
            db_orders_lines_price_decimals->tooltip(_tr("Number of decimals to use on calculations"));
            db_orders_lines_price_decimals->type(2);
            db_orders_lines_price_decimals->labelsize(16);
            db_orders_lines_price_decimals->textsize(16);
            db_orders_lines_price_decimals->align((FL_ALIGN_TOP));
            add_input_field_to_map("orders_lines", "price_decimals", db_orders_lines_price_decimals);
          } // Fl_Int_Input* db_orders_lines_price_decimals
          { local o = db_orders_lines_weight = new My_Fl_Float_Input(255, 435, 70, 25, _tr("Weight"));
            db_orders_lines_weight->type(1);
            db_orders_lines_weight->box(FL_DOWN_BOX);
            db_orders_lines_weight->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_weight->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_weight->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_weight->labelfont(0);
            db_orders_lines_weight->labelsize(16);
            db_orders_lines_weight->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_weight->textsize(16);
            db_orders_lines_weight->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_weight->when(FL_WHEN_RELEASE);
            o->decimal_places(-6);
            add_input_field_to_map("orders_lines", "weight", db_orders_lines_weight);
          } // My_Fl_Float_Input* db_orders_lines_weight
          { db_orders_lines_first_total = new Fl_Float_Input_Fmt(330, 435, 100, 25, _tr("1st total"));
            db_orders_lines_first_total->type(1);
            db_orders_lines_first_total->box(FL_DOWN_BOX);
            db_orders_lines_first_total->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_first_total->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_first_total->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_first_total->labelfont(0);
            db_orders_lines_first_total->labelsize(16);
            db_orders_lines_first_total->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_first_total->textsize(16);
            db_orders_lines_first_total->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_first_total->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "first_total", db_orders_lines_first_total);
          } // Fl_Float_Input_Fmt* db_orders_lines_first_total
          { db_orders_lines_discount_pct = new My_Fl_Float_Input(435, 435, 60, 25, _tr("Disc. %"));
            db_orders_lines_discount_pct->type(1);
            db_orders_lines_discount_pct->box(FL_DOWN_BOX);
            db_orders_lines_discount_pct->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_discount_pct->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_discount_pct->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_discount_pct->labelfont(0);
            db_orders_lines_discount_pct->labelsize(16);
            db_orders_lines_discount_pct->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_discount_pct->textsize(16);
            db_orders_lines_discount_pct->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_discount_pct->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "discount_pct", db_orders_lines_discount_pct);
          } // My_Fl_Float_Input* db_orders_lines_discount_pct
          { db_orders_lines_discount_amt = new Fl_Float_Input_Fmt(505, 435, 80, 25, _tr("Disc. $"));
            db_orders_lines_discount_amt->type(1);
            db_orders_lines_discount_amt->box(FL_DOWN_BOX);
            db_orders_lines_discount_amt->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_discount_amt->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_discount_amt->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_discount_amt->labelfont(0);
            db_orders_lines_discount_amt->labelsize(16);
            db_orders_lines_discount_amt->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_discount_amt->textsize(16);
            db_orders_lines_discount_amt->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_discount_amt->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "discount_amt", db_orders_lines_discount_amt);
          } // Fl_Float_Input_Fmt* db_orders_lines_discount_amt
          { db_orders_lines_line_subtotal = new Fl_Float_Input_Fmt(21, 485, 108, 25, _tr("Subtotal"));
            db_orders_lines_line_subtotal->type(1);
            db_orders_lines_line_subtotal->box(FL_DOWN_BOX);
            db_orders_lines_line_subtotal->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_line_subtotal->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_line_subtotal->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_line_subtotal->labelfont(0);
            db_orders_lines_line_subtotal->labelsize(16);
            db_orders_lines_line_subtotal->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_line_subtotal->textsize(16);
            db_orders_lines_line_subtotal->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_line_subtotal->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "line_subtotal", db_orders_lines_line_subtotal);
          } // Fl_Float_Input_Fmt* db_orders_lines_line_subtotal
          { db_orders_lines_sales_tax2_pct = new Fl_Float_Input_Fmt(139, 485, 64, 25, _tr("R.E. %"));
            db_orders_lines_sales_tax2_pct->type(1);
            db_orders_lines_sales_tax2_pct->box(FL_DOWN_BOX);
            db_orders_lines_sales_tax2_pct->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_sales_tax2_pct->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_sales_tax2_pct->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_sales_tax2_pct->labelfont(0);
            db_orders_lines_sales_tax2_pct->labelsize(16);
            db_orders_lines_sales_tax2_pct->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_sales_tax2_pct->textsize(16);
            db_orders_lines_sales_tax2_pct->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_sales_tax2_pct->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "sales_tax2_pct", db_orders_lines_sales_tax2_pct);
          } // Fl_Float_Input_Fmt* db_orders_lines_sales_tax2_pct
          { db_orders_lines_sales_tax1_pct = new Fl_Float_Input_Fmt(213, 485, 64, 25, _tr("V.A.T. %"));
            db_orders_lines_sales_tax1_pct->type(1);
            db_orders_lines_sales_tax1_pct->box(FL_DOWN_BOX);
            db_orders_lines_sales_tax1_pct->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_sales_tax1_pct->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_sales_tax1_pct->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_sales_tax1_pct->labelfont(0);
            db_orders_lines_sales_tax1_pct->labelsize(16);
            db_orders_lines_sales_tax1_pct->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_sales_tax1_pct->textsize(16);
            db_orders_lines_sales_tax1_pct->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_sales_tax1_pct->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "sales_tax1_pct", db_orders_lines_sales_tax1_pct);
          } // Fl_Float_Input_Fmt* db_orders_lines_sales_tax1_pct
          { db_orders_lines_sales_tax1_amt = new Fl_Float_Input_Fmt(287, 485, 87, 25, _tr("V.A.T. $"));
            db_orders_lines_sales_tax1_amt->type(1);
            db_orders_lines_sales_tax1_amt->box(FL_DOWN_BOX);
            db_orders_lines_sales_tax1_amt->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_sales_tax1_amt->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_sales_tax1_amt->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_sales_tax1_amt->labelfont(0);
            db_orders_lines_sales_tax1_amt->labelsize(16);
            db_orders_lines_sales_tax1_amt->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_sales_tax1_amt->textsize(16);
            db_orders_lines_sales_tax1_amt->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_sales_tax1_amt->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "sales_tax1_amt", db_orders_lines_sales_tax1_amt);
          } // Fl_Float_Input_Fmt* db_orders_lines_sales_tax1_amt
          { btnSaveLine = new My_Fl_Return_Button(384, 485, 83, 25, _tr("Save"));
            btnSaveLine->tooltip(_tr("Save the line currently edited"));
            btnSaveLine->box(FL_UP_BOX);
            btnSaveLine->color(FL_BACKGROUND_COLOR);
            btnSaveLine->selection_color(FL_BACKGROUND_COLOR);
            btnSaveLine->labeltype(FL_NORMAL_LABEL);
            btnSaveLine->labelfont(1);
            btnSaveLine->labelsize(16);
            btnSaveLine->labelcolor(FL_FOREGROUND_COLOR);
            btnSaveLine->align((FL_ALIGN_CENTER));
            btnSaveLine->when(FL_WHEN_RELEASE);
          } // My_Fl_Return_Button* btnSaveLine
          { db_orders_lines_line_total = new Fl_Float_Input_Fmt(477, 485, 108, 25, _tr("Total"));
            db_orders_lines_line_total->type(1);
            db_orders_lines_line_total->box(FL_DOWN_BOX);
            db_orders_lines_line_total->color(FL_BACKGROUND2_COLOR);
            db_orders_lines_line_total->selection_color(FL_SELECTION_COLOR);
            db_orders_lines_line_total->labeltype(FL_NORMAL_LABEL);
            db_orders_lines_line_total->labelfont(1);
            db_orders_lines_line_total->labelsize(16);
            db_orders_lines_line_total->labelcolor(FL_FOREGROUND_COLOR);
            db_orders_lines_line_total->textfont(1);
            db_orders_lines_line_total->textsize(16);
            db_orders_lines_line_total->align((FL_ALIGN_TOP_LEFT));
            db_orders_lines_line_total->when(FL_WHEN_RELEASE);
            add_input_field_to_map("orders_lines", "line_total", db_orders_lines_line_total);
          } // Fl_Float_Input_Fmt* db_orders_lines_line_total
          group_lines->end();
        } // Fl_Group* group_lines
        { tabLineNotes = new Fl_Group(10, 320, 585, 200, _tr("Line Notes"));
          tabLineNotes->labelfont(2);
          tabLineNotes->labelsize(16);
          tabLineNotes->hide();
          { db_orders_lines_notes = new Fl_Input(80, 330, 505, 180, _tr("Notes"));
            db_orders_lines_notes->tooltip(_tr("Type partal description to search on availlable products"));
            db_orders_lines_notes->type(4);
            db_orders_lines_notes->labelsize(16);
            db_orders_lines_notes->textsize(16);
            add_input_field_to_map("orders_lines", "notes", db_orders_lines_notes);
          } // Fl_Input* db_orders_lines_notes
          tabLineNotes->end();
        } // Fl_Group* tabLineNotes
        { tabDues = new Fl_Group(10, 320, 585, 200, _tr("Dues"));
          tabDues->labelfont(2);
          tabDues->labelsize(16);
          tabDues->hide();
          { grid_dues = new Fl_Data_Table(15, 330, 575, 130);
            grid_dues->box(FL_NO_BOX);
            grid_dues->color(FL_BACKGROUND2_COLOR);
            grid_dues->selection_color(FL_SELECTION_COLOR);
            grid_dues->labeltype(FL_NORMAL_LABEL);
            grid_dues->labelfont(0);
            grid_dues->labelsize(16);
            grid_dues->labelcolor(FL_FOREGROUND_COLOR);
            grid_dues->textsize(16);
            grid_dues->align((FL_ALIGN_BOTTOM));
            grid_dues->when(FL_WHEN_RELEASE_ALWAYS);
          } // Fl_Data_Table* grid_dues
          { local o = due_id = new Fl_Output(15, 485, 80, 25, _tr("ID"));
            due_id->labelsize(16);
            due_id->textsize(16);
            due_id->align((FL_ALIGN_TOP_LEFT));
            o->right_to_left(1);
          } // Fl_Output* due_id
          { due_date = new Fl_Input(100, 485, 125, 25, _tr("Due"));
            due_date->labelsize(16);
            due_date->textfont(1);
            due_date->textsize(16);
            due_date->align((FL_ALIGN_TOP_LEFT));
          } // Fl_Input* due_date
          { btnShowCalendarDueDate = new Fl_Button(230, 485, 30, 25, _tr("@<->"));
            btnShowCalendarDueDate->tooltip(_tr("Show calendar"));
            btnShowCalendarDueDate->labelsize(18);
            btnShowCalendarDueDate->labelcolor(22);
          } // Fl_Button* btnShowCalendarDueDate
          { due_pct = new Fl_Float_Input_Fmt(270, 485, 80, 25, _tr("Pct. %"));
            due_pct->type(1);
            due_pct->box(FL_DOWN_BOX);
            due_pct->color(FL_BACKGROUND2_COLOR);
            due_pct->selection_color(FL_SELECTION_COLOR);
            due_pct->labeltype(FL_NORMAL_LABEL);
            due_pct->labelfont(1);
            due_pct->labelsize(16);
            due_pct->labelcolor(FL_FOREGROUND_COLOR);
            due_pct->textfont(1);
            due_pct->textsize(16);
            due_pct->align((FL_ALIGN_TOP_LEFT));
            due_pct->when(FL_WHEN_RELEASE);
          } // Fl_Float_Input_Fmt* due_pct
          { due_amount = new Fl_Float_Input_Fmt(355, 485, 120, 25, _tr("Amount"));
            due_amount->type(1);
            due_amount->box(FL_DOWN_BOX);
            due_amount->color(FL_BACKGROUND2_COLOR);
            due_amount->selection_color(FL_SELECTION_COLOR);
            due_amount->labeltype(FL_NORMAL_LABEL);
            due_amount->labelfont(1);
            due_amount->labelsize(16);
            due_amount->labelcolor(FL_FOREGROUND_COLOR);
            due_amount->textfont(1);
            due_amount->textsize(16);
            due_amount->align((FL_ALIGN_TOP_LEFT));
            due_amount->when(FL_WHEN_RELEASE);
          } // Fl_Float_Input_Fmt* due_amount
          { btnDueSave = new My_Fl_Return_Button(485, 485, 100, 25, _tr("Save"));
            btnDueSave->box(FL_UP_BOX);
            btnDueSave->color(FL_BACKGROUND_COLOR);
            btnDueSave->selection_color(FL_BACKGROUND_COLOR);
            btnDueSave->labeltype(FL_NORMAL_LABEL);
            btnDueSave->labelfont(0);
            btnDueSave->labelsize(16);
            btnDueSave->labelcolor(FL_FOREGROUND_COLOR);
            btnDueSave->align((FL_ALIGN_CENTER));
            btnDueSave->when(FL_WHEN_RELEASE);
          } // My_Fl_Return_Button* btnDueSave
          tabDues->end();
        } // Fl_Group* tabDues
        { tabPayments = new Fl_Group(10, 320, 585, 200, _tr("Payments"));
          tabPayments->labelfont(2);
          tabPayments->labelsize(16);
          tabPayments->hide();
          { grid_payments = new Fl_Data_Table(15, 330, 575, 130);
            grid_payments->box(FL_NO_BOX);
            grid_payments->color(FL_BACKGROUND2_COLOR);
            grid_payments->selection_color(FL_SELECTION_COLOR);
            grid_payments->labeltype(FL_NORMAL_LABEL);
            grid_payments->labelfont(0);
            grid_payments->labelsize(16);
            grid_payments->labelcolor(FL_FOREGROUND_COLOR);
            grid_payments->textsize(16);
            grid_payments->align((FL_ALIGN_BOTTOM));
            grid_payments->when(FL_WHEN_RELEASE_ALWAYS);
          } // Fl_Data_Table* grid_payments
          { local o = payment_id = new Fl_Output(15, 485, 80, 25, _tr("ID"));
            payment_id->labelsize(16);
            payment_id->textsize(16);
            payment_id->align((FL_ALIGN_TOP_LEFT));
            o->right_to_left(1);
          } // Fl_Output* payment_id
          { payment_date = new Fl_Input(100, 485, 125, 25, _tr("Date"));
            payment_date->labelsize(16);
            payment_date->textfont(1);
            payment_date->textsize(16);
            payment_date->align((FL_ALIGN_TOP_LEFT));
          } // Fl_Input* payment_date
          { btnShowCalendarPaymentDate = new Fl_Button(230, 485, 30, 25, _tr("@<->"));
            btnShowCalendarPaymentDate->tooltip(_tr("Show calendar"));
            btnShowCalendarPaymentDate->labelsize(18);
            btnShowCalendarPaymentDate->labelcolor(22);
          } // Fl_Button* btnShowCalendarPaymentDate
          { payemnt_amount = new Fl_Float_Input_Fmt(270, 485, 120, 25, _tr("Amount"));
            payemnt_amount->type(1);
            payemnt_amount->box(FL_DOWN_BOX);
            payemnt_amount->color(FL_BACKGROUND2_COLOR);
            payemnt_amount->selection_color(FL_SELECTION_COLOR);
            payemnt_amount->labeltype(FL_NORMAL_LABEL);
            payemnt_amount->labelfont(1);
            payemnt_amount->labelsize(16);
            payemnt_amount->labelcolor(FL_FOREGROUND_COLOR);
            payemnt_amount->textfont(1);
            payemnt_amount->textsize(16);
            payemnt_amount->align((FL_ALIGN_TOP_LEFT));
            payemnt_amount->when(FL_WHEN_RELEASE);
          } // Fl_Float_Input_Fmt* payemnt_amount
          { btnPaymentSave = new My_Fl_Return_Button(395, 485, 100, 25, _tr("Save"));
            btnPaymentSave->box(FL_UP_BOX);
            btnPaymentSave->color(FL_BACKGROUND_COLOR);
            btnPaymentSave->selection_color(FL_BACKGROUND_COLOR);
            btnPaymentSave->labeltype(FL_NORMAL_LABEL);
            btnPaymentSave->labelfont(0);
            btnPaymentSave->labelsize(16);
            btnPaymentSave->labelcolor(FL_FOREGROUND_COLOR);
            btnPaymentSave->align((FL_ALIGN_CENTER));
            btnPaymentSave->when(FL_WHEN_RELEASE);
          } // My_Fl_Return_Button* btnPaymentSave
          tabPayments->end();
        } // Fl_Group* tabPayments
        linesTab->end();
      } // Fl_Tabs* linesTab
      o->end();
    } // Fl_Group* o
    { btnNumberOrder = new My_Fl_Return_Button(230, 525, 82, 25, _tr("Number"));
      btnNumberOrder->box(FL_UP_BOX);
      btnNumberOrder->color(FL_BACKGROUND_COLOR);
      btnNumberOrder->selection_color(FL_BACKGROUND_COLOR);
      btnNumberOrder->labeltype(FL_NORMAL_LABEL);
      btnNumberOrder->labelfont(0);
      btnNumberOrder->labelsize(16);
      btnNumberOrder->labelcolor(FL_FOREGROUND_COLOR);
      btnNumberOrder->align((FL_ALIGN_CENTER));
      btnNumberOrder->when(FL_WHEN_RELEASE);
    } // My_Fl_Return_Button* btnNumberOrder
    { dbAction = new Fl_Button(10, 525, 119, 25);
      dbAction->resize(9, 525, 119, 25);
      dbAction->down_box(FL_BORDER_BOX);
      dbAction->labeltype(FL_NO_LABEL);
      dbAction->labelsize(16);
      dbAction->textsize(16);
    } // Fl_Choice* dbAction
    { btnDbAction = new My_Fl_Return_Button(138, 525, 82, 25, _tr("Action"));
      btnDbAction->resize(138, 525, 82, 25);
      btnDbAction->label(_tr("Action"));
      btnDbAction->tooltip(_tr("Perform the selected action on this order"));
      btnDbAction->box(FL_UP_BOX);
      btnDbAction->color(FL_BACKGROUND_COLOR);
      btnDbAction->selection_color(FL_BACKGROUND_COLOR);
      btnDbAction->labeltype(FL_NORMAL_LABEL);
      btnDbAction->labelfont(1);
      btnDbAction->labelsize(16);
      btnDbAction->labelcolor(FL_FOREGROUND_COLOR);
      btnDbAction->align((FL_ALIGN_CENTER));
      btnDbAction->when(FL_WHEN_RELEASE);
    } // My_Fl_Return_Button* btnDbAction
    tabMain->end();
    Fl_Group.current()->resizable(tabMain);
  } // Fl_Group* tabMain
  { tabDelivery = new Fl_Group(5, 95, 790, 460, _tr("Delivery"));
    tabDelivery->color(246);
    tabDelivery->labelfont(2);
    tabDelivery->labelsize(16);
    tabDelivery->hide();
    { db_orders_entity_address = new Fl_Input(91, 105, 694, 25, _tr("Address"));
      db_orders_entity_address->labelsize(16);
      db_orders_entity_address->textsize(16);
      add_input_field_to_map("orders", "entity_address", db_orders_entity_address);
    } // Fl_Input* db_orders_entity_address
    { db_orders_entity_zip = new Fl_Input(91, 135, 95, 25, _tr("ZIP"));
      db_orders_entity_zip->labelsize(16);
      db_orders_entity_zip->textsize(16);
      add_input_field_to_map("orders", "entity_zip", db_orders_entity_zip);
    } // Fl_Input* db_orders_entity_zip
    { db_orders_entity_city = new Fl_Input(288, 135, 497, 25, _tr("City"));
      db_orders_entity_city->labelsize(16);
      db_orders_entity_city->textsize(16);
      add_input_field_to_map("orders", "entity_city", db_orders_entity_city);
    } // Fl_Input* db_orders_entity_city
    { db_orders_entity_state = new Fl_Input(91, 165, 407, 25, _tr("State"));
      db_orders_entity_state->labelsize(16);
      db_orders_entity_state->textsize(16);
      add_input_field_to_map("orders", "entity_state", db_orders_entity_state);
    } // Fl_Input* db_orders_entity_state
    { db_orders_entity_country = new Fl_Input(585, 165, 201, 25, _tr("Country"));
      db_orders_entity_country->labelsize(16);
      db_orders_entity_country->textsize(16);
      add_input_field_to_map("orders", "entity_country", db_orders_entity_country);
    } // Fl_Input* db_orders_entity_country
    { db_orders_entity_phone = new Fl_Input(91, 195, 201, 25, _tr("Phone"));
      db_orders_entity_phone->labelsize(16);
      db_orders_entity_phone->textsize(16);
      add_input_field_to_map("orders", "entity_phone", db_orders_entity_phone);
    } // Fl_Input* db_orders_entity_phone
    { db_orders_entity_tax_number = new Fl_Input(585, 195, 200, 25, _tr("Tax Num."));
      db_orders_entity_tax_number->labelsize(16);
      db_orders_entity_tax_number->textsize(16);
      add_input_field_to_map("orders", "entity_tax_number", db_orders_entity_tax_number);
    } // Fl_Input* db_orders_entity_tax_number
    { db_orders_order_valid_till_date = new Fl_Input(90, 225, 121, 25, _tr("Valid till"));
      db_orders_order_valid_till_date->labelsize(16);
      db_orders_order_valid_till_date->textsize(16);
      add_input_field_to_map("orders", "order_valid_till_date", db_orders_order_valid_till_date);
    } // Fl_Input* db_orders_order_valid_till_date
    { btnShowCalendarValidTill = new Fl_Button(221, 225, 30, 25, _tr("@<->"));
      btnShowCalendarValidTill->tooltip(_tr("Show calendar"));
      btnShowCalendarValidTill->labelsize(18);
      btnShowCalendarValidTill->labelcolor(22);
    } // Fl_Button* btnShowCalendarValidTill
    { db_orders_entity_use_sales_tax2 = new Fl_Check_Button(585, 225, 155, 25, _tr("Use Sales Tax II"));
      db_orders_entity_use_sales_tax2->down_box(FL_DOWN_BOX);
      db_orders_entity_use_sales_tax2->labelsize(16);
      add_input_field_to_map("orders", "entity_use_sales_tax2", db_orders_entity_use_sales_tax2);
    } // Fl_Check_Button* db_orders_entity_use_sales_tax2
    { db_orders_entity_sales_tax_exempt = new Fl_Check_Button(585, 255, 155, 25, _tr("Sales Tax Exempt"));
      db_orders_entity_sales_tax_exempt->down_box(FL_DOWN_BOX);
      db_orders_entity_sales_tax_exempt->labelsize(16);
      add_input_field_to_map("orders", "entity_sales_tax_exempt", db_orders_entity_sales_tax_exempt);
    } // Fl_Check_Button* db_orders_entity_sales_tax_exempt
    { db_orders_notes = new Fl_Text_Editor_Buffered(15, 395, 770, 150, _tr("Notes"));
      db_orders_notes->box(FL_DOWN_FRAME);
      db_orders_notes->color(FL_BACKGROUND2_COLOR);
      db_orders_notes->selection_color(FL_SELECTION_COLOR);
      db_orders_notes->labeltype(FL_NORMAL_LABEL);
      db_orders_notes->labelfont(0);
      db_orders_notes->labelsize(16);
      db_orders_notes->labelcolor(FL_FOREGROUND_COLOR);
      db_orders_notes->textsize(16);
      db_orders_notes->align((FL_ALIGN_TOP_LEFT));
      db_orders_notes->when(FL_WHEN_RELEASE);
      Fl_Group.current()->resizable(db_orders_notes);
      add_input_field_to_map("orders", "notes", db_orders_notes);
    } // Fl_Text_Editor_Buffered* db_orders_notes
    { local o = new Fl_Check_Button(125, 375, 125, 25, _tr("Wrap lines"));
      o->down_box(FL_DOWN_BOX);
      o->value(1);
      o->labelsize(16);
      o->callback(cb_Wrap);
    } // Fl_Check_Button* o
    { db_orders_irpf_pct_retention = new Fl_Float_Input_Fmt(585, 285, 70, 25, _tr("I.R.P.F. %"));
      db_orders_irpf_pct_retention->type(1);
      db_orders_irpf_pct_retention->box(FL_DOWN_BOX);
      db_orders_irpf_pct_retention->color(FL_BACKGROUND2_COLOR);
      db_orders_irpf_pct_retention->selection_color(FL_SELECTION_COLOR);
      db_orders_irpf_pct_retention->labeltype(FL_NORMAL_LABEL);
      db_orders_irpf_pct_retention->labelfont(0);
      db_orders_irpf_pct_retention->labelsize(16);
      db_orders_irpf_pct_retention->labelcolor(FL_FOREGROUND_COLOR);
      db_orders_irpf_pct_retention->textsize(16);
      db_orders_irpf_pct_retention->align((FL_ALIGN_RIGHT));
      db_orders_irpf_pct_retention->when(FL_WHEN_RELEASE);
      add_input_field_to_map("orders", "irpf_pct_retention", db_orders_irpf_pct_retention);
    } // Fl_Float_Input_Fmt* db_orders_irpf_pct_retention
    tabDelivery->end();
  } // Fl_Group* tabDelivery
  { tabOptions = new Fl_Group(5, 95, 790, 460, _tr("Options"));
    tabOptions->color(246);
    tabOptions->labelfont(2);
    tabOptions->labelsize(16);
    tabOptions->hide();
    { opt_ask_for_printer = new Fl_Check_Button(20, 115, 155, 25, _tr("Ask for printer"));
      opt_ask_for_printer->down_box(FL_DOWN_BOX);
      opt_ask_for_printer->labelsize(16);
    } // Fl_Check_Button* opt_ask_for_printer
    { opt_print_only_data = new Fl_Check_Button(20, 150, 155, 25, _tr("Print only data"));
      opt_print_only_data->down_box(FL_DOWN_BOX);
      opt_print_only_data->labelsize(16);
    } // Fl_Check_Button* opt_print_only_data
    { opt_print_to_pdf = new Fl_Check_Button(20, 180, 155, 25, _tr("Print to PDF"));
      opt_print_to_pdf->down_box(FL_DOWN_BOX);
      opt_print_to_pdf->labelsize(16);
    } // Fl_Check_Button* opt_print_to_pdf
    { opt_print_page_format = new Fl_Choice_Str(20, 215, 130, 25);
      opt_print_page_format->box(FL_FLAT_BOX);
      opt_print_page_format->down_box(FL_BORDER_BOX);
      opt_print_page_format->color(FL_BACKGROUND_COLOR);
      opt_print_page_format->selection_color(FL_SELECTION_COLOR);
      opt_print_page_format->labeltype(FL_NO_LABEL);
      opt_print_page_format->labelfont(0);
      opt_print_page_format->labelsize(16);
      opt_print_page_format->labelcolor(FL_FOREGROUND_COLOR);
      opt_print_page_format->textsize(16);
      opt_print_page_format->align((FL_ALIGN_LEFT));
      opt_print_page_format->when(FL_WHEN_RELEASE);
    } // Fl_Choice_Str* opt_print_page_format
    { btnOptPrint = new Fl_Button(20, 250, 130, 25, _tr("Print"));
      btnOptPrint->tooltip(_tr("Print the current order"));
      btnOptPrint->labelsize(16);
    } // Fl_Button* btnOptPrint
    { opt_email_program = new Fl_Input(260, 145, 520, 25, _tr("Email program"));
      opt_email_program->tooltip(_tr("Type partal description to search on availlable entities"));
      opt_email_program->labelsize(16);
      opt_email_program->textsize(16);
      opt_email_program->align((FL_ALIGN_TOP_LEFT));
    } // Fl_Input* opt_email_program
    { opt_email_program_command = new Fl_Input(260, 210, 520, 25, _tr("Email program parameters"));
      opt_email_program_command->tooltip(_tr("Type partal description to search on availlable entities"));
      opt_email_program_command->labelsize(16);
      opt_email_program_command->textsize(16);
      opt_email_program_command->align((FL_ALIGN_TOP_LEFT));
    } // Fl_Input* opt_email_program_command
    { btnOptSendEmail = new Fl_Button(260, 250, 130, 25, _tr("Send as email"));
      btnOptSendEmail->tooltip(_tr("Print the current order"));
      btnOptSendEmail->labelsize(16);
    } // Fl_Button* btnOptSendEmail
    { opt_print_order_company_info = new Fl_Text_Editor_Buffered(20, 350, 375, 200, _tr("Company Info"));
      opt_print_order_company_info->box(FL_DOWN_FRAME);
      opt_print_order_company_info->color(FL_BACKGROUND2_COLOR);
      opt_print_order_company_info->selection_color(FL_SELECTION_COLOR);
      opt_print_order_company_info->labeltype(FL_NORMAL_LABEL);
      opt_print_order_company_info->labelfont(0);
      opt_print_order_company_info->labelsize(16);
      opt_print_order_company_info->labelcolor(FL_FOREGROUND_COLOR);
      opt_print_order_company_info->textsize(16);
      opt_print_order_company_info->align((FL_ALIGN_TOP_LEFT));
      opt_print_order_company_info->when(FL_WHEN_RELEASE);
    } // Fl_Text_Editor_Buffered* opt_print_order_company_info
    { local o = new Fl_Check_Button(200, 330, 125, 25, _tr("Wrap lines"));
      o->down_box(FL_DOWN_BOX);
      o->value(1);
      o->labelsize(16);
      o->callback(cb_Wrap1);
    } // Fl_Check_Button* o
    { opt_print_order_bottom_info = new Fl_Text_Editor_Buffered(405, 350, 375, 200, _tr("Company Info"));
      opt_print_order_bottom_info->box(FL_DOWN_FRAME);
      opt_print_order_bottom_info->color(FL_BACKGROUND2_COLOR);
      opt_print_order_bottom_info->selection_color(FL_SELECTION_COLOR);
      opt_print_order_bottom_info->labeltype(FL_NORMAL_LABEL);
      opt_print_order_bottom_info->labelfont(0);
      opt_print_order_bottom_info->labelsize(16);
      opt_print_order_bottom_info->labelcolor(FL_FOREGROUND_COLOR);
      opt_print_order_bottom_info->textsize(16);
      opt_print_order_bottom_info->align((FL_ALIGN_TOP_LEFT));
      opt_print_order_bottom_info->when(FL_WHEN_RELEASE);
      Fl_Group.current()->resizable(opt_print_order_bottom_info);
    } // Fl_Text_Editor_Buffered* opt_print_order_bottom_info
    { local o = new Fl_Check_Button(590, 330, 125, 25, _tr("Wrap lines"));
      o->down_box(FL_DOWN_BOX);
      o->value(1);
      o->labelsize(16);
      o->callback(cb_Wrap2);
    } // Fl_Check_Button* o
    tabOptions->end();
  } // Fl_Group* tabOptions
  { tabChartStatistics = new Fl_Group(5, 95, 790, 460, _tr("Statistics"));
    tabChartStatistics->color(246);
    tabChartStatistics->labelfont(2);
    tabChartStatistics->labelsize(16);
    tabChartStatistics->hide();
    tabChartStatistics->end();
  } // Fl_Group* tabChartStatistics
  { tabHistory = new Fl_Group(5, 95, 790, 460, _tr("History"));
    tabHistory->color(246);
    tabHistory->labelfont(2);
    tabHistory->labelsize(16);
    tabHistory->hide();
    tabHistory->end();
  } // Fl_Group* tabHistory
  { tabPrintPreview = new Fl_Group(5, 95, 790, 460, _tr("Preview"));
    tabPrintPreview->color(246);
    tabPrintPreview->labelfont(2);
    tabPrintPreview->labelsize(16);
    tabPrintPreview->hide();
    tabPrintPreview->end();
  } // Fl_Group* tabPrintPreview
  { tabTotals = new Fl_Group(5, 95, 790, 460, _tr("Totals"));
    tabTotals->color(246);
    tabTotals->labelfont(2);
    tabTotals->labelsize(16);
    tabTotals->hide();
    { totals_balance = new Fl_Float_Input_Fmt(145, 155, 110, 25, _tr("Balance"));
      totals_balance->type(1);
      totals_balance->box(FL_DOWN_BOX);
      totals_balance->color(FL_BACKGROUND2_COLOR);
      totals_balance->selection_color(FL_SELECTION_COLOR);
      totals_balance->labeltype(FL_NORMAL_LABEL);
      totals_balance->labelfont(0);
      totals_balance->labelsize(16);
      totals_balance->labelcolor(FL_FOREGROUND_COLOR);
      totals_balance->textsize(16);
      totals_balance->align((FL_ALIGN_LEFT));
      totals_balance->when(FL_WHEN_RELEASE);
    } // Fl_Float_Input_Fmt* totals_balance
    { db_totals_overview_sales_quoted = new Fl_Float_Input_Fmt(145, 200, 110, 25, _tr("Sales Quoted"));
      db_totals_overview_sales_quoted->type(1);
      db_totals_overview_sales_quoted->box(FL_DOWN_BOX);
      db_totals_overview_sales_quoted->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_quoted->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_quoted->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_quoted->labelfont(0);
      db_totals_overview_sales_quoted->labelsize(16);
      db_totals_overview_sales_quoted->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_quoted->textsize(16);
      db_totals_overview_sales_quoted->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_quoted->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_quoted", db_totals_overview_sales_quoted);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_quoted
    { db_totals_overview_sales_ordered = new Fl_Float_Input_Fmt(145, 235, 110, 25, _tr("Sales Ordered"));
      db_totals_overview_sales_ordered->type(1);
      db_totals_overview_sales_ordered->box(FL_DOWN_BOX);
      db_totals_overview_sales_ordered->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_ordered->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_ordered->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_ordered->labelfont(0);
      db_totals_overview_sales_ordered->labelsize(16);
      db_totals_overview_sales_ordered->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_ordered->textsize(16);
      db_totals_overview_sales_ordered->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_ordered->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_ordered", db_totals_overview_sales_ordered);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_ordered
    { db_totals_overview_sales_delivered = new Fl_Float_Input_Fmt(145, 270, 110, 25, _tr("Sales Delivered"));
      db_totals_overview_sales_delivered->type(1);
      db_totals_overview_sales_delivered->box(FL_DOWN_BOX);
      db_totals_overview_sales_delivered->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_delivered->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_delivered->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_delivered->labelfont(0);
      db_totals_overview_sales_delivered->labelsize(16);
      db_totals_overview_sales_delivered->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_delivered->textsize(16);
      db_totals_overview_sales_delivered->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_delivered->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_delivered", db_totals_overview_sales_delivered);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_delivered
    { db_totals_overview_sales_invoiced = new Fl_Float_Input_Fmt(145, 305, 110, 25, _tr("Sales Invoiced"));
      db_totals_overview_sales_invoiced->type(1);
      db_totals_overview_sales_invoiced->box(FL_DOWN_BOX);
      db_totals_overview_sales_invoiced->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_invoiced->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_invoiced->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_invoiced->labelfont(0);
      db_totals_overview_sales_invoiced->labelsize(16);
      db_totals_overview_sales_invoiced->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_invoiced->textsize(16);
      db_totals_overview_sales_invoiced->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_invoiced->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_invoiced", db_totals_overview_sales_invoiced);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_invoiced
    { db_totals_overview_sales_discount = new Fl_Float_Input_Fmt(145, 340, 110, 25, _tr("Sales Discount"));
      db_totals_overview_sales_discount->type(1);
      db_totals_overview_sales_discount->box(FL_DOWN_BOX);
      db_totals_overview_sales_discount->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_discount->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_discount->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_discount->labelfont(0);
      db_totals_overview_sales_discount->labelsize(16);
      db_totals_overview_sales_discount->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_discount->textsize(16);
      db_totals_overview_sales_discount->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_discount->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_discount", db_totals_overview_sales_discount);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_discount
    { db_totals_overview_sales_tax1 = new Fl_Float_Input_Fmt(145, 375, 110, 25, _tr("Sales Tax1"));
      db_totals_overview_sales_tax1->type(1);
      db_totals_overview_sales_tax1->box(FL_DOWN_BOX);
      db_totals_overview_sales_tax1->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_tax1->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_tax1->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_tax1->labelfont(0);
      db_totals_overview_sales_tax1->labelsize(16);
      db_totals_overview_sales_tax1->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_tax1->textsize(16);
      db_totals_overview_sales_tax1->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_tax1->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_tax1", db_totals_overview_sales_tax1);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_tax1
    { db_totals_overview_sales_tax2 = new Fl_Float_Input_Fmt(145, 410, 110, 25, _tr("Sales Tax2"));
      db_totals_overview_sales_tax2->type(1);
      db_totals_overview_sales_tax2->box(FL_DOWN_BOX);
      db_totals_overview_sales_tax2->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_tax2->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_tax2->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_tax2->labelfont(0);
      db_totals_overview_sales_tax2->labelsize(16);
      db_totals_overview_sales_tax2->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_tax2->textsize(16);
      db_totals_overview_sales_tax2->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_tax2->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_tax2", db_totals_overview_sales_tax2);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_tax2
    { db_totals_overview_sales_paid = new Fl_Float_Input_Fmt(145, 445, 110, 25, _tr("Sales Paid"));
      db_totals_overview_sales_paid->type(1);
      db_totals_overview_sales_paid->box(FL_DOWN_BOX);
      db_totals_overview_sales_paid->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_sales_paid->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_sales_paid->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_sales_paid->labelfont(0);
      db_totals_overview_sales_paid->labelsize(16);
      db_totals_overview_sales_paid->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_sales_paid->textsize(16);
      db_totals_overview_sales_paid->align((FL_ALIGN_LEFT));
      db_totals_overview_sales_paid->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "sales_paid", db_totals_overview_sales_paid);
    } // Fl_Float_Input_Fmt* db_totals_overview_sales_paid
    { btnRefreshTotals = new My_Fl_Return_Button(415, 155, 110, 25, _tr("Refresh"));
      btnRefreshTotals->box(FL_UP_BOX);
      btnRefreshTotals->color(FL_BACKGROUND_COLOR);
      btnRefreshTotals->selection_color(FL_BACKGROUND_COLOR);
      btnRefreshTotals->labeltype(FL_NORMAL_LABEL);
      btnRefreshTotals->labelfont(0);
      btnRefreshTotals->labelsize(16);
      btnRefreshTotals->labelcolor(FL_FOREGROUND_COLOR);
      btnRefreshTotals->align((FL_ALIGN_CENTER));
      btnRefreshTotals->when(FL_WHEN_RELEASE);
    } // My_Fl_Return_Button* btnRefreshTotals
    { db_totals_overview_buys_quoted = new Fl_Float_Input_Fmt(415, 200, 110, 25, _tr("Buys Quoted"));
      db_totals_overview_buys_quoted->type(1);
      db_totals_overview_buys_quoted->box(FL_DOWN_BOX);
      db_totals_overview_buys_quoted->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_quoted->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_quoted->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_quoted->labelfont(0);
      db_totals_overview_buys_quoted->labelsize(16);
      db_totals_overview_buys_quoted->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_quoted->textsize(16);
      db_totals_overview_buys_quoted->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_quoted->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_quoted", db_totals_overview_buys_quoted);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_quoted
    { db_totals_overview_buys_ordered = new Fl_Float_Input_Fmt(415, 235, 110, 25, _tr("Buys Ordered"));
      db_totals_overview_buys_ordered->type(1);
      db_totals_overview_buys_ordered->box(FL_DOWN_BOX);
      db_totals_overview_buys_ordered->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_ordered->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_ordered->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_ordered->labelfont(0);
      db_totals_overview_buys_ordered->labelsize(16);
      db_totals_overview_buys_ordered->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_ordered->textsize(16);
      db_totals_overview_buys_ordered->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_ordered->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_ordered", db_totals_overview_buys_ordered);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_ordered
    { db_totals_overview_buys_received = new Fl_Float_Input_Fmt(415, 270, 110, 25, _tr("Buys Received"));
      db_totals_overview_buys_received->type(1);
      db_totals_overview_buys_received->box(FL_DOWN_BOX);
      db_totals_overview_buys_received->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_received->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_received->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_received->labelfont(0);
      db_totals_overview_buys_received->labelsize(16);
      db_totals_overview_buys_received->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_received->textsize(16);
      db_totals_overview_buys_received->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_received->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_received", db_totals_overview_buys_received);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_received
    { db_totals_overview_buys_invoiced = new Fl_Float_Input_Fmt(415, 305, 110, 25, _tr("Buys Invoiced"));
      db_totals_overview_buys_invoiced->type(1);
      db_totals_overview_buys_invoiced->box(FL_DOWN_BOX);
      db_totals_overview_buys_invoiced->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_invoiced->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_invoiced->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_invoiced->labelfont(0);
      db_totals_overview_buys_invoiced->labelsize(16);
      db_totals_overview_buys_invoiced->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_invoiced->textsize(16);
      db_totals_overview_buys_invoiced->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_invoiced->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_invoiced", db_totals_overview_buys_invoiced);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_invoiced
    { db_totals_overview_buys_discount = new Fl_Float_Input_Fmt(415, 340, 110, 25, _tr("Buys Discount"));
      db_totals_overview_buys_discount->type(1);
      db_totals_overview_buys_discount->box(FL_DOWN_BOX);
      db_totals_overview_buys_discount->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_discount->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_discount->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_discount->labelfont(0);
      db_totals_overview_buys_discount->labelsize(16);
      db_totals_overview_buys_discount->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_discount->textsize(16);
      db_totals_overview_buys_discount->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_discount->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_discount", db_totals_overview_buys_discount);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_discount
    { db_totals_overview_buys_tax1 = new Fl_Float_Input_Fmt(415, 375, 110, 25, _tr("Buys Tax1"));
      db_totals_overview_buys_tax1->type(1);
      db_totals_overview_buys_tax1->box(FL_DOWN_BOX);
      db_totals_overview_buys_tax1->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_tax1->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_tax1->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_tax1->labelfont(0);
      db_totals_overview_buys_tax1->labelsize(16);
      db_totals_overview_buys_tax1->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_tax1->textsize(16);
      db_totals_overview_buys_tax1->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_tax1->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_tax1", db_totals_overview_buys_tax1);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_tax1
    { db_totals_overview_buys_tax2 = new Fl_Float_Input_Fmt(415, 410, 110, 25, _tr("Buys Tax2"));
      db_totals_overview_buys_tax2->type(1);
      db_totals_overview_buys_tax2->box(FL_DOWN_BOX);
      db_totals_overview_buys_tax2->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_tax2->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_tax2->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_tax2->labelfont(0);
      db_totals_overview_buys_tax2->labelsize(16);
      db_totals_overview_buys_tax2->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_tax2->textsize(16);
      db_totals_overview_buys_tax2->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_tax2->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_tax2", db_totals_overview_buys_tax2);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_tax2
    { db_totals_overview_buys_paid = new Fl_Float_Input_Fmt(415, 445, 110, 25, _tr("Buys Paid"));
      db_totals_overview_buys_paid->type(1);
      db_totals_overview_buys_paid->box(FL_DOWN_BOX);
      db_totals_overview_buys_paid->color(FL_BACKGROUND2_COLOR);
      db_totals_overview_buys_paid->selection_color(FL_SELECTION_COLOR);
      db_totals_overview_buys_paid->labeltype(FL_NORMAL_LABEL);
      db_totals_overview_buys_paid->labelfont(0);
      db_totals_overview_buys_paid->labelsize(16);
      db_totals_overview_buys_paid->labelcolor(FL_FOREGROUND_COLOR);
      db_totals_overview_buys_paid->textsize(16);
      db_totals_overview_buys_paid->align((FL_ALIGN_LEFT));
      db_totals_overview_buys_paid->when(FL_WHEN_RELEASE);
      add_input_field_to_map("totals_overview", "buys_paid", db_totals_overview_buys_paid);
    } // Fl_Float_Input_Fmt* db_totals_overview_buys_paid
    tabTotals->end();
  } // Fl_Group* tabTotals
  tabs->end();
  Fl_Group.current()->resizable(tabs);
} // Fl_Tabs* tabs
db_orders_order_type_id->textfont(db_orders_order_type_id->labelfont());
db_orders_order_type_id->textsize(db_orders_order_type_id->labelsize());
end();
}

function cb_Wrap(){}
function cb_Wrap1(){}
function cb_Wrap2(){}
}
