print("Python 3 Evaluating")

# Uniform field in the beginning is converted to a number
if isinstance(TInner,(float,)):
    TInner=numpy.asarray([TInner]*refValue.size)

maxVel=max(slipVel)
print("Maximum slip velocity ",maxVel)

cnt={0:0,1:0,2:0}

for i in range(len(stateFace)):
    state=stateFace[i]
    cnt[state]+=1

    if state==0:
        refValue[i]=referenceT[i]+5
        valueFraction[i]=0.5
        if TInner[i]-referenceT[i]>1:
            stateFace[i]=1
    elif state==1:
        refValue[i]=referenceT[i]-5*slipVel[i]/maxVel
        valueFraction[i]=1
        if TInner[i]-referenceT[i]<-1:
            stateFace[i]=2
    elif state==2:
        valueFraction[i]=0
        refValue[i]=referenceT[i]
        if position.z[i]<0:
            refGrad[i]=-10
        else:
            refGrad[i]=10
    else:
        # Should not happen
        stateFace[i]=-1
        valueFraction[i]=0

for i,nr in cnt.items():
   print("State ",i," : ",nr)
