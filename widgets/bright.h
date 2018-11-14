#ifndef WBRIGHT_H
#define WBRIGHT_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_bright_settings {
	char *hdcolor;
	char *bgcolor;
	char *fgcolor;
	int   chart_width;
	char *chart_bgcolor;
	char *chart_pgcolor;
};

bool  wbright_enabled(void *wstate);
void  wbright_draw(void *wstate, struct xctx*);

#endif
