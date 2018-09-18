#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xcore.h"

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

/*
 * Creates core XCB/X11 connection
 */
void
xcore_setup_x_connection_screen_visual(xinfo_t *x)
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
xcore_setup_x_window(
      xinfo_t *xinfo,
      const char *name,
      uint32_t x, uint32_t y,
      uint32_t w, uint32_t h,
      const char *bgcolor)
{
   static uint32_t valwin[2] = {
      XCB_NONE,
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS
   };

   xinfo->x = x;
   xinfo->y = y;
   xinfo->w = w;
   xinfo->h = h;

   xinfo->xwindow = xcb_generate_id(xinfo->xcon);
   xcb_create_window(
         xinfo->xcon,
         XCB_COPY_FROM_PARENT,
         xinfo->xwindow,
         xinfo->xscreen->root,
         xinfo->x,
         xinfo->y,
         xinfo->w,
         xinfo->h,
         0, /* border width */
         XCB_WINDOW_CLASS_INPUT_OUTPUT,
         xinfo->xscreen->root_visual,
         XCB_CW_EVENT_MASK | XCB_CW_BACK_PIXMAP,
         valwin);

   xcb_icccm_set_wm_name(
         xinfo->xcon,
         xinfo->xwindow,
         XCB_ATOM_STRING, 8,
         strlen(name),
         name);

   xinfo->bgcolor = strdup(bgcolor);
   if (NULL == xinfo->bgcolor)
      err(1, "%s: strdup failed", __FUNCTION__);
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
void
xcore_setup_x_wm_hints(xinfo_t *x)
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

   static const char *atoms[] = {
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

   struts[top] = x->y + x->h;
   struts[top_start_x] = x->x;
   struts[top_end_x] = x->x + x->w;

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
xcore_setup_cairo(xinfo_t *x)
{
   x->surface = cairo_xcb_surface_create(
         x->xcon,
         x->xwindow,
         x->xvisual,
         x->w,
         x->h);
   x->cairo = cairo_create(x->surface);
   /* TODO Should I be checking the surface & cairo objects here? */
}

void
xcore_setup_xfont(
      xinfo_t *x,
      const char *font_description)
{
   if (NULL == (x->font = strdup(font_description)))
      err(1, "%s: strdup failed", __FUNCTION__);

   x->playout = pango_cairo_create_layout(x->cairo);
   x->pfont = pango_font_description_from_string(x->font);
   pango_layout_set_font_description(x->playout, x->pfont);
   /* TODO Are there any error checking methods to call after pango? */
}

xinfo_t *
xcore_init(
      const char *name,
      double x, double y,
      double w, double h,
      double padding,
      const char *bgcolor,
      const char *font)
{
   xinfo_t *xinfo = malloc(sizeof(xinfo_t));
   if (NULL == xinfo)
      err(1, "%s: couldn't malloc xinfo", __FUNCTION__);

   /* These need to be done in a specific order */
   xcore_setup_x_connection_screen_visual(xinfo);

   /* TODO Support display height based on font size
    * I used to support this using the cairo font API where the font size was
    * explicit, and I could use that to autoscale the display height as an
    * option (display height = font size + 2 * padding).
    * This would be great to support again, but after migrating to pango for
    * font loading and rendering (a big improvement in appearance) I haven't
    * yet figured out how to retrieve font height (not specific TEXT RENDERED
    * IN A FONT but rather generic font height).
    *
   if (-1 == height)
      height = (uint32_t)(ceil(fontpt + (2 * padding)));
    */
   if (-1 == y)
      y = xinfo->display_height - h;

   if (-1 == w)
      w = xinfo->display_width;

   xinfo->padding = padding;
   xcore_setup_x_window(
         xinfo,
         name,
         x, y,
         w, h,
         bgcolor);
   xcore_setup_x_wm_hints(xinfo);
   xcore_setup_cairo(xinfo);
   xcore_setup_xfont(xinfo, font);
   xcb_map_window(xinfo->xcon, xinfo->xwindow);
   return xinfo;
}

void
xcore_free(xinfo_t *x)
{
   pango_font_description_free(x->pfont);
   cairo_surface_destroy(x->surface);
   cairo_destroy(x->cairo);
   xcb_disconnect(x->xcon);
   free(x);
}
