#ifndef UI_H
#define UI_H

#include "xcore.h"
#include "histogram.h"

typedef struct oxbarui {
   /* not packed */
   xinfo_t  *xinfo;

   char     *bgcolor;
   char     *fgcolor;
} oxbarui_t;

oxbarui_t*
ui_create(
      const char *wname,   /* name for window in window manager            */
      int         x,       /* (x,y) top-left pixel coordinates for oxbar   */
      int         y,
      int         width,   /* (width,height) of oxbar in pixels            */
      int         height,
      int         padding, /* internal padding between text and border     */
      double      fontpt,  /* font-size in classic point scale             */
      const char *font,    /* font specified by user (natively)            */

      const char *bgcolor,
      const char *fgcolor
      );

void ui_destroy(oxbarui_t *ui);
void ui_flush(oxbarui_t *ui);
void ui_clear(oxbarui_t *ui);

uint32_t
ui_draw_text(
      oxbarui_t *ui,
      const char *color,
      double x, double y,
      const char *text);

void
ui_draw_top_header(
      oxbarui_t *ui,
      const char *color,
      double x1, double x2);

uint32_t
ui_draw_vertical_stack(
      oxbarui_t   *ui,
      uint32_t     x,
      double       width,
      size_t       nvalues,
      const char **colors,
      double      *percents);

uint32_t
ui_draw_histogram(
      oxbarui_t *ui,
      double x,
      const char **colors,
      histogram_t *h);

void
hex2rgba(const char *s, double *r, double *g, double *b, double *a);

#endif
