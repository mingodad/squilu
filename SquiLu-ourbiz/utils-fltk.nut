/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
function setAppFont(fontName)
{
    local my_font, numfonts, font1, font2;
 
    numfonts=Fl.set_fonts();
 
    for(local i=0; i < numfonts; ++i)
    {
        my_font=Fl.get_font(i); // get the name of the current font
        if( my_font )
        {
            local font = Fl.get_font_name(i);
            //print(my_font_name, my_font_attr)
            if( font.name )
            {
                if( font.name.find(fontName) >= 0)
                {
		    //print(font.name);
                    if( font.attributes != 0 )
                    {
                        if (( font.attributes & FL_BOLD) &&
                                ( font.attributes & FL_ITALIC))
                        {
                            font1 = FL_HELVETICA_BOLD_ITALIC;
                            font2 = FL_TIMES_BOLD_ITALIC;
                        }
                        else if(  font.attributes & FL_BOLD )
                        {
                            font1 = FL_HELVETICA_BOLD;
                            font2 = FL_TIMES_BOLD;
                        }
                        else if(  font.attributes & FL_ITALIC )
                        {
                            font1 = FL_HELVETICA_ITALIC;
                            font2 = FL_TIMES_ITALIC;
                        }
                    }
                    else
                    {
                        font1 = FL_HELVETICA;
                        font2 = FL_TIMES;
                    }
                    Fl.set_font(font1, i);
                    Fl.set_font(font2, i);
                    //break
                }
            }
        }
    }
}

function widget_focus_changing_Match(widget){
	if (widget){
		local ttype = widget.classId();
		if (ttype.find("_Input")) return 1;
		if (ttype.find("_Browser")) return 2;
		if (ttype.find("_Tree")) return false;
		if (ttype.find("_Editor")) return 3;
		local parent = widget.parent();
		if (parent && parent.classId().find("_Choice")){
			return 3;
		}
	}
	return false;
}
		
function fltk_focus_changing(wfrom, wto){
	if (widget_focus_changing_Match(wfrom)){
		wfrom.color(FL_WHITE);
		wfrom.redraw();
	}
	local wt = widget_focus_changing_Match(wto);
	if (wt) {
		wto.color(FL_YELLOW);
		wto.redraw();
	}
	
	return 0;
}

local function _do_delayed_focus (widget){
	widget->take_focus();
}
function delayed_focus (widget){
	Fl.add_timeout(0.05, _do_delayed_focus, widget);
}

local function _do_delayed_method_call (mcall){
    mcall[1].call(mcall[0], mcall[2]);
}

function delayed_method_call (obj, mcb, udata)
{
	local mcall = [obj, mcb, udata];
	Fl.add_timeout(0.05, _do_delayed_method_call, mcall);
}


math.number_format_set_dec_point(".");
math.number_format_set_thousand_sep(",");
Fl_Input.default_number_format("\x02.,");

//Fl:scheme("plastic");
Fl.scheme("gtk+");
//use partial match to find verdana font
Fl.visual(FL_RGB);
//allow arrow keys navigation
Fl.option(Fl.OPTION_ARROW_FOCUS, true);
fl_register_images();
/*
//setting font this way on linux makes Fl_Pack child misbehave
//also with this bad pdf right align
setAppFont("erdana");
*/
Fl.add_focus_changing_handler(fltk_focus_changing);
fl_preferences(Fl.FL_PREFERENCES_USER, "dadbiz", "ourbiz");
