#! /usr/bin/env bash

rm -rf processor*

pyFoamDecompose.py . 2 --method=simple  --n=1,2,1 --delta=0.0001
