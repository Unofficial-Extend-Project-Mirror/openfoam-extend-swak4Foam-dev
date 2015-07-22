#! /bin/sh

./makeMesh.sh

pyFoamDecompose.py --clear . $1

pyFoamRunner.py --proc=$1 topoSet -dict system/topoSetDict.decompose
