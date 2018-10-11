#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <machine/apmvar.h>

#include "battery.h"

battery_info_t BATTERY;

struct apm_power_info   apm_info;
int                     apm_dev_fd;

void
battery_init()
{
   BATTERY.is_setup = false;

   apm_dev_fd = open("/dev/apm", O_RDONLY);
   if (apm_dev_fd < 0)
   {
      warn("battery: failed to open /dev/apm");
      return;
   }

   BATTERY.plugged_in = false;
   BATTERY.charge_pct = -1.0;
   BATTERY.minutes_remaining = -1;

   BATTERY.is_setup = true;
}

void
battery_update()
{
   if (!BATTERY.is_setup)
      return;

   if (ioctl(apm_dev_fd, APM_IOC_GETPOWER, &(apm_info)) < 0)
      warn("battery update: APM_IOC_GETPOWER");

   switch (apm_info.ac_state)
   {
      case APM_AC_OFF:
         BATTERY.plugged_in = false;
         break;
      case APM_AC_ON:
         BATTERY.plugged_in = true;
         break;
      default:
         warnx("battery: failed to decode APM status");
   }

   BATTERY.charge_pct = apm_info.battery_life;
   BATTERY.minutes_remaining = apm_info.minutes_left;
}

void
battery_close()
{
   if (!BATTERY.is_setup)
      return;

   close(apm_dev_fd);
}
