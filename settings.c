#include <err.h>
#include <pwd.h>
#include <util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/limits.h>

#include "settings.h"

static bool
parse_keyvalue(char *keyvalue, char **key, char **value)
{
   char tkey[101] = { 0 };
   char tvalue[101] = { 0 };

   if (2 != sscanf(keyvalue, " %100[^= ] = \"%100[ a-zA-Z0-9<>|#%:-]\"", tkey, tvalue))
      if (2 != sscanf(keyvalue, " %100[^= ] = %100[a-zA-Z0-9<>|#%:-]", tkey, tvalue))
         return false;

   *key = strdup(tkey);
   *value = strdup(tvalue);
   return true;
}

static char*
find_home_config()
{
   /* get the home directory needed to define the default config file */
   struct passwd *pw;
   char          *home;
   char          *config_file;

   if (NULL == (home = getenv("HOME")) || '\0' == *home) {
      if (NULL == (pw = getpwuid(getuid())))
         errx(1, "couldn't determine home directory");

      home = pw->pw_dir;
   }

   if (-1 == asprintf(&config_file, "%s/.oxbar.conf", home))
      err(1, "%s: asprintf failed", __FUNCTION__);

   return config_file;
}

void
settings_load_defaults(settings_t *s)
{
   /* default config file is "~/.oxbar.conf" and default theme is null / none */
   s->config_file = find_home_config();
   s->theme = NULL;

   /* the rest here are the default values for all settings */

   s->display.x = 0;
   s->display.y = -1;
   s->display.w = -1;
   s->display.h = -1;
   s->display.padding_top = 10;
   s->display.widget_spacing = 15;
   s->display.wmname  = strdup("oxbar");
   s->display.font    = strdup("DejaVu Sans 16px");
   s->display.bgcolor = strdup("1c1c1c99");
   s->display.fgcolor = strdup("93a1a1");
   s->display.widget_bgcolor = strdup("1c1c1c");
   s->display.widgets = strdup("< nprocs cpus memory net | time > battery volume time");

   s->battery.hdcolor             = strdup("b58900");
   s->battery.fgcolor_unplugged   = strdup("dc322f");
   s->battery.chart_width         = 7;
   s->battery.chart_bgcolor = strdup("dc322f");
   s->battery.chart_pgcolor = strdup("859900");

   s->volume.hdcolor       = strdup("cb4b16");
   s->volume.chart_width   = 7;
   s->volume.chart_bgcolor = strdup("dc322f");
   s->volume.chart_pgcolor = strdup("859900");

   s->nprocs.hdcolor = strdup("dc322f");

   s->memory.hdcolor             = strdup("d33682");
   s->memory.chart_bgcolor       = strdup("555555");
   s->memory.chart_color_free    = strdup("859900");
   s->memory.chart_color_total   = strdup("bbbb00");
   s->memory.chart_color_active  = strdup("dc322f");

   s->cpus.hdcolor               = strdup("6c71c4");
   s->cpus.chart_bgcolor         = strdup("555555");
   s->cpus.chart_color_sys       = strdup("ff0000");
   s->cpus.chart_color_interrupt = strdup("ffff00");
   s->cpus.chart_color_user      = strdup("0000ff");
   s->cpus.chart_color_nice      = strdup("ff00ff");
   s->cpus.chart_color_spin      = strdup("00ffff");
   s->cpus.chart_color_idle      = strdup("859900");

   s->network.hdcolor                        = strdup("268bd2");
   s->network.chart_bgcolor                  = strdup("555555");
   s->network.inbound_chart_color_bgcolor    = strdup("859900");
   s->network.inbound_chart_color_pgcolor    = strdup("157ad2");
   s->network.outbound_chart_color_bgcolor   = strdup("859900");
   s->network.outbound_chart_color_pgcolor   = strdup("dc322f");

   s->time.hdcolor = strdup("859900");
   s->time.format  = strdup("%a %d %b %Y  %I:%M:%S %p");
}

/* TODO Create a settings_free() (or refactor to make easier)
 * I should have this - but this is cumbersome given the above setup.
 * Go to array based? Then every settings retrieval in the draw loop is an
 * array lookup. Bleh. I had a version of this started, but seemed silly.
 * I'll chew more on how best to refactor but for now it's low priority.
void
settings_free(settings_t *s)
{
   free(s->display.wmname);
   free(s->display.bgcolor);
   free(s->display.fgcolor);
   free(s->display.font);

   free(s->battery.hdcolor);
   free(s->battery.str_plugged_in);
   free(s->battery.str_unplugged);
   ...
}
*/

#define SET_STRING_VALUE(name) \
   if (strlen( key ) == strlen( #name ) \
   &&  0 == strncasecmp( #name , key , strlen( #name ))) { \
      s->name = value; \
      free( key );\
      return; \
   }


#define SET_INT_VALUE(name) \
   if (strlen( key ) == strlen( #name ) \
   &&  0 == strncasecmp( #name , key , strlen( #name ))) { \
      s->name = strtonum(value, -1, INT_MAX, &errstr); \
      if (errstr) \
         errx(1, "%s: bad value %s for key %s: %s", __FUNCTION__, value, key, errstr); \
      \
      free( key );\
      free( value ); \
      return; \
   }

void
settings_set_keyvalue(settings_t *s, char *keyvalue)
{
   const char *errstr;
   char *key, *value;

   if (!parse_keyvalue(keyvalue, &key, &value))
      errx(1, "invalid format '%s' (should be 'key = value')", keyvalue);

   /* display */
   SET_INT_VALUE(display.x);
   SET_INT_VALUE(display.y);
   SET_INT_VALUE(display.w);
   SET_INT_VALUE(display.h);
   SET_INT_VALUE(display.padding_top);
   SET_INT_VALUE(display.widget_spacing);
   SET_STRING_VALUE(display.wmname);
   SET_STRING_VALUE(display.font);
   SET_STRING_VALUE(display.bgcolor);
   SET_STRING_VALUE(display.fgcolor);
   SET_STRING_VALUE(display.widget_bgcolor);
   SET_STRING_VALUE(display.widgets);

   /* battery */
   SET_STRING_VALUE(battery.hdcolor);
   SET_STRING_VALUE(battery.fgcolor_unplugged);
   SET_INT_VALUE(battery.chart_width);
   SET_STRING_VALUE(battery.chart_bgcolor);
   SET_STRING_VALUE(battery.chart_pgcolor);

   /* volume */
   SET_STRING_VALUE(volume.hdcolor);
   SET_INT_VALUE(volume.chart_width);
   SET_STRING_VALUE(volume.chart_bgcolor);
   SET_STRING_VALUE(volume.chart_pgcolor);

   /* nprocs */
   SET_STRING_VALUE(nprocs.hdcolor);

   /* memory */
   SET_STRING_VALUE(memory.hdcolor);
   SET_STRING_VALUE(memory.chart_bgcolor);
   SET_STRING_VALUE(memory.chart_color_free);
   SET_STRING_VALUE(memory.chart_color_total);
   SET_STRING_VALUE(memory.chart_color_active);

   /* cpus */
   SET_STRING_VALUE(cpus.hdcolor);
   SET_STRING_VALUE(cpus.chart_bgcolor);
   SET_STRING_VALUE(cpus.chart_color_sys);
   SET_STRING_VALUE(cpus.chart_color_interrupt);
   SET_STRING_VALUE(cpus.chart_color_user);
   SET_STRING_VALUE(cpus.chart_color_nice);
   SET_STRING_VALUE(cpus.chart_color_spin);
   SET_STRING_VALUE(cpus.chart_color_idle);

   /* network */
   SET_STRING_VALUE(network.hdcolor);
   SET_STRING_VALUE(network.chart_bgcolor);
   SET_STRING_VALUE(network.inbound_chart_color_bgcolor);
   SET_STRING_VALUE(network.inbound_chart_color_pgcolor);
   SET_STRING_VALUE(network.outbound_chart_color_bgcolor);
   SET_STRING_VALUE(network.outbound_chart_color_pgcolor);

   /* time */
   SET_STRING_VALUE(time.hdcolor);
   SET_STRING_VALUE(time.format);

   /* unknown key! */
   errx(1, "unknown key '%s''", keyvalue);
}

void
usage()
{
   printf(
"usage: oxbar [-H] [-x xloc] [-y yloc] [-w width] [-h height]\n"
"             [-f font] [-p padding] [-s spacing] [-t time_format]\n"
"             [-W widgets] [-S key=value]\n"
"             [named-configuration]\n"
"Each of the options is described briefly below. They are explained in full\n"
"detail in the man page.\n"
"   -H               Show this help text\n"
"   -F file          Use file as the config file rather than ~/.oxbar.conf\n"
"   -x xloc          The x coordinate in pixels of the upper-left corner\n"
"   -y yloc          The y coordinate in pixels of the upper-left corner\n"
"                    If -1, auto-align to the bottom of the display\n"
"   -w width         The width of the display in pixels\n"
"                    If -1, use the full width of the X display\n"
"   -h height        The height of the display in pixels\n"
"                    If -1, derive the height based on the font used\n"
"   -f font          The font to use, and any styles/sizing\n"
"                    See \"Specifying Fonts\" below for more details\n"
"   -p padding       The padding in pixels between a widget's content and edge\n"
"   -s spacing       The spacing in pixels between widgets\n"
"   -t time_format   The format to display date/time in (see strftime(3))\n"
"   -W widgets       The list of widgets to display\n"
"                    See \"Specifying Widgets\" below for more details\n"
"   -S key=value     Set any configurable value in oxba\n\n"
"Specifying Fonts (and styles, sizes)\n"
"   oxbar uses pango to load & render fonts, and passes the string specified\n"
"   here to pango_font_description_from_string() - see that documentation for\n"
"   full details on the format. Roughly, the format is \"Family (style) (size)\"\n"
"   such as \"Helvetica italic 16px\" or just \"Helvetica 16px\". Note that when\n"
"   specifying the size, it must be in pixels (not points or pt)\n\n"
"Specifying Widgets\n"
"   The list of widgets to show is specified as a space separate list of widget\n"
"   names. A string such as \"cpus memory network time\" would show those four\n"
"   widgets in that order. Some additional characters can be used to control\n"
"   the alignment of widgets, as a common use case for oxbar is it render\n"
"   widgets across the full width of the display, where some are aligned on\n"
"   the left, others on the right, and others centered. The characters to\n"
"   control such alignment are:\n"
"      '<'     All widgets after this are in the left-aligned stack\n"
"              (this is the default)\n"
"      '|'     All widgets after this are in the center-aligned stack\n"
"      '>'     All widgets after this are in the right-aligned stack\n"
"   So the string \"cpus memory network | time > volume battery\" would show\n"
"   cpus/memory/network widgets on the left, time in the center, and volume\n"
"   and battery widgets on the right.\n"
         );
   exit(1);
}

void
settings_parse_cmdline(settings_t *s, int argc, char *argv[])
{
   const char *errstr;
   char *keyvalue;
   int ch;

   while (-1 != (ch = getopt(argc, argv, "HF:x:y:w:h:f:p:s:t:W:S:"))) {
      switch (ch) {
      case 'H':
         usage();
         break;
      case 'F':
         free(s->config_file);
         s->config_file = strdup(optarg);
         if (NULL == s->config_file)
            err(1, "strdup failed for config_file");
         break;
      case 'x':
         s->display.x = strtonum(optarg, 0, INT_MAX, &errstr);
         if (errstr)
            errx(1, "illegal x value '%s': %s", optarg, errstr);
         break;
      case 'y':
         s->display.y = strtonum(optarg, -1, INT_MAX, &errstr);
         if (errstr)
            errx(1, "illegal y value '%s': %s", optarg, errstr);
         break;
      case 'w':
         s->display.w = strtonum(optarg, -1, INT_MAX, &errstr);
         if (errstr)
            errx(1, "illegal w value '%s': %s", optarg, errstr);
         break;
      case 'h':
         s->display.h = strtonum(optarg, -1, INT_MAX, &errstr);
         if (errstr)
            errx(1, "illegal h value '%s': %s", optarg, errstr);
         break;
      case 'f':
         free(s->display.font);
         s->display.font = strdup(optarg);
         if (NULL == s->display.font)
            err(1, "strdup failed for font");
         break;
      case 'p':
         s->display.padding_top = strtonum(optarg, 0, INT_MAX, &errstr);
         if (errstr)
            errx(1, "illegal p value '%s': %s", optarg, errstr);
         break;
      case 's':
         s->display.widget_spacing = strtonum(optarg, 0, INT_MAX, &errstr);
         if (errstr)
            errx(1, "illegal s value '%s': %s", optarg, errstr);
         break;
      case 't':
         free(s->time.format);
         s->time.format = strdup(optarg);
         if (NULL == s->time.format)
            err(1, "strdup failed for time format");
         break;
      case 'W':
         free(s->display.widgets);
         s->display.widgets = strdup(optarg);
         if (NULL == s->display.widgets)
            err(1, "strdup failed for display.widgets");
         break;
      case 'S':
         if (NULL == (keyvalue = strdup(optarg)))
            err(1, "strdup failed for font");

         settings_set_keyvalue(s, keyvalue);
         free(keyvalue);
         break;
      default:
         usage();
      }
   }

   argc -= optind;
   argv += optind;

   if (1 == argc) {
      if (NULL == (s->theme = strdup(argv[argc - 1])))
         err(1, "%s: strdup failed", __FUNCTION__);
   } else if (argc)
      usage();
}

void
settings_parse_config(settings_t *s, const char *file, const char *theme)
{
   char theme_name[100];
   size_t length, linenum = 0;
   char  *line;
   FILE  *fin;
   bool   parse_lines = true;
   bool   found_theme = false;

   /* open file */
   if (NULL == (fin = fopen(file, "r"))) {
      if (NULL == theme)
         return;
      else
         errx(1, "can't read '%s' and thus don't know about '%s'", file, theme);
   }

   /* start reading & parsing file */
   while (!feof(fin)) {
      /* read next line */
      if (NULL == (line = fparseln(fin, &length, &linenum, NULL, 0))) {
         if (ferror(fin))
            err(1, "error reading config file '%s'", file);
         else
            break;
      }

      /* skip blanklines */
      char *copy = line;
      copy += strspn(copy, " \t\n");
      if ('\0' == copy[0]) {
         free(line);
         continue;
      }

      /* do we have a new 'theme' section starting? */
      if (1 == sscanf(line, " [%100[a-zA-Z0-9]] ", theme_name)) {
         if (NULL != theme && 0 == strcmp(theme_name, theme)) {
            found_theme = true;
            parse_lines = true;
         } else
            parse_lines = false;
      } else {
         /*
          * not a theme line - go ahead and parse the line IF we're either at
          * the start of the file or we've entered a theme section matching
          * the theme specified on the command line
          */
         if (parse_lines)
            settings_set_keyvalue(s, line);
      }

      free(line);
   }

   fclose(fin);

   if (NULL != theme && !found_theme)
      errx(1, "did not find a theme named '%s' in '%s'", theme, file);
}
