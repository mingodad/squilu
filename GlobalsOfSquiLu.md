A list of all globals of `SquiLu`.

# Using this script #
```
local base_class_key = " [base class] -> ";
local function getMembersNotInBaseClass(klass, klass_members){
	local bklass = klass.getbase();
	local bklass_members = {};
	if(bklass) foreach(k,v in bklass) bklass_members[k] <- true;
	foreach(k,v in klass) {
		if(bklass_members.get(k, klass) == klass) { //not found
			klass_members.push([k,v]);
		}
	}
	klass_members.push([base_class_key, gettypetag(bklass)]);
}

local function getParamsCheck(pc){
	local mask_values = {
		["NULL"]		=	0x00000001,
		INTEGER		=	0x00000002,
		FLOAT		=	0x00000004,
		BOOL		=	0x00000008,
		STRING		=	0x00000010,
		TABLE		=	0x00000020,
		ARRAY		=	0x00000040,
		USERDATA	=	0x00000080,
		CLOSURE		=	0x00000100,
		NATIVECLOSURE	= 0x00000200,
		GENERATOR	=	0x00000400,
		USERPOINTER	=	0x00000800,
		THREAD		=	0x00001000,
		FUNCPROTO	=	0x00002000,
		CLASS		=	0x00004000,
		INSTANCE	=	0x00008000,
		WEAKREF		=	0x00010000,
		OUTER		=	0x00020000,
	}
	local ANY = -1;
	local tmp = [];
	foreach(v in pc){
		if(v == ANY) {
			tmp.push("ANY");
			continue;
		}
		local m = [];
		foreach(k, mv in mask_values){
			if(mv & v) m.push(k);
		}
		tmp.push(m.concat("|"));
	}
	return "(" + tmp.concat(" , ") + ")";
}

/*local do not allow recursion*/
function showVars(avar, myvars, prefix=null){
	local isClass = type(avar) == "class";
	local isTable = type(avar) == "table";

	if(isClass) getMembersNotInBaseClass(avar, myvars);
	else 
	{
		foreach(k,v in avar) {
			/* class and instance do not work with this code
			if(isTable){
				if(avar.rawin(k)) {
					myvars.push([k,v]);
				}
			}
			else */
			myvars.push([k,v]);
		}
	}
	myvars.sort(@(a,b) a[0] <=> b[0]);
	foreach(v in myvars) {
		local vtype = type(v[1]);
		if(prefix) print1(prefix);
		local vvalue = "";
		try { 
			if(vtype == "function"){
				local infos = v[1].getinfos();
				if(infos.native){
					vvalue = getParamsCheck(infos.typecheck);
				}
			}
			else
			{
				vvalue = v[1].tostring(); 
				if(vvalue[0] == '(') vvalue = "";
			}
		} catch(e) {}
		if(v[0] == base_class_key) {
			vtype = "";
			if(vvalue == "") vvalue = "NONE";
		}
		print(v[0], vtype, vvalue);
		if(vtype == "class" || vtype == "table") showVars(v[1], [], prefix ? prefix + "\t" : "\t");
	}
}
//showVars(this, []);

local function hideFromGlobals(){
	/*
	There is a bug in the language that when a class implements the metamethod _get 
	that expects other types than string  make impossible to call default delegate functions
	like here blob/file/std_stream implements _get expecting a number (integer|float).
	local instBlob = blob();
	local weakv = instBlob.weakref();
	*/
	local inst_klass = Decimal(); //SlaveVM(1024);
	local weakv = inst_klass.weakref();
	local intv = 2;
	local tblv = {};
	local arrayv = [];
	local str = "";
	local coro = ::newthread(showVars);
	local function geny(n){
		for(local i=0;i<n;i+=1)
		yield i;
		return null;
	}
	local gtor=geny(10);

	local myvars = [
			["array_delegate", arrayv.getdelegate()],
			["class_delegate", getdefaultdelegate(blob)],
			["instance_delegate", getdefaultdelegate(inst_klass)],
			["closure_delegate", getdefaultdelegate(showVars)],
			["generator_delegate", getdefaultdelegate(gtor)],
			["number_delegate", getdefaultdelegate(intv)],
			["string_delegate", str.getdelegate()],
			["table_delegate", getdefaultdelegate(tblv)],
			["thread_delegate", getdefaultdelegate(coro)],
			["weakref_delegate", getdefaultdelegate(weakv)],
		];

	showVars(this, myvars);
	
	print("\n<<<Constants>>>\n")
	local const_array = [];
	foreach(k,v in getconsttable()){
		const_array.push([k,v]);
	}
	const_array.sort(@(a,b) a[0] <=> b[0]);
	foreach(v in const_array){
		print(v[0], v[1]);
	}
}
hideFromGlobals();
```


# As of 2013-02-08 #
```
Decimal	class	
	 [base class] -> 		NONE
	abs	function	(INSTANCE)
	constructor	function	(INSTANCE , FLOAT|STRING|INSTANCE|INTEGER)
	context	instance	
	iseven	function	(INSTANCE)
	isfinite	function	(INSTANCE)
	isinfinite	function	(INSTANCE)
	isinteger	function	(INSTANCE)
	isnan	function	(INSTANCE)
	isnegative	function	(INSTANCE)
	isodd	function	(INSTANCE)
	ispositive	function	(INSTANCE)
	isqnan	function	(INSTANCE)
	issigned	function	(INSTANCE)
	issnan	function	(INSTANCE)
	isspecial	function	(INSTANCE)
	iszero	function	(INSTANCE)
	max	function	(INSTANCE , INSTANCE)
	min	function	(INSTANCE , INSTANCE)
	next_minus	function	(INSTANCE)
	next_plus	function	(INSTANCE)
	next_toward	function	(INSTANCE , INSTANCE)
DecimalCtx	class	
	 [base class] -> 		NONE
	Clamped	integer	1
	Conversion_syntax	integer	2
	DECIMAL128	integer	128
	DECIMAL32	integer	32
	DECIMAL64	integer	64
	Division_by_zero	integer	4
	Division_impossible	integer	8
	Division_undefined	integer	16
	Fpu_error	integer	32
	IEEE_CONTEXT_MAX_BITS	integer	512
	IEEE_Invalid_operation	integer	954
	Inexact	integer	64
	Invalid_context	integer	128
	Invalid_operation	integer	256
	MAX_EMAX	integer	425000000
	MAX_PREC	integer	425000000
	MIN_EMIN	integer	-425000000
	Malloc_error	integer	512
	Not_implemented	integer	1024
	Overflow	integer	2048
	ROUND_05UP	integer	7
	ROUND_CEILING	integer	2
	ROUND_DOWN	integer	1
	ROUND_FLOOR	integer	3
	ROUND_HALF_DOWN	integer	5
	ROUND_HALF_EVEN	integer	6
	ROUND_HALF_UP	integer	4
	ROUND_TRUNC	integer	8
	ROUND_UP	integer	0
	Rounded	integer	4096
	Subnormal	integer	8192
	Underflow	integer	16384
	clamp	function	(INSTANCE , INTEGER)
	constructor	function	(INSTANCE , INTEGER)
	cr	function	(INSTANCE , INTEGER)
	emax	function	(INSTANCE , INTEGER)
	emin	function	(INSTANCE , INTEGER)
	prec	function	(INSTANCE , INTEGER)
	round	function	(INSTANCE , INTEGER)
	status	function	(INSTANCE , INTEGER)
	traps	function	(INSTANCE , INTEGER)
Fl	class	
	 [base class] -> 		NONE
	FL_PREFERENCES_SYSTEM	integer	0
	FL_PREFERENCES_USER	integer	1
	OPTION_ARROW_FOCUS	integer	0
	OPTION_DND_TEXT	integer	2
	OPTION_SHOW_TOOLTIPS	integer	3
	OPTION_VISIBLE_FOCUS	integer	1
	add_focus_changing_handler	function	(CLASS , NATIVECLOSURE|CLOSURE)
	add_idle	function	(CLASS , NATIVECLOSURE|CLOSURE , ANY)
	add_timeout	function	(CLASS , FLOAT|INTEGER , NATIVECLOSURE|CLOSURE , ANY)
	check	function	(CLASS)
	delete_widget	function	(CLASS , INSTANCE)
	do_widget_deletion	function	(CLASS)
	event	function	(CLASS)
	event_alt	function	(CLASS)
	event_button	function	(CLASS)
	event_button1	function	(CLASS)
	event_button2	function	(CLASS)
	event_button3	function	(CLASS)
	event_buttons	function	(CLASS)
	event_clicks	function	(CLASS , INTEGER)
	event_command	function	(CLASS)
	event_ctrl	function	(CLASS)
	event_dx	function	(CLASS)
	event_dy	function	(CLASS)
	event_is_click	function	(CLASS , INTEGER)
	event_key	function	(CLASS , INTEGER)
	event_length	function	(CLASS)
	event_shift	function	(CLASS)
	event_state	function	(CLASS , INTEGER)
	event_x	function	(CLASS)
	event_x_root	function	(CLASS)
	event_y	function	(CLASS)
	event_y_root	function	(CLASS)
	get_font	function	(CLASS , INTEGER)
	get_font_name	function	(CLASS , INTEGER)
	h	function	(CLASS)
	option	function	(CLASS , INTEGER , BOOL)
	ready	function	(CLASS)
	run	function	(CLASS)
	scheme	function	(CLASS , STRING)
	screen_count	function	(CLASS)
	scrollbar_size	function	(CLASS , INTEGER)
	set_font	function	(CLASS , INTEGER , STRING|INTEGER)
	set_fonts	function	(CLASS , STRING)
	visible_focus	function	(CLASS , INTEGER)
	visual	function	(CLASS , INTEGER)
	w	function	(CLASS)
	x	function	(CLASS)
	y	function	(CLASS)
Fl_Box	class	
	 [base class] -> 		Fl_Widget
	constructor	function	(INSTANCE , INTEGER , INTEGER , INTEGER , INTEGER , NULL|STRING)
	draw	function	(INSTANCE)
Fl_Browser	class	
	 [base class] -> 		Fl_Browser_
Fl_Browser_	class	
	 [base class] -> 		Fl_Group
	textcolor	function	(INSTANCE , INTEGER)
Fl_Button	class	
	 [base class] -> 		Fl_Widget
	constructor	function	(INSTANCE , INTEGER , INTEGER , INTEGER , INTEGER , NULL|STRING)
	down_box	function	(INSTANCE , INTEGER)
	setonly	function	(INSTANCE)
	value	function	(INSTANCE , INTEGER)
Fl_Check_Button	class	
	 [base class] -> 		Fl_Light_Button
Fl_Choice	class	
	 [base class] -> 		Fl_Menu_
Fl_Device	class	
	 [base class] -> 		NONE
	class_name	function	(INSTANCE)
Fl_Display_Device	class	
	 [base class] -> 		Fl_Surface_Device
	display_device	function	(CLASS)
Fl_Double_Window	class	
	 [base class] -> 		Fl_Window
Fl_Float_Input	class	
	 [base class] -> 		Fl_Input
Fl_Group	class	
	 [base class] -> 		Fl_Widget
	add	function	(INSTANCE , INSTANCE)
	begin	function	(INSTANCE)
	constructor	function	(INSTANCE , INTEGER , INTEGER , INTEGER , INTEGER , NULL|STRING)
	current	function	(CLASS)
	end	function	(INSTANCE)
	insert	function	(INSTANCE , INSTANCE , INSTANCE|INTEGER)
	remove	function	(INSTANCE , INSTANCE|INTEGER)
	resizable	function	(INSTANCE , INSTANCE)
Fl_Help_View	class	
	 [base class] -> 		Fl_Group
	find	function	(INSTANCE , STRING|INTEGER)
	link	function	(INSTANCE , NULL|NATIVECLOSURE|CLOSURE)
	reformat	function	(INSTANCE)
	textcolor	function	(INSTANCE , INTEGER)
	topline	function	(INSTANCE , STRING|INTEGER)
	value	function	(INSTANCE , STRING)
Fl_Hold_Browser	class	
	 [base class] -> 		Fl_Browser
Fl_Image	class	
	 [base class] -> 		NONE
	cheap_rtti_info	function	(CLASS)
	className	function	(CLASS)
	constructor	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
	copy	function	(INSTANCE , INTEGER , INTEGER)
	count	function	(INSTANCE)
	d	function	(INSTANCE)
	data	function	(INSTANCE)
	h	function	(INSTANCE)
	ld	function	(INSTANCE)
	uncache	function	(INSTANCE)
	w	function	(INSTANCE)
Fl_Input	class	
	 [base class] -> 		Fl_Input_
	decimal_places	function	(INSTANCE , INTEGER)
	default_number_format	function	(CLASS , STRING)
	number_format	function	(INSTANCE , STRING)
	right_to_left	function	(INSTANCE , INTEGER)
	use_numeric_format	function	(INSTANCE , INTEGER)
Fl_Input_	class	
	 [base class] -> 		Fl_Widget
	constructor	function	(INSTANCE , INTEGER , INTEGER , INTEGER , INTEGER , NULL|STRING)
	value	function	(INSTANCE , STRING)
Fl_Int_Input	class	
	 [base class] -> 		Fl_Input
Fl_JPEG_Image	class	
	 [base class] -> 		Fl_RGB_Image
	encode	function	(CLASS , INSTANCE , INTEGER)
Fl_Light_Button	class	
	 [base class] -> 		Fl_Button
Fl_Menu_	class	
	 [base class] -> 		Fl_Widget
	add	function	(INSTANCE , STRING , NULL|STRING|INTEGER , NATIVECLOSURE|FLOAT|CLOSURE|INTEGER , ANY , INTEGER)
	constructor	function	(INSTANCE , INTEGER , INTEGER , INTEGER , INTEGER , NULL|STRING)
	copy	function	(INSTANCE , ARRAY , ANY)
	down_box	function	(INSTANCE , INTEGER)
	value	function	(INSTANCE)
Fl_Menu_Bar	class	
	 [base class] -> 		Fl_Menu_
Fl_Menu_Button	class	
	 [base class] -> 		Fl_Menu_
	POPUP1	integer	1
	POPUP12	integer	3
	POPUP123	integer	7
	POPUP13	integer	5
	POPUP2	integer	2
	POPUP23	integer	6
	POPUP3	integer	4
	popup	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER)
Fl_Output	class	
	 [base class] -> 		Fl_Input
Fl_PNG_Image	class	
	 [base class] -> 		Fl_RGB_Image
	encode	function	(CLASS , INSTANCE)
Fl_Pack	class	
	 [base class] -> 		Fl_Group
	spacing	function	(INSTANCE , INTEGER)
	with_label	function	(INSTANCE , INTEGER)
Fl_Paged_Device	class	
	 [base class] -> 		Fl_Surface_Device
	end_job	function	(INSTANCE)
	end_page	function	(INSTANCE)
	origin	function	(INSTANCE , INTEGER , INTEGER)
	print_widget	function	(INSTANCE , INSTANCE , INTEGER , INTEGER)
	start_job	function	(INSTANCE , TABLE)
	start_page	function	(INSTANCE)
Fl_Pdf_File_Device	class	
	 [base class] -> 		Fl_Paged_Device
	compress	function	(INSTANCE , BOOL)
	constructor	function	(INSTANCE)
Fl_PostScript_File_Device	class	
	 [base class] -> 		Fl_Paged_Device
	file_chooser_title	function	(CLASS , STRING)
Fl_RGB_Image	class	
	 [base class] -> 		Fl_Image
	max_size	function	(CLASS , INTEGER)
Fl_Radio_Button	class	
	 [base class] -> 		Fl_Button
Fl_Repeat_Button	class	
	 [base class] -> 		Fl_Button
Fl_Return_Button	class	
	 [base class] -> 		Fl_Button
Fl_Round_Button	class	
	 [base class] -> 		Fl_Light_Button
Fl_Scroll	class	
	 [base class] -> 		Fl_Group
Fl_Secret_Input	class	
	 [base class] -> 		Fl_Input
Fl_Surface_Device	class	
	 [base class] -> 		Fl_Device
	set_current	function	(INSTANCE)
Fl_Tabs	class	
	 [base class] -> 		Fl_Group
	value	function	(INSTANCE , INSTANCE)
Fl_Text_Buffer	class	
	 [base class] -> 		NONE
	constructor	function	(INSTANCE , INTEGER , INTEGER)
	input_file_was_transcoded	function	(INSTANCE)
	length	function	(INSTANCE , INTEGER , INTEGER)
	loadfile	function	(INSTANCE , STRING , INTEGER)
	search_forward	function	(INSTANCE , INTEGER , STRING , INTEGER)
	select	function	(INSTANCE , INTEGER , INTEGER)
	text	function	(INSTANCE , STRING)
Fl_Text_Display	class	
	 [base class] -> 		Fl_Group
	buffer	function	(INSTANCE , INSTANCE)
	highlight_data	function	(INSTANCE , INSTANCE , ARRAY , INTEGER , INTEGER , NATIVECLOSURE|CLOSURE , ANY)
	insert_position	function	(INSTANCE , INTEGER)
	show_insert_position	function	(INSTANCE)
	wrap_mode	function	(INSTANCE , INTEGER , INTEGER)
Fl_Text_Editor	class	
	 [base class] -> 		Fl_Text_Display
Fl_Text_Editor_Buffered	class	
	 [base class] -> 		Fl_Text_Editor
	value	function	(INSTANCE , STRING)
Fl_Widget	class	
	 [base class] -> 		NONE
	activate	function	(INSTANCE)
	align	function	(INSTANCE , INTEGER)
	argument	function	(INSTANCE , INTEGER)
	box	function	(INSTANCE , INTEGER)
	callback	function	(INSTANCE , NATIVECLOSURE|CLOSURE , ANY)
	changed	function	(INSTANCE)
	changed2	function	(INSTANCE)
	cheap_rtti_info	function	(CLASS)
	classId	function	(INSTANCE)
	className	function	(CLASS)
	classRTTI	function	(INSTANCE)
	clear_changed	function	(INSTANCE)
	clear_changed2	function	(INSTANCE)
	color	function	(INSTANCE , INTEGER)
	copy_label	function	(INSTANCE , STRING)
	deactivate	function	(INSTANCE)
	h	function	(INSTANCE , FLOAT|INTEGER)
	handle	function	(INSTANCE , INTEGER)
	hide	function	(INSTANCE)
	image	function	(INSTANCE , NULL|INSTANCE)
	inherits_from	function	(INSTANCE , USERPOINTER)
	label	function	(INSTANCE , NULL|STRING)
	labelcolor	function	(INSTANCE , INTEGER)
	labelfont	function	(INSTANCE , INTEGER)
	labelsize	function	(INSTANCE , INTEGER)
	labeltype	function	(INSTANCE , INTEGER)
	parent	function	(INSTANCE)
	parent_root	function	(INSTANCE)
	position	function	(INSTANCE , INTEGER , INTEGER)
	redraw	function	(INSTANCE)
	redraw_label	function	(INSTANCE)
	resize	function	(INSTANCE , INTEGER , INTEGER , INTEGER , INTEGER)
	selection_color	function	(INSTANCE , INTEGER)
	set_changed	function	(INSTANCE)
	set_changed2	function	(INSTANCE)
	show	function	(INSTANCE)
	textfont	function	(INSTANCE , INTEGER)
	textsize	function	(INSTANCE , INTEGER)
	tooltip	function	(INSTANCE , STRING)
	type	function	(INSTANCE , INTEGER)
	w	function	(INSTANCE , FLOAT|INTEGER)
	when	function	(INSTANCE , INTEGER)
	window	function	(INSTANCE)
	x	function	(INSTANCE , FLOAT|INTEGER)
	y	function	(INSTANCE , FLOAT|INTEGER)
Fl_Window	class	
	 [base class] -> 		Fl_Group
	on_first_time_show	function	(INSTANCE)
	set_non_modal	function	(INSTANCE)
	show_main	function	(INSTANCE)
	shown	function	(INSTANCE)
Flu_Combo_Box	class	
	 [base class] -> 		Fl_Group
	add_item	function	(INSTANCE , ANY , STRING)
	clear_items	function	(INSTANCE)
	get_data_at	function	(INSTANCE , INTEGER)
	hasItems	function	(INSTANCE)
	select_by_data	function	(INSTANCE , ANY)
Flu_Combo_List	class	
	 [base class] -> 		Flu_Combo_Box
	list	function	(INSTANCE)
Flu_Combo_Tree	class	
	 [base class] -> 		Flu_Combo_Box
	tree	function	(INSTANCE)
Flu_Tree_Browser	class	
	 [base class] -> 		Fl_Group
	all_branches_always_open	function	(INSTANCE , BOOL)
	auto_branches	function	(INSTANCE , BOOL)
	branch_text	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
	leaf_text	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
	shaded_entry_colors	function	(INSTANCE , INTEGER , INTEGER)
	show_branches	function	(INSTANCE , BOOL)
Flv_Data_Table	class	
	 [base class] -> 		Flv_Table
	draw_offset	function	(INSTANCE , FLOAT|INTEGER)
Flv_List	class	
	 [base class] -> 		Fl_Group
	add_callback_when	function	(INSTANCE , INTEGER)
	callback_when	function	(INSTANCE , INTEGER)
	clear_callback_when	function	(INSTANCE , INTEGER)
	feature	function	(INSTANCE , INTEGER)
	feature_add	function	(INSTANCE , INTEGER)
	feature_remove	function	(INSTANCE , INTEGER)
	get_style	function	(INSTANCE , INSTANCE , INTEGER , INTEGER)
	global_style	function	(INSTANCE)
	has_scrollbar	function	(INSTANCE , INTEGER)
	row	function	(INSTANCE , INTEGER)
	row_style	function	(INSTANCE)
	rows	function	(INSTANCE , INTEGER)
	scrollbar_width	function	(INSTANCE , INTEGER)
	why_event	function	(INSTANCE , INTEGER)
Flv_Style	class	
	 [base class] -> 		NONE
	align	function	(INSTANCE , INTEGER)
	background	function	(INSTANCE , INTEGER)
	background_defined	function	(INSTANCE)
	border	function	(INSTANCE , FLOAT|INTEGER)
	border_color	function	(INSTANCE , INTEGER)
	border_spacing	function	(INSTANCE , FLOAT|INTEGER)
	font_size	function	(INSTANCE , FLOAT|INTEGER)
	foreground	function	(INSTANCE , INTEGER)
	foreground_defined	function	(INSTANCE)
	height	function	(INSTANCE , FLOAT|INTEGER)
	resizable	function	(INSTANCE , BOOL)
	width	function	(INSTANCE , FLOAT|INTEGER)
	x_margin	function	(INSTANCE , FLOAT|INTEGER)
	y_margin	function	(INSTANCE , FLOAT|INTEGER)
Flv_Style_List	class	
	 [base class] -> 		NONE
Flv_Table	class	
	 [base class] -> 		Flv_List
	col	function	(INSTANCE , INTEGER)
	col_style	function	(INSTANCE)
	cols	function	(INSTANCE , INTEGER)
Mongoose	class	
	 [base class] -> 		NONE
	constructor	function	(INSTANCE)
	modify_passwords_file	function	(INSTANCE)
	show_errors_on_stdout	function	(INSTANCE , BOOL)
	start	function	(INSTANCE)
	stop	function	(INSTANCE)
	version	function	(INSTANCE)
SQLite3	class	
	 [base class] -> 		NONE
	IsAutoCommitOn	function	(INSTANCE)
	Null	userdata	
	SQLITE_INTERRUPT	integer	9
	SQLITE_OK	integer	0
	SQLITE_OPEN_CREATE	integer	4
	SQLITE_OPEN_READWRITE	integer	2
	SQLITE_OPEN_SHAREDCACHE	integer	131072
	SQLITE_OPEN_SUBLATIN_NA_LIKE	integer	134217728
	busy_handler	function	(INSTANCE , NULL|NATIVECLOSURE|CLOSURE , ANY)
	busy_timeout	function	(INSTANCE , INTEGER)
	changes	function	(INSTANCE)
	close	function	(INSTANCE)
	constructor	function	(INSTANCE , STRING , INTEGER)
	create_aggregate	function	(INSTANCE , STRING , INTEGER , NATIVECLOSURE|CLOSURE , NATIVECLOSURE|CLOSURE)
	create_function	function	(INSTANCE , STRING , INTEGER , NATIVECLOSURE|CLOSURE)
	db_ptr	function	(INSTANCE)
	enable_shared_cache	function	(INSTANCE , BOOL)
	errcode	function	(INSTANCE)
	errmsg	function	(INSTANCE)
	exec	function	(INSTANCE , STRING)
	exec_dml	function	(INSTANCE , STRING)
	exec_get_first_row	function	(INSTANCE , STRING)
	exec_get_one	function	(INSTANCE , STRING)
	get_db_name	function	(INSTANCE)
	interrupt	function	(INSTANCE)
	key	function	(INSTANCE , STRING)
	last_row_id	function	(INSTANCE)
	prepare	function	(INSTANCE , STRING)
	progress_handler	function	(INSTANCE , NULL|INTEGER , NATIVECLOSURE|CLOSURE , ANY)
	rekey	function	(INSTANCE , STRING)
	set_busy_timeout	function	(INSTANCE , INTEGER)
	sleep	function	(INSTANCE)
	temp_directory	function	(INSTANCE , STRING)
	total_changes	function	(INSTANCE)
	trace	function	(INSTANCE , NULL|NATIVECLOSURE|CLOSURE , ANY)
	version	function	(INSTANCE)
SQLite3Stmt	class	
	 [base class] -> 		NONE
	AS_STRING_ALWAYS	integer	1
	NULL_AS_EMPTY_STR	integer	2
	Null	userdata	
	SQLITE_DONE	integer	101
	SQLITE_OK	integer	0
	SQLITE_ROW	integer	100
	asArray	function	(INSTANCE , INTEGER)
	asArrayOfArrays	function	(INSTANCE , INTEGER)
	asArrayOfTables	function	(INSTANCE , INTEGER)
	asBool	function	(INSTANCE , STRING|INTEGER)
	asFloat	function	(INSTANCE , STRING|INTEGER)
	asInteger	function	(INSTANCE , STRING|INTEGER)
	asSleArray	function	(INSTANCE)
	asString	function	(INSTANCE , STRING|INTEGER)
	asStringOrNull	function	(INSTANCE , STRING|INTEGER)
	asTable	function	(INSTANCE , INTEGER)
	bind	function	(INSTANCE , INTEGER , NULL|BOOL|FLOAT|STRING|INTEGER)
	bind_blob	function	(INSTANCE , INTEGER , STRING)
	bind_empty_null	function	(INSTANCE , INTEGER , NULL|BOOL|FLOAT|STRING|INTEGER)
	bind_names	function	(INSTANCE , TABLE|ARRAY)
	bind_parameter_index	function	(INSTANCE , STRING)
	bind_values	function	(INSTANCE , NULL|BOOL|FLOAT|STRING|INTEGER)
	col	function	(INSTANCE , STRING|INTEGER)
	col_count	function	(INSTANCE)
	col_declared_type	function	(INSTANCE , INTEGER)
	col_name	function	(INSTANCE , INTEGER)
	col_type	function	(INSTANCE , INTEGER)
	colsAsArray	function	(INSTANCE)
	colsAsTable	function	(INSTANCE)
	constructor	function	(INSTANCE , INSTANCE , STRING , NULL|BOOL|FLOAT|STRING|INTEGER)
	finalize	function	(INSTANCE)
	get_db	function	(INSTANCE)
	get_sql	function	(INSTANCE)
	next_row	function	(INSTANCE)
	prepare	function	(INSTANCE , STRING , NULL|BOOL|FLOAT|STRING|INTEGER)
	reset	function	(INSTANCE)
	step	function	(INSTANCE)
	stmt_ptr	function	(INSTANCE)
SlaveVM	class	
	 [base class] -> 		NONE
	call	function	(INSTANCE , BOOL , STRING)
	close	function	(INSTANCE)
	compilestring	function	(INSTANCE , STRING , STRING , BOOL)
	constructor	function	(INSTANCE , INTEGER)
	dofile	function	(INSTANCE , STRING , BOOL , BOOL)
	loadfile	function	(INSTANCE , STRING , STRING , BOOL)
	set	function	(INSTANCE , FLOAT|USERPOINTER|STRING|INTEGER , TABLE|NULL|BOOL|ARRAY|FLOAT|USERPOINTER|STRING|INTEGER)
SqRs232	class	
	 [base class] -> 		NONE
	RS232_BAUD_115200	integer	7
	RS232_BAUD_19200	integer	4
	RS232_BAUD_2400	integer	1
	RS232_BAUD_300	integer	0
	RS232_BAUD_38400	integer	5
	RS232_BAUD_460800	integer	8
	RS232_BAUD_4800	integer	2
	RS232_BAUD_57600	integer	6
	RS232_BAUD_9600	integer	3
	RS232_DATA_5	integer	0
	RS232_DATA_6	integer	1
	RS232_DATA_7	integer	2
	RS232_DATA_8	integer	3
	RS232_DTR_OFF	integer	0
	RS232_DTR_ON	integer	1
	RS232_ERR_CLOSE	integer	3
	RS232_ERR_CONFIG	integer	5
	RS232_ERR_FLUSH	integer	4
	RS232_ERR_IOCTL	integer	10
	RS232_ERR_NOERROR	integer	0
	RS232_ERR_OPEN	integer	2
	RS232_ERR_PORT_CLOSED	integer	11
	RS232_ERR_READ	integer	6
	RS232_ERR_SELECT	integer	8
	RS232_ERR_TIMEOUT	integer	9
	RS232_ERR_UNKNOWN	integer	1
	RS232_ERR_WRITE	integer	7
	RS232_FLOW_HW	integer	1
	RS232_FLOW_OFF	integer	0
	RS232_FLOW_XON_XOFF	integer	2
	RS232_PARITY_EVEN	integer	2
	RS232_PARITY_NONE	integer	0
	RS232_PARITY_ODD	integer	1
	RS232_RTS_OFF	integer	0
	RS232_RTS_ON	integer	1
	RS232_STOP_1	integer	0
	RS232_STOP_2	integer	1
	baud_rate	function	(INSTANCE , INTEGER)
	baud_rate_tostring	function	(ANY , INTEGER)
	close	function	(INSTANCE)
	constructor	function	(INSTANCE)
	data_bits	function	(INSTANCE , INTEGER)
	data_bits_tostring	function	(ANY , INTEGER)
	device	function	(INSTANCE)
	dtr	function	(INSTANCE , INTEGER)
	dtr_tostring	function	(ANY , INTEGER)
	fd	function	(INSTANCE)
	flow_control	function	(INSTANCE , INTEGER)
	flow_control_tostring	function	(ANY , INTEGER)
	flush	function	(INSTANCE)
	open	function	(INSTANCE , STRING)
	parity	function	(INSTANCE , INTEGER)
	parity_tostring	function	(ANY , INTEGER)
	read	function	(INSTANCE , INTEGER , INTEGER , BOOL)
	rts	function	(INSTANCE , INTEGER)
	rts_tostring	function	(ANY , INTEGER)
	stop_bits	function	(INSTANCE , INTEGER)
	stop_bits_tostring	function	(ANY , INTEGER)
	strerror	function	(ANY , INTEGER)
	write	function	(INSTANCE , STRING , INTEGER)
Sq_Fpdf	class	
	 [base class] -> 		NONE
	AcceptPageBreak	function	(INSTANCE)
	AddFont	function	(INSTANCE , STRING , STRING , STRING)
	AddLink	function	(INSTANCE)
	AddPage	function	(INSTANCE , INTEGER , STRING)
	AliasNbPages	function	(INSTANCE , STRING)
	CalcLines	function	(INSTANCE , FLOAT|INTEGER , STRING)
	Cell	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , INTEGER , BOOL , INTEGER)
	CellFit	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , INTEGER , BOOL , INTEGER , INTEGER , INTEGER)
	CellFitScale	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , INTEGER , BOOL , INTEGER)
	CellFitScaleForce	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , INTEGER , BOOL , INTEGER)
	CellFitSpace	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , INTEGER , BOOL , INTEGER)
	CellFitSpaceForce	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , INTEGER , BOOL , INTEGER)
	CheckPageBreak	function	(INSTANCE , FLOAT|INTEGER)
	Circle	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , STRING)
	ClippedCell	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , INTEGER , BOOL , INTEGER)
	ClippingRect	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , INTEGER)
	ClippingText	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , INTEGER)
	Close	function	(INSTANCE)
	Ellipse	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , STRING)
	Error	function	(INSTANCE , STRING)
	Footer	function	(INSTANCE)
	GetAliasNbPages	function	(INSTANCE)
	GetDrawColor	function	(INSTANCE , TABLE)
	GetFillColor	function	(INSTANCE , TABLE)
	GetFontSettings	function	(INSTANCE , TABLE)
	GetFontSize	function	(INSTANCE)
	GetH	function	(INSTANCE)
	GetLeftMargin	function	(INSTANCE)
	GetRightMargin	function	(INSTANCE)
	GetStringWidth	function	(INSTANCE , STRING)
	GetTextColor	function	(INSTANCE , TABLE)
	GetW	function	(INSTANCE)
	GetX	function	(INSTANCE)
	GetY	function	(INSTANCE)
	Header	function	(INSTANCE)
	Image	function	(INSTANCE , STRING , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , STRING , INTEGER)
	IncludeJS	function	(INSTANCE , STRING)
	Line	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
	Link	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , INTEGER)
	Ln	function	(INSTANCE , FLOAT|INTEGER)
	MultiCell	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , INTEGER , BOOL)
	MultiCellBlt	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , STRING , STRING , BOOL , INTEGER)
	Open	function	(INSTANCE)
	Output	function	(INSTANCE , STRING|INSTANCE , INTEGER)
	PageNo	function	(INSTANCE)
	Rect	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , STRING)
	Rotate	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
	RotatedText	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , FLOAT|INTEGER)
	RoundedRect	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , STRING)
	SetAlpha	function	(INSTANCE , FLOAT|INTEGER , STRING)
	SetAuthor	function	(INSTANCE , STRING)
	SetAutoPageBreak	function	(INSTANCE , INTEGER , FLOAT|INTEGER)
	SetCompression	function	(INSTANCE , BOOL)
	SetCreator	function	(INSTANCE , STRING)
	SetDash	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER)
	SetDisplayMode	function	(INSTANCE , INTEGER , INTEGER)
	SetDoubleSided	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER)
	SetDrawColor	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
	SetFillColor	function	(INSTANCE , TABLE|INTEGER , INTEGER , INTEGER)
	SetFont	function	(INSTANCE , STRING , STRING , FLOAT|INTEGER)
	SetFontSettings	function	(INSTANCE , TABLE)
	SetFontSize	function	(INSTANCE , FLOAT|INTEGER)
	SetKeywords	function	(INSTANCE , STRING)
	SetLeftMargin	function	(INSTANCE , FLOAT|INTEGER)
	SetLineWidth	function	(INSTANCE , FLOAT|INTEGER)
	SetLink	function	(INSTANCE , INTEGER , FLOAT|INTEGER , INTEGER)
	SetMargins	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
	SetRightMargin	function	(INSTANCE , FLOAT|INTEGER)
	SetSubject	function	(INSTANCE , STRING)
	SetTextColor	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
	SetTitle	function	(INSTANCE , STRING)
	SetTopMargin	function	(INSTANCE , FLOAT|INTEGER)
	SetX	function	(INSTANCE , FLOAT|INTEGER)
	SetXY	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER)
	SetY	function	(INSTANCE , FLOAT|INTEGER)
	Text	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING)
	TextShadow	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER , STRING , FLOAT|INTEGER)
	UnsetClipping	function	(INSTANCE)
	Write	function	(INSTANCE , FLOAT|INTEGER , STRING , INTEGER)
	constructor	function	(INSTANCE)
	getCustomZoom	function	(INSTANCE)
	reset	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
	setCustomZoom	function	(INSTANCE , INTEGER)
TinyXml2	table	
	COLLAPSE_WHITESPACE	integer	1
	PRESERVE_WHITESPACE	integer	0
	XMLAttribute	class	
		 [base class] -> 		NONE
		Name	function	(INSTANCE)
		Next	function	(INSTANCE)
		Value	function	(INSTANCE)
	XMLComment	class	
		 [base class] -> 		XMLNode
	XMLDeclaration	class	
		 [base class] -> 		XMLNode
	XMLDocument	class	
		 [base class] -> 		XMLNode
		DeleteNode	function	(INSTANCE , INSTANCE)
		Error	function	(INSTANCE)
		ErrorID	function	(INSTANCE)
		GetErrorStr1	function	(INSTANCE)
		GetErrorStr2	function	(INSTANCE)
		HasBOM	function	(INSTANCE)
		LoadFile	function	(INSTANCE , STRING)
		NewComment	function	(INSTANCE , STRING)
		NewDeclaration	function	(INSTANCE , STRING)
		NewElement	function	(INSTANCE , STRING)
		NewText	function	(INSTANCE , STRING)
		NewUnknown	function	(INSTANCE , STRING)
		Parse	function	(INSTANCE , STRING , INTEGER)
		ProcessEntities	function	(INSTANCE)
		RootElement	function	(INSTANCE)
		SaveFile	function	(INSTANCE , STRING , BOOL)
		SetBOM	function	(INSTANCE , BOOL)
		WhitespaceMode	function	(INSTANCE)
		constructor	function	(INSTANCE , BOOL , INTEGER)
	XMLElement	class	
		 [base class] -> 		XMLNode
		Attribute	function	(INSTANCE , STRING , STRING)
		DeleteAttribute	function	(INSTANCE , STRING)
		FindAttribute	function	(INSTANCE , STRING)
		FirstAttribute	function	(INSTANCE)
		GetText	function	(INSTANCE)
		Name	function	(INSTANCE , STRING)
		SetAttribute	function	(INSTANCE , STRING , STRING)
	XMLNode	class	
		 [base class] -> 		NONE
		DeleteChild	function	(INSTANCE , INSTANCE)
		DeleteChildren	function	(INSTANCE)
		FirstChild	function	(INSTANCE)
		FirstChildElement	function	(INSTANCE)
		GetDocument	function	(INSTANCE)
		InsertAfterChild	function	(INSTANCE , INSTANCE , INSTANCE)
		InsertEndChild	function	(INSTANCE , INSTANCE)
		InsertFirstChild	function	(INSTANCE , INSTANCE)
		LastChild	function	(INSTANCE)
		LastChildElement	function	(INSTANCE)
		LinkEndChild	function	(INSTANCE , INSTANCE)
		NextSibling	function	(INSTANCE)
		NextSiblingElement	function	(INSTANCE)
		NoChildren	function	(INSTANCE)
		Parent	function	(INSTANCE)
		PreviousSibling	function	(INSTANCE)
		PreviousSiblingElement	function	(INSTANCE)
		ShallowClone	function	(INSTANCE , INSTANCE)
		ShallowEqual	function	(INSTANCE , INSTANCE)
		ToComment	function	(INSTANCE)
		ToDeclaration	function	(INSTANCE)
		ToDocument	function	(INSTANCE)
		ToElement	function	(INSTANCE)
		ToText	function	(INSTANCE)
		ToUnknown	function	(INSTANCE)
		Value	function	(INSTANCE , STRING)
	XMLPrinter	class	
		 [base class] -> 		NONE
		CloseElement	function	(INSTANCE)
		OpenElement	function	(INSTANCE , STRING)
		PushAttribute	function	(INSTANCE , STRING , STRING)
		PushComment	function	(INSTANCE , STRING)
		PushDeclaration	function	(INSTANCE , STRING)
		PushHeader	function	(INSTANCE , BOOL , BOOL)
		PushText	function	(INSTANCE , STRING , BOOL)
		PushUnknown	function	(INSTANCE , STRING)
		constructor	function	(INSTANCE , BOOL)
	XMLText	class	
		 [base class] -> 		XMLNode
		CData	function	(INSTANCE , BOOL)
	XMLUnknown	class	
		 [base class] -> 		XMLNode
	XML_CAN_NOT_CONVERT_TEXT	integer	18
	XML_ERROR_ELEMENT_MISMATCH	integer	6
	XML_ERROR_EMPTY_DOCUMENT	integer	15
	XML_ERROR_FILE_COULD_NOT_BE_OPENED	integer	4
	XML_ERROR_FILE_NOT_FOUND	integer	3
	XML_ERROR_FILE_READ_ERROR	integer	5
	XML_ERROR_IDENTIFYING_TAG	integer	9
	XML_ERROR_MISMATCHED_ELEMENT	integer	16
	XML_ERROR_PARSING	integer	17
	XML_ERROR_PARSING_ATTRIBUTE	integer	8
	XML_ERROR_PARSING_CDATA	integer	11
	XML_ERROR_PARSING_COMMENT	integer	12
	XML_ERROR_PARSING_DECLARATION	integer	13
	XML_ERROR_PARSING_ELEMENT	integer	7
	XML_ERROR_PARSING_TEXT	integer	10
	XML_ERROR_PARSING_UNKNOWN	integer	14
	XML_NO_ATTRIBUTE	integer	1
	XML_NO_ERROR	integer	0
	XML_NO_TEXT_NODE	integer	19
	XML_SUCCESS	integer	0
	XML_WRONG_ATTRIBUTE_TYPE	integer	2
_charsize_	integer	1
_floatsize_	integer	8
_intsize_	integer	4
_version_	string	SquiLu based on Squirrel 3.0.4 stable and Lua 5.1.5
_versionnumber_	integer	304
array	function	(ANY , FLOAT|INTEGER)
array_delegate	table	
	append	function	(ARRAY)
	apply	function	(ARRAY , NATIVECLOSURE|CLOSURE)
	bsearch	function	(ARRAY , ANY)
	clear	function	(ANY)
	concat	function	(ARRAY , STRING)
	concat2	function	(ARRAY , STRING)
	empty	function	(ARRAY)
	extend	function	(ARRAY , ARRAY)
	filter	function	(ARRAY , NATIVECLOSURE|CLOSURE)
	find	function	(ARRAY , ANY)
	get	function	(ARRAY , INTEGER , ANY)
	getdelegate	function	(ANY)
	insert	function	(ARRAY , FLOAT|INTEGER)
	len	function	(ARRAY)
	map	function	(ARRAY , NATIVECLOSURE|CLOSURE)
	pop	function	(ARRAY)
	push	function	(ARRAY)
	reduce	function	(ARRAY , NATIVECLOSURE|CLOSURE)
	remove	function	(ARRAY , FLOAT|INTEGER)
	resize	function	(ARRAY , FLOAT|INTEGER)
	reverse	function	(ARRAY)
	size	function	(ARRAY)
	slice	function	(ARRAY , FLOAT|INTEGER , FLOAT|INTEGER)
	sort	function	(ARRAY , NATIVECLOSURE|CLOSURE)
	top	function	(ARRAY)
	tostring	function	(ANY)
	weakref	function	
assert	function	
axtls	table	
	SLL_ALERT_TYPE_FATAL	integer	2
	SSL_AES128_SHA	integer	47
	SSL_AES256_SHA	integer	53
	SSL_ALERT_BAD_CERTIFICATE	integer	42
	SSL_ALERT_BAD_RECORD_MAC	integer	20
	SSL_ALERT_CLOSE_NOTIFY	integer	0
	SSL_ALERT_DECODE_ERROR	integer	50
	SSL_ALERT_DECRYPT_ERROR	integer	51
	SSL_ALERT_HANDSHAKE_FAILURE	integer	40
	SSL_ALERT_ILLEGAL_PARAMETER	integer	47
	SSL_ALERT_INVALID_VERSION	integer	70
	SSL_ALERT_NO_RENEGOTIATION	integer	100
	SSL_ALERT_TYPE_WARNING	integer	1
	SSL_ALERT_UNEXPECTED_MESSAGE	integer	10
	SSL_BUILD_ENABLE_CLIENT	integer	4
	SSL_BUILD_ENABLE_VERIFICATION	integer	3
	SSL_BUILD_FULL_MODE	integer	5
	SSL_BUILD_MODE	integer	0
	SSL_BUILD_SERVER_ONLY	integer	2
	SSL_BUILD_SKELETON_MODE	integer	1
	SSL_CLIENT_AUTHENTICATION	integer	65536
	SSL_CLOSE_NOTIFY	integer	-3
	SSL_CONNECT_IN_PARTS	integer	8388608
	SSL_DEFAULT_CLNT_SESS	integer	1
	SSL_DEFAULT_SVR_SESS	integer	5
	SSL_DISPLAY_BYTES	integer	1048576
	SSL_DISPLAY_CERTS	integer	2097152
	SSL_DISPLAY_RSA	integer	4194304
	SSL_DISPLAY_STATES	integer	524288
	SSL_ERROR_BAD_CERTIFICATE	integer	-268
	SSL_ERROR_CONN_LOST	integer	-256
	SSL_ERROR_DEAD	integer	-2
	SSL_ERROR_FINISHED_INVALID	integer	-271
	SSL_ERROR_INVALID_HANDSHAKE	integer	-260
	SSL_ERROR_INVALID_HMAC	integer	-262
	SSL_ERROR_INVALID_KEY	integer	-269
	SSL_ERROR_INVALID_PROT_MSG	integer	-261
	SSL_ERROR_INVALID_SESSION	integer	-265
	SSL_ERROR_INVALID_VERSION	integer	-263
	SSL_ERROR_NOT_SUPPORTED	integer	-274
	SSL_ERROR_NO_CERT_DEFINED	integer	-272
	SSL_ERROR_NO_CIPHER	integer	-266
	SSL_ERROR_NO_CLIENT_RENOG	integer	-273
	SSL_ERROR_SOCK_SETUP_FAILURE	integer	-258
	SSL_HAS_PEM	integer	3
	SSL_MAX_CA_CERT_CFG_OFFSET	integer	2
	SSL_MAX_CERT_CFG_OFFSET	integer	1
	SSL_NOT_OK	integer	-1
	SSL_NO_DEFAULT_KEY	integer	262144
	SSL_OBJ_PKCS12	integer	5
	SSL_OBJ_PKCS8	integer	4
	SSL_OBJ_RSA_KEY	integer	3
	SSL_OBJ_X509_CACERT	integer	2
	SSL_OBJ_X509_CERT	integer	1
	SSL_OK	integer	0
	SSL_RC4_128_MD5	integer	4
	SSL_RC4_128_SHA	integer	5
	SSL_SERVER_VERIFY_LATER	integer	131072
	SSL_SESSION_ID_SIZE	integer	32
	SSL_X509_CA_CERT_COMMON_NAME	integer	3
	SSL_X509_CA_CERT_ORGANIZATION	integer	4
	SSL_X509_CA_CERT_ORGANIZATIONAL_NAME	integer	5
	SSL_X509_CERT_COMMON_NAME	integer	0
	SSL_X509_CERT_ORGANIZATION	integer	1
	SSL_X509_CERT_ORGANIZATIONAL_NAME	integer	2
	SSL_X509_OFFSET	integer	-512
	display_error	function	(ANY , INTEGER)
	get_config	function	(ANY , INTEGER)
	get_error	function	(ANY , INTEGER)
	ssl	class	
		 [base class] -> 		NONE
		free	function	(INSTANCE)
		get_cert_dn	function	(INSTANCE , INTEGER)
		get_cert_subject_alt_dnsname	function	(INSTANCE , INTEGER)
		get_cipher_id	function	(INSTANCE)
		get_session_id	function	(INSTANCE)
		get_session_id_size	function	(INSTANCE)
		handshake_status	function	(INSTANCE)
		read	function	(INSTANCE)
		renegotiate	function	(INSTANCE)
		verify_cert	function	(INSTANCE)
		write	function	(INSTANCE , STRING , INTEGER)
	ssl_ctx	class	
		 [base class] -> 		NONE
		client_new	function	(INSTANCE , INTEGER , STRING , INTEGER)
		constructor	function	(INSTANCE , INTEGER , INTEGER)
		find	function	(INSTANCE , STRING)
		free	function	(INSTANCE)
		obj_load	function	(INSTANCE , STRING)
		obj_memory_load	function	(INSTANCE , STRING)
		server_new	function	(INSTANCE , INTEGER)
	version	function	(ANY)
base64	table	
	decode	function	(ANY , STRING)
	encode	function	(ANY , STRING)
blob	class	
	 [base class] -> 		std_stream
	constructor	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER)
	memset	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
	reserve	function	(INSTANCE , FLOAT|INTEGER)
	resize	function	(INSTANCE , FLOAT|INTEGER)
	setLen	function	(INSTANCE , INTEGER)
	swap2	function	(INSTANCE)
	swap4	function	(INSTANCE)
callee	function	
castf2i	function	(ANY , FLOAT|INTEGER)
casti2f	function	(ANY , FLOAT|INTEGER)
class_delegate	table	
	get	function	(CLASS)
	getattributes	function	(CLASS , ANY)
	getbase	function	(CLASS)
	getdelegate	function	(ANY)
	instance	function	(CLASS)
	newmember	function	(CLASS)
	rawget	function	(CLASS)
	rawin	function	(CLASS)
	rawnewmember	function	(CLASS)
	rawset	function	(CLASS)
	setattributes	function	(CLASS , ANY , ANY)
	tostring	function	(ANY)
	weakref	function	
closure_delegate	table	
	acall	function	(NATIVECLOSURE|CLOSURE , ARRAY)
	acall2	function	(NATIVECLOSURE|CLOSURE , ANY , ARRAY)
	bindenv	function	(NATIVECLOSURE|CLOSURE , TABLE|CLASS|INSTANCE)
	call	function	(NATIVECLOSURE|CLOSURE)
	getdelegate	function	(ANY)
	getenv	function	(NATIVECLOSURE|CLOSURE)
	getinfos	function	(NATIVECLOSURE|CLOSURE)
	pacall	function	(NATIVECLOSURE|CLOSURE , ARRAY)
	pcall	function	(NATIVECLOSURE|CLOSURE)
	setenv	function	(NATIVECLOSURE|CLOSURE , TABLE|CLASS|INSTANCE)
	tostring	function	(ANY)
	weakref	function	
collectgarbage	function	
compilestring	function	(ANY , STRING , STRING)
debug_print	function	(ANY , STRING)
dofile	function	(ANY , STRING , BOOL)
dostring	function	(ANY , STRING , BOOL)
dummy	function	
dumpclosure	function	(ANY , STRING , NATIVECLOSURE|CLOSURE)
enabledebuginfo	function	
error	function	
file	class	
	 [base class] -> 		std_stream
	close	function	(INSTANCE)
	constructor	function	(INSTANCE)
fl_alert	function	(ANY , STRING)
fl_color	function	(ANY , INTEGER)
fl_color_average	function	(ANY , INTEGER , INTEGER , FLOAT)
fl_cursor	function	(ANY , INTEGER , INTEGER , INTEGER)
fl_descent	function	(ANY)
fl_draw	function	(ANY , FLOAT|STRING|INTEGER , FLOAT|STRING|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
fl_font	function	(ANY , INTEGER , INTEGER)
fl_height	function	(ANY , INTEGER , INTEGER)
fl_line	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
fl_line_style	function	(ANY , INTEGER , INTEGER , NULL|STRING)
fl_loop	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
fl_measure	function	(ANY , STRING , TABLE , BOOL)
fl_not_clipped	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
fl_point	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER)
fl_polygon	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
fl_pop_clip	function	(ANY)
fl_preferences	function	(ANY , INTEGER , STRING , STRING)
fl_push_clip	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER)
fl_push_no_clip	function	(ANY)
fl_rect	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , INTEGER)
fl_rectf	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , FLOAT|INTEGER , INTEGER)
fl_size	function	(ANY)
fl_width	function	(ANY , STRING , INTEGER)
format	function	(ANY , STRING)
generator_delegate	table	
	getdelegate	function	(ANY)
	getstatus	function	(GENERATOR)
	tostring	function	(ANY)
	weakref	function	
get_last_error	function	
get_last_stackinfo	function	
get_sle_size	function	(ANY , INTEGER)
getatexithandler	function	
getconsttable	function	
getdefaultdelegate	function	(ANY , ANY)
geterrorhandler	function	
getroottable	function	
getstackinfos	function	(ANY , FLOAT|INTEGER)
getstacktop	function	(ANY)
gettypetag	function	(ANY , ANY)
import	function	
instance_delegate	table	
	get	function	(INSTANCE)
	getclass	function	(INSTANCE)
	getdelegate	function	(ANY)
	rawget	function	(INSTANCE)
	rawin	function	(INSTANCE)
	rawset	function	(INSTANCE)
	tostring	function	(ANY)
	weakref	function	
loadfile	function	(ANY , STRING , BOOL)
loadstring	function	(ANY , STRING , BOOL)
markdown2html	function	(ANY , STRING , INTEGER)
math	table	
	PI	float	3.14159
	RAND_MAX	integer	2147483647
	abs	function	(ANY , FLOAT|INTEGER)
	acos	function	(ANY , FLOAT|INTEGER)
	asin	function	(ANY , FLOAT|INTEGER)
	atan	function	(ANY , FLOAT|INTEGER)
	atan2	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER)
	broundf	function	(ANY , FLOAT|INTEGER , INTEGER)
	ceil	function	(ANY , FLOAT|INTEGER)
	cos	function	(ANY , FLOAT|INTEGER)
	exp	function	(ANY , FLOAT|INTEGER)
	fabs	function	(ANY , FLOAT|INTEGER)
	floor	function	(ANY , FLOAT|INTEGER)
	log	function	(ANY , FLOAT|INTEGER)
	log10	function	(ANY , FLOAT|INTEGER)
	number_format	function	(ANY , FLOAT|INTEGER , INTEGER , STRING , STRING)
	number_format_get_dec_point	function	(ANY)
	number_format_get_thousand_sep	function	(ANY)
	number_format_set_dec_point	function	(ANY , STRING)
	number_format_set_thousand_sep	function	(ANY , STRING)
	pow	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER)
	rand	function	
	random	function	(ANY , INTEGER , INTEGER)
	roundf	function	(ANY , FLOAT|INTEGER , INTEGER)
	sin	function	(ANY , FLOAT|INTEGER)
	sqrt	function	(ANY , FLOAT|INTEGER)
	srand	function	(ANY , FLOAT|INTEGER)
	tan	function	(ANY , FLOAT|INTEGER)
md5	function	(ANY , STRING)
newthread	function	(ANY , NATIVECLOSURE|CLOSURE)
number_delegate	table	
	getdelegate	function	(ANY)
	tochar	function	(BOOL|FLOAT|INTEGER)
	tofloat	function	(BOOL|FLOAT|INTEGER)
	tointeger	function	(BOOL|FLOAT|INTEGER)
	tostring	function	(ANY)
	weakref	function	
os	table	
	clock	function	
	date	function	(ANY , STRING , FLOAT|INTEGER)
	difftime	function	(ANY , FLOAT|INTEGER , FLOAT|INTEGER)
	exit	function	(ANY , BOOL|INTEGER , BOOL)
	getenv	function	(ANY , STRING)
	getmillicount	function	(ANY)
	getmillispan	function	(ANY , INTEGER)
	remove	function	(ANY , STRING)
	rename	function	(ANY , STRING , STRING)
	setlocale	function	(ANY , STRING , STRING)
	sleep	function	(ANY , FLOAT|INTEGER)
	system	function	(ANY , STRING)
	time	function	(ANY , TABLE)
	tmpname	function	(ANY)
print	function	
print1	function	
printf	function	(ANY , STRING)
regexp	class	
	 [base class] -> 		NONE
	capture	function	(INSTANCE , STRING , FLOAT|INTEGER)
	constructor	function	(ANY , STRING)
	getxcapture	function	(INSTANCE , STRING , FLOAT|INTEGER , ARRAY)
	gmatch	function	(INSTANCE , STRING , NATIVECLOSURE|CLOSURE)
	gsub	function	(INSTANCE , STRING , TABLE|NATIVECLOSURE|ARRAY|CLOSURE)
	match	function	(INSTANCE , STRING)
	search	function	(INSTANCE , STRING , FLOAT|INTEGER)
	subexpcount	function	(INSTANCE)
	xcapture	function	(INSTANCE , STRING , FLOAT|INTEGER)
resurrectunreachable	function	
setatexithandler	function	(ANY , NATIVECLOSURE|CLOSURE)
setconsttable	function	
setdebughook	function	
seterrorhandler	function	(ANY , NATIVECLOSURE|CLOSURE)
setroottable	function	
showVars	function	
sle2vecOfvec	function	(ANY , STRING|INSTANCE , ARRAY , INTEGER)
socket	table	
	IO_CLOSED	integer	-2
	IO_DONE	integer	0
	IO_SSL	integer	-4
	IO_TIMEOUT	integer	-1
	IO_UNKNOWN	integer	-3
	gethostname	function	(ANY)
	gettime	function	(ANY)
	select	function	(ANY , ARRAY , ARRAY , FLOAT|INTEGER)
	sleep	function	(ANY , INTEGER)
	sleepMillisecs	function	(ANY , INTEGER)
	tcp	class	
		 [base class] -> 		NONE
		accept	function	(INSTANCE)
		bind	function	(INSTANCE , STRING , INTEGER)
		close	function	(INSTANCE)
		connect	function	(INSTANCE , STRING , INTEGER)
		constructor	function	(INSTANCE)
		dirty	function	(INSTANCE)
		getfd	function	(INSTANCE)
		getpeername	function	(INSTANCE)
		getsockname	function	(INSTANCE)
		getstats	function	(INSTANCE)
		listen	function	(INSTANCE , INTEGER)
		receive	function	(INSTANCE , STRING|INTEGER , STRING)
		send	function	(INSTANCE , STRING , INTEGER , INTEGER)
		setfd	function	(INSTANCE , INTEGER)
		setoption	function	(INSTANCE , STRING , BOOL|FLOAT|STRING|INTEGER)
		setstats	function	(INSTANCE , INTEGER , INTEGER , INTEGER)
		settimeout	function	(INSTANCE , FLOAT|INTEGER , STRING)
		shutdown	function	(INSTANCE , STRING)
	tohostname	function	(ANY , STRING)
	toip	function	(ANY , STRING)
	udp	class	
		 [base class] -> 		NONE
		close	function	(INSTANCE)
		constructor	function	(INSTANCE)
		dirty	function	(INSTANCE)
		getfd	function	(INSTANCE)
		getpeername	function	(INSTANCE)
		getsockname	function	(INSTANCE)
		receive	function	(INSTANCE , INTEGER)
		receivefrom	function	(INSTANCE , INTEGER)
		send	function	(INSTANCE , STRING)
		sendto	function	(INSTANCE , STRING , STRING , INTEGER)
		setfd	function	(INSTANCE , INTEGER)
		setoption	function	(INSTANCE , STRING , BOOL|FLOAT|STRING|INTEGER)
		setpeername	function	(INSTANCE , STRING , INTEGER)
		setsockname	function	(INSTANCE , STRING , INTEGER)
		settimeout	function	(INSTANCE , FLOAT|INTEGER)
sqfs	table	
	_COPYRIGHT	string	Copyright (C) Copyright (C) 2003-2012 Kepler Project, Domingo Alvarez Duarte
	_DESCRIPTION	string	LuaFileSystem is a Lua library developed to complement the set of functions related to file systems offered by the standard Lua distribution, adapted to Squirrel by Domingo Alvarez Duarte
	_VERSION	string	LuaFileSystem 1.61
	attributes	function	(ANY , STRING , STRING)
	chdir	function	(ANY , STRING)
	currentdir	function	(ANY)
	dir	class	
		 [base class] -> 		NONE
		close	function	(INSTANCE)
		constructor	function	(INSTANCE , STRING)
		currFileName	null	
	link	function	(ANY , STRING , STRING , BOOL)
	mkdir	function	(ANY , STRING)
	rmdir	function	(ANY , STRING)
	symlinkattributes	function	(ANY , STRING , STRING)
	touch	function	(ANY , STRING , INTEGER , INTEGER)
sqmix	class	
	 [base class] -> 		NONE
	loadfile	function	(ANY , STRING , NULL|STRING , NULL|STRING , NULL|STRING , STRING)
	loadstring	function	(ANY , STRING , NULL|STRING , NULL|STRING , NULL|STRING , STRING)
	parsefile	function	(ANY , STRING , NULL|STRING , NULL|STRING , NULL|STRING , STRING)
	parsestring	function	(ANY , STRING , NULL|STRING , NULL|STRING , NULL|STRING , STRING)
stderr	instance	
stdin	instance	
stdout	instance	
str_from_chars	function	(ANY , INTEGER)
stream	class	
	 [base class] -> 		NONE
	eos	function	(INSTANCE)
	flush	function	(INSTANCE)
	read	function	(INSTANCE , FLOAT|INTEGER)
	read_line	function	(INSTANCE)
	readblob	function	(INSTANCE , FLOAT|INTEGER)
	readn	function	(INSTANCE , FLOAT|INTEGER)
	seek	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER)
	tell	function	(INSTANCE)
	write	function	(INSTANCE , ANY)
	write_str	function	(INSTANCE , STRING , INTEGER , INTEGER)
	writeblob	function	(INSTANCE , INSTANCE)
	writen	function	(INSTANCE , FLOAT|INTEGER , FLOAT|INTEGER)
string_delegate	table	
	empty	function	(STRING)
	endswith	function	(STRING , STRING)
	find	function	(STRING , STRING , FLOAT|INTEGER)
	find_close_quote	function	(STRING , FLOAT|INTEGER)
	find_lua	function	(STRING , STRING , TABLE|NATIVECLOSURE|ARRAY|CLOSURE , FLOAT|INTEGER , BOOL)
	getdelegate	function	(ANY)
	gmatch	function	(STRING , STRING , NATIVECLOSURE|CLOSURE)
	gsub	function	(STRING , STRING , TABLE|NATIVECLOSURE|ARRAY|STRING|CLOSURE , FLOAT|INTEGER)
	len	function	(STRING)
	lstrip	function	(STRING)
	match	function	(STRING , STRING)
	rep	function	(STRING , INTEGER)
	replace	function	(STRING , STRING , STRING)
	reverse	function	(STRING , STRING)
	rstrip	function	(STRING)
	size	function	(STRING)
	sl_cmp_noaccents	function	(STRING , STRING)
	sl_deaccent	function	(STRING)
	sl_icmp	function	(STRING , STRING)
	sl_icmp_noaccents	function	(STRING , STRING)
	sl_len	function	(STRING)
	sl_like_cmp	function	(STRING , STRING , INTEGER)
	sl_like_cmp_noaccents	function	(STRING , STRING , INTEGER)
	sl_lower	function	(STRING)
	sl_lower_deaccent	function	(STRING)
	sl_upper	function	(STRING)
	slice	function	(STRING , FLOAT|INTEGER , FLOAT|INTEGER)
	split	function	(STRING , INTEGER)
	split_by_strtok	function	(STRING , STRING)
	startswith	function	(STRING , STRING)
	strip	function	(STRING)
	substr	function	(STRING , FLOAT|INTEGER , FLOAT|INTEGER)
	tofloat	function	(STRING)
	tointeger	function	(STRING , FLOAT|INTEGER)
	tolower	function	(STRING)
	tostring	function	(ANY)
	toupper	function	(STRING)
	weakref	function	
suspend	function	
swap2	function	(ANY , FLOAT|INTEGER)
swap4	function	(ANY , FLOAT|INTEGER)
swapfloat	function	(ANY , FLOAT|INTEGER)
table_delegate	table	
	clear	function	(ANY)
	get	function	(TABLE)
	getdelegate	function	(ANY)
	len	function	(TABLE)
	rawdelete	function	(TABLE)
	rawget	function	(TABLE)
	rawin	function	(TABLE)
	rawset	function	(TABLE)
	setdelegate	function	(ANY , TABLE|NULL)
	size	function	(TABLE)
	tostring	function	(ANY)
	weakref	function	
thread_delegate	table	
	call	function	(THREAD)
	getdelegate	function	(ANY)
	getstackinfos	function	(THREAD , FLOAT|INTEGER)
	getstatus	function	(THREAD)
	tostring	function	(ANY)
	wakeup	function	(THREAD)
	weakref	function	
type	function	
uri_decode	function	(ANY , STRING)
url_decode	function	(ANY , STRING)
url_encode	function	(ANY , STRING)
weakref_delegate	table	
	getdelegate	function	(ANY)
	ref	function	(WEAKREF)
	tostring	function	(ANY)
	weakref	function	
writeclosuretofile	function	(ANY , STRING , NATIVECLOSURE|CLOSURE)
zlib	table	
	Z_BEST_COMPRESSION	integer	9
	Z_BEST_SPEED	integer	1
	Z_DEFAULT_COMPRESSION	integer	-1
	deflate	function	(ANY , STRING , INTEGER)
	inflate	function	(ANY , STRING , INTEGER)
	version	function	(ANY)
zmq_ctx	class	
	 [base class] -> 		NONE
	IO_THREADS	integer	1
	IO_THREADS_DFLT	integer	1
	MAX_SOCKETS	integer	2
	MAX_SOCKETS_DFLT	integer	1024
	constructor	function	(INSTANCE)
	destroy	function	(INSTANCE)
	errno	function	(INSTANCE)
	set	function	(INSTANCE , INTEGER , INTEGER)
	socket	function	(INSTANCE , INTEGER)
	strerror	function	(INSTANCE , INTEGER)
	version	function	(INSTANCE)
zmq_msg	class	
	 [base class] -> 		NONE
	DONTWAIT	integer	1
	MORE	integer	1
	SNDMORE	integer	2
	close	function	(INSTANCE)
	constructor	function	(INSTANCE , STRING|INTEGER , INTEGER)
	copy	function	(INSTANCE , INSTANCE)
	data	function	(INSTANCE , STRING)
	more	function	(INSTANCE)
	recv	function	(INSTANCE , INSTANCE , INTEGER)
	send	function	(INSTANCE , INSTANCE , INTEGER)
	set	function	(INSTANCE , INTEGER , INTEGER)
zmq_socket	class	
	 [base class] -> 		NONE
	AFFINITY	integer	4
	BACKLOG	integer	19
	DEALER	integer	5
	DELAY_ATTACH_ON_CONNECT	integer	39
	EVENTS	integer	15
	EVENT_ACCEPTED	integer	32
	EVENT_ACCEPT_FAILED	integer	64
	EVENT_ALL	integer	1023
	EVENT_BIND_FAILED	integer	16
	EVENT_CLOSED	integer	128
	EVENT_CLOSE_FAILED	integer	256
	EVENT_CONNECTED	integer	1
	EVENT_CONNECT_DELAYED	integer	2
	EVENT_CONNECT_RETRIED	integer	4
	EVENT_DISCONNECTED	integer	512
	EVENT_LISTENING	integer	8
	FD	integer	14
	IDENTITY	integer	5
	IPV4ONLY	integer	31
	LAST_ENDPOINT	integer	32
	LINGER	integer	17
	MAXMSGSIZE	integer	22
	MULTICAST_HOPS	integer	25
	PAIR	integer	0
	PUB	integer	1
	PULL	integer	7
	PUSH	integer	8
	RATE	integer	8
	RCVBUF	integer	12
	RCVHWM	integer	24
	RCVMORE	integer	13
	RCVTIMEO	integer	27
	RECONNECT_IVL	integer	18
	RECONNECT_IVL_MAX	integer	21
	RECOVERY_IVL	integer	9
	REP	integer	4
	REQ	integer	3
	ROUTER	integer	6
	ROUTER_MANDATORY	integer	33
	SNDBUF	integer	11
	SNDHWM	integer	23
	SNDTIMEO	integer	28
	SUB	integer	2
	SUBSCRIBE	integer	6
	TCP_ACCEPT_FILTER	integer	38
	TCP_KEEPALIVE	integer	34
	TCP_KEEPALIVE_CNT	integer	35
	TCP_KEEPALIVE_IDLE	integer	36
	TCP_KEEPALIVE_INTVL	integer	37
	TYPE	integer	16
	UNSUBSCRIBE	integer	7
	XPUB	integer	9
	XPUB_VERBOSE	integer	40
	XSUB	integer	10
	bind	function	(INSTANCE , STRING)
	close	function	(INSTANCE)
	connect	function	(INSTANCE , STRING)
	constructor	function	(INSTANCE , INSTANCE , INTEGER)
	disconnect	function	(INSTANCE , STRING)
	getsockopt	function	(INSTANCE , INTEGER)
	recv	function	(INSTANCE , INTEGER)
	recvmsg	function	(INSTANCE , INSTANCE , INTEGER)
	send	function	(INSTANCE , STRING , INTEGER)
	sendmsg	function	(INSTANCE , INSTANCE , INTEGER)
	setsockopt	function	(INSTANCE , INTEGER , STRING|INTEGER)
	unbind	function	(INSTANCE , STRING)

<<<Constants>>>

FLVE_ALL_CLICKED	15
FLVE_CLICKED	7
FLVE_COLS_CHANGED	5
FLVE_COL_CHANGED	4
FLVE_COL_FOOTER_CLICKED	12
FLVE_COL_HEADER_CLICKED	11
FLVE_ENTER_PRESSED	8
FLVE_FEATURE_CHANGED	6
FLVE_LABEL_CLICKED	13
FLVE_ROWS_CHANGED	2
FLVE_ROW_CHANGED	1
FLVE_ROW_FOOTER_CLICKED	10
FLVE_ROW_HEADER_CLICKED	9
FLVE_SELECTION_CHANGED	3
FLVE_TITLE_CLICKED	14
FLVEcb_ALL_CLICKED	16384
FLVEcb_CLICKED	64
FLVEcb_COLS_CHANGED	16
FLVEcb_COL_CHANGED	8
FLVEcb_COL_FOOTER_CLICKED	2048
FLVEcb_COL_HEADER_CLICKED	1024
FLVEcb_ENTER_PRESSED	128
FLVEcb_FEATURE_CHANGED	32
FLVEcb_LABEL_CLICKED	4096
FLVEcb_ROWS_CHANGED	2
FLVEcb_ROW_CHANGED	1
FLVEcb_ROW_FOOTER_CLICKED	512
FLVEcb_ROW_HEADER_CLICKED	256
FLVEcb_SELECTION_CHANGED	4
FLVEcb_TITLE_CLICKED	8192
FLVF_ALL_COL	56
FLVF_ALL_ROW	7
FLVF_COL_DIVIDER	32
FLVF_COL_ENDS	24
FLVF_COL_FOOTER	16
FLVF_COL_HEADER	8
FLVF_DIVIDERS	36
FLVF_FOOTERS	18
FLVF_FULL_RESIZE	512
FLVF_HEADERS	9
FLVF_MULTI_SELECT	64
FLVF_NONE	0
FLVF_PERSIST_SELECT	256
FLVF_ROW_DIVIDER	4
FLVF_ROW_ENDS	3
FLVF_ROW_FOOTER	2
FLVF_ROW_HEADER	1
FLVF_ROW_SELECT	128
FLVS_ALWAYS	4
FLVS_AUTOMATIC	0
FLVS_BOTH	3
FLVS_BOTH_ALWAYS	7
FLVS_HORIZONTAL	1
FLVS_HORIZONTAL_ALWAYS	5
FLVS_NONE	0
FLVS_VERTICAL	2
FLVS_VERTICAL_ALWAYS	6
FLV_EDIT_ALWAYS	1
FLV_EDIT_AUTOMATIC	2
FLV_EDIT_MANUAL	3
FL_ACTIVATE	14
FL_ALIGN_BOTTOM	2
FL_ALIGN_CENTER	0
FL_ALIGN_CLIP	64
FL_ALIGN_INSIDE	16
FL_ALIGN_LEFT	4
FL_ALIGN_RIGHT	8
FL_ALIGN_TOP	1
FL_ALIGN_TOP_LEFT	5
FL_ALIGN_TOP_RIGHT	9
FL_ALIGN_WRAP	128
FL_ALT	524288
FL_Alt_L	65513
FL_Alt_R	65514
FL_BACKGROUND2_COLOR	7
FL_BACKGROUND_COLOR	49
FL_BLACK	56
FL_BLUE	216
FL_BOLD	1
FL_BORDER_BOX	14
FL_BORDER_FRAME	16
FL_BUTTON1	16777216
FL_BUTTON2	33554432
FL_BUTTON3	67108864
FL_BUTTONS	2130706432
FL_BackSpace	65288
FL_Button	65256
FL_CAPS_LOCK	131072
FL_CAP_FLAT	256
FL_CAP_ROUND	512
FL_CAP_SQUARE	768
FL_CLOSE	10
FL_COMMAND	262144
FL_COURIER	4
FL_COURIER_BOLD	5
FL_COURIER_BOLD_ITALIC	7
FL_COURIER_ITALIC	6
FL_CTRL	262144
FL_CURSOR_DEFAULT	0
FL_CURSOR_WAIT	76
FL_CYAN	223
FL_Caps_Lock	65509
FL_Control_L	65507
FL_Control_R	65508
FL_DARK1	47
FL_DARK2	45
FL_DARK3	39
FL_DARK_BLUE	136
FL_DARK_CYAN	140
FL_DARK_GREEN	60
FL_DARK_MAGENTA	152
FL_DARK_RED	72
FL_DARK_YELLOW	76
FL_DASH	1
FL_DASHDOT	3
FL_DASHDOTDOT	4
FL_DEACTIVATE	13
FL_DND_DRAG	21
FL_DND_ENTER	20
FL_DND_LEAVE	22
FL_DND_RELEASE	23
FL_DOT	2
FL_DOUBLE	2
FL_DOWN_BOX	3
FL_DOWN_FRAME	5
FL_DRAG	5
FL_Delete	65535
FL_Down	65364
FL_ENGRAVED_BOX	10
FL_ENGRAVED_FRAME	12
FL_ENTER	3
FL_Eisu	65327
FL_End	65367
FL_Enter	65293
FL_Escape	65307
FL_F	65469
FL_FLAT_BOX	1
FL_FOCUS	6
FL_FOREGROUND_COLOR	0
FL_FULLSCREEN	25
FL_F_Last	65504
FL_GRAY	49
FL_GREEN	63
FL_HELVETICA	0
FL_HELVETICA_BOLD	1
FL_HELVETICA_BOLD_ITALIC	3
FL_HELVETICA_ITALIC	2
FL_HIDE	15
FL_Help	65384
FL_Home	65360
FL_INACTIVE_COLOR	8
FL_INDEX	1
FL_ITALIC	2
FL_Insert	65379
FL_Iso_Key	65292
FL_JIS_Underscore	65329
FL_JOIN_BEVEL	12288
FL_JOIN_MITER	4096
FL_JOIN_ROUND	8192
FL_KEYBOARD	8
FL_KEYDOWN	8
FL_KEYUP	9
FL_KEY_MASK	65535
FL_KP	65408
FL_KP_0	65456
FL_KP_1	65457
FL_KP_2	65458
FL_KP_3	65459
FL_KP_4	65460
FL_KP_5	65461
FL_KP_6	65462
FL_KP_7	65463
FL_KP_8	65464
FL_KP_9	65465
FL_KP_Division	65455
FL_KP_Dot	65454
FL_KP_Enter	65421
FL_KP_Last	65469
FL_KP_Minus	65453
FL_KP_Plus	65451
FL_KP_Times	65450
FL_Kana	65326
FL_Key_0	48
FL_Key_9	57
FL_Key_Minus	45
FL_Key_Plus	61
FL_Key_Space	32
FL_LEAVE	4
FL_LEFT_MOUSE	1
FL_Left	65361
FL_MAGENTA	248
FL_MENU_DIVIDER	128
FL_META	4194304
FL_MIDDLE_MOUSE	2
FL_MOUSEWHEEL	19
FL_MOVE	11
FL_Menu	65383
FL_Meta_L	65511
FL_Meta_R	65512
FL_NORMAL_LABEL	0
FL_NO_BOX	0
FL_NO_EVENT	0
FL_NO_LABEL	1
FL_NUM_LOCK	1048576
FL_Num_Lock	65407
FL_PASTE	17
FL_PUSH	1
FL_Page_Down	65366
FL_Page_Up	65365
FL_Pause	65299
FL_Print	65377
FL_RADIO_BUTTON	102
FL_RED	88
FL_RELEASE	2
FL_RGB	0
FL_RGB8	64
FL_RIGHT_MOUSE	3
FL_Right	65363
FL_SCREEN_CONFIGURATION_CHANGED	24
FL_SCROLL_LOCK	8388608
FL_SELECTIONCLEAR	18
FL_SELECTION_COLOR	15
FL_SHADOW_LABEL	2
FL_SHIFT	65536
FL_SHORTCUT	12
FL_SHOW	16
FL_SOLID	0
FL_SUBMENU	64
FL_Scroll_Lock	65300
FL_Shift_L	65505
FL_Shift_R	65506
FL_TIMES	8
FL_TIMES_BOLD	9
FL_TIMES_BOLD_ITALIC	11
FL_TIMES_ITALIC	10
FL_Tab	65289
FL_UNFOCUS	7
FL_UP_BOX	2
FL_Up	65362
FL_WHEN_CHANGED	1
FL_WHEN_ENTER_KEY	8
FL_WHEN_ENTER_KEY_ALWAYS	10
FL_WHEN_ENTER_KEY_CHANGED	11
FL_WHEN_NEVER	0
FL_WHEN_NOT_CHANGED	2
FL_WHEN_RELEASE	4
FL_WHEN_RELEASE_ALWAYS	6
FL_WHITE	255
FL_YELLOW	95
FL_Yen	65328
PAGE_FORMAT_A0	0
PAGE_FORMAT_A1	1
PAGE_FORMAT_A2	2
PAGE_FORMAT_A3	3
PAGE_FORMAT_A4	4
PAGE_FORMAT_A5	5
PAGE_FORMAT_A6	6
PAGE_FORMAT_A7	7
PAGE_FORMAT_A8	8
PAGE_FORMAT_A9	9
PAGE_FORMAT_B0	10
PAGE_FORMAT_B1	11
PAGE_FORMAT_B10	20
PAGE_FORMAT_B2	12
PAGE_FORMAT_B3	13
PAGE_FORMAT_B4	14
PAGE_FORMAT_B5	15
PAGE_FORMAT_B6	16
PAGE_FORMAT_B7	17
PAGE_FORMAT_B8	18
PAGE_FORMAT_B9	19
PAGE_FORMAT_C5E	21
PAGE_FORMAT_DLE	22
PAGE_FORMAT_ENVELOPE	29
PAGE_FORMAT_EXECUTIVE	23
PAGE_FORMAT_FOLIO	24
PAGE_FORMAT_LEDGER	25
PAGE_FORMAT_LEGAL	26
PAGE_FORMAT_LETTER	27
PAGE_FORMAT_MEDIA	4096
PAGE_FORMAT_TABLOID	28
PAGE_LAYOUT_LANDSCAPE	256
PAGE_LAYOUT_ORIENTATION	768
PAGE_LAYOUT_PORTRAIT	0
PAGE_LAYOUT_REVERSED	512

```