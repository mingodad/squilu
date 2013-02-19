#ifndef FL_TABLEBOX_H
#define FL_TABLEBOX_H

#ifdef WIN32
#define uint unsigned int
#endif

#include <cstdlib>
#include <cstring>

#include <FL/Fl_Group.H>

#define ATTACH_WIDGET(tbl, wid, label, row, col) (wid*) tbl->Attach((Fl_Widget *) new wid (0, 0, 0, 0, label), row, col)

class Fl_TableBox : public Fl_Group {
    public:

    enum Expand_Type {NONE, EXPAND=2, FILL=4};
    enum TableBox_Type {TABLEBOX=1, VBOX, HBOX};

    struct TABLE_CELL {
        Fl_Widget * o;
        uint w, h;
        uint x_span, y_span;
        float x_align, y_align;
        bool x_expand, y_expand;
        bool x_fill, y_fill;
        void reset()
        {
            o = 0;
            w = 100;
            h = 25;
            x_expand = false;
            y_expand = false;
            x_fill = false;
            y_fill = false;
            x_span = 1;
            y_span = 1;
            x_align = 0.0f;
            y_align = 0.0f;
        }
    };

    struct CELL_GAP {
        uint w, h;
    };

    Fl_Vector_Based_On_Malloc(TABLE_CELL, TABLE_CELL, 2);
    Fl_Vector_Based_On_Malloc(TABLE_CELL_Vector, Fl_TABLE_CELL_Vector, 2);

    struct TABLE {
        uint cols, rows;
        Fl_TABLE_CELL_Vector_Vector cell;
        CELL_GAP * gap;
    };

    private:
        TableBox_Type __type;
        uint cur_row, cur_col;
        TABLE * t;


    public:
        Fl_TableBox (int _x, int _y, int _w, int _h);
        ~Fl_TableBox ();

        void resize(int _x, int _y, int _w, int _h);
        void draw();

        // get
        TableBox_Type Type ();
        void Size (uint * _rows, uint * _cols);
        uint Size ();
        uint Rows ();
        uint Cols ();
        CELL_GAP * Gap ();
        void Gap (uint * _w, uint * _h);
        Fl_Widget * Widget (uint _row, uint _col);
        TABLE_CELL * Cell (uint _row, uint _col);

        uint fixed_width();
        uint fixed_height();

        // set
        void Type (TableBox_Type _type);
        void Size (uint _rows, uint _cols);
        bool Size (uint _len);
        void Rows (uint _len);
        void Cols (uint _len);
        void AddCell (uint _len);
        void Gap (CELL_GAP * _gap);
        void Gap (uint _w, uint _h);
        void Widget (Fl_Widget * _widget, uint row, uint col);
        void Cell (TABLE_CELL * _cell, uint _row, uint _col);
        void Attach (const TABLE_CELL & _cell, uint _row, uint _col);
        Fl_Widget * Attach (
            Fl_Widget * _widget,
            uint _row, uint _col,
            uint _w = 100, uint _h = 25,
            Expand_Type _x_prop = FILL,
            Expand_Type _y_prop = NONE,
            uint _x_span = 1, uint _y_span = 1,
            float _x_align = 0.0f, float _y_align = 0.0f
        );
        Fl_Widget *  Add (
            Fl_Widget * _widget,
            uint _w = 100, uint _h = 25,
            Expand_Type _x_prop = FILL,
            Expand_Type _y_prop = NONE,
            uint _x_span = 1, uint _y_span = 1,
            float _x_align = 0.0f, float _y_align = 0.0f
        );
DECLARE_CLASS_CHEAP_RTTI_2(Fl_TableBox, Fl_Group)
};

#endif //FL_TABLEBOX_H

