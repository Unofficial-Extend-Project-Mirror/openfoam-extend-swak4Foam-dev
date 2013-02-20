bins=numpy.linspace(min(minY),max(maxY),25)
vol,edges=numpy.histogram(heightField,bins=bins,weights=volume)
alpha,edges=numpy.histogram(heightField,bins=bins,weights=volume*alphaField)

xVals=0.5*(edges[1:]+edges[0:-1])
vals=alpha/vol

data=numpy.asarray([xVals,vals]).T
numpy.savetxt(dataFile("alphaByHeight"),data)
