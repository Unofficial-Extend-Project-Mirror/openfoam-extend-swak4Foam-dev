#!/opt/rocks/python/python
#
#$ -cwd
#$ -j y
#$ -S /opt/rocks/bin/python
#$ -m be
# #$ -pe mpi 2
#
import sys,os
from os import path
from os import environ

from PyFoam.Infrastructure.ClusterJob import SolverJob
from PyFoam.FoamInformation import injectVariables
injectVariables("/home/bgschaid/OpenFOAM/OpenFOAM-2.1.x/etc/bashrc")

environ["PATH"]+=":/home/common/bin:/home/common/pythonbin/PyFoam"
#environ["PATH"]+=environ["MPI_ARCH_PATH"]+"/bin:"+environ["PATH"]

template=sys.argv[1]
suffix=sys.argv[2]

#print environ
#print
#print "LD_LIBRARY_PATH",environ["LD_LIBRARY_PATH"]
#print
#print "MPI_ARCH_PATH",environ["MPI_ARCH_PATH"]
environ["LD_LIBRARY_PATH"]+=":"+environ["MPI_ARCH_PATH"]+"/lib"

class EmptyAndClean(SolverJob):
    def __init__(self):
        SolverJob.__init__(self,
                           "EmptyAndClean_"+path.basename(path.abspath(template))+"_"+suffix,
                           "interFoam",
                           template=template,
                           steady=False,
                           cloneParameters=["--add-item=prepareCase.sh",
                                            "--add-item=0.orig",
                                            "--no-vcs"],
                           foamVersion="2.1.x")
    def setup(self,parameters):
        self.execute("./prepareCase.sh")

EmptyAndClean().doIt()
