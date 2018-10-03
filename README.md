# oxbar

Having a small, concise, visual indicator of numerous system stats is often
useful on a workstation for developers. While there are many options for Linux,
there are few that work well with OpenBSD naively. This project aims to offer
a solution specifically for OpenBSD.

Also it's a replacement for my old, very crappy
[xstatbar](https://ryanflannery.net/hacking/xstatbar/).

# status

This is still pretty rough, but mostly stable and usable.
I'm only publishing now to github to track my work work and share with a few
other folk on irc who seem interested.

There is much refactoring needed -- this is known. I'm still organizing key
features and really building a PoC at this point.

Note there's no `man` page yet, or `-h` flag to provide useful help
information. That said, this is quite configurable. It supports all the
command line flags `xstatbar` did and much more... Most aspects of the widgets
can be configured through `-S widget.name=value`. If you take a look at
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
   * Types of stats supported:
      * Battery/AC status, simple progress bar, time remaining
      * Volume level w/ progress bar (mute status forthcoming )
      * Number of processes (current total # processes only as of now)
      * Memory usage & breakdown (current only as of now)
      * CPUs usage & breakdown w/ charts
      * Current date/time (is that a status?)

# screenshots

<a href="/images/default.png?raw=true">
![](images/default.png)
</a>

# usage

See 'ol `xstatbar` for the basics of height/width and placement (or see
`settings.c`). Otherwise wait until I can document this.

## i liked xstatbar's look

You can achieve that basically with:
```bash
oxbar -y 0 -p 0 -f "fixed 16px" -S display.bgcolor=#000000
```

The only thing lacking is each individual cpu's states being shown. That is
forthcoming.

# outstanding stuff, prioritized

Roughly in order or my priority...

   * cleanup widget interface - remove state from the static methods and make
     them a proper component
   * fully support right-to-left rendering (properly)... If i do this right,
     i'll also get vertical rendering (so you could have a sidebar)
   * DETERMINE "MUTE" STATUS FOR VOLUME
   * add stats & widgets for temperature sensors and... weather
   * icons in the display could help condense it further
