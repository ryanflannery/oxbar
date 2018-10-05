#ifndef WMEMORY_H
#define WMEMORY_H

#include <stdbool.h>
#include "../widgets.h"

bool wmemory_enabled(widget_t *w);
void wmemory_draw(
      widget_t *w,
      xctx_t   *ctx);

#endif
