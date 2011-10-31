#! /bin/sh

rm -rf 0
cp -r 0.orig 0

blockMesh

setSet -batch makeFaceSet.setSet

setsToZones -noFlipMap

createBaffles jump1 jump1 -overwrite
createBaffles jump2 jump2 -overwrite
