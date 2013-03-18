#! /bin/sh

pyFoamClearCase.py .

rm -rf 0
cp -r 0.orig 0

pyFoamFromTemplate.py --template-file=constant/polyMesh/blockMeshDict.template --values-dict=constant/normalValues --output-file=constant/polyMesh/blockMeshDict --expression-delimiter=$

blockMesh

funkySetFields -time 0
