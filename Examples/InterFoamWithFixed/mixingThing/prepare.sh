#! /bin/sh

blockMesh
cp 0/alpha1.org 0/alpha1
funkySetFields -time 0 -field alpha1  -expression "pos().y<0 ? 1 : 0"

