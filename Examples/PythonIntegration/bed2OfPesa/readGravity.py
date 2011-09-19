from PyFoam.RunDictionary.SolutionDirectory import SolutionDirectory
from PyFoam.RunDictionary.ParsedParameterFile import ParsedParameterFile
from os import path
from math import sqrt

sol=SolutionDirectory(caseDir)
gFile=ParsedParameterFile(path.join(sol.constantDir(),"g"))

g=gFile["value"]
gAbs=sqrt(g[0]*g[0]+g[1]*g[1]+g[2]*g[2])
up=g/(-gAbs)
