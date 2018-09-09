#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"

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

oxbarui_t*
ui_create(
      const char *wname,
      int         x,
      int         y,
      int         width,
      int         height,
      int         padding,
      double      fontpt,
      const char *font
      )
{
   oxbarui_t *ui = malloc(sizeof(oxbarui_t));
   if (NULL == ui)
      err(1, "%s: couldn't malloc ui", __FUNCTION__);

   ui->xinfo = malloc(sizeof(xinfo_t));
   if (NULL == ui->xinfo)
      err(1, "%s: couldn't malloc xinfo", __FUNCTION__);

   /* XXX These need to be done in a specific order */
   xcore_setup_x_connection_screen_visual(ui->xinfo);

   if (-1 == height)
      height = (uint32_t)(ceil(fontpt + (2 * padding)));

   if (-1 == y)
      y = ui->xinfo->display_height - height;

   if (-1 == width)
      width = ui->xinfo->display_width;

   ui->xinfo->padding = padding;

   xcore_setup_x_window(
         ui->xinfo,
         wname,
         x, y,
         width, height);
   xcore_setup_x_wm_hints(ui->xinfo);
   xcore_setup_cairo(ui->xinfo);
   xcore_setup_xfont(ui->xinfo, font, fontpt);

   /* now map the window & do an initial paint */
   xcb_map_window(ui->xinfo->xcon, ui->xinfo->xwindow);
   cairo_surface_flush(ui->xinfo->csurface);
   xcb_flush(ui->xinfo->xcon);

   return ui;
}

uint32_t
ui_draw_text(
      oxbarui_t *ui,
      const char *color,
      double x, double y,
      const char *text)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);

   cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, a);
   cairo_move_to(ui->xinfo->cairo, x, y);
   cairo_show_text(ui->xinfo->cairo, text);

   cairo_text_extents_t extents;
   cairo_text_extents(ui->xinfo->cairo, text, &extents);

   return extents.x_advance;
}

void
ui_draw_top_header(
      oxbarui_t *ui,
      const char *color,
      double x1, double x2)
{
   double r, g, b, a;
   hex2rgba(color, &r, &g, &b, &a);

   cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, 0.7); /* TODO config 0.7 */
   cairo_set_line_width(ui->xinfo->cairo, ui->xinfo->padding);
   cairo_move_to(ui->xinfo->cairo, x1, 0);
   cairo_line_to(ui->xinfo->cairo, x2, 0);
   cairo_stroke(ui->xinfo->cairo);
}

uint32_t
ui_draw_vertical_stack_bar(
      oxbarui_t *ui,
      double x,
      double pct)
{
   double width = 7.0; /* TODO configurable width */
   double r, g, b, a;

   hex2rgba("dc322f", &r, &g, &b, &a); /* TODO configurable color */
   cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, a);
   cairo_rectangle(ui->xinfo->cairo,
         x,
         ui->xinfo->padding,
         width,
         ui->xinfo->fontpt);
   cairo_fill(ui->xinfo->cairo);

   double height = (pct/100.0) * ui->xinfo->fontpt;

   hex2rgba("859900", &r, &g, &b, &a); /* TODO configurable color */
   cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, a);
   cairo_rectangle(ui->xinfo->cairo,
         x,
         ui->xinfo->padding + (ui->xinfo->fontpt - height),
         width,
         height);
   cairo_fill(ui->xinfo->cairo);

   return width;
}

uint32_t
ui_draw_histogram(oxbarui_t *ui, histogram_t *h, double x)
{
   double r, g, b, a;
   int width = h->nsamples;

   /* paint green to start */
   hex2rgba("859900", &r, &g, &b, &a);
   cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, a);
   cairo_rectangle(ui->xinfo->cairo,
         x,
         ui->xinfo->padding,
         width,
         ui->xinfo->fontpt);
   cairo_fill(ui->xinfo->cairo);

   return width;
}
