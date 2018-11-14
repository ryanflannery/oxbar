#include <err.h>
#include <time.h>

#include "util.h"
#include "time.h"

bool
wtime_enabled(__attribute__((unused)) void *wstate)
{
	return true;
}

void
wtime_draw(void *wstate, struct xctx *ctx)
{
	struct generic_wstate *w = wstate;
	struct widget_time_settings *settings = w->settings;
#define GUI_TIME_MAXLEN 100
	static char buffer[GUI_TIME_MAXLEN];

	time_t now = time(NULL);
	strftime(buffer, GUI_TIME_MAXLEN, settings->format, localtime(&now));
	xdraw_printf(ctx, settings->fgcolor, buffer);
}
