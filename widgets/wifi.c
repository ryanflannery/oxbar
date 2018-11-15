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

#include <err.h>

#include "util.h"
#include "wifi.h"

bool
wwifi_enabled(void *wstate)
{
	struct generic_wstate *w = wstate;
	return w->stats->wifi.is_setup;
}

void
wwifi_draw(void *wstate, struct xctx *ctx)
{
	struct generic_wstate *w = wstate;
	struct widget_wifi_settings *settings = w->settings;
	xdraw_printf(ctx,
		settings->fgcolor,
		"Wifi:% 3.0f%%",
		w->stats->wifi.signal_strength);
}
