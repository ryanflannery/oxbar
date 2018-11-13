/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <machine/apmvar.h>

#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include "battery.h"

static struct apm_power_info   apm_info;
static int                     apm_dev_fd;

void
battery_init(struct battery_stats *stats)
{
   stats->is_setup = false;

   apm_dev_fd = open("/dev/apm", O_RDONLY);
   if (apm_dev_fd < 0)
      return;

   stats->is_setup = true;
}

void
battery_update(struct battery_stats *stats)
{
   if (!stats->is_setup)
      return;

   if (ioctl(apm_dev_fd, APM_IOC_GETPOWER, &(apm_info)) < 0)
      errx(1, "APM_IOC_GETPOWER");

   switch (apm_info.ac_state) {
   case APM_AC_OFF:
      stats->plugged_in = false;
      break;
   case APM_AC_ON:
      stats->plugged_in = true;
      break;
   default:
      warnx("battery: failed to decode APM status");
   }

   stats->charge_pct = apm_info.battery_life;
   stats->minutes_remaining = apm_info.minutes_left;
}

void
battery_close(struct battery_stats *stats)
{
   if (stats->is_setup)
      close(apm_dev_fd);
}
