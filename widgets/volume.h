#ifndef WVOLUME_H
#define WVOLUME_H

#include <stdbool.h>
#include "../widgets.h"

bool wvolume_enabled(widget_t *w);
void wvolume_draw(
      widget_t *w,
      xctx_t   *ctx);

#endif
