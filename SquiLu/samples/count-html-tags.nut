socket.open();

local db = SQLite3("/home/mingo/dev/tribiq/tribiq.com.webcontent.db");
db.exec_dml("begin;");
db.exec_dml("create table if not exists webcontent(id integer primary key, link text, content text, unique(link));");
local stmt = db.prepare("insert into webcontent(link) values(?);");
local stmt_content = db.prepare("update webcontent set content=? where link=?;");

function download(host, file, extra_header=null){
	local sock = socket.tcp();
	sock.connect(host, 80);
	local info = sock.getpeername()
	print("info", info.address, info.port);
	//sock.settimeout(1, "t");

	local count = 0;    // counts number of bytes read
	local req = "GET " + file + " HTTP/1.1\r\nHost: " + host + "\r\n";
	if (extra_header) req += extra_header + "\r\n";
	req += "\r\n";
	//print("req", req.len(), req);
	sock.send(req);
	local s, rc;
	local tbl = {};
	local len = 0;
	while (true){
		rc = sock.receive("*l");
		s = rc[0];
		print("s", s, rc[1]);
		//if err == "closed" then break end
		if (s.len() == 0) break;
		//if (rc[1] == socket.IO_CLOSED) break;
		local slen = s.match("Content%-Length: (%d+)");
		local sid = s.match("Set%-Cookie: session_id=([^;]+)");
		if(sid) session_id = sid;
		if (slen) {
			len = slen.tointeger();
		}
	}
	rc = sock.receive(len);
	s = rc[0];
	//print("len", len, rc[1], s.len());
	sock.close();
	//print(file, count)
	return s;
}

function downloadChunked(host, file, extra_header=null){
	local sock = socket.tcp();
	sock.settimeout(1000);
	sock.connect(host, 80);

	local count = 0;    // counts number of bytes read
	local req;
	if (extra_header) req = extra_header;
	else req = format("GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", file, host);
	
	print("REQUEST:", req)
	sock.send(req);
	local s, rc;
	local data = [];
	while (true){
		rc = sock.receive("*a");
		s = rc[0];
		//print("s", s, rc[1]);
		if (s.len() == 0) break;
		data.push(s);
	}
	sock.close();
	//print(file, count)
	return data.concat("\n");
}

function count_html_links(content){
	local lines = [];
	local line_pos = 0;
	while((line_pos = content.find("\n", line_pos))){
		lines.push(line_pos++);
	}
	if(line_pos < content.len()) lines.push(content.len());
	print("Lines :", lines.len());
	
	local tag_re = "<a href=\"([^\"]+)\"";
	local tags = {};
	content.find_lua(tag_re, function(start, end, m){
		if(m != "#"){
			local start_pos = start;
			local line = lines.bsearch(start_pos);
			if(line < 0){
				//binary search didn't found an exact match
				//but returned the closest element it was tried
				//turned in negative value
				line = -line;
				while(lines[line] < start_pos) ++line;
			}
			//find the character position on the line
			start_pos -= (line > 0 ? lines[line-1] : 0);
			
			if(!tags.get(m, false)){
				stmt.reset();
				stmt.bind(1, m);
				stmt.step();
				
				tags[m] <- [start, end, line];
				//print(start, end, line+1, start_pos, m);
			}
		}
		return true;
	});
	return tags;
}

function count_html_tags(content){
	local lines = [];
	local line_pos = 0;
	while((line_pos = content.find("\n", line_pos))){
		lines.push(line_pos++);
	}
	if(line_pos < content.len()) lines.push(content.len());
	print("Lines :", lines.len());
	
	local tag_re = "(</?div[^>]*>)";
	local tags = [];
	content.find_lua(tag_re, function(start, end, m){
		local start_pos = start;
		local line = lines.bsearch(start_pos);
		if(line < 0){
			//binary search didn't found an exact match
			//but returned the closest element it was tried
			//turned in negative value
			line = -line;
			while(lines[line] < start_pos) ++line;
		}
		//find the character position on the line
		start_pos -= (line > 0 ? lines[line-1] : 0);
		
		tags.push([start, end, line, m]);
		//print(start, end, m[1] == '/' ? "close" : "open", line+1, start_pos, m);
		return true;
	});
	return tags;
}

function count_html_tags_from_file(fn){
	local fd = file(fn, "r");
	local content = fd.read(fd.len());
	fd.close();
	
	return count_html_tags(content);
}

function count_html_tags_from_http(host, fn){
	local content = downloadChunked(host, fn);
	return count_html_tags(content);
}

//local tags = count_html_tags_from_file("/var/www/tribiq2/index2.html");
//local tags = count_html_tags_from_http("tribiq.com", "/");

local content = downloadChunked("tribiq.com", "/");

local divs = count_html_tags(content);
print("Html divs :", divs.len());

local links = count_html_links(content);
print("Html links :", links.len());

foreach(k,v in links){
	if(!k.startswith("http://")){
		local fn = k[0] == '/' ? k : "/" + k;
		local page_content = downloadChunked("tribiq.com", fn);
		local page_divs = count_html_tags(page_content);

		local found;
		foreach(idx, rec in page_divs){
			local tag = rec[3];
			if(tag.find("\"Main\"")){
				found = idx
				break;
			}
		}

		if(found){
			local start_pos = page_divs[found][1] +1; //end of start tag
			local nested = 1;
			for(local i=found+1, count=page_divs.len(); i < count; ++i){
				local rec = page_divs[i];
				local tag = rec[3];
				if(tag[1] == '/') --nested;
				else ++nested;
				if(nested == 0){
					local end_pos = rec[0]; //start of closing tag
					local div_content = page_content.slice(start_pos, end_pos);
					div_content = div_content.gsub("\n%s+", "");
					//print( div_content );
					print("Link:", k, start_pos, end_pos, div_content);
					stmt_content.reset();
					stmt_content.bind(1, div_content);
					stmt_content.bind(2, k);
					stmt_content.step();
					break;
				}
			} 
		}
	}
}


if (vargv.len() > 1){ 
	count_html_tags(vargv[1]);
}

stmt.finalize();
stmt_content.finalize();
db.exec_dml("commit;");
db.close();
socket.close();