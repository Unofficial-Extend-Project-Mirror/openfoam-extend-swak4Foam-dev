#!/bin/sh
# Source tutorial run functions

sed -i 's/\(nNonOrthogonalCorrectors\).*;/\1 10;/g' system/fvSolution
potentialFoam -writep
sed -i 's/\(nNonOrthogonalCorrectors\).*;/\1 0;/g' system/fvSolution

simpleFoam
