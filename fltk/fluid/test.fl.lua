function write_to_log(str)
	local fh = io.open('lua_script_log.txt', 'a+')
	if fh then
		fh:write(str)
		fh:close()
	end	
end

function preprocess_file(fn)
	--write_to_log(fn)
	local fh = io.open(fn)
	if fh then
		local fc = fh:read('*a')
		fh:close()
		fh = io.open(fn, 'w')
		if fh then
			fh:write('//lua preprocess\n')
			fh:write(fc)
			fh:close()
		end	
	end
end
