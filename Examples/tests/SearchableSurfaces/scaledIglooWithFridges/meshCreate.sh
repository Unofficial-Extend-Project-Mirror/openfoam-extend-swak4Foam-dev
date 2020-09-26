#! /bin/bash

rm -f constant/polyMesh/*.gz

blockMesh
snappyHexMesh -overwrite
