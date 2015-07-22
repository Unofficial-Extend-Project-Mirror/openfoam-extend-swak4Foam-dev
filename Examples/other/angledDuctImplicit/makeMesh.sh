#! /bin/sh

rm -f constant/polyMesh/*.gz
rm -rf constant/polyMesh/sets

blockMesh

topoSet

setsToZones
