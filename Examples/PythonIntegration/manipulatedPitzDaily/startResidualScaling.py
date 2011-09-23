from os import path
from PyFoam.RunDictionary.ParsedParameterFile import ParsedParameterFile
from math import log,exp,pow
control=ParsedParameterFile(path.join(caseDir,"system","controlDict"))

end=float(control["endTime"])

del control

scaleTill=int(end*0.5)
scaleFactor=10.

factor=exp(log(scaleFactor)/scaleTill)

print "Scale by actor:",factor
print

fvSol=ParsedParameterFile(path.join(caseDir,"system","fvSolution"),backup=True)
