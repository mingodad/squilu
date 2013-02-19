//	======================================================================
//	File:    Flve_Combo.h - Flve_Combo implementation
//	Library: flvw - FLTK Widget
//	Version: 0.1.0
//	Started: 01/12/2000
//
//	Copyright (C) 1999 Laurence Charlton
//
//	Description:
//	Flve_Combo implements combo box functionality
//	Included are select from list, text with list, incremental search
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//	======================================================================

#ifndef FLVE_COMBO_H
#define FLVE_COMBO_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Flv_List.H>

#define flv_ctrl(x) ((x>='a' && x<='z'?x-('a'-'A'):x)|0x100)
#define flv_alt(x) ((x>='a' && x<='z'?x-('a'-'A'):x)|0x200)
#define flv_shift(x) ((x>='a' && x<='z'?x-('a'-'A'):x)|0x400)

class FL_EXPORT Flv_Combo_Item
{
public:
    Flv_Combo_Item();
    virtual ~Flv_Combo_Item();

    const char *item(void);
    void item(const char *v);
    long value(void);
    void value(long v);
protected:
    char *vitem;
    long vvalue;
};

class Flv_Combo_Items
{
public:
    Flv_Combo_Items();
    ~Flv_Combo_Items();

    int count(void)	//	# of styles
    {
        return vcount;
    }

    void add( const char *item, long v=0L );
    void insert( int index, const char *item, long v=0L );
    void remove( int index );
    void change( int i, const char *item, long v );
    void change( int i, const char *item );
    void change( int i, long v );

    void sort(void);	//	Sort list
    void clear(void);	//	Clear list
    int index(void)	//	Get current index
    {
        return vcurrent;
    };
    void index(int i);	//	Set current index

    int findi( const char *v );	//	Find starting with v (-1 not found)
    int find( const char *v );	//	Find text return index (-1 not found)
    int find( long v );	//	Find value return index (-1 not found)

    Flv_Combo_Item *current(void);
    Flv_Combo_Item &operator[](int index);
private:
    void make_room_for( int n );
    Flv_Combo_Item **list;	//	Array of item pointers
    int vcount;	//	# of item pointers defined
    int vallocated;	//	# of item pointers allocated
    int vcurrent;	//	Current item index
};


class Flve_Combo : public Fl_Group
{
public:
    Flve_Combo(int x, int y, int w, int h, const char *l );
    ~Flve_Combo();
    virtual void resize(int x, int y, int w, int h);
    void list_title(const char *v);
    void open_list(void);
    const char *value();
    void value(const char *v);
    int display_rows()
    {
        return vdisplay_rows;
    };
    void display_rows(int v);
    int drop_key(void)
    {
        return vdrop_key;
    }
    int drop_key( int v )
    {
        return vdrop_key = v;
    }

    bool incremental_search(void)
    {
        return vincremental_search;
    }
    bool incremental_search(bool v)
    {
        return (vincremental_search=v);
    }
    bool list_only(void)
    {
        return vlist_only;
    }
    bool list_only(bool v);

    Flv_Combo_Items item;
    Fl_Window *list;

    DECLARE_CLASS_CHEAP_RTTI_2(Flve_Combo, Fl_Group)
protected:
    int vdrop_key;
    char *vlist_title;
    int vdisplay_rows;
    int handle(int event);
    void draw(void);
    Fl_Input *input;
    bool vincremental_search;
    bool vlist_only;
};
#endif

