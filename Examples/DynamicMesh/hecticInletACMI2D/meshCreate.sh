#!/bin/sh

blockMesh

topoSet -constant

# split the mesh to generate the ACMI coupled patches
createBaffles -overwrite
