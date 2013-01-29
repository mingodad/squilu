function _tr(str){ return str;}

class MyBaseWindow extends Fl_Window {
	childWindows=null;

	constructor(px, py, pw, ph, pl) {
		base.constructor(px, py, pw, ph, pl);
		childWindows = {};
	}

	function showChildWindow(winName, WindowClass){
		local win = childWindows.get(winName, false);
		if(!win){
			win = new WindowClass();
			win.label(winName);
			childWindows[winName] <- win;
		}
		win->show();
		return win;
	}
}

class Fl_Data_Table extends Flv_Table {
	constructor(px, py, pw, ph, pl=""){
		base.constructor(px, py, pw, ph, pl);
	}
}

dofile("utils-fltk.nut");
dofile("base-report-A4.nut");
dofile("invoice-A4.nut");
dofile("edit-product-window.nut");
dofile("edit-entity-window.nut");
dofile("edit-order-window.nut");
dofile("list-search-window.nut");
dofile("sales-tax-window.nut");
dofile("main-window.nut");


local win = new MainWindow(10, 50, 330, 320, "OURBIZ");
//local win = new SalesTaxRatesEditWindow();
win->resizable(win);
win->show_main();

Fl.run();
