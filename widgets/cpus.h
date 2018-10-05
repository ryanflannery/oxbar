#ifndef WCPUS_H
#define WCPUS_H

#include <stdbool.h>
#include "../widgets.h"

void wcpus_init(widget_t *w);
void wcpus_free(widget_t *w);
bool wcpus_enabled(widget_t *w);
void wcpus_draw(widget_t *w, xctx_t *ctx);

#endif
