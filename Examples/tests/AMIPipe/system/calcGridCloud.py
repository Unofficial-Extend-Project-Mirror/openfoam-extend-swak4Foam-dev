# -*- python -*-

import math
import numpy as np

center=((minX+maxX)/2,(minY+maxY)/2,(minZ+maxZ)/2)
lx=maxX-minX
ly=maxY-minY
lz=maxZ-minZ

nrPoints=10000

def calc2D(x,y):
    area=x*y
    return math.sqrt(area/nrPoints)

if lx>0 and ly>0 and lz>0:
    volume=lx*ly*lz
    spaceX=math.pow(volume/nrPoints,1./3)
elif lx>0 and ly>0:
    spaceX=calc2D(lx,ly)
elif lx>0 and lz>0:
    spaceX=calc2D(lx,lz)
elif ly>0 and lz>0:
    spaceX=calc2D(ly,lz)
else:
    # at least one particle
    spaceX=1

nx=int(math.ceil(lx/spaceX))+1
ny=int(math.ceil(ly/spaceX))+1
nz=int(math.ceil(lz/spaceX))+1

positions=np.zeros((nx*ny*nz,3))
for i in range(nx):
    for j in range(ny):
        for k in range(nz):
            ind=i+nx*(j+ny*k)
            positions[ind,0]=i*spaceX+minX
            positions[ind,1]=j*spaceX+minY
            positions[ind,2]=k*spaceX+minZ

# Make C-contiguous
positions=np.array(positions.tolist())
U=0*positions

d=0.0001*np.ones(nx*ny*nz)
