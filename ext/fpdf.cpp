// fpdf.cpp
//

#include "fpdf.h"
#line 12 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
//#define PDF_USING_ZLIB 1
#define FPDF_VERSION "1.7"
//#include "sqlite3.h"
//#define pdf_vsnprintf sqlite3_ivsnprintf //vsnprintf
//#define pdf_snprintf sqlite3_isnprintf //snprintf
#define pdf_vsnprintf vsnprintf //vsnprintf
#define pdf_snprintf snprintf //snprintf
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cmath>
#ifdef PDF_USING_ZLIB
#include "zlib.h"
#endif
//#include "duma.h"
#line 2252 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
#define M_SOF0  0xC0     // Start Of Frame N
#define M_SOF1  0xC1     // N indicates which compression process
#define M_SOF2  0xC2     // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5     // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8
#define M_EOI   0xD9     // End Of Image (end of datastream)
#define M_SOS   0xDA     // Start Of Scan (begins compressed data)
#define M_COM   0xFE     // COMment

#define M_PSEUDO 0xFFD8  // pseudo marker for start of image(byte 0)
#define LZZ_INLINE inline
#ifdef PDF_USING_ZLIB
#line 30 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string & gzcompress (std::string & dest, std::string const & src)
#line 31 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
{
    z_stream stream;
    int err;

    int nExtraChunks;
    size_t src_len = src.size();
    size_t dest_len = 2048 + src_len;
    dest.resize(dest_len);

    stream.next_in = (Bytef*)&src[0];
    stream.avail_in = (uInt)src_len;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    //err = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    err = deflateInit(&stream, Z_BEST_SPEED);
    if (err != Z_OK) throw "Could not initialize zstream !";

    nExtraChunks = 0;
    do
    {
        stream.next_out = (unsigned char*)&dest[0];
        stream.avail_out = dest_len;
        err = deflate(&stream, Z_FINISH);
        if (err == Z_STREAM_END )
            break;
        if (err != Z_OK)
        {
            inflateEnd(&stream);
            throw "Could not uncompress data !";
        }
        nExtraChunks += 1;
    }
    while (stream.avail_out == 0);

    dest.resize(stream.total_out);

    err = deflateEnd(&stream);
    if (nExtraChunks || (err != Z_OK)) throw "Could finalize zstream !";
    return dest;
}
#line 75 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string & gzuncompress (std::string & dest, std::string const & src)
#line 76 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
{
    z_stream stream;
    int err;

    int nExtraChunks;
    size_t src_len = src.size();
    size_t dest_len = 2048 + src_len*5;
    dest.resize(dest_len);

    stream.next_in = (Bytef*)&src[0];
    stream.avail_in = (uInt)src_len;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    err = inflateInit(&stream);
    if (err != Z_OK) throw "Could not initialize zstream !";

    nExtraChunks = 0;
    do
    {
        stream.next_out = (unsigned char*)&dest[0];
        stream.avail_out = dest_len;
        err = inflate(&stream, Z_FINISH);
        if (err == Z_STREAM_END )
            break;
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            err = Z_DATA_ERROR;
        if (err != Z_BUF_ERROR)
        {
            inflateEnd(&stream);
            throw "Could not uncompress data !";
        }
        nExtraChunks += 1;
    }
    while (stream.avail_out == 0);

    dest.resize(stream.total_out);

    err = inflateEnd(&stream);
    if (nExtraChunks || (err != Z_OK)) throw "Could finalize zstream !";
    return dest;
}
#endif
#line 132 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
char const * FPDF_FONTPATH = 0;
#line 261 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::~ FPDF ()
#line 262 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        font_map_t::iterator iter = m_fonts.begin();
        font_map_t::iterator iend = m_fonts.end();
        while(iter != iend)
        {
            delete iter->second;
            ++iter;
        }
    }
#line 272 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::FPDF ()
#line 273 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        reset(e_orientation_portrait, e_mm, e_A4);
    }
#line 277 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::FPDF (e_orientation orientation, e_units unit, e_page_sizes psize)
#line 278 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        reset(orientation, unit, psize);
    }
#line 282 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::FPDF (char orientation, char const * unit, char const * psize)
#line 283 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        e_orientation eo = orientation == 'L' ? e_orientation_landscape : e_orientation_portrait;
        e_units eu;
        e_page_sizes es;
        if(strcasecmp(unit, "pt")==0) eu = e_pt;
        else if(strcasecmp(unit, "cm")==0) eu = e_cm;
        else if(strcasecmp(unit, "in")==0) eu = e_in;
        else eu = e_mm;

        if(strcasecmp(psize, "a3")==0) es = e_A3;
        else if(strcasecmp(psize, "a5")==0) es = e_A5;
        else if(strcasecmp(psize, "letter")==0) es = e_Letter;
        else if(strcasecmp(psize, "legal")==0) es = e_Legal;
        else es = e_A4;

        reset(eo, eu, es);
    }
#line 301 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::reset (e_orientation orientation, e_units unit, e_page_sizes psize)
#line 302 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Scale factor
        switch(unit)
        {
        case e_pt:
            m_k = 1;
            break;
        case e_mm:
            m_k = 72/25.4;
            break;
        case e_cm:
            m_k = 72/2.54;
            break;
        case e_in:
            m_k = 72;
            break;
        default:
            Error("Incorrect unit: %s", unit);
        }

        // Some checks
        _dochecks();
        // Initialization of properties
        m_page = 0;
        m_pages.clear();
        m_PageSizes.clear();
        m_offsets.clear();
        m_fonts.clear();
        m_FontFiles.clear();
        m_diffs.clear();
        m_images.clear();
        m_PageLinks.clear();
        m_links.clear();
        m_extgstates.clear();
        m_offsets.resize(3, 0);
        m_n = 2;
        m_buffer.clear();
        m_buffer.reserve(1024);
        //this->buffer = "";
        //this->m_pages = array();
        //this->PageSizes = array();
        m_state = 0;
        //this->fonts = array();
        //this->FontFiles = array();
        //this->diffs = array();
        //this->images = array();
        //this->links = array();
        m_InHeader = false;
        m_InFooter = false;
        m_lasth = 0;
        //this->FontFamily = "";
        //this->FontStyle = "";
        m_FontSizePt = 12;
        m_underline = false;
        m_DrawColor = "0 G";
        m_DrawColor_rgb = {0,0,0,0};
        m_FillColor = "0 g";
        m_FillColor_rgb = {0,0,0,0};
        m_TextColor = "0 g";
        m_TextColor_rgb = {0,0,0,0};
        m_ColorFlag = false;
        m_ws = 0;
        // Font path
        if(FPDF_FONTPATH)
        {
            m_fontpath = FPDF_FONTPATH;
            char clast = *m_fontpath.rbegin();
            if(clast != '/' && clast != '\\')
                m_fontpath += "/";
        }
#if 0
        else if(is_dir(dirname(__FILE__) + "/font"))
            m_fontpath = dirname(__FILE__) + "/font/";
#endif
        else
            m_fontpath.clear();

        m_CurrentFont = 0;

        st_pagesize page_size;
        _getpagesize(page_size, psize);
        m_DefPageSize = page_size;
        m_CurPageSize = page_size;
        // Page orientation
        switch(orientation)
        {
        case e_orientation_portrait:
        {
            m_DefOrientation = e_orientation_portrait;
            m_w = page_size.w;
            m_h = page_size.h;
        }
        break;
        case e_orientation_landscape:
        {
            m_DefOrientation = e_orientation_landscape;
            m_w = page_size.h;
            m_h = page_size.w;
        }
        break;
        default:
            Error("Incorrect orientation: %d", orientation);
        }

        m_CurOrientation = m_DefOrientation;
        m_wPt = m_w * m_k;
        m_hPt = m_h * m_k;
        m_angle = 0;
        // Page margins (1 cm)
        pdf_float_t margin = 28.35/m_k;
        SetMargins(margin,margin);
        // Interior cell margin (1 mm)
        m_cMargin = margin/10.0;
        // Line width (0.2 mm)
        m_LineWidth = .567/m_k;
        // Automatic page break
        SetAutoPageBreak(true,2*margin);
        // Default display mode
        SetDisplayMode(e_zoom_default);
        m_CustomZoom = 0;
        // Enable compression
        SetCompression(true);
        // Set default PDF version number
        m_PDFVersion = "1.3";

        m_doubleSided=false;
        m_xDelta=0;
        m_innerMargin=10;
        m_outerMargin=10;
        m_n_js = 0;
    }
#line 434 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetDoubleSided (pdf_float_t inner, pdf_float_t outer)
#line 435 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(outer != inner)
        {
            m_doubleSided=true;
            m_outerMargin=outer;
            m_innerMargin=inner;
        }
    }
#line 444 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetMargins (pdf_float_t left, pdf_float_t top, pdf_float_t right)
#line 445 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set left, top and right margins
        m_lMargin = left;
        m_tMargin = top;
        m_rMargin = (right==0.0f) ? left : right;
    }
#line 452 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetLeftMargin (pdf_float_t margin)
#line 453 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set left margin
        m_lMargin = margin;
        if(m_page > 0 && m_x < margin)
            m_x = margin;
    }
#line 482 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetAutoPageBreak (bool b, pdf_float_t margin)
#line 483 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set auto page break mode and triggering margin
        m_AutoPageBreak = b;
        m_bMargin = margin;
        m_PageBreakTrigger = m_h-margin;
    }
#line 490 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::CheckPageBreak (pdf_float_t height)
#line 491 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        //If the height h would cause an overflow, add a new page immediately
        if(GetY()+height > m_PageBreakTrigger)
        AddPage(m_CurOrientation);
    }
#line 507 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetDisplayMode (e_zoom_mode zoom, e_layout_mode layout)
#line 508 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set display mode in viewer
        if(zoom==e_zoom_fullpage || zoom==e_zoom_fullwidth ||
                zoom==e_zoom_real || zoom==e_zoom_default)
            m_ZoomMode = zoom;
        else
            Error("Incorrect zoom display mode %d", zoom);
        if(layout==e_layout_single || layout==e_layout_continuous ||
                layout==e_layout_two || layout==e_layout_default)
            m_LayoutMode = layout;
        else
            Error("Incorrect layout display mode: %d", layout);
    }
#line 522 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetCompression (bool compress)
#line 523 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set page compression
#ifndef PDF_USING_ZLIB
        m_compress = false;
#else
        m_compress = compress;
#endif
    }
#line 532 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetTitle (char const * title)
#line 533 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Title of document
        m_title = title;
    }
#line 538 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetSubject (char const * subject)
#line 539 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Subject of document
        m_subject = subject;
    }
#line 544 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetAuthor (char const * author)
#line 545 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Author of document
        m_author = author;
    }
#line 550 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetKeywords (char const * keywords)
#line 551 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Keywords of document
        m_keywords = keywords;
    }
#line 556 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetCreator (char const * creator)
#line 557 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Creator of document
        m_creator = creator;
    }
#line 562 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::AliasNbPages (char const * alias)
#line 563 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Define an alias for total number of pages
        m_AliasNbPages = alias;
    }
#line 574 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Error (char const * msg, ...)
#line 575 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Fatal error
        va_list args;
        va_start( args, msg );
        pdf_vsnprintf(m_scratch_buf, sizeof(m_scratch_buf), msg, args);
        va_end( args );
        std::string error_msg = "FPDF error: ";
        error_msg +=  m_scratch_buf;
        throw error_msg;
    }
#line 592 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Close ()
#line 593 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Terminate document
        if(m_state==3) return;
        if(m_page==0) AddPage();
        // Page footer
        m_InFooter = true;
        Footer();
        m_InFooter = false;
        // Close page
        _endpage();
        // Close document
        _enddoc();
    }
#line 607 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::AddPage (e_orientation orientation, st_pagesize * psize)
#line 608 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Start a new page
        if(m_state==0) Open();
        std::string family = m_FontFamily;
        std::string style = m_FontStyle;
        if(m_underline) style += "U";
        pdf_float_t fontsize = m_FontSizePt;
        pdf_float_t lw = m_LineWidth;
        std::string dc = m_DrawColor;
        std::string fc = m_FillColor;
        std::string tc = m_TextColor;
        bool cf = m_ColorFlag;
        if(m_page > 0)
        {
            // Page footer
            m_InFooter = true;
            Footer();
            m_InFooter = false;
            // Close page
            _endpage();
        }
        // Start new page
        _beginpage(orientation,psize);
        // Set line cap style to square
        _out("2 J");
        // Set line width
        m_LineWidth = lw;
        _outfmt(true, "%.2f w", lw*m_k);
        // Set font
        if(!family.empty()) SetFont(family.c_str(),style.c_str(),fontsize);
        // Set colors
        m_DrawColor = dc;
        if(dc != "0 G") _out(dc);
        m_FillColor = fc;
        if(fc != "0 g") _out(fc);
        m_TextColor = tc;
        m_ColorFlag = cf;
        // Page header
        m_InHeader = true;
        Header();
        m_InHeader = false;
        // Restore line width
        if(m_LineWidth != lw)
        {
            m_LineWidth = lw;
            _outfmt(true, "%.2f w", lw*m_k);
        }
        // Restore font
        if(!family.empty()) SetFont(family.c_str(),style.c_str(),fontsize);
        // Restore colors
        if(m_DrawColor != dc)
        {
            m_DrawColor = dc;
            _out(dc);
        }
        if(m_FillColor != fc)
        {
            m_FillColor = fc;
            _out(fc);
        }
        m_TextColor = tc;
        m_ColorFlag = cf;
    }
#line 672 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Header ()
#line 673 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {}
#line 677 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Footer ()
#line 678 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {}
#line 688 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetDrawColor (unsigned char r)
#line 689 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set color for all stroking operations
        m_DrawColor_rgb = {r,r,r,0};
        pdf_sprintf(m_DrawColor, "%.3f G", r/255.0);
        if(m_page>0) _out(m_DrawColor);
    }
#line 696 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetDrawColor (unsigned char r, unsigned char g, unsigned char b)
#line 697 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set color for all stroking operations
        m_DrawColor_rgb = {r,g,b,0};
        pdf_sprintf(m_DrawColor, "%.3f %.3f %.3f RG", r/255.0, g/255.0, b/255.0);
        if(m_page>0) _out(m_DrawColor);
    }
#line 712 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetFillColor (unsigned char r)
#line 713 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set color for all filling operations
        m_FillColor_rgb = {r,r,r,0};
        pdf_sprintf(m_FillColor, "%.3f g", r/255.0);
        m_ColorFlag = (m_FillColor != m_TextColor);
        if(m_page>0) _out(m_FillColor);
    }
#line 721 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetFillColor (unsigned char r, unsigned char g, unsigned char b)
#line 722 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set color for all filling operations
        m_FillColor_rgb = {r,g,b,0};
        pdf_sprintf(m_FillColor, "%.3f %.3f %.3f rg", r/255.0, g/255.0, b/255.0);
        m_ColorFlag = (m_FillColor != m_TextColor);
        if(m_page>0) _out(m_FillColor);
    }
#line 738 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetTextColor (unsigned char r)
#line 739 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set color for text
        m_TextColor_rgb = {r,r,r,0};
        pdf_sprintf(m_TextColor, "%.3f g", r/255.0);
        m_ColorFlag = (m_FillColor != m_TextColor);
    }
#line 746 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetTextColor (unsigned char r, unsigned char g, unsigned char b)
#line 747 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set color for text
        m_TextColor_rgb = {r,g,b,0};
        pdf_sprintf(m_TextColor, "%.3f %.3f %.3f rg", r/255.0, g/255.0, b/255.0);
        m_ColorFlag = (m_FillColor != m_TextColor);
    }
#line 766 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetAlpha (pdf_float_t alpha, char const * bm)
#line 767 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // set alpha for stroking (CA) and non-stroking (ca) operations
        st_alpha_t gs;
        gs.alpha = alpha;
        gs.bm = bm ? bm : "Normal";
        m_extgstates.push_back(gs);
        _outfmt(true, "/GS%d gs", m_extgstates.size());
    }
#line 776 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
pdf_float_t FPDF::GetStringWidth (char const * s)
#line 777 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Get width of a string in the current font
        font_width_t *cw = m_CurrentFont->font.cw;
        pdf_float_t w = 0.0;
        if(s)
        {
            while(*s)
            {
                w += cw[(unsigned char)*s++];
            }
        }
        return w*m_FontSize/1000.0;
    }
#line 796 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetLineWidth (pdf_float_t width)
#line 797 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set line width
        m_LineWidth = width;
        if(m_page>0) _outfmt(true, "%.2f w", width*m_k);
    }
#line 803 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetDash (pdf_float_t black, pdf_float_t white)
#line 804 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(black >= 0.0f && white >= 0.0f)
            _outfmt(true, "[%.3f %.3f] 0 d", black*m_k, white*m_k);
        else
            _out("[] 0 d");
    }
#line 811 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Line (pdf_float_t x1, pdf_float_t y1, pdf_float_t x2, pdf_float_t y2)
#line 812 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Draw a line
        _outfmt(true, "%.2f %.2f m %.2f %.2f l S", x1*m_k,(m_h-y1)*m_k,x2*m_k,(m_h-y2)*m_k);
    }
#line 817 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Rect (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, char const * style)
#line 818 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Draw a rectangle
        const char *op = "S";
        if(style)
        {
            if(strcasecmp(style, "F") == 0) op = "f";
            else if( (strcasecmp(style, "FD") == 0) ||
                     (strcasecmp(style, "DF") == 0) ) op = "B";
        }
        _outfmt(true, "%.2f %.2f %.2f %.2f re %s", x*m_k,(m_h-y)*m_k,w*m_k,-h*m_k, op);
    }
#line 830 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::AddFont (char const * afamily, char const * astyle, char const * afile)
#line 831 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Add a TrueType, OpenType or Type1 font
        std::string family, style;
        if(afamily && afamily[0])
        {
            family = afamily;
            _str_tolower(family);
        }
        else
        {
            family = m_FontFamily;
        }
        if(astyle && astyle[0])
        {
            style = astyle;
            _str_tolower(style);
        }

        if(family=="arial") family = "helvetica";
        if(style=="ib") style = "bi";
        std::string fontkey = family + style;
        font_map_t::iterator iter = m_fonts.find(fontkey);
        if(iter != m_fonts.end()) return; //already inserted

        st_pdf_font_base *font;
        if(fontkey == "helveticai") font = new pdf_font_HelveticaOblique();
        else if(fontkey == "helveticab") font = new pdf_font_HelveticaBold();
        else if(fontkey == "helveticabi") font = new pdf_font_HelveticaBoldOblique();
        else if(fontkey == "times") font = new pdf_font_Times();
        else if(fontkey == "timesi") font = new pdf_font_TimesOblique();
        else if(fontkey == "timesb") font = new pdf_font_TimesBold();
        else if(fontkey == "timesbi") font = new pdf_font_TimesBoldOblique();
        else if(fontkey == "courier") font = new pdf_font_Courier();
        else if(fontkey == "courieri") font = new pdf_font_CourierOblique();
        else if(fontkey == "courierb") font = new pdf_font_CourierBold();
        else if(fontkey == "courierbi") font = new pdf_font_CourierBoldOblique();
        else if(fontkey == "symbol") font = new pdf_font_Symbol();
        else if(fontkey == "zapfdingbats") font = new pdf_font_ZapfDingbats();
        else font = new pdf_font_Helvetica();
        m_fonts[fontkey] = font;
        font->i = m_fonts.size();
#if 0
        if(m_fonts.find(fontkey) == -1) return;
        st_pdf_font info = _loadfont(file);
        info.i = m_fonts.size()+1;
        if(!empty(info["diff"]))
        {
            // Search existing encodings
            n = array_search(info["diff"],m_diffs);
            if(!n)
            {
                n = count(m_diffs)+1;
                m_diffs[n] = info["diff"];
            }
            info["diffn"] = n;
        }
        if(!empty(info["file"]))
        {
            // Embedded font
            if(info["type"]=="TrueType")
                m_FontFiles[info["file"]] = array("length1"=>info["originalsize"]);
            else
                m_FontFiles[info["file"]] = array("length1"=>info["size1"], "length2"=>info["size2"]);
        }
        m_fonts[fontkey] = info;
#endif
    }
#line 904 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::GetFontSettings (font_settings_st & fs)
#line 904 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                                              {
        fs.family = m_FontFamily;
        fs.style = m_FontStyle;
        fs.size = m_FontSizePt;
    }
#line 910 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetFontSettings (font_settings_st & fs)
#line 910 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                                              {
        SetFont(fs.family.c_str(), fs.style.c_str(), fs.size);
    }
#line 914 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetFont (char const * afamily, char const * astyle, pdf_float_t size)
#line 915 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Select a font; size given in points
        std::string family, style;
        if(afamily && afamily[0])
        {
            family = afamily;
            _str_tolower(family);
            if(family=="arial") family = "helvetica";
        }
        else
        {
            family = m_FontFamily;
        }
        if(astyle && astyle[0])
        {
            style = astyle;
            _str_tolower(style);
        }

        size_t found = style.find("u");
        if(found != std::string::npos )
        {
            m_underline = true;
            style.erase(found, 1);
        }
        else
            m_underline = false;
        if(style=="ib") style = "bi";
        if(size==0) size = m_FontSizePt;
        // Test if font is already selected
        if(m_FontFamily==family && m_FontStyle==style && m_FontSizePt==size)
            return;
        // Test if font is already loaded
        std::string fontkey = family + style;
        font_map_t::iterator iter = m_fonts.find(fontkey);
        if(iter == m_fonts.end())
        {
            // Test if one of the core fonts
            if(isPdfFontCore(family.c_str()))
            {
                if(family=="symbol" || family=="zapfdingbats") style.clear();
                fontkey = family + style;
                iter = m_fonts.find(fontkey);
                if(iter == m_fonts.end()) AddFont(family.c_str(),style.c_str());
            }
            else
                Error("Undefined font: %s %s", family.c_str(), style.c_str());
        }
        // Select it
        m_FontFamily = family;
        m_FontStyle = style;
        m_FontSizePt = size;
        m_FontSize = size/m_k;
        m_CurrentFont = m_fonts[fontkey];
        if(m_page>0)
            _outfmt(true, "BT /F%d %.2f Tf ET", m_CurrentFont->i, m_FontSizePt);
    }
#line 973 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetFontSize (pdf_float_t size)
#line 974 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set font size in points
        if(m_FontSizePt==size) return;
        m_FontSizePt = size;
        m_FontSize = size/m_k;
        if(m_page>0)
            _outfmt(true, "BT /F%d %.2f Tf ET", m_CurrentFont->i, m_FontSizePt);
    }
#line 988 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
int FPDF::AddLink ()
#line 989 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Create a new internal link
        int n = m_links.size();
        st_link link = {0,0};
        m_links.push_back(link);
        return n;
    }
#line 997 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetLink (int link, pdf_float_t y, int page)
#line 998 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set destination of internal link
        st_link &nlink = m_links[link];
        if(y==-1) nlink.to = m_y;
        if(page==-1) page = m_page;
        nlink.from = page;
    }
#line 1006 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Link (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, int link)
#line 1007 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Put a link on the page
        st_page_link pl;
        pl.x = x*m_k;
        pl.y = m_hPt-y*m_k;
        pl.w = w*m_k;
        pl.h = h*m_k;
        pl.link = link;
        m_PageLinks[m_page] = pl;
    }
#line 1019 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_TextBase (std::string & result, pdf_float_t x, pdf_float_t y, char const * txt)
#line 1020 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Output a string
        //because the format buffer is limited
        //we send the string separated from format
        if(m_ColorFlag)
        {
            result += "q " + m_TextColor + " ";
        }
        pdf_sprintf_append(result, "BT %.2f %.2f Td (", x, y);
        result += _escape(txt);
        result += ") Tj ET";
        if(m_underline && txt)
        {
            result += " " + _dounderline(x, y, txt);
        }
        if(m_ColorFlag) result += " Q";
    }
#line 1040 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Rotate (pdf_float_t angle, pdf_float_t x, pdf_float_t y)
#line 1041 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(x==-1) x=m_x;
        if(y==-1) y=m_y;
        if(m_angle!=0) _out("Q");
        m_angle=angle;
        if(angle!=0)
        {
            angle*=M_PI/180.0;
            pdf_float_t c=cos(angle);
            pdf_float_t s=sin(angle);
            pdf_float_t cx=x*m_k;
            pdf_float_t cy=(m_h-y)*m_k;
            _outfmt(true, "q %.5f %.5f %.5f %.5f %.2f %.2f cm 1 0 0 1 %.2f %.2f cm",
                    c,s,-s,c,cx,cy,-cx,-cy);
        }
    }
#line 1058 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::RotatedText (pdf_float_t x, pdf_float_t y, char const * txt, pdf_float_t angle)
#line 1059 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        //Text rotated around its origin
        Rotate(angle,x,y);
        Text(x,y,txt);
        Rotate(0);
    }
#line 1071 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::ClippingText (pdf_float_t x, pdf_float_t y, char const * txt, bool outline)
#line 1072 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        int op= outline ? 5 : 7;
        _outfmt(false, "q BT %.2f %.2f Td %d Tr (", x*m_k, (m_h-y)*m_k, op);
        _out(_escape(txt), false);
        _out(") Tj ET");
    }
#line 1079 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Text (pdf_float_t x, pdf_float_t y, char const * txt)
#line 1080 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        std::string result;
        _TextBase(result, x*m_k, (m_h-y)*m_k, txt);
        _out(result);
    }
#line 1086 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::TextShadow (pdf_float_t x, pdf_float_t y, char const * txt, pdf_float_t displacement)
#line 1088 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        pdf_color_t saved_color = m_TextColor_rgb;
        SetTextColor(200,200,200);
        Text(x+displacement, y+displacement, txt);
        SetTextColor(saved_color);
        Text(x, y, txt);
    }
#line 1096 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
bool FPDF::AcceptPageBreak ()
#line 1097 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Accept automatic page break or not
        return m_AutoPageBreak;
    }
#line 1102 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Cell (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, int ln, char align, bool fill, int link)
#line 1104 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Output a cell
        std::string s;
        pdf_float_t k = m_k;
        if(m_y+h > m_PageBreakTrigger && !m_InHeader && !m_InFooter && AcceptPageBreak())
        {
            // Automatic page break
            pdf_float_t x = m_x;
            pdf_float_t ws = m_ws;
            if(ws>0)
            {
                m_ws = 0;
                _out("0 Tw");
            }
            AddPage(m_CurOrientation, &m_CurPageSize);
            m_x = x + m_xDelta;
            if(ws>0)
            {
                m_ws = ws;
                _outfmt(true, "%.3f Tw", ws*k);
            }
        }
        if(w==0)
            w = m_w-m_rMargin-m_x;

        if(fill || (border && border[0]=='1'))
        {
            pdf_sprintf(s, "%.2f %.2f %.2f %.2f re %s ", m_x*k,(m_h-m_y)*k,w*k,-h*k,
                    fill ? ((border && border[0]=='1') ? "B" : "f") : "S");
        }
        if(border)
        {
            pdf_float_t x = m_x;
            pdf_float_t y = m_y;
            if(strchr(border,'L'))
                pdf_sprintf_append(s, "%.2f %.2f m %.2f %.2f l S ",x*k,(m_h-y)*k,x*k,(m_h-(y+h))*k);
            if(strchr(border,'T'))
                pdf_sprintf_append(s, "%.2f %.2f m %.2f %.2f l S ",x*k,(m_h-y)*k,(x+w)*k,(m_h-y)*k);
            if(strchr(border,'R'))
                pdf_sprintf_append(s, "%.2f %.2f m %.2f %.2f l S ",(x+w)*k,(m_h-y)*k,(x+w)*k,(m_h-(y+h))*k);
            if(strchr(border,'B'))
                pdf_sprintf_append(s, "%.2f %.2f m %.2f %.2f l S ",x*k,(m_h-(y+h))*k,(x+w)*k,(m_h-(y+h))*k);
        }
        if(txt)
        {
            pdf_float_t dx;
            pdf_float_t txt_width = GetStringWidth(txt);
            if(align=='R')
                dx = w-m_cMargin-txt_width;
            else if(align=='C')
                dx = (w-txt_width)/2;
            else
                dx = m_cMargin;

            _TextBase(s, (m_x+dx)*k, (m_h-(m_y+.5*h+.3*m_FontSize))*k, txt);

            if(link)
                Link(m_x+dx, m_y+.5*h-.5*m_FontSize, txt_width, m_FontSize, link);
        }
        if(!s.empty()) _out(s);
        m_lasth = h;
        if(ln>0)
        {
            // Go to next line
            m_y += h;
            if(ln==1) m_x = m_lMargin;
        }
        else m_x += w;
    }
#line 1180 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::MultiCell (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, char align, bool fill)
#line 1182 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Output text with automatic or explicit line breaks
        font_width_t *cw = m_CurrentFont->font.cw;
        if(w==0) w = m_w-m_rMargin-m_x;
        pdf_float_t wmax = (w-2*m_cMargin)*1000.0/m_FontSize;
        std::string stmp, s = txt ? txt : "";
        _erasestrch(s, '\r');
        int nb = s.size();
        if(nb>0 && s[nb-1]=='\n') --nb;
        std::string b, b2;
        if(border)
        {
            if(border[0]=='1')
            {
                border = "LTRB";
                b = "LRT";
                b2 = "LR";
            }
            else
            {
                if(strchr(border,'L')) b2 += "L";
                if(strchr(border,'R')) b2 += "R";
                b = strchr(border,'T') ? b2+"T" : b2;
            }
        }
        int sep = -1;
        int i = 0;
        int j = 0;
        int l = 0;
        int ns = 0;
        int nl = 1;
        int ls = 0;
        while(i<nb)
        {
            // Get next character
            unsigned char c = (unsigned char)s[i];
            if(c=='\n')
            {
                // Explicit line break
                if(m_ws>0)
                {
                    m_ws = 0;
                    _out("0 Tw");
                }
                stmp = s.substr(j, i-j);
                Cell(w,h,stmp,b.c_str(),2,align,fill);
                ++i;
                sep = -1;
                j = i;
                l = 0;
                ns = 0;
                ++nl;
                if(border && nl==2)	b = b2;
                continue;
            }
            if(c==' ')
            {
                sep = i;
                ls = l;
                ++ns;
            }
            l += cw[c];
            if(l>wmax)
            {
                // Automatic line break
                if(sep==-1)
                {
                    if(i==j) ++i;
                    if(m_ws>0)
                    {
                        m_ws = 0;
                        _out("0 Tw");
                    }
                    stmp = s.substr(j, i-j);
                    Cell(w,h,stmp,b.c_str(),2,align,fill);
                }
                else
                {
                    if(align=='J')
                    {
                        m_ws = (ns>1) ? (wmax-ls)/1000.0*m_FontSize/(ns-1) : 0;
                        //printf("%d %f %f %d %f\n", ns, m_ws, wmax, ls, m_FontSize);
                        _outfmt(true, "%.3f Tw",m_ws*m_k);
                    }
                    stmp = s.substr(j, sep-j);
                    Cell(w,h,stmp,b.c_str(),2,align,fill);
                    i = sep+1;
                }
                sep = -1;
                j = i;
                l = 0;
                ns = 0;
                ++nl;
                if(border && nl==2)
                    b = b2;
            }
            else
                ++i;
        }
        // Last chunk
        if(m_ws>0)
        {
            m_ws = 0;
            _out("0 Tw");
        }
        if(border && strchr(border,'B'))
            b += "B";
        stmp = s.substr(j, i-j);
        Cell(w,h,stmp,b.c_str(),2,align,fill);
        m_x = m_lMargin;
    }
#line 1301 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
int FPDF::CalcLines (pdf_float_t w, char const * txt)
#line 1302 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        //Computes the number of lines a MultiCell of width w will take
        font_width_t *cw = m_CurrentFont->font.cw;
        if(w==0) w = m_w-m_rMargin-m_x;
        pdf_float_t wmax = (w-2*m_cMargin)*1000.0/m_FontSize;
        std::string stmp, s = txt ? txt : "";
        _erasestrch(s, '\r');
        int nb = s.size();
        if(nb>0 && s[nb-1]=='\n') --nb;

        int sep = -1;
        int i = 0;
        int j = 0;
        int l = 0;
        int nl = 1;
        while(i<nb)
        {
            // Get next character
            unsigned char c = (unsigned char)s[i];
            if(c == '\n')
            {
                ++i;
                sep = -1;
                j = i;
                l = 0;
                ++nl;
                continue;
            }
            if(c == ' ') sep=i;
            l += cw[c];
            if(l > wmax)
            {
                if(sep == -1)
                {
                    if(i == j) ++i;
                }
                else i = sep+1;
                sep = -1;
                j = i;
                l = 0;
                ++nl;
            }
            else
                ++i;
        }
        return nl;
    }
#line 1351 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::MultiCellBlt (pdf_float_t w, pdf_float_t h, char const * blt, char const * txt, char const * border, char align, bool fill)
#line 1354 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        //Get bullet width including margins
        pdf_float_t blt_width = GetStringWidth(blt)+m_cMargin*2;

        //Save x
        pdf_float_t bak_x = m_x;

        //Output bullet
        Cell(blt_width, h, blt,0,' ', fill);

        //Output text
        MultiCell(w-blt_width, h, txt, border, align, fill);

        //Restore x
        m_x = bak_x;
    }
#line 1371 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::ClippingRect (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, bool outline)
#line 1372 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        char op=outline ? 'S' : 'n';
        _outfmt(true, "q %.2f %.2f %.2f %.2f re W %c",
            x*m_k, (m_h-y)*m_k, w*m_k,-h*m_k, op);
    }
#line 1378 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::UnsetClipping ()
#line 1379 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _out("Q");
    }
#line 1383 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::ClippedCell (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, int ln, char align, bool fill, int link)
#line 1385 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(border || fill || m_y+h>m_PageBreakTrigger)
        {
            Cell(w,h,0,border,0,' ',fill);
            m_x-=w;
        }
        ClippingRect(m_x,m_y,w,h);
        Cell(w,h,txt,0,ln,align,false,link);
        UnsetClipping();
    }
#line 1397 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::CellFit (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, int ln, char align, bool fill, int link, bool scale, bool force)
#line 1400 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        //Get string width
        pdf_float_t str_width=GetStringWidth(txt);

        //Calculate ratio to fit cell
        if(w==0.0f) w = m_w-m_rMargin-m_x;
        pdf_float_t ratio = (w-m_cMargin*2)/str_width;

        bool fit = (ratio < 1 || (ratio > 1 && force));
        if (fit)
        {
            if (scale)
            {
                //Calculate horizontal scaling
                pdf_float_t horiz_scale=ratio*100.0;
                //Set horizontal scaling
                _outfmt(true, "BT %.2f Tz ET", horiz_scale);
            }
            else
            {
                //Calculate character spacing in points
                // TODO (mingo2#1#): UTF-8 strlen
                int txt_size = txt ? strlen(txt) : 0;
                pdf_float_t char_space=(w-m_cMargin*2-str_width)/std::max(txt_size-1,1)*m_k;
                //Set character spacing
                _outfmt(true, "BT %.2f Tc ET", char_space);
            }
            //Override user alignment (since text will fill up cell)
            align=' ';
        }

        //Pass on to Cell method
        Cell(w,h,txt,border,ln,align,fill,link);

        //Reset character spacing/horizontal scaling
        if (fit)
        {
            _out("BT ", false);
            _out(scale ? "100 Tz" : "0 Tc", false);
            _out(" ET");
        }
    }
#line 1516 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Write (pdf_float_t h, char const * txt, int link)
#line 1517 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Output text in flowing mode
        font_width_t *cw = m_CurrentFont->font.cw;
        pdf_float_t w = m_w-m_rMargin-m_x;
        pdf_float_t wmax = (w-2*m_cMargin)*1000.0/m_FontSize;
        std::string stmp, s = txt ? txt : "";
        _erasestrch(s, '\r');
        int nb = s.size();
        int sep = -1;
        int i = 0;
        int j = 0;
        int l = 0;
        int nl = 1;
        while(i<nb)
        {
            // Get next character
            unsigned char c = (unsigned char)s[i];
            if(c=='\n')
            {
                // Explicit line break
                stmp = s.substr(j, i-j);
                Cell(w,h,stmp,0,2,' ',0,link);
                i++;
                sep = -1;
                j = i;
                l = 0;
                if(nl==1)
                {
                    m_x = m_lMargin;
                    w = m_w-m_rMargin-m_x;
                    wmax = (w-2*m_cMargin)*1000.0/m_FontSize;
                }
                nl++;
                continue;
            }
            if(c==' ') sep = i;
            l += cw[c];
            if(l>wmax)
            {
                // Automatic line break
                if(sep==-1)
                {
                    if(m_x > m_lMargin)
                    {
                        // Move to next line
                        m_x = m_lMargin;
                        m_y += h;
                        w = m_w-m_rMargin-m_x;
                        wmax = (w-2*m_cMargin)*1000.0/m_FontSize;
                        i++;
                        nl++;
                        continue;
                    }
                    if(i==j) i++;
                    stmp = s.substr(j, i-j);
                    Cell(w,h,stmp,0,2,' ',0,link);
                }
                else
                {
                    stmp = s.substr(j, sep-j);
                    Cell(w,h,stmp,0,2,' ',0,link);
                    i = sep+1;
                }
                sep = -1;
                j = i;
                l = 0;
                if(nl==1)
                {
                    m_x = m_lMargin;
                    w = m_w-m_rMargin-m_x;
                    wmax = (w-2*m_cMargin)*1000.0/m_FontSize;
                }
                nl++;
            }
            else
                i++;
        }
        // Last chunk
        if(i!=j)
        {
            stmp = s.substr(j, j);
            Cell(l/1000.0*m_FontSize,h,stmp,0,0,' ',0,link);
        }
    }
#line 1602 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Ln (pdf_float_t h)
#line 1603 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Line feed; default value is last cell height
        m_x = m_lMargin;
        if(h==0.0) m_y += m_lasth;
        else m_y += h;
    }
#line 1612 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Image (char const * image_name, unsigned char const * image_blob, size_t blob_size, pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, char const * atype, int link)
#line 1617 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        st_image info;
        // Put an image on the page
        if(!image_name) Error("Image name is null !");
        if(m_images.find(image_name) == m_images.end())
        {
            // First use of this image, get info
            std::string type;
            if(atype) type = atype;
            if(type.empty())
            {
                if(image_blob)
                    Error("Image type was not specified: %s", image_name);
                //else assume image_name is a filename
                const char *pos = strrchr(image_name,'.');
                if(!pos)
                    Error("Image file has no extension and no type was specified: %s", image_name);
                type = pos+1;
            }
            _str_tolower(type);
            if(type=="jpeg") type = "jpg";
            if(type == "jpg") {
                if(image_blob) _parsejpg_blob(info, image_name, image_blob, blob_size);
                else _parsejpg(info, image_name);
            }
            else if(type == "png") {
                if(image_blob) _parsepng_blob(info, image_name, image_blob, blob_size);
                else _parsepng(info, image_name);
            }
            else Error("Unsupported image type: %s", type.c_str());
            info.i = m_images.size()+1;
            m_images[image_name] = info;
        }
        else
            info = m_images[image_name];
        // Automatic width and height calculation if needed
        if(w==0.0 && h==0.0)
        {
            // Put image at 96 dpi
            w = -96;
            h = -96;
        }
        else
        {
        if(w==0) w = h* info.w/info.h;
        else if(w<0)	w = - info.w*72.0/w/m_k;
        if(h==0) h = w* info.h/info.w;
        else if(h<0) h = - info.h*72.0/h/m_k;
        }

        pdf_float_t _y, _x;
        // Flowing mode
        if(y==-1)
        {
            if(m_y+h>m_PageBreakTrigger && !m_InHeader && !m_InFooter && AcceptPageBreak())
            {
                // Automatic page break
                pdf_float_t x2 = m_x;
                AddPage(m_CurOrientation, &m_CurPageSize);
                m_x = x2;
            }
            _y = m_y;
            m_y += h;
        }
        else _y = y;

        if(x==-1) _x = m_x;
        else _x = x;
        _outfmt(true, "q %.2f 0 0 %.2f %.2f %.2f cm /I%d Do Q",w*m_k,h*m_k,_x*m_k,(m_h-(_y+h))*m_k,info.i);
        if(link) Link(_x,_y,w,h,link);
    }
#line 1689 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Image (char const * file, pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, char const * atype, int link)
#line 1692 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        Image(file, 0, 0, x, y, w, h, atype, link);
    }
#line 1702 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetX (pdf_float_t x)
#line 1703 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set x position
        if(x>=0) m_x = x;
        else m_x = m_w+x;
    }
#line 1715 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::SetY (pdf_float_t y)
#line 1716 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Set y position and reset x
        m_x = m_lMargin;
        if(y>=0) m_y = y;
        else m_y = m_h+y;
    }
#line 1742 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string FPDF::Output (char const * name, char dest)
#line 1743 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Output PDF to some destination
        if(m_state<3) Close();
        if(dest==' ')
        {
            if(!name)
            {
                name = "doc.pdf";
                dest = 'I';
            }
            else
                dest = 'F';
        }
        switch(dest)
        {
#if 0
        case 'I':
            // Send to standard output
            _checkoutput();
            if(PHP_SAPI!="cli")
            {
                // We send to a browser
                header("Content-Type: application/pdf");
                header("Content-Disposition: inline; filename="".name.""");
                header("Cache-Control: private, max-age=0, must-revalidate");
                header("Pragma: public");
            }
            echo m_buffer;
            break;
        case 'D':
            // Download file
            _checkoutput();
            header("Content-Type: application/x-download");
            header("Content-Disposition: attachment; filename="".name.""");
            header("Cache-Control: private, max-age=0, must-revalidate");
            header("Pragma: public");
            echo m_buffer;
            break;
#endif
        case 'F':
        {
            // Save to local file
            FILE *f = fopen(name,"wb");
            if(!f)
                Error("Unable to create output file: %s", name);
            fwrite(m_buffer.c_str(), m_buffer.size(), 1, f);
            fclose(f);
        }
        break;
        case 'S':
            // Return as a string
            return m_buffer;
        default:
            Error("Incorrect output destination: %c", dest);
        }
        return "";
    }
#line 1801 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::RoundedRect (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, pdf_float_t r, char const * style)
#line 1803 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        pdf_float_t k = m_k;
        pdf_float_t hp = m_h;
        pdf_float_t MyArc = 4.0/3.0 * (sqrt(2) - 1);
        _outfmt(true, "%.2f %.2f m",(x+r)*k,(hp-y)*k);
        pdf_float_t xc = x+w-r;
        pdf_float_t yc = y+r;
        _outfmt(true, "%.2f %.2f l", xc*k,(hp-y)*k);

        _Arc(xc + r*MyArc, yc - r, xc + r, yc - r*MyArc, xc + r, yc);
        xc = x+w-r;
        yc = y+h-r;
        _outfmt(true, "%.2f %.2f l",(x+w)*m_k,(hp-yc)*m_k);
        _Arc(xc + r, yc + r*MyArc, xc + r*MyArc, yc + r, xc, yc + r);
        xc = x+r;
        yc = y+h-r;
        _outfmt(true, "%.2f %.2f l",xc*m_k,(hp-(y+h))*m_k);
        _Arc(xc - r*MyArc, yc + r, xc - r, yc + r*MyArc, xc - r, yc);
        xc = x+r;
        yc = y+r;
        _outfmt(true, "%.2F %.2F l",(x)*m_k,(hp-yc)*m_k);
        _Arc(xc - r, yc - r*MyArc, xc - r*MyArc, yc - r, xc, yc - r);
        std::string str_style = style ? style : "";
        if(str_style=="F") _out("f");
        else if(str_style=="FD" || str_style=="DF") _out("B");
        else _out("S");
    }
#line 1831 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Circle (pdf_float_t x, pdf_float_t y, pdf_float_t r, char const * style)
#line 1832 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        Ellipse(x,y,r,r,style);
    }
#line 1836 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::Ellipse (pdf_float_t x, pdf_float_t y, pdf_float_t rx, pdf_float_t ry, char const * style)
#line 1838 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        const char *op = "S";
        std::string str_style = style ? style : "";
        if(str_style=="F") op ="f";
        else if(str_style=="FD" || str_style=="DF") op ="B";
        pdf_float_t tmp = 4.0/3.0*(sqrt(2)-1);
        pdf_float_t lx=tmp*rx;
        pdf_float_t ly=tmp*ry;
        _outfmt(true, "%.2f %.2f m %.2f %.2f %.2f %.2f %.2f %.2f c",
                (x+rx)*m_k,(m_h-y)*m_k,
                (x+rx)*m_k,(m_h-(y-ly))*m_k,
                (x+lx)*m_k,(m_h-(y-ry))*m_k,
                x*m_k,(m_h-(y-ry))*m_k);
        _outfmt(true, "%.2f %.2f %.2f %.2f %.2f %.2f c",
                (x-lx)*m_k,(m_h-(y-ry))*m_k,
                (x-rx)*m_k,(m_h-(y-ly))*m_k,
                (x-rx)*m_k,(m_h-y)*m_k);
        _outfmt(true, "%.2f %.2f %.2f %.2f %.2f %.2f c",
                (x-rx)*m_k,(m_h-(y+ly))*m_k,
                (x-lx)*m_k,(m_h-(y+ry))*m_k,
                x*m_k,(m_h-(y+ry))*m_k);
        _outfmt(true, "%.2f %.2f %.2f %.2f %.2f %.2f c %s",
                (x+lx)*m_k,(m_h-(y+ry))*m_k,
                (x+rx)*m_k,(m_h-(y+ly))*m_k,
                (x+rx)*m_k,(m_h-y)*m_k,
                op);
    }
#line 1866 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::IncludeJS (char const * script)
#line 1867 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        m_javascript=script;
    }
#line 1878 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_Arc (pdf_float_t x1, pdf_float_t y1, pdf_float_t x2, pdf_float_t y2, pdf_float_t x3, pdf_float_t y3)
#line 1879 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        pdf_float_t h = m_h;
        _outfmt(true, "%.2f %.2f %.2f %.2f %.2f %.2f c ", x1*m_k,
                (h-y1)*m_k, x2*m_k, (h-y2)*m_k, x3*m_k, (h-y3)*m_k);
    }
#line 1885 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string & FPDF::_erasestrch (std::string & str, char c)
#line 1886 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(!str.empty())
        {
            size_t size = str.size();
            //int used intentionally for the case of 0nt char should be erased
            for(size_t i=0; i<size; ++i)
            {
                if(str.at(i) == c)
                {
                    str.erase(i, 1);
                    --size;
                    --i;
                }
            }
        }
        return str;
    }
#line 1904 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_str_tolower (std::string & str)
#line 1905 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        for(size_t i=0, size=str.size(); i < size; ++i)
        {
            str[i] = tolower(str[i]);
        }
    }
#line 1912 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_dochecks ()
#line 1913 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {}
#line 1916 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_checkoutput ()
#line 1917 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
#if 0

        if(PHP_SAPI!="cli")
        {
            if(headers_sent(file,line))
                Error("Some data has already been output, can't send PDF file (output started at file:line)");
        }
        if(ob_get_length())
        {
            // The output buffer is not empty
            if(preg_match("/^(\xEF\xBB\xBF)?\s*/",ob_get_contents()))
            {
                // It contains only a UTF-8 BOM and/or whitespace, let"s clean it
                ob_clean();
            }
            else
                Error("Some data has already been output, can't send PDF file");
        }
#endif
    }
#line 1939 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::st_pagesize & FPDF::_getpagesize (st_pagesize & result, e_page_sizes size)
#line 1940 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        switch(size)
        {
        case e_A3:
            result.w = 841.89;
            result.h = 1190.55;
            break;
        case e_A5:
            result.w = 420.94;
            result.h = 595.28;
            break;
        case e_Letter:
            result.w = 612;
            result.h = 792;
            break;
        case e_Legal:
            result.w = 612;
            result.h = 1008;
            break;
        default:
            /*case e_A4:*/
            result.w = 595.28;
            result.h = 841.89;
            break;
        }

        result.w /= m_k;
        result.h /=m_k;
        return result;
    }
#line 1971 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_beginpage (e_orientation orientation, st_pagesize * size)
#line 1972 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        m_page++;
        m_pages.resize(m_page);
        m_state = 2;
        m_x = m_lMargin;
        m_y = m_tMargin;
        m_FontFamily = "";
        // Check page size and orientation
        if(orientation==e_orientation_none) orientation = m_DefOrientation;
        st_pagesize *psize;
        if(!size) psize = &m_DefPageSize;
        else psize = size;
        if(orientation!=m_CurOrientation || psize->w != m_CurPageSize.w || psize->h  != m_CurPageSize.h)
        {
            // New size or orientation
            if(orientation==e_orientation_portrait)
            {
                m_w = psize->w;
                m_h = psize->h;
            }
            else
            {
                m_w = psize->h;
                m_h = psize->w;
            }
            m_wPt = m_w*m_k;
            m_hPt = m_h*m_k;
            m_PageBreakTrigger = m_h-m_bMargin;
            m_CurOrientation = orientation;
            m_CurPageSize = *psize;
        }
        if(orientation != m_DefOrientation || psize->w != m_DefPageSize.w || psize->h != m_DefPageSize.h)
        {
            st_pagesize ps;
            ps.w = m_wPt;
            ps.h = m_hPt;
            m_PageSizes[m_page] = ps;
        }

        if ( m_doubleSided )
        {
            if( m_page % 2 == 0 )
            {
                m_xDelta=m_outerMargin - m_innerMargin;
                SetLeftMargin(m_outerMargin);
                SetRightMargin(m_innerMargin);
            }
            else
            {
                m_xDelta=m_innerMargin - m_outerMargin;
                SetLeftMargin(m_innerMargin);
                SetRightMargin(m_outerMargin);
            }
            m_x=m_lMargin;
            m_y=m_tMargin;
        }
    }
#line 2051 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string FPDF::_escape (std::string const & s)
#line 2052 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        std::string str = s;
        if(!str.empty())
        {
            size_t size = str.size();
            for(size_t i=0; i<size; ++i)
            {
                switch((unsigned char)str.at(i))
                {
                case '\\':
                case '(':
                case ')':
                    str.insert(i, "\\");
                    ++size;
                    ++i;
                    continue;
                case '\r':
                    str.erase(i, 1);
                    --size;
                    continue;
                //now a cheap ut82latin
                case 0xE2: //euro 0xE282AC = \200
                    if(((unsigned char)str[i+1]) == 0x82 && ((unsigned char)str[i+2]) == 0xAC)
                    {
                        str[i] = '\\';
                        str[i+1] = '2';
                        str[i+2] = '0';
                        str.insert(i+3, "0");
                        size += 1;
                        i += 3;
                    }
                    continue;
                case 0xC2:
                    str.erase(i, 1);
                    --size;
                    continue;
                case 0xC3:
                    str.erase(i, 1);
                    str[i] += 64;
                    --size;
                    --i;
                    continue;
                }
            }
        }
        return str;
    }
#line 2100 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_textstring (std::string & result, std::string const & s)
#line 2101 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Format a text string
        result = "(" + _escape(s) + ")";
    }
#line 2141 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
int FPDF::substr_count (char const * str, char c)
#line 2142 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        int result = 0;
        if(str)
        {
            while(*str)
            {
                if(*str++ == c) result++;
            }
        }
        return result;
    }
#line 2154 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string FPDF::_dounderline (pdf_float_t x, pdf_float_t y, char const * txt)
#line 2155 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Underline text
        int up = m_CurrentFont->font.up;
        int ut = m_CurrentFont->font.ut;
        int w = GetStringWidth(txt)+m_ws*substr_count(txt,' ');
        pdf_snprintf(m_scratch_buf, sizeof(m_scratch_buf), "%.2f %.2f %.2f %.2f re f",x*m_k,(m_h-(y-up/1000.0*m_FontSize))*m_k,w*m_k,-ut/1000.0*m_FontSizePt);
        return m_scratch_buf;
    }
#line 2169 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::blob_stream_t::~ blob_stream_t ()
#line 2169 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                                {}
#line 2176 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::blob_stream_memory_t::blob_stream_memory_t (unsigned char const * ablob, size_t asize)
#line 2176 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                                                                      {
            blob = ablob;
            size = asize;
            offset = 0;
        }
#line 2181 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
bool FPDF::blob_stream_memory_t::eof ()
#line 2181 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                  {
            return offset == size;
        }
#line 2184 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
size_t FPDF::blob_stream_memory_t::tell ()
#line 2184 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                     {
            return offset;
        }
#line 2188 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
size_t FPDF::blob_stream_memory_t::read (void * dest, size_t num_read)
#line 2189 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
        {
          if (offset + num_read > size)
            num_read = size - offset;

          memcpy ((char *)dest, blob + offset, num_read);

          offset += num_read;

          return num_read;
        }
#line 2200 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::blob_stream_memory_t::seek (size_t to_offset, int whence)
#line 2201 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
        {
          size_t npos;

          switch (whence) {
            case SEEK_SET:
              npos = to_offset;
              break;

            case SEEK_CUR:
              npos = offset + to_offset;
              break;

            case SEEK_END:
              npos = size;
              break;
          }

          offset = npos > size ? size : npos;
        }
#line 2224 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::blob_stream_file_t::blob_stream_file_t (FILE * afp)
#line 2224 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                                     {
            fp = afp;
        }
#line 2227 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
FPDF::blob_stream_file_t::~ blob_stream_file_t ()
#line 2227 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                             {
            if(fp) fclose(fp);
        }
#line 2230 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
bool FPDF::blob_stream_file_t::eof ()
#line 2230 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                  {
            return feof(fp);
        }
#line 2233 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
size_t FPDF::blob_stream_file_t::tell ()
#line 2233 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                     {
            return ftell(fp);
        }
#line 2237 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
size_t FPDF::blob_stream_file_t::read (void * dest, size_t num_read)
#line 2238 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
        {
          return fread(dest, 1, num_read, fp);
        }
#line 2242 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::blob_stream_file_t::seek (size_t to_offset, int whence)
#line 2243 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
        {
            fseek(fp, to_offset, whence);
        }
#line 2273 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_parsejpg (st_image & info, blob_stream_t & fp, char const * image_name)
#line 2274 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        bool isValid = false;
        info.cs.clear();
        info.pal.clear();
        info.trns.clear();
        info.data.clear();

        std::string buf;
        _readstream(buf,fp,3);
        if (buf != "\xff\xd8\xff")
            Error("Not a JPG image: %s", image_name);

        // Extract info from a JPEG file
        unsigned int   marker = M_PSEUDO;
        unsigned short length, ffRead = 1;
        unsigned char  bits     = 0;
        unsigned short height   = 0;
        unsigned short width    = 0;
        unsigned char  channels = 0;


        bool ready = false;
        int lastMarker;
        int commentCorrection;
        int a;
        while (!ready)
        {
            lastMarker = marker;
            commentCorrection = 1;
            a = 0;

            // get marker byte, swallowing possible padding
            if (lastMarker == M_COM && commentCorrection)
            {
                // some software does not count the length bytes of COM section
                // one company doing so is very much envolved in JPEG... so we accept too
                // by the way: some of those companies changed their code now...
                commentCorrection = 2;
            }
            else
            {
                lastMarker = 0;
                commentCorrection = 0;
            }
            if (ffRead)
            {
                a = 1; // already read 0xff in filetype detection
            }
            do
            {
                _readstream(buf,fp,1);
                if (fp.eof())
                {
                    marker = M_EOI; // we hit EOF
                    break;
                }
                marker = (unsigned char)buf[0];
                if (lastMarker == M_COM && commentCorrection > 0)
                {
                    if (marker != 0xFF)
                    {
                        marker = 0xff;
                        commentCorrection--;
                    }
                    else
                    {
                        lastMarker = M_PSEUDO; // stop skipping non 0xff for M_COM
                    }
                }
                if (++a > 10)
                {
                    // who knows the maxim amount of 0xff? though 7
                    // but found other implementations
                    marker = M_EOI;
                    break;
                }
            }
            while (marker == 0xff);

            if (a < 2)
            {
                marker = M_EOI; // at least one 0xff is needed before marker code
            }
            if (lastMarker == M_COM && commentCorrection)
            {
                marker = M_EOI; // ah illegal: char after COM section not 0xFF
            }

            ffRead = 0;
            switch (marker)
            {
            case M_SOF0:
            case M_SOF1:
            case M_SOF2:
            case M_SOF3:
            case M_SOF5:
            case M_SOF6:
            case M_SOF7:
            case M_SOF9:
            case M_SOF10:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
                // handle SOFn block
                length = (unsigned short)_readshort(fp);
                _readstream(buf, fp,1);
                bits = (unsigned char)buf[0];
                height = (unsigned short)_readshort(fp);
                width  = (unsigned short)_readshort(fp);
                _readstream(buf, fp,1);
                channels = (unsigned char)buf[0];
                isValid = true;
                ready = true;
                break;

            case M_SOS:
            case M_EOI:
                isValid = false;
                ready = true;

            default:
            {
                // anything else isn't interesting
                long pos = (unsigned short) _readshort(fp);
                pos = pos-2;
                if (pos)
                {
                    fp.seek(pos, SEEK_CUR );
                }
            }
            break;
            }
        }

        if (isValid)
        {
            if (channels == 3)
            {
                info.cs = "DeviceRGB";
            }
            else if(channels == 4)
            {
                info.cs = "DeviceCMYK";
            }
            else
            {
                info.cs = "DeviceGray";
            }
            info.bpc = bits;

            //Read whole file
            fp.seek(0, SEEK_END);
            int fsize = fp.tell();
            fp.seek(0, SEEK_SET);
            _readstream(info.data, fp, fsize);
            info.w  = width;
            info.h = height;
            info.f = "DCTDecode";
            return;
        }
        Error("Invalid JPG image: %s", image_name);
    }
#line 2438 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_parsejpg (st_image & info, char const * file_name)
#line 2439 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Extract info from a JPG file
        FILE *fp = fopen(file_name,"rb");

        if(!fp) Error("Can't open image file: %s", file_name);
        blob_stream_file_t sf(fp);
        _parsejpg(info, sf, file_name);
    }
#line 2448 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_parsejpg_blob (st_image & info, char const * image_name, unsigned char const * image_blob, size_t blob_size)
#line 2450 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(!image_name) Error("Image name is NULL!");
        if(!image_blob) Error("Image blob is NULL!");
        if(!blob_size) Error("Image blob size is zero!");
        blob_stream_memory_t sm(image_blob, blob_size);
        _parsejpg(info, sm, image_name);
    }
#line 2458 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_parsepng (st_image & info, char const * file_name)
#line 2459 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Extract info from a PNG file
        FILE *fp = fopen(file_name,"rb");

        if(!fp) Error("Can't open image file: %s", file_name);
        blob_stream_file_t sf(fp);
        _parsepngstream(info, sf, file_name);
    }
#line 2469 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_parsepng_blob (st_image & info, char const * image_name, unsigned char const * image_blob, size_t blob_size)
#line 2471 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(!image_name) Error("Image name is NULL!");
        if(!image_blob) Error("Image blob is NULL!");
        if(!blob_size) Error("Image blob size is zero!");
        blob_stream_memory_t sm(image_blob, blob_size);
        _parsepngstream(info, sm, image_name);
    }
#line 2479 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_getGrayImgColorAndalpha (std::string & color, std::string & alpha, std::string & line)
#line 2479 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                                                                                            {
        size_t size = line.size();
        color.reserve(size);
        alpha.reserve(size);
        for(size_t i=0; i<size; i+=2){
            color += line[i];
            alpha += line[i+1];
        }
    }
#line 2489 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_getRGBImgColorAndalpha (std::string & color, std::string & alpha, std::string & line)
#line 2489 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
                                                                                           {
        size_t size = line.size();
        color.reserve(size);
        alpha.reserve(size/3);
        for(size_t i=0; i<size; i+=4){
            color.append(line.c_str()+i, 3);
            alpha += line[i+3];
        }
    }
#line 2499 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_parsepngstream (st_image & info, blob_stream_t & fp, char const * image_name)
#line 2500 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        std::string buf;
        // Check signature
        _readstream(buf, fp, 8);

        if(buf != "\x89PNG\x0D\x0A\x1A\x0A")
            Error("Not a PNG image: %s", image_name);

        // Read header chunk
        _readstream(buf, fp, 4);
        _readstream(buf, fp, 4);
        if(buf != "IHDR") Error("Incorrect PNG image: %s", image_name);
        info.w = _readint(fp);
        info.h = _readint(fp);
        _readstream(buf, fp, 1);
        info.bpc = buf[0];
        if(info.bpc>8) Error("16-bit depth not supported: %s", image_name);
        _readstream(buf, fp, 1);
        int ct = buf[0];
        if(ct==0 || ct==4) info.cs = "DeviceGray";
        else if(ct==2 || ct==6) info.cs = "DeviceRGB";
        else if(ct==3) info.cs = "Indexed";
        else Error("Unknown color type: %s", image_name);
        _readstream(buf, fp, 3);
        if(buf[0]!=0)
            Error("Unknown compression method: %s", image_name);
        if(buf[1]!=0)
            Error("Unknown filter method: %s", image_name);
        if(buf[2]!=0)
            Error("Interlacing not supported: %s", image_name);
        _readstream(buf,fp,4);
        pdf_sprintf(info.dp, "/Predictor 15 /Colors %d /BitsPerComponent %d  /Columns %d",
                (info.cs=="DeviceRGB" ? 3 : 1), (int)info.bpc, (int)info.w);

        // Scan chunks looking for palette, transparency and image data
        info.pal.clear();
        info.trns.clear();
        info.data.clear();
        int n;
        do
        {
            n = _readint(fp);
            _readstream(buf, fp, 4);
            if(buf=="PLTE")
            {
                // Read palette
                _readstream(info.pal, fp,n);
                _readstream(buf, fp, 4);
            }
            else if(buf=="tRNS")
            {
                // Read transparency info
                _readstream(buf,fp,n);
                if(ct==0)
                {
                    info.trns.push_back(buf[1]);
                }
                else if(ct==2)
                {
                    info.trns.push_back(buf[1]);
                    info.trns.push_back(buf[3]);
                    info.trns.push_back(buf[5]);
                }
                else
                {
                    size_t pos = buf.find('\0');
                    if(pos!=std::string::npos)
                        info.trns.push_back(buf[pos]);
                }
                _readstream(buf,fp,4);
            }
            else if(buf=="IDAT")
            {
                // Read image data block
                info.data += _readstream(buf,fp,n);
                _readstream(buf, fp, 4);
            }
            else if(buf=="IEND")
                break;
            else
                _readstream(buf,fp,n+4);
        }
        while(n);

        if(info.cs=="Indexed" && info.pal.empty()) Error("Missing palette in %s", image_name);
        info.f = "FlateDecode";
#ifdef PDF_USING_ZLIB
        if(ct>=4)
        {
            // Extract alpha channel
            std::string data;
            gzuncompress(data, info.data);
            std::string color, alpha, line;
            if(ct==4)
            {
                // Gray image
                int len = 2*info.w;
                for(int i=0; i<info.h; i++)
                {
                    int pos = (1+len)*i;
                    color.append(data[pos], 1);
                    alpha.append(data[pos], 1);
                    line.assign(data.c_str()+pos+1,len);
                    _getGrayImgColorAndalpha(color, alpha, line);
                    //color += preg_replace("/(.)./s","1",line);
                    //alpha += preg_replace("/.(.)/s","1",line);
                }
            }
            else
            {
                // RGB image
                int len = 4*info.w;
                for(int i=0; i<info.h; i++)
                {
                    int pos = (1+len)*i;
                    color.append(data[pos], 1);
                    alpha.append(data[pos], 1);
                    line.assign(data.c_str()+pos+1,len);
                    _getRGBImgColorAndalpha(color, alpha, line);
                    //color += preg_replace("/(.{3})./s","1",line);
                    //alpha += preg_replace("/.{3}(.)/s","1",line);
                }
            }
            gzcompress(info.smask, alpha);
            data.clear();
            gzcompress(info.data, color);
            if(m_PDFVersion < "1.4") m_PDFVersion = "1.4";
        }
#endif
    }
#line 2631 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string & FPDF::_readstream (std::string & result, blob_stream_t & fp, size_t n)
#line 2632 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Read n bytes from stream
        result.resize(n);
        if(fp.read(&result[0], n) != n) Error("Can't read from stream !");
        return result;
    }
#line 2639 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
int FPDF::_readint (blob_stream_t & fp)
#line 2640 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Read a 4-byte integer from stream
        std::string buf;
        _readstream(buf, fp, 4);
        int i = 1;
        char *p = (char *)&i;
        if (p[0] == 1)  //LITTLE_ENDIAN
        {
            unsigned char c0 = buf[0], c1 = buf[1], c2 = buf[2], c3 = buf[3];
            buf[0] = c3;
            buf[1] = c2;
            buf[2] = c1;
            buf[3] = c0;
        }
        //else BIG_ENDIAN
        return *((int*)&buf[0]);
    }
#line 2658 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
int FPDF::_readshort (blob_stream_t & fp)
#line 2659 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Read a 4-byte integer from stream
        std::string buf;
        _readstream(buf, fp, 2);
        short i = 1;
        char *p = (char *)&i;
        if (p[0] == 1)  //LITTLE_ENDIAN
        {
            unsigned char c0 = buf[0], c1 = buf[1];
            buf[0] = c1;
            buf[1] = c0;
        }
        //else BIG_ENDIAN
        return *((short*)&buf[0]);
    }
#line 2675 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_parsegif (std::string & file)
#line 2676 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
#if 0
        // Extract info from a GIF file (via PNG conversion)
        if(!function_exists("imagepng"))
            Error("GD extension is required for GIF support");
        if(!function_exists("imagecreatefromgif"))
            Error("GD has no GIF read support");
        im = imagecreatefromgif(file);
        if(!im)
            Error("Missing or incorrect image file: ".file);
        imageinterlace(im,0);
        f = @fopen("php://temp","rb+");
        if(f)
        {
            // Perform conversion in memory
            ob_start();
            imagepng(im);
            data = ob_get_clean();
            imagedestroy(im);
            fwrite(f,data);
            fseek(fp, 0, SEEK_SET);
            info = _parsepngstream(f,file);
            fclose(f);
        }
        else
        {
            // Use temporary file
            tmp = tempnam(".","gif");
            if(!tmp)
                Error("Unable to create a temporary file");
            if(!imagepng(im,tmp))
                Error("Error while saving to temporary file");
            imagedestroy(im);
            info = _parsepng(tmp);
            unlink(tmp);
        }
        return info;
#endif
    }
#line 2716 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_newobj ()
#line 2717 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Begin a new object
        m_n++;
        m_offsets.push_back(m_buffer.size());
        _outfmt(true, "%d 0 obj", m_n);
    }
#line 2724 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putstream (std::string const & s)
#line 2725 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _out("stream");
        _out(s);
        _out("endstream");
    }
#line 2731 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_out (char const * s, size_t size, bool nl)
#line 2732 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        // Add a line to the document
        if(m_state==2)
        {
            std::string &str = m_pages[m_page-1];
            str.append(s, size);
            if(nl) str += "\n";
        }
        else
        {
            m_buffer.append(s, size);
            if(nl) m_buffer += "\n";
        }
    }
#line 2747 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
std::string & FPDF::pdf_sprintf (std::string & s, char const * fmt, ...)
#line 2748 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        va_list args;
        va_start( args, fmt );
        int size = pdf_vsnprintf(m_scratch_buf, sizeof(m_scratch_buf), fmt, args);
        va_end( args );
        if(size < 0) Error("Too big string passed to sprintf %d", __LINE__);
        s = m_scratch_buf;
        return s;
    }
#line 2758 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::pdf_sprintf_append (std::string & s, char const * fmt, ...)
#line 2759 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        va_list args;
        va_start( args, fmt );
        int size = pdf_vsnprintf(m_scratch_buf, sizeof(m_scratch_buf), fmt, args);
        va_end( args );
        if(size < 0) Error("Too big string passed to sprintf %d", __LINE__);
        s += m_scratch_buf;
    }
#line 2768 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_outfmt (bool nl, char const * fmt, ...)
#line 2769 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        va_list args;
        va_start( args, fmt );
        int size = pdf_vsnprintf(m_scratch_buf, sizeof(m_scratch_buf), fmt, args);
        va_end( args );
        if(size < 0) Error("Too big string passed to sprintf %d", __LINE__);
        _out(m_scratch_buf, size, nl);
    }
#line 2778 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_out (char const * s, bool nl)
#line 2779 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _out(s, strlen(s), nl);
    }
#line 2783 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_out (std::string const & s, bool nl)
#line 2784 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _out(s.c_str(), s.size(), nl);
    }
#line 2788 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putpages ()
#line 2789 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        int nb = m_page;
        if(!m_AliasNbPages.empty())
        {
            // Replace number of pages
            std::string str;
            pdf_sprintf(str, "%d", nb);
            for(int n=0; n<nb; n++)
            {
                std::string &page = m_pages[n];
                for(std::string::size_type loc = page.find(m_AliasNbPages);
                        loc != std::string::npos; loc = page.find(m_AliasNbPages, loc))
                {
                    page.replace(loc, m_AliasNbPages.size(), str, 0, str.size());
                }
            }
        }
        pdf_float_t wPt, hPt;
        if(m_DefOrientation==e_orientation_portrait)
        {
            wPt = m_DefPageSize.w*m_k;
            hPt = m_DefPageSize.h*m_k;
        }
        else
        {
            wPt = m_DefPageSize.h*m_k;
            hPt = m_DefPageSize.w*m_k;
        }
        std::string filter = (m_compress) ? "/Filter /FlateDecode " : "";
        for(int n=1; n<=nb; n++)
        {
            // Page
            _newobj();
            _outfmt(true, "<<\n/Type /Page");
            _outfmt(true, "/Parent 1 0 R");
            if(m_PageSizes.find(n) != m_PageSizes.end())
                _outfmt(true, "/MediaBox [0 0 %.2f %.2f]",m_PageSizes[n].w,m_PageSizes[n].h);
            _outfmt(true, "/Resources 2 0 R");
            if(m_PageLinks.find(n) != m_PageLinks.end())
            {
                // Links
                std::string annots = "/Annots [";
#if 0
                link_map_t::iterator iter = m_PageLinks[n].begin();
                link_map_t::iterator iend = m_PageLinks[n].end();
                for(; iter != iend; ++iter)
                {
                    st_page_link &pl = *iter->second;
                    pdf_snprintf(m_scratch_buf, sizeof(m_scratch_buf), "%.2f %.2f %.2f %.2f",pl[0],pl[1],pl[0]+pl[2],pl[1]-pl[3]);
                    rect = m_scratch_buf;
                    annots += "<<\n/Type /Annot /Subtype /Link /Rect [" +  rect + "] /Border [0 0 0] ";
                    if(is_string(pl[4]))
                        annots += "/A\n<<\n/S /URI /URI " + _3textstring(pl[4]) + "\n>>\n>>\n";
                    else
                    {
                        l = m_links[pl[4]];
                        int lidx = l[0];
                        h = m_PageSizes.find([lidx]) != -1 ? m_PageSizes[lidx].h : hPt;
                        pdf_sprintf_append(annots, "/Dest [%d 0 R /XYZ 0 %.2f null]\n>>\n",1+2*lidx,h-l[1]*m_k);
                    }
                }
#endif
                _out(annots + "]");
            }
            if(m_PDFVersion > "1.3")
                _outfmt(true, "/Group\n<<\n/Type /Group /S /Transparency /CS /DeviceRGB\n>>");
            _outfmt(true, "/Contents %d 0 R\n>>", m_n+1);
            _out("endobj");
            // Page content
            std::string zbuf, &p =
#ifdef PDF_USING_ZLIB
                (m_compress) ? gzcompress(zbuf, m_pages[n-1]) :
#endif
                m_pages[n-1];
            _newobj();
            _out("<<");
            _out(filter, false);
            _outfmt(true, "/Length %d\n>>", p.size());
            _putstream(p);
            _out("endobj");
        }
        // Pages root
        m_offsets[1] = m_buffer.size();
        _out("1 0 obj");
        _out("<<\n/Type /Pages");
        std::string kids = "/Kids [";
        for(int i=0; i<nb; i++) pdf_sprintf_append(kids, "%d 0 R ",(3+2*i));
        _out(kids + "]");
        _outfmt(true, "/Count %d", nb);
        _outfmt(true, "/MediaBox [0 0 %.2f %.2f]", wPt, hPt);
        _out(">>");
        _out("endobj");
    }
#line 2883 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putfonts ()
#line 2884 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
#if 0
        int nf = m_n;
        foreach(m_diffs as diff)
        {
            // Encodings
            _newobj();
            _outfmt(true, "<<\n/Type /Encoding /BaseEncoding /WinAnsiEncoding /Differences [".diff."]\n>>");
            _outfmt(true, "endobj");
        }
        foreach(m_FontFiles as file=>info)
        {
            // Font file embedding
            _newobj();
            m_FontFiles[file].n = m_n;
            font = file_get_contents(m_fontpath + file,true);
            if(!font) Error("Font file not found: ".file);
            compressed = (substr(file,-2)==".z");
            if(!compressed && isset(info.length2))
                font = substr(font,6,info.length1).substr(font,6+info.length1+6,info.length2);
            _outfmt(true, "<<\n/Length %d", strlen(font));
            if(compressed) _outfmt(true, "/Filter /FlateDecode");
            _outfmt(true, "/Length1 %d", info.length1);
            if(isset(info["length2"]))
                _outfmt(true, "/Length2 %d /Length3 0", info.length2);
            _outfmt(true, ">>");
            _putstream(font);
            _outfmt(true, "endobj");
        }
#endif
        font_map_t::iterator iter = m_fonts.begin();
        font_map_t::iterator iend = m_fonts.end();
        for(; iter != iend; ++iter)
        {
            // Font objects
            st_pdf_font_base &font = *iter->second;
            font.n = m_n+1;
            e_font_type type = font.font.type;
            std::string name = font.name;
            if(type==e_font_type_core)
            {
                // Core font
                _newobj();
                _out("<<\n/Type /Font");
                _out("/BaseFont /" + name);
                _out("/Subtype /Type1");
                if(name!="Symbol" && name!="ZapfDingbats")
                    _out("/Encoding /WinAnsiEncoding");
                _out(">>");
                _out("endobj");
            }
#if 0
            else if(type==e_font_type_type1 || type==e_font_type_ttf)
            {
                // Additional Type1 or TrueType/OpenType font
                _newobj();
                _out("<<\n/Type /Font");
                _out("/BaseFont /" + name);
                _out("/Subtype /" + type);
                _out("/FirstChar 32 /LastChar 255");
                _outfmt(true, "/Widths %d 0 R", (m_n+1));
                _outfmt(true, "/FontDescriptor %d 0 R", (m_n+2));
                if(isset(font["diffn"]))
                    _outfmt(true, "/Encoding %d 0 R", nf+font.diffn);
                else
                    _out("/Encoding /WinAnsiEncoding");
                _out(">>");
                _out("endobj");
                // Widths
                _newobj();
                cw = font.cw;
                std::string s = "[";
                s.reserve(1024);
                for(i=32; i<=255; i++)
                    pdf_sprintf_append(s, "%d ", cw[chr(i)]);
                _out(s + "]");
                _out("endobj");
                // Descriptor
                _newobj();
                s = "<<\n/Type /FontDescriptor /FontName /" + name;
                foreach(font["desc"] as k=>v)
                s += " /".k." ".v;
                if(!empty(font["file"]))
                    s += " /FontFile".(type=="Type1" ? "" : "2")." ".m_FontFiles[font["file"]]["n"]." 0 R";
                _outfmt(true, s.">>");
                _outfmt(true, "endobj");
            }
            else
            {
                // Allow for additional types
                mtd = "_put".strtolower(type);
                if(!method_exists(this,mtd))
                    Error("Unsupported font type: ".type);
                mtd(font);
            }
#endif
        }
    }
#line 2983 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putimages ()
#line 2984 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        for(image_map_t::iterator iter = m_images.begin(); iter != m_images.end(); ++iter)
        {
            st_image &image = iter->second;
            _putimage(image);
            image.data.clear();
            image.smask.clear();
        }
    }
#line 2994 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putimage (st_image & info)
#line 2995 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _newobj();
        info.n = m_n;
        _out("<<\n/Type /XObject");
        _out("/Subtype /Image");
        _outfmt(true, "/Width %d", (int)info.w);
        _outfmt(true, "/Height %d", (int)info.h);
        if(info.cs=="Indexed")
            _outfmt(true, "/ColorSpace [/Indexed /DeviceRGB %d %d 0 R]", (info.pal.size())/3-1, (m_n+1));
        else
        {
            _out("/ColorSpace /" + info.cs);
            if(info.cs=="DeviceCMYK")
                _out("/Decode [1 0 1 0 1 0 1 0]");
        }
        _outfmt(true, "/BitsPerComponent %d", info.bpc);
        if(!info.f.empty())
            _out("/Filter /" + info.f);
        if(!info.dp.empty())
            _out("/DecodeParms\n<<\n" + info.dp + "\n>>");
        if(!info.trns.empty())
        {
            std::string trns = "/Mask [";
            for(int i=0, size = info.trns.size(); i<size; i++)
            {
                pdf_sprintf_append(trns, "%d", info.trns[i]);
                trns += " ";
                pdf_sprintf_append(trns, "%d", info.trns[i]);
                trns += " ";
            }
            _out(trns + "]");
        }
        if(!info.smask.empty())
            _outfmt(true, "/SMask %d  0 R", (m_n+1));
        _outfmt(true, "/Length %d\n>>", info.data.size());
        _putstream(info.data);
        _out("endobj");
        // Soft mask
        if(!info.smask.empty())
        {
            std::string dp;
            pdf_sprintf(dp, "/Predictor 15 /Colors 1 /BitsPerComponent 8 /Columns %d", (int)info.w);
            st_image smask = info;
            smask.cs = "DeviceGray";
            smask.bpc = 8;
            smask.dp = dp;
            smask.data = info.smask;
            _putimage(smask);
        }
        // Palette
        if(info.cs=="Indexed")
        {
            std::string filter = (m_compress) ? "/Filter /FlateDecode " : "";
            std::string zbuf, &pal =
#ifdef PDF_USING_ZLIB
                (m_compress) ? gzcompress(zbuf, info.pal) :
#endif
                info.pal;
            _newobj();
            _outfmt(true, "<<\n%s/Length %d\n>>", filter.c_str(), pal.size());
            _putstream(pal);
            _out("endobj");
        }
    }
#line 3060 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putxobjectdict ()
#line 3061 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        for(image_map_t::iterator iter = m_images.begin(); iter != m_images.end(); ++iter)
            _outfmt(true, "/I%d %d 0 R", iter->second.i, iter->second.n);
    }
#line 3066 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putextgstates ()
#line 3067 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        for (size_t i = 1, size = m_extgstates.size(); i <= size; ++i)
        {
            _newobj();
            st_alpha_t &gs = m_extgstates[i-1];
            gs.n = m_n;
            _out("<<\n/Type /ExtGState");
                _outfmt(true, "/ca %0.3f", gs.alpha);
                _outfmt(true, "/CA %0.3f", gs.alpha);
                _outfmt(true, "/BM /%s", gs.bm.c_str());
            _out(">>\nendobj");
        }
    }
#line 3081 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putjavascript ()
#line 3082 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _newobj();
        m_n_js=m_n;
        _outfmt(true, "<<\n/Names [(EmbeddedJS) %d 0 R]\n>>\nendobj", m_n+1);
        _newobj();
        _out("<<\n/S /JavaScript\n/JS (", false);
        _out(_escape(m_javascript), false);
        _out(")\n>>\nendobj");
    }
#line 3092 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putresourcedict ()
#line 3093 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _out("/ProcSet [/PDF /Text /ImageB /ImageC /ImageI]");
        _out("/Font\n<<");
        for(font_map_t::iterator iter = m_fonts.begin(); iter != m_fonts.end(); ++iter)
            _outfmt(true, "/F%d %d 0 R", iter->second->i, iter->second->n);
        _outfmt(true, ">>");
        _outfmt(true, "/XObject\n<<");
        _putxobjectdict();
        _outfmt(true, ">>");
        if(m_extgstates.size())
        {
            _out("/ExtGState\n<<");
            for(size_t i = 1, size = m_extgstates.size(); i <= size; ++i)
            {
                _outfmt(true, "/GS%d %d 0 R", i, m_extgstates[i-1].n);
            }
            _out(">>");
        }
    }
#line 3113 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putresources ()
#line 3114 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _putextgstates();
        _putfonts();
        _putimages();
        // Resource dictionary
        m_offsets[2] = m_buffer.size();
        _outfmt(true, "2 0 obj");
        _outfmt(true, "<<");
        _putresourcedict();
        _out(">>\nendobj");
        if (!m_javascript.empty())
        {
            _putjavascript();
        }
    }
#line 3130 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putinfo ()
#line 3131 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        std::string str = _escape(FPDF_VERSION);
        _outfmt(true, "/Producer (CppPDF %s)", str.c_str());
        if(!m_title.empty())
        {
            str = _escape(m_title);
            _out("/Title (" + str + ")");
        }
        if(!m_subject.empty())
        {
            str = _escape(m_subject);
            _out("/Subject (" + str + ")");
        }
        if(!m_author.empty())
        {
            str = _escape(m_author);
            _out("/Author (" + str + ")");
        }
        if(!m_keywords.empty())
        {
            str = _escape(m_keywords);
            _out("/Keywords (" + str + ")");
        }
        if(!m_creator.empty())
        {
            str = _escape(m_creator);
            _out("/Creator (" + str + ")");
        }
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        strftime (m_scratch_buf,sizeof(m_scratch_buf),"/CreationDate (D:%Y%m%d%H%M%S)", timeinfo);
        _out(m_scratch_buf);
    }
#line 3167 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putcatalog ()
#line 3168 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _outfmt(true, "/Type /Catalog");
        _outfmt(true, "/Pages 1 0 R");
        if(m_CustomZoom)
            _outfmt(true, "/OpenAction [3 0 R /XYZ null null %.2f]", m_CustomZoom/100.0);
        else if(m_ZoomMode==e_zoom_fullpage)
            _outfmt(true, "/OpenAction [3 0 R /Fit]");
        else if(m_ZoomMode==e_zoom_fullwidth)
            _outfmt(true, "/OpenAction [3 0 R /FitH null]");
        else if(m_ZoomMode==e_zoom_real)
            _outfmt(true, "/OpenAction [3 0 R /XYZ null null 1]");
        if(m_LayoutMode==e_layout_single)
            _outfmt(true, "/PageLayout /SinglePage");
        else if(m_LayoutMode==e_layout_continuous)
            _outfmt(true, "/PageLayout /OneColumn");
        else if(m_LayoutMode==e_layout_two)
            _outfmt(true, "/PageLayout /TwoColumnLeft");
        if (!m_javascript.empty())
        {
            _outfmt(true, "/Names\n<<\n/JavaScript %d 0 R\n>>", m_n_js);
        }
    }
#line 3191 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_putheader ()
#line 3192 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _out("%PDF-" + m_PDFVersion);
    }
#line 3196 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_puttrailer ()
#line 3197 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        _outfmt(true, "/Size %d", (m_n+1));
        _outfmt(true, "/Root %d 0 R", m_n);
        _outfmt(true, "/Info %d 0 R", (m_n-1));
    }
#line 3203 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
void FPDF::_enddoc ()
#line 3204 "/home/mingo/dev/dadbiz++/ourbiz-uk/parts/fpdf.zz"
    {
        if(m_extgstates.size() && (m_PDFVersion < "1.4")) m_PDFVersion = "1.4";
        _putheader();
        _putpages();
        _putresources();
        // Info
        _newobj();
        _out("<<");
        _putinfo();
        _out(">>\nendobj");
        // Catalog
        _newobj();
        _out("<<");
        _putcatalog();
        _out(">>\nendobj");
        //save begining of xref
        int saved_startxref = m_buffer.size();
        // Cross-ref
        _out("xref");
        _outfmt(true, "0 %d", (m_n+1));
        _out("0000000000 65535 f ");
        for(int i=1; i<=m_n; i++)
        {
            _outfmt(true, "%010d 00000 n ",m_offsets[i]);
        }
        // Trailer
        _out("trailer\n<<");
        _puttrailer();
        _out(">>\nstartxref");
        _outfmt(true, "%d", saved_startxref);
        _out("%%EOF", false);
        m_state = 3;
    }
#undef LZZ_INLINE
