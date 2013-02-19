class DeliveryCalcWindow extends MyBaseWindow {
  
  // Declaration of class members
  gui_destination_zone = null;
  gui_destination_city = null;
  gui_weight = null;
  gui_cost = null;
  gui_discount = null;
  gui_subtotal = null;
  gui_insurance = null;
  gui_increment = null;
  gui_total = null;
  gui_btnCalc = null;
  gui_btnAdd = null;
  
  constructor(){
    base.constructor(338, 167, 416, 289);
    // Create member functions and widgets
    {
      local o = Flu_Combo_List(10, 25, 395, 25, _tr("Destination Zone"));
      gui_destination_zone = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
    }
    {
      local o = Flu_Combo_List(10, 80, 395, 25, _tr("Destination City"));
      gui_destination_city = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
    }
    {
      local o = Fl_Float_Input(10, 132, 100, 25, _tr("Weight"));
      gui_weight = o;
      o.textsize(16);
      o.align(5);
      o.textfont(1);
      o.labelsize(16);
    }
    {
      local o = Fl_Output(130, 133, 130, 24, _tr("Cost"));
      gui_cost = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      o->right_to_left(1);
      o->use_numeric_format(1);
    }
    {
      local o = Fl_Output(276, 131, 130, 24, _tr("Discount"));
      gui_discount = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      o->right_to_left(1);
      o->use_numeric_format(1);
    }
    {
      local o = Fl_Output(130, 185, 130, 24, _tr("Subtotal"));
      gui_subtotal = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      o->right_to_left(1);
      o->use_numeric_format(1);
    }
    {
      local o = Fl_Output(275, 181, 130, 24, _tr("Insurance"));
      gui_insurance = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      o->right_to_left(1);
      o->use_numeric_format(1);
    }
    {
      local o = Fl_Output(276, 219, 130, 24, _tr("Increment"));
      gui_increment = o;
      o.textsize(16);
      o.labelsize(16);
      o->right_to_left(1);
      o->use_numeric_format(1);
    }
    {
      local o = Fl_Output(276, 253, 130, 24, _tr("Total"));
      gui_total = o;
      o.textsize(16);
      o.textfont(1);
      o.labelsize(16);
      o->right_to_left(1);
      o->use_numeric_format(1);
    }
    {
      local o = Fl_Button(10, 217, 100, 25, _tr("Calc"));
      gui_btnCalc = o;
      o.labelsize(16);
      o.tooltip(_tr("Calculate the delivery price"));
    }
    {
      local o = Fl_Button(10, 252, 100, 25, _tr("Add"));
      gui_btnAdd = o;
      o.labelsize(16);
      o.tooltip(_tr("Add the calculated value on the invoice line"));
    }
  }
}

