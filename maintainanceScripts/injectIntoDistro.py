#! /usr/bin/env python

from os import path,remove,mkdir,listdir
from os import environ as e
from shutil import rmtree
from subprocess import call
import sys

srcDir=path.join(e["FOAM_SRC"],"swak4Foam")
appDir=path.join(e["FOAM_UTILITIES"],"swak4Foam")
tutDir=path.join(e["FOAM_TUTORIALS"],"swak4Foam")

for aDir in [srcDir,appDir,tutDir]:
    if path.exists(aDir):
        print "Removing",aDir
        rmtree(aDir)

tarFile="/tmp/swak4FoamRepository.tar"
if path.exists(tarFile):
    print "Removing",tarFile
    remove(tarFile)
    
print "Creating tar of this repository at",tarFile
status=call(["hg","archive",tarFile])
if status!=0:
    print "Problem while creating tar"
    sys.exit(-1)
    
destTuples=[ ("Libraries", srcDir),
             ("Utilities", appDir),
             ("Examples", tutDir) ]

for orig,dest in destTuples:
    print "Creating",dest
    mkdir(dest)
    print "Untarring",orig,"into",dest
    cmd=["tar","-x",
         "-f",tarFile,
         "--include","*"+orig+"*",
         "--strip-components","2",
         "-C",dest]
    status=call(cmd)
    if status!=0:
        print "Problem"
        sys.exit(-1)

    print "Generating Mercurial information"
    call(["hg","sum"],stdout=open(path.join(dest,"upstreamMerurialSummary"),"w"))

testDir=path.join(tutDir,"tests")
print "Removing tests in",testDir
rmtree(testDir)

if not path.exists(path.join(e["FOAM_SRC"],"finiteArea")):
    print "This OF does not have finite area"
    for main,sub in [ (srcDir,["swakFiniteArea"]),
                      (appDir,["funkySetAreaFields"]),
                      (tutDir,["FiniteArea"]) ]:
        for s in sub:
            pth=path.join(main,s)
            print "Removing",pth
            rmtree(pth)

def replaceAndWrite(fName,rules):
    lines=open(fName).readlines()
    f=open(fName,"w")
    for l in lines:
        for old,new in rules:
            l=l.replace(old,new)
        f.write(l)
    f.close()

def findAndProcessMake(main):
    for d in listdir(main):
        f=path.join(main,d)
        if path.isdir(f):
            if d=="Make":
                print "Processing",f
                replaceAndWrite(path.join(f,"files"),
                                [("FOAM_USER_LIBBIN","FOAM_LIBBIN"),
                                 ("FOAM_USER_APPBIN","FOAM_APPBIN")])
                replaceAndWrite(path.join(f,"options"),
                                [("-L$(FOAM_USER_LIBBIN)",""),
                                 ("-I../../../Libraries/","-I$(LIB_SRC)/swak4Foam/"),
                                 ("-I../../Libraries/","-I$(LIB_SRC)/swak4Foam/"),
                                 ("-I../","-I$(LIB_SRC)/swak4Foam/")])
            else:
                findAndProcessMake(f)
                
for d in [srcDir,appDir,tutDir]:
    print "Processing Make-directories in",d
    findAndProcessMake(d)

appMake=path.join(appDir,"Allwmake")
print "Adding 'build libraries' to",appMake

lines=open(appMake).readlines()
lines=lines[:2]+["wmake all $FOAM_SRC/swak4Foam\n","\n"]+lines[2:]
open(appMake,"w").writelines(lines)
