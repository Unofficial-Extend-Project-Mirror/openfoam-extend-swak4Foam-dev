#!/bin/sh

# Source tutorial run functions
. $WM_PROJECT_DIR/bin/tools/RunFunctions

rm -rf constant/polyMesh/sets

runApplication blockMesh
runApplication setSet -batch makeCellSets.setSet

rm -f constant/polyMesh/sets/*_old

runApplication setsToZones -noFlipMap
runApplication splitMeshRegions -cellZones -overwrite

# remove fluid fields from solid regions (important for post-processing)
for i in heater leftSolid rightSolid
do
   rm -f 0*/$i/{mut,alphat,epsilon,k,p,p,U}
done

# remove solid fields from fluid regions (important for post-processing)
for i in bottomAir topAir
do
   rm -f 0*/$i/{cp,K,rho}
done

for i in bottomAir topAir heater leftSolid rightSolid
do
   changeDictionary -region $i > log.changeDictionary.$i 2>&1
done

# -----------------------------------------------------------------------------
