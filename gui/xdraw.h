#ifndef XDRAW_H
#define XDRAW_H

#include <cairo/cairo.h>
#include <pango/pango-font.h>

#include "chart.h"
#include "xcore.h"

/*
 * struct xctx: A stateful context for rendering widgets sequentially.
 * All drawing in the gui and widgets is done ONTO a xctx context.
 * Specifically, all the drawing primitives below draw onto such a context,
 * and the context tracts advancing the "pen" when drawing. E.g. when drawing
 * left-to-right (L2R), it advances the x-offset after each drawing primitive
 * below, so that the next primitive knows where to start. This encapsulation
 * greatly simplifies the calling code, eliminating much boilerplate code.
 *
 * Note that a "root" context is one that's *actually* rendered to the screen.
 * As such, a typical pipeline will create a temporary context, render a bunch
 * of primitives to it, and then draw that context onto a "root" context
 * using xdraw_context(root, source).
 *
 * TODO This abstraction should also permit vertical rendering (for a vertical
 * bar) rather easily, but that hasn't been a priority yet.
 */

typedef enum {
   L2R,
   R2L,
   CENTERED,
} xctx_direction_t;

typedef enum {
   BEFORE_RENDER,
   AFTER_RENDER
} xctx_state_t;

struct padding {
   double top, bottom, left, right;
};

struct xctx {
   bool              is_root;
   xctx_direction_t  direction;
   struct xfont     *xfont;
   cairo_t          *cairo;
   cairo_surface_t  *surface;
   int               h, w;
   struct padding   *padding;
   double            xoffset;
   double            yoffset;
};

struct xctx *xctx_init_root(struct xfont *font, struct xwin *win,
      xctx_direction_t direction, struct padding *padding);
struct xctx *xctx_init_scratchpad(struct xfont *font, struct xwin *win,
      xctx_direction_t direction, struct padding *padding);

void xctx_free(struct xctx *ctx);
void xctx_reset(struct xctx *ctx);
void xctx_complete(struct xctx *ctx);
void xctx_advance(struct xctx *ctx, xctx_state_t state, double xplus, double yplus);

/*
 * Drawing Primitives
 * All widget rendering is done using these simple primitives (so far)
 */

/* draw one context onto another */
void
xdraw_context(
      struct xctx     *dest,
      struct xctx     *source);

/* draw a color over an entire context */
void
xdraw_colorfill(
      struct xctx     *ctx,
      const char *const color);

/* draw a colored headerline on a widget */

typedef enum {
   NONE,    /* don't show them at all */
   ABOVE,   /* show the headers above the widget in the padding region */
   BELOW    /* show the headers below the widget in the padding region */
} header_style_t;

void
xdraw_headerline(
      struct xctx         *ctx,
      header_style_t  style,
      const char     *color);

/* draw some colored text (printf(3) style) */
void
xdraw_printf(
      struct xctx *ctx,
      const char *color,
      const char *fmt,
      ...);

/* draw simple progress bar showing some % completion */
void
xdraw_progress_bar(
      struct xctx     *ctx,
      const char *bgcolor,
      const char *pgcolor,
      double      width,
      double      pct);

/* draw a historical bar chart */
void
xdraw_chart(
      struct xctx       *ctx,
      struct chart *chart);

#endif
