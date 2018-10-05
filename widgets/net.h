#ifndef WNET_H
#define WNET_H

#include <stdbool.h>
#include "../widgets.h"

void wnet_init(widget_t *w);
void wnet_free(widget_t *w);
bool wnet_enabled(widget_t *w);
void wnet_draw(widget_t *w, xctx_t *ctx);

#endif
