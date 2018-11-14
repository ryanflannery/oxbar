#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void*
generic_init(struct oxstats *stats, void *settings)
{
	struct generic_wstate *w;
	if (NULL == (w = malloc(sizeof(struct generic_wstate))))
		err(1, "malloc() failed for generic widget state");

	w->settings = settings;
	w->stats    = stats;
	return w;
}

void
generic_free(void *wstate)
{
	free(wstate);
}

const char *
fmt_memory(const char *fmt, int kbytes)
{
#define GUI_FMT_MEMORY_BUFFER_MAXLEN 100
	static char          buffer[GUI_FMT_MEMORY_BUFFER_MAXLEN];
	static const char   *suffixes[] = { "k", "M", "G", "T", "P" };
	static const size_t  snum       = sizeof(suffixes) / sizeof(suffixes[0]);

	double dbytes = (double) kbytes;
	size_t step   = 0;
	int    ret;

	if (1024 < kbytes)
		for (step = 0; (kbytes / 1024) > 0 && step < snum; step++, kbytes /= 1024)
			dbytes = kbytes / 1024.0;

	ret = snprintf(buffer, GUI_FMT_MEMORY_BUFFER_MAXLEN, fmt, dbytes);
	if (0 > ret)
		err(1, "%s: snprintf failed for %d", __FUNCTION__, kbytes);

	ret = snprintf(buffer + ret, GUI_FMT_MEMORY_BUFFER_MAXLEN- ret, "%s",
	suffixes[step]);
	if (0 > ret)
		err(1, "%s: snprintf failed for %s", __FUNCTION__, suffixes[step]);

	return buffer;
}
