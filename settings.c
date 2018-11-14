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

#include <sys/limits.h>

#include <err.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util.h>

#include "settings.h"
#include "gui/xcore.h"

/* useful utilities */
static void  print_usage();
static char *get_default_config();
static void  get_config_and_theme(int argc, char * const argv[],
                                  char **config_file, char **theme);
static bool  parse_keyvalue(const char * const keyvalue,
                            char **key, char **value);

/* parsers for settings that are struct's (not base types) */
static void parse_padding(struct padding *padding, const char * const str);
static void parse_header_style(header_style_t *style, const char * const str);

/* core settings api */
static void settings_set_defaults(struct settings *s);
static bool settings_set_one_keyvalue(struct settings *s,
                                      const char *key, const char *value);
static void settings_set_keyvalue(struct settings *s,
                                  const char * const keyvalue);
static void settings_parse_cmdline(struct settings *s,
                                   int argc, char * const argv[]);
void settings_reload_config(struct settings *s);
void settings_init(struct settings *settings, int argc, char *argv[]);


/* The set of allowed switches to oxbar, getopt(3) style (used in two places) */
static const char * const SWITCHES = "HF:x:y:w:h:f:m:p:s:t:c:W:S:";


/* Print basic usage information */
static void
print_usage()
{
	printf(
"usage: oxbar [-H] [-F config] [-x xloc] [-y yloc] [-w width] [-h height]\n"
"             [-f font] [-m margins] [-p paddings] [-s spacing]\n"
"             [-c header-style] [-t time_format] [-W widgets]\n"
"             [-S key=value]\n"
"             [theme]\n"
"Where: (see man page for full details)\n"
"   -H               Show this help text\n"
"   -F file          Use file as the config file rather than ~/.oxbar.conf\n"
"   -x xloc          The x coordinate in pixels of the upper-left corner\n"
"   -y yloc          The y coordinate in pixels of the upper-left corner\n"
"                    If -1, auto-align to the bottom of the display\n"
"   -w width         The width of the display in pixels\n"
"                    If -1, use the full width of the X display\n"
"   -h height        The height of the display in pixels\n"
"                    If -1, derive the height based on the font used\n"
"   -f font          The font to use, and any styles/sizing\n"
"   -m margins       The margins (pixels) between a widget's edge & display\n"
"   -p paddings      The paddings (pixels) between a widget's content & edge\n"
"                    For magin & padding you can specify all 4 components at\n"
"                    once, eg `-p \"top right bottom left\"`\n"
"   -s spacing       The spacing in pixels between widgets\n"
"   -c header-style  If and where to show the colored headlines above/below\n"
"                    widgets. Can be \'none\', \'above\', or \'below\'.\n"
"   -t time_format   The format to display date/time in (see strftime(3))\n"
"   -W widgets       The list of widgets to display\n"
"   -S key=value     Set any configurable value in oxbar (see oxbar(1))\n"
	);
	exit(1);
}

/*
 * This constructs the string for the default config file, effectively just
 * replacing the "~/.oxbar.conf" with the appropriate value of "~".
 * XXX It is the callers responsibility to free() the result.
 */
static char*
get_default_config()
{
	struct passwd *pw;
	char          *home;
	char          *config_file;

	if (NULL == (home = getenv("HOME")) || '\0' == *home) {
		if (NULL == (pw = getpwuid(getuid())))
			errx(1, "failed to determine home directory");

		home = pw->pw_dir;
	}

	if (-1 == asprintf(&config_file, "%s/.oxbar.conf", home))
		err(1, "failed to build config file path using '%s'", home);

	return config_file;
}

/*
 * This parses argc/argv and extracts JUST the config file and theme, if either
 * are specified. The format for config file and them are:
 *    $ oxbar ... -F /path/to/config ... theme
 * That is, config file is specified via [-F file] and theme is just a string
 * after all options. Both are optional.
 * If either are specified in the passed argc/argv pair, the corresponding
 * parameters will be set to those. Otherwise they remain unset.
 * XXX It is the callers responsibility to free() the result.
 */
static void
get_config_and_theme(int argc, char * const argv[],
      char **config_file, char **theme)
{
	int ch;
	opterr = 0; /* disable getopt(3) errors on unknown parameters */

	/* get the config file (if specified) */
	while (-1 != (ch = getopt(argc, argv, SWITCHES))) {
		if ('F' == ch) {
			if (NULL == (*config_file = strdup(optarg)))
				err(1, "strdup config file '%s'", optarg);
		}
	}

	/* get the theme (if specified) */
	argc -= optind;
	argv += optind;

	if (1 == argc) {
		/* we have a theme - set it */
		if (NULL == (*theme = strdup(argv[argc - 1])))
			err(1, "failed to strdup theme '%s'", argv[argc - 1]);
	}

	/* reest getopt(3) */
	opterr = 1;
	optreset = 1;
	optind = 1;
}

/*
 * Parse a "key = value" string (value may be quoted) into its components.
 * XXX Note the component vlues (for key and value) are allocated - it's the
 * callers responsibility to free() them.
 */
static bool
parse_keyvalue(const char * const keyval, char **key, char **value)
{
	static const char *pattern1 = " %100[^= ] = "
	                              "\"%100[ a-zA-Z0-9<>|#%:-]\"";
	static const char *pattern2 = " %100[^= ] = "
		                      "%100[a-zA-Z0-9<>|#%:-]";
	char tkey[101] = { 0 };
	char tvalue[101] = { 0 };

	if (2 != sscanf(keyval, pattern1, tkey, tvalue)
	&&  2 != sscanf(keyval, pattern2, tkey, tvalue))
		return false;

	*key = strdup(tkey);
	*value = strdup(tvalue);
	return true;
}

/*
 * This parses a a string into a padding. The string can either be a single
 * number ("%lf") in which case all 4 components of the struct padding are set
 * to the extracted number, or a string with all four components listed.
 * No allocations done here.
 */
static void
parse_padding(struct padding *padding, const char * const value)
{
	static const char *pattern = "%lf %lf %lf %lf";
	double top, right, bottom, left;
	switch (sscanf(value, pattern, &top, &right, &bottom, &left)) {
	case 1:
		padding->top    = top;
		padding->right  = top;
		padding->bottom = top;
		padding->left   = top;
		break;
	case 4:
		padding->top    = top;
		padding->right  = right;
		padding->bottom = bottom;
		padding->left   = left;
		break;
	default:
		errx(1, "invalid padding string '%s' encountered", value);
	}
}

/* This parses a string into a header_style_t. No allocations done here. */
static void
parse_header_style(header_style_t *style, const char * const value)
{
	if (0 == strcasecmp("none", value))
		*style = NONE;
	else if (0 == strcasecmp("above", value))
		*style = ABOVE;
	else if (0 == strcasecmp("below", value))
		*style = BELOW;
	else
		errx(1, "failed to parse header-style '%s'", value);
}

/*
 * Initialize default values for EVERYTHING in settings. Note this is always
 * called first - so all values of settings are set to a reasonable default.
 * XXX This assumptions also means that whenever we change any value that's
 * dynamically allocated (strings), those need to be free()'d before
 * resetting. The macro's below (used to set these) ensure that.
 *
 * ALL settings values should be set here, to a sane default.
 */
static void
settings_set_defaults(struct settings *s)
{
	s->widgets = strdup("nprocs cpu memory net > battery wifi "
	                    "bright volume time");

	s->font.desc = strdup("DejaVu sans mono 16px");
	s->font.fgcolor = strdup("93a1a1");

	s->window.x = 0;
	s->window.y = 0;
	s->window.w = -1;
	s->window.h = -1;
	s->window.wname = strdup("oxbar");
	s->window.bgcolor = strdup("0a0a0a");

	s->gui.widget_bgcolor = strdup("");
	s->gui.spacing = 20;
	s->gui.padding.top    = 5;
	s->gui.padding.bottom = 5;
	s->gui.padding.left   = 5;
	s->gui.padding.right  = 5;
	s->gui.margin.top    = 2;
	s->gui.margin.bottom = 2;
	s->gui.margin.left   = 2;
	s->gui.margin.right  = 2;
	s->gui.header_style = BELOW;

	s->battery.hdcolor            = strdup("b58900");
	s->battery.bgcolor            = strdup("");
	s->battery.fgcolor            = strdup("");
	s->battery.fgcolor_unplugged  = strdup("dc322f");
	s->battery.chart_width        = 7;
	s->battery.chart_bgcolor      = strdup("dc322f");
	s->battery.chart_pgcolor      = strdup("859900");

	s->bright.hdcolor             = strdup("8900b5");
	s->bright.bgcolor             = strdup("");
	s->bright.fgcolor             = strdup("");
	s->bright.chart_width         = 7;
	s->bright.chart_bgcolor       = strdup("dc322f");
	s->bright.chart_pgcolor       = strdup("859900");

	s->cpus.hdcolor               = strdup("6c71c4");
	s->cpus.bgcolor               = strdup("");
	s->cpus.fgcolor               = strdup("");
	s->cpus.chart_bgcolor         = strdup("333333");
	s->cpus.chart_color_system    = strdup("ff0000");
	s->cpus.chart_color_interrupt = strdup("ffff00");
	s->cpus.chart_color_user      = strdup("3333ff");
	s->cpus.chart_color_nice      = strdup("ff00ff");
	s->cpus.chart_color_spin      = strdup("00ffff");
	s->cpus.chart_color_idle      = strdup("859900");

	s->memory.hdcolor             = strdup("d33682");
	s->memory.bgcolor             = strdup("");
	s->memory.fgcolor             = strdup("");
	s->memory.chart_bgcolor       = strdup("333333");
	s->memory.chart_color_free    = strdup("859900");
	s->memory.chart_color_total   = strdup("bbbb00");
	s->memory.chart_color_active  = strdup("dc322f");

	s->nprocs.hdcolor = strdup("dc322f");
	s->nprocs.bgcolor = strdup("");
	s->nprocs.fgcolor = strdup("");

	s->net.hdcolor                        = strdup("268bd2");
	s->net.bgcolor                        = strdup("");
	s->net.fgcolor                        = strdup("");
	s->net.inbound_chart_color_bgcolor    = strdup("859900");
	s->net.inbound_chart_color_pgcolor    = strdup("157ad2");
	s->net.inbound_text_fgcolor           = strdup("157ad2");
	s->net.outbound_chart_color_bgcolor   = strdup("859900");
	s->net.outbound_chart_color_pgcolor   = strdup("dc322f");
	s->net.outbound_text_fgcolor          = strdup("dc322f");

	s->volume.hdcolor       = strdup("cb4b16");
	s->volume.bgcolor       = strdup("");
	s->volume.fgcolor       = strdup("");
	s->volume.chart_width   = 7;
	s->volume.chart_bgcolor = strdup("dc322f");
	s->volume.chart_pgcolor = strdup("859900");

	s->time.hdcolor = strdup("859900");
	s->time.bgcolor = strdup("");
	s->time.fgcolor = strdup("");
	s->time.format  = strdup("%a %d %b %Y  %I:%M:%S %p");

	s->wifi.hdcolor = strdup("ff0000");
	s->wifi.bgcolor = strdup("");
	s->wifi.fgcolor = strdup("");
}

/*
 * TODO Create a settings_free() (or refactor to make easier)
 * I should have this - but this is cumbersome given the above setup.
 * Note the way I've setup ALL settings tracked by the settings construct,
 * the initial allocations for all dynamic stuff is managed here -- it's
 * initialized here (in settings_set_defaults() above) and then free()'d and
 * realloc'd on every change, managed below.
 * I'll chew more on how best to refactor but for now it's low priority.
void
settings_free(struct settings *s)
{
   free(s->display.wmname);
   free(s->display.bgcolor);
   free(s->display.fgcolor);
   free(s->display.font);

   free(s->battery.hdcolor);
   free(s->battery.str_plugged_in);
   free(s->battery.str_unplugged);
   ...
}
*/

/*
 * Key Match(?) then Set (KMS) macros used below (macro-fu or macro-goo?)
 * These macros help matching against the names of the settings and their
 * struct names, and rely on variables 'key' and 'value' outside their scope,
 * as well as a settings pointser 's'.
 *
 * They check if "s->#name" matches key, and if so set s->name = value, for
 * different types of values (strings, ints, then other complex types).
 */
#define KMS_STRING(name)                                                      \
   if (0 == strcmp( (key), (#name) )) {                                       \
      if (NULL == (s->name = strdup(value)))                                  \
         err(1, "%s: strdup failed for key %s", __FUNCTION__, (key));         \
      return true;                                                            \
   }

#define KMS_INT(name)                                                         \
   if (0 == strcmp( (key), (#name) )) {                                       \
      s->name = strtonum((value), -1, INT_MAX, &errstr);                      \
      if (errstr)                                                             \
         errx(1, "%s: bad value %s for key %s: %s", __FUNCTION__, (value), (key), errstr); \
      return true;                                                            \
   }

#define KMS_PADDING(name)                                                     \
   if (0 == strcmp( (key), (#name) )) {                                       \
      parse_padding(&s->name, (value));                                       \
      return true;                                                            \
   }

#define KMS_HEADER_STYLE(name)                                                \
   if (0 == strcmp( (key), (#name) )) {                                       \
      parse_header_style(&s->name, (value));                                  \
      return true;                                                            \
   }

/*
 * This method takes a settings and a key + value pair (as strings) and
 * will attempt to set the appropriate member of the settings object
 * accordingly. The assumption is:
 *    key   => is a string that's the actual name of the struct memeber
 *    value => a string appropriate for that value
 *
 * Note that key here is the string representation of the actual struct member.
 * So "s->window.bgcolor", the key is "window.bgcolor".
 * The macros above are meant to help this parsing logic and cut-down on the
 * boilerplate logic around it.
 *
 * Every member of the settings object should have a corresponding line
 * here. Each line checks if the passed name matches key. If so, it sets that
 * member to value and returns true. Otherwise it just continues. If no
 * matching key is found, it returns false.
 */
static bool
settings_set_one_keyvalue(struct settings *s,
                          const char *key, const char *value)
{
	const char *errstr;

	/* globals */
	KMS_STRING(widgets);

	/* font */
	KMS_STRING(font.desc);
	KMS_STRING(font.fgcolor);

	/* window */
	KMS_INT(window.x);
	KMS_INT(window.y);
	KMS_INT(window.w);
	KMS_INT(window.h);
	KMS_STRING(window.wname);
	KMS_STRING(window.bgcolor);

	/* gui */
	KMS_STRING(gui.widget_bgcolor);
	KMS_INT(gui.spacing);
	KMS_PADDING(gui.padding);
	KMS_INT(gui.padding.top);
	KMS_INT(gui.padding.right);
	KMS_INT(gui.padding.bottom);
	KMS_INT(gui.padding.left);
	KMS_PADDING(gui.margin);
	KMS_INT(gui.margin.top);
	KMS_INT(gui.margin.right);
	KMS_INT(gui.margin.bottom);
	KMS_INT(gui.margin.left);
	KMS_HEADER_STYLE(gui.header_style);

	/* battery */
	KMS_STRING(battery.hdcolor);
	KMS_STRING(battery.bgcolor);
	KMS_STRING(battery.fgcolor);
	KMS_STRING(battery.fgcolor_unplugged);
	KMS_INT(battery.chart_width);
	KMS_STRING(battery.chart_bgcolor);
	KMS_STRING(battery.chart_pgcolor);

	/* bright */
	KMS_STRING(bright.hdcolor);
	KMS_STRING(bright.bgcolor);
	KMS_STRING(bright.fgcolor);
	KMS_INT(bright.chart_width);
	KMS_STRING(bright.chart_bgcolor);
	KMS_STRING(bright.chart_pgcolor);

	/* cpus */
	KMS_STRING(cpus.hdcolor);
	KMS_STRING(cpus.bgcolor);
	KMS_STRING(cpus.fgcolor);
	KMS_STRING(cpus.chart_bgcolor);
	KMS_STRING(cpus.chart_color_system);
	KMS_STRING(cpus.chart_color_interrupt);
	KMS_STRING(cpus.chart_color_user);
	KMS_STRING(cpus.chart_color_nice);
	KMS_STRING(cpus.chart_color_spin);
	KMS_STRING(cpus.chart_color_idle);

	/* memory */
	KMS_STRING(memory.hdcolor);
	KMS_STRING(memory.bgcolor);
	KMS_STRING(memory.fgcolor);
	KMS_STRING(memory.chart_bgcolor);
	KMS_STRING(memory.chart_color_free);
	KMS_STRING(memory.chart_color_total);
	KMS_STRING(memory.chart_color_active);

	/* network */
	KMS_STRING(net.hdcolor);
	KMS_STRING(net.bgcolor);
	KMS_STRING(net.fgcolor);
	KMS_STRING(net.inbound_chart_color_bgcolor);
	KMS_STRING(net.inbound_chart_color_pgcolor);
	KMS_STRING(net.inbound_text_fgcolor);
	KMS_STRING(net.outbound_chart_color_bgcolor);
	KMS_STRING(net.outbound_chart_color_pgcolor);
	KMS_STRING(net.outbound_text_fgcolor);

	/* nprocs */
	KMS_STRING(nprocs.hdcolor);
	KMS_STRING(nprocs.bgcolor);
	KMS_STRING(nprocs.fgcolor);

	/* volume */
	KMS_STRING(volume.hdcolor);
	KMS_STRING(volume.bgcolor);
	KMS_STRING(volume.fgcolor);
	KMS_INT(volume.chart_width);
	KMS_STRING(volume.chart_bgcolor);
	KMS_STRING(volume.chart_pgcolor);

	/* time */
	KMS_STRING(time.hdcolor);
	KMS_STRING(time.bgcolor);
	KMS_STRING(time.fgcolor);
	KMS_STRING(time.format);

	/* wifi */
	KMS_STRING(wifi.hdcolor);
	KMS_STRING(wifi.bgcolor);
	KMS_STRING(wifi.fgcolor);

	return false;
}

/* Given a settings and a "key=value", set the appropriate settings member */
static void
settings_set_keyvalue(struct settings *s, const char * const keyvalue)
{
	char *key, *value;
	if (!parse_keyvalue(keyvalue, &key, &value))
		errx(1, "failed to parse key & value from '%s'", keyvalue);

	if (!settings_set_one_keyvalue(s, key, value))
		errx(1, "unknown key in in '%s'", keyvalue);

	free(key);
	free(value);
}

/* Parses an argc/argv pair and updates a settings appropriately */
static void
settings_parse_cmdline(struct settings *s, int argc, char * const argv[])
{
	const char *errstr;
	char *keyvalue;
	int ch;

	while (-1 != (ch = getopt(argc, argv, SWITCHES))) {
		switch (ch) {
		case 'c':
			parse_header_style(&s->gui.header_style, optarg);
			break;
		case 'f':
			free(s->font.desc);
			s->font.desc= strdup(optarg);
			if (NULL == s->font.desc)
				err(1, "strdup failed for '%s'", optarg);
			break;
		case 'F':
			/* Already handled this in settings_init() */
			break;
		case 'h':
			s->window.h = strtonum(optarg, -1, INT_MAX, &errstr);
			if (errstr)
				errx(1, "bad height '%s': %s", optarg, errstr);
			break;
		case 'H':
			print_usage();
			break;
		case 'm':
			parse_padding(&s->gui.margin, optarg);
			break;
		case 'p':
			parse_padding(&s->gui.padding, optarg);
			break;
		case 's':
			s->gui.spacing = strtonum(optarg, 0, INT_MAX, &errstr);
			if (errstr)
				errx(1, "bad spacing '%s': %s", optarg, errstr);
			break;
		case 'S':
			if (NULL == (keyvalue = strdup(optarg)))
				err(1, "strdup failed for '%s'", optarg);

			settings_set_keyvalue(s, keyvalue);
			free(keyvalue);
			break;
		case 't':
			free(s->time.format);
			if (NULL == (s->time.format = strdup(optarg)))
				err(1, "strdup failed for '%s'", optarg);
			break;
		case 'w':
			s->window.w = strtonum(optarg, -1, INT_MAX, &errstr);
			if (errstr)
				errx(1, "bad width '%s': %s", optarg, errstr);
			break;
		case 'W':
			free(s->widgets);
			if (NULL == (s->widgets = strdup(optarg)))
				err(1, "strdup failed for '%s'", optarg);
			break;
		case 'x':
			s->window.x = strtonum(optarg, 0, INT_MAX, &errstr);
			if (errstr)
				errx(1, "bad x '%s': %s", optarg, errstr);
			break;
		case 'y':
			s->window.y = strtonum(optarg, -1, INT_MAX, &errstr);
			if (errstr)
				errx(1, "bad y '%s': %s", optarg, errstr);
			break;
		default:
			print_usage();
		}
	}

	argc -= optind;

	/*
	* XXX We should have at most 1 - the theme (which is always at the end),
	* and that theme is extracted below in the settings_init() method.
	*/
	if (1 < argc)
		print_usage();
}

/*
 * (Re)read the config file and update the passed settings accordingly.
 * Note this respects if oxbar was run with a theme, and will reload that
 * additional part (and no other themes).
 * The config file and theme are loaded in the settings object at startup,
 * and this retrieves them from there.
 * XXX Be aware! This file can be called MORE THAN ONCE via a SIGHUP at
 * runtime (hence the name).
 */
void
settings_reload_config(struct settings *s)
{
	char   theme_name[100];
	size_t length, linenum = 0;
	char  *line;
	FILE  *fin;
	bool   parse_lines = true;
	bool   found_theme = false;

	fin = fopen(s->config_file, "r");
	if (NULL == fin) {
		if (NULL == s->theme)
			return;
		else
			errx(1, "specified theme '%s' but i can't read '%s'",
			     s->theme, s->config_file);
	}

	/* start reading & parsing file */
	while (!feof(fin)) {

		/* read next line */
		line = fparseln(fin, &length, &linenum, NULL, 0);
		if (NULL == line) {
			if (ferror(fin)) {
				err(1, "failed to parse line %zd in %s",
				    linenum, s->config_file);
			} else
				break;
		}

		/* skip blank lines */
		char *copy = line;
		copy += strspn(copy, " \t\n");
		if ('\0' == copy[0]) {
			free(line);
			continue;
		}

		/* do we have a new 'theme' section starting? */
		if (1 == sscanf(line, " [%100[-a-zA-Z0-9]] ", theme_name)) {
			/* yes - does it match specified theme? */
			if (NULL != s->theme
			&&  0 == strcmp(theme_name, s->theme)) {
				found_theme = true;
				parse_lines = true;
			} else
				parse_lines = false;
		} else {
			/*
			 * not a theme line - go ahead and parse the line IF
			 * we're either at the start of the file (before any
			 * theme) or we've entered a theme section matching
			 * the theme specified on the command line
			 */
			if (parse_lines)
				settings_set_keyvalue(s, line);
		}

		free(line);
	}

	fclose(fin);

	/* if we never found a theme, but had one specified, that's an error */
	if (NULL != s->theme && !found_theme)
		errx(1, "failed to find a theme named '%s' in %s",

	s->theme, s->config_file);
}

/*
 * This wraps-up all the above logic and is meant as the main entry point to
 * loading an initial settings. After this, only settings_reload_config()
 * would be expected to be called.
 */
void
settings_init(struct settings *settings, int argc, char *argv[])
{
	char *config_file = NULL;
	char *theme = NULL;

	/* first determine config file and theme */
	get_config_and_theme(argc, argv, &config_file, &theme);
	if (NULL == config_file)
		config_file = get_default_config();

	/* store the config file & theme in the settings object */
	settings->config_file = config_file;
	settings->theme = theme;

	/* load defaults, then load config, and then parse command line */
	settings_set_defaults(settings);
	settings_reload_config(settings);
	settings_parse_cmdline(settings, argc, argv);
}
