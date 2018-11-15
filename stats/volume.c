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
#include <sys/audioio.h>

#include <err.h>
#include <fcntl.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "volume.h"

static int mixer_fd;
static int outputs_class = -1;
static int outputs_master_idx = -1;
static int outputs_mute_idx = -1;

void
volume_init(struct volume_stats *stats)
{
	mixer_devinfo_t devinfo;
	stats->is_setup = false;

	/* determine mixer device & open it */
	char *device = getenv("MIXERDEVICE");
	if (NULL == device)
		device = "/dev/mixer";

	if (0 >= (mixer_fd = open(device, O_RDWR)))
		return;

	/* find outputs.master volume and mute devices */
	devinfo.index = 0;
	while (-1 != ioctl(mixer_fd, AUDIO_MIXER_DEVINFO, &devinfo)) {
		if (0 == strncmp(devinfo.label.name, AudioCoutputs, MAX_AUDIO_DEV_LEN))
			outputs_class = devinfo.mixer_class;

		if (0 == strncmp(devinfo.label.name, AudioNmaster, MAX_AUDIO_DEV_LEN)
		&& devinfo.mixer_class == outputs_class)
			outputs_master_idx = devinfo.index;

		if (0 == strncmp(devinfo.label.name, AudioNmute, MAX_AUDIO_DEV_LEN)
		&& devinfo.mixer_class == outputs_class)
			outputs_mute_idx = devinfo.index;

		devinfo.index++;
	}

	/* did we find them? */
	if (-1 == outputs_class
	||  -1 == outputs_master_idx
	||  -1 == outputs_mute_idx) {
		close(mixer_fd);
		stats->is_setup = false;
		return;
	}

	/* reopen mixer device readonly */
	close(mixer_fd);
	if (0 >= (mixer_fd = open(device, O_RDONLY)))
		err(1, "failed to re-open %s readonly", device);

	stats->is_setup = true;
}

void
volume_update(struct volume_stats *stats)
{
	mixer_ctrl_t vinfo;

	if (!stats->is_setup)
		return;

	/* update volume */
	vinfo.dev  = outputs_master_idx;
	vinfo.type = AUDIO_MIXER_VALUE;

	if (-1 == ioctl(mixer_fd, AUDIO_MIXER_READ, &vinfo))
		err(1, "AUDIO_MIXER_READ failed (volume)");

	stats->left  = (float)vinfo.un.value.level[AUDIO_MIXER_LEVEL_LEFT]
	             / (float)AUDIO_MAX_GAIN * 100.0;
	stats->right = (float)vinfo.un.value.level[AUDIO_MIXER_LEVEL_RIGHT]
	             / (float)AUDIO_MAX_GAIN * 100.0;

	/* update mute */
	vinfo.dev  = outputs_mute_idx;
	vinfo.type = AUDIO_MIXER_ENUM;

	if (-1 == ioctl(mixer_fd, AUDIO_MIXER_READ, &vinfo))
		err(1, "AUDIO_MIXER_READ failed (mute)");

	stats->muted = vinfo.un.ord == 1;
}

void
volume_close(struct volume_stats *stats)
{
	if (stats->is_setup)
		close(mixer_fd);
}
