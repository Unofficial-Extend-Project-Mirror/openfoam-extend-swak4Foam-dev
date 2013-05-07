#!/bin/sh

pyFoamClearCase.py .

rm -f constant/polyMesh/*.gz
rm -rf constant/polyMesh/sets

blockMesh
snappyHexMesh -overwrite
