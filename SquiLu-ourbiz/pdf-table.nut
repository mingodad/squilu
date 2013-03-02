/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
class PdfSqlTable extends Sq_Fpdf {
	col_h = null;
	col_h0 = null;
	doFill = null;
	_cols_info = null;
	page_title = null;
	water_mark = null;
	
	constructor(){
		base.constructor();
		AliasNbPages();
		SetFont("Arial","",10);
		SetFillColor(224, 235, 255);
		SetDrawColor(0xCC);
		col_h = 5;
		col_h0 = 8;
		doFill = false;
		_cols_info = [];
		page_title = "";
		water_mark = "";
	}

	function parse_field_header(col_name){
		local ci = col_name.split('|');
		local col_info = {};
		col_info.colname <- ci.get(0, "?");
		col_info.header <- ci.get(1, "?");
		col_info.width <- ci.get(2, 10).tointeger();
		col_info.align <- ci.get(3, "L");
		col_info.format <- ci.get(4, "");
		col_info.color <- ci.get(5, 0).tointeger();
		col_info.bgcolor <- ci.get(6, 0).tointeger();
		col_info.header <- col_info.header;
		return col_info;
	}

	function set_cols(szCols, max_cols=12){
		_cols_info = [];
		for(local i=0; i < max_cols; ++i){
			local p = szCols.get(i, null);
			if (!p) break;
			local col_info = parse_field_header(p);
			_cols_info.push(col_info);
		}
		calc_cols_width();
	}

	function calc_cols_width(){
		local grid_width, total_widths, char_width;
		grid_width = GetW()-(GetLeftMargin()+GetRightMargin());
		char_width = GetStringWidth("w");

		total_widths = 0;
		local col_count = _cols_info.len();
		for(local k=0; k < col_count; ++k){
			local v = _cols_info[k].width;
			if (v > 0) total_widths += ((v * char_width) + 1);
		}

		for(local k=0; k < col_count; ++k){
			local v = _cols_info[k];
			local col_width = v.width;
			if ( col_width < 0) col_width = grid_width - total_widths;
			else col_width = col_width * char_width;
			v.width = col_width;
		}
	}

	function Header(){
		SetFont("","BI",18);
		Cell(160,0,page_title,"0",0,'L');
		SetFont("","",10);
		Cell(0,0,format("%d", PageNo()),"",2,'R');
		//Line break
		SetY(20);
		//MyLine(m_lMargin, 18, m_w - m_rMargin, 18);
		//Ln(15);
		SetFont("","B",12);
		local saved_fill = doFill;
		doFill = true;
		local saved_color = GetFillColor();
		SetFillColor(0xEE);
		for(local k=0, count=_cols_info.len(); k < count; ++k){
			local v = _cols_info[k];
			ClippedCell(v.width, col_h0, v.header, "1", 0, 'C', doFill);
		}
		Ln();
		SetFillColor(saved_color);
		doFill = saved_fill;
		SetFont("","",10);
	}

	function GetPDF(db, sql, clipped, mf=null){
		local stmt = db.prepare(sql);

		local col_count = stmt.col_count();
		_cols_info = [];

		stmt.step();
		for(local i=0; i < col_count; ++i){
			local col_info = parse_field_header(stmt.col_name(i));
			_cols_info.push(col_info);
		}
		stmt.reset();
		calc_cols_width();
		local result = clipped ? _gen_pdf_clipped(stmt, mf) : _gen_pdf_multiline(stmt, mf);
		stmt.finalize();
		return result;
	}

	function _gen_pdf_multiline(stmt, mf=null){
		AddPage();
		local cols_size = _cols_info.len();

		while(stmt.next_row()){
			local max_lines = 0
			for(local k=0; k < cols_size; ++k){
				local v = _cols_info[k];
				local lines = CalcLines(v.width, stmt.col(k).tostring());
				if(lines > max_lines) max_lines = lines;
			}
			local col_height = max_lines * col_h;
			CheckPageBreak(col_height);
			local saved_y = GetY();
			for(local k=0; k < cols_size; ++k){
				local saved_x = GetX();
				local v = _cols_info[k];
				local fill = doFill ? "DF" : "D";
				Rect(saved_x, saved_y, v.width, col_height, fill);
				MultiCell(v.width, col_h, stmt.col(k).tostring(), "LRT", v.align[0], doFill);
				SetXY(saved_x + v.width, saved_y);
			}
			doFill = !doFill;
			Ln(col_height);
		}
		//SetDisplayMode(FPDF::e_zoom_fullpage, FPDF::e_layout_single);
		if(mf) {
			Output(mf);
			return true;
		}
		return Output("doc.pdf", 'S');
	}

	function _gen_pdf_clipped(stmt, mf=null){
		AddPage();
		local cols_size = _cols_info.len();

		while(stmt.next_row()){
			for(local k=0; k < cols_size; ++k){
				local v = _cols_info[k];
				ClippedCell(v.width, col_h, stmt.col(k).tostring(),
					"1", 0, v.align[0], doFill);
			}
			doFill = !doFill;
			Ln();
		}
		//SetDisplayMode(FPDF::e_zoom_fullpage, FPDF::e_layout_single);
		if(mf) {
			Output(mf);
			return true;
		}
		return Output("doc.pdf", 'S');
	}

	function MyLine(x1, y1, x2, y2){
		SetLineWidth(0.5);
		local saved_color = GetDrawColor();
		SetDrawColor(200,200,200);
		Line(x1, y1, x2, y2);
		SetDrawColor(saved_color);
	}

	function Footer(){
		//Position at 1.5 cm from bottom
		//MyLine(m_lMargin, m_h-20, m_w - m_rMargin, m_h-20)
		SetY(-20);
		//Page number
		Cell(0,3, format("Page  %d / %s", PageNo(),
							GetAliasNbPages()),"",1,'C');
		SetFont("","I",8);
		Cell(0,6, "OURBIZ is an excelente application to manage a business.","",0,'C');

		if (water_mark && water_mark.len() > 0) {
			SetAlpha(0.7, "Darken");
			//SetAlpha(0.5);
			SetFont("Arial","B",50);
			SetTextColor(255,192,203);
			RotatedText(35,190, water_mark, 45);
			SetAlpha(1);
			SetTextColor(0);
		}
	}
}
