class EditEntityWindow extends EditWindow {
	db_entities_id=null;
	db_entities_name_ro=null;
	db_entities_mdate=null;
	db_entities_cdate=null;
	tabs=null;
	tabMain=null;
	db_entities_name=null;
	db_entities_company=null;
	db_entities_address=null;
	db_entities_zip=null;
	db_entities_city=null;
	db_entities_state=null;
	db_entities_country=null;
	db_entities_tax_number=null;
	db_entities_show_on_sales=null;
	db_entities_show_on_buys=null;
	db_entities_is_active=null;
	db_entities_phone=null;
	db_entities_fax=null;
	db_entities_use_sales_tax2=null;
	db_entities_email=null;
	db_entities_sales_tax_exempt=null;
	db_entities_web=null;
	db_entities_irpf_pct_retention=null;
	db_entities_contact=null;
	db_entities_gps_coordinates=null;
	btnDbAction=null;
	dbAction=null;
	tabNotes=null;
	db_entities_notes=null;
	tabTags=null;
	db_entities_tags=null;
	tabGroups=null;
	db_entities_group_id=null;
	tabTotals=null;
	db_entities_credit_allowed=null;
	db_entities_balance=null;
	db_entities_sales_quoted=null;
	db_entities_sales_ordered=null;
	db_entities_sales_delivered=null;
	db_entities_sales_invoiced=null;
	db_entities_sales_discount=null;
	db_entities_sales_paid=null;
	db_entities_buys_quoted=null;
	db_entities_buys_ordered=null;
	db_entities_buys_received=null;
	db_entities_buys_invoiced=null;
	db_entities_buys_paid=null;
	db_entities_buys_discount=null;
	tabChartStatistics=null;
	tabHistory=null;
	tabReports=null;
	btnEntitesListContactReport=null;
	print_progress=null;
	
	constructor(){
		base.constructor(10, 50, 800, 560, "Edit Entity");
		//clear_flag(GROUP_RELATIVE);
begin();
this->box(FL_FLAT_BOX);
this->color(FL_BACKGROUND_COLOR);
this->selection_color(FL_BACKGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(14);
this->labelcolor(FL_FOREGROUND_COLOR);
this->align(FL_ALIGN_TOP);
this->when(FL_WHEN_RELEASE);
{ db_entities_id = new Fl_Output(5, 5, 96, 25);
  db_entities_id->labeltype(FL_NO_LABEL);
  db_entities_id->labelsize(16);
  db_entities_id->textsize(16);
  add_input_field_to_map("entities", "id", db_entities_id);
} // Fl_Output* db_entities_id
{ db_entities_name_ro = new Fl_Output(110, 5, 545, 25);
  db_entities_name_ro->labeltype(FL_NO_LABEL);
  db_entities_name_ro->labelsize(16);
  db_entities_name_ro->textsize(16);
} // Fl_Output* db_entities_name_ro
{ db_entities_mdate = new Fl_Output(666, 5, 130, 25);
  db_entities_mdate->tooltip(_tr("This record last update date"));
  db_entities_mdate->labeltype(FL_NO_LABEL);
  db_entities_mdate->labelsize(16);
  db_entities_mdate->textsize(16);
  add_input_field_to_map("entities", "mdate", db_entities_mdate);
} // Fl_Output* db_entities_mdate
{ db_entities_cdate = new Fl_Output(665, 35, 130, 25);
  db_entities_cdate->tooltip(_tr("This record creation date"));
  db_entities_cdate->labeltype(FL_NO_LABEL);
  db_entities_cdate->labelsize(16);
  db_entities_cdate->textsize(16);
  add_input_field_to_map("entities", "cdate", db_entities_cdate);
} // Fl_Output* db_entities_cdate
{ tabs = new Fl_Tabs(5, 35, 790, 521);
  tabs->selection_color(4);
  tabs->labelsize(16);
  tabs->labelcolor(FL_BACKGROUND2_COLOR);
  { tabMain = new Fl_Group(5, 62, 790, 494, _tr("Main"));
    tabMain->color(246);
    tabMain->labelsize(16);
    { db_entities_name = new Fl_Input(115, 70, 670, 25, _tr("Name"));
      db_entities_name->labelsize(16);
      db_entities_name->textsize(16);
      add_input_field_to_map("entities", "name", db_entities_name);
    } // Fl_Input* db_entities_name
    { db_entities_company = new Fl_Input(115, 100, 670, 25, _tr("Company"));
      db_entities_company->labelsize(16);
      db_entities_company->textsize(16);
      add_input_field_to_map("entities", "company", db_entities_company);
    } // Fl_Input* db_entities_company
    { db_entities_address = new Fl_Input(115, 130, 670, 25, _tr("Address"));
      db_entities_address->labelsize(16);
      db_entities_address->textsize(16);
      add_input_field_to_map("entities", "address", db_entities_address);
    } // Fl_Input* db_entities_address
    { db_entities_zip = new Fl_Input(115, 160, 95, 25, _tr("ZIP"));
      db_entities_zip->labelsize(16);
      db_entities_zip->textsize(16);
      add_input_field_to_map("entities", "zip", db_entities_zip);
    } // Fl_Input* db_entities_zip
    { db_entities_city = new Fl_Input(311, 160, 474, 25, _tr("City"));
      db_entities_city->labelsize(16);
      db_entities_city->textsize(16);
      add_input_field_to_map("entities", "city", db_entities_city);
    } // Fl_Input* db_entities_city
    { db_entities_state = new Fl_Input(115, 189, 390, 25, _tr("State"));
      db_entities_state->labelsize(16);
      db_entities_state->textsize(16);
      add_input_field_to_map("entities", "state", db_entities_state);
    } // Fl_Input* db_entities_state
    { db_entities_country = new Fl_Input(585, 190, 200, 25, _tr("Country"));
      db_entities_country->labelsize(16);
      db_entities_country->textsize(16);
      add_input_field_to_map("entities", "country", db_entities_country);
    } // Fl_Input* db_entities_country
    { db_entities_tax_number = new Fl_Input(115, 220, 200, 25, _tr("Tax Num."));
      db_entities_tax_number->labelsize(16);
      db_entities_tax_number->textsize(16);
      add_input_field_to_map("entities", "tax_number", db_entities_tax_number);
    } // Fl_Input* db_entities_tax_number
    { db_entities_show_on_sales = new Fl_Check_Button(325, 220, 115, 25, _tr("Sales"));
      db_entities_show_on_sales->down_box(FL_DOWN_BOX);
      db_entities_show_on_sales->labelsize(16);
      add_input_field_to_map("entities", "show_on_sales", db_entities_show_on_sales);
    } // Fl_Check_Button* db_entities_show_on_sales
    { db_entities_show_on_buys = new Fl_Check_Button(445, 220, 125, 25, _tr("Buys"));
      db_entities_show_on_buys->down_box(FL_DOWN_BOX);
      db_entities_show_on_buys->labelsize(16);
      add_input_field_to_map("entities", "show_on_buys", db_entities_show_on_buys);
    } // Fl_Check_Button* db_entities_show_on_buys
    { db_entities_is_active = new Fl_Check_Button(585, 220, 155, 25, _tr("Active"));
      db_entities_is_active->down_box(FL_DOWN_BOX);
      db_entities_is_active->labelsize(16);
      add_input_field_to_map("entities", "is_active", db_entities_is_active);
    } // Fl_Check_Button* db_entities_is_active
    { db_entities_phone = new Fl_Input(115, 249, 200, 25, _tr("Phone"));
      db_entities_phone->labelsize(16);
      db_entities_phone->textsize(16);
      add_input_field_to_map("entities", "phone", db_entities_phone);
    } // Fl_Input* db_entities_phone
    { db_entities_fax = new Fl_Input(370, 249, 200, 25, _tr("Fax"));
      db_entities_fax->labelsize(16);
      db_entities_fax->textsize(16);
      add_input_field_to_map("entities", "fax", db_entities_fax);
    } // Fl_Input* db_entities_fax
    { db_entities_use_sales_tax2 = new Fl_Check_Button(585, 250, 155, 25, _tr("Use Sales Tax II"));
      db_entities_use_sales_tax2->down_box(FL_DOWN_BOX);
      db_entities_use_sales_tax2->labelsize(16);
      add_input_field_to_map("entities", "use_sales_tax2", db_entities_use_sales_tax2);
    } // Fl_Check_Button* db_entities_use_sales_tax2
    { db_entities_email = new Fl_Input(115, 280, 455, 25, _tr("Email"));
      db_entities_email->labelsize(16);
      db_entities_email->textsize(16);
      add_input_field_to_map("entities", "email", db_entities_email);
    } // Fl_Input* db_entities_email
    { db_entities_sales_tax_exempt = new Fl_Check_Button(585, 280, 155, 25, _tr("Sales Tax Exempt"));
      db_entities_sales_tax_exempt->down_box(FL_DOWN_BOX);
      db_entities_sales_tax_exempt->labelsize(16);
      add_input_field_to_map("entities", "sales_tax_exempt", db_entities_sales_tax_exempt);
    } // Fl_Check_Button* db_entities_sales_tax_exempt
    { db_entities_web = new Fl_Input(115, 310, 455, 25, _tr("Web"));
      db_entities_web->labelsize(16);
      db_entities_web->textsize(16);
      add_input_field_to_map("entities", "web", db_entities_web);
    } // Fl_Input* db_entities_web
    { db_entities_irpf_pct_retention = new Fl_Float_Input_Fmt(585, 310, 60, 25, _tr("I.R.P.F. %"));
      db_entities_irpf_pct_retention->type(1);
      db_entities_irpf_pct_retention->box(FL_DOWN_BOX);
      db_entities_irpf_pct_retention->color(FL_BACKGROUND2_COLOR);
      db_entities_irpf_pct_retention->selection_color(FL_SELECTION_COLOR);
      db_entities_irpf_pct_retention->labeltype(FL_NORMAL_LABEL);
      db_entities_irpf_pct_retention->labelfont(0);
      db_entities_irpf_pct_retention->labelsize(16);
      db_entities_irpf_pct_retention->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_irpf_pct_retention->textsize(16);
      db_entities_irpf_pct_retention->align(FL_ALIGN_RIGHT);
      db_entities_irpf_pct_retention->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "irpf_pct_retention", db_entities_irpf_pct_retention);
    } // Fl_Float_Input_Fmt* db_entities_irpf_pct_retention
    { db_entities_contact = new Fl_Input(115, 340, 455, 25, _tr("Contact"));
      db_entities_contact->labelsize(16);
      db_entities_contact->textsize(16);
      add_input_field_to_map("entities", "contact", db_entities_contact);
    } // Fl_Input* db_entities_contact
    { db_entities_gps_coordinates = new Fl_Input(630, 340, 155, 25, _tr("GPS"));
      db_entities_gps_coordinates->labelsize(16);
      db_entities_gps_coordinates->textsize(16);
      add_input_field_to_map("entities", "gps_coordinates", db_entities_gps_coordinates);
    } // Fl_Input* db_entities_gps_coordinates
    { local o = new Fl_Group(675, 394, 110, 151);
      { local o = new Fl_Box(675, 394, 110, 73, _tr("Spacer"));
        o->labeltype(FL_NO_LABEL);
        Fl_Group.current()->resizable(o);
      } // Fl_Box* o
      { 
        btnDbAction = new My_Fl_Return_Button(675, 470, 110, 25, _tr("Action"));
        btnDbAction->labelsize(16);
      } // Fl_Button* btnDbAction
      { 
        dbAction = new Fl_Choice_dbAction(675, 520, 110, 25);
        dbAction->label(_tr("Action"));
        dbAction->down_box(FL_BORDER_BOX);
        dbAction->labelsize(16);
        dbAction->textsize(16);
        dbAction->align(FL_ALIGN_TOP);
      } // Fl_Choice* dbAction
      o->end();
    } // Fl_Group* o
    { local o = new Fl_Tabs(15, 375, 650, 181);
      o->selection_color(4);
      o->labelsize(16);
      o->labelcolor(FL_BACKGROUND2_COLOR);
      { tabNotes = new Fl_Group(15, 405, 650, 150, _tr("Notes"));
        tabNotes->labelsize(16);
        { db_entities_notes = new Fl_Text_Editor_Buffered(15, 405, 650, 150);
          db_entities_notes->type(4);
          db_entities_notes->box(FL_DOWN_BOX);
          db_entities_notes->color(FL_BACKGROUND2_COLOR);
          db_entities_notes->selection_color(FL_SELECTION_COLOR);
          db_entities_notes->labeltype(FL_NO_LABEL);
          db_entities_notes->labelfont(0);
          db_entities_notes->labelsize(16);
          db_entities_notes->labelcolor(FL_FOREGROUND_COLOR);
          db_entities_notes->textsize(16);
          db_entities_notes->align(FL_ALIGN_TOP_LEFT);
          db_entities_notes->when(FL_WHEN_RELEASE);
          Fl_Group.current()->resizable(db_entities_notes);
          add_input_field_to_map("entities", "notes", db_entities_notes);
        } // Fl_Text_Editor_Buffered* db_entities_notes
        tabNotes->end();
      } // Fl_Group* tabNotes
      { tabTags = new Fl_Group(15, 405, 650, 150, _tr("Tags"));
        tabTags->labelsize(16);
        tabTags->hide();
        { db_entities_tags = new Fl_Text_Editor_Buffered(15, 405, 650, 150);
          db_entities_tags->type(4);
          db_entities_tags->box(FL_DOWN_BOX);
          db_entities_tags->color(FL_BACKGROUND2_COLOR);
          db_entities_tags->selection_color(FL_SELECTION_COLOR);
          db_entities_tags->labeltype(FL_NO_LABEL);
          db_entities_tags->labelfont(0);
          db_entities_tags->labelsize(16);
          db_entities_tags->labelcolor(FL_FOREGROUND_COLOR);
          db_entities_tags->textsize(16);
          db_entities_tags->align(FL_ALIGN_TOP_LEFT);
          db_entities_tags->when(FL_WHEN_RELEASE);
          Fl_Group.current()->resizable(db_entities_tags);
          add_input_field_to_map("entities", "tags", db_entities_tags);
        } // Fl_Text_Editor_Buffered* db_entities_tags
        tabTags->end();
      } // Fl_Group* tabTags
      { tabGroups = new Fl_Group(15, 400, 650, 156, _tr("Groups"));
        tabGroups->hide();
        { db_entities_group_id = new Flu_Tree_Browser(15, 405, 650, 150);
          db_entities_group_id->box(FL_DOWN_BOX);
          db_entities_group_id->color(FL_BACKGROUND2_COLOR);
          db_entities_group_id->selection_color(FL_SELECTION_COLOR);
          db_entities_group_id->labeltype(FL_NORMAL_LABEL);
          db_entities_group_id->labelfont(0);
          db_entities_group_id->labelsize(16);
          db_entities_group_id->labelcolor(FL_FOREGROUND_COLOR);
          db_entities_group_id->align(FL_ALIGN_TOP);
          db_entities_group_id->when(FL_WHEN_CHANGED);
          add_input_field_to_map("entities", "group_id", db_entities_group_id);
        } // Flu_Tree_Browser* db_entities_group_id
        tabGroups->end();
      } // Fl_Group* tabGroups
      o->end();
    } // Fl_Tabs* o
    { local o = new Fl_Check_Button(375, 380, 102, 25, _tr("Wrap lines"));
      o->down_box(FL_DOWN_BOX);
      o->value(1);
      o->labelsize(16);
      //o->callback(cb_Wrap);
    } // Fl_Check_Button* o
    tabMain->end();
    Fl_Group.current()->resizable(tabMain);
  } // Fl_Group* tabMain
  { tabTotals = new Fl_Group(5, 62, 790, 493, _tr("Totals"));
    tabTotals->color(246);
    tabTotals->labelsize(16);
    tabTotals->hide();
    { db_entities_credit_allowed = new Fl_Float_Input_Fmt(140, 75, 110, 25, _tr("Credit"));
      db_entities_credit_allowed->type(1);
      db_entities_credit_allowed->box(FL_DOWN_BOX);
      db_entities_credit_allowed->color(FL_BACKGROUND2_COLOR);
      db_entities_credit_allowed->selection_color(FL_SELECTION_COLOR);
      db_entities_credit_allowed->labeltype(FL_NORMAL_LABEL);
      db_entities_credit_allowed->labelfont(0);
      db_entities_credit_allowed->labelsize(16);
      db_entities_credit_allowed->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_credit_allowed->textsize(16);
      db_entities_credit_allowed->align(FL_ALIGN_LEFT);
      db_entities_credit_allowed->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "credit_allowed", db_entities_credit_allowed);
    } // Fl_Float_Input_Fmt* db_entities_credit_allowed
    { db_entities_balance = new Fl_Float_Input_Fmt(140, 110, 110, 25, _tr("Balance"));
      db_entities_balance->type(1);
      db_entities_balance->box(FL_DOWN_BOX);
      db_entities_balance->color(FL_BACKGROUND2_COLOR);
      db_entities_balance->selection_color(FL_SELECTION_COLOR);
      db_entities_balance->labeltype(FL_NORMAL_LABEL);
      db_entities_balance->labelfont(0);
      db_entities_balance->labelsize(16);
      db_entities_balance->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_balance->textsize(16);
      db_entities_balance->align(FL_ALIGN_LEFT);
      db_entities_balance->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "balance", db_entities_balance);
    } // Fl_Float_Input_Fmt* db_entities_balance
    { db_entities_sales_quoted = new Fl_Float_Input_Fmt(140, 155, 110, 25, _tr("Sales Quoted"));
      db_entities_sales_quoted->type(1);
      db_entities_sales_quoted->box(FL_DOWN_BOX);
      db_entities_sales_quoted->color(FL_BACKGROUND2_COLOR);
      db_entities_sales_quoted->selection_color(FL_SELECTION_COLOR);
      db_entities_sales_quoted->labeltype(FL_NORMAL_LABEL);
      db_entities_sales_quoted->labelfont(0);
      db_entities_sales_quoted->labelsize(16);
      db_entities_sales_quoted->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_sales_quoted->textsize(16);
      db_entities_sales_quoted->align(FL_ALIGN_LEFT);
      db_entities_sales_quoted->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "sales_quoted", db_entities_sales_quoted);
    } // Fl_Float_Input_Fmt* db_entities_sales_quoted
    { db_entities_sales_ordered = new Fl_Float_Input_Fmt(140, 190, 110, 25, _tr("Sales Ordered"));
      db_entities_sales_ordered->type(1);
      db_entities_sales_ordered->box(FL_DOWN_BOX);
      db_entities_sales_ordered->color(FL_BACKGROUND2_COLOR);
      db_entities_sales_ordered->selection_color(FL_SELECTION_COLOR);
      db_entities_sales_ordered->labeltype(FL_NORMAL_LABEL);
      db_entities_sales_ordered->labelfont(0);
      db_entities_sales_ordered->labelsize(16);
      db_entities_sales_ordered->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_sales_ordered->textsize(16);
      db_entities_sales_ordered->align(FL_ALIGN_LEFT);
      db_entities_sales_ordered->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "sales_ordered", db_entities_sales_ordered);
    } // Fl_Float_Input_Fmt* db_entities_sales_ordered
    { db_entities_sales_delivered = new Fl_Float_Input_Fmt(140, 225, 110, 25, _tr("Sales Delivered"));
      db_entities_sales_delivered->type(1);
      db_entities_sales_delivered->box(FL_DOWN_BOX);
      db_entities_sales_delivered->color(FL_BACKGROUND2_COLOR);
      db_entities_sales_delivered->selection_color(FL_SELECTION_COLOR);
      db_entities_sales_delivered->labeltype(FL_NORMAL_LABEL);
      db_entities_sales_delivered->labelfont(0);
      db_entities_sales_delivered->labelsize(16);
      db_entities_sales_delivered->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_sales_delivered->textsize(16);
      db_entities_sales_delivered->align(FL_ALIGN_LEFT);
      db_entities_sales_delivered->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "sales_delivered", db_entities_sales_delivered);
    } // Fl_Float_Input_Fmt* db_entities_sales_delivered
    { db_entities_sales_invoiced = new Fl_Float_Input_Fmt(140, 260, 110, 25, _tr("Sales Invoiced"));
      db_entities_sales_invoiced->type(1);
      db_entities_sales_invoiced->box(FL_DOWN_BOX);
      db_entities_sales_invoiced->color(FL_BACKGROUND2_COLOR);
      db_entities_sales_invoiced->selection_color(FL_SELECTION_COLOR);
      db_entities_sales_invoiced->labeltype(FL_NORMAL_LABEL);
      db_entities_sales_invoiced->labelfont(0);
      db_entities_sales_invoiced->labelsize(16);
      db_entities_sales_invoiced->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_sales_invoiced->textsize(16);
      db_entities_sales_invoiced->align(FL_ALIGN_LEFT);
      db_entities_sales_invoiced->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "sales_invoiced", db_entities_sales_invoiced);
    } // Fl_Float_Input_Fmt* db_entities_sales_invoiced
    { db_entities_sales_discount = new Fl_Float_Input_Fmt(140, 295, 110, 25, _tr("Sales Discount"));
      db_entities_sales_discount->type(1);
      db_entities_sales_discount->box(FL_DOWN_BOX);
      db_entities_sales_discount->color(FL_BACKGROUND2_COLOR);
      db_entities_sales_discount->selection_color(FL_SELECTION_COLOR);
      db_entities_sales_discount->labeltype(FL_NORMAL_LABEL);
      db_entities_sales_discount->labelfont(0);
      db_entities_sales_discount->labelsize(16);
      db_entities_sales_discount->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_sales_discount->textsize(16);
      db_entities_sales_discount->align(FL_ALIGN_LEFT);
      db_entities_sales_discount->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "sales_discount", db_entities_sales_discount);
    } // Fl_Float_Input_Fmt* db_entities_sales_discount
    { db_entities_sales_paid = new Fl_Float_Input_Fmt(140, 330, 110, 25, _tr("Sales Paid"));
      db_entities_sales_paid->type(1);
      db_entities_sales_paid->box(FL_DOWN_BOX);
      db_entities_sales_paid->color(FL_BACKGROUND2_COLOR);
      db_entities_sales_paid->selection_color(FL_SELECTION_COLOR);
      db_entities_sales_paid->labeltype(FL_NORMAL_LABEL);
      db_entities_sales_paid->labelfont(0);
      db_entities_sales_paid->labelsize(16);
      db_entities_sales_paid->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_sales_paid->textsize(16);
      db_entities_sales_paid->align(FL_ALIGN_LEFT);
      db_entities_sales_paid->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "sales_paid", db_entities_sales_paid);
    } // Fl_Float_Input_Fmt* db_entities_sales_paid
    { db_entities_buys_quoted = new Fl_Float_Input_Fmt(410, 155, 110, 25, _tr("Buys Quoted"));
      db_entities_buys_quoted->type(1);
      db_entities_buys_quoted->box(FL_DOWN_BOX);
      db_entities_buys_quoted->color(FL_BACKGROUND2_COLOR);
      db_entities_buys_quoted->selection_color(FL_SELECTION_COLOR);
      db_entities_buys_quoted->labeltype(FL_NORMAL_LABEL);
      db_entities_buys_quoted->labelfont(0);
      db_entities_buys_quoted->labelsize(16);
      db_entities_buys_quoted->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_buys_quoted->textsize(16);
      db_entities_buys_quoted->align(FL_ALIGN_LEFT);
      db_entities_buys_quoted->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "buys_quoted", db_entities_buys_quoted);
    } // Fl_Float_Input_Fmt* db_entities_buys_quoted
    { db_entities_buys_ordered = new Fl_Float_Input_Fmt(410, 190, 110, 25, _tr("Buys Ordered"));
      db_entities_buys_ordered->type(1);
      db_entities_buys_ordered->box(FL_DOWN_BOX);
      db_entities_buys_ordered->color(FL_BACKGROUND2_COLOR);
      db_entities_buys_ordered->selection_color(FL_SELECTION_COLOR);
      db_entities_buys_ordered->labeltype(FL_NORMAL_LABEL);
      db_entities_buys_ordered->labelfont(0);
      db_entities_buys_ordered->labelsize(16);
      db_entities_buys_ordered->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_buys_ordered->textsize(16);
      db_entities_buys_ordered->align(FL_ALIGN_LEFT);
      db_entities_buys_ordered->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "buys_ordered", db_entities_buys_ordered);
    } // Fl_Float_Input_Fmt* db_entities_buys_ordered
    { db_entities_buys_received = new Fl_Float_Input_Fmt(410, 225, 110, 25, _tr("Buys Received"));
      db_entities_buys_received->type(1);
      db_entities_buys_received->box(FL_DOWN_BOX);
      db_entities_buys_received->color(FL_BACKGROUND2_COLOR);
      db_entities_buys_received->selection_color(FL_SELECTION_COLOR);
      db_entities_buys_received->labeltype(FL_NORMAL_LABEL);
      db_entities_buys_received->labelfont(0);
      db_entities_buys_received->labelsize(16);
      db_entities_buys_received->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_buys_received->textsize(16);
      db_entities_buys_received->align(FL_ALIGN_LEFT);
      db_entities_buys_received->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "buys_received", db_entities_buys_received);
    } // Fl_Float_Input_Fmt* db_entities_buys_received
    { db_entities_buys_invoiced = new Fl_Float_Input_Fmt(410, 260, 110, 25, _tr("Buys Invoiced"));
      db_entities_buys_invoiced->type(1);
      db_entities_buys_invoiced->box(FL_DOWN_BOX);
      db_entities_buys_invoiced->color(FL_BACKGROUND2_COLOR);
      db_entities_buys_invoiced->selection_color(FL_SELECTION_COLOR);
      db_entities_buys_invoiced->labeltype(FL_NORMAL_LABEL);
      db_entities_buys_invoiced->labelfont(0);
      db_entities_buys_invoiced->labelsize(16);
      db_entities_buys_invoiced->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_buys_invoiced->textsize(16);
      db_entities_buys_invoiced->align(FL_ALIGN_LEFT);
      db_entities_buys_invoiced->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "buys_invoiced", db_entities_buys_invoiced);
    } // Fl_Float_Input_Fmt* db_entities_buys_invoiced
    { db_entities_buys_paid = new Fl_Float_Input_Fmt(410, 330, 110, 25, _tr("Buys Paid"));
      db_entities_buys_paid->type(1);
      db_entities_buys_paid->box(FL_DOWN_BOX);
      db_entities_buys_paid->color(FL_BACKGROUND2_COLOR);
      db_entities_buys_paid->selection_color(FL_SELECTION_COLOR);
      db_entities_buys_paid->labeltype(FL_NORMAL_LABEL);
      db_entities_buys_paid->labelfont(0);
      db_entities_buys_paid->labelsize(16);
      db_entities_buys_paid->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_buys_paid->textsize(16);
      db_entities_buys_paid->align(FL_ALIGN_LEFT);
      db_entities_buys_paid->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "buys_paid", db_entities_buys_paid);
    } // Fl_Float_Input_Fmt* db_entities_buys_paid
    { db_entities_buys_discount = new Fl_Float_Input_Fmt(410, 295, 110, 25, _tr("Buys Discount"));
      db_entities_buys_discount->type(1);
      db_entities_buys_discount->box(FL_DOWN_BOX);
      db_entities_buys_discount->color(FL_BACKGROUND2_COLOR);
      db_entities_buys_discount->selection_color(FL_SELECTION_COLOR);
      db_entities_buys_discount->labeltype(FL_NORMAL_LABEL);
      db_entities_buys_discount->labelfont(0);
      db_entities_buys_discount->labelsize(16);
      db_entities_buys_discount->labelcolor(FL_FOREGROUND_COLOR);
      db_entities_buys_discount->textsize(16);
      db_entities_buys_discount->align(FL_ALIGN_LEFT);
      db_entities_buys_discount->when(FL_WHEN_RELEASE);
      add_input_field_to_map("entities", "buys_discount", db_entities_buys_discount);
    } // Fl_Float_Input_Fmt* db_entities_buys_discount
    tabTotals->end();
  } // Fl_Group* tabTotals
  { tabChartStatistics = new Fl_Group(5, 62, 790, 493, _tr("Statistics"));
    tabChartStatistics->color(246);
    tabChartStatistics->labelsize(16);
    tabChartStatistics->hide();
    tabChartStatistics->end();
  } // Fl_Group* tabChartStatistics
  { tabHistory = new Fl_Group(5, 59, 790, 496, _tr("History"));
    tabHistory->color(246);
    tabHistory->labelsize(16);
    tabHistory->hide();
    tabHistory->end();
  } // Fl_Group* tabHistory
  { tabReports = new Fl_Group(5, 59, 790, 496, _tr("Reports"));
    tabReports->color(246);
    tabReports->labelsize(16);
    tabReports->hide();
    { btnEntitesListContactReport = new Fl_Button(255, 100, 215, 25, _tr("Entities List Contact Report"));
    } // Fl_Button* btnEntitesListContactReport
    { print_progress = new Fl_Progress(255, 140, 215, 25, _tr("Printing Progress"));
    } // Fl_Progress* print_progress
    tabReports->end();
  } // Fl_Group* tabReports
  tabs->end();
  Fl_Group.current()->resizable(tabs);
} // Fl_Tabs* tabs
end();
}
	
}
