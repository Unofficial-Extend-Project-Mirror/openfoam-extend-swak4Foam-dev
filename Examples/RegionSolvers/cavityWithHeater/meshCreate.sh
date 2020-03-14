#! /bin/sh

blockMesh

blockMesh -region leftHeater

blockMesh -region rightHeater

topoSet -region rightHeater
