var etnetstrings = (function () {

	this.parse = function(data) {
		return this.parsePrivate(data, false, 0, []);
	}
	
    this.parsePrivate = function(data, expected, offset, outArray) {
        var result;

        var idx = -1;
		var max_len = data.length - offset;
		//9 is maximun integer digits allowed
		if(max_len > 12) max_len = 12;
		var slen = data.substring(offset, offset+max_len);
		slen = slen.match(/^\d+/);
		if(!slen) throw 'missing or invalid length header';

        var blob_length = slen[0];
		var type_pos = blob_length.length+offset;
		blob_length |= 0;
		var blob_type = data.charAt(type_pos);
		var blob_begin = type_pos + 1;
		var blob_end = type_pos + blob_length;

		if(expected && (expected != blob_type)) 
			throw 'type did not match expected';

        if(blob_type == ':'){
            result = data.substring(blob_begin, blob_end+1)
        } else if(blob_type == '#'){
			var str = data.substring(blob_begin, blob_end+1)
            result = parseFloat(str, 10);
        } else if(blob_type == '!'){
            result = data.charAt(blob_begin) == '1';
        } else if(blob_type == '~'){
            if (blob_length != 0)
                throw "Payload must be 0 length for null.";
            result = null;
        } else if(blob_type == '{'){
			result = {};
			if (blob_length > 0) {
				var p = [blob_begin];
				var key;
				for(var i=0; i < blob_length; i++) {
					key = this.parsePrivate(data, ':', p[0], p);
					value = this.parsePrivate(data, false, p[0], p);
					result[key] = value;
				}
				blob_end = p[0]-1;
			}
        } else if(blob_type == '['){
			result = [];
			if (blob_length > 0) {
				var p = [blob_begin];
				var value;
				for(var i=0; i < blob_length; i++) {
					value = this.parsePrivate(data, false, p[0], p);
					result[result.length] = value;
				}
				blob_end = p[0]-1
			}
        } else {
            throw "Invalid blob type: " + blob_type;
        }
		if(data.charAt(blob_end+1) != ',') throw 'invalid end mark ';
		outArray[0] = blob_end+2;
        return result;
    }

    this.dump = function(data) {
        var otype = typeof data;
        
		if(otype == 'string'){
            return data.length + ':' + data + ',';
        } else if(otype == 'number'){
            // return null for infinite numbers
            if (!isFinite(data)) output.append('0~,');
            var str = data.toString();
			return str.length + '#' + str + ',';
        } else if(otype == 'boolean'){
            if(data) return '1!1,';
			else return '1!0,';
        } else if(otype == 'object'){
            // object in js could be dict, list, null
			var i, type, result = [], count = 0;
			if (data === null) {
				return '0~,';
			}
			else if (data.length) {
				count = data.length;
				tmp_output = count + '[';
				for (i = 0; i < count; i++) {
					tmp_output += dump(data[i]);
				}
				return tmp_output + ',';
			} else {
				count = 0;
				tmp_output = "";
				for (i in data) {
					if (data[i]) {
						tmp_output += dump(i);
						tmp_output += dump(data[i]);
						count++;
					}
				}
				return count + '{' + tmp_output + ',';
			}
        } else {
            throw "Invalid blob type: " + otype;
        }
    }
	
    return this;
})();
