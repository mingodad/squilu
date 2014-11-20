<!doctype html>
<!--
Design by Free Responsive Templates
http://www.free-responsive-templates.com
Released for free under a Creative Commons Attribution 3.0 Unported License (CC BY 3.0)
-->
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Companies Info UK</title>
<link href="css/styles.css" rel="stylesheet" type="text/css">
<style type="text/css">
article table td:nth-child(2), td:nth-child(3) {
	text-align: right;
}
article table.nj td:nth-child(2), td:nth-child(3) {
	text-align: left;
}
article table.dist td:nth-child(3), td:nth-child(4), td:nth-child(5)  {
	text-align: right;
}
table {
	margin-left:auto;
    margin-right:auto;
}
table, th, td
{
	border: 1px solid #ccc;
}
table td {
	vertical-align:top;
    padding-left: 0.5em;
    padding-right: 0.5em;
}
  table.bold2 td:nth-child(2) {font-weight:bold;}
.post header {
      text-align:center;
	float:none;
	width:100%;
	padding:0;
}
</style>

<?
  local account_categories = [
["TOTAL EXEMPTION SMALL","1,417,506","46.38%"],
["NO ACCOUNTS FILED","837,086","27.39%"],
["DORMANT","406,070","13.29%"],
["TOTAL EXEMPTION FULL","164,883","5.40%"],
["FULL","137,209","4.49%"],
["SMALL","61,113","2.00%"],
["GROUP","21,292","0.70%"],
["MEDUM","5,523","0.18%"],
["ACCOUNTS TYPE NOT AVAILABLE","5,099","0.17%"],
["PARTIAL EXEMPTION","230","0.01%"],
["INITIAL","7","0%"],
  ]

  local company_categories = [
["Private Limited Company","2,816,656","92.17%"],
["PRI/LTD BY GUAR/NSC (Private, limited by guarantee, no share capital)","77,564","2.54%"],
["Limited Liability Partnership","54,887","1.80%"],
["PRI/LBG/NSC (Private, Limited by guarantee, no share capital, use of 'Limited' exemption)","39,623","1.30%"],
["Limited Partnership","22,542","0.74%"],
["Other company type","11,580","0.38%"],
["Industrial and Provident Society","9,961","0.33%"],
["Public Limited Company","8,473","0.28%"],
["Community Interest Company","7,101","0.23%"],
["Private Unlimited Company","6,011","0.20%"],
["Royal Charter Company","832","0.03%"],
["Investment Company with Variable Capital","500","0.02%"],
["Private Unlimited","140","0.00%"],
["Investment Company with Variable Capital(Umbrella)","99","0.00%"],
["Old Public Company","22","0.00%"],
["PRIV LTD SECT. 30 (Private limited company, section 30 of the Companies Act)","15","0.00%"],
["Investment Company with Variable Capital (Securities)","12","0.00%"],
  ]

  local company_status = [
["Active","2,824,726","92.43%"],
["Active - Proposal to Strike off","139,788","4.57%"],
["Liquidation","79,770","2.61%"],
["In Administration","4,740","0.16%"],
["Live but Receiver Manager on at least one charge","2,516","0.08%"],
["Voluntary Arrangement","1,864","0.06%"],
["ADMINISTRATIVE RECEIVER","1,492","0.05%"],
["In Administration/Administrative Receiver","425","0.01%"],
["RECEIVERSHIP","322","0.01%"],
["ADMINISTRATION ORDER","189","0.01%"],
["In Administration/Receiver Manager","102","0.00%"],
["RECEIVER MANAGER / ADMINISTRATIVE RECEIVER","74","0.00%"],
["VOLUNTARY ARRANGEMENT / RECEIVER MANAGER","8","0.00%"],
["VOLUNTARY ARRANGEMENT / ADMINISTRATIVE RECEIVER","1","0.00%"],
["VOLUNTARY ARRANGEMENT / RECEIVERSHIP","1","0.00%"],
  ]

local company_ages = [
["475","1","0.00%"],
["+150","75","0.00%"],
["100-149","5,758","0.19%"],
["50-99","53,788","1.76%"],
["30-49","120,862","3.95%"],
["20-29","175,876","5.76%"],
["10-19","565,363","18.50%"],
["5-9","719,719","23.55%"],
["4","179,057","5.86%"],
["3","233,894","7.65%"],
["2","316,072","10.34%"],
["1","451,577","14.78%"],
["-1","233,976","7.66%"],
]

function emptyOnNull(key){
	local str = this.get(key, null);
	return str ? str : "";
}

function getPageLink(page){
	local fmt = "/?search_str=%s&search_post_code=%s&search_sic_code=%s&search_origin_post_code=%s&search_around_post_code=%s&page=%d";
	return format(fmt,
		url_encode(emptyOnNull("search_str")),
		url_encode(emptyOnNull("search_post_code")),
		url_encode(emptyOnNull("search_sic_code")),
		url_encode(emptyOnNull("search_origin_post_code")),
		url_encode(emptyOnNull("search_around_post_code")),
		page);
}

?>
<? if (this.get("use_vjs", false) && this.get("page_name", null) == "search_results"){ ?>
    <!--Load the AJAX API-->
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">

      // Load the Visualization API and the piechart package.
      google.load('visualization', '1.0', {'packages':['corechart']});

      // Set a callback to run when the Google Visualization API is loaded.
      google.setOnLoadCallback(drawChart);

      // Callback that creates and populates a data table,
      // instantiates the pie chart, passes in the data and
      // draws it.
      function drawChart() {

        // Create the data table.
        var data = new google.visualization.DataTable();
        data.addColumn('string', 'Topping');
        data.addColumn('number', 'Slices');
        data.addRows([
          <? foreach(k,v in account_categories) { ?>
          ["<?=v[0]?>", <?=v[1].gsub(",","")?>],
          <? } ?>
          ['', 0]
        ]);

        // Set chart options
        var options = {'title':'Distribution of companies by Account Category',
                       'width2':400,
                       'height':300};

        // Instantiate and draw our chart, passing in some options.
        var chart = new google.visualization.PieChart(document.getElementById('chart_div'));
        chart.draw(data, options);

        // Create the data table.
        var data2 = new google.visualization.DataTable();
        data2.addColumn('string', 'Topping');
        data2.addColumn('number', 'Slices');
        data2.addRows([
          <? foreach(k,v in company_categories){ ?>
          ["<?=v[0]?>", <?=v[1].gsub(",","")?>],
          <? } ?>
          ['', 0]
        ]);

        // Set chart options
        var options2 = {'title':'Distribution of companies by Company Category',
                       'width2':400,
                       'height':300};

        // Instantiate and draw our chart, passing in some options.
        var chart2 = new google.visualization.PieChart(document.getElementById('chart2_div'));
        chart2.draw(data2, options2);

        // Create the data table.
        var data3 = new google.visualization.DataTable();
        data3.addColumn('string', 'Topping');
        data3.addColumn('number', 'Slices');
        data3.addRows([
          <? foreach(k,v in company_status) { ?>
          ["<?=v[0]?>", <?=v[1].gsub(",","")?>],
          <? } ?>
          ['', 0]
        ]);

        // Set chart options
        var options3 = {'title':'Distribution of companies by Company Status',
                       'width2':400,
                       'height':300};

        // Instantiate and draw our chart, passing in some options.
        var chart3 = new google.visualization.PieChart(document.getElementById('chart3_div'));
        chart3.draw(data3, options3);

        // Create the data table.
        var data4 = new google.visualization.DataTable();
        data4.addColumn('string', 'Topping');
        data4.addColumn('number', 'Slices');
        data4.addRows([
          <? foreach(k,v in company_ages){ ?>
          ["<?=v[0]?> years", <?=v[1].gsub(",","")?>],
          <? } ?>
          ['', 0]
        ]);

        // Set chart options
        var options4 = {'title':'Distribution of companies by Age',
                       'width2':400,
                       'height':300};

        // Instantiate and draw our chart, passing in some options.
        var chart4 = new google.visualization.PieChart(document.getElementById('chart4_div'));
        chart4.draw(data4, options4);

        var data5 = google.visualization.arrayToDataTable([
          ['Year', 'Survive', 'Registered'],
["2000",50134,210350],
["2001",58321,231281],
["2002",60334,211903],
["2003",95499,280408],
["2004",126032,378591],
["2005",102056,317459],
["2006",114497,341322],
["2007",135753,374450],
["2008",167628,423018],
["2009",157289,320284],
["2010",191538,333040],
["2011",253577,365982],
["2012",377167,413809],
        ]);

        var options5 = {
          title: 'Historical Company Registered and Survival',
          hAxis: {title: 'Year',  titleTextStyle: {color: 'red'}}
        };

        var chart5 = new google.visualization.ColumnChart(document.getElementById('chart5_div'));
        chart5.draw(data5, options5);
      }
    </script>
<? } ?>
<script type="text/javascript">
function getCookie(c_name)
{
var i,x,y,ARRcookies=document.cookie.split(";");
for (i=0;i<ARRcookies.length;i++)
  {
  x=ARRcookies[i].substr(0,ARRcookies[i].indexOf("="));
  y=ARRcookies[i].substr(ARRcookies[i].indexOf("=")+1);
  x=x.replace(/^\s+|\s+$/g,"");
  if (x==c_name)
    {
    return unescape(y);
    }
  }
}

function setCookie(c_name,value,exdays)
{
var exdate=new Date();
exdate.setDate(exdate.getDate() + exdays);
var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString());
document.cookie=c_name + "=" + c_value;
}

var saveLoadFormFields = ['search_str', 'search_post_code', 'search_sic_code', 'search_origin_post_code', 'search_around_post_code'];
function saveFormFields(frm){
	for(var i=0; i< saveLoadFormFields.length; ++i){
		setCookie(saveLoadFormFields[i], frm[saveLoadFormFields[i]].value, 180);
	}
	return true;
}

function loadFormFields(){
	var frm = document.getElementById("search_form");
	if(frm){
		for(var i=0; i< saveLoadFormFields.length; ++i){
			 frm[saveLoadFormFields[i]].value = getCookie(saveLoadFormFields[i]) || '';
		}
	}
}

function show_location(position) {
	var request;
	if (window.XMLHttpRequest)  request = new XMLHttpRequest();     // Firefox, Safari, ...
	else if (window.ActiveXObject)   request = new ActiveXObject("Microsoft.XMLHTTP"); // Internet Explorer
	request.onreadystatechange = function(event, cached){
		if (request.readyState != 4) return;
		var cpc = document.getElementById("cpc");
		cpc.value = request.responseText;
	}
	request.open("GET", "/getpc?lat=" +  position.coords.latitude + "&lon=" +position.coords.longitude, true);
	request.send(null);
}

function get_post_code(){
	navigator.geolocation.getCurrentPosition(show_location);
}

function setSicCode(code){
	document.getElementById("search_form").search_sic_code.value=code
}

var isSortedByDistance = true;

function sortTable(id, col, numeric){
    var tbl = document.getElementById(id).tBodies[0];
    var store = [];
    for(var i=1, len=tbl.rows.length; i<len; i++){
        store.push( tbl.rows[i]);
    }
    if(numeric){
	store.sort(function(x,y){
		var num1 = parseFloat(x.cells[col].textContent || row.cells[col].innerText);
		var num2 = parseFloat(y.cells[col].textContent || row.cells[col].innerText);
		return num1-num2;
	});
    } else {
	store.sort(function(x,y){
		var str1 = x.cells[col].textContent || row.cells[col].innerText;
		var str2 = y.cells[col].textContent || row.cells[col].innerText;
		return (str1 < str2 ? -1 : (str1 > str2 ? 1 : 0));
	});
    }

    for(var i=0, len=store.length; i<len; i++){
        tbl.appendChild(store[i]);
    }
    store = null;
    return false;
}
</script>
</head>
<body onload="loadFormFields()">
<div class="wrapper">
	<header>
      <div class="logo"><h1><a href="/">CIUK</a></h1></div>
        <nav>
            <ul id="navlist">
              <li id="active"><a href="/">Home</a></li>
                <li><a href="#">About</a></li>
                <li><a href="#">Shop</a></li>
                <li><a href="#">Customers</a></li>
                <li><a href="#">Contact</a></li>
            </ul>
        </nav>
        <div class="clearfloat"></div>
    </header>
    <section>
    	<div class="horSeparator"></div>
    	<h1>Welcome to Companies Info UK</h1>
        <p>
        	Here you'll find interesting statistics and info about companies in UK.
        </p>
<form action="/#results" method="post" onsubmit="saveFormFields(this)" id="search_form">
<table>
<tr><td>Company Name</td><td><input type="text" name="search_str" value="<?=search_str?>" title="enter a company name to search"></td></tr>
<tr><td>Extra info</td><td><input type="text" name="search_extra_info" value="<?=search_extra_info?>" title="enter extra info to search"></td></tr>
<tr><td>Post Code</td><td><input type="text" name="search_post_code" value="<?=search_post_code?>" title="enter a complete or parcial post code"></td></tr>
<tr><td><select name="sic_street"><option value="sic">SIC Code</option><option value="street">Street</option></select></td>
<td><input type="text" name="search_sic_code" value="<?=search_sic_code?>" title="enter a complete or parcial sic code"></td></tr>
<tr><td>Your Post Code</td><td><input type="text" id="cpc" name="search_origin_post_code" value="<?=search_origin_post_code?>" title="enter your postcode if you want see distances" size="7">
	<input type="text" name="search_around_post_code" value="<?=search_around_post_code?>" title="enter the distance in meters to search around your postcode" size="7">
</td></tr>
<tr><td><button type=submit>Search</button></td><td id="cloc"><button type=button onclick="get_post_code()">Get Postcode</button>
	<button type=reset>Reset</button></td></tr>
</table>
</form>
<? if (this.get("isAndroid", false)){ ?>
<a href="/mk-gpx?sic_term=<?=search_sic_code?>">Generate GPX</a>
<? } ?>
        <div class="horSeparator"></div>
    </section>
    <article class="post">
<? if (page_name == "search_results"){ ?>

<? if (this.get("sic_street", null)  == "street"){ ?>

<header><h1 id="results">Street Search Results</h1></header>
<? if (this.get("queryWasInterrupted", false)){ ?>
<p>Please try again with more specific search terms, it took too long !</p>
<? } ?>
<? if (this.get("rows", false) && rows.len() > 0){ ?>
<table class="nj">
<tr><th>Company</th><th>Street</th><th>Postcode</th></tr>
<? foreach( k, rec in rows){ ?>
<tr><td><a href="view?id=<?=rec[0]?>#results"><?=rec[0]?></a></td><td><?=rec[1]?></td><td><?=rec[2]?></td></tr>
<? }
	page = page.tointeger();
	local hasPrevious = page;
	local hasNext = rows.len() == limit;
	if (hasPrevious || hasNext){
?>
<tr><td>
&nbsp;
<? if (hasPrevious){ ?>
<a href="/?search_sic_code=<?=url_encode(this.get("search_sic_code", ""))?>&page=<?=page-1?>">Prev</a>
<? } ?>
</td><td>&nbsp;</td><td>
&nbsp;
<? if (hasNext){ ?>
<a href="/?search_sic_code=<?=url_encode(this.get("search_sic_code", ""))?>&page=<?=page+1?>">Next</a>
<? } ?>
</td></tr>
<? } ?>
</table>
<?
	}
} else if (this.get("sicSearchResults", false)) { ?>

<header><h1 id="results">SIC Code Search Results</h1></header>
<? if (this.get("queryWasInterrupted", false)){ ?>
<p>Please try again with more specific search terms, it took too long !</p>
<? } ?>
<? if (this.get("rows", false) && rows.len() > 0){ ?>
<table class="nj">
<tr><th>Code</th><th>Description</th><th>#Companies</th></tr>
<? foreach( k, rec in rows){ ?>
<tr><td><a href="#search_form" onclick="setSicCode(<?=rec[1]?>)"><?=rec[1]?></a></td><td><?=rec[2]?></td><td><?=rec.get(3, "")?></td></tr>
<? }
	page = page.tointeger();
	local hasPrevious = page > 0;
	local hasNext = rows.len() == limit;
	if (hasPrevious || hasNext){
?>
<tr><td>
&nbsp;
<? if (hasPrevious){ ?>
<a href="/?search_sic_code=<?=url_encode(this.get("search_sic_code", ""))?>&page=<?=page-1?>">Prev</a>
<? } ?>
</td><td>&nbsp;</td><td>
&nbsp;
<? if (hasNext){ ?>
<a href="/?search_sic_code=<?=url_encode(this.get("search_sic_code", ""))?>&page=<?=page+1?>">Next</a>
<? } ?>
</td></tr>
<? } ?>
</table>
<?
	}
} else { ?>

<header><h1 id="results">Companies Search Results</h1></header>
<? if (this.get("queryWasInterrupted",false)) { ?>
<p>Please try again with more specific search terms, it took too long !</p>
<? } ?>
<? if (this.get("rows", false) && rows.len() > 0){ ?>
<table class="nj dist" id="tblcr">
<tr><th><a href="" onclick="return sortTable('tblcr', 0)">Number</a></th>
	<th><a href="" onclick="return sortTable('tblcr', 1)">Name</a></th>
	<th><a href="" onclick="return sortTable('tblcr', 2, true)">Age (years)</a></th>
<?
	local hasDistances = this.get("search_origin_post_code", false) && search_origin_post_code.len() > 0;
	if (hasDistances){
?>
<th><a href="" onclick="return sortTable('tblcr', 3, true)">Dist. Meters</a></th>
<? } ?>
<th>Google</th></tr>
<? foreach( k, rec in rows){ ?>
<tr><td><?=rec[1]?></td><td><a href="view?id=<?=rec[0]?>#results"><?=rec[2]?></a></td><td><?=rec[3]?></td>
<?if (hasDistances){ ?>
<td><?=rec[5]?> <img src="images/small-arrow.png"  style="transform:rotate(<?=rec[6]?>deg);-webkit-transform:rotate(<?=rec[6]?>deg)"></td>
<? } ?>
<td><a href="http://www.google.co.uk/search?q=<?=url_encode(format("%s %s", rec[2], rec[4]))?>" target="gsearch">Google</a></td></tr>
<? }
	page = page.tointeger();
	local hasPrevious = page > 0;
	local hasNext = rows.len() == limit;
	if (hasPrevious || hasNext){
?>
<tr><td>
&nbsp;
<? if (hasPrevious){ ?>
<a href="<?=getPageLink(page-1)?>#results">Prev</a>
<? } ?>
</td><td>&nbsp;</td><td>
&nbsp;
<? if (hasNext){ ?>
<a href="<?=getPageLink(page+1)?>#results">Next</a>
<? } ?>
</td>
<?if (hasDistances){ ?>
<td>&nbsp;</td>
<? } ?>
<td>&nbsp;</td></tr>
<? } ?>
</table>
<?
		}
	}
 }
   else if (page_name == "view_company") {
       var pc1, pc2, google_map_url, geo_link;
       if (company.get("post_code", false) && company.post_code.len() > 0){
    	company.post_code.gmatch("([^%s]+)%s([^%s]+)", function(m1,m2) {pc1=m1;pc2=m2;});
        if (pc1 && pc2){
          google_map_url = format("https://maps.google.co.uk/maps?key=AIzaSyAF53Wut8B5cvaUDY-pfegBhbftJdRHovM&amp;channel=cs&amp;q=%s+%s&amp;ie=UTF8&amp;hq=&amp;hnear=%s+%s,+United+Kingdom&amp;gl=uk&amp;t=m&amp;z=14&amp;source=embed", pc1, pc2, pc1, pc2);
        }
	if (longitude) geo_link = format("geo:%f,%f&z=16", latitude,longitude);
	else  geo_link = "#";
?>

<header><h1 id="results"><?=company.number?> - <?=company.name?></h1></header>
<table class="nj dist">
<tr><td>Address</td><td>
<?=company.address?><br>
<?=company.address2?><br>
<?=company.post_town?><br>
<?=company.county?><br>
</td></tr>
<tr><td>Post code</td><td>
<a href="<?=geo_link?>"><?=company.post_code?></a>
<? if (google_map_url){ ?>
  <small><a href="<?=google_map_url?>" style="color:#0000FF;text-align:right" target="ViewOnMap">View Map</a></small>
<? } ?>
</td></tr>
<tr><td>Status</td><td><?=company.status?></td></tr>
<tr><td>Date of Incorporation</td><td><?=company.incorporation_date?></td></tr>
<tr><td>Country of Origin</td><td><?=company.country_of_origin?></td></tr>
<tr><td>Accounting Reference Date</td><td><?=company.incorporation_date?></td></tr>
<tr><td>Last Accounts Made Up To</td><td><?=company.account_last_made_up_date?></td></tr>
<tr><td>Next Accounts Due</td><td><?=company.account_next_due_date?></td></tr>
<tr><td>Last Return Made Up To</td><td><?=company.returns_last_made_up_date?></td></tr>
<tr><td>Next Return Due</td><td><?=company.returns_next_due_date?></td></tr>
<tr><td>Company Type</td><td><?=company.category?></td></tr>
<tr><td>Nature of Business (SIC)</td><td>
<? foreach(k,v in company_sic_codes){ ?>
<?=v[0]?> - <?=v[1]?><br>
<? } ?>
</td></tr>
<tr><td>Previous Names</td><td>
<? if (this.get("company_old_names", false) && company_old_names.len() > 0){ ?>
<table>
<tr><th>Date of change</th><th>Previous Name</th></tr>
<? foreach(k,v in company_old_names){ ?>
<tr><td><?=v[0]?></td><td><?=v[1]?></td></tr>
<? } ?>
</table>
<? } ?>
</td></tr>
</table>
<? if (this.get("extra_data", false) && extra_data.len() > 0){ ?>
        <table>
          <tr><th>Doc. Type</th><th>Date</th><th>Description</th></tr>
          <? foreach(k,v in extra_data.split('\n')){
              local rec = v.split('|');
          ?>
          <tr><td><?=rec[0]?></td><td><?=rec[1]?></td><td><?=rec[2]?></td></tr>
          <? } ?>
        </table>
<? } ?>

<?
  if (this.get("show_map", false) && pc1 && pc2){
?>
<br />
<div style="margin: 0 auto; width: 425px;">
<iframe width="425" height="350" frameborder="0" scrolling="no" marginheight="0" marginwidth="0"
src="https://maps.google.co.uk/maps?key=AIzaSyAF53Wut8B5cvaUDY-pfegBhbftJdRHovM&amp;channel=cs&amp;q=<?=pc1?>+<?=pc2?>&amp;ie=UTF8&amp;hq=&amp;hnear=<?=pc1?>+<?=pc2?>,+United+Kingdom&amp;gl=uk&amp;t=m&amp;z=14&amp;output=embed"></iframe>
  <br /><small><a href="<?=google_map_url?>" style="color:#0000FF;text-align:left">View Larger Map</a></small>
</div>
<?
	}
   }
} ?>
    </article>
<? if (page_name == "search_results"){ ?>
    <article class="post">
    	<header><h1>Distribution of companies by Account Category</h1></header>
        <table>
          <tr><th>Account Category</th><th>Companies</th><th>%Total</th></tr>
          <tr><td>All Companies</td><td>3,056,018</td><td>100%</td></tr>
          <? foreach(k,v in account_categories){ ?>
          <tr><td><?=v[0]?></td><td><?=v[1]?></td><td><?=v[2]?></td></tr>
          <? } ?>
        </table>
    <!--Div that will hold the pie chart-->
    <div id="chart_div"></div>

        <div class="clearfloat"></div>
    </article>
    <article class="post">
    	<header><h1>Distribution of companies by Company Category</h1></header>
        <table>
          <tr><th>Company Category</th><th>Companies</th><th>%Total</th></tr>
          <tr><td>All Companies</td><td>3,056,018</td><td>100%</td></tr>
          <? foreach(k,v in company_categories){ ?>
          <tr><td><?=v[0]?></td><td><?=v[1]?></td><td><?=v[2]?></td></tr>
          <? } ?>
        </table>
    <!--Div that will hold the pie chart-->
    <div id="chart2_div"></div>
        <div class="clearfloat"></div>
    </article>
    <article class="post">
    	<header><h1>Distribution of companies by Company Status</h1></header>
        <table>
          <tr><th>Company Status</th><th>Companies</th><th>%Total</th></tr>
          <tr><td>All Companies</td><td>3,056,018</td><td>100%</td></tr>
          <? foreach(k,v in company_status){ ?>
          <tr><td><?=v[0]?></td><td><?=v[1]?></td><td><?=v[2]?></td></tr>
          <? } ?>
      </table>
    <!--Div that will hold the pie chart-->
    <div id="chart3_div"></div>
        <div class="clearfloat"></div>
    </article>
    <article class="post">
    	<header><h1>Distribution of companies by Age (years)</h1></header>
        <table>
          <tr><th>Age(years)</th><th>Companies</th><th>%Total</th></tr>
          <tr><td>All Companies</td><td>3,056,018</td><td>100%</td></tr>
          <? foreach(k,v in company_ages){ ?>
          <tr><td><?=v[0]?></td><td><?=v[1]?></td><td><?=v[2]?></td></tr>
          <? } ?>
      </table>
    <!--Div that will hold the pie chart-->
    <div id="chart4_div"></div>
        <div class="clearfloat"></div>

    <!--Div that will hold the pie chart-->
    <div id="chart5_div"></div>
        <div class="clearfloat"></div>
    </article>
<? } ?>
    <section class="sectionFooter">
    	<div class="footerBox">
        	<h2>Free data from <a href="http://www.companieshouse.gov.uk/index.shtml">Companies House</a></h2>
            <p>
            	The Free Public Data Product is a downloadable data snapshot containing basic company data of live companies on the register. This snapshot is provided as ZIP files containing data in CSV format and is split into multiple files for ease of downloading.
            </p>
        </div>
        <div class="footerBox">
        	<h2>Free data from <a href="https://www.ordnancesurvey.co.uk/">Ordnance Survey</a></h2>
            <p>
            	Code-Point Open provides a precise geographic location for each postcode unit in Great Britain. The product is a CSV file containing postcodes, grid references, NHS® health and regional health authority codes, administrative ward, district, county and country area codes.
            </p>
        </div>
        <div class="clearfloat"></div>
    </section>
</div>
<footer>
	<p>
        Copyright &copy; Domingo Alvarez Duarte. All rights reserved. Designed by <a href="http://www.free-responsive-templates.com" title="free responsive templates">Free Responsive Templates</a>, Validation
        <a class="footerLink" href="http://validator.w3.org/check/referer" title="This page validates as HTML5"><abbr title="HyperText Markup Language">HTML5</abbr></a> |
        <a class="footerLink" href="http://jigsaw.w3.org/css-validator/check/referer" title="This page validates as CSS"><abbr title="Cascading Style Sheets">CSS3</abbr></a>
    </p>
</footer>
</body>
</html>
