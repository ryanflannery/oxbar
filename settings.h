#ifndef SETTINGS_H
#define SETTINGS_H

/* TODO Create Settings Abstraction
 * This was my first attempt at abstracting settings away from manually
 * passing them around (which I was doing when starting to build oxbar).
 * It seems intuiive...
 *    ...there's a settings type to wrap it all together
 *    ...there's a display component in settings to hold display settings
 *    ...there's a battery component in settings to hold battery widget settings
 *    ...and so on
 *    ...and standard methods to read settings from files/parse argc/argv, etc
 *
 * vitunes actually does something similar, but the settings there are very
 * few (most settings are "Commands" and it has a full command interpreter).
 * I'm not doing that here, and this seems damn cumbersom.
 *
 * I'd like to make adding stats & widgets easy. Having to couple new stats
 * and new widgets is unavoidable, but throwing in a new settings object (with
 * parser etc) seems too much. Is there anyway I could simplify this, using
 * macros or similar, to something like
 * settings[] = {
 *    //DSETTINGS(name,             type,    default),
 *    DSETTINGS("display.x",        int,     -1),
 *    ...
 *    DSETTINGS("volume.bar.text",  char*,  "Volume: "),
 *    DSETTINGS("volume.bar.icon",  char*,  "/path/to/png.png");
 *    ...
 * };
 * That could automagically build setters/getters/etc?
 */

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

/* TODO Implement the below settings.* loaders once settings are settled
 * First figure out a proper settings abstraction to commit to. Then
 * implement these.
void settings_parse_config(settings_t *s, const char *file);
void settings_parse_cmdline(settings_t *s, int argc, char *argv[]);
 */

#endif
