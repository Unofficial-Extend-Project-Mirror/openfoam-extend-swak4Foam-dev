
Uinternal=OpenFOAM.vectorField(Upatch.patchInternalField())
xOld=xMin
xRevert=xMax
for i in faceIDs:
    if Uinternal[i].x()>0:
        xRevert=0.5*(xOld+Upatch.patch().Cf()[i].x())
        break
    else:
        xOld=Upatch.patch().Cf()[i].x()

print "Ux changed direction at",xRevert

