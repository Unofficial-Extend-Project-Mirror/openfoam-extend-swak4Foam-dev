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

#include "SolidDisplacementRegionSolverFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(SolidDisplacementRegionSolverFunctionObject, 0);

addNamedToRunTimeSelectionTable
(
    functionObject,
    SolidDisplacementRegionSolverFunctionObject,
    dictionary,
    solidDisplacementRegionSolver
);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

SolidDisplacementRegionSolverFunctionObject::
    SolidDisplacementRegionSolverFunctionObject(
        const word &name,
        const Time &t,
        const dictionary &dict
    )
    :
        SimpleRegionSolverFunctionObject(
            name,
            t,
            dict
        ),
        stressControl_(
            mesh().solutionDict().subDict("stressAnalysis")
        ),
        mechanicalProperties_
        (
            IOobject
            (
                "mechanicalProperties",
                mesh().time().constant(),
                mesh(),
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE
            )
        ),
        thermalProperties_
        (
            IOobject
            (
                "thermalProperties",
                mesh().time().constant(),
                mesh(),
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE
            )
        ),
        thermalStress_(
            readBool(
                thermalProperties_.lookup("thermalStress")
            )
        ),
        threeKalpha_
        (
            IOobject
            (
                "threeKalpha",
                mesh().time().constant(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar(
                "threeKappa",
                dimensionSet(0, 2, -2 , -1, 0),
                pTraits<scalar>::zero
            )
        ),
        DT_
        (
            IOobject
            (
                "DT",
                mesh().time().constant(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar(
                "DT",
                dimensionSet(0, 2, -1 , 0, 0),
                pTraits<scalar>::zero
            )
        ),
        D_
        (
            IOobject
            (
                "D",
                mesh().time().timeName(),
                mesh(),
                IOobject::MUST_READ,
                IOobject::AUTO_WRITE
            ),
            mesh()
        )
{
    Dbug << "SolidDisplacementRegionSolverFunctionObject::SolidDisplacementRegionSolverFunctionObject" << endl;

    fvMesh &mesh = this->mesh();
    const Time &runTime = mesh.time();

    const dictionary &rhoDict(mechanicalProperties_.subDict("rho"));
    word rhoType(rhoDict.lookup("type"));

    IOobject rhoIO
    (
        "rho",
        runTime.timeName(0),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    );

    if (rhoType == "uniform")
    {
        scalar rhoValue(readScalar(rhoDict.lookup("value")));

        rhoPtr_.reset(
            new volScalarField
            (
                rhoIO,
                mesh,
                dimensionedScalar
                (
                    "rho",
                    dimMass / dimVolume,
                    rhoValue
                )
            )
        );
    }
    else if (rhoType == "field")
    {
        rhoIO.readOpt() = IOobject::MUST_READ;

        rhoPtr_.reset
        (
            new volScalarField
            (
                rhoIO,
                mesh
            )
        );
    }
    else
    {
        FatalErrorInFunction
            << "Valid type entries are uniform or field for rho"
            << abort(FatalError);
    }

    const dictionary& EDict(mechanicalProperties_.subDict("E"));
    word EType(EDict.lookup("type"));

    IOobject EHeader
    (
        "E",
        runTime.timeName(0),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    );

    if (EType == "uniform")
    {
        scalar rhoEValue(readScalar(EDict.lookup("value")));

        EPtr_.reset
        (
            new volScalarField
            (
                EHeader,
                mesh,
                dimensionedScalar
                (
                    "Erho",
                    dimMass / dimLength / sqr(dimTime),
                    rhoEValue
                )
            )
        );
    }
    else if (EType == "field")
    {
        EHeader.readOpt() = IOobject::MUST_READ;

        EPtr_.reset
        (
            new volScalarField
            (
                EHeader,
                mesh
            )
        );
    }
    else
    {
        FatalErrorInFunction
            << "Valid type entries are uniform or field for E"
            << abort(FatalError);
    }

    IOobject nuIO
    (
        "nu",
        runTime.timeName(0),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    );

    const dictionary& nuDict(mechanicalProperties_.subDict("nu"));
    word nuType(nuDict.lookup("type"));

    if (nuType == "uniform")
    {
        scalar nuValue(
            readScalar(
                nuDict.lookup("value")
            )
        );
        nuPtr_.reset
        (
            new volScalarField
            (
                nuIO,
                mesh,
                dimensionedScalar
                (
                    "nu",
                    dimless,
                    nuValue
                )
            )
        );
    }
    else if (nuType == "field")
    {
        nuIO.readOpt() = IOobject::MUST_READ;
        nuPtr_.reset
        (
            new volScalarField
            (
                nuIO,
                mesh
            )
        );
    }
    else
    {
        FatalErrorInFunction
            << "Valid type entries are uniform or field for nu"
            << abort(FatalError);
    }

    volScalarField &rho = rhoPtr_();
    //    volScalarField &rhoE = EPtr_();
    volScalarField &nu = nuPtr_();

    Info << "Normalising E : E/rho\n" << endl;
    ERealPtr_.set(
        new volScalarField(
            EPtr_()/rhoPtr_()
        )
    );

    volScalarField &E = ERealPtr_();

    Info<< "Calculating Lame's coefficients\n" << endl;

    muPtr_.set(
        new volScalarField(
            E / (2.0 * (1.0 + nu))
        )
    );

    volScalarField &mu = muPtr_();

    lambdaPtr_.set(
        new volScalarField (
            nu*E/((1.0 + nu)*(1.0 - 2.0*nu))
        )
    );
    volScalarField &lambda = lambdaPtr_();

    threeKPtr_.set(
        new volScalarField (
            E/(1.0 - 2.0*nu)
        )
    );

    volScalarField &threeK = threeKPtr_();
    if (
        readBool(
            mechanicalProperties_.lookup("planeStress")
        )
    )
    {
        Info<< "Plane Stress\n" << endl;

        lambda = nu*E/((1.0 + nu)*(1.0 - nu));
        threeK = E/(1.0 - nu);
    }
    else
    {
        Info<< "Plane Strain\n" << endl;
    }

    if (thermalStress_)
    {
        const dictionary &thermalProperties = thermalProperties_;

        autoPtr<volScalarField> CPtr;

        IOobject CIO
            (
                "C",
                runTime.timeName(0),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            );

        const dictionary& CDict(thermalProperties.subDict("C"));
        word CType(CDict.lookup("type"));
        if (CType == "uniform")
        {
            scalar CValue(
                readScalar(
                    CDict.lookup("value")
                )
            );

            CPtr.reset
                (
                    new volScalarField
                    (
                        CIO,
                        mesh,
                        dimensionedScalar
                        (
                            "C",
                            dimensionSet(0, 2, -2 , -1, 0),
                            CValue
                        )
                    )
                );
        }
        else if (CType == "field")
        {
            CIO.readOpt() = IOobject::MUST_READ;

            CPtr.reset
                (
                    new volScalarField
                    (
                        CIO,
                        mesh
                    )
                );
        }
        else
        {
            FatalErrorInFunction
                << "Valid type entries are uniform or field for C"
                    << abort(FatalError);
        }

        volScalarField& C = CPtr();

        autoPtr<volScalarField> rhoKPtr;

        IOobject rhoKIO
            (
                "k",
                runTime.timeName(0),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            );

        const dictionary& kDict(thermalProperties.subDict("k"));
        word kType(kDict.lookup("type"));
        if (kType == "uniform")
        {
            scalar rhoKValue(
                readScalar(
                    kDict.lookup("value")
                )
            );

            rhoKPtr.reset
                (
                    new volScalarField
                    (
                        rhoKIO,
                        mesh,
                        dimensionedScalar
                        (
                            "rhoK",
                            dimensionSet(1, 1, -3 , -1, 0),
                            rhoKValue
                        )
                    )
                );

        }
        else if (kType == "field")
        {
            rhoKIO.readOpt() = IOobject::MUST_READ;

            rhoKPtr.reset
                (
                    new volScalarField
                    (
                        rhoKIO,
                        mesh
                    )
                );
        }
        else
        {
            FatalErrorInFunction
                << "Valid type entries are uniform or field for K"
                    << abort(FatalError);
        }

        volScalarField& rhoK = rhoKPtr();

        autoPtr<volScalarField> alphaPtr;

        IOobject alphaIO
            (
                "alpha",
                runTime.timeName(0),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            );


        const dictionary& alphaDict(thermalProperties.subDict("alpha"));
        word alphaType(alphaDict.lookup("type"));

        if (alphaType == "uniform")
        {
            scalar alphaValue(
                readScalar(
                    alphaDict.lookup("value")
                )
            );
            alphaPtr.reset
                (
                    new volScalarField
                    (
                        alphaIO,
                        mesh,
                        dimensionedScalar
                        (
                            "alpha",
                            inv(dimTemperature),
                            alphaValue
                        )
                    )
                );
        }
        else if (alphaType == "field")
        {
            alphaIO.readOpt() = IOobject::MUST_READ;

            alphaPtr.reset
                (
                    new volScalarField
                    (
                        alphaIO,
                        mesh
                    )
                );
        }
        else
        {
            FatalErrorInFunction
                << "Valid type entries are uniform or field for alpha"
                    << abort(FatalError);
        }

        volScalarField& alpha = alphaPtr();

        Info<< "Normalising k : k/rho\n" << endl;
        volScalarField k(rhoK/rho);

        Info<< "Calculating thermal coefficients\n" << endl;

        threeKalpha_ = threeK*alpha;
        DT_ = k/C;
    }

    if (thermalStress_)
    {
        Info<< "Reading field T\n" << endl;
        Tptr_.reset
            (
                new volScalarField
                (
                    IOobject
                    (
                        "T",
                        runTime.timeName(),
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::AUTO_WRITE
                    ),
                    mesh
                )
            );
    }

    volVectorField &D = D_;

    Info<< "Calculating stress field sigmaD\n" << endl;
    sigmaD_.set(
        new volSymmTensorField (
            IOobject
            (
                "sigmaD",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mu*twoSymm(fvc::grad(D)) + lambda*(I*tr(fvc::grad(D)))
        )
    );
    volSymmTensorField &sigmaD = sigmaD_();
    Info<< "Calculating explicit part of div(sigma) divSigmaExp\n" << endl;
    divSigmaExp_.set(
        new volVectorField (
            IOobject
            (
                "divSigmaExp",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            fvc::div(sigmaD)
        )
    );
    volVectorField &divSigmaExp = divSigmaExp_();

    bool compactNormalStress = readBool(
        stressControl_.lookup("compactNormalStress")
    );
    if (compactNormalStress)
    {
        divSigmaExp -= fvc::laplacian(2*mu + lambda, D, "laplacian(DD,D)");
    }
    else
    {
        divSigmaExp -= fvc::div((2*mu + lambda)*fvc::grad(D), "div(sigmaD)");
    }

    mesh
#ifdef FOAM_FVSCHEMES_HAS_SETFLUXREQUIRED
        .schemesDict().setFluxRequired(
#else
       .fluxRequired(
#endif
           D.name()
       );
}

bool SolidDisplacementRegionSolverFunctionObject::start() {
    return true;
}

//- actual solving
bool SolidDisplacementRegionSolverFunctionObject::solveRegion() {
    fvMesh &mesh = this->mesh();
    const Time &runTime = mesh.time();

    volScalarField &rho = rhoPtr_();
    //    volScalarField &rhoE = EPtr_();
    //    volScalarField &E = ERealPtr_();
    //    volScalarField &nu = nuPtr_();
    volScalarField &mu = muPtr_();
    volScalarField &lambda = lambdaPtr_();
    //    volScalarField &threeK = threeKPtr_();
    volScalarField &DT = DT_;
    volVectorField &D = D_;
    volVectorField &divSigmaExp = divSigmaExp_();
    volScalarField &threeKalpha = threeKalpha_;
    volSymmTensorField &sigmaD = sigmaD_();

    const dictionary &stressControl = stressControl_;
    const bool &thermalStress = thermalStress_;

    int nCorr = stressControl.lookupOrDefault<int>("nCorrectors", 1);
    scalar convergenceTolerance = readScalar(
        stressControl.lookup("D")
    );
    bool compactNormalStress = readBool(
        stressControl.lookup("compactNormalStress")
    );

    int iCorr = 0;
    scalar initialResidual = 0;

    do
    {
        if (thermalStress)
        {
            volScalarField& T = Tptr_();
            solve
                (
                    fvm::ddt(T) == fvm::laplacian(DT, T)
                );
        }

        {
            fvVectorMatrix DEqn
                (
                    fvm::d2dt2(D)
                    ==
                    fvm::laplacian(2*mu + lambda, D, "laplacian(DD,D)")
                    + divSigmaExp
                );

            if (thermalStress)
            {
                const volScalarField& T = Tptr_();
                DEqn += fvc::grad(threeKalpha*T);
            }

            initialResidual =
#ifdef FOAM_SOLVER_PERFORMANCE_NO_MAX
                DEqn.solve().initialResidual();
#else
                DEqn.solve().max().initialResidual();
#endif

            if (!compactNormalStress)
            {
                divSigmaExp = fvc::div(DEqn.flux());
            }
        }

        {
            volTensorField gradD(fvc::grad(D));
            sigmaD = mu*twoSymm(gradD) + (lambda*I)*tr(gradD);

            if (compactNormalStress)
            {
                divSigmaExp = fvc::div
                    (
                        sigmaD - (2*mu + lambda)*gradD,
                        "div(sigmaD)"
                    );
            }
            else
            {
                divSigmaExp += fvc::div(sigmaD);
            }
        }
    } while (initialResidual > convergenceTolerance && ++iCorr < nCorr);
    Info << iCorr << " solid iterations" << endl;

    if (
        runTime.outputTime()
    )
    {
        volSymmTensorField sigma
        (
            IOobject
            (
                "sigma",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            rho*sigmaD
        );

        if (thermalStress)
        {
            const volScalarField& T = Tptr_();
            sigma = sigma - I*(rho*threeKalpha*T);
        }

        volScalarField sigmaEq
        (
            IOobject
            (
                "sigmaEq",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            sqrt((3.0/2.0)*magSqr(dev(sigma)))
        );

        Info<< "Max sigmaEq = " << max(sigmaEq).value()
            << endl;

        sigma.write();
        sigmaEq.write();
    }

    return true;
}

bool SolidDisplacementRegionSolverFunctionObject::read(const dictionary& dict) {
    return SimpleRegionSolverFunctionObject::read(dict);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
