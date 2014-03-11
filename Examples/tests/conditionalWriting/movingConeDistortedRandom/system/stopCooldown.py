global lastWrite,written,startedThisWrite
print "Check stop of cooldown",lastWrite+deltaT*len(written)
if runTime>lastWrite+deltaT*len(written):
    print "Cooling down till",lastWrite+deltaT*len(written)
    return False
else:
    print "Cooldown finished after ",deltaT*len(written)
    return True
