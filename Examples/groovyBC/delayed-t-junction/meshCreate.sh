#! /bin/sh

blockMesh
pyFoamChangeBoundaryType.py . defaultFaces wall
