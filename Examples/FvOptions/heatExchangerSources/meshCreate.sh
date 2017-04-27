#!/bin/sh

blockMesh -region air
blockMesh -region porous

topoSet -region air -dict system/topoSetDict.1

createBaffles -region air -overwrite

topoSet -region air -dict system/topoSetDict.2

rm -rf constant/air/polyMesh/sets
