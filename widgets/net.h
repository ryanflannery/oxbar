#ifndef WNET_H
#define WNET_H

#include <stdbool.h>
#include "../widgets.h"

void wnet_init(struct widget *w);
void wnet_free(struct widget *w);
bool wnet_enabled(struct widget *w);
void wnet_draw(struct widget *w, struct xctx *ctx);

#endif
