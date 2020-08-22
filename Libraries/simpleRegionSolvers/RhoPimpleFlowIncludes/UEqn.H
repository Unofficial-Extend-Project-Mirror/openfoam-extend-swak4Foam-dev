// Solve the Momentum equation

MRF.correctBoundaryVelocity(U);

tmp<fvVectorMatrix> tUEqn
(
    fvm::ddt(rho, U) + fvm::div(phi, U)
  + MRF.DDt(rho, U)
#ifdef FOAM_TURBULENCE_HAS_DIV_DEV_TAU
  + turbulence->divDevTau(U)
#else
  + turbulence->divDevRhoReff(U)
#endif
#ifdef FOAM_HAS_FVOPTIONS
 ==
    fvOptions(rho, U)
#endif
);
fvVectorMatrix& UEqn = tUEqn.ref();

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
