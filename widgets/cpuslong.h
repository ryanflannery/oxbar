#ifndef WCPUSLONG_H
#define WCPUSLONG_H

#include <stdbool.h>
#include "../widgets.h"

void wcpuslong_init(widget_t *w);
void wcpuslong_free(widget_t *w);
bool wcpuslong_enabled(widget_t *w);
void wcpuslong_draw(widget_t *w, xctx_t *ctx);

#endif
