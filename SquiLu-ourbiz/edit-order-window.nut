class EditOrderWindow extends EditWindow {
  
  // Declaration of class members
  delivery_calc_window = null;
  entities_list_window = null;
  products_list_window = null;
  calendar_window = null;
  db_orders_mdate = null;
  db_orders_order_type_id = null;
  pack_line2 = null;
  db_orders_series = null;
  db_orders_order_number = null;
  db_orders_entity_id = null;
  db_orders_entity_name = null;
  btnSearchEntity = null;
  db_orders_entity_order_number = null;
  tabs = null;
  tabMain = null;
  pack_line3 = null;
  out_lines = null;
  db_orders_weight_total = null;
  btnCalcDelivery = null;
  db_orders_payment_type_id = null;
  btnSortLines = null;
  db_orders_order_date = null;
  btnShowCalendar = null;
  grid_lines = null;
  group_totals = null;
  db_orders_discount_amt = null;
  db_orders_subtotal_amt = null;
  db_orders_sales_tax1_amt = null;
  db_orders_sales_tax2_amt = null;
  db_orders_irpf_amt = null;
  db_orders_total_amt = null;
  db_orders_cash = null;
  cash_return = null;
  btnPrint = null;
  linesTab = null;
  group_lines = null;
  db_orders_lines_batch_order_line_quantity = null;
  db_orders_lines_batch_order_line_id = null;
  btnSearchBatchOrderLineId = null;
  db_orders_lines_xref_order_line_quantity = null;
  db_orders_lines_xref_order_line_id = null;
  btnSearchXrefOrderLineId = null;
  db_orders_lines_product_id = null;
  db_orders_lines_description = null;
  btnSearchProduct = null;
  db_orders_lines_quantity = null;
  db_orders_lines_price = null;
  db_orders_lines_price_decimals = null;
  db_orders_lines_weight = null;
  db_orders_lines_first_total = null;
  db_orders_lines_discount_pct = null;
  db_orders_lines_discount_amt = null;
  db_orders_lines_line_subtotal = null;
  db_orders_lines_sales_tax2_pct = null;
  db_orders_lines_sales_tax1_pct = null;
  db_orders_lines_sales_tax1_amt = null;
  btnSaveLine = null;
  db_orders_lines_line_total = null;
  tabLineNotes = null;
  db_orders_lines_notes = null;
  tabDues = null;
  grid_dues = null;
  due_id = null;
  due_date = null;
  btnShowCalendarDueDate = null;
  due_pct = null;
  due_amount = null;
  btnDueSave = null;
  tabPayments = null;
  grid_payments = null;
  payment_id = null;
  payment_date = null;
  btnShowCalendarPaymentDate = null;
  payemnt_amount = null;
  btnPaymentSave = null;
  btnNumberOrder = null;
  dbAction = null;
  btnDbAction = null;
  tabDelivery = null;
  db_orders_entity_address = null;
  db_orders_entity_zip = null;
  db_orders_entity_city = null;
  db_orders_entity_state = null;
  db_orders_entity_country = null;
  db_orders_entity_phone = null;
  db_orders_entity_tax_number = null;
  db_orders_order_valid_till_date = null;
  btnShowCalendarValidTill = null;
  db_orders_entity_use_sales_tax2 = null;
  db_orders_entity_sales_tax_exempt = null;
  db_orders_notes = null;
  db_orders_irpf_pct_retention = null;
  tabOptions = null;
  opt_ask_for_printer = null;
  opt_print_only_data = null;
  opt_print_to_pdf = null;
  opt_print_page_format = null;
  btnOptPrint = null;
  opt_email_program = null;
  opt_email_program_command = null;
  btnOptSendEmail = null;
  opt_print_order_company_info = null;
  opt_print_order_bottom_info = null;
  tabChartStatistics = null;
  tabHistory = null;
  tabPrintPreview = null;
  tabTotals = null;
  totals_balance = null;
  db_totals_overview_sales_quoted = null;
  db_totals_overview_sales_ordered = null;
  db_totals_overview_sales_delivered = null;
  db_totals_overview_sales_invoiced = null;
  db_totals_overview_sales_discount = null;
  db_totals_overview_sales_tax1 = null;
  db_totals_overview_sales_tax2 = null;
  db_totals_overview_sales_paid = null;
  btnRefreshTotals = null;
  db_totals_overview_buys_quoted = null;
  db_totals_overview_buys_ordered = null;
  db_totals_overview_buys_received = null;
  db_totals_overview_buys_invoiced = null;
  db_totals_overview_buys_discount = null;
  db_totals_overview_buys_tax1 = null;
  db_totals_overview_buys_tax2 = null;
  db_totals_overview_buys_paid = null;
  
  constructor(){
    base.constructor(255, 97, 800, 560, _tr("Edit Order"));
    // Create member functions and widgets
    delivery_calc_window = 0;
    entities_list_window = 0;
    products_list_window = 0;
    calendar_window = 0;
    {
      local o = Fl_Group(5, 1, 792, 62);
      {
        {
          local o = Fl_Output(670, 5, 125, 25);
          db_orders_mdate = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          add_input_field_to_map("orders", "mdate", db_orders_mdate);
        }
        {
          local o = Flu_Combo_List(5, 5, 655, 25);
          db_orders_order_type_id = o;
          o.labelfont(1);
          o.labeltype(FL_NO_LABEL);
          o.tooltip(_tr("Selection of the order type"));
          o.box(FL_DOWN_BOX);
          o.labelsize(16);
          add_input_field_to_map("orders", "order_type_id", db_orders_order_type_id);
        }
        {
          local o = Fl_Pack(5, 35, 790, 25);
          pack_line2 = o;
          o.type(1);
          o.labelsize(16);
          o->spacing(5);
          o->with_label(1);
          {
            {
              local o = Fl_Input(55, 35, 45, 25, _tr("Series"));
              db_orders_series = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "series", db_orders_series);
            }
            {
              local o = Fl_Int_Input(172, 35, 95, 25, _tr("Number"));
              db_orders_order_number = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "order_number", db_orders_order_number);
              o->use_numeric_format(0);
            }
            {
              local o = Fl_Int_Input(319, 35, 95, 25, _tr("Entity"));
              db_orders_entity_id = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_id", db_orders_entity_id);
            }
            {
              local o = Fl_Input(420, 35, 335, 25);
              db_orders_entity_name = o;
              o.textsize(16);
              o.labeltype(FL_NO_LABEL);
              o.textfont(1);
              o.labelsize(16);
              o.tooltip(_tr("Type partal description to search on availlable entities"));
              add_input_field_to_map("orders", "entity_name", db_orders_entity_name);
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Button(765, 35, 30, 25, _tr("@<->"));
              btnSearchEntity = o;
              o.labelcolor(22);
              o.labelsize(18);
              o.tooltip(_tr("Show entities search window"));
            }
          }
          o.end();
        }
      }
      o.end();
    }
    {
      local o = Fl_Input(585, 65, 210, 25, _tr("Their Nº"));
      db_orders_entity_order_number = o;
      o.textsize(16);
      o.labelsize(16);
      add_input_field_to_map("orders", "entity_order_number", db_orders_entity_order_number);
    }
    {
      local o = Fl_Tabs(5, 68, 790, 487);
      tabs = o;
      o.labelcolor(7);
      o.labelsize(16);
      o.selection_color(4);
      {
        {
          local o = Fl_Group(5, 95, 790, 460, _tr("Main"));
          tabMain = o;
          o.labelsize(16);
          o.labelfont(2);
          o.color(246);
          {
            {
              local o = Fl_Pack(11, 105, 780, 25);
              pack_line3 = o;
              o.type(1);
              o.labelsize(16);
              o->spacing(5);
              o->with_label(1);
              {
                {
                  local o = Fl_Output(57, 105, 69, 25, _tr("Lines"));
                  out_lines = o;
                  o.textsize(16);
                  o.labelsize(16);
                  o->right_to_left(1);
                }
                {
                  local o = Fl_Output(189, 105, 110, 25, _tr("Weight"));
                  db_orders_weight_total = o;
                  o.textsize(16);
                  o.labelsize(16);
                  o->right_to_left(1);
                  o->decimal_places(-2);
                  o->use_numeric_format(1);
                }
                {
                  local o = Fl_Button(309, 105, 30, 25, _tr("@<->"));
                  btnCalcDelivery = o;
                  o.labelcolor(22);
                  o.labelsize(18);
                  o.tooltip(_tr("Show delivery calc window"));
                }
                {
                  local o = Fl_Choice_Int(345, 105, 235, 25);
                  db_orders_payment_type_id = o;
                  o.textsize(16);
                  o.labeltype(FL_NO_LABEL);
                  o.textfont(1);
                  o.labelsize(16);
                  o.down_box(FL_BORDER_BOX);
                  add_input_field_to_map("orders", "payment_type_id", db_orders_payment_type_id);
                  Fl_Group.current().resizable(o);
                }
                {
                  local o = Fl_Button(590, 105, 30, 25, _tr("@2"));
                  btnSortLines = o;
                  o.labelcolor(22);
                  o.labelsize(18);
                  o.tooltip(_tr("Sort lines by description"));
                }
                {
                  local o = Fl_Input(629, 105, 120, 25);
                  db_orders_order_date = o;
                  o.textsize(16);
                  o.labeltype(FL_NO_LABEL);
                  o.textfont(1);
                  o.labelsize(16);
                  add_input_field_to_map("orders", "order_date", db_orders_order_date);
                }
                {
                  local o = Fl_Button(760, 105, 30, 25, _tr("@<->"));
                  btnShowCalendar = o;
                  o.labelcolor(22);
                  o.labelsize(18);
                  o.tooltip(_tr("Show calendar"));
                }
              }
              o.end();
            }
            {
              local o = Fl_Data_Table(10, 135, 780, 150);
              grid_lines = o;
              o.textsize(16);
              o.labelsize(16);
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(10, 290, 781, 260);
              {
                {
                  local o = Fl_Group(600, 291, 190, 259);
                  group_totals = o;
                  o.box(FL_UP_BOX);
                  o.color(174);
                  {
                    {
                      local o = Fl_Output(685, 297, 100, 25, _tr("Discount"));
                      db_orders_discount_amt = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o->right_to_left(1);
                      o->use_numeric_format(1);
                      add_input_field_to_map("orders", "discount_amt", db_orders_discount_amt);
                    }
                    {
                      local o = Fl_Output(684, 326, 100, 25, _tr("Subtotal"));
                      db_orders_subtotal_amt = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o->right_to_left(1);
                      o->use_numeric_format(1);
                      add_input_field_to_map("orders", "subtotal_amt", db_orders_subtotal_amt);
                    }
                    {
                      local o = Fl_Output(685, 356, 100, 25, _tr("V.A.T."));
                      db_orders_sales_tax1_amt = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o->right_to_left(1);
                      o->use_numeric_format(1);
                      add_input_field_to_map("orders", "sales_tax1_amt", db_orders_sales_tax1_amt);
                    }
                    {
                      local o = Fl_Output(684, 386, 100, 25, _tr("R.E."));
                      db_orders_sales_tax2_amt = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o->right_to_left(1);
                      o->use_numeric_format(1);
                      add_input_field_to_map("orders", "sales_tax2_amt", db_orders_sales_tax2_amt);
                    }
                    {
                      local o = Fl_Output(685, 416, 100, 25, _tr("I.R.P.F."));
                      db_orders_irpf_amt = o;
                      o.textsize(16);
                      o.labelsize(16);
                      o->right_to_left(1);
                      o->use_numeric_format(1);
                      add_input_field_to_map("orders", "irpf_amt", db_orders_irpf_amt);
                    }
                    {
                      local o = Fl_Output(610, 453, 174, 33, _tr("Total"));
                      db_orders_total_amt = o;
                      o.textsize(30);
                      o.align(5);
                      o.textfont(1);
                      o.color(119);
                      o.labelfont(1);
                      o->right_to_left(1);
                      o->use_numeric_format(1);
                      add_input_field_to_map("orders", "total_amt", db_orders_total_amt);
                    }
                    {
                      local o = Fl_Float_Input_Fmt(685, 490, 100, 25, _tr("Cash"));
                      db_orders_cash = o;
                      o.textsize(16);
                      o.labelfont(1);
                      o.labelsize(16);
                      o.textfont(1);
                      add_input_field_to_map("orders", "cash", db_orders_cash);
                    }
                    {
                      local o = Fl_Output(685, 520, 100, 25, _tr("Return"));
                      cash_return = o;
                      o.textsize(16);
                      o.labelfont(1);
                      o.textfont(1);
                      o.labelsize(16);
                      o->right_to_left(1);
                      o->use_numeric_format(1);
                    }
                  }
                  o.end();
                }
                {
                  local o = My_Fl_Return_Button(485, 525, 100, 25, _tr("Print"));
                  btnPrint = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Tabs(10, 290, 585, 230);
                  linesTab = o;
                  o.labelcolor(7);
                  o.labelfont(2);
                  o.selection_color(4);
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Group(10, 321, 585, 199, _tr("Line Edit"));
                      group_lines = o;
                      o.labelsize(16);
                      o.color(166);
                      o.labelfont(2);
                      o.box(FL_UP_BOX);
                      {
                        {
                          local o = Fl_Output(265, 330, 110, 25, _tr("Batch Qty."));
                          db_orders_lines_batch_order_line_quantity = o;
                          o.textsize(16);
                          o.labelsize(16);
                          o->right_to_left(1);
                          add_input_field_to_map("orders_lines", "batch_order_line_quantity", db_orders_lines_batch_order_line_quantity);
                        }
                        {
                          local o = Fl_Int_Input(435, 329, 110, 25, _tr("Batch #"));
                          db_orders_lines_batch_order_line_id = o;
                          o.textsize(16);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "batch_order_line_id", db_orders_lines_batch_order_line_id);
                        }
                        {
                          local o = Fl_Button(555, 329, 30, 25, _tr("@<->"));
                          btnSearchBatchOrderLineId = o;
                          o.labelcolor(22);
                          o.labelsize(18);
                          o.tooltip(_tr("Show products search window"));
                        }
                        {
                          local o = Fl_Output(265, 360, 110, 25, _tr("XRef. Qty."));
                          db_orders_lines_xref_order_line_quantity = o;
                          o.textsize(16);
                          o.labelsize(16);
                          o->right_to_left(1);
                          add_input_field_to_map("orders_lines", "xref_order_line_quantity", db_orders_lines_xref_order_line_quantity);
                        }
                        {
                          local o = Fl_Int_Input(435, 359, 110, 25, _tr("XRef #"));
                          db_orders_lines_xref_order_line_id = o;
                          o.textsize(16);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "xref_order_line_id", db_orders_lines_xref_order_line_id);
                        }
                        {
                          local o = Fl_Button(555, 359, 30, 25, _tr("@<->"));
                          btnSearchXrefOrderLineId = o;
                          o.labelcolor(22);
                          o.labelsize(18);
                          o.tooltip(_tr("Show products search window"));
                        }
                        {
                          local o = Fl_Int_Input(21, 387, 70, 25, _tr("Code"));
                          db_orders_lines_product_id = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "product_id", db_orders_lines_product_id);
                        }
                        {
                          local o = Fl_Input(101, 387, 444, 25, _tr("Description"));
                          db_orders_lines_description = o;
                          o.textsize(16);
                          o.align(5);
                          o.tooltip(_tr("Type partal description to search on availlable products"));
                          o.labelsize(16);
                          o.textfont(1);
                          add_input_field_to_map("orders_lines", "description", db_orders_lines_description);
                        }
                        {
                          local o = Fl_Button(555, 387, 30, 25, _tr("@<->"));
                          btnSearchProduct = o;
                          o.labelcolor(22);
                          o.labelsize(18);
                          o.tooltip(_tr("Show products search window"));
                        }
                        {
                          local o = My_Fl_Float_Input(21, 435, 84, 25, _tr("Quantity"));
                          db_orders_lines_quantity = o;
                          o.labelfont(1);
                          o.textfont(1);
                          o.textsize(16);
                          o.labelsize(16);
                          o.align(5);
                          o->decimal_places(-6);
                          add_input_field_to_map("orders_lines", "quantity", db_orders_lines_quantity);
                        }
                        {
                          local o = My_Fl_Float_Input(115, 435, 108, 25, _tr("Price"));
                          db_orders_lines_price = o;
                          o.textsize(16);
                          o.align(5);
                          o.textfont(1);
                          o.labelsize(16);
                          o.labelfont(1);
                          add_input_field_to_map("orders_lines", "price", db_orders_lines_price);
                        }
                        {
                          local o = Fl_Int_Input(230, 435, 20, 25, _tr("(.)"));
                          db_orders_lines_price_decimals = o;
                          o.textsize(16);
                          o.align(1);
                          o.tooltip(_tr("Number of decimals to use on calculations"));
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "price_decimals", db_orders_lines_price_decimals);
                        }
                        {
                          local o = My_Fl_Float_Input(255, 435, 70, 25, _tr("Weight"));
                          db_orders_lines_weight = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          o->decimal_places(-6);
                          add_input_field_to_map("orders_lines", "weight", db_orders_lines_weight);
                        }
                        {
                          local o = Fl_Float_Input_Fmt(330, 435, 100, 25, _tr("1st total"));
                          db_orders_lines_first_total = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "first_total", db_orders_lines_first_total);
                        }
                        {
                          local o = My_Fl_Float_Input(435, 435, 60, 25, _tr("Disc. %"));
                          db_orders_lines_discount_pct = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "discount_pct", db_orders_lines_discount_pct);
                        }
                        {
                          local o = Fl_Float_Input_Fmt(505, 435, 80, 25, _tr("Disc. $"));
                          db_orders_lines_discount_amt = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "discount_amt", db_orders_lines_discount_amt);
                        }
                        {
                          local o = Fl_Float_Input_Fmt(21, 485, 108, 25, _tr("Subtotal"));
                          db_orders_lines_line_subtotal = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "line_subtotal", db_orders_lines_line_subtotal);
                        }
                        {
                          local o = Fl_Float_Input_Fmt(139, 485, 64, 25, _tr("R.E. %"));
                          db_orders_lines_sales_tax2_pct = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "sales_tax2_pct", db_orders_lines_sales_tax2_pct);
                        }
                        {
                          local o = Fl_Float_Input_Fmt(213, 485, 64, 25, _tr("V.A.T. %"));
                          db_orders_lines_sales_tax1_pct = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "sales_tax1_pct", db_orders_lines_sales_tax1_pct);
                        }
                        {
                          local o = Fl_Float_Input_Fmt(287, 485, 87, 25, _tr("V.A.T. $"));
                          db_orders_lines_sales_tax1_amt = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "sales_tax1_amt", db_orders_lines_sales_tax1_amt);
                        }
                        {
                          local o = My_Fl_Return_Button(384, 485, 83, 25, _tr("Save"));
                          btnSaveLine = o;
                          o.labelfont(1);
                          o.labelsize(16);
                          o.tooltip(_tr("Save the line currently edited"));
                        }
                        {
                          local o = Fl_Float_Input_Fmt(477, 485, 108, 25, _tr("Total"));
                          db_orders_lines_line_total = o;
                          o.textsize(16);
                          o.align(5);
                          o.textfont(1);
                          o.labelsize(16);
                          o.labelfont(1);
                          add_input_field_to_map("orders_lines", "line_total", db_orders_lines_line_total);
                        }
                      }
                      o.end();
                    }
                    {
                      local o = Fl_Group(10, 320, 585, 200, _tr("Line Notes"));
                      tabLineNotes = o;
                      o.labelfont(2);
                      o.labelsize(16);
                      {
                        {
                          local o = Fl_Multiline_Input(80, 330, 505, 180, _tr("Notes"));
                          db_orders_lines_notes = o;
                          o.textsize(16);
                          o.tooltip(_tr("Type partal description to search on availlable products"));
                          o.labelsize(16);
                          add_input_field_to_map("orders_lines", "notes", db_orders_lines_notes);
                        }
                      }
                      o.end();
                    }
                    {
                      local o = Fl_Group(10, 320, 585, 200, _tr("Dues"));
                      tabDues = o;
                      o.labelfont(2);
                      o.labelsize(16);
                      {
                        {
                          local o = Fl_Data_Table(15, 330, 575, 130);
                          grid_dues = o;
                          o.labelsize(16);
                          o.textsize(16);
                          o.end();
                        }
                        {
                          local o = Fl_Output(15, 485, 80, 25, _tr("ID"));
                          due_id = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          o->right_to_left(1);
                        }
                        {
                          local o = Fl_Input(100, 485, 125, 25, _tr("Due"));
                          due_date = o;
                          o.textsize(16);
                          o.textfont(1);
                          o.labelsize(16);
                          o.align(5);
                        }
                        {
                          local o = Fl_Button(230, 485, 30, 25, _tr("@<->"));
                          btnShowCalendarDueDate = o;
                          o.labelcolor(22);
                          o.labelsize(18);
                          o.tooltip(_tr("Show calendar"));
                        }
                        {
                          local o = Fl_Float_Input_Fmt(270, 485, 80, 25, _tr("Pct. %"));
                          due_pct = o;
                          o.textsize(16);
                          o.align(5);
                          o.textfont(1);
                          o.labelsize(16);
                          o.labelfont(1);
                        }
                        {
                          local o = Fl_Float_Input_Fmt(355, 485, 120, 25, _tr("Amount"));
                          due_amount = o;
                          o.textsize(16);
                          o.align(5);
                          o.textfont(1);
                          o.labelsize(16);
                          o.labelfont(1);
                        }
                        {
                          local o = My_Fl_Return_Button(485, 485, 100, 25, _tr("Save"));
                          btnDueSave = o;
                          o.labelsize(16);
                        }
                      }
                      o.end();
                    }
                    {
                      local o = Fl_Group(10, 320, 585, 200, _tr("Payments"));
                      tabPayments = o;
                      o.labelfont(2);
                      o.labelsize(16);
                      {
                        {
                          local o = Fl_Data_Table(15, 330, 575, 130);
                          grid_payments = o;
                          o.textsize(16);
                          o.labelsize(16);
                          o.end();
                        }
                        {
                          local o = Fl_Output(15, 485, 80, 25, _tr("ID"));
                          payment_id = o;
                          o.textsize(16);
                          o.labelsize(16);
                          o.align(5);
                          o->right_to_left(1);
                        }
                        {
                          local o = Fl_Input(100, 485, 125, 25, _tr("Date"));
                          payment_date = o;
                          o.textsize(16);
                          o.align(5);
                          o.textfont(1);
                          o.labelsize(16);
                        }
                        {
                          local o = Fl_Button(230, 485, 30, 25, _tr("@<->"));
                          btnShowCalendarPaymentDate = o;
                          o.labelcolor(22);
                          o.labelsize(18);
                          o.tooltip(_tr("Show calendar"));
                        }
                        {
                          local o = Fl_Float_Input_Fmt(270, 485, 120, 25, _tr("Amount"));
                          payemnt_amount = o;
                          o.textsize(16);
                          o.align(5);
                          o.textfont(1);
                          o.labelsize(16);
                          o.labelfont(1);
                        }
                        {
                          local o = My_Fl_Return_Button(395, 485, 100, 25, _tr("Save"));
                          btnPaymentSave = o;
                          o.labelsize(16);
                        }
                      }
                      o.end();
                    }
                  }
                  o.end();
                }
              }
              o.end();
            }
            {
              local o = My_Fl_Return_Button(230, 525, 82, 25, _tr("Number"));
              btnNumberOrder = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Choice_dbAction(9, 525, 119, 25);
              dbAction = o;
	      o->label(_tr("Action"));
              o.textsize(16);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
            }
            {
              local o = My_Fl_Return_Button(138, 525, 82, 25, _tr("Action"));
              btnDbAction = o;
              o.labelfont(1);
              o.tooltip(_tr("Perform the selected action on this order"));
              o.labelsize(16);
            }
          }
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(5, 95, 790, 460, _tr("Delivery"));
          tabDelivery = o;
          o.labelfont(2);
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Input(91, 105, 694, 25, _tr("Address"));
              db_orders_entity_address = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_address", db_orders_entity_address);
            }
            {
              local o = Fl_Input(91, 135, 95, 25, _tr("ZIP"));
              db_orders_entity_zip = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_zip", db_orders_entity_zip);
            }
            {
              local o = Fl_Input(288, 135, 497, 25, _tr("City"));
              db_orders_entity_city = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_city", db_orders_entity_city);
            }
            {
              local o = Fl_Input(91, 165, 407, 25, _tr("State"));
              db_orders_entity_state = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_state", db_orders_entity_state);
            }
            {
              local o = Fl_Input(585, 165, 201, 25, _tr("Country"));
              db_orders_entity_country = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_country", db_orders_entity_country);
            }
            {
              local o = Fl_Input(91, 195, 201, 25, _tr("Phone"));
              db_orders_entity_phone = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_phone", db_orders_entity_phone);
            }
            {
              local o = Fl_Input(585, 195, 200, 25, _tr("Tax Num."));
              db_orders_entity_tax_number = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_tax_number", db_orders_entity_tax_number);
            }
            {
              local o = Fl_Input(90, 225, 121, 25, _tr("Valid till"));
              db_orders_order_valid_till_date = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("orders", "order_valid_till_date", db_orders_order_valid_till_date);
            }
            {
              local o = Fl_Button(221, 225, 30, 25, _tr("@<->"));
              btnShowCalendarValidTill = o;
              o.labelcolor(22);
              o.labelsize(18);
              o.tooltip(_tr("Show calendar"));
            }
            {
              local o = Fl_Check_Button(585, 225, 155, 25, _tr("Use Sales Tax II"));
              db_orders_entity_use_sales_tax2 = o;
              o.down_box(FL_DOWN_BOX);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_use_sales_tax2", db_orders_entity_use_sales_tax2);
            }
            {
              local o = Fl_Check_Button(585, 255, 155, 25, _tr("Sales Tax Exempt"));
              db_orders_entity_sales_tax_exempt = o;
              o.down_box(FL_DOWN_BOX);
              o.labelsize(16);
              add_input_field_to_map("orders", "entity_sales_tax_exempt", db_orders_entity_sales_tax_exempt);
            }
            {
              local o = Fl_Text_Editor_Buffered(15, 395, 770, 150, _tr("Notes"));
              db_orders_notes = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              #include <FL/Fl_Text_Editor.H>
              add_input_field_to_map("orders", "notes", db_orders_notes);
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Check_Button(125, 375, 125, 25, _tr("Wrap lines"));
              o.labelsize(16);
              o.value(1);
              o.down_box(FL_DOWN_BOX);
              o.callback(function(sender, udata){
                db_orders_notes->wrap_mode(o->value(), 0);
              });
            }
            {
              local o = Fl_Float_Input_Fmt(585, 285, 70, 25, _tr("I.R.P.F. %"));
              db_orders_irpf_pct_retention = o;
              o.textsize(16);
              o.align(8);
              o.labelsize(16);
              add_input_field_to_map("orders", "irpf_pct_retention", db_orders_irpf_pct_retention);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 95, 790, 460, _tr("Options"));
          tabOptions = o;
          o.labelfont(2);
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Check_Button(20, 115, 155, 25, _tr("Ask for printer"));
              opt_ask_for_printer = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
            }
            {
              local o = Fl_Check_Button(20, 150, 155, 25, _tr("Print only data"));
              opt_print_only_data = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
            }
            {
              local o = Fl_Check_Button(20, 180, 155, 25, _tr("Print to PDF"));
              opt_print_to_pdf = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
            }
            {
              local o = Fl_Choice_Str(20, 215, 130, 25);
              opt_print_page_format = o;
              o.textsize(16);
              o.labeltype(FL_NO_LABEL);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
            }
            {
              local o = Fl_Button(20, 250, 130, 25, _tr("Print"));
              btnOptPrint = o;
              o.labelsize(16);
              o.tooltip(_tr("Print the current order"));
            }
            {
              local o = Fl_Input(260, 145, 520, 25, _tr("Email program"));
              opt_email_program = o;
              o.textsize(16);
              o.align(5);
              o.tooltip(_tr("Type partal description to search on availlable entities"));
              o.labelsize(16);
            }
            {
              local o = Fl_Input(260, 210, 520, 25, _tr("Email program parameters"));
              opt_email_program_command = o;
              o.textsize(16);
              o.align(5);
              o.tooltip(_tr("Type partal description to search on availlable entities"));
              o.labelsize(16);
            }
            {
              local o = Fl_Button(260, 250, 130, 25, _tr("Send as email"));
              btnOptSendEmail = o;
              o.labelsize(16);
              o.tooltip(_tr("Print the current order"));
            }
            {
              local o = Fl_Text_Editor_Buffered(20, 350, 375, 200, _tr("Company Info"));
              opt_print_order_company_info = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              #include <FL/Fl_Text_Editor.H>
            }
            {
              local o = Fl_Check_Button(200, 330, 125, 25, _tr("Wrap lines"));
              o.labelsize(16);
              o.value(1);
              o.down_box(FL_DOWN_BOX);
              o.callback(function(sender, udata){
                opt_print_order_company_info->wrap_mode(o->value(), 0);
              });
            }
            {
              local o = Fl_Text_Editor_Buffered(405, 350, 375, 200, _tr("Company Info"));
              opt_print_order_bottom_info = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              #include <FL/Fl_Text_Editor.H>
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Check_Button(590, 330, 125, 25, _tr("Wrap lines"));
              o.labelsize(16);
              o.value(1);
              o.down_box(FL_DOWN_BOX);
              o.callback(function(sender, udata){
                opt_print_order_bottom_info->wrap_mode(o->value(), 0);
              });
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 95, 790, 460, _tr("Statistics"));
          tabChartStatistics = o;
          o.labelsize(16);
          o.labelfont(2);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 95, 790, 460, _tr("History"));
          tabHistory = o;
          o.labelsize(16);
          o.labelfont(2);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 95, 790, 460, _tr("Preview"));
          tabPrintPreview = o;
          o.labelsize(16);
          o.labelfont(2);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 95, 790, 460, _tr("Totals"));
          tabTotals = o;
          o.labelfont(2);
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Float_Input_Fmt(145, 155, 110, 25, _tr("Balance"));
              totals_balance = o;
              o.textsize(16);
              o.labelsize(16);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 200, 110, 25, _tr("Sales Quoted"));
              db_totals_overview_sales_quoted = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_quoted", db_totals_overview_sales_quoted);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 235, 110, 25, _tr("Sales Ordered"));
              db_totals_overview_sales_ordered = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_ordered", db_totals_overview_sales_ordered);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 270, 110, 25, _tr("Sales Delivered"));
              db_totals_overview_sales_delivered = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_delivered", db_totals_overview_sales_delivered);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 305, 110, 25, _tr("Sales Invoiced"));
              db_totals_overview_sales_invoiced = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_invoiced", db_totals_overview_sales_invoiced);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 340, 110, 25, _tr("Sales Discount"));
              db_totals_overview_sales_discount = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_discount", db_totals_overview_sales_discount);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 375, 110, 25, _tr("Sales Tax1"));
              db_totals_overview_sales_tax1 = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_tax1", db_totals_overview_sales_tax1);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 410, 110, 25, _tr("Sales Tax2"));
              db_totals_overview_sales_tax2 = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_tax2", db_totals_overview_sales_tax2);
            }
            {
              local o = Fl_Float_Input_Fmt(145, 445, 110, 25, _tr("Sales Paid"));
              db_totals_overview_sales_paid = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "sales_paid", db_totals_overview_sales_paid);
            }
            {
              local o = My_Fl_Return_Button(415, 155, 110, 25, _tr("Refresh"));
              btnRefreshTotals = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 200, 110, 25, _tr("Buys Quoted"));
              db_totals_overview_buys_quoted = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_quoted", db_totals_overview_buys_quoted);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 235, 110, 25, _tr("Buys Ordered"));
              db_totals_overview_buys_ordered = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_ordered", db_totals_overview_buys_ordered);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 270, 110, 25, _tr("Buys Received"));
              db_totals_overview_buys_received = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_received", db_totals_overview_buys_received);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 305, 110, 25, _tr("Buys Invoiced"));
              db_totals_overview_buys_invoiced = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_invoiced", db_totals_overview_buys_invoiced);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 340, 110, 25, _tr("Buys Discount"));
              db_totals_overview_buys_discount = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_discount", db_totals_overview_buys_discount);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 375, 110, 25, _tr("Buys Tax1"));
              db_totals_overview_buys_tax1 = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_tax1", db_totals_overview_buys_tax1);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 410, 110, 25, _tr("Buys Tax2"));
              db_totals_overview_buys_tax2 = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_tax2", db_totals_overview_buys_tax2);
            }
            {
              local o = Fl_Float_Input_Fmt(415, 445, 110, 25, _tr("Buys Paid"));
              db_totals_overview_buys_paid = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("totals_overview", "buys_paid", db_totals_overview_buys_paid);
            }
          }
          o.end();
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    db_orders_order_type_id->textfont(db_orders_order_type_id->labelfont());
    db_orders_order_type_id->textsize(db_orders_order_type_id->labelsize());
  }
}
