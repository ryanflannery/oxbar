#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

#include "stats/stats.h"

typedef struct xinfo {
   uint32_t       display_width, display_height;

   uint32_t       bar_width,  bar_height;
   uint32_t       bar_x,      bar_y;

   double         font_size;

   xcb_connection_t *xcon;
   xcb_screen_t     *xscreen;
   xcb_drawable_t    xwindow;
   xcb_visualtype_t *xvisual;

   cairo_t          *cairo;
   cairo_surface_t  *csurface;
} xinfo_t;


xcb_screen_t*
get_xscreen(xcb_connection_t *c, int screen)
{
   xcb_screen_iterator_t i = xcb_setup_roots_iterator(xcb_get_setup(c));;
   for (; i.rem; --screen, xcb_screen_next(&i)) {
      if (0 == screen)
         return i.data;
   }
   return NULL;
}

xcb_visualtype_t*
get_xvisual(xcb_screen_t *screen)
{
   xcb_depth_iterator_t i = xcb_screen_allowed_depths_iterator(screen);
   for (; i.rem; xcb_depth_next(&i)) {
      xcb_visualtype_iterator_t vi;
      vi = xcb_depth_visuals_iterator(i.data);
      for (; vi.rem; xcb_visualtype_next(&vi)) {
         if (screen->root_visual == vi.data->visual_id) {
            return vi.data;
         }
      }
   }

   return NULL;
}

void
setup_x_connection_screen_visual(xinfo_t *x)
{
   int   default_screen;

   x->xcon = xcb_connect(NULL, &default_screen);
   if (xcb_connection_has_error(x->xcon)) {
      xcb_disconnect(x->xcon);
      errx(1, "Failed to establish connection to X");
   }

   if (NULL == (x->xscreen = get_xscreen(x->xcon, default_screen)))
      errx(1, "Failed to retrieve X screen");

   x->display_width  = x->xscreen->width_in_pixels;
   x->display_height = x->xscreen->height_in_pixels;

   if (NULL == (x->xvisual = get_xvisual(x->xscreen)))
      errx(1, "Failed to retrieve X visual context");
}

void
setup_x_window(xinfo_t *xinfo, const char *name,
      uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
   xinfo->bar_x = x;
   xinfo->bar_y = y;
   xinfo->bar_width = w;
   xinfo->bar_height = h;

   xinfo->xwindow = xcb_generate_id(xinfo->xcon);
   xcb_create_window(
         xinfo->xcon,
         XCB_COPY_FROM_PARENT,
         xinfo->xwindow,
         xinfo->xscreen->root,
         xinfo->bar_x,
         xinfo->bar_y,
         xinfo->bar_width,
         xinfo->bar_height,
         0, /* border width */
         XCB_WINDOW_CLASS_INPUT_OUTPUT,
         xinfo->xscreen->root_visual,
         XCB_CW_BACK_PIXEL,
         &xinfo->xscreen->black_pixel);

   xcb_icccm_set_wm_name(
         xinfo->xcon,
         xinfo->xwindow,
         XCB_ATOM_STRING, 8,
         strlen(name),
         name);
}

/*
 * Note: here be dragons. This is a hodge-podge of miscellaneous excrement I've
 * pieced together over the years (spanning multiple projects) which attempts
 * to achieve the following goals:
 *    1. remove a window from any 'tab order' of a window manager (so
 *       "alt+TAB" doesn't include it)
 *    2. remove a window from any window manager attempts to move, resize, or
 *       otherwise alter the placement of a window
 *    3. keep a window on-top-of other windows and prevent it from being
 *       minimized/hidden/etc during common window manager actions (like
 *       switching "spaces"/"workplaces"/etc).
 *
 * I'm sure this could be done better, and I welcome the brave knight who
 * succeeds in doing so. Their righteous pull request would earn an instant
 * pitcher of "Beer of Thou's Chosing" from me.
 *                                                                   -ryan
 */
void
setup_x_wm_hints(xinfo_t *x)
{
   enum {
      NET_WM_XINFO_TYPE,
      NET_WM_XINFO_TYPE_DOCK,
      NET_WM_DESKTOP,
      NET_WM_STRUT_PARTIAL,
      NET_WM_STRUT,
      NET_WM_STATE,
      NET_WM_STATE_STICKY,
      NET_WM_STATE_ABOVE
   };

   const char *atoms[] = {
      "_NET_WM_XINFO_TYPE",
      "_NET_WM_XINFO_TYPE_DOCK",
      "_NET_WM_DESKTOP",
      "_NET_WM_STRUT_PARTIAL",
      "_NET_WM_STRUT",
      "_NET_WM_STATE",
      "_NET_WM_STATE_STICKY",
      "_NET_WM_STATE_ABOVE"
   };
   const size_t natoms = sizeof(atoms)/sizeof(char*);

   xcb_intern_atom_cookie_t xcookies[natoms];
   xcb_atom_t               xatoms[natoms];
   xcb_intern_atom_reply_t *xatom_reply;
   size_t i;

   for (i = 0; i < natoms; i++)
      xcookies[i] = xcb_intern_atom(x->xcon, 0, strlen(atoms[i]), atoms[i]);

   for (i = 0; i < natoms; i++) {
      xatom_reply = xcb_intern_atom_reply(x->xcon, xcookies[i], NULL);
      if (!xatom_reply)
         errx(1, "%s: xcb atom reply failed for %s", __FUNCTION__, atoms[i]);

      xatoms[i] = xatom_reply->atom;
      free(xatom_reply);
   }

   enum {
      left,           right,
      top,            bottom,
      left_start_y,   left_end_y,
      right_start_y,  right_end_y,
      top_start_x,    top_end_x,
      bottom_start_x, bottom_end_x
   };
   unsigned long struts[12] = { 0 };

   struts[top] = x->bar_y + x->bar_height;
   struts[top_start_x] = x->bar_x;
   struts[top_end_x] = x->bar_x + x->bar_width;
   /* TODO - see xstatbar.c - need more work here */

	xcb_change_property(x->xcon, XCB_PROP_MODE_REPLACE, x->xwindow,
         xatoms[NET_WM_XINFO_TYPE], XCB_ATOM_ATOM, 32, 1,
         &xatoms[NET_WM_XINFO_TYPE_DOCK]);
	xcb_change_property(x->xcon, XCB_PROP_MODE_APPEND,  x->xwindow,
         xatoms[NET_WM_STATE], XCB_ATOM_ATOM, 32, 2,
         &xatoms[NET_WM_STATE_STICKY]);
	xcb_change_property(x->xcon, XCB_PROP_MODE_REPLACE, x->xwindow,
         xatoms[NET_WM_DESKTOP], XCB_ATOM_CARDINAL, 32, 1,
         (const uint32_t []){ -1 } );
	xcb_change_property(x->xcon, XCB_PROP_MODE_REPLACE, x->xwindow,
         xatoms[NET_WM_STRUT_PARTIAL], XCB_ATOM_CARDINAL, 32, 12, struts);
	xcb_change_property(x->xcon, XCB_PROP_MODE_REPLACE, x->xwindow,
         xatoms[NET_WM_STRUT], XCB_ATOM_CARDINAL, 32, 4, struts);

   /* remove window from window manager tabbing */
   const uint32_t val[] = { 1 };
   xcb_change_window_attributes(x->xcon, x->xwindow,
         XCB_CW_OVERRIDE_REDIRECT, val);
}

void
setup_cairo(xinfo_t *x)
{
   x->csurface = cairo_xcb_surface_create(
         x->xcon,
         x->xwindow,
         x->xvisual,
         x->bar_width,
         x->bar_height
         );

   x->cairo = cairo_create(x->csurface);
}

void
setup_xfont(xinfo_t *x, const char *font_description, double font_size)
{
   x->font_size = font_size;

   cairo_select_font_face(
         x->cairo,
         font_description,
         CAIRO_FONT_SLANT_NORMAL,
         CAIRO_FONT_WEIGHT_NORMAL
         );

   cairo_set_font_size(x->cairo, x->font_size);
}

void
hex2rgba(const char *s, double *r, double *g, double *b, double *a)
{
   unsigned int ir, ig, ib, ia;

   if ('#' == s[0])
      s++;

   switch (strlen(s)) {
      case 6:
         if (3 != sscanf(s, "%02x%02x%02x", &ir, &ig, &ib))
            errx(1, "%s: malformed rgb color '%s'", __FUNCTION__, s);

         ia = 255;
         break;
      case 8:
         if (4 != sscanf(s, "%02x%02x%02x%02x", &ir, &ig, &ib, &ia))
            errx(1, "%s: malformed rgba color '%s'", __FUNCTION__, s);

         break;
      default:
         errx(1, "%s: malformed color '%s'", __FUNCTION__, s);
   }

   *r = (double)ir / 255.0;
   *g = (double)ig / 255.0;
   *b = (double)ib / 255.0;
   *a = (double)ia / 255.0;
}

int
window_draw_text(xinfo_t *xinfo, const char *color, double x, double y, const char *text)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);

   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_move_to(xinfo->cairo, x, y);
   cairo_show_text(xinfo->cairo, text);

   cairo_text_extents_t extents;
   cairo_text_extents(xinfo->cairo, text, &extents);

   return extents.x_advance;
}

void
window_draw_top_header(xinfo_t *xinfo, const char *color, double x1, double x2)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);

   cairo_set_source_rgba(xinfo->cairo, r, g, b, 0.7);
   cairo_set_line_width(xinfo->cairo, 8);
   cairo_move_to(xinfo->cairo, x1, 0);
   cairo_line_to(xinfo->cairo, x2, 0);
   cairo_stroke(xinfo->cairo);
}

void
destroy_x(xinfo_t *x)
{
   cairo_destroy(x->cairo);
   cairo_surface_destroy(x->csurface);
   xcb_disconnect(x->xcon);
}

void
clear_background(xinfo_t *x)
{
   cairo_paint(x->cairo);
}

int
main ()
{
   xinfo_t xinfo;

   /* inputs */
   const char *bgcolor  = "1c1c1c"; /*"#212429";*/
   const char *fgcolor  = "93a1a1"; /*"839496"; "#cc5500";*/
   const char *font     = "DejaVu Sans";
   double font_size     = 16.0;
   double bar_padding   = 4.0;
   int bar_x            = 100;
   int bar_y            = 100;   /* -1 means "bottom"          */
   int bar_width        = 1500;  /* -1 means "display width"   */
   int bar_height;               /* determined by font size (is that right?) */


   /* these need to be done in order */
   setup_x_connection_screen_visual(&xinfo);

   bar_height = (uint32_t)(ceil(font_size + (2 * bar_padding)));
   if (-1 == bar_y)     bar_y = xinfo.display_height - bar_height;
   if (-1 == bar_width) bar_width = xinfo.display_width;

   setup_x_window(&xinfo, getprogname(), bar_x, bar_y, bar_width, bar_height);
   setup_x_wm_hints(&xinfo);
   setup_cairo(&xinfo);
   setup_xfont(&xinfo, font, font_size);

   /* now map the window & do an initial paint */
   xcb_map_window(xinfo.xcon, xinfo.xwindow);
   cairo_surface_flush(xinfo.csurface);
   xcb_flush(xinfo.xcon);

   stats_init();

   double x = bar_padding;
   double y = font_size + bar_padding;

   double p = 10.0;
   double s = 5.0;

   double sbarx;

   while (1) {
      x = bar_padding;
      stats_update();

      double r, g, b, a;
      hex2rgba(bgcolor, &r, &g, &b, &a);
      cairo_set_source_rgba(xinfo.cairo, r, g, b, a);
      clear_background(&xinfo);
      hex2rgba(fgcolor, &r, &g, &b, &a);
      cairo_set_source_rgba(xinfo.cairo, r, g, b, a);

      if (BATTERY.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo,
               BATTERY.plugged_in ? fgcolor : "dc322f",
               x, y,
               BATTERY.plugged_in ? "AC:" : "BAT:");
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, BATTERY.str_charge_pct);
         if (-1 != BATTERY.minutes_remaining) {
            x += s;
            x += window_draw_text(&xinfo, fgcolor, x, y, BATTERY.str_time_remaining);
         }

         /*
         double scale = 0.65;
         cairo_surface_t *png = cairo_image_surface_create_from_png("battery_charging.png");
         cairo_scale(xinfo.cairo, scale, scale);
         cairo_set_source_surface(xinfo.cairo, png, x, 0);
         x += cairo_image_surface_get_width(png);
         cairo_scale(xinfo.cairo, 1.0/scale, 1.0/scale);
         cairo_paint(xinfo.cairo);
         */

         window_draw_top_header(&xinfo, "b58900", sbarx, x);
         x += p;
      }

      if (VOLUME.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "VOLUME:");
         x += s;
         if (VOLUME.left_pct == VOLUME.right_pct) {
            x += window_draw_text(&xinfo, fgcolor, x, y, VOLUME.str_left_pct);
         } else {
            x += window_draw_text(&xinfo, fgcolor, x, y, VOLUME.str_left_pct);
            x += s;
            x += window_draw_text(&xinfo, fgcolor, x, y, VOLUME.str_right_pct);
         }
         window_draw_top_header(&xinfo, "cb4b16", sbarx, x);
         x += p;
      }

      if (NPROCS.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "#PROCS:");
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, NPROCS.str_nprocs);
         window_draw_top_header(&xinfo, "dc322f", sbarx, x);
         x += p;
      }

      if (MEMORY.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "MEMORY:");
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, MEMORY.str_active);
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, "active");
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, MEMORY.str_total);
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, "total");
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, MEMORY.str_free);
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, "free");
         window_draw_top_header(&xinfo, "d33682", sbarx, x);
         x += p;
      }

      if (CPUS.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "CPUS:");
         x += s;
         for (int i = 0; i < CPUS.ncpu; i++) {
            x += window_draw_text(&xinfo, fgcolor, x, y,
                  CPUS.cpus[i].str_percentages[CP_IDLE]);
            x += s;
         }
         window_draw_top_header(&xinfo, "6c71c4", sbarx, x);
         x += p;
      }

      cairo_surface_flush(xinfo.csurface);
      xcb_flush(xinfo.xcon);

      sleep(1);
   }

   stats_close();
   destroy_x(&xinfo);

   return 0;
}
