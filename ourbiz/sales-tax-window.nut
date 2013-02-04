class SalesTaxRatesEditWindow extends MyBaseWindow {
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
	db_map = null;
	
constructor() {
	base.constructor(-1, -1, 500, 385, _tr("Sales Tax Rates List / Edit"));
	db_map = {};
	
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
}