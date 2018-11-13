/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xcb/xcb_icccm.h>
#include <cairo/cairo-xcb.h>
#include <pango/pangocairo.h>

#include "xcore.h"

/* private functions */
static xcb_screen_t     *get_root_screen(xcb_connection_t*, int);
static xcb_visualtype_t *get_root_visual(xcb_screen_t*);
static void              create_xcb_window(struct xwin*, const struct xdisp*,
                                           const char*, int16_t, int16_t,
                                           uint16_t, uint16_t);
static void              setup_wm_hints(const struct xdisp*, struct xwin*);
static void              setup_cairo(const struct xdisp*, struct xwin*);

/* pango wrapper */

struct xfont*
xfont_init(struct xfont_settings *settings)
{
   struct xfont *xf = malloc(sizeof(struct xfont));
   if (NULL == xf)
      err(1, "%s: malloc failed", __FUNCTION__);

   const char *font_desc = settings->desc;
   xf->pfont = pango_font_description_from_string(font_desc);
   if (!xf->pfont)
      errx(1, "pango failed to parse or load font '%s'", font_desc);

   if (pango_font_description_get_size_is_absolute(xf->pfont))
      xf->height = pango_font_description_get_size(xf->pfont) / PANGO_SCALE;
   else {
      /* XXX apparently in this case you just try & measure the size :( */
      if (NULL == pango_font_description_get_family(xf->pfont))
         errx(1, "pango failed to determine font family from '%s'", font_desc);

      struct xwin_settings s = {
         .bgcolor = "ff000",
         .wname = "oxbar-xfont-init",
         .x = 0,     .y = 0,
         .w = 1000,  .h = 1000
      };
      struct xdisp *x = xdisp_init();
      struct xwin  *w = xwin_init(x, &s);
      int width, height;
      PangoLayout *layout = pango_cairo_create_layout(w->cairo);
      pango_layout_set_font_description(layout, xf->pfont);
      pango_layout_set_text(layout, "SOME BIG TEXT |$!", -1);
      pango_layout_get_pixel_size(layout, &width, &height);
      g_object_unref(layout);

      xf->height = height;
      xwin_free(w);
      xdisp_free(x);
   }

   if (0 == xf->height)
      errx(1, "pango failed to determine font height from '%s'", font_desc);

   xf->settings = settings;
   return xf;
}

void
xfont_free(struct xfont *xf)
{
   pango_font_description_free(xf->pfont);
   free(xf);
}

/* xcb display wrapper */

static xcb_screen_t*
get_root_screen(xcb_connection_t *c, int screen)
{
   xcb_screen_iterator_t i = xcb_setup_roots_iterator(xcb_get_setup(c));;
   for (; i.rem; --screen, xcb_screen_next(&i)) {
      if (0 == screen)
         return i.data;
   }

   return NULL;
}

static xcb_visualtype_t*
get_root_visual(xcb_screen_t *screen)
{
   xcb_depth_iterator_t i = xcb_screen_allowed_depths_iterator(screen);
   for (; i.rem; xcb_depth_next(&i)) {
      if (i.data->depth != 32)
         continue;

      xcb_visualtype_iterator_t vi;
      vi = xcb_depth_visuals_iterator(i.data);
      for (; vi.rem; xcb_visualtype_next(&vi)) {
         return vi.data;
      }
   }

   return NULL;
}

struct xdisp*
xdisp_init()
{
   static struct xdisp *x = NULL;

   if (NULL == (x = malloc(sizeof(struct xdisp))))
      err(1, "%s: malloc failed", __FUNCTION__);

   int default_screen;
   x->con = xcb_connect(NULL, &default_screen);
   if (xcb_connection_has_error(x->con)) {
      xcb_disconnect(x->con);
      errx(1, "Failed to establish connection to X");
   }

   if (NULL == (x->root_screen = get_root_screen(x->con, default_screen)))
      errx(1, "Failed to retrieve X root screen");

   x->display_width  = x->root_screen->width_in_pixels;
   x->display_height = x->root_screen->height_in_pixels;

   if (NULL == (x->root_visual = get_root_visual(x->root_screen)))
      errx(1, "Failed to retrieve X root visual context");

   return x;
}

void
xdisp_free(__attribute__((unused)) struct xdisp *x)
{
   xcb_disconnect(x->con);
   free(x);
}

/* xcb window wrapper */

static void
create_xcb_window(
      struct xwin        *xwin,
      const struct xdisp *xdisp,
      const char         *name,
      int16_t x,  int16_t y,
      uint16_t w, uint16_t h)
{
   xcb_colormap_t colormap = xcb_generate_id(xdisp->con);
   xcb_create_colormap(xdisp->con, XCB_COLORMAP_ALLOC_NONE, colormap,
         xdisp->root_screen->root, xdisp->root_visual->visual_id);

   uint32_t valwin[5] = {
      XCB_NONE,
      0,
      1,
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS,
      colormap
   };

   xwin->window = xcb_generate_id(xdisp->con);
   xcb_create_window(
         xdisp->con,
         32, /* XCB_COPY_FROM_PARENT? no - force 32 bit */
         xwin->window,
         xdisp->root_screen->root,
         x, y, w, h,
         0, /* border width */
         XCB_WINDOW_CLASS_INPUT_OUTPUT,
         xdisp->root_visual->visual_id,
         XCB_CW_BACK_PIXMAP | XCB_CW_BORDER_PIXEL | XCB_CW_OVERRIDE_REDIRECT
         | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP,
         valwin);

   xcb_icccm_set_wm_name(
         xdisp->con,
         xwin->window,
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
 * When starting oxbar, I searched for other applications who were doing such
 * things to see how "the modern folk" were doing this. The code I found from
 * yabar and a couple others looked like xstatbar's (my previous app), but
 * introduced an enum to replaced magic numbers. I did that below...it's an
 * improvement to the chaos. I still don't get it though.
 *
 * I'm sure this could be done better, and I welcome the brave knight who
 * succeeds in doing so. Their righteous pull request would earn an instant
 * pitcher of "Beer of Thou's Chosing" from me.
 *
 * Ideally each end-goal-feature would be independently set-able, such as
 * "immovable", "un-tab-able", "always-in-front", etc.
 *                                                                   -ryan
 * TODO cleanup & separate wm hints ... and dragons
 */
static void
setup_wm_hints(const struct xdisp *x, struct xwin *w)
{
   enum {
      NET_WM_XINFO_TYPE,
      NET_WM_XINFO_TYPE_DOCK,
      NET_WM_WINDOW_TYPE_DESKTOP,
      NET_WM_DESKTOP,
      NET_WM_STRUT_PARTIAL,
      NET_WM_STRUT,
      NET_WM_STATE,
      NET_WM_STATE_STICKY,
      NET_WM_STATE_ABOVE
   };

   static const char *atoms[] = {
      "_NET_WM_XINFO_TYPE",
      "_NET_WM_XINFO_TYPE_DOCK",
      "_NET_WM_WINDOW_TYPE_DESKTOP",
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
      xcookies[i] = xcb_intern_atom(x->con, 0, strlen(atoms[i]), atoms[i]);

   for (i = 0; i < natoms; i++) {
      xatom_reply = xcb_intern_atom_reply(x->con, xcookies[i], NULL);
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

   struts[top] = w->settings->y + w->settings->h;
   struts[top_start_x] = w->settings->x;
   struts[top_end_x] = w->settings->x + w->settings->w;

   xcb_change_property(x->con, XCB_PROP_MODE_REPLACE, w->window,
         xatoms[NET_WM_XINFO_TYPE], XCB_ATOM_ATOM, 32, 1,
         &xatoms[NET_WM_XINFO_TYPE_DOCK]);
   xcb_change_property(x->con, XCB_PROP_MODE_APPEND, w->window,
         xatoms[NET_WM_STATE], XCB_ATOM_ATOM, 32, 2,
         &xatoms[NET_WM_STATE_STICKY]);
   xcb_change_property(x->con, XCB_PROP_MODE_REPLACE, w->window,
         xatoms[NET_WM_DESKTOP], XCB_ATOM_CARDINAL, 32, 1,
         (const uint32_t []){ -1 } );
   xcb_change_property(x->con, XCB_PROP_MODE_REPLACE, w->window,
         xatoms[NET_WM_STRUT_PARTIAL], XCB_ATOM_CARDINAL, 32, 12, struts);
   xcb_change_property(x->con, XCB_PROP_MODE_REPLACE, w->window,
         xatoms[NET_WM_STRUT], XCB_ATOM_CARDINAL, 32, 4, struts);
}

static void
setup_cairo(const struct xdisp *x, struct xwin *w)
{
   w->surface = cairo_xcb_surface_create(
         x->con,
         w->window,
         x->root_visual,
         w->settings->w,
         w->settings->h);
   if (CAIRO_STATUS_SUCCESS != cairo_surface_status(w->surface))
      errx(1, "%s: failed to create xcb cairo surface", __FUNCTION__);

   w->cairo = cairo_create(w->surface);
   if (CAIRO_STATUS_SUCCESS != cairo_status(w->cairo))
      errx(1, "%s: failed to create cairo object", __FUNCTION__);
}

struct xwin *
xwin_init(
      struct xdisp         *xdisp,
      struct xwin_settings *settings)
{
   struct xwin *xwin = malloc(sizeof(struct xwin));
   if (NULL == xwin)
      err(1, "%s: couldn't malloc xdisp", __FUNCTION__);

   xwin->settings = settings;
   create_xcb_window(
         xwin,
         xdisp,
         settings->wname,
         settings->x, settings->y,
         settings->w, settings->h);

   setup_wm_hints(xdisp, xwin);
   setup_cairo(xdisp, xwin);
   xcb_map_window(xdisp->con, xwin->window);
   xwin->xdisp = xdisp;
   return xwin;
}

void
xwin_free(struct xwin *w)
{
   cairo_destroy(w->cairo);
   cairo_device_finish(cairo_surface_get_device(w->surface));
   cairo_surface_destroy(w->surface);
   xcb_destroy_window(w->xdisp->con, w->window);
   free(w);
}

void
xwin_push(struct xwin *w)
{
   double r, g, b, a;
   cairo_push_group(w->cairo);
   hex2rgba(w->settings->bgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(w->cairo, r, g, b, a);
   cairo_paint(w->cairo);
}

void
xwin_pop(struct xwin *w)
{
   cairo_pop_group_to_source(w->cairo);
   cairo_set_operator(w->cairo, CAIRO_OPERATOR_SOURCE);
   cairo_paint(w->cairo);
   cairo_set_operator(w->cairo, CAIRO_OPERATOR_OVER);
   xcb_flush(w->xdisp->con);
}

/* color util: translate a color string to r/g/b/a components */
void
hex2rgba(const char *s, double *r, double *g, double *b, double *a)
{
   unsigned int ir, ig, ib, ia;
   double scale;

   if (NULL == s || '\0' == s[0])
      errx(1, "%s: empty color!", __FUNCTION__);

   if ('#' == s[0])
      s++;

   switch (strlen(s)) {
   case 3:
      ia = 15;
      scale = 15.0;
      if (3 != sscanf(s, "%01x%01x%01x", &ir, &ig, &ib))
         errx(1, "%s: malformed rgb(3) color '%s'", __FUNCTION__, s);
      break;
   case 4:
      scale = 15.0;
      if (4 != sscanf(s, "%01x%01x%01x%01x", &ir, &ig, &ib, &ia))
         errx(1, "%s: malformed rgb(4) color '%s'", __FUNCTION__, s);
      break;
   case 6:
      ia = 255;
      scale = 255.0;
      if (3 != sscanf(s, "%02x%02x%02x", &ir, &ig, &ib))
         errx(1, "%s: malformed rgb(6) color '%s'", __FUNCTION__, s);
      break;
   case 8:
      scale = 255.0;
      if (4 != sscanf(s, "%02x%02x%02x%02x", &ir, &ig, &ib, &ia))
         errx(1, "%s: malformed rgba(8) color '%s'", __FUNCTION__, s);
      break;
   default:
      errx(1, "%s: malformed color '%s'", __FUNCTION__, s);
   }

   *r = (double)ir / scale;
   *g = (double)ig / scale;
   *b = (double)ib / scale;
   *a = (double)ia / scale;
}
