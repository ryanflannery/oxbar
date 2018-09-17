#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"

void
settings_load_defaults(settings_t *s)
{
   s->display.x = 0;
   s->display.y = -1; /*1360;*/
   s->display.w = -1;
   s->display.h = 30;
   s->display.top_padding = 10;
   s->display.widget_padding = 15;
   s->display.wmname  = strdup(getprogname());
   s->display.bgcolor = strdup("1c1c1c");
   s->display.fgcolor = strdup("93a1a1");
   s->display.font    = strdup("DejaVu Sans 18px");

   s->battery.hdcolor = strdup("b58900");
   s->battery.color_unplugged = strdup("dc322f");
   s->battery.chart_width = 7;
   s->battery.chart_color_power = strdup("859900");
   s->battery.chart_color_remaining = strdup("dc322f");
}

/* TODO Create Settings Abstraction
 * See previous todo's on this
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
}
*/

#define add_isetting(fname, fdfault) \
   { .name = fname, .type = TINT, .un.i = { .v = fdfault, .dfault = fdfault } }

#define add_fsetting(fname, fdfault) \
   { .name = fname, .type = TFLOAT, .un.f = { .v = fdfault, .dfault = fdfault } }

#define add_ssetting(fname, fdfault) \
   { .name = fname, .type = TSTRING, .un.s = { .v = fdfault, .dfault = fdfault } }

setting_t NewSettings[]  = {
   add_isetting("display.x", 0),    /* left aligned */
   add_isetting("display.y", -1),   /* will use bottom of screen */
   add_isetting("display.w", -1),   /* will us screen width */
   add_isetting("display.h", 30),

   add_isetting("display.top_padding", 10),
   add_isetting("display.widget_padding", 15),

   add_ssetting("display.wmname", "oxbar"),
   add_ssetting("display.bgcolor", "1c1c1c"),
   add_ssetting("display.fgcolor", "93a1a1"),
   add_ssetting("display.font", "DejaVu Sans 18px"),
};
size_t nsettings = sizeof(NewSettings) / sizeof(setting_t);

const char *
fmt_stype(setting_type t)
{
   switch (t)
   {
      case TINT:     return "int";
      case TFLOAT:   return "float";
      case TSTRING:  return "string";
      default:       return "?";
   }
}

void
print_new_settings()
{
   setting_t *s;
   size_t i;

   for (i = 0; i < nsettings; i++) {
      s = &NewSettings[i];
      printf("%2zu: %-25s (%s)\t", i, s->name, fmt_stype(s->type));
      switch (NewSettings[i].type) {
         case TINT:
            printf("value = %d (default = %d)\n", s->un.i.v, s->un.i.dfault);
            break;
         case TFLOAT:
            printf("value = %f (default = %f)\n", s->un.f.v, s->un.f.dfault);
            break;
         case TSTRING:
            printf("value = %s (default = %s)\n", s->un.s.v, s->un.s.dfault);
            break;
         default:
            errx(1, "%s: invalid setting type %d", __FUNCTION__, NewSettings[i].type);
      }
   }
}
