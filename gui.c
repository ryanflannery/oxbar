#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"
#include "gui/histogram.h"

oxbarui_t*
ui_create(settings_t *s)
{
   oxbarui_t *ui = malloc(sizeof(oxbarui_t));
   if (NULL == ui)
      err(1, "%s: couldn't malloc ui", __FUNCTION__);

   ui->xinfo = malloc(sizeof(xinfo_t));
   if (NULL == ui->xinfo)
      err(1, "%s: couldn't malloc xinfo", __FUNCTION__);

   /* These need to be done in a specific order */
   xcore_setup_x_connection_screen_visual(ui->xinfo);

   /* TODO this would be lovely to support again. i need to figure out how
    * to query pango, for a loaded font description, for the font size.
   if (-1 == height)
      height = (uint32_t)(ceil(fontpt + (2 * padding)));
    */

   if (-1 == s->display.y)
      s->display.y = ui->xinfo->display_height - s->display.h ;

   if (-1 == s->display.w)
      s->display.w = ui->xinfo->display_width;

   ui->xinfo->padding = s->display.top_padding;

   xcore_setup_x_window(
         ui->xinfo,
         s->display.wmname,
         s->display.x, s->display.y,
         s->display.w, s->display.h);

   xcore_setup_x_wm_hints(ui->xinfo);
   xcore_setup_cairo(ui->xinfo);
   xcore_setup_xfont(ui->xinfo, s->display.font);

   /* now map the window & do an initial paint */
   xcb_map_window(ui->xinfo->xcon, ui->xinfo->xwindow);

   ui->settings = s;

   /* TODO remove these once migrated over to settings */
   ui->widget_padding = ui->settings->display.widget_padding;
   ui->fgcolor = strdup(ui->settings->display.fgcolor);
   ui->bgcolor = strdup(ui->settings->display.bgcolor);
   /* end TODO */
   ui->small_space = xdraw_text(ui->xinfo, NULL, 0, 0, " ");

   return ui;
}

void
ui_destroy(oxbarui_t *ui)
{
   free(ui->bgcolor);
   free(ui->fgcolor);
   xcore_destroy(ui->xinfo);
   free(ui->xinfo);
   free(ui);
}

void
ui_clear(oxbarui_t *ui)
{
   xdraw_clear_all(ui->xinfo, ui->settings->display.bgcolor);
   ui->xcurrent = ui->xinfo->padding;
}

void
ui_flush(oxbarui_t *ui)
{
   xdraw_flush(ui->xinfo);
}

void
ui_widget_battery_draw(
      oxbarui_t      *ui,
      battery_info_t *battery)
{
   static const char *colors[] = {
      "dc322f",
      "859900"
   };

   double startx = ui->xcurrent;

   const char *fgcolor = ui->settings->display.fgcolor;
   if (!battery->plugged_in && NULL != ui->settings->battery.color_unplugged)
      fgcolor = ui->settings->battery.color_unplugged;

   if (NULL != ui->settings->battery.chart_color_remaining)
      colors[0] = ui->settings->battery.chart_color_remaining;

   if (NULL != ui->settings->battery.chart_color_power)
      colors[1] = ui->settings->battery.chart_color_power;

   ui->xcurrent += xdraw_text(
         ui->xinfo,
         fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         battery->plugged_in ? "AC " : "BAT ");
   ui->xcurrent += xdraw_vertical_stack(
         ui->xinfo,
         ui->xcurrent,
         ui->settings->battery.chart_width,
         2, colors,
         (double[]){100.0 - battery->charge_pct, battery->charge_pct});
   /*ui->xcurrent += ui->small_space;*/
   ui->xcurrent += xdraw_percent(
         ui->xinfo,
         ui->settings->display.fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         battery->charge_pct);

   if (-1 != battery->minutes_remaining) {
      /*ui->xcurrent += ui->small_space;*/
      ui->xcurrent += xdraw_timespan(
            ui->xinfo,
            ui->settings->display.fgcolor,
            ui->xcurrent,
            ui->xinfo->padding,
            battery->minutes_remaining);
   }
   xdraw_hline(ui->xinfo, ui->settings->battery.hdcolor, ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->settings->display.widget_padding;
}

void
ui_widget_volume_draw(
      oxbarui_t      *ui,
      volume_info_t  *volume)
{
   static const char *colors[] = {
      "dc322f",
      "859900"
   };
   double startx = ui->xcurrent;

   ui->xcurrent += xdraw_text(
         ui->xinfo,
         ui->fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         "VOLUME: ");
   if (volume->left_pct == volume->right_pct) {
      ui->xcurrent += xdraw_vertical_stack(ui->xinfo, ui->xcurrent, 7, 2,
            colors,
            (double[]){100.0 - volume->left_pct, volume->left_pct});
      /*ui->xcurrent += ui->small_space;*/
      ui->xcurrent += xdraw_percent(
            ui->xinfo,
            ui->fgcolor,
            ui->xcurrent,
            ui->xinfo->padding,
            volume->left_pct);
   } else {
      ui->xcurrent += xdraw_percent(
            ui->xinfo,
            ui->fgcolor,
            ui->xcurrent,
            ui->xinfo->padding,
            volume->left_pct);
      /*ui->xcurrent += ui->small_space;*/
      ui->xcurrent += xdraw_percent(
            ui->xinfo,
            ui->fgcolor,
            ui->xcurrent,
            ui->xinfo->padding,
            volume->right_pct);
   }
   xdraw_hline(ui->xinfo, "cb4b16b2", ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->widget_padding;
}

void
ui_widget_nprocs_draw(
      oxbarui_t      *ui,
      nprocs_info_t  *nprocs)
{
   double startx = ui->xcurrent;

   ui->xcurrent += xdraw_text(
         ui->xinfo,
         ui->fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         "#PROCS: ");
   ui->xcurrent += xdraw_int(
         ui->xinfo,
         ui->fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         nprocs->nprocs);
   xdraw_hline(ui->xinfo, "dc322fb2", ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->widget_padding;
}

void
ui_widget_memory_draw(
      oxbarui_t      *ui,
      memory_info_t  *memory)
{
   static const char *colors[] = {
      "859900",
      "bbbb00",
      "dc322f"
   };
   double startx = ui->xcurrent;

   static histogram_t *histogram = NULL;
   if (NULL == histogram)
      histogram = histogram_init(60, 3);

   histogram_update(histogram, (double[]) {
         memory->free_pct,
         memory->total_pct,
         memory->active_pct
         });

   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "MEMORY: ");
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_histogram(ui->xinfo, ui->xcurrent, colors, histogram);
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_memory(ui->xinfo, "dc322f", ui->xcurrent, ui->xinfo->padding, memory->active);
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "active");
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_memory(ui->xinfo, "b58900", ui->xcurrent, ui->xinfo->padding, memory->total);
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "total");
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_memory(ui->xinfo, "859900", ui->xcurrent, ui->xinfo->padding, memory->free);
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "free");
   xdraw_hline(ui->xinfo, "d33682b2", ui->xinfo->padding, startx, ui->xcurrent);

   ui->xcurrent += ui->widget_padding;
}

void
ui_widget_cpus_draw(
      oxbarui_t  *ui,
      cpus_t     *cpus)
{
   static const char *colors[] = {
      "859900",   /* idle */
      "d33682",   /* interrupt */
      "b58900",   /* sys  */
      "2aa198",   /* nice */
      "dc322f"    /* user */
   };
   double startx = ui->xcurrent;
   int i;

   static histogram_t **hist_cpu = NULL;
   if (NULL == hist_cpu) {
      hist_cpu = calloc(cpus->ncpu, sizeof(histogram_t*));
      if (NULL == hist_cpu)
         err(1, "%s: calloc hist_cpu failed", __FUNCTION__);

      for (i = 0; i < cpus->ncpu; i++)
         hist_cpu[i] = histogram_init(60, CPUSTATES);
   }

   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "CPUS: ");
   for (i = 0; i < cpus->ncpu; i++) {
      histogram_update(hist_cpu[i], (double[]) {
            cpus->cpus[i].percentages[CP_IDLE],
            cpus->cpus[i].percentages[CP_INTR],
            cpus->cpus[i].percentages[CP_SYS],
            cpus->cpus[i].percentages[CP_NICE],
            cpus->cpus[i].percentages[CP_USER]
            });
      ui->xcurrent += xdraw_histogram(ui->xinfo, ui->xcurrent, colors, hist_cpu[i]);
      /*ui->xcurrent += ui->small_space;*/
      ui->xcurrent += xdraw_percent(ui->xinfo, ui->fgcolor, ui->xcurrent,
            ui->xinfo->padding,
            CPUS.cpus[i].percentages[CP_IDLE]);
      ui->xcurrent += ui->small_space;
   }
   xdraw_hline(ui->xinfo, "6c71c4b2", ui->xinfo->padding, startx, ui->xcurrent);

   ui->xcurrent += ui->widget_padding;
}

void
ui_widget_time_draw(
      oxbarui_t  *ui)
{
   static const size_t OXBAR_STR_MAX_TIME_LEN = 100;
   static char buffer[OXBAR_STR_MAX_TIME_LEN];

   time_t now = time(NULL);
   strftime(buffer, OXBAR_STR_MAX_TIME_LEN, "%a %d %b %Y %I:%M:%S %p", localtime(&now));
   int width = xdraw_text_right_aligned(
         ui->xinfo,
         ui->fgcolor,
         ui->xinfo->display_width,
         ui->xinfo->padding,
         buffer);

   xdraw_hline(ui->xinfo, "268bd2b2", ui->xinfo->padding,
         ui->xinfo->display_width - width, ui->xinfo->display_width);
}
