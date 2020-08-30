// Solve the Momentum equation

#ifdef FOAM_HAS_IOMRFLIST
MRF.correctBoundaryVelocity(U);
#endif

tmp<fvVectorMatrix> tUEqn
(
    fvm::ddt(rho, U) + fvm::div(phi, U)
#ifdef FOAM_HAS_IOMRFLIST
  + MRF.DDt(rho, U)
#endif
#ifdef FOAM_TURBULENCE_HAS_DIV_DEV_TAU
  + turbulence->divDevTau(U)
#else
  + turbulence->divDevRhoReff(
#ifndef FOAM_TURB_DIVDEVRHOEFF_NEEDS_NO_U
      U
#endif
  )
#endif
#ifdef FOAM_HAS_FVOPTIONS
 ==
    fvOptions(rho, U)
#endif
);
fvVectorMatrix& UEqn = tUEqn
#ifdef FOAM_FVMATRIX_HAS_REF_METHOD
    .ref();
#else
        ();
#endif

UEqn.relax();

#ifdef FOAM_HAS_FVOPTIONS
fvOptions.constrain(UEqn);
#endif

if (pimple.momentumPredictor())
{
    solve(UEqn == -fvc::grad(p));

#ifdef FOAM_HAS_FVOPTIONS
    fvOptions.correct(U);
#endif
    K = 0.5*magSqr(U);
}
