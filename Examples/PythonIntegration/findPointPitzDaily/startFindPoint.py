hasPlot=False

try:
    import matplotlib
    matplotlib.use('Agg')

    import matplotlib.pyplot as plt

    figure=plt.figure()
    axis=figure.add_subplot(111)
    line,=axis.plot([],[],label="x")
    axis.legend()

    tValues=[]
    xValues=[]
    
    hasPlot=True
except ImportError:
    print "No Matplotlib .... no plotting"

from Foam.OpenFOAM import word

mesh=finiteVolume.fvMesh.ext_lookupObject(time,word("region0"))
patchID=mesh.boundaryMesh().findPatchID(word("lowerWall"))

U=finiteVolume.volVectorField.ext_lookupObject(mesh,word("U"))
Upatch=U.ext_boundaryField()[patchID]

yMin=Upatch.patch().Cf().gMin().y()
xMin=Upatch.patch().Cf().gMax().x()
xMax=Upatch.patch().Cf().gMin().x()

if hasPlot:
    axis.set_ybound(lower=xMin,upper=xMax)
    
lst=[]
for i in range(Upatch.patch().Cf().size()):
   if abs(Upatch.patch().Cf()[i].y()-yMin)<1e-5:
      lst.append((i,Upatch.patch().Cf()[i]))
      if Upatch.patch().Cf()[i].x()>xMax:
          xMax=Upatch.patch().Cf()[i].x()
      if Upatch.patch().Cf()[i].x()<xMin:
          xMin=Upatch.patch().Cf()[i].x()

print len(lst),"faces with x-range [",xMin,",",xMax,"]"

# Make sure the patches are sorted in x-direction
lst.sort(cmp=lambda a,b:cmp(a[1].x(),b[1].x()))

faceIDs=[a[0] for a in lst]
del lst

