#ifndef UI_H
#define UI_H

#include "xcore.h"

typedef struct oxbarui {
   /* not packed */
   xinfo_t    *xinfo;
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
      const char *font     /* font specified by user (natively)            */
      );

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
ui_draw_vertical_stack_bar(
      oxbarui_t *ui,
      double x,
      double pct);

void
hex2rgba(const char *s, double *r, double *g, double *b, double *a);

#endif
