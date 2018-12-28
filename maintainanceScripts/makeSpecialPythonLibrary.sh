#! /bin/bash

if [ -z $3 ];then
    echo "3 Necessary parameters:  SWAK_PYTHON2_INCLUDE SWAK_PYTHON2_LINK SWAK_PYTHON2_INTEGRATION_SUFFIX"
    exit 42
fi

export SWAK_PYTHON2_INCLUDE=$1
export SWAK_PYTHON2_LINK=$2
export SWAK_PYTHON2_INTEGRATION_SUFFIX=$3

echo "Compiling special python library $SWAK_PYTHON2_INTEGRATION_SUFFIX"
echo "Include: $SWAK_PYTHON2_INCLUDE"
echo "Link: $SWAK_PYTHON2_LINK"

thisFile=$0
if [ $thisFile == "bash" ]
then
    # for bash-versions that don't know who they're called
    thisFile=${BASH_SOURCE[0]}
fi

# make path absolute
thisFile=`python -c "from os import path; print path.abspath('$thisFile')"`

PYTHON2_LIBDIR=`dirname $thisFile`"/../Libraries/languageIntegration/swakPythonIntegration"
unset thisFile

echo "   Cleaning $PYTHON2_LIBDIR"
wclean $PYTHON2_LIBDIR
echo "   Building $PYTHON2_LIBDIR"
wmake libso $PYTHON2_LIBDIR
