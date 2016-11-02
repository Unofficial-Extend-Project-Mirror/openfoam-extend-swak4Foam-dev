#! /bin/sh

rm -rf constant/polyMesh/sets/

blockMesh
# funkyWarpMesh -expression "vector(pts().x,pts().y*(interpolateToPoint(1)+(pts().x-min(pts().x))/(max(pts().x)-min(pts().x))),pts().z)"
# funkyWarpMesh -relative -overwrite -expression "vector(interpolateToPoint(0),pts().y*(pts().x-min(pts().x))/(max(pts().x)-min(pts().x)),interpolateToPoint(0))"
funkyWarpMesh -dictExt bend -overwrite
