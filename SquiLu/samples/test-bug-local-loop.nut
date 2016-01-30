local count = 10;
local last_count = 0;

while(count > 0 && last_count != count){
	print(__LINE__, last_count, count);
	last_count = count;
	print(__LINE__, last_count, count);
	os.sleep(10);
}

local m_State = 0;

function pump( data, start, datasize ){
	assert( datasize != 0 );
	local count = datasize;
	local data_idx = 0;
	local last_count = 0;

	while( count > 0 && m_State != 2  && last_count != count)
	{
print(__LINE__, count, last_count, data)
		last_count = count;
print(__LINE__, count, last_count, data)
		os.sleep(10);
	}
}

local str = "<pre>";
pump(str, 0, 173);
