def makePlot():
    pyplot.figure()
    pyplot.xlabel("Time [s]")
    pyplot.subplot(2,1,1)
    pyplot.title("Development of parameters over time")
    pyplot.plot(times,offsets,"r")
    pyplot.ylabel("Offset [m]")
    pyplot.subplot(2,1,2)
    pyplot.plot(times,slopes,"g")
    pyplot.ylabel("Slope [m/m]")
    pyplot.xlabel("Time [s]")

makePlot()
pyplot.savefig("ParameterDevelopment.png")
try:
    pyplot.xkcd()
    makePlot()
    pyplot.savefig("ParameterDevelopmentXKCD.png")
except AttributeError:
    print "No XKCD"
