tail=10
thres=1e-1;

if len(pressureMeasure)<tail:
    print "Less than",tail,"values:",len(pressureMeasure)
    return endTime

p=pressureMeasure[-10:]
diff=p.max()-p.min()

if diff<thres:
    print "Range",diff,"of tail",p,"smaller than",thres," -> Stopping"
    return runTime
else:
    print "Range",diff,"bigger than",thres
    return endTime
