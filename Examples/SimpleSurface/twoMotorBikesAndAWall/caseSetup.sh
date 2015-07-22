#!/bin/sh

pyFoamClearCase.py .

rm -rf constant/triSurface 0
rm -f constant/polyMesh/*.gz
rm -rf constant/polyMesh/sets

mkdir constant/triSurface
cp $FOAM_TUTORIALS/incompressible/simpleFoam/motorBike/constant/triSurface/motorBike.stl constant/triSurface/motorBike.stl
cp $FOAM_TUTORIALS/resources/geometry/motorBike.obj.gz constant/triSurface/

cp system/fvSolution.org system/fvSolution
cp -r 0.org 0 > /dev/null 2>&1

blockMesh
snappyHexMesh -overwrite
