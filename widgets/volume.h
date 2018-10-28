#ifndef WVOLUME_H
#define WVOLUME_H

#include <stdbool.h>
#include "../widgets.h"

bool wvolume_enabled(struct widget *w);
void wvolume_draw(struct widget *w, struct xctx *ctx);

#endif
