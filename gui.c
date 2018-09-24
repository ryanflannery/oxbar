#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"

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

   return gui;
}

void
gui_free(gui_t *gui)
{
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
}

/* define widgets */
typedef void (*widget_t)(xdraw_context_t*, const settings_t*const, const oxstats_t*const);

static void widget_battery(xdraw_context_t*, const settings_t*const, const oxstats_t*const);
static void widget_volume(xdraw_context_t*, const settings_t*const, const oxstats_t*const);
static void widget_nprocs(xdraw_context_t*, const settings_t*const, const oxstats_t*const);
static void widget_memory(xdraw_context_t*, const settings_t*const, const oxstats_t*const);
static void widget_cpus(xdraw_context_t*, const settings_t*const, const oxstats_t*const);
static void widget_net(xdraw_context_t*, const settings_t*const, const oxstats_t*const);
static void widget_time(xdraw_context_t*, const settings_t*const, const oxstats_t*const);

/* TODO Make list of left & right aligned widgets configurale later */
static
widget_t LeftAlignedWidgets[] = {
   widget_battery,
   widget_volume,
   widget_nprocs,
   widget_memory,
   widget_cpus,
   widget_net
};
static const size_t nLeftAlignedWidgets = sizeof(LeftAlignedWidgets) / sizeof(widget_t);

static
widget_t RightAlignedWidgets[] = {
   widget_time
};
static const size_t nRightAlignedWidgets = sizeof(RightAlignedWidgets) / sizeof(widget_t);

/* generic widget render method */
static void
widget_render(
      widget_t          w,
      xdraw_context_t  *ctx,
      const settings_t *const settings,
      const oxstats_t  *const stats)
{
   (w)(ctx, settings, stats);
   xdraw_advance_offsets(ctx, BEFORE_RENDER, 15, 0);/* TODO => widget_spacing */
   xdraw_advance_offsets(ctx, AFTER_RENDER, 15, 0); /* TODO => widget_spacing */
}

/* draws all widgets */
void
gui_draw(gui_t *gui)
{
   /* TODO gui_draw: remove local state of each context */
   static xdraw_context_t *l2r = NULL;
   static xdraw_context_t *r2l = NULL;

   if (NULL == r2l)
      r2l = xdraw_context_init(gui->xinfo, R2L);

   if (NULL == l2r)
      l2r = xdraw_context_init(gui->xinfo, L2R);

   xcore_clear(gui->xinfo);
   xdraw_context_reset_offsets(l2r);
   xdraw_context_reset_offsets(r2l);

   size_t i;
   for (i = 0; i < nLeftAlignedWidgets; i++)
      widget_render(LeftAlignedWidgets[i], l2r, gui->settings, &OXSTATS);

   for (i = 0; i < nRightAlignedWidgets; i++)
      widget_render(RightAlignedWidgets[i], r2l, gui->settings, &OXSTATS);

   xcore_flush(gui->xinfo);
}

/* TODO Remove state From widget_*_draw(...) components
 * So currently, *most* widget_*_draw() routines are stateless: they take
 * in the ui object and the relevant stat\/.* component(s) and render them.
 * The ones that need to draw history (say for histograms or timeseries),
 * however, store a static local chart initialized on the first call.
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
widget_battery(
      xdraw_context_t   *context,
      const settings_t  *const settings,
      const oxstats_t   *const stats)
{
   double startx = context->xoffset;
   if (!stats->battery->is_setup)
      return;

   xdraw_printf(context,
         stats->battery->plugged_in ?
            settings->display.fgcolor :
            settings->battery.fgcolor_unplugged ,
         stats->battery->plugged_in ? "AC " : "BAT ");

   xdraw_progress_bar(context,
         settings->battery.chart_bgcolor,
         settings->battery.chart_pgcolor,
         settings->battery.chart_width,
         stats->battery->charge_pct);

   xdraw_printf(context,
         settings->display.fgcolor,
         "% 3.0f%%", stats->battery->charge_pct);

   if (-1 != stats->battery->minutes_remaining) {
      xdraw_printf(context,
            settings->display.fgcolor,
            " %dh %dm",
            stats->battery->minutes_remaining / 60,
            stats->battery->minutes_remaining % 60);
   }

   draw_headerline(context, settings->battery.hdcolor, startx);
}

void
widget_volume(
      xdraw_context_t   *context,
      const settings_t  *const settings,
      const oxstats_t   *const stats)
{
   double startx = context->xoffset;
   if (!stats->volume->is_setup)
      return;

   xdraw_printf(context,
         settings->display.fgcolor,
         "Vol: ");

   /* TODO Should volume widget ever handle this case!? I've never had it */
   if (stats->volume->left_pct != stats->volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal",
            __FUNCTION__);

   xdraw_progress_bar(context,
         settings->volume.chart_bgcolor,
         settings->volume.chart_pgcolor,
         settings->volume.chart_width,
         stats->volume->left_pct);

   xdraw_printf(context,
         settings->display.fgcolor,
         "% 3.0f%%", stats->volume->left_pct);

   draw_headerline(context, settings->volume.hdcolor, startx);
}

void
widget_nprocs(
      xdraw_context_t   *context,
      const settings_t  *const settings,
      const oxstats_t   *const stats)
{
   double startx = context->xoffset;
   if (!stats->nprocs->is_setup)
      return;

   xdraw_printf(context,
         settings->display.fgcolor,
         "#Procs: %d", stats->nprocs->nprocs);

   draw_headerline(context, settings->nprocs.hdcolor, startx);
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
widget_memory(
      xdraw_context_t   *context,
      const settings_t  *const settings,
      const oxstats_t   *const stats)
{
   double startx = context->xoffset;
   if (!stats->memory->is_setup)
      return;

   static chart_t *chart = NULL;
   if (NULL == chart) {             /* FIRST time setup (only on first call) */
      const char *colors[] = {
         settings->memory.chart_color_active,
         settings->memory.chart_color_total,
         settings->memory.chart_color_free
      };
      chart = chart_init(60, 3, true, settings->memory.chart_bgcolor, colors);
   }

   chart_update(chart, (double[]) {
         stats->memory->active_pct,
         stats->memory->total_pct,
         stats->memory->free_pct
         });

   xdraw_printf(context, settings->display.fgcolor, "Mem: ");
   xdraw_chart(context, chart);
   xdraw_printf(context, settings->memory.chart_color_active, " %s",
         fmt_memory("%.1lf", stats->memory->active));
   xdraw_printf(context, settings->display.fgcolor, " active ");
   xdraw_printf(context, settings->memory.chart_color_total, "%s",
         fmt_memory("%.1lf", stats->memory->total));
   xdraw_printf(context, settings->display.fgcolor, " total ");
   xdraw_printf(context, settings->memory.chart_color_free,
         fmt_memory("%.1lf", stats->memory->free));
   xdraw_printf(context, settings->display.fgcolor, " free");
   draw_headerline(context, settings->memory.hdcolor, startx);
}

void
widget_cpus(
      xdraw_context_t   *context,
      const settings_t  *const settings,
      const oxstats_t   *const stats)
{
   double startx = context->xoffset;
   if (!stats->cpus->is_setup)
      return;

   int i;
   static chart_t **charts = NULL;
   if (NULL == charts) {            /* FIRST time setup (only on first call) */
      const char *colors[] = {
         settings->cpus.chart_color_sys,
         settings->cpus.chart_color_interrupt,
         settings->cpus.chart_color_user,
         settings->cpus.chart_color_nice,
         settings->cpus.chart_color_spin,
         settings->cpus.chart_color_idle
      };

      charts = calloc(stats->cpus->ncpu, sizeof(chart_t*));
      if (NULL == charts)
         err(1, "%s: calloc charts failed", __FUNCTION__);

      for (i = 0; i < stats->cpus->ncpu; i++)
         charts[i] = chart_init(60, CPUSTATES, true,
               settings->cpus.chart_bgcolor, colors);
   }


   xdraw_printf(context, settings->display.fgcolor, "CPUs: ");
   for (i = 0; i < stats->cpus->ncpu; i++) {
      chart_update(charts[i], (double[]) {
            stats->cpus->cpus[i].percentages[CP_SYS],
            stats->cpus->cpus[i].percentages[CP_INTR],
            stats->cpus->cpus[i].percentages[CP_USER],
            stats->cpus->cpus[i].percentages[CP_NICE],
            stats->cpus->cpus[i].percentages[CP_SPIN],
            stats->cpus->cpus[i].percentages[CP_IDLE]
            });
      xdraw_chart(context, charts[i]);
      /* TODO revisit "cpulong" widget printing individual states
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_SYS]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_INTR]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_USER]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_NICE]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_SPIN]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_IDLE]);
      */
      xdraw_printf(context, settings->display.fgcolor, "% 3.0f%%",
            stats->cpus->cpus[i].percentages[CP_IDLE]);
      if (i != stats->cpus->ncpu - 1) xdraw_printf(context, "000000", " ");
   }
   draw_headerline(context, settings->cpus.hdcolor, startx);
}

void
widget_net(
      xdraw_context_t   *context,
      const settings_t  *const settings,
      const oxstats_t   *const stats)
{
   double startx = context->xoffset;
   if (!stats->network->is_setup)
      return;

   static chart_t *chart_in  = NULL;
   static chart_t *chart_out = NULL;
   if (NULL == chart_in || NULL == chart_out) {          /* FIRST time setup */
      const char *colors_in[] = {
         settings->network.inbound_chart_color_pgcolor
      };
      const char *colors_out[] = {
         settings->network.outbound_chart_color_pgcolor
      };
      char *bgcolor_in  = settings->network.inbound_chart_color_bgcolor;
      char *bgcolor_out = settings->network.outbound_chart_color_bgcolor;

      chart_in  = chart_init(60, 1, false, bgcolor_in,  colors_in);
      chart_out = chart_init(60, 1, false, bgcolor_out, colors_out);
   }

   chart_update(chart_in,  (double[]){ stats->network->new_bytes_in });
   chart_update(chart_out, (double[]){ stats->network->new_bytes_out });

   xdraw_printf(context, settings->display.fgcolor, "Net: ");
   xdraw_chart(context, chart_in);
   xdraw_printf(context, "268bd2", " %s ",
         fmt_memory("% .0f", stats->network->new_bytes_in / 1000));
   xdraw_chart(context, chart_out);
   xdraw_printf(context, "dc322f", " %s",
         fmt_memory("% .0f", stats->network->new_bytes_out / 1000));
   draw_headerline(context, settings->network.hdcolor, startx);
}

void
widget_time(
      xdraw_context_t   *context,
      const settings_t  *const settings,
      __attribute__((unused))
      const oxstats_t   *const stats)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   double startx = context->xoffset;
   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, settings->time.format, localtime(&now));

   xdraw_printf(context,
         settings->display.fgcolor,
         buffer);
   draw_headerline(context, settings->time.hdcolor, startx);
}
