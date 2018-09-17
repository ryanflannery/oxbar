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

uint32_t
xdraw_text_right_aligned(
      xinfo_t    *xinfo,
      const char *color,
      double      x,
      double      y,
      const char *text)
{
   double r, g, b, a;
   int width, height;
   hex2rgba(color, &r, &g, &b, &a);

   pango_layout_set_text(xinfo->playout, text, -1);
   pango_layout_get_pixel_size(xinfo->playout, &width, &height);

   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_move_to(xinfo->cairo, x - width, y);
   pango_cairo_show_layout(xinfo->cairo, xinfo->playout);

   return width;
}

void
xdraw_hline(
      xinfo_t    *xinfo,
      const char *color,
      double      width,
      double      x1,
      double      x2)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);
   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_set_line_width(xinfo->cairo, width);
   cairo_move_to(xinfo->cairo, x1, 0);
   cairo_line_to(xinfo->cairo, x2, 0);
   cairo_stroke(xinfo->cairo);
}

void
xdraw_vertical_stack(
      xdraw_context_t  *ctx,
      double            x,
      double            width,
      size_t            nvalues,
      const char      **colors,
      double           *percents)
{
   double r, g, b, a;
   size_t i;
   double offset = 0;

   for (i = 0; i < nvalues; i++) {
      if (0.0 == percents[i])
         continue;

      hex2rgba(colors[i], &r, &g, &b, &a);
      cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
      double height = percents[i] / 100.0 * (ctx->xinfo->h - ctx->xinfo->padding);
      cairo_rectangle(ctx->xinfo->cairo,
            x,
            ctx->xinfo->padding + offset,
            width,
            height);
      offset += height;
      cairo_fill(ctx->xinfo->cairo);
   }
   /* ctx->xoffset += width; */
}

void
xdraw_series(
      xdraw_context_t  *ctx,
      const char      **colors,
      tseries_t        *t)
{
   double r, g, b, a;
   size_t width = t->size;

   /* paint grey background to start */
   hex2rgba("535353", &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_rectangle(ctx->xinfo->cairo,
         ctx->xoffset,
         ctx->xinfo->padding,
         width,
         (ctx->xinfo->h - ctx->xinfo->padding));
   cairo_fill(ctx->xinfo->cairo);

   size_t count, i;
   double max = t->values[0];
   for (i = 1; i < t->size; i++)
      if (t->values[i] > max)
         max = t->values[i];

   for (count = 0, i = t->current + 1; count < t->size; count++, i++) {
      if (i >= t->size)
         i = 0;

      xdraw_vertical_stack(
            ctx,
            ctx->xoffset + count,
            1,
            2,
            colors,
            (double[]) {
               (max - t->values[i]) / max * 100,
               t->values[i] / max * 100
            });
   }
   ctx->xoffset += width;
}

void
xdraw_histogram(
      xdraw_context_t *ctx,
      const char     **colors,
      histogram_t      *h)
{
   double r, g, b, a;
   int width = h->nsamples;

   /* paint grey background to start */
   hex2rgba("535353", &r, &g, &b, &a);
   cairo_set_source_rgba(ctx->xinfo->cairo, r, g, b, a);
   cairo_rectangle(ctx->xinfo->cairo,
         ctx->xoffset,
         ctx->xinfo->padding,
         width,
         (ctx->xinfo->h - ctx->xinfo->padding));
   cairo_fill(ctx->xinfo->cairo);

   size_t count, i;
   for (count = 0, i = h->current + 1; count < h->nsamples; count++, i++) {
      if (i >= h->nsamples)
         i = 0;

      xdraw_vertical_stack(
            ctx,
            ctx->xoffset + count,
            1,
            h->nseries,
            colors,
            h->series[i]);
   }
   ctx->xoffset += width;
}
