#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xdraw.h"

void
hex2rgba(const char *s, double *r, double *g, double *b, double *a)
{
   unsigned int ir, ig, ib, ia;

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
xdraw_text(
      xinfo_t    *xinfo,
      const char *color,
      double      x,
      double      y,
      const char *text)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);

   cairo_set_source_rgba(xinfo->cairo, r, g, b, a);
   cairo_move_to(xinfo->cairo, x, y);
   cairo_show_text(xinfo->cairo, text);

   cairo_text_extents_t extents;
   cairo_text_extents(xinfo->cairo, text, &extents);

   return extents.x_advance;
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
      double height = percents[i] / 100.0 * xinfo->fontpt;
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
         xinfo->fontpt);
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
