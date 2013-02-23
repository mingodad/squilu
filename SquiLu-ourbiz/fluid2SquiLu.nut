// squilu generator from FLUID project
// Usage:  
//   squilu  fluid2SquiLu.nut  input.fl  [output.nut]
//
//translated to SquiLu (http://code.google.com/p/squilu) by Domingo Alvarez Duarte
//

// murgaLua generator from FLUID project
// Usage:  
//   murgaLua  lua_from_fluid.lua  input.fl  [output.lua]

// Copyright (c) 2008 Patrick Rapin by Olivetti Engineering SA
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// Configuration variables. Change the fields of the configuration table to 
// match your preferences. You can also override them on the command line.
local default_configuration = {
	// If "shebang" is defined as a file path to the Lua interpreter, a Unix shebang will
	// be added at top of the file. If the string contains backslashes or end in .exe,
	// a batch header is added to turn the script into a valid Windows .BAT file.
	shebang = null,
	
	// Indentation parameter. If it is a number, that number of spaces will be used 
	// for each level. If it is a string, it is places once for each level.
	indentation = 2,
	
	// Current widget variable name. FLUID normally uses "o" when generating code.
	currentvar = "o",
	
	// Internationalization text function. If defined to a function name, every 
	// text label will be passed to that function. If not defined, the parameter in
	// "Project Settings... Internationalization... GNU gettext... Function" will be used.
	textfilter = "_tr",
	
	// If set to "syntax", the syntax of the generated file is checked at the end.
	// If set to "run", the generated script is run
	check = "none",
}

// Global variables
local Grammar, source_file, configuration;
// Forward declaration of functions
//local DecodeOption, ReadFile, BuildGrammar, GetItem, ParseItem, ParseGroup, ParseAttributes;
// Table of functions for rendering
local Write = {}, FindMembers = {};
// Functions to output data into a file or stdout
local Output, Output2;// OpenOutput, WriteScript, WriteVariables, GetAccessMode;

// In this function are concentrated most of the bugs... The list try to enumerate all
// 
function BuildGrammar(){
	local fields = { name = "string", body = "group", attr = "widget"  };
	local types = [ [ "name", "attr" ], [ "name", "attr", "body" ], [ "name" ] ];
	local list1 = [  "comment", "code", "decl", "MenuItem", "Fl_" ];
	local list2 = [ "class", "codeblock", "declblock", "Fl_Window", "Fl_Group", "widget_class",
		"Fl_Choice", "Fl_Input_Choice", "Submenu", "Function", "Fl_Scroll", "Fl_Menu_Bar", 
		"Fl_Tabs", "Fl_Menu_Button", "Fl_Wizard", "Fl_Pack", "Fl_Tile", "Fl_Table" /*, "Fl_Tree"*/ ];
	local list3 = [ "version", "header_name", "code_name", "gridx", "gridy", "snap", 
		"i18n_type", "i18n_include", "i18n_function", "i18n_file", "i18n_set" ];
	local str_methods = [ "tooltip", "type", "box", "down_box", "xclass", "labeltype" ];
	local str_ext_methods = [ "label", "callback", "after", "image", "user_data", "class",
		"code0", "code1", "code2", "code3", ":", "macro_name", "dirty_name" ];
	local number_methods = [ "labelsize", "align", "value", "textcolor", "selection_color",
		"labelcolor", "color", "labelfont", "textfont", "when", "textsize", "step", 
		"maximum", "minimum", "shortcut" ];
	local img_list = [ "GIF", "BMP", "JPEG", "XBM", "XPM", "PNG" ];
	local composed_list = [ "Fl_Window", "Fl_Browser", "Fl_Input", "Fl_Output" ];
	local group_list = [ "Fl_Group", "Fl_Browser", "Fl_Color_Chooser", "Fl_Help_View", 
		"Fl_Input_Choice", "Fl_Pack", "Fl_Scroll", "Fl_Tabs", "Fl_Spinner", 
		"Fl_Text_Display", "Fl_Tile", "Fl_Window", "Fl_Wizard" ];
	local images = { BM="BMP", PPM="PNM", PGM="PNM", PBM="PNM", JPG="JPEG" };
	local FLUID = {Normal=0, Vertical=0, Dot=0, Integer=0, normal=0, No_Select=0, BOTH=0, 
		Toggle=1, Horizontal=1, Simple=1, Line=1, Float=1, popup1=1, Select=1, HORIZONTAL=1, Vert_Fill=2,
		Fill=2, Int=2, popup2=2, Hold=2, VERTICAL=2, Horz_Fill=3, popup3=3, Multi=3,
		Vert_Knob=4, Multiline=4, popup12=4, Horz_Knob=5, Secret=5, popup23=5, Radio=102,
		HORIZONTAL_ALWAYS=5, popup13=6, VERTICAL_ALWAYS=6, popup123=7, BOTH_ALWAYS=7,
		["Vert Fill"]=2, ["Horz Fill"]=3, ["Vert Knob"]=4, ["Horz Knob"]=5 };
	local menu_const = { Toggle = 2, Radio = 8, divider=128, deactivate=1, hide=16 };
	local attr = { xywh = "list", size_range = "list" };
	local methods = {}, composed = {}, groups = {};
	local grammar = { attr = attr, konst = FLUID, images = images, groups = groups,
		methods = methods, composed = composed, menu_const = menu_const };
	
	foreach( k,v in fields) fields[k] = [ k, v ];
	foreach(i in types) for(local j=0, len = i.len(); j < len; ++j) i[j] = fields[i[j]];
	foreach(k in list1) grammar[k] <- types[0];
	foreach(k in list2) grammar[k] <- types[1];
	foreach(k in list3) grammar[k] <- types[2];
	foreach(k in str_methods) attr[k] <- "string";
	foreach(k in str_ext_methods) attr[k] <- "string";
	foreach(k in number_methods) attr[k] <- "integer";
	foreach(k in img_list) images[k] <- k;
	foreach(k in str_methods) methods[k] <- true;
	foreach(k in number_methods) methods[k] <- true;
	foreach(k in composed_list) composed[k] <- true;
	foreach(k in group_list) groups[k] <- true;
	return grammar;
}

function ReadFile(filename){
	local fd = file(filename, "rt");
	local data = fd.read(fd.len());
	fd.close();
	// Replace escaped characters to simplify parsing
	data = data.gsub("\\([^%d])", function(c) {return format("\\%03d", c[0]); return true; })
	return data;
}

function GetItem(pos, type, parent=null){
	local p1, p2, item;
	source_file.find_lua("^%s*([%w_%.%-<>:]+)%s+", function(start, end, match){
		p1 = start; p2 = end; item = match;
	}, pos);
	if (p1){ 
		if (type == "integer") return [item.tointeger(), p2+1];
		assert(type == "string");
		return [item, p2+1];
	}
	p1 = null;
	source_file.find_lua("^%s*(%b{})%s+", function(start, end, match){
		p1 = start; p2 = end; item = match;
	}, pos);
	assert(p1);

	local content = item.slice(1,-1);
	if (type == "string") item = content.gsub("\\(%-?%d%d%d)", function(val) {return str_from_chars(val.tointeger());});
	else if (type == "list"){
		item = [];
		content.gmatch("%-?%d+", function(k){
			item.push(k.tointeger());
			return true;
		});
	}
	else if (type == "group") 	item = ParseGroup(pos+1, pos+content.len(), parent);
	else if (type == "widget") item = ParseAttributes(pos+1, pos+content.len());
	return [item, p2+1];
}

function ParseItem(pos, parent){
	local rc, key, item = {};
	rc = GetItem(pos, "string", item);
	key = rc[0]; pos = rc[1]; 
	item.key <- key;
	local modes = Grammar.get(key, false);
	if (!modes && key.match("Fl_")) modes = Grammar.Fl_;
	assert(modes);
	foreach(mode in modes){
		local name = mode[0], type = mode[1];
		rc = GetItem(pos, type, item);
		key = rc[0]; pos = rc[1]; 
		item[name] <- key;
	}
	return [item, pos];
}

function ParseGroup(pos, endpos, parent){
	local rc, item
	local group = [];
	while (pos < endpos){
		rc = ParseItem(pos, group);
		item = rc[0]; pos = rc[1]; 
		item.parent <- parent;
		group.push(item);
	}
	return group;
}

function ParseAttributes(pos, endpos){
	local rc, item, key;
	local attr = {};
	while (pos < endpos){
		rc = GetItem(pos, "string");
		key = rc[0]; pos = rc[1]; 
		local type = Grammar.attr.get(key, false);
		if (type){
			rc = GetItem(pos, type);
			item = rc[0]; pos = rc[1]; 
			attr[key] <- item;
		}
		else attr[key] <- true;
	}
	return attr;
}

function OpenOutput(out){
	if (type(configuration.indentation) == "integer"){
		configuration.indentation = " ".rep(configuration.indentation);
	}

	local indstr = function(k){
		return configuration.indentation.rep(k);
	}
	//local fended = true;
	local fOutput = function(ind, ...){
		//if (fended) 
		out.push(indstr(ind));
		local str = format.acall2(this, vargv);
		//print(select(1, ...))
		// Indent all code, except multi-line strings
		if (! str.match("[^%-]%[=*%[.*\n.*%]=*%]") ){
			str = str.gsub("([^\\\n]\n)(.)", "%1" + indstr(ind) + "%2");
		}
		out.push(str);
		//fended = str.match("\n$");
	}
	return fOutput;
}

Write.Function <- function(t, ind){
	if(t.name.startswith("__constructor")) return;
	Output(ind, "function %s\n", t.name);
	t.varname <- "res";
	Write.group(t.body, ind+1);
	local names = [];
	foreach(i in t.body) names.push(i.get("varname", null));
	if (names.len() > 0) Output(ind+1, "return %s;\n", names.concat(", "));
	Output(ind, "}\n\n");
}

Write.Atributes <- function(t, ind, name){
	local lname;
	local klass = t.attr.get("class", false) || t.key;
	if (klass.match("^Fl_")) lname = name;
	else lname = name ;

	//if (klass.match("Fl_Button") t.attr.when = 12;
	
	foreach( k,v in t.attr) {
		if (Grammar.methods.get(k, false) && v){
			Output(ind, "%s.%s(", lname, k);
			if (type(v) == "integer") Output(0, "%d", v);
			else if (Grammar.konst.get(v, false)) Output(0, "%d", Grammar.konst[v]);
			else if (v.match("^[A-Z_]+$") ) Output(0, "FL_%s", v);
			else Output(0, "%s(%q)", configuration.textfilter, v);
			Output(0, ");\n");
		}			
	}
}

Write.widget <- function(t, ind){
	local fgroup = Grammar.groups.get(t.key, false);
	if (Grammar.composed.get(t.key, false) && t.attr.get("type", false)){
		t.key = t.key.slice(0,3) + t.attr.type + t.key.slice(2);
		t.attr.type = null;
	}
	local klass = t.attr.get("class", false) || t.key;
	local constClearLabel = "_ClearLabel";
	local isClearLabel = false;
	if (klass.match(constClearLabel)){ 
		klass = klass.gsub(constClearLabel, ""); 
		isClearLabel = true;
	}
	//if (klass.match("^Fl_")) klass = klass;
	local ar = t.attr.xywh;
	local x = ar[0], y = ar[1], w = ar[2], h = ar[3];
	local name = configuration.currentvar;
	t.xoffset <- t.parent.get("xoffset", "");
	t.yoffset <- t.parent.get("yoffset", "");
	local newMethodCallStr, firstParamStr;
	if (klass.match("^fltk%.")) {
		newMethodCallStr = "";
		firstParamStr = "";
	}
	else
	{
		newMethodCallStr = "";
		firstParamStr = "";
	}
	//print(t.key, name, klass)
	Output(ind, "{\n"); 
	++ind;
	local dirty_name =  t.attr.get("dirty_name", false);
	local need_var_assign = true;
	if(dirty_name && dirty_name[0] == '@'){
		need_var_assign = false;
		dirty_name = dirty_name.slice(1);
		Output(ind, "local o = %s;\n", dirty_name);
		Output(ind, "Fl_Group.current()->add(o);\n", dirty_name);
		Output(ind, "o->resize(%s%s%d, %s%d, %d, %d);\n",  firstParamStr, t.xoffset, x, t.yoffset, y, w, h);
		if (! isClearLabel && t.attr.get("label", false)) 
			Output(ind, "o->lable(%s(%q));\n", configuration.textfilter, t.attr.label);
	}
	else
	{
		Output(ind, "local %s = %s%s(%s%s%d, %s%d, %d, %d", name, 
			klass, newMethodCallStr, firstParamStr, t.xoffset, x, t.yoffset, y, w, h);
		if (! isClearLabel && t.attr.get("label", false)) Output(0, ", %s(%q)", configuration.textfilter, t.attr.label);
		Output(0, ");\n");
	}	

	t.varname <- name;
	if (t.name.len() > 0){ 
		local obj_name;
		if (t.name.find(" ") >= 0){
			local tbl_name = [];
			t.name.gmatch("%S+", function(word){
				tbl_name.push(word);
				return true;
			});
			obj_name = tbl_name[1];
			local obj_fld_name ;
			local sep_pos = obj_name.find(":");
			if (sep_pos >= 0){
				obj_fld_name = obj_name.slice(0, sep_pos-1);
				obj_name = obj_name.slice(sep_pos+1);
			}
			else obj_fld_name = obj_name;

			Output(ind, "db_%s_map(\"%s\", %s, \"%s\", %s);\n", tbl_name[0], obj_fld_name, name, 
				tbl_name.get(2, "rw"), tbl_name.get(3, "0"));
		}
		else obj_name = t.name;

		if(need_var_assign) Output(ind, "%s = %s;\n", obj_name, name);
		t.varname = t.name;
	}
	
	Write.Atributes(t, ind, name);

	local img = t.attr.get("image", false);
	if (img){
		local ext = img.match("%.(%a+)$").toupper();
		Output(ind, "%s.image(Fl_%s_Image(%q));\n", name, Grammar.images[ext], img);
	}
	if (t.attr.get("modal", false)) Output(ind, "%s.set_modal();\n", name);
	if (t.attr.get("non_modal", false)) Output(ind, "%s.set_non_modal();\n", name);
	for(local i=0; i <= 3; ++i){
		local code = t.attr.get("code" + i, false);
		if (code && code.len()) {
			if(code[0] == '='){
				code = code.slice(1);
				local macro = t.attr.get("macro_name", false);
				if(macro){
					macro = macro.split(' ');
					code = code.gsub("$%((%d+)%)", function(m){
						local idx = m.tointeger();
						return macro.get(idx, "?");
					});
					code = code.gsub("$%(([^)]+)%)", function(m){
						if(m == "name") return t.name;
						return "?";
					});					
					Output(ind, "%s\n", code);
				}
			}
			else Output(ind, "%s\n", code);
		}
	}
	if (t.attr.get("noborder", false)) Output(ind, "%s.clear_border();\n", name);
	if (t.attr.get("size_range", false)) Output(ind, "%s.size_range(%s);\n", name, table.concat(t.attr.size_range, ", "));
	local cb = t.attr.get("callback", false);
	if (cb){
		if (cb.match("^[%w_]+$")) Output(ind, "%s.callback(%s);\n", name, cb);
		else
		{
			Output(ind, "%s.callback(function(sender, udata){\n", name);
			Output(ind+1, "%s\n", cb);
			Output(ind, "});\n");
		}
	}
	if (t.get("body", false) && t.body.len() > 0) {
		Output(ind, "{\n");
		Write.group(t.body, ind+1);
		Output(ind, "}\n");
	}
	
	if (fgroup){ 
		if (klass.match("^fltk%.")) Output(ind, "%s.end();\n", name);
		else Output(ind, "%s.end();\n", name);
	}
	if (t.attr.get("resizable", false)) { 
		if (t.parent.key != "Function"){
			Output(ind, "Fl_Group.current().resizable(%s);\n", name);
			t.parent.resized <- true;
		}
		else if (! t.get("resized", false) ) Output(ind, "%s.resizable(%s);\n", name, name);
	}
	Output(ind-1, "}\n"); 
}

Write.Submenu <- function(t, ind){
	if (t.parent.get("path", false)) t.path <- t.parent.path + "/" + t.attr.label;
	else t.path <- t.attr.label;

	Output(ind, "//%s %s\n", t.name, t.path);
	if (t.body.len() > 0) Write.group(t.body, ind);
	else Write.MenuItem(t, ind);
}

Write.MenuItem <- function(t, ind){
	if (t.parent.get("path", false)) t.path <- t.parent.path + "/" +  t.attr.get("label", "");
	else t.path <- t.attr.label;

	local w = t;
	while (!w.get("varname", false)) w = w.parent;

	local cb = t.attr.get("callback", false);
	
	if (!cb && t.name > ""){
		//cb = "dispatch_func(" + t.name + "_cb, self)";
		//Output(ind, "%s = %s:add(%q", t.name, w.varname, t.path)
		Output(ind, "%s = o.add(%s(%q)", t.name, configuration.textfilter, t.path);
	}
	else
	{
		//Output(ind, "%s:add(%q", w.varname, t.path)
		Output(ind, "o.add(%s(%q)", configuration.textfilter, t.path);
	}

	Output(0, ", %s", t.attr.get("shortcut", "0").tostring());
	if (cb){
		if (cb.match("^[%w_]+$")) Output(ind, ", %s", cb);
		else
		{
			Output(0, ", function(sender, udata){\n");
			Output(ind+1, "%s\n", cb);
			Output(ind, "}");
		}

		if (t.name > ""){
			local ind2 = 0;
			if (t.attr.get("deactivate", false)) Output2(ind2, "// deactivated base_class.%s\n", t.name);
			Output2(ind2, "function base_class.%s_cb(sender);\n", t.name);
			Output2(ind2+2, "fl_alert(\"%s en construccion\");\n", t.name);
			Output2(ind2, "}\n\n", t.name);
		}
	}
	local type = null;
	if(t.attr.get("type", false)) type = Grammar.menu_const[t.attr.type];
	if (t.attr.get("divider", false)){ 
		if (type) type = type + Grammar.menu_const.divider;
		else type = Grammar.menu_const.divider;
	}
	if (t.attr.get("deactivate", false)){ 
		if (type) type = type + Grammar.menu_const.deactivate;
		else type = Grammar.menu_const.deactivate ;
	}
	if (t.attr.get("hide", false)) { 
		if (type) type = type + Grammar.menu_const.hide;
		else type = Grammar.menu_const.hide;
	}
	if (t.attr.get("value", false)){
		if (type) type = type + 4; // FL_MENU_VALUE = 4
		else type = 4; // FL_MENU_VALUE = 4 
	}
	if (type) {
		Output(0, ", null".rep(cb && 1 || 2));
		Output(0, ", %d", type);
	}
	Output(0, ");\n");
}

Write.group <- function(t, ind){
	foreach(i in t){
		if (Write.get(i.key, false))	Write[i.key](i, ind);
		else if (i.key.match("^Fl_")) Write.widget(i, ind);
	}
}

Write.comment <- function(t, ind){
	local text = "\n" + t.name;
	text = text.gsub("(\n%s*)//", "%1//");
	text = text.gsub("(\n%s*)([^-][^-])", "%1// %2");
	Output(ind, "%s\n", text.slice(1,-1));
}

Write.codeblock <- function(t, ind){
	Output(ind, "%s\n", t.name);
	Write.group(t.body, ind+1);
	Output(ind, "%s\n", t.attr.after or "}");
}

Write.code <- function(t, ind){
	Output(ind, "%s\n", t.name);
}

Write.decl <- function(t, ind){}

Write.declblock <- function(t, ind){
	Output(ind, "%s\n", t.name);
	Write.group(t.body, ind+1);
	Output(ind, "%s\n", t.attr.get("after", "}"));
}

Write["class"] <- function(t, ind){
	Output(ind, "class %s extends %s {\n", t.name, t.get("class", "?"));
	local vars = [];
	FindMembers.group(t.body, vars);
	WriteVariables(ind+1, vars, "class members");
	Output(ind+1, "// Create member functions\n"	);
	Write.group(t.body, ind+1);
	if (t.constructor) Output(ind+1, "foreach(i in {%s}) i.show();\n", t.constructor);
	Output(ind+1, "return %s;\n", t.name);
	Output(ind, "}\n\n");
}

Write.widget_class <- function(t, ind){
	local vars = [];
	local wObj = "";
	local typeName;
	//if (t.attr.get("class", false) && t.attr["class"].match("^Fl_")) typeName = "fltk." + t.attr["class"];
	//else  
	typeName = t.attr.get("class", "Fl_Group");

	local widgetType = typeName;
	Output(ind, "class %s extends %s {\n", t.name, widgetType);
	Output(ind+1, "\n")	;

	FindMembers.group(t.body, vars);
	WriteVariables(ind+1, vars, "class members");
	
	local ar = t.attr.xywh;
	local x = ar[0], y = ar[1], w = ar[2], h = ar[3];
	local wlabel = t.attr.get("label", false);

	local strCreateWidget = format("constructor(px=%d, py=%d, pw=%d, ph=%d, pl=%s){\n",
		x, y, w, h, wlabel ? format( "%s(%q)", configuration.textfilter, wlabel): "null");
	Output(ind+1, strCreateWidget);
	Output(ind+2, "base.constructor(px, py, pw, ph, pl);\n");
	if (! (t.attr.get("class", "")).match("_Window$") ) {
		t.xoffset <- "_x + ";
		t.yoffset <- "_y + ";
		Output(ind+2, "local _x = %d, _y = %d;\n", x, y);
	}

	FindMembers.group(t.body, vars);
	//WriteVariables(ind+1, vars, "class members")	
	Output(ind+2, "// Create member functions and widgets\n");
	Write.group(t.body, ind+2);
	
	Output(ind+1, "}\n");
	Output(ind, "}\n\n");
}

FindMembers.group <- function(t, list){
	foreach(i in t){
		if (FindMembers.get(i.key, false)) FindMembers[i.key](i, list);
		else if (i.key.match("^Fl_")) FindMembers.widget(i, list);
		else if (i.key == "Submenu") FindMembers.submenu(i, list);
	}
}

function GetAccessMode(t, mode){
	foreach(i in ["private", "protected", "public"]) if (t.attr.get(i, false)) mode = i;
	return mode;
}

FindMembers.Function <- function(t, list){
	if (t.name == "") t.name = "make_window()";
	local name = t.name.match("[%w_]+");
	if (t.parent.name == name) { // constructor
		t.name = "__constructor()";
		t.parent.constructor <- t.name;
	}
	else 	list.push([ name, GetAccessMode(t, "public") ]);
	FindMembers.group(t.body, list);
}

FindMembers["class"] <- function(t, list){
	list.push([ t.name, "public" ]);
}

FindMembers.widget_class <- FindMembers["class"];

FindMembers.widget <- function(t, list){
	if (t.name.match("^[%w_]+[%.%[]")){
		list.push([ t.name.match("^[%w_]+"), GetAccessMode(t, "public"), "{}" ]);
	}
	else if (t.name.match("^[%w_]+$")){
		list.push([ t.name, GetAccessMode(t, "public") ]);
	}
	if (t.get("body", false)) FindMembers.group(t.body, list);
}

FindMembers.decl <- function(t, list){
	list.push([ t.name, GetAccessMode(t, "private") ]);
}

FindMembers.submenu <- function(t, list){
	foreach(k,v in t) {
		//print("submenu", k,v);
		if(type(v) == "array"){
			foreach(k2,v2 in v) {
				//print("body", k2, v2);
				if(type(v2) == "table"){
					//foreach(k3,v3 in v2) print("menuitem", k3,v3);
					if(v2.get("name", false)) list.push([ v2.name, GetAccessMode(t, "public") ]);
				}
			}
		}
	}
}

function WriteVariables(ind, vars, type){
	if (vars.len() == 0) return;
	local defined = {};
	Output(ind, "// Declaration of %s\n", type);
	foreach(i in vars) {
		if (defined.get(i[0], false)) {}
		else if (i[1] == "public") Output(ind, "%s = null;\n", i[0]);
		else {
			local multi_decl = i[0].strip().split(';');
			if(multi_decl.len() > 1){
				foreach(d in multi_decl) {
					d = d.strip();
					local isPointer = d.find("*");
					if( isPointer >= 0){
						Output(ind, "%s = null;\n", d.slice(isPointer+1));
					}
					else Output(ind, "%s;\n", d);
				}
			}
			else Output(ind, "local %s = %s;\n", i[0], i.get(2, "null"));
		}
		defined[i[0]] <- true;
	}
	Output(ind, "\n");
}

function WriteScript(tree){
	local vars = [];
	local ind = 0;
	local fct = configuration.textfilter;
	foreach(k in tree) if (k.key == "i18n_function") fct = fct || k.name;

	configuration.textfilter = fct || "";
	if (configuration.shebang){
		if (configuration.shebang.match("\\") || configuration.shebang.match("%.[eE][xX][eE]")){
			Output(ind, "rem=[[ Start Lua script\n");
			Output(ind, "@echo off\n");
			Output(ind, "%s %%0 %%*\n", configuration.shebang);
			Output(ind, "exit /B\n]]\n\n");
		}
		else 	Output(ind, "#! %s\n", configuration.shebang);
	}
	FindMembers.group(tree, vars);
	//WriteVariables(ind, vars, "global variables");
	Write.group(tree, ind);
	foreach(i in vars) {
		if (i[0] == "main") Output(ind, "main(...){\n");
		else if (i[0] == "make_window") Output(ind, "foreach(i in {make_window()}) i.show();\nFl.run();\n");
	}
}

function DecodeOption(arg){
	local config = {};
	for(local i=0, len=arg.len(); i < len; i +=2){
		if (arg[i].slice(0,1) != "-") {
			local ar = [config];
			for(local iv = i, ivlen = arg.len(); iv < ivlen; ++iv) ar.push(arg[iv]);
			return ar;
		}
		local val = arg[i+1];
		if (val.tointeger()) val = val.tointeger();
		config[arg[i].slice(1)] <- val;
	}
}

// Main global function
function Fluid2SquiLu(infile, outfile, config){
	configuration = {};
	foreach( k,v in default_configuration) configuration[k] <- v;
	foreach( k,v in config || {}) configuration[k] <- v;
	Grammar = BuildGrammar();
	source_file = ReadFile(infile);
	local tree = ParseGroup(source_file.find("\n")+1, source_file.len(), {});
	local outdata = [];
	local outdata2 = [];
	Output = OpenOutput(outdata);
	Output2 = OpenOutput(outdata2);
	WriteScript(tree);
	outdata = outdata.concat();
	outdata2 = outdata2.concat();
	if (outfile == "-") print(outdata);
	else if (outfile){
		local fd = file(outfile, "wt");
		fd.write(outdata);
		fd.close();
	}
	if (outdata2.len() > 0){
		local fd = file("extra-generated-code.lua", "wt");
		fd.write(outdata2);
		fd.close();
	}
	if (configuration.check == "syntax") assert(loadstring(outdata, "@" + (outfile || infile)));
	else if (configuration.check == "run") assert(loadstring(outdata, "@" + (outfile || infile)))();
	return outdata;
}

if (vargv.len() > 0){ 
	local rc = DecodeOption(vargv);
	local config = rc[0], infile = rc[1], outfile = rc.get(2, "-"); 
	Fluid2SquiLu(infile, outfile, config);
}

local isDebugging = false;

function mydebughook(event_type,sourcefile,line,funcname)
{
	if(line == 140 || (isDebugging && event_type == 'l')){
/*
		foreach(k,v in getstackinfos(2)) {
			if(type(v) == "table") {
				foreach(k2, v2 in v) print(k2,v2);
			}
			else print(k,v);
		}
*/
		local pos = getstackinfos(2).locals.get("pos", false);
		if(isDebugging || pos == 8926) {
			foreach(k,v in getstackinfos(2)) {
				if(type(v) == "table") {
					foreach(k2, v2 in v) {
						local val = v2;
						if(type(val) == "string"){
							if(val.len() > 30) val = val.slice(0,30);
						}
						print(k2,val);
					}
				}
				else print(k,v);
			}
			print("Debug stop. Press 'c' to continue, 'n' to line step !", line);
			local cmd = stdin.readn('c');
			switch(cmd){
				case 'c': isDebugging = false; break;
				case 'n': isDebugging = true; break;
			}
		}
	}
/*
	local fname=funcname ? funcname:"unknown";
	local srcfile=sourcefile ? sourcefile:"unknown"
	switch (event_type) {
		case 'l': //called every line(that contains some code)
			::print("LINE line [" + line + "] func [" + fname + "]");
			::print("file [" + srcfile + "]\n");
		break;
		case 'c': //called when a function has been called
			::print("LINE line [" + line + "] func [" + fname + "]");
			::print("file [" + srcfile + "]\n");
		break;
		case 'r': //called when a function returns
			::print("LINE line [" + line + "] func [" + fname + "]");
			::print("file [" + srcfile + "]\n");
		break;
	}
*/
}
//enabledebuginfo(true);
//setdebughook(mydebughook);

//Fluid2SquiLu("sqlite3-cc-gui.fl", "-", {});
//Fluid2SquiLu("ourbiz-gui.fl", "-", {});
//Fluid2SquiLu("entity-edit-gui.fl", "-", {});
//Fluid2SquiLu("orders-edit-gui.fl", "-", {});
//Fluid2SquiLu("barchart-gui.fl", "-", {});
//Fluid2SquiLu("history-gui.fl", "-", {});
//Fluid2SquiLu("product-kit-gui.fl", "-", {});
//Fluid2SquiLu("product-prices-gui.fl", "-", {});
//Fluid2SquiLu("print-preview-gui.fl", "-", {});
//Fluid2SquiLu("order-page-gui.fl", "-", {});
