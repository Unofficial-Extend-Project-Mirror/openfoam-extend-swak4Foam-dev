#! /usr/bin/env bash

rm -rf constant/polyMesh/*.gz constant/polyMesh/sets

blockMesh

snappyHexMesh -overwrite

topoSet
setsToZones -noFlipMap

changeDictionary -constant

mergeOrSplitBaffles -split -overwrite
