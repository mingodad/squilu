class OrderPageGroup extends Fl_Scroll {
  
  // Declaration of class members
  page = null;
  pageDesignGroup = null;
  logoGroupH = null;
  logoH = null;
  companyBoxH = null;
  logoGroupV = null;
  logoV = null;
  companyBoxV = null;
  logoGroupHV = null;
  logoHV1 = null;
  logoHV2 = null;
  companyBoxHV = null;
  OrderNumber = null;
  box_header_number_date = null;
  box_data_number_date = null;
  header_date_box = null;
  box_data_date_box = null;
  box_to = null;
  orderLines = null;
  header_lines_code_box = null;
  header_lines_description_box = null;
  header_lines_quantity_box = null;
  header_lines_price_box = null;
  header_lines_discount_box = null;
  header_lines_sales_tax1_box = null;
  header_lines_sales_tax2_box = null;
  header_lines_subtotal_box = null;
  designLinesGroup = null;
  data_lines_code_box = null;
  data_lines_description_box = null;
  data_lines_quantity_box = null;
  data_tlines_price_box = null;
  data_lines_discount_box = null;
  data_lines_sales_tax1_box = null;
  data_lines_sales_tax2_box = null;
  data_lines_subtotal_box = null;
  orderTotals = null;
  header_PAGENO_box = null;
  box_data_pageno_box = null;
  header_totals_subtotal_box = null;
  data_totals_subtotal_box = null;
  header_totals_total_sales_tax1_box = null;
  data_totals_total_sales_tax1_box = null;
  header_totals_total_sales_tax2_box = null;
  data_totals_total_sales_tax2_box = null;
  header_totals_total_box = null;
  data_totals_total_box = null;
  notes_box = null;
  printDataGroup = null;
  orderTypeBox = null;
  numberBox = null;
  dateBox = null;
  entityBox = null;
  dataLinesGroup = null;
  codeBox = null;
  descriptionBox = null;
  quantityBox = null;
  priceBox = null;
  discountBox = null;
  sales_tax1Box = null;
  sales_tax2Box = null;
  lineTotalBox = null;
  bottomTotals = null;
  orderPageBox = null;
  fisrtTotalBox = null;
  sales_tax1AmountBox = null;
  sales_tax2AmountBox = null;
  totalBox = null;
  
  constructor(px=213, py=29, pw=630, ph=685, pl=null){
    base.constructor(px,py,pw,ph,pl);
    local _x = 213, _y = 39;
    // Create member functions and widgets
    {
      local o = Fl_Group(_x + 0, _y + 0, 634, 686);
      page = o;
      o.color(7);
      {
        {
          local o = Fl_Group(_x + 2, _y + 0, 630, 686);
          pageDesignGroup = o;
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Group(_x + 15, _y + 10, 316, 130);
              logoGroupH = o;
              {
                {
                  local o = Fl_Box(_x + 16, _y + 10, 310, 50, _tr("FLTK"));
                  logoH = o;
                  o.align(660);
                  o.labelfont(3);
                  o.labeltype(FL_SHADOW_LABEL);
                  o.labelsize(40);
                }
                {
                  local o = Fl_Box(_x + 16, _y + 65, 308, 75);
                  companyBoxH = o;
                  o.labelfont(2);
                  o.align(144);
                  o.labelsize(10);
                }
              }
              o.end();
            }
            {
              local o = Fl_Group(_x + 15, _y + 10, 316, 130);
              logoGroupV = o;
              {
                {
                  local o = Fl_Box(_x + 16, _y + 10, 100, 130, _tr("FLTK"));
                  logoV = o;
                  o.align(660);
                  o.labelfont(3);
                  o.labeltype(FL_SHADOW_LABEL);
                  o.labelsize(40);
                }
                {
                  local o = Fl_Box(_x + 120, _y + 10, 210, 130);
                  companyBoxV = o;
                  o.labelfont(2);
                  o.align(144);
                  o.labelsize(10);
                }
              }
              o.end();
            }
            {
              local o = Fl_Group(_x + 15, _y + 10, 316, 130);
              logoGroupHV = o;
              {
                {
                  local o = Fl_Box(_x + 16, _y + 10, 100, 130, _tr("FLTK"));
                  logoHV1 = o;
                  o.align(660);
                  o.labelfont(3);
                  o.labeltype(FL_SHADOW_LABEL);
                  o.labelsize(40);
                }
                {
                  local o = Fl_Box(_x + 120, _y + 10, 210, 50, _tr("FLTK"));
                  logoHV2 = o;
                  o.align(660);
                  o.labelfont(3);
                  o.labeltype(FL_SHADOW_LABEL);
                  o.labelsize(40);
                }
                {
                  local o = Fl_Box(_x + 120, _y + 50, 210, 90);
                  companyBoxHV = o;
                  o.labelfont(2);
                  o.align(144);
                  o.labelsize(9);
                }
              }
              o.end();
            }
            {
              local o = Fl_Group(_x + 336, _y + 10, 286, 40);
              OrderNumber = o;
              o.box(FL_FLAT_BOX);
              o.color(39);
              o.labeltype(FL_NO_LABEL);
              {
                {
                  local o = Fl_Box(_x + 337, _y + 11, 142, 18);
                  box_header_number_date = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labeltype(FL_NO_LABEL);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 337, _y + 30, 142, 19);
                  box_data_number_date = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labeltype(FL_NO_LABEL);
                  o.color(7);
                }
                {
                  local o = Fl_Box(_x + 480, _y + 11, 141, 18, _tr("DATE"));
                  header_date_box = o;
                  o.labelsize(12);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 480, _y + 30, 141, 19);
                  box_data_date_box = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labelfont(1);
                  o.labeltype(FL_NO_LABEL);
                  o.color(7);
                }
              }
              o.end();
            }
            {
              local o = Fl_Box(_x + 336, _y + 55, 286, 85);
              box_to = o;
              o.box(FL_BORDER_FRAME);
              o.align(21);
              o.labelfont(1);
              o.labelsize(16);
              o.color(39);
            }
            {
              local o = Fl_Group(_x + 16, _y + 148, 606, 433);
              orderLines = o;
              o.box(FL_FLAT_BOX);
              o.color(39);
              o.labeltype(FL_NO_LABEL);
              {
                {
                  local o = Fl_Box(_x + 17, _y + 149, 46, 20, _tr("CODE"));
                  header_lines_code_box = o;
                  o.labelsize(10);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 64, _y + 149, 300, 20, _tr("DESCRIPTION"));
                  header_lines_description_box = o;
                  o.labelsize(10);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 365, _y + 149, 40, 20, _tr("QTY."));
                  header_lines_quantity_box = o;
                  o.labelsize(10);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 406, _y + 149, 60, 20, _tr("PRICE"));
                  header_lines_price_box = o;
                  o.labelsize(10);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 467, _y + 149, 30, 20, _tr("DISC. %"));
                  header_lines_discount_box = o;
                  o.align(128);
                  o.labelsize(8);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 498, _y + 149, 30, 20, _tr("S.T. I %"));
                  header_lines_sales_tax1_box = o;
                  o.align(128);
                  o.box(FL_FLAT_BOX);
                  o.labelsize(8);
                  o.color(47);
                  o.labelfont(1);
                }
                {
                  local o = Fl_Box(_x + 529, _y + 149, 30, 20, _tr("S.T. II %"));
                  header_lines_sales_tax2_box = o;
                  o.align(128);
                  o.box(FL_FLAT_BOX);
                  o.labelsize(8);
                  o.color(47);
                  o.labelfont(1);
                }
                {
                  local o = Fl_Box(_x + 560, _y + 149, 61, 20, _tr("SUBTOTAL"));
                  header_lines_subtotal_box = o;
                  o.labelsize(10);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Group(_x + 17, _y + 170, 604, 410);
                  designLinesGroup = o;
                  o.labeltype(FL_NO_LABEL);
                  {
                    {
                      local o = Fl_Box(_x + 17, _y + 170, 46, 410);
                      data_lines_code_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                    {
                      local o = Fl_Box(_x + 64, _y + 170, 300, 410);
                      data_lines_description_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                    {
                      local o = Fl_Box(_x + 365, _y + 170, 40, 410);
                      data_lines_quantity_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                    {
                      local o = Fl_Box(_x + 406, _y + 170, 60, 410);
                      data_tlines_price_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                    {
                      local o = Fl_Box(_x + 467, _y + 170, 30, 410);
                      data_lines_discount_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                    {
                      local o = Fl_Box(_x + 498, _y + 170, 30, 410);
                      data_lines_sales_tax1_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                    {
                      local o = Fl_Box(_x + 529, _y + 170, 30, 410);
                      data_lines_sales_tax2_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                    {
                      local o = Fl_Box(_x + 560, _y + 170, 61, 410);
                      data_lines_subtotal_box = o;
                      o.box(FL_FLAT_BOX);
                      o.labelsize(10);
                      o.labelfont(1);
                      o.labeltype(FL_NO_LABEL);
                      o.color(7);
                    }
                  }
                  o.end();
                  Fl_Group.current().resizable(o);
                }
              }
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(_x + 16, _y + 585, 606, 46);
              orderTotals = o;
              o.box(FL_FLAT_BOX);
              o.color(39);
              o.labeltype(FL_NO_LABEL);
              {
                {
                  local o = Fl_Box(_x + 17, _y + 586, 65, 20, _tr("PAGE"));
                  header_PAGENO_box = o;
                  o.labelsize(12);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 17, _y + 607, 65, 23);
                  box_data_pageno_box = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labelfont(1);
                  o.labeltype(FL_NO_LABEL);
                  o.color(7);
                }
                {
                  local o = Fl_Box(_x + 83, _y + 586, 150, 20, _tr("SUBTOTAL"));
                  header_totals_subtotal_box = o;
                  o.labelsize(12);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 83, _y + 607, 150, 23);
                  data_totals_subtotal_box = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labelfont(1);
                  o.labeltype(FL_NO_LABEL);
                  o.color(7);
                }
                {
                  local o = Fl_Box(_x + 234, _y + 586, 120, 20, _tr("Sales Tax I"));
                  header_totals_total_sales_tax1_box = o;
                  o.labelsize(12);
                  o.labelfont(1);
                  o.color(47);
                  o.box(FL_FLAT_BOX);
                }
                {
                  local o = Fl_Box(_x + 234, _y + 607, 120, 23);
                  data_totals_total_sales_tax1_box = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labelfont(1);
                  o.labeltype(FL_NO_LABEL);
                  o.color(7);
                }
                {
                  local o = Fl_Box(_x + 355, _y + 586, 115, 20, _tr("Sales Tax II"));
                  header_totals_total_sales_tax2_box = o;
                  o.labelsize(12);
                  o.labelfont(1);
                  o.color(47);
                  o.box(FL_FLAT_BOX);
                }
                {
                  local o = Fl_Box(_x + 355, _y + 607, 115, 23);
                  data_totals_total_sales_tax2_box = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labelfont(1);
                  o.labeltype(FL_NO_LABEL);
                  o.color(7);
                }
                {
                  local o = Fl_Box(_x + 471, _y + 586, 150, 20, _tr("TOTAL"));
                  header_totals_total_box = o;
                  o.labelsize(12);
                  o.labelfont(1);
                  o.box(FL_FLAT_BOX);
                  o.color(47);
                }
                {
                  local o = Fl_Box(_x + 471, _y + 607, 150, 23);
                  data_totals_total_box = o;
                  o.box(FL_FLAT_BOX);
                  o.labelsize(12);
                  o.labelfont(1);
                  o.labeltype(FL_NO_LABEL);
                  o.color(7);
                }
              }
              o.end();
            }
            {
              local o = Fl_Box(_x + 17, _y + 638, 606, 37);
              notes_box = o;
              o.labelfont(2);
              o.align(144);
              o.labelsize(9);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(_x + 3, _y + 0, 628, 637);
          printDataGroup = o;
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Box(_x + 338, _y + 10, 139, 20);
              orderTypeBox = o;
              o.labelfont(1);
              o.labelsize(12);
            }
            {
              local o = Fl_Box(_x + 338, _y + 33, 139, 16);
              numberBox = o;
              o.labelfont(5);
              o.labelsize(15);
              o.align(16);
            }
            {
              local o = Fl_Box(_x + 484, _y + 32, 133, 16);
              dateBox = o;
              o.labelfont(5);
              o.labelsize(15);
              o.align(16);
            }
            {
              local o = Fl_Box(_x + 338, _y + 57, 282, 82);
              entityBox = o;
              o.align(85);
              o.labelsize(12);
            }
            {
              local o = Fl_Group(_x + 16, _y + 169, 605, 412);
              dataLinesGroup = o;
              o.align(16);
              o.labeltype(FL_NO_LABEL);
              {
                {
                  local o = Fl_Box(_x + 16, _y + 174, 46, 404);
                  codeBox = o;
                  o.align(25);
                  o.labelsize(11);
                }
                {
                  local o = Fl_Box(_x + 64, _y + 174, 300, 404);
                  descriptionBox = o;
                  o.align(85);
                  o.labelsize(11);
                }
                {
                  local o = Fl_Box(_x + 365, _y + 174, 40, 404);
                  quantityBox = o;
                  o.align(25);
                  o.labelsize(11);
                }
                {
                  local o = Fl_Box(_x + 406, _y + 174, 60, 404);
                  priceBox = o;
                  o.align(25);
                  o.labelsize(11);
                }
                {
                  local o = Fl_Box(_x + 467, _y + 174, 30, 404);
                  discountBox = o;
                  o.align(25);
                  o.labelsize(11);
                }
                {
                  local o = Fl_Box(_x + 498, _y + 174, 30, 404);
                  sales_tax1Box = o;
                  o.align(25);
                  o.labelsize(11);
                }
                {
                  local o = Fl_Box(_x + 529, _y + 174, 30, 404);
                  sales_tax2Box = o;
                  o.align(25);
                  o.labelsize(11);
                }
                {
                  local o = Fl_Box(_x + 560, _y + 174, 61, 404);
                  lineTotalBox = o;
                  o.align(25);
                  o.labelsize(11);
                }
              }
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(_x + 16, _y + 585, 605, 48);
              bottomTotals = o;
              o.labeltype(FL_NO_LABEL);
              {
                {
                  local o = Fl_Box(_x + 16, _y + 608, 65, 25);
                  orderPageBox = o;
                  o.labelfont(5);
                  o.labelsize(15);
                }
                {
                  local o = Fl_Box(_x + 83, _y + 608, 150, 25);
                  fisrtTotalBox = o;
                  o.labelfont(5);
                  o.labelsize(15);
                }
                {
                  local o = Fl_Box(_x + 234, _y + 608, 120, 25);
                  sales_tax1AmountBox = o;
                  o.labelfont(5);
                  o.labelsize(15);
                }
                {
                  local o = Fl_Box(_x + 355, _y + 608, 115, 25);
                  sales_tax2AmountBox = o;
                  o.labelfont(5);
                  o.labelsize(15);
                }
                {
                  local o = Fl_Box(_x + 500, _y + 608, 121, 25);
                  totalBox = o;
                  o.labelfont(5);
                  o.labelsize(15);
                }
              }
              o.end();
            }
          }
          o.end();
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
    }
  }
}
