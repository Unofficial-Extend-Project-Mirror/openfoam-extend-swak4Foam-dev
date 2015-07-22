#! /bin/bash

pyFoamClearCase.py .
rm -f 0/alpha1 0/alpha1.gz

blockMesh

cp 0/alpha1Start 0/alpha1

funkySetFields -time 0
