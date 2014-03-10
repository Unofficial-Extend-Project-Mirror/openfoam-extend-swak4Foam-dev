print "Check writing"
global lastWrite,written,startedThisWrite
if runTime>(lastWrite+2*deltaT*len(written)):
    written.append(runTime)
    print "First writing after",lastWrite
    print "Written till now",written
    lastWrite=runTime
    startedThisWrite=runTime
    return True
else:
    print "No writing before",(lastWrite+deltaT*len(written))
    return False
