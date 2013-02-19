// pdf-font.h
//

#ifndef LZZ_pdf_font_h
#define LZZ_pdf_font_h
#include <string>
#include <set>
#define LZZ_INLINE inline
enum e_font_type
{
  e_font_type_core,
  e_font_type_type1,
  e_font_type_ttf
};
typedef int font_width_t;
struct st_pdf_font_core
{
  e_font_type type;
  int up;
  int ut;
  font_width_t (cw) [256];
};
struct st_pdf_font_base
{
  int i;
  int n;
  char const * name;
  st_pdf_font_core & font;
  st_pdf_font_base (st_pdf_font_core & f);
};
struct pdf_font_Courier : public st_pdf_font_base
{
  pdf_font_Courier ();
};
struct pdf_font_CourierOblique : public st_pdf_font_base
{
  pdf_font_CourierOblique ();
};
struct pdf_font_CourierBold : public st_pdf_font_base
{
  pdf_font_CourierBold ();
};
struct pdf_font_CourierBoldOblique : public st_pdf_font_base
{
  pdf_font_CourierBoldOblique ();
};
struct pdf_font_Helvetica : public st_pdf_font_base
{
  pdf_font_Helvetica ();
};
struct pdf_font_HelveticaOblique : public st_pdf_font_base
{
  pdf_font_HelveticaOblique ();
};
struct pdf_font_HelveticaBold : public st_pdf_font_base
{
  pdf_font_HelveticaBold ();
};
struct pdf_font_HelveticaBoldOblique : public st_pdf_font_base
{
  pdf_font_HelveticaBoldOblique ();
};
struct pdf_font_Times : public st_pdf_font_base
{
  pdf_font_Times ();
};
struct pdf_font_TimesOblique : public st_pdf_font_base
{
  pdf_font_TimesOblique ();
};
struct pdf_font_TimesBold : public st_pdf_font_base
{
  pdf_font_TimesBold ();
};
struct pdf_font_TimesBoldOblique : public st_pdf_font_base
{
  pdf_font_TimesBoldOblique ();
};
struct pdf_font_Symbol : public st_pdf_font_base
{
  pdf_font_Symbol ();
};
struct pdf_font_ZapfDingbats : public st_pdf_font_base
{
  pdf_font_ZapfDingbats ();
};
bool isPdfFontCore (char const * fname);
extern st_pdf_font_core pdf_font_core_Courier;
extern st_pdf_font_core pdf_font_core_Helvetica;
extern st_pdf_font_core pdf_font_core_HelveticaBold;
extern st_pdf_font_core pdf_font_core_Times;
extern st_pdf_font_core pdf_font_core_TimesOblique;
extern st_pdf_font_core pdf_font_core_TimesBold;
extern st_pdf_font_core pdf_font_core_TimesBoldOblique;
extern st_pdf_font_core pdf_font_core_Symbol;
extern st_pdf_font_core pdf_font_core_ZapfDingbats;
#undef LZZ_INLINE
#endif
