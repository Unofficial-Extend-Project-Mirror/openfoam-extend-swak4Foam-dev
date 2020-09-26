#! /bin/bash

blockMesh
pyFoamChangeBoundaryType.py . defaultFaces wall
