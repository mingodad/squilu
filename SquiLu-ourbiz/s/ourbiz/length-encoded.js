var LengthEncodeData = (function(){
	var IBYTE1 = 255;
	var IBYTE2 = (255*255);
	var IBYTE3 = (255*255*255);
	var IBYTE4 = (255*255*255*255);

	var SIZE1BYTE = 250;
	var SIZE2BYTE = 251;
	var SIZE3BYTE = 252;
	var SIZE4BYTE = 253;

	var SLSMARK = 254;
	var SLSEND = 255;

	this.sls2callable = function(sls, sls_size, use_str){
		if(sls_size == 0) return 0;
		var p = 0;
		var size;
		var data_count = 0, data_len = 0;
		while(p != SLSEND) //data finished now follow digest
		{
			size = sls.charAt(p++);
			if(size > 249)
			{
				if(size == SIZE1BYTE)
				{
					//data bigger than 250 and less 500 next byte has more info
					size += sls.charAt(p++);
				}
				else if(size == SIZE2BYTE)
				{
					//data bigger than 250 next two bytes has more info
					size = sls.charAt(p++) * IBYTE1;
					size += sls.charAt(p++);
				}
				else if(size == SIZE3BYTE)
				{
					//data bigger than 250 next three bytes has more info
					size = sls.charAt(p++) * IBYTE2;
					size += sls.charAt(p++) * IBYTE1;
					size += sls.charAt(p++);
				}
				else if(size == SIZE4BYTE)
				{
					//data bigger than 250 next four bytes has more info
					size = sls.charAt(p++) * IBYTE3;
					size += sls.charAt(p++) * IBYTE2;
					size += sls.charAt(p++) * IBYTE1;
					size += sls.charAt(p++);
				}
				else if(size == SLSMARK)
				{
					//reserved can be used for multi part data, metadata, digest, ...
					break;
				}
			}
			use_str(p, size);
			p += size;
			data_count++;
			data_len += size;
			if(data_len > sls_size)
			{
				//something went wrong here
				throw "We got more data than expected !";
				break;
			}
		}
		return ++p;
	}

	this.add2sls = function(sls, str, size){
		var next_size, tmp_size;
		if(size < 250)
		{
			sls.append(size);
		}
		else if(size < 500)
		{
			sls.append(SIZE1BYTE);
			next_size = size - 250;
			sls.append(next_size);
		}
		else if(size < IBYTE2)
		{
			sls.append(SIZE2BYTE);
			next_size = size/IBYTE1;
			sls.append(next_size);
			next_size = size%IBYTE1;
			sls.append(next_size);
		}
		else if(size < IBYTE3)
		{
			sls.append(SIZE3BYTE);
			next_size = size/IBYTE2;
			sls.append(next_size);
			tmp_size = size%IBYTE2;
			next_size = tmp_size/IBYTE1;
			sls.append(next_size);
			next_size = tmp_size%IBYTE1;
			sls.append(next_size);
		}
		else if(size < IBYTE4)
		{
			sls.append(SIZE4BYTE);
			next_size = size/IBYTE3;
			sls.append(next_size);
			tmp_size = size%IBYTE3;
			next_size = tmp_size/IBYTE2;
			sls.append(next_size);
			tmp_size = tmp_size%IBYTE2;
			next_size = tmp_size/IBYTE1;
			sls.append(next_size);
			next_size = tmp_size%IBYTE1;
			sls.append(next_size);
		}
		sls.append(str, size);
		return size;
	}
	
	return this;
})();