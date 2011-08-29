# Read time from the controlDict and compare with current time

controlDict=ParsedParameterFile(path.join(caseDir,'system','controlDict'))
nowTime=float(controlDict['endTime'])

if abs(nowTime-runTime)<1e-6:
    print "The end is here"
else:
    print 'Time till end:',nowTime-runTime
