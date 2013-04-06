h,xedges,yedges=numpy.histogram2d(x=UField.x,y=RField.xx,bins=20,weights=volume,normed=True)
xVals=0.5*(xedges[1:]+xedges[0:-1])
yVals=0.5*(yedges[1:]+yedges[0:-1])
x,y=numpy.meshgrid(yVals,xVals)
data=numpy.asarray([x.flatten(),y.flatten(),h.flatten()]).T

numpy.savetxt(timeDataFile("xVelVsReynoldsXX"),data)

componentSum=UField.x+UField.y
