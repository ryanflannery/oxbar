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

#include <xcb/xcb.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>
#include <xcb/randr.h>

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "brightness.h"

/* all the xcb/xrandr private local state */
static xcb_connection_t        *x;
static xcb_generic_error_t     *error;
static xcb_intern_atom_cookie_t cookie;
static xcb_intern_atom_reply_t *reply;
static xcb_atom_t               atom;
static xcb_screen_iterator_t    si;
static xcb_screen_t            *screen;
static xcb_window_t             window;

static xcb_randr_get_screen_resources_current_cookie_t rec_cookie;
static xcb_randr_get_screen_resources_current_reply_t *rec_reply;
static xcb_randr_output_t                             *outputs;
static xcb_randr_query_output_property_cookie_t        query_cookie;
static xcb_randr_query_output_property_reply_t        *query_reply;

static int32_t max_backlight;
static int32_t min_backlight;
/*
 * if http had been designed like the x11 protocol, the internet wouldn't have
 * happened...
 */

void
brightness_init(struct brightness_stats *stats)
{
	stats->is_setup = false;
	xcb_randr_get_output_property_cookie_t  prop_cookie;
	xcb_randr_get_output_property_reply_t  *prop_reply;

	x = xcb_connect(NULL, NULL);
	cookie = xcb_intern_atom(x, 0, strlen("Backlight"), "Backlight");
	reply = xcb_intern_atom_reply(x, cookie, NULL);
	if (!reply) {
		warnx("xcb atom reply failed for Backlight");
		return;
	}

	atom = reply->atom;
	free(reply);

	if (XCB_ATOM_NONE == atom)
		warnx("no xcb atom for Backlight");

	si = xcb_setup_roots_iterator(xcb_get_setup(x));;
	screen = si.data;
	window = screen->root;

	rec_cookie = xcb_randr_get_screen_resources_current(x, window);
	rec_reply = xcb_randr_get_screen_resources_current_reply(x, rec_cookie,
			&error);
	outputs = xcb_randr_get_screen_resources_current_outputs(rec_reply);

	prop_cookie = xcb_randr_get_output_property(x, outputs[0], atom,
			XCB_ATOM_NONE, 0, 4, 0, 0);
	prop_reply = xcb_randr_get_output_property_reply(x, prop_cookie,
			&error);

	if (NULL == prop_reply
	||  XCB_ATOM_INTEGER != prop_reply->type
	||  1 != prop_reply->num_items
	||  32 != prop_reply->format)
		warnx("xcb randr backlight query failed\n");

	query_cookie = xcb_randr_query_output_property(x, outputs[0], atom);
	query_reply = xcb_randr_query_output_property_reply(x, query_cookie,
			&error);

	int32_t *scale;
	scale = xcb_randr_query_output_property_valid_values(query_reply);
	min_backlight = scale[0];
	max_backlight = scale[1];

	uint8_t value = *xcb_randr_get_output_property_data(prop_reply);
	stats->brightness = ((float)value - (float)min_backlight)
	                  / ((float)max_backlight - (float)min_backlight)
	                  * 100.0;

	free(prop_reply);
	stats->is_setup = true;
}

void
brightness_update(struct brightness_stats *stats)
{
	if (!stats->is_setup)
		return;

	xcb_randr_get_output_property_cookie_t  prop_cookie;
	xcb_randr_get_output_property_reply_t  *prop_reply;

	prop_cookie = xcb_randr_get_output_property(x, outputs[0], atom,
			XCB_ATOM_NONE, 0, 4, 0, 0);
	prop_reply = xcb_randr_get_output_property_reply(x, prop_cookie,
			&error);

	if (NULL == prop_reply
	||  XCB_ATOM_INTEGER != prop_reply->type
	||  1 != prop_reply->num_items
	||  32 != prop_reply->format)
		warnx("xcb randr backlight query failed\n");

	uint8_t value = *xcb_randr_get_output_property_data(prop_reply);
	stats->brightness = ((float)value - (float)min_backlight)
	                  / ((float)max_backlight - (float)min_backlight)
	                  * 100.0;
	free(prop_reply);
}

void
brightness_close(struct brightness_stats *stats)
{
	if (stats->is_setup) {
		free(query_reply);
		free(rec_reply);
		xcb_disconnect(x);
	}
}
