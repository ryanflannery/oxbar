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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pango/pangocairo.h>

#include "xdraw.h"

static struct padding ZEROPAD = {
	.top    = 0,
	.right  = 0,
	.bottom = 0,
	.left   = 0
};

static struct xctx*
xctx_init(
	struct xfont     *font,
	struct xwin      *win,
	xctx_direction_t  direction,
	double            height,
	struct padding   *padding,
	bool              make_root)
{
	struct xctx *ctx = malloc(sizeof(struct xctx));
	if (NULL == ctx)
		err(1, "%s: malloc failed", __FUNCTION__);

	ctx->is_root   = make_root;
	ctx->direction = direction;
	ctx->xfont     = font;
	ctx->h         = height;
	ctx->w         = win->settings->w;

	if (NULL == padding)
		ctx->padding = &ZEROPAD;
	else
		ctx->padding = padding;

	/* if root, use xcore's root window cairo info, otherwise a new one */
	if (make_root) {
		ctx->surface = win->surface;
		ctx->cairo   = win->cairo;
	} else {
		ctx->surface = cairo_surface_create_similar(
			win->surface,
			CAIRO_CONTENT_COLOR_ALPHA,
			win->settings->w,
			win->settings->h);

		if (CAIRO_STATUS_SUCCESS != cairo_surface_status(ctx->surface))
			errx(1, "failed to create cairo surface");

		ctx->cairo = cairo_create(ctx->surface);
		if (CAIRO_STATUS_SUCCESS != cairo_status(ctx->cairo))
			errx(1, "failed to create cairo object");
	}

	xctx_reset(ctx);
	return ctx;
}

struct xctx*
xctx_init_root(
	struct xfont     *font,
	struct xwin      *win,
	xctx_direction_t  direction,
	struct padding   *padding)
{
	return xctx_init(font, win, direction, win->settings->h, padding, true);
}

struct xctx*
xctx_init_scratchpad(
	struct xfont     *font,
	struct xwin      *win,
	xctx_direction_t  direction,
	struct padding   *padding)
{
	double h = win->settings->h;
	if (NULL == padding)
		padding = &ZEROPAD;
	else
		h = font->height + padding->top + padding->bottom;

	return xctx_init(font, win, direction, h, padding, false);
}

void
xctx_free(struct xctx *ctx)
{
	if (!ctx->is_root) {
		cairo_surface_destroy(ctx->surface);
		cairo_destroy(ctx->cairo);
	}
	free(ctx);
}

void
xctx_reset(struct xctx *ctx)
{
	switch (ctx->direction) {
	case L2R:
		ctx->xoffset = ctx->padding->left;
		ctx->yoffset = ctx->padding->top;
		break;
	case R2L:
		ctx->xoffset = ctx->w - ctx->padding->right;
		ctx->yoffset = ctx->padding->top;
		break;
	case CENTERED:
		ctx->xoffset = ctx->w / 2 - ctx->padding->left;
		ctx->yoffset = ctx->padding->top;
		break;
	}
}

void
xctx_complete(struct xctx *ctx)
{
	switch (ctx->direction) {
	case L2R:
		ctx->xoffset += ctx->padding->right;
		break;
	case R2L:
		ctx->xoffset -= ctx->padding->left;
		break;
	case CENTERED:
		ctx->xoffset += ctx->padding->right;
		break;
	}
}

void
xctx_advance(
	struct xctx *ctx,
	xctx_state_t state,
	double       xdelta,
	__attribute__((unused))
	double       ydelta)
{
	switch (ctx->direction) {
	case L2R:
		switch (state) {
		case BEFORE_RENDER:
			return;
		case AFTER_RENDER:
			ctx->xoffset += xdelta;
			break;
		}
		break;

	case R2L:
		switch (state) {
		case BEFORE_RENDER:
			ctx->xoffset -= xdelta;
			break;
		case AFTER_RENDER:
			return;
		}
		break;

	case CENTERED:
		switch (state) {
		case BEFORE_RENDER:
			ctx->xoffset -= (xdelta / 2);
			break;
		case AFTER_RENDER:
			ctx->xoffset += (xdelta / 2);
			break;
		}
		break;
	}
}

/*
 * Drawing Primitives - note they all follow the same pattern of
 *
 *    xctx_advance(dest, BEFORE_RENDER, width, height);
 *    ...do drawing stuff...
 *    xctx_advance(dest, AFTER_RENDER, width, height);
 *
 * This is to support right-to-left (RTL) rendering, in which we need to
 * advance the "pen" when drawing *before* we actually begin drawing.
 * The xctx_advance() api knows which of these actually advances the pen,
 * based on the direction of the destination context.
 */

void
xdraw_context(
	struct xctx *dest,
	struct xctx *source)
{
	xctx_advance(dest, BEFORE_RENDER, source->xoffset, source->yoffset);

	cairo_set_source_surface(
		dest->cairo,
		source->surface,
		dest->xoffset,
		dest->padding->top);
	cairo_rectangle(
		dest->cairo,
		dest->xoffset,
		dest->padding->top,
		source->xoffset,
		source->h);
	cairo_fill(dest->cairo);

	xctx_advance(dest, AFTER_RENDER, source->xoffset, source->yoffset);
}

void
xdraw_colorfill(
	struct xctx      *ctx,
	const char *const color)
{
	double r, g, b, a;
	hex2rgba(color, &r, &g, &b, &a);
	cairo_set_source_rgba(ctx->cairo, r, g, b, a);
	cairo_paint(ctx->cairo);
}

void
xdraw_headerline(
	struct xctx    *ctx,
	header_style_t  style,
	const char     *color)
{
	double width, y;
	switch (style) {
	case NONE:
		return;
	case ABOVE:
		width = ctx->padding->top;
		y = 0;
		break;
	case BELOW:
		width = ctx->padding->bottom;
		y = ctx->h;
		break;
	default:
		errx(1, "%s: bad header style %d\n", __FUNCTION__, style);
	}

	double r, g, b, a;
	hex2rgba(color, &r, &g, &b, &a);
	cairo_set_source_rgba(ctx->cairo, r, g, b, a);
	cairo_set_line_width(ctx->cairo, width);
	cairo_move_to(ctx->cairo, 0, y);
	cairo_line_to(ctx->cairo, ctx->xoffset, y);
	cairo_stroke(ctx->cairo);
}

void
xdraw_printf(
	struct xctx *ctx,
	const char  *color,
	const char  *fmt,
	...)
{
#define XDRAW_PRINTF_BUFF_MAXLEN 1000
	static char buffer[XDRAW_PRINTF_BUFF_MAXLEN];

	double r, g, b, a;
	int width, height;
	hex2rgba(color, &r, &g, &b, &a);

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, XDRAW_PRINTF_BUFF_MAXLEN, fmt, ap);
	va_end(ap);

	PangoLayout *layout = pango_cairo_create_layout(ctx->cairo);
	pango_layout_set_font_description(layout, ctx->xfont->pfont);
	pango_layout_set_text(layout, buffer, -1);
	pango_layout_get_pixel_size(layout, &width, &height);

	xctx_advance(ctx, BEFORE_RENDER, width, height);

	cairo_set_source_rgba(ctx->cairo, r, g, b, a);
	cairo_move_to(ctx->cairo, ctx->xoffset, ctx->yoffset);
	pango_cairo_show_layout(ctx->cairo, layout);

	xctx_advance(ctx, AFTER_RENDER, width, height);

	g_object_unref(layout);
}

void
xdraw_progress_bar(
	struct xctx *ctx,
	const char  *bgcolor,
	const char  *pgcolor,
	double       width,
	double       pct)
{
	double r, g, b, a;
	double height = ctx->h - ctx->padding->top - ctx->padding->bottom;

	xctx_advance(ctx, BEFORE_RENDER, width, height);

	hex2rgba(bgcolor, &r, &g, &b, &a);
	cairo_set_source_rgba(ctx->cairo, r, g, b, a);
	cairo_rectangle(
		ctx->cairo,
		ctx->xoffset,
		ctx->padding->top,
		width,
		height);
	cairo_fill(ctx->cairo);

	hex2rgba(pgcolor, &r, &g, &b, &a);
	cairo_set_source_rgba(ctx->cairo, r, g, b, a);
	cairo_rectangle(
		ctx->cairo,
		ctx->xoffset,
		ctx->padding->top + ((100.0 - pct)/100.0) * height,
		width,
		(pct/100.0) * height);
	cairo_fill(ctx->cairo);

	xctx_advance(ctx, AFTER_RENDER, width, height);
}

void
xdraw_chart(
	struct xctx  *ctx,
	struct chart *c)
{
	double chart_height = ctx->h - ctx->padding->top - ctx->padding->bottom;
	double width = c->nsamples;
	double r, g, b, a;

	xctx_advance(ctx, BEFORE_RENDER, width, chart_height);

	hex2rgba(c->bgcolor, &r, &g, &b, &a);
	cairo_set_source_rgba(ctx->cairo, r, g, b, a);
	cairo_rectangle(
		ctx->cairo,
		ctx->xoffset,
		ctx->padding->top,
		width,
		chart_height);
	cairo_fill(ctx->cairo);

	double min, max;
	chart_get_minmax(c, &min, &max);

	size_t i, j, count;
	for (count = 0, i = c->current + 1; count < c->nsamples; count++, i++) {
		if (i >= c->nsamples)
			i = 0;

		/* we draw the bars for this sample from the bottom UP */
		double y_bottom = ctx->h - ctx->padding->bottom;
		for (j = 0; j < c->nseries; j++) {
			if (!c->values[i][j])
				continue;

			double height;
			if (c->percents)
				height = c->values[i][j] / 100.0 * chart_height;
			else
				height = c->values[i][j] / max * chart_height;

			double y_top = y_bottom - height;

			/* don't go past the top (can happen w/ rounding) */
			if (y_top < ctx->padding->top)
				y_top = ctx->padding->top;
			else if (c->percents && j == c->nseries - 1)
				y_top = ctx->padding->top;

			/* using cairo lines appears fuzzy - use rectangles */
			hex2rgba(c->colors[j], &r, &g, &b, &a);
			cairo_set_source_rgba(ctx->cairo, r, g, b, a);
			cairo_rectangle(ctx->cairo,
				ctx->xoffset + count,
				y_top,
				1.0,
				height);
			cairo_fill(ctx->cairo);
			y_bottom = y_top;
		}
	}
	xctx_advance(ctx, AFTER_RENDER, width, chart_height);
}
