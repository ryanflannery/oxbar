#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb_icccm.h>
#include <cairo/cairo-xcb.h>

#include "xcore.h"

/* pango wrapper */

xfont_t*
xfont_init(const char *font_description)
{
   xfont_t *xf = malloc(sizeof(xfont_t));
   if (NULL == xf)
      err(1, "%s: malloc failed", __FUNCTION__);

   if (NULL == (xf->ufont = strdup(font_description)))
      err(1, "%s: strdup failed", __FUNCTION__);

   xf->pfont = pango_font_description_from_string(font_description);
   if (!xf->pfont)
      errx(1, "pango failed to load font '%s'", font_description);

   xf->height = pango_font_description_get_size(xf->pfont) / PANGO_SCALE;
   return xf;
}

void
xfont_free(xfont_t *xf)
{
   free(xf->ufont);
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

xdisp_t*
xdisp_init()
{
   xdisp_t *x = malloc(sizeof(xdisp_t));
   if (NULL == x)
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
xdisp_free(xdisp_t *x)
{
   xcb_disconnect(x->con);
   free(x);
}

/* window wrapper */

static void
create_x_window(
      xwin_t *xwin,
      const xdisp_t *xdisp,
      const char *name,
      uint32_t x, uint32_t y,
      uint32_t w, uint32_t h)
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

   xwin->x = x;
   xwin->y = y;
   xwin->w = w;
   xwin->h = h;

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
 *                                                                   -ryan
 */
static void
setup_x_wm_hints(const xdisp_t *x, xwin_t *w)
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

   struts[top] = w->y + w->h;
   struts[top_start_x] = w->x;
   struts[top_end_x] = w->x + w->w;

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
setup_cairo(const xdisp_t *x, xwin_t *w)
{
   w->surface = cairo_xcb_surface_create(
         x->con,
         w->window,
         x->root_visual,
         w->w,
         w->h);
   if (CAIRO_STATUS_SUCCESS != cairo_surface_status(w->surface))
      errx(1, "%s: failed to create xcb cairo surface", __FUNCTION__);

   w->cairo = cairo_create(w->surface);
   if (CAIRO_STATUS_SUCCESS != cairo_status(w->cairo))
      errx(1, "%s: failed to create cairo object", __FUNCTION__);
}

xwin_t *
xwin_init(
      const xdisp_t *xdisp,
      const char *name,
      double x, double y,
      double w, double h)
{
   xwin_t *xwin = malloc(sizeof(xwin_t));
   if (NULL == xwin)
      err(1, "%s: couldn't malloc xdisp", __FUNCTION__);

   create_x_window(
         xwin,
         xdisp,
         name,
         x, y,
         w, h);

   setup_x_wm_hints(xdisp, xwin);
   setup_cairo(xdisp, xwin);
   xcb_map_window(xdisp->con, xwin->window);
   return xwin;
}

void
xwin_free(xdisp_t *xdisp, xwin_t *w)
{
   cairo_surface_destroy(w->surface);
   cairo_destroy(w->cairo);
   xcb_destroy_window(xdisp->con, w->window);
   free(w);
}
