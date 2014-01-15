bins=numpy.linspace(min(minX),max(maxX),min(nrPlanes)+1)
vol,edges=numpy.histogram(allPositions,bins=bins,weights=allVolume)
pBin,edges=numpy.histogram(allPositions,bins=bins,weights=allVolume*allPressure)

xVals=0.5*(edges[1:]+edges[0:-1])
vals=pBin/vol

data=numpy.asarray([xVals,vals,numpy.array(positions),numpy.array(pressure)]).T
numpy.savetxt(dataFile("pressureByX"),data)

diff=vals-pressure
print "Difference between planes and bins: min",diff.min(),"max",diff.max(),"mean",diff.mean()
