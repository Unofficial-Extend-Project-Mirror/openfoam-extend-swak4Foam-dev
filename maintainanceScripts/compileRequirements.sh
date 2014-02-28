#! /bin/bash

echo
echo "This script is rather crude. If it doesn't work try the steps by hand. And be sure to run it from the root of the swak-installation (as ./maintainanceScripts/compileRequirements.sh). Otherwise it probably won't work"
echo

swakDir=$(pwd)
requirementsDir=$swakDir/privateRequirements

mkdir -p $requirementsDir/sources
mkdir -p $requirementsDir/compilation

if [ -e $requirementsDir/bin/bison ];
then
    echo "Bison alread installed/compiled"
else
    bisonTarball=bison-2.7.1.tar.gz
    if [ -e  $requirementsDir/sources/$bisonTarball ];
    then
	echo "$bisonTarball already downloaded"
    else
	(cd $requirementsDir/sources; wget http://ftp.gnu.org/gnu/bison/bison-2.7.1.tar.gz)
    fi
    echo "Untarring bison-sources"
    ( cd $requirementsDir/compilation; tar xzf $requirementsDir/sources/$bisonTarball )

    ( cd $requirementsDir/compilation/bison-2.7.1 ; ./configure --prefix=$requirementsDir; make; make install )

fi

if [ -e $requirementsDir/bin/bison ];
then
    echo "Bison successfully compiled"
else
    echo
    echo "Bison not compiled. Check output"
    echo
fi

echo
echo "Add $requirementsDir/bin to PATH-variable. The ./Allwmake-script will use this directory automatically"
echo
