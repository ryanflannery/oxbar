#ifndef XINFO_H
#define XINFO_H

#include <sys/types.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#include <cairo/cairo-xcb.h>
#include <pango/pangocairo.h>

/* not packed */
typedef struct xinfo {

   /* "human readable" display / core x information */
   const char *wname;
   uint32_t    display_width, display_height;  /* display dimensions */
   uint32_t    x, y;             /* (x,y) top-left pixel for oxbar   */
   uint32_t    w, h;             /* (w,h) pixel dimensions of oxbar  */
   uint32_t    padding;          /* internal padding of oxbar        */

   /*double      fontpt;            font-size in classic point scale */
   const char *font;             /* font specified by user (natively)*/

   /* core xcb/cairo/pango components */
   xcb_connection_t *xcon;       /* connection to x server           */
   xcb_screen_t     *xscreen;    /* screen we render to              */
   xcb_drawable_t    xwindow;    /* oxbar xwindow                    */
   xcb_visualtype_t *xvisual;    /* oxbar window's visual            */
   cairo_t          *cairo;      /* core ciaro object for rendering  */
   cairo_surface_t  *surface;    /* core ciaro surface mapped to X   */

   PangoLayout          *playout;
   PangoFontDescription *pfont;
} xinfo_t;

/*
 * Creates core XCB/X11 connection
 */
void xcore_setup_x_connection_screen_visual(xinfo_t *x);

/*
 * Creates initial X11 window
 */
void xcore_setup_x_window(
      xinfo_t *xinfo,
      const char *name,          /* name of window (in x/WM world    */
      uint32_t x, uint32_t y,    /* (x,y) of top-left window pixel   */
      uint32_t w, uint32_t h);   /* width x height in window pixels  */

/*
 * Sets up appropriate window manager hints for *most* WMs
 * ...dragons lurk here
 */
void xcore_setup_x_wm_hints(xinfo_t *x);

/*
 * Sets up cairo objects used for rendering
 */
void xcore_setup_cairo(xinfo_t *x);

/*
 * Sets up pango/font configuration (supports freetype)
 */
void xcore_setup_xfont(
   xinfo_t    *x,
   const char *font_description);   /* PangoFontDescription spec */

/*
 * Disconnects & destroys all pango/cairo/x11/xcb related objects
 */
void xcore_destroy(xinfo_t *x);

#endif
