#ifndef WUTIL_H
#define WUTIL_H

#include "../stats/stats.h"

/* for widgets that have no local state (other than stats & settings hooks) */
struct generic_wstate {
	struct oxstats *stats;
	void           *settings;
};
void* generic_init(struct oxstats*, void *settings);
void  generic_free(void *wstate);

/* a method to format memtory into a string */
const char *fmt_memory(const char*, int);

#endif
