#! /bin/sh

rm -f constant/polyMesh/*.gz

blockMesh
snappyHexMesh -overwrite
