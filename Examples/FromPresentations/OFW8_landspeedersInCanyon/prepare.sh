#! /bin/sh

pyFoamClearCase.sh .
rm -fr 0
rm -f constant/polyMesh/*.gz

blockMesh

snappyHexMesh -overwrite

cp -r 0.org/* 0

potentialFoam -writep -noFunctionObjects
