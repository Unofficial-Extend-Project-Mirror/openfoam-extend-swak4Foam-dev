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

#include "swak.H"

#ifdef FOAM_HAS_FLUIDTHERMO

#ifndef FOAM_LOCAL_EULER_DT_HAS_SCHEME_IN_NAME

#include "swakTime.H"

#include "RhoPimpleFlowRegionSolverFunctionObject.H"
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
#ifndef FOAM_HAS_NO_LOCAL_EULER_DDT
#include "fvcSmooth.H"
#include "localEulerDdtScheme.H"
#endif

#include "swakTurbulence.H"

#ifdef FOAM_FIRST_ITER_MOVED_FROM_PIMPLE_TO_PISO
#ifdef FOAM_PISO_CONTROL_METHODS_NO_CAPITAL
#define firstIter firstPisoIter
#define nCorrPISO nCorrPiso
#define SIMPLErho simpleRho
#else
#define firstIter firstPISOIter
#endif
#endif

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(RhoPimpleFlowRegionSolverFunctionObject, 0);

addNamedToRunTimeSelectionTable
(
    functionObject,
    RhoPimpleFlowRegionSolverFunctionObject,
    dictionary,
    rhoPimpleFlowRegionSolver
);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

RhoPimpleFlowRegionSolverFunctionObject::RhoPimpleFlowRegionSolverFunctionObject
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
#ifndef FOAM_HAS_NO_LOCAL_EULER_DDT
#ifndef FOAM_LOCAL_EULER_DT_HAS_SCHEME_IN_NAME
    LTS_ (
        fv::localEulerDdt::enabled(this->mesh())
    ),
    trDeltaT(
        LTS_
        ?
        new volScalarField
        (
            IOobject
            (
                fv::localEulerDdt::rDeltaTName,
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::READ_IF_PRESENT,
                IOobject::AUTO_WRITE
            ),
            this->mesh(),
            dimensionedScalar("one", dimless/dimTime, 1),
            extrapolatedCalculatedFvPatchScalarField::typeName
        )
        :
        nullptr
    ),
#endif
#endif
    pimple_(
        this->mesh()
    ),
    thermo_(
        fluidThermo::New(this->mesh())
    ),
#ifdef FOAM_HAS_FVOPTIONS
    fvOptions_(
        this->mesh()
    ),
#endif
    rho_(
        IOobject
        (
            "rho",
            this->mesh().time().timeName(),
            this->mesh(),
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        thermo().rho()
    ),
    U_(
        IOobject
        (
            "U",
            this->mesh().time().timeName(),
            this->mesh(),
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh()
    ),
    phi_(
        IOobject
        (
            "phi",
            this->mesh().time().timeName(),
            this->mesh(),
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        linearInterpolate(rho_*U_) & this->mesh().Sf()
    ),
#ifndef FOAM_HAS_NO_PRESURE_CONTROL_CLASS
    pressureControl_(
        thermo().p(),
        rho_,
        pimple_.dict(),
        false
    ),
#endif
    turbulence_
    (
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
      compressible::momentumTransportModel
#else
      compressible::turbulenceModel
#endif
        ::New
        (
            rho_,
            U_,
            phi_,
            thermo()
        )
    ),
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
    thermophysicalTransport_(
        fluidThermophysicalTransportModel::New(
            turbulence_(),
            thermo()
        )
    ),
#endif
    dpdt_
    (
        IOobject
        (
            "dpdt",
            this->mesh().time().timeName(),
            this->mesh(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        fvc::ddt(thermo().p())
    ),
    K_(
        "K",
        0.5*magSqr(U_)
    ),
#ifdef FOAM_HAS_IOMRFLIST
    MRF_(
        this->mesh()
    ),
#endif
    cumulativeContErrIO_(
        IOobject
        (
            "cumulativeContErr",
            this->mesh().time().timeName(),
            "uniform",
            this->mesh(),
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        dimensionedScalar("zero", dimless, 0)
    )
{
    Dbug << "RhoPimpleFlowRegionSolverFunctionObject::RhoPimpleFlowRegionSolverFunctionObject" << endl;

#ifdef FOAM_HAS_FLUIDTHERMO
    thermo().validate(this->name(), "h", "e");

    if (!thermo().dpdt())
    {
        dpdt_ == dimensionedScalar("zero", dpdt_.dimensions(), 0);
        dpdt_.writeOpt() = IOobject::NO_WRITE;
    }
#endif

    volVectorField &U = U_;
    volScalarField &K = K_;

    if (U.nOldTimes())
    {
        volVectorField* Uold = &U.oldTime();
        volScalarField* Kold = &K.oldTime();
        *Kold == 0.5*magSqr(*Uold);

        while (Uold->nOldTimes())
        {
            Uold = &Uold->oldTime();
            Kold = &Kold->oldTime();
            *Kold == 0.5*magSqr(*Uold);
        }
    }

}

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
bool RhoPimpleFlowRegionSolverFunctionObject::start() {
    return true;
}
#endif

//- actual solving
bool RhoPimpleFlowRegionSolverFunctionObject::solveRegion() {
    // References so that the includes work
#ifndef FOAM_HAS_NO_LOCAL_EULER_DDT
#ifndef FOAM_LOCAL_EULER_DT_HAS_SCHEME_IN_NAME
    const bool &LTS = this->LTS_;
#endif
#endif
    pimpleControl &pimple = this->pimple_;
    fluidThermo& thermo = this->thermo();
    volScalarField &p = thermo.p();
    volScalarField &rho = rho_;
    volVectorField &U = U_;
    surfaceScalarField &phi = phi_;
    fvMesh &mesh = this->mesh();
#ifndef FOAM_HAS_NO_PRESURE_CONTROL_CLASS
    pressureControl &pressureControl = pressureControl_;
#endif
    autoPtr<
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
      compressible::momentumTransportModel
#else
      compressible::turbulenceModel
#endif
        > &turbulence = turbulence_;
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
    autoPtr<fluidThermophysicalTransportModel>
        &thermophysicalTransport = thermophysicalTransport_;
#endif
    const volScalarField& psi = thermo.psi();
    const Time &runTime = mesh.time();
#ifdef FOAM_HAS_FVOPTIONS
#ifdef FOAM_FVOPTIONS_IN_FV
    fv::options
#else
    fv::IOoptionList
#endif
        &fvOptions = fvOptions_;
#endif
#ifdef FOAM_CORRECT_RHO_USES_RHOMIN_MAX
    const dimensionedScalar rhoMax("rhoMax", dimDensity, GREAT, pimple.dict());
    const dimensionedScalar rhoMin("rhoMin", dimDensity, Zero, pimple.dict());
#endif
    volScalarField &dpdt = dpdt_;
    volScalarField &K = K_;
#ifdef FOAM_HAS_IOMRFLIST
    IOMRFZoneList &MRF = MRF_;
#endif
    autoPtr<surfaceVectorField> &rhoUf = rhoUf_;
    scalar& cumulativeContErr = cumulativeContErrIO_.value();

#ifndef FOAM_HAS_NO_LOCAL_EULER_DDT
#ifndef FOAM_LOCAL_EULER_DT_HAS_SCHEME_IN_NAME
    if (LTS)
    {
         #include "RhoPimpleFlowIncludes/setRDeltaT.H"
    }
    else
#endif
#endif
    {
    #include "compressibleCourantNo.H"
        // #include "setDeltaT.H"
    }

    //        ++runTime;

    // Info<< "Time = " << runTime.timeName() << nl << endl;

    // --- Pressure-velocity PIMPLE corrector loop
    while (pimple.loop())
    {
        if (pimple.firstIter())
        {
            // Store momentum to set rhoUf for introduced faces.
            autoPtr<volVectorField> rhoU;
            if (rhoUf.valid())
            {
                rhoU.reset(new volVectorField("rhoU", rho*U));
            }
        }

        if (
            pimple.firstIter()
#ifdef FOAM_PIMPLE_CONTROL_HAS_SIMPLERHO
            &&
            !pimple.SIMPLErho()
#endif
        )
        {
            #include "rhoEqn.H"
        }

        #include "RhoPimpleFlowIncludes/EEqn.H"
        #include "RhoPimpleFlowIncludes/UEqn.H"

        // --- Pressure corrector loop
        while (pimple.correct())
        {
            if (pimple.consistent())
            {
                #include "RhoPimpleFlowIncludes/pcEqn.H"
            }
            else
            {
                #include "RhoPimpleFlowIncludes/pEqn.H"
            }
        }

        if (pimple.turbCorr())
        {
            turbulence->correct();
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
            thermophysicalTransport->correct();
#endif
        }
    }

    rho = thermo.rho();

    return true;
}

bool RhoPimpleFlowRegionSolverFunctionObject::read(const dictionary& dict) {
    return SimpleRegionSolverFunctionObject::read(dict);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

#endif

#endif

// ************************************************************************* //
