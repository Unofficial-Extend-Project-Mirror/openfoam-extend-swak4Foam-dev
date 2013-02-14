#! /bin/bash

if [ -z $3 ];then
    echo "3 Necessary parameters:  SWAK_PYTHON_INCLUDE SWAK_PYTHON_LINK SWAK_PYTHON_INTEGRATION_SUFFIX"
    exit 42
fi

export SWAK_PYTHON_INCLUDE=$1
export SWAK_PYTHON_LINK=$2
export SWAK_PYTHON_INTEGRATION_SUFFIX=$3

echo "Compiling special python library $SWAK_PYTHON_INTEGRATION_SUFFIX"
echo "Include: $SWAK_PYTHON_INCLUDE"
echo "Link: $SWAK_PYTHON_LINK"

thisFile=$0
if [ $thisFile == "bash" ]
then
    # for bash-versions that don't know who they're called
    thisFile=${BASH_SOURCE[0]}
fi

# make path absolute
thisFile=`python -c "from os import path; print path.abspath('$thisFile')"`

PYTHON_LIBDIR=`dirname $thisFile`"/../Libraries/swakPythonIntegration"
unset thisFile

echo "   Cleaning $PYTHON_LIBDIR"
wclean $PYTHON_LIBDIR
echo "   Building $PYTHON_LIBDIR"
wmake libso $PYTHON_LIBDIR
