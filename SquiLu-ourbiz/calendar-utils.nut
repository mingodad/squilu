/*
This code is based on https://github.com/leafo/lua-date

Copyright (C) 2006, by Jas Latrix (jastejada@yahoo.com)
All Rights Deserved.
use this code at your own risk!.
keep out of reach of children.

Ported and adapted to Squirrel/SquiLu by Domingo Alvarez Duarte
*/

class CalendarBase {
	static _month_names  = [
		"January", "February", "March", "April", "May", "June", "July",
		"August", "September", "October", "November", "December"
		];
	/*
	static _month_names  = [
		"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio",
		"Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
		];
	*/
	static _month_names_abbr = [
			"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			];
	/*
	static _month_names_abbr = [
			"Ene", "Feb", "Mar", "Abr", "May", "Jun",
			"Jul", "Ago", "Sep", "Oct", "Nov", "Dic"
			];
	*/
	static _day_names = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
	//static _day_names = ["Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"];

	//static _day_names_abbr = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
	static _day_names_abbr = ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"];
	//static _day_names_abbr = ["Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"];
	//static _day_names_abbr = ["Do", "Lu", "Ma", "Mi", "Ju", "Vi", "Sa"];
	
	static _month_names_abbr = [
			"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			];
	/*
	static _month_names_abbr = [
			"Ene", "Feb", "Mar", "Abr", "May", "Jun",
			"Jul", "Ago", "Sep", "Oct", "Nov", "Dic"
			];
	*/
	static _days_in_month = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 ];
	static _seconds_in_hour = 3600;
	static _seconds_in_day = 86400;
	static _seconds_in_week = 604800;
	
	constructor(){}

	function parse_hour_minutes(s){
		local ar_t = [];
		local function set_matches(m){ar_t.push(v.tointeger()); return true;};
		set_matches.setenv(this);
		s.gmatch("(%d+)", set_matches);
		return {
			hour = ar_t.get(0, 0),
			minutes = ar_t.get(1, 0),
			seconds = ar_t.get(2, 0),
			};
	}

	function parse_day_month_year(s){
		local ar_t = [];
		local function set_matches(m){ar_t.push(v.tointeger()); return true;};
		set_matches.setenv(this);
		s.gmatch("(%d+)", set_matches);
		local day = ar_t.get(0, 0);
		local month = ar_t.get(1, 0);
		local year = ar_t.get(2, 0);
		if(day > 31){
			local tmp = day;
			day = year;
			year = tmp;
		}
		return {day = day,	month = month, year = year,};
	}

	function parse_year_month_day(s){
		local ar_t = [];
		local function set_matches(m){ar_t.push(v.tointeger()); return true;};
		set_matches.setenv(this);
		s.gmatch("(%d+)", set_matches);
		local year = ar_t.get(0, 0);
		local month = ar_t.get(1, 0);
		local day = ar_t.get(2, 0);
		if(day > 31){
			local tmp = day;
			day = year;
			year = tmp;
		}
		return {day = day,	month = month, year = year,};
	}

	function parse_date( value ){
		local adate = os.date("*t");
		if (value.len() > 0){
			local nums = [];
			
			local  function set_matches(m) {nums.push(m.tointeger()); return true};
			set_matches.setenv(this);

			local setDay = function(idx){
				local aday = nums[idx];
				if (aday > 0 && aday < 32) adate.day = aday;
			}
			local setMonth = function(idx){
				local amonth = nums[idx];
				if (amonth > 0 && amonth < 13) adate.month = amonth-1; //zero based
			}
			local setYear = function(idx){
				local ayear = nums[idx];
				if (ayear < 100) ayear += 2000; //current milenium
				adate.year = ayear;
			}
			local setHour = function(){
				local ahour = nums[0];
				if (ahour > 0 && ahour < 23) adate.hour = ahour;
			}
			local setMin = function(){
				local amin = nums[1];
				if (amin > 0 && amin < 60) adate.min = amin;
			}
			local setSec = function(){
				local asec = nums[2];
				if (asec > 0 && asec < 60) adate.sec = asec;
			}
			//print(value);
			local avalue = value.split(' ');
			avalue[0].gmatch("(%d+)", set_matches);
			switch(nums.len()){
				case 1: //day in current month
					setDay(0);
				break;
				case 2: //day and month in current year
					if (nums[1] > 12) {
						setDay(1);
						setMonth(0);
					}
					else
					{
						setDay(0);
						setMonth(1);
					}
				break;
				case 3:
					if (nums[1] > 31){
						//year and month and day
						setYear(0);
						setMonth(1);
						setDay(2);
					}
					else
					{
						//day and month and year
						if (nums[2] > 12){
							setDay(1);
							setMonth(0);
						}
						else
						{
							setDay(0);
							setMonth(1);
						}
						setYear(2);
					}
				break;
			}
			
			if (avalue.len() > 1){
				nums = [];
				avalue[1].gmatch("(%d+)",set_matches);
				//print(nums.len(), avalue[1]);
				
				adate.hour = 0;
				adate.min = 0;
				adate.sec = 0;
				local nl = nums.len();
				if (nl > 0) setHour();
				if (nl > 1) setMin();
				if (nl > 2) setSec();
			}
		}
		//foreach( k,v in adate) do print(k,v);
		//print(os.date(os.time(adate)))
		return adate;
	}


	function julian_to_unix(J){
		// converts a julian date into unit time
		return (J - 2440588) * 86400;
	}

	function get_julian_day(y,m,d){
		// returns the current time in julian date format
		local now = os.time({year=y,month=m,day=d});
		return now / 86400.0 + 2440588;
	}

	function get_julian_now(){
		// returns the current time in julian date format
		local now = os.time();
		return now / 86400.0 + 2440588;
	}

	function get_timezone_offset(){
		// returns the number of seconds of timezone offset
		local tz = os.date("%z").tointeger();
		local tzh = math.floor(tz / 100 + 0.5);
		local tzm = math.abs(tz) % 100 / 60.0;
		if (tzh < 0) tzm *= -1;
		return (tzh + tzm) * 3600;
	}

	function makemonth(y,m){
		local month_array = [];
 		local atime = os.time({year=y,month=m,day=1});
		local adate = os.date("*t", atime);
		// get back to the nearest monday
		atime -= _seconds_in_day * adate.wday;
		adate = os.date("*t", atime);
		if (adate.day < 7 && adate.day != 1){
			//go back one more week
			atime -=  _seconds_in_week;
			adate = os.date("*t", atime);
		}
		do
		{
			local week_array = [];
			month_array.push(week_array);
			do
			{
				// insert the week days
				week_array.push(adate.day);
				atime += _seconds_in_day;
				adate = os.date("*t", atime);
			} while(adate && adate.wday != 0); // sunday
		} while(adate && adate.month == m);
		return month_array;
	}
}

/*
local html_foot = "\n</body>\n</html>\n";
local html_head = [=[
<html>
<style>
th {background:black; color: silver; vertical-align: middle;}
td {vertical-align: middle; text-align:center;}
td.sun {color: red;}
td.sat {color: blue;}
</style>
<body>
]=];
local html_yearhead = "\n<table align=\"left\">";
local html_monhead = "\n<tr><th colspan = \"7\">%s, %s</th></tr><tr><td>sun</td><td>mon</td><td>tue</td><td>wed</td><td>thu</td><td>fri</td><td>sat</td></tr>";
local html_monweek = "\n<tr><td class=\"sun\">%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td class=\"sat\">%d</td></tr>";
local html_yearfoot = "\n</table>";
function makecalendar(y, iox){
	iox.write(html_yearhead);
	for(local i = 0; i < 12; ++i){
		local tm = CalendarBase.makemonth(y, i);
		iox.write(format(html_monhead, CalendarBase._month_names[i], y.tostring()));
		foreach( k, v in tm) iox.write(format(html_monweek, v[0], v[1], v[2], v[3], v[4], v[5], v[6]));
	}
	iox.write(html_yearfoot);
}

local outf = file("year-calendar.html", "w");
outf.write(html_head);

foreach(v in [2012,2013,2014] ){
	local y = v.tointeger();
	if (y) makecalendar(y, outf);
}

outf.write(html_foot);
*/