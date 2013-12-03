#! /bin/sh

rm -rf 0
cp -r 0.orig 0

blockMesh

funkySetFields -time 0
