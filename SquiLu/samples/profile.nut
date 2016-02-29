// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
// contributed by Mike Pall

local _sq_profile_calls_at, _sq_profile_calls, _sq_profile_total, _sq_profile_this,
	_sq_profile_start_time, _sq_profile_end_time;

local function profileReset()
{
	_sq_profile_calls_at = {};
	_sq_profile_calls = {};
	_sq_profile_total = {};
	_sq_profile_this = {};
	_sq_profile_start_time = 0;
	_sq_profile_end_time = 0;
}

profileReset();

local function profileDebughook(event_type,sourcefile,line,funcname)
{
	//local fname = format("%s:%d", funcname ? funcname : "unknown", line);
	local fname = funcname ? funcname : "unknown";
	local srcfile=sourcefile ? sourcefile : "unknown";
	local fname_at = format("%s:%d:%s", fname, line, srcfile);
	//local fname_at = fname + ":" + line + ":" + srcfile;
	switch (event_type) {
		//case 'l': //called every line(that contains some code)
			//::print("LINE line [" + line + "] func [" + fname + "]");
			//::print("file [" + srcfile + "]\n");
		//break;
		case 'c': //called when a function has been called
			//::print("LINE line [" + line + "] func [" + fname + "]");
			//::print("file [" + srcfile + "]\n");
			_sq_profile_calls_at.rawset(fname_at, _sq_profile_calls_at.rawget(fname_at, 0) + 1);
			_sq_profile_this.rawset(fname, os.clock());
		break;
		case 'r': //called when a function returns
			//::print("LINE line [" + line + "] func [" + fname + "]");
			//::print("file [" + srcfile + "]\n");
			local time = os.clock() - _sq_profile_this.rawget(fname, 0);
			_sq_profile_total.rawset(fname, _sq_profile_total.rawget(fname, 0) + time);
			_sq_profile_calls.rawset(fname, _sq_profile_calls.rawget(fname, 0) + 1);
		break;
	}
}

local function profileStart()
{
	profileReset();
	_sq_profile_start_time = os.clock();
	setdebughook(profileDebughook);
}

local function profileEnd()
{
	setdebughook(null);
	_sq_profile_end_time = os.clock();
}

local function profileDump()
{
	// print the results
	local total_time = _sq_profile_end_time - _sq_profile_start_time;
	print(format("Profile info: %.3f seconds", total_time));
	local info_ary = [];
	foreach( fname, time in _sq_profile_total )
	{
		if(fname == "profileStart" || fname == "profileEnd") continue;
		local relative_time = time / (total_time / 100.0);
		local rt_int = relative_time.tointeger();
		local rt_frac = ((relative_time - rt_int) * 100).tointeger();
		info_ary.append(format("%02d.%02d %% in %.3f seconds after %d calls to %s", rt_int, rt_frac, time, _sq_profile_calls.rawget(fname, 0), fname));
	}
	info_ary.sort(@(a,b) a<b ? 1 : (a>b ? -1 : 0));
	foreach(line in info_ary)
	{
		print(line);
	}
	info_ary.clear();
	foreach( fname, count in _sq_profile_calls_at )
	{
		if(fname.startswith("profileStart") || fname.startswith("profileEnd")) continue;
		info_ary.append(format("%6d\tcalls to %s", count, fname));
	}
	info_ary.sort(@(a,b) a<b ? 1 : (a>b ? -1 : 0));
	foreach(line in info_ary)
	{
		print(line);
	}	
}

profileStart();

local function BottomUpTree(item, depth){
  if (depth > 0){
    local i = item + item;
    --depth;
    return [ item, BottomUpTree(i-1, depth),  BottomUpTree(i, depth) ];
  }
  return [ item ];
}

local function ItemCheck(tree){
  if (tree.get(1, false))  return tree[0] + ItemCheck(tree[1]) - ItemCheck(tree[2])
  return tree[0]
}

local start = os.clock()
//check_delayed_release_hooks(false);

local N = vargv.get(1, 4).tointeger();
local mindepth = 4
local maxdepth = mindepth + 2
if (maxdepth < N) maxdepth = N

{
  local stretchdepth = maxdepth + 1
  local stretchtree = BottomUpTree(0, stretchdepth)
  print(format("stretch tree of depth %d\t check: %d",
    stretchdepth, ItemCheck(stretchtree)))
}

local longlivedtree = BottomUpTree(0, maxdepth)

for(local depth=mindepth; depth <= maxdepth; depth += 2){
  local iterations = math.pow(2, (maxdepth - depth + mindepth)).tointeger()
  local check = 0
  for(local i=0; i < iterations; ++i){
    check += ItemCheck(BottomUpTree(1, depth)) +
            ItemCheck(BottomUpTree(-1, depth))
  }
  print(format("%d\t trees of depth %d\t check: %d",
    iterations*2, depth, check))
}

print(format("long lived tree of depth %d\t check: %d\n",
  maxdepth, ItemCheck(longlivedtree)))

local function printResult()
{
	print("binary-tree", N, os.clock()-start)
}

printResult();

profileEnd();
profileDump();