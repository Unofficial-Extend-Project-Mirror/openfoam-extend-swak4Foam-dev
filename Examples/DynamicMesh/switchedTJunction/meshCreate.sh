#!/bin/sh

rm -rf constant/polyMesh/
blockMesh
rm constant/cellToRegion.gz

transformPoints -rotate "((1 0 0) (0 -1 0))"

topoSet -constant

# split the mesh to generate the ACMI coupled patches
createBaffles -overwrite

mkdir 0
cp 0.org/valveField 0
funkyWarpMesh -dictExt outletDown -allowFunctionObjects -overwrite
rm -r 0
