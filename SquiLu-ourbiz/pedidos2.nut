function _tr(str) {return str;}

class Fl_Multiline_Output extends Fl_Output {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		type(4);
	}
}

enum Fl_Data_Table_Events {e_none, e_event, e_select, e_insert, e_update, e_delete};

class Fl_Data_Table extends Flv_Data_Table {
	_cols_info = null;
	_data = null;
	_call_this = null;
	_forPrint = null;

	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		_cols_info = [];
		_data = [];
		_forPrint = false;
		has_scrollbar(FLVS_BOTH);
	}

	function for_print(bval){_forPrint = bval; }

	function resize(ax, ay, aw, ah){
		base.resize(ax, ay, aw, ah);
		calc_cols();
	}

	function clear_data_rows(){
		row(0);
		rows(0);
		_data.clear();
	}
	function recalc_data(){
		rows(_data.size());
		redraw();
	}

	function set_data(data_array){
		_data = data_array;
		recalc_data();
	}

	function set_new_data(data_array){
		clear_data_rows();
		_data = data_array;
		local mycols = _data[0];
		_data.remove(0);
		set_cols(mycols);
		recalc_data();
	}

	function get_data_value(arow, acol){ return _data[arow][acol];}

	function get_value(arow, acol){
		if(acol < 0)
		{
		    if(arow < 0) return "#";
		    return format("%d", arrow+1);
		}
		if(arow < 0) return _cols_info[acol].header;

		local ci = _cols_info[acol];

		local value = _data[arow][acol];

		switch(ci.format)
		{
			case 'M': return math.number_format(value.size() ? value.tofloat() : 0, 2);
			break;
			case 'P': return math.number_format(value.size() ? value.tofloat() : 0, 3);
			break;
			case 'N': return math.number_format(value.size() ? value.tofloat() : 0, -6);
			break;
			case 'Z': //Empty when zero
			case 'D': //Zero monetary //ZM':
			{
			    if(value == "0" || value == "") return "";
			    else
			    {
				if(ci.format == 'Z') return math.number_format(value.tofloat(), -6);
				else return math.number_format(value.tofloat(), 2);
			    }
			}
			break;
			case 'B': return (value == "1" ? "@-3+" : "");
			break;
			case 'S': return value.tostring();
			break;
			case '@': return value.replace("@", "@@"); //escape any @ 	{
			break;
		}
		return value;
	}

	function get_style(style, Row, Col){
		base.get_style(style, Row, Col);
		if(Row >= 0 && Col >= 0)
		{
		    local rs = row_style().get(Row);
		    local cs = col_style().get(Col);
		    local have_bg = 0;
		    if(cs.background_defined()) have_bg = cs.background();
		    else if(rs.background_defined()) have_bg = rs.background();

		    if((Row % 2) == 0)
		    {
			if(have_bg)
			    //s:background(fltk.fl_color_average(207, have_bg, 0.5))
			    style.background(have_bg);
			else
			    style.background(207);
		    }
		    else
		    {
			if(have_bg)
			    //s:background(have_bg)
			    style.background(fl_color_average(FL_WHITE, have_bg, 0.5));
			else
			    style.background(FL_WHITE);
		    }
		}
		else if(_forPrint)
		{
		    style.frame(FL_NO_BOX);
		}
	}

	function handle(event){
		switch(event){
			case FL_RELEASE:{
				local done = false;
				if(Fl.event_clicks() > 0){
					row_selected(Fl_Data_Table_Events.e_update);
					done = true;
				}
				if(done){
					Fl.event_clicks(0);
					return 1;
				}
			}
			break;
			case FL_KEYBOARD:
				local key = Fl.event_key();
				switch(key){
					case FL_KP_Plus:
						if(Fl.event_ctrl()) break;
					case FL_Insert:
						row_selected(Fl_Data_Table_Events.e_insert);
					break;
					//case FL_KP_Minus:
					//    if(Fl::event_ctrl()) break;
					case FL_Delete:
						row_selected(Fl_Data_Table_Events.e_delete);
					break;
					case FL_KP_Enter:
					case FL_Enter:
					case FL_Key_Space:
						if(!Fl.event_ctrl()){
							row_selected(Fl_Data_Table_Events.e_update);
						}
					break;
				}
			break;
		}
		local rc = base.handle(event);
		return rc;
	}
	function row_selected(ev){ if(_call_this) _call_this.row_selected(this, ev);}
	function set_cols(mycols, size_absolute=false){
		_cols_info.clear();
		for(local i=0, max_cols=mycols.size(); i < max_cols; ++i){
		    local col_info = {};
		    parse_field_header(mycols[i], col_info);
		    _cols_info.push(col_info);
		}
		cols(_cols_info.size());
		calc_cols(size_absolute);
	}
	function parse_field_header(str, col_info){
		local ci = str.split('|');
		local ci_size = ci.size();
		local curr_ci = 0;
		col_info.colname <- (ci_size > curr_ci++) ? ci[curr_ci-1] : "?";
		col_info.header <- (ci_size > curr_ci++) ? ci[curr_ci-1] : "?";
		col_info.width <- (ci_size > curr_ci++) ? ci[curr_ci-1].tofloat() : 10;
		col_info.align <- (ci_size > curr_ci++) ? ci[curr_ci-1][0] : 'L';
		col_info.format <- (ci_size > curr_ci++) ? ci[curr_ci-1][0] : '\0';
		col_info.color <- (ci_size > curr_ci++) ? ci[curr_ci-1].tointeger() : 0;
		col_info.bgcolor <- (ci_size > curr_ci++) ? ci[curr_ci-1].tointeger() : 0;
		if(ci_size == 1) {
			col_info.colname = str;
			col_info.header = str;
		}
	}
	function calc_cols(size_absolute=false){
		calc_cols_width(size_absolute);
		for(local k=0, len = cols(); k < len; ++k)
		{
		    local v = _cols_info[k];
		    //v.header = _tr(v.header.c_str());
		    local col_align;
		    switch(v.align)
		    {
		    case 'C':
			col_align = FL_ALIGN_CENTER;
			break;
		    case 'R':
			col_align = FL_ALIGN_RIGHT;
			break;
		    default:
			col_align = FL_ALIGN_LEFT;
		    }
		    local cs = col_style().get(k);
		    cs.align(col_align);
		    if(v.color) cs.foreground(v.color);
		    if(v.bgcolor) cs.background(v.bgcolor);
		}
	}
	function calc_cols_width(size_absolute=false){
		if(size_absolute){
			for(local k=0, len = cols(); k < len; ++k)
			{
			    local cs = col_style().get(k);
			    cs.width(_cols_info[k].width);
			}
			return;
		}
		local grid_width, total_widths, char_width;
		grid_width = w();
		if(has_scrollbar()) grid_width -= scrollbar_width();
		//char_width = self:labelsize() --fltk.fl_width('w')
		//it seems that if not set the text font on fluid
		//we get segfault here
		fl_font(textfont(), textsize());
		local gs = global_style();
		gs.height(fl_height()+fl_descent());
		char_width = fl_width("w");
		draw_offset(char_width/3);
		//print(grid_width, char_width)

		total_widths = 0;
		for(local k=0, len = cols(); k < len; ++k)
		{
		    local v = _cols_info[k].width;
		    if(v > 0) total_widths += ((v * char_width) + 1);
		}

		for(local k=0, len = cols(); k < len; ++k)
		{
		    local v = _cols_info[k];
		    local col_width = v.width;
		    if( col_width < 0)	col_width = grid_width - total_widths;
		    else col_width *= char_width;
		    local cs = col_style().get(k);
		    cs.width(col_width);
		}
	}
	function get_selection(ar_out, withIds=false){
	}
	function clear_selection(){
	}
	function get_row_id(arow=null){
		if(arow == null) arow = row();
		return _data[arow][0].tointeger();
	}	
}

class Base_Window extends Fl_Window {
	_db_map = null;
	
	constructor(px, py, pw, ph, pl){
		base.constructor(px, py, pw, ph, pl);
		_db_map = {};
	}
	function add_input_field_to_map(tbl, fldname, fld){
		local tbl_map = _db_map.get(tbl, false);
		if(!tbl_map){
			tbl_map = {};
			_db_map[tbl] <- tbl_map;
		}
		tbl_map[fldname] <- fld;
	}
}

class Fl_Multiline_Input extends Fl_Input {
	constructor(px, py, pw, ph, pl=null){
		base.constructor(px, py, pw, ph, pl);
		type(4);
	}
}

local function _do_delayed_focus (widget){
	widget->take_focus();
}
function delayed_focus (widget){
	Fl.add_timeout(0.05, _do_delayed_focus, widget);
}

dofile("pedidos2-gui.nut");

class Pedidos2 extends PedidosWindow {
  
	// Declaration of class members
	menu_bar_deactivated_menus = null;
	pedidos_edit_record = null;
  
	constructor(){
		base.constructor();
	}
	
	function row_selected(sender, ev){
	}
	
	function handle(event){
		if (event == FL_KEYBOARD && Fl.event_command() == 0){
			local key = Fl.event_key();
			//print(key)
			switch(key){
				case FL_Menu: menu_bar_navigate(); break;
				case FL_F+1: mostrar_ventana_ayuda(); break;
				case FL_F+2: mostrar_ventana_pedido(); break;
				case FL_F+3: mostrar_ventana_pedidos(); break;
				case FL_F+4: mostrar_ventana_clientes(); break;
				case FL_F+5: mostrar_ventana_articulos(); break;
				case FL_F+6: mostrar_ventana_totales(); break;
				case FL_F+9: mostrar_ventana_opciones(); break;
			}
		}
		
		return base.handle(event);
	}
	
	function reset_menus_desactivados(){
		if (menu_bar_deactivated_menus){
			local mb = menu_bar;
			foreach(k,v in menu_bar_deactivated_menus) mb.menu_at(v).deactivate();
		}
	}
	
	function reset_menus_desactivados_activando(list){
		if (menu_bar){
			local mb = menu_bar;
			reset_menus_desactivados();
			foreach(k,v in list) mb.menu_at(v).activate();
		}
	}
	
	function menu_bar_navigate(){
		local mb = menu_bar;
		local v = mb.menu().pulldown(mb.x(), mb.y(), mb.w(), mb.h(), mb.menu_at(0), mb, null, 1);
		mb.picked(v);
	}
	function mostrar_ventana_ayuda(){
	}
	function mostrar_ventana_pedido(){
		tabs.value(tab_pedido);
		if (pedidos_edit_record && pedidos_edit_record.id)
			delayed_focus(pedido_lineas_codigo);
		else
			delayed_focus(pedidos_cliente_codigo_r);

		reset_menus_desactivados_activando([
				menu_pedido_guardar,
				menu_pedido_borrar,
				menu_pedido_imprimir,
			]);
	}
	function mostrar_ventana_pedidos(){
		tabs.value(tab_pedidos_lista);
	}
	function mostrar_ventana_clientes(){
		tabs.value(tab_clientes_lista);
	}
	function mostrar_ventana_articulos(){
		tabs.value(tab_articulos_lista);
	}
	function mostrar_ventana_totales(){
		tabs.value(tab_totales);
	}
	function mostrar_ventana_opciones(){
		tabs.value(tab_opciones);
	}
}

Fl.scheme("gtk+");
Fl.visual(FL_RGB);
//allow arrow keys navigation
Fl.option(Fl.OPTION_ARROW_FOCUS, true);

local win = new Pedidos2();
//local win = new SalesTaxRatesEditWindow();
win->resizable(win);
win->show_main();

Fl.run();