#ifndef WNPROCS_H
#define WNPROCS_H

#include <stdbool.h>
#include "../widgets.h"

bool wnprocs_enabled(struct widget *w);
void wnprocs_draw(struct widget *w, struct xctx *ctx);

#endif
