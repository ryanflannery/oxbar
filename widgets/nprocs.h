#ifndef WNPROCS_H
#define WNPROCS_H

#include <stdbool.h>
#include "../widgets.h"

bool wnprocs_enabled(widget_t *w);
void wnprocs_draw(
      widget_t *w,
      xctx_t   *ctx);

#endif
