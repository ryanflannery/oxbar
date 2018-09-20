#ifndef XDRAW_H
#define XDRAW_H

#include "chart.h"
#include "xcore.h"

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

typedef enum {
   L2R,
   R2L
} xdirection_t;

typedef enum {
   BEFORE_RENDER,
   AFTER_RENDER
} xrenderstate_t;

typedef struct xdraw_context {
   xdirection_t   direction;  /* const after init */
   xinfo_t       *xinfo;      /* const after init */
   double         xoffset;
   double         yoffset;
} xdraw_context_t;

xdraw_context_t *xdraw_context_init(xinfo_t *xinfo, xdirection_t direction);
void xdraw_context_free(xdraw_context_t *ctx);

void
xdraw_context_reset_offsets(
      xdraw_context_t *ctx);

void
xdraw_advance_offsets(
      xdraw_context_t  *ctx,
      xrenderstate_t    state,
      double            xadvance,
      double            yadvance);

void
xdraw_printf(
      xdraw_context_t *ctx,
      const char *color,
      const char *fmt,
      ...);

void
xdraw_hline(
      xdraw_context_t  *ctx,
      const char       *color,
      double            width,
      double            x1,
      double            x2);

void
xdraw_progress_bar(
      xdraw_context_t  *ctx,
      const char       *bgcolor,
      const char       *pgcolor,
      double            width,
      double            pct);

void
xdraw_chart(
      xdraw_context_t  *ctx,
      chart_t          *chart
      );

#endif
