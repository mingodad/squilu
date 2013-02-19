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
                if( font.name.find(fontName) )
                {
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
                    //print(bit.band(my_font_attr, FL_BOLD), my_font_attr, font1, font2)
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
		print(ttype);
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
	//print(wfrom.classId(), wto.classId());
	//print(wfrom, wto);
	
	if (widget_focus_changing_Match(wfrom)){
		wfrom.color(FL_WHITE);
		wfrom.redraw();
	}
	local wt = widget_focus_changing_Match(wto);
	if (wt) {
		wto.color(FL_YELLOW);
		wto.redraw();
		//if (wt == 1) wto.position(0,10000);
	}
	
	return 0;
}

math.number_format_set_dec_point(",");
math.number_format_set_thousand_sep(".");
Fl_Input.default_number_format("\0x02,.");
//Fl:scheme("plastic");
Fl.scheme("gtk+");
//use partial match to find verdana font
Fl.visual(FL_RGB);
//allow arrow keys navigation
Fl.option(Fl.OPTION_ARROW_FOCUS, true);
//setAppFont("erdana");
Fl.add_focus_changing_handler(fltk_focus_changing);

local myWindows = [];


class MyWindow extends Fl_Window {
	constructor(px, py, pw, ph, pl){
		base.constructor(px, py, pw, ph, pl);
	}
	function hide(){
		print("Before base hide");
		base.hide();
		Fl.delete_widget(this);
		print("After base hide");
	}
	function on_close(){
		print("on_close");
	}
}

class MyInput extends Fl_Input {
	constructor(px, py, pw, ph, pl){
		base.constructor(px, py, pw, ph, pl);
	}
	function handle(event){
		local rc = base.handle(event);
		print("After base event", rc, event);
		return rc;
	}
}

local series = "Series";

function _tr(str) {return str;}

function MyNewWin(){
	//local win = Fl_Double_Window(100, 100, 200,400, "My FLTK Window");
	local win = new MyWindow(100, 100, 800,500, "My FLTK Window");
	//local win = Fl_Window(100, 100, 200,400, "My FLTK Window");
/*
	win.callback(function(sender, uwin){ 
			print("mycb", sender, uwin);
			//myWindows.push(MyNewWin());
			//uwin.hide();
		}, win);
*/
	local btnNew = new Fl_Button(50,20, 80,25, "New Win");

	btnNew.callback(function(sender, udata){ 
			//print("mycb", sender, udata);
			myWindows.push(MyNewWin());
			Fl.add_timeout(1, @(p) print(p), "I'm timeout");
		}, "dad");

	local btnClose = Fl_Button(50,55, 80,25, "Close Win");

	btnClose.callback(function(sender, uwin){ 
			print("mycb", sender, uwin, sender.x());
			//myWindows.push(MyNewWin());
			uwin.hide();
		}, win);

	//local input = MyInput(50,85, 80,25, "Name");
	local input = Fl_Input(50,85, 80,25, "Name");
	local int_input = Fl_Int_Input(50,115, 80,25, "Age");
	int_input->color(FL_RED);
	//local box = Fl_Box(50,145, 80,25, "Img");
	//box.color(FL_RED);
	
	local editor = Fl_Text_Editor_Buffered(10,200, 180,100, "Notes");
	editor->value("Hello everybody here we are !");
	local btnText = Fl_Button(50,145, 80,25, "Text");
	btnText.callback(function(sender, uwin){
		print(input.value());
		});

	local tabs = Fl_Tabs(10,320, 180,40, "Tabs");
	local grp = Fl_Group(10,345, 180,20, "Tab1");
	grp.end()
	grp = Fl_Group(10,345, 180,20, "Tab2");
	grp.end()
	grp = Fl_Group(10,345, 180,20, "Tab3");
	grp.end()
	tabs.end();


          local o = Fl_Pack2(5, 380, 790, 25);
          o.type(1);
          o.labelsize(16);
          o->spacing(5);
          o->with_label(1);
          {
            {
              local o = Fl_Input(55, 35, 45, 25, series);
              o.textsize(16);
              o.labelsize(16);
            }
/*
            {
              local o = Fl_Int_Input(172, 35, 95, 25, _tr("Number"));
              o.textsize(16);
              o.labelsize(16);
              o->use_numeric_format(0);
            }
            {
              local o = Fl_Int_Input(319, 35, 95, 25, _tr("Entity"));
              o.textsize(16);
              o.labelsize(16);
            }
            {
              local o = Fl_Input(420, 35, 335, 25);
              o.textsize(16);
              //o.labeltype(FL_NO_LABEL);
	      o.label("DAD");
              o.textfont(1);
              o.labelsize(16);
              o.tooltip(_tr("Type partal description to search on availlable entities"));
              //Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Button(765, 35, 30, 25, _tr("@<->"));
              o.labelcolor(22);
              o.labelsize(18);
              o.tooltip(_tr("Show entities search window"));
            }
*/
          }
          o.end();


	win->end();
	win->resizable(win);
	win->show_main();
	return win;
}

MyNewWin();

//Fl.add_idle(@(p) print(p), "I'm idle !");

Fl.run()

