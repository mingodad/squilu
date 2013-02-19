//class InvoiceA4 extends Fl_Group {
class InvoiceA4 extends Fl_Window {
	page=null;
	pageDesignGroup=null;
	logoGroupH=null;
	logoH=null;
	companyBoxH=null;
	logoGroupV=null;
	logoV=null;
	companyBoxHV=null;
	companyBoxV=null;
	logoGroupHV=null;
	logoGroupHV=null;
	logoHV1=null;
	logoHV2=null;
	OrderNumber=null;
	box_header_number_date=null;
	box_data_number_date=null;
	header_date_box=null;
	box_data_date_box=null;
	box_to=null;
	orderLines=null;
	header_lines_code_box=null;
	header_lines_description_box=null;
	header_lines_quantity_box=null;
	header_lines_price_box=null;
	header_lines_discount_box=null;
	header_lines_sales_tax1_box=null;
	header_lines_sales_tax2_box=null;
	header_lines_subtotal_box=null;
	designLinesGroup=null;
	data_lines_code_box=null;
	data_lines_description_box=null;
	data_lines_quantity_box=null;
	data_tlines_price_box=null;
	data_lines_discount_box=null;
	data_lines_sales_tax1_box=null;
	data_lines_sales_tax2_box=null;
	data_lines_subtotal_box=null;
	orderTotals=null;
	header_PAGENO_box=null;
	box_data_pageno_box=null;
	header_totals_subtotal_box=null;
	data_totals_subtotal_box=null;
	header_totals_total_sales_tax1_box=null;
	data_totals_total_sales_tax1_box=null;
	header_totals_total_sales_tax2_box=null;
	data_totals_total_sales_tax2_box=null;
	header_totals_total_box=null;
	data_totals_total_box=null;
	notes_box=null;
	printDataGroup=null;
	orderTypeBox=null;
	numberBox=null;
	dateBox=null;
	entityBox=null;
	dataLinesGroup=null;
	codeBox=null;
	descriptionBox=null;
	quantityBox=null;
	priceBox=null;
	discountBox=null;
	sales_tax1Box=null;
	sales_tax2Box=null;
	lineTotalBox=null;
	bottomTotals=null;
	orderPageBox=null;
	fisrtTotalBox=null;
	sales_tax1AmountBox=null;
	sales_tax2AmountBox=null;
	totalBox=null;
	
constructor() {
	base.constructor(10, 50, 634, 686, _tr("InvoiceA4"));
begin();
this->box(FL_FLAT_BOX);
this->color(FL_BACKGROUND_COLOR);
this->selection_color(FL_BACKGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(14);
this->labelcolor(FL_FOREGROUND_COLOR);
this->align((FL_ALIGN_TOP));
this->when(FL_WHEN_RELEASE);
{ page = new Fl_Group(0, 0, 634, 686);
  page->color(FL_BACKGROUND2_COLOR);
  { pageDesignGroup = new Fl_Group(2, 0, 630, 686);
    pageDesignGroup->labeltype(FL_NO_LABEL);
    { logoGroupH = new Fl_Group(15, 10, 316, 130);
      logoGroupH->hide();
      { logoH = new Fl_Box(16, 10, 310, 50, _tr("FLTK"));
        logoH->labeltype(FL_SHADOW_LABEL);
        logoH->labelfont(3);
        logoH->labelsize(40);
        logoH->align((644|FL_ALIGN_INSIDE));
      } // Fl_Box* logoH
      { companyBoxH = new Fl_Box_ClearLabel(16, 65, 308, 75, _tr(@"FLTK (pronounced ""fulltick"") is a cross-platform C++ GUI toolkit for UNIX\
\302\256/Linux\302\256 (X11), Microsoft\302\256 Windows\302\256, and MacOS\
\302\256 X. FLTK provides modern GUI functionality without the bloat and suppo\
rts 3D graphics via OpenGL\302\256 and its built-in GLUT emulation."));
        companyBoxH->box(FL_NO_BOX);
        companyBoxH->color(FL_BACKGROUND_COLOR);
        companyBoxH->selection_color(FL_BACKGROUND_COLOR);
        companyBoxH->labeltype(FL_NORMAL_LABEL);
        companyBoxH->labelfont(2);
        companyBoxH->labelsize(10);
        companyBoxH->labelcolor(FL_FOREGROUND_COLOR);
        companyBoxH->align((FL_ALIGN_WRAP|FL_ALIGN_INSIDE));
        companyBoxH->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* companyBoxH
      logoGroupH->end();
    } // Fl_Group* logoGroupH
    { logoGroupV = new Fl_Group(15, 10, 316, 130);
      logoGroupV->hide();
      { logoV = new Fl_Box(16, 10, 100, 130, _tr("FLTK"));
        logoV->labeltype(FL_SHADOW_LABEL);
        logoV->labelfont(3);
        logoV->labelsize(40);
        logoV->align((644|FL_ALIGN_INSIDE));
      } // Fl_Box* logoV
      { companyBoxV = new Fl_Box_ClearLabel(120, 10, 210, 130, _tr(@"FLTK (pronounced ""fulltick"") is a cross-platform C++ GUI toolkit for UNIX\
\302\256/Linux\302\256 (X11), Microsoft\302\256 Windows\302\256, and MacOS\
\302\256 X. FLTK provides modern GUI functionality without the bloat and suppo\
rts 3D graphics via OpenGL\302\256 and its built-in GLUT emulation."));
        companyBoxV->box(FL_NO_BOX);
        companyBoxV->color(FL_BACKGROUND_COLOR);
        companyBoxV->selection_color(FL_BACKGROUND_COLOR);
        companyBoxV->labeltype(FL_NORMAL_LABEL);
        companyBoxV->labelfont(2);
        companyBoxV->labelsize(10);
        companyBoxV->labelcolor(FL_FOREGROUND_COLOR);
        companyBoxV->align((FL_ALIGN_WRAP|FL_ALIGN_INSIDE));
        companyBoxV->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* companyBoxV
      logoGroupV->end();
    } // Fl_Group* logoGroupV
    { logoGroupHV = new Fl_Group(15, 10, 316, 130);
      { logoHV1 = new Fl_Box(16, 10, 100, 130, _tr("FLTK"));
        logoHV1->labeltype(FL_SHADOW_LABEL);
        logoHV1->labelfont(3);
        logoHV1->labelsize(40);
        logoHV1->align((644|FL_ALIGN_INSIDE));
      } // Fl_Box* logoHV1
      { logoHV2 = new Fl_Box(120, 10, 210, 50, _tr("FLTK"));
        logoHV2->labeltype(FL_SHADOW_LABEL);
        logoHV2->labelfont(3);
        logoHV2->labelsize(40);
        logoHV2->align((644|FL_ALIGN_INSIDE));
      } // Fl_Box* logoHV2
      { companyBoxHV = new Fl_Box_ClearLabel(120, 50, 210, 90, _tr(@"FLTK (pronounced ""fulltick"") is a cross-platform C++ GUI toolkit for UNIX\
\302\256/Linux\302\256 (X11), Microsoft\302\256 Windows\302\256, and MacOS\
\302\256 X. FLTK provides modern GUI functionality without the bloat and suppo\
rts 3D graphics via OpenGL\302\256 and its built-in GLUT emulation."));
        companyBoxHV->box(FL_NO_BOX);
        companyBoxHV->color(FL_BACKGROUND_COLOR);
        companyBoxHV->selection_color(FL_BACKGROUND_COLOR);
        companyBoxHV->labeltype(FL_NORMAL_LABEL);
        companyBoxHV->labelfont(2);
        companyBoxHV->labelsize(9);
        companyBoxHV->labelcolor(FL_FOREGROUND_COLOR);
        companyBoxHV->align((FL_ALIGN_WRAP|FL_ALIGN_INSIDE));
        companyBoxHV->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* companyBoxHV
      logoGroupHV->end();
    } // Fl_Group* logoGroupHV
    { OrderNumber = new Fl_Group(336, 10, 286, 40);
      OrderNumber->box(FL_FLAT_BOX);
      OrderNumber->color(FL_DARK3);
      OrderNumber->labeltype(FL_NO_LABEL);
      { box_header_number_date = new Fl_Box(337, 11, 142, 18);
        box_header_number_date->box(FL_FLAT_BOX);
        box_header_number_date->color(FL_DARK1);
        box_header_number_date->labeltype(FL_NO_LABEL);
        box_header_number_date->labelsize(12);
      } // Fl_Box* box_header_number_date
      { box_data_number_date = new Fl_Box(337, 30, 142, 19);
        box_data_number_date->box(FL_FLAT_BOX);
        box_data_number_date->color(FL_BACKGROUND2_COLOR);
        box_data_number_date->labeltype(FL_NO_LABEL);
        box_data_number_date->labelsize(12);
      } // Fl_Box* box_data_number_date
      { header_date_box = new Fl_Box(480, 11, 141, 18, _tr("DATE"));
        header_date_box->box(FL_FLAT_BOX);
        header_date_box->color(FL_DARK1);
        header_date_box->labelfont(1);
        header_date_box->labelsize(12);
      } // Fl_Box* header_date_box
      { box_data_date_box = new Fl_Box(480, 30, 141, 19);
        box_data_date_box->box(FL_FLAT_BOX);
        box_data_date_box->color(FL_BACKGROUND2_COLOR);
        box_data_date_box->labeltype(FL_NO_LABEL);
        box_data_date_box->labelfont(1);
        box_data_date_box->labelsize(12);
      } // Fl_Box* box_data_date_box
      OrderNumber->end();
    } // Fl_Group* OrderNumber
    { box_to = new Fl_Box(336, 55, 286, 85);
      box_to->box(FL_BORDER_FRAME);
      box_to->color(FL_DARK3);
      box_to->labelfont(1);
      box_to->labelsize(16);
      box_to->align((FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));
    } // Fl_Box* box_to
    { orderLines = new Fl_Group(16, 148, 606, 433);
      orderLines->box(FL_FLAT_BOX);
      orderLines->color(FL_DARK3);
      orderLines->labeltype(FL_NO_LABEL);
      { header_lines_code_box = new Fl_Box(17, 149, 46, 20, _tr("CODE"));
        header_lines_code_box->box(FL_FLAT_BOX);
        header_lines_code_box->color(FL_DARK1);
        header_lines_code_box->labelfont(1);
        header_lines_code_box->labelsize(10);
      } // Fl_Box* header_lines_code_box
      { header_lines_description_box = new Fl_Box(64, 149, 300, 20, _tr("DESCRIPTION"));
        header_lines_description_box->box(FL_FLAT_BOX);
        header_lines_description_box->color(FL_DARK1);
        header_lines_description_box->labelfont(1);
        header_lines_description_box->labelsize(10);
      } // Fl_Box* header_lines_description_box
      { header_lines_quantity_box = new Fl_Box(365, 149, 40, 20, _tr("QTY."));
        header_lines_quantity_box->box(FL_FLAT_BOX);
        header_lines_quantity_box->color(FL_DARK1);
        header_lines_quantity_box->labelfont(1);
        header_lines_quantity_box->labelsize(10);
      } // Fl_Box* header_lines_quantity_box
      { header_lines_price_box = new Fl_Box(406, 149, 60, 20, _tr("PRICE"));
        header_lines_price_box->box(FL_FLAT_BOX);
        header_lines_price_box->color(FL_DARK1);
        header_lines_price_box->labelfont(1);
        header_lines_price_box->labelsize(10);
      } // Fl_Box* header_lines_price_box
      { header_lines_discount_box = new Fl_Box(467, 149, 30, 20, _tr("DISC. %"));
        header_lines_discount_box->box(FL_FLAT_BOX);
        header_lines_discount_box->color(FL_DARK1);
        header_lines_discount_box->labelfont(1);
        header_lines_discount_box->labelsize(8);
        header_lines_discount_box->align((FL_ALIGN_WRAP));
      } // Fl_Box* header_lines_discount_box
      { header_lines_sales_tax1_box = new Fl_Box(498, 149, 30, 20, _tr("S.T. I %"));
        header_lines_sales_tax1_box->box(FL_FLAT_BOX);
        header_lines_sales_tax1_box->color(FL_DARK1);
        header_lines_sales_tax1_box->labelfont(1);
        header_lines_sales_tax1_box->labelsize(8);
        header_lines_sales_tax1_box->align((FL_ALIGN_WRAP));
      } // Fl_Box* header_lines_sales_tax1_box
      { header_lines_sales_tax2_box = new Fl_Box(529, 149, 30, 20, _tr("S.T. II %"));
        header_lines_sales_tax2_box->box(FL_FLAT_BOX);
        header_lines_sales_tax2_box->color(FL_DARK1);
        header_lines_sales_tax2_box->labelfont(1);
        header_lines_sales_tax2_box->labelsize(8);
        header_lines_sales_tax2_box->align((FL_ALIGN_WRAP));
      } // Fl_Box* header_lines_sales_tax2_box
      { header_lines_subtotal_box = new Fl_Box(560, 149, 61, 20, _tr("SUBTOTAL"));
        header_lines_subtotal_box->box(FL_FLAT_BOX);
        header_lines_subtotal_box->color(FL_DARK1);
        header_lines_subtotal_box->labelfont(1);
        header_lines_subtotal_box->labelsize(10);
      } // Fl_Box* header_lines_subtotal_box
      { designLinesGroup = new Fl_Group(17, 170, 604, 410);
        designLinesGroup->labeltype(FL_NO_LABEL);
        { data_lines_code_box = new Fl_Box(17, 170, 46, 410);
          data_lines_code_box->box(FL_FLAT_BOX);
          data_lines_code_box->color(FL_BACKGROUND2_COLOR);
          data_lines_code_box->labeltype(FL_NO_LABEL);
          data_lines_code_box->labelfont(1);
          data_lines_code_box->labelsize(10);
        } // Fl_Box* data_lines_code_box
        { data_lines_description_box = new Fl_Box(64, 170, 300, 410);
          data_lines_description_box->box(FL_FLAT_BOX);
          data_lines_description_box->color(FL_BACKGROUND2_COLOR);
          data_lines_description_box->labeltype(FL_NO_LABEL);
          data_lines_description_box->labelfont(1);
          data_lines_description_box->labelsize(10);
        } // Fl_Box* data_lines_description_box
        { data_lines_quantity_box = new Fl_Box(365, 170, 40, 410);
          data_lines_quantity_box->box(FL_FLAT_BOX);
          data_lines_quantity_box->color(FL_BACKGROUND2_COLOR);
          data_lines_quantity_box->labeltype(FL_NO_LABEL);
          data_lines_quantity_box->labelfont(1);
          data_lines_quantity_box->labelsize(10);
        } // Fl_Box* data_lines_quantity_box
        { data_tlines_price_box = new Fl_Box(406, 170, 60, 410);
          data_tlines_price_box->box(FL_FLAT_BOX);
          data_tlines_price_box->color(FL_BACKGROUND2_COLOR);
          data_tlines_price_box->labeltype(FL_NO_LABEL);
          data_tlines_price_box->labelfont(1);
          data_tlines_price_box->labelsize(10);
        } // Fl_Box* data_tlines_price_box
        { data_lines_discount_box = new Fl_Box(467, 170, 30, 410);
          data_lines_discount_box->box(FL_FLAT_BOX);
          data_lines_discount_box->color(FL_BACKGROUND2_COLOR);
          data_lines_discount_box->labeltype(FL_NO_LABEL);
          data_lines_discount_box->labelfont(1);
          data_lines_discount_box->labelsize(10);
        } // Fl_Box* data_lines_discount_box
        { data_lines_sales_tax1_box = new Fl_Box(498, 170, 30, 410);
          data_lines_sales_tax1_box->box(FL_FLAT_BOX);
          data_lines_sales_tax1_box->color(FL_BACKGROUND2_COLOR);
          data_lines_sales_tax1_box->labeltype(FL_NO_LABEL);
          data_lines_sales_tax1_box->labelfont(1);
          data_lines_sales_tax1_box->labelsize(10);
        } // Fl_Box* data_lines_sales_tax1_box
        { data_lines_sales_tax2_box = new Fl_Box(529, 170, 30, 410);
          data_lines_sales_tax2_box->box(FL_FLAT_BOX);
          data_lines_sales_tax2_box->color(FL_BACKGROUND2_COLOR);
          data_lines_sales_tax2_box->labeltype(FL_NO_LABEL);
          data_lines_sales_tax2_box->labelfont(1);
          data_lines_sales_tax2_box->labelsize(10);
        } // Fl_Box* data_lines_sales_tax2_box
        { data_lines_subtotal_box = new Fl_Box(560, 170, 61, 410);
          data_lines_subtotal_box->box(FL_FLAT_BOX);
          data_lines_subtotal_box->color(FL_BACKGROUND2_COLOR);
          data_lines_subtotal_box->labeltype(FL_NO_LABEL);
          data_lines_subtotal_box->labelfont(1);
          data_lines_subtotal_box->labelsize(10);
        } // Fl_Box* data_lines_subtotal_box
        designLinesGroup->end();
        Fl_Group.current()->resizable(designLinesGroup);
      } // Fl_Group* designLinesGroup
      orderLines->end();
      Fl_Group.current()->resizable(orderLines);
    } // Fl_Group* orderLines
    { orderTotals = new Fl_Group(16, 585, 606, 46);
      orderTotals->box(FL_FLAT_BOX);
      orderTotals->color(FL_DARK3);
      orderTotals->labeltype(FL_NO_LABEL);
      { header_PAGENO_box = new Fl_Box(17, 586, 65, 20, _tr("PAGE"));
        header_PAGENO_box->box(FL_FLAT_BOX);
        header_PAGENO_box->color(FL_DARK1);
        header_PAGENO_box->labelfont(1);
        header_PAGENO_box->labelsize(12);
      } // Fl_Box* header_PAGENO_box
      { box_data_pageno_box = new Fl_Box(17, 607, 65, 23);
        box_data_pageno_box->box(FL_FLAT_BOX);
        box_data_pageno_box->color(FL_BACKGROUND2_COLOR);
        box_data_pageno_box->labeltype(FL_NO_LABEL);
        box_data_pageno_box->labelfont(1);
        box_data_pageno_box->labelsize(12);
      } // Fl_Box* box_data_pageno_box
      { header_totals_subtotal_box = new Fl_Box(83, 586, 150, 20, _tr("SUBTOTAL"));
        header_totals_subtotal_box->box(FL_FLAT_BOX);
        header_totals_subtotal_box->color(FL_DARK1);
        header_totals_subtotal_box->labelfont(1);
        header_totals_subtotal_box->labelsize(12);
      } // Fl_Box* header_totals_subtotal_box
      { data_totals_subtotal_box = new Fl_Box(83, 607, 150, 23);
        data_totals_subtotal_box->box(FL_FLAT_BOX);
        data_totals_subtotal_box->color(FL_BACKGROUND2_COLOR);
        data_totals_subtotal_box->labeltype(FL_NO_LABEL);
        data_totals_subtotal_box->labelfont(1);
        data_totals_subtotal_box->labelsize(12);
      } // Fl_Box* data_totals_subtotal_box
      { header_totals_total_sales_tax1_box = new Fl_Box(234, 586, 120, 20, _tr("Sales Tax I"));
        header_totals_total_sales_tax1_box->box(FL_FLAT_BOX);
        header_totals_total_sales_tax1_box->color(FL_DARK1);
        header_totals_total_sales_tax1_box->labelfont(1);
        header_totals_total_sales_tax1_box->labelsize(12);
      } // Fl_Box* header_totals_total_sales_tax1_box
      { data_totals_total_sales_tax1_box = new Fl_Box(234, 607, 120, 23);
        data_totals_total_sales_tax1_box->box(FL_FLAT_BOX);
        data_totals_total_sales_tax1_box->color(FL_BACKGROUND2_COLOR);
        data_totals_total_sales_tax1_box->labeltype(FL_NO_LABEL);
        data_totals_total_sales_tax1_box->labelfont(1);
        data_totals_total_sales_tax1_box->labelsize(12);
      } // Fl_Box* data_totals_total_sales_tax1_box
      { header_totals_total_sales_tax2_box = new Fl_Box(355, 586, 115, 20, _tr("Sales Tax II"));
        header_totals_total_sales_tax2_box->box(FL_FLAT_BOX);
        header_totals_total_sales_tax2_box->color(FL_DARK1);
        header_totals_total_sales_tax2_box->labelfont(1);
        header_totals_total_sales_tax2_box->labelsize(12);
      } // Fl_Box* header_totals_total_sales_tax2_box
      { data_totals_total_sales_tax2_box = new Fl_Box(355, 607, 115, 23);
        data_totals_total_sales_tax2_box->box(FL_FLAT_BOX);
        data_totals_total_sales_tax2_box->color(FL_BACKGROUND2_COLOR);
        data_totals_total_sales_tax2_box->labeltype(FL_NO_LABEL);
        data_totals_total_sales_tax2_box->labelfont(1);
        data_totals_total_sales_tax2_box->labelsize(12);
      } // Fl_Box* data_totals_total_sales_tax2_box
      { header_totals_total_box = new Fl_Box(471, 586, 150, 20, _tr("TOTAL"));
        header_totals_total_box->box(FL_FLAT_BOX);
        header_totals_total_box->color(FL_DARK1);
        header_totals_total_box->labelfont(1);
        header_totals_total_box->labelsize(12);
      } // Fl_Box* header_totals_total_box
      { data_totals_total_box = new Fl_Box(471, 607, 150, 23);
        data_totals_total_box->box(FL_FLAT_BOX);
        data_totals_total_box->color(FL_BACKGROUND2_COLOR);
        data_totals_total_box->labeltype(FL_NO_LABEL);
        data_totals_total_box->labelfont(1);
        data_totals_total_box->labelsize(12);
      } // Fl_Box* data_totals_total_box
      orderTotals->end();
    } // Fl_Group* orderTotals
    { notes_box = new Fl_Box_ClearLabel(17, 638, 606, 37, _tr(@"FLTK is provided under the terms of the GNU Library Public License, Version 2\
 with exceptions that allow for static linking."));
      notes_box->box(FL_NO_BOX);
      notes_box->color(FL_BACKGROUND_COLOR);
      notes_box->selection_color(FL_BACKGROUND_COLOR);
      notes_box->labeltype(FL_NORMAL_LABEL);
      notes_box->labelfont(2);
      notes_box->labelsize(9);
      notes_box->labelcolor(FL_FOREGROUND_COLOR);
      notes_box->align((FL_ALIGN_WRAP|FL_ALIGN_INSIDE));
      notes_box->when(FL_WHEN_RELEASE);
    } // Fl_Box_ClearLabel* notes_box
    pageDesignGroup->end();
  } // Fl_Group* pageDesignGroup
  { printDataGroup = new Fl_Group(3, 0, 628, 637);
    printDataGroup->labeltype(FL_NO_LABEL);
    { orderTypeBox = new Fl_Box(338, 10, 139, 20);
      orderTypeBox->labelfont(1);
      orderTypeBox->labelsize(12);
    } // Fl_Box* orderTypeBox
    { numberBox = new Fl_Box_ClearLabel(338, 33, 139, 16);
      numberBox->box(FL_NO_BOX);
      numberBox->color(FL_BACKGROUND_COLOR);
      numberBox->selection_color(FL_BACKGROUND_COLOR);
      numberBox->labeltype(FL_NORMAL_LABEL);
      numberBox->labelfont(5);
      numberBox->labelsize(15);
      numberBox->labelcolor(FL_FOREGROUND_COLOR);
      numberBox->align((FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
      numberBox->when(FL_WHEN_RELEASE);
    } // Fl_Box_ClearLabel* numberBox
    { dateBox = new Fl_Box_ClearLabel(484, 32, 133, 16);
      dateBox->box(FL_NO_BOX);
      dateBox->color(FL_BACKGROUND_COLOR);
      dateBox->selection_color(FL_BACKGROUND_COLOR);
      dateBox->labeltype(FL_NORMAL_LABEL);
      dateBox->labelfont(5);
      dateBox->labelsize(15);
      dateBox->labelcolor(FL_FOREGROUND_COLOR);
      dateBox->align((FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
      dateBox->when(FL_WHEN_RELEASE);
    } // Fl_Box_ClearLabel* dateBox
    { entityBox = new Fl_Box_ClearLabel(338, 57, 282, 82);
      entityBox->box(FL_NO_BOX);
      entityBox->color(FL_BACKGROUND_COLOR);
      entityBox->selection_color(FL_BACKGROUND_COLOR);
      entityBox->labeltype(FL_NORMAL_LABEL);
      entityBox->labelfont(0);
      entityBox->labelsize(12);
      entityBox->labelcolor(FL_FOREGROUND_COLOR);
      entityBox->align((69|FL_ALIGN_INSIDE));
      entityBox->when(FL_WHEN_RELEASE);
    } // Fl_Box_ClearLabel* entityBox
    { dataLinesGroup = new Fl_Group(16, 169, 605, 412);
      dataLinesGroup->labeltype(FL_NO_LABEL);
      dataLinesGroup->align((FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
      { codeBox = new Fl_Box_ClearLabel(16, 174, 46, 404);
        codeBox->box(FL_NO_BOX);
        codeBox->color(FL_BACKGROUND_COLOR);
        codeBox->selection_color(FL_BACKGROUND_COLOR);
        codeBox->labeltype(FL_NORMAL_LABEL);
        codeBox->labelfont(0);
        codeBox->labelsize(11);
        codeBox->labelcolor(FL_FOREGROUND_COLOR);
        codeBox->align((FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE));
        codeBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* codeBox
      { descriptionBox = new Fl_Box_ClearLabel(64, 174, 300, 404);
        descriptionBox->box(FL_NO_BOX);
        descriptionBox->color(FL_BACKGROUND_COLOR);
        descriptionBox->selection_color(FL_BACKGROUND_COLOR);
        descriptionBox->labeltype(FL_NORMAL_LABEL);
        descriptionBox->labelfont(0);
        descriptionBox->labelsize(11);
        descriptionBox->labelcolor(FL_FOREGROUND_COLOR);
        descriptionBox->align((69|FL_ALIGN_INSIDE));
        descriptionBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* descriptionBox
      { quantityBox = new Fl_Box_ClearLabel(365, 174, 40, 404);
        quantityBox->box(FL_NO_BOX);
        quantityBox->color(FL_BACKGROUND_COLOR);
        quantityBox->selection_color(FL_BACKGROUND_COLOR);
        quantityBox->labeltype(FL_NORMAL_LABEL);
        quantityBox->labelfont(0);
        quantityBox->labelsize(11);
        quantityBox->labelcolor(FL_FOREGROUND_COLOR);
        quantityBox->align((FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE));
        quantityBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* quantityBox
      { priceBox = new Fl_Box_ClearLabel(406, 174, 60, 404);
        priceBox->box(FL_NO_BOX);
        priceBox->color(FL_BACKGROUND_COLOR);
        priceBox->selection_color(FL_BACKGROUND_COLOR);
        priceBox->labeltype(FL_NORMAL_LABEL);
        priceBox->labelfont(0);
        priceBox->labelsize(11);
        priceBox->labelcolor(FL_FOREGROUND_COLOR);
        priceBox->align((FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE));
        priceBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* priceBox
      { discountBox = new Fl_Box_ClearLabel(467, 174, 30, 404);
        discountBox->box(FL_NO_BOX);
        discountBox->color(FL_BACKGROUND_COLOR);
        discountBox->selection_color(FL_BACKGROUND_COLOR);
        discountBox->labeltype(FL_NORMAL_LABEL);
        discountBox->labelfont(0);
        discountBox->labelsize(11);
        discountBox->labelcolor(FL_FOREGROUND_COLOR);
        discountBox->align((FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE));
        discountBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* discountBox
      { sales_tax1Box = new Fl_Box_ClearLabel(498, 174, 30, 404);
        sales_tax1Box->box(FL_NO_BOX);
        sales_tax1Box->color(FL_BACKGROUND_COLOR);
        sales_tax1Box->selection_color(FL_BACKGROUND_COLOR);
        sales_tax1Box->labeltype(FL_NORMAL_LABEL);
        sales_tax1Box->labelfont(0);
        sales_tax1Box->labelsize(11);
        sales_tax1Box->labelcolor(FL_FOREGROUND_COLOR);
        sales_tax1Box->align((FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE));
        sales_tax1Box->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* sales_tax1Box
      { sales_tax2Box = new Fl_Box_ClearLabel(529, 174, 30, 404);
        sales_tax2Box->box(FL_NO_BOX);
        sales_tax2Box->color(FL_BACKGROUND_COLOR);
        sales_tax2Box->selection_color(FL_BACKGROUND_COLOR);
        sales_tax2Box->labeltype(FL_NORMAL_LABEL);
        sales_tax2Box->labelfont(0);
        sales_tax2Box->labelsize(11);
        sales_tax2Box->labelcolor(FL_FOREGROUND_COLOR);
        sales_tax2Box->align((FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE));
        sales_tax2Box->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* sales_tax2Box
      { lineTotalBox = new Fl_Box_ClearLabel(560, 174, 61, 404);
        lineTotalBox->box(FL_NO_BOX);
        lineTotalBox->color(FL_BACKGROUND_COLOR);
        lineTotalBox->selection_color(FL_BACKGROUND_COLOR);
        lineTotalBox->labeltype(FL_NORMAL_LABEL);
        lineTotalBox->labelfont(0);
        lineTotalBox->labelsize(11);
        lineTotalBox->labelcolor(FL_FOREGROUND_COLOR);
        lineTotalBox->align((FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE));
        lineTotalBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* lineTotalBox
      dataLinesGroup->end();
      Fl_Group.current()->resizable(dataLinesGroup);
    } // Fl_Group* dataLinesGroup
    { bottomTotals = new Fl_Group(16, 585, 605, 48);
      bottomTotals->labeltype(FL_NO_LABEL);
      { orderPageBox = new Fl_Box_ClearLabel(16, 608, 65, 25);
        orderPageBox->box(FL_NO_BOX);
        orderPageBox->color(FL_BACKGROUND_COLOR);
        orderPageBox->selection_color(FL_BACKGROUND_COLOR);
        orderPageBox->labeltype(FL_NORMAL_LABEL);
        orderPageBox->labelfont(5);
        orderPageBox->labelsize(15);
        orderPageBox->labelcolor(FL_FOREGROUND_COLOR);
        orderPageBox->align((FL_ALIGN_CENTER));
        orderPageBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* orderPageBox
      { fisrtTotalBox = new Fl_Box_ClearLabel(83, 608, 150, 25);
        fisrtTotalBox->box(FL_NO_BOX);
        fisrtTotalBox->color(FL_BACKGROUND_COLOR);
        fisrtTotalBox->selection_color(FL_BACKGROUND_COLOR);
        fisrtTotalBox->labeltype(FL_NORMAL_LABEL);
        fisrtTotalBox->labelfont(5);
        fisrtTotalBox->labelsize(15);
        fisrtTotalBox->labelcolor(FL_FOREGROUND_COLOR);
        fisrtTotalBox->align((FL_ALIGN_CENTER));
        fisrtTotalBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* fisrtTotalBox
      { sales_tax1AmountBox = new Fl_Box_ClearLabel(234, 608, 120, 25);
        sales_tax1AmountBox->box(FL_NO_BOX);
        sales_tax1AmountBox->color(FL_BACKGROUND_COLOR);
        sales_tax1AmountBox->selection_color(FL_BACKGROUND_COLOR);
        sales_tax1AmountBox->labeltype(FL_NORMAL_LABEL);
        sales_tax1AmountBox->labelfont(5);
        sales_tax1AmountBox->labelsize(15);
        sales_tax1AmountBox->labelcolor(FL_FOREGROUND_COLOR);
        sales_tax1AmountBox->align((FL_ALIGN_CENTER));
        sales_tax1AmountBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* sales_tax1AmountBox
      { sales_tax2AmountBox = new Fl_Box_ClearLabel(355, 608, 115, 25);
        sales_tax2AmountBox->box(FL_NO_BOX);
        sales_tax2AmountBox->color(FL_BACKGROUND_COLOR);
        sales_tax2AmountBox->selection_color(FL_BACKGROUND_COLOR);
        sales_tax2AmountBox->labeltype(FL_NORMAL_LABEL);
        sales_tax2AmountBox->labelfont(5);
        sales_tax2AmountBox->labelsize(15);
        sales_tax2AmountBox->labelcolor(FL_FOREGROUND_COLOR);
        sales_tax2AmountBox->align((FL_ALIGN_CENTER));
        sales_tax2AmountBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* sales_tax2AmountBox
      { totalBox = new Fl_Box_ClearLabel(500, 608, 121, 25);
        totalBox->box(FL_NO_BOX);
        totalBox->color(FL_BACKGROUND_COLOR);
        totalBox->selection_color(FL_BACKGROUND_COLOR);
        totalBox->labeltype(FL_NORMAL_LABEL);
        totalBox->labelfont(5);
        totalBox->labelsize(15);
        totalBox->labelcolor(FL_FOREGROUND_COLOR);
        totalBox->align((FL_ALIGN_CENTER));
        totalBox->when(FL_WHEN_RELEASE);
      } // Fl_Box_ClearLabel* totalBox
      bottomTotals->end();
    } // Fl_Group* bottomTotals
    printDataGroup->end();
    Fl_Group.current()->resizable(printDataGroup);
  } // Fl_Group* printDataGroup
  page->end();
} // Fl_Group* page
end();
resizable(this);
}
}
