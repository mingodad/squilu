dad = {}

function $id(id){return document.getElementById(id);}

dad.detectEnvironment = function () {
  var n = navigator;
  var ua = n.userAgent;
  var av = n.appVersion;
  dad.isOpera = (ua.indexOf("Opera") > -1);
  dad.isKhtml = (av.indexOf("Konqueror") > -1) ||
    (av.indexOf("Safari") > -1);
  dad.isSafari = (av.indexOf("Safari") > -1);
  dad.isMoz = ((ua.indexOf('Gecko') > -1) && (!dad.isKhtml));
  dad.isFF = false;
  dad.isIE = false;
  try {
    if (dad.isMoz) {
      dad.isFF = (ua.indexOf('Firefox') > -1);
    }
    if ((document.all) && (!dad.isOpera)) {
      dad.isIE = (ua.indexOf('MSIE ') > -1);
    }
		if(dad.isIE){
			dad.isIE = (function(){
				var undef, v = 3, div = document.createElement('div'),
					all = div.getElementsByTagName('i');
				while (
						div.innerHTML = '<!--[if gt IE ' + (++v) + ']><i></i><![endif]-->',
						all[0]
				){};
				return v > 4 ? v : undef;
			}());
		}
  }
  // Squelch
  catch(e) {}
  dad.isMac = (ua.indexOf('Mac') > -1);
  dad.isUnix = (ua.indexOf('Linux') > -1) ||
    (ua.indexOf('BSD') > -1) || (ua.indexOf('SunOS') > -1);
  dad.isLinux = (ua.indexOf('Linux') > -1);
  dad.isWindows = (ua.indexOf('Windows') > -1);
};

dad.detectEnvironment();

dad._seqID_ = 0;
dad.newSeqId = function(){return this._seqID_++;}
dad._renewLastSeqId = true;
dad._lastSeqId = false;
dad.newIdBy2calls = function(){
	if(dad._renewLastSeqId)	dad._lastSeqId = 'rb' + dad.newSeqId();
	dad._renewLastSeqId = !dad._renewLastSeqId;
	return dad._lastSeqId;
}

dad.isArray = function(obj) { return Object.prototype.toString.call( obj ) === '[object Array]';}

dad.number_format = function(num, decp){
	if(decp == null) decp = 2;
	var a = new Number(num);
	var b = a.toFixed(decp); //get 12345678.90
	a = parseInt(a); // get 12345678
	b = decp ? (b-a).toPrecision(decp) : 0; //get 0.90
	b = parseFloat(b).toFixed(decp); //in case we get 0.0, we pad it out to 0.00
	a = a.toLocaleString();//put in commas - IE also puts in .00, so we'll get 12,345,678.00
	//if IE (our number ends in .00)
	if(a < 1 && a.lastIndexOf('.00') == (a.length - 3))
	{
			a=a.substr(0, a.length-3); //delete the .00
	}
	return a+b.substr(1);//remove the 0 from b, then return a + b = 12,345,678.90
}

dad.dumpObj = function(obj){
	var result = '';
	for(var key in obj){
		result += key + '\n';
	}
	return result;
}

dad._scriptsIncluded = {}
dad.include = function(jsFile){
	if (dad._scriptsIncluded[jsFile] != null) return;
	var scriptElt = document.createElement('script');
	scriptElt.type = 'text/javascript';
	scriptElt.src = jsFile;
	document.getElementsByTagName('head')[0].appendChild(scriptElt);
	dad._scriptsIncluded[jsFile] = true; // or whatever value your prefer
}

dad.getAsPixels = function(x){ return x + "px";}
dad.getEventSource = function(evt) {
	var target = evt.target ? evt.target : evt.srcElement;
	return (target.nodeType == 3) ? target.parentNode : target;
}
dad.getOffsetX = function(evt) {return evt.changedTouches ? evt.changedTouches[0].offsetX : evt.offsetX || evt.layerX;}
dad.getOffsetY = function(evt) {return evt.changedTouches ? evt.changedTouches[0].offsetY : evt.offsetY || evt.layerY;}
dad.getClientX = function(evt) {return evt.changedTouches ? evt.changedTouches[0].pageX : evt.clientX;}
dad.getClientY = function(evt) {return evt.changedTouches ? evt.changedTouches[0].pageY : evt.clientY;}

dad.getMousePos = function (evt){
	if (evt.targetTouches && evt.targetTouches.length){
		var t = evt.targetTouches[0];
		 return { x: t.pageX, y: t.pageY };
	} else if (evt.pageX || evt.pageY)
		 return { x: evt.pageX, y: evt.pageY };
	else {
		var delm = document.documentElement;
		var bdy = document.body;
		return { x: evt.clientX + delm.scrollLeft - bdy.clientLeft,
							y: evt.clientY + delm.scrollTop  - bdy.clientTop };
	}
}

if( document.documentElement && document.documentElement.scrollTop ){
	dad.getScrolledX = function() {return document.documentElement.scrollLeft;}
	dad.getScrolledY = function() {return document.documentElement.scrollTop;}
} else {
	dad.getScrolledX = function() {return document.body.scrollLeft;}
	dad.getScrolledY = function() {return document.body.scrollTop;}
}
if( document.documentElement && document.documentElement.clientHeight ){
	dad.getClientWidth = function() {return document.documentElement.clientWidth;}
	dad.getClientHeight = function() {return document.documentElement.clientHeight;}
} else {
	dad.getClientWidth = function() {return document.body.clientWidth;}
	dad.getClientHeight = function() {return document.body.clientHeight;}
}
dad.checkEvent = function(evt) {return evt || window.event;}
dad.getEventKey = function(evt){
	var key = (evt.charCode) ? evt.charCode:
		((evt.keyCode) ? evt.keyCode: ((evt.which) ? evt.which : 0));
	return key;
}
dad.trim = function (st) {return st.replace(/^\s+|\s+$/g, '');}
dad.inArray = function (v, a){
for (var i = 0, numA = a.length; i < numA; i++)
	if ((v != null && a[i].constructor == Array && 
			v.constructor == Array && a[i].toString() == v.toString()) || 
			(a[i] == v)) return true;
}

dad.parseJSON = window.JSON ? window.JSON.parse : function(str) {return eval('(' + str + ')')}

dad.arrayApply = function (ary, func) {
	var isFunc = (typeof p == 'function');
	for (var i = 0, numObjs = ary.length; i < numObjs; i++) 
		isFunc ? func(ary[i]) : eval('ary[i].' + func);
};

dad.getHash = function ( url ) {return url.substring( url.lastIndexOf ( '#' ) + 1 );}

//check if the first node is an element node
dad.getFirstChild = function(n){
	x=n.firstChild;
	while (x.nodeType!=1) x=x.nextSibling;
	return x;
}

dad.getFirstChildWithTagName = function( element, tagName ) {
	var cn = element.childNodes;
	for ( var i = 0, cnl = cn.length; i < cnl; i++ ) {
		var elm = cn[i];
		if ( elm.nodeName == tagName ) return elm;
		var found = dad.getFirstChildWithTagName( elm, tagName );
		if( found ) return found;
	}
}

dad.isSpace = function( ch ) {
		return (ch == " ") || (ch == "\t") || (ch == "\n") || (ch == "\r") || (ch == "\v");
}

dad.strHasWord = function(str, word){
	var found = str.indexOf(word);
	while(found >= 0) {
		if(found == 0){
			if( (str.length == word.length) ||
				(dad.isSpace(str[word.length])) ) return true;
		} else {
			if( dad.isSpace(str[found-1]) ) {
				if( (str.length == found+word.length) ||
					(dad.isSpace(str[found+word.length]))) return true;
			}
		}
		found = str.indexOf(word, found+1);
	}
	return false;
}

dad.strRemoveWord = function(str, word){
	var found = str.indexOf(word);
	while(found >= 0) {
		if(found == 0){
			if(str.length == word.length) return "";
			if(dad.isSpace(str[word.length])) return str.substring(word.length+1);
		} else {
			if( dad.isSpace(str[found-1]) ) {
				var lastPos = found+word.length;
				if(str.length == lastPos) {
					return str.substring(0, found-1);
				}
				if(dad.isSpace(str[lastPos])) {
					return str.substring(0, found-1) + str.substring(lastPos);
				}
			}
		}
		found = str.indexOf(word, found+1);
	}
	return str;
}

dad.hasClass = function(elm, className) {
	var elm_className = elm.className;
	return elm_className && dad.strHasWord(elm_className, className);
}

dad.addClass = function(elm, className) {
	if(dad.hasClass(elm, className)) return;
	var elm_className = elm.className;
	if(elm_className) elm.className = elm_className + " " + className;
	else elm.className = className;
}

dad.removeClass = function(elm, className) {
	elm.className = dad.strRemoveWord(elm.className, className);
}

dad.getFirstChildWithClassName = function( element, className ) {
	if(element.childNodes){
		var cn = element.childNodes;
		for ( var i = 0, cnl = cn.length; i < cnl; i++ ) {
			var elm = cn[i];
			if(dad.hasClass(elm, className)) return elm;
	
			var found = dad.getFirstChildWithClassName( elm, className );
			if( found ) return found;
		}
	}
}

dad.getFirstChildWithAttribute = function( element, attrName, attrValue ) {
	var cn = element.childNodes;
	for ( var i = 0, cnl = cn.length; i < cnl; i++ ) {
		var elm = cn[i];
		if ( elm[attrName] && ((attrValue == null) || (elm[attrName] == attrValue))) {
			return elm;
		}
		var found = dad.getFirstChildWithAttribute( elm, attrName, attrValue );
		if( found ) return found;
	}
}

dad.getFirstParentWithClassName = function( element, className ) {
	var pn = element;
	while(pn = pn.parentNode){
		if ( dad.hasClass(pn, className) ) return pn;
	}
}

dad.getFirstParentWithTagName = function( element, tagName ) {
	var pn = element;
	while(pn = pn.parentNode){
		if ( pn.tagName == tagName ) return pn;
	}
}

dad.getFirstParentWithWithAttribute = function( element, attrName ) {
	var pn = element;
	while(pn = pn.parentNode){
		if ( pn[attrName] ) return pn;
	}
}

dad.getChildById = function( element, id , recurse) {
	var cn = element.childNodes;
	for ( var i = 0, cnl = cn.length; i < cnl; i++ ) {
		if ( cn[i].id == id ) return cn[i];
	}
}

dad.resizeImg = function (img, maxWidth){
	maxWidth = maxWidth || 400;
	if (img.width > maxWidth){
		if (img.style.msInterpolationMode) img.style.msInterpolationMode = 'bicubic';
		img.width = maxWidth;
	}
}

dad.initTab = function(tabContainer) {
  var buttons = new Array();
  var table_rows = new Array();
  var tabObj = {
	'buttons': buttons,
	'table_rows': table_rows
  }
  var cn = tabContainer.cells[0].childNodes;
  for ( var i = 0, j = 0, tl = cn.length; i < tl; i++ ) {
		var item = cn[i];
		if ( item.nodeName == "BUTTON" ) {
			var id = item.name;
			buttons[id] = item;
			item.tabObj = tabObj;
		// Assign onclick events to the tab links, and
			item.onclick = dad.showTab;
		// highlight the first tab
			if ( j == 0 ) dad.addClass(item, 'selected');
			var tabContent = $id(id);
			table_rows[id] = tabContent;
			// Hide all content divs except the first
			if ( j > 0 ) dad.addClass(tabContent, 'tabContent hide');
			j++;
		}
  }
}

dad.showTab = function() {
  var selectedId = this.name;
  // Highlight the selected tab, and dim all others.
  // Also show the selected content div, and hide all others.
  var table_rows = this.tabObj.table_rows;
  var buttons = this.tabObj.buttons;
  for ( var id in table_rows ) {
		if ( id == selectedId ) {
			dad.addClass(buttons[id], 'selected');
			dad.removeClass(table_rows[id], 'hide');
		} else {
			dad.removeClass(buttons[id], 'selected');
			dad.addClass(table_rows[id], 'hide');
		}
  }
  // Stop the browser following the link
  return false;
}

dad.addEvent = window.addEventListener ? 
	function(obj,evt,fn) {obj.addEventListener(evt,fn,false);} :
	function(obj,evt,fn) {obj.attachEvent('on'+evt,fn)};

dad.removeEvent = window.removeEventListener ?
	function(obj,evt,fn) {obj.removeEventListener(evt,fn,false);} :
	function(obj,evt,fn) {obj.detachEvent('on'+evt,fn)};
	
dad.cancelEvent = window.addEventListener ? 
	function (e, c) {e.preventDefault(); if (c) e.stopPropagation();}:
	function (e, c) {e.preventDefault ? e.preventDefault() : e.returnValue = false; if (c) e.cancelBubble = true;};

dad.addEventMulti = function(obj, evts, fn){
	var ary = evts.split(' ');
	for(var i in ary) dad.addEvent(obj, ary[i], fn);
}

dad.removeEventMulti = function(obj, evts, fn){
	var ary = evts.split(' ');
	for(var i in ary) dad.removeEvent(obj, ary[i], fn);
}

dad.dragDrop = {
	keySpeed: 10, // pixels per keypress event
	initialMouseX: undefined,
	initialMouseY: undefined,
	startX: undefined,
	startY: undefined,
	dXKeys: undefined,
	dYKeys: undefined,
	startTime: undefined,
	draggedObject: undefined,
	initElement: function (element, dragThisObj, keyboardLink) {
		var dd = dad.dragDrop;
		if (typeof element == 'string') element = $id(element);
		element.onmousedown = dd.startDragMouse;
		element.ontouchstart = dd.startDragMouse;
		element._dragThisObj = dragThisObj;
		if(keyboardLink) {
			if (typeof keyboardLink == 'string')
				keyboardLink = $id(keyboardLink);
			keyboardLink.relatedElement = element;
			keyboardLink.onclick = dd.startDragKeys;
		}
	},
	startDragMouse: function (e) {
		var dd = dad.dragDrop;
		dd.startDrag(this);
		var evt = dad.checkEvent(e);
		var pos = dad.getMousePos(evt);
		dd.initialMouseX = pos.x; //dad.getClientX(evt);
		dd.initialMouseY = pos.y; //dad.getClientY(evt);
		dad.addEventMulti(document,'mousemove touchmove',dd.dragMouse);
		dad.addEventMulti(document,'mouseup touchend',dd.releaseElement);
		return false;
	},
	startDragKeys: function () {
		var dd = dad.dragDrop;
		dd.startDrag(this.relatedElement);
		dd.dXKeys = dd.dYKeys = 0;
		dad.addEvent(document,'keydown',dd.dragKeys);
		dad.addEvent(document,'keypress',dd.switchKeyEvents);
		this.blur();
		return false;
	},
	startDrag: function (obj) {
		var dd = dad.dragDrop;
		if (dd.draggedObject) dd.releaseElement();
		var dragThisObj = obj._dragThisObj ? obj._dragThisObj : obj;
		dad.bringToFront(dragThisObj);
		dd.startTime = new Date().getTime();
		dd.startX = dragThisObj.offsetLeft;
		dd.startY = dragThisObj.offsetTop;
		dd.draggedObject = dragThisObj;
		dad.addClass(obj, 'dragged');
		if(dragThisObj.onStartDragging)  dragThisObj.onStartDragging();
	},
	dragMouse: function (e) {
		var dd = dad.dragDrop;
		var evt = dad.checkEvent(e);
		var pos = dad.getMousePos(evt);
		var dX = pos.x - dd.initialMouseX; //dad.getClientX(evt) - dd.initialMouseX;
		var dY = pos.y - dd.initialMouseY; //dad.getClientY(evt) - dd.initialMouseY;
		dd.setPosition(dX,dY);
		return false;
	},
	dragKeys: function(e) {
		var dd = dad.dragDrop;
		var evt = dad.checkEvent(e);
		var key = evt.keyCode;
		switch (key) {
			case 37:	// left
			case 63234:
				dd.dXKeys -= dd.keySpeed;
				break;
			case 38:	// up
			case 63232:
				dd.dYKeys -= dd.keySpeed;
				break;
			case 39:	// right
			case 63235:
				dd.dXKeys += dd.keySpeed;
				break;
			case 40:	// down
			case 63233:
				dd.dYKeys += dd.keySpeed;
				break;
			case 13: 	// enter
			case 27: 	// escape
				dd.releaseElement();
				return false;
			default:
				return true;
		}
		dd.setPosition(dd.dXKeys,dd.dYKeys);
		if (evt.preventDefault)
			evt.preventDefault();
		return false;
	},
	setPosition: function (dx,dy) {
		var dd = dad.dragDrop;
		var style = dd.draggedObject.style;
		var xpos = dd.startX + dx;
		if(xpos < 0) xpos = 0;
		var ypos = dd.startY + dy;
		if(ypos < 0) ypos = 0;
		style.left = dad.getAsPixels(xpos);
		style.top = dad.getAsPixels(ypos);
		dd.updatePending = false;
	},
	switchKeyEvents: function () {
		// for Opera and Safari 1.3
		var dd = dad.dragDrop;
		dad.removeEventMulti(document,'keydown keypress',dd.dragKeys);
		dad.removeEvent(document,'keypress',dd.switchKeyEvents);
	},
	releaseElement: function() {
		var dd = dad.dragDrop;
		var removeEvent = dad.removeEvent;
		var removeEventMulti = dad.removeEventMulti;
		removeEventMulti(document,'mousemove touchmove',dd.dragMouse);
		removeEventMulti(document,'mouseup touchend',dd.releaseElement);
		removeEventMulti(document,'keypress keydown',dd.dragKeys);
		removeEvent(document,'keypress',dd.switchKeyEvents);
		var dobj = dd.draggedObject;
		dad.removeClass(dobj, 'dragged');
		if(dobj.onStopDragging)  dobj.onStopDragging();
		dd.draggedObject = null;
	}
}

dad.newAjaxRequest = function(){
 var activexmodes=["Msxml2.XMLHTTP", "Microsoft.XMLHTTP"] //activeX versions to check for in IE
 if (window.ActiveXObject){ //Test for support for ActiveXObject in IE first (as XMLHttpRequest in IE7 is broken)
  for (var i=0; i<activexmodes.length; i++){
   try{
    return new ActiveXObject(activexmodes[i])
   }
   catch(e){
    //suppress error
   }
  }
 }
 else if (window.XMLHttpRequest) // if Mozilla, Safari etc
  return new XMLHttpRequest()
 else
  return false
}

dad.Ajax = function(callback, params, cache)
{
   var thisObj   = this;
   this.cache    = {};
   this.callback = callback;
   this.request = dad.newAjaxRequest();

   this.runCallback = function (event, cached)
   {
      if (!cached)
         {
            if (thisObj.request.readyState != 4)
               return;
            thisObj.responseText            = thisObj.request.responseText;
            thisObj.responseXML             = thisObj.request.responseXML;
            thisObj.status                  = thisObj.request.status;
            thisObj.cache[thisObj.cacheKey] = thisObj.responseText;
         }

      if(thisObj.callback) thisObj.callback(params);
   }


   this.post = function (where, what, verb, headers)
   {
      var cachedValue;
      this.cacheKey = where + '?' + what;
      if (cache && ((cachedValue = this.cache[this.cacheKey]) != null))
         {
            this.responseText = cachedValue;
            this.runCallback('', true);
            return;
         }

	  /*IE needs a fresh new ajax obj each time*/
	  if(dad.isIE) this.request = dad.newAjaxRequest(); 
      this.request.onreadystatechange = this.runCallback;

      verb = verb ? dad.trim(((verb == true) ? 'GET' : verb).toUpperCase()) : 'POST';
      this.request.open(verb, dad.inArray(verb, ['POST', 'PUT']) ? where : where + (what ? '?' + what : ''), true);

      if (dad.inArray(verb, ['POST', 'PUT']))
         {
            this.request.setRequestHeader('Content-Type',   'application/x-www-form-urlencoded; charset=utf-8');
            this.request.setRequestHeader('Content-length', what.length);
            this.request.setRequestHeader('Connection',     'close');
         }
      if (headers)
         for (var i = 0, len = headers.length; i < len; i++)
            this.request.setRequestHeader(headers[i][0], headers[i][1]);

      this.request.send(dad.inArray(verb, ['POST', 'PUT']) ? what : null);
   }
   return this;
}

dad._topmost_zindex = false;

dad.find_topmost_zindex = function() {
  var current_top = 0;
  var page_elements = null;
  
  if(document.getElementsByTagName) {
    page_elements = document.body.getElementsByTagName('div');
  } else if(document.all) {
    page_elements = document.all;
  } else { return false; }
 
  for(var i=0;i<page_elements.length;i++) {
	var elm = page_elements[i];
    if(elm.style.zIndex){
	  var zIndex = parseInt(elm.style.zIndex);
      if(zIndex>current_top) {
		current_top = zIndex;
	  }
	}
  }
  return current_top;
} 

dad.bringToFront = function(el){
	if (typeof el == 'string')
			el = $id(el);
	if(el != dad._topmost_zindex){
		el.style.zIndex = dad.find_topmost_zindex()+1;
		dad._topmost_zindex = el;
	}
	if(el.style.display == 'none') el.style.display = "block";
}

dad.bringToFrontParent = function(event){
	var evt = dad.checkEvent(event);
	var el = dad.getEventSource(evt);
	dad.bringToFront(el.parentNode);
}

dad.getStyle = function(el,styleProp){
	if (el.currentStyle)
		var y = el.currentStyle[styleProp];
	else if (window.getComputedStyle)
		var y = document.defaultView.getComputedStyle(el,null).getPropertyValue(styleProp);
	return y;
}

dad._winOnAfterResize = function(){
/*
	var child = this.firstChild;
	if(child){
		var style = this.style;
		var border = 2; //parseInt(style.borderWidth);
		var newLeft = parseInt(style.left)+border;
		var newTop = parseInt(style.top)+border;
		var newWidth = parseInt(style.width)-border;
		var newHeight = parseInt(style.height)-border;
		//style = child.style;
		var AsPixels = dad.getAsPixels;
		//child.left = AsPixels(newLeft);
		//child.top = AsPixels(newTop);
		//child.width = AsPixels(newWidth);
		//child.height = AsPixels(newHeight);
	}
*/
	if(this._onAfterResizeList){
		for(id in this._onAfterResizeList){
			this._onAfterResizeList[id]();
		}
	}
}

dad.addWinOnAfterResize = function(win, id, func){
	if(!win._onAfterResizeList) win._onAfterResizeList = {};
	win._onAfterResizeList[id] = func;
}

dad._windows = {}

dad.closeTopParent = function (child){
	var topParent = dad.getWindowForChild(child);
	if(topParent) {
		for(win in dad._windows){
			if(topParent == dad._windows[win]) {
				if(!dad.hasClass(topParent, "mainWindow")){
					dad._windows[win] = null;
					document.body.removeChild(topParent);
				}
				break;
			}
		}
	}
	return false;
}

dad.hideWindow = function (win){
	if(win && (!dad.hasClass(win, "mainWindow"))){
		win.style.display="none";
	}
	return false;
}

dad.hideTopParent = function (child){
	var topParent = dad.getWindowForChild(child);
	dad.hideWindow(topParent);
	return false;
}

dad.centerOnScreen = function(win){
	// Xwidth is the width of the div, Yheight is the height of the
	// div passed as arguments to the function:
	var leftOffset = dad.getScrolledX() + (dad.getClientWidth() - win.offsetWidth) / 2;
	var topOffset = dad.getScrolledY() + (dad.getClientHeight() - win.offsetHeight) / 2;
	// The initial width and height of the div can be set in the
	// style sheet with display:none; divid is passed as an argument to // the function
	var style = win.style;
	if(topOffset < 0) topOffset = 0;
	if(leftOffset < 0) leftOffset = 0;
	style.top = topOffset + 'px';
	style.left = leftOffset + 'px';
}

Jaml.register('Window', function(args) {
		var AsPixels = dad.getAsPixels;
		var swidth = "";
		if(args.x > 0) swidth += "left:" + AsPixels(args.x) + ";";
		if(args.y > 0) swidth += "top:" + AsPixels(args.y) + ";";
		if(args.w > 0) swidth += "width:" + AsPixels(args.w) + ";";
		if(args.h > 0) swidth += "height:" + AsPixels(args.h) + ";";
		
		var winOptClass = args.noWinOpt ? "noWinOpt" : "winOpt";
		
		div({
				style: swidth,
				"class":"resizeable",
				onAfterResize: "dad._winOnAfterResize"
			},
			table({
					style:"width:100%;height:100%;cursor:default;",
					"class": "window"
				},
				tr({"class":"winTitle"},
					td({"class": "winHeader"}, args.title),
					td({cls: winOptClass}, a({href:"#",  onclick: "return dad.hideTopParent(this)"}, "_")),
					td({cls: winOptClass}, a({href:"#"}, "^")),
					td({cls: winOptClass}, a({href:"#",  onclick: "return dad.closeTopParent(this)"}, "X"))
				),
				tr({"class":"winContent"},
					td({colspan:4},
						div({id:"divContent" + args.id, style:"width:100%;height:100%;"},
							args.content
						)
					)
				)
			)
		)
	});

Jaml.register('Window2', function(args) {
		var AsPixels = dad.getAsPixels;
		var swidth = "";
		if(args.x > 0) swidth += "left:" + AsPixels(args.x) + ";";
		if(args.y > 0) swidth += "top:" + AsPixels(args.y) + ";";
		if(args.w > 0) swidth += "width:" + AsPixels(args.w) + ";";
		if(args.h > 0) swidth += "height:" + AsPixels(args.h) + ";";
		
		var winOptClass = args.noWinOpt ? "noWinOpt" : "winOpt";
		
		div({
				style: swidth,
				"class":"resizeable window",
				onAfterResize: "dad._winOnAfterResize"
			},
			div({style: "height:1.5em;", "class":"winTitle winHeader"},
				args.title,
				a({href:"#",  onclick: "return dad.closeTopParent(this)", cls: winOptClass}, "X"),
				a({href:"#", cls: winOptClass}, "^"),
				a({href:"#",  onclick: "return dad.hideTopParent(this)", cls: winOptClass}, "_")
			),
			div({style:"position:absolute;top:1.6em;bottom:0.1em;left:0.1em;right:0.1em", "class":"winContent", id:"divContent" + args.id},
				args.content
			)
		)
	});
	
dad.setOpacity = function setOpacity(obj, value) {
	obj.style.opacity = value/10;
	obj.style.filter = 'alpha(opacity=' + value*10 + ')';
}

dad.newWindow = function(id,x,y,w,h,title, content, noWinOpt, noCenter){
	var args = {id:id, x: x, y: y, w: w, h: h, title: title, content: content, noWinOpt: noWinOpt};
	var swin = Jaml.render("Window", args);
	var elm = document.createElement('div');
	elm.innerHTML = swin;
	document.body.appendChild(elm);
	var win = elm.firstChild;
	win.onAfterResize = dad._winOnAfterResize;
	elm.parentNode.replaceChild(win, elm);
	win = document.body.lastChild;
	win.style.position = "absolute";

	win.ud = {};
	win.ud.header = dad.getFirstChildWithClassName(win, "winHeader");
	win.ud.content = dad.getFirstChildWithClassName(win, "winContent");
	//set draggable
	dad.dragDrop.initElement(win.ud.header, win);
	win.onStartDragging = function(){
		dad.setOpacity(win, 7);
		win.ud.content.style.visibility="hidden"
	}
	win.onStopDragging = function(){
		dad.setOpacity(win, 10);
		win.ud.content.style.visibility="visible"
	}
	dad._windows[title] = win;
	if(!noCenter) dad.centerOnScreen(win);
	return win;
}

dad.setContentOverflow = function(id, value){
	var divContent = $id("divContent" + id);
	divContent.style.overflow = value || "auto";
}

dad.isFocusableTag = function(tag, extra_tags){
	var focusabeTags = ['INPUT', 'SELECT', "BUTTON", "SELECT"];
	for(var i in focusabeTags){
		if(tag == focusabeTags[i]) return true;
	}
	if(extra_tags){
		for(var i in extra_tags){
			if(tag == extra_tags[i]) return true;
		}
	}
	return false;
}

dad.formGetNextFormFocusableElement = function(elm, backward){
	var result = null;
	var startChecking = false;
	var form = elm.form;
	if(backward){
		for(var x=form.length-1; x >= 0; x--){
			var curr_elm = form[x];
			if(startChecking){
				if(dad.isFocusableTag(curr_elm.tagName)){
					result = curr_elm;
					break;
				}
			} else if(curr_elm == elm) startChecking = true;
		}
	} else {
		for(var x=0, len=form.length; x < len; x++){
			var curr_elm = form[x];
			if(startChecking){
				if(dad.isFocusableTag(curr_elm.tagName)){
					result = curr_elm;
					break;
				}
			} else if(curr_elm == elm) startChecking = true;
		}
	}
	return result;
}

dad.formFocusNext = function(elm, backward){
	var nextElm = dad.formGetNextFormFocusableElement(elm, backward);
	if(nextElm) nextElm.focus();
	return nextElm;
}

dad.formFocusNextFromEvent = function(evt, backward){
	var nextElm = dad.formGetNextFormFocusableElement(dad.getEventSource(evt), backward);
	if(nextElm) nextElm.focus();
	return nextElm;
}

dad.formInputTextOnKeydown = function(event){
	var evt = dad.checkEvent(event);
	//alert(dad.getEventSource(evt).name);
	if (evt.altKey || evt.ctrlKey || evt.shiftKey) return true;
	switch(dad.getEventKey(evt)){
		case 40: //arrow down
			//if(!o.editable || (o.editable && !ed.activeCellEditor && !ed.activeRow)) navigate('down');
			dad.formFocusNextFromEvent(evt);
			return false;
		break;
		case 38: //arrow up
			//if(!o.editable || (o.editable && !ed.activeCellEditor && !ed.activeRow)) navigate('up');
			dad.formFocusNextFromEvent(evt, true);
			return false;
		break;
		case 37: //arrow left
			//if(!o.editable || (o.editable && !ed.activeCellEditor && !ed.activeRow)) navigate('left');
		break;
		case 39: //arrow right
			//if(!o.editable || (o.editable && !ed.activeCellEditor) && !ed.activeRow) navigate('right');
		break;
		case 34: //pagedown
			//navigate('pgdown');
		break;
		case 33: //pageup
			//navigate('pgup');
		break;
		case 36: //home
			//navigate('home');
		break;
		case 35: //end
			//navigate('end');
		break;
		case 9: //tab
			//if(e.shiftKey) navigate('left');
			//else navigate('right');
		break;
		case 13: //enter
			//if(!o.onValidateRow && !o.onValidateCell) navigate('down');
			if(dad.formFocusNextFromEvent(evt)) dad.cancelEvent(evt, true);
		break;
		case 113: //F2
		case 32: //space bar
			//if(o.editable && (!ed.activeCellEditor)){ ed.Edit(e); o.Event.Cancel(e); }
		break;
		case 27: //escape
		break;
	}
}	

dad.getTableForTableChild = function(child){
	return dad.getFirstParentWithTagName(child, 'TABLE');
}

dad.createTable = function(header){
	var table = document.createElement('table');
	var th = table.createTHead();
	var tr = th.insertRow(-1);
	tr.className = "theader";
	var len = header.length;
	for(var i=0; i < len; i++){
		tr.insertCell(-1).innerHTML = header[i];
	}
	return table;
}

dad.tableAddRow = function(table, row){
	var tr = table.insertRow(-1);
	var len = row.length;
	for(var i=0; i < len; i++){
		tr.insertCell(-1).innerHTML = row[i];
	}
}

dad.tableAddRows = function(table, rows, cellClass){
	var rlen = rows.length;
	for(var x=0; x < rlen; x++){
		dad.tableAddRow(table, rows[x]);
	}
	if(rows.length > 1) {
		for(var i=rows.length-1; i > 0; i--){
			//table.deleteRow(i);
			rows[i].display = 'none';
		}
	}
}

dad.onTableClick = function(table){
	alert(table.rowIndex);
}

dad.onTableRowClick = function(){
	alert(this.rowIndex);
	return false;
}

dad.appLogShow = false;

dad.showDebug = function(str){ if(this.appLogShow) this.appLogShow.innerHTML = str; }

dad.RESIZE_DIR = {
	north: 0x01,
	south: 0x02,
	east: 0x04,
	west: 0x08
}
var rd = dad.RESIZE_DIR;
dad.CursorResizeDirection = new Array(rd.west+1);
var crd = dad.CursorResizeDirection;
crd[rd.north] = "n-resize";
crd[rd.south] = "s-resize";
crd[rd.east] = "e-resize";
crd[rd.west] = "w-resize";
crd[rd.north | rd.east] = "ne-resize";
crd[rd.south | rd.east] = "se-resize";
crd[rd.north | rd.west] = "nw-resize";
crd[rd.south | rd.west] = "sw-resize";

//Find out what kind of resize! Return a string inlcluding the directions
dad.getResizeDirection = function(el, evt) {
	var xPos, yPos, offset, dir;
	dir = 0;

	xPos = dad.getOffsetX(evt);
	yPos = dad.getOffsetY(evt);

	offset = 8; //The distance from the edge in pixels

	if (yPos<offset) dir |= dad.RESIZE_DIR.north;
	else if (yPos > el.offsetHeight-offset) dir |= dad.RESIZE_DIR.south;
	if (xPos<offset) dir |= dad.RESIZE_DIR.west;
	else if (xPos > el.offsetWidth-offset) dir |= dad.RESIZE_DIR.east;

	return dir;
}

dad._resizingObject = null;
dad.getIsCalssResizeable = function(el){
	return dad.hasClass(el, "resizeable");
}

dad.getWindowForChild = function(child){
	return topParent = dad.getFirstParentWithClassName(child, "resizeable");
}

dad.checkIsTop = function(el){
	var topParent = dad.getWindowForChild(el);
	var result = topParent == dad._topmost_zindex;
	if(topParent) dad.bringToFront(topParent);
	return result;
}

dad._nonChar = false;
dad.handleNonChar = function(vchar){}
dad.handleChar = function(vchar){}
	 
dad.handleKeyboard = function(e) {
    var vchar;
    var evt = (e) ? e : window.event;       //IE reports window.event not arg
    if (evt.type == "keydown") {
        vchar = evt.keycode;
        if (vchar < 16 ||                    // non printables
            (vchar > 16 && vchar < 32) ||     // avoid shift
            (vchar > 32 && vchar < 41) ||     // navigation keys
            vchar == 46) {                   // Delete Key (Add to these if you need)
            dad.handleNonChar(vchar);            // function to handle non Characters
            dad.nonChar = true;
        } else
            dad.nonChar = false;
    } else {                                // This is keypress
        if (dad.nonChar) return;                // Already Handled on keydown
        vchar = (evt.charCode) ?
                   evt.charCode : evt.keyCode;
        if (vchar > 31 && vchar < 256)        // safari and opera
            dad.handleChar(vchar);               //
    }
    if (e)                                  // Non IE
        Event.stop(evt);                    // Using prototype
    else if (evt.keyCode == 8)              // Catch IE backspace
        evt.returnValue = false;            // and stop it!
}

dad.appOnKeyDown = function(event) {
	if(event.keyCode==27) {
		//ESC key hide windows
		if(dad._topmost_zindex) {
			var win = dad._topmost_zindex;
			dad.hideWindow(win);
			if(win.ud.caller) {
				dad._topmost_zindex = win.ud.caller;
				win.ud.caller = null;
			}
		}
		return false;
	}
}

dad.appOnMouseDown = function(event) {
	var evt = dad.checkEvent(event);
	var el = dad.getEventSource(evt);
	
	if (el == null) {
		dad._resizingObject = null;
		return;
	}		
	if(dad.getIsCalssResizeable(el)){
		var dir = dad.getResizeDirection(el, evt);
		if (dir == "") return;
		dad._resizingObject = {};
		var _resizingObject = dad._resizingObject;
		
		_resizingObject.el = el;
		_resizingObject.dir = dir;

		_resizingObject.grabx = dad.getClientX(evt);
		_resizingObject.graby = dad.getClientY(evt);
		_resizingObject.width = el.offsetWidth;
		_resizingObject.height = el.offsetHeight;
		_resizingObject.left = el.offsetLeft;
		_resizingObject.top = el.offsetTop;

		dad.cancelEvent(evt, true);
	} else 	{
		if(!dad.checkIsTop(el)) dad.cancelEvent(evt, true);
	}
}

dad.appOnMouseUp = function() {
	if (dad._resizingObject != null) {
		var el = dad._resizingObject.el;
		if(el.onAfterResize) el.onAfterResize();
		dad._resizingObject = null;
	}
}

dad.appOnMouseMove = function(event){
	var evt = dad.checkEvent(event);
	var el = dad.getEventSource(evt);
	//Dragging starts here

	if(dad._resizingObject != null) {
		var xMin = 8; //The smallest width possible
		var yMin = 8; //             height
		var obj = dad._resizingObject;
		el = obj.el;
		var dir = obj.dir;
		var rd = dad.RESIZE_DIR;
		var mousePos = dad.getMousePos(evt);

		if (dir & rd.east)
			el.style.width = Math.max(xMin, obj.width + mousePos.x - obj.grabx) + "px";
	
		if (dir & rd.south)
			el.style.height = Math.max(yMin, obj.height + mousePos.y - obj.graby) + "px";

		if (dir & rd.west) {
			var leftPos = Math.min(obj.left + mousePos.x - obj.grabx, obj.left + obj.width - xMin);
			if(leftPos < 0) leftPos = 0;
			el.style.left = leftPos + "px";
			el.style.width = Math.max(xMin, obj.width - mousePos.x + obj.grabx) + "px";
		}
		if (dir & rd.north) {
			var topPos = Math.min(obj.top + mousePos.y - obj.graby, obj.top + obj.height - yMin);
			if(topPos < 0) topPos = 0;
			el.style.top = topPos + "px";
			el.style.height = Math.max(yMin, obj.height - mousePos.y + obj.graby) + "px";
		}
		dad.cancelEvent(evt, true);
	} else {
		if(el && dad.getIsCalssResizeable(el)){
			var dir = dad.CursorResizeDirection[dad.getResizeDirection(el, evt)];
			if(dir) el.style.cursor = dir;
		}
	}
}

var _transalations_ = {}

function _tr(str){
	var val = _transalations_[str];
	return val ? val : str;
}

Jaml.register('ActionSelect', function(actionOnChange) {
	select({name: "formAction", onchange: "dad.actionSelectOnActionChange(this)"},
		option({value: "insert"}, _tr("Insert")),
		option({value: "update"}, _tr("Update")),
		option({value: "refresh"}, _tr("Refresh")),
		option({value: "delete"}, _tr("Delete"))
	);
});

dad.actionSelectOnActionChange = function (select) {
	var fields = select.form.elements;
	fields.btnAction.value = select.options[select.selectedIndex].text;
}

dad.fillFormField = function(fld, fld_value){
		if(fld.type){
			if(fld.type == 'checkbox')
				fld.checked = fld_value == 1;
			else if(fld.type == 'text')
				fld.value = fld_value || '';
			else if(fld.type == 'textarea')
				fld.innerHTML = fld_value || '';
			else if(fld.type == 'select-one'){
				for(var i=0; i < fld.options.length; ++i){
					var opt = fld.options[i];
					opt.selected = opt.value == fld_value;
				}
			}
		}
}

dad.fillFormWithExistingFields = function(form, record){
	var prefix = form.my_field_prefix;
	for(var name in record){
		var fld = form[prefix + name];
		if(fld) dad.fillFormField(fld, record[name]);
	}
}

dad.getImageForImg = function(img, img_id, thumbnail){
	if(img_id && img_id !== ""){
		img.src="/DB/GetBin?image=" + img_id;
		if(thumbnail) img.src += "&thumbnail=1";
		img.style.visibility="visible";
	} else img.style.visibility="hidden";
}

dad.formFillByRecord = function(form, record, prefix){
	var prefix_len = prefix ? prefix.length : 0;
	var fields = form.elements;
	for (i=0; i < fields.length; i++){
		var fld = fields[i];
		if(!fld.name) continue;
		if(prefix_len && fld.name.indexOf(prefix) != 0) continue;
		var fld_name = prefix_len ? fld.name.substring(prefix_len) : fld.name;
		var fld_value = record[fld_name];
		if(form.onFillForm && form.onFillForm(fld, fld_value)) continue;
		dad.fillFormField(fld, fld_value);
	}
}

dad.formAjaxLoadResponse = function(id){
	if(this.status == 200){
		var form = $id('form' + id);
		var result_array;
		if(form.processResponse){
			result_array = form.processResponse(this.responseText);
		}
		else  result_array = dad.parseSLEData(this.responseText, true);
		
		var record = {};
		if(result_array.length > 1){
			var names = result_array[0];
			var values = result_array[1];
			for(var i in names){
				record[names[i]] = values[i]; 
			}
		}
		form._dbrecord = record;
		var fields = form.elements;
		dad.formFillByRecord(form, record, form.my_field_prefix);
		if(form.afterFill) form.afterFill();
		var formAction = fields.formAction;
		if(formAction && (formAction.value != 'update')){
			formAction.value = 'update';
			formAction.onchange();
		}
	} else {
		alert("An error has occured making the request");
	}
}

dad.stringBytesLength = function (str) {
  str = str.toString();
	var i = str.length, len = 0, ch;
	while (i--) {
			ch = str.charCodeAt(i);
			if (ch <= 0x007F) len += 1;
			else if (ch <= 0x07FF) len += 2;
			else if (ch <= 0xFFFF) len += 3;
			else if (ch <= 0x10FFFF) len += 4;
			else throw new Error("Bad Charcode: " + ch);
	}
	return len;
}

dad.encodeSizeString = function(str){
	return dad.stringBytesLength(str) + ':' + str;
}

dad.getFormDataWithPrefix = function(form, prefix, doNotCheckOriginal) {
	var prefix_len = prefix ? prefix.length : 0;
	var fields = form.elements;
	var values = [];
	var original = form._dbrecord;
	for(var i=0; i < form.length; i++){
		var fld = fields[i];
		if(!fld.name) continue;
		var fld_value;
		if(fld.type == "text") {
			fld_value = fld.value;
		}
		else if(fld.type == "checkbox") {
			if(fld.checked) fld_value = 1;
			else fld_value = 0;
		}
		else if(fld.type == "radio"){
			if(!fld.checked) continue;
		}
		else if(fld.options && (fld.selectedIndex >= 0)) {
			fld_value = fld.options[fld.selectedIndex].value;
		}
		else fld_value = fld.value;
		if(prefix_len && fld.name.indexOf(prefix) != 0) continue;
		var fld_name = prefix_len ? fld.name.substring(prefix_len) : fld.name;
		if(original && !doNotCheckOriginal){
			if(original[fld_name] == fld_value) continue;
			if(fld_value == "" && original[fld_name] == null) continue;
		}
		values.push(encodeURIComponent(fld_name) + '=' + encodeURIComponent(fld_value));
	}
	return values;
}

dad.getFormData = function(form) {
	return dad.getFormDataWithPrefix(form, form.my_field_prefix);
}

dad.formOnSubmit = function(EditWindowRefresh, form, table) {
	var fields = form.elements;
	var action = fields.formAction.value;
	if(action == 'refresh'){
		EditWindowRefresh(this.ajax);
		return false;
	}
	var values = dad.getFormDataWithPrefix(form, form.my_field_prefix);
	if(values.length == 0 && action != "delete") return;
	var original = form._dbrecord;
	values.push(encodeURIComponent("__table__") + '=' + encodeURIComponent(table));
	if(action != "insert"){
		values.push(encodeURIComponent("__id__") + '=' + encodeURIComponent(original["id"]));
		values.push(encodeURIComponent("__version__") + '=' + encodeURIComponent(original["_version_"]));
	}
	values.push(encodeURIComponent("__action__") + '=' + encodeURIComponent(action));
	var ajax = form.ajaxSubmit ? form.ajaxSubmit : new dad.Ajax(null, null, false);
	ajax.post(form.action, values.join('&'), 'POST', null);
	//form.submit();
	return false;
}

dad.listEditWindowOnSubmitRespose = function (id) {
	if(this.status == 200){
		//retrieve result as an JavaScript object
		var myform = $id('form' +id);
		var record = dad.parseSLEData2Object(this.responseText);
		if(record.changes == 1){
			myform._dbrecord._version_++;
		}
		var mywin = dad.getWindowForChild(myform);
		if(mywin.ud.WindowRefresh)
			mywin.ud.WindowRefresh(mywin.ud.ajaxTable);
	} else {
		alert("An error has occured making the request");
	}
}

dad.setupEditForm = function(myform){
	for(var x=0, len=myform.length; x < len; x++){
		var elm = myform[x];
		if(dad.isFocusableTag(elm.tagName)) elm.onkeydown = dad.formInputTextOnKeydown;
	}
}

dad.setupKeyboardNavigation = function(node, extra_tags){
	var cn = node.childNodes;
	for ( var i = 0, cnl = cn.length; i < cnl; i++ ) {
		var elm = cn[i];
		if(dad.isFocusableTag(elm.tagName, extra_tags))
			elm.onkeydown = dad.formInputTextOnKeydown;
		dad.setupKeyboardNavigation( elm , extra_tags);
	}
}

dad.onLoadInit = function(){	
	dad.appLogShow = $id("logShow");
	dad.addEventMulti(document, 'mousedown', dad.appOnMouseDown);
	dad.addEventMulti(document, 'mousemove', dad.appOnMouseMove);
	dad.addEventMulti(document, 'mouseup', dad.appOnMouseUp);
	dad.addEvent(document, "keydown", dad.appOnKeyDown);
}

dad.loadScript = function(scriptName) {
	if(!self.myScripts) self.myScripts = [];
	if (self.myScripts[scriptName]) { // Already exists
		return;
	}
	var head = document.getElementsByTagName("head")[0];
	script = document.createElement('script');
	script.type = 'text/javascript';
	script.src = scriptName;
	head.appendChild(script);
	self.myScripts[scriptName] = true;
}

dad._getTableAndHeaderKey = function(id){
	var keys = {};
	if(id.id) {
		keys.table_key = id.table;
		keys.table_header_key = id.table_header;
	} else {
		keys.table_key = 'table' + id;
		keys.table_header_key = 'table_header' + id;
	}
	return keys;
}

dad.AjaxDataTableResizeHeader = function(id) {
	var keys = dad._getTableAndHeaderKey(id);
	var table_src = $id(keys.table_key);
	var table_dest = $id(keys.table_header_key);
	var th0 = table_dest.tHead.rows[0].cells;
	var th1_rows = table_src.tFoot.rows[0];
	var th1 = th1_rows.cells;

	var col_count = th0.length-1;
	for(var i=col_count; i >= 0 && (dad.hasClass(th0[i], "display_none")); --i, --col_count){};
	for(var i=0; i < col_count; ++i){
		if(dad.hasClass(th0[i], "display_none")) continue;
		th0[i].style.width = (th1[i].offsetWidth -3) +'px';
	}
}

dad.tableInputOnKeydown = function(event){
	var evt = dad.checkEvent(event);
	if (evt.altKey || evt.ctrlKey || evt.shiftKey) return true;
	var key = dad.getEventKey(evt);
	switch(key){
		case 40: //arrow down
			dad.formFocusNextFromEvent(evt);
			return false;
		break;
		case 38: //arrow up
			dad.formFocusNextFromEvent(evt, true);
			return false;
		break;
		case 37: //arrow left
		case 39: //arrow right
			var elm = dad.getEventSource(evt);
			elm = dad.getFirstParentWithTagName( this, "TABLE" );
			if(key == 37 && elm.myPrevFocus){
				elm.myPrevFocus.focus();
				return false;
			}
			if(key == 39 && elm.myNextFocus){
				elm.myNextFocus.focus();
				return false;
			}
		break;
		case 45: //insert
		case 46: //delete
			var tbl = dad.getFirstParentWithTagName( this, "TABLE" );
			if(key == 45 && tbl._onInsert_cb) tbl._onInsert_cb(this);
			else if(key == 46 && tbl._onDelete_cb) tbl._onDelete_cb(this);
			return false;
		break;
		case 32: //space bar
			this.checked = !this.checked;
			return false;
		break;
		case 13: //enter
			var tr = dad.getFirstParentWithTagName( this, "TR" );
			//alert(tr.cells[1].innerHTML);
			if(tr.onclick) tr.onclick(event);
			else if(tr.ondblclick) tr.ondblclick(event);
			return false;
		break;
	}
}

dad.tableInputOnFocus = function(){
	var tr = dad.getFirstParentWithTagName( this, "TR" );
	this.tr_color = tr.style.backgroundColor;
	tr.style.backgroundColor = "blue";
}

dad.tableInputOnBlur = function(){
	var tr = dad.getFirstParentWithTagName( this, "TR" );
	tr.style.backgroundColor = this.tr_color;
}

dad.localeDecimalSeparator = (1.1).toLocaleString().substring(1, 2);

dad.formatCurrency = function(v, number_of_decimals, decimal_separator, currency_sign){
	if(number_of_decimals == null) number_of_decimals = 2;
	if(currency_sign == null) currency_sign = "";
	if(decimal_separator == null) decimal_separator = dad.localeDecimalSeparator;

  return (isNaN(v)? v : parseInt(v||0).toLocaleString() + 
		(decimal_separator) + 
		(v*1).toFixed(number_of_decimals).slice(-number_of_decimals)) + 
		currency_sign;
}

dad.formatNumeric = function(v){
	return isNaN(v)? v : parseFloat(v);
}

dad.formatBoolean = function(v){
	return v == "1" ? "+" : "";
}

dad.fillTableById = function(records, id){
	var keys = dad._getTableAndHeaderKey(id);
	var table = $id(keys.table_key);
	table.style.display = "none";
	var header_title_col = 1
	var record_header = table.my_record_header;
	if(!record_header) {
		record_header = records[0];
		var col = record_header[0];
		header_title_col = (col && col.indexOf('|') > 0) ? 1 : 0;
	}
	var col_count = record_header.length;				
	
	var row_over_cb = table.row_over_cb;
	var row_click_cb = table.row_click_cb;
	var row_dblclick_cb = table.row_dblclick_cb;
	var first_row_click_cb = table.first_row_click_cb;
	var selection_click_cb = table.selection_click_cb;
	var th_row = table.tFoot.rows[0];
	/*if(!dad.isFF)*/ th_row.style.visibility = "hidden";
	var th_cells = th_row.cells;
	var tBody = table.tBodies[0];
	try { 
		tBody.innerHTML = "";
	} catch(e) {
		var rows = tBody.rows;
		for(var i=rows.length-1; i >= 0; --i){
			table.deleteRow(i);
		}
	}
	var rows = tBody.rows;

	var table_header = $id(keys.table_header_key);
	var th_header_row = table_header.tHead.rows[0];
	var th_cells_header = th_header_row.cells;
	
	//setting the header
	//header, width, align, format, color, bgcolor
	for(var i = th_cells.length-1; i > 0; --i){
		th_row.removeChild(th_cells[i]);
		th_header_row.removeChild(th_cells_header[i]);
	}
	var myColsAlign = new Array(col_count);
	var myColsFormat = new Array(col_count);
	for(var i=0; i<col_count; ++i){
		var ar = record_header[i].split("|");
		var new_th=document.createElement('TH');
		new_th.innerHTML = ar[header_title_col] || "?";
		var align = ar[3];
		if(align){
			switch(align){
				case "C": myColsAlign[i] =  "center"; break;
				case "R": myColsAlign[i] =  "right"; break;
				default: myColsAlign[i] = "left"; //default align left
			}
		}
		if(ar[2] && ar[2] == "0"){
			dad.addClass(new_th, "display_none");//hide the col
		}
		var format = ar[4];
		if(format == "M") myColsFormat[i] = dad.formatCurrency;
		else if(format == "N") myColsFormat[i] = dad.formatNumeric;
		else if(format == "B") myColsFormat[i] = dad.formatBoolean;
		else myColsFormat[i] = false;

		th_row.appendChild(new_th);
		th_header_row.appendChild(new_th.cloneNode(true));
	}

	var first_record_no = table.my_record_header ? 0 : 1;
	if(records.length > first_record_no){
		var rec_count = records.length;
		var rowVisible = dad.isMOZ ? 'table-row' : 'block';
		var alignSupported = !dad.isIE;
		
		for (var i = first_record_no; i < rec_count; i++) { //1 to skip header
			var rec = records[i];
			var tr = rows[i];
			if(!tr){
				tr = tBody.insertRow(-1);
				if(i%2) dad.addClass(tr, "odd");
				if(row_over_cb) tr.onmouseover = row_over_cb;
				if(row_click_cb) tr.onclick = row_click_cb;
				else if(row_dblclick_cb) tr.ondblclick = row_dblclick_cb;
			}
			var cells = tr.cells;
			cell = tr.insertCell(-1);
			cell.align = "center";
			var chk = document.createElement("INPUT");
			chk.type = "checkbox";
			chk.onkeydown = dad.tableInputOnKeydown;
			chk.onfocus = dad.tableInputOnFocus;
			chk.onblur = dad.tableInputOnBlur;
			chk.onclick = selection_click_cb;
			cell.appendChild(chk);
			if(i==1) chk.focus();
			
			for(var j=0, k=1; j < col_count; ++j, ++k){
				var cell = cells[k];
				if(!cell){
					cell = tr.insertCell(-1);
					var cn = th_cells[k].className;
					if(cn) cell.className = cn + '_c';
					if(alignSupported) cell.align = myColsAlign[j];
				}
				if(j==0 && first_row_click_cb) cell.onclick = first_row_click_cb;
				var value = rec[j] || '';
				if(myColsFormat[j]) value = myColsFormat[j](value);
				cell.innerHTML = value;
			}
		}
	}
	table.style.display = "table";
	//setEditTable(table_id, 2);
	setTimeout(function() {
		dad.AjaxDataTableResizeHeader(id);
	},100);
	return table;
}
		
dad.parseSLEData = function(data, withHeader){
	var records = [];
	sle = dad.StrLenEncoded();
	sle.sle2vecOfvec(data, data.length, records);
	if(records.length < 1){
		alert("An error has occured making the request");
	}
	if(!withHeader) records.shift();
	return records;
}

dad.parseSLEData2Object = function(data){
		var result_array = dad.parseSLEData(data, true);
		var record = {};
		if(result_array.length > 1){
			var names = result_array[0];
			var values = result_array[1];
			for(var i in names){
				record[names[i]] = values[i]; 
			}
		}
		return record;
}

dad.newAjaxDataTableAjax = function(win, newId){
	// win.onAfterResize = function(){
		// setTimeout(function() {
				// dad.AjaxDataTableResizeHeader(newId);
			// },100);
	// }		
	return new dad.Ajax(function(id){
			if(this.status == 200){
				//retrieve result as an JavaScript object
				var records = dad.parseSLEData(this.responseText, true);
				//remove headers
				if(!this._withHeaders) records.shift();
				dad.fillTableById(records, id);
				dad.addWinOnAfterResize(win, newId, function(){
							setTimeout(function() {
							dad.AjaxDataTableResizeHeader(newId);
						},100);
					});	
			} else {
				alert("An error has occured making the request");
			}
		}, newId, false);
}

dad._getSABTitle = function(all_sales_buys, title){
	if(all_sales_buys == "S") title += ' Sales';
	else if(all_sales_buys == "B") title += ' Buys';
	return title;
}

dad.fillSelectByRecords = function(select, records, addDumy, dummyValue){
	var options = select.options;
	if(records && records.length > 0) {
		var opt, nrec = records.length;
		if(addDumy){
			opt = new Option("----", dummyValue ? dummyValue : 0);
			if(dad.isIE) options.add(opt);
			else options.add(opt, null);
		}
		for(var i=0; i < nrec; i++){
			var rec = records[i];
			opt = new Option(rec[1], rec[0]);
			if(dad.isIE) options.add(opt);
			else options.add(opt, i+1);
		}
	}
}

dad.StrLenEncoded = function(){
var IBYTE1 = 255;
var IBYTE2 = 255*255;
var IBYTE3 = 255*255*255;
var IBYTE4 = 255*255*255*255;

var SIZE1BYTE = 250;
var SIZE2BYTE = 251;
var SIZE3BYTE = 252;
var SIZE4BYTE = 253;

var SLEMARK = 254;
var SLEEND = 255;

this.charCodeAt = function(str, idx){
	return str.charCodeAt(idx) & 0xFF;
}

this.stringToBytesFaster = function ( str ) { 
    var ch, st, re = [], j=0;
    for (var i = 0; i < str.length; i++ ) { 
        ch = str.charCodeAt(i);
        if(ch < 127)
        {
            re[j++] = ch & 0xFF;
        }
        else
        {
            st = [];    // clear stack
            do {
                st.push( ch & 0xFF );  // push byte to stack
                ch = ch >> 8;          // shift value down by 1 byte
            }
            while ( ch );
            // add stack contents to result
            // done because chars have "wrong" endianness
            st = st.reverse();
            for(var k=0;k<st.length; ++k)
                re[j++] = st[k];
        }
    }   
    // return an array of bytes
    return re; 
}

this.utf8toUtf16 = function(string) {   
      var result = '';
      var start = 0;
      var end = 0;
	  var enc, c1, c2, c3;
	  var mycharCodeAt = this.charCodeAt;
      for (var i = 0, len = string.length; i < len; ++i) {
        c1 = mycharCodeAt(string, i);
        enc = null;
    
        if (c1 < 128) end++;
        else if (c1 > 0x07FF) {
			c2 = mycharCodeAt(string, ++i);
			c3 = mycharCodeAt(string, ++i);
			enc = String.fromCharCode(((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F));
		}
		else {
			c2 = mycharCodeAt(string, ++i);
			enc = String.fromCharCode(((c1 & 0x1F) << 6) | (c2 & 0x3F));
		}
    
        if (enc !== null) {
          if (end > start) result += string.slice(start, end);
    
          result += enc;
          start = end = i + 1;
        }
      }
    
      if (end > start) result += string.slice(start, string.length);
    
      return result;
}

this.sle2array = function(strascii, strascii_size, p, result_array)
{
    if(strascii_size == 0) return 0;
    var size = 0, data_count = 0, data_len = 0;
	var rec = [];
	var mycharCodeAt = this.charCodeAt;
    while((mycharCodeAt(strascii, p) != SLEEND) || (p >= strascii_size)) //data finished now follow digest
    {
        size = mycharCodeAt(strascii, p++);
        if(size >= SIZE1BYTE)
        {
            if(size == SIZE1BYTE)
            {
                //data bigger than 250 and less 500 next byte has more info
                size += mycharCodeAt(strascii, p++);
            }
            else if(size == SIZE2BYTE)
            {
                //data bigger than 250 next two bytes has more info
                size = mycharCodeAt(strascii, p++) * IBYTE1;
                size += mycharCodeAt(strascii, p++);
            }
            else if(size == SIZE3BYTE)
            {
                //data bigger than 250 next three bytes has more info
                size = mycharCodeAt(strascii, p++) * IBYTE2;
                size += mycharCodeAt(strascii, p++) * IBYTE1;
                size += mycharCodeAt(strascii, p++);
            }
            else if(size == SIZE4BYTE)
            {
                //data bigger than 250 next four bytes has more info
                size = mycharCodeAt(strascii, p++) * IBYTE3;
                size += mycharCodeAt(strascii, p++) * IBYTE2;
                size += mycharCodeAt(strascii, p++) * IBYTE1;
                size += mycharCodeAt(strascii, p++);
            }
            else if(size == SLEMARK)
            {
                //reserved can be used for multi part data, metadata, digest, ...
                break;
            }
        }
		if(size > 0) {
			var segment = strascii.substr(p, size);
			rec.push(this.utf8toUtf16(segment));
			//try {rec.push(decodeURIComponent(escape(segment)));} catch(e) {rec.push(segment);}
		}
		else rec.push("");
        p += size;		
        data_count++;
        data_len += size;
        if(data_len > strascii_size)
        {
            //something went wrong here
            throw "We got more data than expected !";
            break;
        }
    }
	result_array.push(rec);
    return ++p;
}

this.sle2vec = function(strascii, strascii_size, p, result_array)
{
    if(strascii[p] != '[') throw "Invalid encoded vector !";
    if(strascii[p+1] == ']') return p+2; //empty array
    result = this.sle2array(strascii, strascii_size-1, p+1, result_array);
    if(strascii[result] != ']') throw "Invalid end of vector !";
    return ++result;
}

this.sle2vecOfvec = function(strascii, strascii_size, result_array, p)
{
	if(!p) p = 0;
	if(strascii[p] != '[') throw "Invalid encoded vector !";
	if(strascii[p+1] == ']') return 2; //empty array
	p = this.sle2vec(strascii, strascii_size-1, p+1, result_array);
	while(strascii[p] == '['){
		p = this.sle2vec(strascii, strascii_size - (p - strascii_size), p, result_array);
	}
	if(strascii[p] != ']') throw "Invalid end of vector !";
	//printf("RowCount %d\n", row_count);
	return ++p;
}
	
	return this;
};

dad.rightClickMenu = null;

dad.showRightClickMenu = function(event) {
     if(!dad.rightClickMenu) return;
     /*  check whether the event is a right click 
       *  because different browser (ahem IE) assign different numbers to the keys to
       *  your mouse buttons and different values to the event, you'll have to do some evaluation
       */
     var rightclick; //will be set to true or false
     var evt = dad.checkEvent(event);
     if (evt.button) {
          rightclick = (event.button == 2);
     } else if (evt.which) {
          rightclick = (event.which == 3);
     }
 
     if(rightclick) { //if the secondary mouse botton was clicked
          var menu = document.getElementById(dad.rightClickMenu);
          menu.style.display = "block"; //show menu
 
	  var pos =  dad.getMousePos(evt);
 
          //position the menu
          menu.style.position = "fixed"; // use fixed or it will not work when the window is scrolled
          menu.style.top = pos.y+"px";
          menu.style.left= pos.x+"px";
     }
}
 
dad.clearRightClickMenu = function() { //used to make the menu disappear
     //this function should be used at the beginning of any function that is called from the menu
     if(dad.rightClickMenu) document.getElementById(dad.rightClickMenu).style.display = "none"; //don't show menu
}
