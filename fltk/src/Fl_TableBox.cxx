#include <FL/Fl.H>
#include <FL/Fl_TableBox.H>
#include <cassert>

Fl_TableBox::Fl_TableBox (int _x, int _y, int _w, int _h) : Fl_Group (_x, _y, _w, _h)
{
    t = new TABLE;
    t->gap = new CELL_GAP;
    Type (TABLEBOX);
    Size (1, 1);
    Gap (5, 5);
    cur_row = 0;
    cur_col = 0;
    set_changed();
}

Fl_TableBox::~Fl_TableBox ()
{
    delete t->gap;
    delete t;
}

void Fl_TableBox::draw()
{
    if(changed())   //first time draw()
    {
        resize(x(),y(),w(),h());
        clear_changed();
    };
    Fl_Group::draw();
};

uint Fl_TableBox::fixed_width()
{
    uint fw = 0;
    for (uint i = 0; i < t->cols; i++)
    {
        uint x_fixed = 0;
        for (uint j = 0; j < t->rows; j++)
        {
            TABLE_CELL & curr_cell = t->cell [j][i];
            if (curr_cell.x_expand) x_fixed += t->gap->w;
            else if (curr_cell.w > x_fixed) x_fixed = curr_cell.w;
        }
        fw += x_fixed + t->gap->w;
    }
    return fw ? fw + t->gap->w : 0;
}

uint Fl_TableBox::fixed_height()
{
    uint fh = 0;
    for (uint i = 0; i < t->rows; i++)
    {
        uint h_fixed = 0;
        for (uint j = 0; j < t->cols; j++)
        {
            TABLE_CELL & curr_cell = t->cell [j][i];
            if (curr_cell.y_expand) h_fixed += t->gap->h;
            else if(curr_cell.h > h_fixed) h_fixed = curr_cell.h;
        }
        fh += h_fixed+t->gap->h;
    }
    return fh ? fh + t->gap->h : 0;
}

void Fl_TableBox::resize(int i_x, int i_y, int i_w, int i_h)
{
    //Fl_Group::resize(i_x,i_y,i_w,i_h); // make new xywh values visible for children
    Fl_Widget::resize(i_x,i_y,i_w,i_h);
    uint x_expand_items = 0;
    uint x_expand_size_item = 0;
    uint x_fixed_size_item [t->cols];
    uint y_expand_items = 0;
    uint y_expand_size_item = 0;
    uint y_fixed_size_item [t->rows];

    uint x_fixed_size_all_items = 0;
    uint y_fixed_size_all_items = 0;

    uint _x = x()+t->gap->w;
    uint _y = y()+t->gap->h;

    // x-axes
    for (uint i = 0; i < t->cols; i++)
    {
        x_fixed_size_item [i] = 0;

        for (uint j = 0; j < t->rows; j++)
        {
            TABLE_CELL & curr_cell = t->cell [j][i];
            if (curr_cell.x_expand)
            {
                x_expand_items++;
                x_fixed_size_item [i] = 0;

                for (uint k = i; k < t->cols; k++)
                    x_fixed_size_item [k] = 0;

                break;
            }
            else
            {
                if (x_fixed_size_item [i] < curr_cell.w)
                    x_fixed_size_item [i] = curr_cell.w;
            }
        }
    }

    for (uint i = 0; i < t->cols; i++)
        if (x_fixed_size_item [i] > 0)
            x_fixed_size_all_items += x_fixed_size_item [i];

    if (x_expand_items > 0)
        x_expand_size_item = (uint)((w () -t->gap->w - x_fixed_size_all_items -
                                     (t->cols ) * t->gap->w) / x_expand_items);

    // y-axes
    for (uint j = 0; j < t->rows; j++)
    {
        y_fixed_size_item [j] = 0;

        for (uint i = 0; i < t->cols; i++)
        {
            TABLE_CELL & curr_cell = t->cell [j][i];
            if (curr_cell.y_expand)
            {
                y_expand_items++;
                y_fixed_size_item [j] = 0;

                for (uint k = j; k < t->rows; k++)
                    y_fixed_size_item [k] = 0;

                break;
            }
            else
            {
                if (y_fixed_size_item [j] < curr_cell.h)
                    y_fixed_size_item [j] = curr_cell.h;
            }
        }
    }

    for (uint j = 0; j < t->rows; j++)
        if (y_fixed_size_item [j] > 0)
            y_fixed_size_all_items += y_fixed_size_item [j];

    if (y_expand_items > 0)
        y_expand_size_item = (uint)((h() -t->gap->h - y_fixed_size_all_items -
                                     (t->rows ) * t->gap->h) / y_expand_items);

    uint Wv, Wm, Xm;
    uint Hv, Hm, Ym;

    // packing
    for (uint j = 0; j < t->rows; j++)
    {
        for (uint i = 0; i < t->cols; i++)
        {
            TABLE_CELL & curr_cell = t->cell [j][i];
            // w
            if (x_fixed_size_item [i] > 0) Wv = x_fixed_size_item [i];
            else Wv = x_expand_size_item;

            // x span
            if (curr_cell.x_span > 1)
            {
                for (uint k=1; k < curr_cell.x_span; k++)
                {
                    if (x_fixed_size_item [i+k] > 0) Wv += x_fixed_size_item [i+k]
                                                               + t->gap->w;
                    else Wv += x_expand_size_item + t->gap->w;
                }
            }

            // x fill
            if (curr_cell.x_fill) Wm = Wv;
            else if (Wv > curr_cell.w) Wm = curr_cell.w;
            else Wm = Wv;

            Xm = (uint)(curr_cell.x_align * (Wv - Wm));

            // h
            if (y_fixed_size_item [j] > 0) Hv = y_fixed_size_item [j];
            else Hv = y_expand_size_item;

            // y span
            if (curr_cell.y_span > 1)
            {
                for (uint k=1; k < curr_cell.y_span; k++)
                {
                    if (y_fixed_size_item [j+k] > 0) Hv += y_fixed_size_item [j+k]
                                                               + t->gap->h;
                    else Hv += y_expand_size_item + t->gap->h;
                }
            }

            // y fill
            if (curr_cell.y_fill) Hm = Hv;
            else if (Hv > curr_cell.h) Hm = curr_cell.h;
            else Hm = Hv;

            Ym = (uint)(curr_cell.y_align * (Hv - Hm));

            // resizing / positioning
            if (curr_cell.o != 0)
            {
                curr_cell.o->resize (_x + Xm, _y + Ym, Wm, Hm);
                //printf("%s %d:%d:%d:%d:%d:%d\n", curr_cell.o->label(),
                //    _x, _y, _x + Xm, _y + Ym, Wm, Hm);
            }

            //if (curr_cell.o && curr_cell.o->as_group () && !curr_cell.o->as_window ())
            //	curr_cell.o->layout ();

            // update X
            if (x_fixed_size_item [i] > 0) _x += x_fixed_size_item [i] + t->gap->w;
            else _x += x_expand_size_item + t->gap->w;
        }

        // beginning of next row
        _x = x()+t->gap->w;

        // update Y
        if (y_fixed_size_item [j] > 0) _y += y_fixed_size_item [j] + t->gap->h;
        else _y += y_expand_size_item + t->gap->h;
    }
}

// get
Fl_TableBox::TableBox_Type Fl_TableBox::Type ()
{
    return __type;
}

void Fl_TableBox::Size (uint * _rows, uint * _cols)
{
    *_rows = t->rows;
    *_cols = t->cols;
}

uint Fl_TableBox::Size ()
{
    if (Type () == VBOX)
        return t->rows;
    else if (Type () == HBOX)
        return t->cols;
    else
        return 0;
}

uint Fl_TableBox::Rows ()
{
    return t->rows;
}

uint Fl_TableBox::Cols ()
{
    return t->rows;
}

Fl_TableBox::CELL_GAP * Fl_TableBox::Gap ()
{
    return t->gap;
}

void Fl_TableBox::Gap (uint * _w, uint * _h)
{
    *_w = t->gap->w;
    *_h = t->gap->h;
}

Fl_Widget * Fl_TableBox::Widget (uint _row, uint _col)
{
    assert(_row < t->rows && _col < t->cols);
    return t->cell [_row][_col].o;
}

Fl_TableBox::TABLE_CELL * Fl_TableBox::Cell (uint _row, uint _col)
{
    assert(_row < t->rows && _col < t->cols);
    return & t->cell [_row][_col];
}

// set
void Fl_TableBox::Type (TableBox_Type _type)
{
    __type = _type;
}

void Fl_TableBox::Size (uint _rows, uint _cols)
{
    t->rows = _rows;
    t->cols = _cols;

    t->cell.size (t->rows);

    for (uint j=0; j<t->rows; j++)
        t->cell [j].size (t->cols);
}

bool Fl_TableBox::Size (uint _len)
{
    if (Type () == VBOX)
        Rows (_len);
    else if (Type () == HBOX)
        Cols (_len);
    else
        return false; //error condition
    return true; //ok done
}

void Fl_TableBox::Rows (uint _len)
{
    t->rows = _len;
    Size ((uint)t->rows, (uint)t->cols);
}

void Fl_TableBox::Cols (uint _len)
{
    t->cols = _len;
    Size ((uint)t->rows, (uint)t->cols);
}

void Fl_TableBox::AddCell (uint _len)
{
    Size (Size () + 1);
}

void Fl_TableBox::Gap (CELL_GAP * _gap)
{
    t->gap = _gap;
}

void Fl_TableBox::Gap (uint _w, uint _h)
{
    t->gap->w = _w;
    t->gap->h = _h;
}

void Fl_TableBox::Widget (Fl_Widget * _widget, uint _row, uint _col)
{
    assert(_row < t->rows && _col < t->cols);
    t->cell [_row][_col].o = _widget;
}

void Fl_TableBox::Cell (TABLE_CELL * _cell, uint _row, uint _col)
{
    assert(_row < t->rows && _col < t->cols);
    t->cell [_row][_col] = * _cell;
}

void Fl_TableBox::Attach (const TABLE_CELL & _cell, uint _row, uint _col)
{
    assert(_row < t->rows && _col < t->cols);
    t->cell [_row] [_col] = _cell;

    for (uint j = 0; j < _cell.y_span; j++)
    {
        assert((_row+j) < t->rows);
        for (uint i = 0; i < _cell.x_span; i++)
        {
            assert((_col+i) < t->cols);
            TABLE_CELL &cell = t->cell [_row+j] [_col+i];
            cell.x_align = _cell.x_align;
            cell.y_align = _cell.y_align;
            cell.x_expand = _cell.x_expand;
            cell.y_expand = _cell.y_expand;
        }
    }
}

Fl_Widget *  Fl_TableBox::Attach (
    Fl_Widget * _widget,
    uint _row, uint _col,
    uint _w, uint _h,
    Expand_Type _x_prop,
    Expand_Type _y_prop,
    uint _x_span, uint _y_span,
    float _x_align, float _y_align
)
{
    assert(_row < t->rows && _col < t->cols);
    TABLE_CELL cell;
    cell.o = _widget;
    cell.w = _w;
    cell.h = _h;
    cell.x_fill = _x_prop & FILL;
    cell.y_fill = _y_prop & FILL;
    cell.x_span = _x_span;
    cell.y_span = _y_span;
    cell.x_align = _x_align;
    cell.y_align = _y_align;
    cell.x_expand = _x_prop & EXPAND;
    cell.y_expand = _y_prop & EXPAND;
    Attach(cell, _row, _col);
    return _widget;
}

Fl_Widget *  Fl_TableBox::Add (
    Fl_Widget * _widget,
    uint _w, uint _h,
    Expand_Type _x_prop,
    Expand_Type _y_prop,
    uint _x_span,
    uint _y_span,
    float _x_align, float _y_align
)
{
    uint _row = cur_row;
    uint _col = cur_col;

    TABLE_CELL &cell = t->cell [_row] [_col];
    cell.o = _widget;
    cell.w = _w;
    cell.h = _h;
    cell.x_fill = _x_prop & FILL;
    cell.y_fill = _y_prop & FILL;
    cell.x_span = _x_span;
    cell.y_span = _y_span;
    cell.x_align = _x_align;
    cell.y_align = _y_align;

    for (uint j = 0; j < _y_span; j++)
    {
        assert((_row+j) < t->rows);
        for (uint i = 0; i < _x_span; i++)
        {
            assert((_col+i) < t->cols);
            TABLE_CELL &cell = t->cell [_row+j] [_col+i];
            cell.x_expand = _x_prop & EXPAND;
            cell.y_expand = _y_prop & EXPAND;
        }
    }

    if (Type () == VBOX)
        cur_row++;
    else if (Type () == HBOX)
        cur_col++;

    return _widget;
}

