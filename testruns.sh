#!/bin/sh -x
# This should be run via `make testruns`
# It's a list of sample configurations to run-throug/test oxbar with.

trap "" INT TERM

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
./oxbar -F sample.oxbar.conf islands
