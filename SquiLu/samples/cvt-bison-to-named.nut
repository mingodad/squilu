__max_print_stack_str_size <- 100;

auto fname;
if(vargv.len() > 1) fname = vargv[1];
//else fname = "/home/mingo/dev/dadbiz++/third-party/dad/CG-SQL/sources/cql-0.y";

auto txt = readfile(fname);

auto parser_start_mark = "\n%%\n";
auto parser_start_pos = txt.indexOf(parser_start_mark);
auto before_parser = txt.slice(0, parser_start_pos);

txt = txt.slice(parser_start_pos+parser_start_mark.len());

auto brackets = {"'{'": "_ZCHAR_OB_", "'}'": "_ZCHAR_CB_", "';'": "_ZCHAR_SCOL_", "':'": "_ZCHAR_COL_"};
txt = txt.gsub("'[{};:]'", brackets);
auto buf = blob();

local function fixDeclarations(dtxt)
{
  //print("===", dtxt.slice(0, 12));
  dtxt.gmatch(
    //"\n *(%S+)%s*:(.-})%s*;",
    "\n *(%S+)%s*:(.+)",
    function(key, val)
    {
      //print("---", key);
      //print("---", key, val);
      //print(key  + " :");
      buf.write(key, ":\n");
      val = val.replace("$$", "$" + key /*+ "_0"*/);
      auto ary = [];
      auto last_pos = 0;
      val.gmatch(
        "(.-)%s*(%b{})()",
        function(klist, code, lp)
        {
	  last_pos = lp;
          if(ary.len()) ary.append("_z_"); //dummy for interleaved {}
          //print("+++", klist, "<=>", code);
          auto done = false;
          klist.gmatch(
            "(%S+)",
            function(m)
            {
              if(m == ';')
              {
                //print("***done");
                done = true;
                return false;
              }
              if(m == "|") ary.clear(); //reset
              else
              {
                ary.append(m);
              }
              return true;
            }
          );
          if(done) return false;

          foreach(m in ary)
          {
            auto idx = 0;
	    if(m == key) ++idx;
            foreach(m2 in ary) if(m == m2) ++idx;
            if(idx > 1)
            {
              while((idx = ary.find(m)) >= 0)
              {
                ary[idx] = ary[idx] + "_" + (idx+1);
              }
            }
          }

          code = code.gsub(
            "([$@])(%d+)",
            function(pfx, idx)
            {
              idx = idx.tointeger() -1;
              //print("===", ary.join("::"));
              return pfx + ary[idx];
            }
          );
          //print("==", code);
          //print("", klist.trim(), code);
          buf.write("\t", klist.trim(), "\t", code, "\n");
          return true;
        }
      );
      if(buf[buf.len()-1] == '\n') buf.setLen(buf.len()-1);
      buf.write(val.slice(last_pos), "\n\n");
      //print(";\n");
      //buf.write("\t;\n\n");
      return true;
    }
  );
}

auto last_sp = 0;
txt.gmatch(
  "\n%s*;()%s*\n",
  function(sp)
  {
    //print(sp, txt.slice(last_sp, sp));
    //buf.write(txt.slice(last_sp, sp));
    //print(sp);
    fixDeclarations(txt.slice(last_sp, sp));
    last_sp = sp;
    return true;
  }
);
//print(last_sp, txt.slice(last_sp));
//buf.write(txt.slice(last_sp));
fixDeclarations(txt.slice(last_sp));

auto txt2 = buf.tostring();
auto brackets_back = {};
foreach(k,v in brackets) brackets_back[v] <- k;
auto txt_named = txt2.gsub("_ZCHAR_[^_]+_", brackets_back);
print(txt_named);
