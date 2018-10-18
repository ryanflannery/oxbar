#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct settings {

   /* these aren't directly 'set-able' settings - more meta-settings */
   char *config_file;   /* file to read settings from */
   char *theme;         /* theme name specified on command line (if any) */

   /* core display settings */
   struct display_t {
      int      x, y;
      int      w, h;
      double   margin;
      double   padding;
      char    *wmname;
      char    *font;
      char    *bgcolor;
      char    *fgcolor;
      char    *widget_bgcolor;
      char    *widgets;
   } display;

   /* per-widget settings */

   struct widget_battery{
      char *hdcolor;
      char *fgcolor_unplugged;
      int   chart_width;
      char *chart_bgcolor;
      char *chart_pgcolor;
   } battery;

   struct widget_volume {
      char *hdcolor;
      int   chart_width;
      char *chart_bgcolor;
      char *chart_pgcolor;
   } volume;

   struct widget_nprocs {
      char *hdcolor;
   } nprocs;

   struct widget_memory {
      char *hdcolor;
      char *chart_bgcolor;
      char *chart_color_free;
      char *chart_color_total;
      char *chart_color_active;
   } memory;

   struct widget_cpus {
      char *hdcolor;
      char *chart_bgcolor;
      char *chart_color_sys;
      char *chart_color_interrupt;
      char *chart_color_user;
      char *chart_color_nice;
      char *chart_color_spin;
      char *chart_color_idle;
   } cpus;

   struct widget_network {
      char *hdcolor;
      char *chart_bgcolor;
      char *inbound_chart_color_bgcolor;
      char *inbound_chart_color_pgcolor;
      char *outbound_chart_color_bgcolor;
      char *outbound_chart_color_pgcolor;
   } network;

   struct widget_time {
      char *hdcolor;
      char *format;
   } time;

} settings_t;

void settings_load_defaults(settings_t *s);
void settings_parse_cmdline(settings_t *s, int argc, char *argv[]);
void settings_parse_config(settings_t *s, const char *file, const char *theme);

#endif
