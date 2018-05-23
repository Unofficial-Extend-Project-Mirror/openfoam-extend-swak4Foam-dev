print("Lua Init")

referenceT=swak4foam.newScalarField(#refValue)
for i=1,#referenceT do
   referenceT[i]=refValue[i]
end

stateFace=swak4foam.newScalarField(#refValue)
for i=1,#referenceT do
   stateFace[i]=1
end
