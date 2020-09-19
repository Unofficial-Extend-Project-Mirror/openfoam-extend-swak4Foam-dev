{
    volScalarField& he =
#ifdef FOAM_HAS_FLUIDTHERMO
        thermo.he();
#else
        thermo.h();
#endif

    fvScalarMatrix EEqn
    (
        fvm::ddt(rho, he) + fvm::div(phi, he)
      + fvc::ddt(rho, K) + fvc::div(phi, K)
      + (
#ifdef FOAM_HAS_FLUIDTHERMO
            he.name() == "e"
          ? fvc::div
            (
                fvc::absolute(phi/fvc::interpolate(rho), U),
                p,
                "div(phiv,p)"
            )
          :
#endif
            -dpdt
        )
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
      + thermophysicalTransport->divq(he)
#else
      - fvm::laplacian(turbulence->alphaEff(), he)
#endif
#ifdef FOAM_HAS_FVOPTIONS
     ==
        fvOptions(rho, he)
#endif
    );

    EEqn.relax();

#ifdef FOAM_HAS_FVOPTIONS
    fvOptions.constrain(EEqn);
#endif

    EEqn.solve();

#ifdef FOAM_HAS_FVOPTIONS
    fvOptions.correct(he);
#endif

    thermo.correct();
}
