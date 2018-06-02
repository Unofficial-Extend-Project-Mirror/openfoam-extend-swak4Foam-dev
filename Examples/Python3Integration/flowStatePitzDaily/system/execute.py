print("Evaluating Python3")

vMax=max(absVel(UOut))
print("Velocity on Outlet: ",vMax)
if vMax<5:
    print("Not interested")
else:
    cnt=0
    for i in range(len(flowState)):
        oldState=flowState[i]
        if UInner.y[i]>1:
            if oldState==0.:
                flowState[i]=2
            elif oldState<0:
                flowState[i]=1
        elif UInner.y[i]<-1:
            if oldState==0.:
                flowState[i]=-2
            elif oldState>0:
                flowState[i]=-1

        if oldState!=flowState[i]:
            cnt+=1
    print(cnt," cells changed state")

sState=sumStates(flowState)
printStates(sState)
