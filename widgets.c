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
#include <string.h>

#include "gui/xdraw.h"
#include "widgets.h"
#include "widgets/util.h"

#include "widgets/battery.h"
#include "widgets/bright.h"
#include "widgets/cpus.h"
#include "widgets/cpushort.h"
#include "widgets/cpuslong.h"
#include "widgets/memory.h"
#include "widgets/net.h"
#include "widgets/nprocs.h"
#include "widgets/time.h"
#include "widgets/volume.h"
#include "widgets/wifi.h"

/* private functions */
static struct		 widget_recipe* find_recipe(const char *);
static void		*get_settings_component(const char * const,
			                        struct settings*);
static struct widget	*widget_create_from_recipe(const char *,
			                           struct settings *,
			                           struct oxstats*);
static void		 widgets_create(const char *, struct gui *,
			                struct settings *, struct oxstats *);

/*
 * Global list of all known widget types and how to create them.
 * When adding a widget, they must be added here.
 */
struct widget_recipe {
	struct widget widget;                            /* the widget itself */
	void  *(*init)(struct oxstats *, void *settings);/* how to build it   */
	void   (*free)(void *widget);                    /* how to destroy it */
};

/* define all widget recipes */
#define WCLRS() NULL, NULL, NULL
struct widget_recipe WIDGET_RECIPES[] = {
   {{"battery",  WCLRS(), wbattery_enabled, wbattery_draw,  NULL}, generic_init,  generic_free },
   {{"bright",   WCLRS(), wbright_enabled,  wbright_draw,   NULL}, generic_init,  generic_free },
   {{"cpu",      WCLRS(), wcpu_enabled,     wcpu_draw,      NULL}, wcpu_init,     wcpu_free },
   {{"cpulong",  WCLRS(), wcpulong_enabled, wcpulong_draw,  NULL}, wcpu_init,     wcpu_free },
   {{"cpushort", WCLRS(), wcpushort_enabled,wcpushort_draw, NULL}, wcpu_init,     wcpu_free },
   {{"memory",   WCLRS(), wmemory_enabled,  wmemory_draw,   NULL}, wmemory_init,  wmemory_free },
   {{"net",      WCLRS(), wnet_enabled,     wnet_draw,      NULL}, wnet_init,     wnet_free },
   {{"nprocs",   WCLRS(), wnprocs_enabled,  wnprocs_draw,   NULL}, generic_init,  generic_free },
   {{"time",     WCLRS(), wtime_enabled,    wtime_draw,     NULL}, generic_init,  generic_free },
   {{"volume",   WCLRS(), wvolume_enabled,  wvolume_draw,   NULL}, generic_init,  generic_free },
   {{"wifi",     WCLRS(), wwifi_enabled,    wwifi_draw,     NULL}, generic_init,  generic_free }
};
const size_t NWIDGET_RECIPES = sizeof(WIDGET_RECIPES) / sizeof(struct widget_recipe);

/* this tracks all widgets created, so they can be destroyed on shutdown */
#define MAX_ALL_WIDGETS 1000
static struct widget *WIDGETS[MAX_ALL_WIDGETS];
static size_t         NWIDGETS = 0;

static struct widget_recipe*
find_recipe(const char *name)
{
	size_t i = 0;
	for (; i < NWIDGET_RECIPES; i++) {
		if (0 == strcmp(name, WIDGET_RECIPES[i].widget.name))
			return &WIDGET_RECIPES[i];
	}
	return NULL;
}

static void *
get_settings_component(const char * const name, struct settings *settings)
{
	if (0 == strcmp(name, "battery"))
		return &settings->battery;
	if (0 == strcmp(name, "bright"))
		return &settings->bright;
	if (0 == strcmp(name, "cpu"))
		return &settings->cpus;
	if (0 == strcmp(name, "cpulong"))
		return &settings->cpus;
	if (0 == strcmp(name, "cpushort"))
		return &settings->cpus;
	if (0 == strcmp(name, "memory"))
		return &settings->memory;
	if (0 == strcmp(name, "net"))
		return &settings->net;
	if (0 == strcmp(name, "nprocs"))
		return &settings->nprocs;
	if (0 == strcmp(name, "time"))
		return &settings->time;
	if (0 == strcmp(name, "volume"))
		return &settings->volume;
	if (0 == strcmp(name, "wifi"))
		return &settings->wifi;

	errx(1, "failed to find settings component for '%s'", name);
}

static struct widget*
widget_create_from_recipe(
	const char      *name,
	struct settings *settings,
	struct oxstats  *stats)
{
	struct widget_recipe *recipe;
	struct widget        *w;

	if (NWIDGETS == MAX_ALL_WIDGETS)
		errx(1, "reached max widget count %d", MAX_ALL_WIDGETS);

	if (NULL == (recipe = find_recipe(name)))
		errx(1, "no widget recipe found named '%s'", name);

	w = malloc(sizeof(struct widget));
	if (NULL == w)
		err(1, "failed to malloc widget named '%s'", name);

	w->name     = recipe->widget.name;
	w->hdcolor  = recipe->widget.hdcolor;
	w->bgcolor  = recipe->widget.bgcolor;
	w->fgcolor  = recipe->widget.fgcolor;
	w->enabled  = recipe->widget.enabled;
	w->draw     = recipe->widget.draw;

	if (NULL != recipe->init) {
		void *state = recipe->init(stats,
		                        get_settings_component(name, settings));
		w->state = state;
	}

	WIDGETS[NWIDGETS++] = w;   /* track to cleanup in widgets_free() */
	return w;
}

static void
widgets_create(
	const char      *list,
	struct gui      *gui,
	struct settings *settings,
	struct oxstats  *stats)
{
	xctx_direction_t direction = L2R;
	char *token;
	char *copylist = strdup(list);   /* strsep(3) will change this */
	char *memhandle = copylist;      /* need this for free() and clang */

	if (NULL == copylist)
		err(1, "failed to strdup widget list '%s'", list);

	while (NULL != (token = strsep(&copylist, " ,"))) {
		if ('\0' == *token)
			continue;

		if (0 == strcasecmp("<", token))
			direction = L2R;
		else if (0 == strcasecmp("|", token))
			direction = CENTERED;
		else if (0 == strcasecmp(">", token))
			direction = R2L;
		else {
			gui_add_widget(gui, direction,
			     widget_create_from_recipe(token, settings, stats));
		}
	}
	free(memhandle);
}

void
widgets_init(struct gui *gui, struct settings *settings, struct oxstats *stats)
{
	/* connect hdcolor, bgcolor, and fgcolor components to their globals */
	widget_set_hdcolor("battery",   &settings->battery.hdcolor);
	widget_set_hdcolor("bright",    &settings->bright.hdcolor);
	widget_set_hdcolor("cpu",       &settings->cpus.hdcolor);
	widget_set_hdcolor("cpushort",  &settings->cpus.hdcolor);
	widget_set_hdcolor("cpulong",   &settings->cpus.hdcolor);
	widget_set_hdcolor("memory",    &settings->memory.hdcolor);
	widget_set_hdcolor("net",       &settings->net.hdcolor);
	widget_set_hdcolor("nprocs",    &settings->nprocs.hdcolor);
	widget_set_hdcolor("time",      &settings->time.hdcolor);
	widget_set_hdcolor("volume",    &settings->volume.hdcolor);
	widget_set_hdcolor("wifi",      &settings->wifi.hdcolor);

	widget_set_bgcolor("battery",   &settings->battery.bgcolor);
	widget_set_bgcolor("bright",    &settings->bright.bgcolor);
	widget_set_bgcolor("cpu",       &settings->cpus.bgcolor);
	widget_set_bgcolor("cpushort",  &settings->cpus.bgcolor);
	widget_set_bgcolor("cpulong",   &settings->cpus.bgcolor);
	widget_set_bgcolor("memory",    &settings->memory.bgcolor);
	widget_set_bgcolor("net",       &settings->net.bgcolor);
	widget_set_bgcolor("nprocs",    &settings->nprocs.bgcolor);
	widget_set_bgcolor("time",      &settings->time.bgcolor);
	widget_set_bgcolor("volume",    &settings->volume.bgcolor);
	widget_set_bgcolor("wifi",      &settings->wifi.bgcolor);

	widget_set_fgcolor("battery",   &settings->battery.fgcolor);
	widget_set_fgcolor("bright",    &settings->bright.fgcolor);
	widget_set_fgcolor("cpu",       &settings->cpus.fgcolor);
	widget_set_fgcolor("cpushort",  &settings->cpus.fgcolor);
	widget_set_fgcolor("cpulong",   &settings->cpus.fgcolor);
	widget_set_fgcolor("memory",    &settings->memory.fgcolor);
	widget_set_fgcolor("net",       &settings->net.fgcolor);
	widget_set_fgcolor("nprocs",    &settings->nprocs.fgcolor);
	widget_set_fgcolor("time",      &settings->time.fgcolor);
	widget_set_fgcolor("volume",    &settings->volume.fgcolor);
	widget_set_fgcolor("wifi",      &settings->wifi.fgcolor);

	widgets_create(settings->widgets, gui, settings, stats);
}

void
widgets_free()
{
	size_t i = 0;

	for (; i < NWIDGETS; i++) {
		struct widget_recipe *recipe = find_recipe(WIDGETS[i]->name);
		if (NULL != recipe && NULL != recipe->free)
			recipe->free(WIDGETS[i]->state);

		free(WIDGETS[i]);
	}
	NWIDGETS = 0;
}

void
widget_set_hdcolor(const char * const name, char **color)
{
	struct widget_recipe *recipe = find_recipe(name);
	if (NULL == recipe)
		errx(1, "unknown widget '%s' used when setting hdcolor", name);

	recipe->widget.hdcolor = color;
}

void
widget_set_bgcolor(const char * const name, char **color)
{
	struct widget_recipe *recipe = find_recipe(name);
	if (NULL == recipe)
		errx(1, "unknown widget '%s' used when setting bgcolor", name);

	recipe->widget.bgcolor = color;
}

void
widget_set_fgcolor(const char * const name, char **color)
{
	struct widget_recipe *recipe = find_recipe(name);
	if (NULL == recipe)
		errx(1, "unknown widget '%s' used when setting fgcolor", name);

	recipe->widget.fgcolor = color;
}
