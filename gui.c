#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"
#include "gui/histogram.h"

void widget_battery_draw(gui_t *gui, battery_info_t *battery);
void widget_volume_draw(gui_t *gui, volume_info_t *volume);
void widget_nprocs_draw(gui_t *gui, nprocs_info_t *nprocs);
void widget_memory_draw(gui_t *gui, memory_info_t *memory);
void widget_cpus_draw(gui_t *gui, cpus_t *cpus);
void widget_net_draw(gui_t *gui, net_info_t *net);
void widget_time_draw(gui_t *gui);

gui_t*
gui_init(settings_t *s)
{
   gui_t *gui = malloc(sizeof(gui_t));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->settings = s;
   gui->xinfo = xcore_init(
         gui->settings->display.wmname,
         gui->settings->display.x, gui->settings->display.y,
         gui->settings->display.w, gui->settings->display.h,
         gui->settings->display.top_padding,
         gui->settings->display.bgcolor,
         gui->settings->display.font);

   /* xcore_init can mutate some settings */
   gui->settings->display.y = gui->xinfo->y;
   gui->settings->display.w = gui->xinfo->w;

   /* initial context */
   gui->xcontext = xdraw_context_init(gui->xinfo);

   return gui;
}

void
gui_free(gui_t *gui)
{
   xdraw_context_free(gui->xcontext);
   xcore_free(gui->xinfo);
   free(gui);
}

void
gui_draw(gui_t *gui)
{
   xdraw_clear(gui->xcontext);

   if (BATTERY.is_setup)
      widget_battery_draw(gui, &BATTERY);

   if (VOLUME.is_setup)
      widget_volume_draw(gui, &VOLUME);

   if (NPROCS.is_setup)
      widget_nprocs_draw(gui, &NPROCS);

   if (MEMORY.is_setup)
      widget_memory_draw(gui, &MEMORY);

   if (CPUS.is_setup)
      widget_cpus_draw(gui, &CPUS);

   if (NET.is_setup)
      widget_net_draw(gui, &NET);

   widget_time_draw(gui);

   xdraw_flush(gui->xcontext);
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

/* TODO Reduce widget boilerplate
 * Currently, every widget needs to follow the below pattern to render
 * properly:
      void
      ui_widget_FOO_draw(
            gui_t      *ui,
            FOO_info_t  *nprocs)
      {
         double startx = ui->xcontext->xoffset;

         ... xdraw_stuff() ...

         xdraw_hline(ui->xinfo, ui->settings->FOO.hdcolor, ui->xinfo->padding, startx, ui->xcontext->xoffset);
         ui->xcontext->xoffset += ui->settings->display.widget_padding;
      }
 * This is cumbersome and error prone. If I move widgets to their own section,
 * I could enforce the boilerplate elsewhere (in gui.*, while widgets live
 * elsewhere).
 */

void
widget_battery_draw(
      gui_t          *gui,
      battery_info_t *battery)
{
   double startx = gui->xcontext->xoffset;

   xdraw_printf(
         gui->xcontext,
         battery->plugged_in ?
            gui->settings->display.fgcolor :
            gui->settings->battery.fgcolor_unplugged ,
         battery->plugged_in ? "AC " : "BAT ");

   xdraw_progress_bar(
         gui->xcontext,
         gui->settings->battery.chart_bgcolor,
         gui->settings->battery.chart_pgcolor,
         gui->settings->battery.chart_width,
         battery->charge_pct);

   xdraw_printf(
         gui->xcontext,
         gui->settings->display.fgcolor,
         "% 3.0f%%", battery->charge_pct);

   if (-1 != battery->minutes_remaining) {
      xdraw_printf(
            gui->xcontext,
            gui->settings->display.fgcolor,
            " %dh %dm",
               battery->minutes_remaining / 60,
               battery->minutes_remaining % 60);
   }

   xdraw_hline(gui->xinfo, gui->settings->battery.hdcolor, gui->xinfo->padding, startx, gui->xcontext->xoffset);
   gui->xcontext->xoffset += gui->settings->display.widget_padding;
}

void
widget_volume_draw(
      gui_t         *gui,
      volume_info_t *volume)
{
   double startx = gui->xcontext->xoffset;

   xdraw_printf(
         gui->xcontext,
         gui->settings->display.fgcolor,
         "Volume: ");

   /* TODO Should volume widget ever handle this case!?
    * I used to render seperate bars & percentages for each. I don't think
    * I've ever used that though in...10 years!?
    * I'm going to skip doing it in oxbar and just warn here.
    */
   if (volume->left_pct != volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal", __FUNCTION__);

   xdraw_progress_bar(
         gui->xcontext,
         gui->settings->volume.chart_bgcolor,
         gui->settings->volume.chart_pgcolor,
         gui->settings->volume.chart_width,
         volume->left_pct);

   xdraw_printf(
         gui->xcontext,
         gui->settings->display.fgcolor,
         "% 3.0f%%", volume->left_pct);

   xdraw_hline(gui->xinfo, gui->settings->volume.hdcolor, gui->xinfo->padding, startx, gui->xcontext->xoffset);
   gui->xcontext->xoffset += gui->settings->display.widget_padding;
}

void
widget_nprocs_draw(
      gui_t         *gui,
      nprocs_info_t *nprocs)
{
   double startx = gui->xcontext->xoffset;

   xdraw_printf(
         gui->xcontext,
         gui->settings->display.fgcolor,
         "#Procs: %d", nprocs->nprocs);

   xdraw_hline(gui->xinfo, gui->settings->nprocs.hdcolor, gui->xinfo->padding, startx, gui->xcontext->xoffset);
   gui->xcontext->xoffset += gui->settings->display.widget_padding;
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
widget_memory_draw(
      gui_t          *gui,
      memory_info_t  *memory)
{
   const char *colors[] = {
      gui->settings->memory.chart_color_free,
      gui->settings->memory.chart_color_total,
      gui->settings->memory.chart_color_active
   };
   double startx = gui->xcontext->xoffset;

   static histogram_t *histogram = NULL;
   if (NULL == histogram)
      histogram = histogram_init(60, 3);

   histogram_update(histogram, (double[]) {
         memory->free_pct,
         memory->total_pct,
         memory->active_pct
         });

   xdraw_printf(gui->xcontext, gui->settings->display.fgcolor, "Memory: ");
   xdraw_histogram(gui->xcontext, colors, histogram);
   xdraw_printf(gui->xcontext, "dc322f", " %s", fmt_memory("%.1lf", memory->active));
   xdraw_printf(gui->xcontext, gui->settings->display.fgcolor, " active ");
   xdraw_printf(gui->xcontext, "b58900", "%s", fmt_memory("%.1lf", memory->total));
   xdraw_printf(gui->xcontext, gui->settings->display.fgcolor, " total ");
   xdraw_printf(gui->xcontext, "859900", fmt_memory("%.1lf", memory->free));
   xdraw_printf(gui->xcontext, gui->settings->display.fgcolor, " free");

   xdraw_hline(gui->xinfo, gui->settings->memory.hdcolor, gui->xinfo->padding, startx, gui->xcontext->xoffset);
   gui->xcontext->xoffset += gui->settings->display.widget_padding;
}

void
widget_cpus_draw(
      gui_t  *gui,
      cpus_t *cpus)
{
   const char *colors[] = {
      gui->settings->cpus.chart_color_idle,
      gui->settings->cpus.chart_color_user,
      gui->settings->cpus.chart_color_sys,
      gui->settings->cpus.chart_color_nice,
      gui->settings->cpus.chart_color_interrupt
   };
   double startx = gui->xcontext->xoffset;
   int i;

   static histogram_t **hist_cpu = NULL;
   if (NULL == hist_cpu) {
      hist_cpu = calloc(cpus->ncpu, sizeof(histogram_t*));
      if (NULL == hist_cpu)
         err(1, "%s: calloc hist_cpu failed", __FUNCTION__);

      for (i = 0; i < cpus->ncpu; i++)
         hist_cpu[i] = histogram_init(60, CPUSTATES);
   }

   xdraw_printf(gui->xcontext, gui->settings->display.fgcolor, "CPUs: ");
   for (i = 0; i < cpus->ncpu; i++) {
      histogram_update(hist_cpu[i], (double[]) {
            cpus->cpus[i].percentages[CP_IDLE],
            cpus->cpus[i].percentages[CP_USER],
            cpus->cpus[i].percentages[CP_SYS],
            cpus->cpus[i].percentages[CP_NICE],
            cpus->cpus[i].percentages[CP_INTR]
            });
      xdraw_histogram(gui->xcontext, colors, hist_cpu[i]);
      xdraw_printf(gui->xcontext, gui->settings->display.fgcolor, "% 3.0f%%", CPUS.cpus[i].percentages[CP_IDLE]);
      if (i != cpus->ncpu - 1) xdraw_printf(gui->xcontext, "000000", " ");
   }

   xdraw_hline(gui->xinfo, gui->settings->cpus.hdcolor, gui->xinfo->padding, startx, gui->xcontext->xoffset);
   gui->xcontext->xoffset += gui->settings->display.widget_padding;
}

void
widget_net_draw(
      gui_t       *gui,
      net_info_t  *net)
{
   const char *colors_in[] = {
      gui->settings->network.inbound_chart_color_bgcolor,
      gui->settings->network.inbound_chart_color_pgcolor
   };
   const char *colors_out[] = {
      gui->settings->network.outbound_chart_color_bgcolor,
      gui->settings->network.outbound_chart_color_pgcolor
   };

   static tseries_t *bytes_in  = NULL;
   static tseries_t *bytes_out = NULL;

   if (NULL == bytes_in || NULL == bytes_out) {
      bytes_in  = tseries_init(60);
      bytes_out = tseries_init(60);
   }

   tseries_update(bytes_in,  net->new_bytes_in);
   tseries_update(bytes_out, net->new_bytes_out);

   double startx = gui->xcontext->xoffset;

   xdraw_printf(gui->xcontext, gui->settings->display.fgcolor, "Network: ");
   xdraw_series(gui->xcontext, colors_in, bytes_in);
   xdraw_printf(gui->xcontext, "268bd2", " %s ", fmt_memory("% .0f", net->new_bytes_in / 1000));
   xdraw_series(gui->xcontext, colors_out, bytes_out);
   xdraw_printf(gui->xcontext, "dc322f", " %s", fmt_memory("% .0f", net->new_bytes_out / 1000));

   xdraw_hline(gui->xinfo, gui->settings->network.hdcolor, gui->xinfo->padding, startx, gui->xcontext->xoffset);
   gui->xcontext->xoffset += gui->settings->display.widget_padding;
}

void
widget_time_draw(gui_t  *gui)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, "%a %d %b %Y  %I:%M:%S %p", localtime(&now));

   xdraw_context_t newctx;
   newctx.xinfo = gui->xcontext->xinfo;
   newctx.xoffset = gui->xcontext->xinfo->w;
   newctx.yoffset = gui->xcontext->yoffset;

   double startx = newctx.xoffset;
   xdraw_text_right_aligned(
         &newctx,
         gui->settings->display.fgcolor,
         buffer);

   xdraw_hline(gui->xinfo, "859900", gui->xinfo->padding,
         newctx.xoffset, startx);
}
