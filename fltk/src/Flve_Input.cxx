//	======================================================================
//	File:    Flve_Input.cxx - Flve_Input implementation
//	Library: flvw - FLTK Virtual widget library
//	Version: 0.1.0
//	Started: 01/12/2000
//
//	Copyright (C) 1999 Laurence Charlton
//
//	Description:
//	Flve_Input implements cell text editing for a list/table.
//	======================================================================

#include <FL/Fl.H>
#include <FL/Flve_Input.H>

int Flve_Input::handle(int event)
{
	int stat = Fl_Input::handle(event);
	if (event==FL_KEYBOARD)
	{
//		if (stat)
//		{
//			damage(FL_DAMAGE_CHILD);
//			draw();
//		}
		if ( !stat && owner )
		{
			if ( owner->handle(FL_SHORTCUT) )
				return 1;
		}
	}
	return stat;
}


