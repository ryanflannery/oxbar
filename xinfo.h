#ifndef XINFO_H
#define XINFO_H

#include <sys/types.h>
#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

typedef struct xinfo {
   uint32_t       display_width, display_height;

   uint32_t       bar_width,  bar_height;
   uint32_t       bar_x,      bar_y;
   uint32_t       bar_padding;

   double         font_size;

   xcb_connection_t *xcon;
   xcb_screen_t     *xscreen;
   xcb_drawable_t    xwindow;
   xcb_visualtype_t *xvisual;

   cairo_t          *cairo;
   cairo_surface_t  *csurface;
} xinfo_t;

void setup_x_connection_screen_visual(xinfo_t *x);
void setup_x_window(xinfo_t *xinfo, const char *name,
      uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void setup_x_wm_hints(xinfo_t *x);
void setup_cairo(xinfo_t *x);
void setup_xfont(xinfo_t *x, const char *font_description, double font_size);

int  window_draw_text(xinfo_t *xinfo, const char *color, double x, double y, const char *text);
void window_draw_top_header(xinfo_t *xinfo, const char *color, double x1, double x2);

int  window_draw_vertical_stack_bar(
      xinfo_t *xinfo,
      double x,
      double pct);

void destroy_x(xinfo_t *x);
void clear_background(xinfo_t *x);

void hex2rgba(const char *s, double *r, double *g, double *b, double *a);

#endif
