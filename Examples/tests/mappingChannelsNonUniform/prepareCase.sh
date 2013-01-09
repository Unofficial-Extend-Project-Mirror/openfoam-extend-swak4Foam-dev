#! /bin/sh

pyFoamClearCase.py .

rm -f 0/*.gz

blockMesh

calcNonUniformOffsetsForMapped

funkySetFields -time 0
