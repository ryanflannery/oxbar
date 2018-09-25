#include <stdio.h>

#include <err.h>
#include <fcntl.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/audioio.h>

#include "volume.h"

volume_info_t VOLUME;

/* old */
static int volume_dev_fd;
static int volume_master_idx;
static int volume_max;
static int volume_nchan;
static int volume_left;
static int volume_right;

int
volume_check_dev(int fd, int class, char *name)
{
   mixer_devinfo_t devinfo;

   if (class < 0 || name == NULL)
      return -1;

   devinfo.index = 0;
   while (ioctl(fd, AUDIO_MIXER_DEVINFO, &devinfo) >= 0)
   {
      if ((devinfo.type == AUDIO_MIXER_VALUE)
      &&  (devinfo.mixer_class == class)
      &&  (strncmp(devinfo.label.name, name, MAX_AUDIO_DEV_LEN) == 0))
         return (devinfo.index);

      devinfo.index++;
   }

   return -1;
}

void
volume_init()
{
   mixer_devinfo_t devinfo;
   int oclass_idx, iclass_idx, mclass_idx;

   VOLUME.is_setup = false;

   /* open mixer */
   const char *file = getenv("MIXERDEVICE");
   if (file == NULL)
      file = "/dev/mixer";

   if ((volume_dev_fd = open(file, O_RDWR)) < 0)
   {
      warn("volume: failed to open /dev/mixer");
      return;
   }

   /* find the outputs and inputs classes */
   oclass_idx = iclass_idx = mclass_idx = -1;
   devinfo.index = 0;
   while (ioctl(volume_dev_fd, AUDIO_MIXER_DEVINFO, &devinfo) >= 0)
   {
      if (devinfo.type != AUDIO_MIXER_CLASS)
      {
         devinfo.index++;
         continue;
	   }

      if (strncmp(devinfo.label.name, AudioCoutputs, MAX_AUDIO_DEV_LEN) == 0)
         oclass_idx = devinfo.index;
      if (strncmp(devinfo.label.name, AudioCinputs, MAX_AUDIO_DEV_LEN) == 0)
         iclass_idx = devinfo.index;
      /*
      if (strncmp(devinfo.label.name, AudioNmute, MAX_AUDIO_DEV_LEN) == 0) {
         printf("mute at idx = %d type = %d\n", devinfo.index, devinfo.type);
         mclass_idx = devinfo.index;
      }
      */

      if (oclass_idx != -1 && iclass_idx != -1) /* && mclass_idx != -1)*/
         break;

      devinfo.index++;
   }
   /*volume_mute_idx = mclass_idx;*/

   /* find the master device */
   volume_master_idx = volume_check_dev(volume_dev_fd, oclass_idx, AudioNmaster);
   if (volume_master_idx == -1)
      volume_master_idx = volume_check_dev(volume_dev_fd, iclass_idx, AudioNdac);
   if (volume_master_idx == -1)
      volume_master_idx = volume_check_dev(volume_dev_fd, oclass_idx, AudioNdac);
   if (volume_master_idx == -1)
      volume_master_idx = volume_check_dev(volume_dev_fd, oclass_idx, AudioNoutput);

   if (volume_master_idx == -1)
   {
      warnx("volume: failed to find \"master\" mixer device");
      return;
   }

   devinfo.index = volume_master_idx;
   if (ioctl(volume_dev_fd, AUDIO_MIXER_DEVINFO, &devinfo) == -1)
   {
      warn("AUDIO_MIXER_DEVINFO");
      return;
   }

   volume_max = AUDIO_MAX_GAIN;
   volume_nchan = devinfo.un.v.num_channels;

   /* finished... now close the device and reopen as read only */
   close(volume_dev_fd);
   volume_dev_fd = open("/dev/mixer", O_RDONLY);
   if (volume_dev_fd < 0)
   {
      warn("volume: failed to re-open /dev/mixer");
      return;
   }

   VOLUME.muted = false;
   VOLUME.left_pct = -1.0;
   VOLUME.right_pct = -1.0;

   VOLUME.is_setup = true;
}

/* TODO GET VOLUME "IS MUTED?" WORKING
 * There's a great deal of commented-out code here. This is mostly around
 * trying to figure out how to determine "mute" status, which is something
 * I would really like.
 *
 * I've been digging through mixerctl(1) code and it nicely shows how to
 * recursively iterate through all devices and check their stats. I haven't
 * been able to extract from that how to determine mute status though.
 *
 * Jacob Meuser gave some great advice in the thread below about how best to
 * query all devices and extract such stats. What I need to do is dedicate
 * an evening to just dive-into and figure this.
 * See: https://marc.info/?l=openbsd-ports&m=125177906413076&w=2
void
volume_update_mute()
{
   static mixer_devinfo_t vinfo;

   if (!VOLUME.is_setup)
      return;

   vinfo.index = volume_master_idx;
   vinfo.type  = AUDIO_MIXER_ENUM;
   if (ioctl(volume_dev_fd, AUDIO_MIXER_DEVINFO, &vinfo) < 0)
   {
      warn("volume update: AUDIO_MIXER_DEVINFO failed");
      return;
   }

   printf("vinfo.type = %d\n", vinfo.type);
   if (vinfo.type != AUDIO_MIXER_ENUM) {
      printf("not enum :(\n");
   } else {
      printf("is enum )\n");
   }

   printf("%s\n", vinfo.label.name);
   printf("%s\n", vinfo.un.e.member.label.name);
   VOLUME.muted = false;
}
 */

void
volume_update()
{
   static mixer_ctrl_t vinfo;

   if (!VOLUME.is_setup)
      return;

   /* query info */
   vinfo.dev = volume_master_idx;
   vinfo.type = AUDIO_MIXER_VALUE;
   if (ioctl(volume_dev_fd, AUDIO_MIXER_READ, &vinfo) < 0)
   {
      warn("volume update: AUDIO_MIXER_READ failed");
      return;
   }

   /* record in global struct */
   if (1 == vinfo.un.value.num_channels)
      volume_left = volume_right = vinfo.un.value.level[AUDIO_MIXER_LEVEL_MONO];
   else
   {
      volume_left  = vinfo.un.value.level[AUDIO_MIXER_LEVEL_LEFT];
      volume_right = vinfo.un.value.level[AUDIO_MIXER_LEVEL_RIGHT];
   }

   /* create percents */
   VOLUME.left_pct  = roundf(100.0 * (float)volume_left / (float)volume_max);
   VOLUME.right_pct = roundf(100.0 * (float)volume_right / (float)volume_max);

   /* mute status --- wtf!? */
   /*
   mixer_devinfo_t dinfo;
   dinfo.index = volume_mute_idx;
   dinfo.type = AUDIO_MIXER_VALUE;
   if (ioctl(volume_dev_fd, AUDIO_MIXER_DEVINFO, &dinfo) < 0)
   {
      warn("volume update: AUDIO_MIXER_DEVINFO failed");
      return;
   }
   printf("%d ? %d\n", volume_mute_idx, dinfo.index);
   printf("%d ? %d\n", AUDIO_MIXER_VALUE, dinfo.type);
   printf("%d\n", dinfo.un.v.num_channels);
   printf("%d\n", dinfo.un.v.delta);
   printf("%s\n", dinfo.un.v.units.name);

   printf("%d %d\n", dinfo.un.e.member[0].ord, dinfo.un.e.member[1].ord);
   printf("%s\n", dinfo.un.e.member[0].label.name);
   printf("%s\n", dinfo.un.e.member[1].label.name);
   */
}

void
volume_close()
{
   if (!VOLUME.is_setup)
      return;

   close(volume_dev_fd);
}
