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
    echo "Bison already installed/compiled"
else
    bisonTarball=bison-3.0.4.tar.gz
    if [ -e  $requirementsDir/sources/$bisonTarball ];
    then
	echo "$bisonTarball already downloaded"
    else
	(cd $requirementsDir/sources; wget http://ftp.gnu.org/gnu/bison/$bisonTarball)
    fi
    echo "Untarring bison-sources"
    ( cd $requirementsDir/compilation; tar xzf $requirementsDir/sources/$bisonTarball )

    ( cd $requirementsDir/compilation/bison-3.0.4 ; ./configure --prefix=$requirementsDir; make; make install )

fi

if [ -e $requirementsDir/bin/bison ];
then
    echo "Bison successfully compiled"
else
    echo
    echo "Bison not compiled. Check output"
    echo
fi

if [ -e $requirementsDir/bin/lua ];
then
    echo "Lua already installed/compiled"
else
    luaTarball=lua-5.3.4.tar.gz
    if [ -e  $requirementsDir/sources/$luaTarball ];
    then
	echo "$luaTarball already downloaded"
    else
	(cd $requirementsDir/sources; wget https://www.lua.org/ftp/$luaTarball)
    fi
    echo "Untarring lua-sources"
    ( cd $requirementsDir/compilation; tar xzf $requirementsDir/sources/$luaTarball )

    (
        cd $requirementsDir/compilation/lua-5.3.4;
        sed -i bak -e "s|/usr/local|$requirementsDir|" Makefile ;
        sed -i bak -e "s|/usr/local|$requirementsDir|" src/luaconf.h ;
        if [[ $(uname) == "Darwin" ]];
        then
            make macosx
        else
            make linux
        fi
        make install
    )
fi

if [ -e $requirementsDir/bin/lua ];
then
    echo "Lua successfully compiled"
else
    echo
    echo "Lua not compiled. Check output"
    echo
fi

echo
echo "Add $requirementsDir/bin to PATH-variable. The ./Allwmake-script will use this directory automatically"
echo
