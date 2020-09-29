#! /bin/bash

fluent3DMeshToFoam constant/solid.msh
changeDictionary
changeDictionary -dict system/changeDictionaryDict.solid

rm -rf constant/solid/polyMesh
mv constant/polyMesh/ constant/solid

fluent3DMeshToFoam constant/fluid.msh
changeDictionary
changeDictionary -dict system/changeDictionaryDict.fluid
