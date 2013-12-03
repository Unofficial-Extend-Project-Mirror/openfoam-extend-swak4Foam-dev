import numpy as np
from random import randint
nrPoints=500

# fails
# positions=np.random.shuffle(centers)[:nrPoints,:]

index=[randint(0,centers.shape[0]-1) for i in range(nrPoints)]
positions=centers[index,:]

U=np.outer(np.ones(nrPoints),[0,+1,0])

dMin=5e-6
dMax=5e-4

y=positions[:,1]

d=dMin+(dMax-dMin)*(y-y.min())/(y.max()-y.min())
