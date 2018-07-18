print("Python3 Init")

try:
    print("Reading states")
    referenceT=faceStates["refT"]
    stateFace=faceStates["state"]
except NameError:
    print("Initializing states")
    referenceT=refValue.copy()
    stateFace=numpy.zeros(refValue.size,dtype="i")
    faceStates={
        'refT'  : referenceT,
        'state' : stateFace
    }
