#ifndef WTIME_H
#define WTIME_H

#include <stdbool.h>
#include "../widgets.h"

bool wtime_enabled(widget_t *w);
void wtime_draw(
      widget_t *w,
      xctx_t   *ctx);

#endif
