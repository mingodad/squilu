function fromCSV (s){
  local t = [];        // table to collect fields
  local fieldstart = 0;
  local slen = s.len();
  //local count = 0;
  do {
    //print("Count", ++count);
    // next field is quoted? (start with `"'?)
    if (s[fieldstart] == '"') {
      local i = s.find_close_quote(fieldstart+1);
      if (i<0) throw("unmatched \"");
      local f = s.slice(fieldstart+1, i);
//print(f, f.replace("\"\"", "\""));
      t.push(f.replace("\"\"", "\""));
       local nextc = s.find(",", i);
       if(!nextc) nextc=slen-1;
      fieldstart = nextc + 1;
    }  
    else                // unquoted; find next comma
    {
      local nexti = s.find(",", fieldstart);
      if(!nexti) nexti = slen-1;
//print("nn", fieldstart, nexti, s.slice(fieldstart, nexti))
      t.push(s.slice(fieldstart, nexti));
      fieldstart = nexti + 1;
    }
  } while(fieldstart < slen);
  if(s[slen-1] == ',') t.push("");
  return t;
}
