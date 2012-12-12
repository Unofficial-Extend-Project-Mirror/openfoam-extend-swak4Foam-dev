vals,edges=numpy.histogram(a=UField.x,bins=50,weights=volume,density=True)
xVals=0.5*(edges[1:]+edges[0:-1])
data=numpy.asarray([xVals,vals]).T
numpy.savetxt(dataFile("xVelDistribution"),data)
