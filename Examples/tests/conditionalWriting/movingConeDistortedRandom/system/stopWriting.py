global lastWrite,written,startedThisWrite
print "Check stop of writing",startedThisWrite
if runTime<(startedThisWrite+deltaT*len(written)):
    print "Writing till",(written[-1]+deltaT*len(written))
    lastWrite=runTime
    return False
else:
    print "Ended writing"
    print "Written times:",written
    return True
