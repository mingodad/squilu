class EditEntityWindow extends EditWindow {

  // Declaration of class members
  db_products_id = null;
  db_products_description_ro = null;
  db_products_mdate = null;
  db_products_cdate = null;
  tabs = null;
  tabMain = null;
  db_products_reference_code = null;
  db_products_supplier_code = null;
  db_products_bar_code = null;
  db_products_sell_description = null;
  db_products_buy_description = null;
  db_products_sell_price = null;
  db_products_measure_unit_id = null;
  db_products_sales_tax_id = null;
  db_products_warranty_id = null;
  db_products_sell_quantity_min = null;
  db_products_units_by_package = null;
  db_products_weight = null;
  db_products_sell_without_stock = null;
  db_products_is_active = null;
  db_products_show_on_sales = null;
  db_products_show_on_buys = null;
  db_products_show_on_web = null;
  db_products_show_price_on_web = null;
  dbAction = null;
  btnDbAction = null;
  tabsMoreData = null;
  tabNotes = null;
  db_products_sell_notes = null;
  tabNotesSupplier = null;
  db_products_buy_notes = null;
  tabTags = null;
  db_products_tags = null;
  db_products_group_id = null;
  btnImage = null;
  db_products_quantity_onhand = null;
  tabPrices = null;
  db_products_buy_price = null;
  db_products_buy_discount = null;
  db_products_buy_other_costs = null;
  db_products_sell_markup = null;
  markup_to_discount = null;
  db_products_sell_price_ro = null;
  db_products_price_decimals = null;
  btnSaveProrduct = null;
  db_products_stock_min = null;
  db_products_stock_max = null;
  db_products_buy_quantity_min = null;
  db_products_price_formula = null;
  db_products_price_date = null;
  productPrices = null;
  tabKit = null;
  tabChartStatistics = null;
  tabHistory = null;
  tabReports = null;
  grpExportImport = null;
  btnProductsExport = null;
  btnProductsImport = null;
  chkWithoutPrices = null;
  chkWebOnly = null;
  boxExportImportCount = null;
  btnProductGroupsExport = null;
  btnProductGroupsImport = null;
  btnUpdateWeb = null;
  grpPrintPriceList = null;
  chkPriceListWithVAT = null;
  chkPriceListSellOnWeb = null;
  btnPrintPriceList = null;
  chkPriceListSellSales = null;
  chkPriceListSellPDF = null;
  tabTotals = null;
  db_products_quantity_quoted_sales = null;
  db_products_quantity_ordered_sales = null;
  db_products_quantity_delivered = null;
  db_products_quantity_invoiced_sales = null;
  db_products_quantity_lost = null;
  db_products_quantity_quoted_buys = null;
  db_products_quantity_ordered_buys = null;
  db_products_quantity_received = null;
  db_products_quantity_invoiced_buys = null;
  
  constructor(px=34, py=28, pw=800, ph=560, pl=_tr("Edit Products")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Output(5, 5, 96, 25);
      db_products_id = o;
      o.textsize(16);
      o.labelsize(16);
      o.labeltype(FL_NO_LABEL);
      add_input_field_to_map("products", "id", db_products_id);
    }
    {
      local o = Fl_Output(110, 5, 545, 25);
      db_products_description_ro = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
    }
    {
      local o = Fl_Output(666, 5, 130, 25);
      db_products_mdate = o;
      o.textsize(16);
      o.labelsize(16);
      o.labeltype(FL_NO_LABEL);
      add_input_field_to_map("products", "mdate", db_products_mdate);
    }
    {
      local o = Fl_Output(665, 35, 130, 25);
      db_products_cdate = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      add_input_field_to_map("products", "cdate", db_products_cdate);
    }
    {
      local o = Fl_Tabs(5, 35, 790, 520);
      tabs = o;
      o.labelcolor(7);
      o.selection_color(4);
      o.labelsize(16);
      {
        {
          local o = Fl_Group(5, 60, 790, 495, _tr("Main"));
          tabMain = o;
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Input(115, 70, 165, 25, _tr("Reference"));
              db_products_reference_code = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("products", "reference_code", db_products_reference_code);
            }
            {
              local o = Fl_Input(380, 70, 165, 25, _tr("Ref. Supp."));
              db_products_supplier_code = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("products", "supplier_code", db_products_supplier_code);
            }
            {
              local o = Fl_Input(620, 70, 165, 25, _tr("Barcode"));
              db_products_bar_code = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("products", "bar_code", db_products_bar_code);
            }
            {
              local o = Fl_Input(115, 100, 670, 25, _tr("Description"));
              db_products_sell_description = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("products", "sell_description", db_products_sell_description);
            }
            {
              local o = Fl_Input(115, 130, 670, 25, _tr("Desc. Supp."));
              db_products_buy_description = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("products", "buy_description", db_products_buy_description);
            }
            {
              local o = Fl_Float_Input_Fmt(115, 160, 110, 25, _tr("Sales Price"));
              db_products_sell_price = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("products", "sell_price", db_products_sell_price);
            }
            {
              local o = Fl_Choice_Int(370, 160, 110, 25, _tr("Measure Unit"));
              db_products_measure_unit_id = o;
              o.textsize(16);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
              add_input_field_to_map("products", "measure_unit_id", db_products_measure_unit_id);
            }
            {
              local o = Fl_Choice_Int(115, 190, 110, 25, _tr("V.A.T."));
              db_products_sales_tax_id = o;
              o.textsize(16);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
              add_input_field_to_map("products", "sales_tax_id", db_products_sales_tax_id);
            }
            {
              local o = Fl_Choice_Int(370, 190, 110, 25, _tr("Warranty"));
              db_products_warranty_id = o;
              o.textsize(16);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
              add_input_field_to_map("products", "warranty_id", db_products_warranty_id);
            }
            {
              local o = Fl_Float_Input(370, 250, 110, 25, _tr("Sell Min. Qty."));
              db_products_sell_quantity_min = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "sell_quantity_min", db_products_sell_quantity_min);
            }
            {
              local o = Fl_Float_Input(370, 280, 110, 25, _tr("Units by Pack."));
              db_products_units_by_package = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "units_by_package", db_products_units_by_package);
            }
            {
              local o = Fl_Float_Input(370, 220, 110, 25, _tr("Weight"));
              db_products_weight = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "weight", db_products_weight);
            }
            {
              local o = Fl_Check_Button(10, 250, 215, 25, _tr("Sell without stock"));
              db_products_sell_without_stock = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("products", "sell_without_stock", db_products_sell_without_stock);
            }
            {
              local o = Fl_Check_Button(10, 280, 110, 25, _tr("Active"));
              db_products_is_active = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("products", "is_active", db_products_is_active);
            }
            {
              local o = Fl_Check_Button(10, 310, 105, 25, _tr("Sales"));
              db_products_show_on_sales = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("products", "show_on_sales", db_products_show_on_sales);
            }
            {
              local o = Fl_Check_Button(10, 340, 95, 25, _tr("Buys"));
              db_products_show_on_buys = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("products", "show_on_buys", db_products_show_on_buys);
            }
            {
              local o = Fl_Check_Button(115, 310, 120, 25, _tr("Web"));
              db_products_show_on_web = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("products", "show_on_web", db_products_show_on_web);
            }
            {
              local o = Fl_Check_Button(115, 340, 100, 25, _tr("Web price"));
              db_products_show_price_on_web = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("products", "show_price_on_web", db_products_show_price_on_web);
            }
            {
              local o = Fl_Group(675, 402, 110, 143);
              {
                {
                  local o = Fl_Box(675, 402, 110, 68, _tr("Spacer"));
                  o.labeltype(FL_NO_LABEL);
                  Fl_Group.current().resizable(o);
                }
                {
                  local o = Fl_Choice_dbAction(675, 520, 110, 25, _tr("Action"));
                  dbAction = o;
                  o.textsize(16);
                  o.align(1);
                  o.down_box(FL_BORDER_BOX);
                  o.labelsize(16);
                }
                {
                  local o = Fl_Button(675, 470, 110, 25, _tr("Action"));
                  btnDbAction = o;
                  o.labelsize(16);
                }
              }
              o.end();
            }
            {
              local o = Fl_Tabs(10, 370, 650, 181);
              tabsMoreData = o;
              o.labelcolor(7);
              o.labelsize(16);
              o.selection_color(4);
              {
                {
                  local o = Fl_Group(10, 400, 650, 150, _tr("Notes"));
                  tabNotes = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(10, 400, 650, 150);
                      db_products_sell_notes = o;
                      o.textsize(16);
                      o.align(5);
                      o.labeltype(FL_NO_LABEL);
                      o.labelsize(16);
                      add_input_field_to_map("products", "sell_notes", db_products_sell_notes);
                      Fl_Group.current().resizable(o);
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(10, 400, 650, 150, _tr("Notes Supplier"));
                  tabNotesSupplier = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(10, 400, 650, 150);
                      db_products_buy_notes = o;
                      o.textsize(16);
                      o.align(5);
                      o.labeltype(FL_NO_LABEL);
                      o.labelsize(16);
                      add_input_field_to_map("products", "buy_notes", db_products_buy_notes);
                      Fl_Group.current().resizable(o);
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(10, 400, 650, 150, _tr("Tags"));
                  tabTags = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(10, 400, 650, 150);
                      db_products_tags = o;
                      o.textsize(16);
                      o.align(5);
                      o.labeltype(FL_NO_LABEL);
                      o.labelsize(16);
                      add_input_field_to_map("products", "tags", db_products_tags);
                      Fl_Group.current().resizable(o);
                    }
                  }
                  o.end();
                }
                {
                  local o = Flu_Tree_Browser(10, 400, 650, 150, _tr("Groups"));
                  db_products_group_id = o;
                  o.selection_color(23);
                  o.labelsize(16);
                  add_input_field_to_map("products", "group_id", db_products_group_id);
                }
              }
              o.end();
            }
            {
              local o = Fl_Image_Box(485, 158, 300, 232);
              btnImage = o;
              o.box(FL_ENGRAVED_BOX);
              o.align(80);
              o.color(7);
            }
            {
              local o = Fl_Check_Button(370, 375, 102, 25, _tr("Wrap lines"));
              o.labelsize(16);
              o.value(1);
              o.down_box(FL_DOWN_BOX);
            }
            {
              local o = Fl_Float_Input(115, 220, 110, 25, _tr("Onhand"));
              db_products_quantity_onhand = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_onhand", db_products_quantity_onhand);
            }
          }
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(5, 60, 790, 495, _tr("Prices"));
          tabPrices = o;
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Group(15, 88, 770, 201, _tr("Sales Price"));
              o.labelsize(16);
              o.color(166);
              o.box(FL_UP_BOX);
              {
                {
                  local o = Fl_Float_Input_Fmt(25, 115, 115, 25, _tr("Buy $"));
                  db_products_buy_price = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("products", "buy_price", db_products_buy_price);
                }
                {
                  local o = Fl_Float_Input_Fmt(151, 115, 115, 25, _tr("Buy Disc. %"));
                  db_products_buy_discount = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("products", "buy_discount", db_products_buy_discount);
                  o->decimal_places(-6);
                }
                {
                  local o = Fl_Float_Input_Fmt(278, 115, 115, 25, _tr("Other Costs $"));
                  db_products_buy_other_costs = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("products", "buy_other_costs", db_products_buy_other_costs);
                }
                {
                  local o = Fl_Float_Input_Fmt(404, 115, 115, 25, _tr("Markup %"));
                  db_products_sell_markup = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("products", "sell_markup", db_products_sell_markup);
                  //o->decimal_places(-6);
                }
                {
                  local o = Fl_Float_Input_Fmt(531, 115, 115, 25, _tr("Margin %"));
                  markup_to_discount = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  //o->decimal_places(-6);
                }
                {
                  local o = Fl_Float_Input_Fmt(658, 115, 115, 25, _tr("Sales $"));
                  db_products_sell_price_ro = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                }
                {
                  local o = Fl_Int_Input(530, 150, 30, 25, _tr("Decimals for calculation"));
                  db_products_price_decimals = o;
                  o.textsize(16);
                  o.labelfont(1);
                  o.textfont(1);
                  o.labelsize(16);
                  add_input_field_to_map("products", "price_decimals", db_products_price_decimals);
                  //o->decimal_places(-6);
                }
                {
                  local o = Fl_Button(662, 150, 111, 25, _tr("Save"));
                  btnSaveProrduct = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Float_Input(665, 195, 110, 25, _tr("Stock Min."));
                  db_products_stock_min = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("products", "stock_min", db_products_stock_min);
                  o->decimal_places(-6);
                }
                {
                  local o = Fl_Float_Input(665, 225, 110, 25, _tr("Stock Max."));
                  db_products_stock_max = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("products", "stock_max", db_products_stock_max);
                  o->decimal_places(-6);
                }
                {
                  local o = Fl_Float_Input(665, 255, 110, 25, _tr("Buy Min. Qty."));
                  db_products_buy_quantity_min = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("products", "buy_quantity_min", db_products_buy_quantity_min);
                  o->decimal_places(-6);
                }
                {
                  local o = Fl_Multiline_Input(25, 197, 495, 83, _tr("Price formula"));
                  db_products_price_formula = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("products", "price_formula", db_products_price_formula);
                }
                {
                  local o = Fl_Output(25, 150, 130, 25, _tr("Last Update"));
                  db_products_price_date = o;
                  o.textsize(16);
                  o.align(8);
                  o.labelsize(16);
                }
              }
              o.end();
            }
            {
              local o = Fl_Box(15, 290, 770, 40, _tr("Spacer"));
              o.labeltype(FL_NO_LABEL);
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(15, 332, 770, 212);
              productPrices = o;
              o.box(FL_UP_BOX);
              o.labelsize(16);
              o.color(166);
              o.labeltype(FL_NO_LABEL);
              o.end();
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 59, 790, 495, _tr("Kit"));
          tabKit = o;
          o.labelsize(16);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 60, 790, 495, _tr("Statistics"));
          tabChartStatistics = o;
          o.labelsize(16);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 60, 790, 495, _tr("History"));
          tabHistory = o;
          o.labelsize(16);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 60, 790, 495, _tr("Reports"));
          tabReports = o;
          o.labelsize(16);
          o.color(246);
          o.box(FL_UP_BOX);
          {
            {
              local o = Fl_Group(15, 465, 770, 80, _tr("Export / Import"));
              grpExportImport = o;
              o.labelsize(16);
              o.box(FL_ENGRAVED_FRAME);
              {
                {
                  local o = Fl_Button(25, 475, 175, 25, _tr("Products Export"));
                  btnProductsExport = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Button(25, 510, 175, 25, _tr("Products Import"));
                  btnProductsImport = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Check_Button(225, 480, 150, 25, _tr("Withouth Prices"));
                  chkWithoutPrices = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                }
                {
                  local o = Fl_Check_Button(225, 510, 150, 25, _tr("WEB Only"));
                  chkWebOnly = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                }
                {
                  local o = Fl_Box(405, 499, 42, 25, _tr("0"));
                  boxExportImportCount = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Button(555, 475, 220, 25, _tr("Product Groups Export"));
                  btnProductGroupsExport = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Button(555, 510, 220, 25, _tr("Product Groups import"));
                  btnProductGroupsImport = o;
                  o.labelsize(16);
                }
              }
              o.end();
            }
            {
              local o = Fl_Button(610, 70, 175, 25, _tr("Update WEB"));
              btnUpdateWeb = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Group(10, 85, 195, 165, _tr("Price List"));
              grpPrintPriceList = o;
              o.labelsize(16);
              o.box(FL_ENGRAVED_FRAME);
              {
                {
                  local o = Fl_Check_Button(20, 95, 175, 25, _tr("With V.A.T."));
                  chkPriceListWithVAT = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                }
                {
                  local o = Fl_Check_Button(20, 125, 170, 25, _tr("Sell on WEB"));
                  chkPriceListSellOnWeb = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                }
                {
                  local o = Fl_Button(20, 215, 175, 25, _tr("Print"));
                  btnPrintPriceList = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Check_Button(20, 155, 175, 25, _tr("Sales"));
                  chkPriceListSellSales = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                }
                {
                  local o = Fl_Check_Button(20, 185, 175, 25, _tr("PDF"));
                  chkPriceListSellPDF = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                }
              }
              o.end();
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 60, 790, 495, _tr("Totals"));
          tabTotals = o;
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Float_Input(220, 80, 110, 25, _tr("Quantity quoted sales"));
              db_products_quantity_quoted_sales = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_quoted_sales", db_products_quantity_quoted_sales);
            }
            {
              local o = Fl_Float_Input(220, 115, 110, 25, _tr("Quantity ordered sales"));
              db_products_quantity_ordered_sales = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_ordered_sales", db_products_quantity_ordered_sales);
            }
            {
              local o = Fl_Float_Input(220, 150, 110, 25, _tr("Quantity delivered"));
              db_products_quantity_delivered = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_delivered", db_products_quantity_delivered);
            }
            {
              local o = Fl_Float_Input(220, 185, 110, 25, _tr("Quantity invoiced sales"));
              db_products_quantity_invoiced_sales = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_invoiced_sales", db_products_quantity_invoiced_sales);
            }
            {
              local o = Fl_Float_Input(220, 220, 110, 25, _tr("Quantity lost"));
              db_products_quantity_lost = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_lost", db_products_quantity_lost);
            }
            {
              local o = Fl_Float_Input(665, 85, 110, 25, _tr("Quantity quoted buys"));
              db_products_quantity_quoted_buys = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_quoted_buys", db_products_quantity_quoted_buys);
            }
            {
              local o = Fl_Float_Input(665, 120, 110, 25, _tr("Quantity ordered buys"));
              db_products_quantity_ordered_buys = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_ordered_buys", db_products_quantity_ordered_buys);
            }
            {
              local o = Fl_Float_Input(665, 155, 110, 25, _tr("Quantity received"));
              db_products_quantity_received = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_received", db_products_quantity_received);
            }
            {
              local o = Fl_Float_Input(665, 190, 110, 25, _tr("Quantity invoiced buys"));
              db_products_quantity_invoiced_buys = o;
              o.textsize(16);
              o.labelsize(16);
              o->decimal_places(-6);
              add_input_field_to_map("products", "quantity_invoiced_buys", db_products_quantity_invoiced_buys);
            }
          }
          o.end();
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
  }
}
