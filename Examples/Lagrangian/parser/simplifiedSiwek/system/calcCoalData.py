import math
import numpy as np

center=(min(minX+maxX)/2,min(minY+maxY)/2,min(minZ+maxZ)/2)
radius=max(max(maxX-minX),max(maxY-minY))/2

nrPoints=500
turns=10
index=1.+np.arange(nrPoints)
s=2*math.pi*index*turns/nrPoints
positions=np.array([radius*index/nrPoints*np.sin(s)+center[0],
                    radius*index/nrPoints*np.cos(s)+center[1],
                    np.zeros(nrPoints)+center[2]]).transpose()
# Make C-contiguous
positions=np.array(positions.tolist())
U=positions-center

dMin=5e-6
dMax=5e-4
d=dMin+(dMax-dMin)*(positions[:,1]-min(minX))/min(maxY-minY)

Ysolid=np.random.normal(loc=0.7,scale=0.1,size=nrPoints).clip(0.1,0.9)
Ygas=0.9*(1-Ysolid)
Yliquid=1-(Ysolid+Ygas)
