#!/usr/bin/python
# -*- coding: utf-8 -*-

import os,re
import sys
import subprocess
import fileinput
import platform

from optparse import OptionParser


def yumDependencies():
    if os.geteuid() != 0:
        print "You must be root to run this command."
        sys.exit(1)
    else:
        command="yum","install","mesa-libOSMesa.x86_64","tk.x86_64","tcl.x86_64","gcc","gcc-c++","bison","ncurses-devel.x86_64","tix.x86_64","glibc-devel.x86_64","flex","zlib-devel.x86_64","libXt-devel.x86_64"
        subprocess.call(command)

def replaceInLine(file,search,replace):
    print "Replacing "+search+" with "+replace+" in file"+file
    for line in fileinput.FileInput(file,inplace=1):
        line = line.replace(search,replace)
        print line.rstrip('\n')

def writeTobashrc(path,version,name,extra):
    fileHandle = open ( os.environ['HOME']+"/.bashrc", 'a' )
    fileHandle.write ( 'alias '+name+'=\"'+path+'/'+name+'-'+version+'/'+extra+'\"\n' )
    fileHandle.close()

def writeToOFbashrc(alias,path,version,name,extra):
    fileHandle = open ( os.environ['HOME']+"/.bashrc", 'a' )
    fileHandle.write ( 'alias '+alias+'=\". '+path+'/OpenFOAM/'+name+'-'+version+'/'+extra+'\"\n' )
    fileHandle.close()

def writeStartNetgen(path,package,name):
    fileName = path+"/"+package+"/bin/start_"+name
    fileHandle = open ( fileName , 'w' )
    fileHandle.write (
"""#!/bin/bash\n
export NETGENDIR="""+path+"""/"""+package+"""/bin\n
export PATH="""+path+"""/"""+package+"""/bin:$PATH\n
export LD_LIBRARY_PATH="""+path+"""/"""+package+"""/lib:$LD_LIBRARY_PATH\n
"""+path+"""/"""+package+"""/bin/"""+name+"""\n
""" )
    fileHandle.close()
    os.chmod(fileName,0755)

def writeStartEngrid(path,package,name):
    fileName = path+"/"+package+"/bin/start_"+name
    fileHandle = open ( fileName , 'w' )
    fileHandle.write(
"""#!/bin/bash\n
export PATH="""+path+"""/"""+package+"""/bin:$PATH\n
export LD_LIBRARY_PATH="""+path+"""/"""+package+"""/lib:$LD_LIBRARY_PATH\n
"""+path+"""/"""+package+"""/bin/"""+name+"""\n
""")
    fileHandle.close()
    os.chmod(fileName,0755)

def writeStartParaview(path,package,name,version):
    fileName = path+"/"+package+"/bin/start_"+name
    fileHandle = open ( fileName , 'w' )
    fileHandle.write(
"""#!/bin/bash\n
export PATH="""+path+"""/"""+package+"""/bin:$PATH\n
export LD_LIBRARY_PATH="""+path+"""/"""+package+"""/lib/"""+name+"""-"""+version+""":$LD_LIBRARY_PATH\n
"""+path+"""/"""+package+"""/bin/"""+name+"""\n
""")
    fileHandle.close()
    os.chmod(fileName,0755)

def writeStartGmsh(path,package,name):
    fileName = path+"/"+package+"/bin/start_"+name
    fileHandle = open ( fileName , 'w' )
    fileHandle.write (
"""#!/bin/bash\n
export PATH="""+path+"""/"""+package+"""/bin:$PATH\n
export LD_LIBRARY_PATH="""+path+"""/"""+package+"""/lib:$LD_LIBRARY_PATH\n
"""+path+"""/"""+package+"""/bin/"""+name+"""\n
""" )
    fileHandle.close()
    os.chmod(fileName,0755)

def setupInstallDir(path):
    if os.access(path, os.F_OK) == True:
        print "Folder exists"
    else:
        print "Folder does not exist, now creating if possible"
        os.makedirs(path)

        if os.access(path,os.W_OK):
#        print path
            print "You have write access to this folder"
        else:
            print "You do not have write access to this folder"
            sys.exit(1)

def deleteTmpFiles(package):
    os.remove(package)


def untarPackage(path,package):
    if os.access(path+package, os.F_OK) == True:
        print "Already unpacked "+package+", skipping"
    else:
        print "Unpacking "+package+", might take a minut or two"
        command = "tar","-xf",package,"-C",path,"--totals"
        subprocess.call(command)

def downloadPackage(url,package):
    if os.path.isfile(package):
        print "Already downloaded "+package+",skipping"
    else:
        print "Downloading "+package
        down = url+package
        command = "wget",down
        subprocess.call(command)

def installNetgen(path,version,rel):
    print "installing Netgen-"+version+" to "+path+"/netgen-"+version
    setupInstallDir(path)
    package="netgen-"+version
    url="http://sourceforge.net/projects/centfoam/files/"+rel+"/"
    downloadPackage(url, package+".tar.gz")
    try:
        fh = open(package+".tar.gz")
    except:
        print "File was not downloaded please check your connection"
        sys.exit(1)

    untarPackage(path, package+".tar.gz")
    print "adding netgen-"+version+" to .bashrc file for user running the script"
    writeStartNetgen(path, package, "netgen")
    writeTobashrc(path, version, "netgen", "/bin/./start_netgen")

def installEngrid(path,version,rel):
    print "installing Engrid-"+version+" to "+path+"/engrid-"+version
    setupInstallDir(path)
    package="engrid-"+version
    url="http://sourceforge.net/projects/centfoam/files/"+rel+"/"
    downloadPackage(url, package+".tar.gz")
    try:
        fh = open(package+".tar.gz")
    except:
        print "File was not downloaded please check your connection"
        sys.exit(1)
    untarPackage(path, package+".tar.gz")
    print "adding engrid-"+version+" to .bashrc file for user running the script"
    writeStartEngrid(path, package, "engrid")
    writeTobashrc(path, version, "engrid", "/bin/./start_engrid")

def installParaview(path,version,rel):
    print "installing Paraview-"+version+" to "+path+"/paraview-"+version
    setupInstallDir(path)
    package="paraview-"+version
    url="http://sourceforge.net/projects/centfoam/files/"+rel+"/"
    downloadPackage(url, package+".tar.gz")
    try:
        fh = open(package+".tar.gz")
    except:
        print "File was not downloaded please check your connection"
        sys.exit(1)
    untarPackage(path, package+".tar.gz")
    print "adding paraview-"+version+" to .bashrc file for user running the script"
    writeStartParaview(path, package, "paraview",version[:-2])
    writeTobashrc(path, version, "paraview", "/bin/./start_paraview")

def installGmsh(path,version,rel):
    print "installing Gmsh-"+version+" to "+path+"/gmsh-"+version
    setupInstallDir(path)
    package="gmsh-"+version
    url="http://sourceforge.net/projects/centfoam/files/"+rel+"/"
    downloadPackage(url, package+".tar.gz")
    try:
        fh = open(package+".tar.gz")
    except:
        print "File was not downloaded please check your connection"
        sys.exit(1)
    untarPackage(path, package+".tar.gz")
    print "adding gmsh-"+version+" to .bashrc file for user running the script"
    writeStartGmsh(path, package, "gmsh")
    writeTobashrc(path, version, "gmsh", "/bin/./start_gmsh")

def installOpenFOAM(path,version,alias,rel):
    print "installing OpenFOAM-"+version+" to "+path+"/OpenFOAM/OpenFOAM-"+version
    OFpath=path+"/OpenFOAM/"
    setupInstallDir(OFpath)
    package="OpenFOAM-"+version
    url="http://sourceforge.net/projects/centfoam/files/"+rel+"/"
    downloadPackage(url, package+".tar.gz")
    try:
        fh = open(package+".tar.gz")
    except:
        print package+".tar.gz"
        print "File was not downloaded please check your connection"
        sys.exit(1)
    untarPackage(OFpath, package+".tar.gz")
    bashFile=OFpath+package+"/etc/bashrc"
    cshFile=OFpath+package+"/etc/cshrc"
    if version=="2.0.x" and rel=="5.x":
        replaceInLine(bashFile,"foamInstall=/opt/software/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
        replaceInLine(cshFile,"set foamInstall = $HOME/$WM_PROJECT","set foamInstall ="+path+"/$WM_PROJECT")
    elif version=="2.1.0" and rel=="5.x":
        replaceInLine(bashFile,"foamInstall=$HOME/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
        replaceInLine(cshFile,"set foamInstall = $HOME/$WM_PROJECT","set foamInstall ="+path+"/$WM_PROJECT")
    elif version=="2.2.x" and rel=="5.x":
        replaceInLine(bashFile,"foamInstall=$HOME/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
        replaceInLine(cshFile,"set foamInstall = $HOME/$WM_PROJECT","set foamInstall ="+path+"/$WM_PROJECT")
    elif version=="1.6-ext" and rel=="5.x":
        replaceInLine(bashFile,"foamInstall=/home/44792/centFoam/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
    elif version=="2.0.x" and rel=="6.x":
        replaceInLine(bashFile,"foamInstall=$HOME/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
        replaceInLine(cshFile,"set foamInstall = $HOME/$WM_PROJECT","set foamInstall ="+path+"/$WM_PROJECT")
    elif version=="2.1.0" and rel=="6.x":
        replaceInLine(bashFile,"foamInstall=$HOME/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
        replaceInLine(cshFile,"set foamInstall = $HOME/$WM_PROJECT","set foamInstall ="+path+"/$WM_PROJECT")
    elif version=="2.2.x" and rel=="6.x":
        replaceInLine(bashFile,"foamInstall=/home/centfoam/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
        replaceInLine(cshFile,"set foamInstall = $HOME/$WM_PROJECT","set foamInstall ="+path+"/$WM_PROJECT")
    elif version=="1.6-ext" and rel=="6.x":
        replaceInLine(bashFile,"foamInstall=$HOME/$WM_PROJECT","foamInstall="+path+"/$WM_PROJECT")
        replaceInLine(cshFile,"set foamInstall = $HOME/$WM_PROJECT","set foamInstall ="+path+"/$WM_PROJECT")
    else:
        print ""

    print "adding OpenFOAM-"+version+" to .bashrc file for user running the script"
    writeToOFbashrc(alias, path, version, "OpenFOAM", "/etc/bashrc")


def main():
    usage = "usage: %prog [options] arg"

    parser = OptionParser()

    parser.add_option("--yum",
            action="store_false", dest="yum", default=True,
            help="Install yum packages")

    parser.add_option("--paraview",
            action="store_false", dest="paraview", default=True,
            help="Install latest paraview")

    parser.add_option("--netgen",
            action="store_false", dest="netgen", default=True,
            help="Install latest netgen")

    parser.add_option("--engrid",
            action="store_false", dest="engrid", default=True,
            help="Install latest engrid")

    parser.add_option("--gmsh",
            action="store_false", dest="gmsh", default=True,
            help="Install latest gmsh")

    parser.add_option("--nonOF",
        action="store_false", dest="nonOF", default=True,
        help="Install all non OpenFOAM applications (the four above)")

    parser.add_option("--OF20",
            action="store_false", dest="OF20", default=True,
            help="Install OpenFOAM-2.0.x")

    parser.add_option("--OF21",
            action="store_false", dest="OF21", default=True,
            help="Install OpenFOAM-2.1.1")

    parser.add_option("--OF22",
            action="store_false", dest="OF22", default=True,
            help="Install OpenFOAM-2.2.x")

    parser.add_option("--OF16",
            action="store_false", dest="OF16", default=True,
            help="Install OpenFOAM-1.6-ext")

    parser.add_option("-p", "--path",
        action="store", type="string", dest="path", default=os.environ['HOME']+"/centFOAM/",
        help="Path to install directory, default "+os.environ['HOME']+"/centFOAM/")

    parser.add_option("-q", "--quiet",
            action="store_false", dest="verbose", default=True,
            help="don't print status messages to stdout")

    (options, args) = parser.parse_args()

    print "All packages will be downloaded to this folder\n and extracted to "+options.path
#    raw_input("Press enter to continue")

    release=platform.release()
    print 'release  :', release
    if re.search("el6",release):
        rel="6.x"
        print "Using 6.x\n"
        if options.yum==False:
            yumDependencies()
        if options.netgen==False:
            installNetgen(options.path, "4.9.14-svn",rel)
        if options.engrid==False:
            installEngrid(options.path, "1.3",rel)
        if options.paraview==False:
            installParaview(options.path, "3.98.1",rel)
        if options.gmsh==False:
            installGmsh(options.path, "2.5.0",rel)
        if options.nonOF==False:
            installNetgen(options.path, "4.9.14-svn",rel)
            installParaview(options.path, "3.98.1",rel)
            installGmsh(options.path, "2.5.0",rel)
        if options.OF20==False:
            installOpenFOAM(options.path, "2.0.x", "OF20",rel)
        if options.OF21==False:
            installOpenFOAM(options.path, "2.1.1", "OF21",rel)
        if options.OF22==False:
            installOpenFOAM(options.path, "2.2.x", "OF22",rel)
        if options.OF16==False:
            installOpenFOAM(options.path, "1.6-ext", "OF16",rel)

    elif re.search("el5",release):
        rel="5.x"
        print "Using 5.x\n"
        if options.yum==False:
            yumDependencies()
        if options.netgen==False:
            installNetgen(options.path, "4.9.14-svn",rel)
        if options.engrid==False:
            installEngrid(options.path, "1.3",rel)
        if options.paraview==False:
            installParaview(options.path, "3.98.1",rel)
        if options.gmsh==False:
            installGmsh(options.path, "2.5.0",rel)
        if options.nonOF==False:
            installNetgen(options.path, "4.9.14-svn",rel)
            installEngrid(options.path, "1.3",rel)
            installParaview(options.path, "3.98.1",rel)
            installGmsh(options.path, "2.5.0",rel)
        if options.OF20==False:
            installOpenFOAM(options.path, "2.0.x", "OF20",rel)
        if options.OF21==False:
            installOpenFOAM(options.path, "2.1.1", "OF21",rel)
        if options.OF22==False:
            installOpenFOAM(options.path, "2.2.x", "OF22",rel)
        if options.OF16==False:
            installOpenFOAM(options.path, "1.6-ext", "OF16",rel)
    else:
        print "The system is neither 5.x or 6.x in the RHEL family\n"

if __name__ == "__main__":
    main()
