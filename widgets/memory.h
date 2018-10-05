#ifndef WMEMORY_H
#define WMEMORY_H

#include <stdbool.h>
#include "../widgets.h"

void wmemory_init(widget_t *w);
void wmemory_free(widget_t *w);
bool wmemory_enabled(widget_t *w);
void wmemory_draw(widget_t *w, xctx_t *ctx);

#endif
