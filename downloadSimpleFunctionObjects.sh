#! /bin/sh

if [ -d .svn ]; then
    echo "Can't do this because this directory is under SVN-control"
    exit 0
fi

if [ ! -d Libraries ]; then
    echo "Can't find directory Libraries. Called from the wrong place"
    exit 0
fi

if [ -d Libraries/simpleFunctionObjects ]; then
    echo " Libraries/simpleFunctionObjects already downloaded. Updating it"
    svn update Libraries/simpleFunctionObjects
else
    echo "Downloading to Libraries/simpleFunctionObjects"
    svn checkout https://openfoam-extend.svn.sourceforge.net/svnroot/openfoam-extend/trunk/Breeder_1.6/libraries/simpleFunctionObjects Libraries/simpleFunctionObjects
fi
