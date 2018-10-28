#ifndef WCPUSHORT_H
#define WCPUSHORT_H

#include <stdbool.h>
#include "../widgets.h"

bool wcpushort_enabled(struct widget *w);
void wcpushort_draw(struct widget *w, struct xctx *ctx);

#endif
