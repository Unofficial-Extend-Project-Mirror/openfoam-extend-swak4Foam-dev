#! /bin/sh

pyFoamClearCase.py .

rm -rf 0
cp -r 0.orig 0

blockMesh

topoSet
setsToZones

funkySetFields -time 0
funkySetLagrangianField -time 0
