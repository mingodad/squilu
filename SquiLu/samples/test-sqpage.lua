--local fd = io.open("/dev/null", "w")
function myprint(...)
	for i = 1, select("#",...) do
		--fd:write(tostring(select(i,...)));
		tostring(select(i,...));
	end
	--print.acall2(this, vargv);
end
function read()
	return "some reading";
end

local request_info = {
	request_method = "POST",
	remote_ip="localhost",
	remote_port="8080",
	uri="http://www.dad.dad/",
	http_version="1.1",
	http_headers={
		name="carl marks",
		age="200",
		high="1.8",
		country="urss",
		language="english",
	},
};

local now = os.clock();
for i=0, 100000 do
	
myprint([[<html>
  <p>Prime numbers from 0 to 100, calculated by Squirrel:</p>
]]);

    function is_prime(n)
      if( n <= 0 ) then return false end;
      if( n <= 2 ) then return true end;
      if (n % 2 == 0) then return false end;
      for i = 3, n / 2, 2 do
        if (n % i == 0) then return false end;
      end
      return true;
    end

    for i = 1, 100 do
      if( is_prime(i) ) then myprint("<span>", i , "</span>&nbsp;") end;
   end

myprint([[

 <p>Reading POST data from Squirrel (click submit):</p>
 <form method=""POST""><input type=""text"" name=""t1""/><input type=""submit""></form>

 <pre>
   POST data: []]);
   local post_data = read(); myprint(post_data); 
   myprint([[]
   request method: []]);
   myprint(request_info.request_method);
myprint([[]
   IP/port: []]);
   myprint(request_info.remote_ip, ":", request_info.remote_port);
myprint([[]
   URI: []]);
myprint(request_info.uri);
myprint([[]
   HTTP version []]);
myprint(request_info.http_version);
myprint([[]
   HEADERS:
]]);
     for name, value in pairs(request_info.http_headers) do
       myprint(name, ":", value, "\n");
     end
myprint([[
     
 </pre>


</html>
]]);
end
print(os.clock()-now);