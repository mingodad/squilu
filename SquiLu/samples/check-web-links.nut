auto trget = table_rawget;
auto trset = table_rawset;

int_t curl_reader(buffer, size, nitems, udata)
{
	//print("***Got", udata, buffer.len(), size, nitems, "\n", buffer, "\n");
	udata.write(buffer);
	return buffer.len();
}

auto curl = EasyCurl();
//curl.set_writer(data);
//curl.set_writer(curl_reader);

//curl.setopt( curl.CURLOPT_VERBOSE, 1 );
//accept compressed
curl.setopt( curl.CURLOPT_ACCEPT_ENCODING, "" );
//insecure
//curl.setopt( curl.CURLOPT_SSL_VERIFYPEER, 0 );

curl.setopt( curl.CURLOPT_FOLLOWLOCATION, 1);

int_t curl_reader_header(buffer, size, nitems, udata)
{
	//print("***Got", udata, buffer.len(), size, nitems, "\n", buffer, "\n");
	udata.write(buffer);
	return buffer.len() == 2 ? 0 : buffer.len(); //2  == \n\n after headers
}

auto getUrlHeadersData(url)
{
	auto data = blob(0, 8192);
	curl.set_writer(curl_reader_header, data);
	curl.setopt( curl.CURLOPT_HEADER, 1);
	curl.setopt( curl.CURLOPT_URL, url );
	auto result = curl.perform();
	curl.setopt( curl.CURLOPT_HEADER, 0);
	return data.tostring();
}

auto getUrlData(url)
{
	auto data = blob(0, 8192);
	curl.set_writer(curl_reader, data);
	curl.setopt( curl.CURLOPT_URL, url );
	auto result = curl.perform();
	return data.tostring();
}

auto function testGetUrl()
{
	auto url = "https://www.easycredito.me/";
	auto txt = getUrlHeadersData(url);
	print(txt);
	txt = getUrlData(url);
	print(txt);
}

//testGetUrl();

auto checkLinks(url, host, links, links_visited)
{	
	auto txt = getUrlData(url);
	if(!txt || !txt.len()) return;
	print("txt", txt.len());
	auto host_re = host.replace(".", "%."); 
	auto url_host = url.match("^[^/]*//[^/]+/");

	auto function setLink(mt, mlt, ml)
	{
		auto first_chr = ml[0];
		switch(first_chr)
		{
			case '"':
			case '\'':
				ml = ml.match(format("%c([^%c]+)%c", first_chr, first_chr, first_chr));
				if(ml)
				{
					if(!ml.match("//"))
					{
						if(ml[0] == '/')
						{
							ml = ml.slice(1);
						}
						ml = url_host + ml;
					}
					trset(links, ml, mt);
				}
			break;			
		}
		return true;
	}

	txt.gmatch("<(%a+)%s+[^<>]*(src)=(%S+)", setLink);
	txt.gmatch("<(%a+)%s+[^<>]*(href)=(%S+)", setLink);
	/*
	txt.gmatch(
		"url=(%S+)",
		function(m)
		{
			auto first_chr = m[0];
			switch(first_chr)
			{
				case '"':
				case '\'':
					m = m.match(format("%c([^%c]+)%c", first_chr, first_chr, first_chr));
				break;			
			}
			trset(links, m, true);
			return true;
		}
	);
	*/
	trset(links_visited, url, true);
	
	foreach(k,v in links)
	{
		//print(v, k);
		auto wasVisited = trget(links_visited, k, false);
		if( (v == "a")
			&& (k.endswith(".html"))
			&& !wasVisited
			&& k.match(host_re)
			)
		{
			print("***", v, k);
			checkLinks(k, host, links, links_visited);
		}
		else if(!wasVisited)
		{
			auto found = getUrlHeadersData(k);
			print(":::", v, k, found.len());
			trset(links_visited, k, found.len() > 0);
		}
	}
	//print(txt);
}

auto links = {};
auto links_visited = {};
checkLinks("https://www.easycredito.me/", "easycredito.me", links, links_visited);
//checkLinks("http://mymvpblueprint.com/", "mymvpblueprint.com", links, links_visited);
foreach(k,v in links) print(v, k);
foreach(k,v in links_visited) print(v, k);
