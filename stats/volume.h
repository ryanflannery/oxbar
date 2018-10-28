#ifndef VOLUME_H
#define VOLUME_H

#include <stdbool.h>

struct volume_info {
   bool  is_setup;
   bool  muted;
   float left_pct, right_pct;
};

extern struct volume_info VOLUME;

void volume_init();
void volume_update();
void volume_close();

#endif
