#! /bin/sh

blockMesh
rm -f 0/alpha1 0/alpha1.gz
cp 0/alpha1.orig 0/alpha1
funkySetFields -time 0 -allowFunctionObjects -addDummyPhi
