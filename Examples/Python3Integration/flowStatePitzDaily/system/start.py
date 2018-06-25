print("Starting Python3-stuff")

def absVel(v):
   return numpy.sqrt(v.x*v.x+v.y*v.y+v.z*v.z)

def sumStates(s):
   states={}
   for v in s:
      if v in states:
         states[v]+=1
      else:
         states[v]=1
   return states

def printStates(s):
   print("Summary of states")
   for i,nr in s.items():
      print("  {} -> {}".format(i,nr))

sState=sumStates(flowState)
printStates(sState)
