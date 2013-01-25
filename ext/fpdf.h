// fpdf.h
//

#ifndef LZZ_fpdf_h
#define LZZ_fpdf_h
#include <string>
#include <vector>
#include <map>
#include "pdf-font.h"
typedef float pdf_float_t;
#define LZZ_INLINE inline
std::string & gzcompress (std::string & dest, std::string const & src);
std::string & gzuncompress (std::string & dest, std::string const & src);
extern char const * FPDF_FONTPATH;
class FPDF
{
public:
  enum e_orientation
  {
    e_orientation_none,
    e_orientation_portrait,
    e_orientation_landscape
  };
  enum e_units
  {
    e_units_none,
    e_mm,
    e_pt,
    e_cm,
    e_in
  };
  enum e_page_sizes
  {
    e_page_size_none,
    e_A3,
    e_A4,
    e_A5,
    e_Letter,
    e_Legal,
    e_page_sizes_last
  };
  enum e_zoom_mode
  {
    e_zoom_default,
    e_zoom_fullpage,
    e_zoom_fullwidth,
    e_zoom_real
  };
  enum e_layout_mode
  {
    e_layout_default,
    e_layout_single,
    e_layout_continuous,
    e_layout_two
  };
  struct st_image
  {
    pdf_float_t w;
    pdf_float_t h;
    std::string cs;
    std::string dp;
    std::string f;
    std::string parms;
    std::string pal;
    std::string trns;
    std::string imgdata;
    std::string file;
    std::string data;
    std::string smask;
    int i;
    int n;
    int bpc;
  };
  struct st_pagesize
  {
    pdf_float_t w;
    pdf_float_t h;
  };
  struct st_link
  {
    int from;
    int to;
  };
  struct st_page_link
  {
    pdf_float_t x;
    pdf_float_t y;
    pdf_float_t w;
    pdf_float_t h;
    int link;
  };
  struct pdf_color_t
  {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char t;
  };
protected:
  int m_page;
  int m_n;
  std::vector <int> m_offsets;
  std::string m_buffer;
  std::vector <std::string> m_pages;
  int m_state;
  int m_compress;
  pdf_float_t m_k;
  e_orientation m_DefOrientation;
  e_orientation m_CurOrientation;
  st_pagesize m_DefPageSize;
  st_pagesize m_CurPageSize;
  std::map <int, st_pagesize> m_PageSizes;
  pdf_float_t m_wPt;
  pdf_float_t m_hPt;
  pdf_float_t m_w;
  pdf_float_t m_h;
  pdf_float_t m_angle;
  pdf_float_t m_lMargin;
  pdf_float_t m_tMargin;
  pdf_float_t m_rMargin;
  pdf_float_t m_bMargin;
  pdf_float_t m_cMargin;
  pdf_float_t m_x;
  pdf_float_t m_y;
  pdf_float_t m_lasth;
  pdf_float_t m_LineWidth;
  std::string m_fontpath;
  typedef std::map <std::string, st_pdf_font_base*> font_map_t;
  font_map_t m_fonts;
  std::vector <int> m_FontFiles;
  std::vector <int> m_diffs;
  std::string m_FontFamily;
  std::string m_FontStyle;
  bool m_underline;
  st_pdf_font_base * m_CurrentFont;
  pdf_float_t m_FontSizePt;
  pdf_float_t m_FontSize;
  std::string m_DrawColor;
  pdf_color_t m_DrawColor_rgb;
  std::string m_FillColor;
  pdf_color_t m_FillColor_rgb;
  std::string m_TextColor;
  pdf_color_t m_TextColor_rgb;
  bool m_ColorFlag;
  pdf_float_t m_ws;
  typedef std::map <std::string, st_image> image_map_t;
  image_map_t m_images;
  typedef std::map <int, st_page_link> link_map_t;
  link_map_t m_PageLinks;
  std::vector <st_link> m_links;
  bool m_AutoPageBreak;
  pdf_float_t m_PageBreakTrigger;
  bool m_InHeader;
  bool m_InFooter;
  e_zoom_mode m_ZoomMode;
  int m_CustomZoom;
  int m_LayoutMode;
  std::string m_title;
  std::string m_subject;
  std::string m_author;
  std::string m_keywords;
  std::string m_creator;
  std::string m_AliasNbPages;
  std::string m_PDFVersion;
  char (m_scratch_buf) [128];
  struct st_alpha_t
  {
    int n;
    pdf_float_t alpha;
    std::string bm;
  };
  std::vector <st_alpha_t> m_extgstates;
  pdf_float_t m_outerMargin;
  pdf_float_t m_innerMargin;
  pdf_float_t m_xDelta;
  bool m_doubleSided;
  std::string m_javascript;
  int m_n_js;
public:
  virtual ~ FPDF ();
  FPDF ();
  FPDF (e_orientation orientation, e_units unit, e_page_sizes psize);
  FPDF (char orientation, char const * unit, char const * psize);
  void reset (e_orientation orientation = e_orientation_portrait, e_units unit = e_mm, e_page_sizes psize = e_A4);
  void SetDoubleSided (pdf_float_t inner = 7, pdf_float_t outer = 13);
  void SetMargins (pdf_float_t left, pdf_float_t top, pdf_float_t right = 0.0f);
  void SetLeftMargin (pdf_float_t margin);
  void SetTopMargin (pdf_float_t margin);
  void SetRightMargin (pdf_float_t margin);
  pdf_float_t GetLeftMargin ();
  pdf_float_t GetRightMargin ();
  void SetAutoPageBreak (bool b, pdf_float_t margin = 0.0f);
  void CheckPageBreak (pdf_float_t height);
  void setCustomZoom (int zoom);
  int getCustomZoom ();
  void SetDisplayMode (e_zoom_mode zoom, e_layout_mode layout = e_layout_default);
  void SetCompression (bool compress);
  void SetTitle (char const * title);
  void SetSubject (char const * subject);
  void SetAuthor (char const * author);
  void SetKeywords (char const * keywords);
  void SetCreator (char const * creator);
  void AliasNbPages (char const * alias = "{nb}");
  std::string const & GetAliasNbPages ();
  void Error (char const * msg, ...);
  void Open ();
  void Close ();
  void AddPage (e_orientation orientation = e_orientation_none, st_pagesize * psize = 0);
  virtual void Header ();
  virtual void Footer ();
  int PageNo ();
  void SetDrawColor (unsigned char r);
  void SetDrawColor (unsigned char r, unsigned char g, unsigned char b);
  void SetDrawColor (pdf_color_t & color);
  void GetDrawColor (pdf_color_t & color);
  void SetFillColor (unsigned char r);
  void SetFillColor (unsigned char r, unsigned char g, unsigned char b);
  void SetFillColor (pdf_color_t & color);
  void GetFillColor (pdf_color_t & color);
  void SetTextColor (unsigned char r);
  void SetTextColor (unsigned char r, unsigned char g, unsigned char b);
  void SetTextColor (pdf_color_t & color);
  void GetTextColor (pdf_color_t & color);
  void SetAlpha (pdf_float_t alpha, char const * bm = 0);
  pdf_float_t GetStringWidth (char const * s);
  pdf_float_t GetStringWidth (std::string const & s);
  void SetLineWidth (pdf_float_t width);
  void SetDash (pdf_float_t black = -1, pdf_float_t white = -1);
  void Line (pdf_float_t x1, pdf_float_t y1, pdf_float_t x2, pdf_float_t y2);
  void Rect (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, char const * style = 0);
  void AddFont (char const * afamily, char const * astyle = 0, char const * afile = 0);
  struct font_settings_st
  {
    std::string family;
    std::string style;
    pdf_float_t size;
  };
  void GetFontSettings (font_settings_st & fs);
  void SetFontSettings (font_settings_st & fs);
  void SetFont (char const * afamily = 0, char const * astyle = 0, pdf_float_t size = 0);
  void SetFontSize (pdf_float_t size);
  pdf_float_t GetFontSize ();
  int AddLink ();
  void SetLink (int link, pdf_float_t y = 0, int page = -1);
  void Link (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, int link);
protected:
  void _TextBase (std::string & result, pdf_float_t x, pdf_float_t y, char const * txt);
public:
  void Rotate (pdf_float_t angle, pdf_float_t x = -1, pdf_float_t y = -1);
  void RotatedText (pdf_float_t x, pdf_float_t y, char const * txt, pdf_float_t angle);
  void RotatedText (pdf_float_t x, pdf_float_t y, std::string const & txt, pdf_float_t angle);
  void ClippingText (pdf_float_t x, pdf_float_t y, char const * txt, bool outline = false);
  void Text (pdf_float_t x, pdf_float_t y, char const * txt);
  void TextShadow (pdf_float_t x, pdf_float_t y, char const * txt, pdf_float_t displacement = .3);
  virtual bool AcceptPageBreak ();
  void Cell (pdf_float_t w, pdf_float_t h = 0.0f, char const * txt = 0, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void Cell (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void MultiCell (pdf_float_t w, pdf_float_t h, char const * txt, char const * border = 0, char align = 'J', bool fill = false);
  void MultiCell (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border = 0, char align = 'J', bool fill = false);
  int CalcLines (pdf_float_t w, char const * txt);
  void MultiCellBlt (pdf_float_t w, pdf_float_t h, char const * blt, char const * txt, char const * border = 0, char align = 'J', bool fill = false);
  void ClippingRect (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, bool outline = false);
  void UnsetClipping ();
  void ClippedCell (pdf_float_t w, pdf_float_t h = 0, char const * txt = 0, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFit (pdf_float_t w, pdf_float_t h = 0, char const * txt = 0, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0, bool scale = false, bool force = true);
  void CellFitScale (pdf_float_t w, pdf_float_t h = 0, char const * txt = 0, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFitScale (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFitScaleForce (pdf_float_t w, pdf_float_t h = 0, char const * txt = 0, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFitScaleForce (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFitSpace (pdf_float_t w, pdf_float_t h = 0, char const * txt = 0, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFitSpace (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFitSpaceForce (pdf_float_t w, pdf_float_t h = 0, char const * txt = 0, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void CellFitSpaceForce (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border = 0, int ln = 0, char align = ' ', bool fill = false, int link = 0);
  void Write (pdf_float_t h, char const * txt, int link = 0);
  void Ln (pdf_float_t h = 0.0);
  void Image (char const * image_name, unsigned char const * image_blob, size_t blob_size, pdf_float_t x = -1, pdf_float_t y = -1, pdf_float_t w = 0.0, pdf_float_t h = 0.0, char const * atype = 0, int link = 0);
  void Image (char const * file, pdf_float_t x = -1, pdf_float_t y = -1, pdf_float_t w = 0.0, pdf_float_t h = 0.0, char const * atype = 0, int link = 0);
  pdf_float_t GetX ();
  void SetX (pdf_float_t x);
  pdf_float_t GetY ();
  void SetY (pdf_float_t y);
  void SetXY (pdf_float_t x, pdf_float_t y);
  pdf_float_t GetW ();
  pdf_float_t GetH ();
  std::string Output (char const * name = 0, char dest = ' ');
  void RoundedRect (pdf_float_t x, pdf_float_t y, pdf_float_t w, pdf_float_t h, pdf_float_t r, char const * style = "");
  void Circle (pdf_float_t x, pdf_float_t y, pdf_float_t r, char const * style = "D");
  void Ellipse (pdf_float_t x, pdf_float_t y, pdf_float_t rx, pdf_float_t ry, char const * style = "D");
  void IncludeJS (char const * script);
protected:
  void _Arc (pdf_float_t x1, pdf_float_t y1, pdf_float_t x2, pdf_float_t y2, pdf_float_t x3, pdf_float_t y3);
  std::string & _erasestrch (std::string & str, char c);
  void _str_tolower (std::string & str);
  void _dochecks ();
  void _checkoutput ();
  st_pagesize & _getpagesize (st_pagesize & result, e_page_sizes size);
  void _beginpage (e_orientation orientation = e_orientation_none, st_pagesize * size = 0);
  void _endpage ();
  std::string _escape (std::string const & s);
  void _textstring (std::string & result, std::string const & s);
  int substr_count (char const * str, char c);
  std::string _dounderline (pdf_float_t x, pdf_float_t y, char const * txt);
  struct blob_stream_t
  {
    virtual bool eof () = 0;
    virtual size_t tell () = 0;
    virtual size_t read (void * dest, size_t num_read) = 0;
    virtual void seek (size_t to_offset, int whence) = 0;
    virtual ~ blob_stream_t ();
  };
  struct blob_stream_memory_t : public blob_stream_t
  {
    size_t size;
    size_t offset;
    unsigned char const * blob;
    blob_stream_memory_t (unsigned char const * ablob, size_t asize);
    bool eof ();
    size_t tell ();
    size_t read (void * dest, size_t num_read);
    void seek (size_t to_offset, int whence);
  };
  struct blob_stream_file_t : public blob_stream_t
  {
    FILE * fp;
    blob_stream_file_t (FILE * afp);
    ~ blob_stream_file_t ();
    bool eof ();
    size_t tell ();
    size_t read (void * dest, size_t num_read);
    void seek (size_t to_offset, int whence);
  };
  void _parsejpg (st_image & info, blob_stream_t & fp, char const * image_name);
  void _parsejpg (st_image & info, char const * file_name);
  void _parsejpg_blob (st_image & info, char const * image_name, unsigned char const * image_blob, size_t blob_size);
  void _parsepng (st_image & info, char const * file_name);
  void _parsepng_blob (st_image & info, char const * image_name, unsigned char const * image_blob, size_t blob_size);
  void _getGrayImgColorAndalpha (std::string & color, std::string & alpha, std::string & line);
  void _getRGBImgColorAndalpha (std::string & color, std::string & alpha, std::string & line);
  void _parsepngstream (st_image & info, blob_stream_t & fp, char const * image_name);
  std::string & _readstream (std::string & result, blob_stream_t & fp, size_t n);
  int _readint (blob_stream_t & fp);
  int _readshort (blob_stream_t & fp);
  void _parsegif (std::string & file);
  void _newobj ();
  void _putstream (std::string const & s);
  void _out (char const * s, size_t size, bool nl = true);
  std::string & pdf_sprintf (std::string & s, char const * fmt, ...);
  void pdf_sprintf_append (std::string & s, char const * fmt, ...);
  void _outfmt (bool nl, char const * fmt, ...);
  void _out (char const * s, bool nl = true);
  void _out (std::string const & s, bool nl = true);
  void _putpages ();
  void _putfonts ();
  void _putimages ();
  void _putimage (st_image & info);
  void _putxobjectdict ();
  void _putextgstates ();
  void _putjavascript ();
  void _putresourcedict ();
  void _putresources ();
  void _putinfo ();
  void _putcatalog ();
  void _putheader ();
  void _puttrailer ();
  void _enddoc ();
};
LZZ_INLINE void FPDF::SetTopMargin (pdf_float_t margin)
    {
        // Set top margin
        m_tMargin = margin;
    }
LZZ_INLINE void FPDF::SetRightMargin (pdf_float_t margin)
    {
        // Set right margin
        m_rMargin = margin;
    }
LZZ_INLINE pdf_float_t FPDF::GetLeftMargin ()
    {
        return m_lMargin;
    }
LZZ_INLINE pdf_float_t FPDF::GetRightMargin ()
    {
        return m_rMargin;
    }
LZZ_INLINE void FPDF::setCustomZoom (int zoom)
    {
        m_CustomZoom = zoom;
    }
LZZ_INLINE int FPDF::getCustomZoom ()
    {
        return m_CustomZoom;
    }
LZZ_INLINE std::string const & FPDF::GetAliasNbPages ()
    {
        // Define an alias for total number of pages
        return m_AliasNbPages;
    }
LZZ_INLINE void FPDF::Open ()
    {
        // Begin document
        m_state = 1;
    }
LZZ_INLINE int FPDF::PageNo ()
    {
        // Get current page number
        return m_page;
    }
LZZ_INLINE void FPDF::SetDrawColor (pdf_color_t & color)
    {
        SetDrawColor(color.r, color.g, color.b);
    }
LZZ_INLINE void FPDF::GetDrawColor (pdf_color_t & color)
    {
        color = m_DrawColor_rgb;
    }
LZZ_INLINE void FPDF::SetFillColor (pdf_color_t & color)
    {
        SetFillColor(color.r, color.g, color.b);
    }
LZZ_INLINE void FPDF::GetFillColor (pdf_color_t & color)
    {
        color = m_FillColor_rgb;
    }
LZZ_INLINE void FPDF::SetTextColor (pdf_color_t & color)
    {
        SetTextColor(color.r, color.g, color.b);
    }
LZZ_INLINE void FPDF::GetTextColor (pdf_color_t & color)
    {
        color = m_TextColor_rgb;
    }
LZZ_INLINE pdf_float_t FPDF::GetStringWidth (std::string const & s)
    {
        return GetStringWidth(s.c_str());
    }
LZZ_INLINE pdf_float_t FPDF::GetFontSize ()
    {
        return m_FontSizePt;
    }
LZZ_INLINE void FPDF::RotatedText (pdf_float_t x, pdf_float_t y, std::string const & txt, pdf_float_t angle)
    {
        RotatedText(x, y, txt.c_str(), angle);
    }
LZZ_INLINE void FPDF::Cell (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border, int ln, char align, bool fill, int link)
    {
        Cell(w, h, txt.c_str(), border, ln, align, fill, link);
    }
LZZ_INLINE void FPDF::MultiCell (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border, char align, bool fill)
    {
        MultiCell(w, h, txt.c_str(), border, align, fill);
    }
LZZ_INLINE void FPDF::CellFitScale (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, int ln, char align, bool fill, int link)
    {
        CellFit(w,h,txt,border,ln,align,fill,link,true,false);
    }
LZZ_INLINE void FPDF::CellFitScale (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border, int ln, char align, bool fill, int link)
    {
        CellFit(w,h,txt.c_str(),border,ln,align,fill,link,true,false);
    }
LZZ_INLINE void FPDF::CellFitScaleForce (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, int ln, char align, bool fill, int link)
    {
        CellFit(w,h,txt,border,ln,align,fill,link,true,true);
    }
LZZ_INLINE void FPDF::CellFitScaleForce (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border, int ln, char align, bool fill, int link)
    {
        CellFit(w,h,txt.c_str(),border,ln,align,fill,link,true,true);
    }
LZZ_INLINE void FPDF::CellFitSpace (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, int ln, char align, bool fill, int link)
    {
        CellFit(w,h,txt,border,ln,align,fill,link,false,false);
    }
LZZ_INLINE void FPDF::CellFitSpace (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border, int ln, char align, bool fill, int link)
    {
        CellFit(w,h,txt.c_str(),border,ln,align,fill,link,false,false);
    }
LZZ_INLINE void FPDF::CellFitSpaceForce (pdf_float_t w, pdf_float_t h, char const * txt, char const * border, int ln, char align, bool fill, int link)
    {
        //Same as calling CellFit directly
        CellFit(w,h,txt,border,ln,align,fill,link,false,true);
    }
LZZ_INLINE void FPDF::CellFitSpaceForce (pdf_float_t w, pdf_float_t h, std::string const & txt, char const * border, int ln, char align, bool fill, int link)
    {
        //Same as calling CellFit directly
        CellFit(w,h,txt.c_str(),border,ln,align,fill,link,false,true);
    }
LZZ_INLINE pdf_float_t FPDF::GetX ()
    {
        // Get x position
        return m_x;
    }
LZZ_INLINE pdf_float_t FPDF::GetY ()
    {
        // Get y position
        return m_y;
    }
LZZ_INLINE void FPDF::SetXY (pdf_float_t x, pdf_float_t y)
    {
        // Set x and y positions
        SetY(y);
        SetX(x);
    }
LZZ_INLINE pdf_float_t FPDF::GetW ()
    {
        // Get y position
        return m_w;
    }
LZZ_INLINE pdf_float_t FPDF::GetH ()
    {
        // Get y position
        return m_h;
    }
LZZ_INLINE void FPDF::_endpage ()
    {
        if(m_angle!=0)
        {
            m_angle=0;
            _out("Q");
        }
        m_state = 1;
    }
#undef LZZ_INLINE
#endif
