print("Lua Init")

referenceT=swak4foam.newScalarField(#refValue)
for i=1,#referenceT do
   referenceT[i]=refValue[i]
end
