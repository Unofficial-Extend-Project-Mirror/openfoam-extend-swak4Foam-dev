print("Python Writing")
from os import path

f=open(path.join(timeDir,"stateFromPython"),"w")
for i in cnt.keys():
   f.write("%d\t%d\n" %(i,cnt[i]))
f.close()
