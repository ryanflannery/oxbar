#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"

void widget_battery_draw(xdraw_context_t*, settings_t*, battery_info_t*);
void widget_volume_draw(xdraw_context_t*, settings_t*, volume_info_t*);
void widget_nprocs_draw(xdraw_context_t*, settings_t*, nprocs_info_t*);
void widget_memory_draw(xdraw_context_t*, settings_t*, memory_info_t*);
void widget_cpus_draw(xdraw_context_t*, settings_t*, cpus_t*);
void widget_net_draw(xdraw_context_t*, settings_t*, net_info_t*);
void widget_time_draw(xdraw_context_t*, settings_t*);

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
         gui->settings->display.padding_top,
         gui->settings->display.bgcolor,
         gui->settings->display.font);

   /* initial context */
   gui->xcontext = xdraw_context_init(gui->xinfo, L2R);

   return gui;
}

void
gui_free(gui_t *gui)
{
   xdraw_context_free(gui->xcontext);
   xcore_free(gui->xinfo);
   free(gui);
}

static void
draw_headerline(
      xdraw_context_t *ctx,
      const char      *color,
      double           start)
{
   xdraw_hline(ctx, color, ctx->xinfo->padding, start, ctx->xoffset);
   xdraw_advance_offsets(ctx, BEFORE_RENDER, 15, 0);/* TODO => widget_spacing */
   xdraw_advance_offsets(ctx, AFTER_RENDER, 15, 0); /* TODO => widget_spacing */
}

void
gui_draw(gui_t *gui)
{
   /* TODO remove this state */
   static xdraw_context_t *l2r = NULL;
   static xdraw_context_t *r2l = NULL;

   if (NULL == r2l)
      r2l = xdraw_context_init(gui->xinfo, R2L);

   if (NULL == l2r)
      l2r = xdraw_context_init(gui->xinfo, L2R);

   double startx;

   xcore_clear(gui->xinfo);
   xdraw_context_reset_offsets(l2r);
   xdraw_context_reset_offsets(r2l);

   if (BATTERY.is_setup) {
      startx = l2r->xoffset;
      widget_battery_draw(l2r, gui->settings, &BATTERY);
      draw_headerline(l2r, gui->settings->battery.hdcolor, startx);
   }

   if (VOLUME.is_setup) {
      startx = l2r->xoffset;
      widget_volume_draw(l2r, gui->settings, &VOLUME);
      draw_headerline(l2r, gui->settings->volume.hdcolor, startx);
   }

   if (NPROCS.is_setup) {
      startx = l2r->xoffset;
      widget_nprocs_draw(l2r, gui->settings, &NPROCS);
      draw_headerline(l2r, gui->settings->nprocs.hdcolor, startx);
   }

   if (MEMORY.is_setup) {
      startx = l2r->xoffset;
      widget_memory_draw(l2r, gui->settings, &MEMORY);
      draw_headerline(l2r, gui->settings->memory.hdcolor, startx);
   }

   if (CPUS.is_setup) {
      startx = l2r->xoffset;
      widget_cpus_draw(l2r, gui->settings, &CPUS);
      draw_headerline(l2r, gui->settings->cpus.hdcolor, startx);
   }

   if (NET.is_setup) {
      startx = l2r->xoffset;
      widget_net_draw(l2r, gui->settings, &NET);
      draw_headerline(l2r, gui->settings->network.hdcolor, startx);
   }

   startx = r2l->xoffset;
   widget_time_draw(r2l, gui->settings);
   draw_headerline(r2l, gui->settings->time.hdcolor, startx);


   xcore_flush(gui->xinfo);

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
widget_battery_draw(
      xdraw_context_t   *context,
      settings_t        *settings,
      battery_info_t    *battery)
{
   xdraw_printf(
         context,
         battery->plugged_in ?
            settings->display.fgcolor :
            settings->battery.fgcolor_unplugged ,
         battery->plugged_in ? "AC " : "BAT ");

   xdraw_progress_bar(
         context,
         settings->battery.chart_bgcolor,
         settings->battery.chart_pgcolor,
         settings->battery.chart_width,
         battery->charge_pct);

   xdraw_printf(
         context,
         settings->display.fgcolor,
         "% 3.0f%%", battery->charge_pct);

   if (-1 != battery->minutes_remaining) {
      xdraw_printf(
            context,
            settings->display.fgcolor,
            " %dh %dm",
               battery->minutes_remaining / 60,
               battery->minutes_remaining % 60);
   }
}

void
widget_volume_draw(
      xdraw_context_t   *context,
      settings_t        *settings,
      volume_info_t     *volume)
{
   xdraw_printf(
         context,
         settings->display.fgcolor,
         "Volume: ");

   /* TODO Should volume widget ever handle this case!? I've never had it */
   if (volume->left_pct != volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal",
            __FUNCTION__);

   xdraw_progress_bar(
         context,
         settings->volume.chart_bgcolor,
         settings->volume.chart_pgcolor,
         settings->volume.chart_width,
         volume->left_pct);

   xdraw_printf(
         context,
         settings->display.fgcolor,
         "% 3.0f%%", volume->left_pct);
}

void
widget_nprocs_draw(
      xdraw_context_t   *context,
      settings_t        *settings,
      nprocs_info_t     *nprocs)
{
   xdraw_printf(
         context,
         settings->display.fgcolor,
         "#Procs: %d", nprocs->nprocs);
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

   ret = snprintf(buffer + ret, GUI_FMT_MEMORY_BUFFER_MAXLEN- ret, "%s",
         suffixes[step]);
   if (0 > ret)
      err(1, "%s: snprintf failed for %s", __FUNCTION__, suffixes[step]);

   return buffer;
}

void
widget_memory_draw(
      xdraw_context_t   *context,
      settings_t        *settings,
      memory_info_t     *memory)
{
   const char *colors[] = {
      settings->memory.chart_color_active,
      settings->memory.chart_color_total,
      settings->memory.chart_color_free
   };

   static chart_t *chart = NULL;
   if (NULL == chart)
      chart = chart_init(60, 3, true, settings->memory.chart_bgcolor, colors);

   chart_update(chart, (double[]) {
         memory->active_pct,
         memory->total_pct,
         memory->free_pct
         });

   xdraw_printf(context, settings->display.fgcolor, "Memory: ");
   xdraw_chart(context, chart);
   xdraw_printf(context, settings->memory.chart_color_active, " %s",
         fmt_memory("%.1lf", memory->active));
   xdraw_printf(context, settings->display.fgcolor, " active ");
   xdraw_printf(context, settings->memory.chart_color_total, "%s",
         fmt_memory("%.1lf", memory->total));
   xdraw_printf(context, settings->display.fgcolor, " total ");
   xdraw_printf(context, settings->memory.chart_color_free,
         fmt_memory("%.1lf", memory->free));
   xdraw_printf(context, settings->display.fgcolor, " free");
}

void
widget_cpus_draw(
      xdraw_context_t   *context,
      settings_t        *settings,
      cpus_t            *cpus)
{
   const char *colors[] = {
      settings->cpus.chart_color_interrupt,
      settings->cpus.chart_color_nice,
      settings->cpus.chart_color_sys,
      settings->cpus.chart_color_user,
      settings->cpus.chart_color_idle
   };

   int i;

   static chart_t **charts = NULL;
   if (NULL == charts) {
      charts = calloc(cpus->ncpu, sizeof(chart_t*));
      if (NULL == charts)
         err(1, "%s: calloc charts failed", __FUNCTION__);

      for (i = 0; i < cpus->ncpu; i++)
         charts[i] = chart_init(60, CPUSTATES, true,
               settings->cpus.chart_bgcolor, colors);
   }


   xdraw_printf(context, settings->display.fgcolor, "CPUs: ");
   for (i = 0; i < cpus->ncpu; i++) {
      chart_update(charts[i], (double[]) {
            cpus->cpus[i].percentages[CP_INTR],
            cpus->cpus[i].percentages[CP_NICE],
            cpus->cpus[i].percentages[CP_SYS],
            cpus->cpus[i].percentages[CP_USER],
            cpus->cpus[i].percentages[CP_IDLE]
            });
      xdraw_chart(context, charts[i]);
      xdraw_printf(context, settings->display.fgcolor, "% 3.0f%%",
            cpus->cpus[i].percentages[CP_IDLE]);
      if (i != cpus->ncpu - 1) xdraw_printf(context, "000000", " ");
   }
}

void
widget_net_draw(
      xdraw_context_t   *context,
      settings_t        *settings,
      net_info_t        *net)
{
   const char *colors_in[] = {
      settings->network.inbound_chart_color_pgcolor
   };
   const char *colors_out[] = {
      settings->network.outbound_chart_color_pgcolor
   };
   char *bgcolor_in  = settings->network.inbound_chart_color_bgcolor;
   char *bgcolor_out = settings->network.outbound_chart_color_bgcolor;

   static chart_t *chart_in  = NULL;
   static chart_t *chart_out = NULL;
   if (NULL == chart_in || NULL == chart_out) {
      chart_in  = chart_init(60, 1, false, bgcolor_in,  colors_in);
      chart_out = chart_init(60, 1, false, bgcolor_out, colors_out);
   }

   chart_update(chart_in,  (double[]){ net->new_bytes_in });
   chart_update(chart_out, (double[]){ net->new_bytes_out });

   xdraw_printf(context, settings->display.fgcolor, "Network: ");
   xdraw_chart(context, chart_in);
   xdraw_printf(context, "268bd2", " %s ",
         fmt_memory("% .0f", net->new_bytes_in / 1000));
   xdraw_chart(context, chart_out);
   xdraw_printf(context, "dc322f", " %s",
         fmt_memory("% .0f", net->new_bytes_out / 1000));
}

void
widget_time_draw(
      xdraw_context_t   *context,
      settings_t        *settings)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, "%a %d %b %Y  %I:%M:%S %p",
         localtime(&now));

   xdraw_printf(
         context,
         settings->display.fgcolor,
         buffer);
}
