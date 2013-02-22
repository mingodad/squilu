class EditEntityWindow extends EditWindow {

  // Declaration of class members
  db_entities_id = null;
  db_entities_name_ro = null;
  db_entities_mdate = null;
  db_entities_cdate = null;
  tabs = null;
  tabMain = null;
  db_entities_name = null;
  db_entities_company = null;
  db_entities_address = null;
  db_entities_zip = null;
  db_entities_city = null;
  db_entities_state = null;
  db_entities_country = null;
  db_entities_tax_number = null;
  db_entities_show_on_sales = null;
  db_entities_show_on_buys = null;
  db_entities_is_active = null;
  db_entities_phone = null;
  db_entities_fax = null;
  db_entities_use_sales_tax2 = null;
  db_entities_email = null;
  db_entities_sales_tax_exempt = null;
  db_entities_web = null;
  db_entities_irpf_pct_retention = null;
  db_entities_contact = null;
  db_entities_gps_coordinates = null;
  btnDbAction = null;
  dbAction = null;
  tabNotes = null;
  db_entities_notes = null;
  tabTags = null;
  db_entities_tags = null;
  tabGroups = null;
  db_entities_group_id = null;
  tabTotals = null;
  db_entities_credit_allowed = null;
  db_entities_balance = null;
  db_entities_sales_quoted = null;
  db_entities_sales_ordered = null;
  db_entities_sales_delivered = null;
  db_entities_sales_invoiced = null;
  db_entities_sales_discount = null;
  db_entities_sales_paid = null;
  db_entities_buys_quoted = null;
  db_entities_buys_ordered = null;
  db_entities_buys_received = null;
  db_entities_buys_invoiced = null;
  db_entities_buys_paid = null;
  db_entities_buys_discount = null;
  tabChartStatistics = null;
  tabHistory = null;
  tabReports = null;
  btnEntitesListContactReport = null;
  print_progress = null;
  
  constructor(px=54, py=85, pw=800, ph=560, pl=_tr("Edit Entity")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Output(5, 5, 96, 25);
      db_entities_id = o;
      o.textsize(16);
      o.labelsize(16);
      o.labeltype(FL_NO_LABEL);
      add_input_field_to_map("entities", "id", db_entities_id);
    }
    {
      local o = Fl_Output(110, 5, 545, 25);
      db_entities_name_ro = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
    }
    {
      local o = Fl_Output(666, 5, 130, 25);
      db_entities_mdate = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.tooltip(_tr("This record last update date"));
      o.labelsize(16);
      add_input_field_to_map("entities", "mdate", db_entities_mdate);
    }
    {
      local o = Fl_Output(665, 35, 130, 25);
      db_entities_cdate = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.tooltip(_tr("This record creation date"));
      o.labelsize(16);
      add_input_field_to_map("entities", "cdate", db_entities_cdate);
    }
    {
      local o = Fl_Tabs(5, 35, 790, 521);
      tabs = o;
      o.labelcolor(7);
      o.selection_color(4);
      o.labelsize(16);
      {
        {
          local o = Fl_Group(5, 62, 790, 494, _tr("Main"));
          tabMain = o;
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Input(115, 70, 670, 25, _tr("Name"));
              db_entities_name = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "name", db_entities_name);
            }
            {
              local o = Fl_Input(115, 100, 670, 25, _tr("Company"));
              db_entities_company = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "company", db_entities_company);
            }
            {
              local o = Fl_Input(115, 130, 670, 25, _tr("Address"));
              db_entities_address = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "address", db_entities_address);
            }
            {
              local o = Fl_Input(115, 160, 95, 25, _tr("ZIP"));
              db_entities_zip = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "zip", db_entities_zip);
            }
            {
              local o = Fl_Input(311, 160, 474, 25, _tr("City"));
              db_entities_city = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "city", db_entities_city);
            }
            {
              local o = Fl_Input(115, 189, 390, 25, _tr("State"));
              db_entities_state = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "state", db_entities_state);
            }
            {
              local o = Fl_Input(585, 190, 200, 25, _tr("Country"));
              db_entities_country = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "country", db_entities_country);
            }
            {
              local o = Fl_Input(115, 220, 200, 25, _tr("Tax Num."));
              db_entities_tax_number = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "tax_number", db_entities_tax_number);
            }
            {
              local o = Fl_Check_Button(325, 220, 115, 25, _tr("Sales"));
              db_entities_show_on_sales = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("entities", "show_on_sales", db_entities_show_on_sales);
            }
            {
              local o = Fl_Check_Button(445, 220, 125, 25, _tr("Buys"));
              db_entities_show_on_buys = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("entities", "show_on_buys", db_entities_show_on_buys);
            }
            {
              local o = Fl_Check_Button(585, 220, 155, 25, _tr("Active"));
              db_entities_is_active = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("entities", "is_active", db_entities_is_active);
            }
            {
              local o = Fl_Input(115, 249, 200, 25, _tr("Phone"));
              db_entities_phone = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "phone", db_entities_phone);
            }
            {
              local o = Fl_Input(370, 249, 200, 25, _tr("Fax"));
              db_entities_fax = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "fax", db_entities_fax);
            }
            {
              local o = Fl_Check_Button(585, 250, 155, 25, _tr("Use Sales Tax II"));
              db_entities_use_sales_tax2 = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("entities", "use_sales_tax2", db_entities_use_sales_tax2);
            }
            {
              local o = Fl_Input(115, 280, 455, 25, _tr("Email"));
              db_entities_email = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "email", db_entities_email);
            }
            {
              local o = Fl_Check_Button(585, 280, 155, 25, _tr("Sales Tax Exempt"));
              db_entities_sales_tax_exempt = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("entities", "sales_tax_exempt", db_entities_sales_tax_exempt);
            }
            {
              local o = Fl_Input(115, 310, 455, 25, _tr("Web"));
              db_entities_web = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "web", db_entities_web);
            }
            {
              local o = Fl_Float_Input_Fmt(585, 310, 60, 25, _tr("I.R.P.F. %"));
              db_entities_irpf_pct_retention = o;
              o.textsize(16);
              o.align(8);
              o.labelsize(16);
              add_input_field_to_map("entities", "irpf_pct_retention", db_entities_irpf_pct_retention);
            }
            {
              local o = Fl_Input(115, 340, 455, 25, _tr("Contact"));
              db_entities_contact = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "contact", db_entities_contact);
            }
            {
              local o = Fl_Input(630, 340, 155, 25, _tr("GPS"));
              db_entities_gps_coordinates = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "gps_coordinates", db_entities_gps_coordinates);
            }
            {
              local o = Fl_Group(675, 394, 110, 151);
              {
                {
                  local o = Fl_Box(675, 394, 110, 73, _tr("Spacer"));
                  o.labeltype(FL_NO_LABEL);
                  Fl_Group.current().resizable(o);
                }
                {
                  local o = Fl_Button(675, 470, 110, 25, _tr("Action"));
                  btnDbAction = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Choice_dbAction(675, 520, 110, 25, _tr("Action"));
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
              local o = Fl_Tabs(15, 375, 650, 181);
              o.labelcolor(7);
              o.selection_color(4);
              o.labelsize(16);
              {
                {
                  local o = Fl_Group(15, 405, 650, 150, _tr("Notes"));
                  tabNotes = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(15, 405, 650, 150);
                      db_entities_notes = o;
                      o.textsize(16);
                      o.align(5);
                      o.labeltype(FL_NO_LABEL);
                      o.labelsize(16);
                      add_input_field_to_map("entities", "notes", db_entities_notes);
                      Fl_Group.current().resizable(o);
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(15, 405, 650, 150, _tr("Tags"));
                  tabTags = o;
                  o.labelsize(16);
                  {
                    {
                      local o = Fl_Text_Editor_Buffered(15, 405, 650, 150);
                      db_entities_tags = o;
                      o.textsize(16);
                      o.align(5);
                      o.labeltype(FL_NO_LABEL);
                      o.labelsize(16);
                      add_input_field_to_map("entities", "tags", db_entities_tags);
                      Fl_Group.current().resizable(o);
                    }
                  }
                  o.end();
                }
                {
                  local o = Fl_Group(15, 400, 650, 156, _tr("Groups"));
                  tabGroups = o;
                  {
                    {
                      local o = Flu_Tree_Browser(15, 405, 650, 150);
                      db_entities_group_id = o;
                      o.labelsize(16);
                      add_input_field_to_map("entities", "group_id", db_entities_group_id);
                    }
                  }
                  o.end();
                }
              }
              o.end();
            }
            {
              local o = Fl_Check_Button(375, 380, 102, 25, _tr("Wrap lines"));
              o.labelsize(16);
              o.value(1);
              o.down_box(FL_DOWN_BOX);
            }
          }
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(5, 62, 790, 493, _tr("Totals"));
          tabTotals = o;
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Float_Input_Fmt(140, 75, 110, 25, _tr("Credit"));
              db_entities_credit_allowed = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "credit_allowed", db_entities_credit_allowed);
            }
            {
              local o = Fl_Float_Input_Fmt(140, 110, 110, 25, _tr("Balance"));
              db_entities_balance = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "balance", db_entities_balance);
            }
            {
              local o = Fl_Float_Input_Fmt(140, 155, 110, 25, _tr("Sales Quoted"));
              db_entities_sales_quoted = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "sales_quoted", db_entities_sales_quoted);
            }
            {
              local o = Fl_Float_Input_Fmt(140, 190, 110, 25, _tr("Sales Ordered"));
              db_entities_sales_ordered = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "sales_ordered", db_entities_sales_ordered);
            }
            {
              local o = Fl_Float_Input_Fmt(140, 225, 110, 25, _tr("Sales Delivered"));
              db_entities_sales_delivered = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "sales_delivered", db_entities_sales_delivered);
            }
            {
              local o = Fl_Float_Input_Fmt(140, 260, 110, 25, _tr("Sales Invoiced"));
              db_entities_sales_invoiced = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "sales_invoiced", db_entities_sales_invoiced);
            }
            {
              local o = Fl_Float_Input_Fmt(140, 295, 110, 25, _tr("Sales Discount"));
              db_entities_sales_discount = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "sales_discount", db_entities_sales_discount);
            }
            {
              local o = Fl_Float_Input_Fmt(140, 330, 110, 25, _tr("Sales Paid"));
              db_entities_sales_paid = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "sales_paid", db_entities_sales_paid);
            }
            {
              local o = Fl_Float_Input_Fmt(410, 155, 110, 25, _tr("Buys Quoted"));
              db_entities_buys_quoted = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "buys_quoted", db_entities_buys_quoted);
            }
            {
              local o = Fl_Float_Input_Fmt(410, 190, 110, 25, _tr("Buys Ordered"));
              db_entities_buys_ordered = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "buys_ordered", db_entities_buys_ordered);
            }
            {
              local o = Fl_Float_Input_Fmt(410, 225, 110, 25, _tr("Buys Received"));
              db_entities_buys_received = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "buys_received", db_entities_buys_received);
            }
            {
              local o = Fl_Float_Input_Fmt(410, 260, 110, 25, _tr("Buys Invoiced"));
              db_entities_buys_invoiced = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "buys_invoiced", db_entities_buys_invoiced);
            }
            {
              local o = Fl_Float_Input_Fmt(410, 330, 110, 25, _tr("Buys Paid"));
              db_entities_buys_paid = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "buys_paid", db_entities_buys_paid);
            }
            {
              local o = Fl_Float_Input_Fmt(410, 295, 110, 25, _tr("Buys Discount"));
              db_entities_buys_discount = o;
              o.textsize(16);
              o.labelsize(16);
              add_input_field_to_map("entities", "buys_discount", db_entities_buys_discount);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 62, 790, 493, _tr("Statistics"));
          tabChartStatistics = o;
          o.labelsize(16);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 59, 790, 496, _tr("History"));
          tabHistory = o;
          o.labelsize(16);
          o.color(246);
          o.end();
        }
        {
          local o = Fl_Group(5, 59, 790, 496, _tr("Reports"));
          tabReports = o;
          o.labelsize(16);
          o.color(246);
          {
            {
              local o = Fl_Button(255, 100, 215, 25, _tr("Entities List Contact Report"));
              btnEntitesListContactReport = o;
            }
            {
              local o = Fl_Progress(255, 140, 215, 25, _tr("Printing Progress"));
              print_progress = o;
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
