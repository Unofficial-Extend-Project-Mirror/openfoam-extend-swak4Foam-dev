#! /bin/sh

pyFoamClearCase.py .

rm -f 0/*.gz

blockMesh

funkySetFields -time 0
