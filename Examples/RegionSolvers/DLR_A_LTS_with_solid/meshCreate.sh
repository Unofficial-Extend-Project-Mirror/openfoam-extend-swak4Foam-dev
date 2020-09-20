#!/bin/bash

rm -rf processor*

chemkinToFoam \
    chemkin/grimech30.dat chemkin/thermo30.dat chemkin/transportProperties \
    constant/reactionsGRI constant/thermo.compressibleGasGRI

blockMesh

blockMesh -region burner

#------------------------------------------------------------------------------
