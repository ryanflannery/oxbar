#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <stdbool.h>

struct brightness_info {
   bool  is_setup;
   float brightness;
};

extern struct brightness_info BRIGHTNESS;

void brightness_init();
void brightness_update();
void brightness_close();

#endif
