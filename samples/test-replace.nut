local str = "a day in life";

print(str.replace("day", "night"));

function my_find_close_quote(src, init=0){
	local src_size = str.len();
	if(init >= src_size) throw "invalid start position";
	for(; init < src_size; ++init) {
		if(src[init] == '"'){
			if(init < (src_size-1) && src[init+1] == '"') ++init; //skip quoted quote
			else break;
		}
	}
	if(src[init] != '"') init = -1;
	return init;
}

// Convert from CSV string to table
function fromCSV (s){
  local t = [];        // table to collect fields
  local fieldstart = 0;
  local slen = s.len();
  do {
    // next field is quoted? (start with `"'?)
    if (s[fieldstart] == '"') {
      local i = s.find_close_quote(fieldstart+1);
      //local i = my_find_close_quote(s, fieldstart+1);
      if (i<0) throw("unmatched \"");
      local f = s.slice(fieldstart+1, i);
//print(f);
      t.push(f.replace("\"\"", "\""));
       local nextc = s.find(",", i);
       if(!nextc) nextc=slen-1;
      fieldstart = nextc + 1;
    }  
    else                // unquoted; find next comma
    {
      local nexti = s.find(",", fieldstart);
      if(!nexti) nexti = slen-1;
//print("nn", s.slice(fieldstart, nexti))
      t.push(s.slice(fieldstart, nexti));
      fieldstart = nexti + 1;
    }
  } while(fieldstart < slen);
  return t;
}

str = "12,345.9,leon,,234.56,9,32,44444444444444444444444444444444444444444444444444444444444444444444444444444444444444,\"Domingo\"\",,\"tais\",leon,,234.56,9,32,44,\"laura\"";
//str = [=[
//"""ARTHUR BALFOUR"",CONSERVATIVE WORKING MEN'S CLUB LIMITED","IP10067R","","","","","","","","","Industrial and Provident Society","Active","United Kingdom","","01/01/1981","","","","","NO ACCOUNTS FILED","","","0","0","0","0","None Supplied","","","","0","0","http://business.data.gov.uk/id/company/IP10067R","","","","","","","","","","","","","","","","","","","",""]=];
local ar2 =  fromCSV(str);
print(ar2.len());
foreach(i, s in ar2) print(i,s);

local ar
local now = os.clock();
for(local i=0; i< 100000; ++i){
	ar = fromCSV(str);
}
print("Spent", os.clock()-now)
print(ar.len());
foreach( i, s in ar) print(s);

local pos;
now = os.clock();
for(local i=0; i< 100000; ++i){
	pos = str.find_close_quote();
}
print("Spent native", os.clock()-now)

now = os.clock();
for(local i=0; i< 100000; ++i){
	pos = my_find_close_quote(str);
}
print("Spent interpreted", os.clock()-now)

os.exit();