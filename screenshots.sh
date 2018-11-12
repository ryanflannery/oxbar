#!/bin/sh

# NOTE: This references some images in my personal directory that are used
# as background images for some of the screenshots. They are not included
# with the git repository as they are not mine and I don't have permission
# to redistribute.

# oxbar -F sample.oxbar.conf -w 1700 [theme] &           # run oxbar
# feh --bg-fill /home/ryan/images/ice-mountain-lake.jpg  # set background
# import -window root  foo.png                           # grab whole screen
# import -window oxbar foo.png                           # grab just oxbar
# feh foo.png                                            # to view / best way

function generate_load
{
   gitrepo="https://github.com/openbsd/src"
   cwd=`pwd`

   # start w/ some quiet time
   sleep 5

   # start some cpu & network activity
   ubench -c > /dev/null 2> /dev/null &
   tmp=`mktemp -d`
   cd $tmp && git clone $gitrepo > /dev/null 2> /dev/null &
   sleep 20

   # start some memory activity
   ubench -m > /dev/null 2> /dev/null &
   sleep 25

   # 50 seconds in - kill cpu & memory tests
   pkill ubench
   # kill network test
   pkill git

   # 5 seconds of quiet
   sleep 5

   # remove git temp dir & return to old directory
   rm -rf $tmp
   cd $cwd
}

# kill all running instances
pkill oxbar
WOPTS="-w 2200 -S window.wname=oxshort"
WBG="-S window.bgcolor=fff"

# default
feh --bg-fill ~/images/spiff.jpg
./oxbar ${WOPTS} &
generate_load
import -screen -window oxshort images/theme-default.png
pkill oxbar

# islands
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -F sample.oxbar.conf islands &
generate_load
import -screen -window oxshort images/theme-islands.png
pkill oxbar

# islands-colorful
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -F sample.oxbar.conf islands-colorful &
generate_load
import -screen -window oxshort images/theme-islands-colorful.png
pkill oxbar

# minimal
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -F sample.oxbar.conf minimal &
generate_load
import -screen -window oxshort images/theme-minimal.png
pkill oxbar

# pastel-dense
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -F sample.oxbar.conf pastel-dense &
generate_load
import -screen -window oxshort images/theme-pastel-dense.png
pkill oxbar

# ryan
feh --bg-fill ~/images/spiff.jpg
./oxbar ${WOPTS} -w 2500 -F sample.oxbar.conf ryan &
generate_load
import -screen -window oxshort images/theme-ryan.png
pkill oxbar

# xstatbar
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} -F sample.oxbar.conf xstatbar &
generate_load
import -screen -window oxshort images/theme-xstatbar.png
pkill oxbar
