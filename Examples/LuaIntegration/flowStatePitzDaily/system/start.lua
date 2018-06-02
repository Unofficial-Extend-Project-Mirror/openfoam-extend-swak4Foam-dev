print("Starting Lua-stuff")

function absVel(v)
   return math.sqrt(v.x*v.x+v.y*v.y+v.z*v.z)
end

function maxVel(vField)
   mVel=0
   for i=1,#vField do
      mVel=math.max(mVel,absVel(vField[i]))
   end
   return mVel
end

function sumStates(s)
   states={}
   for i = 1,#s do
      if states[s[i]] then
         states[s[i]]=states[s[i]]+1
      else
         states[s[i]]=1
      end
   end
   return states
end

function printStates(s)
   print("Summary of states")
   for i,nr in pairs(s) do
      print("  "..i.." -> "..nr)
   end
end

sState=sumStates(flowState)
printStates(sState)
