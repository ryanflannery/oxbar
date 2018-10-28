#ifndef WMEMORY_H
#define WMEMORY_H

#include <stdbool.h>
#include "../widgets.h"

void wmemory_init(struct widget *w);
void wmemory_free(struct widget *w);
bool wmemory_enabled(struct widget *w);
void wmemory_draw(struct widget *w, struct xctx *ctx);

#endif
