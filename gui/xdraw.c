#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pango/pangocairo.h>

#include "xdraw.h"

/* a useful local method to translate colors to into rgba pairs */
static void
hex2rgba(const char *s, double *r, double *g, double *b, double *a)
{
   unsigned int ir, ig, ib, ia;

   if (NULL == s || '\0' == s[0]) {
      *r = *g = *b = *a = 1.0;
      return;
   }

   if ('#' == s[0])
      s++;

   switch (strlen(s)) {
      case 6:
         if (3 != sscanf(s, "%02x%02x%02x", &ir, &ig, &ib))
            errx(1, "%s: malformed rgb color '%s'", __FUNCTION__, s);

         ia = 255;
         break;
      case 8:
         if (4 != sscanf(s, "%02x%02x%02x%02x", &ir, &ig, &ib, &ia))
            errx(1, "%s: malformed rgba color '%s'", __FUNCTION__, s);

         break;
      default:
         errx(1, "%s: malformed color '%s'", __FUNCTION__, s);
   }

   *r = (double)ir / 255.0;
   *g = (double)ig / 255.0;
   *b = (double)ib / 255.0;
   *a = (double)ia / 255.0;
}

xctx_t*
xctx_init(
      xfont_t          *font,
      xwin_t           *win,
      xctx_direction_t  direction,
      double            padding,
      bool              make_root)
{
   xctx_t *ctx = malloc(sizeof(xctx_t));
   if (NULL == ctx)
      err(1, "%s: malloc failed", __FUNCTION__);

   ctx->is_root   = make_root;
   ctx->direction = direction;
   ctx->xfont     = font;
   ctx->h         = win->h;
   ctx->w         = win->w;

   ctx->paddings.top    = padding;
   ctx->paddings.bottom = padding;
   ctx->paddings.left   = padding;
   ctx->paddings.right  = padding;

   /* if root, use xcore's root window surface/cairo, otherwise a new one */
   if (make_root) {
      ctx->surface = win->surface;
      ctx->cairo   = win->cairo;
   } else {
      ctx->surface = cairo_surface_create_similar(
            win->surface,
            CAIRO_CONTENT_COLOR_ALPHA,
            win->w,
            win->h);
      if (CAIRO_STATUS_SUCCESS != cairo_surface_status(ctx->surface))
         errx(1, "%s: failed to create cairo surface", __FUNCTION__);

      ctx->cairo = cairo_create(ctx->surface);
      if (CAIRO_STATUS_SUCCESS != cairo_status(ctx->cairo))
         errx(1, "%s: failed to create cairo object", __FUNCTION__);
   }

   xctx_reset(ctx);
   return ctx;
}

void
xctx_free(xctx_t *ctx)
{
   if (!ctx->is_root) {
      cairo_surface_destroy(ctx->surface);
      cairo_destroy(ctx->cairo);
   }
   free(ctx);
}

void
xctx_reset(xctx_t *ctx)
{
   switch (ctx->direction) {
   case L2R:
      ctx->xoffset = ctx->paddings.left;
      ctx->yoffset = ctx->paddings.top;
      break;
   case R2L:
      ctx->xoffset = ctx->w - ctx->paddings.right;
      ctx->yoffset = ctx->paddings.top;
      break;
   case CENTERED:
      ctx->xoffset = ctx->w / 2;
      ctx->yoffset = 0;
      break;
   }
}

void
xctx_advance(
      xctx_t      *ctx,
      xctx_state_t state,
      double       xdelta,
      __attribute__((unused))
      double       ydelta)
{
   switch (ctx->direction) {
   case L2R:
      switch (state) {
      case BEFORE_RENDER:
         return;
      case AFTER_RENDER:
         ctx->xoffset += xdelta;
         break;
      }
      break;

   case R2L:
      switch (state) {
      case BEFORE_RENDER:
         ctx->xoffset -= xdelta;
         break;
      case AFTER_RENDER:
         return;
      }
      break;

   case CENTERED:
      switch (state) {
      case BEFORE_RENDER:
         ctx->xoffset -= (xdelta / 2);
         break;
      case AFTER_RENDER:
         ctx->xoffset += (xdelta / 2);
         break;
      }
      break;
   }
}

void
xctx_root_push(xctx_t *ctx)
{
   if (!ctx->is_root)
      errx(1, "%s: this doesn't make sense", __FUNCTION__);

   cairo_push_group(ctx->cairo);
}

void
xctx_root_pop(xctx_t *ctx)
{
   if (!ctx->is_root)
      errx(1, "%s: this doesn't make sense", __FUNCTION__);

   cairo_pop_group_to_source(ctx->cairo);
   cairo_set_operator(ctx->cairo, CAIRO_OPERATOR_SOURCE);
   cairo_paint(ctx->cairo);
   cairo_set_operator(ctx->cairo, CAIRO_OPERATOR_OVER);
}

/*
 * Drawing Primitives - note they all follow the same pattern of
 *
 *    xctx_advance(dest, BEFORE_RENDER, width, height);
 *    ...do drawing stuff...
 *    xctx_advance(dest, AFTER_RENDER, width, height);
 *
 * This is to support right-to-left (RTL) rendering, in which we need to
 * advance the "pen" when drawing *before* we actually begin drawing.
 * The xctx_advance() api knows which of these actually advances the pen,
 * based on the direction of the destination context.
 */

void
xdraw_context(
      xctx_t      *dest,
      xctx_t      *source)
{
   xctx_advance(dest, BEFORE_RENDER, source->xoffset, source->yoffset);

   cairo_set_source_surface(
         dest->cairo,
         source->surface,
         dest->xoffset,
         0);
   cairo_rectangle(
         dest->cairo,
         dest->xoffset,
         0,
         source->xoffset,
         source->h);
   cairo_fill(dest->cairo);

   xctx_advance(dest, AFTER_RENDER, source->xoffset, source->yoffset);
}
void
xdraw_outline(
      xctx_t *ctx,
      double  x, double y,
      double  w, double h)
{
   double r, g, b, a;
   hex2rgba("ffffff", &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->cairo, r, g, b, a);
   cairo_set_line_width(ctx->cairo, 1);
   cairo_rectangle(
         ctx->cairo,
         x, y,
         w, h);
   cairo_stroke(ctx->cairo);
}
void
xdraw_context_at(
      xctx_t      *dest,
      double       xdest,
      double       ydest,
      xctx_t      *source)
{
   cairo_set_source_surface(
         dest->cairo,
         source->surface,
         xdest, ydest);
   cairo_rectangle(
         dest->cairo,
         xdest, ydest,
         source->xoffset,
         source->h);
   cairo_fill(dest->cairo);
}

void
xdraw_color(
      xctx_t     *ctx,
      const char *const bgcolor)
{
   double r, g, b, a;
   hex2rgba(bgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->cairo, r, g, b, a);
   cairo_paint(ctx->cairo);
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
   cairo_set_source_rgba(ctx->cairo, r, g, b, a);
   cairo_set_line_width(ctx->cairo, width);
   cairo_move_to(ctx->cairo, x1, 0);
   cairo_line_to(ctx->cairo, x2, 0);
   cairo_stroke(ctx->cairo);
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

   PangoLayout *layout = pango_cairo_create_layout(ctx->cairo);
   pango_layout_set_font_description(layout, ctx->xfont->pfont);
   pango_layout_set_text(layout, buffer, -1);
   pango_layout_get_pixel_size(layout, &width, &height);

   xctx_advance(ctx, BEFORE_RENDER, width, height);

   cairo_set_source_rgba(ctx->cairo, r, g, b, a);
   cairo_move_to(ctx->cairo, ctx->xoffset, ctx->yoffset);
   pango_cairo_show_layout(ctx->cairo, layout);

   xctx_advance(ctx, AFTER_RENDER, width, height);

   g_object_unref(layout);
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
   double height = ctx->h - (ctx->paddings.top + ctx->paddings.bottom);

   xctx_advance(ctx, BEFORE_RENDER, width, height);

   hex2rgba(bgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->cairo, r, g, b, a);
   cairo_rectangle(
         ctx->cairo,
         ctx->xoffset,
         ctx->paddings.top,
         width,
         height);
   cairo_fill(ctx->cairo);

   hex2rgba(pgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->cairo, r, g, b, a);
   cairo_rectangle(
         ctx->cairo,
         ctx->xoffset,
         ctx->paddings.top + ((100.0 - pct)/100.0) * height,
         width,
         (pct/100.0) * height);
   cairo_fill(ctx->cairo);

   xctx_advance(ctx, AFTER_RENDER, width, height);
}

void
xdraw_chart(
      xctx_t  *ctx,
      chart_t *c
      )
{
   double chart_height = ctx->h - (ctx->paddings.top + ctx->paddings.bottom);
   double width = c->nsamples;
   double r, g, b, a;

   xctx_advance(ctx, BEFORE_RENDER, width, chart_height);

   hex2rgba(c->bgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->cairo, r, g, b, a);
   cairo_rectangle(
         ctx->cairo,
         ctx->xoffset,
         ctx->paddings.top,
         width,
         chart_height);
   cairo_fill(ctx->cairo);

   double min, max;
   chart_get_minmax(c, &min, &max);

   size_t i, j, count;
   for (count = 0, i = c->current + 1; count < c->nsamples; count++, i++) {
      if (i >= c->nsamples)
         i = 0;

      /* we draw the bars for this sample from the bottom UP */
      double y_bottom = ctx->h - ctx->paddings.bottom;
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
         if (y_top < ctx->paddings.top)
            y_top = ctx->paddings.top;

         /* NOTE: using cairo lines appears fuzzy - stick w/ rectangle */
         hex2rgba(c->colors[j], &r, &g, &b, &a);
         cairo_set_source_rgba(ctx->cairo, r, g, b, a);
         cairo_rectangle(ctx->cairo,
               ctx->xoffset + count,
               y_top,
               1.0,
               height);
         cairo_fill(ctx->cairo);
         y_bottom = y_top;
      }
   }
   xctx_advance(ctx, AFTER_RENDER, width, chart_height);
}
