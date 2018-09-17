#ifndef XDRAW_H
#define XDRAW_H

#include "xcore.h"
#include "tseries.h"
#include "histogram.h"

/*
 * xdraw_context_t: A stateful context for rendering widgets sequentially
 * In our rendering pipeline, widgest and pieces of widgets are rendered
 * sequentially. This is the context passed from rendering to rendering to
 * record 1) WHAT to render to (the xinfo object) and 2) WHERE we're currently
 * rendering at. After a render is done, that WHERE part is *advanced*, so
 * that subsequent calls will know WHERE to render.
 *
 * This abstraction removes a lot of boilerplate x/y offset advances from
 * rendering widgets (making those easier) and also makes things like
 * right-aligning widgets easier, as well as vertical rendering (once that's
 * done).
 * TODO Once xdraw_context is fully adopted, support vertical rendering!
 *
 * The "WHERE" state tracked is an x/y offset pair, which always records the
 * upper-left corner of where the next object should be rendered. After any
 * xdraw_* method is called on a context, it should update that context's
 * x/y offset to where the next object should be drawn.
 */
typedef struct xdraw_context {
   xinfo_t *xinfo;
   double   xoffset;
   double   yoffset;
} xdraw_context_t;

xdraw_context_t *xdraw_context_init(xinfo_t *xinfo); /* TODO add direction HERE! */
void xdraw_context_free(xdraw_context_t *ctx);
void xdraw_clear(xdraw_context_t *ctx);
void xdraw_flush(xdraw_context_t *ctx);

void
xdraw_printf(
      xdraw_context_t *ctx,
      const char *color,
      const char *fmt,
      ...);

uint32_t
xdraw_text_right_aligned(
      xinfo_t    *xinfo,
      const char *color,
      double      x,
      double      y,
      const char *text);

void
xdraw_hline(
      xinfo_t    *xinfo,
      const char *color,
      double      width,
      double      x1,
      double      x2);

/* TODO remove xdraw_vertical_stack() - replace with a logical */
void
xdraw_vertical_stack(
      xdraw_context_t  *ctx,
      double            x,
      double            width,
      size_t            nvalues,
      const char      **colors,
      double           *percents);

void
xdraw_series(
      xdraw_context_t  *ctx,
      const char      **colors,
      tseries_t        *t);

void
xdraw_histogram(
      xdraw_context_t  *ctx,
      const char      **colors,
      histogram_t      *h);

#endif
