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

   /* settings specified by user and stored locally here */
   const char *wname;
   uint32_t    x, y;             /* (x,y) top-left pixel for oxbar      */
   uint32_t    w, h;             /* (w,h) pixel dimensions of oxbar     */
   const char *font;             /* font specified by user (natively)   */

   /* dervied stats useful for saving + core xcb/cairo/pango components */
   uint32_t              display_width;   /* determiend from xcb        */
   uint32_t              display_height;  /* " " "                      */
   uint8_t               font_size; /* determined from font/pango       */
   xcb_connection_t     *xcon;      /* connection to x server           */
   xcb_screen_t         *xscreen;   /* screen we render to              */
   xcb_drawable_t       xwindow;    /* oxbar xwindow                    */
   xcb_visualtype_t     *xvisual;   /* oxbar window's visual            */
   cairo_t              *cairo;     /* core ciaro object for rendering  */
   cairo_surface_t      *surface;   /* core ciaro surface mapped to X   */
   PangoFontDescription *pfont;     /* pango font structure             */
} xinfo_t;


xinfo_t *xcore_init(
   const char *name,          /* name of window (in x/WM world          */
   double x, double y,        /* (x,y) of top-left window pixel         */
   double w, double h,        /* width x height in window pixels        */
   double padding,            /* padding between top & left sides       */
   const char *font);         /* PangoFontDescription spec              */

void xcore_free(xinfo_t *x);
void xcore_flush(xinfo_t *x);

#endif
