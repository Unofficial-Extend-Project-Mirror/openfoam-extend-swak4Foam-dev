print("Lua Evaluating")

-- Uniform field in the beginning is converted to a number
if type(TInner)=="number" then
   val=TInner
   TInner=swak4foam.newScalarField(#referenceT)
   for i=1,#TInner do
      TInner[i]=val
   end
end

-- Don't think too much about this. It is just for testing
for i=1,#referenceT do
   if position[i].z>0 then
      refValue[i]=referenceT[i]+5
      valueFraction[i]=0.5
   else
      if TInner[i]>referenceT[i] then
         refValue[i]=TValue[i]-5
         valueFraction[i]=0.5
      else
         refValue[i]=referenceT[i]+5
         valueFraction[i]=1
      end
   end
end
