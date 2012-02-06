#! /bin/sh

./makeMesh.sh

pyFoamDecompose.py --clear . $1

cp system/cellSetDict.3 system/cellSetDict
pyFoamRunner.py --proc=$1 cellSet
cp system/cellSetDict.4 system/cellSetDict
pyFoamRunner.py --proc=$1 cellSet
cp system/cellSetDict.5 system/cellSetDict
pyFoamRunner.py --proc=$1 cellSet

cp system/faceSetDict.3 system/faceSetDict
pyFoamRunner.py --proc=$1 faceSet
cp system/faceSetDict.4 system/faceSetDict
pyFoamRunner.py --proc=$1 faceSet


