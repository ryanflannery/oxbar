#include <time.h>

#include "time.h"

bool
wtime_enabled(__attribute__((unused)) struct widget *w)
{
   return true;
}

void
wtime_draw(struct widget *w, struct xctx *ctx)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, w->context->settings->time.format,
         localtime(&now));

   xdraw_printf(ctx, w->context->settings->font.fgcolor, buffer);
}
