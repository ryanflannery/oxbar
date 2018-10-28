#ifndef WBATTERY_H
#define WBATTERY_H

#include <stdbool.h>
#include "../widgets.h"

bool wbattery_enabled(struct widget *w);
void wbattery_draw(struct widget *w, struct xctx *ctx);

#endif
