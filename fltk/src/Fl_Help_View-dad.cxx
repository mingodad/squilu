//
// "$Id: Fl_Help_View.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Fl_Help_View widget routines.
//
// Copyright 1997-2010 by Easy Software Products.
// Image support by Matthias Melcher, Copyright 2000-2009.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//
// Contents:
//
//   Fl_Help_View::add_block()       - Add a text block to the list.
//   Fl_Help_View::add_link()        - Add a new link to the list.
//   Fl_Help_View::add_target()      - Add a new target to the list.
//   Fl_Help_View::compare_targets() - Compare two targets.
//   Fl_Help_View::do_align()        - Compute the alignment for a line in
//                                     a block.
//   Fl_Help_View::draw()            - Draw the Fl_Help_View widget.
//   Fl_Help_View::format()          - Format the help text.
//   Fl_Help_View::format_table()    - Format a table...
//   Fl_Help_View::free_data()       - Free memory used for the document.
//   Fl_Help_View::get_align()       - Get an alignment attribute.
//   Fl_Help_View::get_attr()        - Get an attribute value from the string.
//   Fl_Help_View::get_color()       - Get an alignment attribute.
//   Fl_Help_View::handle()          - Handle events in the widget.
//   Fl_Help_View::Fl_Help_View()    - Build a Fl_Help_View widget.
//   Fl_Help_View::~Fl_Help_View()   - Destroy a Fl_Help_View widget.
//   Fl_Help_View::load()            - Load the specified file.
//   Fl_Help_View::resize()          - Resize the help widget.
//   Fl_Help_View::topline()         - Set the top line to the named target.
//   Fl_Help_View::topline()         - Set the top line by number.
//   Fl_Help_View::value()           - Set the help text directly.
//   scrollbar_callback()            - A callback for the scrollbar.
//

//
// Include necessary header files...
//

#ifdef _DAD_DEBUG_
#include "Fl_Help_View_Debug.H"
#else

#include <FL/Fl_Help_View.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Pixmap.H>
#include <stdio.h>
#include <stdlib.h>
#include <FL/fl_utf8.h>
#include <FL/filename.H>	// fl_open_uri()
#include "flstring.h"
#include <ctype.h>
#include <errno.h>
#include <math.h>

#if defined(WIN32) && ! defined(__CYGWIN__)
#  include <io.h>
#  include <direct.h>
#else
#  include <unistd.h>
#endif // WIN32

#endif //_DAD_DEBUG_

#define MAX_COLUMNS	200

#define GET_ATTR(aname) get_attr ( attrs, aname, attr, sizeof ( attr ) )

#define TD_MARGIN td_margin_


//
// Typedef the C API sort function type the only way I know how...
//

extern "C"
{
    typedef int ( *compare_func_t )( const void *, const void * );
}


//
// Local functions...
//

static int	quote_char( const char * );
static void	scrollbar_callback( Fl_Widget *s, void * );
static void	hscrollbar_callback( Fl_Widget *s, void * );

//
// global flag for image loading (see get_image).
//

static char initial_load = 0;

//
// Broken image...
//

static const char *broken_xpm[] =
{
    "16 24 4 1",
    "@ c #000000",
    "  c #ffffff",
    "+ c none",
    "x c #ff0000",
    // pixels
    "@@@@@@@+++++++++",
    "@    @++++++++++",
    "@   @+++++++++++",
    "@   @++@++++++++",
    "@    @@+++++++++",
    "@     @+++@+++++",
    "@     @++@@++++@",
    "@ xxx  @@  @++@@",
    "@  xxx    xx@@ @",
    "@   xxx  xxx   @",
    "@    xxxxxx    @",
    "@     xxxx     @",
    "@    xxxxxx    @",
    "@   xxx  xxx   @",
    "@  xxx    xxx  @",
    "@ xxx      xxx @",
    "@              @",
    "@              @",
    "@              @",
    "@              @",
    "@              @",
    "@              @",
    "@              @",
    "@@@@@@@@@@@@@@@@",
    NULL
};

static Fl_Pixmap broken_image( broken_xpm );

//
// Simple margin stack for Fl_Help_View::format()...
//

struct fl_margins
{
    int depth_;
    int margins_[100];

    fl_margins()
    {
        clear();
    }

    int clear()
    {
//    puts("fl_margins::clear()");

        depth_ = 0;
        return margins_[0] = 4;
    }

    int current()
    {
        return margins_[depth_];
    }

    int pop()
    {
//    printf("fl_margins::pop(): depth_=%d, xx=%d\n", depth_,
//           depth_ > 0 ? margins_[depth_ - 1] : 4);

        if ( depth_ > 0 )
        {
            depth_ --;
            return margins_[depth_];
        }
        else return 4;
    }

    int push( int indent )
    {
        int xx;

        xx = margins_[depth_] + indent;

//    printf("fl_margins::push(indent=%d): depth_=%d, xx=%d\n", indent,
//           depth_ + 1, xx);

        if ( depth_ < 99 )
        {
            depth_ ++;
            margins_[depth_] = xx;
        }

        return xx;
    }
};

//
// All the stuff needed to implement text selection in Fl_Help_View
//

/* matt:
 * We are trying to keep binary compatibility with previous versions
 * of FLTK. This means that we are limited to adding static variables
 * only to not enlarge the Fl_Help_View class. Lucky for us, only one
 * text can be selected system wide, so we can remember the selection
 * in a single set of variables.
 *
 * Still to do:
 * - &word; style characters mess up our count inside a word boundary
 * - we can only select words, no individual characters
 * - no dragging of the selection into another widget
 * - selection must be cleared if another widget get focus!
 * - write a comment for every new function
 */

/*
The following functions are also used to draw stuff and should be replaced with
local copies that are much faster when merely counting:

fl_color(Fl_Color);
fl_rectf(int, int, int, int);
fl_push_clip(int, int, int, int);
fl_xyline(int, int, int);
fl_rect()
fl_line()
img->draw()
*/


/*
 * Limitation: if a word contains &code; notations, we will calculate a wrong length.
 *
 * This function must be optimized for speed!
 */
void Fl_Help_View::hv_draw( const char *t, int x, int y )
{
    int len_t = strlen ( t );
    int flw = ( int )fl_width( t, len_t );
    int flh = fl_height();
    int fld = fl_descent();

    if ( selected && current_view == this && current_pos < selection_last && current_pos >= selection_first )
    {
        Fl_Color c = fl_color();
        fl_color( hv_selection_color );

        if ( current_pos + len_t < selection_last )
            flw += ( int )fl_width( ' ' );

        fl_rectf( x, y + fld - flh, flw, flh );
        fl_color( hv_selection_text_color );
        fl_draw ( t, len_t, x, y );
        fl_color( c );
    }
    else
    {
        fl_draw ( t, len_t, x, y );
    }
//printf("%s : %d, %d, %d\n", t, len_t, x, y);

    if ( draw_mode )
    {

        if ( mouse_x >= x && mouse_x < x + flw )
        {
            if ( mouse_y >= y - flh + fld && mouse_y <= y + fld )
            {
                int f = current_pos;
                int l = f + len_t; // use 'quote_char' to calculate the true length of the HTML string

                if ( draw_mode == 1 )
                {
                    selection_push_first = f;
                    selection_push_last = l;
                }
                else
                {
                    selection_drag_first = f;
                    selection_drag_last = l;
                }
            }
        }
    }
}

#define MEM_BLOCKS_INCREMENT 16
#define CHECK_NEED_RESIZE(theArray, elementType, aBlocks, currBlock) \
	if (currBlock >= aBlocks)\
	{\
		aBlocks += MEM_BLOCKS_INCREMENT;\
		int new_blocks_size = sizeof(elementType) * aBlocks;\
		theArray = (elementType *) ((aBlocks == MEM_BLOCKS_INCREMENT) ?\
									malloc(new_blocks_size) : realloc(theArray, new_blocks_size));\
	}


/** Adds a text block to the list. */
Fl_Help_Block *					// O - Pointer to new block
Fl_Help_View::add_block( const char   *s,	// I - Pointer to start of block text
                         int           xx,	// I - X position of block
                         int           yy,	// I - Y position of block
                         int           ww,	// I - Right margin of block
                         int           hh,	// I - Height of block
                         unsigned char border,	// I - Draw border?
                         Fl_Color border_color )  	// I - Draw border color ?
{
    Fl_Help_Block	*temp;				// New block


//  printf("add_block(s = %p, xx = %d, yy = %d, ww = %d, hh = %d, border = %d)\n",
//         s, xx, yy, ww, hh, border);
    CHECK_NEED_RESIZE ( blocks_, Fl_Help_Block, ablocks_, nblocks_ );

    temp = blocks_ + nblocks_;
    memset( temp, 0, sizeof( Fl_Help_Block ) );
    temp->start   = s;
    temp->end     = s;
    temp->x       = xx;
    temp->y       = yy;
    temp->w       = ww;
    temp->h       = hh;
    temp->border  = border;
    temp->border_color = border_color;
    temp->bgcolor = bgcolor_;
    nblocks_ ++;

    return ( temp );
}


/** Adds a new link to the list. */
void Fl_Help_View::add_link( const char *n,	// I - Name of link
                             int        xx,	// I - X position of link
                             int        yy,	// I - Y position of link
                             int        ww,	// I - Width of link text
                             int        hh )  	// I - Height of link text
{
    Fl_Help_Link	*temp;			// New link
    char		*target;		// Pointer to target name

    CHECK_NEED_RESIZE ( links_, Fl_Help_Link, alinks_, nlinks_ );

    temp = links_ + nlinks_;

    temp->x       = xx;
    temp->y       = yy;
    temp->w       = xx + ww;
    temp->h       = yy + hh;

    strlcpy( temp->filename, n, sizeof( temp->filename ) );

    if ( ( target = strrchr( temp->filename, '#' ) ) != NULL )
    {
        *target++ = '\0';
        strlcpy( temp->name, target, sizeof( temp->name ) );
    }
    else
        temp->name[0] = '\0';

    nlinks_ ++;
}


/** Adds a new target to the list. */
void Fl_Help_View::add_target( const char *n,	// I - Name of target
                               int        yy )  	// I - Y position of target
{
    Fl_Help_Target	*temp;			// New target

    CHECK_NEED_RESIZE ( targets_, Fl_Help_Target, atargets_, ntargets_ );

    temp = targets_ + ntargets_;

    temp->y = yy;
    strlcpy( temp->name, n, sizeof( temp->name ) );

    ntargets_ ++;
}

/** Compares two targets.*/
int							// O - Result of comparison
Fl_Help_View::compare_targets( const Fl_Help_Target *t0,	// I - First target
                               const Fl_Help_Target *t1 )  	// I - Second target
{
    return ( strcasecmp( t0->name, t1->name ) );
}

/** Computes the alignment for a line in a block.*/
int						// O - New line
Fl_Help_View::do_align( Fl_Help_Block *block,	// I - Block to add to
                        int          line,	// I - Current line
                        int          xx,		// I - Current X position
                        int          a,		// I - Current alignment
                        int          &l )  	// IO - Starting link
{
    int	offset;					// Alignment offset


    switch ( a )
    {
    case RIGHT :	// Right align
        offset = block->w - xx;
        break;
    case CENTER :	// Center
        offset = ( block->w - xx ) / 2;
        break;
    default :		// Left align
        offset = TD_MARGIN;
        break;
    }
    block->line[line] = block->x + offset;

    if ( line < line31_ )
        line ++;

    while ( l < nlinks_ )
    {
        links_[l].x += offset;
        links_[l].w += offset;
        l ++;
    }

    return ( line );
}

enum html_tags_enum
{
    TAG_UNKNOWN,
    TAG_A, TAG_CLOSE_A,
    TAG_ALIGN, TAG_CLOSE_ALIGN,
    TAG_B, TAG_CLOSE_B,
    TAG_BODY, TAG_CLOSE_BODY,
    TAG_BR,
    TAG_BRV,
    TAG_CENTER, TAG_CLOSE_CENTER,
    TAG_CODE, TAG_CLODE_CODE,
    TAG_DD, TAG_CLODE_DD,
    TAG_DL, TAG_CLODE_DL,
    TAG_DT, TAG_CLODE_DT,
    TAG_EM,
    TAG_FONT, TAG_CLOSE_FONT,
    TAG_H1, TAG_CLOSE_H1,
    TAG_H2, TAG_CLOSE_H2,
    TAG_H3, TAG_CLOSE_H3,
    TAG_H4, TAG_CLOSE_H4,
    TAG_H5, TAG_CLOSE_H5,
    TAG_H6, TAG_CLOSE_H6,
    TAG_HEAD, TAG_CLOSE_HEAD,
    TAG_HR,
    TAG_HTML, TAG_CLOSE_HTML,
    TAG_I, TAG_CLOSE_I,
    TAG_IMG,
    TAG_KBD, TAG_CLOSE_KBD,
    TAG_LEFT, TAG_CLOSE_LEFT,
    TAG_LI, TAG_CLOSE_LI,
    TAG_OL, TAG_CLOSE_OL,
    TAG_P, TAG_CLOSE_P,
    TAG_PRE, TAG_CLOSE_PRE,
    TAG_RIGHT, TAG_CLOSE_RIGHT,
    TAG_STRONG, TAG_CLOSE_STRONG,
    TAG_TABLE, TAG_CLOSE_TABLE,
    TAG_TITLE,
    TAG_TD, TAG_CLOSE_TD,
    TAG_TH, TAG_CLOSE_TH,
    TAG_TR, TAG_CLOSE_TR,
    TAG_TRM, TAG_CLOSE_TRM,
    TAG_TT, TAG_CLOSE_TT,
    TAG_U, TAG_CLOSE_U,
    TAG_UL, TAG_CLOSE_UL,
    TAG_VAR, TAG_CLOSE_VAR,
};

/**
	Check if the buf argument contains a known html tag
	assumes that the close tag follow open tag on enumaration
	Note: invalid close tags will match incorrectly
	Ex: /HR will map to HR+1 = TAG_HTML
*/

#define IF_STRCASECMP_TAG(atag) if (strcasecmp(buf, #atag) == 0) match_tag = TAG_##atag
#define IF_ENDS_TAG(atag) if (*ptrBuf == 0) match_tag = TAG_##atag

static int buffer_has_tag ( const char *buf0 )
{
    int match_tag = TAG_UNKNOWN;
    bool is_close_tag = ( *buf0 == '/' );
    const char *ptrBuf = is_close_tag ? buf0 + 1 : buf0;
    const char *buf = ptrBuf;

    switch ( toupper ( * ( ptrBuf++ ) ) )
    {
    case 'A':
        IF_ENDS_TAG ( A );
        break;
    case 'B':
        IF_ENDS_TAG ( B );
        else IF_STRCASECMP_TAG ( BR );
        else IF_STRCASECMP_TAG ( BRV );
        else IF_STRCASECMP_TAG ( BODY );

        break;
    case 'C':

        switch ( toupper ( * ( ptrBuf++ ) ) )
        {
        case 'E':
            IF_STRCASECMP_TAG ( CENTER );
            break;
        case 'O':
            IF_STRCASECMP_TAG ( CODE );
            break;
        }

        break;
    case 'D':

        switch ( toupper ( * ( ptrBuf++ ) ) )
        {
        case 'D':
            IF_ENDS_TAG ( DD );
            break;
        case 'L':
            IF_ENDS_TAG ( DL );
            break;
        case 'T':
            IF_ENDS_TAG ( DT );
            break;
        }

        break;
    case 'E':
        IF_STRCASECMP_TAG ( EM );
        break;
    case 'F':
        IF_STRCASECMP_TAG ( FONT );
        break;
    case 'H':

        switch ( toupper ( * ( ptrBuf++ ) ) )
        {
        case '1':
            IF_ENDS_TAG ( H1 );
            break;
        case '2':
            IF_ENDS_TAG ( H2 );
            break;
        case '3':
            IF_ENDS_TAG ( H3 );
            break;
        case '4':
            IF_ENDS_TAG ( H4 );
            break;
        case '5':
            IF_ENDS_TAG ( H5 );
            break;
        case '6':
            IF_ENDS_TAG ( H6 );
            break;
        case 'E':
            IF_STRCASECMP_TAG ( HEAD );
            break;
        case 'R':
            IF_STRCASECMP_TAG ( HR );
            break;
        case 'T':
            IF_STRCASECMP_TAG ( HTML );
            break;
        }

        break;
    case 'I':
        IF_ENDS_TAG ( I );
        else IF_STRCASECMP_TAG ( IMG );

        break;
    case 'K':
        IF_STRCASECMP_TAG ( KBD );
        break;
    case 'L':
        IF_STRCASECMP_TAG ( LI );
        IF_STRCASECMP_TAG ( LEFT );
        break;
    case 'O':
        IF_STRCASECMP_TAG ( OL );
        break;
    case 'P':
        IF_ENDS_TAG ( P );
        else IF_STRCASECMP_TAG ( PRE );

        break;
    case 'R':
        IF_STRCASECMP_TAG ( RIGHT );
        break;
    case 'S':
        IF_STRCASECMP_TAG ( STRONG );
        break;
    case 'T':

        switch ( toupper ( * ( ptrBuf++ ) ) )
        {
        case 'A':
            IF_STRCASECMP_TAG ( TABLE );
            break;
        case 'D':
            IF_ENDS_TAG ( TD );
            break;
        case 'H':
            IF_ENDS_TAG ( TH );
            break;
        case 'I':
            IF_STRCASECMP_TAG ( TITLE );
            break;
        case 'R':
            IF_ENDS_TAG ( TR );
            else if((*(ptrBuf+1) == 0) && toupper ( *ptrBuf) == 'M') {
                ++ptrBuf;
                match_tag = TAG_TRM;
            }
            break;
        case 'T':
            IF_ENDS_TAG ( TT );
            break;
        }

        break;
    case 'U':
        IF_ENDS_TAG ( U );
        else IF_STRCASECMP_TAG ( UL );

        break;
    case 'V':
        IF_STRCASECMP_TAG ( VAR );
        break;
    };

    if ( is_close_tag && ( match_tag != TAG_UNKNOWN ) )
    {
        return match_tag + 1;
    }

    return match_tag;
}

#define ATTR_IS(aname, fnt) if (!strncasecmp(attr, #aname, sizeof(#aname))) return fnt
static int get_attr_font ( char *attr )
{
    switch ( tolower ( *attr ) )
    {
    case 'a':
        ATTR_IS ( "arial", FL_HELVETICA );
        break;
    case 'h':
        ATTR_IS ( "helvetica", FL_HELVETICA );
        break;
    case 's':

        switch ( tolower ( * ( attr + 1 ) ) )
        {
        case 'a':
            ATTR_IS ( "sans", FL_HELVETICA );
            break;
        case 'e':
            ATTR_IS ( "serif", FL_TIMES );
            break;
        case 'y':
            ATTR_IS ( "symbol", FL_SYMBOL );
            break;
        }

        break;
    case 't':
        ATTR_IS ( "times", FL_TIMES );
        break;
    }

    return FL_COURIER;
};

#define PUSH_OR_POP_FONT(afont) \
	if(is_close_tag) do_pop_font = true; \
	else {\
		afont;\
		do_push_font = true;\
	}

void Fl_Help_View::check_font_attr(const char *attrs, Fl_Font &font, Fl_Fontsize &fsize)
{
    char attr[32];
    if ( GET_ATTR("FACE") != NULL )
    {
        font = get_attr_font ( attr );
    }

    if ( GET_ATTR("SIZE") != NULL )
    {
        if ( isdigit ( attr[0] ) )
        {
            // Absolute size
            fsize = ( int ) ( textsize_ * pow ( 1.2, atoi ( attr ) - 3.0 ) );
        }
        else
        {
            // Relative size
            fsize = ( int ) ( fsize * pow ( 1.2, atoi ( attr ) ) );
        }
    }
}

/** Draws the Fl_Help_View widget. */
void
Fl_Help_View::draw()
{
    int			i;		// Looping var
    const Fl_Help_Block	*block;		// Pointer to current block
    const char		*ptr,		// Pointer to text in block
    *attrs;		// Pointer to start of element attributes
    char			*s,		// Pointer into buffer
    buf[1024],	// Text buffer
    attr[1024];	// Attribute buffer
    int			xx, yy, ww, hh;	// Current positions and sizes
    int			line;		// Current line
    Fl_Font               font;
    Fl_Fontsize           fsize;          // Current font and size
    Fl_Color              fcolor;         // current font color
    int			head, pre,	// Flags for text
    needspace;	// Do we need whitespace?
    Fl_Boxtype		b = box() ? box() : FL_DOWN_BOX;
    // Box to draw...
    int			underline,	// Underline text?
    xtra_ww;        // Extra width for underlined space between words

    // Draw the scrollbar(s) and box first...
    ww = w();
    hh = h();
    i  = 0;

    draw_box( b, x(), y(), ww, hh, bgcolor_ );

    if ( hscrollbar_.visible() || scrollbar_.visible() )
    {
        int scrollsize = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();
        int hor_vis = hscrollbar_.visible();
        int ver_vis = scrollbar_.visible();
        // Scrollbar corner
        int scorn_x = x() + ww - ( ver_vis ? scrollsize : 0 ) - Fl::box_dw( b ) + Fl::box_dx( b );
        int scorn_y = y() + hh - ( hor_vis ? scrollsize : 0 ) - Fl::box_dh( b ) + Fl::box_dy( b );

        if ( hor_vis )
        {
            if ( hscrollbar_.h() != scrollsize )  		// scrollsize changed?
            {
                hscrollbar_.resize( x(), scorn_y, scorn_x - x(), scrollsize );
                init_sizes();
            }

            draw_child( hscrollbar_ );
            hh -= scrollsize;
        }

        if ( ver_vis )
        {
            if ( scrollbar_.w() != scrollsize )  		// scrollsize changed?
            {
                scrollbar_.resize( scorn_x, y(), scrollsize, scorn_y - y() );
                init_sizes();
            }

            draw_child( scrollbar_ );
            ww -= scrollsize;
        }

        if ( hor_vis && ver_vis )
        {
            // Both scrollbars visible? Draw little gray box in corner
            fl_color( FL_GRAY );
            fl_rectf( scorn_x, scorn_y, scrollsize, scrollsize );
        }
    }

    if ( !value_ )
        return;

    if ( current_view == this && selected )
    {
        hv_selection_color      = FL_SELECTION_COLOR;
        hv_selection_text_color = fl_contrast( textcolor_, FL_SELECTION_COLOR );
    }

    current_pos = 0;

    // Clip the drawing to the inside of the box...
    fl_push_clip( x() + Fl::box_dx( b ), y() + Fl::box_dy( b ),
                  ww - Fl::box_dw( b ), hh - Fl::box_dh( b ) );
    fl_color( textcolor_ );

    initfont( font, fsize, fcolor );

    // Draw all visible blocks...
    for ( i = 0, block = blocks_; i < nblocks_; i ++, block ++ )
        if ( ( block->y + block->h ) >= topline_ && block->y < ( topline_ + h() ) )
        {
            line      = 0;
            xx        = block->line[line];
            yy        = block->y - topline_;
            hh        = 0;
            pre       = 0;
            head      = 0;
            needspace = 0;
            underline = 0;

//			initfont( font, fsize, fcolor );

            for ( ptr = block->start, s = buf; ptr < block->end; )
            {
                if ( ( *ptr == '<' || isspace( ( *ptr ) ) ) && s > buf )
                {
                    if ( !head && !pre )
                    {
                        // Check width...
                        *s = '\0';
                        s  = buf;
                        ww = ( int )fl_width( buf );
                        if ( needspace && xx > block->x )
                            xx += ( int )fl_width( ' ' );

                        if ( ( xx + ww ) > block->w )
                        {
                            if ( line < line31_ )
                                line ++;

                            xx = block->line[line];
                            yy += hh;
                            hh = 0;
                        }

                        hv_draw( buf, xx + x() - leftline_, yy + y() );

                        if ( underline )
                        {
                            xtra_ww = isspace( ( *ptr ) ) ? ( int )fl_width( ' ' ) : 0;
                            fl_xyline( xx + x() - leftline_, yy + y() + 1,
                                       xx + x() - leftline_ + ww + xtra_ww );
                        }

                        current_pos = ptr - value_;

                        xx += ww;

                        if ( ( fsize + font_increment_ ) > hh )
                            hh = fsize + font_increment_;

                        needspace = 0;
                    }
                    else if ( pre )
                    {
                        while ( isspace( ( *ptr ) ) )
                        {
                            if ( *ptr == '\n' )
                            {
                                *s = '\0';
                                s = buf;

                                hv_draw( buf, xx + x() - leftline_, yy + y() );

                                if ( underline ) fl_xyline( xx + x() - leftline_, yy + y() + 1,
                                                                xx + x() - leftline_ +
                                                                ( int )fl_width( buf ) );

                                current_pos = ptr - value_;

                                if ( line < line31_ )
                                    line ++;

                                xx = block->line[line];
                                yy += hh;
                                hh = fsize + font_increment_;
                            }
                            else if ( *ptr == '\t' )
                            {
                                // Do tabs every 8 columns...
                                while ( ( ( s - buf ) & 7 ) )
                                    *s++ = ' ';
                            }
                            else
                                *s++ = ' ';

                            if ( ( fsize + font_increment_ ) > hh )
                                hh = fsize + font_increment_;

                            ptr ++;
                        }

                        if ( s > buf )
                        {
                            *s = '\0';
                            s = buf;

                            hv_draw( buf, xx + x() - leftline_, yy + y() );
                            ww = ( int )fl_width( buf );

                            if ( underline ) fl_xyline( xx + x() - leftline_, yy + y() + 1,
                                                            xx + x() - leftline_ + ww );

                            xx += ww;
                            current_pos = ptr - value_;
                        }

                        needspace = 0;
                    }
                    else
                    {
                        s = buf;

                        while ( isspace( ( *ptr ) ) )
                            ptr ++;

                        current_pos = ptr - value_;
                    }
                }

                if ( *ptr == '<' )
                {
                    ptr ++;

                    if ( strncmp( ptr, "!--", 3 ) == 0 )
                    {
                        // Comment...
                        ptr += 3;

                        if ( ( ptr = strstr( ptr, "-->" ) ) != NULL )
                        {
                            ptr += 3;
                            continue;
                        }
                        else
                            break;
                    }

                    while ( *ptr && *ptr != '>' && !isspace( ( *ptr ) ) )
                        if ( s < ( buf + sizeof( buf ) - 1 ) )
                            *(s++) = *(ptr++);
                        else
                            ptr ++;

                    *s = '\0';
                    s = buf;

                    attrs = ptr;

                    while ( *ptr && *ptr != '>' )
                        ptr ++;

                    if ( *ptr == '>' )
                        ptr ++;

                    // end of command reached, set the supposed start of printed eord here
                    current_pos = ptr - value_;
                    bool do_push_font = false;
                    bool do_pop_font = false;
                    bool is_close_tag = ( *buf == '/' );
                    const char *ptrBuf = is_close_tag ? buf + 1 : buf;
                    int buf_tag = buffer_has_tag ( ptrBuf );

                    switch ( buf_tag )
                    {
                    case TAG_A:
                        underline = !is_close_tag;

                        if ( underline )
                        {
                            if ( GET_ATTR( "HREF") != NULL )
                            {
                                fl_color ( linkcolor_ );
                            }
                        }
                        else
                        {
                            fl_color ( textcolor_ );
                        }

                        break;

                    case TAG_B:
                    case TAG_STRONG:
                        PUSH_OR_POP_FONT ( font |= FL_BOLD );
                        break;
                    case TAG_BR:

                        if ( line < line31_ )
                        {
                            line ++;
                        }

                        xx = block->line[line];
                        yy += hh;
                        hh = 0;
                        break;
                    case TAG_CENTER:
                        do_push_font = true;
                        break;
                    case TAG_I:
                    case TAG_EM:
                        PUSH_OR_POP_FONT ( font |= FL_ITALIC );
                        break;
                    case TAG_CODE:
                        PUSH_OR_POP_FONT ( font = FL_COURIER );
                        break;
                    case TAG_DD:
                    case TAG_DL:

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }

                        break;
                    case TAG_DT:

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }
                        else
                        {
                            do_push_font = true;
                            font  = textfont_ | FL_ITALIC;
                            //fsize = textsize_;
                        }

                        break;
                    case TAG_FONT:

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }
                        else
                        {
                            do_push_font = true;

                            if ( GET_ATTR( "COLOR") != NULL )
                            {
                                textcolor_ = get_color ( attr, textcolor_ );
                            }
                            check_font_attr(attrs, font, fsize);
                        }

                        break;
                    case TAG_H1:
                    case TAG_H2:
                    case TAG_H3:
                    case TAG_H4:
                    case TAG_H5:
                    case TAG_H6:

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }
                        else
                        {
                            do_push_font = true;
                            font  = FL_HELVETICA_BOLD;
                            //fsize = textsize_ + '7' - buf[1];
                            fsize += '7' - buf[1];
                        }

                        break;
                    case TAG_HEAD:
                        head = !is_close_tag;
                        break;
                    case TAG_BRV:
                        if ( line < line31_ )
                        {
                            line ++;
                        }

                        xx = block->line[line];

                        yy += hh;
                        hh = 0;
                        break;
                    case TAG_HR:
                    {
                        int		height, width, lx1, lx2, half1, half2;
                        char	myattr[8], *mydash, myattr_dash[9];
                        if(get_attr ( attrs, "COLOR", myattr, sizeof ( myattr ) ))
                        {
                            fl_color(get_color( myattr, FL_BLACK ));
                        }
                        if(get_attr ( attrs, "DASH", myattr_dash, sizeof ( myattr_dash ) ))
                        {
                            for(unsigned int i=0; i<sizeof(myattr_dash); i++)
                                if(myattr_dash[i] > '0')
                                    myattr_dash[i] -= '0';
                            mydash = myattr_dash;
                        }
                        else
                        {
                            mydash = NULL;
                        }
                        if(get_attr ( attrs, "HEIGHT", myattr, sizeof ( myattr ) ))
                        {
                            height = atoi ( myattr );
                            fl_line_style(FL_SOLID | FL_CAP_SQUARE, height, mydash);
                        }

                        if(get_attr ( attrs, "WIDTH", myattr, sizeof ( myattr ) ))
                            width = get_length( myattr );
                        else
                            width = block->w + x();

                        switch ( get_align ( attrs, LEFT ) )
                        {
                        default :
                            lx1 = block->x + x();
                            lx2 = width;
                            break;

                        case CENTER :
                            half1 = block->w / 2;
                            half2 = width / 2;
                            lx1 = block->x + x() + half1 - half2;
                            lx2 = block->w + x() - half1 + half2;
                            break;

                        case RIGHT :
                            lx1 = block->w + x() - width;
                            lx2 = block->w + x();
                            break;
                        }

                        fl_line ( lx1, yy + y(), lx2, yy + y() );

                        fl_line_style(0); //reset line style

                        if ( line < line31_ )
                        {
                            line ++;
                        }

                        xx = block->line[line];
                        if(get_attr ( attrs, "VHEIGHT", myattr, sizeof ( myattr ) ))
                        {
                            height = atoi ( myattr );
                            yy += height;
                            hh = 0;
                        }
                        else
                        {
                            yy += 2 * hh;
                            hh = 0;
                        }
                    }
                    break;
                    case TAG_IMG:
                    {
                        Fl_Shared_Image *img = 0;
                        int		width, height;
                        char	wattr[8], hattr[8];


                        get_attr ( attrs, "WIDTH", wattr, sizeof ( wattr ) );
                        get_attr ( attrs, "HEIGHT", hattr, sizeof ( hattr ) );
                        width  = get_length ( wattr );
                        height = get_length ( hattr );

                        if ( GET_ATTR( "SRC") )
                        {
                            img = get_image ( attr, width, height );

                            if ( !width )
                            {
                                width = img->w();
                            }

                            if ( !height )
                            {
                                height = img->h();
                            }
                        }

                        if ( !width || !height )
                        {
                            if ( GET_ATTR( "ALT") == NULL )
                            {
                                strcpy ( attr, "IMG" );
                            }
                        }

                        ww = width;

                        if ( needspace && xx > block->x )
                        {
                            xx += ( int ) fl_width ( ' ' );
                        }

                        if ( ( xx + ww ) > block->w )
                        {
                            if ( line < line31_ )
                            {
                                line ++;
                            }

                            xx = block->line[line];
                            yy += hh;
                            hh = 0;
                        }

                        if ( img )
                            img->draw ( xx + x() - leftline_,
                                        yy + y() - fl_height() + fl_descent() + 2 );
                        xx += ww;

                        if ( ( height + 2 ) > hh )
                        {
                            hh = height + 2;
                        }

                        needspace = 0;
                    }
                    break;
                    case TAG_KBD:
                        PUSH_OR_POP_FONT ( font = FL_COURIER_BOLD );
                        break;
                    case TAG_LI:

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }
                        else
                        {
                            do_push_font = true;
                            //fl_font(FL_SYMBOL, fsize); // The default SYMBOL font on my XP box is not Unicode...
                            char buf[8];
                            wchar_t b[] = {0x2022, 0x0};
                            //buf[fl_unicode2utf(b, 1, buf)] = 0;
                            unsigned dstlen = fl_utf8fromwc ( buf, 8, b, 1 );
                            buf[dstlen] = 0;
                            hv_draw ( buf, xx - fsize + x() - leftline_, yy + y() );
                        }

                        break;
                    case TAG_OL:
                    case TAG_P:

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }
                        else
                        {
                            do_push_font = true;
                        }

                        break;
                    case TAG_PRE:
                        pre = !is_close_tag;

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }
                        else
                        {
                            do_push_font = true;
                            font  = FL_COURIER;
                            //fsize = textsize_;
                        }

                        break;
                    case TAG_TH:
                    case TAG_TD:

                        if ( is_close_tag )
                        {
                            do_pop_font = true;
                        }
                        else
                        {
                            do_push_font = true;
                            int tx, ty, tw, th;

                            if ( buf_tag == TAG_TH )
                            {
                                font |= FL_BOLD;
                            }
                            else
                            {
                                font = textfont_;
                            }
                            int fsi = fsize + font_increment_;
                            if(fsi%2) fsi++;
                            tx = block->x /*- 4*/ - leftline_;
                            ty = block->y - topline_ - fsi+td_width_adjust_; // - td_top_gap_;
                            tw = block->w - block->x+td_width_adjust_; // + 7;
                            th = block->h + fsi / 2 - tr_gap_ +td_width_adjust_;

                            if ( tx < 0 )
                            {
                                tw += tx;
                                tx  = 0;
                            }

                            if ( ty < 0 )
                            {
                                th += ty;
                                ty  = 0;
                            }

                            tx += x();
                            ty += y();

                            if ( block->bgcolor != bgcolor_ )
                            {
                                fl_color ( block->bgcolor );
                                fl_rectf ( tx, ty, tw, th );
                                fl_color ( textcolor_ );
                            }

                            if ( block->border )
                            {
                                fl_color ( block->border_color );
                                fl_line_style(FL_SOLID | FL_CAP_SQUARE, block->border, NULL);
                                fl_rect ( tx, ty, tw, th );
                                fl_line_style(0);
                                fl_color ( textcolor_ );
                            }
                        }

                        break;
                    case TAG_TT:
                        PUSH_OR_POP_FONT ( font = FL_COURIER );
                        break;
                    case TAG_VAR:
                        PUSH_OR_POP_FONT ( font = FL_COURIER_ITALIC );
                        break;
                    case TAG_U:
                        underline = !is_close_tag;
                        break;
                    case TAG_UL:

                        if ( is_close_tag )
                        {
                            do_push_font = true;
                        }

                        break;
                    };

                    if ( do_push_font )
                    {
                        pushfont ( font, fsize );
                    }
                    else if ( do_pop_font )
                    {
                        popfont ( font, fsize, fcolor );
                    }
                }
                else if ( *ptr == '\n' && pre )
                {
                    *s = '\0';
                    s = buf;

                    hv_draw( buf, xx + x() - leftline_, yy + y() );

                    if ( line < line31_ )
                        line ++;

                    xx = block->line[line];
                    yy += hh;
                    hh = fsize + font_increment_;
                    needspace = 0;

                    ptr ++;
                    current_pos = ptr - value_;
                }
                else if ( isspace( ( *ptr ) ) )
                {
                    if ( pre )
                    {
                        if ( *ptr == ' ' )
                            *s++ = ' ';
                        else
                        {
                            // Do tabs every 8 columns...
                            while ( ( ( s - buf ) & 7 ) )
                                *s++ = ' ';
                        }
                    }

                    ptr ++;

                    if ( !pre ) current_pos = ptr - value_;

                    needspace = 1;
                }
                else if ( *ptr == '&' )
                {
                    ptr ++;

                    int qch = quote_char( ptr );

                    if ( qch < 0 )
                        *s++ = '&';
                    else
                    {
                        int l;
                        l = fl_utf8encode( ( unsigned int ) qch, s );

                        if ( l < 1 ) l = 1;

                        s += l;
                        ptr = strchr( ptr, ';' ) + 1;
                    }

                    if ( ( fsize + font_increment_ ) > hh )
                        hh = fsize + font_increment_;
                }
                else
                {
                    *s++ = *ptr++;

                    if ( ( fsize + font_increment_ ) > hh )
                        hh = fsize + font_increment_;
                }
            }

            *s = '\0';

            if ( s > buf && !pre && !head )
            {
                ww = ( int )fl_width( buf );

                if ( needspace && xx > block->x )
                    xx += ( int )fl_width( ' ' );

                if ( ( xx + ww ) > block->w )
                {
                    if ( line < line31_ )
                        line ++;

                    xx = block->line[line];
                    yy += hh;
                    hh = 0;
                }
            }

            if ( s > buf && !head )
            {
                hv_draw( buf, xx + x() - leftline_, yy + y() );

                if ( underline ) fl_xyline( xx + x() - leftline_, yy + y() + 1,
                                                xx + x() - leftline_ + ww );

                current_pos = ptr - value_;
            }
        }

    fl_pop_clip();
}



/** Finds the specified string \p s at starting position \p p.

    \return the matching position or -1 if not found
*/
int						// O - Matching position or -1 if not found
Fl_Help_View::find( const char *s,		// I - String to find
                    int        p )  	// I - Starting position
{
    int		i,				// Looping var
    c;				// Current character
    Fl_Help_Block	*b;				// Current block
    const char	*bp,				// Block matching pointer
    *bs,				// Start of current comparison
    *sp;				// Search string pointer


    // Range check input and value...
    if ( !s || !value_ ) return -1;

    if ( p < 0 || p >= ( int )strlen( value_ ) ) p = 0;
    else if ( p > 0 ) p ++;

    // Look for the string...
    for ( i = nblocks_, b = blocks_; i > 0; i --, b ++ )
    {
        if ( b->end < ( value_ + p ) )
            continue;

        if ( b->start < ( value_ + p ) ) bp = value_ + p;
        else bp = b->start;

        for ( sp = s, bs = bp; *sp && *bp && bp < b->end; bp ++ )
        {
            if ( *bp == '<' )
            {
                // skip to end of element...
                while ( *bp && bp < b->end && *bp != '>' ) bp ++;

                continue;
            }
            else if ( *bp == '&' )
            {
                // decode HTML entity...
                if ( ( c = quote_char( bp + 1 ) ) < 0 ) c = '&';
                else bp = strchr( bp + 1, ';' ) + 1;
            }
            else c = *bp;

            if ( tolower( *sp ) == tolower( c ) ) sp ++;
            else
            {
                // No match, so reset to start of search...
                sp = s;
                bs ++;
                bp = bs;
            }
        }

        if ( !*sp )
        {
            // Found a match!
            topline( b->y - b->h );
            return ( b->end - value_ );
        }
    }

    // No match!
    return ( -1 );
}

/** Formats the help text. */
void Fl_Help_View::format()
{
    int		i;		// Looping var
    int		done;		// Are we done yet?
    Fl_Help_Block	*block,		// Current block
    *cell;		// Current table cell
    int		cells[MAX_COLUMNS],
    // Cells in the current row...
    row;		// Current table row (block number)
    const char	*ptr,		// Pointer into block
    *start,		// Pointer to start of element
    *attrs;		// Pointer to start of element attributes
    char		*s,		// Pointer into buffer
    buf[1024],	// Text buffer
    attr[1024],	// Attribute buffer
    wattr[1024],	// Width attribute buffer
    hattr[1024],	// Height attribute buffer
    linkdest[1024];	// Link destination
    int		xx, yy, ww, hh;	// Size of current text fragment
    int		line;		// Current line in block
    int		links;		// Links for current line
    Fl_Font       font;
    Fl_Fontsize   fsize;          // Current font and size
    Fl_Color      fcolor;         // Current font color
    unsigned char	border;		// Draw border?
    int		talign,		// Current alignment
    newalign,	// New alignment
    head,		// In the <HEAD> section?
    pre,		// <PRE> text?
    needspace;	// Do we need whitespace?
    int		table_width,	// Width of table
    table_offset;	// Offset of table
    int		column,		// Current table column number
    columns[MAX_COLUMNS];
    // Column widths
    Fl_Color	tc, rc;		// Table/row background color
    Fl_Color border_color; //Table border color;
    Fl_Boxtype	b = box() ? box() : FL_DOWN_BOX;
    // Box to draw...
    fl_margins	margins;	// Left margin stack...


    // Reset document width...
    int scrollsize = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();
    hsize_ = w() - scrollsize - Fl::box_dw( b );

    done = 0;

    initfont( font, fsize, fcolor );

    while ( !done )
    {
        // Reset state variables...
        done       = 1;
        nblocks_   = 0;
        nlinks_    = 0;
        ntargets_  = 0;
        size_      = 0;
        bgcolor_   = color();
        textcolor_ = textcolor();
        linkcolor_ = fl_contrast( FL_BLUE, color() );

        tc = rc = bgcolor_;

        strcpy( title_, "Untitled" );

        if ( !value_ )
            return;

        // Setup for formatting...
        //cause infinite loop without it
        initfont( font, fsize, fcolor );

        line         = 0;
        links        = 0;
        xx           = margins.clear();
        yy           = fsize + font_increment_;
        ww           = 0;
        column       = 0;
        border       = 0;
        hh           = 0;
        block        = add_block( value_, xx, yy, hsize_, 0 );
        row          = 0;
        head         = 0;
        pre          = 0;
        talign       = LEFT;
        newalign     = LEFT;
        needspace    = 0;
        linkdest[0]  = '\0';
        table_offset = 0;
        border_color = FL_BLACK;

        // Html text character loop
        for ( ptr = value_, s = buf; *ptr; )
        {
            // End of word?
            if ( ( *ptr == '<' || isspace( ( *ptr ) ) ) && s > buf )
            {
                // Get width of word parsed so far...
                *s = '\0';
                ww = ( int )fl_width( buf );

                if ( !head && !pre )
                {
                    // Check width...
                    if ( ww > hsize_ )
                    {
                        hsize_ = ww;
                        done = 0;
                        break;
                    }

                    if ( needspace && xx > block->x )
                        ww += ( int )fl_width( ' ' );

                    //        printf("line = %d, xx = %d, ww = %d, block->x = %d, block->w = %d\n",
                    //	       line, xx, ww, block->x, block->w);

                    if ( ( xx + ww ) > block->w )
                    {
                        line     = do_align( block, line, xx, newalign, links );
                        xx       = block->x;
                        yy       += hh;
                        block->h += hh;
                        hh       = 0;
                    }

                    if ( linkdest[0] )
                        add_link( linkdest, xx, yy - fsize, ww, fsize );

                    xx += ww;

                    if ( ( fsize + font_increment_ ) > hh )
                        hh = fsize + font_increment_;

                    needspace = 0;
                }
                else if ( pre )
                {
                    // Add a link as needed...
                    if ( linkdest[0] )
                        add_link( linkdest, xx, yy - hh, ww, hh );

                    xx += ww;

                    if ( ( fsize + font_increment_ ) > hh )
                        hh = fsize + font_increment_;

                    // Handle preformatted text...
                    while ( isspace( ( *ptr ) ) )
                    {
                        if ( *ptr == '\n' )
                        {
                            if ( xx > hsize_ ) break;

                            line     = do_align( block, line, xx, newalign, links );
                            xx       = block->x;
                            yy       += hh;
                            block->h += hh;
                            hh       = fsize + font_increment_;
                        }
                        else
                            xx += ( int )fl_width( ' ' );

                        if ( ( fsize + font_increment_ ) > hh )
                            hh = fsize + font_increment_;

                        ptr ++;
                    }

                    if ( xx > hsize_ )
                    {
                        hsize_ = xx;
                        done   = 0;
                        break;
                    }

                    needspace = 0;
                }
                else
                {
                    // Handle normal text or stuff in the <HEAD> section...
                    while ( isspace( ( *ptr ) ) )
                        ptr ++;
                }

                s = buf;
            }

            if ( *ptr == '<' )
            {
                // Handle html tags..
                start = ptr;
                ptr ++;

                if ( strncmp( ptr, "!--", 3 ) == 0 )
                {
                    // Comment...
                    ptr += 3;

                    if ( ( ptr = strstr( ptr, "-->" ) ) != NULL )
                    {
                        ptr += 3;
                        continue;
                    }
                    else
                        break;
                }

                while ( *ptr && *ptr != '>' && !isspace( ( *ptr ) ) )
                    if ( s < ( buf + sizeof( buf ) - 1 ) )
                        *s++ = *ptr++;
                    else
                        ptr ++;

                *s = '\0';
                s = buf;

//        puts(buf);

                attrs = ptr;

                while ( *ptr && *ptr != '>' )
                    ptr ++;

                if ( *ptr == '>' )
                    ptr ++;

                bool is_close_tag = ( *buf == '/' );
                const char *ptrBuf = is_close_tag ? buf + 1 : buf;
                bool do_pop_font = false;
                bool do_push_font = false;
                bool do_break_loop = false;

                int buf_tag = buffer_has_tag ( ptrBuf );

                switch ( buf_tag )
                {
                case TAG_A:

                    if ( is_close_tag )
                    {
                        linkdest[0] = '\0';
                    }
                    else
                    {
                        if ( GET_ATTR( "NAME") != NULL )
                        {
                            add_target ( attr, yy - fsize - font_increment_ );
                        }

                        if ( GET_ATTR( "HREF") != NULL )
                        {
                            strlcpy ( linkdest, attr, sizeof ( linkdest ) );
                        }
                    }

                    break;
                case TAG_B:
                case TAG_STRONG:
                    PUSH_OR_POP_FONT ( font |= FL_BOLD );
                    break;
                case TAG_BODY:
                    bgcolor_   = get_color ( GET_ATTR( "BGCOLOR"),
                                             color() );
                    textcolor_ = get_color ( GET_ATTR( "TEXT"),
                                             textcolor() );
                    linkcolor_ = get_color ( GET_ATTR( "LINK"),
                                             fl_contrast ( FL_BLUE, color() ) );
                    break;
                case TAG_BR:
                    line	 = do_align ( block, line, xx, newalign, links );
                    xx	   = block->x;
                    block->h += hh;
                    yy	   += hh;
                    hh	   = 0;
                    break;
                case TAG_CODE:
                case TAG_TT:
                    PUSH_OR_POP_FONT ( font = FL_COURIER );
                    break;
                case TAG_I:
                case TAG_EM:
                    PUSH_OR_POP_FONT ( font |= FL_ITALIC );
                    break;
                case TAG_FONT:

                    if ( is_close_tag )
                    {
                        do_pop_font = true;
                    }
                    else
                    {
                        do_push_font = true;
                        check_font_attr(attrs, font, fsize);
                    }

                    break;
                case TAG_CENTER:
                case TAG_DD:
                case TAG_DL:
                case TAG_DT:
                case TAG_H1:
                case TAG_H2:
                case TAG_H3:
                case TAG_H4:
                case TAG_H5:
                case TAG_H6:
                case TAG_BRV:
                case TAG_HR:
                case TAG_PRE:
                case TAG_LI:
                case TAG_OL:
                case TAG_TABLE:
                case TAG_UL:
                    if ( GET_ATTR( "ID") != NULL ) //add anchor for an ID
                    {
                        add_target ( attr, yy + font_increment_ );
                    }

                    if ( is_close_tag )
                    {
                        do_pop_font = true;
                        line	   = do_align ( block, line, xx, newalign, links );
                        xx		 = block->x;
                        block->end = ptr;

                        switch ( buf_tag )
                        {
                        case TAG_DL:
                        case TAG_OL:
                        case TAG_UL:
                            xx	   = margins.pop();
                            block->h += fsize + font_increment_;
                            break;
                        case TAG_TABLE:
                            block->h += fsize + font_increment_;
                            xx	   = margins.current();
                            break;
                        case TAG_PRE:
                            pre = 0;
                            hh  = 0;
                            break;
                        case TAG_CENTER:
                            talign = LEFT;
                            break;
                        }

                        //#if defined(__GNUC__)
                        //#warning FIXME this isspace & 255 test will probably not work on a utf8 stream... And we use it everywhere!
                        //#endif /*__GNUC__*/
                        while ( isspace ( ( *ptr ) ) )
                        {
                            ptr ++;
                        }

                        if(GET_ATTR( "BGAP"))
                        {
                            int bgap = atoi ( attr );
                            block->h += bgap;
                            yy	   += bgap;
                        }
                        else
                        {
                            block->h += hh;
                            yy	   += hh;
                        }

                        //DAD changing y coordinate when closing h1 ??
                        //if ( buf_tag == TAG_H1 /*tolower ( buf[2] ) == 'l'*/ ) {
                        //	yy += fsize + font_increment_;
                        //}

                        if ( row )
                        {
                            block = add_block ( ptr, xx, yy, block->w, 0 );
                        }
                        else
                        {
                            block = add_block ( ptr, xx, yy, hsize_, 0 );
                        }

                        needspace = 0;
                        hh		= 0;
                        line	  = 0;
                        newalign  = talign;
                    }
                    else
                    {
                        do_push_font = true;
                        block->end = start;
                        line	   = do_align ( block, line, xx, newalign, links );
                        newalign   = buf_tag == TAG_CENTER ? LEFT : CENTER;
                        xx		 = block->x;
                        block->h   += hh;

                        switch ( buf_tag )
                        {
                        case TAG_UL:
                        case TAG_OL:
                        case TAG_DL:
                            block->h += fsize + font_increment_;
                            xx	   = margins.push ( 4 * fsize );
                            break;
                        case TAG_TABLE:

                            if ( GET_ATTR( "BORDER") )
                            {
                                border = ( uchar ) atoi ( attr );
                            }
                            else
                            {
                                border = 0;
                            }

                            tc = rc = get_color ( GET_ATTR( "BGCOLOR"), bgcolor_ );
                            border_color = get_color ( GET_ATTR( "BORDERCOLOR"), FL_BLACK );

                            block->h += fsize + font_increment_;

                            format_table ( &table_width, columns, start );

                            if ( ( xx + table_width ) > hsize_ )
                            {
#ifdef DEBUG
                                printf ( "xx=%d, table_width=%d, hsize_=%d\n", xx, table_width,
                                         hsize_ );
#endif // DEBUG
                                hsize_ = xx + table_width;
                                done   = 0;
                                do_break_loop = true;
                                goto end_tag_switch;
                            }

                            switch ( get_align ( attrs, talign ) )
                            {
                            default :
                                //table_offset = -12;
                                table_offset = fl_width(' ');
                                break;

                            case CENTER :
                                table_offset = ( hsize_ - table_width ) / 2 - fsize;
                                break;

                            case RIGHT :
                                table_offset = hsize_ - table_width - fsize;
                                break;
                            }

                            column = 0;
                            break;
                        case TAG_H1:
                        case TAG_H2:
                        case TAG_H3:
                        case TAG_H4:
                        case TAG_H5:
                        case TAG_H6:
                            font  = FL_HELVETICA_BOLD;
                            fsize = fsize + '7' - buf[1];
                            break;
                        case TAG_DT:
                            font  = textfont_ | FL_ITALIC;
                            //fsize = textsize_;
                            break;
                        case TAG_PRE:
                            font  = FL_COURIER;
                            //fsize = textsize_;
                            pre   = 1;
                            break;
                        default:
                            font  = textfont_;
                            //fsize = textsize_;
                        }

                        yy = block->y + block->h;
                        hh = 0;

                        switch ( buf_tag )
                        {
                        case TAG_H1:
                        case TAG_H2:
                        case TAG_H3:
                        case TAG_H4:
                        case TAG_H5:
                        case TAG_H6:
                        case TAG_DD:
                        case TAG_DT:
                        case TAG_P:
                            yy += fsize + font_increment_;
                            break;
                        case TAG_BRV:
                            int		height;
                            char	hattr[8];
                            if(get_attr ( attrs, "HEIGHT", hattr, sizeof ( hattr ) ))
                            {
                                height = atoi ( hattr );
                                hh += height;
                                yy += height;
                            }
                            else
                            {
                                hh += fsize;
                                yy += fsize;
                            }
                            break;
                        case TAG_HR:
                            if(GET_ATTR( "VHEIGHT"))
                            {
                                height = atoi ( attr );
                                yy += height;
                                hh += height;
                            }
                            else
                            {
                                hh += 2 * fsize;
                                yy += fsize;
                            }
                            break;
                        }

                        if ( row )
                        {
                            block = add_block ( start, xx, yy, block->w, 0 );
                        }
                        else
                        {
                            block = add_block ( start, xx, yy, hsize_, 0 );
                        }

                        needspace = 0;
                        line	  = 0;

                        if ( buf_tag == TAG_CENTER )
                        {
                            newalign = talign = CENTER;
                        }
                        else
                        {
                            newalign = get_align ( attrs, talign );
                        }
                    }

                    break;
                case TAG_HEAD:
                    head = !is_close_tag;
                    break;
                case TAG_IMG:
                {
                    Fl_Shared_Image	*img = 0;
                    int		width;
                    int		height;


                    get_attr ( attrs, "WIDTH", wattr, sizeof ( wattr ) );
                    get_attr ( attrs, "HEIGHT", hattr, sizeof ( hattr ) );
                    width  = get_length ( wattr );
                    height = get_length ( hattr );

                    if ( GET_ATTR( "SRC") )
                    {
                        img	= get_image ( attr, width, height );
                        width  = img->w();
                        height = img->h();
                    }
                    ww = width;

                    if ( ww > hsize_ )
                    {
                        hsize_ = ww;
                        done   = 0;
                        do_break_loop = true;
                        break;
                    }

                    if ( needspace && xx > block->x )
                    {
                        ww += ( int ) fl_width ( ' ' );
                    }

                    if ( ( xx + ww ) > block->w )
                    {
                        line	 = do_align ( block, line, xx, newalign, links );
                        xx	   = block->x;
                        yy	   += hh;
                        block->h += hh;
                        hh	   = 0;
                    }

                    if ( linkdest[0] )
                    {
                        add_link ( linkdest, xx, yy - height, ww, height );
                    }

                    xx += ww;
                    if ( ( height + 2 ) > hh )
                    {
                        hh = height + 2;
                    }
                    needspace = 0;
                }
                break;
                case TAG_KBD:
                    PUSH_OR_POP_FONT ( font = FL_COURIER_BOLD );
                    break;
                case TAG_TITLE:

                    // Copy the title in the document...
                    for ( s = title_;
                            *ptr != '<' && *ptr && s < ( title_ + sizeof ( title_ ) - 1 );
                            *s++ = *ptr++ );

                    *s = '\0';
                    s = buf;
                    break;
                case TAG_TH:
                case TAG_TD:

                    if ( !row )
                    {
                        break;
                    }

                    if ( is_close_tag )
                    {
                        line = do_align ( block, line, xx, newalign, links );
                        popfont ( font, fsize, fcolor );
                        xx = margins.pop();
                        talign = LEFT;
                    }
                    else
                    {
                        int	colspan;		// COLSPAN attribute

                        line	   = do_align ( block, line, xx, newalign, links );
                        block->end = start;
                        block->h   += hh; // - tr_gap_;

                        if ( buf_tag == TAG_TH )
                        {
                            font = textfont_ | FL_BOLD;
                        }
                        else
                        {
                            font = textfont_;
                        }

                        //fsize = textsize_;

                        //xx = blocks_[row].x + fsize + 3 + table_offset;
                        //min left margin
                        xx = blocks_[row].x + table_offset;

                        for ( i = 0; i < column; i ++ )
                        {
                            xx += columns[i]; // + 6;
                        }

                        margins.push ( xx - margins.current() );


                        if ( GET_ATTR( "COLSPAN") != NULL )
                        {
                            colspan = atoi ( attr );
                        }
                        else
                        {
                            colspan = 1;
                        }

                        //for ( i = 0, ww = -6; i < colspan; i ++ ) {
                        //	ww += columns[column + i] + 6;
                        //}
                        for ( i = 0, ww = 0; i < colspan; i ++ )
                        {
                            ww += columns[column + i] ;
                        }

                        if ( block->end == block->start && nblocks_ > 1 )
                        {
                            nblocks_ --;
                            block --;
                        }

                        pushfont ( font, fsize );

                        yy		= blocks_[row].y;
                        hh		= 0;
                        block	 = add_block ( start, xx , yy, xx + ww, 0, border, border_color );
                        xx += TD_MARGIN;
                        needspace = 0;
                        line	  = 0;
                        newalign  = get_align ( attrs, buf_tag == TAG_TH ? CENTER : LEFT );
                        talign	= newalign;

                        cells[column] = block - blocks_;

                        column += colspan;

                        block->bgcolor = get_color ( get_attr ( attrs, "BGCOLOR", attr,
                                                                sizeof ( attr ) ), rc );
                    }

                    break;
                case TAG_TR:
                case TAG_TRM:
                    //if(TAG_TRM) buf_tag = TAG_TR;
                    if ( is_close_tag )
                    {
                        line	   = do_align ( block, line, xx, newalign, links );
                        block->end = start;
                        block->h   += hh;
                        talign	 = LEFT;

                        xx = blocks_[row].x;
                        yy = blocks_[row].y + blocks_[row].h;

                        for ( cell = blocks_ + row + 1; cell <= block; cell ++ )
                            if ( ( cell->y + cell->h ) > yy )
                            {
                                yy = cell->y + cell->h;
                            }

                        block = blocks_ + row;

                        block->h = yy - block->y; // + tr_gap_;

                        for ( i = 0; i < column; i ++ )
                            if ( cells[i] )
                            {
                                cell = blocks_ + cells[i];
                                cell->h = block->h;
                            }

                        yy		= block->y + block->h - (fsize + font_increment_)/2; // - (tr_gap_*2);
                        block	 = add_block ( start, xx, yy, hsize_, 0 );
                        needspace = 0;
                        row	   = 0;
                        hh = fsize + font_increment_;
                        line	  = 0;
                    }
                    else     //opentag
                    {
                        block->end = start;
                        line	   = do_align ( block, line, xx, newalign, links );
                        xx		 = block->x;
                        block->h   += hh;

                        if ( row )
                        {
                            yy = blocks_[row].y + blocks_[row].h;

                            for ( cell = blocks_ + row + 1; cell <= block; cell ++ )
                                if ( ( cell->y + cell->h ) > yy )
                                {
                                    yy = cell->y + cell->h;
                                }

                            block = blocks_ + row;

                            block->h = yy - block->y; // + tr_gap_;

                            for ( i = 0; i < column; i ++ )
                                if ( cells[i] )
                                {
                                    cell = blocks_ + cells[i];
                                    cell->h = block->h;
                                }
                        }

                        memset ( cells, 0, sizeof ( cells ) );

                        yy		= block->y + block->h - tr_gap_;
                        hh		= 0;
                        block	 = add_block ( start, xx, yy, hsize_, 0 );
                        row	   = block - blocks_;
                        needspace = 0;
                        column	= 0;
                        line	  = 0;

                        rc = get_color ( GET_ATTR( "BGCOLOR"), tc );
                    }

                    break;
                case TAG_VAR:

                    if ( is_close_tag )
                    {
                        popfont ( font, fsize, fcolor );
                    }
                    else
                    {
                        pushfont ( font = FL_COURIER_ITALIC, fsize );
                    }

                    break;
                };

end_tag_switch:
                if ( do_push_font )
                {
                    pushfont ( font, fsize );
                }
                else if ( do_pop_font )
                {
                    popfont ( font, fsize, fcolor );
                }

                if ( do_break_loop )
                {
                    break;
                }
            }
            else if ( *ptr == '\n' && pre )
            {
                if ( linkdest[0] )
                    add_link( linkdest, xx, yy - hh, ww, hh );

                if ( xx > hsize_ )
                {
                    hsize_ = xx;
                    done   = 0;
                    break;
                }

                line      = do_align( block, line, xx, newalign, links );
                xx        = block->x;
                yy        += hh;
                block->h  += hh;
                needspace = 0;
                ptr ++;
            }
            else if ( isspace( ( *ptr ) ) )
            {
                needspace = 1;

                if ( pre )
                {
                    xx += ( int )fl_width( ' ' );
                }

                ptr ++;
            }
            else if ( *ptr == '&' && s < ( buf + sizeof( buf ) - 1 ) )
            {
                // Handle html '&' codes, eg. "&amp;"
                ptr ++;

                int qch = quote_char( ptr );

                if ( qch < 0 )
                    *s++ = '&';
                else
                {
                    int l;
                    l = fl_utf8encode( ( unsigned int ) qch, s );

                    if ( l < 1 ) l = 1;

                    s += l;
                    ptr = strchr( ptr, ';' ) + 1;
                }

                if ( ( fsize + font_increment_ ) > hh )
                    hh = fsize + font_increment_;
            }
            else
            {
                if ( s < ( buf + sizeof( buf ) - 1 ) )
                    *s++ = *ptr++;
                else
                    ptr ++;

                if ( ( fsize + font_increment_ ) > hh )
                    hh = fsize + font_increment_;
            }
        }

        if ( s > buf && !head )
        {
            *s = '\0';
            ww = ( int )fl_width( buf );

            //    printf("line = %d, xx = %d, ww = %d, block->x = %d, block->w = %d\n",
            //	   line, xx, ww, block->x, block->w);

            if ( ww > hsize_ )
            {
                hsize_ = ww;
                done   = 0;
                break;
            }

            if ( needspace && xx > block->x )
                ww += ( int )fl_width( ' ' );

            if ( ( xx + ww ) > block->w )
            {
                line     = do_align( block, line, xx, newalign, links );
                xx       = block->x;
                yy       += hh;
                block->h += hh;
                hh       = 0;
            }

            if ( linkdest[0] )
                add_link( linkdest, xx, yy - fsize, ww, fsize );

            xx += ww;
        }

        do_align( block, line, xx, newalign, links );

        block->end = ptr;
        size_      = yy + hh;
    }

//  printf("margins.depth_=%d\n", margins.depth_);

    if ( ntargets_ > 1 )
        qsort( targets_, ntargets_, sizeof( Fl_Help_Target ),
               ( compare_func_t )compare_targets );

    int dx = Fl::box_dw( b ) - Fl::box_dx( b );
    int dy = Fl::box_dh( b ) - Fl::box_dy( b );
    int ss = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();
    int dw = Fl::box_dw( b ) + ss;
    int dh = Fl::box_dh( b );

    if ( hsize_ > ( w() - dw ) )
    {
        hscrollbar_.show();

        dh += ss;

        if ( size_ < ( h() - dh ) )
        {
            scrollbar_.hide();
            hscrollbar_.resize( x() + Fl::box_dx( b ), y() + h() - ss - dy,
                                w() - Fl::box_dw( b ), ss );
        }
        else
        {
            scrollbar_.show();
            scrollbar_.resize( x() + w() - ss - dx, y() + Fl::box_dy( b ),
                               ss, h() - ss - Fl::box_dh( b ) );
            hscrollbar_.resize( x() + Fl::box_dx( b ), y() + h() - ss - dy,
                                w() - ss - Fl::box_dw( b ), ss );
        }
    }
    else
    {
        hscrollbar_.hide();

        if ( size_ < ( h() - dh ) ) scrollbar_.hide();
        else
        {
            scrollbar_.resize( x() + w() - ss - dx, y() + Fl::box_dy( b ),
                               ss, h() - Fl::box_dh( b ) );
            scrollbar_.show();
        }
    }

    // Reset scrolling if it needs to be...
    if ( scrollbar_.visible() )
    {
        int temph = h() - Fl::box_dh( b );

        if ( hscrollbar_.visible() ) temph -= ss;

        if ( ( topline_ + temph ) > size_ ) topline( size_ - temph );
        else topline( topline_ );
    }
    else topline( 0 );

    if ( hscrollbar_.visible() )
    {
        int tempw = w() - ss - Fl::box_dw( b );

        if ( ( leftline_ + tempw ) > hsize_ ) leftline( hsize_ - tempw );
        else leftline( leftline_ );
    }
    else leftline( 0 );
}


/** Formats a table */
void
Fl_Help_View::format_table( int        *table_width,	// O - Total table width
                            int        *columns,		// O - Column widths
                            const char *table )  	// I - Pointer to start of table
{
    int		column,					// Current column
    num_columns,				// Number of columns
    colspan,				// COLSPAN attribute
    width,					// Current width
    temp_width,				// Temporary width
    max_width,				// Maximum width
    incell,					// In a table cell?
    pre,					// <PRE> text?
    needspace;				// Need whitespace?
    char		*s,					// Pointer into buffer
    buf[1024],				// Text buffer
    attr[1024],				// Other attribute
    wattr[1024],				// WIDTH attribute
    hattr[1024];				// HEIGHT attribute
    const char	*ptr,					// Pointer into table
    *attrs,					// Pointer to attributes
    *start;					// Start of element
    int		minwidths[MAX_COLUMNS];			// Minimum widths for each column
    Fl_Font       font;
    Fl_Fontsize   fsize;				        // Current font and size
    Fl_Color      fcolor;                                 // Currrent font color

    // Clear widths...
    *table_width = 0;

    for ( column = 0; column < MAX_COLUMNS; column ++ )
    {
        columns[column]   = 0;
        minwidths[column] = 0;
    }

    num_columns = 0;
    colspan     = 0;
    max_width   = 0;
    pre         = 0;
    needspace   = 0;
    fstack_.top( font, fsize, fcolor );

    // Scan the table...
    for ( ptr = table, column = -1, width = 0, s = buf, incell = 0; *ptr; )
    {
        if ( ( *ptr == '<' || isspace( ( *ptr ) ) ) && s > buf && incell )
        {
            // Check width...
            if ( needspace )
            {
                *s++      = ' ';
                needspace = 0;
            }

            *s         = '\0';
            temp_width = ( int )fl_width( buf ) + TD_MARGIN * 2;
            s          = buf;

            if ( temp_width > minwidths[column] )
                minwidths[column] = temp_width;

            width += temp_width;

            if ( width > max_width )
                max_width = width;
        }

        if ( *ptr == '<' )
        {
            start = ptr;

            for ( s = buf, ptr ++; *ptr && *ptr != '>' && !isspace( ( *ptr ) ); )
                if ( s < ( buf + sizeof( buf ) - 1 ) )
                    *s++ = *ptr++;
                else
                    ptr ++;

            *s = '\0';
            s = buf;

            attrs = ptr;

            while ( *ptr && *ptr != '>' )
                ptr ++;

            if ( *ptr == '>' )
                ptr ++;

            bool do_break_loop = false;
            bool do_push_font = false;
            bool do_pop_font = false;
            bool is_close_tag = ( *buf == '/' );
            const char *ptrBuf = is_close_tag ? buf + 1 : buf;
            int buf_tag = buffer_has_tag ( ptrBuf );

            switch ( buf_tag )
            {
            case TAG_B:
            case TAG_STRONG:
                PUSH_OR_POP_FONT ( font |= FL_BOLD );
                break;
            case TAG_CODE:
            case TAG_TT:
                PUSH_OR_POP_FONT ( font = FL_COURIER );
                break;
            case TAG_I:
            case TAG_EM:
                PUSH_OR_POP_FONT ( font |= FL_ITALIC );
                break;
            case TAG_KBD:
                PUSH_OR_POP_FONT ( font = FL_COURIER_BOLD );
                break;
            case TAG_VAR:
                PUSH_OR_POP_FONT ( font = FL_COURIER_ITALIC );
                break;
            case TAG_BR:
                width	 = 0;
                needspace = 0;
                break;
            case TAG_CENTER:
            case TAG_DD:
            case TAG_DL:
            case TAG_DT:
            case TAG_H1:
            case TAG_H2:
            case TAG_H3:
            case TAG_H4:
            case TAG_H5:
            case TAG_H6:
            case TAG_LI:
            case TAG_PRE:
            case TAG_OL:
            case TAG_P:
            case TAG_UL:

                if ( is_close_tag )
                {
                    width	 = 0;
                    needspace = 0;
                    popfont ( font, fsize, fcolor );
                }
                else
                {
                    width	 = 0;
                    needspace = 0;

                    switch ( buf_tag )
                    {
                    case TAG_H1:
                    case TAG_H2:
                    case TAG_H3:
                    case TAG_H4:
                    case TAG_H5:
                    case TAG_H6:
                        font  = FL_HELVETICA_BOLD;
                        //fsize = textsize_ + '7' - buf[1];
                        fsize += '7' - buf[1];
                        break;
                    case TAG_DT:
                        font  = textfont_ | FL_ITALIC;
                        //fsize = textsize_;
                        break;
                    case TAG_PRE:
                        font  = FL_COURIER;
                        //fsize = textsize_;
                        pre   = 1;
                        break;
                    case TAG_LI:
                        width  += 4 * fsize;
                        font   = textfont_;
                        //fsize  = textsize_;
                        break;
                    default:
                        font  = textfont_;
                        //fsize = textsize_;
                    }

                    pushfont ( font, fsize );
                }

                break;
            case TAG_TR:
            case TAG_TRM:
            case TAG_TABLE:
//		printf("%s column = %d, colspan = %d, num_columns = %d\n",
//		   buf, column, colspan, num_columns);

                if ( column >= 0 )
                {
                    // This is a hack to support COLSPAN...
                    max_width /= colspan;

                    while ( colspan > 0 )
                    {
                        if ( max_width > columns[column] )
                        {
                            columns[column] = max_width;
                        }

                        column ++;
                        colspan --;
                    }
                }

                if ( is_close_tag && ( buf_tag == TAG_TABLE ) )
                {
                    do_break_loop = true;
                    break;
                }

                needspace = 0;
                column	= -1;
                width	 = 0;
                max_width = 0;
                incell	= 0;
                break;
            case TAG_TD:
            case TAG_TH:

                if ( is_close_tag )
                {
                    incell = 0;
                    popfont ( font, fsize, fcolor );
                }
                else
                {
                    //printf("BEFORE column = %d, colspan = %d, num_columns = %d\n",
                    //		   column, colspan, num_columns);

                    if ( column >= 0 )
                    {
                        // This is a hack to support COLSPAN...
                        max_width /= colspan;

                        while ( colspan > 0 )
                        {
                            if ( max_width > columns[column] )
                            {
                                columns[column] = max_width;
                            }

                            column ++;
                            colspan --;
                        }
                    }
                    else
                    {
                        column ++;
                    }

                    if ( GET_ATTR( "COLSPAN") != NULL )
                    {
                        colspan = atoi ( attr );
                    }
                    else
                    {
                        colspan = 1;
                    }

                    //printf("AFTER column = %d, colspan = %d, num_columns = %d\n",
                    //  column, colspan, num_columns);

                    if ( ( column + colspan ) >= num_columns )
                    {
                        num_columns = column + colspan;
                    }

                    needspace = 0;
                    width	 = 0;
                    incell	= 1;

                    if ( buf_tag == TAG_TH )
                    {
                        font = textfont_ | FL_BOLD;
                    }
                    else
                    {
                        font = textfont_;
                    }

                    //fsize = textsize_;

                    pushfont ( font, fsize );

                    if ( GET_ATTR( "WIDTH") != NULL )
                    {
                        max_width = get_length ( attr );
                    }
                    else
                    {
                        max_width = 0;
                    }

                    //printf("max_width = %d\n", max_width);
                }

                break;
            case TAG_IMG:
            {
                Fl_Shared_Image	*img = 0;
                int		iwidth, iheight;

                get_attr ( attrs, "WIDTH", wattr, sizeof ( wattr ) );
                get_attr ( attrs, "HEIGHT", hattr, sizeof ( hattr ) );
                iwidth  = get_length ( wattr );
                iheight = get_length ( hattr );

                if ( GET_ATTR( "SRC") )
                {
                    img	 = get_image ( attr, iwidth, iheight );
                    iwidth  = img->w();
                    iheight = img->h();
                }

                if ( iwidth > minwidths[column] )
                {
                    minwidths[column] = iwidth;
                }

                width += iwidth;

                if ( needspace )
                {
                    width += ( int ) fl_width ( ' ' );
                }

                if ( width > max_width )
                {
                    max_width = width;
                }

                needspace = 0;
            }
            break;
            }

            if ( do_push_font )
            {
                pushfont ( font, fsize );
            }
            else if ( do_pop_font )
            {
                popfont ( font, fsize, fcolor );
            }

            if ( do_break_loop || ( buf_tag == TAG_TABLE && start > table ) )
            {
                break;
            }
        }
        else if ( *ptr == '\n' && pre )
        {
            width     = 0;
            needspace = 0;
            ptr ++;
        }
        else if ( isspace( ( *ptr ) ) )
        {
            needspace = 1;

            ptr ++;
        }
        else if ( *ptr == '&' && s < ( buf + sizeof( buf ) - 1 ) )
        {
            ptr ++;

            int qch = quote_char( ptr );

            if ( qch < 0 )
                *s++ = '&';
            else
            {
//        int l;
//        l = fl_utf8encode((unsigned int) qch, s);
//        if (l < 1) l = 1;
//        s += l;
                *s++ = qch;
                ptr = strchr( ptr, ';' ) + 1;
            }
        }
        else
        {
            if ( s < ( buf + sizeof( buf ) - 1 ) )
                *s++ = *ptr++;
            else
                ptr ++;
        }
    }

    // Now that we have scanned the entire table, adjust the table and
    // cell widths to fit on the screen...
    if ( get_attr( table + 6, "WIDTH", attr, sizeof( attr ) ) )
        *table_width = get_length( attr );
    else
        *table_width = 0;

#ifdef DEBUG
    printf( "num_columns = %d, table_width = %d\n", num_columns, *table_width );
#endif // DEBUG

    if ( num_columns == 0 )
        return;

    // Add up the widths...
    for ( column = 0, width = 0; column < num_columns; column ++ )
        width += columns[column];

#ifdef DEBUG
    printf( "width = %d, w() = %d\n", width, w() );

    for ( column = 0; column < num_columns; column ++ )
        printf( "    columns[%d] = %d, minwidths[%d] = %d\n", column, columns[column],
                column, minwidths[column] );

#endif // DEBUG

    // Adjust the width if needed...
    int scale_width = *table_width;

    int scrollsize = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();

    if ( scale_width == 0 )
    {
        if ( width > ( hsize_ - scrollsize ) ) scale_width = hsize_ - scrollsize;
        else scale_width = width;
    }

    if ( width < scale_width )
    {
#ifdef DEBUG
        printf( "Scaling table up to %d from %d...\n", scale_width, width );
#endif // DEBUG

        *table_width = 0;

        scale_width = ( scale_width - width ) / num_columns;

#ifdef DEBUG
        printf( "adjusted scale_width = %d\n", scale_width );
#endif // DEBUG

        for ( column = 0; column < num_columns; column ++ )
        {
            columns[column] += scale_width;

            ( *table_width ) += columns[column];
        }
    }
    else if ( width > scale_width )
    {
#ifdef DEBUG
        printf( "Scaling table down to %d from %d...\n", scale_width, width );
#endif // DEBUG

        for ( column = 0; column < num_columns; column ++ )
        {
            width       -= minwidths[column];
            scale_width -= minwidths[column];
        }

#ifdef DEBUG
        printf( "adjusted width = %d, scale_width = %d\n", width, scale_width );
#endif // DEBUG

        if ( width > 0 )
        {
            for ( column = 0; column < num_columns; column ++ )
            {
                columns[column] -= minwidths[column];
                columns[column] = scale_width * columns[column] / width;
                columns[column] += minwidths[column];
            }
        }

        *table_width = 0;

        for ( column = 0; column < num_columns; column ++ )
        {
            ( *table_width ) += columns[column];
        }
    }
    else if ( *table_width == 0 )
        *table_width = width;

#ifdef DEBUG
    printf( "FINAL table_width = %d\n", *table_width );

    for ( column = 0; column < num_columns; column ++ )
        printf( "    columns[%d] = %d\n", column, columns[column] );

#endif // DEBUG
}


/** Frees memory used for the document. */
void
Fl_Help_View::free_data()
{
    // Release all images...
    if ( value_ )
    {
        const char	*ptr,		// Pointer into block
        *attrs;		// Pointer to start of element attributes
        char	*s,		// Pointer into buffer
        buf[1024],	// Text buffer
        attr[1024],	// Attribute buffer
        wattr[1024],	// Width attribute buffer
        hattr[1024];	// Height attribute buffer

        for ( ptr = value_; *ptr; )
        {
            if ( *ptr == '<' )
            {
                ptr ++;

                if ( strncmp( ptr, "!--", 3 ) == 0 )
                {
                    // Comment...
                    ptr += 3;

                    if ( ( ptr = strstr( ptr, "-->" ) ) != NULL )
                    {
                        ptr += 3;
                        continue;
                    }
                    else
                        break;
                }

                s = buf;

                while ( *ptr && *ptr != '>' && !isspace( ( *ptr ) ) )
                    if ( s < ( buf + sizeof( buf ) - 1 ) )
                        *s++ = *ptr++;
                    else
                        ptr ++;

                *s = '\0';

                attrs = ptr;

                while ( *ptr && *ptr != '>' )
                    ptr ++;

                if ( *ptr == '>' )
                    ptr ++;

                if ( strcasecmp( buf, "IMG" ) == 0 )
                {
                    Fl_Shared_Image	*img;
                    int		width;
                    int		height;

                    get_attr( attrs, "WIDTH", wattr, sizeof( wattr ) );
                    get_attr( attrs, "HEIGHT", hattr, sizeof( hattr ) );
                    width  = get_length( wattr );
                    height = get_length( hattr );

                    if ( GET_ATTR( "SRC") )
                    {
                        // Get and release the image to free it from memory...
                        img = get_image( attr, width, height );

                        if ( ( void* )img != &broken_image )
                        {
                            img->release();
                        }
                    }
                }
            }
            else
                ptr ++;
        }

        free( ( void * )value_ );
        value_ = 0;
    }

    // Free all of the arrays...
    if ( nblocks_ )
    {
        free( blocks_ );

        ablocks_ = 0;
        nblocks_ = 0;
        blocks_  = 0;
    }

    if ( nlinks_ )
    {
        free( links_ );

        alinks_ = 0;
        nlinks_ = 0;
        links_  = 0;
    }

    if ( ntargets_ )
    {
        free( targets_ );

        atargets_ = 0;
        ntargets_ = 0;
        targets_  = 0;
    }
}

/** Gets an alignment attribute. */
int					// O - Alignment
Fl_Help_View::get_align( const char *p,	// I - Pointer to start of attrs
                         int        a )  	// I - Default alignment
{
    char	buf[255];			// Alignment value


    if ( get_attr( p, "ALIGN", buf, sizeof( buf ) ) == NULL )
        return ( a );

    if ( strcasecmp( buf, "CENTER" ) == 0 )
        return ( CENTER );
    else if ( strcasecmp( buf, "RIGHT" ) == 0 )
        return ( RIGHT );
    else
        return ( LEFT );
}


/** Gets an attribute value from the string. */
const char *					// O - Pointer to buf or NULL
Fl_Help_View::get_attr( const char *p,		// I - Pointer to start of attributes
                        const char *n,		// I - Name of attribute
                        char       *buf,		// O - Buffer for attribute value
                        int        bufsize )  	// I - Size of buffer
{
    char	name[255],				// Name from string
    *ptr,					// Pointer into name or value
    quote;					// Quote


    buf[0] = '\0';

    while ( *p && *p != '>' )
    {
        while ( isspace( ( *p ) ) )
            p ++;

        if ( *p == '>' || !*p )
            return ( NULL );

        for ( ptr = name; *p && !isspace( ( *p ) ) && *p != '=' && *p != '>'; )
            if ( ptr < ( name + sizeof( name ) - 1 ) )
                *ptr++ = *p++;
            else
                p ++;

        *ptr = '\0';

        if ( isspace( ( *p ) ) || !*p || *p == '>' )
            buf[0] = '\0';
        else
        {
            if ( *p == '=' )
                p ++;

            for ( ptr = buf; *p && !isspace( ( *p ) ) && *p != '>'; )
                if ( *p == '\'' || *p == '\"' )
                {
                    quote = *p++;

                    while ( *p && *p != quote )
                        if ( ( ptr - buf + 1 ) < bufsize )
                            *ptr++ = *p++;
                        else
                            p ++;

                    if ( *p == quote )
                        p ++;
                }
                else if ( ( ptr - buf + 1 ) < bufsize )
                    *ptr++ = *p++;
                else
                    p ++;

            *ptr = '\0';
        }

        if ( strcasecmp( n, name ) == 0 )
            return ( buf );
        else
            buf[0] = '\0';

        if ( *p == '>' )
            return ( NULL );
    }

    return ( NULL );
}


/** Gets a color attribute. */
Fl_Color				// O - Color value
Fl_Help_View::get_color( const char *n,	// I - Color name
                         Fl_Color   c )  	// I - Default color value
{
    int	rgb, r, g, b;			// RGB values
    static const struct  			// Color name table
    {
        const char *name;
        int r, g, b;
    }	colors[] =
    {
        //should be ordered to use binary search
        { "aqua",		0x00, 0xff, 0xff },
        { "black",		0x00, 0x00, 0x00 },
        { "blue",		0x00, 0x00, 0xff },
        { "cyan",		0x00, 0xff, 0xff },
        { "fuchsia",	0xff, 0x00, 0xff },
        { "gray",		0x80, 0x80, 0x80 },
        { "green",		0x00, 0x80, 0x00 },
        { "grey",		0x80, 0x80, 0x80 },
        { "lime",		0x00, 0xff, 0x00 },
        { "magenta",	0xff, 0x00, 0xff },
        { "maroon",		0x80, 0x00, 0x00 },
        { "navy",		0x00, 0x00, 0x80 },
        { "olive",		0x80, 0x80, 0x00 },
        { "purple",		0x80, 0x00, 0x80 },
        { "red",		0xff, 0x00, 0x00 },
        { "silver",		0xc0, 0xc0, 0xc0 },
        { "teal",		0x00, 0x80, 0x80 },
        { "white",		0xff, 0xff, 0xff },
        { "yellow",		0xff, 0xff, 0x00 },
    };


    if ( !n || !n[0] ) return c;

    if ( n[0] == '#' )
    {
        // Do hex color lookup
        rgb = strtol( n + 1, NULL, 16 );

        if ( strlen( n ) > 4 )
        {
            r = rgb >> 16;
            g = ( rgb >> 8 ) & 255;
            b = rgb & 255;
        }
        else
        {
            r = ( rgb >> 8 ) * 17;
            g = ( ( rgb >> 4 ) & 15 ) * 17;
            b = ( rgb & 15 ) * 17;
        }

        return ( fl_rgb_color( ( uchar )r, ( uchar )g, ( uchar )b ) );
    }
    else
    {
        int ifirst, ilast;
        ifirst = 0;
        ilast = ( int ) ( sizeof ( colors ) / sizeof ( colors[0] ) ) - 1;

        while ( ifirst <= ilast )
        {
            int mid = ( ifirst + ilast ) / 2; // compute mid point.

            int icmp = strcasecmp ( n, colors[mid].name );

            if ( icmp > 0 )
            {
                ifirst = mid + 1;    // repeat search in top half.
            }
            else if ( icmp < 0 )
            {
                ilast = mid - 1;    // repeat search in bottom half.
            }
            else
                // found it. return
            {
                return fl_rgb_color ( colors[mid].r, colors[mid].g, colors[mid].b );
            }
        }

        return c;
    }
}


/** Gets an inline image.

  The image reference count is maintained accordingly, such that
  the image can be released exactly once when the document is closed.

  \return a pointer to a cached Fl_Shared_Image, if the image can be loaded,
  	  otherwise a pointer to an internal Fl_Pixmap (broken_image).

  \todo Fl_Help_View::get_image() returns a pointer to the internal
  Fl_Pixmap broken_image, but this is _not_ compatible with the
  return type Fl_Shared_Image (release() must not be called).
*/

/* Implementation note: (A.S. Apr 05, 2009)

  Fl_Help_View::get_image() uses a static global flag (initial_load)
  to determine, if it is called from the initial loading of a document
  (load() or value()), or from resize() or draw().

  A better solution would be to manage all loaded images in an own
  structure like Fl_Help_Target (Fl_Help_Image ?) to avoid using this
  global flag, but this would break the ABI !

  This should be fixed in FLTK 1.3 !


  If initial_load is true, then Fl_Shared_Image::get() is called to
  load the image, and the reference count of the shared image is
  increased by one.

  If initial_load is false, then Fl_Shared_Image::find() is called to
  load the image, and the image is released immediately. This avoids
  increasing the reference count when calling get_image() from draw()
  or resize().

  Calling Fl_Shared_Image::find() instead of Fl_Shared_Image::get() avoids
  doing unnecessary i/o for "broken images" within each resize/redraw.

  Each image must be released exactly once in the destructor or before
  a new document is loaded: see free_data().
*/

Fl_Shared_Image *
Fl_Help_View::get_image( const char *name, int W, int H )
{
    const char	*localname;		// Local filename
    char	*link_to_free = NULL;		// Local filename
    char		dir[FL_PATH_MAX];		// Current directory
    char		temp[FL_PATH_MAX],		// Temporary filename
    *tempptr;		// Pointer into temporary name
    Fl_Shared_Image *ip;			// Image pointer...

    // See if the image can be found...
    if ( strchr( directory_, ':' ) != NULL && strchr( name, ':' ) == NULL )
    {
        if ( name[0] == '/' )
        {
            strlcpy( temp, directory_, sizeof( temp ) );

            if ( ( tempptr = strrchr( strchr( directory_, ':' ) + 3, '/' ) ) != NULL )
            {
                strlcpy( tempptr, name, sizeof( temp ) - ( tempptr - temp ) );
            }
            else
            {
                strlcat( temp, name, sizeof( temp ) );
            }
        }
        else
        {
            snprintf( temp, sizeof( temp ), "%s/%s", directory_, name );
        }

        if ( link_ ) localname = ( *link_ )( this, temp , 0);
        else localname = temp;
    }
    else if ( name[0] != '/' && strchr( name, ':' ) == NULL )
    {
        if ( directory_[0] ) snprintf( temp, sizeof( temp ), "%s/%s", directory_, name );
        else
        {
            fl_getcwd( dir, sizeof( dir ) );
            snprintf( temp, sizeof( temp ), "file:%s/%s", dir, name );
        }

        if ( link_ ) localname = ( *link_ )( this, temp , 0 );
        else localname = temp;
    }
    else if ( link_ ) localname = ( *link_ )( this, name , 0);
    else localname = name;

    if(localname != temp) link_to_free = (char*) localname;

    if ( !localname ) return 0;

    if ( strncmp( localname, "file:", 5 ) == 0 ) localname += 5;

    if ( initial_load )
    {
        if ( ( ip = Fl_Shared_Image::get( localname, W, H ) ) == NULL )
        {
            ip = ( Fl_Shared_Image * )&broken_image;
        }
    }
    else     // draw or resize
    {
        if ( ( ip = Fl_Shared_Image::find( localname, W, H ) ) == NULL )
        {
            ip = ( Fl_Shared_Image * )&broken_image;
        }
        else
        {
            ip->release();
        }
    }
    if(link_to_free) free(link_to_free);
    return ip;
}


/** Gets a length value, either absolute or %. */
int
Fl_Help_View::get_length( const char *l )  	// I - Value
{
    int	val;					// Integer value

    if ( !l[0] ) return 0;

    val = atoi( l );

    if ( l[strlen( l ) - 1] == '%' )
    {
        if ( val > 100 ) val = 100;
        else if ( val < 0 ) val = 0;

        int scrollsize = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();
        val = val * ( hsize_ - scrollsize ) / 100;
    }

    return val;
}


Fl_Help_Link *Fl_Help_View::find_link( int xx, int yy )
{
    int		i;
    Fl_Help_Link	*linkp;

    for ( i = nlinks_, linkp = links_; i > 0; i --, linkp ++ )
    {
        if ( xx >= linkp->x && xx < linkp->w &&
                yy >= linkp->y && yy < linkp->h )
            break;
    }

    return i ? linkp : 0L;
}

void Fl_Help_View::follow_link( Fl_Help_Link *linkp )
{
    char		target[32];	// Current target
    const char *namep = linkp->filename; // link filename
	const char *vlink_name;
    char	*link_to_free = NULL;

    clear_selection();

	if( link_ ){ //call link function to give it a chance to cancel it
		vlink_name = (*link_)(this, namep, linkp->name);
		if(!vlink_name) return;
		if(vlink_name != namep){
			link_to_free = (char*) vlink_name;
			namep = vlink_name;
		}
	}

	strlcpy(target, linkp->name, sizeof(target));
	clear_selection(); // Clear text selection
    set_changed();

    if ( strcmp( linkp->filename, filename_ ) != 0 && linkp->filename[0] )
    {
        char	dir[FL_PATH_MAX];	// Current directory
        char	temp[FL_PATH_MAX],	// Temporary filename
        *tempptr;	// Pointer into temporary filename


        if ( strchr( directory_, ':' ) != NULL &&
                strchr( linkp->filename, ':' ) == NULL )
        {
            if ( linkp->filename[0] == '/' )
            {
                strlcpy( temp, directory_, sizeof( temp ) );

                if ( ( tempptr = strrchr( strchr( directory_, ':' ) + 3, '/' ) ) != NULL )
                    strlcpy( tempptr, linkp->filename, sizeof( temp ) );
                else
                    strlcat( temp, linkp->filename, sizeof( temp ) );
            }
            else
                snprintf( temp, sizeof( temp ), "%s/%s", directory_, linkp->filename );
        }
        else if ( linkp->filename[0] != '/' && strchr( linkp->filename, ':' ) == NULL )
        {
            if ( directory_[0] )
                snprintf( temp, sizeof( temp ), "%s/%s", directory_, linkp->filename );
            else
            {
                fl_getcwd( dir, sizeof( dir ) );
                snprintf( temp, sizeof( temp ), "file:%s/%s", dir, linkp->filename );
            }
        }
        else
            strlcpy( temp, linkp->filename, sizeof( temp ) );

        if ( linkp->name[0] )
            snprintf( temp + strlen( temp ), sizeof( temp ) - strlen( temp ), "#%s",
                      linkp->name );

        load( temp );
    }
    else if ( target[0] )
        topline( target );
    else
        topline( 0 );

    leftline( 0 );

    if(link_to_free) free(link_to_free);
}

/** Removes the current text selection. */
void Fl_Help_View::clear_selection()
{
    if ( current_view == this )
        clear_global_selection();
}
/** Selects all the text in the view. */
void Fl_Help_View::select_all()
{
    clear_global_selection();

    if ( !value_ ) return;

    current_view = this;
    selection_drag_last = selection_last = strlen( value_ );
    selected = 1;
}

void Fl_Help_View::clear_global_selection()
{
    if ( selected ) redraw();

    selection_push_first = selection_push_last = 0;
    selection_drag_first = selection_drag_last = 0;
    selection_first = selection_last = 0;
    selected = 0;
}

char Fl_Help_View::begin_selection()
{
    clear_global_selection();

    if ( !fl_help_view_buffer ) fl_help_view_buffer = fl_create_offscreen( 1, 1 );

    mouse_x = Fl::event_x();
    mouse_y = Fl::event_y();
    draw_mode = 1;

    current_view = this;
    fl_begin_offscreen( fl_help_view_buffer );
    draw();
    fl_end_offscreen();

    draw_mode = 0;

    if ( selection_push_last ) return 1;
    else return 0;
}

char Fl_Help_View::extend_selection()
{
    if ( Fl::event_is_click() )
        return 0;

//  printf("old selection_first=%d, selection_last=%d\n",
//         selection_first, selection_last);

    int sf = selection_first, sl = selection_last;

    selected = 1;
    mouse_x = Fl::event_x();
    mouse_y = Fl::event_y();
    draw_mode = 2;

    fl_begin_offscreen( fl_help_view_buffer );
    draw();
    fl_end_offscreen();

    draw_mode = 0;

    if ( selection_push_first < selection_drag_first )
    {
        selection_first = selection_push_first;
    }
    else
    {
        selection_first = selection_drag_first;
    }

    if ( selection_push_last > selection_drag_last )
    {
        selection_last = selection_push_last;
    }
    else
    {
        selection_last = selection_drag_last;
    }

//  printf("new selection_first=%d, selection_last=%d\n",
//         selection_first, selection_last);

    if ( sf != selection_first || sl != selection_last )
    {
//    puts("REDRAW!!!\n");
        return 1;
    }
    else
    {
//    puts("");
        return 0;
    }
}

// convert a command with up to four letters into an unsigned int
static unsigned int command( const char *cmd )
{
    unsigned int ret = ( tolower( cmd[0] ) << 24 );
    char c = cmd[1];

    if ( c == '>' || c == ' ' || c == 0 ) return ret;

    ret |= ( tolower( c ) << 16 );
    c = cmd[2];

    if ( c == '>' || c == ' ' || c == 0 ) return ret;

    ret |= ( tolower( c ) << 8 );
    c = cmd[3];

    if ( c == '>' || c == ' ' || c == 0 ) return ret;

    ret |= tolower( c );
    c = cmd[4];

    if ( c == '>' || c == ' ' || c == 0 ) return ret;

    return 0;
}

#define CMD(a, b, c, d) ((a<<24)|(b<<16)|(c<<8)|d)

void Fl_Help_View::end_selection( int clipboard )
{
    if ( !selected || current_view != this )
        return;

    // convert the select part of our html text into some kind of somewhat readable ASCII
    // and store it in the selection buffer
    char p = 0, pre = 0;;
    int len = strlen( value_ );
    char *txt = ( char* )malloc( len + 1 ), *d = txt;
    const char *s = value_, *cmd, *src;

    for ( ;; )
    {
        char c = *s++;

        if ( c == 0 ) break;

        if ( c == '<' )   // begin of some html command. Skip until we find a '>'
        {
            cmd = s;

            for ( ;; )
            {
                c = *s++;

                if ( c == 0 || c == '>' ) break;
            }

            if ( c == 0 ) break;

            // do something with this command... .
            // the replacement string must not be longer that the command itself plus '<' and '>'
            src = 0;

            switch ( command( cmd ) )
            {
            case CMD( 'p', 'r', 'e', 0 ):
                pre = 1;
                break;
            case CMD( '/', 'p', 'r', 'e' ):
                pre = 0;
                break;
            case CMD( 't', 'd', 0 , 0 ):
            case CMD( 'p', 0 , 0 , 0 ):
            case CMD( '/', 'p', 0 , 0 ):
            case CMD( 'b', 'r', 0 , 0 ):
                src = "\n";
                break;
            case CMD( 'l', 'i', 0 , 0 ):
                src = "\n * ";
                break;
            case CMD( '/', 'h', '1', 0 ):
            case CMD( '/', 'h', '2', 0 ):
            case CMD( '/', 'h', '3', 0 ):
            case CMD( '/', 'h', '4', 0 ):
            case CMD( '/', 'h', '5', 0 ):
            case CMD( '/', 'h', '6', 0 ):
                src = "\n\n";
                break;
            case CMD( 't', 'r', 0 , 0 ):
            case CMD( 'h', '1', 0 , 0 ):
            case CMD( 'h', '2', 0 , 0 ):
            case CMD( 'h', '3', 0 , 0 ):
            case CMD( 'h', '4', 0 , 0 ):
            case CMD( 'h', '5', 0 , 0 ):
            case CMD( 'h', '6', 0 , 0 ):
                src = "\n\n";
                break;
            case CMD( 'd', 't', 0 , 0 ):
                src = "\n ";
                break;
            case CMD( 'd', 'd', 0 , 0 ):
                src = "\n - ";
                break;
            }

            int n = s - value_;

            if ( src && n > selection_first && n <= selection_last )
            {
                while ( *src )
                {
                    *d++ = *src++;
                }

                c = src[-1];
                p = isspace( c ) ? ' ' : c;
            }

            continue;
        }

        if ( c == '&' )   // special characters
        {
            int xx = quote_char( s );

            if ( xx >= 0 )
            {
                c = ( char )xx;

                for ( ;; )
                {
                    char cc = *s++;

                    if ( !cc || cc == ';' ) break;
                }
            }
        }

        int n = s - value_;

        if ( n > selection_first && n <= selection_last )
        {
            if ( !pre && isspace( c ) ) c = ' ';

            if ( p != ' ' || c != ' ' )
                *d++ = c;

            p = c;
        }
    }

    *d = 0;
    Fl::copy( txt, strlen( txt ), clipboard );
    free( txt );
}

#define ctrl(x) ((x)&0x1f)

/** Handles events in the widget. */
int				// O - 1 if we handled it, 0 otherwise
Fl_Help_View::handle( int event )  	// I - Event to handle
{
    static Fl_Help_Link *linkp;   // currently clicked link

    int xx = Fl::event_x() - x() + leftline_;
    int yy = Fl::event_y() - y() + topline_;

    switch ( event )
    {
    case FL_FOCUS:
        redraw();
        return 1;
    case FL_UNFOCUS:
        clear_selection();
        redraw();
        return 1;
    case FL_ENTER :
        Fl_Group::handle( event );
        return 1;
    case FL_LEAVE :
        fl_cursor( FL_CURSOR_DEFAULT );
        break;
    case FL_MOVE:

        if ( find_link( xx, yy ) ) fl_cursor( FL_CURSOR_HAND );
        else fl_cursor( FL_CURSOR_DEFAULT );

        return 1;
    case FL_PUSH:

        if ( Fl_Group::handle( event ) ) return 1;

        linkp = find_link( xx, yy );

        if ( linkp )
        {
            fl_cursor( FL_CURSOR_HAND );
            return 1;
        }

        if ( begin_selection() )
        {
            fl_cursor( FL_CURSOR_INSERT );
            return 1;
        }

        fl_cursor( FL_CURSOR_DEFAULT );
        return 1;
    case FL_DRAG:

        if ( linkp )
        {
            if ( Fl::event_is_click() )
            {
                fl_cursor( FL_CURSOR_HAND );
            }
            else
            {
                fl_cursor( FL_CURSOR_DEFAULT ); // should be "FL_CURSOR_CANCEL" if we had it
            }

            return 1;
        }

        if ( current_view == this && selection_push_last )
        {
            if ( extend_selection() ) redraw();

            fl_cursor( FL_CURSOR_INSERT );
            return 1;
        }

        fl_cursor( FL_CURSOR_DEFAULT );
        return 1;
    case FL_RELEASE:

        if ( linkp )
        {
            if ( Fl::event_is_click() )
            {
                follow_link( linkp );
            }

            fl_cursor( FL_CURSOR_DEFAULT );
            linkp = 0;
            return 1;
        }

        if ( current_view == this && selection_push_last )
        {
            end_selection();
            return 1;
        }

        return 1;
    case FL_SHORTCUT:
    {
        char ascii = Fl::event_text()[0];

        switch ( ascii )
        {
        case ctrl( 'A' ):
            select_all();
            redraw();
            return 1;
        case ctrl( 'C' ):
        case ctrl( 'X' ):
            end_selection( 1 );
            return 1;
        }

        break;
    }
    case FL_KEYBOARD:
    {
        switch (Fl::event_key())
        {
        case FL_Right:
        case FL_Left:
        {
            if(hscrollbar_.visible() && hscrollbar_.handle(event))
                return 1;
        }
        break;
        case FL_Up:
        case FL_Down:
        {
            if(scrollbar_.visible() && scrollbar_.handle(event))
                return 1;
        }
        break;
        }
    }
    break;
    }

    return ( Fl_Group::handle( event ) );
}

/**
  The constructor creates the Fl_Help_View widget at the specified
  position and size.
*/
Fl_Help_View::Fl_Help_View( int        xx,	// I - Left position
                            int        yy,	// I - Top position
                            int        ww,	// I - Width in pixels
                            int        hh,	// I - Height in pixels
                            const char *l )
    : Fl_Group( xx, yy, ww, hh, l ),
    scrollbar_( xx + ww - Fl::scrollbar_size(), yy,
                Fl::scrollbar_size(), hh - Fl::scrollbar_size() ),
    hscrollbar_( xx, yy + hh - Fl::scrollbar_size(),
                 ww - Fl::scrollbar_size(), Fl::scrollbar_size() )
{
    color( FL_BACKGROUND2_COLOR, FL_SELECTION_COLOR );

    title_[0]     = '\0';
    defcolor_     = FL_FOREGROUND_COLOR;
    bgcolor_      = FL_BACKGROUND_COLOR;
    textcolor_    = FL_FOREGROUND_COLOR;
    linkcolor_    = FL_SELECTION_COLOR;
    textfont_     = FL_TIMES;
    textsize_     = 12;
    value_        = NULL;

    ablocks_      = 0;
    nblocks_      = 0;
    blocks_       = ( Fl_Help_Block * )0;

    link_         = ( Fl_Help_Func * )0;

    alinks_       = 0;
    nlinks_       = 0;
    links_        = ( Fl_Help_Link * )0;

    atargets_     = 0;
    ntargets_     = 0;
    targets_      = ( Fl_Help_Target * )0;

    directory_[0] = '\0';
    filename_[0]  = '\0';

    topline_      = 0;
    leftline_     = 0;
    size_         = 0;
    hsize_        = 0;
    scrollbar_size_ = 0;
    font_increment_ = 2;
    td_top_gap_ = 3;
    td_bottom_gap_ = 5;
    tr_gap_ = 2;
    line31_ = 31;
    td_margin_ = 5;
    td_width_adjust_ = 1;

    scrollbar_.value( 0, hh, 0, 1 );
    scrollbar_.step( 8.0 );
    scrollbar_.show();
    scrollbar_.callback( scrollbar_callback );
	scrollbar_.linesize(32); // vertical scroll size, default is 16

    hscrollbar_.value( 0, ww, 0, 1 );
    hscrollbar_.step( 8.0 );
    hscrollbar_.show();
    hscrollbar_.callback( hscrollbar_callback );
    hscrollbar_.type( FL_HORIZONTAL );
    end();

    selection_first = 0;
    selection_last = 0;
    selection_push_first = 0;
    selection_push_last = 0;
    selection_drag_first = 0;
    selection_drag_last = 0;
    selected = 0;
    draw_mode = 0;
    mouse_x = 0;
    mouse_y = 0;
    current_pos = 0;
    current_view = 0L;
    hv_selection_color = 0;
    hv_selection_text_color = 0;

    resize( xx, yy, ww, hh );
}


/** Destroys the Fl_Help_View widget.

  The destructor destroys the widget and frees all memory that has been
  allocated for the current document.
*/
Fl_Help_View::~Fl_Help_View()
{
    clear_selection();
    free_data();
}


/** Loads the specified file.

  This method loads the specified file or URL.
*/
int				// O - 0 on success, -1 on error
Fl_Help_View::load( const char *f )   // I - Filename to load (may also have target)
{
    FILE		*fp;		// File to read from
    long		len;		// Length of file
    char		*target;	// Target in file
    char		*slash;		// Directory separator
    const char	*localname;	// Local filename
    char		error[1024];	// Error buffer
    char		newname[FL_PATH_MAX];	// New filename buffer
    char	    *link_to_free = NULL;

    // printf("load(%s)\n",f); fflush(stdout);

    if ( strncmp( f, "ftp:", 4 ) == 0 ||
            strncmp( f, "http:", 5 ) == 0 ||
            strncmp( f, "https:", 6 ) == 0 ||
            strncmp( f, "ipp:", 4 ) == 0 ||
            strncmp( f, "mailto:", 7 ) == 0 ||
            strncmp( f, "news:", 5 ) == 0 )
    {
        char urimsg[FL_PATH_MAX];

        if ( fl_open_uri( f, urimsg, sizeof( urimsg ) ) == 0 )
        {
            clear_selection();

            strlcpy( newname, f, sizeof( newname ) );

            if ( ( target = strrchr( newname, '#' ) ) != NULL )
                * target++ = '\0';

            if ( link_ )
                localname = ( *link_ )( this, newname, 0 );
            else
                localname = filename_;

            if ( !localname )
                return ( 0 );
            if(localname != newname) link_to_free = (char*) localname;

            free_data();

            strlcpy( filename_, newname, sizeof( filename_ ) );
            strlcpy( directory_, newname, sizeof( directory_ ) );

            // Note: We do not support Windows backslashes, since they are illegal
            //       in URLs...
            if ( ( slash = strrchr( directory_, '/' ) ) == NULL )
                directory_[0] = '\0';
            else if ( slash > directory_ && slash[-1] != '/' )
                *slash = '\0';

            snprintf( error, sizeof( error ),
                      "<HTML><HEAD><TITLE>Error</TITLE></HEAD>"
                      "<BODY><H1>Error</H1>"
                      "<P>Unable to follow the link \"%s\" - "
                      "%s.</P></BODY>",
                      f, urimsg );
            value( error );
            //return(-1);
        }

        if(link_to_free) free(link_to_free);

        return( 0 );
    }

    clear_selection();

    strlcpy( newname, f, sizeof( newname ) );

    if ( ( target = strrchr( newname, '#' ) ) != NULL )
        * target++ = '\0';

    if ( link_ )
        localname = ( *link_ )( this, newname, target);
    else
        localname = filename_;

    if ( !localname )
        return ( 0 );
    if(localname != newname) link_to_free = (char*) localname;

    free_data();

    strlcpy( filename_, newname, sizeof( filename_ ) );
    strlcpy( directory_, newname, sizeof( directory_ ) );

    // Note: We do not support Windows backslashes, since they are illegal
    //       in URLs...
    if ( ( slash = strrchr( directory_, '/' ) ) == NULL )
        directory_[0] = '\0';
    else if ( slash > directory_ && slash[-1] != '/' )
        *slash = '\0';

    if ( strncmp( localname, "file:", 5 ) == 0 )
        localname += 5;	// Adjust for local filename...

    if ( ( fp = fl_fopen( localname, "rb" ) ) != NULL )
    {
        fseek( fp, 0, SEEK_END );
        len = ftell( fp );
        rewind( fp );

        value_ = ( const char * )calloc( len + 1, 1 );
        fread( ( void * )value_, 1, len, fp );
        fclose( fp );
    }
    else
    {
        snprintf( error, sizeof( error ),
                  "<HTML><HEAD><TITLE>Error</TITLE></HEAD>"
                  "<BODY><H1>Error</H1>"
                  "<P>Unable to follow the link \"%s\" - "
                  "%s.</P></BODY>",
                  localname, strerror( errno ) );
        value_ = strdup( error );
    }

    initial_load = 1;
    format();
    initial_load = 0;

    if ( target )
        topline( target );
    else
        topline( 0 );

    if(link_to_free) free(link_to_free);

    return ( 0 );
}


/** Resizes the help widget. */

void
Fl_Help_View::resize( int xx,	// I - New left position
                      int yy,	// I - New top position
                      int ww,	// I - New width
                      int hh )  	// I - New height
{
    Fl_Boxtype		b = box() ? box() : FL_DOWN_BOX;
    // Box to draw...


    Fl_Widget::resize( xx, yy, ww, hh );

    int scrollsize = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();
    scrollbar_.resize( x() + w() - scrollsize - Fl::box_dw( b ) + Fl::box_dx( b ),
                       y() + Fl::box_dy( b ), scrollsize, h() - scrollsize - Fl::box_dh( b ) );
    hscrollbar_.resize( x() + Fl::box_dx( b ),
                        y() + h() - scrollsize - Fl::box_dh( b ) + Fl::box_dy( b ),
                        w() - scrollsize - Fl::box_dw( b ), scrollsize );

    format();
}


/** Scrolls the text to the indicated position, given a named destination.

  \param[in] n target name
*/
void
Fl_Help_View::topline( const char *n )  	// I - Target name
{
    Fl_Help_Target key,			// Target name key
    *target;		// Pointer to matching target


    if ( ntargets_ == 0 )
        return;

    strlcpy( key.name, n, sizeof( key.name ) );

    target = ( Fl_Help_Target * )bsearch( &key, targets_, ntargets_, sizeof( Fl_Help_Target ),
                                          ( compare_func_t )compare_targets );

    if ( target != NULL )
        topline( target->y );
}


/** Scrolls the text to the indicated position, given a pixel line.

  If the given pixel value \p top is out of range, then the text is
  scrolled to the top or bottom of the document, resp.

  \param[in] top top line number in pixels (0 = start of document)
*/
void
Fl_Help_View::topline( int top )  	// I - Top line number
{
    if ( !value_ )
        return;

    int scrollsize = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();

    if ( size_ < ( h() - scrollsize ) || top < 0 )
        top = 0;
    else if ( top > size_ )
        top = size_;

    topline_ = top;

    scrollbar_.value( topline_, h() - scrollsize, 0, size_ );

    do_callback();

    redraw();
}


/** Scrolls the text to the indicated position, given a pixel column.

  If the given pixel value \p left is out of range, then the text is
  scrolled to the left or right side of the document, resp.

  \param[in] left left column number in pixels (0 = left side)
*/
void
Fl_Help_View::leftline( int left )  	// I - Left position
{
    if ( !value_ )
        return;

    int scrollsize = scrollbar_size_ ? scrollbar_size_ : Fl::scrollbar_size();

    if ( hsize_ < ( w() - scrollsize ) || left < 0 )
        left = 0;
    else if ( left > hsize_ )
        left = hsize_;

    leftline_ = left;

    hscrollbar_.value( leftline_, w() - scrollsize, 0, hsize_ );

    redraw();
}


/** Sets the current help text buffer to the string provided and reformats the text.

  The provided character string \p val is copied internally and will be
  freed when value() is called again, or when the widget is destroyed.

  If \p val is NULL, then the widget is cleared.
*/
void
Fl_Help_View::value( const char *val )  	// I - Text to view
{
    clear_selection();
    free_data();
    set_changed();

    if ( !val )
        return;

    value_ = strdup( val );

    initial_load = 1;
    format();
    initial_load = 0;

    topline( 0 );
    leftline( 0 );
}


#ifdef ENC
# undef ENC
#endif
// part b in the table seems to be mac_roman - beku
# define ENC(a, b) a
#define QCHAR_ENTRY(aname, i1, i2) { aname,  sizeof(aname)-1, ENC(i1,i2) }


/** Returns the character code associated with a quoted char. */
static int			// O - Code or -1 on error
quote_char( const char *p )  	// I - Quoted string
{
    static struct
    {
        const char	*name;
        int		namelen;
        int		code;
    }	*nameptr,		// Pointer into name array
    names[] =  		// Quoting names
    {
        //should be ordered for binary search
        QCHAR_ENTRY ( "AElig;", 198, 174 ),
        QCHAR_ENTRY ( "Aacute;", 193, 231 ),
        QCHAR_ENTRY ( "Acirc;", 194, 229 ),
        QCHAR_ENTRY ( "Agrave;", 192, 203 ),
        QCHAR_ENTRY ( "Aring;", 197, 129 ),
        QCHAR_ENTRY ( "Atilde;", 195, 204 ),
        QCHAR_ENTRY ( "Auml;", 196, 128 ),
        QCHAR_ENTRY ( "Ccedil;", 199, 199 ),
        QCHAR_ENTRY ( "ETH;", 208,  -1 ),
        QCHAR_ENTRY ( "Eacute;", 201, 131 ),
        QCHAR_ENTRY ( "Ecirc;", 202, 230 ),
        QCHAR_ENTRY ( "Egrave;", 200, 233 ),
        QCHAR_ENTRY ( "Euml;", 203, 232 ),
        QCHAR_ENTRY ( "Iacute;", 205, 234 ),
        QCHAR_ENTRY ( "Icirc;", 206, 235 ),
        QCHAR_ENTRY ( "Igrave;", 204, 237 ),
        QCHAR_ENTRY ( "Iuml;", 207, 236 ),
        QCHAR_ENTRY ( "Ntilde;", 209, 132 ),
        QCHAR_ENTRY ( "Oacute;", 211, 238 ),
        QCHAR_ENTRY ( "Ocirc;", 212, 239 ),
        QCHAR_ENTRY ( "Ograve;", 210, 241 ),
        QCHAR_ENTRY ( "Oslash;", 216, 175 ),
        QCHAR_ENTRY ( "Otilde;", 213, 205 ),
        QCHAR_ENTRY ( "Ouml;", 214, 133 ),
        QCHAR_ENTRY ( "THORN;", 222,  -1 ),
        QCHAR_ENTRY ( "Uacute;", 218, 242 ),
        QCHAR_ENTRY ( "Ucirc;", 219, 243 ),
        QCHAR_ENTRY ( "Ugrave;", 217, 244 ),
        QCHAR_ENTRY ( "Uuml;", 220, 134 ),
        QCHAR_ENTRY ( "Yacute;", 221,  -1 ),
        QCHAR_ENTRY ( "Yuml;", 159, 217 ),
        QCHAR_ENTRY ( "aacute;", 225, 135 ),
        QCHAR_ENTRY ( "acirc;", 226, 137 ),
        QCHAR_ENTRY ( "acute;", 180, 171 ),
        QCHAR_ENTRY ( "aelig;", 230, 190 ),
        QCHAR_ENTRY ( "agrave;", 224, 136 ),
        QCHAR_ENTRY ( "amp;", '&', '&' ),
        QCHAR_ENTRY ( "aring;", 229, 140 ),
        QCHAR_ENTRY ( "atilde;", 227, 139 ),
        QCHAR_ENTRY ( "auml;", 228, 138 ),
        QCHAR_ENTRY ( "brvbar;", 166,  -1 ),
        QCHAR_ENTRY ( "bull;", 149, 165 ),
        QCHAR_ENTRY ( "ccedil;", 231, 141 ),
        QCHAR_ENTRY ( "cedil;", 184, 252 ),
        QCHAR_ENTRY ( "cent;", 162, 162 ),
        QCHAR_ENTRY ( "copy;", 169, 169 ),
        QCHAR_ENTRY ( "curren;", 164,  -1 ),
        QCHAR_ENTRY ( "deg;", 176, 161 ),
        QCHAR_ENTRY ( "divide;", 247, 214 ),
        QCHAR_ENTRY ( "eacute;", 233, 142 ),
        QCHAR_ENTRY ( "ecirc;", 234, 144 ),
        QCHAR_ENTRY ( "egrave;", 232, 143 ),
        QCHAR_ENTRY ( "eth;", 240,  -1 ),
        QCHAR_ENTRY ( "euml;", 235, 145 ),
        QCHAR_ENTRY ( "euro;", 128, 219 ),
        QCHAR_ENTRY ( "frac12;", 189,  -1 ),
        QCHAR_ENTRY ( "frac14;", 188,  -1 ),
        QCHAR_ENTRY ( "frac34;", 190,  -1 ),
        QCHAR_ENTRY ( "gt;", '>', '>' ),
        QCHAR_ENTRY ( "iacute;", 237, 146 ),
        QCHAR_ENTRY ( "icirc;", 238, 148 ),
        QCHAR_ENTRY ( "iexcl;", 161, 193 ),
        QCHAR_ENTRY ( "igrave;", 236, 147 ),
        QCHAR_ENTRY ( "iquest;", 191, 192 ),
        QCHAR_ENTRY ( "iuml;", 239, 149 ),
        QCHAR_ENTRY ( "laquo;", 171, 199 ),
        QCHAR_ENTRY ( "lt;", '<', '<' ),
        QCHAR_ENTRY ( "macr;", 175, 248 ),
        QCHAR_ENTRY ( "micro;", 181, 181 ),
        QCHAR_ENTRY ( "middot;", 183, 225 ),
        QCHAR_ENTRY ( "nbsp;", ' ', ' ' ),
        QCHAR_ENTRY ( "not;", 172, 194 ),
        QCHAR_ENTRY ( "ntilde;", 241, 150 ),
        QCHAR_ENTRY ( "oacute;", 243, 151 ),
        QCHAR_ENTRY ( "ocirc;", 244, 153 ),
        QCHAR_ENTRY ( "ograve;", 242, 152 ),
        QCHAR_ENTRY ( "ordf;", 170, 187 ),
        QCHAR_ENTRY ( "ordm;", 186, 188 ),
        QCHAR_ENTRY ( "oslash;", 248, 191 ),
        QCHAR_ENTRY ( "otilde;", 245, 155 ),
        QCHAR_ENTRY ( "ouml;", 246, 154 ),
        QCHAR_ENTRY ( "para;", 182, 166 ),
        QCHAR_ENTRY ( "plusmn;", 177, 177 ),
        QCHAR_ENTRY ( "pound;", 163, 163 ),
        QCHAR_ENTRY ( "premil;", 137, 228 ),
        QCHAR_ENTRY ( "quot;", '\"', '\"' ),
        QCHAR_ENTRY ( "raquo;", 187, 200 ),
        QCHAR_ENTRY ( "reg;", 174, 168 ),
        QCHAR_ENTRY ( "sect;", 167, 164 ),
        QCHAR_ENTRY ( "shy;", 173, '-' ),
        QCHAR_ENTRY ( "sup1;", 185,  -1 ),
        QCHAR_ENTRY ( "sup2;", 178,  -1 ),
        QCHAR_ENTRY ( "sup3;", 179,  -1 ),
        QCHAR_ENTRY ( "szlig;", 223, 167 ),
        QCHAR_ENTRY ( "thorn;", 254,  -1 ),
        QCHAR_ENTRY ( "times;", 215, 'x' ),
        QCHAR_ENTRY ( "trade;", 153, 170 ),
        QCHAR_ENTRY ( "uacute;", 250, 156 ),
        QCHAR_ENTRY ( "ucirc;", 251, 158 ),
        QCHAR_ENTRY ( "ugrave;", 249, 157 ),
        QCHAR_ENTRY ( "uml;", 168, 172 ),
        QCHAR_ENTRY ( "uuml;", 252, 159 ),
        QCHAR_ENTRY ( "yacute;", 253,  -1 ),
        QCHAR_ENTRY ( "yen;", 165, 180 ),
        QCHAR_ENTRY ( "yuml;", 255, 216 ),
    };

    if ( !strchr( p, ';' ) ) return -1;

    if ( *p == '#' )
    {
        if ( *( p + 1 ) == 'x' || *( p + 1 ) == 'X' ) return strtol( p + 2, NULL, 16 );
        else return atoi( p + 1 );
    }

    int ifirst, ilast;
    ifirst = 0;
    ilast = ( int ) ( sizeof ( names ) / sizeof ( names[0] ) ) - 1;

    while ( ifirst <= ilast )
    {
        int mid = ( ifirst + ilast ) / 2; // compute mid point.
        nameptr = names + mid;

        int icmp = strncmp ( p, nameptr->name, nameptr->namelen );

        if ( icmp > 0 )
        {
            ifirst = mid + 1;    // repeat search in top half.
        }
        else if ( icmp < 0 )
        {
            ilast = mid - 1;    // repeat search in bottom half.
        }
        else
            // found it. return
        {
            return nameptr->code;
        }
    }

    return -1;
}


/** The vertical scrollbar callback. */
static void
scrollbar_callback( Fl_Widget *s, void * )
{
    ( ( Fl_Help_View * )( s->parent() ) )->topline( int( ( ( Fl_Scrollbar* )s )->value() ) );
}


/** The horizontal scrollbar callback. */
static void
hscrollbar_callback( Fl_Widget *s, void * )
{
    ( ( Fl_Help_View * )( s->parent() ) )->leftline( int( ( ( Fl_Scrollbar* )s )->value() ) );
}


//
// End of "$Id: Fl_Help_View.cxx 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
