#ifndef WCPUS_H
#define WCPUS_H

#include <stdbool.h>
#include "../widgets.h"

bool wcpus_enabled(widget_t *w);
void wcpus_draw(
      widget_t *w,
      xctx_t   *ctx);

#endif
