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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "gui/gui.h"
#include "gui/xdraw.h"
#include "gui/xcore.h"

#include "widgets/battery.h"
#include "widgets/bright.h"
#include "widgets/cpus.h"
#include "widgets/memory.h"
#include "widgets/net.h"
#include "widgets/nprocs.h"
#include "widgets/time.h"
#include "widgets/volume.h"
#include "widgets/wifi.h"

struct settings {

   /* these aren't directly 'set-able' settings - more meta-settings */
   char *config_file;   /* file to read settings from */
   char *theme;         /* theme name specified on command line (if any) */

   /* this is setable but doesn't belong to any proper component below */
   char *widgets;       /* widget list with formatting */

   /* core display settings */
   struct xfont_settings   font;
   struct xwin_settings    window;
   struct gui_settings     gui;

   /* per-widget settings */
   struct widget_battery_settings   battery;
   struct widget_bright_settings   bright;
   struct widget_volume_settings    volume;
   struct widget_nprocs_settings    nprocs;
   struct widget_memory_settings    memory;
   struct widget_cpu_settings       cpus;
   struct widget_net_settings       net;
   struct widget_time_settings      time;
   struct widget_wifi_settings      wifi;
};

void settings_init(struct settings *s, int argc, char *argv[]);
void settings_reload_config(struct settings *s);

#endif
