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
