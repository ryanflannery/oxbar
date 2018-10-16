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
./oxbar -F sample.oxbar.conf Top                # should fail
./oxbar -F sample.oxbar.conf TopVersion
./oxbar -F sample.oxbar.conf BottomVersion
