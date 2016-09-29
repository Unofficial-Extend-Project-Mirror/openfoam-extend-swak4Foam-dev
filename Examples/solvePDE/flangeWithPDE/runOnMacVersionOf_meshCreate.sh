#! /bin/sh

# Necessary because Mac OS X does not pass this variable
export LD_LIBRARY_PATH=/Users/bgschaid/OpenFOAM/OpenFOAM-v1606+/platforms/darwinIntel64Clang37DPInt32Opt/lib/openmpi-macport-mp:/Users/bgschaid/OpenFOAM/OpenFOAM-v1606+/platforms/darwinIntel64Clang37DPInt32Opt/lib:/Users/bgschaid/OpenFOAM/bgschaid-v1606+/platforms/darwinIntel64Clang37DPInt32Opt/lib:/Users/bgschaid/OpenFOAM/site/v1606+/platforms/darwinIntel64Clang37DPInt32Opt/lib


ansysToFoam flange.ans -scale 0.001
