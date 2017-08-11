try:
    import mpi4py
    print "We have mpi4py"
    #return True
    return False
except ImportError:
    print "No mpi4py"
    return False
