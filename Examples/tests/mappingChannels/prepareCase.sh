#! /bin/sh

rm -f 0/*.gz

blockMesh
changeDictionary
funkySetFields -time 0
