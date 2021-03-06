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

#ifndef GUI_H
#define GUI_H

#include <sys/queue.h>

#include "xcore.h"
#include "xdraw.h"

/*
 * A widget, as understood by the gui. It has:
 *    name:    A string name, used solely for debugging.
 *    hdcolor: A color used optionally to draw a bar above/below the widget.
 *    enabled: A method to determine if the widget is enabled or not. Widgets
 *             can appear/disappear based on stats/state.
 *    draw:    A method to render the widget onto a x context.
 */
struct widget {
	const char       *name;             /* only used for debugging       */
	char            **hdcolor;          /* header color                  */
	char            **bgcolor;          /* backround color               */
	char            **fgcolor;          /* backround color               */
	bool (*enabled)(void *);            /* is this widget enabled?       */
	void (*draw)(void *, struct xctx*); /* draw it to a context!         */
	void             *state;            /* internal state to the widget  */
};

/* pad that wraps all user-customizable settings for a gui */
struct gui_settings {
	char          *widget_bgcolor;
	int            spacing;
	struct padding padding;
	struct padding margin;
	header_style_t header_style;
};

struct widget_list_entry {
	TAILQ_ENTRY(widget_list_entry) widget_entry;
	struct widget *widget;
};

/* a gui just contains & orchestrates the drawing of widgets */
struct gui {
	struct gui_settings *s;
	struct xfont        *xfont;
	struct xwin         *xwin;

	/* list of widgets in the left, center, and right lists */
	TAILQ_HEAD(widget_list, widget_list_entry) left, center, right;
};

struct gui* gui_init(struct xfont *, struct xwin *, struct gui_settings *);
void gui_free(struct gui *);
void gui_add_widget(struct gui *, xctx_direction_t, struct widget *);
void gui_draw(struct gui *);

#endif
