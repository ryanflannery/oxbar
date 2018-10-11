#ifndef XINFO_H
#define XINFO_H

#include <sys/types.h>

#include <xcb/xcb.h>
#include <cairo/cairo.h>
#include <pango/pango-font.h>

/* pango font wraper */
typedef struct xfont {
   char                 *ufont;  /* user-specified font string unchanged */
   PangoFontDescription *pfont;  /* pango font loaded from that string   */
   int                   height; /* derived font height (in pixels)      */
} xfont_t;

xfont_t *xfont_init(const char *const font_description);
void xfont_free(xfont_t *xf);

/* x display info - this is readonly info about our x display */
typedef struct xinfo {
   uint32_t              display_width;
   uint32_t              display_height;
   xcb_connection_t     *con;
   xcb_screen_t         *root_screen;
   xcb_visualtype_t     *root_visual;
} xinfo_t;

void xinfo_open(xinfo_t *x);
void xinfo_close(xinfo_t *x);

/* xcb window & cairo wrapper */
typedef struct xwin {
   /* settings specified by user and stored locally here */
   const char *wname;            /* name of window for window manager   */
   uint32_t    x, y;             /* (x,y) top-left pixel for oxbar      */
   uint32_t    w, h;             /* (w,h) pixel dimensions of oxbar     */

   /* dervied stats useful for saving + core xcb/cairo/pango components */
   xcb_drawable_t    window;     /* oxbar xwindow                    */
   cairo_surface_t  *surface;    /* core ciaro surface mapped to X   */
   cairo_t          *cairo;      /* core ciaro object for rendering  */
} xwin_t;

xwin_t *xwin_init(
   const xinfo_t *xinfo,
   const char *name,          /* name of window (in x/WM world          */
   double x, double y,        /* (x,y) of top-left window pixel         */
   double w, double h);       /* width x height in window pixels        */
void xwin_free(xwin_t *x);

#endif
