#ifndef WTIME_H
#define WTIME_H

#include <stdbool.h>
#include "../widgets.h"

bool wtime_enabled(struct widget *w);
void wtime_draw(struct widget *w, struct xctx *ctx);

#endif
