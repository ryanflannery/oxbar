#include <err.h>
#include <math.h>
#include <stdio.h>
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

void
xdraw_clear_all(xinfo_t *xinfo, const char *color)
{
   double r, g, b, a;

   hex2rgba(color, &r, &g, &b, &a);
   cairo_push_group(xinfo->cairo);
   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_paint(xinfo->cairo);

}

void
xdraw_flush(xinfo_t *xinfo)
{
   cairo_pop_group_to_source(xinfo->cairo);
   cairo_paint(xinfo->cairo);

   cairo_surface_flush(xinfo->surface);
   xcb_flush(xinfo->xcon);
}

uint32_t
xdraw_printf(
      xinfo_t    *xinfo,
      const char *color,
      double      x,
      double      y,
      const char *fmt,
      ...)
{
   static const size_t bufflen = 1000;
   static char buffer[bufflen];

   double r, g, b, a;
   int width, height;
   hex2rgba(color, &r, &g, &b, &a);

   va_list ap;
   va_start(ap, fmt);
   vsnprintf(buffer, bufflen, fmt, ap);
   va_end(ap);

   pango_layout_set_text(xinfo->playout, buffer, -1);
   pango_layout_get_pixel_size(xinfo->playout, &width, &height);

   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_move_to(xinfo->cairo, x, y);
   pango_cairo_show_layout(xinfo->cairo, xinfo->playout);

   return width;
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

uint32_t
xdraw_vertical_stack(
      xinfo_t     *xinfo,
      uint32_t     x,
      double       width,
      size_t       nvalues,
      const char **colors,
      double      *percents)
{
   double r, g, b, a;
   size_t i;
   double offset = 0;

   for (i = 0; i < nvalues; i++) {
      if (0.0 == percents[i])
         continue;

      hex2rgba(colors[i], &r, &g, &b, &a);
      cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
      double height = percents[i] / 100.0 * (xinfo->h - xinfo->padding);
      cairo_rectangle(xinfo->cairo,
            x,
            xinfo->padding + offset,
            width,
            height);
      offset += height;
      cairo_fill(xinfo->cairo);
   }

   return width;
}

uint32_t
xdraw_series(
      xinfo_t       *xinfo,
      double         x,
      const char   **colors,
      tseries_t     *t)
{
   double r, g, b, a;
   size_t width = t->size;

   /* paint grey background to start */
   hex2rgba("535353", &r, &g, &b, &a);
   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_rectangle(xinfo->cairo,
         x,
         xinfo->padding,
         width,
         (xinfo->h - xinfo->padding));
   cairo_fill(xinfo->cairo);

   size_t count, i;
   double max = t->values[0];
   for (i = 1; i < t->size; i++)
      if (t->values[i] > max)
         max = t->values[i];

   for (count = 0, i = t->current + 1; count < t->size; count++, i++) {
      if (i >= t->size)
         i = 0;

      xdraw_vertical_stack(
            xinfo,
            x + count,
            1,
            2,
            colors,
            (double[]) {
               (max - t->values[i]) / max * 100,
               t->values[i] / max * 100
            });
   }

   return width;
}

uint32_t
xdraw_histogram(
      xinfo_t     *xinfo,
      double       x,
      const char **colors,
      histogram_t *h)
{
   double r, g, b, a;
   int width = h->nsamples;

   /* paint grey background to start */
   hex2rgba("535353", &r, &g, &b, &a);
   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_rectangle(xinfo->cairo,
         x,
         xinfo->padding,
         width,
         (xinfo->h - xinfo->padding));
   cairo_fill(xinfo->cairo);

   size_t count, i;
   for (count = 0, i = h->current + 1; count < h->nsamples; count++, i++) {
      if (i >= h->nsamples)
         i = 0;

      xdraw_vertical_stack(
            xinfo,
            x + count,
            1,
            h->nseries,
            colors,
            h->series[i]);
   }

   return width;
}
