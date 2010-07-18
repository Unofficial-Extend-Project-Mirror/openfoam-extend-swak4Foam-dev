#!/opt/rocks/python/python
#
#$ -cwd
#$ -j y
#$ -S /opt/rocks/bin/python
#$ -m be
# #$ -pe mpi 2
#

import sys,os
from os import path,listdir

os.environ["WM_64"]="1"

from PyFoam.Infrastructure.ClusterJob import SolverJob

from PyFoam.RunDictionary.SolutionDirectory import SolutionDirectory
from PyFoam.Applications.Decomposer import Decomposer

class Splash(SolverJob):
    def __init__(self):
        SolverJob.__init__(self,
                           "circulatingSplash.run",
                           "interDyMFoam",
                           template="circulatingSplash",
                           steady=False,
                           autoParallel=False,
                           foamVersion="1.5")

    def setup(self,paramters):
        self.foamRun("blockMesh")
        if self.nproc>1:
            Decomposer(args=["--method=metis",
                             "--clear",
                             self.casename(),
                             self.nproc])
            
    def preReconstructCleanup(self,parameters):
        self.foamRun("writeDecomposition")
        
    def additionalReconstruct(self,parameters):
        sol=SolutionDirectory(self.casename())
        if len(sol.processorDirs())>0:
            for t in listdir(path.join(self.casename(),sol.processorDirs()[0])):
                try:
                    tm=float(t)
                    self.foamRun("reconstructParMesh",foamArgs=["-time",t])
                    self.foamRun("reconstructPar",foamArgs=["-time",t])
                except ValueError:
                    print "Skipping",t
                
Splash().doIt()

