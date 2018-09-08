# Status

Note: This is very rough right now. I'm only publishing now to github to track
my work work and share with a few other folk on irc who seem interested.
You're welcome to try it out, but it's very unstable, unconfigurable, and
rough at this point. Collaboration welcomed.

# Motivation

Having a small, concise, visual indicator of numerous system stats is often
useful on a workstation for developers. While there are many options for Linux,
there are few that work well with OpenBSD natively. This project aims to offer
a solution specifically for OpenBSD.

# Goals

Proivde a native, well supported, X11 stats bar for OpenBSD with that can
**quickly and succinctly** summarize key stats of a workstation that a
developer may be interested in. It **should not aim to answer every question**,
but rather provide a clean, easy-to-read visualization of possible issues and
where to investigate further.

It should support configuration/knobs where appropriate (say
appearance) and up-to-date, thorough, documentation is mandatory.

# Samples & Features

Stats currently supported are:

   * CPUs usage & breakdown (historical)
   * Memory usage & breakdown (historical)
   * Netowrk usage & breakdown (historical)
   * PF usage & breakdown (historical)
   * Number of processes (historical)
   * Battery/AC status (current only)
   * Volume/mute status (current only)
   * Date/time (current only... obviously)

# Usage

# Status

# Plan / TODO

   * Determine "mute" status in volume (stats/volume.c)...this has proven challenging
   * Support configuration (time span, XXX)
   * Determine aesthetic configuration options

