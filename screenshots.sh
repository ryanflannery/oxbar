#!/bin/sh

# TODO disclaimer (images not here bc yada, other tools yada, ...)

# oxbar -F sample.oxbar.conf -w 1700 [theme] &           # run oxbar
# feh --bg-fill /home/ryan/images/ice-mountain-lake.jpg  # set background
# import -window root  foo.png                           # grab whole screen
# import -window oxbar foo.png                           # grab just oxbar
# feh foo.png                                            # to view / best way

function generate_load
{
   echo "in generate load"
   cwd=`pwd`
   tmp=`mktemp -d`

   cd $tmp && git clone https://github.com/openbsd/src > /dev/null &
   ubench -c &
   sleep 25
   ubench -m &
   sleep 35
   pkill ubench
   pkill git

   rm -rf $tmp
   cd $cwd
}

# kill all running instances
pkill oxbar

# islands
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar -F sample.oxbar.conf islands &
generate_load
import -window root  images/islands_root.png
import -window oxbar images/islands_oxbar.png
pkill oxbar

# islands-colorful
feh --bg-fill ~/images/ice-mountain-lake.jpg
./oxbar -F sample.oxbar.conf islands-colorful &
generate_load
import -window root  images/islands-colorful_root.png
import -window oxbar images/islands-colorful_oxbar.png
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
