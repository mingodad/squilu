<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>SquiLuWebAppServer</title>
<link rel="stylesheet" href="/codemirror.css">
<script src="/codemirror-compressed.js"></script>
<link rel="stylesheet" href="/dialogs.css">
<link rel="stylesheet" href="/neat.css">
<link rel="stylesheet" href="/docs.css">
<style type="text/css">
.CodeMirror {border: 1px solid #ccc;}
#editTable {
}
table, th, td
{
	border: 1px solid #ccc;
}
#editTable td {
	vertical-align:top;
}
#filesCell {
}
#editCell {
	width:80%;
}
#editCell textarea {
	width:100%;
	height: 30em;
}
</style>
</head>
<body>
<h3>SquiLu Web App Server</h3>
<form method="post">
<table id="editTable">
<tr><td id="filesCell">
<h4>Files</h4>
<ol>
<? foreach( k,v in files) { ?>
  <li><a href="?file=<?=v?>"><?=v?></a></li>
<? } ?>
</ol>
</td><td id="editCell">
<input type="submit" name="save" value="Save">
<!--<input type="submit" name="reload" value="Reload">-->
<input type="text" name="file_name" value="<?=file_name?>">
<button type="button" onclick="window.location.href='/SQ/logout';">Logout</button>
  <a href="/" target="myweb">View Page</a>
<br>
<textarea name="content" id="code"><?=escapeHtml(content)?></textarea>
<?
	function getMode(){
		if (!file_name) return "xml";
		local ext;
		file_name.gmatch("([^.]+)$", function(m) {ext=m; return false;});
		if (ext == "css")  return "css";
		else if (ext == "js") return "javascript";
		else if (ext == "nut") return "javascript";
		else if (ext == "tpl") return "application/x-ejs";
		else if (ext == "html") return "text/html";
		return "xml";
	}
?>
<p><input type="text" value="<?=getMode()?>" id="mode"> 
  <button type="button" onclick="change()">change mode</button></p>
</td></tr>
</table>
</form>
<script>
      var editor = CodeMirror.fromTextArea(document.getElementById("code"), {
        tabMode: "indent",
        matchBrackets: true,
        lineNumbers: true,
    	mode: "<?=getMode()?>"
      });
var modeInput = document.getElementById("mode");
CodeMirror.connect(modeInput, "keypress", function(e) {
  if (e.keyCode == 13) change();
});
function change() {
   editor.setOption("mode", modeInput.value);
}
</script>
</body>
</html>