class PDF_Order extends Sq_Fpdf
{
	logoFileName = null;
	secondColX = null;
	cellHeight = null;
	cellWidth = null;
	fillColorBG = null;
	drawColor = null;
	static borderTop = "LRT";
	static borderBetween = "LR";
	static borderBottom = "LBR";

	water_mark = null;
	page_title = null;
	page_title2 = null;
	page_footer = null;
        labelNumber = null;
	strNumber = null;
	labelDate = null;
	strDate = null;

	lineHeaders = null;
	totalHeaders = null;
	strLines = null;


	strTotals = null;
	strEntity = null;

	constructor()
	{
	    base.constructor();
		AliasNbPages();
		SetFont("Arial", "", 12);
		SetAuthor("DADBIZ & OurBiz");
		//SetCompression(false);
	}

	function _tr(str) {return str;}

	function do_init()
	{
		secondColX = 107;
		cellHeight = 7;
		cellWidth = 44;
		fillColorBG = 0xEE;
		drawColor = 0xCC;
		page_title = "OURBIZ";
		page_title2 = "www.dadbiz.es";
		page_footer = "Thanks for choose our products and services !";
		labelNumber = _tr("Order Number");
		strNumber = "20128456";
		labelDate = _tr("Date");
		strDate = "10/04/2012";
		
		strTotals = [];
		strEntity = [];
		strLines = [];

		lineHeaders = [];
		newColInfo(lineHeaders, _tr("Code"), 16, 'C', false);
		newColInfo(lineHeaders, _tr("Description"), 93, 'L', true),
		newColInfo(lineHeaders, _tr("Quantity"), 16, 'R', true),
		newColInfo(lineHeaders, _tr("Price"), 20, 'R', false),
		newColInfo(lineHeaders, _tr("Disc. %"), 10, 'R', true),
		newColInfo(lineHeaders, _tr("S.T. I %"), 10, 'R', true),
		newColInfo(lineHeaders, _tr("S.T. II %"), 10, 'R', true),
		newColInfo(lineHeaders, _tr("Subtotal"), 20, 'R', false);

		totalHeaders = [];
		newColInfo(totalHeaders, _tr("Page"), 20, 'C', false);
		newColInfo(totalHeaders, _tr("Subtotal"), 45, 'C', true);
		newColInfo(totalHeaders, _tr("Sales Tax I"), 42, 'C', true);
		newColInfo(totalHeaders, _tr("Sales Tax II"), 42, 'C', false);
		newColInfo(totalHeaders, _tr("Total"), 46, 'C', true);
	}

	function newColInfo(vec, str, width, dataAlign, fitScale)
	{
		vec.push({
			str = str,
			width = width,
			dataAlign = dataAlign,
			fitScale = fitScale,
		});
	}

	function getLogo(){
		if(!logoFileName || logoFileName.empty()){
			logoFileName = "dadbiz_120.png";
		}
		return logoFileName;
	}

	function logoImage(px, py, pwidth){
		local logoImg = getLogo();
		if(!logoImg.empty()) Image(logoImg, px, py, pwidth);
	}

	function orderTable(lineCount)
	{
		local norder_pages = strLines.size()/lineCount;
		if(strLines.len()%lineCount) ++norder_pages;
		if(!norder_pages) ++norder_pages;

		local tmp_str;
		local font_settings = {};

		AliasNbPages();
		for(int_t _np=0; _np < norder_pages; ++_np)
		{
			AddPage();
			SetFont("Arial", "0", 12);
			//if(getLogoImage() != null) Image(getLogoImage(), mLogoFileName, 10, 8, 30, 0, 0, 0);
			//Image("logo.png",10,8);
			LogoImage(10, 8, 30);
			// Header

			SetFontSize(12);
			SetFillColor(fillColorBG);
			SetDrawColor(drawColor);
			SetLineWidth(0.3);
			SetFont("", "B");

			local savedY = GetY();

			Cell(32);
			Cell(cellWidth*1.65, cellHeight, page_title, "", 0, 'C');
			Ln();
			Cell(32);
			GetFontSettings(font_settings);
			SetFont("Arial", "i", 9);
			MultiCell(cellWidth*1.65, cellHeight*0.6, page_title2, "", 'C');
			SetFontSettings(font_settings);

			SetY(savedY);

			SetFontSize(10);
			Cell(secondColX);

			Cell(cellWidth, cellHeight, labelNumber, "1", 0, 'C', true);
			Cell(cellWidth, cellHeight, labelDate, "1", 0, 'C', true);

			Ln();
			// Color and font restoration
			SetFillColor(224, 235, 255);
			SetFontSize(12);

			Cell(secondColX);

			Cell(cellWidth, cellHeight, strNumber, "1", 0, 'C');
			Cell(cellWidth, cellHeight, strDate, "1", 0, 'C');
			Ln();
			SetFontSize(10);

			SetY(GetY() + 2);
			Cell(secondColX);
			local savedHeight = cellHeight;
			cellHeight = 6;
			local withBorder = borderTop;
			local x = 0;
			for(local i=0, size = strEntity.size(); i< size; ++i)
			{
				local value = strEntity[i];
				CellFitScale(cellWidth * 2, cellHeight, value, withBorder,
					0, 'L', false, 0);
				if (x++ == size - 2) withBorder = borderBottom;
				else withBorder = borderBetween;
				Ln();
				Cell(secondColX);
			}
			cellHeight = savedHeight;
			SetY(GetY() + 2);
			SetFillColor(fillColorBG);
			SetDrawColor(drawColor);
			SetLineWidth(0.3);
			SetFont("", "B");

			local savedFontSize = GetFontSize();
			for(local i=0, size=lineHeaders.size(); i<size; ++i)
			{
				local header = lineHeaders[i];
				if(header.fitScale) CellFitScale(header.width, cellHeight, header.str, "1", 0,
					'C', true);
				else Cell(header.width, cellHeight, header.str, "1", 0, 'C', true);
			}
			SetFontSize(savedFontSize);
			Ln();
			// Color and font restoration
			SetFillColor(224, 235, 255);
			SetTextColor(0);
			SetFont();
			local fill = false;
			withBorder = borderBetween;
			local line_count = (lineCount == 0) ? 32 : lineCount;
			line_count += line_count * _np;
			savedHeight = cellHeight;
			cellHeight = 6;
			for(local i = _np*lineCount; i < line_count; i++)
			{
				x = 0;
				for (local j=0, size=lineHeaders.size(); j<size; ++j)
				{
					local header = lineHeaders[j];
					if(header.fitScale)  //clipping desciption
					{
						//ClippedCell(header.width, mCellHeight, getLineData(i, x), withBorder,
						//	Position.RIGHTOF, header.dataAlign, fill);
						CellFitScale(header.width, cellHeight, getLineData(i, x), withBorder,
							0, header.dataAlign, fill, 0);
					}
					else
					{
						Cell(header.width, cellHeight, getLineData(i, x).tostring(), withBorder,
							0, header.dataAlign, fill);
					}
					x++;
				}
				Ln();
				fill = !fill;
				if (i > line_count - 3) withBorder = borderBottom;
			}

			cellHeight = savedHeight;

			SetY(GetY() + 2);
			SetFillColor(fillColorBG);
			SetDrawColor(drawColor);
			SetLineWidth(0.3);
			SetFont("", "B");

			for(local i=0, size=totalHeaders.size(); i<size; ++i)
			{
				local header = totalHeaders[i];
				Cell(header.width, cellHeight, header.str, "1", 0, 'C', true);
			}
			Ln();
			// Color and font restoration
			SetFillColor(224, 235, 255);
			SetTextColor(0);
			SetFont("", "B", 12);

			x = 0;
			for(local i=0, size=totalHeaders.size(); i<size; ++i)
			{
				local header = totalHeaders[i];
				if(x == 0)
				{
					tmp_str = format("%d/%s", PageNo(), GetAliasNbPages());
					Cell(header.width, cellHeight, tmp_str, "1", 0, header.dataAlign);
				}
				else
				{
					tmp_str = (_np == (norder_pages-1)) ? getTotalData(x) : "...";
					Cell(header.width, cellHeight, tmp_str, "1", 0, header.dataAlign);
				}
				x++;
			}

			if(!water_mark.empty()){
				SetAlpha(0.7, "Darken");
				//SetAlpha(0.5);
				SetFont("Arial","B",50);
				SetTextColor(255,192,203);
				RotatedText(35,190, water_mark, 45);
				SetAlpha(1);
				SetTextColor(0);
			}
			//footer message
			Ln(10);
			SetFont("", "i", 8);
			MultiCell(0, cellHeight*0.6, page_footer, "", 'C');

		}
	}

	function getLineData(row, col)
	{
		if(strLines.size() && row < strLines.size()) return strLines[row][col];
		return "";
	}

	function getTotalData(col)
	{
		//<= and col-1 because it start with 1
		if(strTotals.size() && col <= strTotals.size()) return strTotals[col-1];
		return "";
	}
}
