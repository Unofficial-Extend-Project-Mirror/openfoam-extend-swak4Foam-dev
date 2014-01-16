#! /usr/bin/env python

from os import environ,path
import re
from subprocess import Popen,PIPE,call

vals={}

vals["libbin"]=environ["FOAM_LIBBIN"][1:]
vals["appbin"]=environ["FOAM_APPBIN"][1:]
interFoam = Popen(["which", "interFoam"], stdout=PIPE).communicate()[0].strip()
vals["ofpkg"] = Popen(   ["dpkg", 
                          "--search",
                          interFoam], 
                       stdout=PIPE).communicate()[0].split(":")[0]
handInstalledOF=False
vals["ofpkgdev"] = Popen(   ["dpkg", 
                             "--search",
                             path.join(environ["FOAM_SRC"],"OpenFOAM")], 
                         stdout=PIPE).communicate()[0].split(":")[0]
vals["ofpkgComma"]=vals["ofpkg"]+","
vals["ofpkgdevComma"]=vals["ofpkgdev"]+","
if vals["ofpkg"]=="":
    # OpenFOAM was not installed as a package
    handInstalledOF=True
    vals["ofpkg"]="of-"+environ["WM_PROJECT_VERSION"]+"-handinstalled"
    vals["ofpkgdev"]="ofdev-"+environ["WM_PROJECT_VERSION"]+"-handinstalled"
    vals["ofpkgComma"]=""
    vals["ofpkgdevComma"]=""

vals["ofproject"]=environ["WM_PROJECT_DIR"][1:]

codeName="unknownDistro"

with open("/etc/lsb-release") as f:
    for l in f.readlines():
        if l.find("DISTRIB_CODENAME")==0:
            codeName=l[l.find("=")+1:].strip()

vals["distro"]=codeName

print "Preparing with vals",vals

def updateTemplates(files,vals):
    for f in files:
        print "Doing",f
        inF=open(f+".template")
        of=open(f.replace("ofpkg",vals["ofpkg"]),"w")
        for l in inF.readlines():
            of.write(l % vals)

pyVersion=None
for pyVer in ["2.7","2.6"]:
    if path.exists("/usr/include/python"+pyVer):
        pyVersion=pyVer
        break

vals["pythonver"]=pyVersion

updateTemplates(["control","rules","changelog",
                 "swak4foam-ofpkg.install","swak4foam-ofpkg.examples",
                 "swak4foam-ofpkg-dev.install"],
                vals)

ver=None
fullver=None
history=open("changelog")
versline=re.compile("swak4foam-%(ofpkg)s\s+\(((.+)-.+)\).+" % vals)
for l in history.readlines():
    m=versline.match(l)
    if m:
        ver=m.group(2)
        fullver=m.group(1)
        break

vals["vers"]=ver
vals["fullvers"]=fullver

updateTemplates(["patches/swak4Foam_pythonSupport"],
                vals)

tarname=path.join(path.pardir,
                  path.pardir,
                  "swak4foam-%(ofpkg)s_%(vers)s.orig.tar.gz" % vals)

print "Generating source tar",tarname

ret=call("hg archive -t tgz "+tarname,shell=True)

