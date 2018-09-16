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

   /* TODO Create settings abstraction
    * Remove these once i've migrated over to that
    */
   ui->widget_padding = ui->settings->display.widget_padding;
   ui->fgcolor = strdup(ui->settings->display.fgcolor);
   ui->bgcolor = strdup(ui->settings->display.bgcolor);
   /* XXX Regarding the above todo, I think this one might make sense to keep */
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
   /* TODO For settings abstraction: simplify this case
    * Notice this widget will use the settings.* component for picking the
    * colors of the bar. But I still need defaults defined here as a fallback
    * option in case those settings weren't set.
    * Now, I could just enforce defaults and that fallback logic in my
    * settings.* component...right? But then here I just have to "assume" those
    * colors are set/kosher/not-null/etc. I don't necessary like that.
    * I want a hard gaurantee that such things are set entering here.
    *
    * This same issue occurs in the memory and cpu components below (and likely
    * more).
    */
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
   ui->xcurrent += xdraw_percent(
         ui->xinfo,
         ui->settings->display.fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         battery->charge_pct);

   if (-1 != battery->minutes_remaining) {
      ui->xcurrent += ui->small_space;
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
         "Volume: ");
   if (volume->left_pct == volume->right_pct) {
      ui->xcurrent += xdraw_vertical_stack(ui->xinfo, ui->xcurrent, 7, 2,
            colors,
            (double[]){100.0 - volume->left_pct, volume->left_pct});
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
      ui->xcurrent += xdraw_percent(
            ui->xinfo,
            ui->fgcolor,
            ui->xcurrent,
            ui->xinfo->padding,
            volume->right_pct);
   }
   xdraw_hline(ui->xinfo, "cb4b16", ui->xinfo->padding, startx, ui->xcurrent);
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
         "#Procs: ");
   ui->xcurrent += xdraw_int(
         ui->xinfo,
         ui->fgcolor,
         ui->xcurrent,
         ui->xinfo->padding,
         nprocs->nprocs);
   xdraw_hline(ui->xinfo, "dc322f", ui->xinfo->padding, startx, ui->xcurrent);
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

   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "Memory: ");
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
   xdraw_hline(ui->xinfo, "d33682", ui->xinfo->padding, startx, ui->xcurrent);

   ui->xcurrent += ui->widget_padding;
}

void
ui_widget_cpus_draw(
      oxbarui_t  *ui,
      cpus_t     *cpus)
{
   static const char *colors[] = {
      "859900",   /* idle */
      "ff0000",   /* user */
      "ffff00",   /* sys  */
      "0000ff",   /* nice */
      "ff00ff"    /* interrupt */
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

   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "CPUs: ");
   for (i = 0; i < cpus->ncpu; i++) {
      histogram_update(hist_cpu[i], (double[]) {
            cpus->cpus[i].percentages[CP_IDLE],
            cpus->cpus[i].percentages[CP_USER],
            cpus->cpus[i].percentages[CP_SYS],
            cpus->cpus[i].percentages[CP_NICE],
            cpus->cpus[i].percentages[CP_INTR]
            });
      ui->xcurrent += xdraw_histogram(ui->xinfo, ui->xcurrent, colors, hist_cpu[i]);
      /*ui->xcurrent += ui->small_space;*/
      ui->xcurrent += xdraw_percent(ui->xinfo, ui->fgcolor, ui->xcurrent,
            ui->xinfo->padding,
            CPUS.cpus[i].percentages[CP_IDLE]);
      ui->xcurrent += ui->small_space;
   }
   xdraw_hline(ui->xinfo, "6c71c4", ui->xinfo->padding, startx, ui->xcurrent);

   ui->xcurrent += ui->widget_padding;
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

   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, "Network: ");
   /*
    * TODO I'm not sure seeing the #packets in/out is...useful. Remove?
   ui->xcurrent += xdraw_int(ui->xinfo, "859900b2", ui->xcurrent, ui->xinfo->padding, net->new_ip_packets_in);
   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, " in");
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_int(ui->xinfo, "dc322fb2", ui->xcurrent, ui->xinfo->padding, net->new_ip_packets_out);
   ui->xcurrent += xdraw_text(ui->xinfo, ui->fgcolor, ui->xcurrent, ui->xinfo->padding, " out ");
   */

   ui->xcurrent += xdraw_series(ui->xinfo, ui->xcurrent, colors_in, bytes_in);
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_memory_noprecision(ui->xinfo, "268bd2", ui->xcurrent, ui->xinfo->padding, net->new_bytes_in / 1000);
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_series(ui->xinfo, ui->xcurrent, colors_out, bytes_out);
   ui->xcurrent += ui->small_space;
   ui->xcurrent += xdraw_memory_noprecision(ui->xinfo, "dc322f", ui->xcurrent, ui->xinfo->padding, net->new_bytes_out / 1000);

   xdraw_hline(ui->xinfo, "268bd2", ui->xinfo->padding, startx, ui->xcurrent);
}

void
ui_widget_time_draw(
      oxbarui_t  *ui)
{
   static const size_t OXBAR_STR_MAX_TIME_LEN = 100;
   static char buffer[OXBAR_STR_MAX_TIME_LEN];

   time_t now = time(NULL);
   strftime(buffer, OXBAR_STR_MAX_TIME_LEN, "%a %d %b %Y  %I:%M:%S %p", localtime(&now));
   int width = xdraw_text_right_aligned(
         ui->xinfo,
         ui->fgcolor,
         ui->xinfo->display_width,
         ui->xinfo->padding,
         buffer);

   xdraw_hline(ui->xinfo, "859900", ui->xinfo->padding,
         ui->xinfo->display_width - width, ui->xinfo->display_width);
}
