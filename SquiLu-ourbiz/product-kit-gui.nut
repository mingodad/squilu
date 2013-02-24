class ProductKitGroup extends Fl_Group {
  
// Declaration of class members
  grid = null;
  kit_group_search_edit = null;
  grpSearchEdit = null;
  db_product_kits_product_id = null;
  db_product_kits_sell_description = null;
  btnSearchProduct = null;
  db_product_kits_quantity = null;
  db_product_kits_product_price = null;
  btnDbAction = null;
  dbAction = null;
  btnKitPartOf = null;
  kit_group_totals = null;
  kit_sales_price = null;
  kit_prices_sum = null;
  kit_parts = null;
  kit_markup = null;
  
  constructor(){
    base.constructor(24, 106, 775, 470, _tr("ProductKitGroup"));
    local _x = 24, _y = 106;
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(_x + 4, _y + 1, 770, 328);
      grid = o;
      o.textsize(16);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(_x + 0, _y + 355, 540, 115, _tr("Search / Edit"));
      kit_group_search_edit = o;
      o.labelsize(16);
      o.color(166);
      o.box(FL_UP_BOX);
      {
        {
          local o = Fl_Group(_x + 10, _y + 365, 525, 93);
          grpSearchEdit = o;
          {
            {
              local o = Fl_Int_Input(_x + 12, _y + 382, 87, 25, _tr("Code"));
              db_product_kits_product_id = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Input(_x + 109, _y + 382, 426, 25, _tr("Description"));
              db_product_kits_sell_description = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Button(_x + 445, _y + 433, 90, 25, _tr("Search"));
              btnSearchProduct = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Float_Input(_x + 10, _y + 433, 89, 25, _tr("Quantity"));
              db_product_kits_quantity = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Float_Input(_x + 109, _y + 433, 108, 25, _tr("Price"));
              db_product_kits_product_price = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Button(_x + 225, _y + 433, 100, 25, _tr("Save"));
              btnDbAction = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Choice_dbAction(_x + 335, _y + 433, 100, 25, _tr("Action"));
              dbAction = o;
              o.textsize(16);
              o.align(1);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Check_Button(_x + 430, _y + 360, 100, 25, _tr("Part of"));
          btnKitPartOf = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
        }
      }
      o.end();
    }
    {
      local o = Fl_Group(_x + 544, _y + 355, 230, 115, _tr("Totals"));
      kit_group_totals = o;
      o.labelsize(16);
      o.color(166);
      o.box(FL_UP_BOX);
      {
        {
          local o = Fl_Float_Input(_x + 664, _y + 433, 100, 25, _tr("Price"));
          kit_sales_price = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
        }
        {
          local o = Fl_Float_Input(_x + 664, _y + 380, 100, 25, _tr("Prices Sum"));
          kit_prices_sum = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
        }
        {
          local o = Fl_Int_Input(_x + 554, _y + 380, 100, 25, _tr("Parts"));
          kit_parts = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
        }
        {
          local o = Fl_Float_Input(_x + 554, _y + 433, 100, 25, _tr("Markup %"));
          kit_markup = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
        }
      }
      o.end();
    }
  }
}
