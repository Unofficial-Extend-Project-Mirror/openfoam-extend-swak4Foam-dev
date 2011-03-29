#! /usr/bin/env python

# Compare the current data with the data in the baselines
# Changes have to be detected manually and are either due to
# a) a change in OpenFOAM
# b) a bug in swak4Foam

import sys
if len(sys.argv)>1:
    vectorMode=sys.argv[1]
else:
    vectorMode="x"
    
from os import listdir,path
from PyFoam.Applications.TimelinePlot import TimelinePlot

gpFile=open("comparison.gnuplot","w")

for f in listdir("baselineData"):
    print "\n\n Comparing:",f
    TimelinePlot(args=[".",
                       "--dir="+f,
                       "--reference-dir="+path.join("baselineData",f),
                       "--basic-mode=lines",
                       "--vector-mode="+vectorMode,
                       "--compare","--metrics"])
    tmp=sys.stdout
    sys.stdout=gpFile
    TimelinePlot(args=[".",
                       "--dir="+f,
                       "--reference-dir="+path.join("baselineData",f),
                       "--basic-mode=lines",
                       "--vector-mode="+vectorMode])
    sys.stdout=tmp

gpFile.close()
