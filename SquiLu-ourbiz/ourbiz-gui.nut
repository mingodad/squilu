class HelpWindow extends Base_Window {
  
  // Declaration of class members
  tabs = null;
  tabView = null;
  view_html = null;
  tabEdit = null;
  help_text = null;
  words_to_search = null;
  btnSearchHelp = null;
  btnSavehelp = null;
  btnBackward = null;
  btnTop = null;
  btnForward = null;
  btnFontSmaller = null;
  btnFontBigger = null;
  
  constructor(px=76, py=128, pw=600, ph=400, pl=_tr("Help")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Tabs(5, 5, 590, 355);
      tabs = o;
      o.labelsize(16);
      {
        {
          local o = Fl_Group(5, 30, 590, 330, _tr("View"));
          tabView = o;
          {
            {
              local o = Fl_Help_View(5, 30, 590, 330);
              view_html = o;
              o.labelsize(16);
              o.end();
            }
          }
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(5, 30, 590, 310, _tr("Edit"));
          tabEdit = o;
          {
            {
              local o = Fl_Text_Editor_Buffered(5, 30, 590, 310);
              help_text = o;
              o.textsize(18);
              o.labelsize(18);
              Fl_Group.current().resizable(o);
            }
          }
          o.end();
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 370, 590, 25);
      {
        {
          local o = Fl_Input(34, 370, 225, 25, _tr("@search"));
          words_to_search = o;
          o.textsize(18);
          o.labelsize(18);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Button(265, 370, 80, 25, _tr("Search"));
          btnSearchHelp = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(350, 370, 80, 25, _tr("Save"));
          btnSavehelp = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(435, 370, 25, 25, _tr("@<-"));
          btnBackward = o;
          o.labelcolor(63);
        }
        {
          local o = Fl_Button(470, 370, 25, 25, _tr("@8->"));
          btnTop = o;
          o.labelcolor(63);
        }
        {
          local o = Fl_Button(503, 370, 25, 25, _tr("@->"));
          btnForward = o;
          o.labelcolor(63);
        }
        {
          local o = Fl_Button(536, 370, 25, 25, _tr("F"));
          btnFontSmaller = o;
          o.labelfont(1);
          o.labelsize(10);
        }
        {
          local o = Fl_Button(569, 370, 25, 25, _tr("F"));
          btnFontBigger = o;
          o.labelfont(1);
        }
      }
      o.end();
    }
    end();
  }
}

class CalendarWindow extends Base_Window {
  
  // Declaration of class members
  label_month = null;
  btnMonth1 = null;
  btnMonth2 = null;
  btnMonth3 = null;
  btnMonth4 = null;
  btnMonth5 = null;
  btnMonth6 = null;
  btnMonth7 = null;
  btnMonth8 = null;
  btnMonth9 = null;
  btnMonth10 = null;
  btnMonth11 = null;
  btnMonth12 = null;
  grid = null;
  btnPrevYear = null;
  btnPrevMonth = null;
  btnToday = null;
  btnSelect = null;
  btnNextMonth = null;
  btnNextYear = null;
  
  constructor(px=732, py=184, pw=350, ph=305, pl=_tr("Calendar")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Box(5, 5, 340, 25, _tr("Calendar"));
      label_month = o;
      o.labelsize(16);
      o.box(FL_ENGRAVED_BOX);
      o.color(7);
    }
    {
      local o = Fl_Group(6, 35, 340, 25);
      o.labelsize(16);
      {
        {
          local o = Fl_Button(6, 35, 25, 25, _tr("1"));
          btnMonth1 = o;
        }
        {
          local o = Fl_Button(34, 35, 25, 25, _tr("2"));
          btnMonth2 = o;
        }
        {
          local o = Fl_Button(63, 35, 25, 25, _tr("3"));
          btnMonth3 = o;
        }
        {
          local o = Fl_Button(91, 35, 25, 25, _tr("4"));
          btnMonth4 = o;
        }
        {
          local o = Fl_Button(120, 35, 25, 25, _tr("5"));
          btnMonth5 = o;
        }
        {
          local o = Fl_Button(148, 35, 25, 25, _tr("6"));
          btnMonth6 = o;
        }
        {
          local o = Fl_Button(177, 35, 25, 25, _tr("7"));
          btnMonth7 = o;
        }
        {
          local o = Fl_Button(205, 35, 25, 25, _tr("8"));
          btnMonth8 = o;
        }
        {
          local o = Fl_Button(234, 35, 25, 25, _tr("9"));
          btnMonth9 = o;
        }
        {
          local o = Fl_Button(262, 35, 25, 25, _tr("10"));
          btnMonth10 = o;
        }
        {
          local o = Fl_Button(291, 35, 25, 25, _tr("11"));
          btnMonth11 = o;
        }
        {
          local o = Fl_Button(320, 35, 25, 25, _tr("12"));
          btnMonth12 = o;
        }
      }
      o.end();
    }
    {
      local o = Fl_Data_Table(5, 65, 340, 202);
      grid = o;
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
    }
    {
      local o = Fl_Group(5, 275, 340, 25);
      o.labelsize(16);
      {
        {
          local o = Fl_Repeat_Button(5, 275, 50, 25, _tr("@<<"));
          btnPrevYear = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to previous year"));
        }
        {
          local o = Fl_Repeat_Button(63, 275, 50, 25, _tr("@<"));
          btnPrevMonth = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to previous month"));
        }
        {
          local o = Fl_Button(121, 275, 50, 25, _tr("@<->"));
          btnToday = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to today date"));
        }
        {
          local o = Fl_Button(179, 275, 50, 25, _tr("@circle"));
          btnSelect = o;
          o.labelsize(16);
          o.tooltip(_tr("Select current date"));
        }
        {
          local o = Fl_Repeat_Button(237, 275, 50, 25, _tr("@>"));
          btnNextMonth = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to next month"));
        }
        {
          local o = Fl_Repeat_Button(295, 275, 50, 25, _tr("@>>"));
          btnNextYear = o;
          o.labelsize(16);
          o.tooltip(_tr("Go to next year"));
        }
      }
      o.end();
    }
    end();
  }
}

class DeliveryCalcWindow extends Base_Window {
  
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
  
  constructor(px=338, py=167, pw=415, ph=285, pl=_tr("Delivery Calc")){
    base.constructor(px, py, pw, ph, pl);
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
    end();
  }
}

class ListSearchWindow extends Base_Window {
  
  // Declaration of class members
  topGroup = null;
  group_filter = null;
  query_limit = null;
  btnNotes = null;
  middleGroup = null;
  grid = null;
  bottomGroup = null;
  pack_search_options = null;
  search_str = null;
  btnSearch = null;
  btnSelect = null;
  btnUpdate = null;
  btnInsert = null;
  btnThumbImage = null;
  pack_search_options2 = null;
  
  constructor(px=132, py=118, pw=800, ph=560, pl=_tr("List Search")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Group(0, 0, 800, 35);
      topGroup = o;
      o.labelsize(16);
      {
        {
          local o = Flu_Combo_List(5, 5, 675, 25);
          group_filter = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
        }
        {
          local o = Fl_Int_Input(690, 6, 60, 25);
          query_limit = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          o->value("50");
        }
        {
          local o = Fl_Button(760, 6, 35, 25, _tr("?"));
          btnNotes = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Group(0, 41, 800, 409);
      middleGroup = o;
      o.labelsize(16);
      {
        {
          local o = Fl_Data_Table(5, 41, 790, 389);
          grid = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          o.end();
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(4, 440, 790, 115);
      bottomGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.color(246);
      {
        {
          local o = Fl_Pack(14, 450, 320, 25);
          pack_search_options = o;
          o.type(1);
          o.labelsize(16);
          o->spacing(10);
          o.end();
        }
        {
          local o = Fl_Input(35, 485, 299, 25, _tr("@search"));
          search_str = o;
          o.textsize(16);
          o.when(8);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(345, 485, 100, 25, _tr("Search"));
          btnSearch = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(14, 520, 100, 25, _tr("Select"));
          btnSelect = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(124, 520, 100, 25, _tr("Update"));
          btnUpdate = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(234, 520, 100, 25, _tr("Insert"));
          btnInsert = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Image_Box(663, 445, 125, 105);
          btnThumbImage = o;
          o.box(FL_NO_BOX);
          o.align(80);
        }
        {
          local o = Fl_Box(650, 445, 5, 105);
          o.align(80);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Pack(345, 520, 300, 25);
          pack_search_options2 = o;
          o.type(1);
          o.labelsize(16);
          o->spacing(10);
          o.end();
        }
      }
      o.end();
    }
    end();
  }
}

class BarChartGroup extends Fl_Group {
  
  // Declaration of class members
  pack = null;
  btnShowChart = null;
  periodes_to_show = null;
  btn_periode_is_years = null;
  btn_periode_is_months = null;
  btn_periode_is_weeks = null;
  btn_periode_is_days = null;
  chkOpt = null;
  bar_chart = null;
  
  constructor(px=288, py=260, pw=760, ph=460, pl=_tr("BarChartGroup")){
    base.constructor(px, py, pw, ph, pl);
    local _x = 288, _y = 260;
    // Create member functions and widgets
    {
      local o = Fl_Pack(_x + 0, _y + 0, 760, 25);
      pack = o;
      o.type(1);
      o.labelsize(16);
      o->spacing(10);
      {
        {
          local o = Fl_Button(_x + 0, _y + 0, 96, 25, _tr("Show"));
          btnShowChart = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Int_Input(_x + 106, _y + 0, 80, 25);
          periodes_to_show = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          o.tooltip(_tr("Write then number of periodes you want to see"));
          o->value("12");
        }
        {
          local o = Fl_Round_Button(_x + 196, _y + 0, 110, 25, _tr("Years"));
          btn_periode_is_years = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
        }
        {
          local o = Fl_Round_Button(_x + 316, _y + 0, 110, 25, _tr("Months"));
          btn_periode_is_months = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
          o->setonly();
        }
        {
          local o = Fl_Round_Button(_x + 436, _y + 0, 110, 25, _tr("Weeks"));
          btn_periode_is_weeks = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
        }
        {
          local o = Fl_Round_Button(_x + 556, _y + 0, 110, 25, _tr("Days"));
          btn_periode_is_days = o;
          o.labelsize(16);
          o.type(102);
          o.down_box(FL_ROUND_DOWN_BOX);
        }
        {
          local o = Fl_Check_Button(_x + 660, _y + 5, 64, 15);
          chkOpt = o;
          o.down_box(FL_DOWN_BOX);
        }
      }
      o.end();
    }
    {
      local o = Fl_Bar_Chart(_x + 0, _y + 30, 760, 430);
      bar_chart = o;
      o.align(5);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.color(7);
      #include "Fl_Bar_Chart.h"
      Fl_Group.current().resizable(o);
    }
    end();
  }
}

class HistoryGroup extends Fl_Group {
  
  // Declaration of class members
  history_choice = null;
  history_query_limit = null;
  btnViewDocHsitory = null;
  grid_history = null;
  
  constructor(px=13, py=33, pw=760, ph=465, pl=_tr("History")){
    base.constructor(px, py, pw, ph, pl);
    local _x = 13, _y = 33;
    // Create member functions and widgets
    {
      local o = Fl_Pack(_x + 0, _y + 0, 760, 25);
      o.type(1);
      o.labelsize(16);
      o->spacing(10);
      {
        {
          local o = Fl_Choice(_x + 0, _y + 0, 564, 25);
          history_choice = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Int_Input(_x + 574, _y + 0, 80, 25);
          history_query_limit = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          o.tooltip(_tr("Write then number of periodes you want to see"));
          o->value("50");
        }
        {
          local o = Fl_Button(_x + 664, _y + 0, 96, 25, _tr("View Doc."));
          btnViewDocHsitory = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Data_Table(_x + 0, _y + 33, 760, 432);
      grid_history = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    end();
  }
}

class BaseReportA4 extends Fl_Window {
  
  // Declaration of class members
  rpt_Logo = null;
  rpt_Title = null;
  rpt_SubTitle = null;
  rpt_Body = null;
  rpt_SubFooter = null;
  rpt_Footer = null;
  
  constructor(px=178, py=95, pw=420, ph=590, pl=_tr("BaseReportA4")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Box(10, 5, 105, 35, _tr("Ourbiz"));
      rpt_Logo = o;
      o.align(660);
      o.labelfont(3);
      o.labeltype(FL_SHADOW_LABEL);
      o.labelsize(30);
    }
    {
      local o = Fl_Box(120, 5, 295, 20, _tr("Report Title"));
      rpt_Title = o;
      o.labelfont(1);
      o.labelsize(16);
    }
    {
      local o = Fl_Box(120, 30, 290, 10, _tr("Date"));
      rpt_SubTitle = o;
      o.align(24);
      o.labelsize(8);
    }
    {
      local o = Fl_Box(10, 40, 405, 1, _tr("Line"));
      o.box(FL_FLAT_BOX);
      o.labeltype(FL_NO_LABEL);
      o.color(34);
    }
    {
      local o = Fl_Data_Table(10, 45, 408, 495);
      rpt_Body = o;
      o.textsize(8);
      o.labelsize(8);
      o->has_scrollbar(FLVS_NONE);
      o->feature_remove(FLVF_ROW_SELECT | FLVF_PERSIST_SELECT);
      o->for_print(true);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Box(10, 545, 405, 1, _tr("Line"));
      o.box(FL_FLAT_BOX);
      o.labeltype(FL_NO_LABEL);
      o.color(34);
    }
    {
      local o = Fl_Box(10, 547, 405, 10, _tr("Page"));
      rpt_SubFooter = o;
      o.labelfont(1);
      o.align(16);
      o.labelsize(8);
    }
    {
      local o = Fl_Box(10, 560, 405, 19, _tr("OURBIZ is an excelente application to manage a business."));
      rpt_Footer = o;
      o.labelfont(2);
      o.align(145);
      o.labelsize(8);
    }
    end();
  }
}

class PrintPreviewGroup extends Fl_Group {
  
  // Declaration of class members
  btnRefresh = null;
  btnPrinter = null;
  btnPrint = null;
  chkOnlyData = null;
  chkA4 = null;
  iNumCopies = null;
  brwPages = null;
  PreviewScroll = null;
  
  constructor(px=181, py=165, pw=770, ph=440, pl=_tr("Print Preview")){
    base.constructor(px, py, pw, ph, pl);
    local _x = 181, _y = 165;
    // Create member functions and widgets
    {
      local o = Fl_Group(_x + 0, _y + 0, 80, 440);
      {
        {
          local o = Fl_Button(_x + 0, _y + 5, 80, 25, _tr("Refresh"));
          btnRefresh = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(_x + 0, _y + 35, 80, 25, _tr("Printer"));
          btnPrinter = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(_x + 0, _y + 65, 80, 25, _tr("Print"));
          btnPrint = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Check_Button(_x + 30, _y + 118, 20, 25, _tr("Only Data"));
          chkOnlyData = o;
          o.align(1);
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Check_Button(_x + 30, _y + 165, 20, 25, _tr("A4"));
          chkA4 = o;
          o.align(1);
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = Fl_Int_Input(_x + 10, _y + 211, 56, 24, _tr("Copies"));
          iNumCopies = o;
          o.textsize(16);
          o.align(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Hold_Browser(_x + 5, _y + 260, 70, 180, _tr("Pages"));
          brwPages = o;
          o.textsize(16);
          o.align(1);
          o.labelsize(16);
          o.end();
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
    }
    {
      local o = OrderPageGroup(_x + 90, _y + 0, 680, 440);
      PreviewScroll = o;
      o.box(FL_ENGRAVED_FRAME);
      o.color(7);
      o.end();
      Fl_Group.current().resizable(o);
    }
    end();
  }
}

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
  
  constructor(px=213, py=39, pw=630, ph=685, pl=_tr("InvoiceA4")){
    base.constructor(px, py, pw, ph, pl);
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
    end();
  }
}

class ProductPricesGroup extends Fl_Group {
  
  // Declaration of class members
  grid = null;
  db_product_prices_quantity = null;
  db_product_prices_markup_pct = null;
  db_product_prices_discount_pct = null;
  db_product_prices_price = null;
  btnDbAction = null;
  dbAction = null;
  
  constructor(px=48, py=153, pw=760, ph=220, pl=_tr("Product Prices by Quantity")){
    base.constructor(px, py, pw, ph, pl);
    local _x = 48, _y = 153;
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
    end();
  }
}

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
  
  constructor(px=24, py=106, pw=775, ph=470, pl=_tr("ProductKitGroup")){
    base.constructor(px, py, pw, ph, pl);
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
    end();
  }
}

class AskLoginWindow extends Fl_Double_Window {
  
  // Declaration of class members
  host = null;
  port = null;
  user = null;
  password = null;
  btnConnect = null;
  
  constructor(px=246, py=157, pw=500, ph=120, pl=_tr("Server Login")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Input(10, 30, 405, 25, _tr("Host"));
      host = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      o->value("localhost");
    }
    {
      local o = Fl_Int_Input(425, 30, 65, 25, _tr("Port"));
      port = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      o->value("8888");
    }
    {
      local o = Fl_Input(10, 85, 170, 25, _tr("User"));
      user = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
    }
    {
      local o = Fl_Secret_Input(200, 85, 170, 25, _tr("Password"));
      password = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
    }
    {
      local o = Fl_Return_Button(385, 85, 106, 25, _tr("Connect"));
      btnConnect = o;
    }
    end();
  }
}

class AppUsersEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_app_users_id = null;
  db_app_users_cdate = null;
  db_app_users_mdate = null;
  db_app_users_is_active = null;
  db_app_users_user_name = null;
  db_app_users_password1 = null;
  db_app_users_password2 = null;
  db_app_users_name = null;
  dbAction = null;
  btnDbAction = null;
  db_app_users_rights = null;
  db_app_users_notes = null;
  
  constructor(px=282, py=148, pw=500, ph=500, pl=_tr("App Users List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 490, 140);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 148, 489, 347);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 173, 82, 26, _tr("ID"));
          db_app_users_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("app_users", "id", db_app_users_id);
        }
        {
          local o = Fl_Output(115, 173, 120, 25, _tr("CDate"));
          db_app_users_cdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("app_users", "cdate", db_app_users_cdate);
        }
        {
          local o = Fl_Output(255, 173, 120, 25, _tr("MDate"));
          db_app_users_mdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("app_users", "mdate", db_app_users_mdate);
        }
        {
          local o = Fl_Check_Button(385, 175, 100, 23, _tr("Active"));
          db_app_users_is_active = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
          add_input_field_to_map("app_users", "is_active", db_app_users_is_active);
        }
        {
          local o = Fl_Input(15, 225, 140, 25, _tr("UserName"));
          db_app_users_user_name = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("app_users", "user_name", db_app_users_user_name);
        }
        {
          local o = Fl_Secret_Input(180, 225, 140, 25, _tr("Password"));
          db_app_users_password1 = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("app_users", "password1", db_app_users_password1);
        }
        {
          local o = Fl_Secret_Input(345, 225, 140, 25, _tr("Repite Password"));
          db_app_users_password2 = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("app_users", "password2", db_app_users_password2);
        }
        {
          local o = Fl_Input(15, 275, 470, 25, _tr("Name"));
          db_app_users_name = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("app_users", "name", db_app_users_name);
        }
        {
          local o = Fl_Choice_dbAction(380, 428, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 463, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Text_Editor_Buffered(15, 325, 470, 55, _tr("Rights"));
          db_app_users_rights = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("app_users", "rights", db_app_users_rights);
        }
        {
          local o = Fl_Text_Editor_Buffered(15, 398, 360, 90, _tr("Notes"));
          db_app_users_notes = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("app_users", "notes", db_app_users_notes);
        }
        {
          local o = Fl_Check_Button(380, 398, 105, 25, _tr("Wrap lines"));
          o.labelsize(16);
          o.value(1);
          o.down_box(FL_DOWN_BOX);
        }
      }
      o.end();
    }
    end();
  }
}

class AppConfigEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_config_id = null;
  db_config_cdate = null;
  db_config_mdate = null;
  db_config_is_active = null;
  db_config_key = null;
  db_config_value = null;
  dbAction = null;
  btnDbAction = null;
  db_config_notes = null;
  
  constructor(px=346, py=83, pw=500, ph=490, pl=_tr("Application Configuration List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 490, 200);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 215, 490, 269);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 240, 82, 26, _tr("ID"));
          db_config_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("config", "id", db_config_id);
        }
        {
          local o = Fl_Output(115, 240, 120, 25, _tr("CDate"));
          db_config_cdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("config", "cdate", db_config_cdate);
        }
        {
          local o = Fl_Output(255, 240, 120, 25, _tr("MDate"));
          db_config_mdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("config", "mdate", db_config_mdate);
        }
        {
          local o = Fl_Check_Button(385, 242, 100, 23, _tr("Active"));
          db_config_is_active = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
          add_input_field_to_map("config", "is_active", db_config_is_active);
        }
        {
          local o = Fl_Input(15, 292, 350, 25, _tr("Key"));
          db_config_key = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("config", "key", db_config_key);
        }
        {
          local o = Fl_Input(15, 345, 350, 25, _tr("Value"));
          db_config_value = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("config", "value", db_config_value);
        }
        {
          local o = Fl_Choice_dbAction(380, 310, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 345, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Multiline_Input(15, 398, 470, 76, _tr("Notes"));
          db_config_notes = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("config", "notes", db_config_notes);
        }
      }
      o.end();
    }
    end();
  }
}

class TanslationsWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  myTabs = null;
  tabLanguages = null;
  grid_languages = null;
  db_languages_lang = null;
  db_languages_date_format = null;
  db_languages_name = null;
  db_languages_native_name = null;
  db_languages_decimal_sep = null;
  db_languages_thousand_sep = null;
  tabTranslations = null;
  grid_translations = null;
  db_translations_language = null;
  db_translations_values_svalue = null;
  btnTransSearch = null;
  btnTransClear = null;
  dbAction = null;
  btnDbAction = null;
  
  constructor(px=217, py=76, pw=490, ph=560, pl=_tr("Translations")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Tabs(5, 10, 482, 505);
      myTabs = o;
      o.labelsize(16);
      {
        {
          local o = Fl_Group(5, 40, 480, 475, _tr("Languages"));
          tabLanguages = o;
          o.labelsize(16);
          {
            {
              local o = Fl_Data_Table(21, 55, 456, 215);
              grid_languages = o;
              o.textsize(16);
              o.labelsize(16);
              o.end();
            }
            {
              local o = Fl_Input(20, 295, 75, 25, _tr("Code"));
              db_languages_lang = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Input(366, 295, 110, 25, _tr("Date format"));
              db_languages_date_format = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Input(20, 345, 456, 25, _tr("Name"));
              db_languages_name = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Input(20, 395, 456, 25, _tr("Native Name"));
              db_languages_native_name = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Input(205, 430, 35, 40, _tr("Separators -> Decimal"));
              db_languages_decimal_sep = o;
              o.textsize(40);
              o.labelsize(16);
            }
            {
              local o = Fl_Input(345, 430, 35, 40, _tr("Thousand"));
              db_languages_thousand_sep = o;
              o.textsize(40);
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 40, 480, 475, _tr("Translations"));
          tabTranslations = o;
          o.labelsize(16);
          {
            {
              local o = Fl_Data_Table(16, 55, 461, 180);
              grid_translations = o;
              o.textsize(16);
              o.labelsize(16);
              o.end();
            }
            {
              local o = Fl_Choice_Int(16, 260, 461, 30, _tr("Language"));
              db_translations_language = o;
              o.textsize(16);
              o.align(5);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
            }
            {
              local o = Fl_Multiline_Input(16, 315, 461, 160, _tr("Content"));
              db_translations_values_svalue = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Button(301, 480, 80, 25, _tr("Sea&rch"));
              btnTransSearch = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(396, 480, 80, 25, _tr("Cl&ear"));
              btnTransClear = o;
              o.labelsize(16);
              o.callback(function(sender, udata){
                db_translations_values_svalue->value("");
              });
            }
          }
          o.end();
        }
      }
      o.end();
    }
    {
      local o = Fl_Choice_dbAction(26, 525, 105, 25);
      dbAction = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.down_box(FL_BORDER_BOX);
      o.labelsize(16);
    }
    {
      local o = Fl_Button(151, 525, 105, 25, _tr("Action"));
      btnDbAction = o;
      o.labelsize(16);
    }
    end();
  }
}

class PaymentTypesEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_payment_types_id = null;
  db_payment_types_cdate = null;
  db_payment_types_mdate = null;
  db_payment_types_is_active = null;
  db_payment_types_code = null;
  db_payment_types_description = null;
  npayments = null;
  payment_periodes = null;
  btnGenerateTerms = null;
  dbAction = null;
  btnDbAction = null;
  db_payment_types_payment_terms = null;
  db_payment_types_notes = null;
  
  constructor(px=302, py=168, pw=500, ph=500, pl=_tr("Payment Types List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 490, 140);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 148, 489, 347);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 173, 82, 26, _tr("ID"));
          db_payment_types_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("payment_types", "id", db_payment_types_id);
        }
        {
          local o = Fl_Output(115, 173, 120, 25, _tr("CDate"));
          db_payment_types_cdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("payment_types", "cdate", db_payment_types_cdate);
        }
        {
          local o = Fl_Output(255, 173, 120, 25, _tr("MDate"));
          db_payment_types_mdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("payment_types", "mdate", db_payment_types_mdate);
        }
        {
          local o = Fl_Check_Button(385, 175, 100, 23, _tr("Active"));
          db_payment_types_is_active = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
          add_input_field_to_map("payment_types", "is_active", db_payment_types_is_active);
        }
        {
          local o = Fl_Input(15, 225, 82, 25, _tr("Code"));
          db_payment_types_code = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("payment_types", "code", db_payment_types_code);
        }
        {
          local o = Fl_Input(105, 225, 380, 25, _tr("Description"));
          db_payment_types_description = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("payment_types", "description", db_payment_types_description);
        }
        {
          local o = Fl_Int_Input(380, 275, 105, 25, _tr("# Payments"));
          npayments = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
        }
        {
          local o = Fl_Choice(380, 320, 105, 25, _tr("Periodes"));
          payment_periodes = o;
          o.textsize(16);
          o.align(5);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 355, 105, 25, _tr("Generate"));
          btnGenerateTerms = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Choice_dbAction(380, 428, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 463, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Text_Editor_Buffered(15, 275, 360, 105, _tr("Terms (#  %  after  periode)"));
          db_payment_types_payment_terms = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("payment_types", "payment_terms", db_payment_types_payment_terms);
        }
        {
          local o = Fl_Text_Editor_Buffered(15, 398, 360, 90, _tr("Notes"));
          db_payment_types_notes = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("payment_types", "notes", db_payment_types_notes);
        }
        {
          local o = Fl_Check_Button(380, 398, 105, 25, _tr("Wrap lines"));
          o.labelsize(16);
          o.value(1);
          o.down_box(FL_DOWN_BOX);
        }
      }
      o.end();
    }
    end();
  }
}

class WarrantyTypesEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_warranty_types_id = null;
  db_warranty_types_cdate = null;
  db_warranty_types_mdate = null;
  db_warranty_types_code = null;
  db_warranty_types_is_active = null;
  db_warranty_types_description = null;
  dbAction = null;
  btnDbAction = null;
  
  constructor(px=219, py=105, pw=500, ph=385, pl=_tr("Warranty Types List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 490, 200);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 215, 490, 165);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 240, 82, 26, _tr("ID"));
          db_warranty_types_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("warranty_types", "id", db_warranty_types_id);
        }
        {
          local o = Fl_Output(115, 240, 120, 25, _tr("CDate"));
          db_warranty_types_cdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("warranty_types", "cdate", db_warranty_types_cdate);
        }
        {
          local o = Fl_Output(255, 240, 120, 25, _tr("MDate"));
          db_warranty_types_mdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("warranty_types", "mdate", db_warranty_types_mdate);
        }
        {
          local o = Fl_Input(15, 295, 82, 25, _tr("Code"));
          db_warranty_types_code = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("warranty_types", "code", db_warranty_types_code);
        }
        {
          local o = Fl_Check_Button(385, 242, 100, 23, _tr("Active"));
          db_warranty_types_is_active = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
          add_input_field_to_map("warranty_types", "is_active", db_warranty_types_is_active);
        }
        {
          local o = Fl_Input(15, 345, 360, 25, _tr("Description"));
          db_warranty_types_description = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("warranty_types", "description", db_warranty_types_description);
        }
        {
          local o = Fl_Choice_dbAction(380, 310, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 345, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    end();
  }
}

class MeasureUnitsEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_measure_units_id = null;
  db_measure_units_cdate = null;
  db_measure_units_mdate = null;
  db_measure_units_code = null;
  db_measure_units_is_active = null;
  db_measure_units_description = null;
  dbAction = null;
  btnDbAction = null;
  
  constructor(px=286, py=134, pw=500, ph=385, pl=_tr("Measure Units List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 490, 200);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 215, 490, 165);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 240, 82, 26, _tr("ID"));
          db_measure_units_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("measure_units", "id", db_measure_units_id);
        }
        {
          local o = Fl_Output(115, 240, 120, 25, _tr("CDate"));
          db_measure_units_cdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("measure_units", "cdate", db_measure_units_cdate);
        }
        {
          local o = Fl_Output(255, 240, 120, 25, _tr("MDate"));
          db_measure_units_mdate = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("measure_units", "mdate", db_measure_units_mdate);
        }
        {
          local o = Fl_Input(15, 295, 82, 25, _tr("Code"));
          db_measure_units_code = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("measure_units", "code", db_measure_units_code);
        }
        {
          local o = Fl_Check_Button(385, 242, 100, 23, _tr("Active"));
          db_measure_units_is_active = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
          add_input_field_to_map("measure_units", "is_active", db_measure_units_is_active);
        }
        {
          local o = Fl_Input(15, 345, 360, 25, _tr("Description"));
          db_measure_units_description = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("measure_units", "description", db_measure_units_description);
        }
        {
          local o = Fl_Choice_dbAction(380, 310, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 345, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    end();
  }
}

class SalesTaxRatesEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_sales_tax_rates_id = null;
  db_sales_tax_rates_cdate = null;
  db_sales_tax_rates_mdate = null;
  db_sales_tax_rates_rate1 = null;
  db_sales_tax_rates_rate2 = null;
  db_sales_tax_rates_is_active = null;
  db_sales_tax_rates_description = null;
  dbAction = null;
  btnDbAction = null;
  
  constructor(px=266, py=87, pw=500, ph=385, pl=_tr("Sales Tax Rates List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 490, 200);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.tooltip(_tr("List of available sales tax"));
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 215, 490, 165);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 240, 82, 26, _tr("ID"));
          db_sales_tax_rates_id = o;
          o.textsize(16);
          o.align(5);
          o.tooltip(_tr("The unique identifier"));
          o.labelsize(16);
          add_input_field_to_map("sales_tax_rates", "id", db_sales_tax_rates_id);
        }
        {
          local o = Fl_Output(115, 240, 120, 25, _tr("CDate"));
          db_sales_tax_rates_cdate = o;
          o.textsize(16);
          o.align(5);
          o.tooltip(_tr("The creation date"));
          o.labelsize(16);
          add_input_field_to_map("sales_tax_rates", "cdate", db_sales_tax_rates_cdate);
        }
        {
          local o = Fl_Output(255, 240, 120, 25, _tr("MDate"));
          db_sales_tax_rates_mdate = o;
          o.textsize(16);
          o.align(5);
          o.tooltip(_tr("The modification date"));
          o.labelsize(16);
          add_input_field_to_map("sales_tax_rates", "mdate", db_sales_tax_rates_mdate);
        }
        {
          local o = Fl_Float_Input(15, 295, 82, 25, _tr("V.A.T. %"));
          db_sales_tax_rates_rate1 = o;
          o.textsize(16);
          o.align(5);
          o.tooltip(_tr("Main sales tax percentage"));
          o.labelsize(16);
          o.textfont(1);
          add_input_field_to_map("sales_tax_rates", "rate1", db_sales_tax_rates_rate1);
        }
        {
          local o = Fl_Float_Input(115, 295, 82, 25, _tr("R.E. %"));
          db_sales_tax_rates_rate2 = o;
          o.textsize(16);
          o.align(5);
          o.tooltip(_tr("Secondary sales tax percentage"));
          o.labelsize(16);
          o.textfont(1);
          add_input_field_to_map("sales_tax_rates", "rate2", db_sales_tax_rates_rate2);
        }
        {
          local o = Fl_Check_Button(255, 294, 100, 23, _tr("Active"));
          db_sales_tax_rates_is_active = o;
          o.down_box(FL_DOWN_BOX);
          o.labelsize(16);
          o.tooltip(_tr("We can deactivate instead of delete it"));
          add_input_field_to_map("sales_tax_rates", "is_active", db_sales_tax_rates_is_active);
        }
        {
          local o = Fl_Input(15, 345, 350, 25, _tr("Description"));
          db_sales_tax_rates_description = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          o.tooltip(_tr("A friendly description to identify this tax"));
          add_input_field_to_map("sales_tax_rates", "description", db_sales_tax_rates_description);
        }
        {
          local o = Fl_Choice_dbAction(380, 310, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 345, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    end();
  }
}

class OrderTypesEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_order_types_code = null;
  db_order_types_description_to_print = null;
  db_order_types_series = null;
  db_order_types_numbering = null;
  db_order_types_group_order = null;
  db_order_types_subgroup_order = null;
  pack = null;
  db_order_types_is_active = null;
  db_order_types_with_sales_tax = null;
  db_order_types_with_sales_tax_included = null;
  db_order_types_show_sales_tax = null;
  db_order_types_show_prices = null;
  db_order_types_description = null;
  db_order_types_with_payment = null;
  db_order_types_with_inventory = null;
  db_order_types_with_order = null;
  db_order_types_with_credit = null;
  db_order_types_is_incoming = null;
  db_order_types_is_expense = null;
  dbAction = null;
  btnDbAction = null;
  db_order_types_notes = null;
  db_order_types_id = null;
  db_order_types_with_quote = null;
  db_order_types_with_invoice = null;
  db_order_types_with_lost = null;
  
  constructor(px=100, py=110, pw=800, ph=535, pl=_tr("Order Types List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 790, 200);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 210, 790, 320);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Input(103, 233, 82, 25, _tr("Code"));
          db_order_types_code = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "code", db_order_types_code);
        }
        {
          local o = Fl_Input(195, 233, 225, 25, _tr("Description to print"));
          db_order_types_description_to_print = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "description_to_print", db_order_types_description_to_print);
        }
        {
          local o = Fl_Input(430, 233, 82, 25, _tr("Series"));
          db_order_types_series = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "series", db_order_types_series);
        }
        {
          local o = Fl_Int_Input(522, 233, 98, 25, _tr("Number"));
          db_order_types_numbering = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "numbering", db_order_types_numbering);
        }
        {
          local o = Fl_Input(630, 233, 70, 25, _tr("Grp 1"));
          db_order_types_group_order = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "group_order", db_order_types_group_order);
        }
        {
          local o = Fl_Input(710, 233, 75, 25, _tr("Grp 2"));
          db_order_types_subgroup_order = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "subgroup_order", db_order_types_subgroup_order);
        }
        {
          local o = Fl_Pack(15, 266, 770, 29);
          pack = o;
          o.type(1);
          o.labelsize(16);
          o->with_label(1);
          o->spacing(10);
          {
            {
              local o = Fl_Check_Button(20, 270, 78, 25, _tr("Active"));
              db_order_types_is_active = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("order_types", "is_active", db_order_types_is_active);
            }
            {
              local o = Fl_Check_Button(100, 270, 100, 25, _tr("With V.A.T."));
              db_order_types_with_sales_tax = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("order_types", "with_sales_tax", db_order_types_with_sales_tax);
            }
            {
              local o = Fl_Check_Button(220, 270, 100, 25, _tr("V.A.T. Included"));
              db_order_types_with_sales_tax_included = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("order_types", "with_sales_tax_included", db_order_types_with_sales_tax_included);
            }
            {
              local o = Fl_Check_Button(365, 270, 100, 25, _tr("Show V.A.T."));
              db_order_types_show_sales_tax = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("order_types", "show_sales_tax", db_order_types_show_sales_tax);
            }
            {
              local o = Fl_Check_Button(490, 270, 100, 25, _tr("Show prices"));
              db_order_types_show_prices = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("order_types", "show_prices", db_order_types_show_prices);
            }
          }
          o.end();
        }
        {
          local o = Fl_Input(15, 315, 385, 25, _tr("Description"));
          db_order_types_description = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "description", db_order_types_description);
        }
        {
          local o = Fl_Input(425, 435, 30, 25, _tr("Payment"));
          db_order_types_with_payment = o;
          o.textsize(24);
          o.align(8);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "with_payment", db_order_types_with_payment);
        }
        {
          local o = Fl_Input(425, 375, 30, 25, _tr("Inventory"));
          db_order_types_with_inventory = o;
          o.textsize(24);
          o.align(8);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "with_inventory", db_order_types_with_inventory);
        }
        {
          local o = Fl_Input(425, 345, 30, 25, _tr("Order"));
          db_order_types_with_order = o;
          o.textsize(24);
          o.align(8);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "with_order", db_order_types_with_order);
        }
        {
          local o = Fl_Input(425, 465, 30, 25, _tr("Credit"));
          db_order_types_with_credit = o;
          o.textsize(24);
          o.align(8);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "with_credit", db_order_types_with_credit);
        }
        {
          local o = Fl_Check_Button(655, 350, 125, 25, _tr("Income"));
          db_order_types_is_incoming = o;
          o.down_box(FL_DOWN_BOX);
          o.labelsize(16);
          o.type(102);
          add_input_field_to_map("order_types", "is_incoming", db_order_types_is_incoming);
        }
        {
          local o = Fl_Check_Button(655, 315, 125, 25, _tr("Expense"));
          db_order_types_is_expense = o;
          o.down_box(FL_DOWN_BOX);
          o.labelsize(16);
          o.type(102);
          add_input_field_to_map("order_types", "is_expense", db_order_types_is_expense);
        }
        {
          local o = Fl_Choice_dbAction(679, 385, 106, 28);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(681, 424, 104, 28, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Multiline_Input(15, 362, 385, 158, _tr("Notes"));
          db_order_types_notes = o;
          o.textsize(16);
          o.align(5);
          o.when(8);
          o.labelsize(16);
          add_input_field_to_map("order_types", "notes", db_order_types_notes);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Output(15, 233, 78, 25, _tr("ID"));
          db_order_types_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("order_types", "id", db_order_types_id);
        }
        {
          local o = Fl_Input(425, 315, 30, 25, _tr("Quote"));
          db_order_types_with_quote = o;
          o.textsize(24);
          o.align(8);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "with_quote", db_order_types_with_quote);
        }
        {
          local o = Fl_Input(425, 405, 30, 25, _tr("Invoice"));
          db_order_types_with_invoice = o;
          o.textsize(24);
          o.align(8);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "with_invoice", db_order_types_with_invoice);
        }
        {
          local o = Fl_Input(425, 495, 30, 25, _tr("Lost"));
          db_order_types_with_lost = o;
          o.textsize(24);
          o.align(8);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("order_types", "with_lost", db_order_types_with_lost);
        }
      }
      o.end();
    }
    end();
  }
}

class ImageWindow extends Base_Window {
  
  // Declaration of class members
  btnImage = null;
  
  constructor(px=325, py=355, pw=305, ph=260, pl=_tr("Image")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Image_Box(0, 0, 305, 263);
      btnImage = o;
      o.box(FL_NO_BOX);
      Fl_Group.current().resizable(o);
    }
    end();
  }
}

class DynamicQueryWindow extends Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  
  constructor(px=285, py=319, pw=700, ph=400, pl=_tr("Dynamic Data")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 690, 390);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    end();
  }
}

class ImagesListEditWindow extends List_Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_images_id = null;
  db_images_name = null;
  db_images_group_set = null;
  db_images_mime_type = null;
  db_images_mdate = null;
  db_images_cdate = null;
  btnSearch = null;
  dbAction = null;
  btnDbAction = null;
  db_images_description = null;
  query_limit = null;
  img_size = null;
  btnImage = null;
  
  constructor(px=81, py=129, pw=800, ph=560, pl=_tr("Images List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 790, 295);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 305, 790, 250);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Group(15, 315, 435, 232);
          {
            {
              local o = Fl_Output(15, 328, 78, 25, _tr("ID"));
              db_images_id = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("images", "id", db_images_id);
            }
            {
              local o = Fl_Input(103, 328, 347, 25, _tr("Name"));
              db_images_name = o;
              o.textsize(16);
              o.align(5);
              o.textfont(1);
              o.labelsize(16);
              add_input_field_to_map("images", "name", db_images_name);
            }
            {
              local o = Fl_Int_Input(15, 380, 80, 25, _tr("Group"));
              db_images_group_set = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("images", "group_set", db_images_group_set);
            }
            {
              local o = Fl_Output(105, 380, 115, 25, _tr("Type"));
              db_images_mime_type = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("images", "mime_type", db_images_mime_type);
            }
            {
              local o = Fl_Output(225, 380, 110, 25, _tr("Update Date"));
              db_images_mdate = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("images", "mdate", db_images_mdate);
            }
            {
              local o = Fl_Output(338, 380, 110, 25, _tr("Create Date"));
              db_images_cdate = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("images", "cdate", db_images_cdate);
            }
            {
              local o = Fl_Button(345, 455, 104, 25, _tr("Search"));
              btnSearch = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Choice_dbAction(343, 487, 106, 28);
              dbAction = o;
              o.textsize(16);
              o.labeltype(FL_NO_LABEL);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
            }
            {
              local o = Fl_Button(345, 519, 104, 28, _tr("Action"));
              btnDbAction = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Multiline_Input(15, 430, 320, 115, _tr("Notes"));
              db_images_description = o;
              o.textsize(16);
              o.align(5);
              o.when(8);
              o.labelsize(16);
              add_input_field_to_map("images", "description", db_images_description);
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Int_Input(385, 425, 60, 25, _tr("Limit"));
              query_limit = o;
              o.textsize(16);
              o.labelsize(16);
              o->value("50");
            }
            {
              local o = Fl_Box(225, 405, 109, 25, _tr("Size"));
              img_size = o;
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Image_Box(485, 315, 300, 232);
          btnImage = o;
          o.box(FL_ENGRAVED_FRAME);
          o.align(80);
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
    }
    end();
  }
}

class GroupsListEditWindow extends Edit_Base_Window {
  
  // Declaration of class members
  tree = null;
  bottomGroup = null;
  db_group_description = null;
  db_group_parent_id = null;
  dbAction = null;
  btnDbAction = null;
  db_group_id = null;
  btnWrapNotes = null;
  db_group_notes = null;
  
  constructor(px=328, py=91, pw=800, ph=560, pl=_tr("Groups List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = MyTree_Browser(10, 10, 780, 360);
      tree = o;
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      //o->showroot(0);
      //o->select_focus(1);
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(0, 375, 800, 185);
      bottomGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.color(246);
      {
        {
          local o = Fl_Input(98, 408, 576, 28, _tr("Description"));
          db_group_description = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("group", "description", db_group_description);
        }
        {
          local o = Fl_Int_Input(684, 408, 106, 28, _tr("Parent"));
          db_group_parent_id = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("group", "parent_id", db_group_parent_id);
        }
        {
          local o = Fl_Choice_dbAction(684, 484, 106, 28);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(686, 522, 104, 28, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Output(10, 408, 78, 28, _tr("ID"));
          db_group_id = o;
          o.textsize(16);
          o.align(5);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("group", "id", db_group_id);
        }
        {
          local o = Fl_Check_Button(95, 440, 115, 25, _tr("Wrap lines"));
          btnWrapNotes = o;
          o.labelsize(16);
          o.value(1);
          o.down_box(FL_DOWN_BOX);
        }
        {
          local o = MyFl_Text_Editor(10, 462, 660, 88, _tr("Notes"));
          db_group_notes = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("group", "notes", db_group_notes);
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
    }
    end();
  }
}

class OrdersSumWindow extends Edit_Base_Window {
  
  // Declaration of class members
  grid_orders_sum = null;
  bottomGroup = null;
  query_limit = null;
  search_str = null;
  btnSearch = null;
  btnDelete = null;
  btnCreate = null;
  orders_sum_detail_type = null;
  grid_orders_sum_details = null;
  
  constructor(px=92, py=155, pw=800, ph=560, pl=_tr("Orders Sum")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 4, 790, 202, _tr("@2 Orders Sum @2"));
      grid_orders_sum = o;
      o.textsize(16);
      o.align(17);
      o.box(FL_ENGRAVED_BOX);
      o.color(175);
      o.labelsize(16);
      o.end();
    }
    {
      local o = Fl_Group(5, 210, 790, 35);
      bottomGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.color(246);
      {
        {
          local o = Fl_Int_Input(15, 216, 40, 24);
          query_limit = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.tooltip(_tr("Limit the amount of records to show"));
          o.labelsize(16);
          o->value("50");
        }
        {
          local o = Fl_Input(65, 215, 375, 25);
          search_str = o;
          o.textsize(16);
          o.labelsize(16);
          o.when(8);
          o.labeltype(FL_NO_LABEL);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Button(455, 215, 100, 25, _tr("Search"));
          btnSearch = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(689, 215, 100, 25, _tr("Delete"));
          btnDelete = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(574, 215, 100, 25, _tr("Create"));
          btnCreate = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Group(5, 255, 790, 300);
      {
        {
          local o = Fl_Choice(5, 255, 790, 25);
          orders_sum_detail_type = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
        }
        {
          local o = Fl_Data_Table(5, 288, 790, 267, _tr("@2 Ordes Sum Details @2"));
          grid_orders_sum_details = o;
          o.textsize(16);
          o.align(17);
          o.box(FL_ENGRAVED_BOX);
          o.color(175);
          o.labelsize(16);
          o.end();
          Fl_Group.current().resizable(o);
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    end();
  }
}

class PaymentEditWindow extends Edit_Base_Window {
  
  // Declaration of class members
  db_payments_id = null;
  btnOrderId = null;
  db_payments_amount = null;
  db_payments_due_date = null;
  db_payments_entity_name = null;
  db_payments_cdate = null;
  db_payments_notes = null;
  btnShowClearedCalendar = null;
  db_payments_cleared_date = null;
  group_lines = null;
  grid_lines = null;
  group_edit_line = null;
  db_payments_lines_amount = null;
  btnSaveLine = null;
  db_payments_lines_notes = null;
  db_payments_lines_payment_date = null;
  btnPaymentDateCalendar = null;
  btnSave = null;
  btnPrint = null;
  
  constructor(px=147, py=111, pw=720, ph=430, pl=_tr("Payment Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Output(85, 5, 100, 25, _tr("Payment #"));
      db_payments_id = o;
      o.textsize(16);
      o.labelsize(16);
      add_input_field_to_map("payments", "id", db_payments_id);
    }
    {
      local o = Fl_Button(190, 5, 30, 25, _tr("@<->"));
      btnOrderId = o;
      o.labelcolor(22);
      o.labelsize(18);
      o.tooltip(_tr("Show Associated order if any [order_id]."));
    }
    {
      local o = Fl_Float_Input(330, 5, 110, 25, _tr("Amount"));
      db_payments_amount = o;
      o.textsize(16);
      o.textfont(1);
      o.labelsize(16);
      add_input_field_to_map("payments", "amount", db_payments_amount);
    }
    {
      local o = Fl_Input(585, 5, 125, 25, _tr("Due Date"));
      db_payments_due_date = o;
      o.textsize(16);
      o.labelsize(16);
      o.textfont(1);
      add_input_field_to_map("payments", "due_date", db_payments_due_date);
    }
    {
      local o = Fl_Input(10, 50, 570, 25, _tr("Entity"));
      db_payments_entity_name = o;
      o.textsize(16);
      o.align(5);
      o.textfont(1);
      o.labelsize(16);
      add_input_field_to_map("payments", "entity_name", db_payments_entity_name);
    }
    {
      local o = Fl_Input(585, 50, 125, 25, _tr("Created"));
      db_payments_cdate = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      add_input_field_to_map("payments", "cdate", db_payments_cdate);
    }
    {
      local o = Fl_Input(10, 95, 535, 25, _tr("Notes"));
      db_payments_notes = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      add_input_field_to_map("payments", "notes", db_payments_notes);
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Button(550, 95, 30, 25, _tr("@<->"));
      btnShowClearedCalendar = o;
      o.labelcolor(22);
      o.labelsize(18);
      o.tooltip(_tr("Show calendar"));
    }
    {
      local o = Fl_Input(585, 95, 125, 25, _tr("Cleared"));
      db_payments_cleared_date = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      add_input_field_to_map("payments", "cleared_date", db_payments_cleared_date);
    }
    {
      local o = Fl_Group(10, 126, 700, 264);
      group_lines = o;
      o.box(FL_THIN_UP_BOX);
      o.labelsize(16);
      o.color(246);
      o.labeltype(FL_NO_LABEL);
      {
        {
          local o = Fl_Data_Table(15, 135, 690, 146);
          grid_lines = o;
          o.textsize(16);
          o.align(5);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(15, 288, 695, 102);
          group_edit_line = o;
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Float_Input(15, 360, 125, 25, _tr("Amount"));
              db_payments_lines_amount = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("payments_lines", "amount", db_payments_lines_amount);
            }
            {
              local o = Fl_Button(340, 360, 110, 25, _tr("Save Line"));
              btnSaveLine = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Input(15, 310, 690, 25, _tr("Description"));
              db_payments_lines_notes = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("payments_lines", "notes", db_payments_lines_notes);
            }
            {
              local o = Fl_Input(155, 359, 125, 25, _tr("Date"));
              db_payments_lines_payment_date = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("payments_lines", "payment_date", db_payments_lines_payment_date);
            }
            {
              local o = Fl_Button(285, 360, 30, 25, _tr("@<->"));
              btnPaymentDateCalendar = o;
              o.labelcolor(22);
              o.labelsize(18);
              o.tooltip(_tr("Show calendar"));
            }
          }
          o.end();
        }
      }
      o.end();
    }
    {
      local o = Fl_Button(495, 400, 105, 25, _tr("Save"));
      btnSave = o;
      o.labelsize(16);
    }
    {
      local o = Fl_Button(605, 400, 105, 25, _tr("Print"));
      btnPrint = o;
      o.labelsize(16);
    }
    end();
  }
}

class GLGroupsListEditWindow extends Edit_Base_Window {
  
  // Declaration of class members
  grid = null;
  editGroup = null;
  db_gl_groups_id = null;
  db_gl_groups_code = null;
  db_gl_groups_debit_op = null;
  db_gl_groups_credit_op = null;
  db_gl_groups_description = null;
  dbAction = null;
  btnDbAction = null;
  
  constructor(px=480, py=215, pw=500, ph=352, pl=_tr("GL Groups List / Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Data_Table(5, 5, 490, 200);
      grid = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(5, 215, 490, 130);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 240, 82, 26, _tr("ID"));
          db_gl_groups_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_groups", "id", db_gl_groups_id);
        }
        {
          local o = Fl_Input(110, 240, 80, 25, _tr("Code"));
          db_gl_groups_code = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_groups", "code", db_gl_groups_code);
        }
        {
          local o = Fl_Input(250, 240, 30, 25, _tr("Op. Debit (+/-)"));
          db_gl_groups_debit_op = o;
          o.textsize(24);
          o.align(1);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("gl_groups", "debit_op", db_gl_groups_debit_op);
        }
        {
          local o = Fl_Input(380, 240, 30, 25, _tr("Op.Credit (+/-)"));
          db_gl_groups_credit_op = o;
          o.textsize(24);
          o.align(1);
          o.textfont(1);
          o.labelsize(16);
          add_input_field_to_map("gl_groups", "credit_op", db_gl_groups_credit_op);
        }
        {
          local o = Fl_Input(15, 310, 350, 25, _tr("Description"));
          db_gl_groups_description = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_groups", "description", db_gl_groups_description);
        }
        {
          local o = Fl_Choice_dbAction(380, 275, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(380, 310, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    end();
  }
}

class GLChartEditWindow extends Edit_Base_Window {
  
  // Declaration of class members
  editGroup = null;
  db_gl_chart_id = null;
  db_gl_chart_budget = null;
  db_gl_chart_is_active = null;
  db_gl_chart_is_header = null;
  db_gl_chart_group_code = null;
  db_gl_chart_gl_group_id = null;
  db_gl_chart_description = null;
  dbAction = null;
  btnDbAction = null;
  db_gl_chart_notes = null;
  db_gl_chart_balance = null;
  
  constructor(px=253, py=173, pw=600, ph=350, pl=_tr("Accounts Chart Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Group(5, 5, 590, 340);
      editGroup = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(8);
      o.color(246);
      {
        {
          local o = Fl_Output(15, 30, 130, 26, _tr("ID"));
          db_gl_chart_id = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_chart", "id", db_gl_chart_id);
        }
        {
          local o = Fl_Float_Input(155, 30, 150, 25, _tr("Budget"));
          db_gl_chart_budget = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_chart", "budget", db_gl_chart_budget);
        }
        {
          local o = Fl_Check_Button(480, 22, 100, 23, _tr("Active"));
          db_gl_chart_is_active = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
          add_input_field_to_map("gl_chart", "is_active", db_gl_chart_is_active);
        }
        {
          local o = Fl_Check_Button(480, 52, 100, 23, _tr("Header"));
          db_gl_chart_is_header = o;
          o.labelsize(16);
          o.down_box(FL_DOWN_BOX);
          add_input_field_to_map("gl_chart", "is_header", db_gl_chart_is_header);
        }
        {
          local o = Fl_Input(15, 85, 130, 25, _tr("Code"));
          db_gl_chart_group_code = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_chart", "group_code", db_gl_chart_group_code);
        }
        {
          local o = Fl_Choice_Int(155, 85, 310, 25, _tr("Group"));
          db_gl_chart_gl_group_id = o;
          o.textsize(16);
          o.align(5);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
          add_input_field_to_map("gl_chart", "gl_group_id", db_gl_chart_gl_group_id);
        }
        {
          local o = Fl_Input(15, 135, 450, 25, _tr("Description"));
          db_gl_chart_description = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_chart", "description", db_gl_chart_description);
        }
        {
          local o = Fl_Choice_dbAction(480, 105, 105, 25);
          dbAction = o;
          o.textsize(16);
          o.labeltype(FL_NO_LABEL);
          o.down_box(FL_BORDER_BOX);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(480, 135, 105, 25, _tr("Action"));
          btnDbAction = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Text_Editor_Buffered(15, 190, 570, 150, _tr("Notes"));
          db_gl_chart_notes = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_chart", "notes", db_gl_chart_notes);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Check_Button(105, 165, 105, 25, _tr("Wrap lines"));
          o.align(16);
          o.labelsize(16);
          o.value(1);
          o.down_box(FL_DOWN_BOX);
          o.callback(function(sender, udata){
            db_gl_chart_notes->wrap_mode(o->value(), 0);
          });
        }
        {
          local o = Fl_Float_Input(315, 30, 150, 25, _tr("Balance"));
          db_gl_chart_balance = o;
          o.textsize(16);
          o.align(5);
          o.labelsize(16);
          add_input_field_to_map("gl_chart", "balance", db_gl_chart_balance);
        }
      }
      o.end();
    }
    end();
  }
}

class GLTransactionEditWindow extends Edit_Base_Window {
  
  // Declaration of class members
  db_gl_transactions_description = null;
  btnOrderId = null;
  db_gl_transactions_transaction_date = null;
  btnShowCalendar = null;
  group_lines = null;
  grid_lines = null;
  group_edit_line = null;
  db_gl_transactions_lines_description = null;
  db_gl_transactions_lines_group_code = null;
  btnSearchAccount = null;
  db_gl_transactions_lines_debit = null;
  db_gl_transactions_lines_credit = null;
  btnSaveLine = null;
  btnBalanceLine = null;
  btnClearLine = null;
  db_gl_transactions_lines_gl_code = null;
  db_gl_transactions_lines_doc = null;
  db_gl_transactions_lines_due_date = null;
  btnDueDateCalendar = null;
  db_gl_transactions_lines_checked_date = null;
  db_gl_transactions_id = null;
  dbAction = null;
  btnDbAction = null;
  
  constructor(px=80, py=133, pw=720, ph=410, pl=_tr("GL Transaction Edit")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Input(10, 24, 570, 25, _tr("Description"));
      db_gl_transactions_description = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      add_input_field_to_map("gl_transactions", "description", db_gl_transactions_description);
    }
    {
      local o = Fl_Button(550, 0, 30, 25, _tr("@<->"));
      btnOrderId = o;
      o.labelcolor(22);
      o.labelsize(18);
      o.tooltip(_tr("Show Associated order if any [order_id]."));
    }
    {
      local o = Fl_Input(590, 25, 125, 25, _tr("Date"));
      db_gl_transactions_transaction_date = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
      add_input_field_to_map("gl_transactions", "transaction_date", db_gl_transactions_transaction_date);
    }
    {
      local o = Fl_Button(680, 0, 30, 25, _tr("@<->"));
      btnShowCalendar = o;
      o.labelcolor(22);
      o.labelsize(18);
      o.tooltip(_tr("Show calendar"));
    }
    {
      local o = Fl_Group(10, 55, 705, 309);
      group_lines = o;
      o.box(FL_THIN_UP_BOX);
      o.labelsize(16);
      o.color(246);
      o.labeltype(FL_NO_LABEL);
      {
        {
          local o = grid_lines;
          Fl_Group.current()->add(o);
          o->resize(15, 55, 695, 150);
          o.textsize(16);
          o.align(5);
          o.labeltype(FL_NO_LABEL);
          o.labelsize(16);
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(15, 230, 695, 130);
          group_edit_line = o;
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Input(190, 230, 480, 25, _tr("Account"));
              db_gl_transactions_lines_description = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "description", db_gl_transactions_lines_description);
            }
            {
              local o = Fl_Input(20, 230, 110, 25, _tr("Code"));
              db_gl_transactions_lines_group_code = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "group_code", db_gl_transactions_lines_group_code);
            }
            {
              local o = Fl_Button(680, 230, 30, 25, _tr("@<->"));
              btnSearchAccount = o;
              o.labelcolor(22);
              o.labelsize(18);
              o.tooltip(_tr("Show calendar"));
            }
            {
              local o = Fl_Float_Input(485, 280, 110, 25, _tr("Debit"));
              db_gl_transactions_lines_debit = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "debit", db_gl_transactions_lines_debit);
            }
            {
              local o = Fl_Float_Input(600, 280, 110, 25, _tr("Credit"));
              db_gl_transactions_lines_credit = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "credit", db_gl_transactions_lines_credit);
            }
            {
              local o = Fl_Button(365, 330, 110, 25, _tr("Save Line"));
              btnSaveLine = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(485, 330, 110, 25, _tr("Balance"));
              btnBalanceLine = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(600, 330, 110, 25, _tr("Clear Line"));
              btnClearLine = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Output(140, 230, 40, 25, _tr("Grp."));
              db_gl_transactions_lines_gl_code = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "gl_code", db_gl_transactions_lines_gl_code);
            }
            {
              local o = Fl_Input(20, 280, 455, 25, _tr("Document"));
              db_gl_transactions_lines_doc = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "doc", db_gl_transactions_lines_doc);
            }
            {
              local o = Fl_Input(20, 330, 125, 25, _tr("Due Date"));
              db_gl_transactions_lines_due_date = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "due_date", db_gl_transactions_lines_due_date);
            }
            {
              local o = Fl_Button(155, 330, 30, 25, _tr("@<->"));
              btnDueDateCalendar = o;
              o.labelcolor(22);
              o.labelsize(18);
              o.tooltip(_tr("Show calendar"));
            }
            {
              local o = Fl_Input(195, 330, 125, 25, _tr("Checked Date"));
              db_gl_transactions_lines_checked_date = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              add_input_field_to_map("gl_transactions_lines", "checked_date", db_gl_transactions_lines_checked_date);
            }
          }
          o.end();
        }
      }
      o.end();
    }
    {
      local o = Fl_Output(615, 375, 100, 25, _tr("Transaction #"));
      db_gl_transactions_id = o;
      o.textsize(16);
      o.labelsize(16);
      add_input_field_to_map("gl_transactions", "id", db_gl_transactions_id);
    }
    {
      local o = Fl_Choice_dbAction(130, 374, 105, 25);
      dbAction = o;
      o.textsize(16);
      o.labeltype(FL_NO_LABEL);
      o.down_box(FL_BORDER_BOX);
      o.labelsize(16);
    }
    {
      local o = Fl_Button(15, 375, 105, 25, _tr("Action"));
      btnDbAction = o;
      o.labelsize(16);
    }
    end();
  }
}

class EditEntityWindow extends Edit_Base_Window {
  
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
    end();
  }
}

class EditProductWindow extends Edit_Base_Window {
  
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
  tabGroups = null;
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
          o.color(246);
          o.labelsize(16);
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
              o.selection_color(4);
              o.labelsize(16);
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
                  local o = Fl_Group(10, 395, 650, 156, _tr("Groups"));
                  tabGroups = o;
                  {
                    {
                      local o = Flu_Tree_Browser(10, 400, 650, 150);
                      db_products_group_id = o;
                      o.labelsize(16);
                      add_input_field_to_map("products", "group_id", db_products_group_id);
                    }
                  }
                  o.end();
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
              o.callback(function(sender, udata){
                db_products_sell_notes->wrap_mode(o->value(), 0);
                db_products_buy_notes->wrap_mode(o->value(), 0);
                db_products_tags->wrap_mode(o->value(), 0);
              });
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
          o.color(246);
          o.labelsize(16);
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
    end();
  }
}

class EditOrderWindow extends Edit_Base_Window {
  
  // Declaration of class members
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
  
  constructor(px=255, py=97, pw=800, ph=560, pl=_tr("Edit Order")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Group(5, 1, 792, 62);
      {
        {
          local o = Fl_Output(670, 5, 125, 25);
          db_orders_mdate = o;
          o.textsize(16);
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
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
      o.selection_color(4);
      o.labelsize(16);
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
                      o.textfont(1);
                      o.labelsize(16);
                      add_input_field_to_map("orders", "cash", db_orders_cash);
                      o->use_numeric_format(1);
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
                  o.labelsize(16);
                  o.labelfont(2);
                  o.selection_color(4);
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
                          o.textfont(1);
                          o.labelsize(16);
                          o.tooltip(_tr("Type partal description to search on availlable products"));
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
                          o.textsize(16);
                          o.align(5);
                          o.textfont(1);
                          o.labelsize(16);
                          o.labelfont(1);
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
                          o.tooltip(_tr("Type partial description to search on availlable products"));
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
                          o.textsize(16);
                          o.labelsize(16);
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
                          o.align(5);
                          o.labelsize(16);
                          o.textfont(1);
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
                          o.align(5);
                          o.labelsize(16);
                          o->right_to_left(1);
                        }
                        {
                          local o = Fl_Input(100, 485, 125, 25, _tr("Date"));
                          payment_date = o;
                          o.textsize(16);
                          o.align(5);
                          o.labelsize(16);
                          o.textfont(1);
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
              o.textsize(16);
              o.labeltype(FL_NO_LABEL);
              o.down_box(FL_BORDER_BOX);
              o.labelsize(16);
            }
            {
              local o = My_Fl_Return_Button(138, 525, 82, 25, _tr("Action"));
              btnDbAction = o;
              o.labelfont(1);
              o.labelsize(16);
              o.tooltip(_tr("Perform the selected action on this order"));
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
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
              add_input_field_to_map("orders", "entity_use_sales_tax2", db_orders_entity_use_sales_tax2);
            }
            {
              local o = Fl_Check_Button(585, 255, 155, 25, _tr("Sales Tax Exempt"));
              db_orders_entity_sales_tax_exempt = o;
              o.labelsize(16);
              o.down_box(FL_DOWN_BOX);
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
              o.labelsize(16);
              o.labeltype(FL_NO_LABEL);
              o.down_box(FL_BORDER_BOX);
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
              o.labelsize(16);
              o.tooltip(_tr("Type partal description to search on availlable entities"));
            }
            {
              local o = Fl_Input(260, 210, 520, 25, _tr("Email program parameters"));
              opt_email_program_command = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
              o.tooltip(_tr("Type partal description to search on availlable entities"));
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
    end();
  }
}

class PosWindow extends Edit_Base_Window {
  
  // Declaration of class members
  groupOrder = null;
  btnUser = null;
  grid_order = null;
  orderTotal = null;
  btnOrder1 = null;
  btnOrder2 = null;
  btnOrder3 = null;
  btnOrder4 = null;
  btnOrder5 = null;
  btnOrder6 = null;
  btnOrder7 = null;
  btnOrder8 = null;
  groupProducts = null;
  grid_products = null;
  btnProducts1 = null;
  btnProducts2 = null;
  btnProducts3 = null;
  btnProducts4 = null;
  groupActions = null;
  btnOptions11 = null;
  calcDisplay = null;
  btnCalc9 = null;
  btnCalc8 = null;
  btnCalc7 = null;
  btnCalc6 = null;
  btnCalc5 = null;
  btnCalc4 = null;
  btnCalc3 = null;
  btnCalc2 = null;
  btnCalc1 = null;
  btnCalc0 = null;
  btnCalcOk = null;
  btnOptions1 = null;
  btnOptions2 = null;
  btnOptions3 = null;
  btnOptions4 = null;
  btnOptions5 = null;
  btnOptions6 = null;
  btnOptions9 = null;
  btnOptions10 = null;
  btnOptions7 = null;
  btnOptions8 = null;
  groupShortCuts = null;
  btnShortcut1 = null;
  btnShortcut2 = null;
  btnShortcut3 = null;
  btnShortcut4 = null;
  btnShortcut5 = null;
  
  constructor(px=53, py=84, pw=800, ph=560, pl=_tr("Point of Sales")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Group(5, 5, 190, 550);
      groupOrder = o;
      o.box(FL_ENGRAVED_BOX);
      {
        {
          local o = Fl_Button(10, 10, 180, 30, _tr("User"));
          btnUser = o;
          o.labelfont(1);
          o.labelsize(26);
        }
        {
          local o = Fl_Data_Table(10, 48, 180, 310);
          grid_order = o;
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Output(70, 364, 120, 31, _tr("Total"));
          orderTotal = o;
          o.textsize(20);
          o.labelfont(1);
          o.labelsize(16);
          o.textfont(1);
        }
        {
          local o = Fl_Button(10, 415, 85, 30, _tr("1"));
          btnOrder1 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(105, 415, 85, 30, _tr("2"));
          btnOrder2 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(10, 450, 85, 30, _tr("3"));
          btnOrder3 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(105, 450, 85, 30, _tr("4"));
          btnOrder4 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(10, 485, 85, 30, _tr("5"));
          btnOrder5 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(105, 485, 85, 30, _tr("6"));
          btnOrder6 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(10, 520, 85, 30, _tr("7"));
          btnOrder7 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(105, 520, 85, 30, _tr("8"));
          btnOrder8 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Group(200, 5, 410, 480);
      groupProducts = o;
      o.box(FL_ENGRAVED_BOX);
      {
        {
          local o = Fl_Data_Table(205, 10, 395, 405);
          grid_products = o;
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Button(210, 425, 75, 55, _tr("@4->"));
          btnProducts1 = o;
          o.labelfont(1);
          o.labelsize(40);
        }
        {
          local o = Fl_Button(295, 425, 75, 55, _tr("@8->"));
          btnProducts2 = o;
          o.labelfont(1);
          o.labelsize(40);
        }
        {
          local o = Fl_Button(440, 425, 75, 55, _tr("@2->"));
          btnProducts3 = o;
          o.labelfont(1);
          o.labelsize(40);
        }
        {
          local o = Fl_Button(525, 425, 75, 55, _tr("@->"));
          btnProducts4 = o;
          o.labelfont(1);
          o.labelsize(40);
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(615, 5, 180, 550);
      groupActions = o;
      o.box(FL_ENGRAVED_BOX);
      {
        {
          local o = Fl_Button(620, 510, 170, 40, _tr("11"));
          btnOptions11 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Output(620, 149, 170, 31);
          calcDisplay = o;
          o.textsize(20);
          o.labelsize(16);
          o.labelfont(1);
          o.labeltype(FL_NO_LABEL);
          o.textfont(1);
        }
        {
          local o = Fl_Button(740, 185, 50, 50, _tr("9"));
          btnCalc9 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(680, 185, 50, 50, _tr("8"));
          btnCalc8 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(620, 185, 50, 50, _tr("7"));
          btnCalc7 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(740, 245, 50, 50, _tr("6"));
          btnCalc6 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(680, 245, 50, 50, _tr("5"));
          btnCalc5 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(620, 245, 50, 50, _tr("4"));
          btnCalc4 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(740, 305, 50, 50, _tr("3"));
          btnCalc3 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(680, 305, 50, 50, _tr("2"));
          btnCalc2 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(620, 305, 50, 50, _tr("1"));
          btnCalc1 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(620, 365, 50, 50, _tr("0"));
          btnCalc0 = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(680, 365, 110, 50, _tr("OK"));
          btnCalcOk = o;
          o.labelfont(1);
          o.labelsize(42);
        }
        {
          local o = Fl_Button(620, 10, 80, 35, _tr("1"));
          btnOptions1 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(710, 10, 80, 35, _tr("2"));
          btnOptions2 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(620, 55, 80, 35, _tr("3"));
          btnOptions3 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(710, 55, 80, 35, _tr("4"));
          btnOptions4 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(620, 100, 80, 35, _tr("5"));
          btnOptions5 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(710, 100, 80, 35, _tr("6"));
          btnOptions6 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(620, 465, 80, 35, _tr("9"));
          btnOptions9 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(710, 465, 80, 35, _tr("10"));
          btnOptions10 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(620, 425, 80, 35, _tr("7"));
          btnOptions7 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(710, 425, 80, 35, _tr("8"));
          btnOptions8 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Group(200, 490, 410, 65);
      groupShortCuts = o;
      o.box(FL_ENGRAVED_BOX);
      {
        {
          local o = Fl_Button(205, 495, 75, 55, _tr("User"));
          btnShortcut1 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(285, 495, 75, 55, _tr("User"));
          btnShortcut2 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(368, 495, 75, 55, _tr("User"));
          btnShortcut3 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(450, 495, 75, 55, _tr("User"));
          btnShortcut4 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
        {
          local o = Fl_Button(530, 495, 75, 55, _tr("User"));
          btnShortcut5 = o;
          o.labelfont(1);
          o.labelsize(16);
        }
      }
      o.end();
    }
    end();
  }
}

class MainWindow extends Base_Window {
  
  // Declaration of class members
  tabs = null;
  tabSales = null;
  btnOrdersSales = null;
  btnPaymentsSales = null;
  btnEntitiesSales = null;
  btnProductsSales = null;
  tabBuys = null;
  btnOrdersBuys = null;
  btnPaymentsBuys = null;
  btnProductsBuys = null;
  btnEntitiesBuys = null;
  tabAll = null;
  btnOrders = null;
  btnPayments = null;
  btnEntities = null;
  btnProducts = null;
  tabGL = null;
  btnGLGroups = null;
  btnGLChart = null;
  btnGLTransactions = null;
  tabConfig = null;
  config_scroll = null;
  config_pack = null;
  btnOrdersSum = null;
  btnSalesTaxRates = null;
  btnOrderTypes = null;
  btnPaymentTypes = null;
  btnMeasureUnits = null;
  btnWarrantyTypes = null;
  btnImages = null;
  btnProductGroups = null;
  btnEntityGroups = null;
  btnConfig = null;
  btnOpenDB = null;
  btnTranslations = null;
  btnAppUsers = null;
  btnAskForPrinter = null;
  btnWebServer = null;
  
  constructor(px=10, py=50, pw=330, ph=320, pl=_tr("OURBIZ")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Tabs(5, 5, 320, 310);
      tabs = o;
      o.labelcolor(7);
      o.labelsize(16);
      o.selection_color(4);
      {
        {
          local o = Fl_Group(5, 35, 320, 280, _tr("Sales"));
          tabSales = o;
          o.labelsize(16);
          o.color(127);
          {
            {
              local o = Fl_Button(30, 50, 275, 55, _tr("Sales"));
              btnOrdersSales = o;
              o.align(16);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
              o.labelfont(3);
              o.labelcolor(8);
            }
            {
              local o = Fl_Button(30, 116, 275, 55, _tr("Payments"));
              btnPaymentsSales = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
            {
              local o = Fl_Button(30, 183, 275, 55, _tr("Customers"));
              btnEntitiesSales = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
            {
              local o = Fl_Button(30, 250, 275, 55, _tr("Products"));
              btnProductsSales = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 35, 320, 280, _tr("Buys"));
          tabBuys = o;
          o.labelsize(16);
          o.color(174);
          {
            {
              local o = Fl_Button(30, 50, 275, 55, _tr("Buys"));
              btnOrdersBuys = o;
              o.align(16);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
              o.labelfont(3);
              o.labelcolor(8);
            }
            {
              local o = Fl_Button(30, 115, 275, 55, _tr("Payments"));
              btnPaymentsBuys = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
            {
              local o = Fl_Button(30, 250, 275, 55, _tr("Products"));
              btnProductsBuys = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
            {
              local o = Fl_Button(30, 183, 275, 55, _tr("Suppliers"));
              btnEntitiesBuys = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 35, 320, 280, _tr("All"));
          tabAll = o;
          o.labelsize(16);
          o.color(239);
          {
            {
              local o = Fl_Button(30, 50, 275, 55, _tr("Orders"));
              btnOrders = o;
              o.align(16);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
              o.labelfont(3);
              o.labelcolor(8);
            }
            {
              local o = Fl_Button(30, 116, 275, 55, _tr("Payments"));
              btnPayments = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
            {
              local o = Fl_Button(30, 183, 275, 55, _tr("Entities"));
              btnEntities = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
            {
              local o = Fl_Button(30, 250, 275, 55, _tr("Products"));
              btnProducts = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
          }
          o.end();
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Group(5, 35, 320, 280, _tr("GL"));
          tabGL = o;
          o.labelsize(16);
          o.color(132);
          {
            {
              local o = Fl_Button(30, 50, 275, 55, _tr("Groups"));
              btnGLGroups = o;
              o.align(16);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
              o.labelfont(3);
              o.labelcolor(8);
            }
            {
              local o = Fl_Button(30, 115, 275, 55, _tr("Chart"));
              btnGLChart = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
            {
              local o = Fl_Button(30, 180, 275, 55, _tr("Transactions"));
              btnGLTransactions = o;
              o.labelcolor(8);
              o.labelfont(3);
              o.labeltype(FL_SHADOW_LABEL);
              o.labelsize(34);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(5, 35, 320, 280, _tr("Config"));
          tabConfig = o;
          o.labelsize(16);
          {
            {
              local o = Fl_Scroll(10, 45, 310, 265);
              config_scroll = o;
              o.labelsize(16);
              {
                {
                  local o = Fl_Pack(30, 45, 270, 260);
                  config_pack = o;
                  o.labelsize(16);
                  o->spacing(5);
                  {
                    {
                      local o = Fl_Button(65, 55, 64, 30, _tr("Orders Sum"));
                      btnOrdersSum = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(55, 45, 64, 30, _tr("Sales Tax Rates"));
                      btnSalesTaxRates = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(65, 80, 64, 30, _tr("Order Types"));
                      btnOrderTypes = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(105, 148, 64, 30, _tr("Payment Types"));
                      btnPaymentTypes = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(115, 158, 64, 30, _tr("Measure Units"));
                      btnMeasureUnits = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(125, 168, 64, 30, _tr("Warranty Types"));
                      btnWarrantyTypes = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(75, 118, 64, 30, _tr("Images"));
                      btnImages = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(85, 128, 64, 30, _tr("Product Groups"));
                      btnProductGroups = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(95, 138, 64, 30, _tr("Entity Groups"));
                      btnEntityGroups = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(105, 148, 64, 30, _tr("App Config"));
                      btnConfig = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(115, 158, 64, 30, _tr("Open Database"));
                      btnOpenDB = o;
                      o.labelsize(16);
                      o.tooltip(_tr("Open a database file"));
                    }
                    {
                      local o = Fl_Button(115, 158, 64, 30, _tr("Translations"));
                      btnTranslations = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Button(75, 65, 64, 30, _tr("App Users"));
                      btnAppUsers = o;
                      o.labelsize(16);
                    }
                    {
                      local o = Fl_Check_Button(55, 70, 64, 15, _tr("Ask for Printer"));
                      btnAskForPrinter = o;
                      o.down_box(FL_DOWN_BOX);
                    }
                    {
                      local o = Fl_Check_Button(65, 80, 64, 15, _tr("Web Server"));
                      btnWebServer = o;
                      o.down_box(FL_DOWN_BOX);
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
      }
      o.end();
    }
    end();
  }
}

