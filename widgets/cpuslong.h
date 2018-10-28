#ifndef WCPUSLONG_H
#define WCPUSLONG_H

#include <stdbool.h>
#include "../widgets.h"

void wcpuslong_init(struct widget *w);
void wcpuslong_free(struct widget *w);
bool wcpuslong_enabled(struct widget *w);
void wcpuslong_draw(struct widget *w, struct xctx *ctx);

#endif
