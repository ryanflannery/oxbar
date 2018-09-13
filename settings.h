#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct settings {

   struct display_t {
      int         x, y;
      int         w, h;
      int         top_padding;
      int         widget_padding;
      const char *wmname;
      const char *font;
      const char *bgcolor;
      const char *fgcolor;
   } display;

   struct widget_battery{
      const char *hdcolor;
      const char *color_unplugged;
      int         chart_width;
      const char *chart_color_power;
      const char *chart_color_remaining;
   } battery;

   struct widget_volume {
      const char *intro;
   } volume;

} settings_t;

void settings_load_defaults(settings_t *s);
void settings_parse_config(settings_t *s, const char *file); /* TODO */
void settings_parse_cmdline(settings_t *s, int argc, char *argv[]); /* TODO */

#endif
