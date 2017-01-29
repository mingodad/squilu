/*
With inpiration and ideas from https://github.com/slembcke/debugger.lua
*/

local globals = getroottable();

local tget = table_get;
local trget = table_rawget;
local trset = table_rawset;

local last_cmd;
local stack_depth = 0;
local stack_depth_offset = 0;
local min_stack_depth;

local function table_merge(t1, t2)
{
	local tbl = {};
	foreach( k, v in t1) tbl[k] <- v;
	foreach( k, v in t2) tbl[k] <- v;

	return tbl;
}

// Create a table of all the locally accessible variables.
// Globals are not included when running the locals command, but are when running the print command.
local function local_bindings(offset, include_globals=false)
{
	local level = stack_depth_offset + offset;
	local stack_info = getstackinfos(level);
	local func = stack_info.func;
	local bindings = {};
		
	// Retrieve the locals (overwriting any upvalues)
	foreach(k,v in stack_info.locals)
	{
		if(k == "this") continue;
		bindings[k] <- v;
	}
	
	if(include_globals)
	{
		trset(bindings, "_index", globals);
	}
	
	return bindings;
}

local function pretty(obj, recurse=false)
{
	// Returns true if a table has a __tostring metamethod.
	local function coerceable(tbl)
	{
		local meta = table_getdelegate(tbl)
		return (meta && tget(meta, "_tostring", false));
	}
	
	if( type(obj) == "string")
	{
		// Dump the string so that escape sequences are printed.
		return format("%q", obj);
	}
	else if( (type(obj) == "table") && !coerceable(obj) && !recurse )
	{
		local str = "{";
		
		foreach(k, v in obj)
		{
			local pair = pretty(k, true) + " = " + pretty(v, true);
			str = str + ((str == "{") && (pair || (", " + pair)));
		}
		
		return str + "}";
	}
	else
	{
		// tostring() can fail if there is an error in a __tostring metamethod.
		local value;
		try {value = obj.tostring();} catch(e){print("tostring error", e);}
		return (value || "<!!error in _tostring metamethod!!>");
	}
}

local help_message = [==[
[return] - re-run last command
c(ontinue) - contiue execution
s(tep) - step forward by one line (into functions)
n(ext) - step forward by one line (skipping over functions)
p(rint) [expression] - execute the expression and print the result
f(inish) - step forward until exiting the current function
u(p) - move up the stack by one frame
d(own) - move down the stack by one frame
t(race) - print the stack trace
l(ocals) [stack frame] - print the function arguments, locals and upvalues.
q(uit) - terminates the program.
h(elp) - print this message
]==];


local function myDebugHook(event_type,sourcefile,line,funcname)
{
	if(sourcefile == __FILE__) return;
	
	if(event_type == 'c')
	{
		++stack_depth;
		return;
	}
	
	if(event_type == 'r')
	{
		--stack_depth;
		return;
	}
	
	if(event_type == 'l')
	{		
		if(min_stack_depth != null)
		{
			if(stack_depth > min_stack_depth) return;
		}
		min_stack_depth = null;
		
		local done = false;
		do
		{
			local stack_info = getstackinfos(2);
			local fname = (stack_info.func || format("<%s:%d>", stack_info.src, stack_info.line));
			stdout.write(format("%s:%d in '%s' $> ", stack_info.src, stack_info.line, fname));
			
			local cmd = stdin.read_line();
			if( cmd == "")
			{
				if(last_cmd)
				{
					cmd = last_cmd;
				}
			}
			
			if(cmd == "c")
			{
				setdebughook(null);
				done = true;
			}
			
			else if(cmd == "s")
			{
				done = true;
			}
			
			else if(cmd == "n")
			{
				done = true;
				min_stack_depth = stack_depth;
			}

			else if(cmd == "f")
			{
				done = true;
				min_stack_depth = stack_depth-1;
			}

			else if(cmd.len() && cmd[0] == 'l')
			{
				local depth = cmd.match("l%s+(%d+)");
				if(depth)
				{
					depth = depth.tointeger() + 2;
				}
				else depth = 3;
				
				local bindings = local_bindings(depth, false);
				
				// Get all the variable binding names and sort them
				local keys = [];
				foreach( k, _ in bindings) keys.append(k);
				keys.sort();
				
				foreach(k in keys)
				{
					local v = bindings[k];
					print(format("\t%s => %s", k, pretty(v)));
				}
			}

			else if(cmd.len() && cmd[0] == 'p')
			{
				auto exp = cmd.match("p%s?(.*)");
				//print("p=", exp);
				try
				{
					local env = local_bindings(3, true);
					local chunk = compilestring("return " + exp, "debugger.nut repl");
					
					// Call the chunk and collect the results.
					local result = chunk.pcall(env);
					print(pretty(result));
				}
				catch(e)
				{
					print("p cmd error", e);
				}
			}

			else if(cmd == "q")
			{
				os.exit(0);
			}

			else if(cmd == "h")
			{
				print(help_message);
			}

			if(last_cmd != cmd) last_cmd = cmd;

		} while (!done);
	}
}

local function dbg()
{
	setdebughook(myDebugHook);
}

return dbg;