#! /usr/bin/env python

from os import environ,path
import re
from subprocess import Popen,PIPE,call

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

ver=None
history=open("changelog")
versline=re.compile("swak4foam-%(ofpkg)s\s+\((.+)-.+\).+" % vals)
for l in history.readlines():
    m=versline.match(l)
    if m:
        ver=m.group(1)
        break

vals["vers"]=ver
tarname=path.join(path.pardir,
                  path.pardir,
                  "swak4foam-%(ofpkg)s_%(vers)s.orig.tar.gz" % vals)

print "Generating source tar",tarname

ret=call("hg archive "+tarname,shell=True)

