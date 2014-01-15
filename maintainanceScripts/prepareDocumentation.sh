#! /bin/bash

export GTAGSCONF=`pwd`/gtags.conf

gtags -v --statistics
doxygen
