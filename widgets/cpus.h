#ifndef WCPUS_H
#define WCPUS_H

#include <stdbool.h>
#include "../widgets.h"

void wcpus_init(struct widget *w);
void wcpus_free(struct widget *w);
bool wcpus_enabled(struct widget *w);
void wcpus_draw(struct widget *w, struct xctx *ctx);

#endif
