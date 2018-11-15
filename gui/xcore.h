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

#ifndef XINFO_H
#define XINFO_H

#include <sys/types.h>

#include <xcb/xcb.h>
#include <cairo/cairo.h>
#include <pango/pango-font.h>

/* pango font wrapper struct & setup/teardown methods */

struct xfont_settings {               /* user provided settings for xfont    */
	char  *desc;                  /* free-form font description string   */
	char  *fgcolor;               /* default foreground color for text   */
};

struct xfont {                        /* core xfont struct (read-only)       */
	struct xfont_settings *settings;
	PangoFontDescription  *pfont; /* pango font loaded from that string  */
	int                    height;/* derived font height (in pixels)     */
};

struct xfont *xfont_init(struct xfont_settings *settings);
void xfont_free(struct xfont *xf);

/* xcb / display-info struct & setup/teardown methods */
struct xdisp {
	uint16_t              display_width;
	uint16_t              display_height;
	xcb_connection_t     *con;
	xcb_screen_t         *root_screen;
	xcb_visualtype_t     *root_visual;
};

struct xdisp *xdisp_init();
void xdisp_free(struct xdisp *x);

/* xcb / window & cairo wrapper struct & setup/teardown methods */

struct xwin_settings {    /* user provided settings                       */
	char *bgcolor;    /* default background color for whole window    */
	char *wname;      /* name of window for window manager            */
	int   x, y;       /* (x,y) pixel coordinates of top-left corner   */
	int   w, h;       /* (width,height) pixel dimensions of window    */
};

struct xwin {                         /* core xwin struct (read-only)     */
	struct xwin_settings *settings;
	struct xdisp         *xdisp;  /* x display server                 */
	xcb_drawable_t        window; /* oxbar xwindow                    */
	cairo_surface_t      *surface;/* core cairo surface mapped to X   */
	cairo_t              *cairo;  /* core cairo object for rendering  */
};

struct xwin *xwin_init(struct xdisp *xdisp, struct xwin_settings *settings);
void xwin_free(struct xwin *w);

/*
 * Double buffering API for an xwin. When starting the main draw loop,
 * call xwin_push() to create a new buffer and clear it with the background
 * color. Once the main draw loop is done, call xwin_pop() to render that
 * buffer to the screen and flush it to the display.
 */
void xwin_push(struct xwin *w); /* double buffer: push buffer & clear it */
void xwin_pop(struct xwin *w);  /* double buffer: pop buffer & render it */

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
