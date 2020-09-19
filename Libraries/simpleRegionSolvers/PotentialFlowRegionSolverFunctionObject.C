/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakTime.H"
#include "PotentialFlowRegionSolverFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"

#ifdef FOAM_MESHOBJECT_GRAVITY
# include "gravityMeshObject.H"
#else
# include "uniformDimensionedFields.H"
#endif

#include "zeroGradientFvPatchFields.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(PotentialFlowRegionSolverFunctionObject, 0);

addNamedToRunTimeSelectionTable
(
    functionObject,
    PotentialFlowRegionSolverFunctionObject,
    dictionary,
    potentialFlowRegionSolver
);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

PotentialFlowRegionSolverFunctionObject::PotentialFlowRegionSolverFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    SimpleRegionSolverFunctionObject(
        name,
        t,
        dict
    ),
    U_(
        IOobject
        (
            "U",
            mesh().time().timeName(),
            mesh(),
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh()
    ),
    potentialFlow_(
        mesh()
#ifndef FOAM_THEREIS_NO_PISO_CONTROL_CLASS
        ,"potentialFlow"
#endif
    )
{
    Dbug << "PotentialFlowRegionSolverFunctionObject::PotentialFlowRegionSolverFunctionObject" << endl;

    // Initialise the velocity internal field to zero
    U_ = dimensionedVector("U", U_.dimensions(), vector::zero);

    U_.correctBoundaryConditions();

    phi_.set(
        new surfaceScalarField(
            IOobject
            (
                "phi",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
#ifdef FOAM_NO_SINGLE_PARAMETER_VERSION_OF_FLUX_FUNCTION
            fvc::interpolate(U_) & mesh().Sf()
#else
            fvc::flux(U_)
#endif
        )
    );

    // Infer the pressure BCs from the velocity
    wordList pBCTypes (
        U_.boundaryField().size(),
        fixedValueFvPatchScalarField::typeName
    );

    forAll(U_.boundaryField(), patchi)
    {
        if (U_.boundaryField()[patchi].fixesValue())
        {
            pBCTypes[patchi] = zeroGradientFvPatchScalarField::typeName;
        }
    }

    p_.set(
        new volScalarField
        (
            IOobject
            (
                "p",
                mesh().time().timeName(),
                mesh(),
                IOobject::READ_IF_PRESENT,
                IOobject::AUTO_WRITE
            ),
            mesh(),
            dimensionedScalar("zero", sqr(dimVelocity), 0),
            pBCTypes
        )
    );

    // Infer the velocity potential BCs from the pressure
    wordList PhiBCTypes (
        p_().boundaryField().size(),
        zeroGradientFvPatchScalarField::typeName
    );

    forAll(p_().boundaryField(), patchi)
    {
        if (p_().boundaryField()[patchi].fixesValue())
        {
            PhiBCTypes[patchi] = fixedValueFvPatchScalarField::typeName;
        }
    }

    Phi_.set(
        new volScalarField
        (
            IOobject
            (
                "Phi",
                mesh().time().timeName(),
                mesh(),
                IOobject::READ_IF_PRESENT,
                IOobject::AUTO_WRITE
            ),
            mesh(),
            dimensionedScalar("zero", dimLength*dimVelocity, 0),
            PhiBCTypes
        )
    );

    label PhiRefCell = 0;
    scalar PhiRefValue = 0;
    setRefCell(
        Phi_(),
        potentialFlow_.dict(),
        PhiRefCell,
        PhiRefValue
    );

    mesh()
#ifdef FOAM_FVSCHEMES_HAS_SETFLUXREQUIRED
        .schemesDict().setFluxRequired(
#else
        .fluxRequired(
#endif
            Phi_().name()
        );
}

bool PotentialFlowRegionSolverFunctionObject::start() {
    return true;
}

//- actual solving
bool PotentialFlowRegionSolverFunctionObject::solveRegion() {
    volVectorField &U=U_;
    surfaceScalarField &phi=phi_();
    volScalarField &p=p_();
    volScalarField &Phi=Phi_();

    U.correctBoundaryConditions();
    phi=
#ifdef FOAM_NO_SINGLE_PARAMETER_VERSION_OF_FLUX_FUNCTION
        fvc::interpolate(U_) & mesh().Sf();
#else
        fvc::flux(U);
#endif

    label PhiRefCell = 0;
    scalar PhiRefValue = 0;
    setRefCell(
        Phi,
        potentialFlow_.dict(),
        PhiRefCell,
        PhiRefValue
    );

    adjustPhi(phi, U, p);
    // Non-orthogonal velocity potential corrector loop

    while (potentialFlow_.correctNonOrthogonal())
    {
        fvScalarMatrix PhiEqn
        (
            fvm::laplacian(dimensionedScalar("1", dimless, 1), Phi)
         ==
            fvc::div(phi)
        );

        PhiEqn.setReference(PhiRefCell, PhiRefValue);
        PhiEqn.solve();

        if (potentialFlow_.finalNonOrthogonalIter())
        {
            phi -= PhiEqn.flux();
        }
    }

    Info<< "Continuity error = "
        << mag(fvc::div(phi))().weightedAverage(mesh().V()).value()
        << endl;

    U = fvc::reconstruct(phi);
    U.correctBoundaryConditions();

    Info<< "Interpolated velocity error = "
        << (sqrt(sum(
            sqr(
#ifdef FOAM_NO_SINGLE_PARAMETER_VERSION_OF_FLUX_FUNCTION
                (fvc::interpolate(U_) & mesh().Sf())
#else
                fvc::flux(U)
#endif
                -
                phi
            )))/sum(mesh().magSf())).value()
        << endl;

    Info<< nl << "Calculating approximate pressure field" << endl;

    label pRefCell = 0;
    scalar pRefValue = 0.0;
    setRefCell(
        p,
        potentialFlow_.dict(),
        pRefCell,
        pRefValue
    );

    // Calculate the flow-direction filter tensor
    volScalarField magSqrU(magSqr(U));
    if(max(mag(magSqrU)).value()<SMALL) {
        return true;
    }
    volSymmTensorField F(sqr(U)/(magSqrU + SMALL*average(magSqrU)));

    // Calculate the divergence of the flow-direction filtered div(U*U)
    // Filtering with the flow-direction generates a more reasonable
    // pressure distribution in regions of high velocity gradient in the
    // direction of the flow
    volScalarField divDivUU (
        fvc::div
        (
            F & fvc::div(phi, U),
            "div(div(phi,U))"
        )
    );

    // Solve a Poisson equation for the approximate pressure
    while (potentialFlow_.correctNonOrthogonal())
    {
        fvScalarMatrix pEqn (
            fvm::laplacian(p) + divDivUU
        );

        pEqn.setReference(pRefCell, pRefValue);
        pEqn.solve();
    }

    return true;
}

bool PotentialFlowRegionSolverFunctionObject::read(const dictionary& dict) {
    return SimpleRegionSolverFunctionObject::read(dict);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
