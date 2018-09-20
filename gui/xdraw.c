#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pango/pangocairo.h>

#include "xdraw.h"

void
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

xdraw_context_t*
xdraw_context_init(xinfo_t *xinfo)
{
   xdraw_context_t *ctx = malloc(sizeof(xdraw_context_t));
   if (NULL == ctx)
      err(1, "%s: malloc failed", __FUNCTION__);

   ctx->xinfo = xinfo;
   ctx->xoffset = xinfo->padding;
   ctx->yoffset = xinfo->padding;
   return ctx;
}

void
xdraw_context_free(xdraw_context_t *ctx)
{
   free(ctx);
}

void
xdraw_clear(xdraw_context_t *ctx)
{
   double r, g, b, a;
   hex2rgba(ctx->xinfo->bgcolor, &r, &g, &b, &a);
   cairo_push_group(ctx->xinfo->cairo);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_paint(ctx->xinfo->cairo);

   ctx->xoffset = ctx->xinfo->padding;
   ctx->yoffset = ctx->xinfo->padding;
}

void
xdraw_flush(xdraw_context_t *ctx)
{
   cairo_pop_group_to_source(ctx->xinfo->cairo);
   cairo_paint(ctx->xinfo->cairo);
   cairo_surface_flush(ctx->xinfo->surface);
   xcb_flush(ctx->xinfo->xcon);
}

void
xdraw_printf(
      xdraw_context_t   *ctx,
      const char        *color,
      const char        *fmt,
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

   pango_layout_set_text(ctx->xinfo->playout, buffer, -1);
   pango_layout_get_pixel_size(ctx->xinfo->playout, &width, &height);

   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_move_to(ctx->xinfo->cairo, ctx->xoffset, ctx->yoffset);
   pango_cairo_show_layout(ctx->xinfo->cairo, ctx->xinfo->playout);

   ctx->xoffset += width;
}

void
xdraw_text_right_aligned(
      xdraw_context_t  *ctx,
      const char       *color,
      const char       *text)
{
   double r, g, b, a;
   int width, height;
   hex2rgba(color, &r, &g, &b, &a);

   pango_layout_set_text(ctx->xinfo->playout, text, -1);
   pango_layout_get_pixel_size(ctx->xinfo->playout, &width, &height);

   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_move_to(ctx->xinfo->cairo, ctx->xoffset - width, ctx->yoffset);
   pango_cairo_show_layout(ctx->xinfo->cairo, ctx->xinfo->playout);

   ctx->xoffset -= width;
}

void
xdraw_hline(
      xdraw_context_t  *ctx,
      const char       *color,
      double            width,
      double            x1,
      double            x2)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_set_line_width(ctx->xinfo->cairo, width);
   cairo_move_to(ctx->xinfo->cairo, x1, 0);
   cairo_line_to(ctx->xinfo->cairo, x2, 0);
   cairo_stroke(ctx->xinfo->cairo);
}

void
xdraw_progress_bar(
      xdraw_context_t  *ctx,
      const char       *bgcolor,
      const char       *pgcolor,
      double            width,
      double            pct)
{
   double r, g, b, a;
   double height = ctx->xinfo->h - ctx->xinfo->padding;

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

   ctx->xoffset += width;
}

void
xdraw_chart(
      xdraw_context_t  *ctx,
      chart_t          *c
      )
{
   const double chart_height = ctx->xinfo->h - ctx->xinfo->padding;
   double r, g, b, a;

   hex2rgba(c->bgcolor, &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_rectangle(
         ctx->xinfo->cairo,
         ctx->xoffset,
         ctx->xinfo->padding,
         c->nsamples,
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
   ctx->xoffset += c->nsamples;
}
