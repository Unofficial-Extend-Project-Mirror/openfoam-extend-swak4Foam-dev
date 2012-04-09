data = (rank+cnt)**2
data = comm.gather(data, root=0)
if rank == 0:
    print "proc0 says",data
else:
    print "proc%d is silent" % rank

cnt+1
