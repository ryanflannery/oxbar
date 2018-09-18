#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"
#include "gui/histogram.h"

oxbarui_t*
ui_init(settings_t *s)
{
   oxbarui_t *ui = malloc(sizeof(oxbarui_t));
   if (NULL == ui)
      err(1, "%s: couldn't malloc ui", __FUNCTION__);

   ui->settings = s;
   ui->xinfo = xcore_init(
         ui->settings->display.wmname,
         ui->settings->display.x, ui->settings->display.y,
         ui->settings->display.w, ui->settings->display.h,
         ui->settings->display.top_padding,
         ui->settings->display.bgcolor,
         ui->settings->display.font);

   /* xcore_init can mutate some settings */
   ui->settings->display.y = ui->xinfo->y;
   ui->settings->display.w = ui->xinfo->w;

   /* initial context */
   ui->xcontext = xdraw_context_init(ui->xinfo);
   xdraw_printf(ui->xcontext, NULL, " ");    /* TODO this now actually draws */
   ui->space_width = ui->xcontext->xoffset;

   return ui;
}

void
ui_free(oxbarui_t *ui)
{
   xcore_free(ui->xinfo);
   free(ui);
}

void
ui_clear(oxbarui_t *ui)
{
   xdraw_clear(ui->xcontext);
}

void
ui_flush(oxbarui_t *ui)
{
   xdraw_flush(ui->xcontext);
}

void
ui_draw(oxbarui_t *ui)
{
   ui_clear(ui);

   if (BATTERY.is_setup)
      ui_widget_battery_draw(ui, &BATTERY);

   if (VOLUME.is_setup)
      ui_widget_volume_draw(ui, &VOLUME);

   if (NPROCS.is_setup)
      ui_widget_nprocs_draw(ui, &NPROCS);

   if (MEMORY.is_setup)
      ui_widget_memory_draw(ui, &MEMORY);

   if (CPUS.is_setup)
      ui_widget_cpus_draw(ui, &CPUS);

   if (NET.is_setup)
      ui_widget_net_draw(ui, &NET);

   ui_widget_time_draw(ui);

   ui_flush(ui);
}

/* TODO Remove state From ui_widget_*_draw(...) components
 * So currently, *most* ui_widget_*_draw() routines are stateless: they take
 * in the ui object and the relevant stat\/.* component(s) and render them.
 * The ones that need to draw history (say for histograms or timeseries),
 * however, store a static local histogram initialized on the first call.
 * I've debated moving each of these into their own object.
 *    1. Making proper widget objects has an appeal - there are properties and
 *       logic seperate to each, likely with a common (enough) interface to
 *       abstract.
 *    2. On the other hand, in the case of oxbar, I will *NEVER* have two of
 *       the same widgets (I can't see a scenario for this). So it seems
 *       wasteful to create a bunch of objects whose sole purpose to maintain
 *       state that could otherwise be done simply like below.
 *    3. Some other yet-to-be-determined, and hopefully elegant, solution.
 */

void
ui_widget_battery_draw(
      oxbarui_t      *ui,
      battery_info_t *battery)
{
   const char *colors[] = {
      ui->settings->battery.chart_color_bgcolor,
      ui->settings->battery.chart_color_pgcolor
   };

   double startx = ui->xcontext->xoffset;

   xdraw_printf(
         ui->xcontext,
         battery->plugged_in ?
            ui->settings->display.fgcolor :
            ui->settings->battery.fgcolor_unplugged ,
         battery->plugged_in ? "AC " : "BAT ");
   xdraw_vertical_stack(
         ui->xcontext,
         ui->xcontext->xoffset,
         ui->settings->battery.chart_width,
         2, colors,
         (double[]){100.0 - battery->charge_pct, battery->charge_pct});
   ui->xcontext->xoffset += ui->settings->battery.chart_width;
   xdraw_printf(
         ui->xcontext,
         ui->settings->display.fgcolor,
         "% 3.0f%%", battery->charge_pct);

   if (-1 != battery->minutes_remaining) {
      xdraw_printf(
            ui->xcontext,
            ui->settings->display.fgcolor,
            " %dh %dm",
               battery->minutes_remaining / 60,
               battery->minutes_remaining % 60);
   }

   xdraw_hline(ui->xinfo, ui->settings->battery.hdcolor, ui->xinfo->padding, startx, ui->xcontext->xoffset);
   ui->xcontext->xoffset += ui->settings->display.widget_padding;
}

void
ui_widget_volume_draw(
      oxbarui_t      *ui,
      volume_info_t  *volume)
{
   double startx = ui->xcontext->xoffset;

   xdraw_printf(
         ui->xcontext,
         ui->settings->display.fgcolor,
         "Volume: ");

   /* TODO Should volume widget ever handle this case!?
    * I used to render seperate bars & percentages for each. I don't think
    * I've ever used that though in...10 years!?
    * I'm going to skip doing it in oxbar and just warn here.
    */
   if (volume->left_pct != volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal", __FUNCTION__);

   xdraw_vertical_stack(
         ui->xcontext,
         ui->xcontext->xoffset,
         ui->settings->volume.chart_width,
         2,
         (const char *[]){
            ui->settings->volume.chart_pgcolor,
            ui->settings->volume.chart_bgcolor
         },
         (double[]){
            100.0 - volume->left_pct,
            volume->left_pct
         });

   ui->xcontext->xoffset += ui->settings->volume.chart_width;
   xdraw_printf(
         ui->xcontext,
         ui->settings->display.fgcolor,
         "% 3.0f%%", volume->left_pct);

   xdraw_hline(ui->xinfo, ui->settings->volume.hdcolor, ui->xinfo->padding, startx, ui->xcontext->xoffset);
   ui->xcontext->xoffset += ui->settings->display.widget_padding;
}

void
ui_widget_nprocs_draw(
      oxbarui_t      *ui,
      nprocs_info_t  *nprocs)
{
   double startx = ui->xcontext->xoffset;

   xdraw_printf(
         ui->xcontext,
         ui->settings->display.fgcolor,
         "#Procs: %d", nprocs->nprocs);

   xdraw_hline(ui->xinfo, ui->settings->nprocs.hdcolor, ui->xinfo->padding, startx, ui->xcontext->xoffset);
   ui->xcontext->xoffset += ui->settings->display.widget_padding;
}

const char *
fmt_memory(const char *fmt, int kbytes)
{
#define GUI_FMT_MEMORY_BUFFER_MAXLEN 100
   static char          buffer[GUI_FMT_MEMORY_BUFFER_MAXLEN];
   static const char   *suffixes[] = { "k", "M", "G", "T", "P" };
   static const size_t  snum       = sizeof(suffixes) / sizeof(suffixes[0]);

   double dbytes = (double) kbytes;
   size_t step   = 0;
   int    ret;

   if (1024 < kbytes)
      for (step = 0; (kbytes / 1024) > 0 && step < snum; step++, kbytes /= 1024)
         dbytes = kbytes / 1024.0;

   ret = snprintf(buffer, GUI_FMT_MEMORY_BUFFER_MAXLEN, fmt, dbytes);
   if (0 > ret)
      err(1, "%s: snprintf failed for %d", __FUNCTION__, kbytes);

   ret = snprintf(buffer + ret, GUI_FMT_MEMORY_BUFFER_MAXLEN- ret, "%s", suffixes[step]);
   if (0 > ret)
      err(1, "%s: snprintf failed for %s", __FUNCTION__, suffixes[step]);

   return buffer;
}

void
ui_widget_memory_draw(
      oxbarui_t      *ui,
      memory_info_t  *memory)
{
   const char *colors[] = {
      ui->settings->memory.chart_color_free,
      ui->settings->memory.chart_color_total,
      ui->settings->memory.chart_color_active
   };
   double startx = ui->xcontext->xoffset;

   static histogram_t *histogram = NULL;
   if (NULL == histogram)
      histogram = histogram_init(60, 3);

   histogram_update(histogram, (double[]) {
         memory->free_pct,
         memory->total_pct,
         memory->active_pct
         });

   xdraw_printf(ui->xcontext, ui->settings->display.fgcolor, "Memory: ");
   xdraw_histogram(ui->xcontext, colors, histogram);
   xdraw_printf(ui->xcontext, "dc322f", " %s", fmt_memory("%.1lf", memory->active));
   xdraw_printf(ui->xcontext, ui->settings->display.fgcolor, " active ");
   xdraw_printf(ui->xcontext, "b58900", "%s", fmt_memory("%.1lf", memory->total));
   xdraw_printf(ui->xcontext, ui->settings->display.fgcolor, " total ");
   xdraw_printf(ui->xcontext, "859900", fmt_memory("%.1lf", memory->free));
   xdraw_printf(ui->xcontext, ui->settings->display.fgcolor, " free");

   xdraw_hline(ui->xinfo, ui->settings->memory.hdcolor, ui->xinfo->padding, startx, ui->xcontext->xoffset);
   ui->xcontext->xoffset += ui->settings->display.widget_padding;
}

void
ui_widget_cpus_draw(
      oxbarui_t  *ui,
      cpus_t     *cpus)
{
   const char *colors[] = {
      ui->settings->cpus.chart_color_idle,
      ui->settings->cpus.chart_color_user,
      ui->settings->cpus.chart_color_sys,
      ui->settings->cpus.chart_color_nice,
      ui->settings->cpus.chart_color_interrupt
   };
   double startx = ui->xcontext->xoffset;
   int i;

   static histogram_t **hist_cpu = NULL;
   if (NULL == hist_cpu) {
      hist_cpu = calloc(cpus->ncpu, sizeof(histogram_t*));
      if (NULL == hist_cpu)
         err(1, "%s: calloc hist_cpu failed", __FUNCTION__);

      for (i = 0; i < cpus->ncpu; i++)
         hist_cpu[i] = histogram_init(60, CPUSTATES);
   }

   xdraw_printf(ui->xcontext, ui->settings->display.fgcolor, "CPUs: ");
   for (i = 0; i < cpus->ncpu; i++) {
      histogram_update(hist_cpu[i], (double[]) {
            cpus->cpus[i].percentages[CP_IDLE],
            cpus->cpus[i].percentages[CP_USER],
            cpus->cpus[i].percentages[CP_SYS],
            cpus->cpus[i].percentages[CP_NICE],
            cpus->cpus[i].percentages[CP_INTR]
            });
      xdraw_histogram(ui->xcontext, colors, hist_cpu[i]);
      xdraw_printf(ui->xcontext, ui->settings->display.fgcolor, "% 3.0f%%", CPUS.cpus[i].percentages[CP_IDLE]);
      ui->xcontext->xoffset += ui->space_width;
   }

   xdraw_hline(ui->xinfo, ui->settings->cpus.hdcolor, ui->xinfo->padding, startx, ui->xcontext->xoffset);
   ui->xcontext->xoffset += ui->settings->display.widget_padding;
}

void
ui_widget_net_draw(
      oxbarui_t  *ui,
      net_info_t *net)
{
   const char *colors_in[] = {
      ui->settings->network.inbound_chart_color_bgcolor,
      ui->settings->network.inbound_chart_color_pgcolor
   };
   const char *colors_out[] = {
      ui->settings->network.outbound_chart_color_bgcolor,
      ui->settings->network.outbound_chart_color_pgcolor
   };

   static tseries_t *bytes_in  = NULL;
   static tseries_t *bytes_out = NULL;

   if (NULL == bytes_in || NULL == bytes_out) {
      bytes_in  = tseries_init(60);
      bytes_out = tseries_init(60);
   }

   tseries_update(bytes_in,  net->new_bytes_in);
   tseries_update(bytes_out, net->new_bytes_out);

   double startx = ui->xcontext->xoffset;

   xdraw_printf(ui->xcontext, ui->settings->display.fgcolor, "Network: ");
   xdraw_series(ui->xcontext, colors_in, bytes_in);
   ui->xcontext->xoffset += ui->space_width;
   xdraw_printf(ui->xcontext, "268bd2", "%s", fmt_memory("% 4.0f", net->new_bytes_in / 1000));
   ui->xcontext->xoffset += ui->space_width;
   xdraw_series(ui->xcontext, colors_out, bytes_out);
   ui->xcontext->xoffset += ui->space_width;
   xdraw_printf(ui->xcontext, "dc322f", "%s", fmt_memory("% 4.0f", net->new_bytes_out / 1000));

   xdraw_hline(ui->xinfo, ui->settings->network.hdcolor, ui->xinfo->padding, startx, ui->xcontext->xoffset);
   ui->xcontext->xoffset += ui->settings->display.widget_padding;
}

void
ui_widget_time_draw(
      oxbarui_t  *ui)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, "%a %d %b %Y  %I:%M:%S %p", localtime(&now));

   xdraw_context_t newctx;
   newctx.xinfo = ui->xcontext->xinfo;
   newctx.xoffset = ui->xcontext->xinfo->w;
   newctx.yoffset = ui->xcontext->yoffset;

   double startx = newctx.xoffset;
   xdraw_text_right_aligned(
         &newctx,
         ui->settings->display.fgcolor,
         buffer);

   xdraw_hline(ui->xinfo, "859900", ui->xinfo->padding,
         newctx.xoffset, startx);
}
