__WIN32__ <- os.getenv("WINDIR") != null

//local port_name = __WIN32__ ?  "COM1" : "/dev/ttyUSB0";
//local port_name = __WIN32__ ?  "COM1" : "/dev/ttyS0";
port_name <- __WIN32__ ?  "COM1" : "/dev/ttyACM0";
platform <- __WIN32__ ? "win32" : "linux";

function time_stamp(){
	local t = os.date("*t");
	return format("[%02d.%02d.%d %02d:%02d:%02d] ",
			     t.day, t.month, t.year,
			     t.hour, t.min, t.sec);
}

local oldprint = print;

function myprint(text){
	text = time_stamp() + text;
	oldprint(text);
	local fd = file("test_sqrs232_log.txt", "a+");
	fd.write(text + "\n");
	fd.close();
}

print = myprint;

/*
local string_delegate = port_name.getdelegate();
string_delegate.lines <- function(str){
	return str.find_lua("([^\n]*)\n");
}
*/

function format_test(text, err){
	local ret = "\n";
	local line_number = 1;
	local si = get_last_stackinfo();
	local line_err = si.line;
	
	foreach( line in text.split('\n')) {
		if (line_number == line_err.tointeger())
			ret += format("-->   %d. %s (%s)\n", line_number, line, err);
		else
			ret += format("% 8d. %s\n", line_number, line);
		++line_number;
	}
	return ret;
}

function test(text){
	local chunk = compilestring(text);
	if (chunk == null){
		print(format([==[
--------------------------------------------------------------- 
%s
loadstring error: '%s'
--------------------------------------------------------------- 
]==], format_test(text, err), err))
		return false;
	}

	try {
		chunk();
	}
	catch(e){
		print(format([==[
--------------------------------------------------------------- 
%s
call error: '%s'
--------------------------------------------------------------- 
]==], format_test(text, e), e));
		//return false;
	}

	return true;
}

local baud_rates = [
	"SqRs232.RS232_BAUD_9600",
	"SqRs232.RS232_BAUD_19200",
	"SqRs232.RS232_BAUD_38400",
	"SqRs232.RS232_BAUD_57600",
	"SqRs232.RS232_BAUD_115200",
];

// It's such a crap, Windows seems to not like 5 data
// bits. If you make this test to work with 5 data bits on
// Windows, I'll buy you a beer or two...
local data_bits;

if (platform == "win32"){
	data_bits = [
		"SqRs232.RS232_DATA_6",
		"SqRs232.RS232_DATA_7",
		"SqRs232.RS232_DATA_8",
	];
}
else
{
	data_bits = [
		"SqRs232.RS232_DATA_5",
		"SqRs232.RS232_DATA_6",
		"SqRs232.RS232_DATA_7",
		"SqRs232.RS232_DATA_8",
	];
}

local stop_bits = [
	"SqRs232.RS232_STOP_1",
	"SqRs232.RS232_STOP_2",
];

local parity_bits = [
	"SqRs232.RS232_PARITY_NONE",
	"SqRs232.RS232_PARITY_ODD",
	"SqRs232.RS232_PARITY_EVEN",
];

local flow_bits = [
	"SqRs232.RS232_FLOW_OFF",
	"SqRs232.RS232_FLOW_HW",
	"SqRs232.RS232_FLOW_XON_XOFF",
];

local dtr_bits = [
	"SqRs232.RS232_DTR_ON",
	"SqRs232.RS232_DTR_OFF",
];

local rts_bits = [
	"SqRs232.RS232_RTS_ON",
	"SqRs232.RS232_RTS_OFF",
];

local errors = [
	"SqRs232.RS232_ERR_NOERROR",
	"SqRs232.RS232_ERR_UNKNOWN",
	"SqRs232.RS232_ERR_OPEN",
	"SqRs232.RS232_ERR_CLOSE",
	"SqRs232.RS232_ERR_FLUSH",
	"SqRs232.RS232_ERR_CONFIG",
	"SqRs232.RS232_ERR_READ",
	"SqRs232.RS232_ERR_WRITE",
	"SqRs232.RS232_ERR_SELECT",
	"SqRs232.RS232_ERR_TIMEOUT",
	"SqRs232.RS232_ERR_IOCTL",
	"SqRs232.RS232_ERR_PORT_CLOSED",
];

assert(
	test(format([==[
		local p = SqRs232();
		p.open("%s");
		assert(p.close() == SqRs232.RS232_ERR_NOERROR);
	]==], port_name))
);

assert(
	test([==[
		local p = SqRs232();
		p.open("/dev/hell");
	]==])
);

foreach(baud in baud_rates) {
 foreach(data in data_bits) { 
  foreach(parity in parity_bits) {
   foreach(stop in stop_bits) {
    foreach(flow in flow_bits) {
     foreach(dtr in dtr_bits) {
      foreach(rts in rts_bits) {
	assert(
		test(
			format([==[
				local p = SqRs232();
				p.open("%s");

				local bret = p.strerror(p.baud_rate(%s));
				local dret = p.strerror(p.data_bits(%s));
				local pret = p.strerror(p.parity(%s));
				local sret = p.strerror(p.stop_bits(%s));
				local fret = p.strerror(p.flow_control(%s));
				local dtr_ret = p.strerror(p.dtr(%s));
				local rts_ret = p.strerror(p.rts(%s));
				 
				local text = p.tostring();
				assert(text != null);
				 
				local errors = "";
				if (bret != "no error") errors = "set baud rate,";
				if (dret != "no error") errors += "set data bits,";
				if (pret != "no error") errors += "set parity,";
				if (sret != "no error") errors += "set stop bits,";
				if (fret != "no error") errors += "set flow control";
				if (dtr_ret != "no error") errors += "set dtr";
				if (rts_ret != "no error") errors += "set rts";
				 
				if (errors.len() > 0) print(format(" [!] ERROR: %%s (failed: %%s)", text, errors));
				else print(format(" [!] OK: %%s", text));
				 
				assert(p.close() == SqRs232.RS232_ERR_NOERROR);
				]==], port_name, baud, data, parity, stop, flow, dtr, rts)
		)
	);
      }
     }
    }
   }
  }
 }
}

foreach(baud in baud_rates) {
 foreach(data in data_bits) { 
  foreach(parity in parity_bits) {
   foreach(stop in stop_bits) {
    foreach(flow in flow_bits) {
     foreach(dtr in dtr_bits) {
      foreach(rts in rts_bits) {
	assert(
		test(
			format([==[
				local timeout = 10;
				local p = SqRs232();
				p.open("%s");
				 
				assert(p.baud_rate(%s) == SqRs232.RS232_ERR_NOERROR);
				assert(p.data_bits(%s) == SqRs232.RS232_ERR_NOERROR);
				assert(p.parity(%s) == SqRs232.RS232_ERR_NOERROR);
				assert(p.stop_bits(%s) == SqRs232.RS232_ERR_NOERROR);
				assert(p.flow_control(%s) == SqRs232.RS232_ERR_NOERROR);
				assert(p.dtr(%s) == SqRs232.RS232_ERR_NOERROR);
				assert(p.rts(%s) == SqRs232.RS232_ERR_NOERROR);
				 
				local baud = p.baud_rate();
				local str_baud1 = p.baud_rate_tostring();
				local str_baud2 = p.baud_rate_tostring(baud);
				assert(str_baud1 == str_baud2);
				 
				local bits = p.data_bits();
				local str_bits1 = p.data_bits_tostring();
				local str_bits2 = p.data_bits_tostring(bits);
				assert(str_bits1 == str_bits2);
				 
				local parity = p.parity();
				local str_parity1 = p.parity_tostring();
				local str_parity2 = p.parity_tostring(parity);
				assert(str_parity1 == str_parity2);
				 
				local stop = p.stop_bits();
				local str_stop1 = p.stop_bits_tostring();
				local str_stop2 = p.stop_bits_tostring(stop);
				assert(str_stop1 == str_stop2);
				 
				local flow = p.flow_control();
				local str_flow1 = p.flow_control_tostring();
				local str_flow2 = p.flow_control_tostring(flow);
				assert(str_flow1 == str_flow2);

				local dtr = p.dtr();
				local str_dtr1 = p.dtr_tostring();
				local str_dtr2 = p.dtr_tostring(dtr);
				assert(str_dtr1 == str_dtr2);

				local rts = p.rts();
				local str_rts1 = p.rts_tostring();
				local str_rts2 = p.rts_tostring(rts);
				assert(str_rts1 == str_rts2);
				 
				local e = p.flush();
				assert(e == SqRs232.RS232_ERR_NOERROR);
				 
				local data = p.read(1, timeout);
				if(type(data) == "integer") assert(data == SqRs232.RS232_ERR_TIMEOUT);
				//assert(d == nil);
				//assert(l == 0);
				 
				// not implemented yet...
				if (platform != "win32"){
					local forced = true;
					data = p.read(1, timeout, forced)
					if(type(data) == "integer") assert(data == SqRs232.RS232_ERR_TIMEOUT)
					//assert(d == nil)
					//assert(l == 0)
				}
				 
				local forced = false;
				data = p.read(1, timeout, forced);
				if(type(data) == "integer") assert(data == SqRs232.RS232_ERR_TIMEOUT)
				//assert(d == nil)
				//assert(l == 0)
				 
				e = p.write("ynezz")
				if(e < 0) assert(e == SqRs232.RS232_ERR_NOERROR)
				 
				// althought the write is successful it returns 0 bytes written
				// in some baud/data/stop/flow combinations...
				 
				if (platform != "win32") assert(e == 5);
				 
				e = p.write("ynezz", timeout)
				if(e < 0) assert(e == SqRs232.RS232_ERR_NOERROR);
				 
				// althought the write is successful it returns 0 bytes written
				// in some baud/data/stop/flow combinations...
				 
				if (platform != "win32") assert(e == 5);
				 
				local text = p.tostring();
				assert(text != null)
				print("tostring(p): " + text);
				 
				assert(p.close() == SqRs232.RS232_ERR_NOERROR);
				]==], port_name, baud, data, parity, stop, flow, dtr, rts)
		)
	);
      }
     }
    }
   }
  }
 }
}

print("[*] All tests passed succesfuly!");
