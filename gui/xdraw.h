#ifndef XDRAW_H
#define XDRAW_H

#include "chart.h"
#include "xcore.h"

/*
 * xctx_t: A stateful context for rendering widgets sequentially
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
} xctx_direction_t;

typedef enum {
   BEFORE_RENDER,
   AFTER_RENDER
} xctx_state_t;

typedef struct xctx {
   xctx_direction_t direction;   /* const after init */
   xinfo_t         *xinfo;       /* const after init */
   double           xoffset;
   double           yoffset;
} xctx_t;

xctx_t *xctx_init(xinfo_t *xinfo, xctx_direction_t direction);
void xctx_free(xctx_t *ctx);
void xctx_reset(xctx_t *ctx);
void xctx_advance(xctx_t *ctx, xctx_state_t state, double xplus, double yplus);

void
xdraw_printf(
      xctx_t *ctx,
      const char *color,
      const char *fmt,
      ...);

void
xdraw_hline(
      xctx_t  *ctx,
      const char       *color,
      double            width,
      double            x1,
      double            x2);

void
xdraw_progress_bar(
      xctx_t  *ctx,
      const char       *bgcolor,
      const char       *pgcolor,
      double            width,
      double            pct);

void
xdraw_chart(
      xctx_t  *ctx,
      chart_t          *chart
      );

#endif
