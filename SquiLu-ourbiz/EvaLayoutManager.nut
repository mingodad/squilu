/*
 EvaLayoutManager without third party dependencies.
 Originally derived from 
 EvaLayout, Lay it be! 
 (http://www.codeproject.com/Articles/13891/EvaLayout-Lay-it-be)
 Copyright (c) 2014, Domingo Alvarez Duarte - mingodad[at]gmail[dot]com
 
 Released under the MIT LICENSE or GNU LESSER GENERAL PUBLIC LICENSE  Version 3
 at your choice, permission to release under this licenses was obtained from the
 original author Alejandro Xalabarder ales[at]elxala[dot]de
 
 */

const HEADER_ADAPT = "A";
const HEADER_EXPAND = "X";
const EXPAND_HORIZONTAL = "-";
const EXPAND_VERTICAL =  "+";

local function max(a, b)
{
	return a >= b ? a : b;
}
	

class WidgetInfo
{
	name = null;      // name of the component in the layout array
	widgetId = null;     // component window handle

	isLaidOut = null; // if the component has been found in the layout array
			// and therefore if indxPos is a valid calculated field
	indxPos = null;   // place in the layout array mesured in array indices
			// left  = column index
			// right = last column index
			// top   = row index
			// right = last row index

	iniRect = null;   // initial pos and size of the component

	constructor(wname, theHandle, originalRect)
	{
		name  = wname;
		widgetId  = theHandle;
		isLaidOut = false;
		iniRect = originalRect;
		indxPos = {left : 0, right : 0, top : 0, bottom : 0};
	}
}

class EvaLayoutManager
{
	isPrecalculated = null;
	Hmargin = null;
	Vmargin = null;
	Hgap = null;
	Vgap = null;
	fijoH = null;
	fijoV = null;
	
	Hpos = null;
	Hdim = null;
	HextraPos = null;
	
	Vdim = null;
	Vpos = null;
	VextraPos = null;
	
	columnsReparto = null;
	rowsReparto = null;
	
	componentArray = null;
	componentArrayIdx = null;
	evaLayout = null;
	
	constructor()
	{
		isPrecalculated = false;
		Hmargin = 0;
		Vmargin = 0;
		Hgap = 0;
		Vgap = 0;
		fijoH = 0;
		fijoV = 0;
		
		Hpos = [];
		Hdim = [];
		HextraPos = [];
		
		Vdim = [];
		Vpos = [];
		VextraPos = [];
		
		columnsReparto = [];
		rowsReparto = [];
		
		componentArray = [];
		componentArrayIdx = {};
		evaLayout = null;
	}
	
	function setLayout (layoutInfo)
	{
		if(type(layoutInfo) === "string")
		{
			var linfo = [];
			var lines = layoutInfo.split('\n');
			var lines_len = lines.len();
			var maxcols = 0;
			for(var i=0; i < lines_len; ++i)
			{
				var rec = lines[i].split(',');
				var rec_len = rec.len();
				if(rec_len === 1) continue; //ignore blank lines
				for(var j=0; j < rec_len; ++j)
				{
					rec[j] = rec[j].strip();
				}
				if(rec_len > maxcols)
				{
					maxcols = rec_len;
				}
				linfo.push(rec);
			}
			for(var i=0, len=linfo.len(); i < len; ++i)
			{
				var rec = linfo[i];
				var rec_len = rec.len();
				if(rec_len < maxcols)
				{
					for(var j=rec_len; j < maxcols; ++j)
					{
						rec.push("");
					}
				}
			}
			evaLayout = linfo;
			//console.log(linfo);
		} else { //assume an array
			evaLayout = layoutInfo;		
		}

		isPrecalculated = false;
	}
	
	function getComponentRect(compWinHandle, rect)
	{
		rect.left <- math.random(5, 380);
		rect.top <- math.random(5, 380);
		rect.right <- math.random(rect.left + 6, 360);
		rect.bottom <- rect.top + 20;
	}
	
	function addComponent (componentName, compWinHandle)
	{
		var rect = {};
		getComponentRect (compWinHandle, rect);

		// add the component to the arrayand index it 
		componentArrayIdx[componentName] <- componentArray.len();
		// add the component to the array
		componentArray.push (new WidgetInfo(componentName, compWinHandle, rect));
		isPrecalculated = false;
	}
	
	function hideShowComponentsInLayout ()
	{
		var compIds = getLayoutWidgetIds ();
		for(var i=0, len=componentArray.len(); i < len; ++i)
		{
			var wi = componentArray[i];
			hideShowComponent (wi.widgetId, !compIds[wi.widgetId]);
		}
	};
	
	function hideShowComponent (theId, bHide)
	{
	};
	
	function removeComponents ()
	{
		componentArray.clear ();
		componentArrayIdx.clear();
		isPrecalculated = false;
	}
	
	function showComponent(cId, bShowHide)
	{
		print("show", cId, bShowHide);
	}
	
	function moveComponent (cId, x, y, dx, dy, bShowHide)
	{
		print("move", cId, x, y, dx, dy, bShowHide);
	}
	
	function distributeWeight (aryExtraPos, aryReparto, HVdim, totalReparto, nrcsize)
	{
		for (var ii = 0, len=aryExtraPos.len(); ii < len; ii ++) aryExtraPos[ii] = 0;
		var arsize = aryReparto.len();
		var totalWeight = 0;
		for (var ii = 0; ii < arsize; ii ++) totalWeight += aryReparto[ii][1];
		var repartHV = totalReparto / ((totalWeight === 0) ? 1 : totalWeight);
		for (var ii = 0; ii < arsize; ii ++)
		{
			var colAry = aryReparto[ii];
			var indx = colAry[0];
			var indxExpandHV = repartHV * colAry[1];
			HVdim[indx] = indxExpandHV;
			for (var res = indx+1; res < nrcsize; res ++)
			{
				aryExtraPos[res] += indxExpandHV;
			}
		}		
	};
	
	function doLayout  (totWidth, totHeight)
	{
		precalculateAll ();

		// repartir H
		distributeWeight(HextraPos, columnsReparto, Hdim, (totWidth  - fijoH), nColumns());

		// repartir V
		distributeWeight(VextraPos, rowsReparto, Vdim, (totHeight - fijoV), nRows());

		for (var ii = 0, len = componentArray.len(); ii < len; ii ++)
		{
			var wi = componentArray[ii];

			showComponent (wi.widgetId, (wi.isLaidOut) ? true : false);
			if (! wi.isLaidOut) continue;

			var indxPos = wi.indxPos;
			var indxPosLeft = indxPos.left;
			var indxPosTop = indxPos.top;
			var indxPosRight = indxPos.right;
			var indxPosBottom = indxPos.bottom;
			var x = Hpos[indxPosLeft] + HextraPos[indxPosLeft];
			var y = Vpos[indxPosTop]  + VextraPos[indxPosTop];
			var dx = 0;
			var dy = 0;
			var mm;
			for (mm = indxPosLeft; mm <= indxPosRight; mm ++)
			{
				if (mm !== indxPosLeft) dx += Hgap;
				dx += Hdim[mm];
			}
			for (mm = indxPosTop; mm <= indxPosBottom; mm ++)
			{
				if (mm !== indxPosTop) dy += Vgap;
				dy += Vdim[mm];
			}

			//here we have a problem when the number become negative
			//the widget is not managed
			if (x < 0 || y < 0 || dx < 0 || dy < 0) //continue;
			{
				var elmRect = {};
				var elm = getComponentRect(wi.widgetId, elmRect);
				if(x < 0) x = elmRect.left;
				if(dx < 0) dx = elmRect.right - elmRect.left;
				if(y < 0) y = elmRect.top;
				if(dy < 0) dy = elmRect.bottom - elmRect.top;
			}

			moveComponent (wi.widgetId, x, y, dx, dy, true);
			//InvalidateRect(hControl, NULL, TRUE);
		}
	};

	function getWidgets (vecNames)
	{
		var ncsize = nColumns();
		for (var rr = 0, rlen = nRows(); rr < rlen; rr ++)
		{
			for (var cc = 0; cc < ncsize; cc ++)
			{
				var name = widgetAt (rr, cc);
				if (name.len() > 0 && name != EXPAND_HORIZONTAL && name != EXPAND_VERTICAL)
				{
					vecNames.push (name);
				}
			}
		}
	}
	
	function nColumns ()
	{
		return max (0, evaLayout[1].len() - 1);
	}
	
	function nRows ()
	{
		return max (0, evaLayout.len() - 2);
	}
	
	function widgetAt (nrow, ncol)
	{
		var theRow = nrow+2;
		if(evaLayout.len() > theRow)
		{
			var theCol = ncol + 1;
			theRow = evaLayout[theRow];
			if(theRow.len() > theCol)
			{
				return theRow[theCol];
			}
		}
		return null;
	}
	
	//private
	function getLayoutWidgetIds ()
	{
		var widgetIds = {};
		var ncols = nColumns();
		for(var row=0, rlen=nRows(); row<rlen; ++row)
		{
			for(var col=0; col < ncols; ++col)
			{
				var wi = widgetAt(row, col);
				var found = widgetIds[wi.widgetId];
				widgetIds[wi.widgetId] = found ? found+1 : 1;
			}
		}
		return widgetIds;
	};
	
	//private
	
	function isExpandWeight (headStr, aryToStore, idxToStore)
	{
		var strLen = headStr.len() ;
		if (strLen && headStr[0] === HEADER_EXPAND[0])
		{
			var weight = 1;
			if(strLen > 1)
			{
				weight = headStr.slice(1).tointeger();
			}
			aryToStore.push ([idxToStore, weight]);   // compute later
			return true;
		}
		return false;
	};
	
	function computeVHDim (nrcsize, getHeader, VHmargin, VHgap, VHdim, 
			VHpos, VHextraPos, aryReparto, getMinOf)
	{
		var fijoHV = VHmargin;
		for (var rr = 0; rr < nrcsize; rr ++)
		{
			var heaStr = getHeader(rr);
			var gap = (rr === 0) ? 0 : VHgap;

			VHdim.push (0);
			VHpos.push (0);
			VHextraPos.push (0);

			if (!isExpandWeight(heaStr, aryReparto, rr))
			{
				if (heaStr === "" || heaStr === HEADER_ADAPT)
				{
					VHdim[rr] = getMinOf(rr);   // maximum-minimum of the row
				}
				else
				{
					VHdim[rr] = heaStr.tointeger(); // indicated size
				}
			}

			VHpos[rr] = fijoHV + gap;
			fijoHV += VHdim[rr];
			fijoHV += gap;
		}
		return fijoHV + VHmargin;
	};
	
	function precalculateAll ()
	{
		if (isPrecalculated) return;

		var el_row = evaLayout[0];
		Hmargin = max(0, el_row[1].tointeger() );
		Vmargin = max(0, el_row[2].tointeger() );
		Hgap    = max(0, el_row[3].tointeger() );
		Vgap    = max(0, el_row[4].tointeger() );

		Hdim.clear ();
		Hpos.clear ();
		Vdim.clear ();
		Vpos.clear ();
		columnsReparto.clear ();
		rowsReparto.clear ();

		for (var ii = 0, len = componentArray.len(); ii < len; ii ++)
		{
			componentArray[ii].isLaidOut = false;
		}

		//var cc;
		//var rr;
		var nrsize = nRows();
		var ncsize = nColumns();

		// compute Vdim
		fijoV = computeVHDim(nrsize, rowHeader, Vmargin, Vgap, Vdim, 
			Vpos, VextraPos, rowsReparto, minHeightOfRow);

		// compute Hdim
		fijoH = computeVHDim(ncsize, columnHeader, Hmargin, Hgap, Hdim, 
			Hpos, HextraPos, columnsReparto, minWidthOfColumn);

		// finding all components in the layout array
		for (var cc = 0; cc < ncsize; cc ++)
		{
			for (var rr = 0; rr < nrsize; rr ++)
			{
				var name = widgetAt(rr, cc);

				var indx = indxComponent (name);
				if (indx === -1) continue;

				var wid = componentArray[indx];
				var indxPos = wid.indxPos;

				// set position x,y
				indxPos.left = cc;
				indxPos.top  = rr;

				// set position x2,y2
				var ava = cc;
				while (ava+1 < ncsize && widgetAt(rr, ava+1) === EXPAND_HORIZONTAL) ava ++;
				indxPos.right = ava;

				ava = rr;
				while (ava+1 < nrsize && widgetAt(ava+1, cc) === EXPAND_VERTICAL) ava ++;
				indxPos.bottom = ava;

				wid.isLaidOut = true;
		      }
		}

		isPrecalculated = true;
	};

	function columnHeader (ncol)
	{
		return evaLayout[1][ncol + 1];
	}
	
	function rowHeader (nrow)
	{
		return evaLayout[2 + nrow][0];
	}
	
	function minHeightOfRow (nrow)
	{
		// el componente mas alto de la columna
		var maxheight = 0;
		for (var cc = 0, len = nColumns(); cc < len; cc ++)
		{
			var name = widgetAt (nrow, cc);
			var indx = indxComponent (name);
			if (indx == -1) continue;

			// if the widget occupies more than one row do not compute it
			if (widgetAt (nrow+1, cc) == EXPAND_VERTICAL) continue;

			var wid = componentArray[indx];
			var height = wid.iniRect.bottom - wid.iniRect.top;
			maxheight = max (maxheight, height);
		}
		return maxheight;
	}
	
	function minWidthOfColumn (ncol)
	{
		// el componente mas ancho de la columna
		var maxwidth = 0;
		for (var rr = 0, len = nRows(); rr < len; rr ++)
		{
			var name = widgetAt (rr, ncol);
			var indx = indxComponent (name);
			if (indx == -1) continue;

			// if the widget occupies more than one column do not compute it
			if (widgetAt (rr, ncol+1) == EXPAND_HORIZONTAL) continue;

			var wid = componentArray[indx];
			var width = wid.iniRect.right - wid.iniRect.left;
			maxwidth = max (maxwidth, width);
		}
		return maxwidth;
	}
	
	function indxComponent (compName)
	{
		return componentArrayIdx.rawget(compName, -1);
	};
}

class MyEvaLayoutManager extends EvaLayoutManager {
	function showComponent(cId, bShowHide)
	{
		//print("show", cId, bShowHide);
	}
	
	function moveComponent (cId, x, y, dx, dy, bShowHide)
	{
		print(format([==[document.getElementById("%s").setAttribute("style", "position:absolute;left:%dpx;top:%dpx;right:%dpx;bottom:%dpx;");]==], cId, x, y, dx, dy, bShowHide));
	}	
}

var layInfo = [
	["EvaLayout", "10", "10", "5", "5"],
	["grid" ,       "75"    ,     "X"    ,      "A"     ],
	[  "A"   , "boton1"   , "memo" ,      "-"      ],
	[  "A"   , "boton2"   ,   "+"      ,      ""       ],
	[  "A"   , "boton3"   ,   "+"      ,      ""       ],
	[  "X"   ,  ""             ,   "+"      ,      ""       ],
	[  "A"   ,  "edit1"     ,   "-"        , "boton4" ],
];

var manager = new MyEvaLayoutManager();
manager.setLayout(layInfo);

manager.addComponent("memo",    "memo");
manager.addComponent("boton1",  "boton1");
manager.addComponent("boton2",  "boton2");
manager.addComponent("boton3",  "boton3");
manager.addComponent("boton4",  "boton4");
manager.addComponent("edit1",  "edit1");


print(manager.nRows(), manager.nColumns());

manager.doLayout(300, 400);
manager.doLayout(400, 300);
manager.doLayout(500, 600);
manager.doLayout(600, 500);
