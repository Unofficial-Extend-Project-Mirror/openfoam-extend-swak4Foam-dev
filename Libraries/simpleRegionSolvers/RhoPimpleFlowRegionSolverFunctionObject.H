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

Class
    RhoPimpleFlowRegionSolverFunctionObject

Description
    Solve compressible flow. Replicates the rhoPimpleFoam solver in v1906
    minus the dynmamic mesh facilities

SourceFiles
    RhoPimpleFlowRegionSolverFunctionObject.C

Contributors/Copyright:
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef RhoPimpleFlowRegionSolverFunctionObject_H
#define RhoPimpleFlowRegionSolverFunctionObject_H

#include "swakThermoTypes.H"

#include "SimpleRegionSolverFunctionObject.H"

#ifdef FOAM_HAS_FVOPTIONS
#ifdef FOAM_FVOPTIONS_IN_FV
#include "fvOptions.H"
#else
#include "fvIOoptionList.H"
#endif
#endif

#include "pimpleControl.H"
#include "surfaceMesh.H"
#ifndef FOAM_HAS_NO_PRESURE_CONTROL_CLASS
#include "pressureControl.H"
#endif
#include "swakTurbulence.H"

#ifdef FOAM_NEW_TURBULENCE_STRUCTURE
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
#include "compressibleMomentumTransportModel.H"
#include "fluidThermoMomentumTransportModel.H"
#include "singlePhaseTransportModel.H"
#include "fluidThermophysicalTransportModel.H"
#else
#include "turbulentFluidThermoModel.H"
#endif
#else
#include "turbulenceModel.H"
#endif

#ifdef FOAM_HAS_IOMRFLIST
#include "IOMRFZoneList.H"
#endif

#include "uniformDimensionedFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

#ifndef FOAM_HAS_FLUIDTHERMO
typedef Foam::basicPsiThermo fluidThermo;
#endif

/*---------------------------------------------------------------------------*\
                           Class RhoPimpleFlowRegionSolverFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class RhoPimpleFlowRegionSolverFunctionObject
:
    public SimpleRegionSolverFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    RhoPimpleFlowRegionSolverFunctionObject(const RhoPimpleFlowRegionSolverFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const RhoPimpleFlowRegionSolverFunctionObject&);

#ifndef FOAM_HAS_NO_LOCAL_EULER_DDT
#ifndef FOAM_LOCAL_EULER_DT_HAS_SCHEME_IN_NAME
    //- will we use LTS?
    bool LTS_;

    //- reverse timestep for LTS
    autoPtr<volScalarField> trDeltaT;
#endif
#endif

    //- controling the PIMPLE-algorithm
    pimpleControl pimple_;

    //- thermo
    autoPtr<fluidThermo> thermo_;

#ifdef FOAM_HAS_FVOPTIONS
    //- the sources
#ifdef FOAM_FVOPTIONS_IN_FV
    fv::options
#else
    fv::IOoptionList
#endif
    fvOptions_;
#endif

    //- the density
    volScalarField rho_;

    //- velocity
    volVectorField U_;

    //- surface flows
    surfaceScalarField phi_;

#ifndef FOAM_HAS_NO_PRESURE_CONTROL_CLASS
    //- control the pressure
    pressureControl pressureControl_;
#endif

    //- turbulence
    autoPtr<
#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
        compressible::momentumTransportModel
#else
        compressible::turbulenceModel
#endif
        > turbulence_;

#ifdef FOAM_HAS_MOMENTUM_TRANSPORT_MODELS
    //- special transport model
    autoPtr<fluidThermophysicalTransportModel> thermophysicalTransport_;
#endif

    //- pressure change
    volScalarField dpdt_;

    //- kinetic energy
    volScalarField K_;

#ifdef FOAM_HAS_IOMRFLIST
    //- rotating zones
    IOMRFZoneList MRF_;
#endif

    //- dummy. Will never be set because we don't have a dynamic mesh
    autoPtr<surfaceVectorField> rhoUf_;

    //- continuity error
    uniformDimensionedScalarField cumulativeContErrIO_;

protected:
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    virtual bool start();
#endif

    //- actual solving
    virtual bool solveRegion();

    fluidThermo &thermo() {
        return thermo_();
    }

public:

    // Constructors

    //- Construct from components
    RhoPimpleFlowRegionSolverFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    TypeName("RhoPimpleFlowRegionSolverFunctionObject");

    // Member Functions

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
