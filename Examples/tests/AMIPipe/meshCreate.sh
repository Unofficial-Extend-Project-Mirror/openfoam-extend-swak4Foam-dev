#! /usr/bin/env bash

rm -rf constant/polyMesh/*.gz constant/polyMesh/sets

blockMesh

snappyHexMesh -overwrite

rm constant/level0Edge.gz constant/polyMesh/level0Edge.gz
rm 0/*Level.gz
rm constant/polyMesh/*Level.gz

pyFoamClearEmptyBoundaries.py .

topoSet
setsToZones -noFlipMap

changeDictionary -constant

mergeOrSplitBaffles -split -overwrite
