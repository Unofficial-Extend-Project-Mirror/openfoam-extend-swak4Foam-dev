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
    Basis for the other function objects here. Evaluates the most common options

SourceFiles
    RhoPimpleFlowRegionSolverFunctionObject.C

Contributors/Copyright:
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef RhoPimpleFlowRegionSolverFunctionObject_H
#define RhoPimpleFlowRegionSolverFunctionObject_H

#include "SimpleRegionSolverFunctionObject.H"

#include "fluidThermo.H"
#include "fvOptions.H"
#include "pimpleControl.H"
#include "surfaceMesh.H"
#include "pressureControl.H"
#include "turbulentFluidThermoModel.H"
#include "IOMRFZoneList.H"
#include "uniformDimensionedFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

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

    //- will we use LTS?
    bool LTS_;

    //- reverse timestep for LTS
    autoPtr<volScalarField> trDeltaT;

    //- controling the PIMPLE-algorithm
    pimpleControl pimple_;

    //- thermo
    autoPtr<fluidThermo> thermo_;

    //- the sources
    fv::options fvOptions_;

    //- the density
    volScalarField rho_;

    //- velocity
    volVectorField U_;

    //- surface flows
    surfaceScalarField phi_;

    //- control the pressure
    pressureControl pressureControl_;

    //- turbulence
    autoPtr<compressible::turbulenceModel> turbulence_;

    //- pressure change
    volScalarField dpdt_;

    //- kinetic energy
    volScalarField K_;

    //- rotating zones
    IOMRFZoneList MRF_;

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
