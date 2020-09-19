#ifdef FOAM_PIMPLE_CONTROL_HAS_SIMPLERHO
if (!pimple.SIMPLErho())
#endif
{
    rho = thermo.rho();
}

// Thermodynamic density needs to be updated by psi*d(p) after the
// pressure solution
const volScalarField psip0(psi*p);

volScalarField rAU(1.0/UEqn.A());
volScalarField rAtU(1.0/(1.0/rAU - UEqn.H1()));
volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, p));

if (pimple.nCorrPISO() <= 1)
{
    tUEqn.clear();
}

surfaceScalarField phiHbyA
(
    "phiHbyA",
    (
        fvc::interpolate(rho)
        *
#ifdef FOAM_NO_SINGLE_PARAMETER_VERSION_OF_FLUX_FUNCTION
            fvc::interpolate(U) & mesh.Sf()
#else
        fvc::flux(HbyA)
#endif
#ifdef FOAM_HAS_IOMRFLIST
      + MRF.zeroFilter
        (
            fvc::interpolate(rho*rAU)*fvc::ddtCorr(rho, U, phi, rhoUf)
        )
#endif
    )
);

fvc::makeRelative(phiHbyA, rho, U);
MRF.makeRelative(fvc::interpolate(rho), phiHbyA);

volScalarField rhorAtU("rhorAtU", rho*rAtU);

// Update the pressure BCs to ensure flux consistency
constrainPressure(p, rho, U, phiHbyA, rhorAtU, MRF);

if (pimple.transonic())
{
    surfaceScalarField phid
    (
        "phid",
        (fvc::interpolate(psi)/fvc::interpolate(rho))*phiHbyA
    );

    phiHbyA +=
        fvc::interpolate(rho*(rAtU - rAU))*fvc::snGrad(p)*mesh.magSf()
      - fvc::interpolate(psi*p)*phiHbyA/fvc::interpolate(rho);

    HbyA -= (rAU - rAtU)*fvc::grad(p);

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
        fvScalarMatrix pEqn(pDDtEqn - fvm::laplacian(rhorAtU, p));

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
    phiHbyA += fvc::interpolate(rho*(rAtU - rAU))*fvc::snGrad(p)*mesh.magSf();
    HbyA -= (rAU - rAtU)*fvc::grad(p);

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
        fvScalarMatrix pEqn(pDDtEqn - fvm::laplacian(rhorAtU, p));

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

#include "rhoEqn.H"
#include "compressibleContinuityErrs.H"

// Explicitly relax pressure for momentum corrector
p.relax();

U = HbyA - rAtU*fvc::grad(p);
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
    ,rhoMin,rhoMax
#endif
) ;
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
