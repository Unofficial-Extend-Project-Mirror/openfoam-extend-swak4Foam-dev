try:
    import mpi4py
    return True
except ImportError:
    return False
    