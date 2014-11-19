print "%12s %12s %12s" % ("U","p","rho")
for i in range(len(U_nIterations)):
    print "%12g %12g %12g" % (U_initialResidual[i],p_initialResidual[i],rho_initialResidual[i])
