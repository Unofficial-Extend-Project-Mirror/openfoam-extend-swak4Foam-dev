#! /bin/sh

pyFoamClearCase.py .

rm -f 0/*.gz

blockMesh
changeDictionary

calcNonUniformOffsetsForMapped

funkySetFields -time 0
