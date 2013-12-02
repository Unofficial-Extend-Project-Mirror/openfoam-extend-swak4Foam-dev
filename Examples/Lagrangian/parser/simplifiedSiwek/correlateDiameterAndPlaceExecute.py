times.append(runTime)
r=stats.linregress(positions,diameter)
slopes.append(r[0])
offsets.append(r[1])

f=pyplot.figure()
pyplot.title("Diameters at t=%f" % runTime)
pyplot.xlabel('y-direction [m]')
pyplot.ylabel('diameter [m]')
data=pyplot.plot(positions,diameter,'b.',label="Data")
# xLine=numpy.array([min(positions),max(positions)])
xLine=numpy.array([0.,1.])
line=pyplot.plot(xLine,r[1]+r[0]*xLine,'r',label="Fit")
a=line[0].get_axes()
text="d = %f * y + %f" % r[:2]
pyplot.text(0.1,0.1,text,transform=a.transAxes)
pyplot.legend()
# pyplot.savefig("Diameter_t=%f.png" % runTime)
pyplot.savefig("Diameter_%04d.png" % index)
index+=1
# pyplot.close()
print text
