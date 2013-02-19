class Fl_Bar_Chart extends Fl_Box {
	_bars = null; //{label, value, color}
	_bar_depth = null;
	_bar_update_count = null;
	_bar_num_format_decimals = null;
	_bar_color = null;
	
	constructor(X, Y, W, H, L = null) {
		base.constructor(X,Y,W,H,L);
		color(FL_WHITE);
		align(FL_ALIGN_RIGHT);
		labeltype(FL_NO_LABEL);
		_bars = [];
		_bar_color = FL_BLUE;
		_bar_depth = 8;
		_bar_update_count = 0;
		_bar_num_format_decimals = 0;
	}

	function bar_count(){return _bars.size();}

	function bar_add(bheader, bvalue, bcolor=FL_BLUE) {
		_bars.push( [bheader, bvalue, bcolor]);
	}

	function bar_get(idx) { return _bars[idx];}

	function bar_depth(depth=null){
		if(depth==null) return _depth;
		if(_bar_depth == depth) return;
		_bar_depth = depth;
		update_bar_chart();
	}

	function bar_num_format_decimals(decimals=null) {
		if(decimals==null) return _bar_num_format_decimals;
		if(_bar_num_format_decimals == decimals) return;
		_bar_num_format_decimals = decimals;
		update_bar_chart();
	}

	function bar_begin_update() {return ++_bar_update_count;}

	function bar_end_update() {
		if(_bar_update_count == 0) throw("update count underflow");
		if(--_bar_update_count == 0) update_bar_chart();
	}

	function update_bar_chart() {
		if(_bar_update_count == 0) redraw();
	}

	function bar_clear(){
		_bars.clear();
		update_bar_chart();
	}


	function normalize_scale_units(oldScale) {
		local result = oldScale;
		if(result < 2) result = 2;
		else if(result <= 5) result = 5;
		else if(result <= 10) result = 10;
		else
		{
			local str = result.tostring();
			local t = str[0] - '0';
			do
			{
				result = result / 10;
				t *= 10;
			}
			while(result >= 10);
			return t;
		}
		return result;
	}

	function draw_mypolygon(x1, y1, x2, y2, x3, y3, x4, y4, line_color, fill_color) {
		fl_color(fill_color);
		fl_polygon(x1, y1, x2, y2, x3, y3, x4, y4);

		fl_color(line_color);
		fl_line_style(FL_SOLID, 1, NULL);
		fl_loop(x1, y1, x2, y2, x3, y3, x4, y4);
	}

	function  draw_mybar(pbx,  pby, pw, ph, pdepth, line_color, fill_color) {
		local lbx2 = pbx+pw;
		local lbx3 = lbx2+pdepth;
		local lby2 = pby+ph;
		local lby3 = pby-pdepth;
		// face rect
		draw_mypolygon(
			pbx, pby,
			lbx2, pby,
			lbx2, lby2,
			pbx, lby2,
			line_color,
			fill_color);
		// top polygon
		draw_mypolygon(
			pbx, pby,
			lbx2, pby,
			lbx3, lby3,
			pbx+pdepth, lby3,
			line_color,
			fill_color);
		// side polygon
		draw_mypolygon(
			lbx2, pby,
			lbx3, lby3,
			lbx3, lby2-pdepth,
			lbx2, lby2,
			line_color,
			fill_color);
	}

	function draw() {
		//set font size, color and clear
		fl_font(labelfont(), labelsize());
		fl_color(color());
		fl_rectf(x(), y(), w(), h());

		local  measure = {};
		fl_measure("W", measure, false);
		local  char_width = measure.width, char_height = measure.height;
		

		local inner_padding = char_height;
		local box_x = x();
		local box_y = y() + char_height;

		local str;
		local nf_buf;

		local bars_box_x, bars_box_y, bars_box_width, bars_box_height;

		local box_label_width = inner_padding;
		local value_max = 0.0;

		foreach(bar in _bars)
		{
			local max_value_str_width = fl_width(bar[0] /*label*/, bar[0].size());
			if(max_value_str_width > box_label_width) box_label_width = max_value_str_width;
			if(bar[1] /*value*/  > value_max) value_max = bar[1];
		}

		box_label_width += inner_padding *2;
		bars_box_x = box_x + box_label_width;
		bars_box_y = box_y + inner_padding;
		bars_box_height = h() - inner_padding - 3 * char_height;
		bars_box_width = w() - box_label_width - 3 * _bar_depth - inner_padding;

		draw_mypolygon(
		    bars_box_x, bars_box_y+_bar_depth,
		    bars_box_x, bars_box_y+_bar_depth+bars_box_height,
		    bars_box_x+_bar_depth, bars_box_y+bars_box_height,
		    bars_box_x+_bar_depth, bars_box_y,
		    FL_BLACK,
		    FL_YELLOW
		);
		draw_mypolygon(
		    bars_box_x, bars_box_y+_bar_depth+bars_box_height,
		    bars_box_x+bars_box_width+_bar_depth, bars_box_y+_bar_depth+bars_box_height,
		    bars_box_x+bars_box_width+_bar_depth*2, bars_box_y+bars_box_height,
		    bars_box_x+_bar_depth, bars_box_y+bars_box_height,
		    FL_BLACK,
		    FL_WHITE
		);

		fl_color(FL_BLACK);
		fl_line_style(FL_SOLID, 1, NULL);
		fl_rect(bars_box_x+_bar_depth, bars_box_y, bars_box_width+_bar_depth, bars_box_height+1);

		fl_line_style(FL_SOLID, 3, NULL);
		fl_line(bars_box_x, bars_box_y+_bar_depth, bars_box_x, bars_box_y+_bar_depth+bars_box_height, bars_box_x+bars_box_width+_bar_depth, bars_box_y+_bar_depth+bars_box_height);
		local my_label = label();
		if(my_label) fl_draw(my_label, my_label.len(), x() + (w() - fl_width(my_label))/2, box_y);

		str = value_max.tostring();
		fl_measure(str, measure, false);
		local max_value_str_width = measure.width, max_value_str_height = measure.height;

		local pixelPerUnit, nScaleLines, scaleUnits, itmp;
		if(value_max == 0) {
		    pixelPerUnit = 1;
		    nScaleLines = 1;
		}
		else
		{
		    itmp = bars_box_width-max_value_str_width-_bar_depth;
		    pixelPerUnit = itmp / value_max;
		    nScaleLines = math.ceil(itmp / (2*max_value_str_width));
		}

		if(nScaleLines == 0) scaleUnits = value_max +1;
		else
		{
		    scaleUnits = math.floor(value_max / nScaleLines) +1;
		}
		scaleUnits = normalize_scale_units(scaleUnits);
		if(scaleUnits == 0)	nScaleLines = 1;
		else nScaleLines = (value_max / scaleUnits)+1;

		fl_color(FL_DARK3);
		fl_line_style(FL_DASH, 1, "\0x05\0x05");

		local half_max_value_str_width = math.floor(max_value_str_width / 2);
		local bars_box_bottom = bars_box_y+_bar_depth+bars_box_height+2;
		local str_zero = "0";

		local function draw_scaleLine(dk,s) {
			local lx, ly, lx2, ly2;
			lx = bars_box_x + dk + _bar_depth;
			ly = bars_box_y + bars_box_height;
			lx2 = bars_box_x + dk;
			ly2 = bars_box_y + _bar_depth+bars_box_height;
			fl_line(lx, bars_box_y, lx, ly, lx2, ly2);
			fl_line(lx2, ly2, lx2, ly2 + 2);
			local oldcolor = fl_color();
			fl_color(FL_BLACK);
			fl_draw(s, s.len(), bars_box_x + dk - half_max_value_str_width, bars_box_bottom+char_height);
			fl_color(oldcolor);
		}

		if(value_max == 0)
		{
		    local ik = math.floor(bars_box_width / 2);
		    draw_scaleLine(ik,str_zero);
		}
		else
		{
		    fl_color(FL_BLACK);
		    fl_draw(str_zero, str_zero.len(), bars_box_x, bars_box_bottom+char_height);
		    fl_color(FL_DARK3);
		}

		local _num_buf;

		for(local ik=0; ik < nScaleLines; ik++)
		{
		    local xs = math.floor(scaleUnits*pixelPerUnit*ik);
		    _num_buf = math.number_format(ik*scaleUnits, _bar_num_format_decimals);
		    draw_scaleLine(xs, _num_buf);
		}

		local bar_height, bar_y;
		if(_bars.size() == 0) bar_height = 0;
		else bar_height = ((bars_box_height- _bar_depth) / ((2.0*_bars.size()+1)));

		bars_box_bottom -= 2*bar_height + _bar_depth;
		local ikl = 0;
		foreach(bar in _bars) {
			bar_y = bars_box_bottom - (2*ikl*bar_height) ;
			local value = bar[1]; //bar value
			local bar_width = math.floor(value*pixelPerUnit);

			draw_mybar(bars_box_x, bar_y, bar_width, bar_height, _bar_depth, FL_DARK3, bar[2] /*color*/);

			_num_buf = math.number_format(value, _bar_num_format_decimals);

			local rcenter = math.floor(bar_y+(bar_height-_bar_depth) / 2);

			fl_color(FL_DARK3);
			fl_line(bars_box_x+bar_width+_bar_depth/2,rcenter, bars_box_x+bar_width+_bar_depth*2, rcenter);

			fl_measure(_num_buf, measure, false);
			local str_width = measure.width, str_height = measure.height;

			local rc_x = bars_box_x+bar_width+_bar_depth*2;
			local rc_w = str_width+_bar_depth;
			local rc_y = rcenter - str_height/2;
			local rc_h = str_height * 1.1;

			fl_color(FL_YELLOW);
			fl_line_style(FL_SOLID, 2);
			fl_rectf(rc_x, rc_y, rc_w, rc_h);
			fl_color(FL_BLACK);
			fl_draw(_num_buf, _num_buf.len(), rc_x+_bar_depth/2, rcenter + str_height/3);

			str = bar[0]; //bar header
			fl_measure(str, measure, false);
			str_width = measure.width;
			str_height = measure.height;

			local str_x, str_y;
			fl_color(FL_BLACK);
			switch(align()){
			    case FL_ALIGN_CENTER:
			    {
				str_x = box_x+(box_label_width/2) - (str_width / 2);
				str_y = rcenter + str_height/3;
			    }
			    break;
			    case FL_ALIGN_RIGHT:
			    {
				str_x = bars_box_x-str_width-inner_padding;
				str_y = rcenter + str_height/3;
			    }
			    break;
			    default:
			    {
				str_x = box_x + inner_padding;
				str_y = rcenter + str_height/3;
			    }
			}

			fl_draw(str, str.len(), str_x, str_y);
			++ikl;
		}
		fl_line_style(0);
	}
};

/*
math.number_format_set_thousand_sep(".");
math.number_format_set_dec_point(",");

math.mynumber_format <- function (value, decp=2){
	value = value.tofloat();
	local fvalue, neg, intp;
	fvalue = format("%.0" + decp + "f", value);
	if (decp > 0){
		if (value < 0) intp = fvalue.slice(1, -(decp+1));
		else intp = fvalue.slice(0, -(decp+1));
	}
	else intp = fvalue;

	local thousands_rep = "%1" + math.thousand_separator();
	local thousandsp = intp.reverse().gsub("(%d%d%d)",thousands_rep);
	if ((intp.len() % 3) == 0) thousandsp = thousandsp.slice(0,-1);
	if (decp > 0) fvalue = thousandsp.reverse() + math.decimal_separator() + fvalue.slice(-decp);
	else fvalue = thousandsp.reverse();
	if (value < 0) return "-" + fvalue;
	else return fvalue;
}

function blank_when_zero(snum) {return snum.tofloat();}

local win = Fl_Double_Window(10,50, 800, 560, "My Barchart");
local mychart = Fl_Bar_Chart(20,20, 760, 520, "My Barchart");
win->end();

mychart->labelsize(18);

local function rand_value(){return math.rand() % 300;}

mychart.bar_add("January", rand_value());
mychart->bar_add("Febrary", rand_value());
mychart->bar_add("March", rand_value());
mychart->bar_add("April", rand_value());
mychart->bar_add("May", rand_value());
mychart->bar_add("June", rand_value(), FL_RED);
mychart->bar_add("July", rand_value(), FL_GREEN);
mychart->bar_add("August", rand_value());
mychart->bar_add("September", rand_value());
mychart->bar_add("November", rand_value());
mychart->bar_add("Dicember", rand_value());

win->resizable(mychart);
win->show_main();
Fl.run();
*/
