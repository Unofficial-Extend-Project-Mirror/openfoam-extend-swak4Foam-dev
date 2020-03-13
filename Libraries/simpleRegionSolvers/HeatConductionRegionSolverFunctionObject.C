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

#include "HeatConductionRegionSolverFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

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

defineTypeNameAndDebug(HeatConductionRegionSolverFunctionObject, 0);

addNamedToRunTimeSelectionTable
(
    functionObject,
    HeatConductionRegionSolverFunctionObject,
    dictionary,
    heatConductionRegionSolver
);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

HeatConductionRegionSolverFunctionObject::HeatConductionRegionSolverFunctionObject
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
    thermo_(
        solidThermo::New(this->mesh())
    ),
    fvOptions_(
        mesh()
    )
{
    Dbug << "HeatConductionRegionSolverFunctionObject::HeatConductionRegionSolverFunctionObject" << endl;

    if (!thermo().isotropic())
    {
        // Info<< "    Adding coordinateSystems\n" << endl;
        coordinates_.set
            (
                coordinateSystem::New
                (
                    mesh(),
                    thermo(),
                    coordinateSystem::typeName_()
                )
            );

        tmp<volVectorField> tkappaByCp =
            thermo().Kappa()/thermo().Cp();

        aniAlpha_.set
            (
                new volSymmTensorField
                (
                    IOobject
                    (
                        "Anialpha",
                        mesh().time().timeName(),
                        mesh(),
                        IOobject::NO_READ,
                        IOobject::NO_WRITE
                    ),
                    mesh(),
                    dimensionedSymmTensor(tkappaByCp().dimensions(), Zero),
                    zeroGradientFvPatchSymmTensorField::typeName
                )
            );

        aniAlpha_().primitiveFieldRef() =
            coordinates_().transformPrincipal
            (
                mesh().cellCentres(),
                tkappaByCp()
            );
        aniAlpha_().correctBoundaryConditions();
    }

    IOobject betavSolidIO
        (
            "betavSolid",
            mesh().time().timeName(),
            mesh(),
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        );

    if (betavSolidIO.typeHeaderOk<volScalarField>(true))
    {
        betav_.set
            (
                new volScalarField(betavSolidIO, mesh())
            );
    }
    else
    {
        betav_.set
            (
                new volScalarField
                (
                    IOobject
                    (
                        "betavSolid",
                        mesh().time().timeName(),
                        mesh(),
                        IOobject::NO_READ,
                        IOobject::NO_WRITE
                    ),
                    mesh(),
                    dimensionedScalar("1", dimless, scalar(1))
                )
            );
    }

}

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
bool HeatConductionRegionSolverFunctionObject::start() {
    return true;
}
#endif

//- actual solving
bool HeatConductionRegionSolverFunctionObject::solveRegion() {
    tmp<volScalarField> trho = thermo().rho();
    const volScalarField& rho = trho();

    tmp<volScalarField> tcp = thermo().Cp();
    const volScalarField& cp = tcp();

    tmp<volSymmTensorField> taniAlpha;
    if (!thermo().isotropic())
    {
        volSymmTensorField& aniAlpha = aniAlpha_();
        tmp<volVectorField> tkappaByCp = thermo().Kappa()/cp;
        const coordinateSystem& coodSys = coordinates_();

        aniAlpha.primitiveFieldRef() =
            coodSys.transformPrincipal
            (
                mesh().cellCentres(),
                tkappaByCp()
            );

        aniAlpha.correctBoundaryConditions();

        taniAlpha = tmp<volSymmTensorField>
            (
                new volSymmTensorField(aniAlpha)
            );
    }

    volScalarField& h = thermo().he();

    const volScalarField& betav = betav_();

    fv::options& fvOptions = fvOptions_;

    // if (finalIter)
    // {
    //     mesh().data::add("finalIteration", true);
    // }

    {
        const dictionary& pimple = mesh().solutionDict().subDict("PIMPLE");

        int nNonOrthCorr =
            pimple.lookupOrDefault<int>("nNonOrthogonalCorrectors", 0);

        for (int nonOrth=0; nonOrth<=nNonOrthCorr; ++nonOrth)
        {
            fvScalarMatrix hEqn
                (
                    fvm::ddt(betav*rho, h)
                    - (
                        thermo().isotropic()
                        ? fvm::laplacian(betav*thermo().alpha(), h, "laplacian(alpha,h)")
                        : fvm::laplacian(betav*taniAlpha(), h, "laplacian(alpha,h)")
                    )
                    ==
                    fvOptions(rho, h)
                );

            hEqn.relax();

            fvOptions.constrain(hEqn);

            hEqn.solve(mesh().solver(h.name()));

            fvOptions.correct(h);
        }

        thermo().correct();

        Info<< meshRegion() << " Min/max T:" << min(thermo().T()).value() << ' '
            << max(thermo().T()).value() << endl;
    }

    // if (finalIter)
    // {
    //     mesh.data::remove("finalIteration");
    // }

    return true;
}

bool HeatConductionRegionSolverFunctionObject::read(const dictionary& dict) {
    return SimpleRegionSolverFunctionObject::read(dict);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
