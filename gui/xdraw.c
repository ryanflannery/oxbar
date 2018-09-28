#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pango/pangocairo.h>

#include "xdraw.h"

xctx_t*
xctx_init(xinfo_t *xinfo, xctx_direction_t direction)
{
   xctx_t *ctx = malloc(sizeof(xctx_t));
   if (NULL == ctx)
      err(1, "%s: malloc failed", __FUNCTION__);

   ctx->xinfo = xinfo;
   ctx->direction = direction;
   xctx_reset(ctx);

   return ctx;
}

void
xctx_free(xctx_t *ctx)
{
   free(ctx);
}

void
xctx_reset(xctx_t *ctx)
{
   switch (ctx->direction) {
   case L2R:
      ctx->xoffset = ctx->xinfo->padding;
      ctx->yoffset = ctx->xinfo->padding;
      break;
   case R2L:
      ctx->xoffset = ctx->xinfo->w - ctx->xinfo->padding;
      ctx->yoffset = ctx->xinfo->padding;
      break;
   }
}

void
xctx_advance(
      xctx_t      *ctx,
      xctx_state_t state,
      double       xplus,
      __attribute__((unused))
      double       yplus)
{
   switch (state) {
   case BEFORE_RENDER:
      if (L2R == ctx->direction) return;
      break;
   case AFTER_RENDER:
      if (R2L == ctx->direction) return;
      break;
   }

   switch (ctx->direction) {
   case L2R:
      ctx->xoffset += xplus;
      break;
   case R2L:
      ctx->xoffset -= xplus;
      break;
   }
}

void
xdraw_printf(
      xctx_t     *ctx,
      const char *color,
      const char *fmt,
      ...)
{
#define XDRAW_PRINTF_BUFF_MAXLEN 1000
   static char buffer[XDRAW_PRINTF_BUFF_MAXLEN];

   double r, g, b, a;
   int width, height;
   hex2rgba(color, &r, &g, &b, &a);

   va_list ap;
   va_start(ap, fmt);
   vsnprintf(buffer, XDRAW_PRINTF_BUFF_MAXLEN, fmt, ap);
   va_end(ap);

   PangoLayout *layout = pango_cairo_create_layout(ctx->xinfo->cairo);
   pango_layout_set_font_description(layout, ctx->xinfo->pfont);

   pango_layout_set_text(layout, buffer, -1);
   pango_layout_get_pixel_size(layout, &width, &height);

   xctx_advance(ctx, BEFORE_RENDER, width, height);

   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_move_to(ctx->xinfo->cairo, ctx->xoffset, ctx->yoffset);
   pango_cairo_show_layout(ctx->xinfo->cairo, layout);

   g_object_unref(layout);

   xctx_advance(ctx, AFTER_RENDER, width, height);
}

void
xdraw_hline(
      xctx_t     *ctx,
      const char *color,
      double      width,
      double      x1,
      double      x2)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_set_line_width(ctx->xinfo->cairo, width);
   cairo_move_to(ctx->xinfo->cairo, x1, 0);
   cairo_line_to(ctx->xinfo->cairo, x2, 0);
   cairo_stroke(ctx->xinfo->cairo);
   /* TODO advance */
}

void
xdraw_progress_bar(
      xctx_t     *ctx,
      const char *bgcolor,
      const char *pgcolor,
      double      width,
      double      pct)
{
   double r, g, b, a;
   double height = ctx->xinfo->h - ctx->xinfo->padding;

   xctx_advance(ctx, BEFORE_RENDER, width, height);

   hex2rgba(bgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_rectangle(
         ctx->xinfo->cairo,
         ctx->xoffset,
         ctx->xinfo->padding,
         width,
         height);
   cairo_fill(ctx->xinfo->cairo);

   hex2rgba(pgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_rectangle(
         ctx->xinfo->cairo,
         ctx->xoffset,
         ctx->xinfo->padding + ((100.0 - pct)/100.0) * height,
         width,
         (pct/100.0) * height);
   cairo_fill(ctx->xinfo->cairo);

   xctx_advance(ctx, AFTER_RENDER, width, height);
}

void
xdraw_chart(
      xctx_t  *ctx,
      chart_t *c
      )
{
   double chart_height = ctx->xinfo->h - ctx->xinfo->padding;
   double width = c->nsamples;
   double r, g, b, a;

   xctx_advance(ctx, BEFORE_RENDER, width, chart_height);

   hex2rgba(c->bgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_rectangle(
         ctx->xinfo->cairo,
         ctx->xoffset,
         ctx->xinfo->padding,
         width,
         chart_height);
   cairo_fill(ctx->xinfo->cairo);

   double min, max;
   chart_get_minmax(c, &min, &max);

   size_t i, j, count;
   for (count = 0, i = c->current + 1; count < c->nsamples; count++, i++) {
      if (i >= c->nsamples)
         i = 0;

      /* we draw the bars for this sample from the bottom UP */
      double y_bottom = ctx->xinfo->h;
      for (j = 0; j < c->nseries; j++) {
         if (!c->values[i][j])
            continue;

         double height;
         if (c->percents)
            height = c->values[i][j] / 100.0 * chart_height;
         else
            height = c->values[i][j] / max * chart_height;

         double y_top = y_bottom - height;

         /* don't go past the top (can happen w/ double rounding */
         if (y_top < ctx->xinfo->padding)
            y_top = ctx->xinfo->padding;

         /* NOTE: using cairo lines appears fuzzy - stick w/ rectangle */
         hex2rgba(c->colors[j], &r, &g, &b, &a);
         cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
         cairo_rectangle(ctx->xinfo->cairo,
               ctx->xoffset + count,
               y_top,
               1.0,
               height);
         cairo_fill(ctx->xinfo->cairo);
         y_bottom = y_top;
      }
   }
   xctx_advance(ctx, AFTER_RENDER, width, chart_height);
}
