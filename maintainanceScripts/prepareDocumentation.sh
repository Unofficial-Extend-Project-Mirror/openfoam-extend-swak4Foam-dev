#! /bin/sh

export GTAGSCONF=`pwd`/gtags.conf

gtags -v --statistics
doxygen
