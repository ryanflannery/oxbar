# oxbar

oxbar is a small, concise system monitor that displays various stats on an X
window that's immovable and outside tabbing in most window managers. It's built
on and for OpenBSD, and is a replacement for my old
[xstatbar](https://ryanflannery.net/hacking/xstatbar/). It supports FreeType
font rendering, alpha transparency, and is highly configurable.

It's motivated mostly by the frequent complaints I get about my old xstatbar
and its many shortcomings/hacks. Shaming works yo.

oxbar is released under an
[ISC license](https://github.com/ryanflannery/oxbar/blob/master/LICENSE)

# status

oxbar is still under active development, but is now stable and usable. There's
no documentation yet (`man` page or usable `-h` info). That's forthcoming, once
development settles, and likely soon.

It supports all the command line flags `xstatbar` did and much more... most
aspects of the widgets can be configured through `-S widget.name=value` flags.
If you take a look at
[settings.c](https://github.com/ryanflannery/oxbar/blob/master/settings.c)
you'll see what they all are.

Once development settles I'll focus on adding documentation and cleaning that
interface up.

# current features

   * True transparency support with a compositing window manager
     (I recommend compton in ports over xcompmgr in base)
   * FreeType fonts rendered beautifully & easily w/ pango
   * Configurable display (all colors, text, spacing, etc)
   * Left/Right/Center aligned widgets (and any combination of those)
   * Widgets can be configured on the command line
   * Types of stats supported:
      * Battery/AC status, simple progress bar, time remaining
      * Volume level w/ progress bar (mute status forthcoming )
      * Number of processes (current total # processes only as of now)
      * Memory usage & breakdown (current only as of now)
      * CPUs usage & breakdown w/ charts
      * Current date/time (is that a status?)

# screenshots

![](images/default.png)

[View (raw) Image](images/default.png?raw=true)

# usage

See 'ol `xstatbar` for the basics of height/width and placement (or see
`settings.c`). Otherwise wait until I can document this.

## i liked xstatbar's look

You can achieve that with:
```bash
oxbar -y 0 -p 0 -f "fixed 16px" -S display.bgcolor=#000000
```

The only thing lacking is each individual cpu's states being shown. That is
forthcoming.

# outstanding stuff, prioritized

Roughly in order or my priority...

   * gui: settle padding and margin mess
   * document! man page and -h flags
   * gui: add 'cpulong' widget, which mimics old xstatbar behavior
   * gui: build test driver for gui/ components (for my sake)
   * stats/gui: support different update frequency per widget
   * gui: icons in the display could help condense it further
   * stats: support "mute" status in volume (this is non-trivial)
   * stats: add tracker for hw.sensors
   * stats: add a weather component...i like weather
   * gui: support vertical rendering (a sidebar!)
