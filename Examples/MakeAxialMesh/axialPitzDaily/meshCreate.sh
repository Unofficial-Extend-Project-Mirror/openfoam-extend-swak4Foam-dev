#! /bin/sh

blockMesh
makeAxialMesh -overwrite -axis axis -wedge frontAndBack
collapseEdges -overwrite
