#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"

void
settings_load_defaults(settings_t *s)
{
   s->display.x = 0;
   s->display.y = -1;
   s->display.w = -1;
   s->display.h = 30;
   s->display.top_padding = 10;
   s->display.widget_padding = 15;
   s->display.wmname  = strdup(getprogname());
   s->display.font    = strdup("DejaVu Sans 18px");
   s->display.bgcolor = strdup("1c1c1c");
   s->display.fgcolor = strdup("93a1a1");

   s->battery.hdcolor             = strdup("b58900");
   s->battery.fgcolor_unplugged   = strdup("dc322f");
   s->battery.chart_width         = 7;
   s->battery.chart_color_bgcolor = strdup("dc322f");
   s->battery.chart_color_pgcolor = strdup("859900");

   s->volume.hdcolor       = strdup("cb4b16");
   s->volume.chart_width   = 7;
   s->volume.chart_bgcolor = strdup("859900");
   s->volume.chart_pgcolor = strdup("dc322f");

   s->nprocs.hdcolor = strdup("dc322f");

   s->memory.hdcolor             = strdup("d33682");
   s->memory.chart_color_free    = strdup("859900");
   s->memory.chart_color_total   = strdup("bbbb00");
   s->memory.chart_color_active  = strdup("dc322f");

   s->cpus.hdcolor = strdup("6c71c4");
   s->cpus.chart_color_idle      = strdup("859900");
   s->cpus.chart_color_user      = strdup("ff0000");
   s->cpus.chart_color_sys       = strdup("ffff00");
   s->cpus.chart_color_nice      = strdup("0000ff");
   s->cpus.chart_color_interrupt = strdup("ff00ff");
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
