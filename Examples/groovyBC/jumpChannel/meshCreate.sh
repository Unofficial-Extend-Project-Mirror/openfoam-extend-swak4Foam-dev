#! /bin/sh

rm -rf 0
cp -r 0.orig 0

blockMesh

setSet -batch makeFaceSet.setSet

setsToZones -noFlipMap

createBaffles -overwrite
