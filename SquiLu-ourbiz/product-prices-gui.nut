class ProductPricesGroup extends Fl_Group {
  
  // Declaration of class members
  grid = null;
  db_product_prices_quantity = null;
  db_product_prices_markup_pct = null;
  db_product_prices_discount_pct = null;
  db_product_prices_price = null;
  btnDbAction = null;
  dbAction = null;
  
  constructor(){
    base.constructor(448, 139, 760, 220, _tr("Product Prices by Quantity"));
    local _x = 448, _y = 139;
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(_x + 5, _y + 5, 750, 155, _tr("Prices by Quantity"));
      grid = o;
      o.textsize(16);
      o.align(1);
      o.labelsize(16);
      o.end();
    }
    {
      local o = Fl_Float_Input(_x + 5, _y + 185, 100, 25, _tr("Quantity"));
      db_product_prices_quantity = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      o->decimal_places(-6);
    }
    {
      local o = Fl_Float_Input(_x + 115, _y + 185, 108, 25, _tr("Markup %"));
      db_product_prices_markup_pct = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      //o->decimal_places(-6);
    }
    {
      local o = Fl_Float_Input(_x + 233, _y + 185, 108, 25, _tr("Discount %"));
      db_product_prices_discount_pct = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      //o->decimal_places(-6);
    }
    {
      local o = Fl_Float_Input(_x + 351, _y + 185, 108, 25, _tr("Price"));
      db_product_prices_price = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
    }
    {
      local o = Fl_Button(_x + 469, _y + 185, 105, 25, _tr("Save"));
      btnDbAction = o;
      o.labelsize(16);
    }
    {
      local o = Fl_Choice_dbAction(_x + 590, _y + 185, 105, 25, _tr("Action"));
      dbAction = o;
      o.textsize(16);
      o.align(1);
      o.down_box(FL_BORDER_BOX);
      o.labelsize(16);
    }
  }
}
