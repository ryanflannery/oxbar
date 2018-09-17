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

   ui->settings = s;
   ui->xinfo = malloc(sizeof(xinfo_t));
   if (NULL == ui->xinfo)
      err(1, "%s: couldn't malloc xinfo", __FUNCTION__);

   /* These need to be done in a specific order */
   xcore_setup_x_connection_screen_visual(ui->xinfo);

   /* TODO Support display height based on font size
    * I used to support this using the cairo font API where the font size was
    * explicit, and I could use that to autoscale the display height as an
    * option (display height = font size + 2 * padding).
    * This would be great to support again, but after migrating to pango for
    * font loading and rendering (a big improvement in appearance) I haven't
    * yet figured out how to retrieve font height (not specific TEXT RENDERED
    * IN A FONT but rather generic font height).
    *
   if (-1 == height)
      height = (uint32_t)(ceil(fontpt + (2 * padding)));
    */

   if (-1 == ui->settings->display.y)
      ui->settings->display.y = ui->xinfo->display_height - ui->settings->display.h ;

   if (-1 == ui->settings->display.w)
      ui->settings->display.w = ui->xinfo->display_width;

   ui->xinfo->padding = ui->settings->display.top_padding;

   xcore_setup_x_window(
         ui->xinfo,
         ui->settings->display.wmname,
         ui->settings->display.x, ui->settings->display.y,
         ui->settings->display.w, ui->settings->display.h);

   xcore_setup_x_wm_hints(ui->xinfo);
   xcore_setup_cairo(ui->xinfo);
   xcore_setup_xfont(ui->xinfo, ui->settings->display.font);

   /* now map the window & do an initial paint */
   xcb_map_window(ui->xinfo->xcon, ui->xinfo->xwindow);

   ui->space_width = xdraw_printf(ui->xinfo, NULL, 0, 0, " ");
   return ui;
}

void
ui_destroy(oxbarui_t *ui)
{
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

/* TODO Simplify xdraw.* For widget rendering
 * See the widget below. They all follow the same pattern of passing the
 * x/y start and other information to each underlying xdraw.* primitive.
 * It works...but it's damn cumbersome. I'd rather try to simplify that more.
 * Could a simple ui-context object encapsulating all that help simplify
 * things? (putting x/y/w/h in there?)
 * That's very similar to what cairo does with its rendering API. You just
 * call methods to alter the current state and then execute that state.
 *
 * Remember the ultimate goal here is:
 *              TO MAKE CREATING AND TWEAKING WIDGETS EASY!
 * That way it's easy and more likely for me to do so in the future.
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

   double startx = ui->xcurrent;

   ui->xcurrent += xdraw_printf(
         ui->xinfo,
         battery->plugged_in ?
            ui->settings->display.fgcolor :
            ui->settings->battery.fgcolor_unplugged ,
         ui->xcurrent,
         ui->xinfo->padding,
         battery->plugged_in ? "AC " : "BAT ");
   ui->xcurrent += xdraw_vertical_stack(
         ui->xinfo,
         ui->xcurrent,
         ui->settings->battery.chart_width,
         2, colors,
         (double[]){100.0 - battery->charge_pct, battery->charge_pct});
   ui->xcurrent += xdraw_printf(
         ui->xinfo,
         ui->settings->display.fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         "% 3.0f%%", battery->charge_pct);

   if (-1 != battery->minutes_remaining) {
      ui->xcurrent += xdraw_printf(
            ui->xinfo,
            ui->settings->display.fgcolor,
            ui->xcurrent,
            ui->xinfo->padding,
            " %dh %dm",
               battery->minutes_remaining / 60,
               battery->minutes_remaining % 60);
   }

   xdraw_hline(ui->xinfo, ui->settings->battery.hdcolor, ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->settings->display.widget_padding;
}

void
ui_widget_volume_draw(
      oxbarui_t      *ui,
      volume_info_t  *volume)
{
   double startx = ui->xcurrent;

   ui->xcurrent += xdraw_printf(
         ui->xinfo,
         ui->settings->display.fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         "Volume: ");

   /* TODO Should volume widget ever handle this case!?
    * I used to render seperate bars & percentages for each. I don't think
    * I've ever used that though in...10 years!?
    * I'm going to skip doing it in oxbar and just warn here.
    */
   if (volume->left_pct != volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal", __FUNCTION__);

   ui->xcurrent += xdraw_vertical_stack(
         ui->xinfo,
         ui->xcurrent,
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

   ui->xcurrent += xdraw_printf(
         ui->xinfo,
         ui->settings->display.fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         "% 3.0f%%", volume->left_pct);

   xdraw_hline(ui->xinfo, ui->settings->volume.hdcolor, ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->settings->display.widget_padding;
}

void
ui_widget_nprocs_draw(
      oxbarui_t      *ui,
      nprocs_info_t  *nprocs)
{
   double startx = ui->xcurrent;

   ui->xcurrent += xdraw_printf(
         ui->xinfo,
         ui->settings->display.fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         "#Procs: %d", nprocs->nprocs);

   xdraw_hline(ui->xinfo, ui->settings->nprocs.hdcolor, ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->settings->display.widget_padding;
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
   double startx = ui->xcurrent;

   static histogram_t *histogram = NULL;
   if (NULL == histogram)
      histogram = histogram_init(60, 3);

   histogram_update(histogram, (double[]) {
         memory->free_pct,
         memory->total_pct,
         memory->active_pct
         });

   ui->xcurrent += xdraw_printf(ui->xinfo, ui->settings->display.fgcolor, ui->xcurrent, ui->xinfo->padding, "Memory: ");
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_histogram(ui->xinfo, ui->xcurrent, colors, histogram);
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, "dc322f", ui->xcurrent, ui->xinfo->padding, "%s", fmt_memory("%.1lf", memory->active));
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, ui->settings->display.fgcolor, ui->xcurrent, ui->xinfo->padding, "active");
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, "b58900", ui->xcurrent, ui->xinfo->padding, "%s", fmt_memory("%.1lf", memory->total));
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, ui->settings->display.fgcolor, ui->xcurrent, ui->xinfo->padding, "total");
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, "859900", ui->xcurrent, ui->xinfo->padding, fmt_memory("%.1lf", memory->free));
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, ui->settings->display.fgcolor, ui->xcurrent, ui->xinfo->padding, "free");

   xdraw_hline(ui->xinfo, ui->settings->memory.hdcolor, ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->settings->display.widget_padding;
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

   ui->xcurrent += xdraw_printf(ui->xinfo, ui->settings->display.fgcolor, ui->xcurrent, ui->xinfo->padding, "CPUs: ");
   for (i = 0; i < cpus->ncpu; i++) {
      histogram_update(hist_cpu[i], (double[]) {
            cpus->cpus[i].percentages[CP_IDLE],
            cpus->cpus[i].percentages[CP_USER],
            cpus->cpus[i].percentages[CP_SYS],
            cpus->cpus[i].percentages[CP_NICE],
            cpus->cpus[i].percentages[CP_INTR]
            });
      ui->xcurrent += xdraw_histogram(ui->xinfo, ui->xcurrent, colors, hist_cpu[i]);
      ui->xcurrent += xdraw_printf(ui->xinfo,
            ui->settings->display.fgcolor,
            ui->xcurrent,
            ui->xinfo->padding,
            "% 3.0f%%", CPUS.cpus[i].percentages[CP_IDLE]);
      ui->xcurrent += ui->space_width;
   }

   xdraw_hline(ui->xinfo, ui->settings->cpus.hdcolor, ui->xinfo->padding, startx, ui->xcurrent);
   ui->xcurrent += ui->settings->display.widget_padding;
}

void
ui_widget_net_draw(
      oxbarui_t  *ui,
      net_info_t *net)
{
   static const char *colors_in[] = {
      "859900",   /* free */
      "157ad2",   /* in */
   };
   static const char *colors_out[] = {
      "859900",   /* free */
      "dc322f",   /* out */
   };

   static tseries_t *bytes_in  = NULL;
   static tseries_t *bytes_out = NULL;

   if (NULL == bytes_in || NULL == bytes_out) {
      bytes_in  = tseries_init(60);
      bytes_out = tseries_init(60);
   }

   tseries_update(bytes_in,  net->new_bytes_in);
   tseries_update(bytes_out, net->new_bytes_out);

   double startx = ui->xcurrent;

   ui->xcurrent += xdraw_printf(ui->xinfo, ui->settings->display.fgcolor, ui->xcurrent, ui->xinfo->padding, "Network: ");
   ui->xcurrent += xdraw_series(ui->xinfo, ui->xcurrent, colors_in, bytes_in);
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, "268bd2", ui->xcurrent, ui->xinfo->padding, "%s", fmt_memory("% 4.0f", net->new_bytes_in / 1000));
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_series(ui->xinfo, ui->xcurrent, colors_out, bytes_out);
   ui->xcurrent += ui->space_width;
   ui->xcurrent += xdraw_printf(ui->xinfo, "dc322f", ui->xcurrent, ui->xinfo->padding, "%s", fmt_memory("% 4.0f", net->new_bytes_out / 1000));

   xdraw_hline(ui->xinfo, "268bd2", ui->xinfo->padding, startx, ui->xcurrent);
}

void
ui_widget_time_draw(
      oxbarui_t  *ui)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, "%a %d %b %Y  %I:%M:%S %p", localtime(&now));
   int width = xdraw_text_right_aligned(
         ui->xinfo,
         ui->settings->display.fgcolor,
         ui->xinfo->display_width,
         ui->xinfo->padding,
         buffer);

   xdraw_hline(ui->xinfo, "859900", ui->xinfo->padding,
         ui->xinfo->display_width - width, ui->xinfo->display_width);
}
