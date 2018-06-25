print("Evaluating Lua")

vMax=maxVel(UOut)
print("Velocity on Outlet: "..vMax)
if vMax<5 then
   print("Not interested")
else
   cnt=0
   for i=1,#flowState do
      oldState=flowState[i]
      if UInner[i].y>1 then
         if oldState==0. then
            -- We were always "up"
            flowState[i]=2
         elseif oldState<0 then
            -- Now we're "up"
            flowState[i]=1
         end
      elseif UInner[i].y<-1 then
         if oldState==0. then
            flowState[i]=-2
         elseif oldState>0 then
            flowState[i]=-1
         end
      end
      if oldState~=flowState[i] then
         cnt=cnt+1
      end
   end
   print(cnt.." cells changed state")
end

sState=sumStates(flowState)
printStates(sState)
