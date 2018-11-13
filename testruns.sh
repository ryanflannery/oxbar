#!/bin/sh -x
# This should be run via `make testruns`
# It's a list of sample configurations to run-throug/test oxbar with.

trap "" INT TERM

# easy bg colors for testing layouts
GSIZE="-x 100 -y 100 -w 1000"
GCOLORS="-S window.bgcolor=ff0000 -S gui.widget_bgcolor=ffffff -c none"
GWIDGETS="nprocs nprocs | nprocs nprocs > nprocs nprocs"

# test SIGHUP reloading config
./oxbar -F sample.oxbar.conf &
pid=$!
/bin/sleep 1 && kill -HUP $pid
/bin/sleep 1 && kill -HUP $pid
/bin/sleep 1 && kill -HUP $pid
/bin/sleep 1 && kill -HUP $pid
kill $pid

# test padding/margins
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 10 -m 20
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 10 -m 0
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 10 -m "0 0 0 0"
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 10 -m "0 20 20 0"
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 0  -m 10
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p "0 0 0 0" -m 20
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p "40 0 20 5" -m 20

# test widget spacing
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 10 -m 10 -s 0
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 10 -m 10 -s 10
./oxbar $GSIZE $GCOLORS -W "$GWIDGETS" -p 10 -m 10 -s 20

# test setting colored headers
./oxbar $GCOLORS -y 100 -p 20 -m 20 -c none
./oxbar $GCOLORS -y 100 -p 20 -m 20 -c above
./oxbar $GCOLORS -y 100 -p 20 -m 20 -c below

# test y/w/h morphing
./oxbar $GCOLORS -y 0   -w 400 -h 50 -W "time"
./oxbar $GCOLORS -y 0   -w 400 -h -1 -W "time"
./oxbar $GCOLORS -y 0   -w -1  -h 50 -W "time"
./oxbar $GCOLORS -y 0   -w -1  -h -1 -W "time"
./oxbar $GCOLORS -y -1  -w 400 -h 50 -W "time"
./oxbar $GCOLORS -y -1  -w 400 -h -1 -W "time"
./oxbar $GCOLORS -y -1  -w -1  -h 50 -W "time"
./oxbar $GCOLORS -y -1  -w -1  -h -1 -W "time"

# test display ordering
./oxbar -y 0 -W "time time"
./oxbar -y 0 -W "< time time"
./oxbar -y 0 -W "| time time"
./oxbar -y 0 -W "> time time"
./oxbar -y 0 -W "time | time > time"
./oxbar -y 0 -W "< | > time"
./oxbar -y 0 -W "< time time | time time > time time"

# test config file & themes
./oxbar -F sample.oxbar.conf
./oxbar -F sample.oxbar.conf FooBar       # should fail
./oxbar -F sample.oxbar.conf top
./oxbar -F sample.oxbar.conf bottom
./oxbar -F sample.oxbar.conf islands
./oxbar -F sample.oxbar.conf islands-colorful
./oxbar -F sample.oxbar.conf minimal
./oxbar -F sample.oxbar.conf ryan
./oxbar -F sample.oxbar.conf pastels
./oxbar -F sample.oxbar.conf xstatbar
