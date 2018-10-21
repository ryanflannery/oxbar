#!/bin/sh -x
# This should be run via `make testruns`
# It's a list of sample configurations to run-throug/test oxbar with.

trap "" INT TERM

# test SIGHUP reloading config
./oxbar -F sample.oxbar.conf Top &
pid=$!
/bin/sleep 1 && kill -HUP $pid
/bin/sleep 1 && kill -HUP $pid
/bin/sleep 1 && kill -HUP $pid
/bin/sleep 1 && kill -HUP $pid
kill $pid

# test y/w/h morphing
./oxbar -S "display.bgcolor=ff0000" -y 0   -w 400 -h 50 -W "time"
./oxbar -S "display.bgcolor=ff0000" -y 0   -w 400 -h -1 -W "time"
./oxbar -S "display.bgcolor=ff0000" -y 0   -w -1  -h 50 -W "time"
./oxbar -S "display.bgcolor=ff0000" -y 0   -w -1  -h -1 -W "time"
./oxbar -S "display.bgcolor=ff0000" -y -1  -w 400 -h 50 -W "time"
./oxbar -S "display.bgcolor=ff0000" -y -1  -w 400 -h -1 -W "time"
./oxbar -S "display.bgcolor=ff0000" -y -1  -w -1  -h 50 -W "time"
./oxbar -S "display.bgcolor=ff0000" -y -1  -w -1  -h -1 -W "time"

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
./oxbar -F sample.oxbar.conf Top
./oxbar -F sample.oxbar.conf Bottom
./oxbar -F sample.oxbar.conf xstatbar
