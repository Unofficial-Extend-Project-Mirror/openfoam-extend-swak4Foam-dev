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
--[[
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
--]]

maxVel=1e-15
for i=1,#slipVel do
   if slipVel[i]>maxVel then
      maxVel=slipVel[i]
   end
end
print("Maximum slip velocity "..maxVel)

cnt={0,0,0}

for i=1,#stateFace do
   state=stateFace[i]
   cnt[state]=cnt[state]+1

   if state==1 then
      refValue[i]=referenceT[i]+5
      valueFraction[i]=0.5
      if TInner[i]-referenceT[i]>1 then
         stateFace[i]=2
      end
   elseif state==2 then
      refValue[i]=referenceT[i]-5*slipVel[i]/maxVel
      valueFraction[i]=1
      if TInner[i]-referenceT[i]<-1 then
         stateFace[i]=3
      end
   elseif state==3 then
      valueFraction[i]=0
      refValue[i]=referenceT[i]
      if position[i].z<0 then
         refGrad[i]=-10
      else
         refGrad[i]=10
      end
   else
      -- Should not happen
      stateFace[i]=0
      valueFraction[i]=0
   end
end
for i,nr in pairs(cnt) do
   print("State "..i.." : "..nr)
end
