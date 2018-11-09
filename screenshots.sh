#!/bin/sh

# TODO disclaimer (images not here bc yada, other tools yada, ...)

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
WOPTS="-w 2000 -S window.wname=oxshort"
WBG="-S window.bgcolor=fff"

# islands
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -F sample.oxbar.conf islands &
generate_load
import -window oxshort images/theme-islands.png
pkill oxbar

# islands-colorful
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -F sample.oxbar.conf islands-colorful &
generate_load
import -window oxshort images/theme-islands-colorful.png
pkill oxbar

# minimal
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -F sample.oxbar.conf minimal &
generate_load
import -window oxshort images/theme-minimal.png
pkill oxbar

# ryan
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} ${WBG} -w 2200 -F sample.oxbar.conf ryan &
generate_load
import -window oxshort images/theme-ryan.png
pkill oxbar

# xstatbar
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar ${WOPTS} -F sample.oxbar.conf xstatbar &
generate_load
import -window oxshort images/theme-xstatbar.png
pkill oxbar

# idea
# TOP LEVEL / WHAT I WANT
# - run a script to generate all screenshots
# - should include full version + oxbar-only version
# - oxbar only version should be large-ish
# - output should be list of images
# - list of images should be easily testable with a simple `feh *.png`
#
# How do i do that?
# RUN THROUGH CONFIGS / THEMES
# - set background + run theme (these are coupled)
# - launch apps to make charts busy
# - wait 60 seconds
# - take screenshot(s)
#
# Config is
# list of < theme , background > pairs
#configs="
#minimal           foo
#islands           bar
#islands-colorful  baz
#xstatbar          fooxstat
#pastel-dense      foopastel
#ryan              fooryan
#"
#
#theme="null"
#bgimage="null"
#
#parseconfig()
#{
#   theme="$1"
#   bgimage="$2"
#}
#
#IFS='
#'
#for i in $configs
#do
#   echo "line: \"$i\""
#   IFS=' '
#   parseconfig $i
#   echo "theme = '$theme' bgimage = '$bgimage'"
#   oxbar -F sample.oxbar.conf $theme &
#   sleep 2
#   import -window oxbar $theme.png
#   pkill oxbar
#   IFS='
#   '
#done
