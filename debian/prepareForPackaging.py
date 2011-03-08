#! /usr/bin/env python

from os import environ
from subprocess import Popen,PIPE

vals={}

vals["libbin"]=environ["FOAM_LIBBIN"][1:]
vals["appbin"]=environ["FOAM_APPBIN"][1:]
interFoam = Popen(["which", "interFoam"], stdout=PIPE).communicate()[0].strip()
vals["ofpkg"] = Popen(["dpkg", "--search",interFoam], stdout=PIPE).communicate()[0].split(":")[0]
vals["ofproject"]=environ["WM_PROJECT_DIR"][1:]

print "Preparing with vals",vals

for f in ["control","rules","changelog","swak4foam-ofpkg.install","swak4foam-ofpkg.examples","swak4foam-ofpkg-dev.install"]:
    print "Doing",f
    inF=open(f+".template")
    of=open(f.replace("ofpkg",vals["ofpkg"]),"w")
    for l in inF.readlines():
        of.write(l % vals)

