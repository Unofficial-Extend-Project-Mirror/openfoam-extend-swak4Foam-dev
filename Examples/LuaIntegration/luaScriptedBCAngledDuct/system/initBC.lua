print("Lua Init")

if faceStates and faceStates.refT and faceStates.state then
   print("Reading states")
   referenceT=faceStates.refT
   stateFace=faceStates.state
else
   print("Initializing states")
   referenceT=swak4foam.newScalarField(#refValue)
   for i=1,#referenceT do
      referenceT[i]=refValue[i]
   end

   stateFace=swak4foam.newScalarField(#refValue)
   for i=1,#referenceT do
      stateFace[i]=1
   end
   faceStates={
      refT=referenceT,
      state=stateFace
   }
end
