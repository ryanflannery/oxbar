#ifndef BATTERY_H
#define BATTERY_H

#include <stdbool.h>

typedef struct battery_info {
   bool     is_setup;

   bool     plugged_in;          /* true: plugged in, false: using battery */
   float    charge_pct;          /* percent battery charged */
   int      minutes_remaining;   /* minutes remaining on battery charge */

} battery_info_t;

extern battery_info_t BATTERY;

void battery_init();
void battery_update();
void battery_close();

#endif
