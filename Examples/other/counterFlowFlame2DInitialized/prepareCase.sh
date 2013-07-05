#! /bin/sh

pyFoamClearCase.py .

rm -rf 0
cp -r 0.org 0
blockMesh
