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
    BISONVER=3.4
#    BISONVER=3.2
#    BISONVER=3.3

    bisonTarball=bison-$BISONVER.tar.gz
    if [ -e  $requirementsDir/sources/$bisonTarball ];
    then
        echo "$bisonTarball already downloaded"
    else
        (cd $requirementsDir/sources; wget http://ftp.gnu.org/gnu/bison/$bisonTarball)
    fi
    echo "Untarring bison-sources"
    ( cd $requirementsDir/compilation; tar xzf $requirementsDir/sources/$bisonTarball )

    ( cd $requirementsDir/compilation/bison-$BISONVER ; ./configure --prefix=$requirementsDir; make; make install )

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
    LUA_VERSION=5.3.5
    luaTarball=lua-$LUA_VERSION.tar.gz
    if [ -e  $requirementsDir/sources/$luaTarball ];
    then
	echo "$luaTarball already downloaded"
    else
	(cd $requirementsDir/sources; wget https://www.lua.org/ftp/$luaTarball)
    fi
    echo "Untarring lua-sources"
    ( cd $requirementsDir/compilation; tar xzf $requirementsDir/sources/$luaTarball )

    (
        cd $requirementsDir/compilation/lua-$LUA_VERSION;
        sed -i bak -e "s|/usr/local|$requirementsDir|" Makefile
        sed -i bak -e "s|/usr/local|$requirementsDir|" src/luaconf.h
        sed -i bak -e "s|CC= gcc -std=gnu99|CC= gcc -fPIC -std=gnu99|" src/Makefile
        if [[ $(uname) == "Darwin" ]];
        then
            make macosx
        else
            make linux
        fi
        make install
    )
    if [ -e $requirementsDir/bin/lua ];
    then
       LUAROCKS_VERSION=3.2.1
       luarocksTarball=luarocks-$LUAROCKS_VERSION.tar.gz
       if [ -e  $requirementsDir/sources/$luarocksTarball ];
       then
	   echo "$luarocksTarball already downloaded"
       else
	   (cd $requirementsDir/sources; wget https://luarocks.org/releases/$luarocksTarball)
       fi
       echo "Untarring luarocks-sources"
       ( cd $requirementsDir/compilation; tar xzf $requirementsDir/sources/$luarocksTarball )
       export PATH=$requirementsDir/bin:$PATH
       (
           cd $requirementsDir/compilation/luarocks-$LUAROCKS_VERSION
           ./configure --prefix=$requirementsDir --with-lua=$requirementsDir
           make bootstrap
       )
       # Move here because install in luarocks-sources doesn't install packages correctly
       luarocks install luaprompt
    fi
    echo "If there were problems during compilation install the readline-devel package (name may be different on platforms)"
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
