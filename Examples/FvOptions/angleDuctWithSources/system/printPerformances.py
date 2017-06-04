print "%12s %12s %12s" % ("k","p","rho")
for i in range(len(k_nIterations)):
    print "%12g %12g %12g" % (k_initialResidual[i],p_initialResidual[i],rho_initialResidual[i])
