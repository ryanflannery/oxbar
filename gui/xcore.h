#ifndef XINFO_H
#define XINFO_H

#include <sys/types.h>

#include <xcb/xcb.h>
#include <cairo/cairo.h>
#include <pango/pango-font.h>

/* pango font wrapper struct & setup/teardown methods */
typedef struct xfont {
   char                 *ufont;  /* user-specified font string unchanged */
   PangoFontDescription *pfont;  /* pango font loaded from that string   */
   int                   height; /* derived font height (in pixels)      */
} xfont_t;

xfont_t *xfont_init(const char *const font_description);
void xfont_free(xfont_t *xf);

/* xcb / display-info struct & setup/teardown methods */
typedef struct xdisp {
   uint16_t              display_width;
   uint16_t              display_height;
   xcb_connection_t     *con;
   xcb_screen_t         *root_screen;
   xcb_visualtype_t     *root_visual;
} xdisp_t;

xdisp_t *xdisp_init();
void xdisp_free(xdisp_t *x);

/* xcb / window & cairo wrapper struct & setup/teardown methods */

/* these are user-specified settings for the window */
typedef struct xwin_settings {
   char *bgcolor;    /* background color of window                   */
   char *wname;      /* name of window for window manager            */
   int   x, y;       /* (x,y) pixel coordinates of top-left corner   */
   int   w, h;       /* (width,height) pixel dimensions of window    */
} xwin_settings_t;

/* this is xwin window struct itself */
typedef struct xwin {
   xwin_settings_t  *settings;
   xdisp_t          *xdisp;      /* x display server                    */
   xcb_drawable_t    window;     /* oxbar xwindow                       */
   cairo_surface_t  *surface;    /* core cairo surface mapped to X      */
   cairo_t          *cairo;      /* core cairo object for rendering     */
} xwin_t;

xwin_t *xwin_init(xdisp_t *xdisp, xwin_settings_t *settings);
void xwin_free(xwin_t *w);

/*
 * Double buffering API for an xwin. When starting the main draw loop,
 * call xwin_push() to create a new buffer and clear it with the background
 * color. Once the main draw loop is done, call xwin_pop() to render that
 * buffer to the screen and flush it to the display.
 */
void xwin_push(xwin_t *w); /* double buffer: push buffer & clear it */
void xwin_pop(xwin_t *w);  /* double buffer: pop buffer & render it */

/*
 * A universal method to translate colors to r/g/b/a components.
 * The input string is a hex color like "#ff0000" (red). The leading '#' is
 * optional. The color spec can have 3 or 4 components, where the 4th is an
 * alpha component, so "#ff000088" is ~50% transparent red (it will blend with
 * what's behind it).
 * Each component can be on a 256 scale (like "#ff0000" where each component
 * takes two hexits), or a 16 scale (like "#f00", also red, where each
 * component takes a single hexit).
 */
void hex2rgba(const char *s, double *r, double *g, double *b, double *a);

#endif
