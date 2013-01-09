//local fd = file("/dev/null", "w")
function myprint(...){
	foreach(v in vargv){
		//fd.write(v.tostring());
	    v.tostring();
	}
	//print.acall2(this, vargv);
}
function read(){
	return "some reading";
}

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
for(local i=0; i<100000; ++i){
	
myprint(@"<html>
  <p>Prime numbers from 0 to 100, calculated by Squirrel:</p>
");

    function is_prime(n){
      if( n <= 0 ) return false;
      if( n <= 2 ) return true;
      if (n % 2 == 0) return false;
      for(local i = 3, m = n / 2; i < m ; i+=2){
        if (n % i == 0) return false;
	}
      return true;
    }

    for(local i = 1; i < 100; ++i){
      if( is_prime(i) ) myprint("<span>", i , "</span>&nbsp;");
   }

myprint(@"

 <p>Reading POST data from Squirrel (click submit):</p>
 <form method=""POST""><input type=""text"" name=""t1""/><input type=""submit""></form>

 <pre>
   POST data: [");
   local post_data = read(); myprint(post_data); 
   myprint(@"]
   request method: [");
   myprint(request_info.request_method);
myprint(@"]
   IP/port: [");
   myprint(request_info.remote_ip, ":", request_info.remote_port);
myprint(@"]
   URI: [");
myprint(request_info.uri);
myprint(@"]
   HTTP version [");
myprint(request_info.http_version);
myprint(@"]
   HEADERS:
");
     foreach( name, value in request_info.http_headers){
       myprint(name, ":", value, "\n");
     }
myprint(@"
     
 </pre>


</html>
");
}
print(os.clock()-now);