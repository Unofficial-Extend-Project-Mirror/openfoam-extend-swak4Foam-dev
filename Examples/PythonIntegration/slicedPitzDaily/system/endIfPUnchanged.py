tail=50
thres=1e-4;

if outputTime:
    numpy.savetxt(dataFile("pressureMeasure"),pressureMeasure)

if len(pressureMeasure)<tail:
    print "Less than",tail,"values:",len(pressureMeasure)
    return endTime

p=pressureMeasure[-10:]
diff=p.max()-p.min()

if diff<thres:
    print "Range",diff,"of tail",p,"smaller than",thres," -> Stopping"
    numpy.savetxt(dataFile("pressureMeasure"),pressureMeasure)
    return runTime
else:
    print "Range",diff,"bigger than",thres
    return endTime
