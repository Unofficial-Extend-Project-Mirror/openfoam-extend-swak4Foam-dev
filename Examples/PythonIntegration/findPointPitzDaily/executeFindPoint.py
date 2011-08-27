
Uinternal=OpenFOAM.vectorField(Upatch.patchInternalField())
xOld=xMin
xRevert=xMax
UOld=Uinternal[faceIDs[0]].x()

for i in faceIDs:
    if Uinternal[i].x()>0 and UOld<=0:
        xRevert=0.5*(xOld+Upatch.patch().Cf()[i].x())
    else:
        xOld=Upatch.patch().Cf()[i].x()
    UOld=Uinternal[i].x()

print "Ux changed direction at",xRevert

