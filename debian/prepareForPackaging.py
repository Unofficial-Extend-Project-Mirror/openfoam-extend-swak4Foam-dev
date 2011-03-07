#! /usr/bin/env python

from os import environ
from subprocess import Popen,PIPE

vals={}

vals["libbin"]=environ["FOAM_LIBBIN"]
vals["appbin"]=environ["FOAM_APPBIN"]
interFoam = Popen(["which", "interFoam"], stdout=PIPE).communicate()[0].strip()
vals["ofpkg"] = Popen(["dpkg", "--search",interFoam], stdout=PIPE).communicate()[0].split(":")[0]

print "Preparing with vals",vals

for f in ["control","rules"]:
    print "Doing",f
    inF=open(f+".template")
    of=open(f,"w")
    for l in inF.readlines():
        of.write(l % vals)

