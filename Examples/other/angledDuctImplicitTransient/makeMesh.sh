#! /bin/sh
 
blockMesh

cp system/faceSetDict.1 system/faceSetDict
faceSet
cp system/faceSetDict.2 system/faceSetDict
faceSet

cp system/cellSetDict.1 system/cellSetDict
cellSet
cp system/cellSetDict.2 system/cellSetDict
cellSet

setsToZones
