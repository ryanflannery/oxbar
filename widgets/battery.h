#ifndef WBATTERY_H
#define WBATTERY_H

#include <stdbool.h>
#include "../widgets.h"

bool wbattery_enabled(widget_t *w);
void wbattery_draw(
      widget_t *w,
      xctx_t   *ctx);

#endif
