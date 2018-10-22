#ifndef XDRAW_H
#define XDRAW_H

#include <cairo/cairo.h>
#include <pango/pango-font.h>

#include "chart.h"
#include "xcore.h"

/*
 * xctx_t: A stateful context for rendering widgets sequentially.
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

typedef struct padding {
   double top, bottom, left, right;
} padding_t;

typedef struct xctx {
   bool              is_root;
   xctx_direction_t  direction;
   xfont_t          *xfont;
   cairo_t          *cairo;
   cairo_surface_t  *surface;
   int               h, w;
   padding_t         padding;
   double            xoffset;
   double            yoffset;
} xctx_t;

xctx_t *xctx_init(xfont_t *font, xwin_t *win, xctx_direction_t direction,
      double height, padding_t padding, bool make_root);
void xctx_free(xctx_t *ctx);
void xctx_reset(xctx_t *ctx);
void xctx_complete(xctx_t *ctx);
void xctx_advance(xctx_t *ctx, xctx_state_t state, double xplus, double yplus);

/*
 * Drawing Primitives
 * All widget rendering is done using these simple primitives (so far)
 */

/* draw one context onto another */
void
xdraw_context(
      xctx_t     *dest,
      xctx_t     *source);

/* draw a color over an entire context */
void
xdraw_color(
      xctx_t     *ctx,
      const char *const bgcolor);

/* draw a horizontal line on a context */
void
xdraw_hline(
      xctx_t     *ctx,
      const char *color,
      double      width,
      double      y,
      double      x1,
      double      x2);

/* draw some colored text (printf(3) style) */
void
xdraw_printf(
      xctx_t *ctx,
      const char *color,
      const char *fmt,
      ...);

/* draw simple progress bar showing some % completion */
void
xdraw_progress_bar(
      xctx_t     *ctx,
      const char *bgcolor,
      const char *pgcolor,
      double      width,
      double      pct);

/* draw a historical bar chart */
void
xdraw_chart(
      xctx_t  *ctx,
      chart_t *chart);

#endif
