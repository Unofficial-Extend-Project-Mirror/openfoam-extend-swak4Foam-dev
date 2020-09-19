if (!pimple.SIMPLErho())
{
    rho = thermo.rho();
}

// Thermodynamic density needs to be updated by psi*d(p) after the
// pressure solution
const volScalarField psip0(psi*p);

volScalarField rAU(1.0/UEqn.A());
surfaceScalarField rhorAUf("rhorAUf", fvc::interpolate(rho*rAU));
volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, p));

if (pimple.nCorrPISO() <= 1)
{
    tUEqn.clear();
}

surfaceScalarField phiHbyA
(
    "phiHbyA",
    fvc::interpolate(rho)*fvc::flux(HbyA)
#ifdef FOAM_HAS_IOMRFLIST
  + MRF.zeroFilter(rhorAUf*fvc::ddtCorr(rho, U, phi, rhoUf))
#endif
);

fvc::makeRelative(phiHbyA, rho, U);
#ifdef FOAM_HAS_IOMRFLIST
MRF.makeRelative(fvc::interpolate(rho), phiHbyA);
// Update the pressure BCs to ensure flux consistency
constrainPressure(p, rho, U, phiHbyA, rhorAUf, MRF);
#endif

if (pimple.transonic())
{
    surfaceScalarField phid
    (
        "phid",
        (fvc::interpolate(psi)/fvc::interpolate(rho))*phiHbyA
    );

    phiHbyA -= fvc::interpolate(psi*p)*phiHbyA/fvc::interpolate(rho);

    fvScalarMatrix pDDtEqn
    (
        fvc::ddt(rho) + psi*correction(fvm::ddt(p))
      + fvc::div(phiHbyA) + fvm::div(phid, p)
#ifdef FOAM_HAS_FVOPTIONS
     ==
        fvOptions(psi, p, rho.name())
#endif
    );

    while (pimple.correctNonOrthogonal())
    {
        fvScalarMatrix pEqn(pDDtEqn - fvm::laplacian(rhorAUf, p));

        // Relax the pressure equation to ensure diagonal-dominance
        pEqn.relax();

#ifdef FOAM_RHOPIMPLEFOAM_USES_SELECT_FOR_P_SOLVER
        pEqn.solve(mesh.solver(p.select(pimple.finalInnerIter())));
#else
        pEqn.solve();
#endif

        if (pimple.finalNonOrthogonalIter())
        {
            phi = phiHbyA + pEqn.flux();
        }
    }
}
else
{
    fvScalarMatrix pDDtEqn
    (
        fvc::ddt(rho) + psi*correction(fvm::ddt(p))
      + fvc::div(phiHbyA)
#ifdef FOAM_HAS_FVOPTIONS
     ==
        fvOptions(psi, p, rho.name())
#endif
    );

    while (pimple.correctNonOrthogonal())
    {
        fvScalarMatrix pEqn(pDDtEqn - fvm::laplacian(rhorAUf, p));

#ifdef FOAM_RHOPIMPLEFOAM_USES_SELECT_FOR_P_SOLVER
        pEqn.solve(mesh.solver(p.select(pimple.finalInnerIter())));
#else
        pEqn.solve();
#endif

        if (pimple.finalNonOrthogonalIter())
        {
            phi = phiHbyA + pEqn.flux();
        }
    }
}


// Explicitly relax pressure for momentum corrector
p.relax();

U = HbyA - rAU*fvc::grad(p);
U.correctBoundaryConditions();
#ifdef FOAM_HAS_FVOPTIONS
fvOptions.correct(U);
#endif
K = 0.5*magSqr(U);

#ifndef FOAM_HAS_NO_PRESURE_CONTROL_CLASS
if (pressureControl.limit(p))
{
    p.correctBoundaryConditions();
}
#endif

thermo.correctRho(
    psi *p - psip0
#ifdef FOAM_CORRECT_RHO_USES_RHOMIN_MAX
    , rhoMin, rhoMax
#endif
) ;

#include "rhoEqn.H"
#include "compressibleContinuityErrs.H"

rho = thermo.rho();

// Correct rhoUf if the mesh is moving
fvc::correctRhoUf(rhoUf, rho, U, phi);

if (thermo.dpdt())
{
    dpdt = fvc::ddt(p);

    if (mesh.moving())
    {
        dpdt -= fvc::div(fvc::meshPhi(rho, U), p);
    }
}
